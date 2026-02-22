#include "std_include.hpp"
#include <psapi.h>

#include "comp.hpp"
#include "modules/comp_settings.hpp"
#include "shared/common/flags.hpp"
#include "modules/d3d9ex.hpp"
#include "modules/d3dxeffects.hpp"

namespace comp
{
	std::unordered_set<HWND> wnd_class_list;

	// #Step 1: Start the game and copy the class name from the console window and put it in here:
	#define WINDOW_CLASS_NAME "GameFrame" // Eg: "GameFrame"

	BOOL CALLBACK enum_windows_proc(HWND hwnd, LPARAM lParam)
	{
		DWORD window_pid, target_pid = static_cast<DWORD>(lParam);
		GetWindowThreadProcessId(hwnd, &window_pid);

		if (window_pid == target_pid && IsWindowVisible(hwnd))
		{
			char class_name[256];
			GetClassNameA(hwnd, class_name, sizeof(class_name));

			if (!wnd_class_list.contains(hwnd))
			{
				char debug_msg[256];
				wsprintfA(debug_msg, "> HWND: %p, PID: %u, Class: %s, Visible: %d \n", hwnd, window_pid, class_name, IsWindowVisible(hwnd));
				shared::common::log("Main", debug_msg, shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
				wnd_class_list.insert(hwnd);
			}

			if (std::string_view(class_name).contains(WINDOW_CLASS_NAME))
			{
				shared::globals::main_window = hwnd;
				return FALSE;
			}
		}

		return TRUE;
	}

	DWORD WINAPI find_game_window([[maybe_unused]] LPVOID lpParam)
	{
		shared::common::console();
		std::uint32_t T = 0;

		shared::common::log("Main", "Waiting for window with classname containing '" WINDOW_CLASS_NAME "' ...", shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
		{
			while (!shared::globals::main_window)
			{
				EnumWindows(enum_windows_proc, static_cast<LPARAM>(GetCurrentProcessId()));
				if (!shared::globals::main_window) {
					Sleep(1u); T += 1u;
				}

				if (T >= 30000)
				{
					Beep(300, 100); Sleep(100); Beep(200, 100);
					shared::common::log("Main", "Could not find '" WINDOW_CLASS_NAME "' Window. Not loading RTX Compatibility Mod.", shared::common::LOG_TYPE::LOG_TYPE_ERROR, true);
					return TRUE;
				}
			}
		}

		if (!shared::common::flags::has_flag("nobeep")) {
			Beep(523, 100);
		}

		comp::main();
		return 0;
	}
}

BOOL APIENTRY DllMain(HMODULE hmodule, const DWORD ul_reason_for_call, LPVOID)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) 
	{
		shared::common::console();
		shared::globals::setup_dll_module(hmodule);
		shared::globals::setup_exe_module();
		shared::globals::setup_homepath();

		shared::common::set_console_color_blue(true);
		std::cout << "Launching RTX Remix Compatiblity Base Version [" << COMP_MOD_VERSION_MAJOR << "." << COMP_MOD_VERSION_MINOR << "." << COMP_MOD_VERSION_PATCH << "]\n";
		std::cout << "> Compiled On : " + std::string(__DATE__) + " " + std::string(__TIME__) + "\n";
		std::cout << "> https://github.com/xoxor4d/remix-comp-base\n\n";
		shared::common::set_console_color_default();

		if (const auto MH_INIT_STATUS = MH_Initialize(); MH_INIT_STATUS != MH_STATUS::MH_OK)
		{
			shared::common::log("Main", std::format("MinHook failed to initialize with code: {:d}", static_cast<int>(MH_INIT_STATUS)), shared::common::LOG_TYPE::LOG_TYPE_ERROR, true);
			return TRUE;
		}

		// Setup memory addresses (eg. patterns)
		comp::game::init_game_addresses();

		// Register d3d9 module to create a d3d9 proxy interface
		shared::common::loader::module_loader::register_module(std::make_unique<comp::d3d9ex>());

		// Register d3dxeffects module to hook D3DX effect functions
		shared::common::loader::module_loader::register_module(std::make_unique<comp::d3dxeffects>());

		// Register comp mod settings module
		shared::common::loader::module_loader::register_module(std::make_unique<comp::comp_settings>());

		// Find game window thread
		if (const auto t = CreateThread(nullptr, 0, comp::find_game_window, nullptr, 0, nullptr); t) {
			CloseHandle(t);
		}
	}

	return TRUE;
}