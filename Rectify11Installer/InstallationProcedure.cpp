#include "InstallationProcedure.h"
#include "Navigation.h"

wchar_t path[MAX_PATH];
wchar_t cmd[1024];

std::wstring copy_list[] = {
L"%r11files%\\Mods|%ProgramData%\\Windhawk\\Engine\\Mods|NONE",
L"%r11files%\\Rectify11|%systemroot%\\Rectify11|INSTALLICONS",
L"%r11files%\\System32|%systemroot%\\System32|NONE",
L"%r11files%\\themes|%systemroot%\\resources\\themes|INSTALLTHEMES",
L"%r11files%\\wallpapers|%systemroot%\\web\\wallpaper\\rectified|INSTALLTHEMES",
L"%r11files%\\cursors|%systemroot%\\cursors|INSTALLTHEMES",
L"%r11files%\\media|%systemroot%\\media\\rectified|INSTALLTHEMES",
L"%r11files%\\SecureUxTheme\\arm64|%systemroot%\\System32|INSTALLTHEMES|ARM64"
L"%r11files%\\SecureUxTheme\\amd64|%systemroot%\\System32|INSTALLTHEMES|AMD64"
};


std::wstring install_list[] = {
L"%r11files%\\windhawk_setup_offline.exe /S|NONE",
L"%r11files%\\SymChk\\symchk.exe \"%systemroot%\\Explorer.exe\" /s SRV*%programdata%\\Windhawk\\Engine\\symbols\\*http://msdl.microsoft.com/download/symbols|NONE",
L"%r11files%\\SymChk\\symchk.exe \"%systemroot%\\system32\\Shlwapi.dll\" /s SRV*%programdata%\\Windhawk\\Engine\\symbols\\*http://msdl.microsoft.com/download/symbols|NONE"
};

std::wstring mod_list[] = {
L"%r11files%\\Regs\\resourcepatch.reg|INSTALLICONS|AMD64",
L"%r11files%\\Regs\\resourcepatchARM.reg|INSTALLICONS|ARM64",
L"%r11files%\\Regs\\sound.reg|INSTALLTHEMES",
L"%r11files%\\Regs\\soundWH.reg|INSTALLTHEMES|AMD64",
L"%r11files%\\Regs\\soundWHARM.reg|INSTALLTHEMES|ARM64",
L"%r11files%\\Regs\\SecureUX.reg|INSTALLTHEMES",
L"%r11files%\\Regs\\Light.reg|LIGHTTHEME",
L"%r11files%\\Regs\\Dark.reg|DARKTHEME",
L"%r11files%\\Regs\\fonts.reg|NONE"
};


void RunEXE(wchar_t exe[], wchar_t args[]) {

	STARTUPINFO startup_info;
	PROCESS_INFORMATION process_info;

	memset(&startup_info, 0, sizeof(STARTUPINFO));
	startup_info.cb = sizeof(STARTUPINFO);
	memset(&process_info, 0, sizeof(PROCESS_INFORMATION));

	if (exe != NULL) InstallationLogger.WriteLine(L"Path: " + std::wstring(exe));
	if (args != NULL) InstallationLogger.WriteLine(L"Arguments: " + std::wstring(args));

	BOOL rv = CreateProcess(exe, args, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startup_info, &process_info);
	if (rv) {
		InstallationLogger.WriteLine(L"Created process successfully.");
		CloseHandle(process_info.hThread);
		WaitForSingleObject(process_info.hProcess, INFINITE);
		CloseHandle(process_info.hProcess);
		InstallationLogger.WriteLine(L"Process completed successfully");
	}
	else {
		InstallationLogger.WriteLine(L"Failed to create process");
	}
}

void extractFiles() {

	StringCchPrintf(cmd, 1024, L"\"%s\\7za.exe\" x -aoa -o\"%s\" \"%s\\Files.7z\"", r11dir, r11dir, r11dir);
	InstallationLogger.WriteLine(L"Extracting files...");
	RunEXE(NULL, cmd);
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

	StringCchPrintf(cmd, 1024, L"/c echo d | xcopy \"%s\" \"%s\" /e /y", src, dest);
	StringCchPrintf(path, MAX_PATH, L"%s\\System32\\cmd.exe", windir);
	RunEXE(path, cmd);
}

void RegisterRegFile(const wchar_t* regpath) {

	StringCchPrintf(cmd, 1024, L"/c reg import \"%s\"", regpath);
	StringCchPrintf(path, MAX_PATH, L"%s\\System32\\cmd.exe", windir);

	RunEXE(path, cmd);
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

	std::wstring ws;
	InstallationLogger.WriteLine(L"Copying files...");
	for (int i = 0; i < (sizeof(copy_list) / sizeof(std::wstring)); i++) {
		ws = copy_list[i];
		std::vector<std::wstring> pathlist(ParseDelimiterString(ws));
		if (InstallFlags[pathlist[2]] == true) {
			for (int i = 0; i < pathlist.size(); i++) {
				parseEnvironmentVariablePath(pathlist[i]);
			}
			if (pathlist.size() > 3) {
				if (InstallFlags[pathlist[3]] == true) {
					InstallationLogger.WriteLine(L"Copying files \"" + pathlist[0] + L"\" to \"" + pathlist[1] + L"\" based on condition \"" + pathlist[2] + L"\"");
					MoveFileCmd(pathlist[0].c_str(), pathlist[1].c_str());
				}
			}
			else {
				InstallationLogger.WriteLine(L"Copying files \"" + pathlist[0] + L"\" to \"" + pathlist[1] + L"\" based on condition \"" + pathlist[2] + L"\"");
				MoveFileCmd(pathlist[0].c_str(), pathlist[1].c_str());
			}
		}
	}
}

