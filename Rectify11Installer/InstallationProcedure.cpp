#include "InstallationProcedure.h"
#include "Navigation.h"

wchar_t path[MAX_PATH];
wchar_t cmd[1024];

STARTUPINFO startup_info;
PROCESS_INFORMATION process_info;

void extractFIles() {
	memset(&startup_info, 0, sizeof(STARTUPINFO));
	startup_info.cb = sizeof(STARTUPINFO);
	memset(&process_info, 0, sizeof(PROCESS_INFORMATION));

	StringCchPrintf(cmd, 1024, L"\"%s\\7za.exe\" x -aoa -o\"%s\" \"%s\\Files.7z\"", r11dir, r11dir, r11dir);

    BOOL rv = CreateProcess(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startup_info, &process_info);
	if (rv) {
		CloseHandle(process_info.hThread);
		WaitForSingleObject(process_info.hProcess, INFINITE);
		CloseHandle(process_info.hProcess);
	}
}

void parseEnvironmentVariablePath(std::wstring& path) {
	int f = path.find(L'%');
	if (f != std::wstring::npos) {
		int t = path.find(L'%', f + 1);
		if (f != std::wstring::npos) {
			wchar_t tmp[MAX_PATH];
			std::wstring variableFromPath = path.substr(f + 1, t - 1);

			if (wcscmp(variableFromPath.c_str(), L"r11files") == 0) {
				path.replace(f, t + 1, r11dir);
			}
			else {
				GetEnvironmentVariable(variableFromPath.c_str(), tmp, MAX_PATH);
				path.replace(f, t + 1, tmp);
			}
		}
	}
}

void MoveFileCmd(const wchar_t* src, const wchar_t* dest) {
	memset(&startup_info, 0, sizeof(STARTUPINFO));
	startup_info.cb = sizeof(STARTUPINFO);
	memset(&process_info, 0, sizeof(PROCESS_INFORMATION));

	wchar_t windir[MAX_PATH];
	GetEnvironmentVariable(L"systemroot", windir, MAX_PATH);

	StringCchPrintf(cmd, 1024, L"/c echo d | xcopy \"%s\" \"%s\" /e /y", src, dest);
	StringCchPrintf(path, MAX_PATH, L"%s\\System32\\cmd.exe", windir);

	BOOL rv = CreateProcess(path, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startup_info, &process_info);
	if (rv) {
		CloseHandle(process_info.hThread);
		WaitForSingleObject(process_info.hProcess, INFINITE);
		CloseHandle(process_info.hProcess);
	}
}

void RegisterRegFile(const wchar_t* regpath) {
	memset(&startup_info, 0, sizeof(STARTUPINFO));
	startup_info.cb = sizeof(STARTUPINFO);
	memset(&process_info, 0, sizeof(PROCESS_INFORMATION));

	wchar_t windir[MAX_PATH];
	GetEnvironmentVariable(L"systemroot", windir, MAX_PATH);

	StringCchPrintf(cmd, 1024, L"/c reg import \"%s\"", regpath);
	StringCchPrintf(path, MAX_PATH, L"%s\\System32\\cmd.exe", windir);

	BOOL rv = CreateProcess(path, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startup_info, &process_info);
	if (rv) {
		CloseHandle(process_info.hThread);
		WaitForSingleObject(process_info.hProcess, INFINITE);
		CloseHandle(process_info.hProcess);
	}
}

std::vector<std::wstring> ParseDelimiterString(std::wstring ws) {
	std::vector<std::wstring> strlist;
	int del = 0;
	while ((del = ws.find('|')) != std::wstring::npos) {
		strlist.push_back(ws.substr(0, del));
		ws.erase(ws.begin(), ws.begin() + del + 1);
	}
	strlist.push_back(ws);
	return strlist;
}

void MoveFilesToTarget() {
	StringCchPrintf(path, MAX_PATH, L"%s\\Copy.list", r11dir);
	std::wfstream fs(path);
	std::wstring ws;
	while (std::getline(fs, ws)) {
		std::vector<std::wstring> pathlist(ParseDelimiterString(ws));
		if (InstallFlags[pathlist[2]] == true) {
			for (int i = 0; i < pathlist.size(); i++) {
				parseEnvironmentVariablePath(pathlist[i]);
			}
			MoveFileCmd(pathlist[0].c_str(), pathlist[1].c_str());
		}
	}
	fs.close();
}

