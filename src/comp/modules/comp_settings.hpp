#pragma once

namespace comp
{
	class comp_settings final : public shared::common::loader::component_module
	{
	public:
		comp_settings();
		~comp_settings() = default;
	
		static inline comp_settings* p_this = nullptr;
		static auto get() { return &vars; }

		static bool is_initialized()
		{
			if (p_this && p_this->m_initialized) {
				return true;
			}
			return false;
		}

		static void write_toml();
		static bool parse_toml();

		union var_value
		{
			bool boolean;
			int integer;
			float value[4];
			remixapi_Float2D remix_float2d[2];
			remixapi_Float3D remix_float3d[2];
			remixapi_Float4D remix_float4d[2];

			var_value() : value{} {}
		};
	
		enum var_type : std::uint8_t
		{
			var_type_boolean = 0,
			var_type_integer = 1,
			var_type_value = 2,
			var_type_vec2 = 3,
			var_type_vec3 = 4,
			var_type_vec4 = 5,
			var_type_remix_float2d_array = 6,
			var_type_remix_float3d_array = 7,
			var_type_remix_float4d_array = 8,
		};
	
		class variable
		{
		public:
			// bool
			variable(const char* name, const char* desc, const bool boolean) :
				m_name(name), m_desc(desc), m_type(var_type_boolean)
			{
				m_var.boolean = boolean;
				m_var_default.boolean = boolean;
			}
	
			// int
			variable(const char* name, const char* desc, const int integer) :
				m_name(name), m_desc(desc), m_type(var_type_integer)
			{
				m_var.integer = integer;
				m_var_default.integer = integer;
			}
	
			// float
			variable(const char* name, const char* desc, const float value) :
				m_name(name), m_desc(desc), m_type(var_type_value)
			{
				m_var.value[0] = value;
				m_var_default.value[0] = value;
			}
	
			// vec2
			variable(const char* name, const char* desc, const float x, const float y) :
				m_name(name), m_desc(desc), m_type(var_type_vec2)
			{
				m_var.value[0] = x; m_var.value[1] = y;
				m_var_default.value[0] = x; m_var_default.value[1] = y;
			}
	
			// vec3
			variable(const char* name, const char* desc, const float x, const float y, const float z) :
				m_name(name), m_desc(desc), m_type(var_type_vec3)
			{
				m_var.value[0] = x; m_var.value[1] = y; m_var.value[2] = z;
				m_var_default.value[0] = x; m_var_default.value[1] = y; m_var_default.value[2] = z;
			}
	
			// vec4
			variable(const char* name, const char* desc, const float x, const float y, const float z, const float w) :
				m_name(name), m_desc(desc), m_type(var_type_vec4)
			{
				m_var.value[0] = x; m_var.value[1] = y; m_var.value[2] = z; m_var.value[3] = w;
				m_var_default.value[0] = x; m_var_default.value[1] = y; m_var_default.value[2] = z; m_var_default.value[3] = w;
			}

			// remix_float2_array
			variable(const char* name, const char* desc, const var_type type,
					 const float x1, const float y1,
					 const float x2, const float y2) :
				m_name(name), m_desc(desc), m_type(type)
			{
				m_var.remix_float2d[0].x = x1; m_var.remix_float2d[0].y = y1;
				m_var.remix_float2d[1].x = x2; m_var.remix_float2d[1].y = y2;

				m_var_default.remix_float2d[0].x = x1; m_var_default.remix_float2d[0].y = y1;
				m_var_default.remix_float2d[1].x = x2; m_var_default.remix_float2d[1].y = y2;
			}

			// remix_float3_array
			variable(const char* name, const char* desc, const var_type type,
					 const float x1, const float y1, const float z1,
					 const float x2, const float y2, const float z2) :
				m_name(name), m_desc(desc), m_type(type)
			{
				m_var.remix_float3d[0].x = x1; m_var.remix_float3d[0].y = y1; m_var.remix_float3d[0].z = z1;
				m_var.remix_float3d[1].x = x2; m_var.remix_float3d[1].y = y2; m_var.remix_float3d[1].z = z2;

				m_var_default.remix_float3d[0].x = x1; m_var_default.remix_float3d[0].y = y1; m_var_default.remix_float3d[0].z = z1;
				m_var_default.remix_float3d[1].x = x2; m_var_default.remix_float3d[1].y = y2; m_var_default.remix_float3d[1].z = z2;
			}

