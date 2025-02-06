#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>
#include <memory>
#include <functional>

#include "UI/Image.h" 
#include "CoreDefines.h"


namespace VeiM::UI
{
	namespace Utils
	{
		CORE_API void SetCharArrayData(char* destinationLocation, size_t destinationSize, const String& sourceData);
		CORE_API String WStringToString(const std::wstring& wstr);
	}


	namespace Colors
	{
		static inline float Convert_sRGB_FromLinear(float theLinearValue)
		{
			return theLinearValue <= 0.0031308f
				? theLinearValue * 12.92f
				: glm::pow<float>(theLinearValue, 1.0f / 2.2f) * 1.055f - 0.055f;
		}

		static inline float Convert_sRGB_ToLinear(float thesRGBValue)
		{
			return thesRGBValue <= 0.04045f
				? thesRGBValue / 12.92f
				: glm::pow<float>((thesRGBValue + 0.055f) / 1.055f, 2.2f);
		}

		CORE_API inline ImVec4 ConvertFromSRGB(ImVec4 colour)
		{
			return ImVec4(Convert_sRGB_FromLinear(colour.x),
				Convert_sRGB_FromLinear(colour.y),
				Convert_sRGB_FromLinear(colour.z),
				colour.w);
		}

		CORE_API inline ImVec4 ConvertToSRGB(ImVec4 colour)
		{
			return ImVec4(std::pow(colour.x, 2.2f),
				glm::pow<float>(colour.y, 2.2f),
				glm::pow<float>(colour.z, 2.2f),
				colour.w);
		}

		CORE_API inline ImU32 ColorWithValue(const ImColor& color, float value)
		{
			const ImVec4& colRow = color.Value;
			float hue, sat, val;
			ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
			return ImColor::HSV(hue, sat, std::min(value, 1.0f));
		}

		CORE_API inline ImU32 ColorWithSaturation(const ImColor& color, float saturation)
		{
			const ImVec4& colRow = color.Value;
			float hue, sat, val;
			ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
			return ImColor::HSV(hue, std::min(saturation, 1.0f), val);
		}

		CORE_API inline ImU32 ColorWithHue(const ImColor& color, float hue)
		{
			const ImVec4& colRow = color.Value;
			float h, s, v;
			ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, h, s, v);
			return ImColor::HSV(std::min(hue, 1.0f), s, v);
		}

		CORE_API inline ImU32 ColorWithMultipliedValue(const ImColor& color, float multiplier)
		{
			const ImVec4& colRow = color.Value;
			float hue, sat, val;
			ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
			return ImColor::HSV(hue, sat, std::min(val * multiplier, 1.0f));
		}

		CORE_API inline ImU32 ColorWithMultipliedSaturation(const ImColor& color, float multiplier)
		{
			const ImVec4& colRow = color.Value;
			float hue, sat, val;
			ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
			return ImColor::HSV(hue, std::min(sat * multiplier, 1.0f), val);
		}

		CORE_API inline ImU32 ColorWithMultipliedHue(const ImColor& color, float multiplier)
		{
			const ImVec4& colRow = color.Value;
			float hue, sat, val;
			ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
			return ImColor::HSV(std::min(hue * multiplier, 1.0f), sat, val);
		}
	}

	CORE_API void ShiftCursorX(float distance);
	CORE_API void ShiftCursorY(float distance);
	CORE_API void ShiftCursor(float x, float y);

	CORE_API ImRect GetItemRect();
	CORE_API ImRect RectExpanded(const ImRect& rect, float x, float y);
	CORE_API ImRect RectOffset(const ImRect& rect, float x, float y);
	CORE_API ImRect RectOffset(const ImRect& rect, ImVec2 xy);

	CORE_API void DrawButtonImage(const std::shared_ptr<VeiM::Image>& imageNormal, const std::shared_ptr<VeiM::Image>& imageHovered, const std::shared_ptr<VeiM::Image>& imagePressed,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
		ImVec2 rectMin, ImVec2 rectMax);

	CORE_API void DrawButtonImage(const std::shared_ptr<VeiM::Image>& imageNormal, const std::shared_ptr<VeiM::Image>& imageHovered, const std::shared_ptr<VeiM::Image>& imagePressed,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
		ImRect rectangle);

	CORE_API void DrawButtonImage(const std::shared_ptr<VeiM::Image>& image,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
		ImVec2 rectMin, ImVec2 rectMax);

	CORE_API void DrawButtonImage(const std::shared_ptr<VeiM::Image>& image,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
		ImRect rectangle);

	CORE_API void DrawButtonImage(const std::shared_ptr<VeiM::Image>& imageNormal, const std::shared_ptr<VeiM::Image>& imageHovered, const std::shared_ptr<VeiM::Image>& imagePressed,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed);

	CORE_API void DrawButtonImage(const std::shared_ptr<VeiM::Image>& image,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed);

	CORE_API void RenderWindowOuterBorders(ImGuiWindow* window);

	CORE_API void InvisibleItem(std::string_view strID, const ImVec2& itemSize);

	// Menubar with custom rectangle
	CORE_API bool BeginMenubar(const ImRect& barRectangle);
	CORE_API void EndMenubar();

	CORE_API void TextCentered(const String& label);
	CORE_API bool ButtonCentered(const char* label, const ImVec2& size = ImVec2(0, 0));

	CORE_API bool ColorEditU32(const char* label, ImU32* color, ImGuiColorEditFlags flags = 0);
	// Parameters
	CORE_API bool BeginParameterTable(const char* label);
	CORE_API void EndParameterTable();

	CORE_API void ParameterRow(const char* label, float height, std::function<void()> buttons = nullptr);
	CORE_API bool ParameterSlider(const char* label, float* v, float min, float max, const char* format = "%.0f", ImGuiSliderFlags flags = 0);
	CORE_API bool ParameterSlider2(const char* label, float v[2], float min, float max, const char* format = "%.0f", ImGuiSliderFlags flags = 0);
	CORE_API bool ParameterCombo(const char* label, int* current_item, const char* items);
	CORE_API bool ParameterColorU32(const char* label, ImU32& color, ImGuiColorEditFlags flags = 0, std::function<void()> buttons = nullptr);
	CORE_API bool ParameterColor4(const char* label, ImVec4& color, ImGuiColorEditFlags flags = 0, std::function<void()> buttons = nullptr);

	CORE_API void HelpMarker(const char* desc);

	CORE_API bool ButtonSelectableFramed(const char* label, bool selected = false, const ImVec2& size_arg = ImVec2(0, 0), ImVec4 selection_color = ImVec4{ 1.f, 1.f, 1.f, 1.f });

	CORE_API void CondEnableWidget(const std::function<void()>& action, bool isEnabled);
}