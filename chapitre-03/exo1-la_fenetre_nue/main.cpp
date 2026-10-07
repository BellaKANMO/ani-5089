#include <windows.h>

LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
    if (m == WM_DESTROY) PostQuitMessage(0);
    return DefWindowProc(h, m, w, l);
}

int WINAPI WinMain(HINSTANCE hi, HINSTANCE, LPSTR, int)
{
    WNDCLASS wc = {0, WndProc, 0, 0, hi, 0, 0, 0, 0, "Fenetre"};
    RegisterClass(&wc);
    HWND w = CreateWindow("Fenetre", "La fenetre nue", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                          CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, 0, 0, hi, 0);
    if (!w) return 1;
    for (MSG m = {}; m.message != WM_QUIT;)
        if (PeekMessage(&m, 0, 0, 0, PM_REMOVE)) DispatchMessage(&m);
    return 0;
}