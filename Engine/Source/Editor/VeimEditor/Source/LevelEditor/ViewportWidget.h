#pragma once
#include "CoreDefines.h"
#include "Widgets/Widget.h"

#include <glm/glm.hpp>
#include <imgui.h>

namespace VeiM
{
	class NativeViewport;

	// TODO: Make NativeViewport class which will be used only for game build and will hold scene viewport
	class ViewportWidget : public Widget
	{
	public:

	public:
		ViewportWidget(Widget* parent);
		virtual bool OnGUI() override;

		virtual bool OnMouseDown(const InputMouseEvent& mouseEvent) override;
		virtual bool OnKeyDown(const InputKeyEvent& keyEvent) override;
		virtual bool OnKeyUp(const InputKeyEvent& keyEvent) override;
		virtual bool OnMouseMove(const InputMouseEvent& mouseEvent) override;
		virtual void OnFinishInput() override;
		virtual bool IsEnabledFallback() override;

		SharedPtr<NativeViewport> GetViewport();

	private:
		void ForwardViewportInput();

	private:
		glm::vec2 m_ViewportBounds[2];
		glm::vec2 m_ViewportSize;
		glm::vec2 m_MouseDeltaTemp;
		bool m_bHasMouseInput = false;

		SharedPtr<NativeViewport> m_NativeViewport;
		// Here goes pointer to scene viewport
		//	Scene viewport holds viewport client
		//	Scene viewport holds rendertexture. Set method for it called in editorengine tick from game viewport
		//		then OnGUI of current class or draw of NativeViewport called to call draw texture, or blit 
	};
}