#pragma once
#include "pch.h"

class Renderer {
private:
    HWND m_hWnd;
    int m_width;
    int m_height;
    HDC m_hdc;
    HDC m_memDC;
    HBITMAP m_hBitmap;
    HBITMAP m_hOldBitmap;
    void* m_pBits;

public:
    Renderer();
    ~Renderer() { Shutdown(); }

    bool Initialize(HWND hWnd);
    void Shutdown();
    void BeginScene();
    void EndScene();
    void Present();
    void Resize(int width, int height);

    void DrawLine(float x1, float y1, float x2, float y2, float r, float g, float b, float a);
    void DrawRect(float x, float y, float width, float height, float r, float g, float b, float a);
    void DrawFilledRect(float x, float y, float width, float height, float r, float g, float b, float a);
    void DrawText(float x, float y, const char* text, float r, float g, float b, float a);

    int GetWidth();
    int GetHeight();

private:
    void CreateGDISurface();
    void DestroyGDISurface();
    void BlitToWindow();
};