#pragma once

namespace comp
{
	extern int g_is_rendering_particle;
	extern int g_is_rendering_car;
	extern int g_is_rendering_car_normalmap;
	extern int g_is_rendering_world;

	extern bool g_rendered_first_primitive;
	extern bool g_applied_hud_hack;
	extern bool g_applied_phone_hack;

	namespace tex_addons
	{
		extern bool initialized;
		extern LPDIRECT3DTEXTURE9 berry;
		extern LPDIRECT3DTEXTURE9 white;
		extern LPDIRECT3DTEXTURE9 red;
		extern void init_texture_addons(bool release = false);
	}

	enum class RemixModifier : std::uint16_t
	{
		None = 0,
		EmissiveScalar = 1 << 0,
		Roughness = 1 << 1,
		EnableVertexColor = 1 << 2,
		Free03 = 1 << 3,
		RemoveVertexColorKeepAlpha = 1 << 4,
		VehicleShader = 1 << 5,
		Free06 = 1 << 6,
		Free07 = 1 << 7,
		Free08 = 1 << 8,
		Free09 = 1 << 9,
		Free10 = 1 << 10,
		Free11 = 1 << 11,
		Free12 = 1 << 12,
		Free13 = 1 << 13,
		Free14 = 1 << 14,
		Free15 = 1 << 15,
	};

	enum remix_custom_rs
	{
		RS_42_TEXTURE_CATEGORY = 42,
		RS_149_REMIX_MODIFIER = 149,
		RS_150_TEXTURE_HASH = 150,
		RS_169_EMISSIVE_SCALE = 169,
		RS_177_FREE = 177,
		RS_210_WETNESS_PARAMS_PACKED = 210,
		RS_211_VEHSHADER_PARAMS_PACKED1 = 211,
		RS_212_VEHSHADER_PARAMS_PACKED2 = 212,
		RS_213_FREE = 213,
		RS_214_FREE = 214,
		RS_215_FREE = 215,
		RS_216_FREE = 216,
		RS_217_FREE = 217,
		RS_218_FREE = 218,
		RS_219_FREE = 219,
		RS_220_FREE = 220,
	};

	constexpr RemixModifier operator|(RemixModifier lhs, RemixModifier rhs) {
		return static_cast<RemixModifier>(static_cast<std::uint32_t>(lhs) | static_cast<std::uint32_t>(rhs));
	}

	constexpr RemixModifier& operator|=(RemixModifier& lhs, RemixModifier rhs) {
		lhs = static_cast<RemixModifier>(static_cast<std::uint32_t>(lhs) | static_cast<std::uint32_t>(rhs));
		return lhs;
	}

	constexpr RemixModifier operator&(RemixModifier lhs, RemixModifier rhs) {
		return static_cast<RemixModifier>(static_cast<std::uint32_t>(lhs) & static_cast<std::uint32_t>(rhs));
	}

	constexpr RemixModifier& operator&=(RemixModifier& lhs, RemixModifier rhs) {
		lhs = static_cast<RemixModifier>(static_cast<std::uint32_t>(lhs) & static_cast<std::uint32_t>(rhs));
		return lhs;
	}

	// can't use remixapi_InstanceCategoryFlags as they don't match up with InstanceCategories
	enum class InstanceCategories : uint32_t
	{
		WorldUI = 1 << 0,
		WorldMatte = 1 << 1, // Background Offset
		Sky = 1 << 2,
		Ignore = 1 << 3,
		IgnoreLights = 1 << 4,
		IgnoreAntiCulling = 1 << 5,
		IgnoreMotionBlur = 1 << 6,
		IgnoreOpacityMicromap = 1 << 7,
		IgnoreAlphaChannel = 1 << 8,
		Hidden = 1 << 9,
		Particle = 1 << 10,
		Beam = 1 << 11,
		DecalStatic = 1 << 12,
		DecalDynamic = 1 << 13,
		DecalSingleOffset = 1 << 14,
		DecalNoOffset = 1 << 15,
		AlphaBlendToCutout = 1 << 16,
		Terrain = 1 << 17,
		AnimatedWater = 1 << 18,
		ThirdPersonPlayerModel = 1 << 19,
		ThirdPersonPlayerBody = 1 << 20,
		IgnoreBakedLighting = 1 << 21,
		IgnoreTransparencyLayer = 1 << 22,
		ParticleEmitter = 1 << 23,
		DisableBackfaceCulling = 1 << 24,
		Count = 24,
		None = 0u
	};

