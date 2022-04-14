// Request.h: interface for the CRequest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REQUEST_H__E9153FE1_935A_11D3_A870_004005A19028__INCLUDED_)
#define AFX_REQUEST_H__E9153FE1_935A_11D3_A870_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCameraGroup;
class CInputGroup;

class CRequest  
{
	// construction / destruction
public:
	CRequest();
	virtual ~CRequest();

	// attributes
public:
	inline WORD  GetID() const;
	inline int   GetNrRequests() const;
	inline WORD  GetRequest(int i) const;
	inline int   GetNextRequestIndex() const;
		   CString ToString();
	inline BOOL	 IsSingleMD() const;

	// operations
public:
	void CalcRequests(CCameraGroup* pCameraGroup, WORD wFirst);

	// implementation
protected:
	void AddCameraBalanced(WORD w);
	void AddCameraSorted(WORD w);
	int  GetCameraIndex(WORD w);

	// data member
private:
	static WORD	m_staticID;
	WORD		m_ID;
	CWordArray	m_CamSubIDs;
	int			m_iNextRequestIndex;
	BOOL		m_bIsSingleMD;
};
//////////////////////////////////////////////////////////////////////
inline int CRequest::GetNextRequestIndex() const
{
	return m_iNextRequestIndex;
}
//////////////////////////////////////////////////////////////////////
inline WORD CRequest::GetID() const
{
	return m_ID;
}
//////////////////////////////////////////////////////////////////////
inline int CRequest::GetNrRequests() const
{
	return m_CamSubIDs.GetSize();
}
//////////////////////////////////////////////////////////////////////
inline WORD CRequest::GetRequest(int i) const
{
	return m_CamSubIDs.GetData()[i];
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CRequest::IsSingleMD() const
{
	return m_bIsSingleMD;
}

#endif // !defined(AFX_REQUEST_H__E9153FE1_935A_11D3_A870_004005A19028__INCLUDED_)