void InstallPrograms() {
	memset(&startup_info, 0, sizeof(STARTUPINFO));
	startup_info.cb = sizeof(STARTUPINFO);
	memset(&process_info, 0, sizeof(PROCESS_INFORMATION));

	wchar_t windir[MAX_PATH];
	GetEnvironmentVariable(L"systemroot", windir, MAX_PATH);

	StringCchPrintf(path, MAX_PATH, L"%s\\Install.list", r11dir);
	std::wfstream fs(path);
	std::wstring ws;
	while (std::getline(fs, ws)) {
		std::vector<std::wstring> progpath(ParseDelimiterString(ws));
		if (InstallFlags[progpath[1]] == true) {
			parseEnvironmentVariablePath(progpath[0]);
			StringCchPrintf(cmd, 1024, L"/c \"%s\"", progpath[0].c_str());
			StringCchPrintf(path, MAX_PATH, L"%s\\System32\\cmd.exe", windir);
			BOOL rv = CreateProcess(path, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startup_info, &process_info);
			if (rv) {
				CloseHandle(process_info.hThread);
				WaitForSingleObject(process_info.hProcess, INFINITE);
				CloseHandle(process_info.hProcess);
			}
		}
	}
	fs.close();
}

void InstallFonts() {
	StringCchPrintf(path, MAX_PATH, L"%s\\Fonts\\*", r11dir);

	HANDLE hFind;
	WIN32_FIND_DATA FindFileData;

	wchar_t windir[MAX_PATH];
	GetEnvironmentVariable(L"systemroot", windir, MAX_PATH);

	hFind = FindFirstFile(path, &FindFileData);
	do {
		if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
			StringCchPrintf(path, MAX_PATH, L"%s\\Fonts\\%s", r11dir, FindFileData.cFileName);
			StringCchPrintf(cmd, MAX_PATH, L"%s\\Fonts\\%s", windir, FindFileData.cFileName);

			CopyFile(path, cmd, false);
			AddFontResource(cmd);
		}
	} while (FindNextFileW(hFind, &FindFileData));
}

void RegisterWHMods() {
	StringCchPrintf(path, MAX_PATH, L"%s\\Mods.list", r11dir);
	std::wfstream fs(path);
	std::wstring ws;
	while (std::getline(fs, ws)) {
		std::vector<std::wstring> regpath(ParseDelimiterString(ws));
		if (InstallFlags[regpath[1]] == true) {
			parseEnvironmentVariablePath(regpath[0]);
			RegisterRegFile(regpath[0].c_str());
		}
	}
	fs.close();
}

void SetupComplete() {
	DWORD lpidProcess[1024] = {};
	DWORD cbNeeded = NULL;
	HANDLE hExplorer = NULL;
	EnumProcesses(lpidProcess, sizeof(lpidProcess), &cbNeeded);

	wchar_t buffer[MAX_PATH];
	for (int i = 0; i < cbNeeded / sizeof(DWORD); i++) {
		hExplorer = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_TERMINATE, true, lpidProcess[i]);
		GetProcessImageFileName(hExplorer, buffer, MAX_PATH);
		if (wcsstr(buffer, L"explorer.exe"))break;
	}
	if (hExplorer) {
		TerminateProcess(hExplorer, 1);
		CloseHandle(hExplorer);
	}

	memset(&startup_info, 0, sizeof(STARTUPINFO));
	startup_info.cb = sizeof(STARTUPINFO);
	memset(&process_info, 0, sizeof(PROCESS_INFORMATION));

	wchar_t windir[MAX_PATH];
	GetEnvironmentVariable(L"systemroot", windir, MAX_PATH);

	wchar_t cmd2[] = L"/c del %localappdata%\\microsoft\\windows\\explorer\\*.db";
	StringCchPrintf(path, MAX_PATH, L"%s\\System32\\cmd.exe", windir);

	BOOL rv = CreateProcess(path, cmd2, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startup_info, &process_info);
	if (rv) {
		CloseHandle(process_info.hThread);
		WaitForSingleObject(process_info.hProcess, INFINITE);
		CloseHandle(process_info.hProcess);
	}
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
		&tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
		(PTOKEN_PRIVILEGES)NULL, 0);

	ExitWindowsEx(EWX_REBOOT | EWX_FORCE,
		SHTDN_REASON_MINOR_INSTALLATION |
		SHTDN_REASON_FLAG_PLANNED);
}