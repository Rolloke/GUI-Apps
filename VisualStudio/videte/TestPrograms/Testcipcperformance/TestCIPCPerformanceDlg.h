// TestCIPCPerformanceDlg.h : Headerdatei
//

#pragma once

#include "resource.h"


#include "wk_CriticalSection.h"

// CTestCIPCPerformanceDlg Dialogfeld
class CTestCIPCPerformanceDlg : public CDialog
{
	friend class CIPCTestPerformance;

// Konstruktion
public:
	CTestCIPCPerformanceDlg(CWnd* pParent = NULL);	// Standardkonstruktor
	virtual ~CTestCIPCPerformanceDlg();	// Standarddestruktor

// Dialogfelddaten
	enum { IDD = IDD_TESTCIPCPERFORMANCE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung

	void TraceSystemInfos(void);

// Implementierung
protected:
	HICON m_hIcon;
	CIPCTestPerformance*m_pCIPC;
	UINT m_nCountTimer;
	void *m_pData;
	CWinThread *m_pThread;

	long m_nBubbleSize;
	int m_nBubblesPerSecond;
	volatile int m_nBubblesCount;
	int m_nBubblesPerSecondRes;
	BOOL m_bCheck;
	BOOL m_bCalcChecksum;
	BOOL m_bInitRandomNumbers;
	CWK_Criticalsection m_cs;

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedBtnTest();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedCkCalcCheckSum();
	afx_msg void OnBnClickedCkInitRandom();
	afx_msg void OnBnClickedBtnClearDebugOut();
	DECLARE_MESSAGE_MAP()
	
	UINT ThreadProc();
	static UINT ThreadProc(LPVOID pParam);
public:
};
