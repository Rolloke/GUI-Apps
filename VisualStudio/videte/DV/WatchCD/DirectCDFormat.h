// DirectCDFormat.h: interface for the CDirectCDFormat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTCDFORMAT_H__40B4FE65_EB4F_11D3_AE54_004005A1D890__INCLUDED_)
#define AFX_DIRECTCDFORMAT_H__40B4FE65_EB4F_11D3_AE54_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWatchCDDlg;
class CDirectCDError;


class CDirectCDFormat

{

// Construction
public:
	CDirectCDFormat(CWatchCDDlg* pWatchCD, CDirectCDError* pDCDError);
	virtual ~CDirectCDFormat();

	// Implementation
public:
	BOOL IsFormatted();
	BOOL FindFormatDiscDlg();
	BOOL FindNameYourDiscDlg();
	BOOL SetNewCDRName();
	BOOL FindFormattingDlg();
	BOOL AllFormatDlgsClosed();

	// Implementation
protected:
	void OnFormatFindDlgFormatDisc();
	void OnFormatFindDlgNameYourDisc();
	void OnFormatSetNewCDRName();
	void OnFormatFindDlgFormattingDisc();
	void OnFormatWaitForSuccess();
	void OnFormatError();

	// Implementation
private:
	void			ChangeDCDFormatStatus(DirectCDStatus DCDFormatStatus);
	static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
	static BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam);
	void CloseFormatDialogs();

	// data member
private:
	CWatchCDDlg*	m_pWatchCD;
	CDirectCDError*	m_pDCDError;
	DirectCDStatus	m_DCDFormatStatus;
	DWORD			m_dwTickCount;
	DWORD			m_dwIDEditField;
	DWORD			m_dwIDButtonFinish;
	HWND			m_hDCDFormatDiscDlg;
	DWORD			m_dwIDButtonNext;
	HWND			m_hNextButton;
	HWND			m_hEditField;
	HWND			m_hFinishButton;
	HWND			m_hDCDNameYourDiscDlg;
	HWND			m_hDCDFormattingDlg;

};

#endif // !defined(AFX_DIRECTCDFORMAT_H__40B4FE65_EB4F_11D3_AE54_004005A1D890__INCLUDED_)
