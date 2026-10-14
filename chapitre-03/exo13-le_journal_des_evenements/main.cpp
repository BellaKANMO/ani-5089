#include <windows.h>
#include <fstream>
#include <chrono>

std::ofstream journal;
std::chrono::steady_clock::time_point debut;

void ecrire(const char* type)
{
    double temps =
        std::chrono::duration<double>(
            std::chrono::steady_clock::now() - debut
        ).count();

    journal << temps << " " << type << "\n";
}

LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
    switch (m)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_KEYDOWN:
        case WM_KEYUP:
            ecrire("Clavier");
            return 0;

        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            ecrire("Souris");
            return 0;

        case WM_MOUSEWHEEL:
            ecrire("Roulette");
            return 0;

        case WM_SIZE:
            ecrire("Redimensionnement");
            return 0;

        case WM_MOUSELEAVE:
            ecrire("SortiePointeur");
            return 0;

    }

    return DefWindowProc(h, m, w, l);
}

int WINAPI WinMain(HINSTANCE hi, HINSTANCE, LPSTR, int)
{
    journal.open("journal.txt");

    debut = std::chrono::steady_clock::now();

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hi;
    wc.lpszClassName = "Fenetre";

    RegisterClass(&wc);

    HWND fenetre = CreateWindow(
        "Fenetre",
        "Journal des evenements",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        800,
        600,
        nullptr,
        nullptr,
        hi,
        nullptr
    );

    TRACKMOUSEEVENT tme = {};
    tme.cbSize = sizeof(TRACKMOUSEEVENT);
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = fenetre;
    TrackMouseEvent(&tme);

    MSG msg = {};

    while (msg.message != WM_QUIT)
    {
        double temps =
            std::chrono::duration<double>(
                std::chrono::steady_clock::now() - debut
            ).count();

        if (temps >= 60.0)
        {
            DestroyWindow(fenetre);
        }

        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    journal.close();

    return 0;
}