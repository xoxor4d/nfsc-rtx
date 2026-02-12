#include "std_include.hpp"
#include "shared/globals.hpp"

#define VA_BUFFER_COUNT		64
#define VA_BUFFER_SIZE		65536

namespace shared::utils
{
	const char* va(const char* fmt, ...)
	{
		static char g_vaBuffer[VA_BUFFER_COUNT][VA_BUFFER_SIZE];
		static int g_vaNextBufferIndex = 0;

		va_list ap;
		va_start(ap, fmt);
		char* dest = g_vaBuffer[g_vaNextBufferIndex];
		vsnprintf(g_vaBuffer[g_vaNextBufferIndex], VA_BUFFER_SIZE, fmt, ap);
		g_vaNextBufferIndex = (g_vaNextBufferIndex + 1) % VA_BUFFER_COUNT;
		va_end(ap);
		return dest;
	}

	std::string str_to_lower(std::string input)
	{
		std::ranges::transform(input.begin(), input.end(), input.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
		return input;
	}

	std::string convert_wstring(const std::wstring& wstr)
	{
		std::string result;
		result.reserve(wstr.size());

		for (const auto& chr : wstr) {
			result.push_back(static_cast<char>(chr));
		}

		return result;
	}

	int is_space(int c)
	{
		if (c < -1) {
			return 0;
		}

		return _isspace_l(c, nullptr);
	}

	// trim from start
	std::string& ltrim(std::string& s)
	{
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int val) {
			return !is_space(val);
		}));

		return s;
	}

	// trim from end
	std::string& rtrim(std::string& s)
	{
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int val) {
			return !is_space(val);
		}).base(), s.end());

		return s;
	}

	// trim from both ends
	std::string& trim(std::string& s) {
		return ltrim(rtrim(s));
	}

	int try_stoi(const std::string& str, const int& default_return_val)
	{
		int ret = default_return_val;

		try {
			ret = std::stoi(str);
		} catch (const std::invalid_argument) {}

		return ret;
	}

	float try_stof(const std::string& str, const float& default_return_val)
	{
		float ret = default_return_val;

		try {
			ret = std::stof(str);
		} catch (const std::invalid_argument) {}

		return ret;
	}

	// -----


	void transpose_float3x4_to_d3dxmatrix(const float3x4& src, D3DXMATRIX& dest)
	{
		dest.m[0][0] = src.m[0][0];
		dest.m[0][1] = src.m[1][0];
		dest.m[0][2] = src.m[2][0];
		dest.m[0][3] = 0.0f;

		dest.m[1][0] = src.m[0][1];
		dest.m[1][1] = src.m[1][1];
		dest.m[1][2] = src.m[2][1];
		dest.m[1][3] = 0.0f;

		dest.m[2][0] = src.m[0][2];
		dest.m[2][1] = src.m[1][2];
		dest.m[2][2] = src.m[2][2];
		dest.m[2][3] = 0.0f;

		dest.m[3][0] = src.m[0][3];
		dest.m[3][1] = src.m[1][3];
		dest.m[3][2] = src.m[2][3];
		dest.m[3][3] = 1.0f;
	}

	void transpose_d3dxmatrix(const D3DXMATRIX* input, D3DXMATRIX* output, const std::uint32_t count)
	{
		for (auto i = 0u; i < count; ++i)
		{
			D3DXMATRIX& out = output[i];

			// column-major D3DXMATRIX from row-major 3x4
			out._11 = input[i].m[0][0]; out._12 = input[i].m[1][0]; out._13 = input[i].m[2][0]; out._14 = input[i].m[3][0];
			out._21 = input[i].m[0][1]; out._22 = input[i].m[1][1]; out._23 = input[i].m[2][1]; out._24 = input[i].m[3][1];
			out._31 = input[i].m[0][2]; out._32 = input[i].m[1][2]; out._33 = input[i].m[2][2]; out._34 = input[i].m[3][2];
			out._41 = input[i].m[0][3]; out._42 = input[i].m[1][3]; out._43 = input[i].m[2][3]; out._44 = input[i].m[3][3];
		}
	}

	void transpose_float4x4(const float* row_major, float* column_major)
	{
		// transpose the matrix by swapping the rows and columns
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j) {
				column_major[j * 4 + i] = row_major[i * 4 + j];
			}
		}
	}

	bool float_equal(const float a, const float b, const float eps)
	{
		return std::fabs(a - b) < eps;
	}

	/**
	* @brief			open handle to a file within the home-path (root)
	* @param sub_dir	sub directory within home-path (root)
	* @param file_name	the file name
	* @param file		in-out file handle
	* @return			file handle state (valid or not)
	*/
	bool open_file_homepath(const std::string& sub_dir, const std::string& file_name, std::ifstream& file)
	{
		file.open(shared::globals::root_path + "\\" + sub_dir + "\\" + file_name);
		if (!file.is_open()) {
			return false;
		}

		return true;
	}

	//fnv1a
	uint32_t data_hash32(const void* data, const size_t size)
	{
		constexpr uint32_t FNV_prime = 16777619u;
		constexpr uint32_t offset_basis = 2166136261u;
		uint32_t hash = offset_basis;
		const uint8_t* bytes = static_cast<const uint8_t*>(data);

		for (size_t i = 0; i < size; ++i) 
		{
			hash ^= bytes[i];
			hash *= FNV_prime;
		}

		return hash;
	}

	//fnv1a
	std::uint64_t string_hash64(const std::string_view& str)
	{
		const std::uint64_t FNV_prime = 1099511628211u;
		const std::uint64_t offset_basis = 14695981039346656037u;
		std::uint64_t hash = offset_basis;

		for (const char c : str)
		{
			hash ^= static_cast<uint64_t>(c);
			hash *= FNV_prime;
		}

		return hash;
	}

	//fnv1a
	std::uint32_t string_hash32(const std::string_view& str)
	{
		const std::uint32_t FNV_prime = 16777619u;
		const std::uint32_t offset_basis = 2166136261u;
		std::uint32_t hash = offset_basis;

		for (const char c : str)
		{
			hash ^= static_cast<uint64_t>(c);
			hash *= FNV_prime;
		}

		return hash;
	}

	uint32_t hash32_combine(uint32_t seed, const char* str)
	{
		while (*str != '\0') 
		{
			seed ^= std::hash<char>{}(*str) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			++str;
		}
		return seed;
	}

	uint32_t hash32_combine(const uint32_t seed, const int val)
	{
		return seed ^ (std::hash<int>{}(val)+0x9e3779b9 + (seed << 6) + (seed >> 2));
	}

	uint32_t hash32_combine(const uint32_t seed, float val)
	{
		const uint32_t* ptr = reinterpret_cast<uint32_t*>(&val);
		return seed ^ (*ptr + 0x9e3779b9 + (seed << 6) + (seed >> 2));
	}


	// can be used to figure out the layout of the vertex buffer
	void lookat_vertex_decl([[maybe_unused]] IDirect3DDevice9* dev)
	{
#ifdef DEBUG
		IDirect3DVertexDeclaration9* vertex_decl = nullptr;
		dev->GetVertexDeclaration(&vertex_decl);

		enum d3ddecltype : BYTE
		{
			D3DDECLTYPE_FLOAT1 = 0,		// 1D float expanded to (value, 0., 0., 1.)
			D3DDECLTYPE_FLOAT2 = 1,		// 2D float expanded to (value, value, 0., 1.)
			D3DDECLTYPE_FLOAT3 = 2,		// 3D float expanded to (value, value, value, 1.)
			D3DDECLTYPE_FLOAT4 = 3,		// 4D float
			D3DDECLTYPE_D3DCOLOR = 4,	// 4D packed unsigned bytes mapped to 0. to 1. range

			// Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
			D3DDECLTYPE_UBYTE4 = 5,		// 4D unsigned byte
			D3DDECLTYPE_SHORT2 = 6,		// 2D signed short expanded to (value, value, 0., 1.)
			D3DDECLTYPE_SHORT4 = 7,		// 4D signed short

			// The following types are valid only with vertex shaders >= 2.0
			D3DDECLTYPE_UBYTE4N = 8,	// Each of 4 bytes is normalized by dividing to 255.0
			D3DDECLTYPE_SHORT2N = 9,	// 2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
			D3DDECLTYPE_SHORT4N = 10,	// 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
			D3DDECLTYPE_USHORT2N = 11,  // 2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
			D3DDECLTYPE_USHORT4N = 12,  // 4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
			D3DDECLTYPE_UDEC3 = 13,		// 3D unsigned 10 10 10 format expanded to (value, value, value, 1)
			D3DDECLTYPE_DEC3N = 14,		// 3D signed 10 10 10 format normalized and expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, 1)
			D3DDECLTYPE_FLOAT16_2 = 15,	// Two 16-bit floating point values, expanded to (value, value, 0, 1)
			D3DDECLTYPE_FLOAT16_4 = 16,	// Four 16-bit floating point values
			D3DDECLTYPE_UNUSED = 17,	// When the type field in a decl is unused.
		};

		enum d3ddecluse : BYTE
		{
			D3DDECLUSAGE_POSITION = 0,
			D3DDECLUSAGE_BLENDWEIGHT,   // 1
			D3DDECLUSAGE_BLENDINDICES,  // 2
			D3DDECLUSAGE_NORMAL,        // 3
			D3DDECLUSAGE_PSIZE,         // 4
			D3DDECLUSAGE_TEXCOORD,      // 5
			D3DDECLUSAGE_TANGENT,       // 6
			D3DDECLUSAGE_BINORMAL,      // 7
			D3DDECLUSAGE_TESSFACTOR,    // 8
			D3DDECLUSAGE_POSITIONT,     // 9
			D3DDECLUSAGE_COLOR,         // 10
			D3DDECLUSAGE_FOG,           // 11
			D3DDECLUSAGE_DEPTH,         // 12
			D3DDECLUSAGE_SAMPLE,        // 13
		};

		struct d3dvertelem
		{
			WORD Stream;		// Stream index
			WORD Offset;		// Offset in the stream in bytes
			d3ddecltype Type;	// Data type
			BYTE Method;		// Processing method
			d3ddecluse Usage;	// Semantics
			BYTE UsageIndex;	// Semantic index
		};

		d3dvertelem decl[MAX_FVF_DECL_SIZE]; UINT numElements = 0;
		vertex_decl->GetDeclaration((D3DVERTEXELEMENT9*)decl, &numElements);
		int break_me = 1; // breakpint and look into decl
#endif
	}
}
