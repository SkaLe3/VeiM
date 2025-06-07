#include "LevelEditorWidgets.h"

#include <imgui_internal.h>

namespace VeiM::UI
{

	bool TreeBehavior(const char* str_id, float triangle_scale)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;

		ImGui::PushID(str_id);

		bool *bOpen = window->DC.StateStorage->GetBoolRef(ImGui::GetID("##TreeOpenState"), false);

		const float h = window->DrawList->_Data->FontSize;

		const float triangle_size = h * 0.8f * triangle_scale;
		const float button_size = triangle_size * 1.2f;
		const ImVec2 triangle_pos = (glm::vec2)window->DC.CursorPos + (glm::vec2)ImVec2((button_size - triangle_size) * 0.5f, (h - triangle_size) * 0.5f);

		ImGui::InvisibleButton("##triangle", ImVec2(button_size, button_size));
		bool hovered = ImGui::IsItemHovered();
		bool clicked = ImGui::IsItemClicked();


		ImU32 triangle_col = hovered ? IM_COL32(150, 150, 150, 255) : IM_COL32(100, 100, 100, 255);

		ImGui::RenderArrow(window->DrawList, triangle_pos, triangle_col,*bOpen ? ImGuiDir_Down : ImGuiDir_Right, triangle_scale);
		
		if (clicked)
		{
			*bOpen = !*bOpen;
		}
		ImGui::SameLine();
		ImGui::SetCursorPosX(0);
		ImGui::PopID();
		return *bOpen;
	}

}

