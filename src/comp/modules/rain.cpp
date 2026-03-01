#include "std_include.hpp"
#include "rain.hpp"

#include "imgui.hpp"
#include "shared/common/remix_api.hpp"

namespace comp
{
	bool rain::init_spawner_mesh()
	{
		//const auto im = imgui::get();
		auto& p = m_remix_particle;

		if (m_spawner_mesh_created && !p.recreate_material) {
			return true;
		}
		
		if (shared::common::remix_api::is_initialized())
		{
			auto& api = shared::common::remix_api::get();

			// Material
			{
				std::wstring albedo_path_w = shared::utils::string_to_wstring(shared::globals::root_path + "\\rtx_comp\\textures\\raindrop.a.rtex.dds");
				if (!std::filesystem::exists(albedo_path_w)) 
				{
					shared::common::log("rain", "init_spawner_mesh: failed to find albedo texture 'rtx_comp\\textures\\raindrop.a.rtex.dds'", shared::common::LOG_TYPE::LOG_TYPE_ERROR);
					albedo_path_w = L"";
				}

				std::wstring emissive_path_w = shared::utils::string_to_wstring(shared::globals::root_path + "\\rtx_comp\\textures\\raindrop_emissive.e.rtex.dds");
				if (!std::filesystem::exists(emissive_path_w))
				{
					shared::common::log("rain", "init_spawner_mesh: failed to find emissive texture 'rtx_comp\\textures\\raindrop_emissive.e.rtex.dds'", shared::common::LOG_TYPE::LOG_TYPE_ERROR);
					emissive_path_w = L"";
				}

				/*
				 enum class BlendType : uint8_t {
				  kAlpha = 0,
				  kAlphaEmissive = 1,
				  kReverseAlphaEmissive = 2,
				  kColor = 3,
				  kColorEmissive = 4,
				  kReverseColorEmissive = 5,
				  kEmissive = 6,
				  kMultiplicative = 7,
				  kDoubleMultiplicative = 8,
				  kReverseAlpha = 9,
				  kReverseColor = 10,
				
				  kMinValue = 0, // kAlpha
				  kMaxValue = 10, // kReverseColor
				};

				enum class AlphaTestType : uint8_t {
				  kNever = 0,
				  kLess = 1,
				  kEqual = 2,
				  kLessOrEqual = 3,
				  kGreater = 4,
				  kNotEqual = 5,
				  kGreaterOrEqual = 6,
				  kAlways = 7,
				
				  kMinValue = 0, // kNever
				  kMaxValue = 7, // kAlways
				};
				
				static const uint32_t alphaTestTypeMask = 0x7u;
				
				enum class RtTextureArgSource : uint8_t {
				  None = 0,
				  Texture,
				  VertexColor0,
				  TFactor
				};

				enum class DxvkRtTextureOperation : uint8_t {
				  Disable = 0,
				  SelectArg1,
				  SelectArg2,
				  Modulate,
				  Modulate2x,
				  Modulate4x,
				  Add,
				  Force_Modulate2x,
				};
				*/

				auto& mat_info_ext = m_spawner_material_info_opaque;
				mat_info_ext.sType = REMIXAPI_STRUCT_TYPE_MATERIAL_INFO_OPAQUE_EXT;
				mat_info_ext.pNext = nullptr;
				mat_info_ext.useDrawCallAlphaState = p.use_drawcall_alpha ? 1 : 0;
				mat_info_ext.blendType_hasvalue = p.use_drawcall_alpha ? 1 : 0;
				mat_info_ext.blendType_value = p.blendtype; // kAlpha
				mat_info_ext.opacityConstant = 1.0f;
				mat_info_ext.roughnessConstant = 1.0f;
				mat_info_ext.metallicConstant = p.metallic_constant;
				mat_info_ext.roughnessTexture = L"";
				mat_info_ext.metallicTexture = L"";
				mat_info_ext.heightTexture = L"";

				auto& mat_info = m_spawner_material_info;
				mat_info.sType = REMIXAPI_STRUCT_TYPE_MATERIAL_INFO;
				mat_info.pNext = &mat_info_ext;
				mat_info.hash = shared::utils::string_hash64("mat_rain_spawner_mesh");
				mat_info.albedoTexture = albedo_path_w.c_str();
				mat_info.normalTexture = L"";
				mat_info.tangentTexture = L"";
				mat_info.emissiveTexture = p.use_emissive_texture ? emissive_path_w.c_str() : L"";
				mat_info.emissiveIntensity = p.emissive_intensity;
				mat_info.emissiveColorConstant = p.emissive_color;

				if (p.recreate_material) {
					api.m_bridge.DestroyMaterial(m_spawner_material_handle);
				}

				if (const auto code = api.m_bridge.CreateMaterial(&mat_info, &m_spawner_material_handle);
					code != REMIXAPI_ERROR_CODE_SUCCESS)
				{
					shared::common::log("rain", std::format("init_spawner_mesh: CreateMaterial() failed: {}", static_cast<int>(code)), shared::common::LOG_TYPE::LOG_TYPE_ERROR);
					return false;
				}
			}
			
			// Mesh
			{
				api.create_quad(&m_spawner_triangles_verts[0], &m_spawner_triangles_indices[0], p.spawner_scale);

				auto& tri = m_spawner_triangles;
				tri.vertices_values = m_spawner_triangles_verts;
				tri.vertices_count = ARRAYSIZE(m_spawner_triangles_verts);
				tri.indices_values = m_spawner_triangles_indices;
				tri.indices_count = 6;
				tri.material = m_spawner_material_handle;

				auto& mesh_info = m_spawner_mesh_info;
				mesh_info.sType = REMIXAPI_STRUCT_TYPE_MESH_INFO;
				mesh_info.pNext = nullptr;
				mesh_info.hash = shared::utils::string_hash64("rain_spawner_mesh");
				mesh_info.surfaces_values = &m_spawner_triangles;
				mesh_info.surfaces_count = 1;

				if (p.recreate_material) {
					api.m_bridge.DestroyMesh(m_spawner_mesh_handle);
				}

				if (const auto code = api.m_bridge.CreateMesh(&mesh_info, &m_spawner_mesh_handle);
					code != REMIXAPI_ERROR_CODE_SUCCESS)
				{
					shared::common::log("rain", std::format("init_spawner_mesh: CreateMesh() failed: {}", static_cast<int>(code)), shared::common::LOG_TYPE::LOG_TYPE_ERROR);
					return false;
				}
			}

			p.recreate_material = false;
			m_spawner_mesh_created = true;
			return true;
		}

		return false;
	}

