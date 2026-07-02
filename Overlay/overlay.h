#pragma once
#include <windows.h>

class OverlayWindow {
private:
    HWND m_hWnd;
    bool m_isVisible;
    bool m_isTopmost;

public:
    OverlayWindow();
    ~OverlayWindow();

    bool Create(int width, int height);
    void Destroy();
    HWND GetHandle() const;
    void Show();
    void Hide();
    void ToggleVisibility();
    void SetTopmost(bool topmost);
    void UpdatePosition(int x, int y, int width, int height);
    bool IsVisible() const;
};