#pragma once

#ifndef MY_HEADERNAV_H
#define MY_HEADERNAV_H

#include "framework.h"
#include "InstallerEngine.h"
#include "Logger.h"
#include "DirectUI/DirectUI.h"

using namespace std;
using namespace DirectUI;

extern unsigned long dKey;

extern bool uninstall;

extern TouchButton* browsebtn;

extern RichText* waitAnimation;
extern RichText* restartWaitAnimation;

extern RichText* progressmeter;
extern RichText* Countdown;

extern RichText* useless;
 
extern DUIXmlParser* pParser;
extern HINSTANCE hinst;
 
extern HWNDElement* HElement;
extern HWNDElement* HElement2;
extern HWNDElement* HElement3;

extern Value* V;
extern WNDPROC WndProc;

extern NativeHWNDHost* pwnd;
extern Element* pMain;

extern NativeHWNDHost* pwndNotes;
extern Element* nMain;

extern NativeHWNDHost* pwndCredits;
extern Element* cMain;

extern vector<Element*> pageArr;
extern vector<Element*> animArr;

extern Element* progressbar;
extern TouchButton* Nxt;
extern TouchButton* Bck;

extern Logger MainLogger;
extern Logger NavLogger;
extern Logger InstallationLogger;

extern wchar_t currdir[MAX_PATH];
extern wchar_t r11dir[MAX_PATH];
extern wchar_t r11targetdir[MAX_PATH];
extern wchar_t windir[MAX_PATH];

extern std::map<std::wstring, bool> InstallFlags;

extern int nxt;
extern int curr;

int ChangePage(int target);
void TaskAtArrival();
void Navigate();
void NavigateBack();

#endif