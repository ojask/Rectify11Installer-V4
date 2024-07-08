#pragma once

#ifndef MY_HEADERNAV_H
#define MY_HEADERNAV_H

#include "InstallerEngine.h"
#include "Logger.h"
#include <vector>
#include "DirectUI/DirectUI.h"

using namespace std;
using namespace DirectUI;

extern unsigned long dKey;

extern TouchButton* defenderbtn;
extern TouchButton* browsebtn;

extern RichText* waitAnimation;
extern RichText* restartWaitAnimation;

extern RichText* progressmeter;
extern RichText* Countdown;
 
extern DUIXmlParser* pParser;
extern HINSTANCE hinst;
 
extern HWNDElement* HElement;
 
extern Value* V;
extern WNDPROC WndProc;

extern PatchType pType;
extern InstallType iType;

extern NativeHWNDHost* pwnd;
extern Element* pMain;

extern vector<Element*> pageArr;
extern vector<Element*> animArr;

extern Element* progressbar;
extern TouchButton* Nxt;
extern TouchButton* Bck;

extern Logger MainLogger;
extern Logger NavLogger;

extern int nxt;
extern int curr;

int ChangePage(int target);
void TaskAtArrival();
void Navigate();
void NavigateBack();

#endif