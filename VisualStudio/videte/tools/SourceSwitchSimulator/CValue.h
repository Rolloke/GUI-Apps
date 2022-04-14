// CValue.h: interface for the CValue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CVALUE_H__CFCFDFAA_3361_4447_98B7_AE6A5586F520__INCLUDED_)
#define AFX_CVALUE_H__CFCFDFAA_3361_4447_98B7_AE6A5586F520__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CValue  
{
public:
	int GetSyncCounter();
	CValue(int nValue, int nField, int nSyncCounter, COLORREF col);
	CValue();
	virtual ~CValue();

	COLORREF GetColor();
	int GetInt();
	int GetField();

private:
	int			m_nValue;
	int			m_nField;
	int			m_nSyncCounter;
	COLORREF	m_col;
};

typedef CValue * CValuePtr;
WK_PTR_ARRAY_CS(CValuePtrArray, CValuePtr, CValuePtrArrayCS)

#endif // !defined(AFX_CVALUE_H__CFCFDFAA_3361_4447_98B7_AE6A5586F520__INCLUDED_)
