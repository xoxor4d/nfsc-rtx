#include "std_include.hpp"
#include "d3dxeffects.hpp"

namespace comp
{
#pragma region D3DXEffect

	HRESULT d3dxeffects::D3DXEffect::QueryInterface(REFIID riid, void** ppvObj)
	{
		*ppvObj = nullptr;
		const HRESULT hRes = m_pID3DXEffect->QueryInterface(riid, ppvObj);

		if (hRes == NOERROR) {
			*ppvObj = this;
		}

		return hRes;
	}

	ULONG d3dxeffects::D3DXEffect::AddRef() {
		return m_pID3DXEffect->AddRef();
	}

	ULONG d3dxeffects::D3DXEffect::Release() {
		ULONG count = m_pID3DXEffect->Release();
		if (!count) delete this;
		return count;
	}

	HRESULT d3dxeffects::D3DXEffect::GetDevice(IDirect3DDevice9** ppDevice) {
		return m_pID3DXEffect->GetDevice(ppDevice);
	}

	HRESULT d3dxeffects::D3DXEffect::GetDesc(D3DXEFFECT_DESC* pDesc) {
		return m_pID3DXEffect->GetDesc(pDesc);
	}

	HRESULT d3dxeffects::D3DXEffect::GetParameterDesc(D3DXHANDLE hParameter, D3DXPARAMETER_DESC* pDesc) {
		return m_pID3DXEffect->GetParameterDesc(hParameter, pDesc);
	}

	HRESULT d3dxeffects::D3DXEffect::GetTechniqueDesc(D3DXHANDLE hTechnique, D3DXTECHNIQUE_DESC* pDesc) {
		return m_pID3DXEffect->GetTechniqueDesc(hTechnique, pDesc);
	}

	HRESULT d3dxeffects::D3DXEffect::GetPassDesc(D3DXHANDLE hPass, D3DXPASS_DESC* pDesc) {
		return m_pID3DXEffect->GetPassDesc(hPass, pDesc);
	}

	HRESULT d3dxeffects::D3DXEffect::GetFunctionDesc(D3DXHANDLE hShader, D3DXFUNCTION_DESC* pDesc) {
		return m_pID3DXEffect->GetFunctionDesc(hShader, pDesc);
	}

	D3DXHANDLE d3dxeffects::D3DXEffect::GetParameter(D3DXHANDLE hParameter, UINT Index) {
		return m_pID3DXEffect->GetParameter(hParameter, Index);
	}

	D3DXHANDLE d3dxeffects::D3DXEffect::GetParameterByName(D3DXHANDLE hParameter, LPCSTR pName) {
		return m_pID3DXEffect->GetParameterByName(hParameter, pName);
	}

	D3DXHANDLE d3dxeffects::D3DXEffect::GetParameterBySemantic(D3DXHANDLE hParameter, LPCSTR pSemantic) {
		return m_pID3DXEffect->GetParameterBySemantic(hParameter, pSemantic);
	}

	D3DXHANDLE d3dxeffects::D3DXEffect::GetTechnique(UINT Index)
	{
		return m_pID3DXEffect->GetTechnique(Index);
	}

	D3DXHANDLE d3dxeffects::D3DXEffect::GetTechniqueByName(LPCSTR pName) {
		return m_pID3DXEffect->GetTechniqueByName(pName);
	}

	D3DXHANDLE d3dxeffects::D3DXEffect::GetPass(D3DXHANDLE hTechnique, UINT Index) {
		return m_pID3DXEffect->GetPass(hTechnique, Index);
	}

	D3DXHANDLE d3dxeffects::D3DXEffect::GetPassByName(D3DXHANDLE hTechnique, LPCSTR pName) {
		return m_pID3DXEffect->GetPassByName(hTechnique, pName);
	}

	D3DXHANDLE d3dxeffects::D3DXEffect::GetFunction(UINT Index) {
		return m_pID3DXEffect->GetFunction(Index);
	}

	D3DXHANDLE d3dxeffects::D3DXEffect::GetFunctionByName(LPCSTR pName) {
		return m_pID3DXEffect->GetFunctionByName(pName);
	}

