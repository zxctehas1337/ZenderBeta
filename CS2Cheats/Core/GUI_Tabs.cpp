#include "GUI.h"
#include "../Features/ESP.h"
#include "../Features/Aimbot.h"

namespace GUI
{
    namespace Tabs
    {
        void DrawESPTab(ImVec4 primaryColor, ImVec4 hoverColor)
        {
            static const float MinRounding = 0.f, MaxRouding = 5.f;
            static const float MinFovFactor = 0.f, MaxFovFactor = 1.f;
            
            // Добавляем скроллинг для контента
            ImGui::BeginChild("ESPContent", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysUseWindowPadding);
            ImGui::SetCursorPosY(10.f); // Начинаем сверху
            ImGui::Columns(2, nullptr, false);
            ImGui::SetColumnWidth(0, ImGui::GetContentRegionAvail().x * 0.5f);
            
            // === ЛЕВАЯ КОЛОНКА ===
            ImGui::SetCursorPosX(25.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("VISUAL ESP");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            Components::PutSwitch(Text::ESP::Enable.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ESPenabled);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15.f);
            ImGui::TextDisabled(Text::ESP::HotKeyList.c_str());
            ImGui::SameLine();
            Components::AlignRight(75.f);
            
            // Styled hotkey button
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.15f, 0.25f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
            if (ImGui::Button(Text::ESP::HotKey.c_str(), { 75.f, 28.f }))
            {
                int key = 0;
                std::string keyName;
                std::thread([&]() {
                    KeyMgr::GetPressedKey(key, &keyName);
                    }).detach();
            }
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar();

            if (ESPConfig::ESPenabled)
            {
                // Main ESP options
                Components::PutSwitch(Text::ESP::Box.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowBoxESP, true, "###BoxCol", reinterpret_cast<float*>(&ESPConfig::BoxColor));
                if (ESPConfig::ShowBoxESP)
                {
                    Components::PutSwitch(Text::ESP::Outline.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::OutLine);
                    ImGui::TextDisabled(Text::ESP::BoxType.c_str());
                    ImGui::SameLine();
                    Components::AlignRight(165.f);
                    ImGui::SetNextItemWidth(165.f);
                    ImGui::Combo("###BoxType", &ESPConfig::BoxType, "Normal\0Corner\0");
                    Components::PutSliderFloat(Text::ESP::BoxRounding.c_str(), 15.f, &ESPConfig::BoxRounding, &MinRounding, &MaxRouding, "%.1f", NULL);
                }
                Components::PutSwitch(Text::ESP::HeadBox.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowHeadBox, true, "###HeadBoxCol", reinterpret_cast<float*>(&ESPConfig::HeadBoxColor));
                Components::PutSwitch(Text::ESP::Skeleton.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowBoneESP, true, "###BoneCol", reinterpret_cast<float*>(&ESPConfig::BoneColor));
                Components::PutSwitch(Text::ESP::OutOfFOVArrow.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowOutOfFOVArrow, true, "###OutFOVCol", reinterpret_cast<float*>(&ESPConfig::OutOfFOVArrowColor));
                if(ESPConfig::ShowOutOfFOVArrow)
                    Components::PutSliderFloat(Text::ESP::OutOfFOVRadius.c_str(), .5f, &ESPConfig::OutOfFOVRadiusFactor, &MinFovFactor, &MaxFovFactor, "%.1f", NULL);

                Components::PutSwitch(Text::ESP::HealthBar.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowHealthBar);
                Components::PutSwitch("Armor Bar", 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowArmorBar);
                Components::PutSwitch("Ammo Bar", 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::AmmoBar);
                Components::PutSwitch(Text::ESP::Weapon.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowWeaponESP);
                Components::PutSwitch("Info", 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowWeaponInfo);
                if (ESPConfig::ShowWeaponInfo) {
                    Components::PutSwitch("Show Bomb Carrier", 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowBombCarrier);
                }
                Components::PutSwitch(Text::ESP::PlayerName.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowPlayerName);
                Components::PutSwitch("Show Distance", 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowDistance);
                Components::PutSwitch(Text::ESP::FlashCheck.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::FlashCheck);
                Components::PutSwitch(Text::ESP::VisCheck.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7f, &ESPConfig::VisibleCheck, true, "###VisibleCol", reinterpret_cast<float*>(&ESPConfig::VisibleColor));
            }
            
            ImGui::NextColumn();
            
            // === ПРАВАЯ КОЛОНКА ===
            ImGui::SetCursorPosY(10.f); // Начинаем сверху для правой колонки
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("ADDITIONAL ESP");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            if (ESPConfig::ESPenabled)
            {
                // Filled box options
                Components::PutSwitch("Filled Box", 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::FilledBox, true, "###FilledBoxCol", reinterpret_cast<float*>(&ESPConfig::FilledColor));
                if (ESPConfig::FilledBox)
                {
                    Components::PutSwitch("Visible Check Fill", 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::FilledVisBox, true, "###FilledVisBoxCol", reinterpret_cast<float*>(&ESPConfig::BoxFilledVisColor));
                    Components::PutSwitch("Multi-Color Fill", 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::MultiColor, true, "###FilledColor2", reinterpret_cast<float*>(&ESPConfig::FilledColor2));
                }
                
                // Line to enemy
                Components::PutSwitch("Line To Enemy", 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowLineToEnemy, true, "###LineToEnemyCol", reinterpret_cast<float*>(&ESPConfig::LineToEnemyColor));
                if (ESPConfig::ShowLineToEnemy)
                {
                    ImGui::TextDisabled("Line Position");
                    ImGui::SameLine();
                    Components::AlignRight(165.f);
                    ImGui::SetNextItemWidth(165.f);
                    ImGui::Combo("###LinePos", &ESPConfig::LinePos, "Top\0Center\0Bottom\0");
                }
                
                // Sound ESP
                Components::PutSwitch("Sound ESP", 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::EnemySound, true, "###EnemySoundCol", reinterpret_cast<float*>(&ESPConfig::EnemySoundColor));
                
                // Grenade ESP
                Components::PutSwitch("Grenade ESP", 15.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::GrenadeESP, true, "###GrenadeESPCol", reinterpret_cast<float*>(&MiscCFG::GrenadeHEColor));
                if (MiscCFG::GrenadeESP)
                {
                    Components::PutSwitch("Show Trajectory", 15.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::ShowGrenadeTrajectory);
                    Components::PutSwitch("Show Explosion Radius", 15.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::ShowGrenadeRadius);
                    Components::PutSwitch("Show Danger Warning", 15.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::ShowGrenadeWarning);
                    Components::PutSwitch("Show Grenade Timer", 15.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::ShowGrenadeTimer);
                    Components::PutSwitch("Show Grenade Name", 15.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::ShowGrenadeName);
                    
                    float minDist = 100.0f, maxDist = 1000.0f;
                    Components::PutSliderFloat("Grenade Distance", 10.f, &MiscCFG::GrenadeWarningDistance, &minDist, &maxDist, "%.0f", NULL);
                    
                    float minTime = 1.0f, maxTime = 5.0f;
                    Components::PutSliderFloat("Warning Time", 10.f, &MiscCFG::GrenadeWarningTime, &minTime, &maxTime, "%.1f", NULL);
                }
                
                // Additional display options
                Components::PutSwitch("Show Health Number", 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowHealthNum);
                Components::PutSwitch("Show Armor Number", 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowArmorNum);
                Components::PutSwitch("Show Is Scoped", 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowIsScoped);
                Components::PutSwitch("Show Is Flashed", 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowIsBlind);
                Components::PutSwitch("Show Eye Ray", 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowEyeRay, true, "###EyeRayCol", reinterpret_cast<float*>(&ESPConfig::EyeRayColor));
            }
            
            ImGui::Columns(1);
            ImGui::EndChild();
        }

