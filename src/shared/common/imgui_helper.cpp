#include "std_include.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "imgui_helper.hpp"

namespace ImGui
{
	void Spacing(const float& x, const float& y) {
		Dummy(ImVec2(x, y));
	}

	void PushFont(shared::common::font::FONTS font)
	{
		ImGuiIO& io = GetIO();

		if (io.Fonts->Fonts[font]) {
			PushFont(io.Fonts->Fonts[font]);
		}
		else {
			PushFont(GetDefaultFont());
		}
	}

	void CenterText(const char* text, bool disabled)
	{
		const auto text_width = CalcTextSize(text).x;
		SetCursorPosX(GetContentRegionAvail().x * 0.5f - text_width * 0.5f);
		if (!disabled) {
			TextUnformatted(text);
		}
		else {
			TextDisabled("%s", text);
		}
	}

	void AddUnterline(ImColor col)
	{
		ImVec2 min = GetItemRectMin();
		ImVec2 max = GetItemRectMax();
		min.y = max.y;
		GetWindowDrawList()->AddLine(min, max, col, 1.0f);
	}

	void TextURL(const char* name, const char* url, bool use_are_you_sure_popup)
	{
		TextUnformatted(name);
		if (IsItemHovered())
		{
			if (IsMouseClicked(0))
			{
				if (use_are_you_sure_popup)
				{
					if (!IsPopupOpen("Are You Sure?"))
					{
						PushID(name);
						OpenPopup("Are You Sure?");
						PopID();
					}
				}
				else
				{
					ImGuiIO& io = GetIO();
					io.AddMouseButtonEvent(0, false);
					io.AddMousePosEvent(0, 0);
					ShellExecuteA(nullptr, nullptr, url, nullptr, nullptr, SW_SHOW);
				}
			}

			AddUnterline(GetStyle().Colors[ImGuiCol_TabHovered]);
			SetTooltip("Clicking this will open the following link:\n[%s]", url);
		}
		else {
			AddUnterline(GetStyle().Colors[ImGuiCol_Button]);
		}

		PushID(name);
		if (BeginPopupModal("Are You Sure?", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
		{
			Spacing(0.0f, 0.0f);

			const auto half_width = GetContentRegionMax().x * 0.5f;
			auto line1_str = "This will open the following link:";

			Spacing();
			SetCursorPosX(5.0f + half_width - (CalcTextSize(line1_str).x * 0.5f));
			TextUnformatted(line1_str);

			SetCursorPosX(5.0f + half_width - (CalcTextSize(url).x * 0.5f));
			TextUnformatted(url);

			InvisibleButton("##spacer", ImVec2(CalcTextSize(url).x, 1));

			Spacing(0, 8);
			Spacing(0, 0); SameLine();

			ImVec2 button_size(half_width - 6.0f - GetStyle().WindowPadding.x, 0.0f);
			if (Button("Open", button_size))
			{
				ImGuiIO& io = GetIO();
				io.AddMouseButtonEvent(0, false);
				io.AddMousePosEvent(0, 0);
				CloseCurrentPopup();
				ShellExecuteA(nullptr, nullptr, url, nullptr, nullptr, SW_SHOW);
			}

			SameLine(0, 6.0f);
			if (Button("Cancel", button_size)) {
				CloseCurrentPopup();
			}

			EndPopup();
		}
		PopID();
	}

	void SetCursorForCenteredText(const char* text)
	{
		const auto text_width = CalcTextSize(text).x;
		SetCursorPosX(GetContentRegionAvail().x * 0.5f - text_width * 0.5f);
	}

	float CalcWidgetWidthForChild(const float label_width)
	{
		return GetContentRegionAvail().x - 4.0f - (label_width + GetStyle().ItemInnerSpacing.x + GetStyle().FramePadding.y);
	}

	void SetItemTooltipWrapper(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);

		if (IsItemHovered(ImGuiHoveredFlags_ForTooltip))
		{
			// (0.124f, 0.124f, 0.124f, 0.776f)
			PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.124f, 0.124f, 0.124f, 0.776f));

			if (!BeginTooltipEx(ImGuiTooltipFlags_OverridePrevious, ImGuiWindowFlags_None))
			{
				PopStyleColor();
				return;
			}
			PopStyleColor();

			const auto padding = 4.0f;

			Spacing(0, padding);			 // top padding
			Spacing(padding, 0); SameLine(); // left padding

			TextV(fmt, args);

			SameLine(); Spacing(padding, 0); // right padding
			Spacing(0, padding);			 // bottom padding

			EndTooltip();
		}

