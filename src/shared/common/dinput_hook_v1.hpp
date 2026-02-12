#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

namespace shared::common
{
	class dinput_v1 final : public loader::component_module
	{
	public:
		dinput_v1();

		static inline dinput_v1* p_this = nullptr;
		static dinput_v1* get() { return p_this; }

	private:
		bool m_initialized = false;
	};
}
