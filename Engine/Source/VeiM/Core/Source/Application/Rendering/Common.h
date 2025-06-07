#pragma once
#include "CoreDefines.h"
#include "Input/InputHandlerCommon.h"

#include <glm/glm.hpp>

namespace VeiM
{
	enum class CORE_API EViewportDynamicRange : uint8
	{
		SDR, HDR
	};

	class CORE_API IApplicationViewport : InputHandlerCommon
	{
	public:
		virtual void OnDrawViewport() {}
		virtual glm::ivec2 GetSize() const = 0;
		virtual uint32 GetRenderTargetTexture() const = 0;
		virtual bool UseSeparateRenderTarget() const { return false; }
		virtual EViewportDynamicRange GetDynamicRange() { return EViewportDynamicRange::HDR; }

		/* InputHandlerCommon Interface */
		virtual bool OnKeyType(const InputKeyTypeEvent& keyTypeEvent) { return false; }
		virtual bool OnKeyDown(const InputKeyEvent& keyEvent) { return false; }
		virtual bool OnKeyUp(const InputKeyEvent& keyEvent) { return false; }
		virtual bool OnMouseUp(const InputMouseEvent& mouseEvent) { return false; }
		virtual bool OnMouseDown(const InputMouseEvent& mouseEvent) { return false; }
		virtual bool OnMouseDoubleClick(const InputMouseEvent& mouseEvent) { return false; }
		virtual bool OnMouseMove(const InputMouseEvent& mouseEvent) { return false; }
		virtual bool OnMouseWheel(const InputMouseEvent& mouseEvent) { return false; }
		virtual void OnFinishInput(){}
		/*      */

		virtual void OnViewportClosed() {}

	};
}