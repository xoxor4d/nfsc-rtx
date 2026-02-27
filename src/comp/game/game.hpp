#pragma once
#include "structs.hpp"

namespace comp::game
{
	// --------------
	// game variables

	extern int* game_input_allowed;

	extern float* drawscenery_cell_dist_check_01;
	extern float* drawscenery_cell_dist_check_02;
	extern float* drawscenery_cell_dist_check_03;
	extern int* preculler_mode;

	extern view_base* views;

	extern int* always_rain;

	extern options* game_options;
	extern int* g_shaderDetailLevel;

	// --------------
	// game functions

	//typedef	void (__cdecl* SampleTemplate_t)(uint32_t arg1, uint32_t arg2);
	//	extern SampleTemplate_t SampleTemplate;

	typedef	bool(__cdecl* IsPaused_t)();
		extern IsPaused_t IsPaused;


	// --------------
	// game asm offsets

	extern uint32_t nop_addr__set_transforms_01;
	extern uint32_t nop_addr__set_transforms_02;
	extern uint32_t nop_addr__set_transforms_03;
	extern uint32_t nop_addr__set_transforms_04;

	extern uint32_t retn_addr__pre_draw_particle;
	extern uint32_t fn_addr__pre_draw_particle;
	extern uint32_t retn_addr__post_draw_particle;
	extern uint32_t fn_addr__post_draw_particle;
	extern uint32_t retn_addr__on_handle_material_data;
	extern uint32_t retn_addr__on_rain_render;
	extern uint32_t func_addr__on_rain_render;

	extern uint32_t retn_addr__on_world_internal_render;

	// comp

	extern uint32_t retn_addr__game_focused_stub;
	extern uint32_t skip_addr__game_focused_stub;

	extern uint32_t mem_addr__get_vis_state_sb;
	extern uint32_t nop_addr__draw_scenery_chk01;
	extern uint32_t nop_addr__draw_scenery_chk02;
	extern uint32_t hk_addr__draw_scenery_comp_vis_fn_call;

	extern uint32_t retn_addr__tree_cull;
	extern uint32_t retn_addr__draw_a_scenery;
	extern uint32_t commit_addr__draw_a_scenery;

	extern uint32_t retn_addr__draw_a_scenery_precull;
	extern uint32_t skip_addr__draw_a_scenery_precull;

	// ---

	extern void init_game_addresses();
}
