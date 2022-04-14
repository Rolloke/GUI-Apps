#include "stdafx.h"
#include "ErrorReporter.h"

CErrReporter::CErrReporter()
{
	m_lRef = 0;
	m_pLogFunction = NULL;
}

void CErrReporter::SetLogFunction(CErrReporter_LogErrorFnc pFunc)
{
	m_pLogFunction = pFunc;
}

STDMETHODIMP_(ULONG) CErrReporter::AddRef()
{
    return InterlockedIncrement(&m_lRef);
}

STDMETHODIMP_(ULONG) CErrReporter::Release()
{
    // Store the decremented count in a temporary
    // variable. 
    ULONG uCount = InterlockedDecrement(&m_lRef);
    if (uCount == 0)
    {
        delete this;
    }
    // Return the temporary variable, not the member
    // variable, for thread safety.
    return uCount;
}

STDMETHODIMP CErrReporter::QueryInterface(REFIID riid, void **ppv)
{
    if (ppv == NULL) return E_POINTER;

    *ppv = NULL;
    if (riid == IID_IUnknown)
	{
        *ppv = static_cast<IUnknown*>(this);
	}
    else if (riid == IID_IAMErrorLog)
	{
        *ppv = static_cast<IAMErrorLog*>(this);
	}
    else 
	{
		return E_NOINTERFACE;
	}
    AddRef();
    return S_OK;
}

HRESULT CErrReporter::LogError(LONG Severity, BSTR ErrorString, LONG ErrorCode, HRESULT hresult, VARIANT *pExtraInfo)
{
	if (m_pLogFunction)
	{
		return m_pLogFunction(Severity, ErrorString, ErrorCode, hresult, pExtraInfo);
	}
	return E_POINTER;
}

