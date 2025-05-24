// Rectify11Installer.cpp : Defines the entry point for the application.
//
#include "framework.h"
#include "Rectify11Installer.h"
#include "resource.h"

#include "DirectUI/DirectUI.h"

#include "Initr11.h"
#include "InitUninst.h"
#include "Navigation.h"
#include "InstallerEngine.h"
#include "Logger.h"
#include "MiscWindow.h"

using namespace DirectUI;
using namespace std;

TouchButton* defenderbtn;
TouchButton* browsebtn;

RichText* waitAnimation;
RichText* restartWaitAnimation;

RichText* progressmeter;
RichText* Countdown;

DUIXmlParser* pParser;
HINSTANCE hinst;

HWNDElement* HElement;

Value* V;
WNDPROC WndProc;

NativeHWNDHost* pwnd;
Element* pMain;

vector<Element*> pageArr;
vector<Element*> animArr;

Element* progressbar;
TouchButton* Nxt;
TouchButton* Bck;

TouchButton* notes;
TouchButton* credits;

Logger MainLogger;
Logger NavLogger;
Logger InstallationLogger;

bool uninstall = false;

wchar_t currdir[MAX_PATH] = {};
wchar_t r11dir[MAX_PATH] = {};
wchar_t r11targetdir[MAX_PATH] = {};
wchar_t windir[MAX_PATH] = {};

int nxt = 1;
int curr = 0;
int currframe = 112;
unsigned long dKey;

std::map<std::wstring, bool> InstallFlags;

void NavNext(Element* elem, Event* iev) {
    if (iev->type == TouchButton::Click) {
        Navigate();
    }
}

void NavBack(Element* elem, Event* iev) {
    if (iev->type == TouchButton::Click) {
        if (curr == DEFENDERPAGE) {
            exit(0);
            return;
        }
        NavigateBack();
    }
}

void NavISO(Element* elem, Event* iev) {
    if (iev->type == TouchButton::Click) {
        Navigate();
        
    }
}

void NavSYS(Element* elem, Event* iev) {
    if (iev->type == TouchButton::Click) {
        nxt = 4;
        Navigate();
        
    }
}

void NavFull(Element* elem, Event* iev) {
    if (iev->type == TouchButton::Click) {
        Navigate();
        InstallFlags[L"INSTALLICONS"] = true;
    }
}

void NavNone(Element* elem, Event* iev) {
    if (iev->type == TouchButton::Click) {
        Navigate();
        InstallFlags[L"INSTALLICONS"] = false;
    }
}

void HandleThemesChk(Element* elem, Event* iev) {
    TouchCheckBox* tch = (TouchCheckBox*)elem;
    if (iev->type == TouchButton::Click) {
        if (tch->GetCheckedState() == CheckedStateFlags_CHECKED) { 
            tch->SetCheckedState(CheckedStateFlags_NONE);
            InstallFlags[L"INSTALLTHEMES"] = false;
        }
        else {
            tch->SetCheckedState(CheckedStateFlags_CHECKED);
            InstallFlags[L"INSTALLTHEMES"] = true;
        }
    }
}

void HandleAsdfChk(Element* elem, Event* iev) {
    TouchCheckBox* tch = (TouchCheckBox*)elem;
    if (iev->type == TouchButton::Click) {
        if (tch->GetCheckedState() == CheckedStateFlags_CHECKED) { 
            tch->SetCheckedState(CheckedStateFlags_NONE); 
            InstallFlags[L"INSTALLASDF"] = false;
        }
        else { 
            tch->SetCheckedState(CheckedStateFlags_CHECKED); 
            InstallFlags[L"INSTALLASDF"] = true;
        }
    }
}

void HandleShellChk(Element* elem, Event* iev) {
    TouchCheckBox* tch = (TouchCheckBox*)elem;
    if (iev->type == TouchButton::Click) {
        if (tch->GetCheckedState() == CheckedStateFlags_CHECKED) tch->SetCheckedState(CheckedStateFlags_NONE);
        else tch->SetCheckedState(CheckedStateFlags_CHECKED);
    }
}

void HandleExplorerChk(Element* elem, Event* iev) {
    TouchCheckBox* tch = (TouchCheckBox*)elem;
    if (iev->type == TouchButton::Click) {
        if (tch->GetCheckedState() == CheckedStateFlags_CHECKED) tch->SetCheckedState(CheckedStateFlags_NONE);
        else tch->SetCheckedState(CheckedStateFlags_CHECKED);
    }
}