        void DrawLegitTab(ImVec4 primaryColor, ImVec4 secondaryColor)
        {
            ImGui::BeginChild("LegitContent", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysUseWindowPadding);
            ImGui::SetCursorPosY(10.f); // Начинаем сверху
            ImGui::Columns(2, nullptr, false);
            ImGui::SetColumnWidth(0, ImGui::GetContentRegionAvail().x * 0.5f);
            
            // === ЛЕВАЯ КОЛОНКА ===
            ImGui::SetCursorPosX(25.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("LEGIT SETTINGS");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            Components::PutSwitch("Anti Flash", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &MiscCFG::AntiFlash);
            
            // Разделитель
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::SetCursorPosX(25.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("VISUAL");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            Components::PutSwitch("Sniper Crosshair", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &MiscCFG::SniperCrosshair);
            
            ImGui::NextColumn();
            
            // === ПРАВАЯ КОЛОНКА ===
            ImGui::SetCursorPosY(10.f); // Начинаем сверху для правой колонки
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("PROTECTION");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            Components::PutSwitch("Anti Record", 10.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &MenuConfig::BypassOBS);
            Components::PutSwitch("Anti AFK Kick", 10.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &MiscCFG::AntiAFKKick);
            
            // Разделитель
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("WALLBANG");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            Components::PutSwitch("Wallbang Indicator", 10.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &MiscCFG::WallbangIndicator);
            
            if (MiscCFG::WallbangIndicator) {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.f);
                ImGui::TextDisabled("Sensitivity:");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(125.f);
                ImGui::SliderFloat("###WallbangSensitivity", &MiscCFG::WallbangSensitivity, 0.0f, 1.0f, "%.2f");
            }
            
            ImGui::Columns(1);
            ImGui::EndChild();
        }

        void DrawAimbotTab(ImVec4 primaryColor, ImVec4 secondaryColor)
        {
            ImGui::BeginChild("AimbotContent", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysUseWindowPadding);
            ImGui::SetCursorPosY(10.f);
            ImGui::Columns(2, nullptr, false);
            ImGui::SetColumnWidth(0, ImGui::GetContentRegionAvail().x * 0.5f);
            
            // Get current config reference
            auto* config = AimbotCFG::GetCurrentConfig();
            
            // === CONFIG MODE SELECTOR ===
            ImGui::SetCursorPosX(25.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("CONFIG MODE");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            ImGui::TextDisabled("Mode:");
            ImGui::SameLine();
            Components::AlignRight(150.f);
            ImGui::SetNextItemWidth(150.f);
            const char* configModes[] = { "Legit", "Semi-Rage", "Rage" };
            if (ImGui::Combo("###ConfigMode", &AimbotCFG::CurrentConfig, configModes, IM_ARRAYSIZE(configModes))) {
                // Config changed, update reference
                config = AimbotCFG::GetCurrentConfig();
            }
            
            // Show current mode with color
            ImGui::TextDisabled("Current:");
            ImGui::SameLine();
            switch (AimbotCFG::CurrentConfig) {
                case AimbotCFG::LEGIT:
                    ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "LEGIT MODE");
                    break;
                case AimbotCFG::SEMIRAGE:
                    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "SEMI-RAGE MODE");
                    break;
                case AimbotCFG::RAGE:
                    ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "RAGE MODE");
                    break;
            }
            
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            // === ЛЕВАЯ КОЛОНКА ===
            ImGui::SetCursorPosX(25.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("MAIN SETTINGS");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            // Main toggles
            Components::PutSwitch("Enable Aimbot", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->Enabled);
            Components::PutSwitch("Auto Aim", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->AimbotEnabled);
            Components::PutSwitch("Trigger Bot", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->TriggerEnabled);
            Components::PutSwitch("Aimlock", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->Aimlock);
            Components::PutSwitch("Silent Aim", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->SilentAim);
            
            // Silent Aim specific settings (only show if Silent Aim is enabled)
            if (config->SilentAim) {
                ImGui::Indent(20.f);
                Components::PutSwitch("Rage Mode", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->RageMode);
                if (config->RageMode) {
                    Components::PutSwitch("Instant Hit", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->RageInstant);
                    // Rage FOV slider
                    float minRageFOV = 90.0f, maxRageFOV = 180.0f;
                    Components::PutSliderFloat("Rage FOV", 15.f, &config->RageFOV, &minRageFOV, &maxRageFOV, "%.1f°");
                }
                ImGui::Unindent(20.f);
            }
            
            // Разделитель
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::SetCursorPosX(25.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("AIM SETTINGS");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            // Aim bone selection
            ImGui::TextDisabled("Aim Bone:");
            ImGui::SameLine();
            Components::AlignRight(120.f);
            ImGui::SetNextItemWidth(120.f);
            const char* boneItems[] = { "Head", "Neck", "Chest Upper", "Chest Lower", "Stomach", "Pelvis" };
            ImGui::Combo("###AimBone", &config->AimBone, boneItems, IM_ARRAYSIZE(boneItems));
            
            // FOV slider
            float minFOV = 1.0f, maxFOV = 180.0f;
            Components::PutSliderFloat("FOV", 15.f, &config->FOV, &minFOV, &maxFOV, "%.1f°");
            
            // Smooth slider
            float minSmooth = 0.1f, maxSmooth = 10.0f;
            Components::PutSliderFloat("Smooth", 15.f, &config->Smooth, &minSmooth, &maxSmooth, "%.2f");
            
            // Speed slider
            float minSpeed = 1.0f, maxSpeed = 20.0f;
            Components::PutSliderFloat("Aim Speed", 15.f, &config->AimSpeed, &minSpeed, &maxSpeed, "%.1f");
            
            // Recoil control
            float minRecoil = 0.0f, maxRecoil = 1.0f;
            Components::PutSliderFloat("Recoil Control", 15.f, &config->RecoilControl, &minRecoil, &maxRecoil, "%.2f");
            
            ImGui::NextColumn();
            
            // === ПРАВАЯ КОЛОНКА ===
            ImGui::SetCursorPosY(10.f);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("VISUAL SETTINGS");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            // Visual toggles
            Components::PutSwitch("Draw FOV Circle", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->DrawFOVCircle, true, "###FOVCircleCol", reinterpret_cast<float*>(&config->FOVCircleColor));
            Components::PutSwitch("Draw Target Line", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->DrawTargetLine, true, "###TargetLineCol", reinterpret_cast<float*>(&config->TargetLineColor));
            
            // FOV circle thickness
            float minThickness = 0.5f, maxThickness = 5.0f;
            Components::PutSliderFloat("Circle Thickness", 15.f, &config->FOVCircleThickness, &minThickness, &maxThickness, "%.1f");
            
            // Разделитель
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("SAFETY SETTINGS");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            // Safety toggles
            Components::PutSwitch("Visible Check", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->VisibleCheck);
            Components::PutSwitch("Flash Check", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->FlashCheck);
            Components::PutSwitch("Smoke Check", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->SmokeCheck);
            Components::PutSwitch("Team Filter", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->TeamFilter);
            Components::PutSwitch("Legit Mode", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->LegitMode);
            
            // Разделитель
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("ADVANCED");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            // Target priority
            ImGui::TextDisabled("Target Priority:");
            ImGui::SameLine();
            Components::AlignRight(120.f);
            ImGui::SetNextItemWidth(120.f);
            const char* priorityItems[] = { "Distance", "Health", "Angle", "FOV" };
            ImGui::Combo("###TargetPriority", &config->TargetPriority, priorityItems, IM_ARRAYSIZE(priorityItems) - 1);
            
            // Advanced toggles
            Components::PutSwitch("Prediction", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->Prediction);
            Components::PutSwitch("Anti Shake", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->AntiShake);
            Components::PutSwitch("Humanization", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->Humanization);
            
            if (config->Humanization) {
                float minRandom = 0.0f, maxRandom = 1.0f;
                Components::PutSliderFloat("Random Factor", 15.f, &config->RandomFactor, &minRandom, &maxRandom, "%.2f");
                
                float minDelay = 0.0f, maxDelay = 1.0f;
                Components::PutSliderFloat("Delay Min", 15.f, &config->DelayMin, &minDelay, &maxDelay, "%.2fs");
                Components::PutSliderFloat("Delay Max", 15.f, &config->DelayMax, &minDelay, &maxDelay, "%.2fs");
            }
            
            // Rage-specific settings
            if (config->RageMode) {
                ImGui::Spacing();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15.f);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f)); // Red color for rage
                ImGui::Text("RAGE SETTINGS");
                ImGui::PopStyleColor();
                ImGui::Spacing();
                