			// remix_float4_array
			variable(const char* name, const char* desc, const var_type type,
					 const float x1, const float y1, const float z1, const float w1,
					 const float x2, const float y2, const float z2, const float w2) :
				m_name(name), m_desc(desc), m_type(type)
			{
				m_var.remix_float4d[0].x = x1; m_var.remix_float4d[0].y = y1; m_var.remix_float4d[0].z = z1; m_var.remix_float4d[0].w = w1;
				m_var.remix_float4d[1].x = x2; m_var.remix_float4d[1].y = y2; m_var.remix_float4d[1].z = z2; m_var.remix_float4d[1].w = w2;

				m_var_default.remix_float4d[0].x = x1; m_var_default.remix_float4d[0].y = y1; m_var_default.remix_float4d[0].z = z1; m_var_default.remix_float4d[0].w = w1;
				m_var_default.remix_float4d[1].x = x2; m_var_default.remix_float4d[1].y = y2; m_var_default.remix_float4d[1].z = z2; m_var_default.remix_float4d[1].w = w2;
			}
	
			const char* get_str_value(bool get_default = false) const
			{
				const auto pvec = !get_default ? &m_var.value[0] : &m_var_default.value[0];

				const auto premix2d = !get_default ? &m_var.remix_float2d[0] : &m_var_default.remix_float2d[0];
				const auto premix3d = !get_default ? &m_var.remix_float3d[0] : &m_var_default.remix_float3d[0];
				const auto premix4d = !get_default ? &m_var.remix_float4d[0] : &m_var_default.remix_float4d[0];

				switch (m_type)
				{
				case var_type_boolean:
					return shared::utils::va("%s", (!get_default ? m_var.boolean : m_var_default.boolean) ? "true" : "false");
	
				case var_type_integer:
					return shared::utils::va("%d", !get_default ? m_var.integer : m_var_default.integer);
	
				case var_type_value:
					return shared::utils::va("%.2f", pvec[0]);
	
				case var_type_vec2:
					return shared::utils::va("[ %.2f, %.2f ]", pvec[0], pvec[1]);
	
				case var_type_vec3:
					return shared::utils::va("[ %.2f, %.2f, %.2f ]", pvec[0], pvec[1], pvec[2]);
	
				case var_type_vec4:
					return shared::utils::va("[ %.2f, %.2f, %.2f, %.2f ]", pvec[0], pvec[1], pvec[2], pvec[3]);

				case var_type_remix_float2d_array:
					return shared::utils::va("[ %.2f, %.2f, %.2f, %.2f ]", premix2d[0].x, premix2d[0].y, premix2d[1].x, premix2d[1].y);

				case var_type_remix_float3d_array:
					return shared::utils::va("[ %.2f, %.2f, %.2f, %.2f, %.2f, %.2f ]", premix3d[0].x, premix3d[0].y, premix3d[0].z, premix3d[1].x, premix3d[1].y, premix3d[1].z);
				
				case var_type_remix_float4d_array:
					return shared::utils::va("[ %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f ]", premix4d[0].x, premix4d[0].y, premix4d[0].z, premix4d[0].w, premix4d[1].x, premix4d[1].y, premix4d[1].z, premix4d[1].w);
				}
	
				return nullptr;
			}
	
			const char* get_str_type() const
			{
				switch (m_type)
				{
				case var_type_boolean:
					return "BOOL";
	
				case var_type_integer:
					return "INT";
	
				case var_type_value:
					return "FLOAT";
	
				case var_type_vec2:
					return "VEC2";
	
				case var_type_vec3:
					return "VEC3";
	
				case var_type_vec4:
					return "VEC4";

				case var_type_remix_float2d_array:
					return "REMIX2D[2]";

				case var_type_remix_float3d_array:
					return "REMIX3D[2]";

				case var_type_remix_float4d_array:
					return "REMIX4D[2]";
				}

				return nullptr;
			}
	
			std::string get_tooltip_string() const
			{
				std::string out;

				const auto desc_lines = shared::utils::split(std::string(this->m_desc), '\n');
				for (const auto& line : desc_lines) {
					out += "# " + line + "\n";
				}

				out += "# Type: " + std::string(this->get_str_type()) + " || Default: " + std::string(this->get_str_value(true));
				return out;
			}

			const bool& _bool(const bool default_value = false) const
			{
				assert(m_type == var_type_boolean && "Type mismatch: expected boolean");
				return !default_value ? m_var.boolean : m_var_default.boolean;
			}

