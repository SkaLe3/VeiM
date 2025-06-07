#include "Widget.h"
#include <imgui.h>

namespace VeiM
{

	void Widget::ResetStatus()
	{
		m_bIsFocused = false;
		m_bIsHovered = false;
	}

	void Widget::UpdateStatus()
	{
		m_bIsFocused |= ImGui::IsWindowFocused();
		m_bIsHovered |= ImGui::IsWindowHovered();
	}

	bool Widget::IsEnabled()
	{
		if (m_Widgets.empty())
		{
			return IsEnabledFallback();
		}
		else
		{
			for (auto it : m_Widgets)
			{
				if (it->IsEnabled())
				{
					return true;
				}
			}
			return IsEnabledFallback();
		}
	}

	bool Widget::IsEnabledFallback()
	{
		return m_bIsFocused || m_bIsHovered;
	}

	bool Widget::OnGUI()
	{
		ResetStatus();
		bool bResult = false;
		for (auto it : m_Widgets)
		{
			if (it)
			{
				bResult |= it->OnGUI();
			}
		}
		return bResult;
	}

	bool Widget::OnKeyType(const InputKeyTypeEvent& keyTypeEvent)
	{
		return RouteEvent([&](Widget* widget)
			{
				return widget->OnKeyType(keyTypeEvent);
			});
	}

	bool Widget::OnKeyDown(const InputKeyEvent& keyEvent)
	{
		return RouteEvent([&](Widget* widget)
			{
				return widget->OnKeyDown(keyEvent);
			});
	}

	bool Widget::OnKeyUp(const InputKeyEvent& keyEvent)
	{
		return RouteEvent([&](Widget* widget)
			{
				return widget->OnKeyUp(keyEvent);
			});
	}

	bool Widget::OnMouseUp(const InputMouseEvent& mouseEvent)
	{
		return RouteEvent([&](Widget* widget)
			{
				return widget->OnMouseUp(mouseEvent);
			});
	}

	bool Widget::OnMouseDown(const InputMouseEvent& mouseEvent)
	{
		return RouteEvent([&](Widget* widget)
			{
				return widget->OnMouseDown(mouseEvent);
			});
	}

	bool Widget::OnMouseDoubleClick(const InputMouseEvent& mouseEvent)
	{
		return RouteEvent([&](Widget* widget)
			{
				return widget->OnMouseDoubleClick(mouseEvent);
			});
	}

	bool Widget::OnMouseMove(const InputMouseEvent& mouseEvent)
	{
		return RouteEvent([&](Widget* widget)
			{
				return widget->OnMouseMove(mouseEvent);
			});
	}

	bool Widget::OnMouseWheel(const InputMouseEvent& mouseEvent)
	{
		return RouteEvent([&](Widget* widget)
			{
				return widget->OnMouseWheel(mouseEvent);
			});
	}

	void Widget::OnFinishInput()
	{
		RouteFunction([](Widget* widget)
			{
				widget->OnFinishInput();
			});
	}

}

