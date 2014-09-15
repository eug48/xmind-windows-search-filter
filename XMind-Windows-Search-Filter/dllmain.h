// dllmain.h : Declaration of module class.

class CXMindWindowsSearchFilterModule : public ATL::CAtlDllModuleT< CXMindWindowsSearchFilterModule >
{
public :
	DECLARE_LIBID(LIBID_XMindWindowsSearchFilterLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_XMINDWINDOWSSEARCHFILTER, "{CEBABA72-467A-4805-9EFD-614FE401B878}")
};

extern class CXMindWindowsSearchFilterModule _AtlModule;
