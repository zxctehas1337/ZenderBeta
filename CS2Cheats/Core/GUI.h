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

// Предварительные объявления для уменьшения зависимостей
namespace ESP { }
namespace Render { class HealthBar; }
struct EntityResult;

namespace GUI
{
    // Основные функции GUI
    void DrawGui();
    void LoadDefaultConfig();
    void LoadImages();

    // Компоненты интерфейса
    namespace Components {
        void AlignRight(float ContentWidth);
        void PutSwitch(const char* string, float CursorX, float ContentWidth, bool* v, bool ColorEditor = false, const char* lable = NULL, float col[4] = NULL, const char* Tip = NULL);
        void PutColorEditor(const char* text, const char* lable, float CursorX, float ContentWidth, float col[4], const char* Tip = NULL);
        void PutSliderFloat(const char* string, float CursorX, float* v, const void* p_min, const void* p_max, const char* format, const char* Tip = NULL);
        void PutSliderInt(const char* string, float CursorX, int* v, const void* p_min, const void* p_max, const char* format, const char* Tip = NULL);
        void SectionSeparator(); // Визуальный разделитель секций
    }

    // Вкладки
    namespace Tabs {
        void DrawESPTab(ImVec4 primaryColor, ImVec4 hoverColor);
        void DrawLegitTab(ImVec4 primaryColor, ImVec4 secondaryColor);
        void DrawAimbotTab(ImVec4 primaryColor, ImVec4 secondaryColor);
        void DrawMiscTab(ImVec4 primaryColor, ImVec4 secondaryColor);
        void DrawListsTab(ImVec4 primaryColor, ImVec4 secondaryColor);
        void DrawConfigTab();
    }

    // Панель инструментов
    namespace ToolBar {
        void DrawSidebar(float sidebarWidth, float headerHeight, ImVec4 sidebarColor, ImVec4 accentColor, 
                        ImVec4 primaryColor, ImVec4 hoverColor, float& tabAnimationTime, int& activeTab, int& lastActiveTab);
        void DrawHeader(float contentAlpha, ImVec4 accentColor, int activeTab);
    }
}
