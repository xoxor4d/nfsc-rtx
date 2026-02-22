#pragma once
#include <unordered_map>
#include <vector>
#include <string>

namespace comp::effects
{
	// ---------------------------------------------------------------------------
	// Technique enum  (string names come from D3DXTechniqueDesc::Name)
	// ---------------------------------------------------------------------------
	enum class ETECH : int
	{
		UNKNOWN											= -1,
		WORLD											=  0,
		LOWLOD											=  1,
		WORLD_1_1										=  2,
		DRYROAD											=  3,
		RAINING_ON_ROAD									=  4,
		WORLD_FIXED										=  5,
		CAR												=  6,
		WORLD_MIN										=  7,
		FILTER											=  8,
		VISUALTREATMENT									=  9,
		VISUALTREATMENT_ENCHANCED						= 10,  // note: original spelling
		MOTIONBLUR										= 11,
		COMPOSITE_BLUR									= 12,
		UVESOVERCLIFF									= 13,
		UVESOVERCLIFFDARKEN								= 14,
		SCREEN_PASSTHRU									= 15,
		FUZZZ											= 16,
		NO_FUZZZ										= 17,
		STREAK_FLARES									= 18,
		FLARES											= 19,
		SKY												= 20,
		SKINNED											= 21,
		DEPTH_TECHNIQUE_NOALPHA							= 22,
		TSHADER_INSTANCING								= 23,
		TWOPASSBLUR										= 24,
		GAUSSBLUR5X5									= 25,
		DOWNSCALE4X4									= 26,
		DOWNSCALE2X2									= 27,
		DOWNSCALE2X2_FOR_MOTIONBLUR						= 28,
		DOWNSCALE_FOR_BLOOM								= 29,
		BLEND_TEXTURES									= 30,
		COMBINE_REFLECTION_COLOUR_AND_HEADLIGHT_ALPHA	= 31,
		FINALHDRPASS									= 32,
		WORLD_MASKED									= 33,
		YUVMOVIE										= 34,
		SCREEN_PASSTHRU_ALPHA_TAG						= 35,
		DOWNSCALE4X4_T0_UVES_HDR						= 36,
		BLOOM											= 37,
		BLUR											= 38,
		BRIGHTPASS										= 39,
		CALCULATE_LUMINANCE								= 40,
		CALCULATE_ADAPTATION							= 41,
		ATTENUATE										= 42,
		DEBUG_SHOW_RED_CHANNEL							= 43,
		MAIN											= 44,
		MAIN_1_1										= 45,
		GLASSREFLECT									= 46,
		WATER											= 47,
		RVM												= 48,
		PIP												= 49,
		GHOSTCAR										= 50,
	};

	// All unique technique names seen across all wrapped effects 
	// (populated at effect-creation time, logged to the console once).
	inline std::vector<std::string>              g_discovered_techniques;

	// Fast name -> enum lookup used inside SetTechnique (O(1) unordered lookup).
	// Populate this map as soon as the enum above is extended.
	inline std::unordered_map<std::string, ETECH> g_tech_name_to_enum;

	// Current active technique – updated every SetTechnique call.
	inline ETECH        g_current_tech      = ETECH::UNKNOWN;
	inline std::string  g_current_tech_name;

	// Public accessors
	inline ETECH              get_current_tech()      { return g_current_tech; }
	inline const std::string& get_current_tech_name() { return g_current_tech_name; }
}

namespace comp
{
	class d3dxeffects final : public shared::common::loader::component_module
	{
	public:
		d3dxeffects();

		// function to create wrapped effects
		static ID3DXEffect* CreateWrappedEffect(ID3DXEffect* pOriginal);

	private:
		class D3DXEffect : public ID3DXEffect
		{
			friend class d3dxeffects;
		public:
			D3DXEffect(ID3DXEffect* pOriginal) : m_pID3DXEffect(pOriginal)
		{
			// Enumerate all techniques in this effect and register any new ones
			D3DXEFFECT_DESC desc{};
			if (SUCCEEDED(pOriginal->GetDesc(&desc)))
			{
				for (UINT i = 0; i < desc.Techniques; i++)
				{
					D3DXHANDLE hTech = pOriginal->GetTechnique(i);
					if (!hTech) continue;

					D3DXTECHNIQUE_DESC td{};
					if (SUCCEEDED(pOriginal->GetTechniqueDesc(hTech, &td)) && td.Name)
					{
						const std::string name(td.Name);
						const bool is_new = std::find(
							effects::g_discovered_techniques.begin(),
							effects::g_discovered_techniques.end(), name)
							== effects::g_discovered_techniques.end();

						if (is_new)
						{
							effects::g_discovered_techniques.push_back(name);
							shared::common::log("d3dxeffects",
								std::format("[technique] '{}'", name),
								shared::common::LOG_TYPE::LOG_TYPE_STATUS, false);
						}
					}
				}
			}
		};
			virtual ~D3DXEffect() {};

			// IUnknown methods
			HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObj);
			ULONG   __stdcall AddRef();
			ULONG   __stdcall Release();

