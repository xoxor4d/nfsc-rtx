#include "std_include.hpp"
#include "comp/modules/map_settings.hpp"

namespace shared::common::toml_ext
{
	/// Builds a string containing all anti culling elements
	/// @return the final string in toml format
	std::string build_anticull_array(const std::vector<comp::map_settings::anti_cull_meshes_s>& entries)
	{
		auto hash_count = 0u;
		auto category_count = 0u;

		std::string toml_str = "ANTICULL = [\n"s;
		for (auto& m : entries)
		{
			if (m.name.empty() && m.hashes.empty()) {
				continue;
			}

			category_count++;

			if (!m.category_name.empty()) {
				toml_str += "\n    # " + m.category_name + "\n";
			} else {
				toml_str += "\n    # NO-NAME" + std::to_string(category_count) + "\n";
			}

			toml_str += "    { ";

			if (!m.hashes.empty())
			{
				bool first_hash = true;
				toml_str += "hashes = [\n        ";
				for (auto& h : m.hashes)
				{
					if (!first_hash) {
						toml_str += ", ";
					}
					else {
						first_hash = false;
					}

					hash_count++;

					if (!(hash_count % 10)) {
						toml_str += "\n        ";
					}

					toml_str += std::format("0x{:X}", h);
				}

				toml_str += "\n    ]";
			}

			if (!m.hashes.empty() && !m.name.empty()) {
				toml_str += ", name = \"" + m.name + "\"";
			}

			toml_str += ", distance = " + std::to_string(m.distance);

			if (m.is_filter) {
				toml_str += ", filter = true";
			}

			toml_str += " },\n";
		}

		toml_str += "]";
		return toml_str;
	}
}
