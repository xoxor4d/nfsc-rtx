#pragma once
#include "structs.hpp"

namespace comp::game
{
	// --------------
	// game variables

	//extern DWORD* d3d_dev_sample_addr;
	
	//inline IDirect3DDevice9* get_d3d_device() {
	//	return reinterpret_cast<IDirect3DDevice9*>(*d3d_dev_sample_addr);
	//}

	extern float* drawscenery_cell_dist_check_01;
	extern float* drawscenery_cell_dist_check_02;
	extern float* drawscenery_cell_dist_check_03;
	extern int* preculler_mode;

	extern view_base* views;

	extern int* options_rain_supported;
	extern int* options_rain_enabled;

	// --------------
	// game functions

	//typedef	void (__cdecl* SampleTemplate_t)(uint32_t arg1, uint32_t arg2);
	//	extern SampleTemplate_t SampleTemplate;

	typedef	bool(__cdecl* IsPaused_t)();
		extern IsPaused_t IsPaused;


	// --------------
	// game asm offsets

	//extern uint32_t retn_addr__func1;
	//extern uint32_t nop_addr__func2;
	extern uint32_t mem_addr__get_vis_state_sb;
	extern uint32_t nop_addr__draw_scenery_chk01;
	extern uint32_t nop_addr__draw_scenery_chk02;
	extern uint32_t hk_addr__draw_scenery_comp_vis_fn_call;



	// ---

	extern void init_game_addresses();
}
