#pragma once

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

		// ---

		bool m_dbg_use_fake_camera = false;

		// View matrix parameters
		float m_dbg_camera_pos[3] = { 0.0f, 0.0f, 1.0f }; // X, Y, Z
		float m_dbg_camera_yaw = 0.0f;   // Rotation around Y (degrees)
		float m_dbg_camera_pitch = 0.0f; // Rotation around X (degrees, downward tilt)

		// Projection matrix parameters
		float m_dbg_camera_fov = 60.0f;         // Vertical FOV in degrees
		float m_dbg_camera_aspect = 1.777f;     // 16:9 aspect ratio
		float m_dbg_camera_near_plane = 1.0f;   // Near clipping plane
		float m_dbg_camera_far_plane = 1000.0f; // Far clipping plane

		// ---

		bool m_dbg_manual_compute_vis = false;
		int m_dbg_manual_compute_vis_num = 0;

		float m_dbg_compute_vis_bounding_rad_offset = 55.0f;
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

		float m_vis_out_roughness = 0.0f;
		float m_vis_out_metalness = 0.0f;

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
		
		bool m_im_window_focused = false;
		bool m_im_window_hovered = false;
		std::string m_devgui_custom_footer_content;

		bool m_initialized = false;
	};
}
