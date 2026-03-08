#pragma once
#include "structs.hpp"

namespace comp::game
{
	class camera_s;

	// --------------
	// game variables

	extern char** shader_fx_path_array;

	extern int* game_input_allowed;

	extern float* drawscenery_cell_dist_check_01;
	extern float* drawscenery_cell_dist_check_02;
	extern float* drawscenery_cell_dist_check_03;
	extern int* preculler_mode;

	extern view_base* views;

	extern int* always_rain;
	extern float* always_rain_intensity;

	extern options* game_options;
	extern int* g_shaderDetailLevel;

	extern bool* cam_stop_updates;
	extern camera_s* the_camera;

	// --------------
	// game functions

	typedef	bool(__cdecl* IsPaused_t)();
		extern IsPaused_t IsPaused;


	typedef	void(__stdcall* SetCameraMatrix_t)(D3DXMATRIX* mtx, float time);
		extern SetCameraMatrix_t SetCameraMatrix;

	class camera_s
	{
	public:
		D3DXMATRIX view_matrix;
		Vector position;
		int pad_pos;
		Vector direction;
		int pad_dir;
		Vector target;
		int pad_target;
		Vector4D noise_frequency_1;
		Vector4D noise_amplitude_1;
		Vector4D noise_frequency_2;
		Vector4D noise_amplitude_2;
		Vector4D fade_color;
		float target_distance;
		float focal_distance;
		float depth_of_field;
		float dof_falloff;
		float dof_max_intensity;
		float near_clip;
		float far_clip;
		float lb_height;
		float sim_time_multiplier;
		std::uint16_t horizontal_fov;

		void set_camera_matrix(D3DXMATRIX* m, float fTime)
		{
			((void(__thiscall*)(camera_s*, D3DXMATRIX*, float))0x4822F0)(this, m, fTime);
			//game::SetCameraMatrix(m, fTime);
		}
	};

	// --------------
	// game asm offsets

	extern uint32_t retn_addr__load_effect_from_input;
	extern uint32_t call_addr__d3dx_create_effect_from_resource;

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