			const bool* _bool_ptr(const bool default_value = false)
			{
				assert(m_type == var_type_boolean && "Type mismatch: expected boolean");
				return &(!default_value ? m_var.boolean : m_var_default.boolean);
			}

			const float& _float(const bool default_value = false) const
			{
				assert(m_type == var_type_value && "Type mismatch: expected float");
				return !default_value ? m_var.value[0] : m_var_default.value[0];
			}

			const float* _float_ptr(const bool default_value = false)
			{
				assert(m_type == var_type_value && "Type mismatch: expected float");
				return !default_value ? m_var.value : m_var_default.value;
			}

			const float& _vec_x(const bool default_value = false) const
			{
				assert(((m_type == var_type_vec2) || (m_type == var_type_vec3) || (m_type == var_type_vec4)) && "Type mismatch: expected vec2, vec3 or vec4");
				return !default_value ? m_var.value[0] : m_var_default.value[0];
			}

			const float& _vec_y(const bool default_value = false) const
			{
				assert(((m_type == var_type_vec2) || (m_type == var_type_vec3) || (m_type == var_type_vec4)) && "Type mismatch: expected vec2, vec3 or vec4");
				return !default_value ? m_var.value[1] : m_var_default.value[1];
			}

			const float& _vec_z(const bool default_value = false) const
			{
				assert(((m_type == var_type_vec3) || (m_type == var_type_vec4)) && "Type mismatch: expected vec2, vec3 or vec4");
				return !default_value ? m_var.value[2] : m_var_default.value[2];
			}

			const float& _vec_w(const bool default_value = false) const
			{
				assert(m_type == var_type_vec4 && "Type mismatch: expected vec2, vec3 or vec4");
				return !default_value ? m_var.value[3] : m_var_default.value[3];
			}

			remixapi_Float2D* _remix2d_ptr(const int& index = 0)
			{
				assert(m_type == var_type_remix_float2d_array && "Type mismatch: expected remixFloat2d");
				return &m_var.remix_float2d[index];
			}

			const remixapi_Float2D* _remix2d_ptr(const int& index = 0) const
			{
				assert(m_type == var_type_remix_float2d_array && "Type mismatch: expected remixFloat2d");
				return &m_var_default.remix_float2d[index];
			}

			remixapi_Float3D* _remix3d_ptr(const int& index = 0)
			{
				assert(m_type == var_type_remix_float3d_array && "Type mismatch: expected remixFloat3d");
				return &m_var.remix_float3d[index];
			}

			const remixapi_Float3D* _remix3d_ptr(const int& index = 0) const
			{
				assert(m_type == var_type_remix_float3d_array && "Type mismatch: expected remixFloat3d");
				return &m_var_default.remix_float3d[index];
			}

			remixapi_Float4D* _remix4d_ptr(const int& index = 0)
			{
				assert(m_type == var_type_remix_float4d_array && "Type mismatch: expected remixFloat4d");
				return &m_var.remix_float4d[index];
			}

			const remixapi_Float4D* _remix4d_ptr(const int& index = 0) const
			{
				assert(m_type == var_type_remix_float4d_array && "Type mismatch: expected remixFloat4d");
				return &m_var_default.remix_float4d[index];
			}
	
