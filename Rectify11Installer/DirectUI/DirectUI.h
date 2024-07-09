#pragma once
#include <Windows.h>
#include <functional>
#include <oleacc.h>
#include <objbase.h>
#include <initguid.h>
#include <oleacc.h>
#include <Commctrl.h>
#include <UIAutomationCore.h>
#include <UIAutomationCoreApi.h>
#include <DbgHelp.h>
#include <XmlLite.h>

#if	defined(DIRECTUI_EXPORTS)
#define UILIB_API __declspec(dllexport)
#else
#define UILIB_API __declspec(dllimport)
#endif

#include "types.h"
#include "misc.h"
#include "Interfaces.h"

#include "AutoLock.h"
#include "AutoThread.h"
#include "AutoVariant.h"
#include "Value.h"

#include "Primitives.h"
#include "parser.h"
#include "element.h"
#include "Button.h"
#include "base.h"
#include "layout.h"
#include "BorderLayout.h"
#include "host.h"
#include "accessibility.h"
#include "provider.h"
#include "ElementWithHWND.h"
#include "HWNDElement.h"
#include "NativeHWNDHost.h"
#include "HWNDHost.h"
#include "CallstackTracker.h"
#include "CheckBoxGlyph.h"
#include "Element.h"
#include "EventManager.h"
#include "FontCache.h"
#include "FontCheckOut.h"
#include "HWNDElement.h"
#include "InvokeHelper.h"
#include "Progress.h"
#include "RangeValue.h"
#include "RefPointElement.h"

#include "RichText.h"
#include "TouchButton.h"
#include "TouchCheckBox.h"

#include "CClassFactory.h"

//UnknownElement

UILIB_API void WINAPI DumpDuiTree(DirectUI::Element *, int);
UILIB_API void WINAPI DumpDuiProperties(DirectUI::Element *);

namespace DirectUI
{
	extern UILIB_API unsigned long g_dwElSlot;

	HRESULT WINAPI InitProcessPriv(int duiVersion, unsigned short*unk1, char unk2, bool bEnableUIAutomationProvider);
	HRESULT WINAPI UnInitProcessPriv(unsigned short*unk1);
	EXTERN_C HRESULT WINAPI InitThread(int iDontKnow);
	void WINAPI UnInitThread();

	int WINAPI CreateDUIWrapper(Element*,class XProvider**);
	int WINAPI CreateDUIWrapperEx(Element*, class IXProviderCP*, class XProvider**);
	int WINAPI CreateDUIWrapperFromResource(HINSTANCE,UCString, UCString, UCString, class XResourceProvider**);

	int WINAPI GetScreenDPI();

	int WINAPI RegisterAllControls();
	int WINAPI RegisterBaseControls();
	int WINAPI RegisterBrowserControls();
	int WINAPI RegisterCommonControls();
	int WINAPI RegisterExtendedControls();
	int WINAPI RegisterMacroControls();
	int WINAPI RegisterMiscControls();
	int WINAPI RegisterStandardControls();
	int WINAPI RegisterXControls();

	int WINAPI StartMessagePump();
	int WINAPI StopMessagePump();


	ATOM WINAPI StrToID(UCString resId);


	int WINAPI UnicodeToMultiByte(UCString lpWideCharStr, int cchWideChar, int unk);
	int WINAPI MultiByteToUnicode(LPCSTR lpMultiByteStr, int cbMultiByte, int unk);

	BOOL WINAPI IsAnimationsEnabled();
	int WINAPI IsPalette(HWND hWnd);
	BOOL WINAPI IsUIAutomationProviderEnabled();

	int WINAPI DUIDrawShadowText(HDC hdcDest, UCString lpchText, int cchText, LPRECT hdcSrc, UINT format, COLORREF dwTextColor);

	int WINAPI BlurBitmap(void*, void*, void*, void*, void*);

	HBRUSH WINAPI BrushFromEnumI(_In_ int Index);

	DWORD WINAPI ColorFromEnumI(_In_ int Index);

	LPVOID WINAPI DisableAnimations();
	int WINAPI DrawShadowTextEx(HDC hdc, const WCHAR *lpchText, int cchText, LPRECT hdcSrc, UINT format, COLORREF dwTextColor, COLORREF dwBkColor, int a9, int a10, COLORREF a11, int a12);
	void* WINAPI ElementFromGadget(void*);
	LPVOID WINAPI EnableAnimations();
	void WINAPI FlushThemeHandles(unsigned int);

	//此函数仅调用DebugBreak，将程序中断
	void WINAPI ForceDebugBreak();

	DWORD WINAPI GetElementDataEntry(int a1);
	LPVOID WINAPI GetFontCache();

	HRESULT WINAPI GetThemeHandle(LPCWSTR, void **);

	//此函数调用SysAllocString，并返回ppStr
	HRESULT WINAPI HrSysAllocString(OLECHAR *psz, BSTR* ppStr);

	//此函数用于复制lpString字符串，并返回ppStr
	HRESULT WINAPI HStrDup(LPCWSTR lpString, LPCWSTR* ppStr);

	//此函数是空实现，无任何作用
	BOOL WINAPI InitPreprocessor();

	HRESULT WINAPI SetDefAction(Element *a1, _In_  DWORD dwRole);

	BOOL WINAPI UiaHideOnGetObject(_In_ HWND hWnd, int a2, int a3);

	//调用RemoveProp 返回举个句柄
	HANDLE WINAPI UiaOnDestroySink(_In_ HWND hWnd);
	HRESULT WINAPI UiaOnGetObject(void* *a1, int a2, InvokeHelper *a3, int a4, int a5);
	BOOL WINAPI UiaOnToolTip(Element *, DWORD);

	void WINAPI NotifyAccessibilityEvent(DWORD event, Element *);
	void *WINAPI PreprocessBuffer(LPCWSTR Src, SIZE_T cSrc, BOOLEAN a3);
	HGDIOBJ WINAPI ProcessAlphaBitmapI(HBITMAP hgdiobj);
	void WINAPI PurgeThemeHandles();

	struct EventListener : public IElementListener {

		using handler_t = std::function<void(Element*, Event*)>;

		handler_t f;

		EventListener(handler_t f) : f(f) { }

		void OnListenerAttach(Element* elem) override { }
		void OnListenerDetach(Element* elem) override { }
		bool OnPropertyChanging(Element* elem, const PropertyInfo* prop, int unk, Value* v1, Value* v2) override {
			return true;
		}
		void OnListenedPropertyChanged(Element* elem, const PropertyInfo* prop, int type, Value* v1, Value* v2) override { }
		void OnListenedEvent(Element* elem, struct Event* iev) override {
			f(elem, iev);
		}
		void OnListenedInput(Element* elem, struct InputEvent* ev) override { }
	};
}