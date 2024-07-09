// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// Windows Header Files
#include <Windows.h>
#include <dwmapi.h>
#include <winerror.h>

// C++ RunTime Header Files
#include <atomic>
#include <chrono>
#include <codecvt>
#include <fstream>
#include <locale>
#include <string>
#include <thread>
#include <vector>

#define DEFENDERPAGE               1
#define PATCHSELECTPAGE            2
#define ISOSELECTPAGE              3
#define INSTALLSELECTPAGE          4
#define COMPONENTSELECTPAGE        5
#define PROGRESSPAGE               6
#define RESTARTPAGE                7

#define MAXPAGE                    8