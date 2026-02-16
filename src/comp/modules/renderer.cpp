#include "std_include.hpp"
#include "renderer.hpp"

#include "comp_settings.hpp"
#include "imgui.hpp"

namespace comp
{
	int g_is_rendering_particle = 0;
	int g_is_rendering_car = 0;
	int g_is_rendering_car_normalmap = 0;
	int g_is_rendering_world = 0;

	int g_is_rendering_dry_road = 0;
	int g_is_rendering_world_normalmap = 0;
	int g_is_rendering_glass_reflect = 0;
	int g_is_rendering_sky = 0;
	int g_is_rendering_water = 0;

	bool g_rendered_first_primitive = false;
	bool g_applied_hud_hack = false; // was hud "injection" applied this frame

	game::material_instance g_current_material_data = {};

	namespace tex_addons
	{
		bool initialized = false;
		LPDIRECT3DTEXTURE9 berry = nullptr;
		LPDIRECT3DTEXTURE9 white = nullptr;
		LPDIRECT3DTEXTURE9 red = nullptr;

		void init_texture_addons(bool release)
		{
			if (release)
			{
				if (tex_addons::berry) tex_addons::berry->Release();
				if (tex_addons::white) tex_addons::white->Release();
				if (tex_addons::red) tex_addons::red->Release();
				return;
			}

			shared::common::log("Renderer", "Loading CompMod Textures ...", shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);

			auto load_texture = [](IDirect3DDevice9* dev, const char* path, LPDIRECT3DTEXTURE9* tex)
				{
					HRESULT hr;
					hr = D3DXCreateTextureFromFileA(dev, path, tex);
					if (FAILED(hr)) shared::common::log("Renderer", std::format("Failed to load {}", path), shared::common::LOG_TYPE::LOG_TYPE_ERROR, true);
				};

			const auto dev = shared::globals::d3d_device;
			load_texture(dev, "rtx_comp\\textures\\berry.png", &tex_addons::berry);
			load_texture(dev, "rtx_comp\\textures\\white.png", &tex_addons::white);
			load_texture(dev, "rtx_comp\\textures\\red.png", &tex_addons::red);
			tex_addons::initialized = true;
		}
	}

	// Uses unused Renderstate 149 to set per drawcall modifiers
	// ~ req. runtime changes
	void renderer::set_remix_modifier(IDirect3DDevice9* dev, RemixModifier mod)
	{
		dc_ctx.save_rs(dev, RS_149_REMIX_MODIFIER);
		dc_ctx.modifiers.remix_modifier |= mod;

		dev->SetRenderState((D3DRENDERSTATETYPE)RS_149_REMIX_MODIFIER, static_cast<DWORD>(dc_ctx.modifiers.remix_modifier));
	}

	// Uses unused Renderstate 149 & 169 to tweak the emissive intensity of remix materials (legacy/opaque)
	// ~ currently req. runtime changes --> remixTempFloat01FromD3D
	/// @param no_overrides	will not override any previously set intensity if true
	void renderer::set_remix_emissive_intensity(IDirect3DDevice9* dev, float intensity, bool no_overrides)
	{
		const bool result = dc_ctx.save_rs(dev, RS_169_EMISSIVE_SCALE);
		if (!result && no_overrides) {
			return;
		}

		dc_ctx.info.shaderconst_emissive_intensity = intensity;
		set_remix_modifier(dev, RemixModifier::EmissiveScalar);
		set_remix_temp_float01(dev, intensity);
	}

	/// Modifies roughness / wetness on a per drawcall level
	/// @param roughness_scalar		scales final roughness (after sampling roughness texture)
	/// @param max_z				determines if a surface can get wet based on its z normal (orientation)
	/// @param blend_width			the blending width going from unmodified roughness to scaled roughness when max_z is hit
	/// @param raindrop_scale		scale of raindrops (if enabled via flags)
	/// @param flags				renderer::eWetnessFlags - additional modifiers
	void renderer::set_remix_roughness_settings(IDirect3DDevice9* dev, float roughness_scalar, float max_z, float blend_width, float raindrop_scale, uint8_t flags)
	{
		set_remix_modifier(dev, RemixModifier::Roughness);

		// encode a float value into n bits
		auto encode_range = [](const float& v, const int& bits, const float& max_range) -> uint16_t
			{
				const uint32_t max_val = (1u << bits) - 1u;
				const float normalized = std::clamp(v, 0.0f, max_range) / max_range;
				return static_cast<uint32_t>(std::round(normalized * max_val));
			};

		// pack wetness_params1 (lower 16 bits): scalar(6) + max_z(5) + blend_width(5) = 16 bits
		const uint16_t scalar_bits = encode_range(roughness_scalar + 2.0f, 6, 4.0f);  // 6 bits: 0-63 → 0-4 (remapping -2 to +2 to 0-4 -> undo on runtime side)
		const uint16_t max_z_bits = encode_range(max_z, 5, 1.0f);                     // 5 bits: 0-31 → 0-1
		const uint16_t blend_width_bits = encode_range(blend_width, 5, 1.0f);         // 5 bits: 0-31 → 0-1
		const uint16_t wetness_params1 = (scalar_bits << 10) | (max_z_bits << 5) | blend_width_bits;

		// pack wetness_params2 (upper 16 bits):
		// lower 8 bits = raindrop_scale (0-10)
		// upper 8 bits = flags
		const uint16_t raindrop_scale_bits = encode_range(raindrop_scale, 8, 10.0f);
		const uint16_t wetness_modifier_bits = uint16_t(flags) << 8; // shift flags to upper 8 bits
		const uint16_t wetness_params2 = raindrop_scale_bits | wetness_modifier_bits;

		// pack into DWORD: lower 16 bits = wetness_params1, upper 16 bits = wetness_params2
		uint32_t packedDword = (uint32_t(wetness_params2) << 16) | uint32_t(wetness_params1);

		dc_ctx.save_rs(dev, RS_215_WETNESS_PARAMS_PACKED);
		dev->SetRenderState((D3DRENDERSTATETYPE)RS_215_WETNESS_PARAMS_PACKED, packedDword);
	}

