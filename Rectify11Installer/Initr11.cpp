#include "framework.h"
#include "Initr11.h"
#include "Rectify11Installer.h"
#include "Navigation.h"
#include "resource.h"
#include "DirectUI/DirectUI.h"

using namespace std;
using namespace DirectUI;

HRESULT err = 0;

bool CheckVer(int build) {
    OSVERSIONINFOEXA vInfo;
    vInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
    vInfo.dwBuildNumber =  build;
    DWORDLONG dwlConditionMask = 0;
    int op = VER_GREATER_EQUAL;
    VER_SET_CONDITION(dwlConditionMask, VER_BUILDNUMBER, op);
    if (!VerifyVersionInfoA(&vInfo, VER_BUILDNUMBER, dwlConditionMask)) return false;
    return true;
}


bool GetUserAppMode() {
    int i = 0;
    LPCWSTR path = L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
    HKEY hKey;
    DWORD lResult = RegOpenKeyEx(HKEY_CURRENT_USER, path, 0, KEY_READ, &hKey);
    if (lResult == ERROR_SUCCESS)
    {
        DWORD dwSize = NULL;
        lResult = RegGetValue(hKey, NULL, L"AppsUseLightTheme", RRF_RT_DWORD, NULL, NULL, &dwSize);
        if (lResult == ERROR_SUCCESS && dwSize != NULL)
        {
            DWORD* dwValue = (DWORD*)malloc(dwSize);
            lResult = RegGetValue(hKey, NULL, L"AppsUseLightTheme", RRF_RT_DWORD, NULL, dwValue, &dwSize);
            i = *dwValue;
            free(dwValue);
        }
        RegCloseKey(hKey);
    }
    if (i == 1) return true;
    return false;
}


int InitPages() {
    for (int i = 1; i < MAXPAGE; i++) {

        Element* page;
        MainLogger.WriteLine(L"Setting XML parser from resource: " + to_wstring(IDR_UIFILE1 + i));
        err = pParser->SetXMLFromResource(IDR_UIFILE1 + i, hinst, hinst);
        if (FAILED(err)) {
            MainLogger.WriteLine(L"failed to set XML parser from resource: " + to_wstring(IDR_UIFILE1 + i), err);
            return err;
        }

        std::wstring n = L"page" + std::to_wstring(i);
        MainLogger.WriteLine(L"Finding Element with ID: " + n);
        Element* pContainer = pMain->FindDescendent(StrToID((UCString)n.c_str()));
        if (!pContainer) {
            err = -30;
            MainLogger.WriteLine(L"Element with ID: " + n + L" not found", err);
            return err;
        }

        MainLogger.WriteLine(L"Creating PageMain Element inside " + n);
        err = pParser->CreateElement((UCString)L"PageMain", pContainer, NULL, NULL, &page);
        if (FAILED(err)) {
            MainLogger.WriteLine(L"Cannot create PageMain Element inside " + n, err);
        }

        std::wstring anim = L"animator" + std::to_wstring(i);
        MainLogger.WriteLine(L"Finding Element with ID: " + anim);
        Element* pAnimator = pMain->FindDescendent(StrToID((UCString)anim.c_str()));
        if (FAILED(err)) {
            err = -31;
            MainLogger.WriteLine(L"Element with ID: " + anim + L" not found", err);
            return err;
        }

        MainLogger.WriteLine(L"Changing " + n + L"'s visibility");
        err = pContainer->SetVisible(false);
        if (FAILED(err)) {
            err = -32;
            MainLogger.WriteLine(L"Failed to change " + n + L"'s visibility", err);
            return err;
        }

        MainLogger.WriteLine(L"Adding page " + to_wstring(i) + L" to page list");
        animArr.push_back(pAnimator);
        pageArr.push_back(pContainer);

        MainLogger.WriteLine(L"\n");
    }
    MainLogger.WriteLine(L"InitPages() completed", err);
    return err;
}


