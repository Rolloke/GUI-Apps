// DibData.h: interface for the CDibData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIBDATA_H__1332BE53_47BB_11D2_B5A9_00C095EC9EFA__INCLUDED_)
#define AFX_DIBDATA_H__1332BE53_47BB_11D2_B5A9_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CDibData  
{
	// construction / destruction
public:
	CDibData(CDib* pDib,
			 int iNr,
			 const CString sPicture,
			 const CString& sData,
			 const CString& sComment,
			 WORD  wSeqNr = 0);
	CDibData(CDib* pDib,
			 const CString sPicture,
			 const CString& sData,
			 const CString& sComment);
	CDibData(CDib* pDib);
	virtual ~CDibData();

	// attributes
public:
	inline CDib* GetDib() const;
	inline int   GetPicNr() const;
	inline WORD	 GetSeqNr() const;

	// operations
public:
	LPCTSTR GetTooltipText();
	inline CString	GetPicture() const;
	inline CString	GetData() const;
	inline CString	GetComment() const;

	// implementation
protected:

	// data member
private:
	CDib*   m_pDib;
	int		m_iPicNr;
	WORD	m_wSeqNr;
	CString m_sToolTip;
	CString m_sPicture;
	CString m_sData;
	CString m_sComment;
};
//////////////////////////////////////////////////////////////////////
inline CDib* CDibData::GetDib() const
{
	return m_pDib;
}
//////////////////////////////////////////////////////////////////////
inline int CDibData::GetPicNr() const
{
	return m_iPicNr;
}
//////////////////////////////////////////////////////////////////////
inline WORD CDibData::GetSeqNr() const
{
	return m_wSeqNr;
}
//////////////////////////////////////////////////////////////////////
inline CString CDibData::GetPicture() const
{
	return m_sPicture;
}
//////////////////////////////////////////////////////////////////////
inline CString CDibData::GetData() const
{
	return m_sData;
}
//////////////////////////////////////////////////////////////////////
inline CString CDibData::GetComment() const
{
	return m_sComment;
}
//////////////////////////////////////////////////////////////////////
typedef CDibData* CDibDataPtr;
WK_PTR_ARRAY(CDibDataArray,CDibDataPtr);
//////////////////////////////////////////////////////////////////////
class CDibDatas : public CDibDataArray
{
	// construction / destruction
public:
	CDibDatas();
	virtual ~CDibDatas();

public:
	inline int GetCurrentPicNr() const;
	inline int GetCurrentSeqNr() const;

	// operations
public:
	void Add(CDibData* pDibData, int nNewSize);
	void DeleteAllExceptCurrent();

private:
	int m_iCurrentPicNr;
	int m_iCurrentSeqNr;
};
//////////////////////////////////////////////////////////////////////
inline int CDibDatas::GetCurrentPicNr() const
{
	return m_iCurrentPicNr;
}
//////////////////////////////////////////////////////////////////////
inline int CDibDatas::GetCurrentSeqNr() const
{
	return m_iCurrentSeqNr;
}



#endif // !defined(AFX_DIBDATA_H__1332BE53_47BB_11D2_B5A9_00C095EC9EFA__INCLUDED_)
