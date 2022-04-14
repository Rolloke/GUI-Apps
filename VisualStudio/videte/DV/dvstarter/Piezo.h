// Piezo.h: interface for the CPiezo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIEZO_H__BFB6DE26_5356_11D5_9004_004005A11E32__INCLUDED_)
#define AFX_PIEZO_H__BFB6DE26_5356_11D5_9004_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPiezo  
{
public:
	CPiezo();
	virtual ~CPiezo();

	// operations
	void InitPiezo();
	void ExitPiezo();
	void SetPiezo(BOOL bOn);
	BOOL PiezoSetFlag(DWORD dwPiezoFlag);
	BOOL PiezoClearFlag(DWORD dwPiezoFlag);

private:
	HANDLE  m_hJaCobPiezo;
	HANDLE  m_hSaVicPiezo;
	HANDLE  m_hTashaPiezo;
	DWORD	m_dwPiezoFlags;
};

#endif // !defined(AFX_PIEZO_H__BFB6DE26_5356_11D5_9004_004005A11E32__INCLUDED_)
