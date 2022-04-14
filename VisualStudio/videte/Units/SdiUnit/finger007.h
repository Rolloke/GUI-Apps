// Finger007.h: interface for the CFinger007 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FINGER007_H__29F926B0_E4C6_4804_923B_52592A9B9CAA__INCLUDED_)
#define AFX_FINGER007_H__29F926B0_E4C6_4804_923B_52592A9B9CAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStarInterface;

class CFinger007  
{
public:
	CFinger007(CStarInterface* pStarInterface,
			   short wWorkIndex,
			   LPCTSTR szBoardIndex,
			   LPCTSTR szCom);
	virtual ~CFinger007();

	// attributes
public:
	inline short   GetWorkIndex() const;
	inline CString GetBoardIndex() const;
		   short   GetBoardIndexShort() const;

	CString GetDate();
	CString GetTime();
	
	// operations
public:

	// implementation
protected:
	void PrepareCMD();

private:
	CStarInterface* m_pStarInterface;
	short			m_wWorkIndex;
	CString			m_sBoardIndex;
	CString			m_sCOM;
};
///////////////////////////////////////////////////////////////////////////
inline short CFinger007::GetWorkIndex() const
{
	return m_wWorkIndex;
}
///////////////////////////////////////////////////////////////////////////
inline CString CFinger007::GetBoardIndex() const
{
	return m_sBoardIndex;
}

#endif // !defined(AFX_FINGER007_H__29F926B0_E4C6_4804_923B_52592A9B9CAA__INCLUDED_)