	D3DXHANDLE d3dxeffects::D3DXEffect::GetAnnotation(D3DXHANDLE hObject, UINT Index) {
		return m_pID3DXEffect->GetAnnotation(hObject, Index);
	}

	D3DXHANDLE d3dxeffects::D3DXEffect::GetAnnotationByName(D3DXHANDLE hObject, LPCSTR pName) {
		return m_pID3DXEffect->GetAnnotationByName(hObject, pName);
	}

	HRESULT d3dxeffects::D3DXEffect::SetValue(D3DXHANDLE hParameter, LPCVOID pData, UINT Bytes) {
		return m_pID3DXEffect->SetValue(hParameter, pData, Bytes);
	}

	HRESULT d3dxeffects::D3DXEffect::GetValue(D3DXHANDLE hParameter, LPVOID pData, UINT Bytes) {
		return m_pID3DXEffect->GetValue(hParameter, pData, Bytes);
	}

	HRESULT d3dxeffects::D3DXEffect::SetBool(D3DXHANDLE hParameter, BOOL b) {
		return m_pID3DXEffect->SetBool(hParameter, b);
	}

	HRESULT d3dxeffects::D3DXEffect::GetBool(D3DXHANDLE hParameter, BOOL* pb) {
		return m_pID3DXEffect->GetBool(hParameter, pb);
	}

	HRESULT d3dxeffects::D3DXEffect::SetBoolArray(D3DXHANDLE hParameter, CONST BOOL* pb, UINT Count) {
		return m_pID3DXEffect->SetBoolArray(hParameter, pb, Count);
	}

	HRESULT d3dxeffects::D3DXEffect::GetBoolArray(D3DXHANDLE hParameter, BOOL* pb, UINT Count) {
		return m_pID3DXEffect->GetBoolArray(hParameter, pb, Count);
	}

	HRESULT d3dxeffects::D3DXEffect::SetInt(D3DXHANDLE hParameter, INT n) {
		return m_pID3DXEffect->SetInt(hParameter, n);
	}

	HRESULT d3dxeffects::D3DXEffect::GetInt(D3DXHANDLE hParameter, INT* pn) {
		return m_pID3DXEffect->GetInt(hParameter, pn);
	}

	HRESULT d3dxeffects::D3DXEffect::SetIntArray(D3DXHANDLE hParameter, CONST INT* pn, UINT Count)
	{
		return m_pID3DXEffect->SetIntArray(hParameter, pn, Count);
	}

	HRESULT d3dxeffects::D3DXEffect::GetIntArray(D3DXHANDLE hParameter, INT* pn, UINT Count) {
		return m_pID3DXEffect->GetIntArray(hParameter, pn, Count);
	}

	HRESULT d3dxeffects::D3DXEffect::SetFloat(D3DXHANDLE hParameter, FLOAT f) {
		return m_pID3DXEffect->SetFloat(hParameter, f);
	}

	HRESULT d3dxeffects::D3DXEffect::GetFloat(D3DXHANDLE hParameter, FLOAT* pf) {
		return m_pID3DXEffect->GetFloat(hParameter, pf);
	}

	HRESULT d3dxeffects::D3DXEffect::SetFloatArray(D3DXHANDLE hParameter, CONST FLOAT* pf, UINT Count) {
		return m_pID3DXEffect->SetFloatArray(hParameter, pf, Count);
	}

	HRESULT d3dxeffects::D3DXEffect::GetFloatArray(D3DXHANDLE hParameter, FLOAT* pf, UINT Count) {
		return m_pID3DXEffect->GetFloatArray(hParameter, pf, Count);
	}

	HRESULT d3dxeffects::D3DXEffect::SetVector(D3DXHANDLE hParameter, CONST D3DXVECTOR4* pVector) {
		return m_pID3DXEffect->SetVector(hParameter, pVector);
	}

	HRESULT d3dxeffects::D3DXEffect::GetVector(D3DXHANDLE hParameter, D3DXVECTOR4* pVector) {
		return m_pID3DXEffect->GetVector(hParameter, pVector);
	}

	HRESULT d3dxeffects::D3DXEffect::SetVectorArray(D3DXHANDLE hParameter, CONST D3DXVECTOR4* pVector, UINT Count) {
		return m_pID3DXEffect->SetVectorArray(hParameter, pVector, Count);
	}

