#pragma once
#include "..\Core\Config.h"
#include "..\Core\Render.h"
#include "..\Core\Init.h"
#include "..\Features\Radar.h"
#include "..\Features\Misc.h"
#include "..\Config\ConfigMenu.h"
#include "..\Config\ConfigSaver.h"

#include "..\Resources\Language.h"
#include "../Helpers/KeyManager.h"

#include "../Features/ESP.h"
#include "../Features/Aimbot.h"

namespace GUI
{
	void LoadDefaultConfig()
	{
		if (!MenuConfig::defaultConfig)
			return;

		// First, reset to defaults
		ConfigMenu::ResetToDefault();
		
		// Then try to load default.cfg if it exists
		MyConfigSaver::LoadConfig("default.cfg");

		MenuConfig::defaultConfig = false;
	}

	void LoadImages()
	{
		// Removed image loading as per user request
	}

	// Components Settings
	// ########################################
	void AlignRight(float ContentWidth)
	{
		float availWidth = ImGui::GetContentRegionAvail().x;
		float targetX = ImGui::GetCursorPosX() + availWidth - ContentWidth - ImGui::GetStyle().ItemSpacing.x;
		ImGui::SetCursorPosX(ImMax(ImGui::GetCursorPosX(), targetX));
	}
	void PutSwitch(const char* string, float CursorX, float ContentWidth, bool* v, bool ColorEditor = false, const char* lable = NULL, float col[4] = NULL, const char* Tip = NULL)
	{
		ImGui::PushID(string);
		
		// Получаем доступную ширину для корректного размещения элементов
		float availWidth = ImGui::GetContentRegionAvail().x;
		
		// Ограничиваем ContentWidth, чтобы избежать выхода за границы
		ContentWidth = ImMin(ContentWidth, availWidth * 0.25f);
		
		float CurrentCursorX = ImGui::GetCursorPosX();
		float CurrentCursorY = ImGui::GetCursorPosY();
		
		// Ограничиваем CursorX, чтобы текст не выходил за границы
		CursorX = ImMin(CursorX, availWidth * 0.4f);
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		
		// Отображаем текст с ограничением длины, если необходимо
		ImGui::TextDisabled(string);
		
		if (Tip && ImGui::IsItemHovered())
			ImGui::SetTooltip(Tip);
		
		ImGui::SameLine();
		ImGui::SetCursorPosY(CurrentCursorY - 2);
		
		if (ColorEditor) {
			// Ограничиваем ширину для цветового редактора
			float colorEditWidth = ImMin(ContentWidth + ImGui::GetFrameHeight() + 7, availWidth * 0.35f);
			AlignRight(colorEditWidth);
			ImGui::ColorEdit4(lable, col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
			ImGui::SameLine();
		}
		else {
			AlignRight(ContentWidth);
		}
		
		Gui.SwitchButton(string, v);
		ImGui::PopID();
	}
	void PutColorEditor(const char* text, const char* lable, float CursorX, float ContentWidth, float col[4], const char* Tip = NULL)
	{
		ImGui::PushID(text);
		
		// Получаем доступную ширину
		float availWidth = ImGui::GetContentRegionAvail().x;
		
		// Ограничиваем CursorX и ContentWidth
		CursorX = ImMin(CursorX, availWidth * 0.25f);
		ContentWidth = ImMin(ContentWidth, availWidth * 0.35f);
		
		float CurrentCursorX = ImGui::GetCursorPosX();
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		
		// Отображаем текст с ограничением длины
		ImGui::TextDisabled(text);
		
		if (Tip && ImGui::IsItemHovered())
			ImGui::SetTooltip(Tip);
		
		ImGui::SameLine();
		
		// Ограничиваем ширину цветового редактора
		float colorEditWidth = ImMin(ContentWidth + ImGui::GetFrameHeight() + 8, availWidth * 0.4f);
		AlignRight(colorEditWidth);
		
		// Используем упрощенный режим для цветового редактора
		ImGui::ColorEdit4(lable, col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
		
		ImGui::PopID();
	}
	void PutSliderFloat(const char* string, float CursorX, float* v, const void* p_min, const void* p_max, const char* format, const char* Tip = NULL)
	{
		// Добавляем уникальный ID для каждого слайдера
		ImGui::PushID(string);
		
		// Получаем доступную ширину
		float availWidth = ImGui::GetContentRegionAvail().x;
		
		// Ограничиваем CursorX
		CursorX = ImMin(CursorX, availWidth * 0.25f);
		
		float CurrentCursorX = ImGui::GetCursorPosX();
		
		// Рассчитываем ширину слайдера с ограничением
		float SliderWidth = ImMin(availWidth - CursorX - 50, availWidth * 0.5f);
		SliderWidth = ImMax(SliderWidth, 50.0f); // Минимальная ширина слайдера
		
		// Устанавливаем позицию для текста
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		ImGui::TextDisabled(string);
		
		// Показываем подсказку при наведении
		if (Tip && ImGui::IsItemHovered())
			ImGui::SetTooltip(Tip);
		
		// Отображаем текущее значение справа от текста
		ImGui::SameLine();
		
		// Ограничиваем длину формата для безопасности
		char buffer[32];
		snprintf(buffer, sizeof(buffer), format, *v);
		ImGui::TextDisabled("%s", buffer);
		
		// Устанавливаем позицию для слайдера
		ImGui::SetCursorPosX(CurrentCursorX + CursorX);
		ImGui::SetNextItemWidth(SliderWidth);
		
		// Используем улучшенный слайдер
		Gui.SliderScalarEx2("", ImGuiDataType_Float, v, p_min, p_max, "", ImGuiSliderFlags_None);
		
		ImGui::PopID();
	}
	void PutSliderInt(const char* string, float CursorX, int* v, const void* p_min, const void* p_max, const char* format, const char* Tip = NULL)
{
    ImGui::PushID(string);
    
    // Получаем доступную ширину
    float availWidth = ImGui::GetContentRegionAvail().x;
    
    // Ограничиваем CursorX
    CursorX = ImMin(CursorX, availWidth * 0.25f);
    
    float CurrentCursorX = ImGui::GetCursorPosX();
    
    // Рассчитываем ширину слайдера с ограничением
    float SliderWidth = ImMin(availWidth - CursorX - 50, availWidth * 0.5f);
    SliderWidth = ImMax(SliderWidth, 50.0f); // Минимальная ширина слайдера
    
    // Устанавливаем позицию для текста
    ImGui::SetCursorPosX(CurrentCursorX + CursorX);
    ImGui::TextDisabled(string);
    
    // Показываем подсказку при наведении
    if (Tip && ImGui::IsItemHovered())
        ImGui::SetTooltip(Tip);
    
    // Отображаем текущее значение справа от текста
    ImGui::SameLine();
    
    // Ограничиваем длину формата для безопасности
    char buffer[32];
    snprintf(buffer, sizeof(buffer), format, *v);
    ImGui::TextDisabled("%s", buffer);
    
    // Устанавливаем позицию для слайдера
    ImGui::SetCursorPosX(CurrentCursorX + CursorX);
    ImGui::SetNextItemWidth(SliderWidth);
    
    // Используем улучшенный слайдер для целых чисел
    Gui.SliderScalarEx2("", ImGuiDataType_S32, v, p_min, p_max, "", ImGuiSliderFlags_None);
    
    ImGui::PopID();
}
	// ########################################

	void DrawGui()
	{
		// LoadImages();//maybe us

		ImColor BorderColor = ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border));

		// Устанавливаем минимальный размер окна
		const ImVec2 minWindowSize(700.0f, 500.0f);
		
		// Проверяем и корректируем размер окна
		if (MenuConfig::WCS.MainWinSize.x < minWindowSize.x)
			MenuConfig::WCS.MainWinSize.x = minWindowSize.x;
		if (MenuConfig::WCS.MainWinSize.y < minWindowSize.y)
			MenuConfig::WCS.MainWinSize.y = minWindowSize.y;
		
		// Устанавливаем флаги окна без возможности изменения размера
		ImGuiWindowFlags Flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar;
		
		// Центрируем окно на экране
		ImGui::SetNextWindowPos({ (ImGui::GetIO().DisplaySize.x - MenuConfig::WCS.MainWinSize.x) / 2.0f, (ImGui::GetIO().DisplaySize.y - MenuConfig::WCS.MainWinSize.y) / 2.0f }, ImGuiCond_Once);
		ImGui::SetNextWindowSize(MenuConfig::WCS.MainWinSize);
		
		// Dark Blue theme colors
		ImVec4 primaryColor = ImVec4(0.2f, 0.4f, 0.8f, 1.0f);      // Bright blue primary
		ImVec4 secondaryColor = ImVec4(0.15f, 0.3f, 0.65f, 1.0f);  // Darker blue
		ImVec4 accentColor = ImVec4(0.3f, 0.6f, 1.0f, 1.0f);       // Light blue accent
		ImVec4 backgroundColor = ImVec4(0.05f, 0.08f, 0.15f, 1.0f); // Very dark blue background
		ImVec4 panelColor = ImVec4(0.08f, 0.12f, 0.20f, 1.0f);     // Dark blue panel
		ImVec4 sidebarColor = ImVec4(0.06f, 0.10f, 0.18f, 1.0f);   // Sidebar background
		ImVec4 textColor = ImVec4(0.85f, 0.90f, 0.95f, 1.0f);      // Light blue-white text
		ImVec4 borderColor = ImVec4(0.15f, 0.25f, 0.45f, 1.0f);    // Blue border
		ImVec4 hoverColor = ImVec4(0.25f, 0.45f, 0.75f, 0.3f);     // Hover effect
		
		// Apply custom styling
		ImGui::PushStyleColor(ImGuiCol_WindowBg, backgroundColor);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, panelColor);
		ImGui::PushStyleColor(ImGuiCol_Border, borderColor);
		ImGui::PushStyleColor(ImGuiCol_Text, textColor);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
		
