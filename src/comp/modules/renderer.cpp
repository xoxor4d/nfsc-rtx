#include "std_include.hpp"
#include "renderer.hpp"

#include "imgui.hpp"

namespace comp
{
	int g_is_rendering_particle = 0;
	int g_is_rendering_car = 0;
	int g_is_rendering_car_normalmap = 0;
	int g_is_rendering_world = 0;

	int g_is_rendering_world_normalmap = 0;
	int g_is_rendering_glass_reflect = 0;
	int g_is_rendering_sky = 0;

	bool g_rendered_first_primitive = false;
	bool g_applied_hud_hack = false; // was hud "injection" applied this frame

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
			}

			if (g_is_rendering_car)
			{
				/*if (im->m_dbg_debug_bool03)
				{
					ctx.save_texture(dev, 0);
					dev->SetTexture(0, tex_addons::red);
				}

				if (im->m_dbg_debug_bool02) 
				{
					ctx.save_texture(dev, 0);

					IDirect3DBaseTexture9* base_tex = nullptr;
					dev->GetTexture(1, &base_tex);
					dev->SetTexture(0, base_tex);
				}*/

				//if (!im->m_dbg_debug_bool03)
				{
					if (ctx.info.cvDiffuseMin.x == 1.0f)
					{
						if (im->m_dbg_debug_bool01) {
							ctx.modifiers.do_not_render = true;
						}

						Vector4D v4_cvDiffuseMin;
						Vector4D v4_cvDiffuseRange;
						Vector4D v4_cvPowers;
						Vector4D v4_cvClampAndScales;

						dev->GetVertexShaderConstantF(21, &v4_cvDiffuseMin.x, 1);
						Vector cvDiffuseMin = v4_cvDiffuseMin;

						dev->GetVertexShaderConstantF(22, &v4_cvDiffuseRange.x, 1);
						Vector cvDiffuseRange = v4_cvDiffuseRange;

						dev->GetVertexShaderConstantF(25, &v4_cvPowers.x, 1);
						dev->GetVertexShaderConstantF(26, &v4_cvClampAndScales.x, 1);

						// NOW: -0.810000 im->m_debug_vector.x fixes vinyls but breaks car paint
						float r0w = std::min(1.0f + im->m_debug_vector.x, v4_cvClampAndScales.x);
						float r1w = r0w * v4_cvClampAndScales.z * (1.0f + im->m_debug_vector.y);

						Vector col;

						if (im->m_dbg_debug_bool04) {
							col = cvDiffuseRange + cvDiffuseMin;
						}
						else {
							col = r1w * cvDiffuseRange + cvDiffuseMin;
						}

						if (im->m_dbg_debug_bool05)
						{
							float vdotn_diffuse = pow(im->m_debug_vector.x, im->m_vis_cvPowers.x);

							if (im->m_dbg_debug_bool06)
							{
								vdotn_diffuse = std::min(vdotn_diffuse, v4_cvClampAndScales.x);
								vdotn_diffuse = vdotn_diffuse * v4_cvClampAndScales.z;
							}

							col = cvDiffuseMin + vdotn_diffuse * cvDiffuseRange;
						}

						bool was_vis = false;
						if (!im->m_vis_drawcall01)
						{
							im->m_vis_drawcall01 = true;
							im->m_vis_cvDiffuseMin = v4_cvDiffuseMin;
							im->m_vis_cvDiffuseRange = v4_cvDiffuseRange;
							im->m_vis_cvPowers = v4_cvPowers;
							im->m_vis_cvClampAndScales = v4_cvClampAndScales;
							im->m_vis_paint_color = col;
							was_vis = true;
						}

						ctx.save_rs(dev, D3DRS_TEXTUREFACTOR); // prob. not needed
						ctx.save_tss(dev, D3DTSS_COLOROP);
						ctx.save_tss(dev, D3DTSS_COLORARG1);
						ctx.save_tss(dev, D3DTSS_COLORARG2);

						if (im->m_dbg_debug_bool07) {
							col.Normalize();
						} 
						else 
						{
							col.x = std::clamp(col.x, 0.0f, 1.0f);
							col.y = std::clamp(col.y, 0.0f, 1.0f);
							col.z = std::clamp(col.z, 0.0f, 1.0f);
						}
						
						//col *= im->m_debug_vector.z;

						if (was_vis) {
							im->m_vis_paint_color_post = col;
						}

						dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(col.x, col.y, col.z, 1.0f));

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
