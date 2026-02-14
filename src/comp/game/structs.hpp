#pragma once

namespace comp::game
{
	struct vis_struct
	{
		int unk1;
		int some_flag;
		int unk3;
		int unk4;
		int unk5;
		int unk6;
		int unk7;
		int unk8;
		Vector4D plane_origin;
		Vector4D plane_normal;
		float max_strength;
		int some_bitflag;
		int unk9;
		int unk10;
	};

	struct ViewTransform
	{
	    D3DXMATRIX ViewMatrix;
	    D3DXMATRIX ProjectionMatrix;
	    D3DXMATRIX ProjectionZBiasMatrix;
	    D3DXMATRIX ViewProjectionMatrix;
	    D3DXMATRIX ViewProjectionZBiasMatrix;
	};

	struct eViewPlatInterface
	{      
	    ViewTransform* m_pTransform;
	};

}