		ImGui::Begin("Zender", nullptr, Flags);
		{
			// Animation variables
			static float tabAnimationTime = 0.0f;
			static int lastActiveTab = 0;
			static int activeTab = 0;
			static float sidebarWidth = 200.0f;
			static float headerHeight = 80.0f;
			
			// Update animation
			tabAnimationTime += ImGui::GetIO().DeltaTime * 8.0f;
			if (tabAnimationTime > 1.0f) tabAnimationTime = 1.0f;
			
			if (lastActiveTab != activeTab) {
				tabAnimationTime = 0.0f;
				lastActiveTab = activeTab;
			}
			
			// Sidebar with tabs
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
				
				// Footer with unhook button
			}
			ImGui::EndChild();
			ImGui::PopStyleColor();
			
			// Main content area
			ImGui::SameLine();
			ImGui::BeginChild("Content", ImVec2(-1, -1), true, ImGuiWindowFlags_NoScrollbar);
			{
				// Content header with smooth transition
				float contentAlpha = tabAnimationTime;
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
				
				// ESP Tab
				if (activeTab == 0)
				{
					static const float MinRounding = 0.f, MaxRouding = 5.f;
					static const float MinFovFactor = 0.f, MaxFovFactor = 1.f;
					ImGui::Columns(2, nullptr, false);
					
					ImGui::SetCursorPosX(25.f);
					ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
					ImGui::Text("VISUAL ESP");
					ImGui::PopStyleColor();
					ImGui::Spacing();
					
					PutSwitch(Text::ESP::Enable.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ESPenabled);
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15.f);
					ImGui::TextDisabled(Text::ESP::HotKeyList.c_str());
					ImGui::SameLine();
					AlignRight(75.f);
					
					// Styled hotkey button
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.15f, 0.25f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
					if (ImGui::Button(Text::ESP::HotKey.c_str(), { 75.f, 28.f }))
					{
						std::thread([&]() {
							KeyMgr::GetPressedKey(ESPConfig::HotKey, &Text::ESP::HotKey);
							}).detach();
					}
					ImGui::PopStyleColor(2);
					ImGui::PopStyleVar();

					if (ESPConfig::ESPenabled)
					{
						PutSwitch(Text::ESP::Box.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowBoxESP, true, "###BoxCol", reinterpret_cast<float*>(&ESPConfig::BoxColor));
						if (ESPConfig::ShowBoxESP)
						{
							PutSwitch(Text::ESP::Outline.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::OutLine);
							ImGui::TextDisabled(Text::ESP::BoxType.c_str());
							ImGui::SameLine();
							AlignRight(165.f);
							ImGui::SetNextItemWidth(165.f);
							ImGui::Combo("###BoxType", &ESPConfig::BoxType, "Normal\0Corner\0");
							PutSliderFloat(Text::ESP::BoxRounding.c_str(), 15.f, &ESPConfig::BoxRounding, &MinRounding, &MaxRouding, "%.1f");
						}
						PutSwitch(Text::ESP::HeadBox.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowHeadBox, true, "###HeadBoxCol", reinterpret_cast<float*>(&ESPConfig::HeadBoxColor));
						PutSwitch(Text::ESP::Skeleton.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowBoneESP, true, "###BoneCol", reinterpret_cast<float*>(&ESPConfig::BoneColor));
						PutSwitch(Text::ESP::OutOfFOVArrow.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowOutOfFOVArrow, true, "###OutFOVCol", reinterpret_cast<float*>(&ESPConfig::OutOfFOVArrowColor));
						if(ESPConfig::ShowOutOfFOVArrow)
							PutSliderFloat(Text::ESP::OutOfFOVRadius.c_str(), .5f, &ESPConfig::OutOfFOVRadiusFactor, &MinFovFactor, &MaxFovFactor, "%.1f");

						PutSwitch(Text::ESP::HealthBar.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowHealthBar);
						PutSwitch("Armor Bar", 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowArmorBar);
						PutSwitch(Text::ESP::Weapon.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowWeaponESP);
						PutSwitch("Info", 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowWeaponInfo);
						if (ESPConfig::ShowWeaponInfo) {
							PutSwitch("Show Bomb Carrier", 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowBombCarrier);
						}
						PutSwitch(Text::ESP::PlayerName.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::ShowPlayerName);
						PutSwitch(Text::ESP::FlashCheck.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::FlashCheck);
						PutSwitch(Text::ESP::VisCheck.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7, &ESPConfig::VisibleCheck, true, "###VisibleCol", reinterpret_cast<float*>(&ESPConfig::VisibleColor));
					}
					ImGui::NewLine();
					
					ImGui::NextColumn();
					ImGui::Columns(1);
				}

