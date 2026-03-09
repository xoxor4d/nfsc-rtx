#pragma once

namespace comp
{
	struct freecam_t
	{
		D3DXVECTOR3 position{};
		D3DXVECTOR3 forward{};
		D3DXVECTOR3 right{};
		D3DXVECTOR3 up{};

		// temps
		D3DXMATRIX yaw_mat;
		D3DXMATRIX pitch_mat;
		D3DXMATRIX roll_mat;
		D3DXMATRIX view;

		float near_clip = 0.5f;
		float far_clip = 10000.0f;
		std::uint16_t horizontal_fov = 0u;

		bool active = false;
		bool pad[3];
	};

	extern freecam_t g_freecam;

	struct remix_distant_light_def
	{
		remixapi_LightHandle m_handle = nullptr;
		remixapi_LightInfoDistantEXT m_ext = {};
		remixapi_LightInfo m_info = {};
		uint64_t m_hash;
	};

	extern remix_distant_light_def m_distant_light;

	void on_begin_scene_cb();
	void main();

	extern bool g_installed_signature_patches;
	extern bool g_install_signature_patches_async;
}