                // Check if we have rage config
                if (AimbotCFG::CurrentConfig == AimbotCFG::RAGE) {
                    auto* rageConfig = static_cast<AimbotCFG::RageConfig*>(config);
                    Components::PutSwitch("Force Headshot", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &rageConfig->ForceHeadshot);
                    Components::PutSwitch("Ignore Smoke", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &rageConfig->IgnoreSmoke);
                    Components::PutSwitch("Ignore Flash", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &rageConfig->IgnoreFlash);
                    Components::PutSwitch("Auto Wall", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &rageConfig->AutoWall);
                    Components::PutSwitch("Perfect Silent", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &rageConfig->PerfectSilent);
                    Components::PutSwitch("No Spread", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &rageConfig->NoSpread);
                    Components::PutSwitch("Instant Hit", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &rageConfig->InstantHit);
                    Components::PutSwitch("Multipoint", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &rageConfig->Multipoint);
                    
                    // Min damage slider
                    float minDamage = 1.0f, maxDamage = 100.0f;
                    Components::PutSliderFloat("Min Damage", 15.f, &rageConfig->MinDamage, &minDamage, &maxDamage, "%.0f HP");
                    
                    // Multipoint scale
                    if (rageConfig->Multipoint) {
                        float minScale = 0.1f, maxScale = 1.0f;
                        Components::PutSliderFloat("Multipoint Scale", 15.f, &rageConfig->MultipointScale, &minScale, &maxScale, "%.2f");
                    }
                }
                
