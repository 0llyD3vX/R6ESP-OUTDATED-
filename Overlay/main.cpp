#include "pch.h"
#include "overlay.h"
#include "renderer.h"
#include "esp.h"
#include "client.h"
#include "structures.h"

Renderer g_Renderer;
ESP g_ESP;
GameClient g_Client;
GameData g_GameData = { 0 };
bool g_Running = true;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    printf("[R6ESP] Window message: 0x%X\n", message);
    switch (message) {
    case WM_DESTROY:
        g_Running = false;
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        g_Renderer.Resize(LOWORD(lParam), HIWORD(lParam));
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

HWND CreateOverlayWindow() {
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
    wc.lpszClassName = L"RainbowSixESP_Overlay";

    if (!RegisterClassEx(&wc)) return NULL;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    HWND hWnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
        L"RainbowSixESP_Overlay", L"Rainbow Six ESP",
        WS_POPUP, 0, 0, screenWidth, screenHeight,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );

    if (!hWnd) return NULL;

    SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);
    return hWnd;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    printf("[R6ESP] Starting...\n");

    if (!g_Client.Connect()) {
        printf("[R6ESP] Failed to connect to driver!\n");
        MessageBox(NULL, L"Failed to connect to driver.", L"Error", MB_OK);
        return 1;
    }
    printf("[R6ESP] Connected to driver\n");

    HWND hWnd = CreateOverlayWindow();
    if (!hWnd) { printf("[R6ESP] Failed to create overlay!\n"); return 1; }
    printf("[R6ESP] Overlay created\n");

    if (!g_Renderer.Initialize(hWnd)) {
        printf("[R6ESP] Failed to initialize renderer!\n");
        return 1;
    }
    printf("[R6ESP] Renderer initialized\n");

    g_ESP.Initialize(&g_Renderer);

    MSG msg = { 0 };
    while (g_Running) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (GetAsyncKeyState(VK_INSERT) & 1) {
            g_ESP.ToggleMenu();
            printf("[R6ESP] INSERT pressed\n");
        }

        if (GetAsyncKeyState(VK_END) & 1) {
            g_Running = false;
            printf("[R6ESP] END pressed, exiting\n");
            break;
        }

        if (g_Client.GetGameData(&g_GameData)) {
            g_Renderer.BeginScene();
            g_ESP.Draw(&g_GameData);
            g_Renderer.EndScene();
            g_Renderer.Present();
        }

        Sleep(30);  // ← 30ms = ~33 FPS (reduces flicker)
    }

    g_Client.Disconnect();
    g_Renderer.Shutdown();
    DestroyWindow(hWnd);
    FreeConsole();
    return 0;
}