			template <typename T>
			T get_as(bool default_val = false)
			{
				// if T is a pointer type, return a ptr
				if constexpr (std::is_pointer_v<T>)
				{
					// get the underlying type (e.g., int from int*)
					using base_type = std::remove_pointer_t<T>;
	
					if constexpr (std::is_same_v<base_type, bool>) {
						assert(m_type == var_type_boolean && "Type mismatch: expected boolean");
						return &(!default_val ? m_var.boolean : m_var_default.boolean);
					}
					else if constexpr (std::is_same_v<base_type, int>) {
						assert(m_type == var_type_integer && "Type mismatch: expected integer");
						return &(!default_val ? m_var.integer : m_var_default.integer);
					}
					else if constexpr (std::is_same_v<base_type, float>) {
						if (m_type == var_type_value) {
							return &(!default_val ? m_var.value[0] : m_var_default.value[0]);
						}
						if (m_type >= var_type_vec2 && m_type <= var_type_vec4) {
							return !default_val ? m_var.value : m_var_default.value;
						}
						assert(false && "Type mismatch: expected float or vector type");
						return nullptr;
					}
					else if constexpr (std::is_same_v<base_type, Vector2D>) {
						assert(m_type == var_type_vec2 && "Type mismatch: expected vec2 for Vector");
						return reinterpret_cast<Vector2D*>(!default_val ? m_var.value : m_var_default.value);
					}
					else if constexpr (std::is_same_v<base_type, Vector>) {
						assert(m_type == var_type_vec3 && "Type mismatch: expected vec3 for Vector");
						return reinterpret_cast<Vector*>(!default_val ? m_var.value : m_var_default.value);
					}
					else if constexpr (std::is_same_v<base_type, Vector4D>) {
						assert(m_type == var_type_vec4 && "Type mismatch: expected vec4 for Vector");
						return reinterpret_cast<Vector4D*>(!default_val ? m_var.value : m_var_default.value);
					}
					else {
						static_assert(std::is_same_v<T, void>, "Unsupported pointer type in get_as");
						return nullptr;
					}
				}
				// return by value for non-pointer types
				else
				{
					if constexpr (std::is_same_v<T, bool>) {
						assert(m_type == var_type_boolean && "Type mismatch: expected boolean");
						return static_cast<T>(!default_val ? m_var.boolean : m_var_default.boolean);
					}
					else if constexpr (std::is_same_v<T, int>) {
						assert(m_type == var_type_integer && "Type mismatch: expected integer");
						return static_cast<T>(!default_val ? m_var.integer : m_var_default.integer);
					}
					else if constexpr (std::is_same_v<T, float>) {
						assert(m_type == var_type_value && "Type mismatch: expected float");
						return static_cast<T>(!default_val ? m_var.value[0] : m_var_default.value[0]);
					}
					else if constexpr (std::is_same_v<T, Vector2D>) {
						assert(m_type == var_type_vec2 && "Type mismatch: expected vec2 for Vector");
						return Vector2D(!default_val ? m_var.value : m_var_default.value);
					}
					else if constexpr (std::is_same_v<T, Vector>) {
						assert(m_type == var_type_vec3 && "Type mismatch: expected vec3 for Vector");
						return Vector(!default_val ? m_var.value : m_var_default.value);
					}
					else if constexpr (std::is_same_v<T, Vector4D>) {
						assert(m_type == var_type_vec4 && "Type mismatch: expected vec4 for Vector");
						return Vector4D(!default_val ? m_var.value : m_var_default.value);
					}
					else {
						static_assert(std::is_same_v<T, void>, "Unsupported return type in get_as");
						return T{};
					}
				}
			}

			var_type get_type() const {
				return m_type;
			}

			// sets var and writes toml (bool)
			void set_var(const bool boolean, bool no_toml_update = false)
			{
				m_var.boolean = boolean;
				if (!no_toml_update) {
					write_toml();
				}
			}
	
			// sets var and writes toml (integer)
			void set_var(const int integer, bool no_toml_update = false)
			{
				m_var.integer = integer;
				if (!no_toml_update) {
					write_toml();
				}
			}
	
			// sets var and writes toml (float)
			void set_var(const float value, bool no_toml_update = false)
			{
				m_var.value[0] = value;
				if (!no_toml_update) {
					write_toml();
				}
			}
	
			// sets var and writes toml (vec4)
			void set_vec(const float* v, bool no_toml_update = false)
			{
				switch (m_type)
				{
				default:
					break;
	
				case var_type_value:
					m_var.value[0] = v[0];
					break;
	
				case var_type_vec2:
					m_var.value[0] = v[0]; m_var.value[1] = v[1];
					break;
	
				case var_type_vec3:
					m_var.value[0] = v[0]; m_var.value[1] = v[1]; m_var.value[2] = v[2];
					break;
	
				case var_type_vec4:
					m_var.value[0] = v[0]; m_var.value[1] = v[1]; m_var.value[2] = v[2]; m_var.value[3] = v[3];
					break;
				}
	
				if (!no_toml_update) {
					write_toml();
				}
			}

