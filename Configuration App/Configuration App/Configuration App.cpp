// Configuration App.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Configuration App.h"
#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <string>
#define TOTAL_BYTES_READ    1024
#define OFFSET_BYTES 1024
using namespace std;
#define SET_TIME 11
#define MONDAY 4
#define TUESDAY 5
#define WEDNESDAY 6
#define THURSDAY 7
#define FRIDAY 8
#define SATURDAY 9
#define SUNDAY 10
#define SET_LIMIT 12
#define SPECIFIC_DATE 13
#define SPECIFIC_TIME 14
#define SPECIFIC_SAVE 15
int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
);
LRESULT CALLBACK WndProc(
	_In_ HWND   hwnd,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
);
void addControls(HWND hWnd);
string getBoxValue(HWND hWnd);
BOOL CreateRegistryKey(HKEY hKeyParent, PWCHAR subkey);
BOOL writeStringInRegistry(HKEY hKeyParent, PWCHAR subkey, PWCHAR valueName, PWCHAR strData);
BOOL readStringFromRegistry(HKEY hKeyParent, PWCHAR subkey, PWCHAR valueName, PWCHAR* readData);
PWCHAR convertToPWCHAR(string stringToConvert);
string readString(string location);
void clearInput(HWND hWnd);
bool insertString(string location, string key);

HINSTANCE hInst;
static TCHAR szWindowClass[] = _T("win32app");
static TCHAR szTitle[] = _T("Sleep Time Configuration");
HWND globalTimeWindow;
HWND globalTimeLabel;
HWND monday;
HWND tuesday;
HWND wednesday;
HWND thursday;
HWND friday;
HWND saturday;
HWND sunday;
HWND specificDate;
HWND specificTime;
HWND doNotCheck;
HWND appNameLimit;
HWND appTimeLimit;
HWND appNameMonitor;
bool mondayChecked;
bool tuesdayChecked;
bool wednesdayChecked;
bool thursdayChecked;
bool fridayChecked;
bool saturdayChecked;
bool sundayChecked;
string global="";
string mondayString= "";
string tuesdayString = "";
string wednesdayString = "";
string thursdayString = "";
string fridayString = "";
string saturdayString = "";
string sundayString = "";
string specificDateString = "";
string specificTimeString = "";
string doNotCheckString = "";
string appNameLimitString = "";
string appTimeLimitString = "";
string appNameMonitorString = "";
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow) {

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(211, 211, 211));
	//wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}
	// Store instance handle in our global variable
	hInst = hInstance;
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
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		800, 800,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}
	ShowWindow(hWnd,
		nCmdShow);
	UpdateWindow(hWnd);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}