	void renderer::set_remix_vehicle_shader_settings(IDirect3DDevice9* dev, const Vector4D& color, float roughness, float metalness, float free2, uint8_t flags)
	{
		set_remix_modifier(dev, RemixModifier::VehicleShader);

		// encode a float value into n bits
		auto encode_range = [](const float& v, const int& bits, const float& max_range) -> uint16_t
			{
				const uint32_t max_val = (1u << bits) - 1u;
				const float normalized = std::clamp(v, 0.0f, max_range) / max_range;
				return static_cast<uint32_t>(std::round(normalized * max_val));
			};

		dc_ctx.save_rs(dev, RS_211_VEHSHADER_PARAMS_PACKED1);
		dc_ctx.save_rs(dev, RS_212_VEHSHADER_PARAMS_PACKED2);

		uint32_t packed_dword = 0u;

		const uint32_t r_enc = encode_range(color.x, 8, 1.0f);
		const uint32_t g_enc = encode_range(color.y, 8, 1.0f);
		const uint32_t b_enc = encode_range(color.z, 8, 1.0f);
		const uint32_t a_enc = encode_range(color.w, 8, 1.0f);

		packed_dword =
			  (r_enc << 0)
			| (g_enc << 8)
			| (b_enc << 16)
			| (a_enc << 24);

		dev->SetRenderState((D3DRENDERSTATETYPE)RS_211_VEHSHADER_PARAMS_PACKED1, packed_dword);

		const uint32_t rough_enc = encode_range(roughness, 8, 1.0f);
		const uint32_t metal_enc = encode_range(metalness, 8, 1.0f);
		const uint32_t free_enc = encode_range(free2, 8, 1.0f);

		packed_dword =
			  (rough_enc << 0)
			| (metal_enc << 8)
			| (free_enc << 16)
			| (uint32_t(flags) << 24);

		dev->SetRenderState((D3DRENDERSTATETYPE)RS_212_VEHSHADER_PARAMS_PACKED2, packed_dword);
	}

	// ---

	// uses unused Renderstate 169 to pass per drawcall data
	// - used by emissive scalar mod
	// ~ req. runtime changes --> remixTempFloat01FromD3D
	void renderer::set_remix_temp_float01(IDirect3DDevice9* dev, float value)
	{
		dc_ctx.save_rs(dev, RS_169_EMISSIVE_SCALE);
		dev->SetRenderState((D3DRENDERSTATETYPE)RS_169_EMISSIVE_SCALE, *reinterpret_cast<DWORD*>(&value));
	}


	// ---

	// Uses unused Renderstate 177 to pass per drawcall data
	// ~ req. runtime changes --> remixTempFloat02FromD3D
	void renderer::set_remix_temp_float02(IDirect3DDevice9* dev, float value)
	{
		dc_ctx.save_rs(dev, RS_177_FREE);
		dev->SetRenderState((D3DRENDERSTATETYPE)RS_177_FREE, *reinterpret_cast<DWORD*>(&value));
	}

	// Uses unused Renderstate 213 to pass per drawcall data
	void renderer::set_remix_temp_float03(IDirect3DDevice9* dev, float value)
	{
		dc_ctx.save_rs(dev, RS_213_FREE);
		dev->SetRenderState((D3DRENDERSTATETYPE)RS_213_FREE, *reinterpret_cast<DWORD*>(&value));
	}

	// Uses unused Renderstate 214 to pass per drawcall data
	void renderer::set_remix_temp_float04(IDirect3DDevice9* dev, float value)
	{
		dc_ctx.save_rs(dev, RS_214_FREE);
		dev->SetRenderState((D3DRENDERSTATETYPE)RS_214_FREE, *reinterpret_cast<DWORD*>(&value));
	}

	// ---

	// Uses unused Renderstate 42 to set remix texture categories
	// ~ req. runtime changes
	void renderer::set_remix_texture_categories(IDirect3DDevice9* dev, const InstanceCategories& cat)
	{
		dc_ctx.save_rs(dev, RS_42_TEXTURE_CATEGORY);
		dc_ctx.modifiers.remix_instance_categories |= cat;
		dev->SetRenderState((D3DRENDERSTATETYPE)RS_42_TEXTURE_CATEGORY, static_cast<DWORD>(dc_ctx.modifiers.remix_instance_categories));
	}

	// Uses unused Renderstate 150 to set custom remix hash
	// ~ req. runtime changes
	void renderer::set_remix_texture_hash(IDirect3DDevice9* dev, const std::uint32_t& hash)
	{
		dc_ctx.save_rs(dev, RS_150_TEXTURE_HASH);
		dev->SetRenderState((D3DRENDERSTATETYPE)RS_150_TEXTURE_HASH, hash);
	}

	enum class paint_type
	{
		unknown,
		perl,
		matte,
		metallic,
		high_gloss,
		iridiance,
		candy,
		chrome
	};

	inline bool approx(float a, float b, float eps = 0.02f) {
		return std::abs(a - b) <= eps;
	}

	inline float avg(Vector v) {
		return (v.x + v.y + v.z) / 3.0f;
	}

	inline bool is_one_vector(const Vector& v) {
		return approx(v.x, 1.0f) && approx(v.y, 1.0f) && approx(v.z, 1.0f);
	}

