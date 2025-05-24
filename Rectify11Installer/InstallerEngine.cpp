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
std::wstring IEngineWrapper::currprogress;
std::mutex IEngineWrapper::progressMutex;
std::mutex IEngineWrapper::logMutex;

unsigned long IEngineWrapper::BeginMainAnim(LPVOID lpParam) {
    while (animate.load()) {
        PostMessage(pwnd->GetHWND(), WM_UPDATEANIMATIONFRAME, NULL, NULL);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    ExitThread(0);
}

unsigned long IEngineWrapper::BeginRestartAnim(LPVOID lpParam) {
    while (animate.load()) {
        PostMessage(pwnd->GetHWND(), WM_UPDATERESTARTANIMATIONFRAME, NULL, NULL);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    ExitThread(0);
}

unsigned long IEngineWrapper::BeginInstall(LPVOID lpParam){

    {
        std::lock_guard<std::mutex> lock(progressMutex);
        currprogress = L"Extracting files...";
    }
    PostMessage(pwnd->GetHWND(), WM_UPDATEPROGRESS, NULL, NULL);
    extractFiles();

    {
        std::lock_guard<std::mutex> lock(progressMutex);
        currprogress = L"Copying files...";
    }
    PostMessage(pwnd->GetHWND(), WM_UPDATEPROGRESS, NULL, NULL);
    MoveFilesToTarget();

    {
        std::lock_guard<std::mutex> lock(progressMutex);
        currprogress = L"Installing fonts...";
    }
    PostMessage(pwnd->GetHWND(), WM_UPDATEPROGRESS, NULL, NULL);
    InstallFonts();

    {
        std::lock_guard<std::mutex> lock(progressMutex);
        currprogress = L"Installing Programs...";
    }
    PostMessage(pwnd->GetHWND(), WM_UPDATEPROGRESS, NULL, NULL);
    InstallPrograms();

    {
        std::lock_guard<std::mutex> lock(progressMutex);
        currprogress = L"Installing Tweaks...";
    }
    PostMessage(pwnd->GetHWND(), WM_UPDATEPROGRESS, NULL, NULL);
    RegisterWHMods();

    {
        std::lock_guard<std::mutex> lock(progressMutex);
        currprogress = L"Finishing Installation...";
    }
    PostMessage(pwnd->GetHWND(), WM_UPDATEPROGRESS, NULL, NULL);
    FinaliseInstall();
     
    PostMessage(pwnd->GetHWND(), WM_SETUPCOMPLETE, NULL, NULL);
    ExitThread(0);
}

unsigned long IEngineWrapper::BeginUninstall(LPVOID lpParam) {

    {
        std::lock_guard<std::mutex> lock(progressMutex);
        currprogress = L"Uninstalling...";
    }
    PostMessage(pwnd->GetHWND(), WM_UPDATEPROGRESS, NULL, NULL);
    RemoveWHMods();
    RemoveSecureUX();
    FinaliseUninstall();
    PostMessage(pwnd->GetHWND(), WM_SETUPCOMPLETE, NULL, NULL);
    ExitThread(0);
}

unsigned long IEngineWrapper::BeginRestartCountdown(LPVOID lpParam) {
    int i = 30;
    while (Ttime.load() >= 0 && animate.load()) {
        PostMessage(pwnd->GetHWND(), WM_UPDATECOUNTDOWN, NULL, NULL);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        i--;
        Ttime.store(i);
    }
    SetupComplete();
    PostMessage(pwnd->GetHWND(), WM_DESTROY, NULL, NULL);
    ExitThread(0);
}

void IEngineWrapper::StartThread(unsigned long (*func)(LPVOID lpParam)) {
    animate.store(true);
    DWORD myThreadID;
    ienThread = CreateThread(0, 0, func, NULL, 0, &myThreadID);
}

void IEngineWrapper::StopThread() {
    animate.store(false);
    CloseHandle(ienThread);
}
