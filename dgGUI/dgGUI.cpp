// dgGUI.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "dgGUI.h"
#include "DungeonParameters.h"
#include "dungeon.h"
#include <time.h>
#include <fstream>
#include <algorithm>
#include <string>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
HWND	  myHWND;
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK NewDng(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void DrawDungeon(HDC hDC);
void SaveDungeon(HDC hDC);
bool SaveToFile(HBITMAP hBitmap, LPCTSTR lpszFileName);
void ClearScreen(HANDLE hConsole);
void DrawDungeonName(HDC hDC);
void SaveDungeonTxt();
void LoadDungeon();

DungeonParameters dp = DungeonParameters();
Dungeon dungeon = Dungeon();

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	srand(time(NULL));

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DGGUI, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DGGUI));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
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

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DGGUI));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_DGGUI);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 200, 200, NULL, NULL, hInstance, NULL);

   myHWND = hWnd;

   if (!hWnd)
   {
      return FALSE;
   }

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
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_FILE_NEW:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, NewDng);
			break;

		case ID_FILE_SAVE_TXT:
			SaveDungeonTxt();
			break;

		case ID_FILE_LOAD:
			LoadDungeon();
			break;

		case ID_FILE_SAVE:
			hdc = GetDC(hWnd);
			SaveDungeon(hdc);
			ReleaseDC(hWnd, hdc);
			break;

		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;

		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		DrawDungeon(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

/*
	SaveDungeon
	
	Saves the current dungeon.
	Creates the windows save dialog to get the
	save name then draws the dungeon to a temporary
	HBITMAP struct. This struct and the filename are
	passed to the SaveToFile to convert the struct into a 
	.bmp file.
*/
void SaveDungeon(HDC hDC)
{
	//Return if there is no dungeon to save
	if (!dungeon.dungeonBuilt)
		return;

	//Create the save file dialog to get the save file name
	OPENFILENAME ofn;
	char saveFileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = myHWND;
	ofn.lpstrFilter = "Bitmap File (*.bmp)\0*.bmp\0";
	ofn.lpstrFile = saveFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "db";
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn.lpstrTitle = "Save New Dungeon";

	if (!GetSaveFileName(&ofn))
		return;

	//Draw the dungeon to an HBITMAP which will be saved to a .bmp file
	Cell** d = dungeon.GetDungeon();
	int w = dungeon.GetWidth();
	int h = dungeon.GetHeight();
	int s = 16;

	HDC memDC = CreateCompatibleDC(hDC);
	HBITMAP memBitmap = CreateCompatibleBitmap(hDC, w * s, h * s);

	SelectObject(memDC, memBitmap);
	HBRUSH filledBlack = CreateSolidBrush(RGB(0, 0, 0));
	HBRUSH filledWhite = CreateSolidBrush(RGB(255, 255, 255));
	HBRUSH filledRed = CreateSolidBrush(RGB(255, 0, 0));
	HPEN outlinePen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

	SelectObject(memDC, filledWhite);
	Rectangle(memDC, 0, 0, w * 16, h * 16);

	SelectObject(memDC, outlinePen);
	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			switch(d[y][x].GetType())
			{
			case EMPTY:
				SelectObject(memDC, filledBlack);
				break;
			case IMPASSABLE:
				SelectObject(memDC, filledBlack);
				break;
			case WALL:
				SelectObject(memDC, filledBlack);
				break;
			case ROOM:
				SelectObject(memDC, filledWhite);
				break;
			case CORRIDOR:
				SelectObject(memDC, filledWhite);
				break;
			case DOOR:
				SelectObject(memDC, filledRed);
				break;
			default:
				SelectObject(memDC, filledBlack);
				break;
			}

			Rectangle(memDC, x * s, y * s, x * s + s, y * s + s);
		}
	}

	DrawDungeonName(memDC);

	//Save the bitmap
	SaveToFile(memBitmap, saveFileName);

	//Delete objects
	DeleteObject(filledBlack);
	DeleteObject(filledWhite);
	DeleteObject(outlinePen);

	DeleteObject(memBitmap);
	DeleteDC(memDC);
}

