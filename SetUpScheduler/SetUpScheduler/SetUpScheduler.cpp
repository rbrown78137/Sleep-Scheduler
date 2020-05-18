// SetUpScheduler.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
// ConsoleApplication2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

/*int main()
{
	std::cout << "Hello World!\n";
}*/

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
/********************************************************************
 This sample schedules a task to start on a weekly basis.
********************************************************************/

/*_bstr_t bstrt(L"Ryan Brown");
hr = pRegInfo->put_Author(bstrt);
*/
#define _WIN32_DCOM

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <comdef.h>
#include <wincred.h>
//  Include the task header file.
#include <taskschd.h>
#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "credui.lib")

using namespace std;


int __cdecl wmain()
{
	//  ------------------------------------------------------
	//  Initialize COM.
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		printf("\nCoInitializeEx failed: %x", hr);
		return 1;
	}

	//  Set general COM security levels.
	hr = CoInitializeSecurity(
		NULL,
		-1,
		NULL,
		NULL,
		RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		0,
		NULL);

	if (FAILED(hr))
	{
		printf("\nCoInitializeSecurity failed: %x", hr);
		CoUninitialize();
		return 1;
	}

	//  ------------------------------------------------------
	//  Create a name for the task.
	LPCWSTR wszTaskName = L"Ryan Sleep Scheduler";

	//  Get the windows directory and set the path to notepad.exe.
	//wstring wstrExecutablePath = _wgetenv(L"WINDIR");

	wstring wstrExecutablePath = L"C:\\SleepScheduler\\closer.EXE";



	//  ------------------------------------------------------
	//  Create an instance of the Task Service. 
	ITaskService* pService = NULL;
	hr = CoCreateInstance(CLSID_TaskScheduler,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_ITaskService,
		(void**)& pService);
	if (FAILED(hr))
	{
		printf("Failed to create an instance of ITaskService: %x", hr);
		CoUninitialize();
		return 1;
	}

	//  Connect to the task service.
	hr = pService->Connect(_variant_t(), _variant_t(),
		_variant_t(), _variant_t());
	if (FAILED(hr))
	{
		printf("ITaskService::Connect failed: %x", hr);
		pService->Release();
		CoUninitialize();
		return 1;
	}

	//  ------------------------------------------------------
	//  Get the pointer to the root task folder.  This folder will hold the
	//  new task that is registered.
	ITaskFolder* pRootFolder = NULL;
	hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
	if (FAILED(hr))
	{
		printf("Cannot get Root Folder pointer: %x", hr);
		pService->Release();
		CoUninitialize();
		return 1;
	}

	// If the same task exists, remove it.
	pRootFolder->DeleteTask(_bstr_t(wszTaskName), 0);

	//  Create the task builder object to create the task.
	ITaskDefinition* pTask = NULL;
	hr = pService->NewTask(0, &pTask);

	pService->Release();  // COM clean up.  Pointer is no longer used.
	if (FAILED(hr))
	{
		printf("Failed to CoCreate an instance of the TaskService class: %x", hr);
		pRootFolder->Release();
		CoUninitialize();
		return 1;
	}

	//  ------------------------------------------------------
	//  Get the registration info for setting the identification.
	IRegistrationInfo* pRegInfo = NULL;
	hr = pTask->get_RegistrationInfo(&pRegInfo);
	if (FAILED(hr))
	{
		printf("\nCannot get identification pointer: %x", hr);
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return 1;
	}

	hr = pRegInfo->put_Author(_bstr_t(L"Ryan Brown"));
	pRegInfo->Release();  // COM clean up.  Pointer is no longer used.
	if (FAILED(hr))
	{
		printf("\nCannot put identification info: %x", hr);
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return 1;
	}

	//  ------------------------------------------------------
	//  Get the trigger collection to insert the daily trigger.
	ITriggerCollection* pTriggerCollection = NULL;
	hr = pTask->get_Triggers(&pTriggerCollection);
	if (FAILED(hr))
	{
		printf("\nCannot get trigger collection: %x", hr);
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return 1;
	}

	//  Add the daily trigger to the task.
	ITrigger* pTrigger = NULL;
	hr = pTriggerCollection->Create(TASK_TRIGGER_DAILY, &pTrigger);
	pTriggerCollection->Release();
	if (FAILED(hr))
	{
		printf("\nCannot create the trigger: %x", hr);
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return 1;
	}

	IDailyTrigger* pDailyTrigger = NULL;
	hr = pTrigger->QueryInterface(
		IID_IDailyTrigger, (void**)& pDailyTrigger);
	pTrigger->Release();
	if (FAILED(hr))
	{
		printf("\nQueryInterface call on IDailyTrigger failed: %x", hr);
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return 1;
	}

	hr = pDailyTrigger->put_Id(_bstr_t(L"Trigger1"));
	if (FAILED(hr))
		printf("\nCannot put trigger ID: %x", hr);

	//  Set the task to start daily at a certain time. The time 
	//  format should be YYYY-MM-DDTHH:MM:SS(+-)(timezone).
	//  For example, the start boundary below
	//  is January 1st 2005 at 12:05
	hr = pDailyTrigger->put_StartBoundary(_bstr_t(L"2020-01-01T0:0:00"));
	if (FAILED(hr))
		printf("\nCannot put start boundary: %x", hr);

	//  Set the time when the trigger is deactivated.
	hr = pDailyTrigger->put_EndBoundary(_bstr_t(L"2020-06-02T12:05:00"));
	if (FAILED(hr))
		printf("\nCannot put the end boundary: %x", hr);

	//  Define the interval for the daily trigger. An interval of 2 produces an
	//  every other day schedule
	hr = pDailyTrigger->put_DaysInterval((short)1);
	if (FAILED(hr))
	{
		printf("\nCannot put days interval: %x", hr);
		pRootFolder->Release();
		pDailyTrigger->Release();
		pTask->Release();
		CoUninitialize();
		return 1;
	}

	// Add a repetition to the trigger so that it repeats
	// five times.
	IRepetitionPattern* pRepetitionPattern = NULL;
	hr = pDailyTrigger->get_Repetition(&pRepetitionPattern);
	pDailyTrigger->Release();
	if (FAILED(hr))
	{
		printf("\nCannot get repetition pattern: %x", hr);
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return 1;
	}

	hr = pRepetitionPattern->put_Duration(_bstr_t(L"PT24H"));
	if (FAILED(hr))
	{
		printf("\nCannot put repetition duration: %x", hr);
		pRootFolder->Release();
		pRepetitionPattern->Release();
		pTask->Release();
		CoUninitialize();
		return 1;
	}

	hr = pRepetitionPattern->put_Interval(_bstr_t(L"PT1M"));
	pRepetitionPattern->Release();
	if (FAILED(hr))
	{
		printf("\nCannot put repetition interval: %x", hr);
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return 1;
	}


	//  ------------------------------------------------------
	//  Add an action to the task. This task will execute notepad.exe.     
	IActionCollection* pActionCollection = NULL;

	//  Get the task action collection pointer.
	hr = pTask->get_Actions(&pActionCollection);
	if (FAILED(hr))
	{
		printf("\nCannot get task collection pointer: %x", hr);
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return 1;
	}

	//  Create the action, specifying that it is an executable action.
	IAction* pAction = NULL;
	hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
	pActionCollection->Release();
	if (FAILED(hr))
	{
		printf("\nCannot create action: %x", hr);
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return 1;
	}

	IExecAction* pExecAction = NULL;
	hr = pAction->QueryInterface(
		IID_IExecAction, (void**)& pExecAction);
	pAction->Release();
	if (FAILED(hr))
	{
		printf("\nQueryInterface call failed for IExecAction: %x", hr);
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return 1;
	}

	//  Set the path of the executable to notepad.exe.
	hr = pExecAction->put_Path(_bstr_t(wstrExecutablePath.c_str()));
	pExecAction->Release();
	if (FAILED(hr))
	{
		printf("\nCannot put the executable path: %x", hr);
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return 1;
	}

	//  ------------------------------------------------------
	//  Securely get the user name and password. The task will
	//  be created to run with the credentials from the supplied 
	//  user name and password.
	CREDUI_INFO cui;
	TCHAR pszName[CREDUI_MAX_USERNAME_LENGTH] = TEXT("");
	TCHAR pszPwd[CREDUI_MAX_PASSWORD_LENGTH] = TEXT("");
	BOOL fSave;
	DWORD dwErr;

	cui.cbSize = sizeof(CREDUI_INFO);
	cui.hwndParent = NULL;
	//  Ensure that MessageText and CaptionText identify
	//  what credentials to use and which application requires them.
	cui.pszMessageText = TEXT("Account information for task registration:");
	cui.pszCaptionText = TEXT("Enter Account Information for Task Registration");
	cui.hbmBanner = NULL;
	fSave = FALSE;

	//  Create the UI asking for the credentials.
	dwErr = CredUIPromptForCredentials(
		&cui,                             //  CREDUI_INFO structure
		TEXT(""),                         //  Target for credentials
		NULL,                             //  Reserved
		0,                                //  Reason
		pszName,                          //  User name
		CREDUI_MAX_USERNAME_LENGTH,       //  Max number for user name
		pszPwd,                           //  Password
		CREDUI_MAX_PASSWORD_LENGTH,       //  Max number for password
		&fSave,                           //  State of save check box
		CREDUI_FLAGS_GENERIC_CREDENTIALS |  //  Flags
		CREDUI_FLAGS_ALWAYS_SHOW_UI |
		CREDUI_FLAGS_DO_NOT_PERSIST);

	if (dwErr)
	{
		cout << "Did not get credentials." << endl;
		CoUninitialize();
		return 1;
	}

	//  ------------------------------------------------------
	//  Save the task in the root folder.
	IRegisteredTask* pRegisteredTask = NULL;
	hr = pRootFolder->RegisterTaskDefinition(
		_bstr_t(wszTaskName),
		pTask,
		TASK_CREATE_OR_UPDATE,
		_variant_t(_bstr_t(pszName)),
		_variant_t(_bstr_t(pszPwd)),
		TASK_LOGON_PASSWORD,
		_variant_t(L""),
		&pRegisteredTask);
	if (FAILED(hr))
	{
		printf("\nError saving the Task : %x", hr);
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		SecureZeroMemory(pszName, sizeof(pszName));
		SecureZeroMemory(pszPwd, sizeof(pszPwd));
		return 1;
	}

	printf("\n Success! Task successfully registered. ");

	//  Clean up
	pRootFolder->Release();
	pTask->Release();
	pRegisteredTask->Release();
	CoUninitialize();
	SecureZeroMemory(pszName, sizeof(pszName));
	SecureZeroMemory(pszPwd, sizeof(pszPwd));
	return 0;
}