#pragma once
#include "CoreDefines.h"

#include <glm/glm.hpp>

struct GLFWwindow;

namespace VeiM
{
	enum class CORE_API EInputEventType : uint32
	{
		InvalidInput = 0,
		KeyInput,
		CharInput,
		MouseButtonInput,
		MouseScrollInput,
		MouseMoveInput,
		WindowSizeInput,
		WindowCloseInput,
		WindowIconifyInput
	};

	struct CORE_API InputEventInternal
	{
		GLFWwindow* NativeWindow;
		EInputEventType EventType;

		InputEventInternal(EInputEventType eventType) : EventType(eventType), NativeWindow(nullptr) {}
	};

	struct CORE_API KeyEventInternal : public InputEventInternal
	{
		int32 Action;
		int32 KeyCode;
		int32 ScanCode;

		KeyEventInternal() : InputEventInternal(EInputEventType::KeyInput){}

	};
	struct CORE_API KeyTypeEventInternal : public InputEventInternal
	{
		uint32 TypeKeyCode;

		KeyTypeEventInternal() : InputEventInternal(EInputEventType::CharInput) {}
	};

	struct CORE_API MouseButtonEventInternal : public InputEventInternal
	{
		int32 Action;
		int32 ButtonCode;

		MouseButtonEventInternal() : InputEventInternal(EInputEventType::MouseButtonInput) {}
	};

	struct CORE_API MouseScrollEventInternal : public InputEventInternal
	{
		double XOffset;
		double YOffset;

		MouseScrollEventInternal() : InputEventInternal(EInputEventType::MouseScrollInput) {}
	};

	struct CORE_API MouseMoveEventInternal : public InputEventInternal
	{
		double XPos;
		double YPos;

		MouseMoveEventInternal() : InputEventInternal(EInputEventType::MouseMoveInput) {}
	};

	struct CORE_API WindowSizeEventInternal : public InputEventInternal
	{
		double Width;
		double Height;

		WindowSizeEventInternal() : InputEventInternal(EInputEventType::WindowSizeInput) {}
	};

	struct CORE_API WindowCloseEventInternal : public InputEventInternal
	{
		WindowCloseEventInternal() :InputEventInternal(EInputEventType::WindowCloseInput) {}
	};

	struct CORE_API WindowIconifyEventInternal : public InputEventInternal
	{
		int32 Iconified;

		WindowIconifyEventInternal() : InputEventInternal(EInputEventType::WindowIconifyInput) {}
	};

	class CORE_API InputHandler
	{
	public: 
		virtual ~InputHandler() {}
		virtual bool OnKeyType(const uint32 character)
		{
			return false;
		}
		virtual bool OnKeyDown(const int32 keyCode, const int32 scanCode, const bool bRepeat)
		{
			return false;
		}
		virtual bool OnKeyUp(const int32 keyCode, const int32 scanCode)
		{
			return false;
		}
		virtual bool OnMouseDown(const int32 button, const glm::vec2& cursorPos)
		{
			return false;
		}
		virtual bool OnMouseUp(const int32 button, const glm::vec2& cursorPos)
		{
			return false;
		}
		virtual bool OnMouseDoubleClick(const int32 button)
		{
			return false;
		}
		virtual bool OnMouseDoubleClick(const int32 button, const glm::vec2& cursorPos)
		{
			return false;
		}
		virtual bool OnMouseWheel(const float offset, const glm::vec2& cursorPos)
		{
			return false;
		}
		virtual bool OnMouseMove(const int32 x, const int32 y)
		{
			return false;
		}
		virtual bool OnCursorSet()
		{
			return false;
		}
		virtual bool OnSizeChanged(const int32 width, const int32 height)
		{
			return false;
		}
		virtual bool OnClose()
		{
			return false;
		}
		virtual bool OnMinimized()
		{
			return false;
		}
		virtual bool OnRestored()
		{
			return false;
		}
		// TODO: Add other events
		// TODO: Add Gamepad support
		
	};
}