void InstallPrograms() {

	std::wstring ws;
	InstallationLogger.WriteLine(L"Installing programs...");
	for (int i = 0; i < (sizeof(install_list) / sizeof(std::wstring)); i++) {
		ws = install_list[i];
		std::vector<std::wstring> progpath(ParseDelimiterString(ws));
		if (InstallFlags[progpath[1]] == true){
			parseEnvironmentVariablePath(progpath[0]);
			StringCchPrintf(cmd, 1024, L"/c \"%s\"", progpath[0].c_str());
			StringCchPrintf(path, MAX_PATH, L"%s\\System32\\cmd.exe", windir);
			RunEXE(path, cmd);
		}
	}
}

void InstallFonts() {
	StringCchPrintf(path, MAX_PATH, L"%s\\Fonts\\*", r11dir);

	HANDLE hFind;
	WIN32_FIND_DATA FindFileData;
	InstallationLogger.WriteLine(L"Installing fonts...");
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

	std::wstring ws;
	InstallationLogger.WriteLine(L"Registering windhawk modules and registry files...");
	for (int i = 0; i < (sizeof(mod_list) / sizeof(std::wstring)); i++) {
		ws = mod_list[i];
		std::vector<std::wstring> regpath(ParseDelimiterString(ws));
		if (InstallFlags[regpath[1]] == true) {
			parseEnvironmentVariablePath(regpath[0]);
			if (regpath.size() > 2) {
				if (InstallFlags[regpath[2]] == true) {
					RegisterRegFile(regpath[0].c_str());
				}
			}
			else {
				RegisterRegFile(regpath[0].c_str());
			}
		}
	}
}

void FinaliseInstall() {
	InstallationLogger.WriteLine(L"Finalising installation...");
	StringCchPrintf(path, MAX_PATH, L"%s\\System32\\cmd.exe", windir);
	wchar_t reg[] = L"/c reg add HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Rectify";
	RunEXE(path, reg);

	wchar_t icon[] = L"/c reg add HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Rectify /v DisplayIcon /t REG_SZ /d %systemroot%\\Rectify11\\Rectify11Installer.exe";
	RunEXE(path, icon);

	wchar_t name[] = L"/c reg add HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Rectify /v DisplayName /t REG_SZ /d Rectify11";
	RunEXE(path, name);

	wchar_t version[] = L"/c reg add HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Rectify /v DisplayVersion /t REG_SZ /d 3.9.6";
	RunEXE(path, version);

	wchar_t location[] = L"/c reg add HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Rectify /v InstallLocation /t REG_SZ /d %systemroot%\\Rectify11";
	RunEXE(path, location);

	wchar_t modify[] = L"/c reg add HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Rectify /v NoModify /t REG_DWORD /d 1";
	RunEXE(path, modify);

	wchar_t repair[] = L"/c reg add HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Rectify /v NoRepair /t REG_DWORD /d 1";
	RunEXE(path, repair);

	wchar_t publisher[] = L"/c reg add HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Rectify /v Publisher /t REG_SZ /d \"The Rectify11 Team\"";
	RunEXE(path, publisher);

	wchar_t uninstallstr[] = L"/c reg add HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Rectify /v UninstallString /t REG_SZ /d \"%systemroot%\\Rectify11\\Rectify11Installer.exe\"";
	RunEXE(path, uninstallstr);

	StringCchPrintf(path, MAX_PATH, L"%s\\Base.dll", currdir);
	StringCchPrintf(cmd, MAX_PATH, L"%s\\Base.dll", r11targetdir);
	CopyFile(path, cmd, false);

	StringCchPrintf(path, MAX_PATH, L"%s\\Controls.dll", currdir);
	StringCchPrintf(cmd, MAX_PATH, L"%s\\Controls.dll", r11targetdir);
	CopyFile(path, cmd, false);

	StringCchPrintf(path, MAX_PATH, L"%s\\PageRes.dll", currdir);
	StringCchPrintf(cmd, MAX_PATH, L"%s\\PageRes.dll", r11targetdir);
	CopyFile(path, cmd, false);

	StringCchPrintf(path, MAX_PATH, L"%s\\Rectify11Installer.exe", currdir);
	StringCchPrintf(cmd, MAX_PATH, L"%s\\Rectify11Installer.exe", r11targetdir);
	CopyFile(path, cmd, false);

	StringCchPrintf(path, MAX_PATH, L"%s\\Segoe_r11.ttf", currdir);
	StringCchPrintf(cmd, MAX_PATH, L"%s\\Segoe_r11.ttf", r11targetdir);
	CopyFile(path, cmd, false);
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

	wchar_t cmd2[] = L"/c del %localappdata%\\microsoft\\windows\\explorer\\*.db";
	StringCchPrintf(path, MAX_PATH, L"%s\\System32\\cmd.exe", windir);

	RunEXE(path, cmd2);

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

	InstallationLogger.WriteLine(L"Setup complete, restarting...");

	ExitWindowsEx(EWX_REBOOT | EWX_FORCE,
		SHTDN_REASON_MINOR_INSTALLATION |
		SHTDN_REASON_FLAG_PLANNED);
}