			// sets var and writes toml (remix vec)
			void set_remix_vec(const float* v, bool no_toml_update = false)
			{
				uint8_t ct = 0u;
				switch (m_type)
				{
				default:
					break;

				case var_type_remix_float2d_array:
					m_var.remix_float2d[0].x = v[ct++]; m_var.remix_float2d[0].y = v[ct++];
					m_var.remix_float2d[1].x = v[ct++]; m_var.remix_float2d[1].y = v[ct];
					break;

				case var_type_remix_float3d_array:
					m_var.remix_float3d[0].x = v[ct++]; m_var.remix_float3d[0].y = v[ct++]; m_var.remix_float3d[0].z = v[ct++];
					m_var.remix_float3d[1].x = v[ct++]; m_var.remix_float3d[1].y = v[ct++]; m_var.remix_float3d[1].z = v[ct];
					break;

				case var_type_remix_float4d_array:
					m_var.remix_float4d[0].x = v[ct++]; m_var.remix_float4d[0].y = v[ct++]; m_var.remix_float4d[0].z = v[ct++]; m_var.remix_float4d[0].w = v[ct++];
					m_var.remix_float4d[1].x = v[ct++]; m_var.remix_float4d[1].y = v[ct++]; m_var.remix_float4d[1].z = v[ct++]; m_var.remix_float4d[1].w = v[ct];
					break;
				}

				if (!no_toml_update) {
					write_toml();
				}
			}

			void reset() {
				m_var = m_var_default;
			}

			const char* m_name;
			const char* m_desc;
	
		private:
			var_value m_var;
			var_value m_var_default;
			var_type m_type;
		};

		private:
			bool m_initialized = false;
	
		struct var_definitions
		{
			// ----------------------------------
			// remix related settings

			variable remix_override_rtxdi_samplecount = {
				"remix_override_rtxdi_samplecount",
				("Remix sets 'rtx.di.initialSampleCount' to hardcoded values on start.\n"
				 "Setting this value to anything greater 0 constantly sets the remix variable with this value."),
				60
			};


			// ----------------------------------
			// culling related settings

			variable nocull_distance = {
				"nocull_distance",
				("Distance (radius around player) where culling of objects is disabled"),
				150.0f
			};


			// ----------------------------------
			// rendering related settings

			variable vertex_colors_global = {
				"vertex_colors_global",
				("Enabled vertex colors (baked lighting) globally"),
				false
			};

			variable vertex_colors_particles = {
				"vertex_colors_particles",
				("Enabled vertex colors (baked lighting) on particles"),
				true
			};

			variable vertex_colors_world = {
				"vertex_colors_world",
				("Enabled vertex colors (baked lighting) on world objects"),
				false
			};

			// ---

			variable flare_enabled =  {
				"flare_enabled", 
				("Render flares"), 
				true
			};

			variable flare_alpha_multiplier = {
				"flare_alpha_multiplier",
				("Alpha multiplier for flares"),
				0.3f
			};

			// ---

			variable wetness_world = {
				"wetness_world",
				("Enable World Wetness"),
				true
			};

			variable wetness_world_variation = {
				"wetness_world_variation",
				("Enable World Wetness Variation"),
				true
			};

			variable wetness_world_puddles = {
				"wetness_world_puddles",
				("Enable World Wetness Puddles"),
				true
			};

			variable wetness_world_occlusion_check = {
				"wetness_world_occlusion_check",
				("Enable World Wetness Occlusion Checks (dry ground under cover)"),
				true
			};

			variable wetness_world_occlusion_smoothing = {
				"wetness_world_occlusion_smoothing",
				 ("Enable World Wetness Occlusion Smoothing (smooth wet <-> dry transition)\n"
				  "Does NOT look good without DLSS!"),
				true
			};

			variable wetness_world_raindrops = {
				"wetness_world_raindrops",
				("Enable World Wetness Raindrops"),
				false
			};

			variable wetness_world_raindrop_scale = {
				"wetness_world_raindrop_scale", ("World Wetness Raindrop Scale"),
				0.20f
			};

			// ---

			variable wetness_car_raindrops = {
				"wetness_car_raindrops",
				("Enable Raindrops on Cars"),
				true
			};

			variable enable_camera_raindrops = {
				"enable_camera_raindrops",
				("Enable Raindrops on the Camera Lense. Does not look good until we have cutout translucents."),
				false
			};


			// ----------------------------------
			// rain related settings

			variable rain_volumetric_fog_influence_enable = {
				"rain_volumetric_fog_influence_enable", ("Enable Game Rain-Intensity influence on 'rtx.volumetrics.transmittanceMeasurementDistanceMeters'"),
				true
			};

			variable rain_volumetric_fog_influence_low_transmission_val = {
				"rain_volumetric_fog_influence_low_transmission_val", ("Value of 'rtx.volumetrics.transmittanceMeasurementDistanceMeters' when Game Rain-Intensity == 1"),
				8.0f
			};

