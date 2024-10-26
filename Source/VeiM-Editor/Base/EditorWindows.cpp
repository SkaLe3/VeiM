#include "EditorWindows.h"

#include <imgui/imgui.h>


void ImGuiWindows::ShowFpsWindow(bool* p_open)
{
	ImGuiIO& io = ImGui::GetIO();
	ImGui::Begin("Fps", p_open);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	ImGui::End();
}
