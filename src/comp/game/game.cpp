#include "std_include.hpp"
#include "shared/common/flags.hpp"

namespace comp::game
{
	// --------------
	// game variables

	//DWORD* d3d_dev_sample_addr = nullptr;
	float* drawscenery_cell_dist_check_01 = nullptr; // used for compare of out dist of vis func
	float* drawscenery_cell_dist_check_02 = nullptr; // used for compare of out dist/drawscenery_cell_dist_check_03 of vis func
	float* drawscenery_cell_dist_check_03 = nullptr; // bounding radius related (goes into vis func and used for comp later)
	int* preculler_mode = nullptr; // setting this to 0 disables occlusion checks?

	view_base* views = nullptr;

	int* always_rain = nullptr;

	int* options_rain_supported = nullptr;
	int* options_rain_enabled = nullptr;

	// --------------
	// game functions

	// SampleTemplate_t SampleTemplate = nullptr;
	IsPaused_t IsPaused = nullptr;

	// --------------
	// game asm offsets

	//uint32_t retn_addr__func1 = 0u;
	//uint32_t nop_addr__func2 = 0u;
	uint32_t mem_addr__get_vis_state_sb = 0u;
	uint32_t nop_addr__draw_scenery_chk01 = 0u;
	uint32_t nop_addr__draw_scenery_chk02 = 0u;
	uint32_t hk_addr__draw_scenery_comp_vis_fn_call = 0u;

