// OEMSkinOptionsDlg.h : Headerdatei
//

#pragma once
#include "..\..\lib\Skins\SkinDialog.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "afxdtctl.h"
#include "..\..\lib\skins\skinbutton.h"
#include "..\..\lib\skins\skinstatic.h"
#include "..\..\lib\skins\skingroupbox.h"

enum eControlType;
// COEMSkinOptionsDlg Dialogfeld
class COEMSkinOptionsDlg : public CSkinDialog
{
// Konstruktion
public:
	COEMSkinOptionsDlg(CWnd* pParent = NULL);	// Standardkonstruktor

// Dialogfelddaten

	enum { IDD = IDD_OEMSKINOPTIONS_DIALOG };
	protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	virtual void OnOK();
	virtual BOOL GetToolTip(UINT nID, CString&sText);


// Implementierung
protected:
	COLORREF ChooseColor(COLORREF colInit);
	DWORD ChooseShape(DWORD colInit);
	DWORD ChooseSurface(DWORD colInit);
	DWORD ChooseBkGndStyle(DWORD colInit);
	CWnd * GetDlgItem(CRuntimeClass*pRC, CWnd*pWndPrevious=NULL);
	void LoadOEM(BOOL bApply=FALSE);
	void SaveOEM();
	void ApplyProperty(eControlType nImage, CString sItem, DWORD dwData, DWORD dwFlags=0);
	HTREEITEM FindTreeItem(int nItemImg);
	HTREEITEM FindTreeItem(HTREEITEM hItem, CString sText);

protected:
	HICON m_hIcon;
    LARGE_INTEGER                  m_liOldIdleTime;
    LARGE_INTEGER                  m_liOldSystemTime;

	// Generierte Funktionen für die Meldungstabellen
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnNMRclickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFileLoad();
	afx_msg void OnUpdateFileLoad(CCmdUI *pCmdUI);
	afx_msg void OnFileSaveas();
	afx_msg void OnUpdateFileSaveas(CCmdUI *pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
	afx_msg void OnTestEnableok();
	afx_msg void OnUpdateTestEnableok(CCmdUI *pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnIcon();
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	CSkinListBox m_ListBox;
//	CListBox m_ListBox;
	CSkinProgress m_progress;
	CSkinTree m_Tree;
	CSkinSlider m_Slider;
	CFilteredSkinEdit m_Edit;
	CSkinListCtrlX m_ListCtrl;
	CSkinComboBox m_Combo;
	CSkinScrollBar m_Scrollbar;
	CSkinDateTimeCtrl m_DateTimeCtrl;
	CSkinSpinButtonCtrl m_SpinButton;
	CSkinButton m_BtnCancel;
	CSkinStatic m_Static;
	CSkinMenu m_Context;
	CSkinGroupBox m_GroupBox;
	CString m_sInifile;
	CSkinButton m_btnIcon;
};
