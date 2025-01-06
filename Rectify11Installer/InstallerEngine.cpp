#include "framework.h"
#include "InstallerEngine.h"
#include "DirectUI/DirectUI.h"
#include "Navigation.h"
#include "InstallationProcedure.h"
#include "UninstallationProcedure.h"

using namespace DirectUI;

std::atomic_bool IEngineWrapper::animate = true;
std::atomic<int> IEngineWrapper::progressnum = 0;
std::atomic<int> IEngineWrapper::Ttime = 30;
std::wstring IEngineWrapper::currprogress = L"";

unsigned long IEngineWrapper::BeginMainAnim(LPVOID lpParam) {
    while (animate.load()) {
        SendMessage(pwnd->GetHWND(), WM_UPDATEANIMATIONFRAME, NULL, NULL);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    ExitThread(0);
}

unsigned long IEngineWrapper::BeginRestartAnim(LPVOID lpParam) {
    while (animate.load()) {
        SendMessage(pwnd->GetHWND(), WM_UPDATERESTARTANIMATIONFRAME, NULL, NULL);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    ExitThread(0);
}

unsigned long IEngineWrapper::BeginInstall(LPVOID lpParam){
    std::wstring ws;

    currprogress = L"Extracting files...";
    SendMessage(pwnd->GetHWND(), WM_UPDATEPROGRESS, NULL, NULL);
    extractFiles();
    currprogress = L"Copying files...";
    SendMessage(pwnd->GetHWND(), WM_UPDATEPROGRESS, NULL, NULL);
    MoveFilesToTarget();
    currprogress = L"Installing fonts...";
    SendMessage(pwnd->GetHWND(), WM_UPDATEPROGRESS, NULL, NULL);
    InstallFonts();
    currprogress = L"Installing Programs...";
    SendMessage(pwnd->GetHWND(), WM_UPDATEPROGRESS, NULL, NULL);
    InstallPrograms();
    currprogress = L"Installing Tweaks...";
    SendMessage(pwnd->GetHWND(), WM_UPDATEPROGRESS, NULL, NULL);
    RegisterWHMods();
    currprogress = L"Finishing Installation...";
    SendMessage(pwnd->GetHWND(), WM_UPDATEPROGRESS, NULL, NULL);
    FinaliseInstall();
     
    SendMessage(pwnd->GetHWND(), WM_SETUPCOMPLETE, NULL, NULL);
    ExitThread(0);
}

unsigned long IEngineWrapper::BeginUninstall(LPVOID lpParam) {

    currprogress = L"Uninstalling...";
    SendMessage(pwnd->GetHWND(), WM_UPDATEPROGRESS, NULL, NULL);
    RemoveWHMods();
    RemoveSecureUX();
    FinaliseUninstall();
    SendMessage(pwnd->GetHWND(), WM_SETUPCOMPLETE, NULL, NULL);
    ExitThread(0);
}

unsigned long IEngineWrapper::BeginRestartCountdown(LPVOID lpParam) {
    int i = 30;
    while (Ttime.load() >= 0 && animate.load()) {
            SendMessage(pwnd->GetHWND(), WM_UPDATECOUNTDOWN, NULL, NULL);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            i--;
            Ttime.store(i);
    }
    SetupComplete();
    SendMessage(pwnd->GetHWND(), WM_DESTROY, NULL, NULL);
    ExitThread(0);
}

void IEngineWrapper::StartThread(unsigned long (*func)(LPVOID lpParam)) {
    animate.store(true);
    DWORD myThreadID;
    ienThread = CreateThread(0, 0, func, NULL, 0, &myThreadID);
}

void IEngineWrapper::StopThread() {
    animate.store(false);
}

void IEngineWrapper::LogWriter(wfstream& wfs, wstring ws) {
    wfs << ws << endl;
}
