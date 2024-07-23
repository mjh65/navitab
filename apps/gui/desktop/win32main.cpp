/*
 *  Navitab - Navigation Tablet for VR flight simulation
 *  Copyright (c) 2024 Michael Hasling
 *  Significantly derived from Avitab
 *  Copyright (c) 2018-2024 Folke Will
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

// This file provides the wWinMain function required for the WIN32 desktop
// build of Navitab. It does the required generic initialisation of the
// Navitab components, and if all is successful starts the standard Win32
// event loop and message processing. If there is a problem during startup
// before 

#include <Windows.h>
#include <memory>
#include "navitab/core.h"

// Forward declarations of local functions and shared data
LRESULT CALLBACK WindowProc(HWND h, UINT msg, WPARAM wp, LPARAM lp);

int WINAPI WinMain(_In_ HINSTANCE hinst, _In_opt_ HINSTANCE hprev, _In_ LPSTR cmdline, _In_ int show)
{
    const static LPCWSTR WINDOW_CLASS = TEXT("NavitabWindow");
    WNDCLASS c = { 0 };
    c.lpfnWndProc = WindowProc;
    c.hInstance = hinst;
    c.hIcon = LoadIcon(0, IDI_APPLICATION);
    c.hCursor = LoadCursor(0, IDC_ARROW);
    c.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    c.lpszClassName = WINDOW_CLASS;
    RegisterClass(&c);

    std::unique_ptr<navitab::System> nvt;
    try {
        // try to initialise logging and preferences - raises exception if fails
        nvt = navitab::System::GetSystem(navitab::SimEngine::MOCK, navitab::AppClass::DESKTOP);
    }
    catch (navitab::StartupError& e) {
        // TODO - if an exception occurs then we should show any information we got in a
        // dialog window
    }
    catch (...) {
        // TODO - handle other exceptions
    }

    HWND h = CreateWindowEx(0, WINDOW_CLASS, TEXT("Navitab"), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, // position and size
        0, 0, hinst, 0);
    if (h == NULL) return 0;

    ShowWindow(h, show);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    default:
        return DefWindowProc(h, msg, wp, lp);

    case WM_DESTROY:
        PostQuitMessage(1);
        return 0;

#if 0
    // these are messages we've seen during debugging, we might want to handle some of them?
    case WM_CREATE:             // 1
    case WM_MOVE:               // 3
    case WM_SIZE:               // 5
    case WM_ACTIVATE:           // 6
    case WM_SETFOCUS:           // 7
    case WM_KILLFOCUS:          // 8
    case WM_PAINT:              // 15
    case WM_CLOSE:              // 16
    case WM_ERASEBKGND:         // 20
    case WM_SHOWWINDOW:         // 24
    case WM_ACTIVATEAPP:        // 28
    case WM_SETCURSOR:          // 32
    case WM_GETMINMAXINFO:      // 36
    case WM_WINDOWPOSCHANGING:  // 70
    case WM_WINDOWPOSCHANGED:   // 71
    case WM_GETICON:            // 127
    case WM_NCCREATE:           // 129
    case WM_NCDESTROY:          // 130
    case WM_NCCALCSIZE:         // 131
    case WM_NCHITTEST:          // 132
    case WM_NCPAINT:            // 133
    case WM_NCACTIVATE:         // 134
    case 0x90:                  // 144 - ????
    case WM_NCMOUSEMOVE:        // 160
    case WM_NCLBUTTONDOWN:      // 161
    case WM_SYSCOMMAND:         // 274
    case WM_MOUSEMOVE:          // 512
    case WM_CAPTURECHANGED:     // 533
    case WM_IME_SETCONTEXT:     // 641
    case WM_IME_NOTIFY:         // 642
    case WM_IME_REQUEST:        // 648
    case WM_NCMOUSELEAVE:       // 674
    case WM_DWMNCRENDERINGCHANGED:  // 799=0x31f
        return DefWindowProc(h, msg, wp, lp);
#endif
    }

    return 0;
}
