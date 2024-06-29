#pragma once

#include "VeiM/Image.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <memory>
namespace VeiM::UI
{
	void ShiftCursorX(float distance);
	void ShiftCursorY(float distance);
	void ShiftCursor(float x, float y);

	ImRect GetItemRect();
	ImRect RectExpanded(const ImRect& rect, float x, float y);
	ImRect RectOffset(const ImRect& rect, float x, float y);
	ImRect RectOffset(const ImRect& rect, ImVec2 xy);

	void DrawButtonImage(const std::shared_ptr<VeiM::Image>& imageNormal, const std::shared_ptr<VeiM::Image>& imageHovered, const std::shared_ptr<VeiM::Image>& imagePressed,
						 ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
						 ImVec2 rectMin, ImVec2 rectMax);

	void DrawButtonImage(const std::shared_ptr<VeiM::Image>& imageNormal, const std::shared_ptr<VeiM::Image>& imageHovered, const std::shared_ptr<VeiM::Image>& imagePressed,
						 ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
						 ImRect rectangle);

	void DrawButtonImage(const std::shared_ptr<VeiM::Image>& image,
						 ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
						 ImVec2 rectMin, ImVec2 rectMax);

	void DrawButtonImage(const std::shared_ptr<VeiM::Image>& image,
						 ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
						 ImRect rectangle);

	void DrawButtonImage(const std::shared_ptr<VeiM::Image>& imageNormal, const std::shared_ptr<VeiM::Image>& imageHovered, const std::shared_ptr<VeiM::Image>& imagePressed,
						 ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed);

	void DrawButtonImage(const std::shared_ptr<VeiM::Image>& image,
						 ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed);

	void RenderWindowOuterBorders(ImGuiWindow* window);

	bool UpdateWindowManualResize(ImGuiWindow* window, ImVec2& newSize, ImVec2& newPosition);

	// Menubar with custom rectangle
	bool BeginMenubar(const ImRect& barRectangle);
	void EndMenubar();

	bool ButtonCentered(const char* label, const ImVec2& size = ImVec2(0, 0));
}