#include "pch.h"
#include "overlay.h"

OverlayWindow::OverlayWindow() : m_hWnd(NULL), m_isVisible(true), m_isTopmost(true) {}

OverlayWindow::~OverlayWindow() {
    Destroy();
}

bool OverlayWindow::Create(int width, int height) {
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
    wc.lpszClassName = L"RainbowSixESP_Overlay";

    if (!RegisterClassEx(&wc)) {
        return false;
    }

    m_hWnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
        L"RainbowSixESP_Overlay",
        L"Rainbow Six ESP",
        WS_POPUP,
        0, 0, width, height,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );

    if (!m_hWnd) {
        return false;
    }

    SetLayeredWindowAttributes(m_hWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);

    return true;
}

void OverlayWindow::Destroy() {
    if (m_hWnd) {
        DestroyWindow(m_hWnd);
        m_hWnd = NULL;
    }
}

HWND OverlayWindow::GetHandle() const {
    return m_hWnd;
}

void OverlayWindow::Show() {
    m_isVisible = true;
    ShowWindow(m_hWnd, SW_SHOW);
}

void OverlayWindow::Hide() {
    m_isVisible = false;
    ShowWindow(m_hWnd, SW_HIDE);
}

void OverlayWindow::ToggleVisibility() {
    if (m_isVisible) Hide();
    else Show();
}

void OverlayWindow::SetTopmost(bool topmost) {
    m_isTopmost = topmost;
    SetWindowPos(m_hWnd, topmost ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void OverlayWindow::UpdatePosition(int x, int y, int width, int height) {
    SetWindowPos(m_hWnd, NULL, x, y, width, height, SWP_NOZORDER);
}

bool OverlayWindow::IsVisible() const {
    return m_isVisible;
}
