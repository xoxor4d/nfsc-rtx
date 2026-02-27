#pragma once
#include "../game/structs.hpp"

namespace comp
{
	class imgui final : public shared::common::loader::component_module
	{
	public:
		imgui();

		static inline imgui* p_this = nullptr;
		static imgui* get() { return p_this; }

		static void on_present();

		void devgui();
		void theme();
		bool input_message(UINT message_type, WPARAM wparam, LPARAM lparam);

		bool m_initialized_device = false;

		ImVec4 ImGuiCol_ButtonGreen = ImVec4(0.3f, 0.4f, 0.05f, 0.7f);
		ImVec4 ImGuiCol_ButtonYellow = ImVec4(0.4f, 0.3f, 0.1f, 0.8f);
		ImVec4 ImGuiCol_ButtonRed = ImVec4(0.48f, 0.15f, 0.15f, 1.00f);
		ImVec4 ImGuiCol_ContainerBackground = ImVec4(0.220f, 0.220f, 0.220f, 0.875f);
		ImVec4 ImGuiCol_ContainerBorder = ImVec4(0.099f, 0.099f, 0.099f, 0.901f);
		ImVec4 ImGuiCol_VerticalFadeContainerBackgroundStart = ImVec4(0.0f, 0.0f, 0.0f, 0.65f);
		ImVec4 ImGuiCol_VerticalFadeContainerBackgroundEnd = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

		Vector m_debug_vector = { 0.0f, 0.0f, 0.0f };
		Vector m_debug_vector2 = { 0.0f, 0.0f, 0.0f };
		Vector m_debug_vector3 = { 0.0f, 0.0f, 0.0f };
		Vector m_debug_vector4 = { 0.0f, 0.0f, 0.0f };
		Vector m_debug_vector5 = { 0.0f, 0.0f, 0.0f };

		bool m_dbg_debug_bool01 = false;
		bool m_dbg_debug_bool02 = false;
		bool m_dbg_debug_bool03 = false;
		bool m_dbg_debug_bool04 = false;
		bool m_dbg_debug_bool05 = false;
		bool m_dbg_debug_bool06 = false;
		bool m_dbg_debug_bool07 = false;
		bool m_dbg_debug_bool08 = false;
		bool m_dbg_debug_bool09 = false;

		int m_dbg_int_01 = -1;
		int m_dbg_int_02 = -1;
		int m_dbg_int_03 = -1;
		int m_dbg_int_04 = -1;
		int m_dbg_int_05 = -1;

		// ---

		bool m_dbg_force_ff_prim_up = false;
		bool m_dbg_force_ff_indexed_prim = true;
		bool m_dbg_force_ff_indexed_prim_up = false;
		bool m_dbg_use_game_matrices = false;

		bool m_dbg_disable_prim_draw = false;
		bool m_dbg_disable_prim_up_draw = false;
		bool m_dbg_disable_indexed_prim_draw = false;
		bool m_dbg_disable_indexed_prim_up_draw = false;

		bool m_dbg_disable_world = false;
		bool m_dbg_disable_world_normalmap = false;
		bool m_dbg_disable_car = false;
		bool m_dbg_disable_car_normalmap = false;
		bool m_dbg_disable_glass = false;
		bool m_dbg_disable_sky = false;
		bool m_dbg_disable_water = false;
		bool m_dbg_disable_nofuzz = false;

		bool m_dbg_disable_remix_car_shader = false;

		bool m_dbg_anticull_mesh_dist_before_hash = false;
		bool m_dbg_anticull_mesh_first_hash_only = false;
		bool m_dbg_anticull_mesh_disable = false;

		// ---

		float m_always_rain_wetness_value = 1.0f;

		// ---

		bool m_dbg_manual_compute_vis = false;
		int m_dbg_manual_compute_vis_num = 0;

		float m_dbg_compute_vis_bounding_rad_offset = 0.0f;
		float m_dbg_compute_vis_out_distance_offset = 0.0f;

		// ---

		bool m_dbg_vehshader_color_override_enabled = false;
		Vector m_dbg_vehshader_color_override;

		bool m_dbg_vehshader_roughness_override_enabled = false;
		float m_dbg_vehshader_roughness_override = 0.0f;

		bool m_dbg_vehshader_metalness_override_enabled = false;
		float m_dbg_vehshader_metalness_override = 0.0f;

		bool m_dbg_vehshader_vinylscale_override_enabled = false;
		float m_dbg_vehshader_vinylscale_override = 0.0f;

		// -----

		bool m_vis_drawcall01 = false;
		Vector4D m_vis_cvDiffuseMin;
		Vector4D m_vis_cvDiffuseRange;
		Vector4D m_vis_cvEnvmapMin;
		Vector4D m_vis_cvEnvmapRange;
		Vector4D m_vis_cvPowers;
		Vector4D m_vis_cvClampAndScales;
		Vector4D m_vis_paint_color;
		Vector4D m_vis_paint_color_post;

