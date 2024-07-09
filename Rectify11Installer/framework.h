// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <Windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#define DEFENDERPAGE               1
#define PATCHSELECTPAGE            2
#define ISOSELECTPAGE              3
#define INSTALLSELECTPAGE          4
#define COMPONENTSELECTPAGE        5
#define PROGRESSPAGE               6
#define RESTARTPAGE                7

#define MAXPAGE                    8