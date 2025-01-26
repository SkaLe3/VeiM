#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>
#include <memory>
#include <functional>


#include "VeiM/Core/Image.h" 
#include "VeiM/Core/CoreDefines.h"


namespace VeiM::UI
{
	namespace Colors
	{

		static inline float Convert_sRGB_FromLinear(float theLinearValue);
		static inline float Convert_sRGB_ToLinear(float thesRGBValue);
		ImVec4 ConvertFromSRGB(ImVec4 colour);
		ImVec4 ConvertToSRGB(ImVec4 colour);

	}

	namespace Colors
	{
		inline float Convert_sRGB_FromLinear(float theLinearValue)
		{
			return theLinearValue <= 0.0031308f
				? theLinearValue * 12.92f
				: glm::pow<float>(theLinearValue, 1.0f / 2.2f) * 1.055f - 0.055f;
		}

		inline float Convert_sRGB_ToLinear(float thesRGBValue)
		{
			return thesRGBValue <= 0.04045f
				? thesRGBValue / 12.92f
				: glm::pow<float>((thesRGBValue + 0.055f) / 1.055f, 2.2f);
		}

		inline ImVec4 ConvertFromSRGB(ImVec4 colour)
		{
			return ImVec4(Convert_sRGB_FromLinear(colour.x),
						  Convert_sRGB_FromLinear(colour.y),
						  Convert_sRGB_FromLinear(colour.z),
						  colour.w);
		}

		inline ImVec4 ConvertToSRGB(ImVec4 colour)
		{
			return ImVec4(std::pow(colour.x, 2.2f),
						  glm::pow<float>(colour.y, 2.2f),
						  glm::pow<float>(colour.z, 2.2f),
						  colour.w);
		}

		inline ImU32 ColorWithValue(const ImColor& color, float value)
		{
			const ImVec4& colRow = color.Value;
			float hue, sat, val;
			ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
			return ImColor::HSV(hue, sat, std::min(value, 1.0f));
		}

		inline ImU32 ColorWithSaturation(const ImColor& color, float saturation)
		{
			const ImVec4& colRow = color.Value;
			float hue, sat, val;
			ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
			return ImColor::HSV(hue, std::min(saturation, 1.0f), val);
		}

		inline ImU32 ColorWithHue(const ImColor& color, float hue)
		{
			const ImVec4& colRow = color.Value;
			float h, s, v;
			ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, h, s, v);
			return ImColor::HSV(std::min(hue, 1.0f), s, v);
		}

		inline ImU32 ColorWithMultipliedValue(const ImColor& color, float multiplier)
		{
			const ImVec4& colRow = color.Value;
			float hue, sat, val;
			ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
			return ImColor::HSV(hue, sat, std::min(val * multiplier, 1.0f));
		}

		inline ImU32 ColorWithMultipliedSaturation(const ImColor& color, float multiplier)
		{
			const ImVec4& colRow = color.Value;
			float hue, sat, val;
			ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
			return ImColor::HSV(hue, std::min(sat * multiplier, 1.0f), val);
		}

		inline ImU32 ColorWithMultipliedHue(const ImColor& color, float multiplier)
		{
			const ImVec4& colRow = color.Value;
			float hue, sat, val;
			ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
			return ImColor::HSV(std::min(hue * multiplier, 1.0f), sat, val);
		}
	}

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

	void InvisibleItem(std::string_view strID, const ImVec2& itemSize);

	// Menubar with custom rectangle
	bool BeginMenubar(const ImRect& barRectangle);
	void EndMenubar();

	void TextCentered(const String& label);
	bool ButtonCentered(const char* label, const ImVec2& size = ImVec2(0, 0));

	bool ColorEditU32(const char* label, ImU32* color, ImGuiColorEditFlags flags = 0);
	// Parameters
	bool BeginParameterTable(const char* label);
	void EndParameterTable();

	void ParameterRow(const char* label, float height, std::function<void()> buttons = nullptr);
	bool ParameterSlider(const char* label, float* v, float min, float max, const char* format = "%.0f", ImGuiSliderFlags flags = 0);
	bool ParameterSlider2(const char* label, float v[2], float min, float max, const char* format = "%.0f", ImGuiSliderFlags flags = 0);
	bool ParameterCombo(const char* label, int* current_item, const char* items);
	bool ParameterColorU32(const char* label, ImU32& color, ImGuiColorEditFlags flags = 0, std::function<void()> buttons = nullptr);
	bool ParameterColor4(const char* label, ImVec4& color, ImGuiColorEditFlags flags = 0, std::function<void()> buttons = nullptr);

	void HelpMarker(const char* desc);

	bool ButtonSelectableFramed(const char* label, bool selected = false, const ImVec2& size_arg = ImVec2(0, 0), ImVec4 frame_color = ImVec4{ 1.f, 1.f, 1.f, 1.f });
}