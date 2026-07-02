#include "pch.h"
#include "renderer.h"
#include "structures.h"
#include <algorithm>

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

Renderer::Renderer() : m_hWnd(NULL), m_width(0), m_height(0),
m_hdc(NULL), m_memDC(NULL), m_hBitmap(NULL),
m_hOldBitmap(NULL), m_pBits(NULL) {
}

bool Renderer::Initialize(HWND hWnd) {
    if (!hWnd) return false;
    m_hWnd = hWnd;

    RECT rect;
    GetClientRect(hWnd, &rect);
    m_width = rect.right - rect.left;
    m_height = rect.bottom - rect.top;

    if (m_width <= 0 || m_height <= 0) return false;

    m_hdc = GetDC(hWnd);
    if (!m_hdc) return false;

    CreateGDISurface();
    return true;
}

void Renderer::Shutdown() {
    DestroyGDISurface();
    if (m_hdc) { ReleaseDC(m_hWnd, m_hdc); m_hdc = NULL; }
    m_hWnd = NULL;
}

void Renderer::CreateGDISurface() {
    if (m_width <= 0 || m_height <= 0) return;

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = m_width;
    bmi.bmiHeader.biHeight = -m_height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    m_memDC = CreateCompatibleDC(m_hdc);
    if (!m_memDC) return;

    m_hBitmap = CreateDIBSection(m_memDC, &bmi, DIB_RGB_COLORS, &m_pBits, NULL, 0);
    if (!m_hBitmap) {
        DeleteDC(m_memDC);
        m_memDC = NULL;
        return;
    }
    m_hOldBitmap = (HBITMAP)SelectObject(m_memDC, m_hBitmap);
}

void Renderer::DestroyGDISurface() {
    if (m_memDC && m_hOldBitmap) {
        SelectObject(m_memDC, m_hOldBitmap);
        m_hOldBitmap = NULL;
    }
    if (m_hBitmap) { DeleteObject(m_hBitmap); m_hBitmap = NULL; }
    if (m_memDC) { DeleteDC(m_memDC); m_memDC = NULL; }
    m_pBits = NULL;
}

void Renderer::BlitToWindow() {
    if (!m_hdc || !m_memDC || !m_hBitmap) return;
    BitBlt(m_hdc, 0, 0, m_width, m_height, m_memDC, 0, 0, SRCCOPY);
}

void Renderer::BeginScene() {
    if (!m_memDC || !m_pBits) return;
    // Clear to transparent black
    memset(m_pBits, 0, m_width * m_height * 4);
}

void Renderer::EndScene() {
    // Nothing needed - blit happens in Present
}

void Renderer::Present() {
    BlitToWindow();
}

void Renderer::Resize(int width, int height) {
    if (width <= 0 || height <= 0) return;
    m_width = width;
    m_height = height;
    DestroyGDISurface();
    CreateGDISurface();
}

static COLORREF ToColorRef(float r, float g, float b) {
    auto clamp = [](float v) { return (int)max(0, min(255, (int)(v * 255))); };
    return RGB(clamp(r), clamp(g), clamp(b));
}

void Renderer::DrawLine(float x1, float y1, float x2, float y2,
    float r, float g, float b, float a) {
    if (!m_memDC || !m_pBits) return;
    HPEN pen = CreatePen(PS_SOLID, 1, ToColorRef(r, g, b));
    HPEN oldPen = (HPEN)SelectObject(m_memDC, pen);
    MoveToEx(m_memDC, (int)x1, (int)y1, NULL);
    LineTo(m_memDC, (int)x2, (int)y2);
    SelectObject(m_memDC, oldPen);
    DeleteObject(pen);
}

void Renderer::DrawRect(float x, float y, float width, float height,
    float r, float g, float b, float a) {
    if (!m_memDC || !m_pBits || width <= 0 || height <= 0) return;
    HPEN pen = CreatePen(PS_SOLID, 1, ToColorRef(r, g, b));
    HPEN oldPen = (HPEN)SelectObject(m_memDC, pen);
    HBRUSH nullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    HBRUSH oldBrush = (HBRUSH)SelectObject(m_memDC, nullBrush);
    Rectangle(m_memDC, (int)x, (int)y, (int)(x + width), (int)(y + height));
    SelectObject(m_memDC, oldBrush);
    SelectObject(m_memDC, oldPen);
    DeleteObject(pen);
}

void Renderer::DrawFilledRect(float x, float y, float width, float height,
    float r, float g, float b, float a) {
    if (!m_memDC || !m_pBits || width <= 0 || height <= 0) return;
    HBRUSH brush = CreateSolidBrush(ToColorRef(r, g, b));
    RECT rect = { (int)x, (int)y, (int)(x + width), (int)(y + height) };
    FillRect(m_memDC, &rect, brush);
    DeleteObject(brush);
}

void Renderer::DrawText(float x, float y, const char* text,
    float r, float g, float b, float a) {
    if (!m_memDC || !m_pBits || !text) return;
    SetTextColor(m_memDC, ToColorRef(r, g, b));
    SetBkMode(m_memDC, TRANSPARENT);
    TextOutA(m_memDC, (int)x, (int)y, text, (int)strlen(text));
}

int Renderer::GetWidth() { return m_width; }
int Renderer::GetHeight() { return m_height; }