	void rain::setup_particle_system(const game::view_base& view)
	{
		auto& p = m_remix_particle;
		auto& pinfo = m_spawner_particle_info;

		pinfo.sType = REMIXAPI_STRUCT_TYPE_INSTANCE_INFO_PARTICLE_SYSTEM_EXT;
		pinfo.maxNumParticles = p.num_particles;
		pinfo.alignParticlesToVelocity = p.align_to_velocity;
		pinfo.useSpawnTexcoords = p.use_spawn_texcoords;
		pinfo.enableCollisionDetection = p.enable_collision;
		pinfo.enableMotionTrail = p.enable_motion_trail;
		pinfo.hideEmitter = p.hide_emitter;
		pinfo.restrictVelocityX = p.restrict_velocity_x;
		pinfo.restrictVelocityY = p.restrict_velocity_y;
		pinfo.restrictVelocityZ = p.restrict_velocity_z;

		pinfo.minColor = { p.min_color, 2 };
		pinfo.maxColor = { p.max_color, 2 };
		pinfo.minSize = { p.min_size, 2 };
		pinfo.maxSize = { p.max_size, 2 };
		pinfo.maxVelocity = { p.max_velocity, 1 };

		if (p.use_cam_as_attractor) {
			pinfo.attractorPosition = view.camera->position.ToRemixFloat3D();
		} else {
			pinfo.attractorPosition = p.attractor_position;
		}

		pinfo.minTimeToLive = p.min_time;
		pinfo.maxTimeToLive = p.max_time;
		pinfo.initialVelocityFromNormal = p.initial_vel_from_normal;
		pinfo.initialVelocityConeAngleDegrees = p.initial_vel_cone_ang_deg;
		pinfo.dragCoefficient = p.drag;
		pinfo.initialRotationDeviationDegrees = p.initial_rot_deg;
		pinfo.gravityForce = p.gravity_force;
		pinfo.turbulenceFrequency = p.turbulence_freq;
		pinfo.turbulenceForce = p.turbulence_force;

		if (p.force_enable) {
			pinfo.spawnRatePerSecond = p.spawn_rate;
		} else {
			pinfo.spawnRatePerSecond = view.rain->render_count * p.spawn_rate_game_multi;
		}

		pinfo.collisionThickness = p.collision_thickness;
		pinfo.collisionRestitution = p.collision_restitution;
		pinfo.motionTrailMultiplier = p.motion_trail_multi;
		pinfo.initialVelocityFromMotion = p.initial_vel_from_motion;
		pinfo.spawnBurstDuration = p.spawn_burst_duration;
		pinfo.attractorRadius = p.attractor_radius;
		pinfo.attractorForce = p.attractor_force;
		pinfo.billboardType = p.billboard_type;
		pinfo.spriteSheetMode = p.sprite_sheet_mode;
		pinfo.collisionMode = p.collision_mode;
		pinfo.randomFlipAxis = p.random_flip_axis;
	}

