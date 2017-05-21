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

CData *dataLog;

INT value;

// buttons
HWND hwndButton;
HWND hZoomY, hZoomX;
HWND hScrollBar;
// sent data
int col = 0;
std::vector<Point> data;
RECT drawArea1 = { 350, 20, 350 + 400, 400 };
RECT drawArea2 = { 50, 400, 650, 422 };

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Buttons(HWND, UINT, WPARAM, LPARAM);

void DrawDoubleBuffer(HWND hWnd);

bool CALLBACK SetFont(HWND child, LPARAM font) {
	SendMessage(child, WM_SETFONT, font, true);
	return true;
}

void MyOnPaint(HDC hdc)
{

	dataLog->Draw(hdc, drawArea1);
}

void repaintWindow(HWND hWnd, HDC &hdc, PAINTSTRUCT &ps, RECT *drawArea)
{

	if (drawArea == NULL)
		InvalidateRect(hWnd, NULL, FALSE); // repaint all
	else
		InvalidateRect(hWnd, drawArea, FALSE); //repaint drawArea

	DrawDoubleBuffer(hWnd);

	//hdc = BeginPaint(hWnd, &ps);
	 //MyOnPaint(hdc);
	 //EndPaint(hWnd, &ps);
}

void inputData()
{
	data.push_back(Point(0, 0));
	for (int i = 1; i < 100; i++) {
		data.push_back(Point(2 * i + 1, 200 * rand() / RAND_MAX));
	}
}