			// ID3DXBaseEffect methods
			HRESULT __stdcall GetDevice(IDirect3DDevice9** ppDevice);
			HRESULT __stdcall GetDesc(D3DXEFFECT_DESC* pDesc);
			HRESULT __stdcall GetParameterDesc(D3DXHANDLE hParameter, D3DXPARAMETER_DESC* pDesc);
			HRESULT __stdcall GetTechniqueDesc(D3DXHANDLE hTechnique, D3DXTECHNIQUE_DESC* pDesc);
			HRESULT __stdcall GetPassDesc(D3DXHANDLE hPass, D3DXPASS_DESC* pDesc);
			HRESULT __stdcall GetFunctionDesc(D3DXHANDLE hShader, D3DXFUNCTION_DESC* pDesc);
			D3DXHANDLE __stdcall GetParameter(D3DXHANDLE hParameter, UINT Index);
			D3DXHANDLE __stdcall GetParameterByName(D3DXHANDLE hParameter, LPCSTR pName);
			D3DXHANDLE __stdcall GetParameterBySemantic(D3DXHANDLE hParameter, LPCSTR pSemantic);
			D3DXHANDLE __stdcall GetTechnique(UINT Index);
			D3DXHANDLE __stdcall GetTechniqueByName(LPCSTR pName);
			D3DXHANDLE __stdcall GetPass(D3DXHANDLE hTechnique, UINT Index);
			D3DXHANDLE __stdcall GetPassByName(D3DXHANDLE hTechnique, LPCSTR pName);
			D3DXHANDLE __stdcall GetFunction(UINT Index);
			D3DXHANDLE __stdcall GetFunctionByName(LPCSTR pName);
			D3DXHANDLE __stdcall GetAnnotation(D3DXHANDLE hObject, UINT Index);
			D3DXHANDLE __stdcall GetAnnotationByName(D3DXHANDLE hObject, LPCSTR pName);
			HRESULT __stdcall SetValue(D3DXHANDLE hParameter, LPCVOID pData, UINT Bytes);
			HRESULT __stdcall GetValue(D3DXHANDLE hParameter, LPVOID pData, UINT Bytes);
			HRESULT __stdcall SetBool(D3DXHANDLE hParameter, BOOL b);
			HRESULT __stdcall GetBool(D3DXHANDLE hParameter, BOOL* pb);
			HRESULT __stdcall SetBoolArray(D3DXHANDLE hParameter, CONST BOOL* pb, UINT Count);
			HRESULT __stdcall GetBoolArray(D3DXHANDLE hParameter, BOOL* pb, UINT Count);
			HRESULT __stdcall SetInt(D3DXHANDLE hParameter, INT n);
			HRESULT __stdcall GetInt(D3DXHANDLE hParameter, INT* pn);
			HRESULT __stdcall SetIntArray(D3DXHANDLE hParameter, CONST INT* pn, UINT Count);
			HRESULT __stdcall GetIntArray(D3DXHANDLE hParameter, INT* pn, UINT Count);
			HRESULT __stdcall SetFloat(D3DXHANDLE hParameter, FLOAT f);
			HRESULT __stdcall GetFloat(D3DXHANDLE hParameter, FLOAT* pf);
			HRESULT __stdcall SetFloatArray(D3DXHANDLE hParameter, CONST FLOAT* pf, UINT Count);
			HRESULT __stdcall GetFloatArray(D3DXHANDLE hParameter, FLOAT* pf, UINT Count);
			HRESULT __stdcall SetVector(D3DXHANDLE hParameter, CONST D3DXVECTOR4* pVector);
			HRESULT __stdcall GetVector(D3DXHANDLE hParameter, D3DXVECTOR4* pVector);
			HRESULT __stdcall SetVectorArray(D3DXHANDLE hParameter, CONST D3DXVECTOR4* pVector, UINT Count);
			HRESULT __stdcall GetVectorArray(D3DXHANDLE hParameter, D3DXVECTOR4* pVector, UINT Count);
			HRESULT __stdcall SetMatrix(D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix);
			HRESULT __stdcall GetMatrix(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix);
			HRESULT __stdcall SetMatrixArray(D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix, UINT Count);
			HRESULT __stdcall GetMatrixArray(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count);
			HRESULT __stdcall SetMatrixPointerArray(D3DXHANDLE hParameter, CONST D3DXMATRIX** ppMatrix, UINT Count);
			HRESULT __stdcall GetMatrixPointerArray(D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count);
			HRESULT __stdcall SetMatrixTranspose(D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix);
			HRESULT __stdcall GetMatrixTranspose(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix);
			HRESULT __stdcall SetMatrixTransposeArray(D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix, UINT Count);
			HRESULT __stdcall GetMatrixTransposeArray(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count);
			HRESULT __stdcall SetMatrixTransposePointerArray(D3DXHANDLE hParameter, CONST D3DXMATRIX** ppMatrix, UINT Count);
			HRESULT __stdcall GetMatrixTransposePointerArray(D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count);
			HRESULT __stdcall SetString(D3DXHANDLE hParameter, LPCSTR pString);
			HRESULT __stdcall GetString(D3DXHANDLE hParameter, LPCSTR* ppString);
			HRESULT __stdcall SetTexture(D3DXHANDLE hParameter, IDirect3DBaseTexture9* pTexture);
			HRESULT __stdcall GetTexture(D3DXHANDLE hParameter, IDirect3DBaseTexture9** ppTexture);
			HRESULT __stdcall GetPixelShader(D3DXHANDLE hParameter, LPDIRECT3DPIXELSHADER9* ppPShader);
			HRESULT __stdcall GetVertexShader(D3DXHANDLE hParameter, LPDIRECT3DVERTEXSHADER9* ppVShader);
			HRESULT __stdcall SetArrayRange(D3DXHANDLE hParameter, UINT uStart, UINT uEnd);
			D3DXHANDLE __stdcall GetParameterElement(D3DXHANDLE hParameter, UINT Index);

