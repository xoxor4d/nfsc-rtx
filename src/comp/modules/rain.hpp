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

		void on_draw();
		bool init_spawner_mesh();
		
	private:

		remixapi_MaterialInfoOpaqueEXT m_spawner_material_info_opaque = {};
		remixapi_MaterialInfo m_spawner_material_info = {};
		remixapi_MaterialHandle m_spawner_material_handle = nullptr;

		remixapi_HardcodedVertex m_spawner_triangles_verts[4] = {};
		uint32_t m_spawner_triangles_indices[6] = {};
		remixapi_MeshInfoSurfaceTriangles m_spawner_triangles = {};
		remixapi_MeshInfo m_spawner_mesh_info = {};
		remixapi_MeshHandle m_spawner_mesh_handle = nullptr;

		remixapi_InstanceInfoParticleSystemEXT m_spawner_particle_info = {};

		bool m_spawner_mesh_created = false;

		bool m_initialized = false;
	};
}
