// CValue.h: interface for the CValue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CVALUES_H__2B8EB948_7E23_4EB1_B31E_FA9A8D679988__INCLUDED_)
#define AFX_CVALUES_H__2B8EB948_7E23_4EB1_B31E_FA9A8D679988__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CValue.h"

class CValues : public CValuePtrArrayCS
{
public:
	int GetNr();
	CString GetName();
	void Init(const CString sName, int nNr);
	CValues();
	virtual ~CValues();

private:
	CString m_sName;
	int		m_nNr;
};

#endif // !defined(AFX_CVALUES_H__2B8EB948_7E23_4EB1_B31E_FA9A8D679988__INCLUDED_)