				// Legit Tab
				if (activeTab == 1)
				{
					ImGui::Columns(2, nullptr, false);
					
					ImGui::SetCursorPosX(25.f);
					ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
					ImGui::Text("LEGIT SETTINGS");
					ImGui::PopStyleColor();
					ImGui::Spacing();
					
					PutSwitch("Anti Flash", 15.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::AntiFlash);
					
					ImGui::Spacing();
					ImGui::SetCursorPosX(25.f);
					ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
					ImGui::Text("VISUAL");
					ImGui::PopStyleColor();
					ImGui::Spacing();
					
					PutSwitch("Sniper Crosshair", 15.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::SniperCrosshair);
					
					ImGui::NextColumn();
					
					ImGui::SetCursorPosX(25.f);
					ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
					ImGui::Text("PROTECTION");
					ImGui::PopStyleColor();
					ImGui::Spacing();
					
					PutSwitch("Anti Record", 15.f, ImGui::GetFrameHeight() * 1.7, &MenuConfig::BypassOBS);
					PutSwitch("Anti AFK Kick", 15.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::AntiAFKKick);
					PutSwitch("Wallbang Indicator", 15.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::WallbangIndicator);
					
					if (MiscCFG::WallbangIndicator) {
						ImGui::SetCursorPosX(25.f);
						ImGui::TextDisabled("Sensitivity:");
						ImGui::SameLine();
						ImGui::SetNextItemWidth(125.f);
						ImGui::SliderFloat("###WallbangSensitivity", &MiscCFG::WallbangSensitivity, 0.0f, 1.0f, "%.2f");
					}
					
					ImGui::Columns(1);
				}