			variable rain_volumetric_fog_influence_high_transmission_val = {
				"rain_volumetric_fog_influence_high_transmission_val", ("Value of 'rtx.volumetrics.transmittanceMeasurementDistanceMeters' when Game Rain-Intensity == 0"),
				24.0f
			};

			// -----

			variable rain_enable = {
				"rain_enable", ("If game can trigger Remix Rain Particle Effect"),
				true
			};

			variable rain_spawner_scale = {
				"rain_spawner_scale", ("Size of rain particle spawner. Particle System needs to be Reset for this to apply."),
				35.0f
			};

			variable rain_metallic_constant = {
				"rain_metallic_constant", ("Can be used to darken the rain. Particle System needs to be Reset for this to apply."),
				0.4f
			};

			variable rain_roughness_constant = {
				"rain_roughness_constant", ("Might be useful. Particle System needs to be Reset for this to apply."),
				1.0f
			};

			variable rain_emissive_intensity = {
				"rain_emissive_intensity", ("Emissive intensity of rain. Particle System needs to be Reset for this to apply."),
				0.01f
			};

			variable rain_emissive_color = {
				"rain_emissive_color", ("Emissive color of rain. Particle System needs to be Reset for this to apply."),
				0.768f, 0.768f, 0.768f
			};

			variable rain_use_emissive_texture = {
				"rain_use_emissive_texture", ("Use 'raindrop_emissive.e.rtex.dds' from rtx_comp/textures as emissive map. Particle System needs to be Reset for this to apply."),
				false
			};

			variable rain_enable_motion_trail = {
				"rain_enable_motion_trail", ("Enable motion trail on rain particles"),
				true
			};

			variable rain_motion_trail_multi = {
				"rain_motion_trail_multi", ("Motion trail multiplier"),
				1.1f
			};


			variable rain_min_color_keyframes = {
				"rain_min_color_keyframes",
				("Min Particle color on spawn (air: keyframe 1) to on death (ground: keyframe 2). Random value between 'rain_min_color_keyframes' and 'rain_max_color_keyframes')"), 
				var_type_remix_float4d_array,
				0.96f, 0.96f, 1.00f, 0.00f, 
				0.96f, 0.96f, 1.00f, 0.40f
			};

			variable rain_max_color_keyframes = {
				"rain_max_color_keyframes",
				("Max Particle color on spawn (air: keyframe 1) to on death (ground: keyframe 2). Random value between 'rain_min_color_keyframes' and 'rain_max_color_keyframes')"),
				var_type_remix_float4d_array,
				0.36f, 0.36f, 0.36f, 0.27f,
				0.61f, 0.61f, 0.61f, 0.43f
			};

			variable rain_min_size_keyframes = {
				"rain_min_size_keyframes",
				("Min Particle size on spawn (air: keyframe 1) to on death (ground: keyframe 2). Random value between 'rain_min_size_keyframes' and 'rain_max_size_keyframes')"),
				var_type_remix_float2d_array,
				0.10f, 0.25f,
				0.10f, 0.25f
			};

			variable rain_max_size_keyframes = {
				"rain_max_size_keyframes",
				("Max Particle size on spawn (air: keyframe 1) to on death (ground: keyframe 2). Random value between 'rain_min_size_keyframes' and 'rain_max_size_keyframes')"),
				var_type_remix_float2d_array,
				0.20f, 0.35f,
				0.20f, 0.35f
			};

			variable rain_max_velocity = {
				"rain_max_velocity", ("Max Particle velocity over the lifetime of the particle. Only 1 keyframe."),
				var_type_remix_float3d_array,
				120.0f, 120.0f, 80.0f,
				0.0f,   0.0f,   0.0f
			};

			variable rain_min_lifetime = {
				"rain_min_lifetime", ("Minimum particle lifetime"),
				0.3f
			};

			variable rain_max_lifetime = {
				"rain_max_lifetime", ("Maximum particle lifetime"),
				0.8f
			};

			variable rain_initial_velocity_from_normal = {
				"rain_initial_velocity_from_normal", ("Spawn particle along the spawner normal with this velocity"),
				-90.0f
			};

			variable rain_initial_velocity_cone_angle_degrees = {
				"rain_initial_velocity_cone_angle_degrees", ("Size of cone angle applied at spawn"),
				2.0f
			};

			variable rain_initial_rotation_degrees = {
				"rain_initial_rotation_degrees", ("Size of random angle applied at spawn"),
				5.0f
			};

