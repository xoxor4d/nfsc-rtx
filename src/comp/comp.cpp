#include "std_include.hpp"

#include "modules/comp_settings.hpp"
#include "modules/imgui.hpp"
#include "modules/map_settings.hpp"
#include "modules/rain.hpp"
#include "modules/remix_vars.hpp"
#include "modules/renderer.hpp"
#include "shared/common/remix_api.hpp"

namespace comp
{
	freecam_t g_freecam = {};
	remix_distant_light_def m_distant_light = {};

	// ----

	void on_begin_scene_cb()
	{
		if (!tex_addons::initialized) {
			tex_addons::init_texture_addons();
		}

		const auto im = imgui::get();
		const auto& cs = comp_settings::get();

		if (im->m_freecam_mode)
		{
			*game::game_input_allowed = false;
			*game::cam_stop_updates = true;

			const bool pressed_w = ImGui::IsKeyDown(ImGuiKey_W);
			const bool pressed_a = ImGui::IsKeyDown(ImGuiKey_A);
			const bool pressed_s = ImGui::IsKeyDown(ImGuiKey_S);
			const bool pressed_d = ImGui::IsKeyDown(ImGuiKey_D);
			const bool pressed_r = ImGui::IsKeyDown(ImGuiKey_R);
			const bool pressed_f = ImGui::IsKeyDown(ImGuiKey_F);
			const bool pressed_c = ImGui::IsKeyDown(ImGuiKey_C);
			const bool pressed_shift = ImGui::IsKeyDown(ImGuiKey_LeftShift);
			const bool pressed_space = ImGui::IsKeyDown(ImGuiKey_Space);

			const float forward_speed = im->m_freecam_fwd_speed * (pressed_shift ? 2.0f : 1.0f);
			const float strafe_speed = im->m_freecam_rt_speed * (pressed_shift ? 2.0f : 1.0f);
			const float upward_speed = im->m_freecam_up_speed * (pressed_shift ? 2.0f : 1.0f);
			const float roll_speed = im->m_freecam_roll_speed * (pressed_shift ? 2.0f : 1.0f);

			float dt = ImGui::GetIO().DeltaTime * 10.0f; // frame rate independent 

			// start free cam at current pos
			if (!g_freecam.active)
			{
				g_freecam.near_clip = game::the_camera->near_clip;
				g_freecam.far_clip = game::the_camera->far_clip;
				g_freecam.horizontal_fov = game::the_camera->horizontal_fov;

				g_freecam.position.x = game::the_camera->position.x;
				g_freecam.position.y = game::the_camera->position.y;
				g_freecam.position.z = game::the_camera->position.z;

				const D3DXMATRIX& vm = game::the_camera->view_matrix;
				g_freecam.right = { vm._11, vm._21, vm._31 };
				g_freecam.up = { vm._12, vm._22, vm._32 };
				g_freecam.forward = { vm._13, vm._23, vm._33 };

				g_freecam.active = true;
			}

			ImGuiIO& io = ImGui::GetIO();
			if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
			{
				float yaw = io.MouseDelta.x * im->m_freecam_mouse_sensitivity;
				float pitch = -io.MouseDelta.y * im->m_freecam_mouse_sensitivity;

				D3DXMatrixRotationAxis(&g_freecam.yaw_mat, &g_freecam.up, yaw);

				D3DXVec3TransformNormal(&g_freecam.forward, &g_freecam.forward, &g_freecam.yaw_mat);
				D3DXVec3TransformNormal(&g_freecam.right, &g_freecam.right, &g_freecam.yaw_mat);

				D3DXMatrixRotationAxis(&g_freecam.pitch_mat, &g_freecam.right, pitch);

				D3DXVec3TransformNormal(&g_freecam.forward, &g_freecam.forward, &g_freecam.pitch_mat);
				D3DXVec3TransformNormal(&g_freecam.up, &g_freecam.up, &g_freecam.pitch_mat);
			}

			// roll
			if (pressed_r || pressed_f)
			{
				const float roll = (pressed_r ? -1.0f : 1.0f) * roll_speed * dt;
				D3DXMatrixRotationAxis(&g_freecam.roll_mat, &g_freecam.forward, roll);

				D3DXVec3TransformNormal(&g_freecam.right, &g_freecam.right, &g_freecam.roll_mat);
				D3DXVec3TransformNormal(&g_freecam.up, &g_freecam.up, &g_freecam.roll_mat);
			}

			// normalize to prevent drift
			D3DXVec3Normalize(&g_freecam.forward, &g_freecam.forward);
			D3DXVec3Cross(&g_freecam.right, &g_freecam.up, &g_freecam.forward);
			D3DXVec3Normalize(&g_freecam.right, &g_freecam.right);
			D3DXVec3Cross(&g_freecam.up, &g_freecam.forward, &g_freecam.right);

			// movement
			if (pressed_w) { g_freecam.position += g_freecam.forward * forward_speed * dt; }
			if (pressed_s) { g_freecam.position -= g_freecam.forward * forward_speed * dt; }

			if (pressed_d) { g_freecam.position += g_freecam.right * strafe_speed * dt; }
			if (pressed_a) { g_freecam.position -= g_freecam.right * strafe_speed * dt; }

			if (pressed_c) { g_freecam.position += g_freecam.up * upward_speed * dt; }
			if (pressed_space) { g_freecam.position -= g_freecam.up * upward_speed * dt; }

			// build view matrix
			D3DXVECTOR3 target = g_freecam.position + g_freecam.forward;

			//D3DXMATRIX view;
			D3DXMatrixLookAtLH(&g_freecam.view, &g_freecam.position, &target, &g_freecam.up);

			game::the_camera->view_matrix = g_freecam.view;
			game::the_camera->position.x = g_freecam.position.x;
			game::the_camera->position.y = g_freecam.position.y;
			game::the_camera->position.z = g_freecam.position.z;

			D3DXVECTOR3 f = g_freecam.forward;
			D3DXVec3Normalize(&f, &f);
			game::the_camera->direction.x = f.x;
			game::the_camera->direction.y = f.y;
			game::the_camera->direction.z = f.z;

			game::the_camera->target.x = g_freecam.position.x + game::the_camera->direction.x;
			game::the_camera->target.y = g_freecam.position.y + game::the_camera->direction.y;
			game::the_camera->target.z = g_freecam.position.z + game::the_camera->direction.z;

			// ---
			game::the_camera->near_clip = g_freecam.near_clip;
			game::the_camera->far_clip = g_freecam.far_clip;
			game::the_camera->horizontal_fov = g_freecam.horizontal_fov;
		}

		// on reset
		else if (g_freecam.active)
		{
			*game::game_input_allowed = shared::globals::imgui_allow_input_bypass;
			*game::cam_stop_updates = false;

			g_freecam = {};
			g_freecam.active = false;
		}

		// for hyperlinked fork tests: (treecull function in hyperlinked is much slower than og)
		//*game::drawscenery_cell_dist_check_01 = cs->nocull_distance_scenery._float();
		//*game::drawscenery_cell_dist_check_02 = cs->nocull_distance._float();

#if 1
		if (const auto options = game::game_options; options)
		{
			options->car_env_map_enabled = 0;
			options->road_reflections_enabled = 0;
			options->motion_blur_enabled = 0;
			options->draw_particles = 1;
			options->particles_supported = 1;
			options->world_lod_level = 3;
			options->car_lod_level = 1;
			options->visual_treatment = 0;
			options->visual_treatment_supported = 0;
			options->shadow_detail = 0;
			options->fsaa_level = 0;
			options->rain_enabled = 1; // using shadermodel 1 disables these?
			options->rain_supported = 1; // ^

			options->bone_anims_enabled = 1;
			options->bone_anims_supported = 1;

			*game::g_shaderDetailLevel = 1; // 3 also works
		}
#endif
		renderer::get()->m_triggered_remix_injection = false;

		if (cs->light_sun_enable._bool())
		{
			auto& api = shared::common::remix_api::get();
			auto& l = m_distant_light;

			if (l.m_handle)
			{
				api.m_bridge.DestroyLight(l.m_handle);
				l.m_handle = nullptr;
			}

			l.m_ext.sType = REMIXAPI_STRUCT_TYPE_LIGHT_INFO_DISTANT_EXT;
			l.m_ext.pNext = nullptr;

			if (im->m_dbg_sun_time_of_day)
			{
				if (const auto tp = game::time_of_day_ptr; tp && tp->tod)
				{
					const auto& tod = tp->tod;

					auto dir = tod->sun_direction;
						 dir.z = dir.z - 1.0f;
						 dir.Normalize();

					l.m_ext.direction = dir.ToRemixFloat3D();

					const auto& sun_col = tod->current.diffuse_color;
					Vector rad = sun_col * cs->light_sun_intensity._float();
					l.m_info.radiance = rad.ToRemixFloat3D();
				}
			}
			else
			{
				auto dir = cs->light_sun_direction.get_as<Vector>(); dir.Normalize();
				l.m_ext.direction = dir.ToRemixFloat3D();

				const auto& sun_col = cs->light_sun_color.get_as<Vector>();
				Vector rad = sun_col * cs->light_sun_intensity._float();
				l.m_info.radiance = rad.ToRemixFloat3D();
			}

			l.m_ext.angularDiameterDegrees = cs->light_sun_diameter_degrees._float();
			l.m_ext.volumetricRadianceScale = cs->light_sun_volumetric_scale._float();

			l.m_info.sType = REMIXAPI_STRUCT_TYPE_LIGHT_INFO;
			l.m_info.pNext = &l.m_ext;
			l.m_info.hash = shared::utils::string_hash64("apilight_distant");

			// TODO
			//const auto weather_factor = 1.0f - timecycle::get_bad_weather_factor() * std::clamp(gs->translate_sunlight_intensity_bad_weather_influence._float(), 0.0f, 1.0f);
			//rad *= weather_factor;

			if (api.m_bridge.CreateLight(&l.m_info, &l.m_handle) == REMIXAPI_ERROR_CODE_SUCCESS && l.m_handle) {
				api.m_bridge.DrawLightInstance(l.m_handle);
			}
		}
		else
		{
			if (m_distant_light.m_handle)
			{
				shared::common::remix_api::get().m_bridge.DestroyLight(m_distant_light.m_handle);
				m_distant_light.m_handle = nullptr;
			}
		}
	}



