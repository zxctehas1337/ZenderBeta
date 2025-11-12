#include "Render.h"

namespace Render
{
    // Implementation of HealthBar methods
    void HealthBar::HealthBarV(float MaxHealth, float CurrentHealth, const ImVec2& Pos, const ImVec2& Size, bool ShowNum)
    {
        ImDrawList* DrawList = ImGui::GetBackgroundDrawList();

        this->MaxHealth = MaxHealth;
        this->CurrentHealth = CurrentHealth;
        this->RectPos = Pos;
        this->RectSize = Size;

        float proportion = (MaxHealth > 0.f) ? CurrentHealth / MaxHealth : 0.f;
        proportion = (proportion < 0.f) ? 0.f : ((proportion > 1.f) ? 1.f : proportion);

        float height = RectSize.y * proportion;
        ImVec2 rectBR = { RectPos.x + RectSize.x, RectPos.y + RectSize.y };

        DrawList->AddRectFilled(RectPos, rectBR, BackGroundColor, 5, 15);

        float colorLerpT = powf(proportion, 2.5f);
        ImColor color = (proportion > 0.5f && proportion <= 1.f) ?
            Mix(FirstStageColor, SecondStageColor, colorLerpT * 3.f - 1.f) :
            Mix(SecondStageColor, ThirdStageColor, colorLerpT * 4.f);

        ImVec2 healthRectTL = { RectPos.x, RectPos.y + RectSize.y - height };
        DrawList->AddRectFilled(healthRectTL, rectBR, color, 0);

        DrawList->AddRect(RectPos, rectBR, FrameColor, 0, 15, 1);

        if (ShowNum && CurrentHealth < MaxHealth)
        {
            char healthStr[16];
            snprintf(healthStr, sizeof(healthStr), "%.f", CurrentHealth);
            Gui.StrokeText(healthStr, healthRectTL, ImColor(255, 255, 255), 13.f, true);
        }
    }

    void HealthBar::ArmorBarV(bool HasHelmet, float MaxArmor, float CurrentArmor, const ImVec2& Pos, const ImVec2& Size, bool ShowNum)
    {
        ImDrawList* DrawList = ImGui::GetBackgroundDrawList();

        this->MaxArmor = MaxArmor;
        this->CurrentArmor = CurrentArmor;
        this->RectPos = Pos;
        this->RectSize = Size;

        float proportion = (MaxArmor > 0.f) ? CurrentArmor / MaxArmor : 0.f;
        proportion = (proportion < 0.f) ? 0.f : ((proportion > 1.f) ? 1.f : proportion);

        float height = RectSize.y * proportion;
        ImVec2 rectBR = { RectPos.x + RectSize.x, RectPos.y + RectSize.y };

        DrawList->AddRectFilled(RectPos, rectBR, BackGroundColor, 5, 15);

        ImColor color = HasHelmet ? ArmorWithHelmetColor : ArmorColor;
        ImVec2 armorRectTL = { RectPos.x, RectPos.y + RectSize.y - height };
        DrawList->AddRectFilled(armorRectTL, rectBR, color, 0);

        DrawList->AddRect(RectPos, rectBR, FrameColor, 0, 15, 1);

        if (ShowNum && CurrentArmor < MaxArmor)
        {
            char armorStr[16];
            snprintf(armorStr, sizeof(armorStr), "%.f", CurrentArmor);
            Gui.StrokeText(armorStr, armorRectTL, ImColor(255, 255, 255), 13.f, true);
        }
    }

    void HealthBar::AmmoBarH(float MaxAmmo, float CurrentAmmo, const ImVec2& Pos, const ImVec2& Size)
    {
        ImDrawList* DrawList = ImGui::GetBackgroundDrawList();

        this->MaxAmmo = MaxAmmo;
        this->CurrentAmmo = CurrentAmmo;
        this->RectPos = Pos;
        this->RectSize = Size;

        float proportion = (MaxAmmo > 0) ? CurrentAmmo / static_cast<float>(MaxAmmo) : 0.f;
        proportion = (proportion < 0.f) ? 0.f : ((proportion > 1.f) ? 1.f : proportion);

        float width = RectSize.x * proportion;
        ImVec2 rectBR = { RectPos.x + RectSize.x, RectPos.y + RectSize.y };

        DrawList->AddRectFilled(RectPos, rectBR, BackGroundColor, 5, 15);

        ImVec2 ammoRectBR = { RectPos.x + width, RectPos.y + RectSize.y };
        DrawList->AddRectFilled(RectPos, ammoRectBR, AmmoColor, 0);

        DrawList->AddRect(RectPos, rectBR, FrameColor, 0, 15, 1);
    }

    ImVec2 GetScreenCenterImVec2()
    {
        int W = GetSystemMetrics(SM_CXSCREEN);
        int H = GetSystemMetrics(SM_CYSCREEN);
        return ImVec2(W / 2.0f, H / 2.0f);
    }
}

// Implement the global functions outside the namespace
void Render::DrawHealthBar(DWORD Sign, float MaxHealth, float CurrentHealth, const ImVec2& Pos, const ImVec2& Size)
{
    static std::unordered_map<DWORD, Render::HealthBar> HealthBarMap;
    Render::HealthBar& hb = HealthBarMap[Sign];
    hb.HealthBarV(MaxHealth, CurrentHealth, Pos, Size, ESPConfig::ShowHealthNum);
}

void Render::DrawAmmoBar(DWORD Sign, float MaxAmmo, float CurrentAmmo, const ImVec2& Pos, const ImVec2& Size)
{
    static std::unordered_map<DWORD, Render::HealthBar> HealthBarMap;
    Render::HealthBar& hb = HealthBarMap[Sign];
    hb.AmmoBarH(MaxAmmo, CurrentAmmo, Pos, Size);
}

void Render::DrawArmorBar(DWORD Sign, float MaxArmor, float CurrentArmor, bool HasHelmet, const ImVec2& Pos, const ImVec2& Size)
{
    static std::unordered_map<DWORD, Render::HealthBar> HealthBarMap;
    Render::HealthBar& hb = HealthBarMap[Sign];
    hb.ArmorBarV(HasHelmet, MaxArmor, CurrentArmor, Pos, Size, ESPConfig::ShowArmorNum);
}
