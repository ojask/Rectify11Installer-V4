#include "UninstallationProcedure.h"
#include "InstallationProcedure.h"
#include "resource.h"
#include "framework.h"
#include "Navigation.h"

wchar_t exepath[MAX_PATH];

void RemoveWHMods() {
	if (InstallFlags[L"INSTALLTHEMES"] == true) {

		InstallationLogger.WriteLine(L"Uninstalling sound hook...");

		StringCchPrintf(exepath, MAX_PATH, L"%s\\System32\\cmd.exe", windir);
		wchar_t args[] = L"/c reg delete HKLM\\SOFTWARE\\Windhawk\\Engine\\Mods\\logon-logoff-shutdown-sounds /f";
		RunEXE(exepath, args);

		InstallationLogger.WriteLine(L"Uninstalling uxtheme hook...");

		StringCchPrintf(exepath, MAX_PATH, L"%s\\System32\\cmd.exe", windir);
		wchar_t args2[] = L"/c reg delete HKLM\\SOFTWARE\\Windhawk\\Engine\\Mods\\uxtheme-hook /f";
		RunEXE(exepath, args2);
	}
	if (InstallFlags[L"INSTALLICONS"] == true) {

		InstallationLogger.WriteLine(L"Uninstalling resource redirect...");

		StringCchPrintf(exepath, MAX_PATH, L"%s\\System32\\cmd.exe", windir);
		wchar_t args[] = L"/c reg delete HKLM\\SOFTWARE\\Windhawk\\Engine\\Mods\\icon-resource-redirect /f";
		RunEXE(exepath, args);
	}
}

void RemoveSecureUX() {
	if (InstallFlags[L"INSTALLTHEMES"] == true) {

		InstallationLogger.WriteLine(L"Uninstalling secureuxtheme...");

		StringCchPrintf(exepath, MAX_PATH, L"%s\\System32\\cmd.exe", windir);
		wchar_t args[] = L"/c reg delete \"HKLM\\Software\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\winlogon.exe\" /f /v VerifierDlls";
		RunEXE(exepath, args);

		StringCchPrintf(exepath, MAX_PATH, L"%s\\System32\\cmd.exe", windir);
		wchar_t args2[] = L"/c reg delete \"HKLM\\SOFTWARE\\WOW6432Node\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\winlogon.exe\" /f /v VerifierDlls";
		RunEXE(exepath, args2);

	}
}

void FinaliseUninstall() {
	if (InstallFlags[L"INSTALLTHEMES"] == true) {

		InstallationLogger.WriteLine(L"Finishing uninstallation...");

		if (InstallFlags[L"LIGHTTHEME"] == true) {
			StringCchPrintf(exepath, MAX_PATH, L"%s\\System32\\cmd.exe", windir);
			wchar_t args[] = L"/c reg add HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce /v ApplyTheme /t REG_SZ /d \"cmd.exe /c %systemroot%\\resources\\themes\\aero.theme\"";
			RunEXE(exepath, args);
		}
		if (InstallFlags[L"DARKTHEME"] == true) {
			StringCchPrintf(exepath, MAX_PATH, L"%s\\System32\\cmd.exe", windir);
			wchar_t args[] = L"/c reg add HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce /v ApplyTheme /t REG_SZ /d \"cmd.exe /c %systemroot%\\resources\\themes\\dark.theme\"";
			RunEXE(exepath, args);
		}

	}
	StringCchPrintf(exepath, MAX_PATH, L"%s\\System32\\cmd.exe", windir);
	wchar_t args[] = L"/c reg delete HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Rectify /f";
	RunEXE(exepath, args);
}

