#if !defined(AFX_TREEDLG_H__901EAF00_C283_11D2_9E4E_0000B458D891__INCLUDED_)
#define AFX_TREEDLG_H__901EAF00_C283_11D2_9E4E_0000B458D891__INCLUDED_

#include "resource.h"
#include "ListDlg.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTreeDlg 
#define ONRCLICKTREETREECTRL 4

class AFX_EXT_CLASS CTreeDlg : public CDialog
{
// Konstruktion
public:

	CTreeDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CTreeDlg)
	enum { IDD = IDD_TREEDLG };
	CTreeCtrl	m_TreeCtrl;
	//}}AFX_DATA
   TVITEM   m_TvItem;
   int      (*m_pCallBack)(CTreeDlg*, int nReason);
   void      *m_pParam;
   void     (*m_pHelp)  (CTreeDlg*, HELPINFO *);
   void      *m_pParamHelp;
   CSize      m_szSize;
   bool       m_bResize;
   bool       m_bOkButton;
   CString    m_strWndTitle;
   CImageList m_ImageList;
   NMHDR     *m_pNMHdr;
   CImageList*m_pDragImage; 

private:
   CSize  m_szButton;
   CPoint m_ptOk;
   static long gm_lOldListWndProc;
   static LRESULT CALLBACK ListSubClassProc(HWND, UINT, WPARAM, LPARAM);

	void EndDrag(UINT, CPoint, UINT);
	void DragLeave();
	void DeleteDragImage();

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CTreeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CTreeDlg)
	afx_msg void OnDblclkTreectrl(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	afx_msg void OnRclickTreeTreectrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnBegindragTreeTreectrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeleditTreeTreectrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginrdragTreeTreectrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitemTreeTreectrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditTreeTreectrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnOK();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_TREEDLG_H__901EAF00_C283_11D2_9E4E_0000B458D891__INCLUDED_
