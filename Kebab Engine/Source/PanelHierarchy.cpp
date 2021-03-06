#include "Application.h"

#include "PanelHierarchy.h"

HierarchyPanel::HierarchyPanel()
{
	active = true;
	scroll = 0;
	goDragging = nullptr;
}

HierarchyPanel::~HierarchyPanel()
{

}

void HierarchyPanel::OnRender(float dt)
{
	if (ImGui::Begin("Hierarchy", &active))
	{
		if (ImGui::GetMousePos().x > ImGui::GetWindowPos().x && ImGui::GetMousePos().x < ImGui::GetWindowPos().x + ImGui::GetWindowWidth() &&
			ImGui::GetMousePos().y > ImGui::GetWindowPos().y && ImGui::GetMousePos().y < ImGui::GetWindowPos().y + ImGui::GetWindowHeight())
		{
			if (app->input->GetMouseZ() < 0)
			{
				scroll = ImGui::GetScrollY();
				scroll -= app->input->GetMouseZ();
				ImGui::SetScrollY(scroll);
			}
			if (app->input->GetMouseZ() > 0)
			{
				scroll = ImGui::GetScrollY();
				scroll -= app->input->GetMouseZ();
				ImGui::SetScrollY(scroll);
			}
			if (scroll <= 0) scroll = 0;
			else if (scroll >= ImGui::GetScrollMaxY()) scroll = ImGui::GetScrollMaxY();
		}

		if (ImGui::CollapsingHeader("Game Objects", ImGuiTreeNodeFlags_DefaultOpen))
		{
			int size = app->scene->GetGameObjects().size();
			for (int i = 0; i < size; ++i)
			{
				GameObject* go = app->scene->GetGameObjects()[i];

				if (ImGui::TreeNodeEx(go->GetName().c_str()))
				{
					if (ImGui::IsItemClicked())
						currentGO = go;
					
					static bool optionsPopup = false;
					if (ImGui::IsItemHovered() && app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN)
					{
						optionsPopup = true;
					}
					if (optionsPopup && !ImGui::IsItemHovered() && app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN)
					{
						optionsPopup = false;
					}

					if(optionsPopup)
					{
						DisplayGameObjectMenu(go);
					}

					if(go->GetChilds().size() > 0)
						DisplayHierarchy(go);

					ImGui::TreePop();
				}
			}
		}
	}
	ImGui::End();
}

void HierarchyPanel::DisplayHierarchy(GameObject* parentGO)
{
	ImGuiTreeNodeFlags flags = 0;
	flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;;

	for (int i = 0; i < parentGO->GetChilds().size(); ++i)
	{
		const auto& goChild = parentGO->GetChilds()[i];

		/*if (ImGui::IsItemClicked())
			currentGO = goChild;*/

		if (ImGui::TreeNodeEx(goChild->GetName().c_str(), flags))
		{
			if (ImGui::IsItemClicked())
				currentGO = goChild;

			static bool optionsPopup = false;
			if (ImGui::IsItemHovered() && app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN)
			{
				optionsPopup = true;
			}
			if (optionsPopup && !ImGui::IsItemHovered() && app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN)
			{
				optionsPopup = false;
			}

			if (optionsPopup)
			{
				DisplayGameObjectMenu(goChild);
			}


			DisplayHierarchy(goChild);
			ImGui::TreePop();
		}


		/////// Drag and drop

		if (ImGui::BeginDragDropSource())
		{
			goDragging = goChild;

			ImGui::SetDragDropPayload("childs", goChild, sizeof(GameObject));
			ImGui::Text(goChild->GetName().c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (ImGui::AcceptDragDropPayload("childs"))
			{
				if (goDragging)
				{
					GameObject* oldParent = goDragging->GetParent();

					std::vector<GameObject*>::iterator it = oldParent->GetChilds().begin();
					while (*it != goDragging)
						++it;
					oldParent->GetChilds().erase(it);

					GameObject* newParent = goChild->GetParent();

					//newParent->AddChild(goDragging);
					goChild->AddChild(goDragging);
					goDragging = nullptr;
				}
			}
			ImGui::EndDragDropTarget();
		}


	}	
}

void HierarchyPanel::DisplayGameObjectMenu(GameObject* go)
{
	ImGui::OpenPopup(go->GetName().c_str());
	if (ImGui::BeginPopup(go->GetName().c_str()))
	{
		//ImGui::Popupitem
		if (ImGui::Button("Delete"))
		{
			app->scene->DeleteGameObject(go);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::Button("Empty Child"))
		{
			GameObject* child = new GameObject("Game Object");
			go->AddChild(child);
			child->SetParent(go);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::Button("Move up"))
		{
			if (go->GetParent())
			{
				int size = go->GetParent()->GetChilds().size();
				for (int i = 0; i < size; ++i)
				{
					GameObject* current = go->GetParent()->GetChilds()[i];
					if ((current == go) && (i >= 1))
					{
						GameObject* tmp = go->GetParent()->GetChilds()[i - 1];
						go->GetParent()->GetChilds()[i - 1] = go->GetParent()->GetChilds()[i];
						go->GetParent()->GetChilds()[i] = tmp;
						break;
					}
				}
			}
			else
			{
				for (int i = 0; i < app->scene->GetGameObjects().size(); ++i)
				{
					GameObject* current = app->scene->GetGameObjects()[i];
					if (current == go && i >= 1)
					{
						GameObject* tmp = app->scene->GetGameObjects()[i - 1];
						app->scene->GetGameObjects()[i - 1] = app->scene->GetGameObjects()[i];
						app->scene->GetGameObjects()[i] = tmp;
						break;
					}
				}
			}

		}
		if (ImGui::Button("Move down"))
		{
			// Childs
			if (go->GetParent())
			{
				int size = go->GetParent()->GetChilds().size();
				for (int i = 0; i < size; ++i)
				{
					GameObject* current = go->GetParent()->GetChilds()[i];
					if ((current == go) && (i <= size - 2))
					{
						GameObject* tmp = go->GetParent()->GetChilds()[i + 1];
						go->GetParent()->GetChilds()[i + 1] = go->GetParent()->GetChilds()[i];
						go->GetParent()->GetChilds()[i] = tmp;
						break;
					}
				}
			}
			// Top parents
			else
			{
				int size = app->scene->GetGameObjects().size();
				for (int i = 0; i < size; ++i)
				{
					GameObject* current = app->scene->GetGameObjects()[i];
					if ((current == go) && (i <= size - 2))
					{
						GameObject* tmp = app->scene->GetGameObjects()[i + 1];
						app->scene->GetGameObjects()[i + 1] = app->scene->GetGameObjects()[i];
						app->scene->GetGameObjects()[i] = tmp;
						break;
					}
				}
			}
		}
		ImGui::EndPopup();
	}
}
