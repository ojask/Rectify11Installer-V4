#pragma once
#include "framework.h"

#ifndef MY_HEADERPROC_H
#define MY_HEADERPROC_H

void extractFiles();
void MoveFilesToTarget();
void InstallPrograms();
void RegisterWHMods();
void SetupComplete();
void InstallFonts();
void RunEXE(wchar_t exe[], wchar_t args[]);
void FinaliseInstall();
#endif