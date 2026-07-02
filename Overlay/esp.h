#pragma once
#include "structures.h"

class Renderer;

class ESP {
private:
    Renderer* m_renderer;
    bool m_menuOpen;
    bool m_enabled;
    bool m_showBox;
    bool m_showSnapline;
    bool m_showDistance;
    bool m_showHealthBar;
    bool m_showName;
    bool m_showCrosshair;
    float m_maxDistance;

    void DrawPlayerESP(PlayerData* player);
    void DrawBox(float x, float y, float width, float height, float r, float g, float b, float a);
    void DrawSnapline(float x, float y, float r, float g, float b, float a);
    void DrawHealthBar(float x, float y, float width, float height, float health);
    void DrawCrosshair();
    void DrawMenu();

public:
    ESP();
    void Initialize(Renderer* renderer);
    void Draw(GameData* data);

    void ToggleMenu();
    void ToggleESP();
    void ToggleBox();
    void ToggleSnapline();
    void ToggleDistance();
    void ToggleHealthBar();
    void ToggleName();
    void ToggleCrosshair();
    void SetMaxDistance(float distance);
};