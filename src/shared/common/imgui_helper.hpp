#pragma once
#include "font_defines.hpp"

namespace ImGui
{
	void Spacing(const float& x, const float& y);
	void PushFont(shared::common::font::FONTS font);

	void CenterText(const char* text, bool disabled = false);
	void AddUnterline(ImColor col);
	void TextURL(const char* name, const char* url, bool use_are_you_sure_popup = false);
	void SetCursorForCenteredText(const char* text);
	float CalcWidgetWidthForChild(float label_width);
	void SetItemTooltipWrapper(const char* fmt, ...);

	bool Widget_WrappedCollapsingHeader(const char* title_text, float height, const ImVec4& border_color, bool default_open = true, bool pre_spacing = false);
	float Widget_ContainerWithCollapsingTitle(const char* child_name, float child_height, const std::function<void()>& callback, bool default_open = true, const char* icon = nullptr, const ImVec4* bg_col = nullptr, const ImVec4* border_col = nullptr);
}
