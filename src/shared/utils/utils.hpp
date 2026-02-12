#pragma once
#include "vector.hpp"

namespace shared::utils
{
	const char* va(const char* fmt, ...);
	
	std::string str_to_lower(std::string input);
	std::string convert_wstring(const std::wstring& wstr);

	std::string& ltrim(std::string& s);
	std::string& rtrim(std::string& s);
	std::string& trim(std::string& s);

	int try_stoi(const std::string& str, const int& default_return_val = 0);
	float try_stof(const std::string& str, const float& default_return_val = 0.0f);

	static std::vector<std::string> split(const std::string& raw_input, const std::vector<char>& delims)
	{
		std::vector<std::string> strings;

		auto find_first_delim = [](const std::string_view& input, const std::vector<char>& delims) -> std::pair<char, std::size_t>
			{
				auto first_delim = 0;
				auto first_delim_index = static_cast<std::size_t>(-1);
				auto index = 0u;

				for (auto& delim : delims)
				{
					if ((index = input.find(delim)) != std::string::npos)
					{
						if (first_delim_index == -1 || index < first_delim_index)
						{
							first_delim = delim;
							first_delim_index = index;
						}
					}
				}

				return { (char)first_delim, first_delim_index };
			};

		std::string input = raw_input;

		while (!input.empty())
		{
			const auto split_delim = find_first_delim(input, delims);
			if (split_delim.first != 0)
			{
				strings.push_back(input.substr(0, split_delim.second));
				input = input.substr(split_delim.second + 1);
			} else {
				break;
			}
		}

		strings.push_back(input);
		return strings;
	}

	static std::vector<std::string> split(std::string& str, char delimiter) {
		return split(str, std::vector < char >({ delimiter }));
	}

	static std::vector<std::string> split(const std::string& str, char delimiter) {
		return split(str, std::vector < char >({ delimiter }));
	}

	void transpose_float3x4_to_d3dxmatrix(const float3x4& src, D3DXMATRIX& dest);
	void transpose_d3dxmatrix(const D3DXMATRIX* input, D3DXMATRIX* output, std::uint32_t count);
	void transpose_float4x4(const float* row_major, float* column_major);
	bool float_equal(float a, float b, float eps = 1.e-6f);
	
	bool open_file_homepath(const std::string& sub_dir, const std::string& file_name, std::ifstream& file);

	uint32_t data_hash32(const void* data, size_t size);
	std::uint64_t string_hash64(const std::string_view& str);
	std::uint32_t string_hash32(const std::string_view& str);

	uint32_t hash32_combine(uint32_t seed, const char* str);
	uint32_t hash32_combine(uint32_t seed, int val);
	uint32_t hash32_combine(uint32_t seed, float val);

	void lookat_vertex_decl(IDirect3DDevice9* dev);
}
