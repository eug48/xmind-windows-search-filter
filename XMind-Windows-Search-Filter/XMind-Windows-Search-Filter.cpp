// XMind-Windows-Search-Filter.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "resource.h"
#include "XMindWindowsSearchFilter_i.h"
#include "dllmain.h"
#include "xdlldata.h"

#include "FilterHandler.h"

using namespace ATL;

// Used to determine whether the DLL can be unloaded by OLE.
STDAPI DllCanUnloadNow(void)
{
	#ifdef _MERGE_PROXYSTUB
	HRESULT hr = PrxDllCanUnloadNow();
	if (hr != S_OK)
		return hr;
#endif
			return _AtlModule.DllCanUnloadNow();
	}

// Returns a class factory to create an object of the requested type.
STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID* ppv)
{
	#ifdef _MERGE_PROXYSTUB
	HRESULT hr = PrxDllGetClassObject(rclsid, riid, ppv);
	if (hr != CLASS_E_CLASSNOTAVAILABLE)
		return hr;
#endif
		return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

// DllRegisterServer - Adds entries to the system registry.
STDAPI DllRegisterServer(void)
{
	// registers object, typelib and all interfaces in typelib
	HRESULT hr = _AtlModule.DllRegisterServer();
	#ifdef _MERGE_PROXYSTUB
	if (FAILED(hr))
		return hr;
	hr = PrxDllRegisterServer();
#endif
		return hr;
}

// DllUnregisterServer - Removes entries from the system registry.
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	#ifdef _MERGE_PROXYSTUB
	if (FAILED(hr))
		return hr;
	hr = PrxDllRegisterServer();
	if (FAILED(hr))
		return hr;
	hr = PrxDllUnregisterServer();
#endif
		return hr;
}

// DllInstall - Adds/Removes entries to the system registry per user per machine.
STDAPI DllInstall(BOOL bInstall, _In_opt_  LPCWSTR pszCmdLine)
{
	HRESULT hr = E_FAIL;

	if (bInstall)
	{	
		hr = DllRegisterServer();
		if (FAILED(hr))
		{
			DllUnregisterServer();
		}
	}
	else
	{
		hr = DllUnregisterServer();
	}

	return hr;
}

/* 
Entry point for rundll32
Use by running e.g. c:\windows\System32\rundll32.exe d:\xmind-windows-search-filter\bin\xmind-search-filter-dbg-x64,xmind2html d:\test.xmind
http://support.microsoft.com/kb/164787
*/
void CALLBACK xmind2htmlW(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow)
{
	::CoInitialize(NULL);

	HRESULT hr;
	CComObject<CFilterHandler>* handler = new CComObject<CFilterHandler>();
	hr = handler->FinalConstruct();
	hr = handler->Load(lpszCmdLine, 0);

	CComBSTR filename;
	filename.Append(lpszCmdLine);
	filename.Append(L".html");

	hr = handler->SaveHtmlToFile(filename);

	delete handler;
}

/* Tracing code
from
http://stackoverflow.com/a/20931125

*/

void CustomTrace(const wchar_t* format, ...)
{
	const int TraceBufferSize = 1024;
	wchar_t buffer[TraceBufferSize];

	va_list argptr; va_start(argptr, format);
	vswprintf_s(buffer, format, argptr);
	va_end(argptr);

	::OutputDebugString(buffer);
}

void CustomTrace(const char* format, ...)
{
	const int TraceBufferSize = 1024;
	char buffer[TraceBufferSize];

	va_list argptr; va_start(argptr, format);
	vsprintf_s(buffer, format, argptr);
	va_end(argptr);

	::OutputDebugStringA(buffer);
}


XMindContentsXSLT CFilterHandler::xmindXSLT;
int XMindContentsXSLT::static_int;