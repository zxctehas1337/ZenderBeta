#include "GUI.h"

namespace GUI
{
    namespace Components
    {
        void SectionSeparator()
        {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        }
    }

    void AlignRight(float ContentWidth)
    {
        float availWidth = ImGui::GetContentRegionAvail().x;
        float targetX = ImGui::GetCursorPosX() + availWidth - ContentWidth - ImGui::GetStyle().ItemSpacing.x;
        ImGui::SetCursorPosX(ImMax(ImGui::GetCursorPosX(), targetX));
    }

    void PutSwitch(const char* string, float CursorX, float ContentWidth, bool* v, bool ColorEditor, const char* lable, float col[4], const char* Tip)
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

    void PutColorEditor(const char* text, const char* lable, float CursorX, float ContentWidth, float col[4], const char* Tip)
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

    void PutSliderFloat(const char* string, float CursorX, float* v, const void* p_min, const void* p_max, const char* format, const char* Tip)
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

    void PutSliderInt(const char* string, float CursorX, int* v, const void* p_min, const void* p_max, const char* format, const char* Tip)
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
        // Removed image loading
    }

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
            ToolBar::DrawSidebar(sidebarWidth, headerHeight, sidebarColor, accentColor, 
                                primaryColor, hoverColor, tabAnimationTime, activeTab, lastActiveTab);
            
            // Main content area
            ImGui::SameLine();
            ImGui::BeginChild("Content", ImVec2(-1, -1), true, ImGuiWindowFlags_NoScrollbar);
            {
                // Content header with smooth transition
                float contentAlpha = tabAnimationTime;
                
                // Draw header with tab title
                ToolBar::DrawHeader(contentAlpha, accentColor, activeTab);
                
                // Draw appropriate tab content
                switch (activeTab) {
                    case 0: // ESP Tab
                        Tabs::DrawESPTab(primaryColor, hoverColor);
                        break;
                    case 1: // Legit Tab
                        Tabs::DrawLegitTab(primaryColor, secondaryColor);
                        break;
                    case 2: // Aimbot Tab
                        Tabs::DrawAimbotTab(primaryColor, secondaryColor);
                        break;
                    case 3: // Misc Tab
                        Tabs::DrawMiscTab(primaryColor, secondaryColor);
                        break;
                    case 4: // Lists Tab
                        Tabs::DrawListsTab(primaryColor, secondaryColor);
                        break;
                    case 5: // Config Tab
                        Tabs::DrawConfigTab();
                        break;
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
