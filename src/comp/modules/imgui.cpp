#include "std_include.hpp"
#include "imgui.hpp"

#include "comp_settings.hpp"
#include "d3dxeffects.hpp"
#include "imgui_internal.h"
#include "map_settings.hpp"
#include "rain.hpp"
#include "remix_vars.hpp"
#include "renderer.hpp"
#include "shared/common/imgui_helper.hpp"
#include "shared/common/font_awesome_solid_900.hpp"
#include "shared/common/font_defines.hpp"
#include "shared/common/font_opensans.hpp"
#include "shared/common/remix_api.hpp"
#include "shared/common/toml_ext.hpp"

// Allow us to directly call the ImGui WndProc function.
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

#define CENTER_URL(text, link)					\
	ImGui::SetCursorForCenteredText((text));	\
	ImGui::TextURL((text), (link), true);

#define TT(TXT) ImGui::SetItemTooltipWrapper((TXT));

#define SET_CHILD_WIDGET_WIDTH			ImGui::SetNextItemWidth(ImGui::CalcWidgetWidthForChild(80.0f));
#define SET_CHILD_WIDGET_WIDTH_MAN(V)	ImGui::SetNextItemWidth(ImGui::CalcWidgetWidthForChild((V)));

#define SPACEY16 ImGui::Spacing(0.0f, 16.0f);
#define SPACEY12 ImGui::Spacing(0.0f, 12.0f);
#define SPACEY8 ImGui::Spacing(0.0f, 8.0f);
#define SPACEY4 ImGui::Spacing(0.0f, 4.0f);

namespace comp
{
	WNDPROC g_game_wndproc = nullptr;
	
	LRESULT __stdcall wnd_proc_hk(HWND window, UINT message_type, WPARAM wparam, LPARAM lparam)
	{
		if (message_type != WM_MOUSEMOVE && message_type != WM_NCMOUSEMOVE)
		{
			if (imgui::get()->input_message(message_type, wparam, lparam)) {
			//	return true;
			}
		}

		// if your game has issues with floating cursors
		/*if (message_type == WM_KILLFOCUS)
		{
			uint32_t counter = 0u;
			while (::ShowCursor(TRUE) < 0 && ++counter < 3) {}
			ClipCursor(NULL);
		}*/

		//printf("MSG 0x%x -- w: 0x%x -- l: 0x%x\n", message_type, wparam, lparam);
		return CallWindowProc(g_game_wndproc, window, message_type, wparam, lparam);
	}

	bool imgui::input_message(const UINT message_type, const WPARAM wparam, const LPARAM lparam)
	{
		if (message_type == WM_KEYUP && wparam == VK_F4) 
		{
			const auto& io = ImGui::GetIO();
			if (!io.MouseDown[1]) 
			{
				shared::globals::imgui_menu_open = !shared::globals::imgui_menu_open;
				*game::game_input_allowed = !shared::globals::imgui_menu_open;
			} else {
				ImGui_ImplWin32_WndProcHandler(shared::globals::main_window, message_type, wparam, lparam);
			}
		}

		if (shared::globals::imgui_menu_open || get()->m_freecam_mode)
		{
			//auto& io = ImGui::GetIO();
			ImGui_ImplWin32_WndProcHandler(shared::globals::main_window, message_type, wparam, lparam);
		} else {
			shared::globals::imgui_allow_input_bypass = false; // always reset if there is no imgui window open
		}

		return shared::globals::imgui_menu_open;
	}

	// ------

	void imgui::tab_about()
	{
		if (tex_addons::berry)
		{
			const float cursor_y = ImGui::GetCursorPosY();
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() * 0.85f, 24));
			ImGui::Image((ImTextureID)tex_addons::berry, ImVec2(48.0f, 48.0f), ImVec2(0.03f, 0.03f), ImVec2(0.96f, 0.96f));
			ImGui::SetCursorPosY(cursor_y);
		}

		ImGui::Spacing(0.0f, 20.0f);

		ImGui::CenterText("NFSC RTX REMIX COMPATIBILITY MOD");
		ImGui::CenterText("                      by #xoxor4d");

		ImGui::Spacing(0.0f, 24.0f);
		ImGui::CenterText("current version");

		const char* version_str = shared::utils::va("%d.%d.%d :: %s", 
			COMP_MOD_VERSION_MAJOR, COMP_MOD_VERSION_MINOR, COMP_MOD_VERSION_PATCH, __DATE__);
		ImGui::CenterText(version_str);

#if DEBUG
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.64f, 0.23f, 0.18f, 1.0f));
		ImGui::CenterText("DEBUG BUILD");
		ImGui::PopStyleColor();