	HRESULT d3dxeffects::D3DXEffect::GetVectorArray(D3DXHANDLE hParameter, D3DXVECTOR4* pVector, UINT Count) {
		return m_pID3DXEffect->GetVectorArray(hParameter, pVector, Count);
	}

	HRESULT d3dxeffects::D3DXEffect::SetMatrix(D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix) {
		return m_pID3DXEffect->SetMatrix(hParameter, pMatrix);
	}

	HRESULT d3dxeffects::D3DXEffect::GetMatrix(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix) {
		return m_pID3DXEffect->GetMatrix(hParameter, pMatrix);
	}

	HRESULT d3dxeffects::D3DXEffect::SetMatrixArray(D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix, UINT Count) {
		return m_pID3DXEffect->SetMatrixArray(hParameter, pMatrix, Count);
	}

	HRESULT d3dxeffects::D3DXEffect::GetMatrixArray(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count) {
		return m_pID3DXEffect->GetMatrixArray(hParameter, pMatrix, Count);
	}

	HRESULT d3dxeffects::D3DXEffect::SetMatrixPointerArray(D3DXHANDLE hParameter, CONST D3DXMATRIX** ppMatrix, UINT Count) {
		return m_pID3DXEffect->SetMatrixPointerArray(hParameter, ppMatrix, Count);
	}

	HRESULT d3dxeffects::D3DXEffect::GetMatrixPointerArray(D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count) {
		return m_pID3DXEffect->GetMatrixPointerArray(hParameter, ppMatrix, Count);
	}

	HRESULT d3dxeffects::D3DXEffect::SetMatrixTranspose(D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix) {
		return m_pID3DXEffect->SetMatrixTranspose(hParameter, pMatrix);
	}

	HRESULT d3dxeffects::D3DXEffect::GetMatrixTranspose(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix) {
		return m_pID3DXEffect->GetMatrixTranspose(hParameter, pMatrix);
	}

	HRESULT d3dxeffects::D3DXEffect::SetMatrixTransposeArray(D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix, UINT Count) {
		return m_pID3DXEffect->SetMatrixTransposeArray(hParameter, pMatrix, Count);
	}

	HRESULT d3dxeffects::D3DXEffect::GetMatrixTransposeArray(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count) {
		return m_pID3DXEffect->GetMatrixTransposeArray(hParameter, pMatrix, Count);
	}

	HRESULT d3dxeffects::D3DXEffect::SetMatrixTransposePointerArray(D3DXHANDLE hParameter, CONST D3DXMATRIX** ppMatrix, UINT Count) {
		return m_pID3DXEffect->SetMatrixTransposePointerArray(hParameter, ppMatrix, Count);
	}

	HRESULT d3dxeffects::D3DXEffect::GetMatrixTransposePointerArray(D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count) {
		return m_pID3DXEffect->GetMatrixTransposePointerArray(hParameter, ppMatrix, Count);
	}

	HRESULT d3dxeffects::D3DXEffect::SetString(D3DXHANDLE hParameter, LPCSTR pString) {
		return m_pID3DXEffect->SetString(hParameter, pString);
	}

	HRESULT d3dxeffects::D3DXEffect::GetString(D3DXHANDLE hParameter, LPCSTR* ppString) {
		return m_pID3DXEffect->GetString(hParameter, ppString);
	}

	HRESULT d3dxeffects::D3DXEffect::SetTexture(D3DXHANDLE hParameter, IDirect3DBaseTexture9* pTexture) {
		return m_pID3DXEffect->SetTexture(hParameter, pTexture);
	}

	HRESULT d3dxeffects::D3DXEffect::GetTexture(D3DXHANDLE hParameter, IDirect3DBaseTexture9** ppTexture) {
		return m_pID3DXEffect->GetTexture(hParameter, ppTexture);
	}

	HRESULT d3dxeffects::D3DXEffect::GetPixelShader(D3DXHANDLE hParameter, LPDIRECT3DPIXELSHADER9* ppPShader) {
		return m_pID3DXEffect->GetPixelShader(hParameter, ppPShader);
	}