		game::material_data m_vis_mat_data;
		std::string m_vis_mat_name;
		std::string m_vis_detected_mat_type;

		bool m_vis_imgui_open = false;
		std::unordered_set<std::string> m_vis_used_mat_names;
		std::unordered_set<std::string> m_vis_used_shader_techniques;

		float m_vis_out_roughness = 0.0f;
		float m_vis_out_metalness = 0.0f;

		// ---

		float m_dbg_visualize_model_info_distance = 80.0f;
		bool m_dbg_visualize_model_info = false;
		std::string m_dbg_visualize_model_info_name_filter;
		std::uint32_t m_dbg_visualize_model_info_name_hash;

		Vector m_dbg_visualize_model_info_cam_pos; // not a setting
		struct visualized_model_info_s
		{
			Vector pos;
			std::string name;
			std::uint32_t hash_a;
			std::uint32_t hash_b;
			std::uint32_t hash_c;
			std::uint32_t hash_d;
			bool _internal_highlight = false;
		};
		std::vector<visualized_model_info_s> visualized_model_infos;

		class ImGuiStats
		{
		private:
			static inline bool m_tracking_enabled = false;

		public:
			bool is_tracking_enabled() const {
				return m_tracking_enabled;
			}

			void enable_tracking(const bool state)
			{
				// reset stats once when tracking gets disabled
				if (!state && m_tracking_enabled) {
					this->reset_stats();
				}

				m_tracking_enabled = state;
			}

			class StatObj
			{
			public:
				enum class Mode
				{
					Single = 0,
					ConditionalCheck = 1,
				};

				StatObj(Mode mode) : m_mode(mode) {};

				auto& get_mode() const { return m_mode; }
				auto& get_total() const { return m_num_total; }
				auto& get_successful() const { return m_num_successful; }

				void track_single()
				{
					if (m_tracking_enabled) {
						++m_num_total;
					}
				}

				bool track_check(const bool is_success = false)
				{
					if (m_tracking_enabled)
					{
						if (!is_success) {
							++m_num_total;
						}
						else {
							++m_num_successful;
						}
					}

					return true;
				}

				void reset()
				{
					m_num_total = 0u;
					m_num_successful = 0u;
				}

				Mode m_mode = Mode::Single;

			private:
				std::uint32_t m_num_total{ 0 };
				std::uint32_t m_num_successful{ 0 };
			};

			StatObj _drawcall_prim = { StatObj::Mode::Single };
			StatObj _drawcall_prim_incl_ignored = { StatObj::Mode::Single };

			StatObj _drawcall_prim_up = { StatObj::Mode::Single };
			StatObj _drawcall_prim_up_incl_ignored = { StatObj::Mode::Single };

			StatObj _drawcall_indexed_prim{ StatObj::Mode::Single };
			StatObj _drawcall_indexed_prim_incl_ignored = { StatObj::Mode::Single };

			StatObj _drawcall_indexed_prim_up{ StatObj::Mode::Single };
			StatObj _drawcall_indexed_prim_up_incl_ignored = { StatObj::Mode::Single };

			ImGuiStats()
			{
				m_stat_list.emplace_back("DrawPrim Calls", &_drawcall_prim);
				m_stat_list.emplace_back("DrawPrim +Ignored", &_drawcall_prim_incl_ignored);

				m_stat_list.emplace_back();
				m_stat_list.emplace_back("DrawPrim UP Calls", &_drawcall_prim_up);
				m_stat_list.emplace_back("DrawPrim UP +Ignored", &_drawcall_prim_up_incl_ignored);

				m_stat_list.emplace_back();
				m_stat_list.emplace_back("DrawIndexedPrim Calls", &_drawcall_indexed_prim);
				m_stat_list.emplace_back("DrawIndexedPrim +Ignored", &_drawcall_indexed_prim_incl_ignored);

				m_stat_list.emplace_back();
				m_stat_list.emplace_back("DrawIndexedPrimUP Calls", &_drawcall_indexed_prim_up);
				m_stat_list.emplace_back("DrawIndexedPrimUP +Ignored", &_drawcall_indexed_prim_up_incl_ignored);
			}

			void draw_stats();

			void reset_stats()
			{
				for (auto& p : m_stat_list)
				{
					if (p.second) {
						p.second->reset();
					}
				}
			}

		private:
			std::vector<std::pair<const char*, StatObj*>> m_stat_list;

			void display_single_stat(const char* name, const StatObj& stat);
		};

		ImGuiStats m_stats = {};

		// -----

		static bool is_initialized()
		{
			if (const auto im = imgui::get(); im && im->m_initialized){
				return true;
			}
			return false;
		}

	private:
		void tab_about();
		void tab_dev();
		void tab_compsettings();
		void tab_map_settings();
		void draw_debug();
		
		bool m_im_window_focused = false;
		bool m_im_window_hovered = false;
		std::string m_devgui_custom_footer_content;

		bool m_initialized = false;
	};
}
