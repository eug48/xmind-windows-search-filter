// FilterHandler.h : Declaration of the filter handler

#pragma once
#include "resource.h"       // main symbols

#define AFX_PREVIEW_STANDALONE

#include "XMindWindowsSearchFilter_i.h"

#include "CustomTrace.h"
#include "unzip.h"
#include "XMindUtils.h"

using namespace ATL;

// CFilterHandler

class ATL_NO_VTABLE CFilterHandler :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CFilterHandler, &CLSID_Search>,
	public IFilter,
	public IPersistStream,
	public IPersistFile
{
public:
	CFilterHandler()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_FILTER_HANDLER)
DECLARE_NOT_AGGREGATABLE(CFilterHandler)

BEGIN_COM_MAP(CFilterHandler)
	COM_INTERFACE_ENTRY(IPersistStream)
	COM_INTERFACE_ENTRY(IPersistFile)
	COM_INTERFACE_ENTRY(IFilter)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		m_zip_buffer = NULL;
		m_contents_buffer = NULL;
		m_zip = NULL;
		return S_OK;
	}

	void FinalRelease()
	{
		if (m_zip_buffer != NULL) {
			free(m_zip_buffer);
			m_zip_buffer = NULL;
		}
		if (m_contents_buffer != NULL) {
			free(m_contents_buffer);
			m_contents_buffer = NULL;
		}

		if (m_zip != NULL) {
			CloseZip(m_zip);
			m_zip = NULL;
		}
	}

