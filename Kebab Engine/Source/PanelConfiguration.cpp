#include "PanelConfiguration.h"

#include "SDL.h"
#include "SDL_cpuinfo.h"
#include "GL/glew.h"

#include <stdio.h>
#include <string>

ConfigPanel::ConfigPanel()
{
}

ConfigPanel::~ConfigPanel()
{
}

bool ConfigPanel::Update(float dt)
{
    if (ImGui::Begin("Configuration", NULL, 0))
    {
        if (ImGui::CollapsingHeader("Application"))
        {
            // Input Text ===========
            static char tmp[32];
            sprintf_s(tmp, 32, "Kebab Engine");
            ImGui::InputText("App Name", tmp, IM_ARRAYSIZE(tmp));
            sprintf_s(tmp, 32, "UPC CITM");
            ImGui::InputText("Organization", tmp, IM_ARRAYSIZE(tmp));

            // Slider ============

            static float limFPS = 0;
            ImGui::SliderFloat("Max FPS", &limFPS, 0.0f, 120.0f, "%.1f");

            ImGui::BulletText("Limit Framerate: %.1f", limFPS);

            // Histogram ============
            if (i == (MAX_IT_HIST - 1))
            {
                for (int y = 0; y < i; y++)
                {
                    int aux = y + 1;
                    fpsLog[y] = fpsLog[aux];
                    fpsLog[i] = ImGui::GetIO().Framerate;
                    msLog[y] = msLog[aux];
                    msLog[i] = 1000.0f / ImGui::GetIO().Framerate;
                    memCost[y] = memCost[aux];
                    memCost[i] = SDL_GetSystemRAM();
                }
            }
            else
            {
                fpsLog[i] = ImGui::GetIO().Framerate;
                msLog[i] = 1000.0f / ImGui::GetIO().Framerate;
                memCost[i] = SDL_GetSystemRAM();
                i++;
            }
            char title[25];
            sprintf_s(title, 25, "Framerate: %.1f", ImGui::GetIO().Framerate);
            ImGui::PlotHistogram("##framerate", fpsLog, IM_ARRAYSIZE(fpsLog), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
            sprintf_s(title, 25, "Milliseconds: %.3f", 1000.0f / ImGui::GetIO().Framerate);
            ImGui::PlotHistogram("##milliseconds", msLog, IM_ARRAYSIZE(msLog), 0, title, 0.0f, 50.0f, ImVec2(310, 100));
            sprintf_s(title, 25, "Memory Consumption");
            ImGui::PlotHistogram("##memoryconsumption", memCost, IM_ARRAYSIZE(memCost), 0, title, 0.0f, 30000.0f, ImVec2(310, 100));

            //SDL_Log();
            
        }
        if (ImGui::CollapsingHeader("Window"))
        {

        }
        if (ImGui::CollapsingHeader("File System"))
        {

        }
        if (ImGui::CollapsingHeader("Input"))
        {

        }
        if (ImGui::CollapsingHeader("Hardware"))
        {
            SDL_version compiled;
            SDL_version linked;

            SDL_VERSION(&compiled);
            SDL_GetVersion(&linked);
            /*printf("We compiled against SDL version %d.%d.%d ...\n",
                compiled.major, compiled.minor, compiled.patch);
            printf("But we are linking against SDL version %d.%d.%d.\n",
                linked.major, linked.minor, linked.patch);*/

            ImGui::Text("SDL Version:");
            ImGui::SameLine(); ImGui::TextColored({ 200,200,0,255 }, "%d.%d.%d", compiled.major, compiled.minor, compiled.patch);

            ImGui::Separator();

            ImGui::Text("CPUs:");
            ImGui::SameLine(); ImGui::TextColored({ 200,200,0,255 }, "%i (Cache: %i kb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());

            std::string s = "";
            ImGui::Text("Caps:");
            if (SDL_HasAVX()) s += "AVX, ";
            if (SDL_HasAVX2()) s += "AVX2, ";
            if (SDL_HasMMX()) s += "MMX, ";
            if (SDL_HasRDTSC()) s += "RDTSC, ";
            if (SDL_HasSSE()) s += "SSE, ";
            if (SDL_HasSSE2()) s += "SSE2, ";
            if (SDL_HasSSE3()) s += "SSE3, ";
            if (SDL_HasSSE41()) s += "SSE41, ";
            if (SDL_HasSSE42()) s += "SSE42, ";
            ImGui::SameLine(); ImGui::TextColored({ 200,200,0,255 }, "%s", s.c_str());

            ImGui::Separator();

            ImGui::Text("GPU:");
            const GLubyte* vendor = glGetString(GL_VENDOR);
            const GLubyte* render = glGetString(GL_RENDERER);
            ImGui::SameLine(); ImGui::TextColored({ 200,200,0,255 }, "%s, from %s", render, vendor);

            ImGui::Text("VRAM Budget:");
            ImGui::SameLine(); ImGui::TextColored({ 200,200,0,255 }, "%i MB", SDL_GetSystemRAM());
        }

        ImGui::BulletText("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
    ImGui::End();

	return true;
}

void ConfigPanel::Draw()
{
}
