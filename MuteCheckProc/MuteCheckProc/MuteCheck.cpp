#include "Epvolume.h"

HWND g_hDlg = NULL;
GUID g_guidMyContext = GUID_NULL;

static IAudioEndpointVolume *g_pEndptVol = NULL;
static BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

#define EXIT_ON_ERROR(hr)  \
              if (FAILED(hr)) { goto Exit; }
#define ERROR_CANCEL(hr)  \
              if (FAILED(hr)) {  \
                  MessageBox(hDlg, TEXT("The program will exit."),  \
                             TEXT("Fatal error"), MB_OK);  \
                  EndDialog(hDlg, TRUE); return TRUE; }

//-----------------------------------------------------------
// WinMain -- Registers an IAudioEndpointVolumeCallback
//   interface to monitor endpoint volume level, and opens
//   a dialog box that displays a volume control that will
//   mirror the endpoint volume control in SndVol.
//-----------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	HRESULT hr = S_OK;
	IMMDeviceEnumerator *pEnumerator = NULL;
	IMMDevice *pDevice = NULL;
	CAudioEndpointVolumeCallback EPVolEvents;

	if (hPrevInstance)
	{
		return 0;
	}

	CoInitialize(NULL);

	hr = CoCreateGuid(&g_guidMyContext);
	EXIT_ON_ERROR(hr)

		// Get enumerator for audio endpoint devices.
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
			NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IMMDeviceEnumerator),
			(void**)&pEnumerator);
	EXIT_ON_ERROR(hr)

		// Get default audio-rendering device.
		hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	EXIT_ON_ERROR(hr)

		hr = pDevice->Activate(__uuidof(IAudioEndpointVolume),
			CLSCTX_ALL, NULL, (void**)&g_pEndptVol);
	EXIT_ON_ERROR(hr)

		hr = g_pEndptVol->RegisterControlChangeNotify(
		(IAudioEndpointVolumeCallback*)&EPVolEvents);
	EXIT_ON_ERROR(hr)

		InitCommonControls();
	DialogBox(hInstance, L"VOLUMECONTROL", NULL, (DLGPROC)DlgProc);

Exit:
	if (FAILED(hr))
	{
		MessageBox(NULL, TEXT("This program requires Windows Vista."),
			TEXT("Error termination"), MB_OK);
	}
	if (pEnumerator != NULL)
	{
		g_pEndptVol->UnregisterControlChangeNotify(
			(IAudioEndpointVolumeCallback*)&EPVolEvents);
	}
	SAFE_RELEASE(pEnumerator)
		SAFE_RELEASE(pDevice)
		SAFE_RELEASE(g_pEndptVol)
		CoUninitialize();
	return 0;
}

//-----------------------------------------------------------
// DlgProc -- Dialog box procedure
//-----------------------------------------------------------

BOOL CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HRESULT hr;
	BOOL bMute;
	float fVolume;
	int nVolume;
	int nChecked;

	switch (message)
	{
	case WM_INITDIALOG:
		g_hDlg = hDlg;
		SendDlgItemMessage(hDlg, IDC_SLIDER_VOLUME, TBM_SETRANGEMIN, FALSE, 0);
		SendDlgItemMessage(hDlg, IDC_SLIDER_VOLUME, TBM_SETRANGEMAX, FALSE, MAX_VOL);
		hr = g_pEndptVol->GetMute(&bMute);
		ERROR_CANCEL(hr)
			SendDlgItemMessage(hDlg, IDC_CHECK_MUTE, BM_SETCHECK,
				bMute ? BST_CHECKED : BST_UNCHECKED, 0);
		hr = g_pEndptVol->GetMasterVolumeLevelScalar(&fVolume);
		ERROR_CANCEL(hr)
			nVolume = (int)(MAX_VOL*fVolume + 0.5);
		SendDlgItemMessage(hDlg, IDC_SLIDER_VOLUME, TBM_SETPOS, TRUE, nVolume);
		return TRUE;

	case WM_HSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
		case SB_LINERIGHT:
		case SB_LINELEFT:
		case SB_PAGERIGHT:
		case SB_PAGELEFT:
		case SB_RIGHT:
		case SB_LEFT:
			// The user moved the volume slider in the dialog box.
			SendDlgItemMessage(hDlg, IDC_CHECK_MUTE, BM_SETCHECK, BST_UNCHECKED, 0);
			hr = g_pEndptVol->SetMute(FALSE, &g_guidMyContext);
			ERROR_CANCEL(hr)
				nVolume = SendDlgItemMessage(hDlg, IDC_SLIDER_VOLUME, TBM_GETPOS, 0, 0);
			fVolume = (float)nVolume / MAX_VOL;
			hr = g_pEndptVol->SetMasterVolumeLevelScalar(fVolume, &g_guidMyContext);
			ERROR_CANCEL(hr)
				return TRUE;
		}
		break;

	case WM_COMMAND:
		switch ((int)LOWORD(wParam))
		{
		case IDC_CHECK_MUTE:
			// The user selected the Mute check box in the dialog box.
			nChecked = SendDlgItemMessage(hDlg, IDC_CHECK_MUTE, BM_GETCHECK, 0, 0);
			bMute = (BST_CHECKED == nChecked);
			hr = g_pEndptVol->SetMute(bMute, &g_guidMyContext);
			ERROR_CANCEL(hr)
				return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}