	paint_type detect_paint_type(const game::material_data& m)
	{
		// differentiate between dynamic + hardcoded materials
		const bool hardcoded_mode = is_one_vector(m.envmap_min) && is_one_vector(m.envmap_max);
		const float env_min = hardcoded_mode ? m.envmap_min_scale : avg(m.envmap_min);
		const float env_max = hardcoded_mode ? m.envmap_max_scale : avg(m.envmap_max);

		const bool env_equal = approx(env_min, env_max);
		const float& flakes = m.specular_flakes;

		// -------------------------------------------------
		// Chrome (most unique)
		// EnvMin == EnvMax AND flakes = 0.1
		// -------------------------------------------------
		if (env_equal && approx(flakes, 0.1f)) {
			return paint_type::chrome;
		}

		// -------------------------------------------------
		// High Gloss
		// Only type with specular > 40
		// -------------------------------------------------
		if (m.specular_power >= 40.0f) {
			return paint_type::high_gloss;
		}

		// -------------------------------------------------
		// Matte
		// No env contribution
		// -------------------------------------------------
		if (approx(env_min, 0.0f) && approx(env_max, 0.0f)) {
			return paint_type::matte;
		}

		// -------------------------------------------------
		// Candy
		// Very high flakes (2 - 3.5)
		// -------------------------------------------------
		if (flakes >= 2.0f && approx(env_max, 0.5f)) {
			return paint_type::candy;
		}

		// -------------------------------------------------
		// PERL
		// EnvMapPower fixed at 0.15 AND no flakes
		// -------------------------------------------------
		if (approx(m.envmap_power, 0.15f) && approx(flakes, 0.0f)) {
			return paint_type::perl;
		}

		// -------------------------------------------------
		// Iridiance
		// Mid spec range remaining
		// -------------------------------------------------
		if (m.specular_power >= 13.0f && m.specular_power <= 20.0f) {
			return paint_type::iridiance;
		}

		// -------------------------------------------------
		// Metallic
		// -------------------------------------------------
		if (approx(env_min, 1.75f) && approx(m.envmap_power, 1.0f)
			&& (m.envmap_clamp >= 0.95f && m.envmap_clamp <= 1.0f)
			&& (flakes >= 0.49f && flakes <= 0.76f))
		{
			return paint_type::metallic;
		}

		return paint_type::unknown;
	}

	bool get_pbr_values_for_paint(
		paint_type paint, 
		float& out_roughness, float& out_metalness, 
		float& out_powerx, float& out_diffuse_clamp, float& out_diffuse_clamp_range)
	{
		const auto& cs = comp_settings::get();

#define ASSIGN_MAT_SETTINGS(VAR) \
	out_roughness = cs->VAR##_roughness._float(); \
	out_metalness = cs->VAR##_metalness._float(); \
	out_powerx = cs->VAR##_view_scalar._float(); \
	out_diffuse_clamp = cs->VAR##_view_primary_color_scalar._float(); \
	out_diffuse_clamp_range = cs->VAR##_view_primary_color_blend_scalar._float();

		switch (paint)
		{
		case paint_type::perl:
			ASSIGN_MAT_SETTINGS(mat_perl);
			return true;

		case paint_type::matte: 
			ASSIGN_MAT_SETTINGS(mat_matte);
			return true;

		case paint_type::metallic:
			ASSIGN_MAT_SETTINGS(mat_metallic);
			return true;

		case paint_type::high_gloss: 
			ASSIGN_MAT_SETTINGS(mat_high_gloss);
			return true;

		case paint_type::iridiance: 
			ASSIGN_MAT_SETTINGS(mat_iridiance);
			return true;

		case paint_type::candy: 
			ASSIGN_MAT_SETTINGS(mat_candy);
			return true;

		case paint_type::chrome: 
			ASSIGN_MAT_SETTINGS(mat_chrome);
			return true;

		default: 
			break;
		}

		return false;
	}

	// ----

	drawcall_mod_context& setup_context(IDirect3DDevice9* dev)
	{
		auto& ctx = renderer::dc_ctx;
		ctx.info.device_ptr = dev;

		// any additional info about the current drawcall here

		return ctx;
	}


	// ----
	HRESULT renderer::on_draw_primitive(IDirect3DDevice9* dev, const D3DPRIMITIVETYPE& PrimitiveType, const UINT& StartVertex, const UINT& PrimitiveCount)
	{
		// Wait for the first rendered prim before further init of the comp mod 
		if (!g_rendered_first_primitive) {
			g_rendered_first_primitive = true;
		}

		if (!is_initialized() || shared::globals::imgui_is_rendering) {
			return dev->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
		}

		const auto im = imgui::get();
		im->m_stats._drawcall_prim_incl_ignored.track_single();

		auto& ctx = setup_context(dev);

		// use any logic to conditionally set this to disable the vertex shader and use fixed function fallback
		bool render_with_ff = false;

		/*if (g_is_rendering_something)
		{
			// do stuff here, eg:
			ctx.modifiers.do_not_render = true;
		}*/

		// use fixed function fallback if true
		if (render_with_ff)
		{
			ctx.save_vs(dev);
			dev->SetVertexShader(nullptr);
		}


		// example code - HUD is mostly drawn with non-indexed prims - the first with non-perspective proj might be a hud element
			//if (const auto viewport = game::vp; viewport)
			//{
			//	if (viewport->proj.m[3][3] == 1.0f) {
			//		manually_trigger_remix_injection(dev);
			//	}
			//}


		// ---------
		// draw

		auto hr = S_OK;

		// do not render next surface if set
		if (!ctx.modifiers.do_not_render && !im->m_dbg_disable_prim_draw)
		{
			hr = dev->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
			im->m_stats._drawcall_prim.track_single();
		}


		// ---------
		// post draw

		ctx.restore_all(dev);
		ctx.reset_context();

		return hr;
	}


