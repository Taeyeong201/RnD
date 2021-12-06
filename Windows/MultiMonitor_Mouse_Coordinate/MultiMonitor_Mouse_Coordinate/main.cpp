// ref : https://stackoverflow.com/questions/62759122/calculate-normalized-coordinates-for-sendinput-in-a-multi-monitor-environment

#include <windows.h>
#include <iostream>
using namespace std;
// ________________________________________________
//
// GetAbsoluteCoordinate
//
// PURPOSE: 
// Convert pixel coordinate to absolute coordinate (0-65535).
//
// RETURN VALUE:
// Absolute Coordinate
// ________________________________________________
//
INT GetAbsoluteCoordinate(INT PixelCoordinate, INT ScreenResolution)
{
    INT AbsoluteCoordinate = MulDiv(PixelCoordinate, 65535, ScreenResolution-1);
    return AbsoluteCoordinate;
}

void GetAbsoluteCoordinates(HMONITOR hMonitor, INT32& X, INT32& Y)
{
    // Get multi-screen coordinates
    MONITORINFO MonitorInfo = { 0 };
    MonitorInfo.cbSize = sizeof(MonitorInfo);
    if (GetMonitorInfoW(hMonitor, &MonitorInfo))
    {
        // 1) Get pixel coordinates of topleft pixel of target screen, relative to the virtual desktop ( coordinates should be 0,0 on Main screen);
        // 2) Get pixel coordinates of mouse cursor, relative to the target screen;
        // 3) Sum topleft margin pixel coordinates with mouse cursor coordinates;
        X = MonitorInfo.rcMonitor.left + X;
        Y = MonitorInfo.rcMonitor.top + Y;

        // 4) Transform the resulting pixel coordinates into absolute coordinates.
        X = GetAbsoluteCoordinate(X, GetSystemMetrics(SM_CXVIRTUALSCREEN));
        Y = GetAbsoluteCoordinate(Y, GetSystemMetrics(SM_CYVIRTUALSCREEN));
    }
}

void SendMouseInput(HMONITOR hMonitor, INT X, INT Y)
{
    INPUT Input[2];
    GetAbsoluteCoordinates(hMonitor, X, Y);
    memset(Input, 0, sizeof(INPUT));
    Input[0].type = Input[1].type = INPUT_MOUSE;
    Input[0].mi.dx = Input[1].mi.dx = X;
    Input[0].mi.dy = Input[1].mi.dy = Y;
    Input[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK;
    Input[1].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP | MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK;

    SendInput(2, Input, sizeof(INPUT));
}
BOOL CALLBACK Monitorenumproc(
    HMONITOR Arg1,
    HDC Arg2,
    LPRECT Arg3,
    LPARAM Arg4)
{
    SendMouseInput(Arg1, 725, 85);
    return TRUE;
}
int main(void)
{
    EnumDisplayMonitors(NULL, NULL, Monitorenumproc, 0);
    
    return 0;
}