// AcdcException.h: interface for the CAcdcException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACDCEXCEPTION_H__D3CC00B9_4D46_4F88_8502_33A313C4D2FD__INCLUDED_)
#define AFX_ACDCEXCEPTION_H__D3CC00B9_4D46_4F88_8502_33A313C4D2FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAcdcException : public CUnhandledException  
{
public:
	CAcdcException(WK_ApplicationId AppId, PVOID pContext);
	virtual ~CAcdcException();

	virtual LONG OnExceptionHandler(LPEXCEPTION_POINTERS pExc);

//members
private:

	BOOL m_bHandleCurrentException;
};


#endif // !defined(AFX_ACDCEXCEPTION_H__D3CC00B9_4D46_4F88_8502_33A313C4D2FD__INCLUDED_)