	HRESULT d3dxeffects::D3DXEffect::GetVertexShader(D3DXHANDLE hParameter, LPDIRECT3DVERTEXSHADER9* ppVShader) {
		return m_pID3DXEffect->GetVertexShader(hParameter, ppVShader);
	}

	HRESULT d3dxeffects::D3DXEffect::SetArrayRange(D3DXHANDLE hParameter, UINT uStart, UINT uEnd)
	{
		return m_pID3DXEffect->SetArrayRange(hParameter, uStart, uEnd);
	}

	D3DXHANDLE d3dxeffects::D3DXEffect::GetParameterElement(D3DXHANDLE hParameter, UINT Index) {
		return m_pID3DXEffect->GetParameterElement(hParameter, Index);
	}

	HRESULT d3dxeffects::D3DXEffect::GetPool(LPD3DXEFFECTPOOL* ppPool) {
		return m_pID3DXEffect->GetPool(ppPool);
	}

	HRESULT d3dxeffects::D3DXEffect::SetTechnique(D3DXHANDLE hTechnique)
	{
		D3DXTECHNIQUE_DESC tech_desc {};
		if (SUCCEEDED(m_pID3DXEffect->GetTechniqueDesc(hTechnique, &tech_desc)) && tech_desc.Name)
		{
			effects::g_current_tech_name = tech_desc.Name;

			// O(1) lookup: map name -> ETECH enum value (empty until enum is populated)
			const auto it = effects::g_tech_name_to_enum.find(tech_desc.Name);
			effects::g_current_tech = (it != effects::g_tech_name_to_enum.end()) ? it->second : effects::ETECH::UNKNOWN;
		}

		return m_pID3DXEffect->SetTechnique(hTechnique);
	}

	D3DXHANDLE d3dxeffects::D3DXEffect::GetCurrentTechnique() {
		return m_pID3DXEffect->GetCurrentTechnique();
	}

	HRESULT d3dxeffects::D3DXEffect::ValidateTechnique(D3DXHANDLE hTechnique) {
		return m_pID3DXEffect->ValidateTechnique(hTechnique);
	}

	HRESULT d3dxeffects::D3DXEffect::FindNextValidTechnique(D3DXHANDLE hTechnique, D3DXHANDLE* pTechnique) {
		return m_pID3DXEffect->FindNextValidTechnique(hTechnique, pTechnique);
	}

	BOOL d3dxeffects::D3DXEffect::IsParameterUsed(D3DXHANDLE hParameter, D3DXHANDLE hTechnique) {
		return m_pID3DXEffect->IsParameterUsed(hParameter, hTechnique);
	}

	HRESULT d3dxeffects::D3DXEffect::Begin(UINT* pPasses, DWORD Flags)
	{
		//shared::common::log("d3dxeffects", "< Begin ------", shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
		return m_pID3DXEffect->Begin(pPasses, Flags);
	}

	HRESULT d3dxeffects::D3DXEffect::BeginPass(UINT Pass)
	{
		//shared::common::log("d3dxeffects", "< BeginPass ------", shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
		const auto hr = m_pID3DXEffect->BeginPass(Pass);
		return hr;
	}

	HRESULT d3dxeffects::D3DXEffect::CommitChanges()
	{
		//shared::common::log("d3dxeffects", "CommitChanges", shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
		return m_pID3DXEffect->CommitChanges();
	}

	HRESULT d3dxeffects::D3DXEffect::EndPass()
	{
		//shared::common::log("d3dxeffects", "------- EndPass ------ >", shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
		const auto hr = m_pID3DXEffect->EndPass();
		return hr;
	}

	HRESULT d3dxeffects::D3DXEffect::End()
	{
		//shared::common::log("d3dxeffects", "------- End ------ >", shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
		return m_pID3DXEffect->End();
	}

	HRESULT d3dxeffects::D3DXEffect::OnLostDevice() {
		return m_pID3DXEffect->OnLostDevice();
	}

	HRESULT d3dxeffects::D3DXEffect::OnResetDevice() {
		return m_pID3DXEffect->OnResetDevice();
	}

	HRESULT d3dxeffects::D3DXEffect::SetStateManager(LPD3DXEFFECTSTATEMANAGER pManager) {
		return m_pID3DXEffect->SetStateManager(pManager);
	}