	void rain::on_draw()
	{
		if (shared::common::remix_api::is_initialized() && game::views)
		{
			if (init_spawner_mesh())
			{
				auto& api = shared::common::remix_api::get();
				if (const auto p1 = game::views[1]; p1.active)
				{
					if (const auto r = p1.rain; r)
					{
						auto& p = m_remix_particle;
						const auto im = imgui::get();

						if (p.enabled || p.force_enable)
						{
							setup_particle_system(p1);

							shared::utils::vector::matrix3x3 mtx;
							mtx.scale(1.0f, 1.0f, 1.0f);

							if (p.pitch_rotate_spawner_based_on_cam)
							{
								const float pitch = std::clamp(p1.rain->local_cam_velocity.x * -1.0f * p.cam_velocity_spawner_pitch_scale, 0.0f, p.cam_velocity_spawner_pitch_max) + p.rotation_offset.z;
								mtx.rotate_z(DEG2RAD(pitch));
							}
							else {
								mtx.rotate_z(DEG2RAD(p.rotation_offset.z));
							}

							// atan2_fast
							if (p.yaw_rotate_spawner_based_on_cam)
							{
								const float yaw = shared::utils::vector::atan2_fast(p1.camera->direction.x, p1.camera->direction.y) + p.rotation_offset.y;
								mtx.rotate_y(-yaw); 
							} else {
								mtx.rotate_y(DEG2RAD(p.rotation_offset.y));
							}

							mtx.rotate_x(DEG2RAD(p.rotation_offset.x));
							mtx.transpose();

							// Fixed offset
							Vector cam_forward_pos = p1.camera->direction.Scale(p.cam_forward_offset) + p1.camera->position + p.position_offset;
							
							// Offset in cam dir based on camera velocity
							if (p.cam_velocity_forward_scale > 0.0f) {
								cam_forward_pos += p1.camera->direction.Scale(p1.rain->local_cam_velocity.x * -1.0f * p.cam_velocity_forward_scale);
							}

							if (shared::globals::imgui_menu_open)
							{
								im->m_dbg_vis_camera_pos = p1.camera->position;
								im->m_dbg_vis_camera_dir = p1.camera->direction;
								im->m_dbg_vis_camera_target = p1.camera->target;
								im->m_dbg_vis_camera_velocity = p1.rain->local_cam_velocity;
								im->m_dbg_vis_camera_final_rain_pos = cam_forward_pos;
								im->m_dbg_vis_game_raindrop_count = p1.rain->render_count;
							}

							remixapi_Transform t = {};
							t = mtx.to_remixapi_transform(cam_forward_pos);

							auto& mat_info_blend = m_spawner_material_info_blend;
							mat_info_blend.sType = REMIXAPI_STRUCT_TYPE_INSTANCE_INFO_BLEND_EXT;
							mat_info_blend.alphaBlendEnabled = p.alpha_blend ? 1 : 0;
							mat_info_blend.alphaTestEnabled = p.alpha_test ? 1 : 0;
							mat_info_blend.alphaTestCompareOp = p.alpha_test_op;
							mat_info_blend.alphaTestReferenceValue = static_cast<uint8_t>(p.alpha_test_val);
							mat_info_blend.writeMask = 0x7FFFFFFF; // VkColorComponentFlagBits: VK_COLOR_COMPONENT_FLAG_BITS_MAX_ENUM
							mat_info_blend.pNext = nullptr;

							mat_info_blend.srcColorBlendFactor = p.col_src_blend; // VK_BLEND_FACTOR_ONE 
							mat_info_blend.dstColorBlendFactor = p.col_dst_blend; // VK_BLEND_FACTOR_ZERO
							mat_info_blend.colorBlendOp = p.col_blend_op; // VK_BLEND_OP_ADD
							mat_info_blend.textureColorArg1Source = p.col_arg1; // RtTextureArgSource::Texture
							mat_info_blend.textureColorArg2Source = p.col_arg2; // RtTextureArgSource::None
							mat_info_blend.textureColorOperation = p.col_op;  // DxvkRtTextureOperation::SelectArg1

							mat_info_blend.srcAlphaBlendFactor = p.alpha_src_blend; // VK_BLEND_FACTOR_ONE 
							mat_info_blend.dstAlphaBlendFactor = p.alpha_dst_blend; // VK_BLEND_FACTOR_ZERO
							mat_info_blend.alphaBlendOp = p.alpha_blend_op; // VK_BLEND_OP_ADD 
							mat_info_blend.textureAlphaArg1Source = p.alpha_arg1; // RtTextureArgSource::Texture
							mat_info_blend.textureAlphaArg2Source = p.alpha_arg2; // RtTextureArgSource::None
							mat_info_blend.textureAlphaOperation = p.alpha_op;  // DxvkRtTextureOperation::SelectArg1

							mat_info_blend.tFactor = D3DCOLOR_COLORVALUE(p.tfactor_col.x, p.tfactor_col.y, p.tfactor_col.z, p.tfactor_col.w);
							mat_info_blend.isTextureFactorBlend = p.use_tfactor ? 1 : 0;

							remixapi_InstanceInfo instance_info =
							{
								.sType = REMIXAPI_STRUCT_TYPE_INSTANCE_INFO,
								.pNext = &m_spawner_material_info_blend,
								.categoryFlags = p.category >= 0 ? (remixapi_InstanceCategoryFlags)(1 << p.category) : 0,
								.mesh = m_spawner_mesh_handle,
								.transform = t,
								.doubleSided = 1,
							};

							api.m_bridge.DrawInstance(&instance_info);

							// --

							mat_info_blend.pNext = &m_spawner_particle_info;
							instance_info.pNext = &mat_info_blend;

							if (m_spawner_mesh_handle && m_spawner_material_handle) {
								api.m_bridge.DrawInstance(&instance_info);
							}
						}
					}
				}
			}
		}
	}

	rain::rain()
	{
		p_this = this;

		// -----
		m_initialized = true;
		shared::common::log("Rain", "Module initialized.", shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
	}
}