public:

	SCODE STDMETHODCALLTYPE SaveHtmlToFile(LPCWSTR filename)
	{
		HRESULT hr;
			
		// convert content.xml into HTML
		CComBSTR html;
		hr = xmindXSLT.ToHTML(m_contents_buffer, m_contents_buffer_len, &html);
		if (FAILED(hr)) return hr;

		// add a BOM
		CComBSTR htmlWithBOM(L"\uFEFF");
		hr = htmlWithBOM.AppendBSTR(html);
		if (FAILED(hr))
		{
			ATLTRACE2("[xmindfilter] [SaveHtmlToFile] AppendBSTR failed: %d", hr);
			return hr;
		}
		
		// write to file


		HANDLE hfile = ::CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hfile == INVALID_HANDLE_VALUE)
		{
			ATLTRACE2("[xmindfilter] [SaveHtmlToFile] CreateFile failed: %d", ::GetLastError());
			return E_FAIL;
		}

		DWORD written;
		BOOL ret = ::WriteFile(hfile, (BYTE*)(LPWSTR)htmlWithBOM, htmlWithBOM.ByteLength(), &written, NULL);
		if (!ret)
		{
			ATLTRACE2("[xmindfilter] [SaveHtmlToFile] WriteFile failed: %d", ::GetLastError());
			return E_FAIL;
		}

		::CloseHandle(hfile);
		return S_OK;
	}

	virtual  SCODE STDMETHODCALLTYPE  Init(ULONG grfFlags, ULONG cAttributes, FULLPROPSPEC const * aAttributes, ULONG * pFlags)
	{
		HRESULT hr;
		
		// convert content.xml into HTML
		CComBSTR html;
		hr = xmindXSLT.ToHTML(m_contents_buffer, m_contents_buffer_len, &html);
		if (FAILED(hr)) return hr;

		// add a BOM
		CComBSTR htmlWithBOM(L"\uFEFF");
		hr = htmlWithBOM.AppendBSTR(html);
		if (FAILED(hr))
		{
			ATLTRACE2("[xmindfilter] [Init] AppendBSTR failed: %d", hr);
			return hr;
		}
		// load Microsoft's nlhtml filter
		CLSID nlhtml_guid;
		hr = CLSIDFromString(L"{e0ca5340-4534-11cf-b952-00aa0051fe20}", &nlhtml_guid);
		if (FAILED(hr))
		{
			ATLTRACE2("[xmindfilter] [Init] CLSIDFromString failed: %d", hr);
			return hr;
		}

		CComPtr<IPersistStream> htmlFltStream;
		hr = htmlFltStream.CoCreateInstance(nlhtml_guid);
		if (FAILED(hr))
		{
			ATLTRACE2("[xmindfilter] [Init] CoCreateInstance nlhtml.dll failed: %d", hr);
			return hr;
		}

		// turn our html into an IStream
		CComPtr<IStream> bstrStream(::SHCreateMemStream((BYTE*)(LPWSTR)htmlWithBOM, htmlWithBOM.ByteLength()));
		if (bstrStream == NULL)
		{
			ATLTRACE2("[xmindfilter] [Init] SHCreateMemStream failed");
			hr = E_FAIL;
			return hr;
		}

		// give html to nlhtml.dll
		hr = htmlFltStream->Load(bstrStream);
		if (FAILED(hr))
		{
			ATLTRACE2("[xmindfilter] [Init] htmlFltStream->Load failed: %d", hr);
			return hr;
		}

		// init nlhtml.dll
		if (htmlIFilter != NULL) htmlIFilter = NULL;
		hr = htmlFltStream->QueryInterface(IID_PPV_ARGS(&htmlIFilter));
		if (FAILED(hr))
		{
			ATLTRACE2("[xmindfilter] [Init] htmlFltStream failed to give IFilter: %d", hr);
			return hr;
		}

		hr = htmlIFilter->Init(grfFlags, cAttributes, aAttributes, pFlags);
		if (FAILED(hr))
		{
			ATLTRACE2("[xmindfilter] [Init] htmlFltStream Init failed: %d", hr);
			return hr;
		}

		hr = S_OK;
		return hr;
	}

	virtual  SCODE STDMETHODCALLTYPE  GetChunk(STAT_CHUNK * pStat)
	{
		if (htmlIFilter == NULL)
		{
			ATLTRACE2("[xmindfilter] [GetChunk] htmlIFilter == NULL");
			return E_INVALIDARG;
		}

		HRESULT hr = htmlIFilter->GetChunk(pStat);
		return hr;
	}

	virtual  SCODE STDMETHODCALLTYPE  GetText(ULONG * pcwcBuffer, WCHAR * awcBuffer)
	{
		if (htmlIFilter == NULL)
		{
			ATLTRACE2("[xmindfilter] [GetText] htmlIFilter == NULL");
			return E_INVALIDARG;
		}

		HRESULT hr = htmlIFilter->GetText(pcwcBuffer, awcBuffer);
		return hr;
	}

	virtual  SCODE STDMETHODCALLTYPE  GetValue(PROPVARIANT * * ppPropValue)
	{
		if (htmlIFilter == NULL)
		{
			ATLTRACE2("[xmindfilter] [GetValue] htmlIFilter == NULL");
			return E_INVALIDARG;
		}

		return htmlIFilter->GetValue(ppPropValue);
	}


	// IPersistStream implementation
	IFACEMETHODIMP GetClassID(CLSID* pClassID)
	{
		*pClassID = CLSID_Search;
		return S_OK;
	}


	virtual  SCODE STDMETHODCALLTYPE  Load(LPCWSTR pszFileName, DWORD dwMode)
	{
		ATLTRACE2(L"[xmindfilter] Going to load %s", pszFileName);

		IStream *stream;
		USES_CONVERSION;
		HRESULT hr = SHCreateStreamOnFile(pszFileName, STGM_READ, &stream);
		if (FAILED(hr))
		{
			ATLTRACE2("[xmindfilter] [Load] SHCreateStreamOnFile failed: %d", hr);
			return hr;
		}

		hr = Load(stream);
		stream->Release();
		return hr;
	}

	virtual HRESULT STDMETHODCALLTYPE Load(/* [unique][in] */ __RPC__in_opt IStream *pStm)
	{
		STATSTG stat;
		HRESULT hr;
		
		// allocate buffer for the zip
#ifdef _DEBUG
		DWORD statFlag = 0;
#else
		DWORD statFlag = STATFLAG_NONAME;
#endif

		hr = pStm->Stat(&stat, statFlag);
		if (FAILED(hr))
		{
			ATLTRACE2("[xmindfilter] [Load] Stat failed: %d", hr);
			goto cleanup;
		}
		if (stat.cbSize.HighPart > 0)
		{
			ATLTRACE2("[xmindfilter] [Load] Error: file too large - stat.cbSize.HighPart > 0"); // FIXME?
			hr = E_FAIL;
			goto cleanup;
		}

		if (stat.pwcsName != NULL)
		{
			ATLTRACE2(L"[xmindfilter] Going to load %s (%d bytes)", stat.pwcsName, stat.cbSize.LowPart);
		}
		else
		{
			ATLTRACE2(L"[xmindfilter] Going to load a %d byte stream", stat.cbSize.LowPart);
		}

		if (m_zip_buffer != NULL)
			free(m_zip_buffer);
		m_zip_buffer = NULL;
		m_zip_buffer = (BYTE*) malloc(stat.cbSize.LowPart);
		if (m_zip_buffer == NULL)
		{
			ATLTRACE2("[xmindfilter] [Load] malloc(m_zip_buffer, %d) failed", stat.cbSize.LowPart);
			hr = E_OUTOFMEMORY;
			goto cleanup;
		}

		// read the zip
		BYTE* zip_buffer_p = m_zip_buffer;
		DWORD toRead = stat.cbSize.LowPart;
		while (toRead > 0)
		{
			ULONG read = 0;
			hr = pStm->Read(zip_buffer_p, toRead, &read);
			if (FAILED(hr))
			{
				ATLTRACE2("[xmindfilter] [Load] Error: Read failed: %d", hr);
				goto cleanup;
			}

			toRead -= read;
			zip_buffer_p += read;
		}
		
		// open zip
		if (m_zip != NULL) {
			CloseZip(m_zip);
			m_zip = NULL;
		}
		m_zip = OpenZip(m_zip_buffer, stat.cbSize.LowPart, NULL);
		if (m_zip == NULL) {
			ATLTRACE2("[xmindfilter] [Load] Error: OpenZip failed: %d", ::GetLastError());
			hr = E_FAIL;
			goto cleanup;
		}

		// find content.xml
		ZRESULT zr;
		ZIPENTRY ze;
		int i;
		zr = FindZipItem(m_zip, L"content.xml", true, &i, &ze);
		if (zr != ZR_OK) {
			ATLTRACE2("[xmindfilter] [Load] Error: FindZipItem(content.xml) failed: %d", zr);
			hr = E_FAIL;
			goto cleanup;
		}

		// load content.xml
		if (m_contents_buffer != NULL)
			free(m_contents_buffer);
		m_contents_buffer = NULL;
		m_contents_buffer_len = ze.unc_size;
		m_contents_buffer = (BYTE*)malloc(ze.unc_size);
		if (m_zip_buffer == NULL)
		{
			ATLTRACE2("[xmindfilter] [Load] malloc(m_contents_buffer, %d) failed", ze.unc_size);
			hr = E_OUTOFMEMORY;
			goto cleanup;
		}

		zr = UnzipItem(m_zip, i, m_contents_buffer, ze.unc_size);
		if (zr != ZR_OK) {
			ATLTRACE2("[xmindfilter] [Load] Error: UnzipItem(content.xml) failed: %d", zr);
			hr = E_FAIL;
			goto cleanup;
		}

		hr = S_OK;
	cleanup:

		return hr;
	}

	virtual HRESULT STDMETHODCALLTYPE Save(/* [unique][in] */ __RPC__in_opt IStream *pStm, /* [in] */ BOOL fClearDirty) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE GetSizeMax(/* [out] */ __RPC__out ULARGE_INTEGER *pcbSize) { return E_NOTIMPL; }
	virtual  SCODE STDMETHODCALLTYPE  BindRegion(FILTERREGION origPos, REFIID riid, void ** ppunk) { return E_NOTIMPL; }
	virtual  SCODE STDMETHODCALLTYPE  IsDirty() { return E_NOTIMPL; }

	virtual  SCODE STDMETHODCALLTYPE  Save(LPCWSTR pszFileName, BOOL fRemember)  { return E_NOTIMPL; }
	virtual  SCODE STDMETHODCALLTYPE  SaveCompleted(LPCWSTR pszFileName)  { return E_NOTIMPL; }
	virtual  SCODE STDMETHODCALLTYPE  GetCurFile(LPWSTR  * ppszFileName)  { return E_NOTIMPL; }


private:
	BYTE* m_zip_buffer;
	BYTE* m_contents_buffer; DWORD m_contents_buffer_len;
	HZIP m_zip;

	CComPtr<IFilter> htmlIFilter;

	static XMindContentsXSLT xmindXSLT; // so that the XSL is only parsed once.
};

OBJECT_ENTRY_AUTO(__uuidof(Search), CFilterHandler)
