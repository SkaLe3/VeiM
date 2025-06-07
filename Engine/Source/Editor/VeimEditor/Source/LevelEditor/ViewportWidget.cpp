#include "ViewportWidget.h"
#include "Application/Application.h"



#include "Editor/Editor.h"
#include "Engine/Classes/Editor/EditorEngine.h"
#include "Engine/World.h"
#include "Engine/GlobalGameState.h"
#include "Engine/Player.h"
#include "Engine/Classes/Game/InputManager.h"
#include "Engine/Controller.h"

#include "Application/Rendering/NativeViewport.h"

namespace VeiM
{

	ViewportWidget::ViewportWidget(Widget* parent) : Widget(parent)
	{
		m_MouseDeltaTemp = { 0.0f, 0.0f };
		m_bHasMouseInput = false;
	}

	bool ViewportWidget::OnGUI()
	{
		Widget::OnGUI();

		ForwardViewportInput();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		UpdateStatus();
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		//Application::Get().GetGUIContext()->BlockEvents(!m_bHovered);
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		Application::Get().ViewportResize(m_ViewportSize.x, m_ViewportSize.y);
		ImGui::Image(reinterpret_cast<void*>(Application::Get().DebugGetFramebuffer()->GetTexture()), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		//ImGui::Image(reinterpret_cast<void*>(Application::Get().DebugGetFramebufferTexture()), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::End();
		ImGui::PopStyleVar();
		return true;
	}

	bool ViewportWidget::OnMouseDown(const InputMouseEvent& mouseEvent)
	{
		bool bHandled = false;
		bHandled = Widget::OnMouseDown(mouseEvent);
		VM_CORE_TRACE("MouseDown in Viewport");
		return true;
	}

	bool ViewportWidget::OnKeyDown(const InputKeyEvent& keyEvent)
	{
		bool bHandled = false;
		bHandled = Widget::OnKeyDown(keyEvent);
		if (bHandled)
			return bHandled;
		static int32 count = 0;
		
		if (count > 0)
		{
			int a = 4;
		}

		World* world = g_Editor->GetPlayWorld();
		if (world)
		{
			 Controller* controller = world->GetGlobalGameState()->GetPlayers()[0]->PlayerController.Get();
			 if (controller)
			 {
				 const KeyParams params(keyEvent.GetKey(), keyEvent.IsRepeat() ? EInputEvent::Repeat : EInputEvent::Pressed, { 1.0f, 0.0f });
				 bHandled = controller->InputKey(params);
			 }

		}
		count++;
		return bHandled;
	}

	bool ViewportWidget::OnKeyUp(const InputKeyEvent& keyEvent)
	{
		bool bHandled = false;
		bHandled = Widget::OnKeyUp(keyEvent);
		if (bHandled)
			return bHandled;

		World* world = g_Editor->GetPlayWorld();
		if (world)
		{
			Controller* controller = world->GetGlobalGameState()->GetPlayers()[0]->PlayerController.Get();
			if (controller)
			{
				const KeyParams params(keyEvent.GetKey(), EInputEvent::Released, { 1.0f, 0.0f });
				bHandled = controller->InputKey(params);
			}

		}

		return bHandled;
	}

	bool ViewportWidget::OnMouseMove(const InputMouseEvent& mouseEvent)
	{
		bool bHandled = false;
		bHandled = Widget::OnMouseMove(mouseEvent);
		if (bHandled)
			return bHandled;

		m_MouseDeltaTemp.x += mouseEvent.GetOffset().x;
		m_MouseDeltaTemp.y += mouseEvent.GetOffset().y;

		m_bHasMouseInput = true;

		return bHandled;
	}

	void ViewportWidget::OnFinishInput()
	{

		if (m_bHasMouseInput)
		{
			World* world = g_Editor->GetPlayWorld();
			if (world)
			{
				Controller* controller = world->GetGlobalGameState()->GetPlayers()[0]->PlayerController.Get();
				if (controller)
				{
					const KeyParams paramsX(IKey::MouseX, m_MouseDeltaTemp.x);
					const KeyParams paramsY(IKey::MouseY, m_MouseDeltaTemp.y);
					controller->InputKey(paramsX);
					controller->InputKey(paramsY);
				}

			}
		}
		m_MouseDeltaTemp = Math::Utils::ZeroVector2d;
		m_bHasMouseInput = false;
	}

	bool ViewportWidget::IsEnabledFallback()
	{
		return m_bIsFocused;
	}

	SharedPtr<NativeViewport> ViewportWidget::GetViewport()
	{
		if (m_NativeViewport)
			return m_NativeViewport;
		return m_NativeViewport = MakeShared<NativeViewport>();
	}

	void ViewportWidget::ForwardViewportInput()
	{
		ImGuiIO& io = ImGui::GetIO();

	}

}