	constexpr InstanceCategories operator|(InstanceCategories lhs, InstanceCategories rhs) {
		return static_cast<InstanceCategories>(static_cast<std::uint32_t>(lhs) | static_cast<std::uint32_t>(rhs));
	}

	constexpr InstanceCategories& operator|=(InstanceCategories& lhs, InstanceCategories rhs) {
		lhs = static_cast<InstanceCategories>(static_cast<std::uint32_t>(lhs) | static_cast<std::uint32_t>(rhs));
		return lhs;
	}

	constexpr InstanceCategories operator&(InstanceCategories lhs, InstanceCategories rhs) {
		return static_cast<InstanceCategories>(static_cast<std::uint32_t>(lhs) & static_cast<std::uint32_t>(rhs));
	}

	constexpr InstanceCategories& operator&=(InstanceCategories& lhs, InstanceCategories rhs) {
		lhs = static_cast<InstanceCategories>(static_cast<std::uint32_t>(lhs) & static_cast<std::uint32_t>(rhs));
		return lhs;
	}

	class drawcall_mod_context
	{
	public:

		bool has_saved_renderstate(const D3DRENDERSTATETYPE& state) const
		{
			if (saved_render_state_.contains(state)) {
				return true;
			}

			return false;
		}

		bool has_saved_renderstate(const uint32_t& state) const {
			return has_saved_renderstate((D3DRENDERSTATETYPE)state);
		}

		// ---

		// set texture 0 transform
		void set_texture_transform(IDirect3DDevice9* device, const D3DXMATRIX* matrix)
		{
			if (matrix)
			{
				device->SetTransform(D3DTS_TEXTURE0, matrix);
				tex0_transform_set_ = true;
			}
		}

		// save vertex shader
		void save_vs(IDirect3DDevice9* device)
		{
			device->GetVertexShader(&vs_);
			vs_set_ = true;
		}

		// save vertex shader
		void save_ps(IDirect3DDevice9* device)
		{
			device->GetPixelShader(&ps_);
			ps_set_ = true;
		}

		// save texture at stage 0 or 1
		void save_texture(IDirect3DDevice9* device, const bool stage)
		{
			if (!stage)
			{
#if DEBUG
				if (tex0_set_) {
					OutputDebugStringA("save_texture:: tex0 was already saved\n"); return;
				}
#endif

				device->GetTexture(0, &tex0_);
				tex0_set_ = true;
			}
			else
			{
#if DEBUG
				if (tex1_set_) {
					OutputDebugStringA("save_texture:: tex1 was already saved\n"); return;
				}
#endif

				device->GetTexture(1, &tex1_);
				tex1_set_ = true;
			}
		}

		// save render state (e.g. D3DRS_TEXTUREFACTOR) - returns false if rs was previously saved
		bool save_rs(IDirect3DDevice9* device, const D3DRENDERSTATETYPE& state)
		{
			if (saved_render_state_.contains(state)) {
				return false;
			}

			DWORD temp;
			device->GetRenderState(state, &temp);
			saved_render_state_[state] = temp;
			return true;
		}

		bool save_rs(IDirect3DDevice9* device, const uint32_t& state)
		{
			return save_rs(device, (D3DRENDERSTATETYPE)state);
		}

		// save sampler state (D3DSAMPLERSTATETYPE)
		void save_ss(IDirect3DDevice9* device, const D3DSAMPLERSTATETYPE& state)
		{
			if (saved_sampler_state_.contains(state)) {
				return;
			}

			DWORD temp;
			device->GetSamplerState(0, state, &temp);
			saved_sampler_state_[state] = temp;
		}

