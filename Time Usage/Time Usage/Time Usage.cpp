// Time Usage.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Time Usage.h"

#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <sstream> 
#define SET_TIME 11
#define TOTAL_BYTES_READ 1024
#define OFFSET_BYTES 1024
using namespace std;
BOOL CreateRegistryKey(HKEY hKeyParent, PWCHAR subkey);
BOOL readStringFromRegistry(HKEY hKeyParent, PWCHAR subkey, PWCHAR valueName, PWCHAR* readData);
PWCHAR convertToPWCHAR(string stringToConvert);
string readString(string location);
vector<string> split(const string& str, const string& delim);
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
HINSTANCE hInst;
static TCHAR szWindowClass[] = _T("win32app");
static TCHAR szTitle[] = _T("App Usage");
HWND globalTimeWindow;
ifstream in;
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
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
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
		switch (wParam) {
		case SET_TIME:

			break;
		}
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}

	return 0;
}
struct appTimeData {
	int minutes;
	string appName;
	appTimeData(int minutes, string appName) {
		this->minutes = minutes;
		this->appName = appName;
	}
};
std::wstring stringToWString(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

void addControls(HWND hWnd) {
	CreateRegistryKey(HKEY_CURRENT_USER, (PWCHAR)L"SleepScheduler");
	//daily activity
	string appUsageDayString = readString("appUsageDay");
	vector<string> appUsageDayStringArray = split(appUsageDayString, "!");
	vector<appTimeData> AppUsageDay;
	for (string appUsageDayString : appUsageDayStringArray) {
		vector<string> appUsageUnit = split(appUsageDayString, "@");
		string appString = appUsageUnit.at(0);
		stringstream stream(appUsageUnit.at(1));
		int minutesUsed;
		stream >> minutesUsed;
			AppUsageDay.push_back(appTimeData(minutesUsed, appString));
	}
	//weekly activity
	string appUsageWeekString = readString("appUsageWeek");
	vector<string> appUsageWeekStringArray = split(appUsageWeekString, "!");
	vector<appTimeData> AppUsageWeek;
	for (string appUsageWeekString : appUsageWeekStringArray) {
		vector<string> appUsageUnit = split(appUsageWeekString, "@");
		string appString = appUsageUnit.at(0);
		stringstream stream(appUsageUnit.at(1));
		int minutesUsed;
		stream >> minutesUsed;
		AppUsageWeek.push_back(appTimeData(minutesUsed, appString));
	}
	//prints out activity
	CreateWindowW(L"Static", L"Daily Usage:", WS_VISIBLE | WS_CHILD | SS_CENTER, 50, 40, 120, 20, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"Static", L"Minutes:", WS_VISIBLE | WS_CHILD | SS_CENTER, 30, 60, 120, 20, hWnd, NULL, NULL, NULL);
	for (int i = 0; i < static_cast<int>(AppUsageDay.size()); i++) {
		string nameString = AppUsageDay.at(i).appName;
		string minuteString = to_string(AppUsageDay.at(i).minutes);
		std::wstring WnameString= stringToWString(nameString);
		LPCWSTR name = WnameString.c_str();
		std::wstring WminuteString = stringToWString(minuteString);
		LPCWSTR minute = WminuteString.c_str();
		CreateWindowW(L"Static", name, WS_CHILD | WS_VISIBLE | SS_LEFT | WS_BORDER, 130, 100 + 30 * i, 240, 30, hWnd, NULL, NULL, NULL);
		CreateWindowW(L"Static",minute, WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER, 50, 100+30*i, 80, 30, hWnd, NULL, NULL, NULL);
	}
	CreateWindowW(L"Static", L"Weekly Usage:", WS_VISIBLE | WS_CHILD | SS_CENTER, 50, 140+30*static_cast<int>(AppUsageDay.size()), 120, 20, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"Static", L"Minutes", WS_VISIBLE | WS_CHILD | SS_CENTER, 30, 160 + 30 * static_cast<int>(AppUsageDay.size()), 120, 20, hWnd, NULL, NULL, NULL);

	for (int i = 0; i < static_cast<int>(AppUsageWeek.size()); i++) {
		string nameString = AppUsageWeek.at(i).appName;
		string minuteString = to_string(AppUsageWeek.at(i).minutes);
		std::wstring WnameString = stringToWString(nameString);
		LPCWSTR name = WnameString.c_str();
		std::wstring WminuteString = stringToWString(minuteString);
		LPCWSTR minute = WminuteString.c_str();
		CreateWindowW(L"Static", name, WS_CHILD | WS_VISIBLE | SS_LEFT | WS_BORDER, 130, 180 + 30 * static_cast<int>(AppUsageDay.size()) + 30 * i, 240, 30, hWnd, NULL, NULL, NULL);
		CreateWindowW(L"Static", minute, WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER, 50, 180 + 30 * static_cast<int>(AppUsageDay.size()) + 30 * i, 80, 30, hWnd, NULL, NULL, NULL);
	}
	/*CreateWindowW(L"Static", L"Test",
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		20, 20, 300, 230,
		hWnd, NULL, NULL, NULL);
		*/
	

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
vector<string> split(const string& str, const string& delim)
{
	vector<string> tokens;
	size_t prev = 0, pos = 0;
	do
	{
		pos = str.find(delim, prev);
		if (pos == string::npos) pos = str.length();
		string token = str.substr(prev, pos - prev);
		if (!token.empty()) tokens.push_back(token);
		prev = pos + delim.length();
	} while (pos < str.length() && prev < str.length());
	return tokens;
}