	// --------------

#define PATTERN_OFFSET_SIMPLE(var, pattern, byte_offset, static_addr) \
		if (const auto offset = shared::utils::mem::find_pattern(##pattern, byte_offset, #var, use_pattern, static_addr); offset) { \
			(var) = offset; found_pattern_count++; \
		} total_pattern_count++;

#define PATTERN_OFFSET_DWORD_PTR_CAST_TYPE(var, type, pattern, byte_offset, static_addr) \
		if (const auto offset = shared::utils::mem::find_pattern(##pattern, byte_offset, #var, use_pattern, static_addr); offset) { \
			(var) = (type)*(DWORD*)offset; found_pattern_count++; \
		} total_pattern_count++;

	// init any adresses here
	void init_game_addresses()
	{
		
		const bool use_pattern = !shared::common::flags::has_flag("no_pattern");
		if (use_pattern) {
			shared::common::log("Game", "Getting offsets ...", shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
		}

		std::uint32_t total_pattern_count = 0u;
		std::uint32_t found_pattern_count = 0u;


#pragma region GAME_VARIABLES

		// Find code that references the global var you are interested in, grab the address of the instruction + pattern
		// Figure out the byte offset that's needed until your global var address starts in the instruction 
		// -> 'mov eax, d3d_dev_sample_addr' == A1 D8 D8 7E 01 where A1 is the mov instruction and the following 4 bytes the addr of the global var -> so offset 1
		
		// Patterns are quite slow on DEBUG builds. The last argument in find_pattern allows you to declare a static offset which will be used
		// when the game gets started with `-no_pattern` in the commandline

		// ----

		// Example verbose
			//if (const auto offset = shared::utils::mem::find_pattern("? ? ? ? ?", 1, "d3d_dev_sample_addr", use_pattern, 0xDEADBEEF); offset) {
			//	d3d_dev_sample_addr = (DWORD*)*(DWORD*)offset; found_pattern_count++; // cast mem at offset
			//} total_pattern_count++;

		// Or via macro
			//PATTERN_OFFSET_DWORD_PTR_CAST_TYPE(d3d_dev_sample_addr, DWORD*, "? ? ? ? ?", 1, 0xDEADBEEF);


		// Another example with a structure object
			//PATTERN_OFFSET_DWORD_PTR_CAST_TYPE(vp, some_struct_containing_matrices*, "? ? ? ? ?", 0, 0xDEADBEEF);

		PATTERN_OFFSET_DWORD_PTR_CAST_TYPE(drawscenery_cell_dist_check_01, float*, 
			"? ? ? ? ? ? DF E0 F6 C4 ? 7B ? ? ? ? ? ? ? DF E0 F6 C4 ? 0F 85 ? ? ? ? EB ? ? ? E8", 2, 0x79FC56);

		PATTERN_OFFSET_DWORD_PTR_CAST_TYPE(drawscenery_cell_dist_check_02, float*,
			"? ? ? ? ? ? DF E0 F6 C4 ? 0F 85 ? ? ? ? EB ? ? ? E8", 2, 0x79FC63);

		PATTERN_OFFSET_DWORD_PTR_CAST_TYPE(drawscenery_cell_dist_check_03, float*,
			"? ? ? ? ? ? 8D 46 ? ? ? ? 50 57", 2, 0x79FB22);

		PATTERN_OFFSET_DWORD_PTR_CAST_TYPE(preculler_mode, int*,
			"A3 ? ? ? ? 8D 44 24 ? 50 8B C8", 1, 0x73085A);
		
		PATTERN_OFFSET_DWORD_PTR_CAST_TYPE(views, view_base*,
			"8D 80 ? ? ? ? 89 46", 2, 0x482D1D);

		PATTERN_OFFSET_DWORD_PTR_CAST_TYPE(always_rain, int*,
			"A1 ? ? ? ? 85 C0 56 8B F1 74 ? A1 ? ? ? ? 89 86", 1, 0x7B3B60);

		if (const auto offset = shared::utils::mem::find_pattern("89 1D ? ? ? ? 89 1D ? ? ? ? 89 1D ? ? ? ? 89 1D ? ? ? ? 5F", 2, "options_rain", use_pattern, 0x712E64); offset)
		{
			options_rain_enabled = (int*)*(DWORD*)offset;
			options_rain_supported = (int*)*(DWORD*)(offset + 0xC); 
			found_pattern_count++;
		} total_pattern_count++;

		// end GAME_VARIABLES
#pragma endregion

		// ---


#pragma region GAME_FUNCTIONS

		// cast func template
		//PATTERN_OFFSET_DWORD_PTR_CAST_TYPE(SampleTemplate, SampleTemplate_t, "? ? ? ? ?", 0, 0xDEADBEEF);

		if (const auto offset = shared::utils::mem::find_pattern(
			"E8 ? ? ? ? 84 C0 0F 85 ? ? ? ? 8B 46 ? 85 C0 89 44 24", 0, "IsPaused", use_pattern, 0x41E5EB); offset) 
		{
			IsPaused = (IsPaused_t)shared::utils::mem::resolve_relative_call_address(offset); found_pattern_count++;
		} total_pattern_count++;

		// end GAME_FUNCTIONS
#pragma endregion

		// ---


#pragma region GAME_ASM_OFFSETS

		// Assembly offsets are simple offsets that do not require additional casting

		// Example verbose
			//if (const auto offset = shared::utils::mem::find_pattern(" ? ? ? ", 0, "nop_addr__func2", use_pattern, 0xDEADBEEF); offset) {
			//	nop_addr__func2 = offset; found_pattern_count++;
			//} total_pattern_count++;

		PATTERN_OFFSET_SIMPLE(mem_addr__get_vis_state_sb, "55 8B EC 83 E4 ? 83 EC ? 8B 45 ? 8B 50 ? 56", 0, 0x71B630);
		PATTERN_OFFSET_SIMPLE(nop_addr__draw_scenery_chk01, "7C ? 8B 43 ? 85 C0", 0, 0x79FC1C);
		PATTERN_OFFSET_SIMPLE(nop_addr__draw_scenery_chk02, "0F 8E ? ? ? ? 8B 56", 0, 0x79FB3F);
		PATTERN_OFFSET_SIMPLE(hk_addr__draw_scenery_comp_vis_fn_call, "E8 ? ? ? ? 83 C4 ? 83 F8 ? 89 44 24", 0, 0x79FB30);

		// end GAME_ASM_OFFSETS
#pragma endregion


		if (use_pattern)
		{
			if (found_pattern_count == total_pattern_count) {
				shared::common::log("Game", std::format("Found all '{:d}' Patterns.", total_pattern_count), shared::common::LOG_TYPE::LOG_TYPE_GREEN, true);
			}
			else
			{
				shared::common::log("Game", std::format("Only found '{:d}' out of '{:d}' Patterns.", found_pattern_count, total_pattern_count), shared::common::LOG_TYPE::LOG_TYPE_ERROR, true);
				shared::common::log("Game", ">> Please create an issue on GitHub and attach this console log and information about your game (version, platform etc.)\n", shared::common::LOG_TYPE::LOG_TYPE_STATUS, true);
			}
		}
	}

#undef PATTERN_OFFSET_SIMPLE

}