void InitControls() {

    progressbar = pMain->FindDescendent(StrToID((UCString)L"progress"));
    Nxt = (TouchButton*)pMain->FindDescendent(StrToID((UCString)L"next"));
    Bck = (TouchButton*)pMain->FindDescendent(StrToID((UCString)L"back"));

    Nxt->AddListener(new EventListener(NavNext));
    Bck->AddListener(new EventListener(NavBack));

    browsebtn = (TouchButton*)pMain->FindDescendent(StrToID((UCString)L"browsebtn"));

    TouchButton* SYS = (TouchButton*)pMain->FindDescendent(StrToID((UCString)L"PatchSystem"));
    TouchButton* ISO = (TouchButton*)pMain->FindDescendent(StrToID((UCString)L"PatchISO"));

    SYS->AddListener(new EventListener(NavSYS));
    ISO->AddListener(new EventListener(NavISO));

    waitAnimation = (RichText*)pMain->FindDescendent(StrToID((UCString)L"WaitAnimation"));
    restartWaitAnimation = (RichText*)pMain->FindDescendent(StrToID((UCString)L"RestartWaitAnimation"));

    TouchButton* credits = (TouchButton*)pMain->FindDescendent(StrToID((UCString)L"credits"));

    TouchButton* Full = (TouchButton*)pMain->FindDescendent(StrToID((UCString)L"Full"));
    TouchButton* None = (TouchButton*)pMain->FindDescendent(StrToID((UCString)L"None"));

    progressmeter = (RichText*)pMain->FindDescendent(StrToID((UCString)L"R11Progress"));
    Countdown = (RichText*)pMain->FindDescendent(StrToID((UCString)L"RestartCountdown"));

    void (*fArr[])(Element*, Event*) = { HandleThemesChk, HandleAsdfChk, HandleShellChk, HandleExplorerChk};
    TouchCheckBox* tch;
    std::wstring ws;
    for (int i = 0; i < 4; i++) {
        ws = L"chk" + std::to_wstring(i + 1);
        tch = (TouchCheckBox*)pMain->FindDescendent(StrToID((UCString)ws.c_str()));
        tch->AddListener(new EventListener(fArr[i]));
    }

    Full->AddListener(new EventListener(NavFull));
    None->AddListener(new EventListener(NavNone));

    credits->AddListener(new EventListener(OpenCredits));
}


int ChangeSheet() {
    MainLogger.WriteLine(L"Setting parser object to resource: " + to_wstring(IDR_UIFILE1));
    err = pParser->SetXMLFromResource((UINT)IDR_UIFILE1, hinst, hinst);
    if (FAILED(err)) {
        MainLogger.WriteLine(L"failed in setting parser object to resource: " + to_wstring(IDR_UIFILE1), err);
        return err;
    }
    Value* v{};
    StyleSheet* s{};

    if (GetUserAppMode()) {
        MainLogger.WriteLine(L"Light mode detected\nGetting Stylesheet information...");
        err = pParser->GetSheet((UCString)L"S", &v);
        InstallFlags[L"LIGHTTHEME"] = true;
        InstallFlags[L"DARKTHEME"] = false;
    }
    else {
        MainLogger.WriteLine(L"Dark mode detected\nGetting Stylesheet information...");
        err = pParser->GetSheet((UCString)L"Sdark", &v);
        InstallFlags[L"LIGHTTHEME"] = false;
        InstallFlags[L"DARKTHEME"] = true;
    }
    if (FAILED(err)) {
        MainLogger.WriteLine(L"Failed to get Stylesheet information.", err);
        return err;
    }
    MainLogger.WriteLine(L"Creating Stylesheet");
    s = v->GetStyleSheet();
    if (!s) {
        err = -34;
        MainLogger.WriteLine(L"Failed to create Stylesheet.", err);
        return err;
    }
    MainLogger.WriteLine(L"Applying stylesheet");
    err = pMain->SetSheet(s);
    if (FAILED(err)) {
        MainLogger.WriteLine(L"Failed to apply stylesheet", err);
        return err;
    }
    MainLogger.WriteLine(L"ChangeSheet() completed", err);
    return err;
}

int InitInstaller() {
    if (!CheckVer(21343)) {
        err = -21343;
        TaskDialog(NULL, NULL, L"no", L"stop", L"unsupported on this windows version", TDCBF_OK_BUTTON, TD_ERROR_ICON, NULL);
        MainLogger.WriteLine(L"This Windows Build is not supported. Windows 10 Build 21343 and above is required.", err);
        return err;
    }
    if (!InternetCheckConnection(L"https://8.8.8.8/", FLAG_ICC_FORCE_CONNECTION, 0))
    {
        err = -69;
        TaskDialog(NULL, NULL, L"no", L"stop", L"internet needed", TDCBF_OK_BUTTON, TD_ERROR_ICON, NULL);
        MainLogger.WriteLine(L"Rectify11 required active internet connection to be installed.", err);
        return err;
    }

    MainLogger.WriteLine(L"Initializing pages...\n\n\n");
    err = InitPages();
    if (FAILED(err)) {
        err = -33;
        MainLogger.WriteLine(L"One or more pages failed to initialize correctly.\n", err);
        return err;
    }
    InitControls();
    err = ChangeSheet();
    if (FAILED(err)) {
        MainLogger.WriteLine(L"Failed to change stylesheet.", err);
        return err;
    }
    return err;
}

