#include "InitUninst.h"
#include "Initr11.h"
#include "resource.h"
#include "framework.h"
#include "Rectify11Installer.h"
#include "Navigation.h"

HRESULT err2 = 0;
RichText* useless;

int InitUninstPages() {
    for (int i = 1; i < MAXUNINSTPAGE; i++) {

        Element* page;
        MainLogger.WriteLine(L"Setting XML parser from resource: " + to_wstring(IDR_UIFILE8 + i));
        err2 = pParser->SetXMLFromResource(IDR_UIFILE8 + i, hinst, hinst);
        if (FAILED(err2)) {
            MainLogger.WriteLine(L"failed to set XML parser from resource: " + to_wstring(IDR_UIFILE8 + i), err2);
            return err2;
        }

        std::wstring n = L"page" + std::to_wstring(i);
        MainLogger.WriteLine(L"Finding Element with ID: " + n);
        Element* pContainer = pMain->FindDescendent(StrToID((UCString)n.c_str()));
        if (!pContainer) {
            err2 = -30;
            MainLogger.WriteLine(L"Element with ID: " + n + L" not found", err2);
            return err2;
        }

        MainLogger.WriteLine(L"Creating PageMain Element inside " + n);
        err2 = pParser->CreateElement((UCString)L"PageMain", pContainer, NULL, NULL, &page);
        if (FAILED(err2)) {
            MainLogger.WriteLine(L"Cannot create PageMain Element inside " + n, err2);
        }

        std::wstring anim = L"animator" + std::to_wstring(i);
        MainLogger.WriteLine(L"Finding Element with ID: " + anim);
        Element* pAnimator = pMain->FindDescendent(StrToID((UCString)anim.c_str()));
        if (FAILED(err2)) {
            err2 = -31;
            MainLogger.WriteLine(L"Element with ID: " + anim + L" not found", err2);
            return err2;
        }

        MainLogger.WriteLine(L"Changing " + n + L"'s visibility");
        err2 = pContainer->SetVisible(false);
        if (FAILED(err2)) {
            err2 = -32;
            MainLogger.WriteLine(L"Failed to change " + n + L"'s visibility", err2);
            return err2;
        }

        MainLogger.WriteLine(L"Adding page " + to_wstring(i) + L" to page list");
        animArr.push_back(pAnimator);
        pageArr.push_back(pContainer);

        MainLogger.WriteLine(L"\n");
    }
    MainLogger.WriteLine(L"InitUninstPages() completed", err2);
    return err2;
}

void InitUninstControls() {

    progressbar = pMain->FindDescendent(StrToID((UCString)L"progress"));
    Nxt = (TouchButton*)pMain->FindDescendent(StrToID((UCString)L"next"));
    Bck = (TouchButton*)pMain->FindDescendent(StrToID((UCString)L"back"));

    Nxt->AddListener(new EventListener(NavNext));
    Bck->AddListener(new EventListener(NavBack));

    useless = (RichText*)pMain->FindDescendent(StrToID((UCString)L"useless"));

    waitAnimation = (RichText*)pMain->FindDescendent(StrToID((UCString)L"WaitAnimation"));
    restartWaitAnimation = (RichText*)pMain->FindDescendent(StrToID((UCString)L"RestartWaitAnimation"));

    progressmeter = (RichText*)pMain->FindDescendent(StrToID((UCString)L"R11Progress"));
    Countdown = (RichText*)pMain->FindDescendent(StrToID((UCString)L"RestartCountdown"));

    void (*fArr[])(Element*, Event*) = { HandleThemesChk, HandleAsdfChk, HandleShellChk, HandleExplorerChk, HandleIconChk };
    TouchCheckBox* tch;
    std::wstring ws;
    for (int i = 0; i < 5; i++) {
        ws = L"chk" + std::to_wstring(i + 1);
        tch = (TouchCheckBox*)pMain->FindDescendent(StrToID((UCString)ws.c_str()));
        tch->AddListener(new EventListener(fArr[i]));
    }
}


int InitUninstaller() {
    if (!CheckVer(21343)) {
        err2 = -21343;
        TaskDialog(NULL, NULL, L"no", L"stop", L"unsupported on this windows version", TDCBF_OK_BUTTON, TD_ERROR_ICON, NULL);
        MainLogger.WriteLine(L"This Windows Build is not supported. Windows 10 Build 21343 and above is required.", err2);
        return err2;
    }

    MainLogger.WriteLine(L"Initializing pages...\n\n\n");
    err2 = InitUninstPages();
    if (FAILED(err2)) {
        err2 = -33;
        MainLogger.WriteLine(L"One or more pages failed to initialize correctly.\n", err2);
        return err2;
    }
    InitUninstControls();
    useless->SetContentString((UCString)L"Uninstalling Rectify11");
    err2 = ChangeSheet();
    if (FAILED(err2)) {
        MainLogger.WriteLine(L"Failed to change stylesheet.", err2);
        return err2;
    }
    return err2;
}
