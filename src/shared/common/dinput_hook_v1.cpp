#include "std_include.hpp"
#include "dinput_hook_v1.hpp"
#include "shared/globals.hpp"

// V1: uses acquire / unaquire, might be problematic when paired with remix - untested with new remix input method

namespace shared::common
{
	namespace
	{
		typedef HRESULT(__stdcall DInput8DeviceGetDeviceStateT)(IDirectInputDevice8*, DWORD, LPVOID);
		DInput8DeviceGetDeviceStateT* g_dinput8_device_get_device_state_original = nullptr;

		typedef HRESULT(__stdcall DInput8DeviceGetDeviceDataT)(IDirectInputDevice8*, DWORD, LPDIDEVICEOBJECTDATA, LPDWORD, DWORD);
		DInput8DeviceGetDeviceDataT* g_dinput8_device_get_device_data_original = nullptr;

		typedef HRESULT(__stdcall DInput8DeviceAcquireT)(IDirectInputDevice8*);
		DInput8DeviceAcquireT* g_dinput8_device_acquire_original = nullptr;
	}

	HRESULT __stdcall dinput8_device_get_device_state_v1_hk(IDirectInputDevice8* device, DWORD cbData, LPVOID lpvData)
	{
		const auto hr = g_dinput8_device_get_device_state_original(device, cbData, lpvData);

		if (globals::imgui_menu_open && !globals::imgui_allow_input_bypass)
		{
			if (cbData == sizeof(DIMOUSESTATE) || cbData == sizeof(DIMOUSESTATE2)) {
				device->Unacquire();
			}
		}
		else 
		{
			if (cbData == sizeof(DIMOUSESTATE) || cbData == sizeof(DIMOUSESTATE2)) {
				device->Acquire();
			}
		}

		return hr;
	}

	HRESULT __stdcall dinput8_device_get_device_data_hk(IDirectInputDevice8* device, DWORD cbData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD flags)
	{
		const auto hr = g_dinput8_device_get_device_data_original(device, cbData, rgdod, pdwInOut, flags);

		if (globals::imgui_menu_open && !globals::imgui_allow_input_bypass) {
			device->Unacquire();
		} else {
			device->Acquire();
		}

		return hr;
	}

	HRESULT __stdcall dinput8_device_acquire_hk(IDirectInputDevice8* device)
	{
		if (globals::imgui_menu_open && !globals::imgui_allow_input_bypass) {
			return DI_OK;
		}

		return g_dinput8_device_acquire_original(device);
	}

	dinput_v1::dinput_v1()
	{
		p_this = this;

		shared::common::log("DInput8 V1", "Creating dummy device.", shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
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

			shared::common::log("DInput8 V1", "Hooking vTable ..", shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
			if (!g_dinput8_device_acquire_original && !g_dinput8_device_get_device_data_original && !g_dinput8_device_get_device_state_original)
			{
				MH_CreateHook(vtable[9], dinput8_device_get_device_state_v1_hk, (LPVOID*)&g_dinput8_device_get_device_state_original);
				MH_CreateHook(vtable[10], dinput8_device_get_device_data_hk, (LPVOID*)&g_dinput8_device_get_device_data_original);
				MH_CreateHook(vtable[7], dinput8_device_acquire_hk, (LPVOID*)&g_dinput8_device_acquire_original);
				MH_EnableHook(MH_ALL_HOOKS);
			}

			dinput8->Release();
		}

		// -----
		shared::common::log("DInput8 V1", "Module initialized.", shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
	}
}
