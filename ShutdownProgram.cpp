//#define _WIN32_IE 0x0800
#include <windows.h>
#include <string.h>
//#include <tchar.h>
#include <stdio.h>
#include <commctrl.h>

#include "resource.h"

#define HorizSpace 50
#define VertSpace 20

//  CreateDatePick creates a DTP control within a dialog box.
//  Returns the handle to the new DTP control if successful, or NULL 
//  otherwise.
//
//    hwndMain - The handle to the main window.
//    g_hinst  - global handle to the program instance.

static HWND g_hLink;

HINSTANCE g_hinst;
SYSTEMTIME lpSysTime;
SYSTEMTIME lpSysDate;
UINT_PTR IDT_TIMER1=10;
UINT_PTR IDT_TIMER2=20;
BYTE showDialogue = 1;
WORD count = 30;
BOOL secondtimerstarted = FALSE;

static HWND hwndTimeP;
static HWND hwndDateP;
static HWND hwndButton;
static HWND g_hwnd;


static TCHAR warnMsg[100] = L"";
/*static COLORREF myColor1 = 0x00ffffff; //0x00919191;
static COLORREF myColor2 = 0x00ffffff; //0x00919191;*/

static COLORREF myColor1 = 0x00FF8040;
static COLORREF myColor2 = 0x00FF8040;

LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
HWND WINAPI CreateDatePick(HWND);
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
HWND CreateSysLink(HWND, HINSTANCE, RECT);

HWND WINAPI CreateTimePick(HWND);
HWND WINAPI CreateDatePick(HWND);
int verify_inputDT();
BYTE react_click(HWND hwnd, HDC hdc);

int verify_inputTime(SYSTEMTIME currentLocalTime);
void runProgram(HWND hwnd);
BOOL isTimeUp ();
BOOL MySystemShutdown();

// The main window class name.
static TCHAR szWindowClass[] = L"myWindow";

// The string that appears in the application's title bar.
static TCHAR szTitle[] = L"Shutdown Timer";

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow) {
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = MainWndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROGICON));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCE(IDR_MENU1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = (HICON) LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_PROGICON));

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            L"Call to RegisterClassEx failed!",
            L"Test prog",
            NULL);

        return 1;
    }

    // The parameters to CreateWindow explained:
    // szWindowClass: the name of the application
    // szTitle: the text that appears in the title bar
    // WS_OVERLAPPEDWINDOW: the type of window to create
    // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
    // 500, 100: initial size (width, length)
    // NULL: the parent of this window
    // NULL: this application dows not have a menu bar
    // hInstance: the first parameter from WinMain
    // NULL: not used in this application
    HWND hWnd = CreateWindow(
        szWindowClass,
        szTitle,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT,
        330, 250,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        MessageBox(NULL,
            L"Call to CreateWindow failed!",
            L"Test prog",
            NULL);

        return 1;
    }

	g_hinst = hInstance;
	g_hwnd = hWnd;

    // The parameters to ShowWindow explained:
    // hWnd: the value returned from CreateWindow
    // nCmdShow: the fourth parameter from WinMain
	InitCommonControls();
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Main message loop:
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
		if (!IsDialogMessage(hWnd, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
    }

    return (int) msg.wParam;
}

