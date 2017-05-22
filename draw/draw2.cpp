// draw.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "draw2.h"
#include "Data.h"
#include <vector>
#include <cstdio>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define MAX_LOADSTRING 100
#define TMR_1 1

const int MAX_ZOOM_Y = 100;
const int MAX_ZOOM_X = 50;

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

CData *loggedData;

INT samplesToDiscard;

// buttons
HWND hwndButton;
HWND hZoomY, hZoomX;
HWND hScrollBar;
HWND hText;
// sent data

RECT drawArea1 = { 350, 20, 750, 400 };


// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		DlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Buttons(HWND, UINT, WPARAM, LPARAM);

void DrawDoubleBuffer(HWND hWnd);

bool CALLBACK SetFont(HWND child, LPARAM font) {
	SendMessage(child, WM_SETFONT, font, true);
	return true;
}


void repaintWindow(HWND hWnd, HDC &hdc, PAINTSTRUCT &ps, RECT *drawArea)
{

	if (drawArea == NULL)
		InvalidateRect(hWnd, NULL, FALSE); // repaint all
	else
		InvalidateRect(hWnd, drawArea, FALSE); //repaint drawArea

	DrawDoubleBuffer(hWnd);

}


// main function (exe hInstance)
int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);


	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DRAW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);



	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DRAW));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	GdiplusShutdown(gdiplusToken);

	return (int)msg.wParam;
}

void DiscardHandler(HWND hWnd)
{
	DWORD textLength = GetWindowTextLength(hText);
	LPWSTR discardBuffer = (LPWSTR)GlobalAlloc(GPTR, textLength + 1);
	GetWindowText(hText, discardBuffer, textLength + 1);
	samplesToDiscard = _wtoi(discardBuffer);
	if (samplesToDiscard > loggedData->dataSize) return;

	int ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DELETE), hWnd, DlgProc);
	if (ret == ID_YES)
	{
		loggedData->DiscardSamples(samplesToDiscard);
		
	}
		
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DRAW));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_DRAW);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//