	HRESULT d3dxeffects::D3DXEffect::GetStateManager(LPD3DXEFFECTSTATEMANAGER* ppManager) {
		return m_pID3DXEffect->GetStateManager(ppManager);
	}

	HRESULT d3dxeffects::D3DXEffect::BeginParameterBlock() {
		return m_pID3DXEffect->BeginParameterBlock();
	}

	D3DXHANDLE d3dxeffects::D3DXEffect::EndParameterBlock() {
		return m_pID3DXEffect->EndParameterBlock();
	}

	HRESULT d3dxeffects::D3DXEffect::ApplyParameterBlock(D3DXHANDLE hParameterBlock) {
		return m_pID3DXEffect->ApplyParameterBlock(hParameterBlock);
	}

	HRESULT d3dxeffects::D3DXEffect::DeleteParameterBlock(D3DXHANDLE hParameterBlock) {
		return m_pID3DXEffect->DeleteParameterBlock(hParameterBlock);
	}

	HRESULT d3dxeffects::D3DXEffect::CloneEffect(IDirect3DDevice9* pDevice, ID3DXEffect** ppEffect) {
		return m_pID3DXEffect->CloneEffect(pDevice, ppEffect);
	}

	HRESULT d3dxeffects::D3DXEffect::SetRawValue(D3DXHANDLE hParameter, LPCVOID pData, UINT ByteOffset, UINT Bytes) {
		return m_pID3DXEffect->SetRawValue(hParameter, pData, ByteOffset, Bytes);
	}

#pragma endregion

	// Static member initialization
	d3dxeffects::D3DXCreateEffectFromResourceA_t d3dxeffects::D3DXCreateEffectFromResourceA_original = nullptr;
	d3dxeffects::D3DXCreateEffectFromFile_t d3dxeffects::D3DXCreateEffectFromFile_original = nullptr;
	d3dxeffects::D3DXCreateEffect_t d3dxeffects::D3DXCreateEffect_original = nullptr;
	d3dxeffects::D3DXCreateEffectFromFileEx_t d3dxeffects::D3DXCreateEffectFromFileEx_original = nullptr;
	d3dxeffects::D3DXCreateEffectEx_t d3dxeffects::D3DXCreateEffectEx_original = nullptr;

	ID3DXEffect* d3dxeffects::CreateWrappedEffect(ID3DXEffect* pOriginal)
	{
		if (!pOriginal) {
			return nullptr;
		}

		return new D3DXEffect(pOriginal);
	}

	HRESULT WINAPI d3dxeffects::HookedD3DXCreateEffectFromResourceA(LPDIRECT3DDEVICE9 pDevice, HMODULE hSrcModule, LPCSTR pSrcResource, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors)
	{
		const HRESULT hr = D3DXCreateEffectFromResourceA_original(pDevice, hSrcModule, pSrcResource, pDefines, pInclude, Flags, pPool, ppEffect, ppCompilationErrors);
		if (SUCCEEDED(hr) && ppEffect && *ppEffect) 
		{
			shared::common::log("d3dxeffects", std::format("D3DXCreateEffectFromResourceA: Wrapping effect '{}'", pSrcResource ? pSrcResource : "unknown"), shared::common::LOG_TYPE::LOG_TYPE_STATUS, false);
			*ppEffect = CreateWrappedEffect(*ppEffect);
		}

		return hr;
	}

	HRESULT WINAPI d3dxeffects::HookedD3DXCreateEffectFromFile(LPDIRECT3DDEVICE9 pDevice, LPCSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors)
	{
		const HRESULT hr = D3DXCreateEffectFromFile_original(pDevice, pSrcFile, pDefines, pInclude, Flags, pPool, ppEffect, ppCompilationErrors);
		if (SUCCEEDED(hr) && ppEffect && *ppEffect) 
		{
			shared::common::log("d3dxeffects", std::format("D3DXCreateEffectFromFile: Wrapping effect '{}'", pSrcFile ? pSrcFile : "unknown"), shared::common::LOG_TYPE::LOG_TYPE_STATUS, false);
			*ppEffect = CreateWrappedEffect(*ppEffect);
		}

		return hr;
	}

