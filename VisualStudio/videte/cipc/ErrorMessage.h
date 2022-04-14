// ErrorMessage.h: interface for the CErrorMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ERRORMESSAGE_H__728C37A3_1804_11D3_B5F2_004005A19028__INCLUDED_)
#define AFX_ERRORMESSAGE_H__728C37A3_1804_11D3_B5F2_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CErrorMessage  
{
	// construction / destruction
public:
	CErrorMessage(const CString& sServer, const CString& sMessage)
	{
		m_sServer = sServer;
		m_sMessage = sMessage;
	}
	virtual ~CErrorMessage()
	{
	}

	// attributes
public:
	inline CString GetServer() const;
	inline CString GetMessage() const;

private:
	CString m_sServer;
	CString m_sMessage;
};
//////////////////////////////////////////////////////////////////////
inline CString CErrorMessage::GetServer() const
{
	return m_sServer;
}
//////////////////////////////////////////////////////////////////////
inline CString CErrorMessage::GetMessage() const
{
	return m_sMessage;
}
//////////////////////////////////////////////////////////////////////
typedef CErrorMessage* CErrorMessagePtr;
WK_PTR_LIST_CS(CErrorMessageList,CErrorMessagePtr,CErrorMessages);
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_ERRORMESSAGE_H__728C37A3_1804_11D3_B5F2_004005A19028__INCLUDED_)