void HandleIconChk(Element* elem, Event* iev) {
    TouchCheckBox* tch = (TouchCheckBox*)elem;
    if (iev->type == TouchButton::Click) {
        if (tch->GetCheckedState() == CheckedStateFlags_CHECKED) {
            tch->SetCheckedState(CheckedStateFlags_NONE);
            InstallFlags[L"INSTALLICONS"] = false;
        }
        else {
            tch->SetCheckedState(CheckedStateFlags_CHECKED);
            InstallFlags[L"INSTALLICONS"] = true;
        }
    }
}

void SetBackdrop() {

    MARGINS margins = { -1, -1, -1, -1 };
    BOOL value = TRUE;
    if (!GetUserAppMode())DwmSetWindowAttribute(pwnd->GetHWND(), DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));
    if (CheckVer(22523)) {
        DwmExtendFrameIntoClientArea(pwnd->GetHWND(), &margins);
        DwmSetWindowAttribute(pwnd->GetHWND(), DWMWA_USE_HOSTBACKDROPBRUSH, &value, sizeof(value));
        DWM_SYSTEMBACKDROP_TYPE backdrop_type = DWMSBT_TABBEDWINDOW;
        DwmSetWindowAttribute(pwnd->GetHWND(), DWMWA_SYSTEMBACKDROP_TYPE, &backdrop_type, sizeof(backdrop_type));
    }
}


void OpenCredits(Element* elem, Event* iev) {
    if (iev->type == TouchButton::Click) {

        InitMiscWindow(false, pMain->GetSheet(), hinst);
    }
}

bool DetectUninstall() {
    LPCWSTR path = L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Rectify";
    HKEY hKey;
    DWORD lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, path, 0, KEY_READ, &hKey);
    if (lResult == ERROR_SUCCESS) return true;
    return false;
}