	// ----
	HRESULT renderer::on_draw_primitive_up(IDirect3DDevice9* dev, D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
	{
		// Wait for the first rendered prim before further init of the comp mod 
		if (!g_rendered_first_primitive) {
			g_rendered_first_primitive = true;
		}

		if (!is_initialized() || shared::globals::imgui_is_rendering) {
			return dev->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
		}

		auto& ctx = setup_context(dev);
		const auto im = imgui::get();
		im->m_stats._drawcall_prim_up_incl_ignored.track_single();

		// if we set do_not_render somewhere before the actual drawcall -> do not draw and reset context
		if (ctx.modifiers.do_not_render)
		{
			ctx.restore_all(dev);
			ctx.reset_context();
			return S_OK;
		}

		// use any logic to conditionally set this to disable the vertex shader and use fixed function fallback
		bool render_with_ff = false;

		if (g_is_rendering_particle && im->m_dbg_force_ff_indexed_prim) {
			render_with_ff = true;
		}


		// use fixed function fallback if true
		if (render_with_ff)
		{
			ctx.save_vs(dev);
			dev->SetVertexShader(nullptr);
		}

		// ---------
		// draw

		auto hr = S_OK;

		// do not render next surface if set
		if (!ctx.modifiers.do_not_render && !im->m_dbg_disable_prim_up_draw)
		{
			hr = dev->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
			im->m_stats._drawcall_prim_up.track_single();
		}


		// ---------
		// post draw

		ctx.restore_all(dev);
		ctx.reset_context();

		return hr;

	}


	// ----
	HRESULT renderer::on_draw_indexed_prim(IDirect3DDevice9* dev, const D3DPRIMITIVETYPE& PrimitiveType, const INT& BaseVertexIndex, const UINT& MinVertexIndex, const UINT& NumVertices, const UINT& startIndex, const UINT& primCount)
	{
		// Wait for the first rendered prim before further init of the comp mod 
		if (!g_rendered_first_primitive) {
			g_rendered_first_primitive = true;
		}

		if (!is_initialized() || shared::globals::imgui_is_rendering) {
			return dev->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
		}

		auto& ctx = setup_context(dev);
		const auto im = imgui::get();

		// use any logic to conditionally set this to disable the vertex shader and use fixed function fallback
		bool render_with_ff = false;

		// any drawcall modifications in here
		if (!shared::globals::imgui_is_rendering)
		{
			im->m_stats._drawcall_indexed_prim_incl_ignored.track_single();

			// if we set do_not_render somewhere before the actual drawcall -> do not draw and reset context
			if (ctx.modifiers.do_not_render) 
			{
				ctx.restore_all(dev);
				ctx.reset_context();
				return S_OK;
			}

			auto& mat = g_current_material_data;
			const auto mat_name = std::string_view(g_current_material_data.name);

			if (im->m_dbg_force_ff_indexed_prim) {
				render_with_ff = true;
			}

			if (g_is_rendering_car)
			{
				if (im->m_dbg_disable_car) {
					ctx.modifiers.do_not_render = true;
				}
			}

			if (g_is_rendering_car_normalmap)
			{
				if (im->m_dbg_disable_world_normalmap) {
					ctx.modifiers.do_not_render = true;
				}
			}

			if (g_is_rendering_world || g_is_rendering_dry_road)
			{
				if (im->m_dbg_debug_bool01)
				{
					set_remix_roughness_settings(dev, im->m_debug_vector5.x, // TODO: wetness
						0.35f,
						0.65f,
						0.1f,
						WETNESS_FLAG_ENABLE_VARIATION | WETNESS_FLAG_ENABLE_PUDDLE_LAYER | WETNESS_FLAG_ENABLE_OCCLUSION_TEST | WETNESS_FLAG_ENABLE_OCCLUSION_SMOOTHING /*| WETNESS_FLAG_ENABLE_RAINDROPS*/);
				}
			}

			if (g_is_rendering_world)
			{
				if (im->m_dbg_disable_world) {
					ctx.modifiers.do_not_render = true;
				}
			}

			if (g_is_rendering_world_normalmap)
			{
				if (im->m_dbg_disable_world_normalmap) {
					ctx.modifiers.do_not_render = true;
				}
			}

			if (g_is_rendering_glass_reflect)
			{
				if (im->m_dbg_disable_glass) {
					ctx.modifiers.do_not_render = true;
				}
			}

			if (g_is_rendering_sky)
			{
				if (im->m_dbg_disable_sky) {
					ctx.modifiers.do_not_render = true;
				}

				render_with_ff = false;
			}

			if (g_is_rendering_car)
			{
				if (im->m_dbg_debug_bool01)
				{
					set_remix_roughness_settings(dev, im->m_debug_vector5.x, // TODO: wetness
						0.60f,
						0.65f,
						0.1f,
						WETNESS_FLAG_ENABLE_EXP_RAINDROPS | WETNESS_FLAG_USE_LOCAL_COORDINATES);
				}

				{
					if (ctx.info.cvDiffuseMin.x == 1.0f)
					{
						Vector4D v4_cvDiffuseMin;
						Vector4D v4_cvDiffuseRange;
						Vector4D v4_cvEnvmapMin;
						Vector4D v4_cvEnvmapRange;
						Vector4D v4_cvPowers;
						Vector4D v4_cvClampAndScales;
						Vector4D v4_cvVinylScales;

						dev->GetVertexShaderConstantF(21, &v4_cvDiffuseMin.x, 1);
						//Vector cvDiffuseMin = v4_cvDiffuseMin;

						dev->GetVertexShaderConstantF(22, &v4_cvDiffuseRange.x, 1);
						//Vector cvDiffuseRange = v4_cvDiffuseRange;

						dev->GetVertexShaderConstantF(23, &v4_cvEnvmapMin.x, 1);
						Vector cvEnvmapMin = v4_cvEnvmapMin;

						dev->GetVertexShaderConstantF(24, &v4_cvEnvmapRange.x, 1);
						Vector cvEnvmapRange = v4_cvEnvmapRange;

						dev->GetVertexShaderConstantF(25, &v4_cvPowers.x, 1);
						Vector cvPowers = v4_cvPowers;

						dev->GetVertexShaderConstantF(26, &v4_cvClampAndScales.x, 1);
						Vector cvClampAndScales = v4_cvClampAndScales;

						dev->GetVertexShaderConstantF(27, &v4_cvVinylScales.x, 1);
						//Vector cvVinylScales = v4_cvVinylScales;

						Vector4D col;

						// this was color approximation for earlier TFACTOR tests
						//float r0w = std::min(1.0f + im->m_debug_vector.x, v4_cvClampAndScales.x);
						//float r1w = r0w * v4_cvClampAndScales.z * (1.0f + im->m_debug_vector.y);
						//col = r1w * v4_cvDiffuseRange + v4_cvDiffuseMin;

#if 0
						float spec_power = cvPowers.y;
						float spec_term = sqrt(2.0f / (spec_power + 2.0f));
						float roughness = spec_term * spec_term;
						roughness *= cvClampAndScales.y;

						if (cvEnvmapRange.x == 0.0f && cvEnvmapRange.y == 0.0f && cvEnvmapRange.z == 0.0f) {
							roughness = 1.0f;
						}

						roughness = std::clamp(roughness, 0.0f, 1.0f);

						float metallic = pow(cvClampAndScales.y, 2.0f);
						metallic = std::clamp(metallic, 0.0f, 1.0f);
#endif

						if (im->m_dbg_debug_bool02 && mat_name == "DULLPLASTIC") {
							ctx.modifiers.do_not_render = true;
						}
						else if (im->m_dbg_debug_bool06 && mat_name == "INTERIOR") {
							ctx.modifiers.do_not_render = true;
						}
						else if (im->m_dbg_debug_bool07 && mat_name == "MAGMATTE") {
							ctx.modifiers.do_not_render = true;
						}
						else if (im->m_dbg_debug_bool08 && mat_name.empty()) {
							ctx.modifiers.do_not_render = true;
						}

						// ------

						// Notes:
						// - empty mat name => dynamic car paint

						float roughness = 1.0f;
						float metallic = 0.0f;
						float powerx_scale = 1.0f;
						float diffuse_clamp_scale = 1.0f;
						float diffuse_clamp_range = 1.0f;

						auto paint_type = detect_paint_type(mat.material);

						// try to detect paint type -> return hardcoded values
						// approximate dynamically otherwise
						if (!get_pbr_values_for_paint( paint_type, 
								roughness, metallic, powerx_scale, diffuse_clamp_scale, diffuse_clamp_range))
						{
							// no settings found -> approximate dynamically

							const float spec_power = cvPowers.y;
							float avg_env_min = (cvEnvmapMin.x + cvEnvmapMin.y + cvEnvmapMin.z) / 3.0;
							float avg_env_range = (cvEnvmapRange.x + cvEnvmapRange.y + cvEnvmapRange.z) / 3.0;

							if (!im->m_dbg_debug_bool03 && abs(avg_env_range) < 0.01f + im->m_debug_vector3.x)  // constant env (or none)
							{
								if (avg_env_min < (0.2f + im->m_debug_vector.y)) {
									roughness = 0.98;
								} else {
									roughness = 0.0;
								}
							}
							else
							{
								roughness = (0.63f + im->m_debug_vector.x) / sqrtf(spec_power);  // fits ~0.2 at power=10, ~0.1 at power=40/13-14
								roughness = std::clamp(roughness, 0.05f, 1.0f);  // small floor
								roughness *= (1.0f + im->m_debug_vector.z);
							}

							// Check if env tint is colored (non-monochrome)
							float var_min = std::max(std::max(abs(cvEnvmapMin.x - avg_env_min), abs(cvEnvmapMin.y - avg_env_min)), abs(cvEnvmapMin.z - avg_env_min));
							float var_range = std::max(std::max(abs(cvEnvmapRange.x - avg_env_range), abs(cvEnvmapRange.y - avg_env_range)), abs(cvEnvmapRange.z - avg_env_range));

							if (!im->m_dbg_debug_bool04 && (var_min > 0.15 + im->m_debug_vector3.y || var_range > 0.15 + im->m_debug_vector3.y)) {
								metallic = 1.0;
							}
							else  // monochrome (gray/white)
							{
								if (!im->m_dbg_debug_bool05 && abs(avg_env_range) < 0.01f) {
									metallic = (avg_env_min > (0.2f + im->m_debug_vector2.y)) ? 1.0f : 0.0f;
								}
								else
								{
									float base = 1.0f - avg_env_min;
									metallic = std::clamp(base * (2.5f + im->m_debug_vector2.x), 0.0f, 1.0f);  // 0.88→0.3, 0.75→0.625≈0.8, 1.0→0
									metallic *= (1.0f + im->m_debug_vector2.z);
								}
							}
						}

						// imgui debug
						bool was_vis = false;
						if (!im->m_vis_drawcall01)
						{
							im->m_vis_drawcall01 = true;
							im->m_vis_cvDiffuseMin = v4_cvDiffuseMin;
							im->m_vis_cvDiffuseRange = v4_cvDiffuseRange;
							im->m_vis_cvEnvmapMin = v4_cvEnvmapMin;
							im->m_vis_cvEnvmapRange = v4_cvEnvmapRange;
							im->m_vis_cvPowers = v4_cvPowers;
							im->m_vis_cvClampAndScales = v4_cvClampAndScales;
							im->m_vis_paint_color = col;
							im->m_vis_out_metalness = metallic;
							im->m_vis_out_roughness = roughness;

							im->m_vis_mat_data = mat.material;
							im->m_vis_mat_name = mat_name;

							std::string dmt;
							switch (paint_type)
							{
							case paint_type::perl: dmt = "perl"; break;
							case paint_type::matte: dmt = "matte"; break;
							case paint_type::metallic: dmt = "metallic"; break;
							case paint_type::high_gloss: dmt = "high_gloss"; break;
							case paint_type::iridiance: dmt = "iridiance"; break;
							case paint_type::candy: dmt = "candy"; break;
							case paint_type::chrome: dmt = "chrome"; break;
							default: dmt = "unkown - using approximation!"; break;
							}

							im->m_vis_detected_mat_type = std::move(dmt);

							was_vis = true;
						}

						ctx.save_rs(dev, D3DRS_TEXTUREFACTOR); // prob. not needed
						ctx.save_tss(dev, D3DTSS_COLOROP);
						ctx.save_tss(dev, D3DTSS_COLORARG1);
						ctx.save_tss(dev, D3DTSS_COLORARG2);

						// some car paints really go way above 1.0 .. so we should use that info somewhere ..
						// we now div by 2 so we do not have to clamp, then rescale in opaque shader
						//col.x = std::clamp(col.x, 0.0f, 1.0f);
						//col.y = std::clamp(col.y, 0.0f, 1.0f);
						//col.z = std::clamp(col.z, 0.0f, 1.0f);
						//col.w = std::clamp(col.w, 0.0f, 1.0f);

						if (was_vis) {
							im->m_vis_paint_color_post = col;
						}

						// -------------------
						// debug overwrites

						if (im->m_dbg_vehshader_color_override_enabled)
						{
							col.x = im->m_dbg_vehshader_color_override.x;
							col.y = im->m_dbg_vehshader_color_override.y;
							col.z = im->m_dbg_vehshader_color_override.z;
						}

						if (im->m_dbg_vehshader_roughness_override_enabled) {
							roughness = im->m_dbg_vehshader_roughness_override;
						}

						if (im->m_dbg_vehshader_metalness_override_enabled) {
							metallic = im->m_dbg_vehshader_metalness_override;
						}

						if (im->m_dbg_vehshader_vinylscale_override_enabled) {
							v4_cvVinylScales.x = im->m_dbg_vehshader_vinylscale_override;
						}

						// -----------------------
						// setup paint shader vars

						col.x = std::clamp(std::clamp(v4_cvDiffuseRange.x, 0.0f, 2.0f) * 0.5f, 0.0f, 1.0f); // color can be above 1 so we scale to 0-1 and back to 0-2 in the opaque shader
						col.y = std::clamp(std::clamp(v4_cvDiffuseRange.y, 0.0f, 2.0f) * 0.5f, 0.0f, 1.0f);
						col.z = std::clamp(std::clamp(v4_cvDiffuseRange.z, 0.0f, 2.0f) * 0.5f, 0.0f, 1.0f);
						col.w = v4_cvPowers.x;

						set_remix_temp_float03(dev, v4_cvPowers.x * powerx_scale); // use powers.x here because it v4_cvClampAndScales.x mostly stays below 1
						set_remix_temp_float04(dev, v4_cvClampAndScales.z * diffuse_clamp_range);

						set_remix_vehicle_shader_settings(dev, col, roughness, metallic, v4_cvVinylScales.x, VEHSHADER_FLAG_NONE);
						//set_remix_modifier(dev, RemixModifier::EnableVertexColor); // vertex colors do not work for some reason
						// use tfactor to pass a secondary color instead
						

						//dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(col.x, col.y, col.z, 1.0f));
						dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(
							std::clamp(v4_cvDiffuseMin.x, 0.0f, 1.0f),
							std::clamp(v4_cvDiffuseMin.y, 0.0f, 1.0f),
							std::clamp(v4_cvDiffuseMin.z, 0.0f, 1.0f),
							std::clamp(v4_cvClampAndScales.x * diffuse_clamp_scale, 0.0f, 1.0f)));

						dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE); 
						dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
						dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

						/*ctx.save_ps(dev);
						dev->SetPixelShader(nullptr);*/
					}
				}
			}

			/*if (g_is_rendering_world)
			{
				if (im->m_dbg_debug_bool01) {
					ctx.modifiers.do_not_render = true;
				}
			}*/
			
			// use fixed function fallback if true
			if (render_with_ff)
			{
				ctx.save_vs(dev);
				dev->SetVertexShader(nullptr);
			}
		} // end !imgui-is-rendering


