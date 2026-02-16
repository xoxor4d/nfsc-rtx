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


	struct material_data
	{
		float diffuse_power;
		float diffuse_clamp;
		float diffuse_flakes;
		float diffuse_vinyl_scale;
		float diffuse_min_scale;
		Vector diffuse_min;
		float diffuse_max_scale;
		Vector diffuse_max;
		float diffuse_min_a;
		float diffuse_max_a;
		float specular_power;
		float specular_flakes;
		float specular_vinyl_scale;
		float specular_min_scale;
		Vector specular_min;
		float specular_max_scale;
		Vector specular_max;
		float envmap_power;
		float envmap_clamp;
		float envmap_vinyl_scale;
		float envmap_min_scale;
		Vector envmap_min;
		float envmap_max_scale;
		Vector envmap_max;
		float vinyl_luminance_min_scale;
		float vinyl_luminance_max_scale;
	};

	struct material_instance
	{
		void* platform_info;
		int pad1;
		int pad2;
		std::uint32_t key;
		std::uint32_t version;
		char name[64];
		material_data material;
	};
}
