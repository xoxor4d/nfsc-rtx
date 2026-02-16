#include "std_include.hpp"
#include "imgui.hpp"

#include "comp_settings.hpp"
#include "imgui_internal.h"
#include "renderer.hpp"
#include "shared/common/imgui_helper.hpp"
#include "shared/common/font_awesome_solid_900.hpp"
#include "shared/common/font_defines.hpp"
#include "shared/common/font_opensans.hpp"

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
			if (!io.MouseDown[1]) {
				shared::globals::imgui_menu_open = !shared::globals::imgui_menu_open;
			} else {
				ImGui_ImplWin32_WndProcHandler(shared::globals::main_window, message_type, wparam, lparam);
			}
		}

		if (shared::globals::imgui_menu_open)
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

		ImGui::CenterText("RTX REMIX COMPATIBILITY BASE");
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
		CENTER_URL("GitHub Repository", "https://github.com/xoxor4d/remix-comp-base");

		SPACEY16;
		ImGui::Separator();
		SPACEY16;

		const char* credits_title_str = "Credits / Thanks to:";
		ImGui::CenterText(credits_title_str);

		ImGui::Spacing(0.0f, 8.0f);

		CENTER_URL("NVIDIA - RTX Remix", "https://github.com/NVIDIAGameWorks/rtx-remix");
		CENTER_URL("Dear Imgui", "https://github.com/ocornut/imgui");
		CENTER_URL("Minhook", "https://github.com/TsudaKageyu/minhook");
		CENTER_URL("Ultimate-ASI-Loader", "https://github.com/ThirteenAG/Ultimate-ASI-Loader");

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

		if (ImGui::CollapsingHeader("Enable / Disable Functions"))
		{
			SPACEY4;

			ImGui::Checkbox("Force FF PrimUp", &im->m_dbg_force_ff_prim_up);
			ImGui::Checkbox("Force FF IndexedPrim", &im->m_dbg_force_ff_indexed_prim);
			ImGui::Checkbox("Force FF IndexedPrim Up", &im->m_dbg_force_ff_indexed_prim_up);

			ImGui::Checkbox("Ignore Prim Drawcalls", &im->m_dbg_disable_prim_draw);
			ImGui::Checkbox("Ignore PrimUp Drawcalls", &im->m_dbg_disable_prim_up_draw);
			ImGui::Checkbox("Ignore IndexedPrim Drawcalls", &im->m_dbg_disable_indexed_prim_draw);
			ImGui::Checkbox("Ignore IndexedPrimUp Drawcalls", &im->m_dbg_disable_indexed_prim_up_draw);

			ImGui::Checkbox("Disable World Drawcalls", &im->m_dbg_disable_world);
			ImGui::Checkbox("Disable WorldNormal Drawcalls", &im->m_dbg_disable_world_normalmap);
			ImGui::Checkbox("Disable Car Drawcalls", &im->m_dbg_disable_car);
			ImGui::Checkbox("Disable CarNormal Drawcalls", &im->m_dbg_disable_car_normalmap);
			ImGui::Checkbox("Disable Glass Drawcalls", &im->m_dbg_disable_glass);
			ImGui::Checkbox("Disable Sky Drawcalls", &im->m_dbg_disable_sky);

			SPACEY8;
		}

		if (ImGui::CollapsingHeader("Camera"))
		{
			SPACEY4;

			ImGui::Checkbox("Use Game Camera Matrices", &im->m_dbg_use_game_matrices);

			ImGui::Checkbox("Use Fake Camera", &im->m_dbg_use_fake_camera);
			ImGui::BeginDisabled(!im->m_dbg_use_fake_camera);
			{
				ImGui::SliderFloat3("Camera Position (X, Y, Z)", im->m_dbg_camera_pos, -10000.0f, 10000.0f);
				ImGui::SliderFloat("Yaw (Y-axis)", &im->m_dbg_camera_yaw, -180.0f, 180.0f);
				ImGui::SliderFloat("Pitch (X-axis)", &im->m_dbg_camera_pitch, -90.0f, 90.0f);

				// Projection matrix adjustments
				ImGui::SliderFloat("FOV", &im->m_dbg_camera_fov, 1.0f, 180.0f);
				ImGui::SliderFloat("Aspect Ratio", &im->m_dbg_camera_aspect, 0.2f, 3.555f);
				ImGui::SliderFloat("Near Plane", &im->m_dbg_camera_near_plane, 0.1f, 1000.0f);
				ImGui::SliderFloat("Far Plane", &im->m_dbg_camera_far_plane, 1.0f, 100000.0f);

				ImGui::EndDisabled();
			}

			SPACEY8;
		}

		if (ImGui::CollapsingHeader("Culling"))
		{
			SPACEY4;

			if (game::preculler_mode)
			{
				bool tmp_preculler_bool = *game::preculler_mode;
				if (ImGui::Checkbox("Enable Preculling", &tmp_preculler_bool)) {
					*game::preculler_mode = tmp_preculler_bool;
				}
			}

			if (game::drawscenery_cell_dist_check_01) {
				ImGui::DragFloat("DrawScenery CmpFloat 01", game::drawscenery_cell_dist_check_01, 0.01f);
			}

			if (game::drawscenery_cell_dist_check_02) {
				ImGui::DragFloat("DrawScenery CmpFloat 02", game::drawscenery_cell_dist_check_02, 0.01f);
			}

			if (game::drawscenery_cell_dist_check_03) {
				ImGui::DragFloat("DrawScenery CmpFloat 03", game::drawscenery_cell_dist_check_03, 0.01f);
			}

			ImGui::Checkbox("Force Return Value for Compute Vis", &im->m_dbg_manual_compute_vis);
			ImGui::BeginDisabled(!im->m_dbg_manual_compute_vis);
			{
				ImGui::DragInt("Forced Return Value", &im->m_dbg_manual_compute_vis_num, 0.2f, 0, 100, "%d", ImGuiSliderFlags_AlwaysClamp);
				ImGui::EndDisabled();
			}

			ImGui::DragFloat("CompVis BoundingRad Offs", &im->m_dbg_compute_vis_bounding_rad_offset, 0.1f);
			ImGui::DragFloat("CompVis OutDist Offs", &im->m_dbg_compute_vis_out_distance_offset, 0.1f);
			
			SPACEY8;
		}

		if (ImGui::CollapsingHeader("Statistics ..."))
		{
			SPACEY4;
			im->m_stats.enable_tracking(true);
			im->m_stats.draw_stats();
			SPACEY8;
		} else {
			im->m_stats.enable_tracking(false);
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

		if (ImGui::CollapsingHeader("Vis States"))
		{
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

			im->m_vis_drawcall01 = false;

			SPACEY8;
		}
		// m_vis_cvDiffuseMin
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


	bool compsettings_bool_widget(const char* desc, comp_settings::variable& var)
	{
		const auto cs_var_ptr = var.get_as<bool*>();
		const bool result = ImGui::Checkbox(desc, cs_var_ptr);
		TT(var.get_tooltip_string().c_str());
		return result;
	}

	bool compsettings_float_widget(const char* desc, comp_settings::variable& var, const float& min = 0.0f, const float& max = 0.0f, const float& speed = 0.02f)
	{
		const auto cs_var_ptr = var.get_as<float*>();
		const bool result = ImGui::DragFloat(desc, cs_var_ptr, speed, min, max, "%.2f", (min != 0.0f || max != 0.0f) ? ImGuiSliderFlags_AlwaysClamp : ImGuiSliderFlags_None);
		TT(var.get_tooltip_string().c_str());
		return result;
	}

	void compsettings_culling_container()
	{
		const auto& cs = comp_settings::get();

		SPACEY4;
		ImGui::SeparatorText(" Anti Culling ");
		SPACEY4;

		compsettings_bool_widget("Disable Preculling", cs->nocull_disable_precull);
		SET_CHILD_WIDGET_WIDTH; compsettings_float_widget("No Culling Until Distance", cs->nocull_distance, 0.0f, FLT_MAX, 0.5f);

		SPACEY4;
	}


	void compsettings_material_container()
	{
		const auto& cs = comp_settings::get();

		SPACEY4;
		ImGui::SeparatorText(" Paint Settings ");
		SPACEY4;

#define MAT_OPTIONS(NAME, VAR) { \
			SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget(#NAME " Roughness", cs->VAR##_roughness, 0.0f, 1.0f, 0.001f); \
			SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget(#NAME " Metalness", cs->VAR##_metalness, 0.0f, 1.0f, 0.001f); \
			SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget(#NAME " View Scalar", cs->VAR##_view_scalar, 0.0f, 4.0f, 0.001f); \
			SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget(#NAME " View Primary Diffuse Scalar", cs->VAR##_view_primary_color_scalar, 0.0f, 4.0f, 0.001f); \
			SET_CHILD_WIDGET_WIDTH_MAN(200.0f); compsettings_float_widget(#NAME " View Primary Diffuse Blend", cs->VAR##_view_primary_color_blend_scalar, 0.0f, 4.0f, 0.001f); \
			SPACEY4; }

		MAT_OPTIONS("Perl", mat_perl);
		MAT_OPTIONS("Matte", mat_matte);
		MAT_OPTIONS("Metallic", mat_metallic);
		MAT_OPTIONS("HighGloss", mat_high_gloss);
		MAT_OPTIONS("Iridiance", mat_iridiance);
		MAT_OPTIONS("Candy", mat_candy);
		MAT_OPTIONS("Chrome", mat_chrome);

#undef MAT_OPTIONS

		SPACEY4;
	}


	void compsettings_other_container()
	{
		static const auto& cs = comp_settings::get();

		SPACEY4;
		ImGui::SeparatorText(" Remix ");
		SPACEY4;

		SET_CHILD_WIDGET_WIDTH; ImGui::DragInt("RTXDI Initial Sample Count Override", cs->remix_override_rtxdi_samplecount.get_as<int*>(), 0.01f);
		TT(cs->remix_override_rtxdi_samplecount.get_tooltip_string().c_str());

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
			static float cont_cs_renderer_height = 0.0f;
			cont_cs_renderer_height = ImGui::Widget_ContainerWithCollapsingTitle("Culling Settings", cont_cs_renderer_height,
				compsettings_culling_container, false, ICON_FA_TV, &im->ImGuiCol_ContainerBackground, &im->ImGuiCol_ContainerBorder);
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

	// -----------

	void imgui::devgui()
	{
		ImGui::SetNextWindowSize(ImVec2(900, 800), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Remix Compatibility-Base Settings", &shared::globals::imgui_menu_open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse))
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
			ADD_TAB("Dev", tab_dev);
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
							else
							{
								ImGui::SetWindowFocus(); // unfocus input text
								shared::globals::imgui_allow_input_bypass = true;
							}
						}

						// ^ wait until mouse is up
						else if (shared::globals::imgui_allow_input_bypass && !io.MouseDown[1] && !shared::globals::imgui_allow_input_bypass_timeout)
						{
							shared::globals::imgui_allow_input_bypass_timeout = 2u;
							shared::globals::imgui_allow_input_bypass = false;
						}
					}
					else 
					{
						io.MouseDrawCursor = false;
						shared::globals::imgui_allow_input_bypass_timeout = 0u;
						shared::globals::imgui_allow_input_bypass = false;
					}

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

		ImGuiCol_ContainerBackground = ImVec4(0.17f, 0.17f, 0.17f, 0.875f);
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



