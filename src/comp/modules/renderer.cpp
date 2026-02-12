#include "std_include.hpp"
#include "renderer.hpp"

#include "imgui.hpp"

namespace comp
{
	int g_is_rendering_something = 0;
	bool g_rendered_first_primitive = false;
	bool g_applied_hud_hack = false; // was hud "injection" applied this frame

	namespace tex_addons
	{
		bool initialized = false;
		LPDIRECT3DTEXTURE9 berry = nullptr;

		void init_texture_addons(bool release)
		{
			if (release)
			{
				if (tex_addons::berry) tex_addons::berry->Release();
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

		static auto im = imgui::get();
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
		if (!ctx.modifiers.do_not_render) 
		{
			hr = dev->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
			im->m_stats._drawcall_prim.track_single();

			if (!render_with_ff) {
				im->m_stats._drawcall_using_vs.track_single();
			}
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


			// --- 
			// The following code is here for example purposes and does not function on its own.


			// uncomment and debug into this to see vertex format of current drawcall
				// shared::utils::lookat_vertex_decl(dev);


			// Eg: render with fixed function if 'g_is_rendering_something' is true 
				//render_with_ff = g_is_rendering_something;


			// Eg: some condition to not render the next drawcall
				//if (your_condition) {
				//	ctx.modifiers.do_not_render = true;
				//}


			// If going from shader to fixed function, you'll need at least the world transformation matrix of the current mesh (given that you set the view and projection matrices in comp::on_begin_scene_cb())
			// If you found or figured out a memory address of a structure or matrices and want to render something via FF:
				//dev->SetTransform(D3DTS_WORLD, game::current_mesh_world_transform_matrix);


			// Some games might also modify the view/projection for certain meshes (eg. first person objects), so its not a bad idea to reset the view and proj matrices every time
			// Eg: If you found a structure in memory and setup the offset and the struct:
				//if (const auto viewport = game::vp; viewport)
				//{
				//	dev->SetTransform(D3DTS_VIEW, &viewport->view);
				//	dev->SetTransform(D3DTS_PROJECTION, &viewport->proj);
				//}


			// Identified some problematic drawcall with eg. blending issues?
			// Modify Renderstates but make sure to save them before doing so to not affect later drawcalls
				/*if (your_condition) 
				{
					ctx.save_rs(dev, D3DRS_ALPHABLENDENABLE);
					dev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

					ctx.save_rs(dev, D3DRS_BLENDOP);
					dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

					ctx.save_rs(dev, D3DRS_SRCBLEND);
					dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

					ctx.save_rs(dev, D3DRS_DESTBLEND);
					dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

					ctx.save_tss(dev, D3DTSS_COLOROP);
					dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

					ctx.save_tss(dev, D3DTSS_COLORARG1);
					dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

					ctx.save_tss(dev, D3DTSS_ALPHAOP);
					dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

					ctx.save_tss(dev, D3DTSS_ALPHAARG1);
					dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

					ctx.save_tss(dev, D3DTSS_ALPHAARG2);
					dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

					ctx.save_rs(dev, D3DRS_TEXTUREFACTOR);
					dev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(0, 0, 0, 1.0f)); 
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
		if (!ctx.modifiers.do_not_render) 
		{
			hr = dev->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);

			if (!shared::globals::imgui_is_rendering) {
				im->m_stats._drawcall_indexed_prim.track_single();
			}

			if (!render_with_ff) {
				im->m_stats._drawcall_indexed_prim_using_vs.track_single();
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

	// assembly stub at the start of a function that is eg. about to issue a bunch of drawcalls for a certain type of mesh
	/*__declspec (naked) void pre_render_something_stub()
	{
		__asm
		{
			mov     ebx, ecx;			// example; this might be an instruction that was overwritten by the hook
			cmp     eax, 0xFFFFFFFF;	// ^; remember that placing a hook takes 5 bytes so make sure to restore what was overwritten

			mov		g_is_rendering_something, 1;	// global to let us know that every drawcall afterwards is 'something' special we want to handle
			jmp		game::retn_addr__pre_draw_something;
		}
	}*/

	// assembly stub at the end of the same function to reset the global var
	/*__declspec (naked) void post_render__something_stub()
	{
		__asm
		{
			mov		g_is_rendering_something, 0;
			retn    0x10;	// eg: this hook was placed on the return instruction, replicate it here
		}
	}*/

	// ---

	renderer::renderer()
	{
		p_this = this;

		// #Step 5: Create hooks as required

		// Eg: detect rendering of some special kind of mesh because the function we hook is issuing a bunch of drawcalls for a certain type of mesh we want modify
		// START OF FUNC: set global helper bool
		// - Every drawcall from here on will be our special type of mesh
		// END OF FUNC: reset global helper bool

		// - retn_addr__pre_draw_something contains the offset we want to return to after our assembly stub, which is mostly the instruction after our hook.
		//   If the instruction at the hook spot is exactly 5 bytes, we can place the hook there and use the return addr minus 5 bytes to retrive the addr of the hook.

		// - hk_addr__post_draw_something contains the direct addr that we want to place the hook at
		//   This example will place a stub on the retn instruction which has additional padding bytes until the next function starts (so more than 5 bytes of space).
		//   That way we do not need an addr to return to and can just replicate the retn instruction in the stub

			//shared::utils::hook(game::retn_addr__pre_draw_something - 5u, pre_render_something_stub, HOOK_JUMP).install()->quick();
			//shared::utils::hook(game::hk_addr__post_draw_something, post_render__something_stub, HOOK_JUMP).install()->quick();


		// Eg: if you only want to modify things based on commandline flags
			//if (shared::common::flags::has_flag("your_flag"))
			//{
			//	// any hooks or mem edits here, eg:
			//
			//	// if you want to place a hook but cant find easy 5 bytes of space, you can nop eg. 7 bytes first, then place the hook
			//	// make sure to replicate the overwritten instructions in your stub if you do so
			//		//shared::utils::hook::nop(game::nop_addr__func2, 7); // nop 7 bytes at addr
			//		//shared::utils::hook(game::nop_addr__func2, your_stub, HOOK_JUMP).install()->quick(); // we can now safely place a hook here without messing up following instructions
			//}


		// -----
		m_initialized = true;
		shared::common::log("Renderer", "Module initialized.", shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
	}

	renderer::~renderer()
	{
		tex_addons::init_texture_addons(true);
	}
}