                float minCurve = 0.0f, maxCurve = 2.0f;
                Components::PutSliderFloat("Human Curve", 15.f, &config->HumanCurve, &minCurve, &maxCurve, "%.1f");
            }
            
            ImGui::Columns(1);
            
            // Разделитель
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            // === SPECIAL MODES SECTION ===
            ImGui::Columns(3, nullptr, false);
            
            // RAGE MODE
            ImGui::SetCursorPosX(25.f);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
            ImGui::Text("RAGE MODE");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            Components::PutSwitch("Enable Rage", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->RageMode);
            
            if (config->RageMode) {
                float minRageFOV = 90.0f, maxRageFOV = 180.0f;
                Components::PutSliderFloat("Rage FOV", 15.f, &config->RageFOV, &minRageFOV, &maxRageFOV, "%.1f°");
                
                float minRageSmooth = 0.1f, maxRageSmooth = 1.0f;
                Components::PutSliderFloat("Rage Smooth", 15.f, &config->RageSmooth, &minRageSmooth, &maxRageSmooth, "%.2f");
                
                Components::PutSwitch("Instant Aim", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->RageInstant);
            }
            
            ImGui::NextColumn();
            
            // LEGIT MODE
            ImGui::SetCursorPosX(25.f);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
            ImGui::Text("LEGIT MODE");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            Components::PutSwitch("Enable Legit", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->LegitModeEnabled);
            
