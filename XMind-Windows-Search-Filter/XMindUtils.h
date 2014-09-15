#pragma once

#define AFX_PREVIEW_STANDALONE
#include <atlhandler.h>
#include <atltrace.h>
#include <atlcomcli.h>
#include <msxml6.h>

using namespace ATL;

class XMindContentsXSLT
{
public:
	XMindContentsXSLT()
	{

	}

	HRESULT ToHTML(BYTE* xml, DWORD xml_len, BSTR* outHTML)
	{
		HRESULT hr;

		if (xsltDoc == NULL)
		{
			hr = LoadXSLT();
			if (FAILED(hr)) return hr;
		}

		ATL::CComPtr< IXMLDOMDocument > contentsDoc;
		hr = contentsDoc.CoCreateInstance(__uuidof(DOMDocument60));
		if (FAILED(hr))
		{
			ATLTRACE2(atlTraceGeneral, 1, "[xmindfilter] [ToHTML] Error:  contents doc create failed hr=%d", hr);
			return hr;
		}

		VARIANT_BOOL success;
		ATL::CComBSTR xmlBSTR(xml_len, (char*)xml);
		hr = contentsDoc->loadXML(xmlBSTR, &success);
		if (FAILED(hr))
		{
			ATLTRACE2(atlTraceGeneral, 1, "[xmindfilter] [ToHTML] Error:  contents doc loadXML hr=%d", hr);
			return hr;
		}

		if (success != VARIANT_TRUE)
		{
			ATLTRACE2(atlTraceGeneral, 1, "[xmindfilter] [ToHTML] Error: contents doc loaXML failed success=%d", success);
			return E_FAIL;
		}

		CComBSTR result;
		hr = contentsDoc->transformNode(xsltDoc, &result);
		if (FAILED(hr))
		{
			ATLTRACE2(atlTraceGeneral, 1, "[xmindfilter] [ToHTML] Error:  transformNode failed hr=%d", hr);
			return hr;
		}

		*outHTML = result.Detach();
		hr = S_OK;
		return hr;
	}


private:

	IXMLDOMDocument* xsltDoc; // can't use CComPtr since static destructors crash - http://blogs.msdn.com/b/larryosterman/archive/2004/04/22/118240.aspx
	static int static_int;

	HRESULT LoadXSLT()
	{
		// load resource
		/* https://github.com/sky-y/xmindoc/blob/master/lib/xmindoc/content.xsl */

		HINSTANCE hInstance = NULL;
		if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)&XMindContentsXSLT::static_int, &hInstance) == 0)
		{
			ATLTRACE2(atlTraceGeneral, 1, "[xmindfilter] [LoadXSLT] Error: GetModuleHandleEx failed hr=%d", ::GetLastError());
			return E_FAIL;
		}

		HRSRC hrsrc = ::FindResource(hInstance, MAKEINTRESOURCE(IDR_RCDATA_XMIND_CONTENT_XSL), RT_RCDATA);
		if (hrsrc == NULL)
		{
			ATLTRACE2(atlTraceGeneral, 1, "[xmindfilter] [LoadXSLT] Error: FindResource failed hr=%d", ::GetLastError());
			return E_FAIL;
		}

		HANDLE hglob = LoadResource(hInstance, hrsrc);
		if (hglob == NULL)
		{
			ATLTRACE2(atlTraceGeneral, 1, "[xmindfilter] [LoadXSLT] Error: LoadResource failed hr=%d", ::GetLastError());
			return E_FAIL;
		}

		BYTE* xsl_buffer = (BYTE*) LockResource(hglob);
		if (xsl_buffer == NULL)
		{
			ATLTRACE2(atlTraceGeneral, 1, "[xmindfilter] [LoadXSLT] Error: LockResource failed hr=%d", ::GetLastError());
			return E_FAIL;
		}

		IStream* xslStreamRaw = ::SHCreateMemStream(xsl_buffer, SizeofResource(hInstance, hrsrc));
		if (xslStreamRaw == NULL)
		{
			ATLTRACE2(atlTraceGeneral, 1, "[xmindfilter] [LoadXSLT] Error: SHCreateMemStream failed hr=%d", ::GetLastError());
			return E_FAIL;
		}

		CComPtr<IStream> xslStream;
		xslStream.Attach(xslStreamRaw);

		// load xml
		HRESULT hr;
		hr = ::CoCreateInstance(__uuidof(DOMDocument60), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&xsltDoc));
		if (FAILED(hr))
		{
			ATLTRACE2(atlTraceGeneral, 1, "[xmindfilter] [LoadXSLT] Error:  XSLT doc CoCreateInstance failed hr=%d", hr);
			return hr;
		}

		xsltDoc->put_async(VARIANT_FALSE);
		xsltDoc->put_validateOnParse(VARIANT_FALSE);
		xsltDoc->put_resolveExternals(VARIANT_FALSE);

		VARIANT_BOOL success;		
		CComVariant xsltStreamVar(xslStream);

		hr = xsltDoc->load(xsltStreamVar, &success);
		if (FAILED(hr) || success != VARIANT_TRUE)
		{
			ATLTRACE2(atlTraceGeneral, 1, "[xmindfilter] [LoadXSLT] Error:  XSLT doc loadXML hr=%d success=%d", hr, success);

			CComPtr<IXMLDOMParseError> parseError;
			HRESULT hr2 = xsltDoc->get_parseError(&parseError);
			if (SUCCEEDED(hr2))
			{
				CComBSTR reason;
				long line = 0, linePos = 0;
				parseError->get_reason(&reason);
				parseError->get_line(&line);
				parseError->get_linepos(&linePos);
			}

			hr = FAILED(hr) ? hr : E_FAIL;
			return hr;
		}

		
		return S_OK;
	}

};

