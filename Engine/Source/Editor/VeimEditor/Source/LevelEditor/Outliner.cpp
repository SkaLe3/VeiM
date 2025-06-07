#include "Outliner.h"

#include "Engine/Entity.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Engine/CoreObjectStatics.h"

#include "LevelEditor/LevelEditor.h"
#include "LevelEditor/LevelEditorWidgets.h"
#include "UI/UI.h"
#include <imgui.h>

namespace VeiM
{


	Outliner::Outliner(LevelEditor* levelEditor) 
		: Widget(levelEditor) 
		, m_LevelEditor(levelEditor)
	{
		UI::Utils::SetCharArrayData(m_SearchComponentQuery, 128, "");
	}

	bool Outliner::OnGUI()
	{
		Widget::OnGUI();

		if (m_bOpenOutliner)
			RenderOutliner();
		if (m_bOpenDetails)
			RenderDetailsPanel();
		return true;
	}

	void Outliner::OnWindowMenu()
	{
		ImGui::MenuItem("Outliner", "", &m_bOpenOutliner);
		ImGui::MenuItem("Details", "", &m_bOpenDetails);
	}

	void Outliner::DropSelectedEntity()
	{
		SelectEntity(nullptr);
		m_SelectedEntityIdx = -1;
	}

	void Outliner::RenderOutliner()
	{
		ImVec2 originalPadding = ImGui::GetStyle().WindowPadding;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
		ImGui::Begin("Outliner", &m_bOpenOutliner);
		UpdateStatus();
		// Here Search

		float bottomBarHeight = 28;
		float availableHeight = ImGui::GetContentRegionAvail().y - bottomBarHeight;
		std::vector<ObjectPtr<Entity>>& entities = g_World->CurrentLevel->Entities;

		if (ImGui::BeginListBox("##EntityList", ImVec2(-FLT_MIN, availableHeight)))
		{
			// TODO: Use table
			for (int i = 0; i < entities.size(); i++)
			{
				if (!entities[i])
				{
					continue;
				}
				bool bIsSelected = (m_SelectedEntityIdx == i);
				if (ImGui::Selectable(entities[i]->GetLabel().data(), bIsSelected))
				{
					if (bIsSelected)
					{
						m_SelectedEntityIdx = -1;
						SelectEntity(nullptr);
					}
					else
					{
						m_SelectedEntityIdx = i;
						SelectEntity(entities[i].Get());
					}
				}
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text("Entity %s tooltip", entities[i]->GetLabel().data());
					ImGui::EndTooltip();
				}

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, originalPadding);
				bool bDeleted = false;
				if (ImGui::BeginPopupContextItem())
				{
					if (m_SelectedEntityIdx != i)
					{
						m_SelectedEntityIdx = i;
						SelectEntity(entities[i].Get());
					}
					ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
					ImGui::SeparatorText("Options");
					ImGui::PopStyleColor();
					if (ImGui::MenuItem("Delete"))
					{
						bDeleted = true;
					}
					ImGui::EndPopup();
				}
				ImGui::PopStyleVar();

				if (bDeleted)
				{
					m_SelectedEntityIdx = -1;
					SelectEntity(nullptr);
					m_LevelEditor->DeleteEntity(entities[i].Get());
				}

			}
			ImGui::EndListBox();
		}
		float textSize = ImGui::GetTextLineHeight();
		float padding = (bottomBarHeight - textSize) * 0.5f;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().ItemSpacing.y);
		UI::ShiftCursorY(padding);
		UI::ShiftCursorX(10);

		if (m_SelectedEntityIdx >= 0 && m_SelectedEntityIdx < entities.size())
		{
			ImGui::Text("%d Entities (Selected: %s)", entities.size(), entities[m_SelectedEntityIdx]->GetLabel().data());
		}
		else
		{
			ImGui::Text("%d Entities", entities.size());
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void Outliner::RenderDetailsPanel()
	{
		ImGui::Begin("Details", &m_bOpenDetails);
		UpdateStatus();
		if (ImGui::IsWindowFocused()) {
			
		}

		if (!m_SelectedEntity)
		{
			ImGui::End();
			return;
		}

		ClassDescriptor* classDesc = m_SelectedEntity->GetClass();
		ImVec2 addButtonSize = { 80, ImGui::GetTextLineHeightWithSpacing() };
		ImGui::Text("%s", m_SelectedEntity->GetLabel().data());
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - addButtonSize.x - 48);
		if (ImGui::Button("Add##Component", addButtonSize))
		{
			ImGui::OpenPopup("AddComponent");
		}

		ImVec2 cursorCoords = UI::Utils::GetNormalizedNextWidgetPos();
		ImVec2 buttonPos = ImGui::GetItemRectMin();
		ImGui::SetNextWindowSize(ImVec2(200, 400));
		if (cursorCoords.y > 0.5)
		{
			ImGui::SetNextWindowPos(ImVec2(buttonPos.x - (200 - addButtonSize.x) * 0.5, buttonPos.y - 400));
		}
		else
		{
			ImGui::SetNextWindowPos(ImVec2(buttonPos.x - (200 - addButtonSize.x) * 0.5, buttonPos.y + ImGui::GetItemRectSize().y));
		}
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(3, 3));
		if (ImGui::BeginPopup("AddComponent", ImGuiWindowFlags_NoMove))
		{
			ImGui::SetNextItemWidth(-1);
			ImGui::InputTextWithHint("##FilterComponentsToAdd", "Component class...", m_SearchComponentQuery, IM_ARRAYSIZE(m_SearchComponentQuery));

			if (ImGui::BeginListBox("##ComponentList", ImVec2(-FLT_MIN, -FLT_MIN)))
			{
				std::map<StringID, ClassDescriptor*, StringIDComparator> allComponents = ClassRegistry::GetAllClassesOfClass<Component>();
				for (auto& [eName, eClass] : allComponents)
				{
					if (ImGui::MenuItem(eName.Get()))
					{
						ClassDescriptor* componentClass = ClassRegistry::FindClass(eName);
						if (componentClass)
						{
							m_LevelEditor->AddComponentToEntity(m_SelectedEntity, componentClass);
							ImGui::CloseCurrentPopup();
						}
					}
				}
				ImGui::EndListBox();
			}
			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();

		RenderEntityHierarchy();


		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
		if (ImGui::TreeNodeEx("Transform", treeNodeFlags))
		{
			SceneComponent* transformComponent = CastObject<SceneComponent>(m_SelectedComponentInHierarchy);
			if (m_bIsEntitySelectedInHierarchy)
			{
				transformComponent = m_SelectedEntity->GetRootComponent();
			}
			if (transformComponent)
			{
				bool bAbsoluteTranslation = transformComponent->IsAbsoluteLocation();
				bool bAbsoluteRotation = transformComponent->IsAbsoluteRotation();
				bool bAbsoluteScale = transformComponent->IsAbsoluteScale();

				glm::vec3 translation = transformComponent->GetRelativeLocation();
				glm::vec3 rotation = transformComponent->GetRelativeRotation();
				glm::vec3 scale = transformComponent->GetRelativeScale();

				DrawVec3Control("Location", translation, &bAbsoluteTranslation);
				DrawVec3Control("Rotation", rotation, &bAbsoluteRotation);
				DrawVec3Control("Scale", scale, &bAbsoluteScale, 1.0f);

				if (translation != transformComponent->GetRelativeLocation()
					|| rotation != transformComponent->GetRelativeRotation()
					|| scale != transformComponent->GetRelativeScale())
				{
					Transform newTransform(rotation, translation, scale);
					transformComponent->SetRelativeTransform(newTransform);
				}

				transformComponent->SetUseAbsoluteLocation(bAbsoluteTranslation);
				transformComponent->SetUseAbsoluteRotation(bAbsoluteRotation);
				transformComponent->SetUseAbsoluteScale(bAbsoluteScale);

			}

			ImGui::TreePop();
		}

		// First goes transform
		// Then everything else

		ImGui::End();
	}

	void Outliner::RenderEntityHierarchy()
	{

		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::BeginChild("##EntityStructureListBoxContainer", ImVec2(-FLT_MIN, m_ListBoxHeight), true, ImGuiWindowFlags_NoScrollbar);

		if (ImGui::BeginListBox("##EntityStructure", ImVec2(-FLT_MIN, m_ListBoxHeight - 5)))
		{
			RenderEntityHierarchyListBoxEntityNode();
			if (m_bIsEntitySelectedInHierarchy)
			{
				m_SelectedComponentInHierarchy = nullptr;
			}
			SceneComponent* rootComp = m_SelectedEntity->GetRootComponent();
			if (rootComp)
			{
				RenderEntityHierarchyTraversal(rootComp, &m_SelectedComponentInHierarchy, 30);
			}
			ImGui::Separator();
			std::vector<Component*> entityComponents;
			m_SelectedEntity->GetComponents(entityComponents);

			for (int i = 0; i < entityComponents.size(); i++)
			{
				//ClassDescriptor* compClass = entityComponents[i]->GetClass();

				if (entityComponents[i]->GetClass()->IsChildOf(SceneComponent::StaticClass()))
				{
					continue;
				}
				String nodeName = entityComponents[i]->GetName();
				String typeName = entityComponents[i]->GetClass()->Name.ToString();
				float typeNameLength = ImGui::CalcTextSize(typeName.data()).x;
				float availWidth = ImGui::GetContentRegionAvail().x;

				bool bIsSelected = (m_SelectedComponentInHierarchy == entityComponents[i]);
				ImGui::Selectable(("##invisible_selectable_comp" + nodeName + std::to_string(i)).data(), bIsSelected);

				if (ImGui::IsItemClicked()) {
					m_SelectedComponentInHierarchy = entityComponents[i];
				}

				ImGui::SameLine(20);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
				ImGui::Text(nodeName.data());
				ImGui::PopStyleColor();

				ImGui::SameLine(availWidth - typeNameLength);
				ImGui::TextDisabled("%s", typeName.data());

				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text("Component %s tooltip", entityComponents[i]->GetName().data());
					ImGui::EndTooltip();
				}
			}
			if (m_SelectedComponentInHierarchy != nullptr)
			{
				m_bIsEntitySelectedInHierarchy = false;
			}
			ImGui::EndListBox();
		}
		UI::ShiftCursorY(-4);
		ImGui::InvisibleButton("##EntityStructureResizeHandle", ImVec2(-FLT_MIN, 5.0f));

		const ImU32 baseColor = ImGui::GetColorU32(ImGuiCol_Border);
		const ImU32 hoverColor = ImGui::GetColorU32(ImGuiCol_ResizeGripHovered);
		const ImU32 activeColor = ImGui::GetColorU32(ImGuiCol_ResizeGripHovered);
		ImU32 lineColor = baseColor;
		if (ImGui::IsItemHovered()) lineColor = hoverColor;
		if (ImGui::IsItemActive()) lineColor = activeColor;

		// Draw the line
		ImVec2 p_min = ImGui::GetItemRectMin();
		ImVec2 p_max = ImGui::GetItemRectMax();
		ImGui::GetWindowDrawList()->AddLine(
			ImVec2(p_min.x, p_min.y + 2.5f),
			ImVec2(p_max.x, p_min.y + 2.5f),
			lineColor,
			2.0f);

		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
		if (ImGui::IsItemActive())
		{
			m_ListBoxHeight += ImGui::GetIO().MouseDelta.y;
			m_ListBoxHeight = ImClamp(m_ListBoxHeight, 50.0f, 500.0f); // Min/Max constraints
		}

		ImGui::EndChild();
		ImGui::PopStyleVar(2);

	}

	void Outliner::RenderEntityHierarchyListBoxEntityNode()
	{
		if (ImGui::Selectable(m_SelectedEntity->GetLabel().data(), m_bIsEntitySelectedInHierarchy))
		{
			m_bIsEntitySelectedInHierarchy = true;
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Entity %s tooltip", m_SelectedEntity->GetLabel().data());
			ImGui::EndTooltip();
		}
	}

	void Outliner::RenderEntityHierarchyTraversal(SceneComponent* rootComp, Component** selectedComponent, float offset)
	{
		if (!rootComp)
		{
			return;
		}

		std::vector<ObjectPtr<SceneComponent>> childrenComps = rootComp->GetAttachChildren();

		bool bIsSelected = (*selectedComponent == rootComp);
		bool bIsTreeNode = !childrenComps.empty();

		String nodeName = rootComp->GetName();
		String typeName = rootComp->GetClass()->Name.ToString();
		float typeNameLength = ImGui::CalcTextSize(typeName.data()).x;
		float availWidth = ImGui::GetContentRegionAvail().x;

		bool bIsOpen = false;
		if (bIsTreeNode)
		{
			ImGui::SetCursorPosX(offset - 15);
			bIsOpen = UI::TreeBehavior(("##TreeNode" + std::to_string((uint64)rootComp)).data(), 0.8f);
		}

		ImGui::Selectable(("##invisible_selectable_" + nodeName + std::to_string((uint64)rootComp)).data(), bIsSelected);

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text(("Component %s tooltip##" + std::to_string((uint64)rootComp)).data(), rootComp->GetName().data());
			ImGui::EndTooltip();
		}

		if (ImGui::IsItemClicked()) {
			*selectedComponent = rootComp;
		}
		ImGui::SameLine();
		ImGui::SetCursorPosX(offset);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		ImGui::Text(nodeName.data());
		ImGui::PopStyleColor();

		ImGui::SameLine(availWidth - typeNameLength);
		ImGui::TextDisabled("%s", typeName.c_str());

		if (bIsOpen && bIsTreeNode)
		{
			for (size_t i = 0; i < childrenComps.size(); i++)
			{
				RenderEntityHierarchyTraversal(childrenComps[i].Get(), selectedComponent, offset + 15);
			}
		}
	}

	void Outliner::SelectEntity(Entity* newSelectedEntity)
	{
		m_SelectedEntity = newSelectedEntity;
		m_SelectedComponentInHierarchy = nullptr;
		m_bIsEntitySelectedInHierarchy = true;
	}


	void Outliner::DrawVec3Control(const char* label, glm::vec3& values, bool* bCheckbox, float resetValue, float columnWidth)
	{
		bool bCheckboxLocal = false;
		if (!bCheckbox)
		{
			bCheckbox = &bCheckboxLocal;
		}
		ImGuiIO& io = ImGui::GetIO();
		ImGui::PushID(label);

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Checkbox("##AbsoluteLocation", bCheckbox);
		ImGui::SameLine();
		ImGui::Text(label);
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		if (ImGui::Button("X", buttonSize))
		{
			values.x = resetValue;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		if (ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		if (ImGui::Button("Z", buttonSize))
		{
			values.z = resetValue;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();

	}
}

