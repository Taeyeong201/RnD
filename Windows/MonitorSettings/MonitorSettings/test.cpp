#include <Windows.h>

// ref : https://docs.microsoft.com/en-us/windows/win32/gdi/enumeration-and-display-control
void ChangeDisplayResolution()
{
	BOOL            FoundSecondaryDisp = FALSE;
	DWORD           DispNum = 0;
	DISPLAY_DEVICE  DisplayDevice;
	LONG            Result;
	TCHAR           szTemp[200];
	int             i = 0;
	DEVMODE   defaultMode;

	// initialize DisplayDevice
	ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
	DisplayDevice.cb = sizeof(DisplayDevice);

	// get all display devices
	while (EnumDisplayDevices(NULL, DispNum, &DisplayDevice, 0))
	{
		ZeroMemory(&defaultMode, sizeof(DEVMODE));
		defaultMode.dmSize = sizeof(DEVMODE);
		if (!EnumDisplaySettings(DisplayDevice.DeviceName, ENUM_REGISTRY_SETTINGS, &defaultMode))
			OutputDebugString(L"Store default failed\n");

		if ((DisplayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) &&
			!(DisplayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE))
		{
			DEVMODE    DevMode;
			ZeroMemory(&DevMode, sizeof(DevMode));
			DevMode.dmSize = sizeof(DevMode);
			DevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
			Result = ChangeDisplaySettingsEx(DisplayDevice.DeviceName,
				&DevMode,
				NULL,
				CDS_UPDATEREGISTRY | CDS_GLOBAL | CDS_RESET,
				NULL);

			Sleep(5000);
			//The code below shows how to re-attach the secondary displays to the desktop
			ChangeDisplaySettingsEx(DisplayDevice.DeviceName,
				&defaultMode,
				NULL,
				CDS_UPDATEREGISTRY | CDS_GLOBAL | CDS_RESET,
				NULL);
		}

		// Reinit DisplayDevice just to be extra clean

		ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
		DisplayDevice.cb = sizeof(DisplayDevice);
		DispNum++;
	} // end while for all display devices
}

void ChangePrimaryDisplayResolution() {
	LONG            Result;
	DEVMODE    DevMode;

	ZeroMemory(&DevMode, sizeof(DevMode));
	DevMode.dmSize = sizeof(DevMode);
	DevMode.dmPelsWidth = 1280;
	DevMode.dmPelsHeight = 720;
	DevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
	Result = ChangeDisplaySettings(
		&DevMode,
		CDS_UPDATEREGISTRY | CDS_GLOBAL | CDS_RESET);

	//The code below shows how to re-attach the secondary displays to the desktop
	Sleep(5000);
	ZeroMemory(&DevMode, sizeof(DevMode));
	DevMode.dmSize = sizeof(DevMode);
	DevMode.dmPelsWidth = 1920;
	DevMode.dmPelsHeight = 1080;
	DevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

	ChangeDisplaySettings(&DevMode,
		CDS_UPDATEREGISTRY | CDS_GLOBAL | CDS_RESET);
}

