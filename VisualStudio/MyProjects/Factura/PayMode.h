#if !defined(AFX_PAYMODE_H__7E681CC0_DE3D_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_PAYMODE_H__7E681CC0_DE3D_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PayMode.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPayMode 


class CPayMode : public CDialog
{
// Konstruktion
public:
	CPayMode(CWnd* pParent = NULL);   // Standardkonstruktor
	void InitPayModeData();
	void SaveChangedData(bool bAsk = true);

// Dialogfelddaten
	//{{AFX_DATA(CPayMode)
	enum { IDD = IDD_PAY_MODE };
	CString	m_strEdit;
	int		m_nIndex;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPayMode)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CPayMode)
	afx_msg void OnPaymodeNew();
	afx_msg void OnPaymodeDelete();
	afx_msg void OnDeltaposPaymodeSpin(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnChangePaymodeEdit();
	afx_msg void OnPaymodeSave();
	virtual void OnCancel();
	virtual void OnOK();
   afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CPtrList *m_pPayModes;
	long      m_nPayMode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PAYMODE_H__7E681CC0_DE3D_11D3_B6EC_0000B458D891__INCLUDED_
