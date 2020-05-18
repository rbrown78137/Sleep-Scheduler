#include<windows.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <sstream> 
#include <tlhelp32.h>
#include <vector>
//#pragma warning(disable : 4996)
#define TOTAL_BYTES_READ 1024
#define OFFSET_BYTES 1024

using namespace std;
bool hasClosedAnApp = false;
bool hasWarnedAnApp = false;
BOOL CreateRegistryKey(HKEY hKeyParent, PWCHAR subkey);
BOOL writeStringInRegistry(HKEY hKeyParent, PWCHAR subkey, PWCHAR valueName, PWCHAR strData);
BOOL readStringFromRegistry(HKEY hKeyParent, PWCHAR subkey, PWCHAR valueName, PWCHAR* readData);
PWCHAR convertToPWCHAR(string stringToConvert);
string readString(string location);
bool insertString(string location, string key);
void getHourAndMinute(int& hour, int& minute, string input);
vector<string> split(const string& str, const string& delim);
bool IsProcessRunning(string processNameString);
void getLimitCurrentDay(int day, int& hour, int& minute);
struct AppUsage {
	AppUsage(int timeUsedMinutes, string appName) {
		this->timeUsedMinutes = timeUsedMinutes;
		this->appName = appName;
	}
	int timeUsedMinutes;
	string appName;
};
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
//void getMode(bool& reminderPeriod, bool& pastTime);
HINSTANCE hInst;
static TCHAR szWindowClass[] = _T("win32app");
static TCHAR szTitle[] = _T("Timer Warning");
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow) {
	

	//general initialziation used by all parts
	CreateRegistryKey(HKEY_CURRENT_USER, (PWCHAR)L"SleepScheduler");
	time_t currentTime = time(0);
	tm now;
	localtime_s(&now, &currentTime);
	string currentDayOfMonth = (to_string(now.tm_mday));
	string dayOnLastRun = readString("dayLastRun");
	insertString("dayLastRun", currentDayOfMonth);


	//creates array with time limits
	string appsToLimit = readString("appLimit");
	vector<string> currentAppToLimitStrings = split(appsToLimit, "!");
	vector<AppUsage> appLimitTimes;
	for (string appUsageString : currentAppToLimitStrings) {
		vector<string> appUsageUnit = split(appUsageString, "@");
		string appString = appUsageUnit.at(0);
		stringstream stream(appUsageUnit.at(1));
		int minutesUsed;
		stream >> minutesUsed;
		appLimitTimes.push_back(AppUsage(minutesUsed, appString));
	}

	//creates array that holds current app usage stats that will be added to
	string appUsageDayString = readString("appUsageDay");
	vector<string> appUsageDayStringArray = split(appUsageDayString, "!");
	vector<AppUsage> AppUsageDay;
	for (string appUsageDayString : appUsageDayStringArray) {
		vector<string> appUsageUnit = split(appUsageDayString, "@");
		string appString = appUsageUnit.at(0);
		stringstream stream(appUsageUnit.at(1));
		int minutesUsed;
		stream >> minutesUsed;
		if (dayOnLastRun.size() <= 0 || dayOnLastRun != currentDayOfMonth) {
			AppUsageDay.push_back(AppUsage(0, appString));
		}
		else {
			AppUsageDay.push_back(AppUsage(minutesUsed, appString));
		}
	}

	//initializes weekly vector
	vector<AppUsage> AppUsageWeek;
	string appUsageOfWeekString = readString("appUsageWeek");
	vector<string> appUsageOfWeekUnits = split(appUsageOfWeekString, "!");
	for (string appPair : appUsageOfWeekUnits) {
		vector<string> appUsageUnit = split(appPair, "@");
		string appString = appUsageUnit.at(0);
		stringstream stream(appUsageUnit.at(1));
		int minutesUsed;
		stream >> minutesUsed;
		if (dayOnLastRun.size() <= 0 || ((dayOnLastRun != currentDayOfMonth) && (now.tm_wday == 0))) {
			AppUsageWeek.push_back(AppUsage(0, appString));
		}
		else {
			AppUsageWeek.push_back(AppUsage(minutesUsed, appString));
		}
	}

	//adds time to currently monitored processes
	string appsToMonitor = readString("appMonitor");
	vector<string> appNamesToMonitor = split(appsToMonitor, "!");
	for (string appName : appNamesToMonitor) {
		bool isRunning = IsProcessRunning(appName);
		bool containsDay = false;
		bool containsWeek = false;
		if (isRunning) {
			for (int i = 0; i < AppUsageDay.size();i++) {
				if (appName == AppUsageDay[i].appName) {
					containsDay = true;
					AppUsageDay[i].timeUsedMinutes = AppUsageDay[i].timeUsedMinutes + 1;
				}
			}
			for (int i = 0; i < AppUsageWeek.size(); i++) {
				if (appName == AppUsageWeek[i].appName) {
					containsWeek = true;
					AppUsageWeek[i].timeUsedMinutes = AppUsageWeek[i].timeUsedMinutes + 1;
				}
			}
			if (!containsDay) {
				AppUsageDay.push_back(AppUsage(1, appName));
			}
			if (!containsWeek) {
				AppUsageWeek.push_back(AppUsage(1, appName));
			}
		}
	}

	// saves current app usage for day
	vector<string> individualDayStringUnits;
	for (AppUsage appUsageUnit : AppUsageDay) {
		individualDayStringUnits.push_back(((appUsageUnit.appName + "@") + to_string(appUsageUnit.timeUsedMinutes)));
	}
	string sumStringDay = "";
	for (string individualApp : individualDayStringUnits) {
		sumStringDay += ("!" + individualApp);
	}
	insertString("appUsageDay", sumStringDay);

	//saves current app usage for week
	vector<string> individualWeekStringUnits;
	for (AppUsage appUsageUnit : AppUsageWeek) {
		individualWeekStringUnits.push_back(((appUsageUnit.appName + "@") + to_string(appUsageUnit.timeUsedMinutes)));
	}
	string sumStringWeek = "";
	for (string individualApp : individualWeekStringUnits) {
		sumStringWeek += ("!" + individualApp);
	}
	insertString("appUsageWeek", sumStringWeek);

	//finds regular limits on day
	string global = readString("global");
	int dayHourLimit =-1, dayMinuteLimit =-1,globalHourLimit=-1,globalMinuteLimit=-1;
	getHourAndMinute(globalHourLimit, globalMinuteLimit, global);
	getLimitCurrentDay(now.tm_wday, dayHourLimit, dayMinuteLimit);

	//looks for specific day limits
	vector<string> specificDayExceptionStrings = split(readString("specificDate"),"!");
	string dateToRemove = "";
	for (string specificDayString : specificDayExceptionStrings) {
			vector<string> specificDay = split(specificDayString, "@");
			string date = specificDay.at(0);
			string time = specificDay.at(1);
			stringstream stream(date);
			int month, day, year;
			char ignoreChar;
			stream >> month;
			stream >> ignoreChar;
			stream >> day;
			stream >> ignoreChar;
			stream >> year;
			if (day == now.tm_mday && month-1 == now.tm_mon && now.tm_year == year-1900) {
				getHourAndMinute(dayHourLimit, dayMinuteLimit, time);
			}
			else if (day == now.tm_mday - 1 && month - 1 == now.tm_mon && now.tm_year == year - 1900) {
				dateToRemove = specificDayString;
			}
	}
	//removes old day from specificDate registry value
	if (dateToRemove.size() > 0) {
		string stringToPutBackInRegistry = "";
		for (string specificDayExceptionString : specificDayExceptionStrings) {
			if (specificDayExceptionString != dateToRemove) {
				stringToPutBackInRegistry += specificDayExceptionString;
			}
		}
		insertString("specificDate", stringToPutBackInRegistry);
	}
	// checks for days not to check limit
	bool checkDay = true;
	vector<string> daysNotToCheck = split(readString("doNotCheck"), "!");
	string dateToRemoveForDoNotCheck = "";
	for (string dayNotChecked : daysNotToCheck) {
		stringstream stream(dayNotChecked);
		int month, day, year;
		char ignoreChar;
		stream >> month;
		stream >> ignoreChar;
		stream >> day;
		stream >> ignoreChar;
		stream >> year;
		if (day == now.tm_mday && month-1 == now.tm_mon && now.tm_year == year-1900) {
			checkDay = false;
		}
		else if (day == now.tm_mday - 1 && month-1 == now.tm_mon && now.tm_year == year-1900) {
			dateToRemoveForDoNotCheck = dayNotChecked;
		}
	}
	//removes old day from doNotCheck registry value
	if (dateToRemoveForDoNotCheck.size() > 0) {
		string registryValue = "";
		for (string day : daysNotToCheck) {
			if (day != dateToRemoveForDoNotCheck) {
				registryValue += day;
			}
		}
		insertString("doNotCheck", registryValue);
	}
	//checks to see if time is over limit and gets list of programs to close or send warnings for
	if (checkDay) {
		vector<string> appsToClose;
		vector<string> appWarnings;
		int hourLimit, minuteLimit;
		if (dayHourLimit == -1) {
			hourLimit = globalHourLimit;
			minuteLimit = globalMinuteLimit;
		}
		else {
			hourLimit = dayHourLimit;
			minuteLimit = dayMinuteLimit;
		}
		if (hourLimit >= 0 && minuteLimit >= 0) {\
			bool isClose = false;
			bool isWarning = false;
			int currentHour = now.tm_hour;
			int currentMinute = now.tm_min;
			if (currentHour < 7) {
				isClose = true;
				isWarning = false;
			}
			else if (currentHour * 60 + currentMinute >= hourLimit * 60 + minuteLimit) {
				isClose = true;
				isWarning = false;
			}
			else if (currentHour * 60 + currentMinute + 5 == hourLimit * 60 + minuteLimit) {
				isWarning = true;
				isClose = false;
			}
			if (isClose) {
				for (string appName: appNamesToMonitor) {
					appsToClose.push_back(appName);
				}
			}
			if (isWarning) {
				for (string appName : appNamesToMonitor) {
					appWarnings.push_back(appName);
				}
			}
			if (!isClose) {
				for (AppUsage limitedApp : appLimitTimes) {
					for (AppUsage appInDailyUsage : AppUsageDay) {
						if (limitedApp.appName == appInDailyUsage.appName && limitedApp.timeUsedMinutes < appInDailyUsage.timeUsedMinutes) {
							appsToClose.push_back(limitedApp.appName);
						}
					}
				}
			}
			if (!isWarning) {
				for (AppUsage limitedApp : appLimitTimes) {
					for (AppUsage appInDailyUsage : AppUsageDay) {
						if (limitedApp.appName == appInDailyUsage.appName && limitedApp.timeUsedMinutes - 5 == appInDailyUsage.timeUsedMinutes) {
							appWarnings.push_back(limitedApp.appName);
						}
					}
				}
			}
		}
		for (string app : appsToClose) {
			if (IsProcessRunning(app)) {
				hasClosedAnApp = true;
				string command = "taskkill /f /im " + app;
				const char* commandToChar = command.c_str();
				system(commandToChar);
			}
		}
		if (appWarnings.size() > 0) {
			hasWarnedAnApp = true;
		}
	}

		if(hasWarnedAnApp || hasClosedAnApp){
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
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
		hInst = hInstance;
		
		HWND hWnd = CreateWindow(
			szWindowClass,
			szTitle,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			300, 300,
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
		UpdateWindow(hWnd);
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

	LRESULT CALLBACK WndProc(
		HWND   hWnd,
		UINT   uMsg,
		WPARAM wParam,
		LPARAM lParam) {
		PAINTSTRUCT ps;
		HDC hdc;

		switch (uMsg)
		{
		case WM_CREATE:
			MessageBeep(MB_ICONSTOP);
			if (hasClosedAnApp) {
				MessageBoxW(NULL, L"Apps were closed", L"Time Reminder", MB_OK);
			}
			else if (hasWarnedAnApp) {
				MessageBoxW(NULL, L"You have less than 5 on some or all apps", L"Time Reminder", MB_OK);
			}
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// End application-specific layout section.
			
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
			break;
		}

		return 0;
	}
	
	bool IsProcessRunning(string processNameString)
	{
		std::wstring wideString = std::wstring(processNameString.begin(), processNameString.end());
		const wchar_t* processName = wideString.c_str();
		bool exists = false;
		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (Process32First(snapshot, &entry))
			while (Process32Next(snapshot, &entry))
				if (!_wcsicmp(entry.szExeFile, processName))
					exists = true;

		CloseHandle(snapshot);
		return exists;
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
	void getHourAndMinute(int& hour, int& minute, string input) {
		stringstream stream;
		stream.str(input);
		if (input.size() > 0) {
			char colon;
			stream >> hour;
			stream >> colon;
			stream >> minute;
		}
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
	void getLimitCurrentDay(int day, int& hour, int& minute) {
		if (day == 0) {
			string dayString = readString("sunday");
			getHourAndMinute(hour, minute, dayString);
		}
		else if (day == 1) {
			string dayString = readString("monday");
			getHourAndMinute(hour, minute, dayString);
		}
		else if (day == 2) {
			string dayString = readString("tuesday");
			getHourAndMinute(hour, minute, dayString);
		}
		else if (day == 3) {
			string dayString = readString("wednesday");
			getHourAndMinute(hour, minute, dayString);
		}
		else if (day == 4) {
			string dayString = readString("thursday");
			getHourAndMinute(hour, minute, dayString);
		}
		else if (day == 5) {
			string dayString = readString("friday");
			getHourAndMinute(hour, minute, dayString);
		}
		else if (day == 6) {
			string dayString = readString("saturday");
			getHourAndMinute(hour, minute, dayString);
		}
	}