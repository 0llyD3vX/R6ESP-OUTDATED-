#include "pch.h"
#include "esp.h"
#include "renderer.h"
#include "structures.h"
#include <algorithm>

// ------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------
ESP::ESP() : m_renderer(NULL), m_menuOpen(false), m_enabled(true),
m_showBox(true), m_showSnapline(true), m_showDistance(true),
m_showHealthBar(true), m_showName(true), m_showCrosshair(true),
m_maxDistance(1000.0f) {
}

// ------------------------------------------------------------------
// Initialize
// ------------------------------------------------------------------
void ESP::Initialize(Renderer* renderer) {
    m_renderer = renderer;
}

// ------------------------------------------------------------------
// Main Draw function
// ------------------------------------------------------------------
void ESP::Draw(GameData* data) {
    if (!m_enabled || !data || !m_renderer) {
        return;
    }

    // --- Debug: print player count every 60 frames ---
    static int frameCounter = 0;
    frameCounter++;
    if (frameCounter % 60 == 0) {
        printf("[ESP] Frame %d | PlayerCount: %d | MenuOpen: %d\n",
            frameCounter, data->playerCount, m_menuOpen);
    }

    // --- Draw crosshair ---
    if (m_showCrosshair) {
        DrawCrosshair();
    }

    // --- Draw players ---
    if (data->playerCount > 0) {
        for (int i = 0; i < data->playerCount; i++) {
            PlayerData* player = &data->players[i];
            if (!player->isValid) continue;
            if (player->distance > m_maxDistance) continue;
            if (player->screenPosition.x == 0 && player->screenPosition.y == 0) continue;
            if (player->screenPosition.x < -10000 || player->screenPosition.x > 10000) continue;
            if (player->screenPosition.y < -10000 || player->screenPosition.y > 10000) continue;
            DrawPlayerESP(player);
        }
    }

    // --- Draw menu ONLY if open ---
    if (m_menuOpen) {
        DrawMenu();
    }
}

// ------------------------------------------------------------------
// Draw a single player's ESP
// ------------------------------------------------------------------
void ESP::DrawPlayerESP(PlayerData* player) {
    if (!player || !m_renderer) return;

    float x = player->screenPosition.x;
    float y = player->screenPosition.y;
    float distance = player->distance;

    float boxSize = 1000.0f / distance * 50.0f;
    if (boxSize < 10.0f) boxSize = 10.0f;
    if (boxSize > 150.0f) boxSize = 150.0f;

    float boxWidth = boxSize * 0.6f;
    float boxHeight = boxSize * 1.2f;

    float healthRatio = player->health / 100.0f;
    float red = 1.0f - healthRatio;
    float green = healthRatio;
    float blue = 0.0f;

    if (m_showBox) {
        DrawBox(x - boxWidth / 2, y - boxHeight, boxWidth, boxHeight, red, green, blue, 1.0f);
    }

    if (m_showSnapline) {
        DrawSnapline(x, y, red, green, blue, 0.7f);
    }

    if (m_showDistance) {
        char distText[32];
        sprintf_s(distText, sizeof(distText), "%.0fm", distance);
        m_renderer->DrawText(x - 20, y + 5, distText, 1.0f, 1.0f, 1.0f, 1.0f);
    }

    if (m_showHealthBar) {
        DrawHealthBar(x - boxWidth / 2, y - boxHeight - 10, boxWidth, 5, player->health);
    }

    if (m_showName) {
        m_renderer->DrawText(x - 20, y - boxHeight - 25, "PLAYER", 1.0f, 1.0f, 1.0f, 1.0f);
    }
}

// ------------------------------------------------------------------
// Box drawing
// ------------------------------------------------------------------
void ESP::DrawBox(float x, float y, float width, float height,
    float r, float g, float b, float a) {
    if (!m_renderer) return;
    m_renderer->DrawLine(x, y, x + width, y, r, g, b, a);
    m_renderer->DrawLine(x, y + height, x + width, y + height, r, g, b, a);
    m_renderer->DrawLine(x, y, x, y + height, r, g, b, a);
    m_renderer->DrawLine(x + width, y, x + width, y + height, r, g, b, a);
}

// ------------------------------------------------------------------
// Snapline
// ------------------------------------------------------------------
void ESP::DrawSnapline(float x, float y, float r, float g, float b, float a) {
    if (!m_renderer) return;
    float cx = m_renderer->GetWidth() / 2.0f;
    float cy = m_renderer->GetHeight();
    m_renderer->DrawLine(cx, cy, x, y, r, g, b, a);
}

// ------------------------------------------------------------------
// Health bar
// ------------------------------------------------------------------
void ESP::DrawHealthBar(float x, float y, float width, float height, float health) {
    if (!m_renderer) return;
    m_renderer->DrawFilledRect(x, y, width, height, 0.0f, 0.0f, 0.0f, 0.5f);
    float healthWidth = width * (health / 100.0f);
    float r = 1.0f - (health / 100.0f);
    float g = health / 100.0f;
    m_renderer->DrawFilledRect(x, y, healthWidth, height, r, g, 0.0f, 1.0f);
}