LRESULT CALLBACK SubclassWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_UPDATEANIMATIONFRAME: {
            V = Value::CreateString((UCString)MAKEINTRESOURCE(currframe), hinst);
            waitAnimation->SetValue(RichText::ContentProp, 2, V);
            currframe++;
            if (currframe == 230) currframe = 112;
            break;
        }
        case WM_UPDATERESTARTANIMATIONFRAME: {
            V = Value::CreateString((UCString)MAKEINTRESOURCE(currframe), hinst);
            restartWaitAnimation->SetValue(RichText::ContentProp, 2, V);
            currframe++;
            if (currframe == 230) currframe = 112;
            break;
        }
        case WM_UPDATEPROGRESS: {
            progressmeter->SetContentString((UCString)IEngineWrapper::currprogress.c_str());
            break;
        }
        case WM_UPDATECOUNTDOWN: {
            std::wstring ws = L"Restarting in: " + std::to_wstring(IEngineWrapper::Ttime.load()) + L" seconds";
            Countdown->SetContentString((UCString)ws.c_str());
            break;
        }
        case WM_SETUPCOMPLETE: {
            Navigate();
            break;
        }
        case WM_MOVE: {
            SetWindowPos(hWnd, NULL, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL);
            SendMessage(hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
            break;
        }
        case WM_DESTROY: {
            exit(0);
            break;
        }
    }
    return CallWindowProc(WndProc, hWnd, uMsg, wParam, lParam);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {


    InstallFlags[L"NONE"] = true;
    InstallFlags[L"INSTALLICONS"] = true;
    InstallFlags[L"INSTALLTHEMES"] = true;
    InstallFlags[L"INSTALLASDF"] = true;
    InstallFlags[L"AMD64"] = true;
    InstallFlags[L"ARM64"] = false;

    USHORT processMachine = 0;
    USHORT nativeMachine = 0;

    HANDLE hProcess = GetCurrentProcess();

    if (IsWow64Process2(hProcess, &processMachine, &nativeMachine)) {
        switch (nativeMachine) {
        case IMAGE_FILE_MACHINE_AMD64: {
            InstallFlags[L"AMD64"] = true;
            InstallFlags[L"ARM64"] = false;
            break;
        }
        case IMAGE_FILE_MACHINE_ARM64: {
            InstallFlags[L"AMD64"] = false;
            InstallFlags[L"ARM64"] = true;
            break;
        }
        }
    }

    HRESULT err = 0;
    GetCurrentDirectory(MAX_PATH, currdir);
    wstring ws(currdir);

    GetEnvironmentVariable(L"systemroot", windir, MAX_PATH);
    StringCchPrintf(r11dir, MAX_PATH, L"%s\\Rectify11", currdir);
    StringCchPrintf(r11targetdir, MAX_PATH, L"%s\\Rectify11", windir);

    MainLogger.StartLogger((ws+L"\\Initialization.log").c_str());
    NavLogger.StartLogger((ws + L"\\Navigation.log").c_str());
    InstallationLogger.StartLogger((ws + L"\\Installation.log").c_str());

    wchar_t fPathOld[MAX_PATH];
    wchar_t fPath[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currdir);
    StringCchPrintf(fPathOld, MAX_PATH, L"%s\\segoe_r11.ttf", currdir);
    StringCchPrintf(fPath, MAX_PATH, L"%s\\segoe_r11.ttf", windir);
    CopyFile(fPathOld, fPath, false);
    AddFontResource(fPath);

    hinst = hInstance;
    pageArr.push_back(NULL);
    animArr.push_back(NULL);

    uninstall = DetectUninstall();


    err = InitProcessPriv(14, NULL, NULL, false);
    MainLogger.WriteLine(L"InitProcessPriv() completed", err);
    if (FAILED(err)) {
        MainLogger.WriteLine(L"DirectUI initialization failed.");
        return err;
    }

    err = InitThread(2);
    MainLogger.WriteLine(L"InitThread() completed", err);
    if (FAILED(err)) {
        MainLogger.WriteLine(L"InitThread() failed.");
        return err;
    }

    err = RegisterAllControls();
    MainLogger.WriteLine(L"RegisterAllControls() completed", err);
    if (FAILED(err)) {
        MainLogger.WriteLine(L"Failed to register DirectUI controls.");
        return err;
    }

    err = NativeHWNDHost::Create((UCString)L"", NULL, NULL,
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        NULL, WS_OVERLAPPED | WS_CAPTION | WS_MAXIMIZE, 0, &pwnd);
    MainLogger.WriteLine(L"NativeHWNDHost::Create() completed", err);
    if (FAILED(err)) {
        MainLogger.WriteLine(L"Failed to create installer window.");
        return err;
    }
    
    SetBackdrop();
    WndProc = (WNDPROC)SetWindowLongPtrW(pwnd->GetHWND(), GWLP_WNDPROC, (LONG_PTR)SubclassWindowProc);
    err = DUIXmlParser::Create(&pParser, NULL, NULL, NULL, NULL);
    MainLogger.WriteLine(L"DUIXmlParser::Create() completed", err);
    if (FAILED(err)) {
        MainLogger.WriteLine(L"Failed to create parser.");
        return err;
    }

    err = pParser->SetXMLFromResource((UINT)IDR_UIFILE1, hInstance, hInstance);
    MainLogger.WriteLine(L"pParser->SetXMLFromResource() complete", err);
    if (FAILED(err)) {
        MainLogger.WriteLine(L"Failed to set parser xml to resource. Please verify that all the resources are present and valid.");
        return err;
    }

    err = HWNDElement::Create(pwnd->GetHWND(), true, 0, NULL, &dKey, (Element**)&HElement);
    MainLogger.WriteLine(L"HWNDElement::Create()  complete", err);
    if (FAILED(err)) {
        MainLogger.WriteLine(L"Failed to create host hwndelement");
        return err;
    }

    err = pParser->CreateElement((UCString)L"Main", HElement, NULL, NULL, &pMain);
    MainLogger.WriteLine(L"pParser->CreateElement() completed", err);
    if (FAILED(err)) {
        MainLogger.WriteLine(L"Failed to copy element from parser to HwndElement");
        return err;
    }

    pMain->SetVisible(true);
    pMain->EndDefer(dKey);
    pwnd->Host(pMain);

    if (!uninstall) {
        err = InitInstaller();
        MainLogger.WriteLine(L"InitInstaller() completed", err);
        if (FAILED(err)) {
            MainLogger.WriteLine(L"Installer Initialisation failed.");
            return err;
        }
    }
    else {
        err = InitUninstaller();
        MainLogger.WriteLine(L"InitUninstaller() completed", err);
        if (FAILED(err)) {
            MainLogger.WriteLine(L"Uninstaller Initialisation failed.");
            return err;
        }
    }
    Navigate();

    pwnd->ShowWindow(SW_SHOW);

    StartMessagePump();
    UnInitProcessPriv(NULL);

    return err;
}

