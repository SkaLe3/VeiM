#pragma once
#include "CoreDefines.h"
#include "Input/InputHandlerCommon.h"

namespace VeiM
{


	class Widget : public InputHandlerCommon
	{
	public:
		Widget(Widget* parent) : m_Parent(parent) { }

		template<typename WT,typename WP>
		UniquePtr<WT> CreateWidget(WP* parent)
		{
			m_Parent = parent;
			UniquePtr<WT> newWidget = MakeUnique<WT>(parent);
			m_Widgets.push_back(newWidget.get());
			return newWidget;
		}

	public:
		void ResetStatus();
		virtual void UpdateStatus();
		virtual bool IsEnabled();
		virtual bool IsEnabledFallback();

		virtual bool OnGUI();
		virtual bool OnKeyType(const InputKeyTypeEvent& keyTypeEvent) override;
		virtual bool OnKeyDown(const InputKeyEvent& keyEvent) override;
		virtual bool OnKeyUp(const InputKeyEvent& keyEvent)  override;
		virtual bool OnMouseUp(const InputMouseEvent& mouseEvent)  override;
		virtual bool OnMouseDown(const InputMouseEvent& mouseEvent)  override;
		virtual bool OnMouseDoubleClick(const InputMouseEvent& mouseEvent)  override;
		virtual bool OnMouseMove(const InputMouseEvent& mouseEvent)  override;
		virtual bool OnMouseWheel(const InputMouseEvent& mouseEvent)  override;
		virtual void OnFinishInput() override;
	protected:
		template<typename HandlerFunc>
		bool RouteEvent(HandlerFunc&& handler, bool condition)
		{
			bool bHandled = false;
			for (auto it : m_Widgets)
			{
				if (it == nullptr)
				{
					continue;
				}
				if (condition)
				{
					bHandled |= handler(it);
				}
			}
			return bHandled;
		}
		template<typename HandlerFunc>
		bool RouteEvent(HandlerFunc&& handler)
		{
			bool bHandled = false;
			for (auto it : m_Widgets)
			{
				if (it == nullptr)
				{
					continue;
				}
				if (it->IsEnabled())
				{
					bHandled |= handler(it);
				}
			}
			return bHandled;
		}
		template<typename HandleFunc>
		void RouteFunction(HandleFunc&& handler)
		{
			for (auto it : m_Widgets)
			{
				if (it == nullptr)
				{
					continue;
				}
				if (it->IsEnabled())
				{
					handler(it);
				}
			}
		}

	protected:
		std::vector<Widget*> m_Widgets;
		Widget* m_Parent;
		bool m_bIsFocused = false;
		bool m_bIsHovered = false;
	};
}