#endif

		SPACEY16;
		CENTER_URL("GitHub Repository", "https://github.com/xoxor4d/nfsc-rtx");

		SPACEY16;
		ImGui::Separator();
		SPACEY16;

		const char* credits_title_str = "Credits / Thanks to:";
		ImGui::CenterText(credits_title_str);

		ImGui::Spacing(0.0f, 8.0f);

		CENTER_URL("NVIDIA - RTX Remix", "https://github.com/NVIDIAGameWorks/rtx-remix");
		CENTER_URL("People of the showcase discord", "https://discord.gg/j6sh7JD3v9");
		CENTER_URL("Dear Imgui", "https://github.com/ocornut/imgui");
		CENTER_URL("Minhook", "https://github.com/TsudaKageyu/minhook");
		CENTER_URL("Ultimate-ASI-Loader", "https://github.com/ThirteenAG/Ultimate-ASI-Loader");
		CENTER_URL("MaxHwoy - hyperlinked", "https://github.com/MaxHwoy/hyperlinked");

		CENTER_URL("xan1242 - XNFS-ShaderLoader-Carbon", "https://github.com/xan1242/xnfs-shaderloader-carbon");
		CENTER_URL("ThirteenAG - Widescreen Fix", "https://fusionfix.io/wfp#nfsc");
		CENTER_URL("nlgzrgn- HUD Adapter", "https://nfsmods.xyz/mod/364");
		CENTER_URL("Archie - Help/Guiding/CameraToolkit", "https://github.com/ArchieGoldmill");

		ImGui::Spacing(0.0f, 24.0f);
		ImGui::CenterText("And of course, all my fellow Ko-Fi and Patreon supporters");
		ImGui::CenterText("and all the people that helped along the way.");
		ImGui::Spacing(0.0f, 4.0f);
		ImGui::CenterText("Thank you!");
	}

	// draw imgui widget
	void imgui::ImGuiStats::draw_stats()
	{
		if (!m_tracking_enabled) {
			return;
		}

		for (const auto& p : m_stat_list)
		{
			if (p.second) {
				display_single_stat(p.first, *p.second);
			}
			else {
				ImGui::Spacing(0, 4);
			}
		}
	}

	void imgui::ImGuiStats::display_single_stat(const char* name, const StatObj& stat)
	{
		switch (stat.get_mode())
		{
		case StatObj::Mode::Single:
			ImGui::Text("%s", name);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.65f);
			ImGui::Text("%d total", stat.get_total());
			break;

		case StatObj::Mode::ConditionalCheck:
			ImGui::Text("%s", name);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.65f);
			ImGui::Text("%d total, %d successful", stat.get_total(), stat.get_successful());
			break;

		default:
			throw std::runtime_error("Uncovered Mode in StatObj");
		}
	}

	void dev_debug_container()
	{
		//SPACEY8;
		const auto& im = imgui::get();

		if (ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
		{
			SPACEY4;

			ImGui::Widget_CategoryWithVerticalLabel("Rain", [&]()
				{
					ImGui::PushID("rain");

					bool temp_rain = *game::always_rain;
					if (ImGui::Checkbox("Always Raining", &temp_rain)) {
						*game::always_rain = temp_rain;
					}

					ImGui::BeginDisabled(!temp_rain);
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); 
					if (ImGui::DragFloat("Wetness Value", &im->m_always_rain_wetness_value, 0.01f, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp)) {
						*game::always_rain_intensity = im->m_always_rain_wetness_value; // adjust actual game rain intensity to also reduce rain
					}
					ImGui::EndDisabled();

					ImGui::PopID();
				});

			SPACEY12;

			ImGui::Widget_CategoryWithVerticalLabel("Time of Day", [&]()
				{
					ImGui::PushID("tod");

					ImGui::Checkbox("Use Game Time of Day", &im->m_dbg_sun_time_of_day);
					TT("Use game 'Time of Day' settings to adjust sun color and direction.");

					const auto& cs = comp_settings::get();

					if (ImGui::Button("Preset Mid-Day", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
					{
						const auto dir = cs->light_sun_direction.get_as<Vector*>();
						dir->Set(0.46f, -2.95f, -1.4f);

						const auto col = cs->light_sun_color.get_as<Vector*>();
						col->Set(0.93f, 0.91f, 0.85f);

						cs->light_sun_intensity.set_var(1.6f, true);
						cs->light_sun_volumetric_scale.set_var(0.5f, true);
						cs->light_sun_diameter_degrees.set_var(1.0f, true);

						const auto rtx_skyBrightness = remix_vars::get_option("rtx.skyBrightness");
						const remix_vars::option_value val{ .value = 5.0f };
						remix_vars::set_option(rtx_skyBrightness, val, false, true);
					}

					if (ImGui::Button("Preset Sunrise", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
					{
						const auto dir = cs->light_sun_direction.get_as<Vector*>();
						dir->Set(-6.4f, -8.3f, -2.85f);

						const auto col = cs->light_sun_color.get_as<Vector*>();
						col->Set(0.94f, 0.82f, 0.63f);

						cs->light_sun_intensity.set_var(1.0f, true);
						cs->light_sun_volumetric_scale.set_var(1.0f, true);
						cs->light_sun_diameter_degrees.set_var(1.0f, true);

						const auto rtx_skyBrightness = remix_vars::get_option("rtx.skyBrightness");
						const remix_vars::option_value val{ .value = 1.5f };
						remix_vars::set_option(rtx_skyBrightness, val, false, true);
					}

					if (ImGui::Button("Stock Night", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
					{
						cs->light_sun_direction.reset();
						cs->light_sun_color.reset();
						cs->light_sun_intensity.reset();
						cs->light_sun_volumetric_scale.reset();
						cs->light_sun_diameter_degrees.reset();

						const auto rtx_skyBrightness = remix_vars::get_option("rtx.skyBrightness");
						remix_vars::reset_option(rtx_skyBrightness);
					}

					ImGui::PopID();
				});

			SPACEY4;
		}

		if (ImGui::CollapsingHeader("Enable / Disable Functions"))
		{
			SPACEY4;

			ImGui::Widget_CategoryWithVerticalLabel("Force FF", [&]()
				{
					ImGui::PushID("forceff");
					ImGui::Checkbox("Force FF PrimUp", &im->m_dbg_force_ff_prim_up);
					ImGui::Checkbox("Force FF IndexedPrim", &im->m_dbg_force_ff_indexed_prim);
					ImGui::Checkbox("Force FF IndexedPrim Up", &im->m_dbg_force_ff_indexed_prim_up);
					ImGui::PopID();
				});

			//ImGui::Checkbox("Force FF PrimUp", &im->m_dbg_force_ff_prim_up);
			//ImGui::Checkbox("Force FF IndexedPrim", &im->m_dbg_force_ff_indexed_prim);
			//ImGui::Checkbox("Force FF IndexedPrim Up", &im->m_dbg_force_ff_indexed_prim_up);

			SPACEY12;

			ImGui::Widget_CategoryWithVerticalLabel("Ignore Draw", [&]()
				{
					ImGui::PushID("ignoredraw");
					ImGui::Checkbox("Ignore Prim Drawcalls", &im->m_dbg_disable_prim_draw);
					ImGui::Checkbox("Ignore PrimUp Drawcalls", &im->m_dbg_disable_prim_up_draw);
					ImGui::Checkbox("Ignore IndexedPrim Drawcalls", &im->m_dbg_disable_indexed_prim_draw);
					ImGui::Checkbox("Ignore IndexedPrimUp Drawcalls", &im->m_dbg_disable_indexed_prim_up_draw);
					ImGui::PopID();
				});

			//ImGui::Checkbox("Ignore Prim Drawcalls", &im->m_dbg_disable_prim_draw);
			//ImGui::Checkbox("Ignore PrimUp Drawcalls", &im->m_dbg_disable_prim_up_draw);
			//ImGui::Checkbox("Ignore IndexedPrim Drawcalls", &im->m_dbg_disable_indexed_prim_draw);
			//ImGui::Checkbox("Ignore IndexedPrimUp Drawcalls", &im->m_dbg_disable_indexed_prim_up_draw);

			SPACEY12;

			ImGui::Widget_CategoryWithVerticalLabel("Disable Shader", [&]()
				{
					ImGui::PushID("disableshader");
					ImGui::Checkbox("Disable World Drawcalls", &im->m_dbg_disable_world);
					ImGui::Checkbox("Disable WorldNormal Drawcalls", &im->m_dbg_disable_world_normalmap);
					ImGui::Checkbox("Disable Car Drawcalls", &im->m_dbg_disable_car);
					ImGui::Checkbox("Disable CarNormal Drawcalls", &im->m_dbg_disable_car_normalmap);
					ImGui::Checkbox("Disable Glass Drawcalls", &im->m_dbg_disable_glass);
					ImGui::Checkbox("Disable Sky Drawcalls", &im->m_dbg_disable_sky);
					ImGui::Checkbox("Disable Water Drawcalls", &im->m_dbg_disable_water);
					ImGui::Checkbox("Disable NOFUZZ (FX) Drawcalls", &im->m_dbg_disable_nofuzz);

					ImGui::Checkbox("Disable Remix Car Shader", &im->m_dbg_disable_remix_car_shader);
					ImGui::PopID();
				});

			//ImGui::Checkbox("Disable World Drawcalls", &im->m_dbg_disable_world);
			//ImGui::Checkbox("Disable WorldNormal Drawcalls", &im->m_dbg_disable_world_normalmap);
			//ImGui::Checkbox("Disable Car Drawcalls", &im->m_dbg_disable_car);
			//ImGui::Checkbox("Disable CarNormal Drawcalls", &im->m_dbg_disable_car_normalmap);
			//ImGui::Checkbox("Disable Glass Drawcalls", &im->m_dbg_disable_glass);
			//ImGui::Checkbox("Disable Sky Drawcalls", &im->m_dbg_disable_sky);

			//SPACEY8;

			//ImGui::Checkbox("Disable Remix Car Shader", &im->m_dbg_disable_remix_car_shader);

			SPACEY8;
		}

		if (ImGui::CollapsingHeader("Culling Debug"))
		{
			SPACEY4;

			ImGui::Widget_CategoryWithVerticalLabel("AntiCull", [&]()
				{
					ImGui::PushID("anticull");

					if (game::preculler_mode)
					{
						bool tmp_preculler_bool = *game::preculler_mode;
						if (ImGui::Checkbox("Enable Preculling", &tmp_preculler_bool)) {
							*game::preculler_mode = tmp_preculler_bool;
						} TT("Preculling / Occlusion checks");
					}

					ImGui::Checkbox("AntiCull Mesh check dist before hash", &im->m_dbg_anticull_mesh_dist_before_hash);
					ImGui::Checkbox("AntiCull Mesh check first hash only", &im->m_dbg_anticull_mesh_first_hash_only);
					ImGui::Checkbox("AntiCull Mesh disable", &im->m_dbg_anticull_mesh_disable);

					ImGui::PopID();
				});

			//if (game::preculler_mode)
			//{
			//	bool tmp_preculler_bool = *game::preculler_mode;
			//	if (ImGui::Checkbox("Enable Preculling", &tmp_preculler_bool)) {
			//		*game::preculler_mode = tmp_preculler_bool;
			//	} TT("Preculling / Occlusion checks");
			//}
			
			//ImGui::Checkbox("AntiCull Mesh check dist before hash", &im->m_dbg_anticull_mesh_dist_before_hash);
			//ImGui::Checkbox("AntiCull Mesh check first hash only", &im->m_dbg_anticull_mesh_first_hash_only);
			//ImGui::Checkbox("AntiCull Mesh disable", &im->m_dbg_anticull_mesh_disable);

			SPACEY12;

#if 0
			if (game::drawscenery_cell_dist_check_01) {
				ImGui::DragFloat("DrawScenery CmpFloat 01", game::drawscenery_cell_dist_check_01, 0.01f);
			}

			if (game::drawscenery_cell_dist_check_02) {
				ImGui::DragFloat("DrawScenery CmpFloat 02", game::drawscenery_cell_dist_check_02, 0.01f);
			}

			if (game::drawscenery_cell_dist_check_03) {
				ImGui::DragFloat("DrawScenery CmpFloat 03", game::drawscenery_cell_dist_check_03, 0.01f);
			}
#endif

			ImGui::Widget_CategoryWithVerticalLabel("Experimental", [&]()
				{
					ImGui::PushID("expanticull");

					ImGui::Checkbox("Force Return Value For a Culling Func (PixelSize)", &im->m_dbg_manual_compute_vis);
					ImGui::BeginDisabled(!im->m_dbg_manual_compute_vis);
					{
						SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::DragInt("Forced Return Value", &im->m_dbg_manual_compute_vis_num, 0.2f, 0, 100, "%d", ImGuiSliderFlags_AlwaysClamp);
						ImGui::EndDisabled();
					}

					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::DragFloat("^ BoundingRad Offs", &im->m_dbg_compute_vis_bounding_rad_offset, 0.1f);
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::DragFloat("^ OutDist Offs", &im->m_dbg_compute_vis_out_distance_offset, 0.1f);

					ImGui::PopID();
				});

			SPACEY8;
		}

		if (ImGui::CollapsingHeader("Temp Debug Values"))
		{
			SPACEY4;
			ImGui::DragFloat3("Debug Vector", &im->m_debug_vector.x, 0.01f, 0, 0, "%.6f");
			ImGui::DragFloat3("Debug Vector 2", &im->m_debug_vector2.x, 0.1f, 0, 0, "%.6f");
			ImGui::DragFloat3("Debug Vector 3", &im->m_debug_vector3.x, 0.1f, 0, 0, "%.6f");
			ImGui::DragFloat3("Debug Vector 4", &im->m_debug_vector4.x, 0.1f, 0, 0, "%.6f");
			ImGui::DragFloat3("Debug Vector 5", &im->m_debug_vector5.x, 0.1f, 0, 0, "%.6f");

			ImGui::Checkbox("Debug Bool 1", &im->m_dbg_debug_bool01);
			ImGui::Checkbox("Debug Bool 2", &im->m_dbg_debug_bool02);
			ImGui::Checkbox("Debug Bool 3", &im->m_dbg_debug_bool03);
			ImGui::Checkbox("Debug Bool 4", &im->m_dbg_debug_bool04);
			ImGui::Checkbox("Debug Bool 5", &im->m_dbg_debug_bool05);
			ImGui::Checkbox("Debug Bool 6", &im->m_dbg_debug_bool06);
			ImGui::Checkbox("Debug Bool 7", &im->m_dbg_debug_bool07);
			ImGui::Checkbox("Debug Bool 8", &im->m_dbg_debug_bool08);
			ImGui::Checkbox("Debug Bool 9", &im->m_dbg_debug_bool09);

			ImGui::DragInt("Debug Int 1", &im->m_dbg_int_01, 0.01f);
			ImGui::DragInt("Debug Int 2", &im->m_dbg_int_02, 0.01f);
			ImGui::DragInt("Debug Int 3", &im->m_dbg_int_03, 0.01f);
			ImGui::DragInt("Debug Int 4", &im->m_dbg_int_04, 0.01f);
			ImGui::DragInt("Debug Int 5", &im->m_dbg_int_05, 0.01f);
			SPACEY8;
		}

		if (ImGui::CollapsingHeader("Debug Visualizations"))
		{
			SPACEY4;
			ImGui::Checkbox("Show Camera Information", &im->m_dbg_show_camera_info);
			SPACEY8;
		}

		// 

		if (ImGui::CollapsingHeader("Statistics ..."))
		{
			SPACEY4;
			im->m_stats.enable_tracking(true);
			im->m_stats.draw_stats();

			ImGui::SeparatorText("  Shader Techniques this Frame  ");
			for (auto& s : im->m_vis_used_shader_techniques) {
				ImGui::Text("%s", s.c_str());
			}

			im->m_vis_used_shader_techniques.clear();

			SPACEY8;
		} else {
			im->m_stats.enable_tracking(false);
		}

		if (ImGui::CollapsingHeader("Shader Tech Ignore ..."))
		{
			SPACEY4;

			ImGui::Checkbox("Enable Logic", &im->m_dbg_tech_ignore_logic_enabled);
			ImGui::BeginDisabled(!im->m_dbg_tech_ignore_logic_enabled);
			{
				ImGui::Checkbox("WORLD", &im->m_dbg_tech_ignore_world);
				ImGui::Checkbox("WORLD_1_1", &im->m_dbg_tech_ignore_world_1_1);
				ImGui::Checkbox("WORLD_FIXED", &im->m_dbg_tech_ignore_world_fixed);
				ImGui::Checkbox("WORLD_MIN", &im->m_dbg_tech_ignore_world_min);
				ImGui::Checkbox("WORLD_MASKED", &im->m_dbg_tech_ignore_world_masked);
				ImGui::Checkbox("LOWLOD", &im->m_dbg_tech_ignore_lowlod);
				ImGui::Checkbox("DRYROAD", &im->m_dbg_tech_ignore_dryroad);
				ImGui::Checkbox("RAINING_ON_ROAD", &im->m_dbg_tech_ignore_raining_on_road);
				ImGui::Checkbox("CAR", &im->m_dbg_tech_ignore_car);
				ImGui::Checkbox("CAR_NORMALMAP", &im->m_dbg_tech_ignore_car_normalmap);
				ImGui::Checkbox("MAIN", &im->m_dbg_tech_ignore_main);
				ImGui::Checkbox("MAIN_1_1", &im->m_dbg_tech_ignore_main_1_1);
				ImGui::Checkbox("GLASSREFLECT", &im->m_dbg_tech_ignore_glassreflect);
				ImGui::Checkbox("WATER", &im->m_dbg_tech_ignore_water);
				ImGui::Checkbox("GHOSTCAR", &im->m_dbg_tech_ignore_ghostcar);
				ImGui::Checkbox("SKY", &im->m_dbg_tech_ignore_sky);
				ImGui::Checkbox("FUZZZ", &im->m_dbg_tech_ignore_fuzzz);
				ImGui::Checkbox("NO_FUZZZ", &im->m_dbg_tech_ignore_no_fuzzz);
				ImGui::Checkbox("FLARES", &im->m_dbg_tech_ignore_flares);
				ImGui::Checkbox("STREAK_FLARES", &im->m_dbg_tech_ignore_streak_flares);
				ImGui::Checkbox("SKINNED", &im->m_dbg_tech_ignore_skinned);
				ImGui::Checkbox("DEPTH_TECHNIQUE_NOALPHA", &im->m_dbg_tech_ignore_depth_noalpha);
				ImGui::Checkbox("TSHADER_INSTANCING", &im->m_dbg_tech_ignore_tshader_instance);
				ImGui::Checkbox("BLEND_TEXTURES", &im->m_dbg_tech_ignore_blend_textures);

				ImGui::EndDisabled();
			}
			
			SPACEY8;
		}
		

		if (ImGui::CollapsingHeader("ImGui"))
		{
			SPACEY4;
			const auto coloredit_flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_Float;
			ImGui::ColorEdit4("ContainerBg", &im->ImGuiCol_ContainerBackground.x, coloredit_flags);
			ImGui::ColorEdit4("ContainerBorder", &im->ImGuiCol_ContainerBorder.x, coloredit_flags);
			ImGui::ColorEdit4("FadeContainerBg Start", &im->ImGuiCol_VerticalFadeContainerBackgroundStart.x, coloredit_flags);
			ImGui::ColorEdit4("FadeContainerBg End", &im->ImGuiCol_VerticalFadeContainerBackgroundEnd.x, coloredit_flags);
			SPACEY8;
		}

		if (ImGui::CollapsingHeader("First Material Info (Paint)"))
		{
			im->m_vis_imgui_open = true;
			SPACEY4;

			ImGui::Text("cvDiffuseMin: %.2f, %.2f, %.2f, %.2f",
				im->m_vis_cvDiffuseMin.x, im->m_vis_cvDiffuseMin.y, im->m_vis_cvDiffuseMin.z, im->m_vis_cvDiffuseMin.w);

			ImGui::Text("cvDiffuseRange: %.2f, %.2f, %.2f, %.2f",
				im->m_vis_cvDiffuseRange.x, im->m_vis_cvDiffuseRange.y, im->m_vis_cvDiffuseRange.z, im->m_vis_cvDiffuseRange.w);

			SPACEY4;

			ImGui::Text("cvEnvmapMin: %.2f, %.2f, %.2f, %.2f",
				im->m_vis_cvEnvmapMin.x, im->m_vis_cvEnvmapMin.y, im->m_vis_cvEnvmapMin.z, im->m_vis_cvEnvmapMin.w);

			ImGui::Text("cvEnvmapRange: %.2f, %.2f, %.2f, %.2f",
				im->m_vis_cvEnvmapRange.x, im->m_vis_cvEnvmapRange.y, im->m_vis_cvEnvmapRange.z, im->m_vis_cvEnvmapRange.w);

			SPACEY4;

			ImGui::Text("cvPowers: %.2f, %.2f, %.2f, %.2f",
				im->m_vis_cvPowers.x, im->m_vis_cvPowers.y, im->m_vis_cvPowers.z, im->m_vis_cvPowers.w);

			ImGui::Text("cvClampAndScales: %.2f, %.2f, %.2f, %.2f",
				im->m_vis_cvClampAndScales.x, im->m_vis_cvClampAndScales.y, im->m_vis_cvClampAndScales.z, im->m_vis_cvClampAndScales.w);
			
			SPACEY4;

#define VIS_VEC3(NAME, VAR) {	\
		auto& v = (VAR);		\
			ImGui::Text("FX: " #NAME ": %.2f, %.2f, %.2f,", v.x, v.y, v.z); } \

			ImGui::Text("FX: EnvMinLevel: %.2f", im->m_vis_mat_data.envmap_min_scale);
			VIS_VEC3("EnvMin", im->m_vis_mat_data.envmap_min);

			ImGui::Spacing(0.0f, 2.0f);

			ImGui::Text("FX: EnvMaxLevel: %.2f", im->m_vis_mat_data.envmap_max_scale);
			VIS_VEC3("EnvMax", im->m_vis_mat_data.envmap_max);

			ImGui::Spacing(0.0f, 2.0f);

			ImGui::Text("FX: EnvMapClamp: %.2f", im->m_vis_mat_data.envmap_clamp);
			ImGui::Text("FX: EnvMapPower: %.2f", im->m_vis_mat_data.envmap_power);

			SPACEY8;

			ImGui::Text("FX: SpecularMinLevel: %.2f", im->m_vis_mat_data.specular_min_scale);
			VIS_VEC3("SpecularMin", im->m_vis_mat_data.specular_min);

			ImGui::Spacing(0.0f, 2.0f);

			ImGui::Text("FX: SpecularMaxLevel: %.2f", im->m_vis_mat_data.specular_max_scale);
			VIS_VEC3("SpecularMax", im->m_vis_mat_data.specular_max);

			ImGui::Spacing(0.0f, 2.0f);

			ImGui::Text("FX: SpecularPower: %.2f", im->m_vis_mat_data.specular_power);
			ImGui::Text("FX: SpecularFlakes: %.2f", im->m_vis_mat_data.specular_flakes);

			ImGui::Spacing(0.0f, 2.0f);

			ImGui::Text("FX: Name: %s", im->m_vis_mat_name.c_str());

			ImGui::Text("DETECTED Paint Name: %s", im->m_vis_detected_mat_type.c_str());

			SPACEY4;
	
			/*ImGui::Text("Paint Color: %.2f, %.2f, %.2f, %.2f",
				im->m_vis_paint_color.x, im->m_vis_paint_color.y, im->m_vis_paint_color.z, im->m_vis_paint_color.w);*/

			/*ImGui::Text("= Diffuse: %.2f, %.2f, %.2f, %.2f",
				im->m_vis_paint_color_post.x, im->m_vis_paint_color_post.y, im->m_vis_paint_color_post.z, im->m_vis_paint_color_post.w);*/

			ImGui::Text("= Roughness: %.2f", im->m_vis_out_roughness);
			ImGui::Text("= Metalness: %.2f", im->m_vis_out_metalness);

			SPACEY4;
			ImGui::Separator();
			SPACEY4;

			ImGui::Checkbox("##ovcolor", &im->m_dbg_vehshader_color_override_enabled);
			ImGui::SameLine();
			ImGui::ColorEdit3("Color Overwrite", &im->m_dbg_vehshader_color_override.x, ImGuiColorEditFlags_Float);

			SPACEY4;

			ImGui::Checkbox("##ovroughness", &im->m_dbg_vehshader_roughness_override_enabled);
			ImGui::SameLine();
			ImGui::SliderFloat("Roughness Overwrite", &im->m_dbg_vehshader_roughness_override, 0, 1);

			SPACEY4;

			ImGui::Checkbox("##ovmetal", &im->m_dbg_vehshader_metalness_override_enabled);
			ImGui::SameLine();
			ImGui::SliderFloat("Metalness Overwrite", &im->m_dbg_vehshader_metalness_override, 0, 1);

			SPACEY4;

			ImGui::Checkbox("##ovvinylscale", &im->m_dbg_vehshader_vinylscale_override_enabled);
			ImGui::SameLine();
			ImGui::SliderFloat("Vinylscale Overwrite", &im->m_dbg_vehshader_vinylscale_override, 0, 1);

			SPACEY4;

			ImGui::SeparatorText("  Currently used Materials  ");
			for (auto& s : im->m_vis_used_mat_names) {
				ImGui::Text("%s", s.c_str());
			}

			im->m_vis_used_mat_names.clear();
			im->m_vis_drawcall01 = false;

			SPACEY8;
		}

		if (ImGui::CollapsingHeader("D3DXEffects"))
		{
			SPACEY4;

			// unique techniques discovered at effect-creation time
			const auto& techs = comp::effects::g_discovered_techniques;
			if (ImGui::Button("Print Techniques to Console", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight())))
			{
				for (const auto& name : techs) {
					std::cout << name << std::endl;
				}
			}

			ImGui::Text("Discovered techniques (%zu):", techs.size());
			for (const auto& name : techs) {
				ImGui::BulletText("%s", name.c_str());
			}

			SPACEY4;
		}
	}

	void quicksettings_util_container()
	{
		const auto& im = imgui::get();

		SPACEY4;
		ImGui::SeparatorText("    Screenshot Mode     ");
		SPACEY4;

		
		{
			ImGui::Style_ColorButtonPush(im->m_screenshot_mode ? ImVec4(0.22f, 0.5f, 0.26f, 1.0f) : ImGui::GetStyleColorVec4(ImGuiCol_Button), true);
			if (ImGui::Button("Toggle Screenshot Mode", ImVec2(ImGui::GetContentRegionAvail().x, 48))) {
				im->m_screenshot_mode = !im->m_screenshot_mode;
			}
			ImGui::Style_ColorButtonPop();

			// ---

			SPACEY8;
			ImGui::SeparatorText("    FreeCam     ");
			SPACEY4;

			ImGui::Style_ColorButtonPush(im->m_freecam_mode ? ImVec4(0.22f, 0.5f, 0.26f, 1.0f) : ImGui::GetStyleColorVec4(ImGuiCol_Button), true);
			if (ImGui::Button("FreeCam Mode", ImVec2(ImGui::GetContentRegionAvail().x, 48))) {
				im->m_freecam_mode = !im->m_freecam_mode;
			} TT("Enable FreeCam Mode");
			ImGui::Style_ColorButtonPop();

			SPACEY4;

			if (ImGui::TreeNode("Controls"))
			{
				ImGui::Checkbox("Allow Controls", &im->m_freecam_freeze_allow_controls);
				SPACEY4;

				if (ImGui::BeginTable("##controls", 2, ImGuiTableFlags_Borders, ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn(); ImGui::TextUnformatted("WASD");
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Forward & Strafing");

					ImGui::TableNextColumn(); ImGui::TextUnformatted("Space/C");
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Up & Down");

					ImGui::TableNextColumn(); ImGui::TextUnformatted("R/F");
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Roll");

					ImGui::TableNextColumn(); ImGui::TextUnformatted("Shift");
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Speedup");

					ImGui::TableNextColumn(); ImGui::TextUnformatted("Holding Right Mouse    ");
					ImGui::TableNextColumn(); ImGui::TextUnformatted("Rotate");

					ImGui::EndTable();
				}

				ImGui::Checkbox("Show Control Hint", &im->m_freecam_mode_hint);

				ImGui::TreePop();
			}

			SPACEY4;

			SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::SliderFloat("FreeCam Forward Speed", &im->m_freecam_fwd_speed, 0.01f, 10.0f, "%.2f");
			SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::SliderFloat("FreeCam Strafe Speed", &im->m_freecam_rt_speed, 0.01f, 10.0f, "%.2f");
			SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::SliderFloat("FreeCam Upward Speed", &im->m_freecam_up_speed, 0.01f, 10.0f, "%.2f");
			SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::SliderFloat("FreeCam Roll Speed", &im->m_freecam_roll_speed, 0.001f, 1.0f, "%.3f");
			SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::SliderFloat("FreeCam Mouse Sensitivity", &im->m_freecam_mouse_sensitivity, 0.0001f, 1.0f, "%.4f");

			SPACEY4;
			ImGui::Separator();
			SPACEY4;

			ImGui::BeginDisabled(!im->m_freecam_mode);
			{
				SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::SliderFloat("FreeCam NearClip", &g_freecam.near_clip, 0.001f, 1000.0f, "%.4f");
				SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::SliderFloat("FreeCam FarClip", &g_freecam.far_clip, 0.001f, 100000.0f, "%.4f");

				int temp_fov = g_freecam.horizontal_fov;
				SET_CHILD_WIDGET_WIDTH_MAN(200.0f); if (ImGui::SliderInt("FreeCam HorzFov", &temp_fov, 0, 65535)) {
					g_freecam.horizontal_fov = static_cast<std::uint16_t>(temp_fov);
				}
				ImGui::EndDisabled();
			}
			
			SPACEY4;
		}
	}

	void imgui::tab_utils()
	{
		// utilities
		{
			static float cont_quick_utilities_height = 0.0f;
			cont_quick_utilities_height = ImGui::Widget_ContainerWithCollapsingTitle("Utilities", cont_quick_utilities_height, quicksettings_util_container,
				true, ICON_FA_TERMINAL, &ImGuiCol_ContainerBackground, &ImGuiCol_ContainerBorder);
		}
	}

	void imgui::tab_dev()
	{
		dev_debug_container();
	}

	void cont_compsettings_quick_cmd()
	{
		if (ImGui::Button("Save Current Settings", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0))) {
			comp_settings::write_toml();
		}

		ImGui::SameLine();
		if (ImGui::Button("Reload CompSettings", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
		{
			if (!ImGui::IsPopupOpen("Reload CompSettings?")) {
				ImGui::OpenPopup("Reload CompSettings?");
			}
		}

		SPACEY4;
		ImGui::PushFont(shared::common::font::BOLD);
		ImGui::CenterText("Use Middle Mouse Button to Reset Variables.");
		ImGui::PopFont();
		SPACEY4;

		// popup
		if (ImGui::BeginPopupModal("Reload CompSettings?", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
		{
			ImGui::Spacing(0.0f, 0.0f);

			const auto half_width = ImGui::GetContentRegionMax().x * 0.5f;
			auto line1_str = "You'll loose all unsaved changes if you continue!   ";
			auto line2_str = "To save your changes, use:";
			auto line3_str = "Save Current Settings";

			ImGui::Spacing();
			ImGui::SetCursorPosX(5.0f + half_width - (ImGui::CalcTextSize(line1_str).x * 0.5f));
			ImGui::TextUnformatted(line1_str);

			ImGui::Spacing();
			ImGui::SetCursorPosX(5.0f + half_width - (ImGui::CalcTextSize(line2_str).x * 0.5f));
			ImGui::TextUnformatted(line2_str);

			ImGui::PushFont(shared::common::font::BOLD);
			ImGui::SetCursorPosX(5.0f + half_width - (ImGui::CalcTextSize(line3_str).x * 0.5f));
			ImGui::TextUnformatted(line3_str);
			ImGui::PopFont();

			ImGui::Spacing(0, 8);
			ImGui::Spacing(0, 0); ImGui::SameLine();

			ImVec2 button_size(half_width - 6.0f - ImGui::GetStyle().WindowPadding.x, 0.0f);
			if (ImGui::Button("Reload", button_size))
			{
				comp_settings::parse_toml();
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine(0, 6.0f);
			if (ImGui::Button("Cancel", button_size)) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}


	void compsettings_var_reset_logic(comp_settings::variable& var)
	{
		std::string popup_id = "Reset "s + var.m_name + " ?";

		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && ImGui::IsMouseDown(ImGuiMouseButton_Middle))
		{
			if (!ImGui::IsPopupOpen(popup_id.c_str())) {
				ImGui::OpenPopup(popup_id.c_str());
			}
		}

		ImGui::SetNextWindowSize(ImVec2(300.0f, 140.0f));
		if (ImGui::BeginPopupModal(popup_id.c_str(), nullptr, ImGuiWindowFlags_NoSavedSettings))
		{
			ImGui::Spacing(0.0f, 0.0f);

			ImGui::Spacing();
			ImGui::CenterText("This will reset the current variable");

			ImGui::PushFont(shared::common::font::BOLD);
			ImGui::CenterText("Are you sure?");
			ImGui::PopFont();

			ImGui::Spacing(0, 8);
			ImGui::Spacing(0, 0); ImGui::SameLine();

			const auto half_width = ImGui::GetContentRegionMax().x * 0.5f;
			ImVec2 button_size(half_width - (ImGui::GetStyle().WindowPadding.x * 2.0f) - ImGui::GetStyle().ItemSpacing.x, 0.0f);
			if (ImGui::Button("Yes", button_size))
			{
				var.reset();
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel", button_size)) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	bool compsettings_bool_widget(const char* desc, comp_settings::variable& var)
	{
		const auto cs_var_ptr = var.get_as<bool*>();
		const bool result = ImGui::Checkbox(desc, cs_var_ptr);
		TT(var.get_tooltip_string().c_str());
		compsettings_var_reset_logic(var);
		return result;
	}

	bool compsettings_float_widget(const char* desc, comp_settings::variable& var, const float& min = 0.0f, const float& max = 0.0f, const float& speed = 0.02f)
	{
		const auto cs_var_ptr = var.get_as<float*>();
		const bool result = ImGui::DragFloat(desc, cs_var_ptr, speed, min, max, "%.2f", (min != 0.0f || max != 0.0f) ? ImGuiSliderFlags_AlwaysClamp : ImGuiSliderFlags_None);
		TT(var.get_tooltip_string().c_str());
		compsettings_var_reset_logic(var);
		return result;
	}

	bool compsettings_vec_widget(const char* desc, comp_settings::variable& var, const int& size, const float& min = 0.0f, const float& max = 0.0f, const float& speed = 0.02f)
	{
		const auto cs_var_ptr = var.get_as<float*>();
		bool result = false;
		switch (size)
		{
		case 2:
			assert(var.get_type() == comp_settings::var_type_vec2 && "Type mismatch: expected vec2");
			result = ImGui::DragFloat2(desc, cs_var_ptr, speed, min, max, "%.2f", (min != 0.0f || max != 0.0f) ? ImGuiSliderFlags_AlwaysClamp : ImGuiSliderFlags_None);
			break;

		case 3:
			assert(var.get_type() == comp_settings::var_type_vec3 && "Type mismatch: expected vec3");
			result = ImGui::DragFloat3(desc, cs_var_ptr, speed, min, max, "%.2f", (min != 0.0f || max != 0.0f) ? ImGuiSliderFlags_AlwaysClamp : ImGuiSliderFlags_None);
			break;

		default:
		case 4:
			assert(var.get_type() == comp_settings::var_type_vec4 && "Type mismatch: expected vec4");
			result = ImGui::DragFloat4(desc, cs_var_ptr, speed, min, max, "%.2f", (min != 0.0f || max != 0.0f) ? ImGuiSliderFlags_AlwaysClamp : ImGuiSliderFlags_None);
			break;
		}

		TT(var.get_tooltip_string().c_str());
		compsettings_var_reset_logic(var);
		return result;
	}

	bool compsettings_color_widget(const char* desc, comp_settings::variable& var, const int& size, const ImGuiColorEditFlags_& flags)
	{
		const auto cs_var_ptr = var.get_as<float*>();
		bool result = false;

		switch (size)
		{
		case 3:
			assert(var.get_type() == comp_settings::var_type_vec3 && "Type mismatch: expected vec3");
			result = ImGui::ColorEdit3(desc, cs_var_ptr, flags);
			break;

		default:
		case 4:
			assert(var.get_type() == comp_settings::var_type_vec4 && "Type mismatch: expected vec4");
			result = ImGui::ColorEdit4(desc, cs_var_ptr, flags);
			break;
		}

		TT(var.get_tooltip_string().c_str());
		compsettings_var_reset_logic(var);
		return result;
	}

	bool compsettings_remix_vec_widget(const char* desc, comp_settings::variable& var, const int& size, const float& min = 0.0f, const float& max = 0.0f, const float& speed = 0.02f, const float next_width = 200.0f)
	{
		bool result = false;
		ImGui::BeginGroup();
		switch (size)
		{
		case 2:
			assert(var.get_type() == comp_settings::var_type_remix_float2d_array && "Type mismatch: expected remix_float2d");
			SET_CHILD_WIDGET_WIDTH_MAN(next_width); result = ImGui::DragFloat2(shared::utils::va("%s #1", desc), &var._remix2d_ptr(0)->x, speed, min, max, "%.2f", (min != 0.0f || max != 0.0f) ? ImGuiSliderFlags_AlwaysClamp : ImGuiSliderFlags_None);
			SET_CHILD_WIDGET_WIDTH_MAN(next_width); result = ImGui::DragFloat2(shared::utils::va("%s #2", desc), &var._remix2d_ptr(1)->x, speed, min, max, "%.2f", (min != 0.0f || max != 0.0f) ? ImGuiSliderFlags_AlwaysClamp : ImGuiSliderFlags_None) ? true : result;
			break;

		case 3:
			assert(var.get_type() == comp_settings::var_type_remix_float3d_array && "Type mismatch: expected remix_float3d");
			SET_CHILD_WIDGET_WIDTH_MAN(next_width); result = ImGui::DragFloat3(shared::utils::va("%s #1", desc), &var._remix3d_ptr(0)->x, speed, min, max, "%.2f", (min != 0.0f || max != 0.0f) ? ImGuiSliderFlags_AlwaysClamp : ImGuiSliderFlags_None);
			SET_CHILD_WIDGET_WIDTH_MAN(next_width); result = ImGui::DragFloat3(shared::utils::va("%s #2", desc), &var._remix3d_ptr(1)->x, speed, min, max, "%.2f", (min != 0.0f || max != 0.0f) ? ImGuiSliderFlags_AlwaysClamp : ImGuiSliderFlags_None) ? true : result;
			break;

		default:
		case 4:
			assert(var.get_type() == comp_settings::var_type_remix_float4d_array && "Type mismatch: expected remix_float4d");
			SET_CHILD_WIDGET_WIDTH_MAN(next_width); result = ImGui::DragFloat4(shared::utils::va("%s #1", desc), &var._remix4d_ptr(0)->x, speed, min, max, "%.2f", (min != 0.0f || max != 0.0f) ? ImGuiSliderFlags_AlwaysClamp : ImGuiSliderFlags_None);
			SET_CHILD_WIDGET_WIDTH_MAN(next_width); result = ImGui::DragFloat4(shared::utils::va("%s #2", desc), &var._remix4d_ptr(1)->x, speed, min, max, "%.2f", (min != 0.0f || max != 0.0f) ? ImGuiSliderFlags_AlwaysClamp : ImGuiSliderFlags_None) ? true : result;
			break;
		}
		ImGui::EndGroup();
		TT(var.get_tooltip_string().c_str());
		compsettings_var_reset_logic(var);
		return result;
	}

	bool compsettings_remix_color_widget(const char* desc, comp_settings::variable& var, const int& size, const ImGuiColorEditFlags_& flags, const float next_width = 200.0f)
	{
		bool result = false;

		ImGui::BeginGroup();
		switch (size)
		{
		case 3:
			assert(var.get_type() == comp_settings::var_type_remix_float3d_array && "Type mismatch: expected remix_float3d");
			SET_CHILD_WIDGET_WIDTH_MAN(next_width); result = ImGui::ColorEdit3(shared::utils::va("%s #1", desc), &var._remix3d_ptr(0)->x, flags);
			SET_CHILD_WIDGET_WIDTH_MAN(next_width); result = ImGui::ColorEdit3(shared::utils::va("%s #2", desc), &var._remix3d_ptr(1)->x, flags) ? true : result;
			break;

		default:
		case 4:
			assert(var.get_type() == comp_settings::var_type_remix_float4d_array && "Type mismatch: expected remix_float4d");
			SET_CHILD_WIDGET_WIDTH_MAN(next_width); result = ImGui::ColorEdit4(shared::utils::va("%s #1", desc), &var._remix4d_ptr(0)->x, flags);
			SET_CHILD_WIDGET_WIDTH_MAN(next_width); result = ImGui::ColorEdit4(shared::utils::va("%s #2", desc), &var._remix4d_ptr(1)->x, flags) ? true : result;
			break;
		}
		ImGui::EndGroup();
		TT(var.get_tooltip_string().c_str());
		compsettings_var_reset_logic(var);
		return result;
	}

	void compsettings_culling_container()
	{
		const auto& cs = comp_settings::get();

		SPACEY4;
		ImGui::SeparatorText(" Anti Culling ");
		SPACEY4;

		ImGui::Widget_CategoryWithVerticalLabel("World", [&]()
			{
				ImGui::PushID("world");
				SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("No Culling Distance", cs->nocull_distance, 0.0f, FLT_MAX, 0.5f);
				ImGui::PopID();
			});

		SPACEY4;
	}

	void compsettings_light_container()
	{
		const auto& cs = comp_settings::get();

		SPACEY4;
		ImGui::SeparatorText(" Distant Light ");
		SPACEY4;

		ImGui::Widget_CategoryWithVerticalLabel("Sun", [&]()
			{
				ImGui::PushID("sunlight");
				compsettings_bool_widget("Enable Sun", cs->light_sun_enable);
				SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_vec_widget("Direction", cs->light_sun_direction, 3, -360.0f, 360.0f, 0.05f); 
				SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Intensity", cs->light_sun_intensity, 0.0f, 100.0f, 0.01f);
				SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_color_widget("Color", cs->light_sun_color, 3, ImGuiColorEditFlags_Float);
				SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Volumetric Scale", cs->light_sun_volumetric_scale, 0.0f, 10.0f, 0.01f);
				SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Diameter Deg", cs->light_sun_diameter_degrees, 0.0f, 180.0f, 0.01f);
				ImGui::PopID();
			});

		SPACEY4;
	}

	void compsettings_rendering_container()
	{
		const auto& cs = comp_settings::get();

		SPACEY4;
		ImGui::SeparatorText(" Vertex Colors ");
		SPACEY4;

		ImGui::Widget_CategoryWithVerticalLabel("Vertex Colors", [&]()
			{
				ImGui::PushID("vercol");
				compsettings_bool_widget("Enable Vertex Colors Globally", cs->vertex_colors_global);
				compsettings_bool_widget("Enable Vertex Colors on Particles", cs->vertex_colors_particles);
				compsettings_bool_widget("Enable Vertex Colors on World Meshes", cs->vertex_colors_world);
				ImGui::PopID();
			});

		SPACEY4;
		ImGui::SeparatorText(" Effects ");
		SPACEY4;

		ImGui::Widget_CategoryWithVerticalLabel("Flares", [&]()
			{
				ImGui::PushID("flares");
				compsettings_bool_widget("Draw Flares", cs->flare_enabled);
				SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Flare Alpha Multiplier", cs->flare_alpha_multiplier, 0.0f, 1.0f, 0.01f);
				ImGui::PopID();
			});

		SPACEY8;
		ImGui::SeparatorText(" Wetness ");
		SPACEY4;

		ImGui::Widget_CategoryWithVerticalLabel("World", [&]()
			{
				ImGui::PushID("world");
				compsettings_bool_widget("Enable World Wetness", cs->wetness_world);
				compsettings_bool_widget("Enable Variation", cs->wetness_world_variation);
				compsettings_bool_widget("Enable Puddles", cs->wetness_world_puddles);
				compsettings_bool_widget("Enable Occlusion Check", cs->wetness_world_occlusion_check);
				compsettings_bool_widget("Enable Occlusion Smoothing", cs->wetness_world_occlusion_smoothing);
				compsettings_bool_widget("Enable Raindrops", cs->wetness_world_raindrops);
				SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Raindrop Scale", cs->wetness_world_raindrop_scale, 0.0f, 1.0f, 0.01f);
				ImGui::PopID();
			});

		SPACEY12;

		ImGui::Widget_CategoryWithVerticalLabel("Car", [&]()
			{
				ImGui::PushID("car");
				compsettings_bool_widget("Car Raindrops", cs->wetness_car_raindrops);
				ImGui::PopID();
			});

		SPACEY12;

		ImGui::Widget_CategoryWithVerticalLabel("Camera", [&]()
			{
				ImGui::PushID("camera");
				compsettings_bool_widget("Camera Raindrops", cs->enable_camera_raindrops);
				ImGui::PopID();
			});

		SPACEY4;
	}

	void compsettings_rain_container()
	{
		//const auto& cs = comp_settings::get();
		const auto im = imgui::get();
		auto cs = comp_settings::get();

		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);
		const auto spacing = ImGui::GetStyle().ItemSpacing;

		SPACEY4;
		auto& p = rain::get()->m_remix_particle;

		auto save_logo = []() 
		{
				ImGui::SameLine(0, 3.0f); ImGui::TextDisabled("  *  ");
				
				ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.124f, 0.124f, 0.124f, 0.776f));
				if (!ImGui::BeginItemTooltip())
				{
					ImGui::PopStyleColor();
					return;
				}
				ImGui::PopStyleColor();

				const auto padding = 4.0f;

				ImGui::Spacing(0, padding);	// top padding
				ImGui::Spacing(padding, 0); ImGui::SameLine(); // left padding

				ImGui::TextUnformatted("This is a saved Setting.");

				ImGui::SameLine(); ImGui::Spacing(padding, 0); // right padding
				ImGui::Spacing(0, padding);	// bottom padding

				ImGui::EndTooltip();
			};

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing.x, 0.0f));
		if (ImGui::CollapsingHeader(" Remix Variables ", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::PopStyleVar();
			ImGui::Widget_CategoryWithVerticalLabel("Remix Vars", [&]()
				{
					ImGui::PushID("remixvar");

					compsettings_bool_widget("Volumetric Fog Influence", cs->rain_volumetric_fog_influence_enable); save_logo();
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Low Transmission Value", cs->rain_volumetric_fog_influence_low_transmission_val, 0.1f, 500.0f, 0.01f); save_logo();
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("High Transmission Value", cs->rain_volumetric_fog_influence_high_transmission_val, 0.1f, 500.0f, 0.01f); save_logo();

					ImGui::PopID();
				});
		}
		else {
			ImGui::PopStyleVar();
		}

		SPACEY12;
		ImGui::Separator();

		SPACEY12;
		compsettings_bool_widget("Enable Particle System", cs->rain_enable); save_logo();
		ImGui::Checkbox("Edit Mode", &p.edit_mode); TT("Always update the particle system variables.");
		ImGui::Checkbox("Force On", &p.force_enable); TT("Always draw the rain particle system.");

		SPACEY12;
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing.x, 0.0f));
		if (ImGui::CollapsingHeader(" Spawner / Material Settings", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::PopStyleVar();
			ImGui::Widget_CategoryWithVerticalLabel("Spawner / Material", [&]()
				{
					ImGui::PushID("spawnmat");

					ImGui::Checkbox("Use Drawcall Alpha", &p.use_drawcall_alpha);
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::SliderInt("BlendType", &p.blendtype, 0, 12);

					SPACEY4;
					compsettings_bool_widget("Use Emissive Texture", cs->rain_use_emissive_texture); save_logo();
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Emissive Intensity", cs->rain_emissive_intensity, 0.0f, 1.0f, 0.01f); save_logo();
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_color_widget("Emissive Color", cs->rain_emissive_color, 3, ImGuiColorEditFlags_Float); save_logo();

					SPACEY4;
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Metallic Constant", cs->rain_metallic_constant, 0.0f, 1.0f, 0.01f); save_logo();
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Roughness Constant", cs->rain_roughness_constant, 0.0f, 1.0f, 0.01f); save_logo();
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Spawner Scale", cs->rain_spawner_scale, 0.0f, 100.0f, 0.05f); save_logo();

					SPACEY4;
					if (ImGui::Button("Recreate System", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f))) {
						p.internal_recreate_material = true;
					} TT("Mesh/Material need to be recreated for these settings to apply.");

					ImGui::PopID();
				});
			SPACEY12;
		}
		else {
			ImGui::PopStyleVar();
		}


		ImGui::Spacing(0, spacing.y);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing.x, 0.0f));
		if (ImGui::CollapsingHeader("Alpha / Blend / General Settings"))
		{
			ImGui::PopStyleVar();
			ImGui::Widget_CategoryWithVerticalLabel("General", [&]()
				{
					ImGui::PushID("general");

					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::SliderInt("Category", &p.category, 0, 24);
					SPACEY4;
					ImGui::Checkbox("Use TFACTOR", &p.use_tfactor);
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::ColorEdit4("TFACTOR Color", &p.tfactor_col.x);

					ImGui::PopID();
				});

			SPACEY8;
			ImGui::Widget_CategoryWithVerticalLabel("Alpha / Blend", [&]()
				{
					ImGui::PushID("alphablend");

					ImGui::Checkbox("Alpha Blending", &p.alpha_blend);
					ImGui::Checkbox("Alpha Test", &p.alpha_test);
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::SliderInt("Alpha Test OP", &p.alpha_test_op, 0, 12);
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::DragInt("Alpha Test Val", &p.alpha_test_val, 0.2f, 0, 255);

					ImGui::PopID();
				});
			SPACEY12;
		}
		else {
			ImGui::PopStyleVar();
		}


		ImGui::Spacing(0, spacing.y);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing.x, 0.0f));
		if (ImGui::CollapsingHeader("Texture OP"))
		{
			ImGui::PopStyleVar();
			ImGui::Widget_CategoryWithVerticalLabel("Tex OP", [&]()
				{
					ImGui::PushID("texop");

					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::SliderInt("Col SRC Blend", &p.col_src_blend, 0, 12);
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::SliderInt("Col DEST Blend", &p.col_dst_blend, 0, 12);
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::SliderInt("Col Blend OP", &p.col_blend_op, 0, 12);
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::SliderInt("Col ARG1", &p.col_arg1, 0, 12);
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::SliderInt("Col ARG2", &p.col_arg2, 0, 12);
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::SliderInt("Col OP", &p.col_op, 0, 12);

					SPACEY4;
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::SliderInt("Alpha SRC Blend", &p.alpha_src_blend, 0, 12);
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::SliderInt("Alpha DEST Blend", &p.alpha_dst_blend, 0, 12);
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::SliderInt("Alpha Blend OP", &p.alpha_blend_op, 0, 12);
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::SliderInt("Alpha ARG1", &p.alpha_arg1, 0, 12);
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::SliderInt("Alpha ARG2", &p.alpha_arg2, 0, 12);
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::SliderInt("Alpha OP", &p.alpha_op, 0, 12);

					ImGui::PopID();
				});
			SPACEY12;
		}
		else {
			ImGui::PopStyleVar();
		}


		ImGui::Spacing(0, spacing.y);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing.x, 0.0f));
		if (ImGui::CollapsingHeader(" Particle Spawn Position / Angle ", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::PopStyleVar();
			ImGui::Widget_CategoryWithVerticalLabel("Position / Angle", [&]()
				{
					ImGui::PushID("partposangle");
					
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_vec_widget("Position Offset", cs->rain_position_offset, 3, -100.0f, 100.0f, 0.05f); save_logo();
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_vec_widget("Rotation Offset", cs->rain_rotation_offset, 3, -360.0f, 360.0f, 0.05f); save_logo();
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Cam Forward Offset", cs->rain_cam_forward_offset, -1000.0f, 1000.0f, 0.05f); save_logo();
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Cam Vel Forward Scale", cs->rain_cam_velocity_forward_scale, -1000.0f, 1000.0f, 0.05f); save_logo();
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Cam Vel Pitch Scale", cs->rain_cam_velocity_spawner_pitch_scale, 0.0f, 500.0f, 0.05f); save_logo();
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Cam Vel Pitch Max Deg", cs->rain_cam_velocity_spawner_pitch_max, -90.0f, 90.0f, 0.05f); save_logo();

					ImGui::Checkbox("Rotate Spawner YAW based on Camera", &p.yaw_rotate_spawner_based_on_cam);
					compsettings_bool_widget("Rotate Spawner PITCH based on Camera", cs->rain_pitch_rotate_spawner_based_on_cam); save_logo();

					ImGui::PopID();
				});
			SPACEY12;
		}
		else {
			ImGui::PopStyleVar();
		}


		ImGui::Spacing(0, spacing.y);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing.x, 0.0f));
		if (ImGui::CollapsingHeader(" Particle Spawn Settings ", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::PopStyleVar();
			ImGui::Widget_CategoryWithVerticalLabel("Spawn Settings", [&]()
				{
					ImGui::PushID("partspawn");

					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::DragInt("Num Particles", &p.num_particles);

					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::DragFloat("Spawn Rate", &p.spawn_rate, 0.25f, 0.0f, 10000.0f);
					TT("Spawn Rate when rain is forced on.");

					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Spawn Rate Multi", cs->rain_spawn_rate_game_multi, 0.0f, 100.0f, 0.01f); save_logo();
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Spawn Rate Speed Scalar", cs->rain_spawn_rate_game_multi_speed_scalar, 0.0f, 100.0f, 0.01f); save_logo();
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Spawn Rate Multi LowerLim", cs->rain_spawn_rate_game_multi_lower_limit, 0.0f, 100.0f, 0.01f); save_logo();
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Spawn Rate Multi UpperLim", cs->rain_spawn_rate_game_multi_upper_limit, 0.0f, 100.0f, 0.01f); save_logo();

					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::DragFloat("Spawn Burst Duration", &p.spawn_burst_duration, 0.01f, 0.0f, 1000.0f);

					SPACEY4;
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Min Lifetime", cs->rain_min_lifetime, 0.01f, 20.0f, 0.001f); save_logo();
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Max Lifetime", cs->rain_max_lifetime, 0.01f, 20.0f, 0.001f); save_logo();

					SPACEY4;
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Initial Rotation Deg", cs->rain_initial_rotation_degrees, 0.00f, 360.0f, 0.05f); save_logo();

					SPACEY4;
					compsettings_remix_vec_widget("Min Size Key", cs->rain_min_size_keyframes, 2, 0.001f, 5.0f, 0.05f); save_logo();

					SPACEY4;
					compsettings_remix_vec_widget("Max Size Key", cs->rain_max_size_keyframes, 2, 0.001f, 5.0f, 0.05f); save_logo();

					ImGui::PopID();
				});
			SPACEY12;
		}
		else {
			ImGui::PopStyleVar();
		}


		ImGui::Spacing(0, spacing.y);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing.x, 0.0f));
		if (ImGui::CollapsingHeader(" Particle Visual Settings ", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::PopStyleVar();
			ImGui::Widget_CategoryWithVerticalLabel("Visual Settings", [&]()
				{
					ImGui::PushID("partvisual");

					ImGui::Checkbox("Use Spawn Texcoords", &p.use_spawn_texcoords);
					ImGui::Checkbox("Hide Emitter", &p.hide_emitter);

					SPACEY4;
					compsettings_bool_widget("Enable Motion Trail", cs->rain_enable_motion_trail); save_logo();
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Motion Trail Multiplier", cs->rain_motion_trail_multi, 0.0f, 100.0f, 0.01f); save_logo();

					SPACEY4;
					compsettings_remix_color_widget("Min Particle Color Key", cs->rain_min_color_keyframes, 4, ImGuiColorEditFlags_Float); save_logo();
					SPACEY4;
					compsettings_remix_color_widget("Max Particle Color Key", cs->rain_max_color_keyframes, 4, ImGuiColorEditFlags_Float); save_logo();

					SPACEY4;
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f);
					int temp_billboard_type = p.billboard_type;
					if (ImGui::DragInt("Billboard Type", &temp_billboard_type)) {
						p.billboard_type = static_cast<::uint8_t>(temp_billboard_type);
					}

					SET_CHILD_WIDGET_WIDTH_MAN(200.0f);
					int temp_spritesheet = p.sprite_sheet_mode;
					if (ImGui::DragInt("Spritesheet Mode", &temp_spritesheet)) {
						p.sprite_sheet_mode = static_cast<::uint8_t>(temp_spritesheet);
					}

					SET_CHILD_WIDGET_WIDTH_MAN(200.0f);
					int temp_flipaxis = p.random_flip_axis;
					if (ImGui::DragInt("Random Flip Axis", &temp_flipaxis)) {
						p.random_flip_axis = static_cast<::uint8_t>(temp_flipaxis);
					}

					ImGui::PopID();
				});
			SPACEY12;
		}
		else {
			ImGui::PopStyleVar();
		}


		ImGui::Spacing(0, spacing.y);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing.x, 0.0f));
		if (ImGui::CollapsingHeader(" Particle Velocity Settings ", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::PopStyleVar();
			ImGui::Widget_CategoryWithVerticalLabel("Velocity Settings", [&]()
				{
					ImGui::PushID("partvel");

					// Only a single KeyFrame in use
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::DragFloat3("Max Velocity", &cs->rain_max_velocity._remix3d_ptr(0)->x, 0.05f, 0.0f, 1000.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp); save_logo();

					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Init Vel from Normal", cs->rain_initial_velocity_from_normal, -1000.0f, 1000.0f, 0.05f); save_logo();
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Init Vel Cone Angle", cs->rain_initial_velocity_cone_angle_degrees, -1000.0f, 1000.0f, 0.05f); save_logo();
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Init Vel from Motion", cs->rain_initial_velocity_from_motion, -1000.0f, 1000.0f, 0.05f); save_logo();

					SPACEY4;
					ImGui::Checkbox("Align to Velocity", &p.align_to_velocity);
					ImGui::Checkbox("Restrict Velocity X", &p.restrict_velocity_x);
					ImGui::Checkbox("Restrict Velocity Y", &p.restrict_velocity_y);
					ImGui::Checkbox("Restrict Velocity Z", &p.restrict_velocity_z);

					ImGui::PopID();
				});
			SPACEY12;
		}
		else {
			ImGui::PopStyleVar();
		}


		ImGui::Spacing(0, spacing.y);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing.x, 0.0f));
		if (ImGui::CollapsingHeader(" Particle Force Settings ", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::PopStyleVar();
			ImGui::Widget_CategoryWithVerticalLabel("Force Settings", [&]()
				{
					ImGui::PushID("partforce");

					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::DragFloat3("Attractor Position", &p.attractor_position.x, 0.05f, -FLT_MAX, FLT_MAX);
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Attractor Radius", cs->rain_attractor_radius, 0.0f, 1000.0f, 0.01f); save_logo();
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Attractor Force", cs->rain_attractor_force, 0.0f, 10000.0f, 0.01f); save_logo();
					ImGui::Checkbox("Use Camera Pos as Attractor", &p.use_cam_as_attractor);

					SPACEY4;
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget("Gravity Force", cs->rain_gravity_force, -1000.0f, 1000.0f, 0.01f); save_logo();
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::DragFloat("Turbulence Freq", &p.turbulence_freq, 0.05f, -1000.0f, 1000.0f);
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::DragFloat("Turbulence Force", &p.turbulence_force, 0.05f, -1000.0f, 1000.0f);
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::DragFloat("Drag", &p.drag, 0.05f, 0.001f, 100.0f);

					ImGui::PopID();
				});
			SPACEY12;
		}
		else {
			ImGui::PopStyleVar();
		}


		ImGui::Spacing(0, spacing.y);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing.x, 0.0f));
		if (ImGui::CollapsingHeader(" Particle Collision Settings "))
		{
			ImGui::PopStyleVar();
			ImGui::Widget_CategoryWithVerticalLabel("Collision Settings", [&]()
				{
					ImGui::PushID("partcollision");

					ImGui::Checkbox("Enable Collision", &p.enable_collision);

					SET_CHILD_WIDGET_WIDTH_MAN(200.0f);
					int temp_collision_mode = p.collision_mode;
					if (ImGui::DragInt("Collision Mode", &temp_collision_mode)) {
						p.collision_mode = static_cast<::uint8_t>(temp_collision_mode);
					}

					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::DragFloat("Collision Thickness", &p.collision_thickness, 0.01f, 0.0f, 10.0f);
					SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::DragFloat("Collision Restitution", &p.collision_restitution, 0.01f, 0.0f, 100.0f);

					ImGui::PopID();
				});
			SPACEY12;
		}
		else {
			ImGui::PopStyleVar();
		}


		ImGui::Spacing(0, spacing.y);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing.x, 0.0f));
		if (ImGui::CollapsingHeader(" Particle Debug "))
		{
			ImGui::PopStyleVar();
			ImGui::Widget_CategoryWithVerticalLabel("Particle Debug", [&]()
				{
					const auto& pinfo = rain::get()->get_particle_system_info();
					ImGui::PushID("partdebug");

					ImGui::Text("Cam Pos: %.2f, %.2f, %.2f", im->m_dbg_vis_camera_pos.x, im->m_dbg_vis_camera_pos.y, im->m_dbg_vis_camera_pos.z);
					ImGui::Text("Cam Dir: %.2f, %.2f, %.2f", im->m_dbg_vis_camera_dir.x, im->m_dbg_vis_camera_dir.y, im->m_dbg_vis_camera_dir.z);

					SPACEY4;
					ImGui::Text("Cam Target: %.2f, %.2f, %.2f", im->m_dbg_vis_camera_target.x, im->m_dbg_vis_camera_target.y, im->m_dbg_vis_camera_target.z);
					ImGui::Text("Cam Velocity: %.2f, %.2f, %.2f", im->m_dbg_vis_camera_velocity.x, im->m_dbg_vis_camera_velocity.y, im->m_dbg_vis_camera_velocity.z);

					SPACEY4;
					ImGui::Text("= Final Rain Pos: %.2f, %.2f, %.2f", im->m_dbg_vis_camera_final_rain_pos.x, im->m_dbg_vis_camera_final_rain_pos.y, im->m_dbg_vis_camera_final_rain_pos.z);

					SPACEY4;
					ImGui::Separator();
					SPACEY4;
					ImGui::Text("Game Raindrop Count: %d", im->m_dbg_vis_game_raindrop_count);
					ImGui::Text("Game Raindrop Count Scaled: %.2f", pinfo.spawnRatePerSecond);

					ImGui::PopID();
				});
		}
		else {
			ImGui::PopStyleVar();
		}

		SPACEY4;
		ImGui::PopStyleVar();
	}

	void compsettings_material_container()
	{
		const auto& cs = comp_settings::get();

		SPACEY4;
		ImGui::SeparatorText(" Paint Settings ");
		SPACEY4;

#define MAT_OPTIONS(NAME, VAR) { \
			ImGui::PushID(#NAME); \
			SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget(" Roughness", cs->VAR##_roughness, 0.0f, 1.0f, 0.001f); \
			SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget(" Metalness", cs->VAR##_metalness, 0.0f, 1.0f, 0.001f); \
			SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget(" View Scalar", cs->VAR##_view_scalar, 0.0f, 4.0f, 0.001f); \
			SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget(" View Primary Diffuse Scalar", cs->VAR##_view_primary_color_scalar, 0.0f, 4.0f, 0.001f); \
			SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget(" View Primary Diffuse Blend", cs->VAR##_view_primary_color_blend_scalar, 0.0f, 4.0f, 0.001f); \
			ImGui::PopID(); \
			SPACEY4; }

		ImGui::Widget_CategoryWithVerticalLabel("Perl", [&]() {
				MAT_OPTIONS("Perl", mat_perl);
			});

		SPACEY12;
		ImGui::Widget_CategoryWithVerticalLabel("Matte", [&]() {
				MAT_OPTIONS("Matte", mat_matte);
			});

		SPACEY12;
		ImGui::Widget_CategoryWithVerticalLabel("Metallic", [&]() {
				MAT_OPTIONS("Metallic", mat_metallic);
			});

		SPACEY12;
		ImGui::Widget_CategoryWithVerticalLabel("HighGloss", [&]() {
				MAT_OPTIONS("HighGloss", mat_high_gloss);
			});

		SPACEY12;
		ImGui::Widget_CategoryWithVerticalLabel("Iridiance", [&]() {
				MAT_OPTIONS("Iridiance", mat_iridiance);
			});

		SPACEY12;
		ImGui::Widget_CategoryWithVerticalLabel("Candy", [&]() {
				MAT_OPTIONS("Candy", mat_candy);
			});

		SPACEY12;
		ImGui::Widget_CategoryWithVerticalLabel("Chrome", [&]() {
				MAT_OPTIONS("Chrome", mat_chrome);
			});

		//MAT_OPTIONS("Perl", mat_perl);
		//MAT_OPTIONS("Matte", mat_matte);
		//MAT_OPTIONS("Metallic", mat_metallic);
		//MAT_OPTIONS("HighGloss", mat_high_gloss);
		//MAT_OPTIONS("Iridiance", mat_iridiance);
		//MAT_OPTIONS("Candy", mat_candy);
		//MAT_OPTIONS("Chrome", mat_chrome);

#undef MAT_OPTIONS

		SPACEY4;
	}


	void compsettings_other_container()
	{
		static const auto& cs = comp_settings::get();

		SPACEY4;
		ImGui::SeparatorText(" Remix ");
		SPACEY4;

		ImGui::Widget_CategoryWithVerticalLabel("RTXDI", [&]()
			{
				ImGui::PushID("rtx");
				SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::DragInt("RTXDI Initial Sample Count Override", cs->remix_override_rtxdi_samplecount.get_as<int*>(), 0.01f);
				TT(cs->remix_override_rtxdi_samplecount.get_tooltip_string().c_str());
				ImGui::PopID();
			});

		SPACEY4;
		ImGui::SeparatorText(" Volumetric Related ");
		SPACEY4;

		ImGui::Widget_CategoryWithVerticalLabel("Atmosphere Height", [&]()
			{
				ImGui::PushID("atmosheight");
				compsettings_bool_widget("Enable AtmosphereHeight Adjustment", cs->remix_sky_horizon_height_adjustment);
				SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_vec_widget("Min Height", cs->remix_sky_horizon_height_min, 2, 0.0f, 2000.0f);
				SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_vec_widget("Max Height", cs->remix_sky_horizon_height_max, 2, 0.0f, 2000.0f);
				ImGui::PopID();
			});

		SPACEY4;
	}

	void imgui::tab_compsettings()
	{
		const auto& im = imgui::get();

		// quick commands
		{
			static float cont_quickcmd_height = 0.0f;
			cont_quickcmd_height = ImGui::Widget_ContainerWithCollapsingTitle("Quick Commands", cont_quickcmd_height, cont_compsettings_quick_cmd,
				true, ICON_FA_TERMINAL, &ImGuiCol_ContainerBackground, &ImGuiCol_ContainerBorder);
		}

		// culling related
		{
			static float cont_cs_culling_height = 0.0f;
			cont_cs_culling_height = ImGui::Widget_ContainerWithCollapsingTitle("Culling Settings", cont_cs_culling_height,
				compsettings_culling_container, false, ICON_FA_TV, &im->ImGuiCol_ContainerBackground, &im->ImGuiCol_ContainerBorder);
		}

		// light related
		{
			static float cont_cs_light_height = 0.0f;
			cont_cs_light_height = ImGui::Widget_ContainerWithCollapsingTitle("Light Settings", cont_cs_light_height,
				compsettings_light_container, false, ICON_FA_SUN, &im->ImGuiCol_ContainerBackground, &im->ImGuiCol_ContainerBorder);
		}

		// rendering related
		{
			static float cont_cs_renderer_height = 0.0f;
			cont_cs_renderer_height = ImGui::Widget_ContainerWithCollapsingTitle("Rendering Settings", cont_cs_renderer_height,
				compsettings_rendering_container, false, ICON_FA_CAMERA, &im->ImGuiCol_ContainerBackground, &im->ImGuiCol_ContainerBorder);
		}

		// rain particle related
		{
			static float cont_cs_rain_height = 0.0f;
			cont_cs_rain_height = ImGui::Widget_ContainerWithCollapsingTitle("Rain Settings", cont_cs_rain_height,
				compsettings_rain_container, false, ICON_FA_CLOUD_RAIN, &im->ImGuiCol_ContainerBackground, &im->ImGuiCol_ContainerBorder);
		}

		// material related
		{
			static float cont_cs_material_height = 0.0f;
			cont_cs_material_height = ImGui::Widget_ContainerWithCollapsingTitle("Material Settings", cont_cs_material_height,
				compsettings_material_container, false, ICON_FA_MAP, &im->ImGuiCol_ContainerBackground, &im->ImGuiCol_ContainerBorder);
		}

		// other
		{
			static float cont_cs_other_height = 0.0f;
			cont_cs_other_height = ImGui::Widget_ContainerWithCollapsingTitle("Other Settings", cont_cs_other_height,
				compsettings_other_container, false, ICON_FA_RANDOM, &im->ImGuiCol_ContainerBackground, &im->ImGuiCol_ContainerBorder);
		}
	}

	// -------------------

	bool reload_mapsettings_popup()
	{
		static bool popup_rendered_this_frame = false;
		static int last_frame_count = -1;

		// Reset flag if we're in a new frame
		int current_frame = ImGui::GetFrameCount();
		if (current_frame != last_frame_count) 
		{
			popup_rendered_this_frame = false;
			last_frame_count = current_frame;
		}

		// Only render the popup once per frame
		if (popup_rendered_this_frame) {
			return false;
		}

		bool result = false;
		if (ImGui::BeginPopupModal("Reload MapSettings?", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
		{
			popup_rendered_this_frame = true;
			const auto half_width = ImGui::GetContentRegionMax().x * 0.5f;
			auto line1_str = "You'll loose all unsaved changes if you continue!";
			auto line2_str = "Use the copy to clipboard buttons and manually update  ";
			auto line3_str = "the map_settings.toml file if you've made changes.";

			ImGui::Spacing();
			ImGui::SetCursorPosX(5.0f + half_width - (ImGui::CalcTextSize(line1_str).x * 0.5f));
			ImGui::TextUnformatted(line1_str);

			ImGui::Spacing();
			ImGui::SetCursorPosX(5.0f + half_width - (ImGui::CalcTextSize(line2_str).x * 0.5f));
			ImGui::TextUnformatted(line2_str);
			ImGui::SetCursorPosX(5.0f + half_width - (ImGui::CalcTextSize(line3_str).x * 0.5f));
			ImGui::TextUnformatted(line3_str);

			ImGui::Spacing(0, 8);
			ImGui::Spacing(0, 0); ImGui::SameLine();

			ImVec2 button_size(half_width - 6.0f - ImGui::GetStyle().WindowPadding.x, 0.0f);
			if (ImGui::Button("Reload", button_size))
			{
				result = true;
				map_settings::load_settings();
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine(0, 6);
			if (ImGui::Button("Cancel", button_size)) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		return result;
	}

	bool reload_mapsettings_button_with_popup(const char* ID)
	{
		ImGui::PushFont(shared::common::font::BOLD);
		if (ImGui::Button(shared::utils::va("Reload MapSettings  %s##%s", ICON_FA_REDO, ID), ImVec2(ImGui::GetContentRegionAvail().x, 0)))
		{
			if (!ImGui::IsPopupOpen("Reload MapSettings?")) {
				ImGui::OpenPopup("Reload MapSettings?");
			}
		}
		ImGui::PopFont();

		return reload_mapsettings_popup();
	}

	void cont_mapsettings_anticull_meshes()
	{
		const auto& im = imgui::get();
		auto& ac = map_settings::get_map_settings().anticull_meshes;

		SPACEY4;
		ImGui::PushFont(shared::common::font::BOLD);
		if (ImGui::Button("Copy to Clipboard   " ICON_FA_SAVE, ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0)))
		{
			ImGui::LogToClipboard();
			ImGui::LogText("%s", shared::common::toml_ext::build_anticull_array(ac).c_str());
			ImGui::LogFinish();
		} ImGui::PopFont();

		ImGui::SameLine();
		reload_mapsettings_button_with_popup("AnticullMeshes");

		SPACEY12;
		ImGui::Separator();
		SPACEY4;

		ImGui::Widget_CategoryWithVerticalLabel("3D Info", [&]()
			{
				ImGui::PushID("acvis");
				ImGui::Checkbox("Visualize Anti Culling Info", &im->m_dbg_visualize_model_info); TT("Visualize Anti Culling Info");
				SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::DragFloat("Info Min Radius", &im->m_dbg_visualize_model_info_distance, 0.05f); TT("A mesh needs to have at least this radius to be visualized.");
				SET_CHILD_WIDGET_WIDTH_MAN(200.0f); ImGui::InputText("3D Model Name Filter", &im->m_dbg_visualize_model_info_name_filter); TT("Filter by string");
				ImGui::PopID();
			});

		/*ImGui::Checkbox("Visualize Anti Culling Info", &im->m_dbg_visualize_model_info); TT("Visualize Anti Culling Info");
		//ImGui::DragFloat("Info Distance", &im->m_dbg_visualize_anti_cull_info_distance, 0.05f);  TT("Only draw mesh vis. up until this distance.");
		ImGui::DragFloat("Info Min Radius", &im->m_dbg_visualize_model_info_distance, 0.05f); TT("A mesh needs to have at least this radius to be visualized.");
		ImGui::InputText("3D Model Name Filter", &im->m_dbg_visualize_model_info_name_filter); TT("Filter by string");*/
		
		SPACEY4;
		ImGui::SeparatorText("  Nearby hashes ~ Use Right Click Context Menu  ");
		SPACEY12;

		static std::uint32_t selected_hash = 0u;
		static ImGuiTextFilter filter;

		std::unordered_set<std::uint32_t> added_hashes;

		if (ImGui::BeginListBox("##nearby", ImVec2(ImGui::GetContentRegionAvail().x, 140)))
		{
			for (size_t i = 0; i < im->visualized_model_infos.size(); ++i)
			{
				const auto& m = im->visualized_model_infos[i];

				std::uint32_t hash = m.hash_a;

				if (!hash || added_hashes.contains(hash)) {
					continue;
				}

				added_hashes.insert(hash);

				char hash_str[17];
				std::snprintf(hash_str, sizeof(hash_str), "%llx", static_cast<unsigned long long>(hash));

				if (!filter.PassFilter(hash_str) && !filter.PassFilter(m.name.c_str())) {
					continue;
				}

				// display hash and category in two columns
				ImGui::PushID(static_cast<int>(hash));

				char popup_id[64];
				std::snprintf(popup_id, sizeof(popup_id), "##ContextMenu_%llx", static_cast<unsigned long long>(hash));

				if (ImGui::Selectable(shared::utils::va("a: %llx", static_cast<unsigned long long>(hash)), selected_hash == hash, 0, ImVec2(ImGui::GetContentRegionAvail().x * 0.2f, 0))) 
				{
					selected_hash = hash;
					im->m_dbg_visualize_model_info_name_hash = hash;
				}

				// right-click context menu
				if (ImGui::BeginPopupContextItem(popup_id))
				{
					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 4.0f));

					// Apply blur and padding similar to tooltip
					ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.124f, 0.124f, 0.124f, 0.776f));

					const auto padding = 0.0f;

					ImGui::Spacing(0, padding); // top padding
					ImGui::Spacing(padding, 0); ImGui::SameLine(); // left pad

					// Header with hash
					ImGui::PushFont(shared::common::font::FONTS::BOLD);
					ImGui::Text("Hash: 0x%llx", static_cast<unsigned long long>(hash));
					ImGui::PopFont();

					ImGui::Spacing(0, 3);
					ImGui::Separator();
					ImGui::Spacing(0, 3);

					if (!ac.empty())
					{
						ImGui::Spacing(padding, 0); ImGui::SameLine(); // left pad
						ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled));
						ImGui::Text("Add to Category:");
						ImGui::PopStyleColor();
						ImGui::Spacing(0, 2);

						for (auto& c : ac)
						{
							const bool already_in_this_cat = c.hashes.contains(hash);

							if (!c.category_name.empty())
							{
								if (ImGui::MenuItem(c.category_name.c_str(), nullptr, already_in_this_cat, !already_in_this_cat)) {
									c.hashes.insert(hash);
								}

								if (already_in_this_cat)
								{
									ImGui::PushID(c.category_name.c_str());
									if (ImGui::MenuItem("Remove Override")) {
										c.hashes.erase(c.hashes.find(hash));
									}
									ImGui::PopID();
								}
							}

						}

						ImGui::Spacing(0, 3);
						ImGui::Separator();
						ImGui::Spacing(0, 3);
					}

					// Option to create new category in this TOML file
					if (ImGui::MenuItem("Create New Category"))
					{
						// Generate unique category name
						std::string base_name = "new";
						std::string new_category_name = base_name + std::to_string(ac.size());

						// Create new category
						map_settings::anti_cull_meshes_s new_category;
						new_category.category_name = new_category_name;
						new_category.distance = 100.0f;
						new_category.hashes.insert(hash);
						ac.emplace_back(new_category);
					}

					ImGui::Spacing(0, padding); // bottom padding

					ImGui::PopStyleVar(2);
					ImGui::PopStyleColor();
					ImGui::EndPopup();
				} // end context menu

				auto bcd_hashes_str = std::format("b: 0x{:08X}, c: 0x{:08X}, d: 0x{:08X}", m.hash_b, m.hash_c, m.hash_d);

				ImGui::SameLine();
				ImGui::PushFont(shared::common::font::BOLD);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.7f, 1.0f));
				ImGui::Text("%s  //  %s", m.name.c_str(), bcd_hashes_str.c_str());
				ImGui::PopFont();
				ImGui::PopStyleColor();

				ImGui::PopID();
			}

			ImGui::EndListBox();
		}

		ImGui::BeginGroup();
		const auto screenpos_prefilter = ImGui::GetCursorScreenPos();
		filter.Draw("##Filter", ImGui::GetContentRegionAvail().x
			- ImGui::GetFrameHeight()
			- ImGui::GetStyle().FramePadding.x + 3.0f);

		if (!filter.IsActive())
		{
			ImGui::SetCursorScreenPos(ImVec2(screenpos_prefilter.x + 12.0f, screenpos_prefilter.y + 5.0f));
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.4f, 0.4f, 0.6f));
			ImGui::TextUnformatted("Filter ..");
			ImGui::PopStyleColor();
		}
		ImGui::EndGroup();

		ImGui::SameLine();
		if (ImGui::Button("X", ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()))) {
			filter.Clear();
		}

		// -----

		int cat_idx = 0;
		for (auto it = ac.begin(); it != ac.end(); )
		{
			auto& elem = *it;

			if (!cat_idx) {
				ImGui::Spacing(0, 20);
			}
			else {
				ImGui::Spacing(0, 8);
			}

			cat_idx++;
			bool pending_cat_removal = false;

			ImGui::PushID(shared::utils::va("cat_%d", cat_idx));
			const auto cat_name = shared::utils::va("Category: %s", elem.category_name.c_str());
			if (ImGui::CollapsingHeader(cat_name))
			{
				SPACEY4;

				if (elem._internal_cat_buffer.empty()) {
					elem._internal_cat_buffer = elem.category_name;
				}

				if (ImGui::InputText("Category Name", &elem._internal_cat_buffer, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					if (!elem._internal_cat_buffer.empty()) // do not allow empty name
					{
						elem.category_name = std::move(elem._internal_cat_buffer);
						elem._internal_cat_buffer.clear();
					}
				}

				{
					SET_CHILD_WIDGET_WIDTH;
					ImGui::DragFloat("Distance", &elem.distance, 0.25f);

					if (elem._internal_name_buffer.empty()) {
						elem._internal_name_buffer = elem.name;
					}

					SPACEY4;

					ImGui::TextUnformatted("CPU intensive, use HASHES - for testing only ...");
					if (ImGui::InputText("Name/Filter", &elem._internal_name_buffer, ImGuiInputTextFlags_EnterReturnsTrue))
					{
						if (!elem._internal_name_buffer.empty()) // do not allow empty name
						{
							elem.name = std::move(elem._internal_name_buffer);
							elem._internal_name_buffer.clear();
						}
					}

					ImGui::BeginDisabled(elem.name.empty());
					ImGui::Checkbox("Is Filter", &elem.is_filter); TT("If enabled, checks if mesh name contains string above instead of 1:1 matching it.");
					ImGui::EndDisabled();

					ImGui::Spacing(0, 4);

					ImGui::PushFont(shared::common::font::REGULAR_SMALL);
					ImGui::TextUnformatted("  Double Click to remove an entry .. ");
					ImGui::PopFont();


					ImGui::Widget_ContainerWithDropdownShadowSquare(120, [&elem, im]()
						{
							if (ImGui::BeginTable("##ac_table", 5, ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg, ImVec2(ImGui::GetContentRegionAvail().x, 100)))
							{
								ImGui::TableSetupColumn("##col1", ImGuiTableColumnFlags_WidthFixed, ImGui::GetContentRegionAvail().x / 5);
								ImGui::TableSetupColumn("##col2", ImGuiTableColumnFlags_WidthFixed, ImGui::GetContentRegionAvail().x / 5);
								ImGui::TableSetupColumn("##col3", ImGuiTableColumnFlags_WidthFixed, ImGui::GetContentRegionAvail().x / 5);
								ImGui::TableSetupColumn("##col4", ImGuiTableColumnFlags_WidthFixed, ImGui::GetContentRegionAvail().x / 5);
								ImGui::TableSetupColumn("##col5", ImGuiTableColumnFlags_WidthFixed, ImGui::GetContentRegionAvail().x / 5);
								int col = 0;

								for (auto set_it = elem.hashes.begin(); set_it != elem.hashes.end(); )
								{
									const auto index_str = std::format("0x{:08X}", *set_it);
									if (col % 4 == 0) {
										ImGui::TableNextRow();
									}

									ImGui::TableNextColumn();
									bool erase_this = false;

									if (ImGui::Selectable(index_str.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
									{
										if (ImGui::IsMouseDoubleClicked(0)) {
											erase_this = true;
										}
									}

									if (ImGui::IsItemHovered()) {
										im->m_dbg_visualize_model_info_name_hash = *set_it;
									}

									++col;
									if (erase_this) {
										set_it = elem.hashes.erase(set_it);
									}
									else {
										++set_it;
									}
								}
								ImGui::EndTable();
							}
						});


					auto add_mesh_index = [](map_settings::anti_cull_meshes_s& ac)
						{
							try
							{
								const int val = std::stoi(ac._internal_hash_buffer);
								ac.hashes.emplace(val);
							}
							catch (const std::invalid_argument&) {
								shared::common::log("ImGui", "AntiCull - Add Index - Invalid Argument", shared::common::LOG_TYPE::LOG_TYPE_ERROR);
							}
							catch (const std::out_of_range&) {
								shared::common::log("ImGui", "AntiCull - Add Index - Out of Range", shared::common::LOG_TYPE::LOG_TYPE_ERROR);
							}

							ac._internal_hash_buffer.clear();
						};

					ImGui::Style_ColorButtonPush(imgui::get()->ImGuiCol_ButtonRed, true);
					if (ImGui::Button("Remove Category")) {
						pending_cat_removal = true;
					}
					ImGui::Style_ColorButtonPop();

					ImGui::SameLine(0, 24);

					ImGui::BeginDisabled(elem._internal_hash_buffer.empty());
					{
						if (ImGui::Button(" + ")) {
							add_mesh_index(elem);
						}

						ImGui::EndDisabled();
					}

					ImGui::SameLine();

					SET_CHILD_WIDGET_WIDTH;
					if (ImGui::InputText("Add Index", &elem._internal_hash_buffer, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue)) {
						add_mesh_index(elem);
					} TT("Use the + Button on the left or press ENTER to add the index.");
				}
			}
			ImGui::PopID();

			if (pending_cat_removal) {
				it = ac.erase(it);
			}
			else {
				++it;
			}
		}

		SPACEY4;
	}

	void imgui::tab_map_settings()
	{
		SPACEY8;
		ImGui::SeparatorText("The following settings do NOT auto-save.");
		ImGui::TextDisabled("Export to clipboard and override the settings manually!");
		SPACEY8;

		// anticull meshes
		{
			static float cont_anticull_height = 0.0f;
			cont_anticull_height = ImGui::Widget_ContainerWithCollapsingTitle("Anti Cull Meshes", cont_anticull_height, 
				cont_mapsettings_anticull_meshes, true, ICON_FA_EYE, &ImGuiCol_ContainerBackground, &ImGuiCol_ContainerBorder);
		}
	}

	// ----------

	bool w2s(const Vector& world_pos, ImVec2& screen_coords, bool allow_offscreen = false)
	{
		auto sview = reinterpret_cast<game::eViewPlatInterface*>(0xB4AF90);
		if (sview && sview->m_pTransform)
		{
			if (shared::globals::d3d_device)
			{
				D3DVIEWPORT9 vp;
				shared::globals::d3d_device->GetViewport(&vp);

				const auto wp = world_pos.ToD3DXVector();

				D3DXVECTOR3 clip_space;
				D3DXVec3Project(&clip_space, &wp, &vp, &sview->m_pTransform->ProjectionMatrix, &sview->m_pTransform->ViewMatrix, nullptr);

				if (clip_space.z < 0.0f || clip_space.z > 1.0f) {
					return false; // behind camera or too far
				}

				screen_coords.x = clip_space.x;
				screen_coords.y = clip_space.y;

				if (!allow_offscreen)
				{
					// cull off-screen points
					if (screen_coords.x < 0 || screen_coords.x > vp.Width || screen_coords.y < 0 || screen_coords.y > vp.Height) {
						return false;
					}
				}

				return true;
			}
		}

		return false;
	}

	void imgui::draw_debug()
	{
		const auto im = imgui::get();

		//const float debug_text_line_height = 20.0f;
		//const float debug_text_line_start_y = 20.0f;
		uint32_t debug_text_curr_line = 0u;

#define DEBUG_TEXT(STR) \
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(20.0f, 20.0f + ((float)debug_text_curr_line++ * 20.0f)), ImGui::GetColorU32(ImGuiCol_Text), STR);

		if (m_freecam_mode && im->m_freecam_mode_hint && !m_screenshot_mode)
		{
			DEBUG_TEXT("FreeCam Controls:");
			DEBUG_TEXT("WASD:\t\t    Forward & Strafing");
			DEBUG_TEXT("Space/C:\t     Up & Down");
			DEBUG_TEXT("R/F:\t\t\t      Roll");
			DEBUG_TEXT("Shift:\t\t\t    Speedup");
			DEBUG_TEXT("Right Mouse:  Rotate");
			(float)debug_text_curr_line++; // empty line
		}

		if (m_dbg_show_camera_info)
		{
			std::string cam_str = std::format("Camera Pos: {:.2f}, {:.2f}, {:.2f}", game::the_camera->position.x, game::the_camera->position.y, game::the_camera->position.z);
			DEBUG_TEXT(cam_str.c_str());

			cam_str = std::format("Camera Dir: {:.2f}, {:.2f}, {:.2f}", game::the_camera->direction.x, game::the_camera->direction.y, game::the_camera->direction.z);
			DEBUG_TEXT(cam_str.c_str());
		}
		
		if (m_dbg_visualize_model_info)
		{
			ImVec2 viewport_pos = {};
			const float dist_sqr = im->m_dbg_visualize_model_info_distance * im->m_dbg_visualize_model_info_distance;

			for (auto& m : visualized_model_infos)
			{
				if (fabs(im->m_dbg_visualize_model_info_cam_pos.DistToSqr(m.pos) < dist_sqr))
				{
					if (w2s(m.pos, viewport_pos)) 
					{
						bool highlight = false;

						if (m.hash_a && m.hash_a == im->m_dbg_visualize_model_info_name_hash) {
							highlight = true;
						} else if (m.hash_b && m.hash_b == im->m_dbg_visualize_model_info_name_hash) {
							highlight = true;
						} else if (m.hash_c && m.hash_c == im->m_dbg_visualize_model_info_name_hash) {
							highlight = true;
						} else if (m.hash_d && m.hash_d == im->m_dbg_visualize_model_info_name_hash) {
							highlight = true;
						} else if ( m.name == im->m_dbg_visualize_model_info_name_filter) {
							highlight = true;
						}

						ImGui::PushStyleColor(ImGuiCol_Text, highlight ? ImVec4(0.3f, 1.0f, 0.3f, 1.0f) : ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
						ImGui::PushFont(highlight ? shared::common::font::BOLD_LARGE : shared::common::font::BOLD);
						ImGui::GetBackgroundDrawList()->AddText(viewport_pos, ImGui::GetColorU32(ImGuiCol_Text), m.name.c_str());
						ImGui::PopFont();
						ImGui::PopStyleColor();
					}
				}
			}

			visualized_model_infos.clear();
		}
	}

	// -----------

	void imgui::devgui()
	{
		ImGui::SetNextWindowSize(ImVec2(900, 800), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Remix NFSC Compatibility-Mod Settings", &shared::globals::imgui_menu_open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse))
		{
			ImGui::End();
			return;
		}

		m_im_window_focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);
		m_im_window_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);

		static bool im_demo_menu = false;
		if (im_demo_menu) {
			ImGui::ShowDemoWindow(&im_demo_menu);
		}


#define ADD_TAB(NAME, FUNC) \
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 0)));			\
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x + 12.0f, 8));	\
	if (ImGui::BeginTabItem(NAME)) {																		\
		ImGui::PopStyleVar(1);																				\
		if (ImGui::BeginChild("##child_" NAME, ImVec2(0, ImGui::GetContentRegionAvail().y - 38), ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_AlwaysVerticalScrollbar )) {	\
			FUNC(); ImGui::EndChild();																		\
		} else {																							\
			ImGui::EndChild();																				\
		} ImGui::EndTabItem();																				\
	} else { ImGui::PopStyleVar(1); } ImGui::PopStyleColor();

		// ---------------------------------------

		const auto col_top = ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 0.0f));
		const auto col_bottom = ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 0.4f));
		const auto col_border = ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 0.8f));
		const auto pre_tabbar_spos = ImGui::GetCursorScreenPos() - ImGui::GetStyle().WindowPadding;

		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(pre_tabbar_spos, pre_tabbar_spos + ImVec2(ImGui::GetWindowWidth(), 40.0f),
			col_top, col_top, col_bottom, col_bottom);

		ImGui::GetWindowDrawList()->AddLine(pre_tabbar_spos + ImVec2(0, 40.0f), pre_tabbar_spos + ImVec2(ImGui::GetWindowWidth(), 40.0f),
			col_border, 1.0f);

		ImGui::SetCursorScreenPos(pre_tabbar_spos + ImVec2(12,8));

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x + 12.0f, 8));
		ImGui::PushStyleColor(ImGuiCol_TabSelected, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		if (ImGui::BeginTabBar("devgui_tabs"))
		{
			ImGui::PopStyleColor();
			ImGui::PopStyleVar(1);
			ADD_TAB("Comp Settings", tab_compsettings);
			ADD_TAB("Map Settings", tab_map_settings);
			ADD_TAB("Dev", tab_dev);
			ADD_TAB("Utils", tab_utils);
			ADD_TAB("About", tab_about);
			ImGui::EndTabBar();
		}
		else {
			ImGui::PopStyleColor();
			ImGui::PopStyleVar(1);
		}
#undef ADD_TAB

		{
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, 16.0f));

			ImGui::Separator();
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

			if (ImGui::Button("Demo", ImVec2(50, 0))) {
				im_demo_menu = !im_demo_menu;
			}

			ImGui::SameLine();
			ImGui::Checkbox("##always_allow_input", &m_im_always_bypass_input); TT("Always pass input to the game");

			ImGui::SameLine();
			ImGui::TextUnformatted("Hold Right Mouse to enable Game Input");
			ImGui::PopStyleVar(2);
		}
		
		ImGui::End();
	}

	void imgui::on_present()
	{
		if (auto* im = imgui::get(); im)
		{
			if (const auto dev = shared::globals::d3d_device; dev)
			{
				if (!im->m_initialized_device)
				{
					//Sleep(1000);
					shared::common::log("ImGui", "ImGui_ImplDX9_Init");
					ImGui_ImplDX9_Init(dev);
					im->m_initialized_device = true;
				}

				// else so we render the first frame one frame later
				else if (im->m_initialized_device)
				{
					// handle srgb
					DWORD og_srgb_samp, og_srgb_write;
					dev->GetSamplerState(0, D3DSAMP_SRGBTEXTURE, &og_srgb_samp);
					dev->GetRenderState(D3DRS_SRGBWRITEENABLE, &og_srgb_write);
					dev->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, 1);
					dev->SetRenderState(D3DRS_SRGBWRITEENABLE, 1);

					ImGui_ImplDX9_NewFrame();
					ImGui_ImplWin32_NewFrame();
					ImGui::NewFrame();

					auto& io = ImGui::GetIO();

					if (shared::globals::imgui_allow_input_bypass_timeout) {
						shared::globals::imgui_allow_input_bypass_timeout--;
					}

					shared::globals::imgui_wants_text_input = ImGui::GetIO().WantTextInput;

					im->m_vis_imgui_open = false;

					if (shared::globals::imgui_menu_open) 
					{
						io.MouseDrawCursor = true;
						im->devgui();

						// ---
						// enable game input via right mouse button logic

						if (!im->m_im_window_hovered && io.MouseDown[1])
						{
							// reset stuck rmb if timeout is active 
							if (shared::globals::imgui_allow_input_bypass_timeout)
							{
								io.AddMouseButtonEvent(ImGuiMouseButton_Right, false);
								shared::globals::imgui_allow_input_bypass_timeout = 0u;
							}

							// enable game input if no imgui window is hovered and right mouse is held
							// and freecam not enabled
							else if (!get()->m_freecam_mode || im->m_freecam_freeze_allow_controls)
							{
								ImGui::SetWindowFocus(); // unfocus input text
								shared::globals::imgui_allow_input_bypass = true;
								*game::game_input_allowed = shared::globals::imgui_allow_input_bypass;
							}
						}

						else if (im->m_im_always_bypass_input)
						{
							shared::globals::imgui_allow_input_bypass_timeout = 0u;
							*game::game_input_allowed = 1;
						}

						// ^ wait until mouse is up
						else if (shared::globals::imgui_allow_input_bypass && !io.MouseDown[1] && !shared::globals::imgui_allow_input_bypass_timeout)
						{
							shared::globals::imgui_allow_input_bypass_timeout = 2u;
							shared::globals::imgui_allow_input_bypass = false;
							*game::game_input_allowed = 0;
						}
					}
					else 
					{
						// so we only write this once
						/*if (shared::globals::imgui_allow_input_bypass) {
							*game::game_input_allowed = 1;
						}*/
						
						io.MouseDrawCursor = false;
						shared::globals::imgui_allow_input_bypass_timeout = 0u;
						shared::globals::imgui_allow_input_bypass = false;
					}

					im->draw_debug();

					if (im->m_stats.is_tracking_enabled()) {
						im->m_stats.reset_stats();
					}

 					shared::globals::imgui_is_rendering = true;
					ImGui::EndFrame();
					ImGui::Render();
					ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
					shared::globals::imgui_is_rendering = false;

					// restore
					dev->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, og_srgb_samp);
					dev->SetRenderState(D3DRS_SRGBWRITEENABLE, og_srgb_write);
				}
			}
		}
	}

	void imgui::theme()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.Alpha = 1.0f;
		style.DisabledAlpha = 0.5f;

		style.WindowPadding = ImVec2(8.0f, 10.0f);
		style.FramePadding = ImVec2(8.0f, 6.0f);
		style.ItemSpacing = ImVec2(10.0f, 5.0f);
		style.ItemInnerSpacing = ImVec2(4.0f, 8.0f);
		style.IndentSpacing = 16.0f;
		style.ColumnsMinSpacing = 10.0f;
		style.ScrollbarSize = 14.0f;
		style.GrabMinSize = 10.0f;

		style.WindowBorderSize = 1.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupBorderSize = 1.0f;
		style.FrameBorderSize = 1.0f;
		style.TabBorderSize = 0.0f;

		style.WindowRounding = 4.0f;
		style.ChildRounding = 2.0f;
		style.FrameRounding = 2.0f;
		style.PopupRounding = 2.0f;
		style.ScrollbarRounding = 2.0f;
		style.GrabRounding = 1.0f;
		style.TabRounding = 4.0f;

		style.CellPadding = ImVec2(5.0f, 4.0f);

		auto& colors = style.Colors;
		colors[ImGuiCol_Text] = ImVec4(0.93f, 0.93f, 0.93f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.23f, 0.23f, 0.23f, 0.96f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.49f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.48f, 0.36f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.21f, 0.61f, 0.46f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.135f, 0.680f, 0.476f, 1.000f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.16f, 0.48f, 0.36f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.21f, 0.60f, 0.45f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.48f, 0.48f, 0.48f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.59f, 0.44f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.58f, 0.44f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.20f, 0.58f, 0.44f, 0.06f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.20f, 0.58f, 0.44f, 0.06f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.31f, 0.31f, 0.31f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.38f, 0.38f, 0.38f, 0.95f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.19f, 0.57f, 0.43f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
		colors[ImGuiCol_TabSelected] = ImVec4(0.16f, 0.48f, 0.36f, 1.00f);
		colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.16f, 0.48f, 0.36f, 1.00f);
		colors[ImGuiCol_TabDimmed] = ImVec4(0.07f, 0.22f, 0.16f, 1.00f);
		colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.12f, 0.33f, 0.24f, 1.00f);

		ImGuiCol_ContainerBackground = ImVec4(0.17f, 0.17f, 0.17f, 0.520f);
		ImGuiCol_ContainerBorder = ImVec4(0.370f, 0.612f, 0.520f, 0.369f);
		ImGuiCol_VerticalFadeContainerBackgroundStart = ImVec4(0.0f, 0.0f, 0.0f, 0.65f);
		ImGuiCol_VerticalFadeContainerBackgroundEnd = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	
	void init_fonts()
	{
		using namespace shared::common::font;

		auto merge_icons_with_latest_font = [](const float& font_size, const bool font_data_owned_by_atlas = false)
			{
				static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

				ImFontConfig icons_config;
				icons_config.MergeMode = true;
				icons_config.PixelSnapH = true;
				icons_config.FontDataOwnedByAtlas = font_data_owned_by_atlas;

				ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)fa_solid_900, sizeof(fa_solid_900), font_size, &icons_config, icons_ranges);
			};

		ImGuiIO& io = ImGui::GetIO();

		io.Fonts->AddFontFromMemoryCompressedTTF(opensans_bold_compressed_data, opensans_bold_compressed_size, 18.0f);
		merge_icons_with_latest_font(12.0f, false);

		io.Fonts->AddFontFromMemoryCompressedTTF(opensans_bold_compressed_data, opensans_bold_compressed_size, 17.0f);
		merge_icons_with_latest_font(12.0f, false);

		io.Fonts->AddFontFromMemoryCompressedTTF(opensans_regular_compressed_data, opensans_regular_compressed_size, 18.0f);
		io.Fonts->AddFontFromMemoryCompressedTTF(opensans_regular_compressed_data, opensans_regular_compressed_size, 16.0f);

		ImFontConfig font_cfg;
		font_cfg.FontDataOwnedByAtlas = false;

		io.FontDefault = io.Fonts->AddFontFromMemoryCompressedTTF(opensans_regular_compressed_data, opensans_regular_compressed_size, 17.0f, &font_cfg);
		merge_icons_with_latest_font(17.0f, false);
	}

	imgui::imgui()
	{
		p_this = this;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		init_fonts();

		//ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.MouseDrawCursor = true;
		//io.ConfigFlags |= ImGuiConfigFlags_IsSRGB;
		//io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

		theme();

		ImGui_ImplWin32_Init(shared::globals::main_window);
		g_game_wndproc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(shared::globals::main_window, GWLP_WNDPROC, LONG_PTR(wnd_proc_hk)));

		// ---
		m_initialized = true;
		shared::common::log("ImGui", "Module initialized.", shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
	}
}



