/* GlobalReplace: m_iNumStandardlMacros --> m_iNumStandardMacros */
/* GlobalReplace: m_iNumNormalMacros --> m_iNumStandardMacros */
/* GlobalReplace: m_sPrototProcessMacroCount --> m_sProtorProcessMacroCount */
/* GlobalReplace: m_sProcessMacroCount --> m_sProtorProcessMacroCount */
// ProtorDlg.h : header file
//

#if !defined(AFX_PROTORDLG_H__12EA3EA8_7270_11D1_B8CD_0060977A76F1__INCLUDED_)
#define AFX_PROTORDLG_H__12EA3EA8_7270_11D1_B8CD_0060977A76F1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "WK_String.h"
#include "PictureDef.h"

/////////////////////////////////////////////////////////////////////////////
// CProtorDlg dialog

class CProtorDlg : public CDialog
{
// Construction
public:
	void AddProcessMacro(
			const CWK_String & sName,
			const CWK_String & sType,
			DWORD dwMainTime, 
			DWORD dwRecTime, 
			DWORD dwPauseTime,
			WORD wNrPict,
			BOOL bInterval, 
			BOOL bHold, 
			Resolution resolution,
			Compression compression
		);
	void ScanProcessMacros();
	void WriteProcessMacros();

	CProtorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CProtorDlg)
	enum { IDD = IDD_PROTOR_DIALOG };
	CString	m_sProtorProcessMacroCount;
	CString	m_sNormalProcessMacroCount;
	CString	m_sProcessMacroCount;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProtorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CProtorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAddProtorMacro();
	afx_msg void OnAddStandardMacros();
	afx_msg void OnRemoveProtorMacro();
	afx_msg void OnRemoveStandardMacros();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void DeleteActivationByPattern(const CWK_String &sPattern);
	BOOL ReadIo(const CString &sSectionName, 
				int inputNr, int ioNr,
				CWK_String &sOneActivation
				);
	//
	CProtorStringArray m_processMacros;
	int		m_iNumProtorMacros;
	int		m_iNumStandardMacros;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROTORDLG_H__12EA3EA8_7270_11D1_B8CD_0060977A76F1__INCLUDED_)