LONG APIENTRY MainWndProc(HWND hwnd,                // window handle 
                          UINT message,             // type of message 
                          WPARAM wParam,            // additional information 
                          LPARAM lParam)            // additional information 
{ 
	PAINTSTRUCT ps;
    static HDC hdc;
	static HWND hwndDlg = NULL;
	char *inst = "Select shutdown time:";
	int ret = 0;

	static LPCSTR dayToday[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


 
 
    switch (message) { 
        case WM_CREATE:
			hwndTimeP = CreateTimePick(hwnd);
			hwndDateP = CreateDatePick(hwnd);
			hwndButton = CreateWindow( 
									L"BUTTON",   // Predefined class; Unicode assumed. 
									L"Start",       // Button text. 
									WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_CENTER,  // Styles. 
									HorizSpace + 60,         // x position. 
									VertSpace + 130,         // y position. 
									60,        // Button width.
									30,        // Button height.
									hwnd,       // Parent window.
									NULL,       // No menu.
									(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 
									NULL);      // Pointer not needed.
            return 0; 

		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps);
			HFONT hFont;
			RECT rect;

			hFont = CreateFont(14,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS, 
				CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, VARIABLE_PITCH,TEXT("Verdana"));
			// Select font
			SelectObject(hdc, hFont);

			if (showDialogue == 1) {
				//SetBkColor(hdc, 0x00606060);
				SetTextColor(hdc, myColor1);
				SetRect(&rect, HorizSpace,VertSpace,100,50);
				DrawText(hdc, TEXT("Choose Shutdown Time:"), -1,&rect, DT_NOCLIP);

				SetTextColor(hdc, myColor2);
				SetRect(&rect, HorizSpace,VertSpace+55,100,50);
				DrawText(hdc, TEXT("Choose Date:"), -1,&rect, DT_NOCLIP);

				SetTextColor(hdc, 0x000000ff);
				SetRect(&rect, HorizSpace,VertSpace+105,100,50);
				DrawText(hdc, warnMsg, -1,&rect, DT_NOCLIP);
			}
			else if (showDialogue == 2){
				SYSTEMTIME lt;
				GetLocalTime(&lt);
				char greeting[100];

				// Day of the week
				sprintf_s(greeting, "Today is %s %d-%d-%d", dayToday[lt.wDayOfWeek], lt.wDay, lt.wMonth, lt.wYear);
				TextOutA(hdc, 25, 20, greeting, strlen(greeting));

				sprintf_s(greeting, "Time now %d:%d:%d %s", (lt.wHour>12)?lt.wHour-12:lt.wHour, lt.wMinute, lt.wSecond, (lt.wHour>11)?"PM":"AM");
				TextOutA(hdc, 35, 40, greeting, strlen(greeting));

				sprintf_s(greeting, "Windows will shutdown in %d seconds.", count);
				TextOutA(hdc, 20, 70, greeting, strlen(greeting));

				sprintf_s(greeting, "Please close all your applications.");
				TextOutA(hdc, 25, 90, greeting, strlen(greeting));
			}

			// End application-specific layout section.
			EndPaint(hwnd, &ps);
			break;

		case WM_COMMAND:
			switch(LOWORD (wParam)) {
				case BN_DBLCLK: case BN_CLICKED:
					// Make a reaction
					DateTime_GetSystemtime(hwndTimeP, &lpSysTime);
					DateTime_GetSystemtime(hwndDateP, &lpSysDate);

					lpSysTime.wYear = lpSysDate.wYear;
					lpSysTime.wMonth = lpSysDate.wMonth;
					lpSysTime.wSecond = lpSysDate.wSecond;

					ret = react_click(hwnd, hdc);
					if (ret == 1)
						SetFocus(hwndDateP);
					else if (ret == 2)
						SetFocus(hwndTimeP);
					/* char str[100];
					SYSTEMTIME clp;
					GetLocalTime(&clp);

					sprintf_s(str, "Local time: %d:%d:%d and date %d/%d/%d.\nInput time: %d:%d:%d and date %d/%d/%d.", clp.wHour, clp.wMinute, clp.wSecond, clp.wYear, clp.wMonth, clp.wDay, lpSysTime.wHour, lpSysTime.wMinute, lpSysTime.wSecond, lpSysDate.wYear, lpSysDate.wMonth, lpSysDate.wDay);
					MessageBoxA(hwnd, str, "Notice", MB_OK);
					//ShowWindow(hwnd, SW_HIDE);*/
					break;
				case ID_ABOUT:
				    hwndDlg = CreateDialog (g_hinst,
                         MAKEINTRESOURCE(ABOUTDLGBOX),
                         hwnd,
                         DlgProc);

					if (!hwndDlg)
						MessageBox(hwnd, L"Could not create DlgBox.", L"Notice", MB_OK);
					break;
				case ID_EXIT:
					PostMessage(hwnd, WM_CLOSE, 0, 0);
				default:
					//MessageBox(hwnd, "Button not Clicked.", "Notice", MB_OK);
					break;
			}
			break;

	   case WM_DESTROY:
			PostQuitMessage(0);
			if (IDT_TIMER1)
				KillTimer(hwnd, IDT_TIMER1);
			break;

	   case WM_TIMER:
			if (secondtimerstarted==FALSE && isTimeUp()) {
				secondtimerstarted = TRUE;
				showDialogue = 2;
				ShowWindow(hwnd, SW_SHOWNORMAL);
				UpdateWindow(hwnd);
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE);
				SetTimer(hwnd, IDT_TIMER2, 1000, (TIMERPROC)NULL);
				HMENU hmenu = GetMenu(hwnd);		// hwnd is the handle of the window containing the menu
				EnableMenuItem(hmenu, ID_EXIT, MF_GRAYED);
				DrawMenuBar(hwnd);
			}
			else if (secondtimerstarted==TRUE) {
				if ((UINT)wParam == IDT_TIMER2)	
					InvalidateRect(hwnd, NULL, true);
					if (count-- == 0) {
						if (!MySystemShutdown())
							MessageBox(hwnd, L"Error in shutdown. Check if user account has previlege.", L"Shutdown error", MB_OK);
						PostMessage(hwnd, WM_CLOSE, 0, 0);
					}
			}

			break;

        default:
			//MessageBox(hwnd, "Default procedurec called.", "Notice", MB_OK);
            return DefWindowProc(hwnd, message, wParam, lParam); 
    } 
    return 0; 
}


