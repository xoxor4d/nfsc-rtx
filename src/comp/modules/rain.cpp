#include "std_include.hpp"
#include "rain.hpp"

#include "imgui.hpp"
#include "shared/common/remix_api.hpp"

namespace comp
{
	bool rain::init_spawner_mesh()
	{
		if (m_spawner_mesh_created) {
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

				auto& mat_info_ext = m_spawner_material_info_opaque;
				mat_info_ext.sType = REMIXAPI_STRUCT_TYPE_MATERIAL_INFO_OPAQUE_EXT;
				mat_info_ext.useDrawCallAlphaState = 1;
				mat_info_ext.opacityConstant = 1.0f;
				mat_info_ext.roughnessConstant = 1.0f;
				mat_info_ext.metallicConstant = 0.0f;
				mat_info_ext.roughnessTexture = L"";
				mat_info_ext.metallicTexture = L"";
				mat_info_ext.heightTexture = L"";
				mat_info_ext.heightTexture = L"";

				auto& mat_info = m_spawner_material_info;
				mat_info.sType = REMIXAPI_STRUCT_TYPE_MATERIAL_INFO;
				mat_info.pNext = &mat_info_ext;
				mat_info.hash = shared::utils::string_hash64("mat_rain_spawner_mesh");
				mat_info.albedoTexture = albedo_path_w.c_str();
				mat_info.normalTexture = L"";
				mat_info.tangentTexture = L"";
				mat_info.emissiveTexture = emissive_path_w.c_str();
				mat_info.emissiveIntensity = 0.2f;
				mat_info.emissiveColorConstant = { 0.1f, 0.1f, 0.1f };

				if (const auto code = api.m_bridge.CreateMaterial(&mat_info, &m_spawner_material_handle);
					code != REMIXAPI_ERROR_CODE_SUCCESS)
				{
					shared::common::log("rain", std::format("init_spawner_mesh: CreateMaterial() failed: {}", static_cast<int>(code)), shared::common::LOG_TYPE::LOG_TYPE_ERROR);
					return false;
				}
			}
			
			// Mesh
			{
				api.create_quad(&m_spawner_triangles_verts[0], &m_spawner_triangles_indices[0], 1.0f);

				m_spawner_triangles.vertices_values = m_spawner_triangles_verts;
				m_spawner_triangles.vertices_count = ARRAYSIZE(m_spawner_triangles_verts);
				m_spawner_triangles.indices_values = m_spawner_triangles_indices;
				m_spawner_triangles.indices_count = 6;
				m_spawner_triangles.material = m_spawner_material_handle;

				m_spawner_mesh_info.sType = REMIXAPI_STRUCT_TYPE_MESH_INFO;
				m_spawner_mesh_info.pNext = nullptr;
				m_spawner_mesh_info.hash = shared::utils::string_hash64("rain_spawner_mesh");
				m_spawner_mesh_info.surfaces_values = &m_spawner_triangles;
				m_spawner_mesh_info.surfaces_count = 1;

				if (const auto code = api.m_bridge.CreateMesh(&m_spawner_mesh_info, &m_spawner_mesh_handle);
					code != REMIXAPI_ERROR_CODE_SUCCESS)
				{
					shared::common::log("rain", std::format("init_spawner_mesh: CreateMesh() failed: {}", static_cast<int>(code)), shared::common::LOG_TYPE::LOG_TYPE_ERROR);
					return false;
				}
			}

			m_spawner_mesh_created = true;
			return true;
		}

		return false;
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
						const auto im = imgui::get();
						//if (!r->no_rain)
						if (im->m_dbg_debug_bool03)
						{
							const uint32_t count = static_cast<uint32_t>((r->raindrop_count1 + r->raindrop_count2));

							remixapi_Float4D particleMinColor[] = { { 1.f, 1.f, 1.f, 1.f } };
							remixapi_Float4D particleMaxColor[] = { { 1.f, 1.f, 1.f, 1.f } };
							remixapi_Float2D particleMinSize[] = { { 1.f, 1.f } };
							remixapi_Float2D particleMaxSize[] = { { 2.f, 2.f } };
							remixapi_Float3D particleMaxVelocity[] = { { 1.f, 1.f, 1.f } };

							auto& pinfo = m_spawner_particle_info;
							pinfo.sType = REMIXAPI_STRUCT_TYPE_INSTANCE_INFO_PARTICLE_SYSTEM_EXT;
							pinfo.maxNumParticles = 1000;
							pinfo.hideEmitter = 0;
							pinfo.minColor = { particleMinColor, 1 };
							pinfo.maxColor = { particleMaxColor, 1 };
							pinfo.minSize = { particleMinSize, 1 };
							pinfo.maxSize = { particleMaxSize, 1 };
							pinfo.maxVelocity = { particleMaxVelocity, 1 };
							pinfo.minTimeToLive = 7.0f;
							pinfo.maxTimeToLive = 10.0f;
							pinfo.gravityForce = 1.0f;
							pinfo.spawnRatePerSecond = 300.0f;

							shared::utils::vector::matrix3x3 mtx;
							mtx.scale(1.0f, 1.0f, 1.0f);
							mtx.rotate_z(DEG2RAD(im->m_debug_vector2.z));
							mtx.rotate_y(DEG2RAD(im->m_debug_vector2.y));
							mtx.rotate_x(DEG2RAD(im->m_debug_vector2.x));
							mtx.transpose();

							remixapi_Transform t = {};
							t = mtx.to_remixapi_transform(p1.camera->position + im->m_debug_vector);

							remixapi_InstanceInfo instance_info =
							{
								.sType = REMIXAPI_STRUCT_TYPE_INSTANCE_INFO,
								//.pNext = &pinfo,
								.categoryFlags = 0,
								.mesh = m_spawner_mesh_handle,
								.transform = t,
								.doubleSided = 1,
							};

							api.m_bridge.DrawInstance(&instance_info);

							instance_info.pNext = &m_spawner_particle_info;

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
