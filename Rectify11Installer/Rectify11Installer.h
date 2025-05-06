#pragma once
#ifndef MY_HEADERR11_H
#define MY_HEADERR11_H

#include "DirectUI/DirectUI.h"
using namespace DirectUI;

void NavNext(Element* elem, Event* iev);

void NavBack(Element* elem, Event* iev);

void NavISO(Element* elem, Event* iev);

void NavSYS(Element* elem, Event* iev);

void NavFull(Element* elem, Event* iev);

void NavNone(Element* elem, Event* iev);

void HandleThemesChk(Element* elem, Event* iev);

void HandleAsdfChk(Element* elem, Event* iev);

void HandleShellChk(Element* elem, Event* iev);

void HandleExplorerChk(Element* elem, Event* iev);

void HandleIconChk(Element* elem, Event* iev);

void OpenCredits(Element* elem, Event* iev);

#endif