/*
	SaveToFile

	Uses the passed HBITMAP struct and file name to
	create a .bmp file.
*/
bool SaveToFile(HBITMAP hBitmap, LPCTSTR lpszFileName)
{
	HDC hDC;
	int iBits;
	WORD wBitCount;
	DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
	BITMAP bitmap;
	BITMAPFILEHEADER bmfHdr;
	BITMAPINFOHEADER bi;
	LPBITMAPINFOHEADER lpbi;
	HANDLE fh, hDib, hPal, hOldPal2 = NULL;

	hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);

	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else
		wBitCount = 24;

	GetObject(hBitmap, sizeof(bitmap), (LPSTR)&bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bitmap.bmWidth;
	bi.biHeight = bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 256;

	dwBmBitsSize = ((bitmap.bmWidth * wBitCount + 31) & ~31) / 8 * bitmap.bmHeight;
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = GetDC(NULL);
		hOldPal2 = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}

	GetDIBits(hDC, hBitmap, 0, (UINT)bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);
	if (hOldPal2)
	{
		SelectPalette(hDC, (HPALETTE)hOldPal2, TRUE);
		RealizePalette(hDC);
		ReleaseDC(NULL, hDC);
	}

	fh = CreateFile(lpszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fh == INVALID_HANDLE_VALUE)
		return false;

	bmfHdr.bfType = 0x4D42; // "BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return true;
}

/*
	DrawDungeon

	Draws the dungeon to the window.
	Iterates through each cell, coloring a 16 x 16 square
	a color based on the type of cell.
*/
void DrawDungeon(HDC hDC)
{
	if (!dungeon.dungeonBuilt)
		return;
	HBRUSH filledBlack = CreateSolidBrush(RGB(0, 0, 0));
	HBRUSH filledWhite = CreateSolidBrush(RGB(255, 255, 255));
	HBRUSH filledRed = CreateSolidBrush(RGB(255, 0, 0));
	HPEN outlinePen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	Cell** d = dungeon.GetDungeon();
	int w = dungeon.GetWidth();
	int h = dungeon.GetHeight();
	int s = 16;

	SetWindowPos(myHWND, HWND_TOP, 0, 0, w * s + s, (int)(h * s + s * 3.5), SWP_NOMOVE);

	RECT windR = RECT();
	GetWindowRect(myHWND, &windR);

	SelectObject(hDC, filledWhite);
	Rectangle(hDC, windR.left, windR.top, windR.right, windR.bottom + 50);

	SelectObject(hDC, outlinePen);
	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			switch(d[y][x].GetType())
			{
			case EMPTY:
				SelectObject(hDC, filledBlack);
				break;
			case IMPASSABLE:
				SelectObject(hDC, filledBlack);
				break;
			case WALL:
				SelectObject(hDC, filledBlack);
				break;
			case ROOM:
				SelectObject(hDC, filledWhite);
				break;
			case CORRIDOR:
				SelectObject(hDC, filledWhite);
				break;
			case DOOR:
				SelectObject(hDC, filledRed);
				break;
			default:
				SelectObject(hDC, filledBlack);
				break;
			}

			Rectangle(hDC, x * s, y * s, x * s + s, y * s + s);
		}
	}

	DrawDungeonName(hDC);

	DeleteObject(filledBlack);
	DeleteObject(filledWhite);
	DeleteObject(filledRed);
	DeleteObject(outlinePen);
}

/* 
	DrawDungeonName
	
	Draws the dungeon name to the screen.
*/
void DrawDungeonName(HDC hDC)
{
	std::string dungName = dungeon.GetName();
	int numChars = strlen(dungName.c_str());
	int startX = (dungeon.GetWidth() * 16 / 2) - (int)(numChars * 3.5);
	int startY = 0;

	HFONT hFont = CreateFont(16, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
		VARIABLE_PITCH, TEXT("Arial"));
	SelectObject(hDC, hFont);
	SetTextColor(hDC, RGB(255, 255, 255));
	SetBkColor(hDC, RGB(0, 0, 0));

	TextOut(hDC, startX, startY, dungName.c_str(), numChars);

	DeleteObject(hFont);
}

