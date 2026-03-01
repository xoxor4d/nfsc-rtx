#pragma once

#include <ranges>
#include <comp/game/structs.hpp>

namespace comp
{
	class rain final : public shared::common::loader::component_module
	{
	public:
		rain();
		~rain() = default;

		static inline rain* p_this = nullptr;
		static rain* get() { return p_this; }

		static bool is_initialized()
		{
			if (const auto mod = get(); mod && mod->m_initialized) {
				return true;
			}
			return false;
		}

		void setup_particle_system(const game::view_base& view);
		void on_draw();
		bool init_spawner_mesh();
		
		struct rain_particle_sys
		{
			bool enabled = true;
			bool force_enable = false;

			// following vars need a re-create
			bool recreate_material = false;
			float spawner_scale = 40.0f; // helps with fast corners
			bool use_drawcall_alpha = true;
			int blendtype = 0;
			float metallic_constant = 0.31f;
			float emissive_intensity = 0.12f;
			remixapi_Float3D emissive_color = { 0.768f, 0.768f, 0.768f };
			bool use_emissive_texture = false;

			// ----

			int category = 9;

			bool use_tfactor = false;
			Vector4D tfactor_col = { 1.0f, 1.0f, 1.0f, 1.0f };

			bool alpha_blend = true;
			bool alpha_test = false;
			int  alpha_test_op = 7; // ALWAYS
			int  alpha_test_val = 255;

			int col_src_blend = 6; // VK_BLEND_FACTOR_SRC_ALPHA
			int col_dst_blend = 7; // VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA
			int col_blend_op = 0;
			int col_arg1 = 1; // Texture
			int col_arg2 = 3; // TFactor
			int col_op = 1; // SelectArg1, 3 = Modulate, 6 = Add

			int alpha_src_blend = 6; // VK_BLEND_FACTOR_SRC_ALPHA
			int alpha_dst_blend = 7; // VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA
			int alpha_blend_op = 0;
			int alpha_arg1 = 1; // Texture
			int alpha_arg2 = 3; // TFactor
			int alpha_op = 0; // SelectArg1, 3 = Modulate, 6 = Add

			// --

			int   num_particles = 8000;
			bool  align_to_velocity = true;
			bool  use_spawn_texcoords = false;
			bool  enable_collision = true;
			bool  enable_motion_trail = true;
			bool  hide_emitter = true;
			bool  restrict_velocity_x = false;
			bool  restrict_velocity_y = false;
			bool  restrict_velocity_z = false;

			remixapi_Float4D min_color[2] = {{ 0.96f, 0.96f, 1.00f, 0.0f }, { 0.96f, 0.96f, 1.00f, 0.15f } };
			remixapi_Float4D max_color[2] = {{ 0.26f, 0.30f, 0.30f, 0.08f }, { 0.92f, 0.98f, 1.00f, 0.24f } };
			remixapi_Float2D min_size[2] = {{ 0.20f, 0.25f }, { 0.25f, 0.30f } };
			remixapi_Float2D max_size[2] = {{ 0.30f, 0.85f }, { 0.30f, 0.85f } };
			remixapi_Float3D max_velocity[1] = {{ 80.0f, 80.0f, 80.0f }};
			remixapi_Float3D attractor_position = { 0.0f, 0.0f, 0.0f };

			float min_time = 0.2f;
			float max_time = 0.5f;
			float initial_vel_from_normal = -30.0f;
			float initial_vel_cone_ang_deg = 0.0f;
			float drag = 0.0f;
			float initial_rot_deg = 5.0f;
			float gravity_force = -40.0f;
			float turbulence_freq = 0.0f;
			float turbulence_force = 0.0f;

			float spawn_rate = 500.0f; // only used when forced on
			float spawn_rate_game_multi = 12.0f; // game raindrop count * multi

			float collision_thickness = 0.5f;
			float collision_restitution = 0.1f;
			float motion_trail_multi = 0.35f;
			float initial_vel_from_motion = -0.70f;
			float spawn_burst_duration = 0.0f;
			float attractor_radius = 10.0f;
			float attractor_force = 500.0f;
			uint8_t billboard_type = 2u;
			uint8_t sprite_sheet_mode = 0u;
			uint8_t collision_mode = 2u; // Kill
			uint8_t random_flip_axis = 0u;

			Vector position_offset = { 0.0f, 0.0f, 8.0f };
			Vector rotation_offset = { -90.0f, 0.0f, 0.0f };
			float cam_forward_offset = 20.0f;
			float cam_velocity_forward_scale = 80.0f;
			float cam_velocity_spawner_pitch_scale = 200.0f;
			float cam_velocity_spawner_pitch_max = 60.0f;
			bool yaw_rotate_spawner_based_on_cam = true;
			bool pitch_rotate_spawner_based_on_cam = true;
			bool use_cam_as_attractor = true;
		};

		rain_particle_sys m_remix_particle = {};

	private:
		bool m_spawner_mesh_created = false;
		remixapi_InstanceInfoBlendEXT m_spawner_material_info_blend = {};
		remixapi_MaterialInfoOpaqueEXT m_spawner_material_info_opaque = {};
		remixapi_MaterialInfo m_spawner_material_info = {};
		remixapi_MaterialHandle m_spawner_material_handle = nullptr;

		remixapi_HardcodedVertex m_spawner_triangles_verts[4] = {};
		uint32_t m_spawner_triangles_indices[6] = {};
		remixapi_MeshInfoSurfaceTriangles m_spawner_triangles = {};
		remixapi_MeshInfo m_spawner_mesh_info = {};
		remixapi_MeshHandle m_spawner_mesh_handle = nullptr;
		remixapi_InstanceInfoParticleSystemEXT m_spawner_particle_info = {};

		bool m_initialized = false;
	};
}
