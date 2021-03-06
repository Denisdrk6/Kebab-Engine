#pragma once

#include "Panel.h"

class HierarchyPanel : public Panel
{
public:
	HierarchyPanel();
	~HierarchyPanel();

	void OnRender(float dt) override;

private:
	void DisplayHierarchy(GameObject* go);

	void DisplayGameObjectMenu(GameObject* go);

public:
	GameObject* currentGO;

private:
	GameObject* goDragging;

};