// ------------------------------------------------------------------
// Crosshair
// ------------------------------------------------------------------
void ESP::DrawMenu() {
    if (!m_renderer) return;

    int x = 100, y = 100;
    int w = 200, h = 210;
    int titleH = 24;
    int padding = 12;
    int rowH = 22;

    // Drop shadow
    m_renderer->DrawFilledRect(x + 4, y + 4, w, h, 0.0f, 0.0f, 0.0f, 0.6f);

    // Main background
    m_renderer->DrawFilledRect(x, y, w, h, 0.08f, 0.08f, 0.12f, 0.95f);

    // Title bar background
    m_renderer->DrawFilledRect(x, y, w, titleH, 0.15f, 0.05f, 0.35f, 1.0f);

    // Outer border
    m_renderer->DrawRect(x, y, w, h, 0.45f, 0.1f, 0.8f, 1.0f);

    // Inner border (inset by 1px for double-border effect)
    m_renderer->DrawRect(x + 1, y + 1, w - 2, h - 2, 0.25f, 0.05f, 0.45f, 1.0f);

    // Title text (centred-ish)
    m_renderer->DrawText(x + 58, y + 5, "R6 ESP v1.0", 0.85f, 0.5f, 1.0f, 1.0f);

    // Separator line under title
    m_renderer->DrawLine(x + 1, y + titleH, x + w - 1, y + titleH,
        0.45f, 0.1f, 0.8f, 1.0f);

    // Keybind hints
    int ky = y + titleH + 6;
    m_renderer->DrawText(x + padding, ky,
        "[INS] Toggle Menu", 0.55f, 0.55f, 0.65f, 1.0f);
    m_renderer->DrawText(x + padding, ky + rowH,
        "[END] Exit", 0.55f, 0.55f, 0.65f, 1.0f);

    // Separator before toggles
    int sepY = ky + rowH * 2 + 4;
    m_renderer->DrawLine(x + padding, sepY, x + w - padding, sepY,
        0.3f, 0.1f, 0.5f, 1.0f);

    // Feature rows — label on left, coloured indicator on right
    struct Row { const char* label; bool on; };
    Row rows[] = {
        { "ESP",      true },
        { "Box",      true },
        { "Snapline", true },
        { "Distance", true },
        { "Health",   true },
    };

    int fy = sepY + 8;
    for (auto& row : rows) {
        // Feature label
        m_renderer->DrawText(x + padding, fy,
            row.label, 0.88f, 0.88f, 0.95f, 1.0f);

        // Small coloured square indicator
        float ir = row.on ? 0.1f : 0.8f;
        float ig = row.on ? 0.9f : 0.15f;
        float ib = row.on ? 0.3f : 0.15f;
        m_renderer->DrawFilledRect(x + w - padding - 26, fy + 1, 26, 13, ir, ig, ib, 1.0f);
        m_renderer->DrawRect(x + w - padding - 26, fy + 1, 26, 13,
            ir * 0.6f, ig * 0.6f, ib * 0.6f, 1.0f);
        m_renderer->DrawText(x + w - padding - 23, fy,
            row.on ? "ON" : "OFF", 1.0f, 0.0f, 0.4f, 1.0f);

        fy += rowH;
    }
}

// ------------------------------------------------------------------
// Toggle functions
// ------------------------------------------------------------------
void ESP::ToggleMenu() {
    m_menuOpen = !m_menuOpen;
    printf("[ESP] Menu toggled: %d\n", m_menuOpen);
}
void ESP::ToggleESP() { m_enabled = !m_enabled; }
void ESP::ToggleBox() { m_showBox = !m_showBox; }
void ESP::ToggleSnapline() { m_showSnapline = !m_showSnapline; }
void ESP::ToggleDistance() { m_showDistance = !m_showDistance; }
void ESP::ToggleHealthBar() { m_showHealthBar = !m_showHealthBar; }
void ESP::ToggleName() { m_showName = !m_showName; }

void ESP::DrawCrosshair() {
    if (!m_renderer) return;
    int cx = m_renderer->GetWidth() / 2;
    int cy = m_renderer->GetHeight() / 2;
    int len = 10, gap = 3;
    // horizontal
    m_renderer->DrawLine((float)(cx - len - gap), (float)cy, (float)(cx - gap), (float)cy, 1, 1, 1, 1);
    m_renderer->DrawLine((float)(cx + gap), (float)cy, (float)(cx + len + gap), (float)cy, 1, 1, 1, 1);
    // vertical
    m_renderer->DrawLine((float)cx, (float)(cy - len - gap), (float)cx, (float)(cy - gap), 1, 1, 1, 1);
    m_renderer->DrawLine((float)cx, (float)(cy + gap), (float)cx, (float)(cy + len + gap), 1, 1, 1, 1);
}

void ESP::SetMaxDistance(float distance) { m_maxDistance = distance; }