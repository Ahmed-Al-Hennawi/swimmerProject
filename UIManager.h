#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "imgui.h"
#include "GameEngine.h"

class UIManager
{
public:
    void renderHUD(GameEngine &engine)
    {
        ImGui::SetNextWindowPos(ImVec2(400, 50), ImGuiCond_Always, ImVec2(0.5f, 0.0f));
        ImGui::SetNextWindowBgAlpha(0.6f);
        ImGui::Begin("HUD", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

        if (engine.state == WAITING)
        {
            ImGui::Text("INFINITE SWIMMER");
            ImGui::Separator();
            ImGui::Text("Personal Best: %.1f m", engine.highScore);
            ImGui::Separator();
            ImGui::Text("CONTROLS GUIDE:");
            ImGui::Text("- Move hand Left/Right to steer");
            ImGui::Text("- Make a PUNCH/FIST to Dive quickly");
            ImGui::Text("- Or use Voice: 'Left', 'Right', 'Dive'");
            ImGui::Separator();
            ImGui::Text("Wave hand or say a command to START");
        }
        else if (engine.state == PLAYING)
        {
            ImGui::Text("Score: %.1f m (High: %.1f m)", engine.distanceSwum, engine.highScore);
            ImGui::Text("Speed: %.1f m/s", engine.currentSpeed);
        }
        else if (engine.state == GAMEOVER)
        {
            if (engine.distanceSwum >= engine.highScore && engine.distanceSwum > 0.0f)
            {
                ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "🏆 NEW HIGH SCORE!");
            }
            else
            {
                ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "GAME OVER!");
            }
            ImGui::Separator();
            ImGui::Text("Final Distance: %.1f m", engine.distanceSwum);
            ImGui::Text("Personal Best: %.1f m", engine.highScore);
            ImGui::Text("\nWave hand or press any key to restart.");
        }
        ImGui::End();
    }
};

#endif