int OnCreate(HWND window)
{
	inputData();

	return 0;
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

	value = 0;

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
	//wcex.hbrBackground = NULL;
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


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;



	hInst = hInstance; // Store instance handle (of exe) in our global variable

	// main window
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 800, 600, NULL, NULL, hInstance, NULL);

	dataLog = new CData(hWnd);

	// create button and store the handle    

	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("DISCARD SAMPLES"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		0, 60,                                  // the left and top co-ordinates
		80, 25,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)DISCARD_BTN,                   // the ID of your button
		hInstance,                            // the instance of your application
		NULL);                               // extra bits you dont really need

	hwndButton = CreateWindow(TEXT("button"),                      // The class name required is button
		TEXT("DrawAll"),                  // the caption of the button
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  // the styles
		0, 0,                                  // the left and top co-ordinates
		80, 25,                              // width and height
		hWnd,                                 // parent window handle
		(HMENU)ID_TIME_DOWN,                   // the ID of your button
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
	// create button and store the handle                                                       

	/*HWND hLeftLabel = CreateWindowW(L"Static", L"100",
		WS_CHILD | WS_VISIBLE, 0, 0, 30, 20, hWnd, (HMENU)1, NULL, NULL);

	HWND hRightLabel = CreateWindowW(L"Static", L"0",
		WS_CHILD | WS_VISIBLE, 0, 0, 10, 20, hWnd, (HMENU)2, NULL, NULL);*/

	CreateWindowW(L"button", L"Gyro X",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		100, 20, 50, 20, hWnd, (HMENU)ID_CHECK_GYRO_X,
		NULL, NULL);
	CreateWindowW(L"button", L"Gyro Y",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		100, 40, 50, 20, hWnd, (HMENU)ID_CHECK_GYRO_Y,
		NULL, NULL);
	CreateWindowW(L"button", L"Gyro Z",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		100, 60, 50, 20, hWnd, (HMENU)ID_CHECK_GYRO_Z,
		NULL, NULL);
	CreateWindowW(L"button", L"Pos X",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		160, 20, 50, 20, hWnd, (HMENU)ID_CHECK_POS_X,
		NULL, NULL);
	CreateWindowW(L"button", L"Pos Y",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		160, 40, 50, 20, hWnd, (HMENU)ID_CHECK_POS_Y,
		NULL, NULL);
	CreateWindowW(L"button", L"Pos Z",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		160, 60, 50, 20, hWnd, (HMENU)ID_CHECK_POS_Z,
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

	SendMessageW(hZoomY, TBM_SETRANGE, TRUE, MAKELONG(1, MAX_ZOOM_Y));
	SendMessageW(hZoomY, TBM_SETPAGESIZE, 0, 1);
	SendMessageW(hZoomY, TBM_SETTICFREQ, 5, 0);
	SendMessageW(hZoomY, TBM_SETPOS, TRUE, MAX_ZOOM_Y - 1);
	//SendMessageW(hTrack, TBM_SETBUDDY, TRUE, (LPARAM)hLeftLabel);
	//SendMessageW(hTrack, TBM_SETBUDDY, FALSE, (LPARAM)hRightLabel);

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

	SCROLLINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE | SIF_PAGE;
	si.nMin = 0;
	si.nMax = dataLog->dataSize * dataLog->zoomX / 2;
	si.nPage = (dataLog->dataSize + dataLog->zoomX) / dataLog->zoomX;

	SetScrollInfo(hScrollBar, SB_CTL, &si, TRUE);

	hwndButton = CreateWindow(TEXT("button"), TEXT("Timer ON"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		0, 155, 100, 30, hWnd, (HMENU)ID_RBUTTON1, GetModuleHandle(NULL), NULL);

	hwndButton = CreateWindow(TEXT("button"), TEXT("Timer OFF"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		0, 200, 100, 30, hWnd, (HMENU)ID_RBUTTON2, GetModuleHandle(NULL), NULL);


	hwndButton = CreateWindow(TEXT("BUTTON"), TEXT("Axis to plot"),
		WS_CHILD | WS_VISIBLE | BS_GROUPBOX | WS_GROUP,
		80, 3, 150, 100,
		hWnd, NULL, hInstance, 0);
	/*hwndButton = CreateWindow(TEXT("button"), TEXT("Graph"),
		WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		600,0, 500, 500, hWnd, (HMENU)ID_GROUP1, GetModuleHandle(NULL), NULL);*/

		//hwndButton = CreateWindow(TEXT("static"),                      // The class name required is button
		//	TEXT("Test123"),                  // the caption of the button
		//	WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | SS_CENTER,  // the styles
		//	500, 60,                                  // the left and top co-ordinates
		//	500, 50,                              // width and height
		//	hWnd,                                 // parent window handle
		//	(HMENU)ID_LABEL1,                   // the ID of your button
		//	hInstance,                            // the instance of your application
		//	NULL);

	OnCreate(hWnd);

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
	dataLog->Draw(Memhdc, drawArea1);

	BitBlt(hdc, 0, 0, win_width, win_height, Memhdc, 0, 0, SRCCOPY);
	DeleteObject(Membitmap);
	DeleteDC(Memhdc);
	DeleteDC(hdc);
	EndPaint(hWnd, &ps);
}

void CheckboxOnCheck(HWND hWnd, int id)
{

	bool *actualAxis;
	switch (id)
	{
	case ID_CHECK_GYRO_X:
		actualAxis = &dataLog->axesToDrawGyro.X;
		if (dataLog->drawingMode == 2 || dataLog->drawingMode == 0) dataLog->drawingMode += 1;
		else dataLog->drawingMode -= 1;
		break;
	case ID_CHECK_GYRO_Y:
		actualAxis = &dataLog->axesToDrawGyro.Y;
		if (dataLog->drawingMode == pos) dataLog->drawingMode = both;
		else dataLog->drawingMode = gyro;
		break;
	case ID_CHECK_GYRO_Z:
		actualAxis = &dataLog->axesToDrawGyro.Z;
		if (dataLog->drawingMode == pos) dataLog->drawingMode = both;
		else dataLog->drawingMode = gyro;
		break;
	case ID_CHECK_POS_X:
		actualAxis = &dataLog->axesToDrawPos.X;
		if (dataLog->drawingMode == gyro) dataLog->drawingMode = both;
		else dataLog->drawingMode = pos;
		break;
	case ID_CHECK_POS_Y:
		actualAxis = &dataLog->axesToDrawPos.Y;
		if (dataLog->drawingMode == gyro) dataLog->drawingMode = both;
		else dataLog->drawingMode = pos;
		break;
	case ID_CHECK_POS_Z:
		actualAxis = &dataLog->axesToDrawPos.Z;
		if (dataLog->drawingMode == gyro) dataLog->drawingMode = both;
		else dataLog->drawingMode = pos;

		break;
	}

	if (IsDlgButtonChecked(hWnd, id))
	{
		CheckDlgButton(hWnd, id, BST_UNCHECKED);
		*actualAxis = FALSE;
	}
	else
	{
		CheckDlgButton(hWnd, id, BST_CHECKED);
		*actualAxis = TRUE;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	SCROLLINFO si;
	static bool erase = 1;
	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);



		// MENU & BUTTON messages
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_FILE_OPEN:
			dataLog->Open();
			EnableWindow(GetDlgItem(hWnd, ID_CHECK_GYRO_X), TRUE);
			EnableWindow(GetDlgItem(hWnd, ID_CHECK_GYRO_Y), TRUE);
			EnableWindow(GetDlgItem(hWnd, ID_CHECK_GYRO_Z), TRUE);
			EnableWindow(GetDlgItem(hWnd, ID_CHECK_POS_X), TRUE);
			EnableWindow(GetDlgItem(hWnd, ID_CHECK_POS_Y), TRUE);
			EnableWindow(GetDlgItem(hWnd, ID_CHECK_POS_Z), TRUE);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case DISCARD_BTN:
			//repaintWindow(hWnd, hdc, ps, &drawArea1);
			//EnableWindow(GetDlgItem(hWnd, ID_TIME_DOWN), FALSE);
			break;
		case ID_TIME_DOWN:
			//repaintWindow(hWnd, hdc, ps, NULL);
			dataLog->Open();
			break;
		case ID_ZOOM_IN:
			dataLog->ChangeZoom(5, TRUE);
			SendMessageW(hZoomY, TBM_SETPOS, TRUE, MAX_ZOOM_Y - dataLog->zoomY);
			repaintWindow(hWnd, hdc, ps, &drawArea1);
			break;
		case ID_ZOOM_OUT:
			dataLog->ChangeZoom(5, FALSE);
			SendMessageW(hZoomY, TBM_SETPOS, TRUE, MAX_ZOOM_Y - dataLog->zoomY);
			repaintWindow(hWnd, hdc, ps, &drawArea1);
			break;
		case ID_ZOOM_X_IN:
			dataLog->zoomX++;
			if (dataLog->zoomX > 100) dataLog->zoomX = 100;
			SendMessageW(hZoomX, TBM_SETPOS, TRUE, dataLog->zoomX);

			ZeroMemory(&si, sizeof(si));
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_RANGE | SIF_PAGE;
			si.nMin = 0;
			si.nMax = dataLog->dataSize * dataLog->zoomX / 2;
			si.nPage = (dataLog->dataSize + dataLog->zoomX) / dataLog->zoomX;

			SetScrollInfo(hScrollBar, SB_CTL, &si, TRUE);
			repaintWindow(hWnd, hdc, ps, &drawArea1);
			break;
		case ID_ZOOM_X_OUT:

			dataLog->zoomX--;
			if (dataLog->zoomX < 1) dataLog->zoomX = 1;
			SendMessageW(hZoomX, TBM_SETPOS, TRUE, dataLog->zoomX);

			ZeroMemory(&si, sizeof(si));
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_RANGE | SIF_PAGE;
			si.nMin = 0;
			si.nMax = dataLog->dataSize * dataLog->zoomX / 2;
			si.nPage = (dataLog->dataSize + dataLog->zoomX) / dataLog->zoomX;

			SetScrollInfo(hScrollBar, SB_CTL, &si, TRUE);
			repaintWindow(hWnd, hdc, ps, &drawArea1);
			break;
		case ID_RBUTTON1:
			SetTimer(hWnd, TMR_1, 25, 0);
			break;
		case ID_RBUTTON2:
			KillTimer(hWnd, TMR_1);
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

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
		/*case WM_PAINT:
			DrawDoubleBuffer(hWnd);
			break;*/

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW));
		repaintWindow(hWnd, hdc, ps, &drawArea1);
		// TODO: Add any drawing code here (not depend on timer, buttons)
		EndPaint(hWnd, &ps);

		break;
	case WM_ERASEBKGND:
		return true;
		break;
	case WM_DESTROY:
		delete dataLog;
		PostQuitMessage(0);
		break;

	case WM_CREATE:
		EnumChildWindows(hWnd, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT));
		break;

	case WM_VSCROLL:
		dataLog->zoomY = 100 - SendMessageW(hZoomY, TBM_GETPOS, 0, 0);
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
			dataLog->scrollPos = GetScrollPos((HWND)lParam, SB_CTL);
		}
		else
		{
			dataLog->zoomX = SendMessageW(hZoomX, TBM_GETPOS, 0, 0);
			ZeroMemory(&si, sizeof(si));

			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_RANGE | SIF_PAGE;
			si.nMin = 0;
			si.nMax = dataLog->dataSize*dataLog->zoomX / 2;
			si.nPage = (dataLog->dataSize + dataLog->zoomX) / dataLog->zoomX;

			SetScrollInfo(hScrollBar, SB_CTL, &si, TRUE);

		}
		repaintWindow(hWnd, hdc, ps, &drawArea1);
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case TMR_1:
			//force window to repaint
			//repaintWindow(hWnd, hdc, ps, &drawArea1);
			value++;
			break;
		}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
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