void ChangePrimary() {
	DWORD           DispNum = 0;
	DISPLAY_DEVICE  DisplayDevice;
	DISPLAY_DEVICE  PrimaryDisplayDevice;
	DISPLAY_DEVICE	SecondaryDisplayDevice;
	LONG            Result;
	TCHAR           szTemp[200];
	int             i = 0;
	DEVMODE   defaultMode;
	DEVMODE   temp = { 0, };

	// initialize DisplayDevice
	ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
	DisplayDevice.cb = sizeof(DisplayDevice);

	// get all display devices
	while (EnumDisplayDevices(NULL, DispNum, &DisplayDevice, 0))
	{
		ZeroMemory(&defaultMode, sizeof(DEVMODE));
		defaultMode.dmSize = sizeof(DEVMODE);
		if (!EnumDisplaySettings(DisplayDevice.DeviceName, ENUM_CURRENT_SETTINGS, &defaultMode))
			OutputDebugString(L"Store default failed\n");

		if ((DisplayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) &&
			!(DisplayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE))
		{
			SecondaryDisplayDevice = DisplayDevice;
			temp = defaultMode;
		}
		else {
			PrimaryDisplayDevice = DisplayDevice;
		}

		// Reinit DisplayDevice just to be extra clean
		ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
		DisplayDevice.cb = sizeof(DisplayDevice);
		DispNum++;
	} // end while for all display devices

	DEVMODE    DevMode;
	ZeroMemory(&DevMode, sizeof(DevMode));
	DevMode.dmSize = sizeof(DevMode);
	DevMode.dmPosition.x = 0;
	DevMode.dmPosition.y = 0;
	DevMode.dmFields = DM_POSITION;

	Result = ChangeDisplaySettingsEx(SecondaryDisplayDevice.DeviceName,
		&DevMode,
		NULL,
		CDS_UPDATEREGISTRY | CDS_NORESET | CDS_SET_PRIMARY | CDS_GLOBAL,
		NULL);

	ZeroMemory(&DevMode, sizeof(DevMode));
	DevMode.dmSize = sizeof(DevMode);
	DevMode.dmPosition.x = -temp.dmPosition.x;
	DevMode.dmPosition.y = -temp.dmPosition.y;
	DevMode.dmFields = DM_POSITION;

	DISP_CHANGE_SUCCESSFUL;
	Result = ChangeDisplaySettingsEx(PrimaryDisplayDevice.DeviceName,
		&DevMode,
		NULL,
		CDS_UPDATEREGISTRY | CDS_NORESET | CDS_GLOBAL,
		NULL);

	// ref : https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-changedisplaysettingsexa
	ChangeDisplaySettingsEx(NULL, NULL, NULL, 0, NULL);
}

void ChangeDisplayPositionX() {
	DWORD           DispNum = 0;
	DISPLAY_DEVICE  DisplayDevice;
	DISPLAY_DEVICE  SecondaryDisplayDevice;
	LONG            Result;
	TCHAR           szTemp[200];
	int             i = 0;
	DEVMODE   defaultMode;
	DEVMODE   primary_current = { 0, };
	DEVMODE   secondary_current = { 0, };

	// initialize DisplayDevice
	ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
	DisplayDevice.cb = sizeof(DisplayDevice);

	// get all display devices
	while (EnumDisplayDevices(NULL, DispNum, &DisplayDevice, 0))
	{
		ZeroMemory(&defaultMode, sizeof(DEVMODE));
		defaultMode.dmSize = sizeof(DEVMODE);
		if (!EnumDisplaySettings(DisplayDevice.DeviceName, ENUM_CURRENT_SETTINGS, &defaultMode))
			OutputDebugString(L"Store default failed\n");

		if ((DisplayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) &&
			!(DisplayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE))
		{
			secondary_current = defaultMode;
			SecondaryDisplayDevice = DisplayDevice;
		}
		else {
			primary_current = defaultMode;
		}
		// Reinit DisplayDevice just to be extra clean

		ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
		DisplayDevice.cb = sizeof(DisplayDevice);
		DispNum++;
	} // end while for all display devices

	DEVMODE    DevMode;
	ZeroMemory(&DevMode, sizeof(DevMode));
	DevMode.dmSize = sizeof(DevMode);
	if(primary_current.dmPelsWidth == secondary_current.dmPosition.x)
		DevMode.dmPosition.x = -(LONG)secondary_current.dmPelsWidth;
	else
		DevMode.dmPosition.x = primary_current.dmPelsWidth;

	DevMode.dmPosition.y = secondary_current.dmPosition.y;
	DevMode.dmFields = DM_POSITION;

	Result = ChangeDisplaySettingsEx(SecondaryDisplayDevice.DeviceName,
		&DevMode,
		NULL,
		CDS_UPDATEREGISTRY | CDS_RESET | CDS_GLOBAL,
		NULL);
}

int main() {
	//ChangePrimaryDisplayResolution();
	//ChangeDisplayResolution();
	//ChangePrimary();
	ChangeDisplayPositionX();
}