#include "std_include.hpp"

#include "modules/imgui.hpp"
#include "modules/remix_vars.hpp"
#include "modules/renderer.hpp"
#include "shared/common/remix_api.hpp"

// see comment in main()
//#include "shared/common/dinput_hook_v1.hpp"
//#include "shared/common/dinput_hook_v2.hpp"

namespace comp
{
	void on_begin_scene_cb()
	{
		if (!tex_addons::initialized) {
			tex_addons::init_texture_addons();
		}

		// fake camera

		const auto& im = imgui::get();
		if (im->m_dbg_use_fake_camera)
		{
			D3DXMATRIX view_matrix
			(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.447f, 0.894f, 0.0f,
				0.0f, -0.894f, 0.447f, 0.0f,
				0.0f, 100.0f, -50.0f, 1.0f
			);

			D3DXMATRIX proj_matrix
			(
				1.359f, 0.0f, 0.0f, 0.0f,
				0.0f, 2.414f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.001f, 1.0f,
				0.0f, 0.0f, -1.0f, 0.0f
			);

			// Construct view matrix
			D3DXMATRIX rotation, translation;
			D3DXMatrixRotationYawPitchRoll(&rotation,
				D3DXToRadian(im->m_dbg_camera_yaw),		// Yaw in radians
				D3DXToRadian(im->m_dbg_camera_pitch),	// Pitch in radians
				0.0f);									// No roll for simplicity

			D3DXMatrixTranslation(&translation,
				-im->m_dbg_camera_pos[0], // Negate for camera (moves world opposite)
				-im->m_dbg_camera_pos[1],
				-im->m_dbg_camera_pos[2]);

			D3DXMatrixMultiply(&view_matrix, &rotation, &translation);

			// Construct projection matrix
			D3DXMatrixPerspectiveFovLH(&proj_matrix,
				D3DXToRadian(im->m_dbg_camera_fov), // FOV in radians
				im->m_dbg_camera_aspect,
				im->m_dbg_camera_near_plane,
				im->m_dbg_camera_far_plane);

			shared::globals::d3d_device->SetTransform(D3DTS_WORLD, &shared::globals::IDENTITY);
			shared::globals::d3d_device->SetTransform(D3DTS_VIEW, &view_matrix);
			shared::globals::d3d_device->SetTransform(D3DTS_PROJECTION, &proj_matrix);
		}


		// Actual camera setup here if matrices are available
		{
			shared::globals::d3d_device->SetTransform(D3DTS_WORLD, &shared::globals::IDENTITY); // does not hurt

			// Example code if you managed to find some kind of matrix struct
				//if (const auto viewport = game::vp; viewport)
				//{
				//	shared::globals::d3d_device->SetTransform(D3DTS_VIEW, &viewport->view);
				//	shared::globals::d3d_device->SetTransform(D3DTS_PROJECTION, &viewport->proj);
				//}
		}
	}

	
	/*__declspec (naked) void _stub()
	{
		__asm
		{
			mov		g_is_rendering_something, 1
			jmp		game::retn_addr__pre_draw_something;
		}
	}*/

	int compute_visibility_of_cell(game::vis_struct* vis, float* pos, float bounding_radius, float* out_distance)
	{
		const auto im = imgui::get();
		if (im->m_dbg_manual_compute_vis) {
			return im->m_dbg_manual_compute_vis_num;
		}

		bounding_radius += im->m_dbg_compute_vis_bounding_rad_offset;

		const float dx = pos[0] - vis->plane_origin.x;
		const float dy = pos[1] - vis->plane_origin.y;
		const float dz = pos[2] - vis->plane_origin.z;

		if (-bounding_radius <= dz * vis->plane_normal.z + dy * vis->plane_normal.y + dx * vis->plane_normal.x)
		{
			const float len = sqrtf(dx * dx + dz * dz + dy * dy);
			*out_distance = len;

			*out_distance += im->m_dbg_compute_vis_out_distance_offset;

			const float delta = len - bounding_radius;
			if (delta > bounding_radius) {
				return static_cast<int>(bounding_radius / delta * vis->max_strength);
			}

			return static_cast<int>(vis->max_strength);
		}

		return 0;
	}

	void main()
	{
		// #Step 2: init remix api if you want to use it or comment it otherwise
		// Requires "exposeRemixApi = True" in the "bridge.conf" that is located in the .trex folder
		shared::common::remix_api::initialize(nullptr, nullptr, nullptr, false);

		// init modules which do not need to be initialized, before the game inits, here
		shared::common::loader::module_loader::register_module(std::make_unique<imgui>());
		shared::common::loader::module_loader::register_module(std::make_unique<renderer>());

		// #Step 3: hook dinput if your game uses direct input (for ImGui) - ONLY USE ONE
		//shared::common::loader::module_loader::register_module(std::make_unique<shared::common::dinput_v1>()); // v1: might cause issues with the Alt+X menu
		//shared::common::loader::module_loader::register_module(std::make_unique<shared::common::dinput_v2>()); // v2: better but might need further tweaks

		shared::common::loader::module_loader::register_module(std::make_unique<remix_vars>());

		// ---

		// let GetVisibleStateSB always return 1 - 0x71B630
		shared::utils::hook::set(game::mem_addr__get_vis_state_sb, 
			0xB8, 0x01, 0x00, 0x00, 0x00,  // mov eax, 1
			0xC2, 0x0C, 0x00, 0x90, 0x90); // retn 0xC + 2 nop

		// 'DrawAScenery' - redirect some vis check func to our own
		shared::utils::hook(game::hk_addr__draw_scenery_comp_vis_fn_call, compute_visibility_of_cell, HOOK_CALL).install()->quick();

		// 'DrawAScenery' - if vis check fn returned less than 18 and viewmode < 3, cull? -> disable that
		shared::utils::hook::nop(game::nop_addr__draw_scenery_chk01, 2); // 0x79FC1C
		//shared::utils::hook::nop(game::nop_addr__draw_scenery_chk02, 6); // draw all no matter what 'compute_visibility_of_cell' returned // 0x79FB3F


		//shared::utils::hook(game::retn_addr__pre_draw_something - 5u, pre_render_something_stub, HOOK_JUMP).install()->quick();

		MH_EnableHook(MH_ALL_HOOKS);
	}
}
