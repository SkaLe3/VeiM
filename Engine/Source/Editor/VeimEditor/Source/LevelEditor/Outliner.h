#pragma once
#include "CoreDefines.h"
#include "Widgets/Widget.h"

#include <glm/glm.hpp>

namespace VeiM
{
	class Entity;
	class Component;
	class SceneComponent;
	class LevelEditor;

	class Outliner : public Widget
	{
	public:
		Outliner(LevelEditor* levelEditor);
		virtual bool OnGUI() override;
		void OnWindowMenu();

		void DropSelectedEntity();
	private:
		void RenderOutliner();
		void RenderDetailsPanel();
		void RenderEntityHierarchy();
		void RenderEntityHierarchyListBoxEntityNode();
		void RenderEntityHierarchyTraversal(SceneComponent* rootComp, Component** selectedComponent, float offset);

		void SelectEntity(Entity* newSelectedEntity);

		void DrawVec3Control(const char* label, glm::vec3& values,bool* bCheckbox = nullptr, float resetValue = 0.0f, float columnWidth = 100.f);

	private:
		bool m_bOpenOutliner = true;
		bool m_bOpenDetails = true;
		LevelEditor* m_LevelEditor;


		Entity* m_SelectedEntity = nullptr;
		int32 m_SelectedEntityIdx = -1;
		bool m_bIsEntitySelectedInHierarchy = true;
		Component* m_SelectedComponentInHierarchy = nullptr;

		char m_SearchComponentQuery[128];
		float m_ListBoxHeight = 100;
	};
}