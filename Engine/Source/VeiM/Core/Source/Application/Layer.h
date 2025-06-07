#pragma once
#include "CoreDefines.h"
#include "Input/InputHandlerCommon.h"

namespace VeiM
{
	class CORE_API Layer : public InputHandlerCommon
	{
	public:
		Layer(const String& name = "Layer")
			: m_Name(name) {
		}
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnUpdateGUI() {}

		virtual bool OnKeyType(const InputKeyTypeEvent& keyTypeEvent) override { return false; }
		virtual bool OnKeyDown(const InputKeyEvent& keyEvent) override { return false; }
		virtual bool OnKeyUp(const InputKeyEvent& keyEvent) override { return false; }
		virtual bool OnMouseUp(const InputMouseEvent& mouseEvent) override { return false; }
		virtual bool OnMouseDown(const InputMouseEvent& mouseEvent) override { return false; }
		virtual bool OnMouseDoubleClick(const InputMouseEvent& mouseEvent) override { return false; }
		virtual bool OnMouseMove(const InputMouseEvent& mouseEvent) override { return false; }
		virtual bool OnMouseWheel(const InputMouseEvent& mouseEvent) override { return false; }

		virtual void OnFinishInput() override {}

		inline const String& GetName() const { return m_Name; }
	protected:
		String m_Name;
	};
}