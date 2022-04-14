// TAG.h: interface for the CTAG class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TAG_H__D52C8E84_D0C4_11D4_9983_004005A19028__INCLUDED_)
#define AFX_TAG_H__D52C8E84_D0C4_11D4_9983_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTAG  
{
public:
	CTAG(const CString& sTag, BOOL bClose=TRUE);
	virtual ~CTAG();

private:
	CString m_sTAG;
};

#endif // !defined(AFX_TAG_H__D52C8E84_D0C4_11D4_9983_004005A19028__INCLUDED_)
