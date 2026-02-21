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
			float value[4] = {};
		};
	
		enum var_type : std::uint8_t
		{
			var_type_boolean = 0,
			var_type_integer = 1,
			var_type_value = 2,
			var_type_vec2 = 3,
			var_type_vec3 = 4,
			var_type_vec4 = 5,
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
	
			const char* get_str_value(bool get_default = false) const
			{
				const auto pvec = !get_default ? &m_var.value[0] : &m_var_default.value[0];
	
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

			/*variable nocull_distance_scenery =
			{
				"nocull_distance_scenery",
				("Distance (radius around player) where culling of level areas is disabled."),
				80.0f
			};*/

			variable nocull_distance_meshes =
			{
				"nocull_distance_meshes",
				("Distance (radius around player) where culling of objects is disabled"),
				150.0f
			};

			/*variable nocull_disable_precull = {
				"nocull_disable_precull",
				("Disables the preculling system (occlusion)"),
				false
			};*/

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