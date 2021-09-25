#pragma once
#include "Module.h"
#include "Globals.h"

#define MAX_MOUSE_BUTTONS 5

enum KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class Input : public Module
{
public:
	
	Input(Application* app, bool startEnabled = true);
	~Input();

	bool Init();
	bool PreUpdate(float dt);
	bool CleanUp();

	KeyState GetKey(int id) const
	{
		return keyboard[id];
	}

	KeyState GetMouseButton(int id) const
	{
		return mouseButtons[id];
	}

	int GetMouseX() const
	{
		return mouseX;
	}

	int GetMouseY() const
	{
		return mouseY;
	}

	int GetMouseZ() const
	{
		return mouseZ;
	}

	int GetMouseXMotion() const
	{
		return mouseXMotion;
	}

	int GetMouseYMotion() const
	{
		return mouseYMotion;
	}

private:
	KeyState* keyboard;
	KeyState mouseButtons[MAX_MOUSE_BUTTONS];
	int mouseX;
	int mouseY;
	int mouseZ;
	int mouseXMotion;
	int mouseYMotion;
	//int mouse_z_motion;
};