	HRESULT WINAPI d3dxeffects::HookedD3DXCreateEffect(LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors)
	{
		const HRESULT hr = D3DXCreateEffect_original(pDevice, pSrcData, SrcDataLen, pDefines, pInclude, Flags, pPool, ppEffect, ppCompilationErrors);
		if (SUCCEEDED(hr) && ppEffect && *ppEffect) 
		{
			shared::common::log("d3dxeffects", "D3DXCreateEffect: Wrapping effect from memory", shared::common::LOG_TYPE::LOG_TYPE_STATUS, false);
			*ppEffect = CreateWrappedEffect(*ppEffect);
		}

		return hr;
	}

	HRESULT WINAPI d3dxeffects::HookedD3DXCreateEffectFromFileEx(LPDIRECT3DDEVICE9 pDevice, LPCSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, LPCSTR pSkipConstants, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors)
	{
		const HRESULT hr = D3DXCreateEffectFromFileEx_original(pDevice, pSrcFile, pDefines, pInclude, pSkipConstants, Flags, pPool, ppEffect, ppCompilationErrors);
		if (SUCCEEDED(hr) && ppEffect && *ppEffect) 
		{
			shared::common::log("d3dxeffects", std::format("D3DXCreateEffectFromFileEx: Wrapping effect '{}'", pSrcFile ? pSrcFile : "unknown"), shared::common::LOG_TYPE::LOG_TYPE_STATUS, false);
			*ppEffect = CreateWrappedEffect(*ppEffect);
		}

		return hr;
	}

	HRESULT WINAPI d3dxeffects::HookedD3DXCreateEffectEx(LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, LPCSTR pSkipConstants, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors)
	{
		const HRESULT hr = D3DXCreateEffectEx_original(pDevice, pSrcData, SrcDataLen, pDefines, pInclude, pSkipConstants, Flags, pPool, ppEffect, ppCompilationErrors);
		if (SUCCEEDED(hr) && ppEffect && *ppEffect) 
		{
			shared::common::log("d3dxeffects", "D3DXCreateEffectEx: Wrapping effect from memory", shared::common::LOG_TYPE::LOG_TYPE_STATUS, false);
			*ppEffect = CreateWrappedEffect(*ppEffect);
		}

		return hr;
	}

