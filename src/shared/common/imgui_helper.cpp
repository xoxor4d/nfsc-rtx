#include "std_include.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "imgui_helper.hpp"

#include "comp/modules/imgui.hpp"

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

	float Widget_ContainerWithDropdownShadowSquare(const float container_height, const std::function<void()>& callback, const ImVec4* bg_col, const ImVec4* border_col)
	{
		Spacing(0, 6);

		const ImVec4 background_color = bg_col ? *bg_col : ImVec4(0.220f, 0.220f, 0.220f, 0.863f);
		const ImVec4 border_color = border_col ? *border_col : ImVec4(0.099f, 0.099f, 0.099f, 0.901f);

		const auto& style = GetStyle();

		const auto window = GetCurrentWindow();
		const auto min_x = window->WorkRect.Min.x - style.WindowPadding.x * 0.5f + 1.0f;
		const auto max_x = window->WorkRect.Max.x + style.WindowPadding.x * 0.5f - 1.0f;

		const auto min = ImVec2(min_x, GetCursorScreenPos().y - style.ItemSpacing.y);
		const auto max = ImVec2(max_x, min.y + container_height);

		GetWindowDrawList()->AddRect(min + ImVec2(-1, -1), max + ImVec2(1, 1), ColorConvertFloat4ToU32(border_color), 0.0f);
		GetWindowDrawList()->AddRectFilled(min, max, ColorConvertFloat4ToU32(background_color), 0.0f);

		// dropshadow
		{
			const auto dshadow_pmin = GetCursorScreenPos() - ImVec2(style.WindowPadding.x * 0.5f, 4);
			const auto dshadow_pmax = dshadow_pmin + ImVec2(GetContentRegionAvail().x + style.WindowPadding.x, 48.0f);

			const auto col_bottom = ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 0.0f));
			const auto col_top = ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 0.4f));
			GetWindowDrawList()->AddRectFilledMultiColor(dshadow_pmin, dshadow_pmax, col_top, col_top, col_bottom, col_bottom);
		}

		Indent(4);
		BeginGroup();
		Spacing(0, 4);
		callback();
		Spacing(0, 4);
		EndGroup();
		Unindent(4);

		Spacing(0, 4);

		return GetItemRectSize().y + 6.0f;
	}

	float Widget_CategoryWithVerticalLabel(const char* category_text, const std::function<void()>& callback, const ImVec4* text_color, const ImVec4* line_color)
	{
		const auto im = comp::imgui::get();

		const ImVec4 default_text_color = GetStyleColorVec4(ImGuiCol_Text);
		const ImVec4 default_line_color = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);

		const ImVec4 final_text_color = text_color ? *text_color : default_text_color;
		const ImVec4 final_line_color = line_color ? *line_color : default_line_color;

		const auto draw_list = GetWindowDrawList();
		ImFont* font = GetFont();

		// Save starting position
		const ImVec2 start_pos = GetCursorScreenPos();
		ImVec2 text_size = CalcTextSize(category_text);

		// When text is rotated 90° counter-clockwise, the text's width becomes its height
		// We need horizontal space = font height
		const float font_height = text_size.y;
		const float text_area_width = font_height; // Font height
		const float line_spacing = 8.0f; // Space between text and line
		const float line_thickness = 2.0f;
		const float content_spacing = 12.0f; // Space between line and content
		const float total_left_margin = text_area_width + line_spacing + line_thickness + content_spacing;

		// Move cursor to the right to make space for vertical text, line, and spacing
		SetCursorScreenPos(ImVec2(start_pos.x + total_left_margin, start_pos.y));

		// Split draw list into channels for proper layering
		// Channel 0: gradient (background)
		// Channel 1: content (foreground)
		draw_list->ChannelsSplit(2);
		draw_list->ChannelsSetCurrent(1); // Draw content on foreground channel

		// Draw content in a group to measure its height
		BeginGroup();
		Spacing(0, 10.0f); // Top padding
		if (callback) {
			callback();
		}
		Spacing(0, 10.0f); // Bottom padding
		EndGroup();

		// Get the actual height of the content group
		const ImVec2 content_rect_size = GetItemRectSize();
		const float content_height = content_rect_size.y;

		// Switch to background channel to draw gradient, text, and line
		draw_list->ChannelsSetCurrent(0);

		// Calculate position for the rotated text (centered vertically along content)
		const float text_center_y = start_pos.y + content_height * 0.5f;
		const float text_x = start_pos.x;

		// Render text vertices at origin first
		const ImVec2 temp_pos = ImVec2(0.0f, 0.0f);
		const size_t vtx_begin = draw_list->VtxBuffer.Size;

		const ImVec4 clip_rect = ImVec4(-FLT_MAX, -FLT_MAX, FLT_MAX, FLT_MAX);
		PushFont(shared::common::font::BOLD_LARGE);
		const float font_size = GetFontSize();

		bool use_dot_label_text = false;
		if (text_size.x > content_height)
		{
			use_dot_label_text = true;
			text_size = CalcTextSize("...");
		}

		font->RenderText(draw_list, font_size, temp_pos, ColorConvertFloat4ToU32(final_text_color),
			clip_rect, use_dot_label_text ? "..." : category_text, nullptr, 0.0f, false);
		PopFont();
		const size_t vtx_end = draw_list->VtxBuffer.Size;

		// Transform vertices for -90 degree rotation
		// After rotation: original (x,y) becomes (y, -x)
		for (size_t i = vtx_begin; i < vtx_end; i++)
		{
			ImDrawVert& vtx = draw_list->VtxBuffer[i];
			const float x = vtx.pos.x;
			const float y = vtx.pos.y;

			// Rotate and translate to final position
			// Keep X consistent (at text_x), center Y along content height
			vtx.pos.x = text_x + y;
			vtx.pos.y = text_center_y - x + text_size.x * 0.5f;
		}

		// Draw vertical line between text and content
		const float line_x = start_pos.x + 18.0f + line_spacing;
		const ImVec2 line_start = ImVec2(line_x, start_pos.y);
		const ImVec2 line_end = ImVec2(line_x, start_pos.y + content_height);
		draw_list->AddLine(line_start, line_end, ColorConvertFloat4ToU32(final_line_color), line_thickness);

		// Draw left gradient rectangle from text to line (fades from transparent to line)
		{
			const float left_gradient_start_x = start_pos.x - 4.0f;
			const float left_gradient_end_x = line_x;
			const ImVec2 left_gradient_pmin = ImVec2(left_gradient_start_x, start_pos.y);
			const ImVec2 left_gradient_pmax = ImVec2(left_gradient_end_x, start_pos.y + content_height);

			const auto col_left = ColorConvertFloat4ToU32(ImVec4(im->ImGuiCol_VerticalFadeContainerBackgroundEnd));   // Transparent on the left
			const auto col_right = ColorConvertFloat4ToU32(ImVec4(im->ImGuiCol_VerticalFadeContainerBackgroundStart)); // Semi-transparent at the line
			draw_list->AddRectFilledMultiColor(left_gradient_pmin, left_gradient_pmax, col_left, col_right, col_right, col_left);
		}

		// Draw right gradient rectangle from line to right edge (fades from line into content)
		{
			const auto& style = GetStyle();
			const auto window = GetCurrentWindow();
			const float gradient_start_x = line_x;
			const float gradient_end_x = window->WorkRect.Max.x + style.WindowPadding.x * 0.5f;
			const ImVec2 gradient_pmin = ImVec2(gradient_start_x, start_pos.y);
			const ImVec2 gradient_pmax = ImVec2(gradient_end_x, start_pos.y + content_height);

			const auto col_left = ColorConvertFloat4ToU32(ImVec4(im->ImGuiCol_VerticalFadeContainerBackgroundStart));  // Semi-transparent at the line
			const auto col_right = ColorConvertFloat4ToU32(ImVec4(im->ImGuiCol_VerticalFadeContainerBackgroundEnd));  // Transparent on the right
			draw_list->AddRectFilledMultiColor(gradient_pmin, gradient_pmax, col_left, col_right, col_right, col_left);
		}

		// Merge channels so background is drawn first, then content on top
		draw_list->ChannelsMerge();

		// Move cursor to the next row (below this widget)
		SetCursorScreenPos(ImVec2(start_pos.x, start_pos.y + content_height));

		return content_height;
	}

	void Style_DeleteButtonPush()
	{
		PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.05f, 0.05f, 1.0f));
		PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.68f, 0.05f, 0.05f, 1.0f));
		PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.75f, 0.2f, 0.2f, 1.0f));
		PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 1.0f));

		PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		PushFont(shared::common::font::BOLD);
	}

	void Style_DeleteButtonPop()
	{
		PopStyleVar(2);
		PopStyleColor(4);
		PopFont();
	}

	void Style_ColorButtonPush(const ImVec4& base_color, bool black_border)
	{
		PushStyleColor(ImGuiCol_Button, base_color);
		PushStyleColor(ImGuiCol_ButtonHovered, base_color * ImVec4(1.4f, 1.4f, 1.4f, 1.0f));
		PushStyleColor(ImGuiCol_ButtonActive, base_color * ImVec4(1.8f, 1.8f, 1.8f, 1.0f));

		PushStyleColor(ImGuiCol_Border, black_border
			? ImVec4(0, 0, 0, 1.0f)
			: GetStyleColorVec4(ImGuiCol_Border));
	}

	void Style_ColorButtonPop() {
		PopStyleColor(4);
	}

	void Style_InvisibleSelectorPush()
	{
		PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
		PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));
	}

	void Style_InvisibleSelectorPop() {
		PopStyleColor(2);
	}
}