		// save texture stage 0 state (e.g. D3DTSS_ALPHAARG1) - returns false if tss was previously saved
		bool save_tss(IDirect3DDevice9* device, const D3DTEXTURESTAGESTATETYPE& type)
		{
			if (saved_texture_stage_state_.contains(type)) {
				return false;
			}

			DWORD temp;
			device->GetTextureStageState(0, type, &temp);
			saved_texture_stage_state_[type] = temp;
			return true;
		}

		// save D3DTS_VIEW
		void save_view_transform(IDirect3DDevice9* device)
		{
			device->GetTransform(D3DTS_VIEW, &view_transform_);
			view_transform_set_ = true;
		}

		// save D3DTS_PROJECTION
		void save_projection_transform(IDirect3DDevice9* device)
		{
			device->GetTransform(D3DTS_PROJECTION, &projection_transform_);
			projection_transform_set_ = true;
		}

		// restore vertex shader
		void restore_vs(IDirect3DDevice9* device)
		{
			if (vs_set_)
			{
				device->SetVertexShader(vs_);
				vs_set_ = false;
			}
		}

		// restore pixel shader
		void restore_ps(IDirect3DDevice9* device)
		{
			if (ps_set_)
			{
				device->SetPixelShader(ps_);
				ps_set_ = false;
			}
		}

		// restore texture at stage 0 or 1
		void restore_texture(IDirect3DDevice9* device, const bool stage)
		{
			if (!stage)
			{
				if (tex0_set_)
				{
					device->SetTexture(0, tex0_);
					tex0_set_ = false;
				}
			}
			else
			{
				if (tex1_set_)
				{
					device->SetTexture(1, tex1_);
					tex1_set_ = false;
				}
			}
		}

		// restore a specific render state (e.g. D3DRS_TEXTUREFACTOR)
		void restore_render_state(IDirect3DDevice9* device, const D3DRENDERSTATETYPE& state)
		{
			if (saved_render_state_.contains(state)) {
				device->SetRenderState(state, saved_render_state_[state]);
			}
		}

		// restore a specific sampler state (D3DSAMPLERSTATETYPE)
		void restore_sampler_state(IDirect3DDevice9* device, const D3DSAMPLERSTATETYPE& state)
		{
			if (saved_sampler_state_.contains(state)) {
				device->SetSamplerState(0, state, saved_sampler_state_[state]);
			}
		}

		// restore a specific texture stage 0 state (e.g. D3DTSS_ALPHAARG1)
		void restore_texture_stage_state(IDirect3DDevice9* device, const D3DTEXTURESTAGESTATETYPE& type)
		{
			if (saved_texture_stage_state_.contains(type)) {
				device->SetTextureStageState(0, type, saved_texture_stage_state_[type]);
			}
		}

		// restore texture 0 transform to identity
		void restore_texture_transform(IDirect3DDevice9* device)
		{
			device->SetTransform(D3DTS_TEXTURE0, &shared::globals::IDENTITY);
			tex0_transform_set_ = false;
		}

		// restore saved D3DTS_VIEW
		void restore_view_transform(IDirect3DDevice9* device)
		{
			if (view_transform_set_)
			{
				device->SetTransform(D3DTS_VIEW, &view_transform_);
				view_transform_set_ = false;
			}
		}

		// restore saved D3DTS_PROJECTION
		void restore_projection_transform(IDirect3DDevice9* device)
		{
			if (projection_transform_set_)
			{
				device->SetTransform(D3DTS_PROJECTION, &projection_transform_);
				projection_transform_set_ = false;
			}
		}