HWND WINAPI CreateTimePick(HWND hwndMain)
{
	HWND hwndTimePick = NULL;
    INITCOMMONCONTROLSEX icex;

    icex.dwSize = sizeof(icex);
    icex.dwICC = ICC_DATE_CLASSES;

    InitCommonControlsEx(&icex);

    hwndTimePick = CreateWindowEx(0,
                         DATETIMEPICK_CLASS,
                         TEXT("DateTime"),
                         WS_BORDER|WS_CHILD|WS_VISIBLE|DTS_TIMEFORMAT|WS_TABSTOP,
                         HorizSpace,VertSpace+20,180,20,
                         hwndMain,
                         NULL,
                         g_hinst,
                         NULL);
    return (hwndTimePick);
}

HWND WINAPI CreateDatePick(HWND hwndMain)
{
	HWND hwndDatePick = NULL;
    INITCOMMONCONTROLSEX icex;

    icex.dwSize = sizeof(icex);
    icex.dwICC = ICC_DATE_CLASSES;

    InitCommonControlsEx(&icex);

    hwndDatePick = CreateWindowEx(0,
                         DATETIMEPICK_CLASS,
                         TEXT("DateTime"),
                         WS_BORDER|WS_CHILD|WS_VISIBLE|WS_TABSTOP,
                         HorizSpace,VertSpace+75,180,20,
                         hwndMain,
                         NULL,
                         g_hinst,
                         NULL);

    return (hwndDatePick);
}

