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

// Forward declarations of local functions
LRESULT CALLBACK WindowProc(HWND h, UINT msg, WPARAM wp, LPARAM lp);

static LPCWSTR WINDOW_CLASS = TEXT("NavitabWindow");

int WINAPI WinMain(_In_ HINSTANCE hinst, _In_opt_ HINSTANCE hprev, _In_ LPSTR cmdline, _In_ int show)
{
    WNDCLASS c = { 0 };
    c.lpfnWndProc = WindowProc;
    c.hInstance = hinst;
    c.hIcon = LoadIcon(0, IDI_APPLICATION);
    c.hCursor = LoadCursor(0, IDC_ARROW);
    c.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    c.lpszClassName = WINDOW_CLASS;
    RegisterClass(&c);

    auto nvt = std::make_unique<navitab::core::SubSystems>(navitab::core::Simulation::NONE, navitab::core::AppClass::DESKTOP);

    try {
        // try to initialise logging and preferences - raises exception if fails
        nvt->early_init();
    }
    catch (...) {
        // TODO - if an exception occurs then we should show any information we got in a
        // dialog window
    }

    HWND h = CreateWindowEx(0, WINDOW_CLASS, TEXT("Navitab"), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, // position and size
        0, 0, hinst, 0);
    if (h == NULL) return 0;

    ShowWindow(h, show);

    while (1)
    {
        MSG msg = { };
        while (GetMessage(&msg, NULL, 0, 0) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return 0;
}




LRESULT CALLBACK WindowProc(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
#if 0
    case WM_CREATE:

        {
        HWND hbutton = CreateWindowEx("BUTTON", "Hey There",  /* class and title */
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, /* style */
            0, 0, 100, 30,            /* position */
            h,                     /* parent */
            (HMENU)101,            /* unique (within the application) integer identifier */
            GetModuleHandle(0), 0   /* GetModuleHandle(0) gets the hinst */
        );
    }
                  break;

    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case 101: /* the unique identifier used above. These are usually #define's */
            PostQuitMessage(1);
            break;
        default:;
        }
        break;

    case WM_CLOSE:
        PostQuitMessage(1);
        break;
#endif
    case WM_DESTROY:
        PostQuitMessage(1);
        return 0;

    default:
        return DefWindowProc(h, msg, wp, lp);
    }

    return 0;
}