		// restore all changes
		void restore_all(IDirect3DDevice9* device)
		{
			restore_vs(device);
			restore_ps(device);
			restore_texture(device, 0);
			restore_texture(device, 1);
			restore_texture_transform(device);
			restore_view_transform(device);
			restore_projection_transform(device);

			for (auto& rs : saved_render_state_) {
				device->SetRenderState(rs.first, rs.second);
			}

			for (auto& ss : saved_sampler_state_) {
				device->SetSamplerState(0, ss.first, ss.second);
			}

			for (auto& tss : saved_texture_stage_state_) {
				device->SetTextureStageState(0, tss.first, tss.second);
			}
		}

		// reset the stored context data
		void reset_context()
		{
			vs_ = nullptr; vs_set_ = false;
			ps_ = nullptr; ps_set_ = false;
			tex0_ = nullptr; tex0_set_ = false;
			tex1_ = nullptr; tex1_set_ = false;
			tex0_transform_set_ = false;
			view_transform_set_ = false;
			projection_transform_set_ = false;
			saved_render_state_.clear();
			saved_sampler_state_.clear();
			saved_texture_stage_state_.clear();
			modifiers.reset();
			info.reset();
		}

		struct modifiers_s
		{
			bool do_not_render = false;

			bool dual_render = false;
			bool dual_render_mode_blend_add = false;
			bool dual_render_mode_blend_diffuse = false;
			IDirect3DBaseTexture9* dual_render_texture = nullptr;

			InstanceCategories remix_instance_categories = InstanceCategories::None;
			RemixModifier remix_modifier = RemixModifier::None;

			void reset()
			{
				do_not_render = false;
				dual_render = false;
				dual_render_mode_blend_add = false;
				dual_render_mode_blend_diffuse = false;
				dual_render_texture = nullptr;

				remix_instance_categories = InstanceCategories::None;
				remix_modifier = RemixModifier::None;
			}
		};

		// special handlers for the next prim/s
		modifiers_s modifiers;

		struct info_s
		{
			// put any info about the current pass here if needed
			// info will be reset after the current drawcall

			//std::string_view shader_name;
			IDirect3DDevice9* device_ptr = nullptr;
			
			float shaderconst_emissive_intensity = -1.0f;

			Vector cvDiffuseMin;
			Vector cvDiffuseRange;

			void reset()
			{
				//shader_name = "";
				device_ptr = nullptr;

				shaderconst_emissive_intensity = -1.0f;

				cvDiffuseMin.Zero();
				cvDiffuseRange.Zero();
			}
		};

		// holds information about the current pass
		info_s info;

		// constructor for singleton
		drawcall_mod_context() = default;

	private:
		// Render states to save
		IDirect3DVertexShader9* vs_ = nullptr;
		IDirect3DPixelShader9* ps_ = nullptr;
		IDirect3DBaseTexture9* tex0_ = nullptr;
		IDirect3DBaseTexture9* tex1_ = nullptr;
		bool vs_set_ = false;
		bool ps_set_ = false;
		bool tex0_set_ = false;
		bool tex1_set_ = false;
		bool tex0_transform_set_ = false;
		char pad1[3]; // pad to align by 4

		D3DMATRIX view_transform_ = {};
		D3DMATRIX projection_transform_ = {};
		bool view_transform_set_ = false;
		bool projection_transform_set_ = false;
		char pad2[2]; // pad to align by 4

		// store saved render states (with the type as the key)
		std::unordered_map<D3DRENDERSTATETYPE, DWORD> saved_render_state_;

		// store saved render states (with the type as the key)
		std::unordered_map<D3DSAMPLERSTATETYPE, DWORD> saved_sampler_state_;

		// store saved texture stage states (with type as the key)
		std::unordered_map<D3DTEXTURESTAGESTATETYPE, DWORD> saved_texture_stage_state_;
	};

	// ----

	class renderer final : public shared::common::loader::component_module
	{
	public:
		renderer();
		~renderer();

		static inline renderer* p_this = nullptr;
		static renderer* get() { return p_this; }

		static bool is_initialized()
		{
			if (const auto mod = get(); mod && mod->m_initialized) {
				return true;
			}
			return false;
		}

