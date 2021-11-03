#if !defined(AFX_TIMERDLG_H__B9A24220_0C8F_4F01_BA09_100577933114__INCLUDED_)
#define AFX_TIMERDLG_H__B9A24220_0C8F_4F01_BA09_100577933114__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimerDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTimerDlg 

class CTimerDlg : public CDialog
{
// Konstruktion
public:
	void CopyDate(StructOfDates&, StructOfDates&);
	CTimerDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CTimerDlg)
	enum { IDD = IDD_TIMER_DLG };
	CSliderCtrl	m_cControl;
	CListCtrl   m_cList;
	//}}AFX_DATA
   StructOfDates m_Date;
   CPtrList *m_pListDates;
   int       m_nActiveDate;
   bool      m_bChanged;
// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CTimerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	void UpdateActiveDate();
	void SelectActiveDate();
	void SaveDate();

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CTimerDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnGetdaystateTimerCalendar(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnTimerPath();
	afx_msg void OnTimerBtnSet();
	afx_msg void OnGetdispinfoTimerList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusTimerMessage();
	afx_msg void OnClickTimerList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimerBtnNew();
	afx_msg void OnTimerBtnDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TIMERDLG_H__B9A24220_0C8F_4F01_BA09_100577933114__INCLUDED_
