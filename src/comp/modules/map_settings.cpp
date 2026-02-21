#include "std_include.hpp"
#include "map_settings.hpp"

#include "shared/common/toml_ext.hpp"

using namespace shared::common::toml_ext;

namespace comp
{
	void map_settings::load_settings()
	{
		if (m_loaded) {
			get()->clear_map_settings();
		}

		if (get()->parse_toml()) {
			m_loaded = true;
		}
	}

	bool map_settings::parse_toml()
	{
		try 
		{
			// Parse main map_settings.toml
			shared::common::log("MapSettings", "Parsing 'map_settings.toml' ...", shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
			toml::basic_value<toml::type_config> config;

			try {
				config = toml::parse(shared::globals::root_path + "\\rtx_comp\\map_settings.toml", toml::spec::v(1, 1, 0));
			} catch (const toml::file_io_error& err) {
				shared::common::log("MapSettings", std::format("{}", err.what()), shared::common::LOG_TYPE::LOG_TYPE_ERROR, true);
			}

			// ####################
			// parse 'ANTICULL' table
			if (config.contains("ANTICULL"))
			{
				// #
				auto process_anticull_entry = [](const toml::value& entry)
					{
						std::string temp_category;
						if (!entry.comments().empty())
						{
							temp_category = entry.comments().at(0);
							temp_category.erase(0, 2); // rem '# '
						}

						if (temp_category.empty()) {
							temp_category = std::to_string(rand());
						}

						float temp_distance = 0;
						if (entry.contains("distance")) {
							temp_distance = to_float(entry.at("distance"), 100);
						}

						std::unordered_set<std::uint32_t> temp_set;
						if (entry.contains("hashes"))
						{
							if (auto& idx = entry.at("hashes"); idx.is_array()) {
								temp_set = toml::get<std::unordered_set<std::uint32_t>>(idx);
							}
						}

						std::string temp_name;
						if (entry.contains("name") && entry.at("name").is_string()) 
						{
							temp_name = entry.at("name").as_string();
							temp_name = shared::utils::str_to_lower(temp_name);
						}

						bool temp_filter = false;
						if (entry.contains("filter")) {
							temp_filter = to_bool(entry.at("filter"), false);
						}

						m_map_settings.anticull_meshes.emplace_back(
							anti_cull_meshes_s {
								.category_name = std::move(temp_category),
								.distance = temp_distance,
								.hashes = std::move(temp_set),
								.name = std::move(temp_name),
								.is_filter = temp_filter
							});
					};

				if (const auto ac = config.at("ANTICULL");
					!ac.is_empty() && !ac.as_array().empty())
				{
					for (const auto& entry : ac.as_array()) {
						process_anticull_entry(entry);
					}
				}
			} // end 'ANTICULL'
		}

		catch (const toml::syntax_error& err)
		{
			shared::common::set_console_color_red(true);
			printf("%s\n", err.what());
			shared::common::set_console_color_default();
			return false;
		}

		return true;
	}

	void map_settings::clear_map_settings()
	{
		m_map_settings.anticull_meshes.clear();
		m_map_settings = {};
		m_loaded = false;
	}

	map_settings::map_settings()
	{
		p_this = this;
		load_settings();

		// -----
		m_initialized = true;
		shared::common::log("MapSettings", "Module initialized.", shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
	}
}
