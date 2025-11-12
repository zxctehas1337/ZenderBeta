#include "GUI.h"
#include "../Features/ESP.h"
#include "../Features/Aimbot.h"

namespace GUI
{

    namespace Components
    {
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
    }
}