void UpdateScrollBar()
{
	SCROLLINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE | SIF_PAGE;
	si.nMin = 0;
	si.nMax = loggedData->dataSize * loggedData->zoomX / 2;
	si.nPage = (loggedData->dataSize + loggedData->zoomX) / loggedData->zoomX;

	SetScrollInfo(hScrollBar, SB_CTL, &si, TRUE);

}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	hInst = hInstance; // Store instance handle (of exe) in our global variable

	// main window
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, 0, 800, 600, NULL, NULL, hInstance, NULL);

	loggedData = new CData(hWnd);

	// create button and store the handle    

	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("DISCARD SAMPLES"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		100, 190,                                  // the left and top co-ordinates
		110, 25,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)DISCARD_BTN,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);                               // extra bits you dont really need

	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("Zoom IN"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		280, 20,                                  // the left and top co-ordinates
		60, 25,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_ZOOM_IN,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);                               // extra bits you dont really need

	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("Zoom OUT"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		280, 375,                                  // the left and top co-ordinates
		60, 25,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_ZOOM_OUT,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);
	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("Zoom X IN"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		345, 420,                                  // the left and top co-ordinates
		60, 25,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_ZOOM_X_IN,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);
	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("Zoom X OUT"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		682, 420,                                  // the left and top co-ordinates
		70, 25,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_ZOOM_X_OUT,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);
	
	CreateWindowW(L"button", L"Gyro X",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		100, 40, 50, 20, hWnd, (HMENU)ID_CHECK_GYRO_X,
		NULL, NULL);
	CreateWindowW(L"button", L"Gyro Y",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		100, 60, 50, 20, hWnd, (HMENU)ID_CHECK_GYRO_Y,
		NULL, NULL);
	CreateWindowW(L"button", L"Gyro Z",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		100, 80, 50, 20, hWnd, (HMENU)ID_CHECK_GYRO_Z,
		NULL, NULL);
	CreateWindowW(L"button", L"Pos X",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		160, 40, 50, 20, hWnd, (HMENU)ID_CHECK_POS_X,
		NULL, NULL);
	CreateWindowW(L"button", L"Pos Y",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		160, 60, 50, 20, hWnd, (HMENU)ID_CHECK_POS_Y,
		NULL, NULL);
	CreateWindowW(L"button", L"Pos Z",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		160, 80, 50, 20, hWnd, (HMENU)ID_CHECK_POS_Z,
		NULL, NULL);
	CreateWindowW(L"button", L"Plot average",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		100, 260, 80, 20, hWnd, (HMENU)ID_CHECK_AVG,
		NULL, NULL);

	EnableWindow(GetDlgItem(hWnd, ID_CHECK_GYRO_X), FALSE);
	EnableWindow(GetDlgItem(hWnd, ID_CHECK_GYRO_Y), FALSE);
	EnableWindow(GetDlgItem(hWnd, ID_CHECK_GYRO_Z), FALSE);
	EnableWindow(GetDlgItem(hWnd, ID_CHECK_POS_X), FALSE);
	EnableWindow(GetDlgItem(hWnd, ID_CHECK_POS_Y), FALSE);
	EnableWindow(GetDlgItem(hWnd, ID_CHECK_POS_Z), FALSE);

	hZoomY = CreateWindowW(TRACKBAR_CLASSW, L"ZOOM Control",
		WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_VERT | TBS_DOWNISLEFT,
		300, 60, 30, 300, hWnd, (HMENU)3, NULL, NULL);

	SendMessageW(hZoomY, TBM_SETRANGE, TRUE, MAKELONG(0, MAX_ZOOM_Y-1));
	SendMessageW(hZoomY, TBM_SETPAGESIZE, 0, 1);
	SendMessageW(hZoomY, TBM_SETTICFREQ, 5, 0);
	SendMessageW(hZoomY, TBM_SETPOS, TRUE, MAX_ZOOM_Y - 1);
	
	hZoomX = CreateWindowW(TRACKBAR_CLASSW, L"ZOOM X Control",
		WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_HORZ,
		410, 420, 270, 30, hWnd, (HMENU)3, NULL, NULL);

	SendMessageW(hZoomX, TBM_SETRANGE, TRUE, MAKELONG(1, MAX_ZOOM_X));
	SendMessageW(hZoomX, TBM_SETPAGESIZE, 0, 1);
	SendMessageW(hZoomX, TBM_SETTICFREQ, 5, 0);
	SendMessageW(hZoomX, TBM_SETPOS, TRUE, 1);

	hScrollBar = CreateWindow(TEXT("SCROLLBAR"),
		NULL, WS_CHILD | WS_VISIBLE,
		410, 470, 270, 21, hWnd, NULL, hInstance, NULL);

	UpdateScrollBar();

	hwndButton = CreateWindow(TEXT("BUTTON"), TEXT("Axis to plot"),
		WS_CHILD | WS_VISIBLE | BS_GROUPBOX | WS_GROUP,
		80, 23, 150, 90,
		hWnd, NULL, hInstance, 0);

	hwndButton = CreateWindow(TEXT("BUTTON"), TEXT("Samples to discard"),
		WS_CHILD | WS_VISIBLE | BS_GROUPBOX | WS_GROUP,
		80, 140, 150, 100,
		hWnd, NULL, hInstance, 0);

	 hText = CreateWindow(TEXT("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
		130, 160, 50, 20, hWnd, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	EnumChildWindows(hWnd, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window (low priority)
//  WM_DESTROY	- post a quit message and return
//
//

void DrawDoubleBuffer(HWND hWnd)
{
	RECT Client_Rect;
	GetClientRect(hWnd, &Client_Rect);
	int win_width = Client_Rect.right - Client_Rect.left;
	int win_height = Client_Rect.bottom + Client_Rect.left;
	PAINTSTRUCT ps;
	HDC Memhdc;
	HDC hdc;
	HBITMAP Membitmap;
	hdc = BeginPaint(hWnd, &ps);
	Memhdc = CreateCompatibleDC(hdc);
	Membitmap = CreateCompatibleBitmap(hdc, win_width, win_height);
	SelectObject(Memhdc, Membitmap);

	FillRect(Memhdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW));
	loggedData->Draw(Memhdc, drawArea1);

	BitBlt(hdc, 0, 0, win_width, win_height, Memhdc, 0, 0, SRCCOPY);
	DeleteObject(Membitmap);
	DeleteDC(Memhdc);
	DeleteDC(hdc);
	EndPaint(hWnd, &ps);
}

void CheckboxOnCheck(HWND hWnd, int id)
{

	bool *drawActualAxis;
	switch (id)
	{
	case ID_CHECK_GYRO_X:
		drawActualAxis = &loggedData->axesToDrawGyro.X;
		if (loggedData->drawingMode == pos) loggedData->drawingMode = both;
		else loggedData->drawingMode = gyro;
		break;
	case ID_CHECK_GYRO_Y:
		drawActualAxis = &loggedData->axesToDrawGyro.Y;
		if (loggedData->drawingMode == pos) loggedData->drawingMode = both;
		else loggedData->drawingMode = gyro;
		break;
	case ID_CHECK_GYRO_Z:
		drawActualAxis = &loggedData->axesToDrawGyro.Z;
		if (loggedData->drawingMode == pos) loggedData->drawingMode = both;
		else loggedData->drawingMode = gyro;
		break;
	case ID_CHECK_POS_X:
		drawActualAxis = &loggedData->axesToDrawPos.X;
		if (loggedData->drawingMode == gyro) loggedData->drawingMode = both;
		else loggedData->drawingMode = pos;
		break;
	case ID_CHECK_POS_Y:
		drawActualAxis = &loggedData->axesToDrawPos.Y;
		if (loggedData->drawingMode == gyro) loggedData->drawingMode = both;
		else loggedData->drawingMode = pos;
		break;
	case ID_CHECK_POS_Z:
		drawActualAxis = &loggedData->axesToDrawPos.Z;
		if (loggedData->drawingMode == gyro) loggedData->drawingMode = both;
		else loggedData->drawingMode = pos;
		break;
	case ID_CHECK_AVG:
		drawActualAxis = NULL;
		if (loggedData->averaged == FALSE) loggedData->averaged = TRUE;
		else loggedData->averaged = FALSE;
		break;
	}

	if (IsDlgButtonChecked(hWnd, id))
	{
		CheckDlgButton(hWnd, id, BST_UNCHECKED);
		if(drawActualAxis != NULL)
		*drawActualAxis = FALSE;
	}
	else
	{
		CheckDlgButton(hWnd, id, BST_CHECKED);
		if (drawActualAxis != NULL)
		*drawActualAxis = TRUE;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	SCROLLINFO si;
	int ret = 0;
	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_FILE_OPEN:
			if (!loggedData->Open()) 
			{
				EnableWindow(GetDlgItem(hWnd, ID_CHECK_GYRO_X), TRUE);
				EnableWindow(GetDlgItem(hWnd, ID_CHECK_GYRO_Y), TRUE);
				EnableWindow(GetDlgItem(hWnd, ID_CHECK_GYRO_Z), TRUE);
				EnableWindow(GetDlgItem(hWnd, ID_CHECK_POS_X), TRUE);
				EnableWindow(GetDlgItem(hWnd, ID_CHECK_POS_Y), TRUE);
				EnableWindow(GetDlgItem(hWnd, ID_CHECK_POS_Z), TRUE);
			}
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case DISCARD_BTN:
		
			DiscardHandler(hWnd);
			repaintWindow(hWnd, hdc, ps, &drawArea1);
			break;
		case ID_ZOOM_IN:
			loggedData->ChangeZoom(5, TRUE);
			SendMessageW(hZoomY, TBM_SETPOS, TRUE, MAX_ZOOM_Y - loggedData->zoomY);
			repaintWindow(hWnd, hdc, ps, &drawArea1);
			break;
		case ID_ZOOM_OUT:
			loggedData->ChangeZoom(5, FALSE);
			SendMessageW(hZoomY, TBM_SETPOS, TRUE, MAX_ZOOM_Y - loggedData->zoomY);
			repaintWindow(hWnd, hdc, ps, &drawArea1);
			break;
		case ID_ZOOM_X_IN:
			loggedData->zoomX++;
			if (loggedData->zoomX > 100) loggedData->zoomX = 100;
			SendMessageW(hZoomX, TBM_SETPOS, TRUE, loggedData->zoomX);

			UpdateScrollBar();

			repaintWindow(hWnd, hdc, ps, &drawArea1);
			break;
		case ID_ZOOM_X_OUT:

			loggedData->zoomX--;
			if (loggedData->zoomX < 1) loggedData->zoomX = 1;
			SendMessageW(hZoomX, TBM_SETPOS, TRUE, loggedData->zoomX);

			UpdateScrollBar();

			repaintWindow(hWnd, hdc, ps, &drawArea1);
			break;
		case ID_CHECK_GYRO_X:
			CheckboxOnCheck(hWnd, ID_CHECK_GYRO_X);
			repaintWindow(hWnd, hdc, ps, &drawArea1);
			break;
		case ID_CHECK_GYRO_Y:
			CheckboxOnCheck(hWnd, ID_CHECK_GYRO_Y);
			repaintWindow(hWnd, hdc, ps, &drawArea1);
			break;
		case ID_CHECK_GYRO_Z:
			CheckboxOnCheck(hWnd, ID_CHECK_GYRO_Z);
			repaintWindow(hWnd, hdc, ps, &drawArea1);
			//RedrawWindow(hWnd, &drawArea1, NULL, NULL);
			break;
		case ID_CHECK_POS_X:
			CheckboxOnCheck(hWnd, ID_CHECK_POS_X);
			repaintWindow(hWnd, hdc, ps, &drawArea1);
			break;
		case ID_CHECK_POS_Y:
			CheckboxOnCheck(hWnd, ID_CHECK_POS_Y);
			repaintWindow(hWnd, hdc, ps, &drawArea1);
			break;
		case ID_CHECK_POS_Z:
			CheckboxOnCheck(hWnd, ID_CHECK_POS_Z);
			repaintWindow(hWnd, hdc, ps, &drawArea1);
			//RedrawWindow(hWnd, &drawArea1, NULL, NULL);
			break;
		case ID_CHECK_AVG:
			CheckboxOnCheck(hWnd, ID_CHECK_AVG);
			repaintWindow(hWnd, hdc, ps, &drawArea1);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW));
		repaintWindow(hWnd, hdc, ps, &drawArea1);
		EndPaint(hWnd, &ps);

		break;
	case WM_ERASEBKGND:
		return true;
		break;
	case WM_DESTROY:
		delete loggedData;
		PostQuitMessage(0);
		break;

	case WM_CREATE:
		EnumChildWindows(hWnd, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));
		break;

	case WM_VSCROLL:
		loggedData->zoomY = 100 - SendMessageW(hZoomY, TBM_GETPOS, 0, 0);
		repaintWindow(hWnd, hdc, ps, &drawArea1);
		break;

	case WM_HSCROLL:
		if (lParam == (LPARAM)hScrollBar)
		{
			int position;
			switch (LOWORD(wParam))
			{
			case SB_LINEUP:
				position = GetScrollPos((HWND)lParam, SB_CTL);
				if (position > 0)position--;
				SetScrollPos((HWND)lParam, SB_CTL, position, true);
				break;
			case SB_LINEDOWN:
				position = GetScrollPos((HWND)lParam, SB_CTL);
				if (position < 1000)position++;
				SetScrollPos((HWND)lParam, SB_CTL, position, true);
				break;
			case SB_THUMBTRACK:
				SetScrollPos(hScrollBar, SB_CTL, HIWORD(wParam), true);

				break;
			default:
				break;
			}
			loggedData->scrollPos = GetScrollPos((HWND)lParam, SB_CTL);
		}
		else
		{
			loggedData->zoomX = SendMessageW(hZoomX, TBM_GETPOS, 0, 0);
			UpdateScrollBar();
		}
		repaintWindow(hWnd, hdc, ps, &drawArea1);
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case TMR_1:
			break;
		}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	TCHAR buffer[60];
	
	switch (Msg)
	{
	case WM_COMMAND:
	{
		
		// reakcja na przyciski
		switch (LOWORD(wParam))
		{
		case ID_YES: EndDialog(hwnd, ID_YES); break;
		case ID_NO: EndDialog(hwnd, ID_NO); break;
		}
	}
	case WM_INITDIALOG:
	
		swprintf_s(buffer, L"Do you really want to DELETE %d first samples?", samplesToDiscard);
		SetWindowText(GetDlgItem(hwnd, 1002), buffer);
	break;
		
	break;
	
	default: return FALSE;
	}

	return TRUE;
}


// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