		// ---------
		// draw

		auto hr = S_OK;

		// do not render next surface if set
		if (!ctx.modifiers.do_not_render && !im->m_dbg_disable_indexed_prim_draw)
		{
			hr = dev->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);

			if (!shared::globals::imgui_is_rendering) {
				im->m_stats._drawcall_indexed_prim.track_single();
			}
		}

		if (ctx.modifiers.dual_render)
		{
			if (ctx.modifiers.dual_render_texture)
			{
				// save og texture
				ctx.save_texture(dev, 0);

				// set new texture
				dev->SetTexture(0, ctx.modifiers.dual_render_texture);
			}

			// BLEND ADD mode
			if (ctx.modifiers.dual_render_mode_blend_add)
			{
				ctx.save_rs(dev, D3DRS_ALPHABLENDENABLE);
				dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

				ctx.save_rs(dev, D3DRS_BLENDOP);
				dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

				ctx.save_rs(dev, D3DRS_SRCBLEND);
				dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);

				ctx.save_rs(dev, D3DRS_DESTBLEND);
				dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

				ctx.save_rs(dev, D3DRS_ZWRITEENABLE);
				dev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

				ctx.save_rs(dev, D3DRS_ZENABLE);
				dev->SetRenderState(D3DRS_ZENABLE, FALSE);

				//set_remix_texture_categories(dev, InstanceCategories::WorldMatte | InstanceCategories::IgnoreOpacityMicromap);
			}

			if (ctx.modifiers.dual_render_mode_blend_diffuse)
			{
				ctx.save_rs(dev, D3DRS_ALPHABLENDENABLE);
				dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

				//ctx.save_rs(dev, D3DRS_BLENDOP);
				//dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

				ctx.save_rs(dev, D3DRS_SRCBLEND);
				dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

				ctx.save_rs(dev, D3DRS_DESTBLEND);
				dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

				ctx.save_tss(dev, D3DTSS_COLOROP);
				dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

				ctx.save_tss(dev, D3DTSS_COLORARG1);
				dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

				//ctx.save_tss(dev, D3DTSS_COLORARG2);
				//dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

				ctx.save_tss(dev, D3DTSS_ALPHAOP);
				dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

				ctx.save_tss(dev, D3DTSS_ALPHAARG1);
				dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

				//ctx.save_tss(dev, D3DTSS_ALPHAARG2);
				//dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			}

			// re-draw surface
			dev->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount-1);

			if (ctx.modifiers.dual_render_texture) {
				ctx.restore_texture(dev, 0);
			}
		}


		// ---------
		// post draw

		ctx.restore_all(dev);
		ctx.reset_context();
		
		return hr;
	}


	HRESULT renderer::on_draw_indexed_prim_up(IDirect3DDevice9* dev, D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
	{
		// Wait for the first rendered prim before further init of the comp mod 
		if (!g_rendered_first_primitive) {
			g_rendered_first_primitive = true;
		}

		if (!is_initialized() || shared::globals::imgui_is_rendering) {
			return dev->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
		}

		auto& ctx = setup_context(dev);
		const auto im = imgui::get();

		// use any logic to conditionally set this to disable the vertex shader and use fixed function fallback
		bool render_with_ff = false;

		// any drawcall modifications in here
		if (!shared::globals::imgui_is_rendering)
		{
			im->m_stats._drawcall_indexed_prim_up_incl_ignored.track_single();

			// if we set do_not_render somewhere before the actual drawcall -> do not draw and reset context
			if (ctx.modifiers.do_not_render)
			{
				ctx.restore_all(dev);
				ctx.reset_context();
				return S_OK;
			}

			if (im->m_dbg_force_ff_indexed_prim_up) {
				render_with_ff = true;
			}

			// use fixed function fallback if true
			if (render_with_ff)
			{
				ctx.save_vs(dev);
				dev->SetVertexShader(nullptr);
			}
		} // end !imgui-is-rendering


		// ---------
		// draw

		auto hr = S_OK;

		// do not render next surface if set
		if (!ctx.modifiers.do_not_render && !im->m_dbg_disable_indexed_prim_up_draw)
		{
			hr = dev->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);

			if (!shared::globals::imgui_is_rendering) {
				im->m_stats._drawcall_indexed_prim_up.track_single();
			}
		}


		// ---------
		// post draw

		ctx.restore_all(dev);
		ctx.reset_context();

		return hr;
	}

	// ---

	// This can be used to manually trigger remix injection without ever needing to manually tag HUD textures
	// Can help if its hard to tag UI because it might be constantly changing - or if there is no UI
	// Call this on the first UI drawcall (you obv. need to detect that on your own via a hook or something)

	void renderer::manually_trigger_remix_injection(IDirect3DDevice9* dev)
	{
		//if (!game::is_in_game) {
		//	return;
		//}

		if (!m_triggered_remix_injection)
		{
			auto& ctx = dc_ctx;

			dev->SetRenderState(D3DRS_FOGENABLE, FALSE);

			ctx.save_vs(dev);
			dev->SetVertexShader(nullptr);
			ctx.save_ps(dev);
			dev->SetPixelShader(nullptr); // needed

			ctx.save_rs(dev, D3DRS_ZWRITEENABLE);
			dev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE); // required - const bool zWriteEnabled = d3d9State().renderStates[D3DRS_ZWRITEENABLE]; -> if (isOrthographic && !zWriteEnabled)

			struct CUSTOMVERTEX
			{
				float x, y, z, rhw;
				D3DCOLOR color;
			};

			const auto color = D3DCOLOR_COLORVALUE(0, 0, 0, 0);
			const auto w = -0.49f;
			const auto h = -0.495f;

			CUSTOMVERTEX vertices[] =
			{
				{ -0.5f, -0.5f, 0.0f, 1.0f, color }, // tl
				{     w, -0.5f, 0.0f, 1.0f, color }, // tr
				{ -0.5f,     h, 0.0f, 1.0f, color }, // bl
				{     w,     h, 0.0f, 1.0f, color }  // br
			};

			dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(CUSTOMVERTEX));

			ctx.restore_vs(dev);
			ctx.restore_ps(dev);
			ctx.restore_render_state(dev, D3DRS_ZWRITEENABLE);
			m_triggered_remix_injection = true;
		}
	}


	// ---


	__declspec (naked) void pre_draw_particle_stub()
	{
		static uint32_t retn_addr = 0x72EA85;
		__asm
		{
			mov		g_is_rendering_particle, 1;
			call    dword ptr[ecx + 0x10C]; // og
			jmp		retn_addr;
		}
	}

	__declspec (naked) void post_draw_particle_stub()
	{
		static uint32_t retn_addr = 0x72EB92;
		static uint32_t og_fn_addr = 0x7D2470;
		__asm
		{
			mov		g_is_rendering_particle, 0;
			call    og_fn_addr; // og
			jmp		retn_addr;
		}
	}

	//

	__declspec (naked) void pre_draw_car_stub()
	{
		static uint32_t retn_addr = 0x7E1166;
		__asm
		{
			mov		g_is_rendering_car, 1;
			mov     ebp, esp;			// og
			and		esp, 0xFFFFFFF0;	// og
			jmp		retn_addr;
		}
	}

	__declspec (naked) void post_draw_car_stub()
	{
		__asm
		{
			mov		g_is_rendering_car, 0;
			pop     esi;
			pop     ebx;
			mov     esp, ebp;
			pop     ebp;
			retn    8;
		}
	}

	//

	__declspec (naked) void pre_draw_car_stub2()
	{
		static uint32_t retn_addr = 0x7E0F06;
		__asm
		{
			mov		g_is_rendering_car, 1;
			mov     ebp, esp;		 // og
			and		esp, 0xFFFFFFF0; // og
			jmp		retn_addr;
		}
	}

	__declspec (naked) void post_draw_car_stub2()
	{
		__asm
		{
			mov		g_is_rendering_car, 0;
			pop     ebx;
			mov     esp, ebp;
			pop     ebp;
			retn;
		}
	}

	// --

	void post_commit_changes(ID3DXEffect* effect)
	{
		g_is_rendering_world = 0;
		g_is_rendering_world_normalmap = 0;
		g_is_rendering_car = 0;
		g_is_rendering_car_normalmap = 0;
		g_is_rendering_glass_reflect = 0;
		g_is_rendering_sky = 0;

		if (effect)
		{
			D3DXEFFECT_DESC d = {};
			effect->GetDesc(&d);

			if (auto htech = effect->GetTechnique(0); htech)
			{
				D3DXTECHNIQUE_DESC tech_desc;
				effect->GetTechniqueDesc(htech, &tech_desc);

				if (std::string_view(tech_desc.Name) == "world") {
					g_is_rendering_world = 1;
				}
				else if (std::string_view(tech_desc.Name) == "worldnormalmap") {
					g_is_rendering_world_normalmap = 1;
				}
				else if (std::string_view(tech_desc.Name) == "car") {
					g_is_rendering_car = 1;
				}
				else if (std::string_view(tech_desc.Name) == "car_normalmap") {
					g_is_rendering_car_normalmap = 1;
				}
				else if (std::string_view(tech_desc.Name) == "glassreflect") {
					g_is_rendering_glass_reflect = 1;
				}
				else if (std::string_view(tech_desc.Name) == "sky") {
					g_is_rendering_sky = 1;
				} else if (std::string_view(tech_desc.Name) == "dryroad") {
					g_is_rendering_dry_road = 1;
				} else if (std::string_view(tech_desc.Name) == "water") {
					g_is_rendering_water = 1;
				}
				else 
				{
					int x = 1;
				}

				if (g_is_rendering_car)
				{

//   cavHarmonicCoeff   c0      10
//   WorldViewProj      c10      4
//   cmWorldView        c14      3
//   cvFogValue         c17      1
//   cfFogEnable        c18      1
//   cvLocalEyePos      c19      1
//   cfEnvmapPullAmount c20      1
//   cvDiffuseMin       c21      1
//   cvDiffuseRange     c22      1
//   cvEnvmapMin        c23      1
//   cvEnvmapRange      c24      1
//   cvPowers           c25      1
//   cvClampAndScales   c26      1
//   cvFlakes           c27      1

					if (tech_desc.Passes > 0)
					{
						auto& ctx = renderer::dc_ctx;

						auto second_pass = effect->GetPass(htech, 0);
						if (second_pass)
						{
							D3DXPASS_DESC pass_desc;
							effect->GetPassDesc(second_pass, &pass_desc);

							//effect->GetParameterByName()
							//D3DXPARAMETER_DESC param_desc;
							//effect->GetParameterDesc(d, &param_desc);

							for (UINT i = 0; i < d.Parameters; ++i)
							{
								D3DXHANDLE param = effect->GetParameter(nullptr, i);

								D3DXPARAMETER_DESC desc = {};
								effect->GetParameterDesc(param, &desc);

								if (desc.Class == D3DXPC_VECTOR && desc.Type == D3DXPT_FLOAT)
								{
									Vector value = {};
									effect->GetValue(param, &value.x, sizeof(value));
									int asd = 0;
								}
							}

							D3DXHANDLE cvDiffuseMin = effect->GetParameterByName(nullptr, "cvDiffuseMin");
							if (cvDiffuseMin)
							{
								D3DXPARAMETER_DESC desc = {};
								effect->GetParameterDesc(cvDiffuseMin, &desc);

								if (desc.Class == D3DXPC_VECTOR && desc.Type == D3DXPT_FLOAT)
								{
									Vector value = {};
									effect->GetValue(cvDiffuseMin, &value.x, sizeof(value));
									ctx.info.cvDiffuseMin = value;
									ctx.info.cvDiffuseMin.x = 1.0f;
								}

								int y = 0;
							}

							D3DXHANDLE cvDiffuseRange = effect->GetParameterByName(nullptr, "cvDiffuseRange");
							if (cvDiffuseRange)
							{
								D3DXPARAMETER_DESC desc = {};
								effect->GetParameterDesc(cvDiffuseRange, &desc);

								if (desc.Class == D3DXPC_VECTOR && desc.Type == D3DXPT_FLOAT)
								{
									Vector value = {};
									effect->GetValue(cvDiffuseRange, &value.x, sizeof(value));
									ctx.info.cvDiffuseRange = value;
								}

								int y = 0;
							}
						}
					}
				}
				

				int z = 0;
			}
		}
		
		int x = 1;
	}

	ID3DXEffect* g_curr_effect_ptr = nullptr;

	__declspec (naked) void pre_effect_commit_changes()
	{
		static uint32_t retn_addr = 0x71EE24;
		__asm
		{
			mov     edi, [edi + 0x44];
			mov     eax, [edi];
			push    edi;
			call    dword ptr[eax + 0x104]; // CommitChanges

			pushad;
			push	edi; //ID3DXBaseEffect *
			call	post_commit_changes;
			add		esp, 4;
			popad;

			jmp		retn_addr;
		}
	}

	// ---

	void on_handle_material_data(game::material_instance* data)
	{
		if (data) {
			g_current_material_data = *data;
		}
	}

	__declspec (naked) void on_handle_material_data_stub()
	{
		static uint32_t retn_addr = 0x71E06B;
		__asm
		{
			pushad;
			push	esi;
			call	on_handle_material_data;
			add		esp, 4;
			popad;

			mov		[edi + 0x1780], esi;
			jmp		retn_addr;
		}
	}

	renderer::renderer()
	{
		p_this = this;

		// always set view and proj transforms even when in shader mode
		shared::utils::hook::nop(0x71E736, 2);
		shared::utils::hook::nop(0x71E6ED, 2);

		// always set world transform
		shared::utils::hook::nop(0x71E82F, 2);
		shared::utils::hook::nop(0x71E845, 2); // still set shader matrices (normally this or that)
		
		shared::utils::hook::nop(0x72EA7F, 6);
		shared::utils::hook(0x72EA7F, pre_draw_particle_stub, HOOK_JUMP).install()->quick(); // retn to 72EA85
		shared::utils::hook(0x72EB8D, post_draw_particle_stub, HOOK_JUMP).install()->quick(); // retn to 72EB92 .. call 7D2470 in stub

		// --

		// for FF
/*		shared::utils::hook(0x7E1161, pre_draw_car_stub, HOOK_JUMP).install()->quick(); // retn to 7E1166
		shared::utils::hook(0x7E1698, post_draw_car_stub, HOOK_JUMP).install()->quick();

		// for Shader
		shared::utils::hook(0x7E0F01, pre_draw_car_stub2, HOOK_JUMP).install()->quick(); // retn to 7E0F06
		shared::utils::hook(0x7E10D5, post_draw_car_stub2, HOOK_JUMP).install()->quick();*/

		// ---

		// 71EE18
		shared::utils::hook(0x71EE18, pre_effect_commit_changes, HOOK_JUMP).install()->quick();

		shared::utils::hook::nop(0x71E065, 6);
		shared::utils::hook(0x71E065, on_handle_material_data_stub, HOOK_JUMP).install()->quick();
		// 71E065

		//shared::utils::hook(game::retn_addr__pre_draw_something - 5u, pre_render_something_stub, HOOK_JUMP).install()->quick();
		//shared::utils::hook(game::hk_addr__post_draw_something, post_render__something_stub, HOOK_JUMP).install()->quick();


		// -----
		m_initialized = true;
		shared::common::log("Renderer", "Module initialized.", shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
	}

	renderer::~renderer()
	{
		tex_addons::init_texture_addons(true);
	}
}
