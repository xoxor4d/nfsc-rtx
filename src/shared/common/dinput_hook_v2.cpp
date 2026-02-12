/*
 * Copyright (c) 2022-2024, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

// a lot of the code was taken from : https://github.com/NVIDIAGameWorks/dxvk-remix/blob/main/bridge/src/client/di_hook.cpp
// V2: does not use acquire / unaquire because it's problematic when paired with remix

#include "std_include.hpp"
#include "dinput_hook_v2.hpp"

#include "comp/modules/imgui.hpp"

namespace shared::common
{
	namespace
	{
		typedef HRESULT(__stdcall DInput8DeviceGetDeviceStateT)(IDirectInputDevice8*, DWORD, LPVOID);
		DInput8DeviceGetDeviceStateT* g_dinput8_device_get_device_state_original = nullptr;
	}

	static void forwardMessage(const dinput_v2::WndMsg& wm)
	{
		// forward directly to imgui msg handler
		comp::imgui::get()->input_message(wm.msg, wm.wParam, wm.lParam);
	}

	void dinput_v2::updateWindowSize()
	{
		RECT rect;
		::GetWindowRect(shared::globals::main_window, &rect);

		// Only accept reasonable window extents.
		// NOTE: A game overlay may setup a zero-sized DirectInput window and make
		// cursor position clamping code produce wrong results.
		if (rect.right - rect.left > 16) {
			s_windowWidth = rect.right - rect.left;
		}

		if (rect.bottom - rect.top > 16) {
			s_windowHeight = rect.bottom - rect.top;
		}
	}

	template<typename T>
	void dinput_v2::updateMouseState(const T* state, bool isAbsoluteAxis)
	{
		updateWindowSize();

		if (isAbsoluteAxis) 
		{
			s_mouseX = state->lX;
			s_mouseY = state->lY;
		}
		else 
		{
			s_mouseX += state->lX;
			s_mouseY += state->lY;
		}

		if (s_mouseX < 0) s_mouseX = 0;
		if (s_mouseY < 0) s_mouseY = 0;
		if (s_mouseX > s_windowWidth) s_mouseX = s_windowWidth;
		if (s_mouseY > s_windowHeight) s_mouseY = s_windowHeight;

		WndMsg wm { shared::globals::main_window };
		wm.msg = WM_MOUSEMOVE;
		wm.lParam = s_mouseX | (s_mouseY << 16);
		wm.wParam = (state->rgbButtons[0] & 0x80) ? MK_LBUTTON : 0;
		wm.wParam += (state->rgbButtons[1] & 0x80) ? MK_RBUTTON : 0;
		wm.wParam += ((s_KS[DIK_LCONTROL] & 0x80) || (s_KS[DIK_RCONTROL] & 0x80)) ? MK_CONTROL : 0;
		wm.wParam += ((s_KS[DIK_LSHIFT] & 0x80) || (s_KS[DIK_RSHIFT] & 0x80)) ? MK_SHIFT : 0;

#if 0	// can be used to clip the cursor to the window if a game does not do that by itself
		bool clipped_cursor = false;

		if (!shared::globals::imgui_is_rendering &&
			GetForegroundWindow() == shared::globals::main_window)
		{
			// Check if cursor visible
			CURSORINFO ci = { sizeof(ci) };
			GetCursorInfo(&ci);
			if (ci.flags & CURSOR_SHOWING)
			{
				uint32_t counter = 0u;
				while (::ShowCursor(FALSE) >= 0 && ++counter < 4) {
					SetCursor(nullptr); // Force hide by decrementing count
				}
			}

			// Lock to window
			RECT rect;
			GetClientRect(shared::globals::main_window, &rect);

			POINT topLeft = { rect.left, rect.top };  // {0, 0}
			POINT bottomRight = { rect.right, rect.bottom };  // {width, height}

			ClientToScreen(shared::globals::main_window, &topLeft);
			ClientToScreen(shared::globals::main_window, &bottomRight);

			RECT screenRect = { topLeft.x, topLeft.y, bottomRight.x, bottomRight.y };

			if (state->lX || state->lY) 
			{
				ClipCursor(&screenRect);
				clipped_cursor = true;
			}
		}
		else {
			ClipCursor(NULL);  // release if not focused
		}
#endif

		// always forward if menu is open - optional
		//if (shared::globals::imgui_menu_open && !clipped_cursor && (state->lX || state->lY)/*|| 0 != memcmp(&wm, &s_mouseMove, sizeof(wm))*/)
		//{
		//	forwardMessage(wm);
		//	s_mouseMove = wm;
		//}


		if (s_mouseButtons[0] != state->rgbButtons[0]) 
		{
			wm.msg = (state->rgbButtons[0] & 0x80) ? WM_LBUTTONDOWN : WM_LBUTTONUP;
			s_mouseButtons[0] = state->rgbButtons[0];

			if (0 != memcmp(&wm, &s_mouseLButton, sizeof(wm))) 
			{
				forwardMessage(wm);
				s_mouseLButton = wm;
			}
		}

		if (s_mouseButtons[1] != state->rgbButtons[1]) 
		{
			wm.msg = (state->rgbButtons[1] & 0x80) ? WM_RBUTTONDOWN : WM_RBUTTONUP;
			s_mouseButtons[1] = state->rgbButtons[1];

			if (0 != memcmp(&wm, &s_mouseRButton, sizeof(wm))) 
			{
				forwardMessage(wm);
				s_mouseRButton = wm;
			}
		}

		if (GET_WHEEL_DELTA_WPARAM(s_mouseWheel.wParam) != state->lZ) 
		{
			// Preserve button codes
			const WORD buttons = LOWORD(wm.wParam);

			wm.msg = WM_MOUSEWHEEL;
			wm.wParam = MAKELONG(buttons, state->lZ);

			forwardMessage(wm);
			s_mouseWheel = wm;
		}
	}

	void dinput_v2::updateKeyState(LPBYTE KS)
	{
		for (uint32_t vsc = 0; vsc < 256; vsc++) 
		{
			if (s_KS[vsc] != KS[vsc]) 
			{
				UINT vk = 0;

				switch (vsc)
				{
				case 210:
					// No translation
					vk = VK_INSERT;
					break;
				default:
					vk = MapVirtualKeyExA(vsc, MAPVK_VSC_TO_VK, NULL);
				}

				if (0 == vk) {
					continue;
				}

				WndMsg wm{ shared::globals::main_window };
				wm.msg = (KS[vsc] & 0x80) ? WM_KEYDOWN : WM_KEYUP;
				wm.wParam = vk;  
				forwardMessage(wm);

				// Emit WM_CHAR on keydown for characters
				if (wm.msg == WM_KEYDOWN) 
				{
					WORD ascii[2] = { 0 };
					if (1 == ToAscii(vk, vsc, KS, ascii, 0)) 
					{
						// Only process keys that have 1:1 character representation
						wm.msg = WM_CHAR;
						wm.wParam = ascii[0];
						forwardMessage(wm);
					}
				}

				s_KS[vsc] = KS[vsc];
			}
		}
	}

	HRESULT __stdcall dinput8_device_get_device_state_v2_hk(IDirectInputDevice8* device, DWORD cbData, LPVOID lpvData)
	{
		HRESULT hr = g_dinput8_device_get_device_state_original(device, cbData, lpvData);
		const auto di = dinput_v2::get();
		bool do_not_clear = false;

		switch (cbData)
		{
			case sizeof(DIMOUSESTATE) :
				di->updateMouseState(static_cast<DIMOUSESTATE*>(lpvData), di->MouseAxisMode == DIPROPAXISMODE_ABS);
				break;

			case sizeof(DIMOUSESTATE2) :
				di->updateMouseState(static_cast<DIMOUSESTATE2*>(lpvData), di->MouseAxisMode == DIPROPAXISMODE_ABS);
				break;

			case 256:
				//di->updateKeyState(static_cast<LPBYTE>(lpvData)); // game sends keyboard input via window messages so no need
				//di->KeyboardDeviceStateUsed = true;

				if (!shared::globals::imgui_wants_text_input) {
					do_not_clear = true;
				}
				
				break;
		}

		if (!do_not_clear && shared::globals::imgui_menu_open && !shared::globals::imgui_allow_input_bypass) {
			memset(lpvData, 0, cbData);
		}

		return hr;
	}

	// hook dinput8 to block / forward imgui input
	dinput_v2::dinput_v2()
	{
		p_this = this;

		shared::common::log("DInput8 V2", "Creating dummy device.", shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
		void* vtable[32];
		{
			IDirectInput8W* dinput8 = nullptr;
			if (DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8W, (void**)&dinput8, nullptr) != DI_OK) {
				return;
			}

			IDirectInputDevice8W* dev = nullptr;
			if (dinput8->CreateDevice(GUID_SysMouse, &dev, nullptr) != DI_OK)
			{
				dinput8->Release();
				return;
			}

			memcpy(vtable, *(void***)dev, 32 * 4);
			dev->Release();

			shared::common::log("DInput8 V2", "Hooking vTable ..", shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
			if (!g_dinput8_device_get_device_state_original)
			{
				MH_CreateHook(vtable[9], dinput8_device_get_device_state_v2_hk, (LPVOID*)&g_dinput8_device_get_device_state_original);
				MH_EnableHook(MH_ALL_HOOKS);
			}

			dinput8->Release();
		}

		// -----
		shared::common::log("DInput8 V2", "Module initialized.", shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
	}
}