BYTE react_click(HWND hwnd, HDC hdc) {
	// verify input
	/************************************************
	Return Codes
	 11 - Date is past
	 12 - Time is past
	2 = Input too near to current Time
	3 = input too far
	4 = ok
	************************************************/

	int ret = verify_inputDT();
	// invalid input
	if (ret == 11) {
		myColor1 = 0x00aaaaaa;
		myColor2 = 0x000000ff;
		// change_font color for time
		wsprintf(warnMsg, L"Entered date is in past!");
		InvalidateRgn(hwnd, NULL, FALSE);
		return 1;
	}
	else if (ret == 12) {
		myColor1 = 0x000000ff;
		myColor2 = 0x00aaaaaa;
		wsprintf(warnMsg, L"Entered time is in past!");
		InvalidateRgn(hwnd, NULL, FALSE);
		return 2;
	}

	// Shutdown too near to current time
	else if (ret == 2) {
		if (MessageBox(hwnd, L"Shutdown time too near. Click yes to continue, no to choose time again.", L"Time too near.", MB_YESNO | MB_ICONINFORMATION) == IDYES) {
			//MessageBox(hwnd, "Yes selected", "Notice", MB_OK);
			// start timer etc to shutdown
			// and hide window
			runProgram(hwnd);
		}
	}
	// too far
	else if (ret == 3){
		if (MessageBox(hwnd, L"Shutdown time too far. Click yes to continue, no to choose time again.", L"Time too far.", MB_YESNO | MB_ICONINFORMATION) == IDYES) {
			//MessageBox(hwnd, "Yes selected", "Notice", MB_OK);
			// and hide window
			// start timer etc to shutdown
			runProgram(hwnd);
		}
	}
	else if (ret == 4) {
		runProgram(hwnd);
	}
	return -1;
}

int verify_inputDT() {
	// get current system time
	SYSTEMTIME currentLocalTime;
	GetLocalTime(&currentLocalTime);

	// if date time all matches return 1
	if (lpSysDate.wYear < currentLocalTime.wYear)
			return 11;
	else if (lpSysDate.wYear == currentLocalTime.wYear) {
		if (lpSysDate.wMonth < currentLocalTime.wMonth)
			return 11;
		else if (lpSysDate.wMonth == currentLocalTime.wMonth) {
			if (lpSysDate.wDay < currentLocalTime.wDay)
				return 11;
			else if (lpSysDate.wDay == currentLocalTime.wDay)
				return verify_inputTime(currentLocalTime);
			else if (lpSysDate.wDay > currentLocalTime.wDay + 1)
				return 3;
			else if (lpSysDate.wDay > currentLocalTime.wDay)
				return 4;
		}
		else
			return 3;
	}
	else
		return 3;

	return -1;
}

int verify_inputTime(SYSTEMTIME currentLocalTime) {
	// 12: Time in past
	// 2: Time too near
	// 4: Ok
	if (lpSysTime.wHour < currentLocalTime.wHour)
		return 12;
	else if (lpSysTime.wHour == currentLocalTime.wHour) {
		if (lpSysTime.wMinute < currentLocalTime.wMinute)
			return 12;
		else if (lpSysTime.wMinute == currentLocalTime.wMinute) {
			if (lpSysTime.wSecond < currentLocalTime.wSecond)
				return 12;
			else
				return 2;
		}
		else if (currentLocalTime.wMinute - lpSysTime.wMinute < 3) {
			return 2;
		}
		else
			return 4;
	}
	else
		return 4;
}

void runProgram(HWND hwnd) {
	myColor1 = 0x00aaaaaa;
	myColor2 = 0x00aaaaaa;
	warnMsg[0] = '\0';

	DestroyWindow(hwndDateP);
	DestroyWindow(hwndTimeP);
	DestroyWindow(hwndButton);
	InvalidateRgn(hwnd, NULL, TRUE);
	ShowWindow(hwnd, SW_HIDE);

	SetTimer(hwnd, IDT_TIMER1, 60000, (TIMERPROC)NULL);
	showDialogue = 0;
}

BOOL isTimeUp () {
	SYSTEMTIME currentLocalTime;
	GetLocalTime(&currentLocalTime);

	if (lpSysTime.wYear == currentLocalTime.wYear && lpSysTime.wMonth == currentLocalTime.wMonth && lpSysTime.wDay == currentLocalTime.wDay && lpSysTime.wHour == currentLocalTime.wHour && lpSysTime.wMinute == currentLocalTime.wMinute)
		return TRUE;
	return FALSE;
}

