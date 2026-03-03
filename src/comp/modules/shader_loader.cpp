/*	MIT License
	Copyright (c) 2022 Lovro Plese (https://github.com/xan1242/xnfs-shaderloader-carbon/tree/master)
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE. */

	// Modified original code to fit codebase

#include "std_include.hpp"
#include "shader_loader.hpp"

#include "shared/common/flags.hpp"

namespace comp
{
	bool check_if_file_exists(char* file_name)
	{
		auto fin = fopen(file_name, "rb");
		if (fin == nullptr)
		{
			char secondary_path[MAX_PATH];
			strcpy(secondary_path, "fx\\");
			strcat(secondary_path, file_name);
			
			if (fin = fopen(secondary_path, "rb"); fin == nullptr) {
				return false;
			}
				
			strcpy(file_name, secondary_path);
		}

		fclose(fin);
		return true;
	}

	ID3DXEffectCompiler* pEffectCompiler;
	ID3DXBuffer* pBuffer, *pEffectBuffer;

	char file_name_buff[2048];
	unsigned int g_current_shader_num = 0u;

	HRESULT WINAPI HookedD3DXCreateEffectFromResourceA_hk(LPDIRECT3DDEVICE9 pDevice, HMODULE hSrcModule, LPCSTR pSrcResource, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors)
	{
		const char* fx_file_path = game::shader_fx_path_array[g_current_shader_num / 4];
		strcpy(file_name_buff, fx_file_path);

		// modify ext of string
		char* ext = strrchr(file_name_buff, '.');
		ext[1] = 'f'; ext[2] = 'x'; ext[3] = '\0';

		if (check_if_file_exists(file_name_buff))
		{
			auto hr = D3DXCreateEffectCompilerFromFileA(file_name_buff, nullptr, nullptr, 0, &pEffectCompiler, &pBuffer);
			if (SUCCEEDED(hr))
			{
				shared::common::log("ShaderLoader", std::format("Compiling shader {}", file_name_buff), shared::common::LOG_TYPE::LOG_TYPE_STATUS, true);
				hr = pEffectCompiler->CompileEffect(0, &pEffectBuffer, &pBuffer);
				if (!SUCCEEDED(hr))
				{
					shared::common::log("ShaderLoader", std::format("{}", *(char**)(pBuffer + 3)), shared::common::LOG_TYPE::LOG_TYPE_ERROR, true);
					shared::common::log("ShaderLoader", std::format("HRESULT: {:X}", hr), shared::common::LOG_TYPE::LOG_TYPE_ERROR, true);
					return hr;
				}
				shared::common::log("ShaderLoader", "Compilation successful!", shared::common::LOG_TYPE::LOG_TYPE_GREEN, false);

				hr = D3DXCreateEffect(pDevice, *(void**)(pEffectBuffer + 3), *(unsigned int*)(pEffectBuffer + 2), pDefines, pInclude, Flags, pPool, ppEffect, &pBuffer);
				if (!SUCCEEDED(hr))
				{
					shared::common::log("ShaderLoader", std::format("Effect creation failed: HRESULT: {:X}", hr), shared::common::LOG_TYPE::LOG_TYPE_ERROR, true);
					shared::common::log("ShaderLoader", std::format("{}", *(char**)(pBuffer + 3)), shared::common::LOG_TYPE::LOG_TYPE_ERROR, true);
				}
				return hr;
			}
			else
			{
				shared::common::log("ShaderLoader", std::format("Error compiling shader: HRESULT: {:X}", hr), shared::common::LOG_TYPE::LOG_TYPE_ERROR, true);
				shared::common::log("ShaderLoader", std::format("{}", *(char**)(pBuffer + 3)), shared::common::LOG_TYPE::LOG_TYPE_ERROR, true);
			}
		}

		if (check_if_file_exists((char*)pSrcResource)) {
			return D3DXCreateEffectFromFileA(pDevice, pSrcResource, pDefines, pInclude, Flags, pPool, ppEffect, ppCompilationErrors);
		}

		return D3DXCreateEffectFromResourceA(pDevice, hSrcModule, pSrcResource, pDefines, pInclude, Flags, pPool, ppEffect, ppCompilationErrors);
	}

	__declspec (naked) void load_effect_from_input_stub()
	{
		__asm
		{
			push    0;
			shl     edx, 4;
			mov		g_current_shader_num, edx;
			jmp		game::retn_addr__load_effect_from_input; // 0x72B6CA
		}
	}

	void disable_original_shader_loader()
	{
		std::filesystem::path file_path = std::filesystem::path(shared::globals::root_path) / "plugins" / "XNFS-ShaderLoader-Carbon.asi";

		if (std::filesystem::exists(file_path)) 
		{
			std::filesystem::rename(file_path, file_path.string() + ".OFF");
			shared::common::log("Shader Loader", "Found original shader loader. Disabled by renaming it to 'XNFS-ShaderLoader-Carbon.asi.OFF'", shared::common::LOG_TYPE::LOG_TYPE_WARN, true);
		}
	}

	shader_loader::shader_loader()
	{
		p_this = this;

		if (!shared::common::flags::has_flag("disable_shaderloader"))
		{
			disable_original_shader_loader();

			shared::utils::hook(game::retn_addr__load_effect_from_input - 5u, load_effect_from_input_stub, HOOK_JUMP).install()->quick(); // 0x72B6C5
			shared::utils::hook(game::call_addr__d3dx_create_effect_from_resource, HookedD3DXCreateEffectFromResourceA_hk, HOOK_CALL).install()->quick(); // 0x72B6D4

			// -----
			m_initialized = true;
			shared::common::log("Shader Loader", "Module initialized.", shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
		}
		else {
			shared::common::log("Shader Loader", "Module NOT loaded due to commandline flag.", shared::common::LOG_TYPE::LOG_TYPE_WARN, true);
		}
	}
}
