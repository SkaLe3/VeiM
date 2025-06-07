#pragma once
#include "CoreDefines.h"
#include "LevelEditor/ViewportWidget.h"
#include "LevelEditor/Outliner.h"
#include "Widgets/Widget.h"


namespace VeiM
{
	class ClassDescriptor;
	class Entity;
	class NativeViewport;

	class LevelEditor : public Widget
	{
	public:
		LevelEditor(Widget* parent);
		bool OnGUI();
		bool OnToolBar(float height);
		void OnWindowMenu();
	private:
		void RequestBeginPlay();
		void RequestEndPlay();

		void PlaceEntity(ClassDescriptor* entityClass); // TODO: Add transform
	public:
		void DeleteEntity(Entity* entity);
		void AddComponentToEntity(Entity* entity, ClassDescriptor* componentClass);

	private:
		SharedPtr<NativeViewport> GetViewport();
	private:
		UniquePtr<ViewportWidget> m_Viewport;
		UniquePtr<Outliner> m_Outliner;
		std::unordered_map<ClassDescriptor*, uint32> m_ClassCountMap;

		char m_SerachEntityQuery[128];
	};
}