BOOL CALLBACK DlgProc(HWND hwndDlg, 
                             UINT message, 
                             WPARAM wParam, 
                             LPARAM lParam) 
{
	PAINTSTRUCT Ps;
	HDC hDC, MemDC;
	HBITMAP hBmp;

	switch (message) {
		case WM_PAINT:
			hDC = BeginPaint(hwndDlg, &Ps);

			// Load the bitmap from the resource
			hBmp = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_BMP));
			// Create a memory device compatible with the above DC variable
			MemDC = CreateCompatibleDC(hDC);
				 // Select the new bitmap
				 SelectObject(MemDC, hBmp);

			// Copy the bits from the memory DC into the current dc
			BitBlt(hDC, 0, 0, 450, 400, MemDC, 0, 0, SRCCOPY);

			// Restore the old bitmap
			DeleteDC(MemDC);
			DeleteObject(hBmp);
			EndPaint(hwndDlg, &Ps);
			break;
		// g_hLink is the handle of the SysLink control.
		case WM_NOTIFY:
			switch (((LPNMHDR)lParam)->code)
			{
				case NM_CLICK:
				case NM_RETURN:
					{
						PNMLINK pNMLink = (PNMLINK)lParam;
						LITEM item = pNMLink->item;
						if ((((LPNMHDR)lParam)->hwndFrom == g_hLink) && (item.iLink == 0))
						{
							ShellExecute(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);
						}/*
						else if (wcscmp(item.szID, L"idInfo") == 0)
						{
							MessageBox(hwndDlg, L"This isn't much help.", L"Example", MB_OK);
						}*/
						break;
					}
			}
			break;
		case WM_INITDIALOG:
			EnableWindow(g_hwnd, FALSE);
			RECT rect;
			SetRect(&rect, 80,151,160,15);

			INITCOMMONCONTROLSEX icex;

			icex.dwSize = sizeof(icex);
			icex.dwICC = ICC_LINK_CLASS;

			InitCommonControlsEx(&icex);

			g_hLink = CreateSysLink(hwndDlg, g_hinst, rect);
			return TRUE;

        case WM_COMMAND: 
            switch (LOWORD(wParam)) {
                case IDOK:
					EndDialog(hwndDlg, IDOK);
					break;
            } 
			break;
		case WM_CLOSE:
			EndDialog(hwndDlg, TRUE);
			break;
		case WM_DESTROY:
			EnableWindow(g_hwnd, TRUE);
			break;

		default:
			return FALSE;
    }
    return FALSE; 
}

HWND CreateSysLink(HWND hDlg, HINSTANCE hInst, RECT rect) {
    return CreateWindowEx(0, WC_LINK, 
        L"<A HREF=\"http://atique.0fees.net/saff/std\">SA First Flush</A>", 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP, 
        rect.left, rect.top, rect.right, rect.bottom, 
        hDlg, NULL, hInst, NULL);
}

BOOL MySystemShutdown()
{
   HANDLE hToken; 
   TOKEN_PRIVILEGES tkp; 
 
   // Get a token for this process. 
 
   if (!OpenProcessToken(GetCurrentProcess(), 
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
      return( FALSE ); 
 
   // Get the LUID for the shutdown privilege. 
 
   LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
        &tkp.Privileges[0].Luid); 
 
   tkp.PrivilegeCount = 1;  // one privilege to set    
   tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
 
   // Get the shutdown privilege for this process. 
 
   AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
        (PTOKEN_PRIVILEGES)NULL, 0); 
 
   if (GetLastError() != ERROR_SUCCESS) 
      return FALSE; 
 
   // Shut down the system and force all applications to close. 
 
   if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 
               SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
               SHTDN_REASON_MINOR_UPGRADE |
               SHTDN_REASON_FLAG_PLANNED)) 
      return FALSE; 

   //shutdown was successful
   return TRUE;
}
