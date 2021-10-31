#if !defined(AFX_LISTDLG_H__ABC60B21_C190_11D2_9E4E_0000B458D891__INCLUDED_)
#define AFX_LISTDLG_H__ABC60B21_C190_11D2_9E4E_0000B458D891__INCLUDED_

#include "resource.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CListDlg 
#define ONINITDIALOG             0
#define ONRCLICKLSTLIST          1
#define ONDBLCLKLSTLIST          2
#define ONOK                     3
#define ONENDLABELEDITLSTLIST    4
#define ONBEGINLABELEDITLSTLIST  5
#define ONBEGINDRAGLSTLIST       6
#define ONSIZE                   7
#define ONENDDRAGLSTLIST         8

#define MK_INLIST 0x1000

#ifndef ListView_SetCheckState
   #define ListView_SetCheckState(hwndLV, i, fCheck) \
      ListView_SetItemState(hwndLV, i, \
      INDEXTOSTATEIMAGEMASK((fCheck)+1), LVIS_STATEIMAGEMASK)
#endif

class AFX_EXT_CLASS CListDlg : public CDialog
{
// Konstruktion
public:
	CListDlg(CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~CListDlg();             // StandardDestruktor

// Dialogfelddaten
	//{{AFX_DATA(CListDlg)
	enum { IDD = IDD_LISTDLG };
	CListCtrl	m_List;
	CString	m_strSelected;
	//}}AFX_DATA
   int (*m_pCallBack)(CListDlg*, int nReason);
   void *m_pParam;
   int   m_nCursel;
   int   m_nSubItem;
   CSize m_szSize;
   bool  m_bResize;
   bool  m_bOkButton;
   NMHDR*m_pNMHdr;
   CImageList*m_pDragImage; 
private:
   static long gm_lOldListWndProc;
   static long gm_lOldEditCtrlWndProc;
   static LRESULT CALLBACK ListSubClassProc(HWND, UINT, WPARAM, LPARAM);
   static LRESULT CALLBACK EditCtrlWndProc(HWND, UINT, WPARAM, LPARAM);

	void EndDrag(UINT, CPoint, UINT);
	void DragLeave();
	void DeleteDragImage();
   CSize  m_szButton;
   CPoint m_ptOk;
// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CListDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	afx_msg void OnDblclkLstList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	virtual void OnOK();
	afx_msg void OnRclickLstList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditLstList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeleditLstList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBegindragLstList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBeginrdragLstList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_LISTDLG_H__ABC60B21_C190_11D2_9E4E_0000B458D891__INCLUDED_
