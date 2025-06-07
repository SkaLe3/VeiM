#include "LevelEditor/LevelEditor.h"
#include "Editor/Editor.h"
#include "Engine/Classes/Editor/EditorEngine.h"
#include "Engine/Entity.h"
#include "Engine/World.h"
#include "Engine/Reflection.h"

#include "UI/UI.h"
#include "Application/Rendering/NativeViewport.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace VeiM
{

	LevelEditor::LevelEditor(Widget* parent) : Widget(parent)
	{
		m_Viewport = CreateWidget<ViewportWidget>(this);
		m_Outliner = CreateWidget<Outliner>(this);
		UI::Utils::SetCharArrayData(m_SerachEntityQuery, 128, "");
	}

	bool LevelEditor::OnGUI()
	{
		Widget::OnGUI();
		return true;
	}

	bool LevelEditor::OnToolBar(float height)
	{
		ImVec2 originalPadding = ImGui::GetStyle().WindowPadding;

		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::BeginChild("##Toolbar", ImVec2(0, height), true,
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		ImGui::SetCursorPosY(4.0f);

		ImGui::SetCursorPosX(10.0f);
		if (ImGui::Button("Save", ImVec2(60, 24)))
		{

		}
		ImGui::SameLine(0.f, 5.f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine(0.f, 5.f);
		if (g_Editor->PlayWorld.Get())
		{
			if (ImGui::Button("End", ImVec2(60, 24)))
			{
				RequestEndPlay();
			}
		}
		else if (ImGui::Button("Play", ImVec2(60, 24)))
		{
			RequestBeginPlay();
		}
		ImGui::SameLine(0.f, 5.f);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine(0.f, 5.f);

		std::map<StringID, ClassDescriptor*, StringIDComparator> allEntities = ClassRegistry::GetAllClassesOfClass<Entity>();

		if (ImGui::Button("+##EntityDropdown", ImVec2(24, 24)))
		{
			ImGui::OpenPopup("AddEntityPopup");

		}
		ImVec2 buttonPos = ImGui::GetItemRectMin();
		ImGui::SetNextWindowPos(ImVec2(buttonPos.x, buttonPos.y + ImGui::GetItemRectSize().y));
		ImGui::SetNextWindowSize(ImVec2(200, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, originalPadding);
		if (ImGui::BeginPopup("AddEntityPopup", ImGuiWindowFlags_NoMove))
		{
			ImGui::SetNextWindowSize(ImVec2(200, 0));
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
			ImGui::SeparatorText("Spawn Entities");
			ImGui::PopStyleColor();
			if (ImGui::BeginMenu("All Entities"))
			{
				ImGui::SetNextItemWidth(-1);
				ImGui::InputTextWithHint("##FilterEntitiesToSpawn", "Type entity class...", m_SerachEntityQuery, IM_ARRAYSIZE(m_SerachEntityQuery));
				for (auto& [eName, eClass] : allEntities)
				{
					if (ImGui::MenuItem(eName.Get()))
					{
						ClassDescriptor* entityClass = ClassRegistry::FindClass(eName);
						if (entityClass)
						{
							PlaceEntity(entityClass);
						}
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();

		ImGui::EndChild();
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor();
		return true;
	}

	void LevelEditor::OnWindowMenu()
	{
		m_Outliner->OnWindowMenu();
	}

	void LevelEditor::RequestBeginPlay()
	{
		SharedPtr<NativeViewport> levelViewport = GetViewport();
		g_Editor->RequestPlaySession(levelViewport);
		m_Outliner->DropSelectedEntity();
	}

	void LevelEditor::RequestEndPlay()
	{
		g_Editor->RequestEndPlaySession();
		m_Outliner->DropSelectedEntity();
	}

	void LevelEditor::PlaceEntity(ClassDescriptor* entityClass)
	{
		if (g_Editor->EditorWorld != g_World)
		{
			VM_CORE_WARN("[Level Editor] Attempt to add entity {0} to level during play", entityClass->Name.ToString());
			return;
		}
		String className = entityClass->Name.ToString();
		String entityName = className + std::to_string(m_ClassCountMap[entityClass]++);
		StringID entityNameID = StringID(entityName.data());
		while (g_Editor->EditorWorld->GetEntityByName(entityNameID))
		{
			entityName = className + std::to_string(m_ClassCountMap[entityClass]++);
			entityNameID = StringID(entityName.data());
		}

		Entity* newEntity = g_Editor->EditorWorld->SpawnEntity(entityClass, Transform::Identity, false, entityNameID);
		newEntity->SetLabel(entityName);
	}

	void LevelEditor::DeleteEntity(Entity* entity)
	{
		entity->Destroy();
	}

	void LevelEditor::AddComponentToEntity(Entity* entity, ClassDescriptor* componentClass)
	{
		entity->AddComponent(componentClass, false, Transform::Identity, componentClass->Name);
	}

	SharedPtr<NativeViewport> LevelEditor::GetViewport()
	{
		return m_Viewport->GetViewport();
	}

}