	d3dxeffects::d3dxeffects()
	{
		// Build the name -> ETECH lookup table (used in SetTechnique for O(1) checking)
		using E = effects::ETECH;
		effects::g_tech_name_to_enum =
		{
			{ "world",                                          E::WORLD                                         },
			{ "lowlod",                                         E::LOWLOD                                        },
			{ "world_1_1",                                      E::WORLD_1_1                                     },
			{ "dryroad",                                        E::DRYROAD                                       },
			{ "raining_on_road",                                E::RAINING_ON_ROAD                               },
			{ "world_fixed",                                    E::WORLD_FIXED                                   },
			{ "car",                                            E::CAR                                           },
			{ "world_min",                                      E::WORLD_MIN                                     },
			{ "filter",                                         E::FILTER                                        },
			{ "visualtreatment",                                E::VISUALTREATMENT                               },
			{ "visualtreatment_enchanced",                      E::VISUALTREATMENT_ENCHANCED                     },
			{ "motionblur",                                     E::MOTIONBLUR                                    },
			{ "composite_blur",                                 E::COMPOSITE_BLUR                                },
			{ "uvesovercliff",                                  E::UVESOVERCLIFF                                 },
			{ "uvesovercliffdarken",                            E::UVESOVERCLIFFDARKEN                           },
			{ "screen_passthru",                                E::SCREEN_PASSTHRU                               },
			{ "fuzzz",                                          E::FUZZZ                                         },
			{ "no_fuzzz",                                       E::NO_FUZZZ                                      },
			{ "streak_flares",                                  E::STREAK_FLARES                                 },
			{ "flares",                                         E::FLARES                                        },
			{ "sky",                                            E::SKY                                           },
			{ "skinned",                                        E::SKINNED                                       },
			{ "depth_technique_noalpha",                        E::DEPTH_TECHNIQUE_NOALPHA                       },
			{ "TShader_Instancing",                             E::TSHADER_INSTANCING                            },
			{ "TwoPassBlur",                                    E::TWOPASSBLUR                                   },
			{ "GaussBlur5x5",                                   E::GAUSSBLUR5X5                                  },
			{ "DownScale4x4",                                   E::DOWNSCALE4X4                                  },
			{ "DownScale2x2",                                   E::DOWNSCALE2X2                                  },
			{ "DownScale2x2ForMotionBlur",                      E::DOWNSCALE2X2_FOR_MOTIONBLUR                   },
			{ "DownScaleForBloom",                              E::DOWNSCALE_FOR_BLOOM                           },
			{ "BlendTextures",                                  E::BLEND_TEXTURES                                },
			{ "CombineReflectionColourAndHeadlightAlpha",       E::COMBINE_REFLECTION_COLOUR_AND_HEADLIGHT_ALPHA },
			{ "finalhdrpass",                                   E::FINALHDRPASS                                  },
			{ "world_masked",                                   E::WORLD_MASKED                                  },
			{ "yuvmovie",                                       E::YUVMOVIE                                      },
			{ "screen_passthru_alpha_tag",                      E::SCREEN_PASSTHRU_ALPHA_TAG                     },
			{ "downscale4x4_t0_uves_hdr",                       E::DOWNSCALE4X4_T0_UVES_HDR                      },
			{ "bloom",                                          E::BLOOM                                         },
			{ "blur",                                           E::BLUR                                          },
			{ "brightpass",                                     E::BRIGHTPASS                                    },
			{ "calculate_luminance",                            E::CALCULATE_LUMINANCE                           },
			{ "calculate_adaptation",                           E::CALCULATE_ADAPTATION                          },
			{ "attenuate",                                      E::ATTENUATE                                     },
			{ "DebugShowRedChannel",                            E::DEBUG_SHOW_RED_CHANNEL                        },
			{ "main",                                           E::MAIN                                          },
			{ "main_1_1",                                       E::MAIN_1_1                                      },
			{ "glassreflect",                                   E::GLASSREFLECT                                  },
			{ "water",                                          E::WATER                                         },
			{ "rvm",                                            E::RVM                                           },
			{ "pip",                                            E::PIP                                           },
			{ "ghostcar",                                       E::GHOSTCAR                                      },
			{ "car_normalmap",                                  E::CAR_NORMALMAP                                 },
		};

		if (HMODULE hD3DX9 = GetModuleHandle(L"d3dx9_43.dll"); hD3DX9) 
		{
			auto try_hook = [&](LPCSTR name, LPVOID hook, LPVOID* original)
				{
					if (const auto addr = (DWORD)GetProcAddress(hD3DX9, name); addr)
					{
						if (MH_CreateHook((LPVOID)addr, hook, original) == MH_OK) 
						{
							MH_EnableHook((LPVOID)addr);
							shared::common::log("d3dxeffects", std::format("Hooked '{}'", name), shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
						}
					}
				};

			try_hook("D3DXCreateEffectFromResourceA", &HookedD3DXCreateEffectFromResourceA, (LPVOID*)&D3DXCreateEffectFromResourceA_original);
			try_hook("D3DXCreateEffectFromFileA",     &HookedD3DXCreateEffectFromFile,       (LPVOID*)&D3DXCreateEffectFromFile_original);
			try_hook("D3DXCreateEffect",              &HookedD3DXCreateEffect,               (LPVOID*)&D3DXCreateEffect_original);
			try_hook("D3DXCreateEffectFromFileExA",   &HookedD3DXCreateEffectFromFileEx,     (LPVOID*)&D3DXCreateEffectFromFileEx_original);
			try_hook("D3DXCreateEffectEx",            &HookedD3DXCreateEffectEx,             (LPVOID*)&D3DXCreateEffectEx_original);
		} 
		else {
			shared::common::log("d3dxeffects", "Failed to find d3dx9_43.dll", shared::common::LOG_TYPE::LOG_TYPE_ERROR, true);
		}

		shared::common::log("d3dxeffects", "Module initialized.", shared::common::LOG_TYPE::LOG_TYPE_DEFAULT, false);
	}
}
