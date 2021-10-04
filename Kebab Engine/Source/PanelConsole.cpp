#include "PanelConsole.h"

#include "mmgr/mmgr.h"

ConsolePanel::ConsolePanel()
{
    active = true;
    ClearConsole();
}

ConsolePanel::~ConsolePanel()
{
    ClearConsole();
}

bool ConsolePanel::Update(float dt)
{
    if (ImGui::Begin("Console", &active))
    {
        ImGui::TextUnformatted(buf.begin());
        ImGui::SetScrollY(ImGui::GetWindowHeight());
    }
    ImGui::End();
	return true;
}

void ConsolePanel::Draw()
{
}

void ConsolePanel::AddLog(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    buf.appendfv(fmt, args);
    buf.appendfv("\n", args);
    va_end(args);
}

void ConsolePanel::ClearConsole()
{
    buf.clear();
}
