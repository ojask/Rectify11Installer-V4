#include "UninstallationProcedure.h"
#include "resource.h"
#include "framework.h"
#include "Navigation.h"

wchar_t path[MAX_PATH];
wchar_t cmd[1024];

STARTUPINFO startup_info;
PROCESS_INFORMATION process_info;

void RunEXE(wchar_t exe[], wchar_t args[]) {
	memset(&startup_info, 0, sizeof(STARTUPINFO));
	startup_info.cb = sizeof(STARTUPINFO);
	memset(&process_info, 0, sizeof(PROCESS_INFORMATION));


	BOOL rv = CreateProcess(exe, args, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startup_info, &process_info);
	if (rv) {
		CloseHandle(process_info.hThread);
		WaitForSingleObject(process_info.hProcess, INFINITE);
		CloseHandle(process_info.hProcess);
	}
}

void RemoveWHMods() {
	if (InstallFlags[L"INSTALLTHEMES"] == true) {
		wchar_t windir[MAX_PATH];
		GetEnvironmentVariable(L"systemroot", windir, MAX_PATH);

		StringCchPrintf(path, MAX_PATH, L"%s\\System32\\cmd.exe", windir);
		wchar_t args[] = L"/c reg delete HKLM\\SOFTWARE\\Windhawk\\Engine\\Mods\\logon-logoff-shutdown-sounds";
		RunEXE(path, args);

		wchar_t windir[MAX_PATH];
		GetEnvironmentVariable(L"systemroot", windir, MAX_PATH);

		StringCchPrintf(path, MAX_PATH, L"%s\\System32\\cmd.exe", windir);
		wchar_t args2[] = L"/c reg delete HKLM\\SOFTWARE\\Windhawk\\Engine\\Mods\\uxtheme-hook";
		RunEXE(path, args2);
	}
	if (InstallFlags[L"INSTALLICONS"] == true) {
		wchar_t windir[MAX_PATH];
		GetEnvironmentVariable(L"systemroot", windir, MAX_PATH);

		StringCchPrintf(path, MAX_PATH, L"%s\\System32\\cmd.exe", windir);
		wchar_t args[] = L"/c reg delete HKLM\\SOFTWARE\\Windhawk\\Engine\\Mods\\icon-resource-redirect";
		RunEXE(path, args);
	}
}

void RemoveSecureUX() {
	if (InstallFlags[L"INSTALLTHEMES"] == true) {
		wchar_t windir[MAX_PATH];
		GetEnvironmentVariable(L"systemroot", windir, MAX_PATH);

		StringCchPrintf(path, MAX_PATH, L"%s\\System32\\cmd.exe", windir);
		wchar_t args[] = L"/c reg delete \"HKLM\\Software\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\winlogon.exe\" /v VerifierDlls";
		RunEXE(path, args);

		wchar_t windir[MAX_PATH];
		GetEnvironmentVariable(L"systemroot", windir, MAX_PATH);

		StringCchPrintf(path, MAX_PATH, L"%s\\System32\\cmd.exe", windir);
		wchar_t args2[] = L"/c reg delete \"HKLM\\SOFTWARE\\WOW6432Node\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\winlogon.exe\" /v VerifierDlls";
		RunEXE(path, args2);
	}
}