			// ID3DXEffect methods
			HRESULT __stdcall GetPool(LPD3DXEFFECTPOOL* ppPool);
			HRESULT __stdcall SetTechnique(D3DXHANDLE hTechnique);
			D3DXHANDLE __stdcall GetCurrentTechnique();
			HRESULT __stdcall ValidateTechnique(D3DXHANDLE hTechnique);
			HRESULT __stdcall FindNextValidTechnique(D3DXHANDLE hTechnique, D3DXHANDLE* pTechnique);
			BOOL __stdcall IsParameterUsed(D3DXHANDLE hParameter, D3DXHANDLE hTechnique);
			HRESULT __stdcall Begin(UINT* pPasses, DWORD Flags);
			HRESULT __stdcall BeginPass(UINT Pass);
			HRESULT __stdcall CommitChanges();
			HRESULT __stdcall EndPass();
			HRESULT __stdcall End();
			HRESULT __stdcall OnLostDevice();
			HRESULT __stdcall OnResetDevice();
			HRESULT __stdcall SetStateManager(LPD3DXEFFECTSTATEMANAGER pManager);
			HRESULT __stdcall GetStateManager(LPD3DXEFFECTSTATEMANAGER* ppManager);
			HRESULT __stdcall BeginParameterBlock();
			D3DXHANDLE __stdcall EndParameterBlock();
			HRESULT __stdcall ApplyParameterBlock(D3DXHANDLE hParameterBlock);
			HRESULT __stdcall DeleteParameterBlock(D3DXHANDLE hParameterBlock);
			HRESULT __stdcall CloneEffect(IDirect3DDevice9* pDevice, ID3DXEffect** ppEffect);
			HRESULT __stdcall SetRawValue(D3DXHANDLE hParameter, LPCVOID pData, UINT ByteOffset, UINT Bytes);

		private:
			ID3DXEffect* m_pID3DXEffect;
		};

		// Hook function types
		typedef HRESULT(WINAPI* D3DXCreateEffectFromResourceA_t)(
			LPDIRECT3DDEVICE9 pDevice, HMODULE hSrcModule, LPCSTR pSrcResource, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors);

		typedef HRESULT(WINAPI* D3DXCreateEffectFromFile_t)(
			LPDIRECT3DDEVICE9 pDevice, LPCSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors);

		typedef HRESULT(WINAPI* D3DXCreateEffect_t)(
			LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors);

		typedef HRESULT(WINAPI* D3DXCreateEffectFromFileEx_t)(
			LPDIRECT3DDEVICE9 pDevice, LPCSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, LPCSTR pSkipConstants, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors);

		typedef HRESULT(WINAPI* D3DXCreateEffectEx_t)(
			LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, LPCSTR pSkipConstants, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors);

		// Original function pointers
		static D3DXCreateEffectFromResourceA_t D3DXCreateEffectFromResourceA_original;
		static D3DXCreateEffectFromFile_t D3DXCreateEffectFromFile_original;
		static D3DXCreateEffect_t D3DXCreateEffect_original;
		static D3DXCreateEffectFromFileEx_t D3DXCreateEffectFromFileEx_original;
		static D3DXCreateEffectEx_t D3DXCreateEffectEx_original;

		// Hooked functions
		static HRESULT WINAPI HookedD3DXCreateEffectFromResourceA(
			LPDIRECT3DDEVICE9 pDevice, HMODULE hSrcModule, LPCSTR pSrcResource, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors);

		static HRESULT WINAPI HookedD3DXCreateEffectFromFile(
			LPDIRECT3DDEVICE9 pDevice, LPCSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors);

		static HRESULT WINAPI HookedD3DXCreateEffect(
			LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors);

		static HRESULT WINAPI HookedD3DXCreateEffectFromFileEx(
			LPDIRECT3DDEVICE9 pDevice, LPCSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, LPCSTR pSkipConstants, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors);

		static HRESULT WINAPI HookedD3DXCreateEffectEx(
			LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, LPCSTR pSkipConstants, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors);
	};
}
