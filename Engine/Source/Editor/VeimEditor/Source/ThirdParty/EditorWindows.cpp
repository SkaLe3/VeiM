#include "ThirdParty/EditorWindows.h"
#include "Application/Application.h"
#include <imgui.h>


void ImGuiWindows::ShowFpsWindow(bool* p_open)
{
	ImGuiIO& io = ImGui::GetIO();
	ImGui::Begin("Fps", p_open);

	ImGui::Text("Application average (ImGui) %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	ImGui::Text("Application average (VeiM)  %.3f ms/frame (%.1f FPS)", VeiM::Application::Get().GetDeltaTime() * 1000.f, 1.0f/ VeiM::Application::Get().GetDeltaTime());
	ImGui::End();
}
