#include "std_include.hpp"

#include "modules/comp_settings.hpp"
#include "modules/imgui.hpp"
#include "modules/map_settings.hpp"
#include "modules/remix_vars.hpp"
#include "modules/renderer.hpp"
#include "shared/common/remix_api.hpp"

namespace comp
{
	void on_begin_scene_cb()
	{
		if (!tex_addons::initialized) {
			tex_addons::init_texture_addons();
		}

		// Actual camera setup here if matrices are available
		// No longer needed
		/*{
			shared::globals::d3d_device->SetTransform(D3DTS_WORLD, &shared::globals::IDENTITY); // does not hurt

			auto sview = reinterpret_cast<game::eViewPlatInterface*>(0xB4AF90);
			if (sview && sview->m_pTransform)
			{
				const auto& im = imgui::get();
				if (im->m_dbg_use_game_matrices)
				{
					shared::globals::d3d_device->SetTransform(D3DTS_VIEW, &sview->m_pTransform->ViewMatrix);
					shared::globals::d3d_device->SetTransform(D3DTS_PROJECTION, &sview->m_pTransform->ProjectionMatrix);
				}
			}
		}*/

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
	}



	// keep game running when imgui open, can pause if imgui not open and wnd unfocused
	int game_focused_hk() {
		return shared::globals::imgui_menu_open || *game::game_input_allowed;
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

	void main()
	{
		// #Step 2: init remix api if you want to use it or comment it otherwise
		// Requires "exposeRemixApi = True" in the "bridge.conf" that is located in the .trex folder
		shared::common::remix_api::initialize(nullptr, nullptr, nullptr, false);

		// init modules which do not need to be initialized, before the game inits, here
		shared::common::loader::module_loader::register_module(std::make_unique<map_settings>());
		shared::common::loader::module_loader::register_module(std::make_unique<imgui>());
		shared::common::loader::module_loader::register_module(std::make_unique<renderer>());

		// #Step 3: hook dinput if your game uses direct input (for ImGui) - ONLY USE ONE
		//shared::common::loader::module_loader::register_module(std::make_unique<shared::common::dinput_v1>()); // v1: might cause issues with the Alt+X menu
		//shared::common::loader::module_loader::register_module(std::make_unique<shared::common::dinput_v2>()); // v2: better but might need further tweaks

		shared::common::loader::module_loader::register_module(std::make_unique<remix_vars>());

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
