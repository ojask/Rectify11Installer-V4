#pragma once
#ifndef MY_HEADERR11_H
#define MY_HEADERR11_H

#include "DirectUI/DirectUI.h"
using namespace DirectUI;

void NavNext(Element* elem, Event* iev);

void NavBack(Element* elem, Event* iev);

void NavISO(Element* elem, Event* iev);

void NavSYS(Element* elem, Event* iev);

void NavExp(Element* elem, Event* iev);


void NavFull(Element* elem, Event* iev);

void NavNone(Element* elem, Event* iev);

void HandleThemesChk(Element* elem, Event* iev);

void HandleAsdfChk(Element* elem, Event* iev);

void HandleStartifyChk(Element* elem, Event* iev);

void HandleEPChk(Element* elem, Event* iev);


void HandleMFEChk(Element* elem, Event* iev);

#endif