			variable rain_gravity_force = {
				"rain_gravity_force", ("Gravity amount applied to particles"),
				-40.0f
			};

			variable rain_spawn_rate_game_multi = {
				"rain_spawn_rate_game_multi", ("Game Raindrop Count * Multi = Final Spawn Rate. Only used when rain is not forced on"),
				16.0f
			};

			variable rain_spawn_rate_game_multi_speed_scalar = {
				"rain_spawn_rate_game_multi_speed_scalar", ("Multi * CameraVelocity * This. Additional scale based on camera velocity"),
				20.0f
			};

			variable rain_spawn_rate_game_multi_lower_limit = {
				"rain_spawn_rate_game_multi_lower_limit", ("Lower Clamp of final rain multiplier"),
				16.0f
			};

			variable rain_spawn_rate_game_multi_upper_limit = {
				"rain_spawn_rate_game_multi_upper_limit", ("Upper Clamp of final rain multiplier"),
				33.0f
			};

			variable rain_initial_velocity_from_motion = {
				"rain_initial_velocity_from_motion", ("Velocity applied on spawn based on current camera motion"),
				-0.1f
			};

			variable rain_attractor_radius = {
				"rain_attractor_radius", ("Rain is attracted towards the player camera. This sets the radius of the attractor"),
				25.0f
			};

			variable rain_attractor_force = {
				"rain_attractor_force", ("Rain is attracted towards the player camera. This sets the force of the attractor"),
				200.0f
			};

			variable rain_position_offset = {
				"rain_position_offset", ("Offset position of the particle spawner"),
				0.0f, 0.0f, 8.0f
			};

			variable rain_rotation_offset = {
				"rain_rotation_offset", ("Offset rotation of the particle spawner. Pitch (Z) and Yaw (Y) are constantly modified based on camera speed and direction."),
				-90.0f, 0.0f, 4.0f
			};

			variable rain_cam_forward_offset = {
				"rain_cam_forward_offset", ("Static position offset of particle spawner along camera direction"),
				20.0f
			};

			variable rain_cam_velocity_forward_scale = {
				"rain_cam_velocity_forward_scale", ("Dynamic position offset of particle spawner along camera direction. Based on camera velocity"),
				120.0f
			};

			variable rain_cam_velocity_spawner_pitch_scale = {
				"rain_cam_velocity_spawner_pitch_scale", ("Dynamic pitch of particle spawner so that the lower end of rain particles starts facing the camera on higher speeds. Based on camera velocity"),
				200.0f
			};

			variable rain_cam_velocity_spawner_pitch_max = {
				"rain_cam_velocity_spawner_pitch_max", ("Upper Clamp / Maximum pitch angle the spawner can reach due to camera velocity"),
				60.0f
			};

			variable rain_pitch_rotate_spawner_based_on_cam = {
				"rain_pitch_rotate_spawner_based_on_cam", ("This enables dynamic pitch changes of the particle spawner based on camera velocity"),
				true
			};

			// ----------------------------------
			// material related settings

			variable mat_perl_roughness = {
				"mat_perl_roughness", ("Roughness used for perl materials (0-1)"),
				0.10f
			};

			variable mat_perl_metalness = {
				"mat_perl_metalness", ("Metalness used for perl materials (0-1)"),
				0.10f
			};

			variable mat_perl_view_scalar = {
				"mat_perl_view_scalar", ("Scales view depended min/max color influence value"),
				1.20f
			};

			variable mat_perl_view_primary_color_scalar = {
				"mat_perl_view_primary_color_scalar", ("Scales view depended primary color influence value"),
				1.20f
			};

			variable mat_perl_view_primary_color_blend_scalar = {
				"mat_perl_view_primary_color_blend_scalar", ("Scales view depended primary color blending value"),
				2.00f
			};


			// --------------------------------------
			variable mat_matte_roughness = {
				"mat_matte_roughness", ("Roughness used for matte materials (0-1)"),
				0.50f };

			variable mat_matte_metalness = {
				"mat_matte_metalness", ("Metalness used for matte materials (0-1)"),
				0.05f };

			variable mat_matte_view_scalar = {
				"mat_matte_view_scalar", ("Scales view depended min/max color influence value"),
				1.15f };

			variable mat_matte_view_primary_color_scalar = {
				"mat_matte_view_primary_color_scalar", ("Scales view depended primary color influence value"),
				0.70f };

