#include "framework.h"
#include "resource.h"
#include "MiscWindow.h"
#include "Initr11.h"
#include "DirectUI/DirectUI.h"
using namespace DirectUI;

NativeHWNDHost* wnd{};

void closeWnd(Element* elem, Event* iev) {
    if (iev->type == TouchButton::Click) {
        wnd->HideWindow();
    }
}

void joinDsc(Element* elem, Event* iev) {
    if (iev->type == TouchButton::Click) {
        ShellExecute(0, 0, L"https://discord.gg/52VChAhtgP", 0, 0, SW_SHOW);
    }
}

void InitMiscWindow(bool coffee, StyleSheet* ssheet, HINSTANCE hInstance) {

    DUIXmlParser* parser{};
    HWNDElement* parent{};
    Element* pMain{};
    unsigned long key;
    const wchar_t* windowname = L"Credits";

    if (coffee) windowname = L"Buy us a coffee";

    NativeHWNDHost::Create((UCString)windowname, NULL, NULL, CW_USEDEFAULT, CW_USEDEFAULT, 500, 600, NULL, NULL, 0, &wnd);

    DUIXmlParser::Create(&parser, NULL, NULL, NULL, NULL);

    parser->SetXMLFromResource(IDR_UIFILE13, hInstance, hInstance);
    if(coffee)  parser->SetXMLFromResource(IDR_UIFILE12, hInstance, hInstance);

    HWNDElement::Create(wnd->GetHWND(), true, NULL, NULL, &key, (Element**)&parent);

    parser->CreateElement((UCString)L"main", parent, NULL, NULL, &pMain);
    pMain->SetVisible(true);
    pMain->EndDefer(key);

    wnd->Host(pMain);

    pMain->SetSheet(ssheet);

    (pMain->FindDescendent(StrToID((UCString)L"close")))->AddListener(new EventListener(closeWnd));

    (pMain->FindDescendent(StrToID((UCString)L"discord")))->AddListener(new EventListener(joinDsc));

    BOOL value = TRUE;
    if (!GetUserAppMode())DwmSetWindowAttribute(wnd->GetHWND(), DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));

    wnd->ShowWindow(SW_SHOW);
}