#pragma once
#define WM_UPDATEANIMATIONFRAME WM_USER+1
#define WM_UPDATERESTARTANIMATIONFRAME WM_USER+2
#define WM_UPDATEPROGRESS WM_USER+3
#define WM_UPDATECOUNTDOWN WM_USER+4
#define WM_SETUPCOMPLETE WM_USER+5

enum PatchType {
	SYSTEM, ISO
};
enum InstallType {
	EXPRESS, FULL, NONE
};

#ifndef MY_HEADER_H
#define MY_HEADER_H

#include <atomic>
#include <thread>
#include <chrono>
#include <string>
#include <fstream>
using namespace std;


void GetInstallInfo(PatchType pTypeW, InstallType iTypeW);

#endif

class IEngineWrapper {
private:
	thread ienThread;

public:
	static atomic_bool animate;
	static atomic<int> progressnum;
	static atomic<int> Ttime;
	static wstring currfile;
	void StartThread(void (*func)());
	void StopThread();
	static void BeginMainAnim();
	static void BeginRestartAnim();
	static void BeginInstall();
	static void BeginRestartCountdown();
	static void LogWriter(wfstream& wfs, wstring ws);
};