	// keep game running when imgui open, can pause if imgui not open and wnd unfocused
	int game_focused_hk() {
		return shared::globals::imgui_menu_open || *game::game_input_allowed || imgui::get()->m_freecam_mode;
	}

	int game_focused_helper = 0;
	__declspec (naked) void game_focused_stub()
	{
		__asm
		{
			pushad;
			call	game_focused_hk;
			mov		game_focused_helper, eax;
			popad;
			cmp		game_focused_helper, ebx;

			//mov		eax, game::game_input_allowed; // og
			//cmp		dword ptr[eax], ebx;

			jz		PREVENT_INPUT;

			mov		eax, game::retn_addr__game_focused_stub;
			add		eax, 2; // instruction after jz (0x711F12)
			jmp		eax;

		PREVENT_INPUT:
			jmp		game::skip_addr__game_focused_stub; // 0x711F20
		}
	}


	// not needed - debug only
	int compute_visibility_of_cell(game::vis_struct* vis, float* pos, float bounding_radius, float* out_distance)
	{
		const auto im = imgui::get();
		if (im->m_dbg_manual_compute_vis) {
			return im->m_dbg_manual_compute_vis_num;
		}

		bounding_radius += im->m_dbg_compute_vis_bounding_rad_offset;

		const float dx = pos[0] - vis->origin.x;
		const float dy = pos[1] - vis->origin.y;
		const float dz = pos[2] - vis->origin.z;

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

	// -------

	game::vis_struct* tree_cull_stub_cullinfo_helper = nullptr;
	game::visible_state tree_cull_stub_curr_vis_helper;

	game::visible_state anticull_check01(game::tree_node* current)
	{
		if (tree_cull_stub_curr_vis_helper != game::visible_state::inside)
		{
			const auto nocull_dist = comp_settings::get()->nocull_distance._float();
			if (nocull_dist > 0.0f)
			{
				Vector center = (current->bbox_min + current->bbox_max) * 0.5f;
				Vector to_center = center - tree_cull_stub_cullinfo_helper->origin;
				float distance_sqr = to_center.LengthSqr();
				float threshold_sqr = nocull_dist * nocull_dist;

				if (distance_sqr <= threshold_sqr) {
					return game::visible_state::inside; // inside state to bypass all visibility checks
				}
			}
		}

		return tree_cull_stub_curr_vis_helper;
	}

	uint8_t tree_cull_stub_ret_helper;
	__declspec (naked) void tree_cull_stub()
	{
		__asm
		{
			mov		eax, [esp + 0x160];
			mov		tree_cull_stub_cullinfo_helper, eax;
			mov		tree_cull_stub_curr_vis_helper, bl;

			pushad;
			push	ebp; // current
			call	anticull_check01;
			mov		tree_cull_stub_ret_helper, al;
			add		esp, 4;
			popad;

			mov		bl, tree_cull_stub_ret_helper;
			sub     esi, 4;
			dec     edi;
			cmp     bl, 1;

			jmp		game::retn_addr__tree_cull; // 0x79FDDF
		}
	}

	// ---

	int anticull_check02(game::scenery_instance* instance, game::vis_struct* cull_info, game::scenery_info* info)
	{
		const auto im = imgui::get();
		if (im->m_dbg_visualize_model_info) 
		{
			bool has_filter = !im->m_dbg_visualize_model_info_name_filter.empty();
			bool passed_filter = false;
			if (has_filter)
			{
				const auto mdl_lower = shared::utils::str_to_lower(std::string(info->debug_name));
				passed_filter = mdl_lower.contains(im->m_dbg_visualize_model_info_name_filter);
			}

			const float dist_sqr = (instance->position - cull_info->origin).LengthSqr();
			if (dist_sqr < im->m_dbg_visualize_model_info_distance * im->m_dbg_visualize_model_info_distance
					&& (!has_filter || (has_filter && passed_filter)))
			{
				im->m_dbg_visualize_model_info_cam_pos = cull_info->origin; // cam
				im->visualized_model_infos.emplace_back(instance->position, info->debug_name, 
					info->solid_keys[0], info->solid_keys[1], info->solid_keys[2], info->solid_keys[3]);
			}
		}

		const auto nocull_dist = comp_settings::get()->nocull_distance._float();
		if (nocull_dist > 0.0f)
		{
			const float dist_sqr = (instance->position - cull_info->origin).LengthSqr();
			const float threshold_sqr = nocull_dist * nocull_dist;

			if (dist_sqr <= threshold_sqr) {
				return 1;
			}
		}

		return 0;
	}

	__declspec (naked) void draw_a_scenery_stub()
	{
		__asm
		{
			add     esp, 0x10; // after get_pixel_size

			// eax = pixel_síze
			// esi = instance
			// edi = cull_info
			// ebx = info

			//mov		das_stub_pixel_size, eax;
			pushad;
			push	ebx; // info
			push	edi; // cull_info
			push	esi; // instance
			call	anticull_check02;
			add		esp, 12;

			cmp		eax, 1;
			jne		ORG;

			popad;
			jmp		game::commit_addr__draw_a_scenery; // 0x79FC86
		
		ORG:
			popad;
			//mov		eax, das_stub_pixel_size;
			
			// og
			cmp     eax, 1;
			jmp		game::retn_addr__draw_a_scenery;
		}
	}


	int draw_a_scenery_precull_hk(game::scenery_pack* pack, game::scenery_instance* instance, game::vis_struct* cull_info)
	{
		const auto im = imgui::get();

		if (im->m_dbg_anticull_mesh_disable) {
			return 0;
		}

		game::scenery_info& info = pack->infos[instance->scenery_info_number];
		std::int32_t preculler = cull_info->preculler_section_number;

		// only do anti cull check if not vis
		if (preculler >= 0 || ((1u << (preculler & 7)) & pack->preculler_infos[instance->preculler_info_index].visibility_bits[(preculler >> 3)]) != 0)
		{
			const auto ms = map_settings::get_map_settings();
			if (!ms.anticull_meshes.empty())
			{
				std::string mdl_lower;

				for (auto& a : ms.anticull_meshes)
				{
					if (im->m_dbg_anticull_mesh_dist_before_hash)
					{
						float dist_sqr = (instance->position - cull_info->origin).LengthSqr();
						const bool in_range = dist_sqr <= a.distance * a.distance;

						if (!in_range) {
							continue;
						}
					}
					

					bool match = false;

					// fast path
					if (!a.hashes.empty())
					{
						if (info.solid_keys[0] && a.hashes.contains(info.solid_keys[0])) {
							match = true;
						} else if (!im->m_dbg_anticull_mesh_first_hash_only && info.solid_keys[1] && info.solid_keys[0] != info.solid_keys[1] && a.hashes.contains(info.solid_keys[1])) {
							match = true;
						} else if (!im->m_dbg_anticull_mesh_first_hash_only && info.solid_keys[2] && info.solid_keys[1] != info.solid_keys[2] && a.hashes.contains(info.solid_keys[2])) {
							match = true;
						} else if (!im->m_dbg_anticull_mesh_first_hash_only && info.solid_keys[3] && info.solid_keys[2] != info.solid_keys[3] && a.hashes.contains(info.solid_keys[3])) {
							match = true;
						}
					}

					// slow path
					else if (!a.name.empty())
					{
						// get lower case name once
						if (mdl_lower.empty()) {
							mdl_lower = shared::utils::str_to_lower(std::string(info.debug_name));
						}

						if (!a.is_filter)
						{
							if (mdl_lower == a.name) {
								match = true;
							}
						}
						else
						{
							if (mdl_lower.contains(a.name)) {
								match = true;
							}
						}
					}

					if (match)
					{
						if (im->m_dbg_anticull_mesh_dist_before_hash) {
							return 1;
						}

						const float dist_sqr = (instance->position - cull_info->origin).LengthSqr();
						if (dist_sqr <= a.distance * a.distance) {
							return 1;
						}
					}
				}
			}
		}

		return 0;
	}

	__declspec (naked) void draw_a_scenery_precull_stub()
	{
		__asm
		{
			mov		[esp + 0x18], eax;

			// esi = instance
			// eax = pack
			// edi = cull_info

			push	eax; // save eax
			pushad;
			push	edi; // cull_info
			push	esi; // instance
			push	eax; // pack
			call	draw_a_scenery_precull_hk;
			add		esp, 12;

			cmp		eax, 1;
			jne		ORG;

			popad;
			pop		eax; // restore eax
			jmp		game::skip_addr__draw_a_scenery_precull; // 0x79FAAE

		ORG:
			popad;
			pop		eax; // restore eax

			// og
			test    ecx, ecx;
			jmp		game::retn_addr__draw_a_scenery_precull; // 0x79FA88
		}
	}

	void on_begin_scene()
	{
		rain::get()->on_draw();
	}

	void main()
	{
		// #Step 2: init remix api if you want to use it or comment it otherwise
		// Requires "exposeRemixApi = True" in the "bridge.conf" that is located in the .trex folder
		shared::common::remix_api::initialize(on_begin_scene, nullptr, nullptr, false);

		// init modules which do not need to be initialized, before the game inits, here
		shared::common::loader::module_loader::register_module(std::make_unique<map_settings>());
		shared::common::loader::module_loader::register_module(std::make_unique<imgui>());
		shared::common::loader::module_loader::register_module(std::make_unique<renderer>());
		shared::common::loader::module_loader::register_module(std::make_unique<remix_vars>());
		shared::common::loader::module_loader::register_module(std::make_unique<rain>());

		// ---

		// keep game running when imgui open, can pause if imgui not open and wnd unfocused
		shared::utils::hook::nop(game::retn_addr__game_focused_stub - 6u, 6);
		shared::utils::hook(game::retn_addr__game_focused_stub - 6u, game_focused_stub, HOOK_JUMP).install()->quick();

#if 0
		// let GetVisibleStateSB always return 1 - 0x71B630
		shared::utils::hook::set(game::mem_addr__get_vis_state_sb, 
			0xB8, 0x01, 0x00, 0x00, 0x00,  // mov eax, 1
			0xC2, 0x0C, 0x00, 0x90, 0x90); // retn 0xC + 2 nop

		// 'DrawAScenery' - if vis check fn returned less than 18 and viewmode < 3, cull? -> disable that
		shared::utils::hook::nop(game::nop_addr__draw_scenery_chk01, 2); // 0x79FC1C
		//shared::utils::hook::nop(game::nop_addr__draw_scenery_chk02, 6); // draw all no matter what 'compute_visibility_of_cell' returned // 0x79FB3F
#endif

		// 'DrawAScenery' - redirect get_pixel_size check func to our own
		shared::utils::hook(game::hk_addr__draw_scenery_comp_vis_fn_call, compute_visibility_of_cell, HOOK_CALL).install()->quick(); // 0x79FB30

		// 'TreeCull' pre visibility check - mid hook
		shared::utils::hook::nop(game::retn_addr__tree_cull - 7u, 7); 
		shared::utils::hook(game::retn_addr__tree_cull - 7u, tree_cull_stub, HOOK_JUMP).install()->quick(); // 0x79FDDF - 7

		// 'DrawAScenery' - get mesh vis info + commit areas in distance
		shared::utils::hook::nop(game::retn_addr__draw_a_scenery - 6u, 6);
		shared::utils::hook(game::retn_addr__draw_a_scenery - 6u, draw_a_scenery_stub, HOOK_JUMP).install()->quick(); // 0x79FB3B

		// 'DrawAScenery' - precull vis check + map settings mesh anti cull - mid hook
		shared::utils::hook::nop(game::retn_addr__draw_a_scenery_precull - 6u, 6); 
		shared::utils::hook(game::retn_addr__draw_a_scenery_precull - 6u, draw_a_scenery_precull_stub, HOOK_JUMP).install()->quick(); // 0x79FA82

		MH_EnableHook(MH_ALL_HOOKS);
	}
}
