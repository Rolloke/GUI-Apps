// StringArrayOnce.h: interface for the CStringArrayOnce class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRINGARRAYONCE_H__CE2DA235_7526_11D5_BB45_0050BF49CEBF__INCLUDED_)
#define AFX_STRINGARRAYONCE_H__CE2DA235_7526_11D5_BB45_0050BF49CEBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStringArrayOnce : public CStringArray  
{
public:
	BOOL	AddOnce(CString& sEntry);
};

#endif // !defined(AFX_STRINGARRAYONCE_H__CE2DA235_7526_11D5_BB45_0050BF49CEBF__INCLUDED_)
