#pragma once
#include "CoreDefines.h"
#include "Input/Events.h"

namespace VeiM
{
	class CORE_API InputHandlerCommon
	{
	public:
		virtual bool OnKeyType(const InputKeyTypeEvent& keyTypeEvent) = 0;
		virtual bool OnKeyDown(const InputKeyEvent& keyEvent) = 0;
		virtual bool OnKeyUp(const InputKeyEvent& keyEvent) = 0;
		virtual bool OnMouseUp(const InputMouseEvent& mouseEvent) = 0;
		virtual bool OnMouseDown(const InputMouseEvent& mouseEvent) = 0;
		virtual bool OnMouseDoubleClick(const InputMouseEvent& mouseEvent) = 0;
		virtual bool OnMouseMove(const InputMouseEvent& mouseEvent) = 0;
		virtual bool OnMouseWheel(const InputMouseEvent& mouseEvent) = 0;
		virtual void OnFinishInput() = 0;
	};
}