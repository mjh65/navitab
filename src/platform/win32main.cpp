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

// This file provides the WinMain function required for the WIN32 desktop
// build of Navitab.

#include <windows.h>

static LRESULT WindowProc(HWND h, UINT msg, WPARAM wp, LPARAM lp);

int CALLBACK WinMain(_In_ HINSTANCE hinst, _In_opt_ HINSTANCE hprev, _In_ LPSTR cmdline, _In_ int show)
{
    WNDCLASS c = { 0 };
    c.lpfnWndProc = WindowProc;
    c.hInstance = hinst;
    c.hIcon = LoadIcon(0, IDI_APPLICATION);
    c.hCursor = LoadCursor(0, IDC_ARROW);
    c.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    c.lpszClassName = "MainWindow";
    RegisterClass(&c);

    HWND h = CreateWindow("MainWindow", "Navitab", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, // position and size
        0, 0, hinst, 0);
    ShowWindow(h, show);

    while (1)
    {
        BOOL bRet;
        MSG msg;
        //while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) // non-blocking version, probably not required for desktop app
        while (bRet = GetMessage(&msg, 0, 0, 0))
        {
            if (bRet < 0)
            {
                // some error, we should give up now
                return 0;
            }
            if (msg.message == WM_QUIT)
            {
                return (int)msg.wParam;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return 0;
}

static LRESULT WindowProc(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_CREATE: {
        HWND hbutton = CreateWindow("BUTTON", "Hey There",  /* class and title */
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

    default:
        return DefWindowProc(h, msg, wp, lp);
    }

    return 0;
}