			variable mat_matte_view_primary_color_blend_scalar = {
				"mat_matte_view_primary_color_blend_scalar", ("Scales view depended primary color blending value"),
				1.00f };


			// --------------------------------------
			variable mat_metallic_roughness = {
				"mat_metallic_roughness", ("Roughness used for metallic materials (0-1)"),
				0.07f };

			variable mat_metallic_metalness = {
				"mat_metallic_metalness", ("Metalness used for metallic materials (0-1)"),
				0.30f };

			variable mat_metallic_view_scalar = {
				"mat_metallic_view_scalar", ("Scales view depended min/max color influence value"),
				1.00f };

			variable mat_metallic_view_primary_color_scalar = {
				"mat_metallic_view_primary_color_scalar", ("Scales view depended primary color influence value"),
				1.00f };

			variable mat_metallic_view_primary_color_blend_scalar = {
				"mat_metallic_view_primary_color_blend_scalar", ("Scales view depended primary color blending value"),
				1.00f };


			// --------------------------------------
			variable mat_high_gloss_roughness = {
				"mat_high_gloss_roughness", ("Roughness used for high_gloss materials (0-1)"),
				0.05f };

			variable mat_high_gloss_metalness = {
				"mat_high_gloss_metalness", ("Metalness used for high_gloss materials (0-1)"),
				0.05f };

			variable mat_high_gloss_view_scalar = {
				"mat_high_gloss_view_scalar", ("Scales view depended min/max color influence value"),
				0.65f };

			variable mat_high_gloss_view_primary_color_scalar = {
				"mat_high_gloss_view_primary_color_scalar", ("Scales view depended primary color influence value"),
				0.80f };

			variable mat_high_gloss_view_primary_color_blend_scalar = {
				"mat_high_gloss_view_primary_color_blend_scalar", ("Scales view depended primary color blending value"),
				0.85f };


			// --------------------------------------
			variable mat_iridiance_roughness = {
				"mat_iridiance_roughness", ("Roughness used for iridiance materials (0-1)"),
				0.25f };

			variable mat_iridiance_metalness = {
				"mat_iridiance_metalness", ("Metalness used for iridiance materials (0-1)"),
				0.75f };

			variable mat_iridiance_view_scalar = {
				"mat_iridiance_view_scalar", ("Scales view depended min/max color influence value"),
				0.20f };

			variable mat_iridiance_view_primary_color_scalar = {
				"mat_iridiance_view_primary_color_scalar", ("Scales view depended primary color influence value"),
				0.70f };

			variable mat_iridiance_view_primary_color_blend_scalar = {
				"mat_iridiance_view_primary_color_blend_scalar", ("Scales view depended primary color blending value"),
				0.80f };


			// --------------------------------------
			variable mat_candy_roughness = {
				"mat_candy_roughness", ("Roughness used for candy materials (0-1)"),
				0.10f };

			variable mat_candy_metalness = {
				"mat_candy_metalness", ("Metalness used for candy materials (0-1)"),
				0.20f };

			variable mat_candy_view_scalar = {
				"mat_candy_view_scalar", ("Scales view depended min/max color influence value"),
				0.75f };

			variable mat_candy_view_primary_color_scalar = {
				"mat_candy_view_primary_color_scalar", ("Scales view depended primary color influence value"),
				0.75f };

			variable mat_candy_view_primary_color_blend_scalar = {
				"mat_candy_view_primary_color_blend_scalar", ("Scales view depended primary color blending value"),
				1.3f };


			// --------------------------------------
			variable mat_chrome_roughness = {
				"mat_chrome_roughness", ("Roughness used for chrome materials (0-1)"),
				0.05f };

			variable mat_chrome_metalness = {
				"mat_chrome_metalness", ("Metalness used for chrome materials (0-1)"),
				0.85f };

			variable mat_chrome_view_scalar = {
				"mat_chrome_view_scalar", ("Scales view depended min/max color influence value"),
				0.70f };

			variable mat_chrome_view_primary_color_scalar = {
				"mat_chrome_view_primary_color_scalar", ("Scales view depended primary color influence value"),
				0.90f };

			variable mat_chrome_view_primary_color_blend_scalar = {
				"mat_chrome_view_primary_color_blend_scalar", ("Scales view depended primary color blending value"),
				1.50f };

			// ----------------------------------
			// light translation related settings

			

			// ----------------------------------
			// emissive related settings

			

			// ----------------------------------
			// general rendering related settings



		};
	
		static inline var_definitions vars = {};
	};
}