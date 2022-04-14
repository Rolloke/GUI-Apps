
// SchrittmotorLPTDlg.h: Headerdatei
//

#pragma once

#include "PlotterDriver.h"
#include "PostscriptInterpreter.h"
#include "afxwin.h"

// CSchrittmotorLPTDlg-Dialogfeld
class CSchrittmotorLPTDlg : public CDialog
{
// Konstruktion
public:
	CSchrittmotorLPTDlg(CWnd* pParent = NULL);	// Standardkonstruktor

// Dialogfelddaten
	enum { IDD = IDD_SCHRITTMOTORLPT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung


// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
    afx_msg void OnDestroy();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnBnClickedBtnLeft();
    afx_msg void OnBnClickedBtnRight();
    afx_msg void OnBnClickedBtnCalibrate();
    afx_msg void OnBnClickedBtnLoadPsFile();
	DECLARE_MESSAGE_MAP()

private:
    PlotterDriver mMotorDriver;
    CPlotterDC    mPlotterDC;
    int mDelay_ms;
    int mTestSteps;
    float mDistanceX;
    float mDistanceY;
    float mDistanceZ;
    CStatic mTestDraw;
    PostscriptInterpreter mInterpreter;
public:
    afx_msg void OnBnClickedBtnPlott();
};