		va_end(args);
	}


	/// Custom Collapsing Header with changeable height - Background color: ImGuiCol_HeaderActive 
	/// @param title_text	Label
	/// @param height		Header Height
	/// @param border_color Border Color
	/// @param default_open	True to collapse by default
	/// @param pre_spacing	8y Spacing in-front of Header
	/// @return				False if Header collapsed
	bool Widget_WrappedCollapsingHeader(const char* title_text, const float height, const ImVec4& border_color, const bool default_open, const bool pre_spacing)
	{
		if (pre_spacing) {
			Spacing(0.0f, 8.0f);
		}

		PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
		PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, height));
		PushStyleColor(ImGuiCol_Border, border_color);

		const auto open_flag = default_open ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;

		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = GetCurrentWindow();
		ImGuiID storage_id = (g.NextItemData.HasFlags & ImGuiNextItemDataFlags_HasStorageID) ? g.NextItemData.StorageId : window->GetID(title_text);
		const bool is_open = TreeNodeUpdateNextOpen(storage_id, open_flag);

		if (is_open) {
			//PushStyleColor(ImGuiCol_Header, ImVec4(0.3f, 0.3f, 0.3f, 0.8f));
			auto header_color_tweak = g.Style.Colors[ImGuiCol_Header];
			header_color_tweak.x += 0.1f;
			header_color_tweak.y += 0.1f;
			header_color_tweak.z += 0.1f;
			header_color_tweak.w += 0.1f;
			PushStyleColor(ImGuiCol_Header, header_color_tweak);
		}

		const auto state = CollapsingHeader(title_text, open_flag | ImGuiTreeNodeFlags_SpanFullWidth);

		if (is_open) {
			PopStyleColor();
		}

		if (IsItemHovered() && IsMouseClicked(ImGuiMouseButton_Middle, false)) {
			SetScrollHereY(0.0f);
		}

		// just toggled
		if (state && is_open != state) {
			//SetScrollHereY(0.0f); 
		}

		PopStyleColor();
		PopStyleVar(2);

		return state;
	}

	float Widget_ContainerWithCollapsingTitle(const char* child_name, const float child_height, const std::function<void()>& callback, const bool default_open, const char* icon, const ImVec4* bg_col, const ImVec4* border_col)
	{
		const std::string child_str = "[ "s + child_name + " ]"s;
		const float child_indent = 2.0f;

		const ImVec4 background_color = bg_col ? *bg_col : ImVec4(0.220f, 0.220f, 0.220f, 0.863f);
		const ImVec4 border_color = border_col ? *border_col : ImVec4(0.099f, 0.099f, 0.099f, 0.901f);

		const auto& style = GetStyle();

		const auto window = GetCurrentWindow();
		const auto min_x = window->WorkRect.Min.x - style.WindowPadding.x * 0.5f + 1.0f;
		const auto max_x = window->WorkRect.Max.x + style.WindowPadding.x * 0.5f - 1.0f;

		PushFont(shared::common::font::BOLD);

		const auto spos_pre_header = GetCursorScreenPos();
		const auto expanded = Widget_WrappedCollapsingHeader(child_str.c_str(), 12.0f, border_color, default_open, false);

		PopFont();

		if (icon)
		{
			const auto spos_post_header = GetCursorScreenPos();
			const auto header_dims = GetItemRectSize();
			const auto icon_dims = CalcTextSize(icon);
			SetCursorScreenPos(spos_pre_header + ImVec2(header_dims.x - icon_dims.x - style.WindowPadding.x - 8.0f, header_dims.y * 0.5f - icon_dims.y * 0.5f));
			TextUnformatted(icon);
			SetCursorScreenPos(spos_post_header);
		}

		if (expanded)
		{
			const auto min = ImVec2(min_x, GetCursorScreenPos().y - style.ItemSpacing.y);
			const auto max = ImVec2(max_x, min.y + child_height);

			GetWindowDrawList()->AddRect(min + ImVec2(-1, 1), max + ImVec2(1, 1), ColorConvertFloat4ToU32(border_color), 10.0f, ImDrawFlags_RoundCornersBottom);
			GetWindowDrawList()->AddRectFilled(min, max, ColorConvertFloat4ToU32(background_color), 10.0f, ImDrawFlags_RoundCornersBottom);

			// dropshadow
			{
				const auto dshadow_pmin = GetCursorScreenPos() - ImVec2(GetStyle().WindowPadding.x * 0.5f, 4);
				const auto dshadow_pmax = dshadow_pmin + ImVec2(GetContentRegionAvail().x + GetStyle().WindowPadding.x, 48.0f);

				const auto col_bottom = ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 0.0f));
				const auto col_top = ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 0.4f));
				GetWindowDrawList()->AddRectFilledMultiColor(dshadow_pmin, dshadow_pmax, col_top, col_top, col_bottom, col_bottom);
			}

			PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2.0f, 4.0f));
			PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(6.0f, 8.0f));
			BeginChild(child_name, ImVec2(max.x - min.x - style.FramePadding.x - 2.0f, 0.0f),
				/*ImGuiChildFlags_Borders | */ ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY);

			Indent(child_indent);
			PushClipRect(min, max, true);
			if (callback)
			{
				Indent(4);
				callback();
				Unindent(4);
			}
			PopClipRect();
			Unindent(child_indent);

			EndChild();
			PopStyleVar(2);
		}
		SetCursorScreenPos(GetCursorScreenPos() + ImVec2(0, 8.0f));
		return GetItemRectSize().y + 6.0f/*- 28.0f*/;
	}
}
