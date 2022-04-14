// TranslateDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTranslateDialog dialog
#ifndef _TRANSLATE_DIALOG_H_
#define _TRANSLATE_DIALOG_H_

#include "afxwin.h"
class CTranslateDialog : public CDialog
{
// Construction
public:
	CTranslateDialog(const CString& sOriginal,
					 const CString& sSuggestion,
					 const CString& sTranslation,
					 const UINT uMaxLength,
					 const CString& sComment,
					 BOOL  bEnableEdit,
					 BOOL  CheckDataAtStart,
					 CWnd* pParent = NULL);   // standard constructor

// Attributes
public:
	inline const CString&	GetOriginal();
	inline const CString&	GetTranslation();
	inline const UINT&		GetMaxLength();
	inline const CString&	GetComment();

// Overrides
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTranslateDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
protected:
	BOOL	CheckData();

	// Generated message map functions
	//{{AFX_MSG(CTranslateDialog)
	afx_msg void OnCopy();
	afx_msg void OnButtonSuggestion();
	virtual void OnOK();
	afx_msg void OnChangeEditTranslate();
	afx_msg void OnChangeEditComment();
	afx_msg void OnChangeEditMaxLength();
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnFont();
	afx_msg void OnBnClickedBtnSearchConvert();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog Data
	//{{AFX_DATA(CTranslateDialog)
	enum { IDD = IDD_EDIT };
	CEdit	m_editSuggestion;
	CEdit	m_editOriginal;
	CEdit	m_editActualLength;
	CStatic	m_txtActualLength;
	CStatic	m_txtMaxLength;
	CEdit	m_editMaxLength;
	CEdit	m_editComment;
	CEdit	m_editTranslation;
	CString	m_sOriginal;
	CString	m_sTranslation;
	CString	m_sSuggestion;
	UINT	m_uMaxLength;
	CString	m_sComment;
	UINT	m_uActualLength;
	//}}AFX_DATA
	BOOL	m_bEnableEdit;
	BOOL	m_bCheckDataAtStart;
public:
	CString m_sResourceTypeName;
};
/////////////////////////////////////////////////////////////////////////////
const CString& CTranslateDialog::GetOriginal() 
{
	return m_sOriginal;
}
/////////////////////////////////////////////////////////////////////////////
const CString& CTranslateDialog::GetTranslation() 
{
	return m_sTranslation;
}
/////////////////////////////////////////////////////////////////////////////
const UINT& CTranslateDialog::GetMaxLength() 
{
	return m_uMaxLength;
}
/////////////////////////////////////////////////////////////////////////////
const CString& CTranslateDialog::GetComment() 
{
	return m_sComment;
}
#endif //_TRANSLATE_DIALOG_H_