            if (config->LegitModeEnabled) {
                float minLegitFOV = 5.0f, maxLegitFOV = 30.0f;
                Components::PutSliderFloat("Legit FOV", 15.f, &config->LegitFOV, &minLegitFOV, &maxLegitFOV, "%.1f°");
                
                float minLegitSmooth = 1.0f, maxLegitSmooth = 10.0f;
                Components::PutSliderFloat("Legit Smooth", 15.f, &config->LegitSmooth, &minLegitSmooth, &maxLegitSmooth, "%.1f");
                
                Components::PutSwitch("Legit Humanization", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->LegitHumanization);
            }
            
            ImGui::NextColumn();
            
            // ADAPTIVE SETTINGS
            ImGui::SetCursorPosX(25.f);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.2f, 1.0f));
            ImGui::Text("ADAPTIVE");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            Components::PutSwitch("Adaptive FOV", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->AdaptiveFOV);
            Components::PutSwitch("Adaptive Speed", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->AdaptiveSpeed);
            
            if (config->AdaptiveFOV) {
                float minAdaptFOV = 0.5f, maxAdaptFOV = 2.0f;
                Components::PutSliderFloat("FOV Scale", 15.f, &config->AdaptiveFOVScale, &minAdaptFOV, &maxAdaptFOV, "%.2f");
            }
            
            if (config->AdaptiveSpeed) {
                float minAdaptSpeed = 0.5f, maxAdaptSpeed = 2.0f;
                Components::PutSliderFloat("Speed Scale", 15.f, &config->AdaptiveSpeedScale, &minAdaptSpeed, &maxAdaptSpeed, "%.2f");
            }
            
            ImGui::Columns(1);
            
            // Разделитель
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            // === TRIGGER BOT SECTION ===
            ImGui::Columns(2, nullptr, false);
            
            ImGui::SetCursorPosX(25.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("TRIGGER BOT");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            Components::PutSwitch("Enable Trigger", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->TriggerEnabled);
            
            if (config->TriggerEnabled) {
                float minTriggerDelay = 0.0f, maxTriggerDelay = 1.0f;
                Components::PutSliderFloat("Trigger Delay", 15.f, &config->TriggerDelay, &minTriggerDelay, &maxTriggerDelay, "%.2fs");
                
                ImGui::TextDisabled("Trigger Key:");
                ImGui::SameLine();
                Components::AlignRight(120.f);
                ImGui::SetNextItemWidth(120.f);
                const char* keyItems[] = { "Left Mouse", "Right Mouse", "Middle Mouse", "X1", "X2" };
                int keyIndex = 0;
                switch (config->TriggerKey) {
                    case VK_LBUTTON: keyIndex = 0; break;
                    case VK_RBUTTON: keyIndex = 1; break;
                    case VK_MBUTTON: keyIndex = 2; break;
                    case VK_XBUTTON1: keyIndex = 3; break;
                    case VK_XBUTTON2: keyIndex = 4; break;
                }
                if (ImGui::Combo("###TriggerKey", &keyIndex, keyItems, IM_ARRAYSIZE(keyItems))) {
                    switch (keyIndex) {
                        case 0: config->TriggerKey = VK_LBUTTON; break;
                        case 1: config->TriggerKey = VK_RBUTTON; break;
                        case 2: config->TriggerKey = VK_MBUTTON; break;
                        case 3: config->TriggerKey = VK_XBUTTON1; break;
                        case 4: config->TriggerKey = VK_XBUTTON2; break;
                    }
                }
                
                Components::PutSwitch("Visible Check", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->TriggerVisibleCheck);
            }
            
            ImGui::NextColumn();
            
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("KILL DELAY");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            Components::PutSwitch("Kill Delay", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &config->KillDelay);
            
            if (config->KillDelay) {
                float minKillDelay = 0.1f, maxKillDelay = 2.0f;
                Components::PutSliderFloat("Delay Time", 15.f, &config->KillDelayTime, &minKillDelay, &maxKillDelay, "%.2fs");
            }
            
            // Разделитель
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("PREDICTION");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            if (config->Prediction) {
                float minPrediction = 0.1f, maxPrediction = 2.0f;
                Components::PutSliderFloat("Prediction Scale", 15.f, &config->PredictionScale, &minPrediction, &maxPrediction, "%.2f");
            }
            
            ImGui::Columns(1);
            ImGui::EndChild();
        }

        void DrawMiscTab(ImVec4 primaryColor, ImVec4 secondaryColor)
        {
            ImGui::BeginChild("MiscContent", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysUseWindowPadding);
            ImGui::SetCursorPosY(10.f); // Начинаем сверху
            ImGui::Columns(2, nullptr, false);
            ImGui::SetColumnWidth(0, ImGui::GetContentRegionAvail().x * 0.5f);
            
            // === ЛЕВАЯ КОЛОНКА ===
            ImGui::SetCursorPosX(25.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("MOVEMENT");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            Components::PutSwitch(Text::Misc::BunnyHop.c_str(), 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &MiscCFG::BunnyHop, false, NULL, NULL, Text::Misc::InsecureTip.c_str());
            
            // Разделитель
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::SetCursorPosX(25.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("ANTI-AIM (КРУТИЛКА)");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            Components::PutSwitch("Enable Anti-Aim", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &MiscCFG::AntiAimEnabled);
            
            if (MiscCFG::AntiAimEnabled) {
                // Anti-aim mode selection
                ImGui::TextDisabled("Anti-Aim Mode:");
                ImGui::SameLine();
                Components::AlignRight(120.f);
                ImGui::SetNextItemWidth(120.f);
                const char* antiAimModes[] = { "Static", "Jitter", "Spin", "Random" };
                ImGui::Combo("###AntiAimMode", &MiscCFG::AntiAimMode, antiAimModes, IM_ARRAYSIZE(antiAimModes));
                
                // Pitch control
                float minPitch = -89.0f, maxPitch = 89.0f;
                Components::PutSliderFloat("Pitch Angle", 10.f, &MiscCFG::AntiAimPitch, &minPitch, &maxPitch, "%.1f");
                
                // Yaw offset control
                float minYaw = -180.0f, maxYaw = 180.0f;
                Components::PutSliderFloat("Yaw Offset", 10.f, &MiscCFG::AntiAimYawOffset, &minYaw, &maxYaw, "%.1f");
                
                if (MiscCFG::AntiAimMode == 2) { // Spin mode
                    float minSpinSpeed = 1.0f, maxSpinSpeed = 20.0f;
                    Components::PutSliderFloat("Spin Speed", 10.f, &MiscCFG::AntiAimSpinSpeed, &minSpinSpeed, &maxSpinSpeed, "%.1f");
                }
                
                if (MiscCFG::AntiAimMode == 1) { // Jitter mode
                    float minJitterRange = 5.0f, maxJitterRange = 90.0f;
                    Components::PutSliderFloat("Jitter Range", 10.f, &MiscCFG::AntiAimJitterRange, &minJitterRange, &maxJitterRange, "%.1f");
                }
                
                Components::PutSwitch("At Targets", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &MiscCFG::AntiAimAtTargets);
                Components::PutSwitch("Only When Moving", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &MiscCFG::AntiAimOnMove);
                Components::PutSwitch("On Key", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &MiscCFG::AntiAimOnKey);
                
                if (MiscCFG::AntiAimOnKey) {
                    ImGui::TextDisabled("Anti-Aim Key:");
                    ImGui::SameLine();
                    Components::AlignRight(75.f);
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.f);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.22f, 0.25f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, secondaryColor);
                    if (ImGui::Button("AntiAimKey", { 75.f, 28.f }))
                    {
                        int key = 0;
                        std::string keyName;
                        std::thread([&]() {
                            KeyMgr::GetPressedKey(key, &keyName);
                            }).detach();
                    }
                    ImGui::PopStyleColor(2);
                    ImGui::PopStyleVar();
                }
                
                // Разделитель
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                
                ImGui::SetCursorPosX(25.f);
                ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
                ImGui::Text("ANTI-RESOLVE");
                ImGui::PopStyleColor();
                ImGui::Spacing();
                
                Components::PutSwitch("Real Angle", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &MiscCFG::AntiAimReal);
                Components::PutSwitch("Fake Angle", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &MiscCFG::AntiAimFake);
                
                if (MiscCFG::AntiAimFake) {
                    float minFakeOffset = 0.0f, maxFakeOffset = 60.0f;
                    Components::PutSliderFloat("Fake Offset", 10.f, &MiscCFG::AntiAimFakeOffset, &minFakeOffset, &maxFakeOffset, "%.1f");
                }
                
                Components::PutSwitch("LBY Breaker", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &MiscCFG::AntiAimLBYBreaker);
                
                if (MiscCFG::AntiAimLBYBreaker) {
                    float minLBYOffset = 0.0f, maxLBYOffset = 180.0f;
                    Components::PutSliderFloat("LBY Offset", 10.f, &MiscCFG::AntiAimLBYOffset, &minLBYOffset, &maxLBYOffset, "%.1f");
                }
            }
            
            // Разделитель
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::SetCursorPosX(25.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("AUTO");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            Components::PutSwitch("Knife bot", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &MiscCFG::AutoKnife);
            Components::PutSwitch("Zeus bot", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &MiscCFG::AutoZeus);
            Components::PutSwitch("Auto Shop", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &MiscCFG::AutoShop);
            
            if (MiscCFG::AutoShop) {
                ImGui::SetCursorPosX(15.f);
                ImGui::TextDisabled("Hotkey:");
                ImGui::SameLine();
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.f);
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.22f, 0.25f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, secondaryColor);
                if (ImGui::Button("AutoShopKey", { 90.f, 30.f }))
                {
                    int key = 0;
                    std::string keyName;
                    std::thread([&]() {
                        KeyMgr::GetPressedKey(key, &keyName);
                        }).detach();
                }
                ImGui::PopStyleColor(2);
                ImGui::PopStyleVar();
            }

            ImGui::NextColumn();
            
            // === ПРАВАЯ КОЛОНКА ===
            ImGui::SetCursorPosY(10.f); // Начинаем сверху для правой колонки
            float CurrentCursorX = ImGui::GetCursorPosX();
            ImGui::SetCursorPosX(CurrentCursorX + 15.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("SETTINGS");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            ImGui::SetCursorPosX(CurrentCursorX + 10.f);
            ImGui::TextDisabled(Text::Misc::MenuKey.c_str());
            ImGui::SameLine();
            Components::AlignRight(90.f);
            
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.f);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.22f, 0.25f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, secondaryColor);
            if (ImGui::Button(Text::Misc::HotKey.c_str(), { 90.f, 30.f }))
            {
                int key = 0;
                std::string keyName;
                std::thread([&]() {
                    KeyMgr::GetPressedKey(key, &keyName);
                    }).detach();
            }
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar();
            
            // Разделитель
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::SetCursorPosX(CurrentCursorX + 15.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("VISUAL");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            Components::PutSwitch("Water Mark", 10.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &MiscCFG::WaterMark);
            
            // Разделитель
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::SetCursorPosX(CurrentCursorX + 15.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("SOUND");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            ImGui::SetCursorPosX(CurrentCursorX + 10.f);
            ImGui::TextDisabled("Hit Sound");
            ImGui::SetCursorPosX(CurrentCursorX + 10.f);
            static const char* hitSoundItems[] = { "Off", "Default", "Metal", "Custom" };
            ImGui::SetNextItemWidth(145.f);
            ImGui::Combo("###HitSound", &MiscCFG::HitSound, hitSoundItems, IM_ARRAYSIZE(hitSoundItems));
            
            Components::PutSwitch("Hit Marker", 10.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &MiscCFG::HitMarker);
            Components::PutSwitch("Fatality Sound", 10.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &MiscCFG::FatalitySound);
            
            ImGui::Columns(1);
            ImGui::EndChild();
        }

        void DrawListsTab(ImVec4 primaryColor, ImVec4 secondaryColor)
        {
            ImGui::BeginChild("ListsContent", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysUseWindowPadding);
            ImGui::SetCursorPosY(10.f); // Начинаем сверху
            ImGui::Columns(2, nullptr, false);
            ImGui::SetColumnWidth(0, ImGui::GetContentRegionAvail().x * 0.5f);
            
            // === ЛЕВАЯ КОЛОНКА ===
            ImGui::SetCursorPosX(25.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("GAME LISTS");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            Components::PutSwitch("Spectator List", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &MiscCFG::SpecList);
            Components::PutSwitch("Bomb Timer", 15.f, static_cast<float>(ImGui::GetFrameHeight() * 1.7), &MiscCFG::bmbTimer, true, "###BombTimerCol", reinterpret_cast<float*>(&MiscCFG::BombTimerCol));
            
            // Разделитель
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::SetCursorPosX(25.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("INFORMATION");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            ImGui::SetCursorPosX(15.f);
            ImGui::TextDisabled("Spectator List shows");
            ImGui::SetCursorPosX(15.f);
            ImGui::TextDisabled("who is watching you");
            ImGui::Spacing();
            ImGui::SetCursorPosX(15.f);
            ImGui::TextDisabled("Bomb Timer shows");
            ImGui::SetCursorPosX(15.f);
            ImGui::TextDisabled("explosion time");
            
            ImGui::NextColumn();
            
            // === ПРАВАЯ КОЛОНКА ===
            ImGui::SetCursorPosY(10.f); // Начинаем сверху для правой колонки
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("SETTINGS");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.f);
            ImGui::TextDisabled("Window Positions");
            ImGui::Spacing();
            
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.f);
            ImGui::TextDisabled("Spectator List:");
            ImGui::SameLine();
            char specPos[64];
            sprintf_s(specPos, "X: %.0f Y: %.0f", MenuConfig::SpecWinPos.x, MenuConfig::SpecWinPos.y);
            ImGui::Text(specPos);
            
            ImGui::Spacing();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.f);
            ImGui::TextDisabled("Bomb Timer:");
            ImGui::SameLine();
            char bombPos[64];
            sprintf_s(bombPos, "X: %.0f Y: %.0f", MenuConfig::BombWinPos.x, MenuConfig::BombWinPos.y);
            ImGui::Text(bombPos);
            
            // Разделитель
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15.f);
            ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
            ImGui::Text("ACTIONS");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            // Styled reset button
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.f);
            ImGui::PushStyleColor(ImGuiCol_Button, primaryColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, secondaryColor);
            if (ImGui::Button("Reset Positions###ResetPosBtn", { 125.f, 34.f }))
            {
                MenuConfig::SpecWinPos = ImVec2(10.0f, 340.0f);
                MenuConfig::BombWinPos = ImVec2(860.0f, 80.0f);
            }
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar();
            
            ImGui::Columns(1);
            ImGui::EndChild();
        }

        void DrawConfigTab()
        {
            ImGui::BeginChild("ConfigContent", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysUseWindowPadding);
            ImGui::SetCursorPosY(10.f);
            
            // Config mode selection at the top
            ImGui::SetCursorPosX(25.f);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            ImGui::Text("CONFIG MODE");
            ImGui::PopStyleColor();
            ImGui::Spacing();
            
            // Config mode selector
            ImGui::SetCursorPosX(25.f);
            ImGui::TextDisabled("Select Config Type:");
            ImGui::SameLine();
            Components::AlignRight(200.f);
            ImGui::SetNextItemWidth(200.f);
            const char* configModes[] = { "LEGIT", "SEMIRAGE", "RAGE" };
            ImGui::Combo("###ConfigMode", &AimbotCFG::CurrentConfig, configModes, IM_ARRAYSIZE(configModes));
            
            // Config description based on selection
            ImGui::Spacing();
            ImGui::SetCursorPosX(25.f);
            switch (AimbotCFG::CurrentConfig) {
            case AimbotCFG::LEGIT:
                ImGui::TextDisabled("Legit: Human-like aiming with smooth movement");
                break;
            case AimbotCFG::SEMIRAGE:
                ImGui::TextDisabled("SemiRage: Balanced features for HvH gameplay");
                break;
            case AimbotCFG::RAGE:
                ImGui::TextDisabled("RAGE: Maximum aggression for HvH domination");
                break;
            }
            
            // Разделитель
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            // Render the original config menu
            ConfigMenu::RenderCFGmenu();
            ImGui::EndChild();
        }
    }
}