				// Aimbot Tab
				if (activeTab == 2)
				{
					ImGui::Columns(2, nullptr, false);
					
					ImGui::SetCursorPosX(25.f);
					ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
					ImGui::Text("AIMBOT SETTINGS");
					ImGui::PopStyleColor();
					ImGui::Spacing();
					
					PutSwitch("Enable Aimbot", 15.f, ImGui::GetFrameHeight() * 1.7, &AimbotCFG::AimbotEnabled);
					PutSwitch("Silent Aim", 15.f, ImGui::GetFrameHeight() * 1.7, &AimbotCFG::SilentAim);
					PutSwitch("Visible Check", 15.f, ImGui::GetFrameHeight() * 1.7, &AimbotCFG::VisibleCheck);
					PutSwitch("Team Check", 15.f, ImGui::GetFrameHeight() * 1.7, &MenuConfig::TeamCheck);
					PutSwitch("Toggle Mode", 15.f, ImGui::GetFrameHeight() * 1.7, &AimbotCFG::ToggleMode);
					
					// Aimbot key bind
					ImGui::TextDisabled("Aimbot Key:");
					ImGui::SameLine();
					AlignRight(75.f);
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.f);
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.22f, 0.25f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, secondaryColor);
					if (ImGui::Button("AimKey", { 75.f, 28.f }))
					{
						std::thread([&]() {
							KeyMgr::GetPressedKey(AimbotCFG::AimbotKey, nullptr);
							}).detach();
					}
					ImGui::PopStyleColor(2);
					ImGui::PopStyleVar();
					
					ImGui::Spacing();
					ImGui::SetCursorPosX(25.f);
					ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
					ImGui::Text("TARGETING");
					ImGui::PopStyleColor();
					ImGui::Spacing();
					
					// Bone selection
					ImGui::TextDisabled("Aim Bone:");
					ImGui::SameLine();
					AlignRight(120.f);
					ImGui::SetNextItemWidth(120.f);
					const char* boneItems[] = { "Head", "Neck", "Chest", "Stomach", "Pelvis", "Auto" };
					ImGui::Combo("###AimBone", &AimbotCFG::AimBone, boneItems, IM_ARRAYSIZE(boneItems));
					
					PutSwitch("Target Head Only", 15.f, ImGui::GetFrameHeight() * 1.7, &AimbotCFG::TargetHeadOnly);
					PutSwitch("Stop When No Head", 15.f, ImGui::GetFrameHeight() * 1.7, &AimbotCFG::StopWhenNoHead);
					PutSwitch("Predict Movement", 15.f, ImGui::GetFrameHeight() * 1.7, &AimbotCFG::PredictMovement);
					
					ImGui::NextColumn();
					
					ImGui::SetCursorPosX(25.f);
					ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
					ImGui::Text("AIMING");
					ImGui::PopStyleColor();
					ImGui::Spacing();
					
					PutSwitch("Smooth Aim", 15.f, ImGui::GetFrameHeight() * 1.7, &AimbotCFG::SmoothAim);
					
					if (AimbotCFG::SmoothAim) {
						float minSmooth = 0.0f, maxSmooth = 1.0f;
						PutSliderFloat("Smooth Value", 15.f, &AimbotCFG::SmoothValue, &minSmooth, &maxSmooth, "%.2f");
					}
					
					ImGui::Spacing();
					ImGui::SetCursorPosX(25.f);
					ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
					ImGui::Text("DISTANCE & FOV");
					ImGui::PopStyleColor();
					ImGui::Spacing();
					
					float minDist = 100.0f, maxDist = 10000.0f;
					PutSliderFloat("Max Distance", 15.f, &AimbotCFG::MaxDistance, &minDist, &maxDist, "%.0f");
					
					// FOV Radius with min/max limits
					ImGui::TextDisabled("FOV Radius:");
					ImGui::SameLine();
					char fovBuffer[32];
					snprintf(fovBuffer, sizeof(fovBuffer), "%.1f", AimbotCFG::FOVRadius);
					ImGui::TextDisabled("%s", fovBuffer);
					ImGui::SetCursorPosX(15.f);
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 20);
					Gui.SliderScalarEx2("###FOVRadius", ImGuiDataType_Float, &AimbotCFG::FOVRadius, &AimbotCFG::FOVRadiusMin, &AimbotCFG::FOVRadiusMax, "", ImGuiSliderFlags_None);
					
					ImGui::Spacing();
					ImGui::SetCursorPosX(25.f);
					ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
					ImGui::Text("AUTO FIRE");
					ImGui::PopStyleColor();
					ImGui::Spacing();
					
					// AutoFire mode selection
					ImGui::TextDisabled("Auto Fire Mode:");
					ImGui::SameLine();
					AlignRight(120.f);
					ImGui::SetNextItemWidth(120.f);
					const char* autoFireItems[] = { "Off", "On Sight", "Auto Aim" };
					ImGui::Combo("###AutoFireMode", &AimbotCFG::AutoFireMode, autoFireItems, IM_ARRAYSIZE(autoFireItems));
					
					ImGui::Spacing();
					ImGui::SetCursorPosX(25.f);
					ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
					ImGui::Text("RECOIL CONTROL");
					ImGui::PopStyleColor();
					ImGui::Spacing();
					
					PutSwitch("Enable RCS", 15.f, ImGui::GetFrameHeight() * 1.7, &AimbotCFG::RCS);
					
					if (AimbotCFG::RCS) {
						float minRCS = 0.1f, maxRCS = 3.0f;
						PutSliderFloat("RCS Strength", 15.f, &AimbotCFG::RCSStrength, &minRCS, &maxRCS, "%.2f");
					}
					
					ImGui::Spacing();
					ImGui::SetCursorPosX(25.f);
					ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
					ImGui::Text("VISUALIZATION");
					ImGui::PopStyleColor();
					ImGui::Spacing();
					
					PutSwitch("Show FOV", 15.f, ImGui::GetFrameHeight() * 1.7, &AimbotCFG::ShowFov);
					
					if (AimbotCFG::ShowFov) {
						PutSwitch("Draw FOV Circle", 15.f, ImGui::GetFrameHeight() * 1.7, &AimbotCFG::DrawAimbotFOV, true, "###FOVCircleCol", reinterpret_cast<float*>(&AimbotCFG::FOVCircleColor));
						
						float minThickness = 0.5f, maxThickness = 5.0f;
						PutSliderFloat("FOV Thickness", 15.f, &AimbotCFG::FOVCircleThickness, &minThickness, &maxThickness, "%.1f");
					}
					
					ImGui::Columns(1);
				}

				// Misc Tab
				if (activeTab == 3) // activeTab for Misc is now 3
				{
					ImGui::Columns(2, nullptr, false);
					
					ImGui::SetCursorPosX(25.f);
					ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
					ImGui::Text("MOVEMENT");
					ImGui::PopStyleColor();
					ImGui::Spacing();
					
					PutSwitch(Text::Misc::BunnyHop.c_str(), 15.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::BunnyHop, false, NULL, NULL, Text::Misc::InsecureTip.c_str());
					
					ImGui::Spacing();
					ImGui::SetCursorPosX(25.f);
					ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
					ImGui::Text("AUTO");
					ImGui::PopStyleColor();
					ImGui::Spacing();
					
					PutSwitch("Knife bot", 15.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::AutoKnife);
					PutSwitch("Zeus bot", 15.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::AutoZeus);
					PutSwitch("Auto Shop", 15.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::AutoShop);
					
					if (MiscCFG::AutoShop) {
						ImGui::SetCursorPosX(15.f);
						ImGui::TextDisabled("Hotkey:");
						ImGui::SameLine();
						ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.f);
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.22f, 0.25f, 1.0f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, secondaryColor);
						if (ImGui::Button("AutoShopKey", { 90.f, 30.f }))
						{
							std::thread([&]() {
								KeyMgr::GetPressedKey(MiscCFG::AutoShopKey, &Text::Misc::AutoShopKey);
								}).detach();
						}
						ImGui::PopStyleColor(2);
						ImGui::PopStyleVar();
					}

					ImGui::NextColumn();
					float CurrentCursorX = ImGui::GetCursorPosX();
					ImGui::SetCursorPosX(CurrentCursorX + 15.f);
					ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
					ImGui::Text("SETTINGS");
					ImGui::PopStyleColor();
					ImGui::Spacing();
					
					ImGui::SetCursorPosX(CurrentCursorX + 10.f);
					ImGui::TextDisabled(Text::Misc::MenuKey.c_str());
					ImGui::SameLine();
					AlignRight(90.f);
					
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.f);
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.22f, 0.25f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, secondaryColor);
					if (ImGui::Button(Text::Misc::HotKey.c_str(), { 90.f, 30.f }))
					{
						std::thread([&]() {
							KeyMgr::GetPressedKey(MenuConfig::HotKey, &Text::Misc::HotKey);
							}).detach();
					}
					ImGui::PopStyleColor(2);
					ImGui::PopStyleVar();
					
					ImGui::Spacing();
					ImGui::SetCursorPosX(CurrentCursorX + 15.f);
					ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
					ImGui::Text("VISUAL");
					ImGui::PopStyleColor();
					ImGui::Spacing();
					
					PutSwitch("Water Mark", 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::WaterMark);
					
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
					
					PutSwitch("Hit Marker", 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::HitMarker);
					PutSwitch("Fatality Sound", 10.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::FatalitySound);
					
					ImGui::Columns(1);
				}
				
				// List Tab
				if (activeTab == 4)
				{
					ImGui::Columns(2, nullptr, false);
					
					ImGui::SetCursorPosX(25.f);
					ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
					ImGui::Text("GAME LISTS");
					ImGui::PopStyleColor();
					ImGui::Spacing();
					
					PutSwitch("Spectator List", 15.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::SpecList);
					PutSwitch("Bomb Timer", 15.f, ImGui::GetFrameHeight() * 1.7, &MiscCFG::bmbTimer);
					
					ImGui::Spacing();
					ImGui::SetCursorPosX(25.f);
					ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
					ImGui::Text("INFORMATION");
					ImGui::PopStyleColor();
					ImGui::Spacing();
					
					ImGui::TextDisabled("Spectator List shows");
					ImGui::TextDisabled("who is watching you");
					ImGui::Spacing();
					ImGui::TextDisabled("Bomb Timer shows");
					ImGui::TextDisabled("explosion time");
					
					ImGui::NextColumn();
					
					ImGui::SetCursorPosX(25.f);
					ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
					ImGui::Text("SETTINGS");
					ImGui::PopStyleColor();
					ImGui::Spacing();
					
					ImGui::TextDisabled("Window Positions");
					ImGui::Spacing();
					
					ImGui::TextDisabled("Spectator List:");
					ImGui::SameLine();
					char specPos[64];
					sprintf_s(specPos, "X: %.0f Y: %.0f", MenuConfig::SpecWinPos.x, MenuConfig::SpecWinPos.y);
					ImGui::Text(specPos);
					
					ImGui::Spacing();
					ImGui::TextDisabled("Bomb Timer:");
					ImGui::SameLine();
					char bombPos[64];
					sprintf_s(bombPos, "X: %.0f Y: %.0f", MenuConfig::BombWinPos.x, MenuConfig::BombWinPos.y);
					ImGui::Text(bombPos);
					
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::SetCursorPosX(25.f);
					ImGui::PushStyleColor(ImGuiCol_Text, primaryColor);
					ImGui::Text("ACTIONS");
					ImGui::PopStyleColor();
					ImGui::Spacing();
					
					// Styled reset button
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.f);
					ImGui::PushStyleColor(ImGuiCol_Button, primaryColor);
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, secondaryColor);
					if (ImGui::Button("Reset Positions", { 125.f, 34.f }))
					{
						MenuConfig::SpecWinPos = ImVec2(10.0f, 340.0f);
						MenuConfig::BombWinPos = ImVec2(860.0f, 80.0f);
					}
					ImGui::PopStyleColor(2);
					ImGui::PopStyleVar();
					
					ImGui::Columns(1);
				}

				// Config Tab
				if (activeTab == 5) // activeTab for Config is now 5
				{
					ConfigMenu::RenderCFGmenu();
				}
			}
			ImGui::EndChild(); // End Content child
		} 
		ImGui::End();
		
		// Pop all the style colors and vars we pushed
		ImGui::PopStyleColor(4);
		ImGui::PopStyleVar(4);

		static bool firstFrame = true;
		if (firstFrame) {
			LoadDefaultConfig();
			firstFrame = false;
		}
	}
}