		void manually_trigger_remix_injection(IDirect3DDevice9* dev);
		HRESULT on_draw_primitive(IDirect3DDevice9* dev, const D3DPRIMITIVETYPE& PrimitiveType, const UINT& StartVertex, const UINT& PrimitiveCount);
		HRESULT on_draw_primitive_up(IDirect3DDevice9* dev, D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
		HRESULT on_draw_indexed_prim(IDirect3DDevice9* dev, const D3DPRIMITIVETYPE& PrimitiveType, const INT& BaseVertexIndex, const UINT& MinVertexIndex, const UINT& NumVertices, const UINT& startIndex, const UINT& primCount);
		HRESULT on_draw_indexed_prim_up(IDirect3DDevice9* dev, D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
		

		static void set_remix_modifier(IDirect3DDevice9* dev, RemixModifier mod);
		static void set_remix_emissive_intensity(IDirect3DDevice9* dev, float intensity, bool no_overrides = false);

		enum eWetnessFlags : uint8_t
		{
			WETNESS_FLAG_ENABLE_VARIATION = 1 << 0,
			WETNESS_FLAG_ENABLE_RAINDROPS = 1 << 1,				// either expensive or normal raindrops
			WETNESS_FLAG_USE_LOCAL_COORDINATES = 1 << 2,		// uses world position but transformed to object local space
			WETNESS_FLAG_ENABLE_EXP_RAINDROPS = 1 << 3,			// either expensive or normal raindrops
			WETNESS_FLAG_RAINDROPS_HALF_DENSITY = 1 << 4,		// raindrops with half density
			WETNESS_FLAG_ENABLE_PUDDLE_LAYER = 1 << 5,			// adds a puddle layer to wet surfaces 
			WETNESS_FLAG_ENABLE_OCCLUSION_TEST = 1 << 6,		// test if something is covering this surface
			WETNESS_FLAG_ENABLE_OCCLUSION_SMOOTHING = 1 << 7,	// smooth occlusion edges - needs DLSS RR
			WETNESS_FLAG_NONE = 0u
		};

		static void set_remix_roughness_settings(IDirect3DDevice9* dev, float roughness_scalar, float max_z = 0.35f, float blend_width = 0.65f, float raindrop_scale = 0.0f, uint8_t flags = WETNESS_FLAG_NONE);
		
		
		enum eVehicleShaderFlags : uint8_t
		{
			VEHSHADER_FLAG_FREE01 = 1 << 0,
			VEHSHADER_FLAG_FREE02 = 1 << 1,
			VEHSHADER_FLAG_FREE03 = 1 << 2,
			VEHSHADER_FLAG_FREE04 = 1 << 3,
			VEHSHADER_FLAG_FREE05 = 1 << 4,
			VEHSHADER_FLAG_FREE06 = 1 << 5,
			VEHSHADER_FLAG_FREE07 = 1 << 6,
			VEHSHADER_FLAG_FREE08 = 1 << 7,
			VEHSHADER_FLAG_NONE = 0u
		};

		static void set_remix_vehicle_shader_settings(IDirect3DDevice9* dev, const Vector4D& color, float roughness, float metalness, float free2, uint8_t flags = VEHSHADER_FLAG_NONE);
		
		static void set_remix_temp_float01(IDirect3DDevice9* dev, float value);
		static void set_remix_temp_float02(IDirect3DDevice9* dev, float value);
		static void set_remix_temp_float03(IDirect3DDevice9* dev, float value);
		static void set_remix_temp_float04(IDirect3DDevice9* dev, float value);
		static void set_remix_texture_categories(IDirect3DDevice9* dev, const InstanceCategories& cat);
		static void set_remix_texture_hash(IDirect3DDevice9* dev, const std::uint32_t& hash);

		bool m_triggered_remix_injection = false;
		bool m_modified_draw_prim = false;
		static inline drawcall_mod_context dc_ctx {};

	private:
		bool m_initialized = false;
	};
}