LRESULT CALLBACK WndProc(
	HWND   hWnd,
	UINT   uMsg,
	WPARAM wParam,
	LPARAM lParam
) {
	PAINTSTRUCT ps;
	HDC hdc;
	mondayChecked = false;
	tuesdayChecked = false;
	wednesdayChecked = false;
	thursdayChecked = false;
	fridayChecked = false;
	saturdayChecked = false;
	sundayChecked = false;
	switch (uMsg)
	{
	case WM_CREATE:
		addControls(hWnd);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// End application-specific layout section.

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		mondayChecked = IsDlgButtonChecked(hWnd, MONDAY);
		tuesdayChecked = IsDlgButtonChecked(hWnd, TUESDAY);
		wednesdayChecked = IsDlgButtonChecked(hWnd, WEDNESDAY);
		thursdayChecked = IsDlgButtonChecked(hWnd, THURSDAY);
		fridayChecked = IsDlgButtonChecked(hWnd, FRIDAY);
		saturdayChecked = IsDlgButtonChecked(hWnd, SATURDAY);
		sundayChecked = IsDlgButtonChecked(hWnd, SUNDAY);
		switch (wParam) {
		case SET_TIME:
			global = getBoxValue(globalTimeWindow);
			clearInput(globalTimeWindow);
			mondayString = getBoxValue(monday);
			clearInput(monday);
			tuesdayString = getBoxValue(tuesday);
			clearInput(tuesday);
			wednesdayString = getBoxValue(wednesday);
			clearInput(wednesday);
			thursdayString = getBoxValue(thursday);
			clearInput(thursday);
			fridayString = getBoxValue(friday);
			clearInput(friday);
			saturdayString = getBoxValue(saturday);
			clearInput(saturday);
			sundayString = getBoxValue(sunday);
			clearInput(sunday);
			specificDateString = getBoxValue(specificDate);
			clearInput(specificDate);
			specificTimeString = getBoxValue(specificTime);
			clearInput(specificTime);
			doNotCheckString = getBoxValue(doNotCheck);
			clearInput(doNotCheck);
			appNameLimitString = getBoxValue(appNameLimit);
			clearInput(appNameLimit);
			appTimeLimitString = getBoxValue(appTimeLimit);
			clearInput(appTimeLimit);
			appNameMonitorString = getBoxValue(appNameMonitor);
			clearInput(appNameMonitor);
			try {
				BOOL status;
	
				status = CreateRegistryKey(HKEY_CURRENT_USER, (PWCHAR)L"SleepScheduler"); //create key
				if (status == TRUE) {
					insertString("global", global);


					if (!mondayChecked && global.size() > 0) {
						insertString("monday", global);
					}
					else {
						insertString("monday", mondayString);
					}
					if (!tuesdayChecked && global.size() > 0) {
						insertString("tuesday", global);
					}
					else {
						insertString("tuesday", tuesdayString);
					}
					if (!wednesdayChecked && global.size() > 0) {
						insertString("wednesday", global);
					}
					else {
						insertString("wednesday", wednesdayString);
					}
					if (!thursdayChecked && global.size() > 0) {
						insertString("thursday", global);
					}
					else {
						insertString("thursday", thursdayString);
					}
					if (!fridayChecked && global.size() > 0) {
						insertString("friday", global);
					}
					else {
						insertString("friday", fridayString);
					}
					if (!saturdayChecked && global.size() > 0) {
						insertString("saturday", global);
					}
					else {
						insertString("saturday", saturdayString);
					}
					if (!sundayChecked && global.size() > 0) {
						insertString("sunday", global);
					}
					else {
						insertString("sunday", sundayString);
					}
					if (specificDateString.size() > 0) {
						insertString("specificDate", (readString("specificDate") + "!" + specificDateString + "@" + specificTimeString));
					}
					if (doNotCheckString.size() > 0) {
						insertString("doNotCheck", (readString("doNotCheck") + "!" + doNotCheckString));
					}
					if (appNameLimitString.size() > 0) {
						insertString("appLimit", (readString("appLimit") + "!" + appNameLimitString + "@" + appTimeLimitString));
					}
					if (appNameMonitorString.size() > 0) {
						insertString("appMonitor", (readString("appMonitor") + "!" + appNameMonitorString));
					}
					/*
						clearInput(specificDate);
					specificTimeString = getBoxValue(specificTime);
					clearInput(specificTime);
					doNotCheckString = getBoxValue(doNotCheck);
					clearInput(doNotCheck);
					appNameLimitString = getBoxValue(appNameLimit);
					clearInput(appNameLimit);
					appTimeLimitString = getBoxValue(appTimeLimit);
					clearInput(appTimeLimit);
					appNameMonitorString = getBoxValue(appNameMonitor);
					clearInput(appNameMonitor);
					*/
				}
				
			}
			catch (exception ex) {

			}
			break;
		}
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}

	return 0;
}
void addControls(HWND hWnd) {
	globalTimeLabel = CreateWindowW(L"Static", L"Global Stop Time:", WS_VISIBLE | WS_CHILD | SS_CENTER, 100, 100, 120, 20, hWnd, NULL, NULL, NULL);
	globalTimeWindow = CreateWindowW(L"Edit", L"", WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER, 230, 100, 80, 20, hWnd, NULL, NULL, NULL);

	CreateWindowW(L"Static", L"Special Instructions", WS_VISIBLE | WS_CHILD | SS_CENTER, 150, 150, 150, 20, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"button", L"Monday", WS_VISIBLE | WS_CHILD | BS_CHECKBOX | SS_CENTER, 100, 200, 100, 20, hWnd, (HMENU)MONDAY, NULL, NULL);
	CreateWindowW(L"button", L"Tuesday", WS_VISIBLE | WS_CHILD | BS_CHECKBOX | SS_CENTER, 100, 220, 100, 20, hWnd, (HMENU)TUESDAY, NULL, NULL);
	CreateWindowW(L"button", L"Wednesday", WS_VISIBLE | WS_CHILD | BS_CHECKBOX | SS_CENTER, 100, 240, 100, 20, hWnd, (HMENU)WEDNESDAY, NULL, NULL);
	CreateWindowW(L"button", L"Thursday", WS_VISIBLE | WS_CHILD | BS_CHECKBOX | SS_CENTER, 100, 260, 100, 20, hWnd, (HMENU)THURSDAY, NULL, NULL);
	CreateWindowW(L"button", L"Friday", WS_VISIBLE | WS_CHILD | BS_CHECKBOX | SS_CENTER, 100, 280, 100, 20, hWnd, (HMENU)FRIDAY, NULL, NULL);
	CreateWindowW(L"button", L"Saturday", WS_VISIBLE | WS_CHILD | BS_CHECKBOX | SS_CENTER, 100, 300, 100, 20, hWnd, (HMENU)SATURDAY, NULL, NULL);
	CreateWindowW(L"button", L"Sunday", WS_VISIBLE | WS_CHILD | BS_CHECKBOX | SS_CENTER, 100, 320, 100, 20, hWnd, (HMENU)SUNDAY, NULL, NULL);

	monday = CreateWindowW(L"Edit", L"", WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER, 230, 200, 80, 20, hWnd, NULL, NULL, NULL);
	tuesday = CreateWindowW(L"Edit", L"", WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER, 230, 220, 80, 20, hWnd, NULL, NULL, NULL);
	wednesday = CreateWindowW(L"Edit", L"", WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER, 230, 240, 80, 20, hWnd, NULL, NULL, NULL);
	thursday = CreateWindowW(L"Edit", L"", WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER, 230, 260, 80, 20, hWnd, NULL, NULL, NULL);
	friday = CreateWindowW(L"Edit", L"", WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER, 230, 280, 80, 20, hWnd, NULL, NULL, NULL);
	saturday = CreateWindowW(L"Edit", L"", WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER, 230, 300, 80, 20, hWnd, NULL, NULL, NULL);
	sunday = CreateWindowW(L"Edit", L"", WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER, 230, 320, 80, 20, hWnd, NULL, NULL, NULL);

	CreateWindowW(L"Static", L"Specific Day Exception:", WS_VISIBLE | WS_CHILD | SS_CENTER, 120, 380, 180, 20, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"Static", L"Date(M/D/Y):", WS_VISIBLE | WS_CHILD | SS_CENTER, 100, 430, 100, 20, hWnd, NULL, NULL, NULL);
	specificDate = CreateWindowW(L"Edit", L"", WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER, 230, 430, 80, 20, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"Static", L"Time:", WS_VISIBLE | WS_CHILD | SS_CENTER, 100, 450, 100, 20, hWnd, NULL, NULL, NULL);
	specificTime = CreateWindowW(L"Edit", L"", WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER, 230, 450, 80, 20, hWnd, NULL, NULL, NULL);

	CreateWindowW(L"Static", L"Do not check limit:", WS_VISIBLE | WS_CHILD | SS_CENTER, 120, 500, 180, 20, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"Static", L"Date(M/D/Y):", WS_VISIBLE | WS_CHILD | SS_CENTER, 100, 550, 100, 20, hWnd, NULL, NULL, NULL);
	doNotCheck = CreateWindowW(L"Edit", L"", WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER, 230, 550, 80, 20, hWnd, NULL, NULL, NULL);

	CreateWindowW(L"Button", L"Save", WS_VISIBLE | WS_CHILD, 100, 600, 80, 30, hWnd, (HMENU)SET_TIME, NULL, NULL);

	CreateWindowW(L"Static", L"Set App Limit", WS_VISIBLE | WS_CHILD | SS_CENTER, 400, 100, 100, 20, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"Static", L"App Name:", WS_VISIBLE | WS_CHILD | SS_CENTER, 400, 140, 100, 20, hWnd, NULL, NULL, NULL);
	appNameLimit= CreateWindowW(L"Edit", L"", WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER, 530, 140, 200, 20, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"Static", L"Time(Hours):", WS_VISIBLE | WS_CHILD | SS_CENTER, 400, 160, 100, 20, hWnd, NULL, NULL, NULL);
	appTimeLimit = CreateWindowW(L"Edit", L"", WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER, 530, 160, 80, 20, hWnd, NULL, NULL, NULL);

	CreateWindowW(L"Static", L"Monitor App Usage:", WS_VISIBLE | WS_CHILD | SS_CENTER, 400, 200, 160, 20, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"Static", L"App Name:", WS_VISIBLE | WS_CHILD | SS_CENTER, 400, 240, 100, 20, hWnd, NULL, NULL, NULL);
	appNameMonitor = CreateWindowW(L"Edit", L"", WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER, 530, 240, 200, 20, hWnd, NULL, NULL, NULL);

}
string getBoxValue(HWND hWnd) {
	wchar_t Text[100];
	GetWindowTextW(hWnd, Text, 100);
	wstring wString(Text);
	return string(wString.begin(), wString.end());
}
void clearInput(HWND hWnd) {
	wstring emptyString = L"";
	LPCWSTR emptyStringPointer; //define an array with size of my_str + 1
	emptyStringPointer = emptyString.c_str();
	SetWindowTextW(hWnd, emptyStringPointer);
}
BOOL CreateRegistryKey(HKEY hKeyParent, PWCHAR subkey)
{
	DWORD dwDisposition; //It verify new key is created or open existing key
	HKEY  hKey;
	DWORD Ret;
	Ret =
		RegCreateKeyEx(
			hKeyParent,
			subkey,
			0,
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,
			NULL,
			&hKey,
			&dwDisposition);
	if (Ret != ERROR_SUCCESS)
	{
		printf("Error opening or creating new key\n");
		return FALSE;
	}
	RegCloseKey(hKey); //close the key
	return TRUE;
}
BOOL writeStringInRegistry(HKEY hKeyParent, PWCHAR subkey, PWCHAR valueName, PWCHAR strData)
{
	DWORD Ret;
	HKEY hKey;
	//Check if the registry exists
	Ret = RegOpenKeyEx(
		hKeyParent,
		subkey,
		0,
		KEY_WRITE,
		&hKey
	);
	if (Ret == ERROR_SUCCESS)
	{
		if (ERROR_SUCCESS !=
			RegSetValueEx(
				hKey,
				valueName,
				0,
				REG_SZ,
				(LPBYTE)(strData),
				((((DWORD)lstrlen(strData) + 1)) * 2)))
		{
			RegCloseKey(hKey);
			return FALSE;
		}
		RegCloseKey(hKey);
		return TRUE;
	}
	return FALSE;
}
BOOL readStringFromRegistry(HKEY hKeyParent, PWCHAR subkey, PWCHAR valueName, PWCHAR* readData)
{
	HKEY hKey;
	DWORD len = TOTAL_BYTES_READ;
	DWORD readDataLen = len;
	PWCHAR readBuffer = (PWCHAR)malloc(sizeof(PWCHAR) * len);
	if (readBuffer == NULL)
		return FALSE;
	//Check if the registry exists
	DWORD Ret = RegOpenKeyEx(
		hKeyParent,
		subkey,
		0,
		KEY_READ,
		&hKey
	);
	if (Ret == ERROR_SUCCESS)
	{
		Ret = RegQueryValueEx(
			hKey,
			valueName,
			NULL,
			NULL,
			(BYTE*)readBuffer,
			&readDataLen
		);
		while (Ret == ERROR_MORE_DATA)
		{
			// Get a buffer that is big enough.
			len += OFFSET_BYTES;
			readBuffer = (PWCHAR)realloc(readBuffer, len);
			readDataLen = len;
			Ret = RegQueryValueEx(
				hKey,
				valueName,
				NULL,
				NULL,
				(BYTE*)readBuffer,
				&readDataLen
			);
		}
		if (Ret != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return false;;
		}
		*readData = readBuffer;
		RegCloseKey(hKey);
		return true;
	}
	else
	{
		return false;
	}
}
PWCHAR convertToPWCHAR(string stringToConvert) {
	std::wstring wideString = std::wstring(stringToConvert.begin(), stringToConvert.end());
	const wchar_t* wideCharArray = wideString.c_str();
	wchar_t* clone = _wcsdup(wideCharArray);
	return (PWCHAR)clone;
}
bool insertString(string location, string key) {
	if (key.size() <= 0) {
		return false;
	}
	bool pass = writeStringInRegistry(HKEY_CURRENT_USER, (PWCHAR)L"SleepScheduler", convertToPWCHAR(location), convertToPWCHAR(key));
	return pass;
}
string readString(string location) {

	BOOL status;
	PWCHAR readMessage = nullptr;
	
	status = readStringFromRegistry(HKEY_CURRENT_USER, (PWCHAR)L"SleepScheduler", convertToPWCHAR(location), &readMessage); //read string
	if (status != TRUE) {
		return "";
	}
	if (readMessage != nullptr)
	{
		std::wstring wideMessage(readMessage);
		std::string keyValue(wideMessage.begin(), wideMessage.end());

		free(readMessage);
		readMessage = nullptr;
		return keyValue;
	}
	return "";
}