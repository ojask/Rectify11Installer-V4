#include "InstallerEngine.h"
#include "framework.h"
#include "DirectUI/DirectUI.h"
#include "Navigation.h"

using namespace DirectUI;

PatchType patchType;
InstallType installType;

std::atomic_bool IEngineWrapper::animate = true;
std::atomic<int> IEngineWrapper::progressnum = 0;
std::atomic<int> IEngineWrapper::Ttime = 30;
std::wstring IEngineWrapper::currfile = L"";

void IEngineWrapper::BeginMainAnim() {
    while (animate.load()) {
        SendMessage(pwnd->GetHWND(), WM_UPDATEANIMATIONFRAME, NULL, NULL);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    ExitThread(0);
}

void IEngineWrapper::BeginRestartAnim() {
    while (animate.load()) {
        SendMessage(pwnd->GetHWND(), WM_UPDATERESTARTANIMATIONFRAME, NULL, NULL);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    ExitThread(0);
}

void IEngineWrapper::BeginInstall(){
    std::wstring ws;
    int i = 0;
    while (progressnum<=100 && animate.load()) {
         currfile = L"file" + std::to_wstring(progressnum) + L".dll";
         SendMessage(pwnd->GetHWND(), WM_UPDATEPROGRESS, NULL, NULL);
         std::this_thread::sleep_for(std::chrono::milliseconds(100));
         i++;
         progressnum.store(i);
    }    
    SendMessage(pwnd->GetHWND(), WM_SETUPCOMPLETE, NULL, NULL);
    ExitThread(0);
}

void IEngineWrapper::BeginRestartCountdown() {
    int i = 30;
    while (Ttime.load() >= 0 && animate.load()) {
            SendMessage(pwnd->GetHWND(), WM_UPDATECOUNTDOWN, NULL, NULL);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            i--;
            Ttime.store(i);
    }
    SendMessage(pwnd->GetHWND(), WM_DESTROY, NULL, NULL);
    ExitThread(0);
}

void IEngineWrapper::StartThread(void (*func)()) {
    animate.store(true);
    std::thread tmp(func);
    ienThread.swap(tmp);
    ienThread.detach();
}

void IEngineWrapper::StopThread() {
    animate.store(false);
}

void IEngineWrapper::LogWriter(wfstream& wfs, wstring ws) {
    wfs << ws << endl;
}

void GetInstallInfo(PatchType pTypeW, InstallType iTypeW) {
    patchType = pTypeW;
    installType = iTypeW;
}
