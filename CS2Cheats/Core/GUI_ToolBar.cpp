#include "GUI.h"

namespace GUI
{
    namespace ToolBar
    {
        void DrawSidebar(float sidebarWidth, float headerHeight, ImVec4 sidebarColor, ImVec4 accentColor, 
                         ImVec4 primaryColor, ImVec4 hoverColor, float& tabAnimationTime, int& activeTab, int& lastActiveTab)
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, sidebarColor);
            ImGui::BeginChild("Sidebar", ImVec2(sidebarWidth, -1), true, ImGuiWindowFlags_NoScrollbar);
            {
                // Header with logo
                ImGui::SetCursorPosY(20.0f);
                ImGui::SetCursorPosX((sidebarWidth - ImGui::CalcTextSize("ZENDER").x) * 0.5f);
                ImGui::PushStyleColor(ImGuiCol_Text, accentColor);
                
                // Check if the font at index 1 exists before using it
                if (ImGui::GetIO().Fonts->Fonts.Size > 1) {
                    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
                    ImGui::Text("ZENDER");
                    ImGui::PopFont();
                } else {
                    // Fallback to default font if the desired font is not available
                    ImGui::Text("ZENDER");
                }
                
                ImGui::PopStyleColor();
                
                // Version
                ImGui::SetCursorPosX((sidebarWidth - ImGui::CalcTextSize("v2.0").x) * 0.5f);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.7f, 0.8f, 0.8f));
                ImGui::Text("v0.7.1");
                ImGui::PopStyleColor();
                
                ImGui::SetCursorPosY(headerHeight);
                
                // Gradient separator
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                ImVec2 p = ImGui::GetCursorScreenPos();
                drawList->AddRectFilledMultiColor(
                    ImVec2(p.x, p.y),
                    ImVec2(p.x + sidebarWidth, p.y + 2),
                    IM_COL32(50, 100, 200, 255),
                    IM_COL32(100, 150, 255, 255),
                    IM_COL32(100, 150, 255, 255),
                    IM_COL32(50, 100, 200, 255)
                );
                
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.0f);
                
                // Tab buttons with animations
                const char* tabNames[] = { "ESP", "Legit", "Aimbot", "Misc", "Lists", "Config" };
            
                for (int i = 0; i < 6; i++) {
                    ImGui::PushID(i);
                
                    // Animation for active tab
                    float animProgress = (i == activeTab) ? tabAnimationTime : (1.0f - tabAnimationTime * 0.5f);
                
                    // Simplified color handling to avoid potential issues
                    ImVec4 buttonColor;
                    if (i == activeTab) {
                        buttonColor = ImVec4(primaryColor.x, primaryColor.y, primaryColor.z, 0.8f);
                    } else {
                        buttonColor = ImVec4(0.1f, 0.15f, 0.25f, 0.5f);
                    }
                
                    ImVec4 hoverButtonColor = (i == activeTab) ? accentColor : hoverColor;
                
                    // Fixed padding to avoid animation issues
                    float padding = 12.0f;
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15.0f, padding));
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
                
                    ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverButtonColor);
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, primaryColor);
                
                    // Tab button with better click handling
                    if (ImGui::Button(tabNames[i], ImVec2(sidebarWidth - 30.0f, 0))) {
                        // Only change tab if it's different from current
                        if (activeTab != i) {
                            activeTab = i;
                            tabAnimationTime = 0.0f; // Reset animation
                        }
                    }
                
                    ImGui::PopStyleColor(3);
                    ImGui::PopStyleVar(2);
                    ImGui::PopID();
                
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
                }
            }
            ImGui::EndChild();
            ImGui::PopStyleColor();
        }

        void DrawHeader(float contentAlpha, ImVec4 accentColor, int activeTab)
        {
            // Content header with smooth transition
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, contentAlpha);
            
            // Watermark in top right corner
            ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 200.0f, 15.0f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.7f, 0.9f, 0.8f));
            ImGui::Text("Zender Beta Test 0.7.1");
            ImGui::PopStyleColor();
            
            ImGui::SetCursorPos(ImVec2(30.0f, 25.0f));
            ImGui::PushStyleColor(ImGuiCol_Text, accentColor);
            
            // Check if the font at index 1 exists before using it
            if (ImGui::GetIO().Fonts->Fonts.Size > 1) {
                ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
            }
            
            const char* tabTitles[] = { "VISUAL ESP", "LEGIT SETTINGS", "AIMBOT SETTINGS", "MISCELLANEOUS", "GAME LISTS", "CONFIGURATION" };
            ImGui::Text(tabTitles[activeTab]);
            
            if (ImGui::GetIO().Fonts->Fonts.Size > 1) {
                ImGui::PopFont();
            }
            
            ImGui::PopStyleColor();
            
            // Animated underline
            ImDrawList* contentDrawList = ImGui::GetWindowDrawList();
            ImVec2 textSize = ImGui::CalcTextSize(tabTitles[activeTab]);
            ImVec2 underlineStart = ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y - 5.0f);
            ImVec2 underlineEnd = ImVec2(underlineStart.x + textSize.x * contentAlpha, underlineStart.y + 2.0f);
            
            contentDrawList->AddRectFilled(
                underlineStart,
                underlineEnd,
                IM_COL32(77, 153, 255, (int)(255 * contentAlpha))
            );
            
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.0f);
            ImGui::PopStyleVar();
        }
    }
}
