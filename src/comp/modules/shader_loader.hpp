#pragma once

namespace comp
{
	class shader_loader final : public shared::common::loader::component_module
	{
	public:
		shader_loader();
		~shader_loader() = default;

		static inline shader_loader* p_this = nullptr;
		static shader_loader* get() { return p_this; }

		static bool is_initialized()
		{
			if (const auto mod = get(); mod && mod->m_initialized) {
				return true;
			}
			return false;
		}

	private:
		bool m_initialized = false;
	};
}