/*
	SaveDungeonTxt

	Saves the dungeon to a text file that can be loaded
	again at a later time.
*/
void SaveDungeonTxt()
{
	//Return if there is no dungeon to save
	if (!dungeon.dungeonBuilt)
		return;

	//Create the save file dialog to get the save file name
	OPENFILENAME ofn;
	char saveFileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = myHWND;
	ofn.lpstrFilter = "Text File (*.txt)\0*.txt\0";
	ofn.lpstrFile = saveFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "db";
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn.lpstrTitle = "Save New Dungeon";

	if (!GetSaveFileName(&ofn))
		return;

	Cell** dng = dungeon.GetDungeon();
	int width = dungeon.GetWidth();
	int height = dungeon.GetHeight();
	std::string name = dungeon.GetName();

	std::ofstream myFile;
	myFile.open(saveFileName);
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			switch (dng[y][x].GetType())
			{
			case EMPTY:
				myFile << ".";
				break;
			case IMPASSABLE:
				myFile << "I";
				break;
			case ROOM:
				myFile << "R";
				break;
			case CORRIDOR:
				myFile << "C";
				break;
			case WALL:
				myFile << "#";
				break;
			case DOOR:
				myFile << "D";
				break;
			case STAIRS_UP:
				myFile << "/";
				break;
			case STAIRS_DN:
				myFile << "\\";
				break;
			}
		}
		myFile << "\n";
	}
	myFile << name;
	myFile.close();
}

/*
	LoadDungeon

	Opens the selected text file and builds a dungeon out of its
	data.
*/
void LoadDungeon()
{
	//Create the save file dialog to get the save file name
	OPENFILENAME ofn;
	char loadFileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = myHWND;
	ofn.lpstrFilter = "Text File (*.txt)\0*.txt\0";
	ofn.lpstrFile = loadFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "db";
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn.lpstrTitle = "Load Dungeon";

	if (!GetOpenFileName(&ofn))
		return;

	std::ifstream myFile;
	myFile.open(loadFileName);
	
	Cell** dng;
	int width = 0;
	int height = 0;
	dungeon = Dungeon();

	std::vector<std::string> textDungeon;
	textDungeon.insert(textDungeon.end(),"");

	while (std::getline(myFile, textDungeon[height]))
	{
		height += 1;
		textDungeon.insert(textDungeon.end(), "");
	}
	myFile.close();

	width = textDungeon[0].length();
	//Compensate for the name
	height -= 1;

	dng = new Cell*[height];
	for (int y = 0; y < height; ++y)
	{
		dng[y] = new Cell[width];

		for (int x = 0; x < width; ++x)
		{
			dng[y][x] = Cell(x, y);

			switch (textDungeon[y].c_str()[x])
			{
			case '.':
				dng[y][x].SetType(EMPTY);
				break;
			case '#':
				dng[y][x].SetType(WALL);
				break;
			case 'I':
				dng[y][x].SetType(IMPASSABLE);
				break;
			case 'R':
				dng[y][x].SetType(ROOM);
				break;
			case 'C':
				dng[y][x].SetType(CORRIDOR);
				break;
			case 'D':
				dng[y][x].SetType(DOOR);
				break;
			case '/':
				dng[y][x].SetType(STAIRS_UP);
				break;
			case '\\':
				dng[y][x].SetType(STAIRS_DN);
				break;
			}
		}
	}

	std::string name = textDungeon[height];

	dungeon.LoadDungeon(dng, width, height, name);

	ShowWindow(myHWND, FALSE);
	ShowWindow(myHWND, TRUE);
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

// Message handler for Dungeon Parameters
INT_PTR CALLBACK NewDng(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_INITDIALOG:
		dp.LinkToControls(hDlg);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDOK)
			{	
				dp.GetData();
				dungeon = Dungeon(dp.width, dp.height, dp.min, dp.max, (DungeonMask)dp.mask, (RoomDensity)dp.roomDensity, (CorridorType)dp.corType, 100 - dp.deadEnd);
				dungeon.GenerateDungeon();

				ShowWindow(myHWND, FALSE);
				ShowWindow(myHWND, TRUE);

				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			else
				if (LOWORD(wParam) == IDCANCEL)
				{
					EndDialog(hDlg, LOWORD(wParam));
					return (INT_PTR)TRUE;
				}

		}
		break;
	}
	return (INT_PTR)FALSE;
}