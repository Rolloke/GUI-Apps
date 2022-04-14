#ifndef CERRLOGREPORTER_H_DEFINED
#define CERRLOGREPORTER_H_DEFINED

#include <Qedit.h>

typedef HRESULT (CALLBACK* CErrReporter_LogErrorFnc)(LONG Severity, BSTR ErrorString, LONG ErrorCode, HRESULT hresult, VARIANT *pExtraInfo);

class AFX_EXT_CLASS CErrReporter : public IAMErrorLog
{
public:
	CErrReporter();
	void	SetLogFunction(CErrReporter_LogErrorFnc pFunc);

    // IUnknown
    STDMETHOD(QueryInterface(REFIID, void**));
    STDMETHOD_(ULONG, AddRef());
    STDMETHOD_(ULONG, Release());


	// IAMErrorLog
    STDMETHOD(LogError(LONG, BSTR, LONG, HRESULT, VARIANT*));

protected:
    long    m_lRef; // Reference count.
	CErrReporter_LogErrorFnc m_pLogFunction;
};

#endif // CERRLOGREPORTER_H_DEFINED