#pragma once

#include <ranges>
#include <comp/game/structs.hpp>

namespace comp
{
	class map_settings final : public shared::common::loader::component_module
	{
	public:
		map_settings();
		~map_settings() = default;

		static inline map_settings* p_this = nullptr;
		static map_settings* get() { return p_this; }

		static bool is_initialized()
		{
			if (const auto mod = get(); mod && mod->m_initialized) {
				return true;
			}
			return false;
		}

		struct anti_cull_meshes_s
		{
			std::string category_name;

			float distance;
			std::unordered_set<std::uint32_t> hashes;

			std::string name;
			bool is_filter = false;
			char pad[3];

			std::string _internal_cat_buffer;
			std::string _internal_name_buffer;
			std::string _internal_hash_buffer;
		};

		struct map_settings_s
		{
			std::vector<anti_cull_meshes_s> anticull_meshes;
		};

		static map_settings_s& get_map_settings() { return m_map_settings; }
		static void load_settings();
		static void clear_map_settings();

	private:
		bool m_initialized = false;
		static inline map_settings_s m_map_settings = {};
		static inline std::vector<std::string> m_args;
		static inline bool m_loaded = false;
		bool parse_toml();
	};
}
