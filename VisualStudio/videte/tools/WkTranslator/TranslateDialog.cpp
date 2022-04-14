// TranslateDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WkTranslator.h"
#include "TranslateString.h"
#include "TranslateDialog.h"
#include "TranslatorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTranslateDialog dialog
extern CWkTranslatorApp theApp;


CTranslateDialog::CTranslateDialog(const CString& sOriginal,
								   const CString& sSuggestion,
								   const CString& sTranslation,
								   const UINT uMaxLength,
								   const CString& sComment,
			   					   BOOL  bEnableEdit,
								   BOOL  CheckDataAtStart,
								   CWnd* pParent /*=NULL*/)
	: CDialog(CTranslateDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTranslateDialog)
	m_sOriginal = sOriginal;
	m_sTranslation = sTranslation;
	m_sSuggestion = sSuggestion;
	m_uMaxLength = uMaxLength;
	m_sComment = sComment;
	m_uActualLength = 0;
	//}}AFX_DATA_INIT
	m_bEnableEdit = bEnableEdit;
	m_bCheckDataAtStart = CheckDataAtStart;

}


void CTranslateDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTranslateDialog)
	DDX_Control(pDX, IDC_EDIT_SUGGESTION, m_editSuggestion);
	DDX_Control(pDX, IDC_EDIT_ORIGINAL, m_editOriginal);
	DDX_Control(pDX, IDC_EDIT_ACTUAL_LENGTH, m_editActualLength);
	DDX_Control(pDX, IDC_TXT_ACTUAL_LENGTH, m_txtActualLength);
	DDX_Control(pDX, IDC_TXT_MAX_LENGTH, m_txtMaxLength);
	DDX_Control(pDX, IDC_EDIT_MAX_LENGTH, m_editMaxLength);
	DDX_Control(pDX, IDC_EDIT_COMMENT, m_editComment);
	DDX_Control(pDX, IDC_EDIT_TRANSLATE, m_editTranslation);
	DDX_Text(pDX, IDC_EDIT_ORIGINAL, m_sOriginal);
	DDX_Text(pDX, IDC_EDIT_TRANSLATE, m_sTranslation);
	DDX_Text(pDX, IDC_EDIT_SUGGESTION, m_sSuggestion);
	DDX_Text(pDX, IDC_EDIT_MAX_LENGTH, m_uMaxLength);
	DDX_Text(pDX, IDC_EDIT_COMMENT, m_sComment);
	DDX_Text(pDX, IDC_EDIT_ACTUAL_LENGTH, m_uActualLength);
	DDX_Text(pDX, IDC_TXT_RESSOURCE_TYPE_NAME, m_sResourceTypeName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTranslateDialog, CDialog)
	//{{AFX_MSG_MAP(CTranslateDialog)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_BUTTON_SUGGESTION, OnButtonSuggestion)
	ON_EN_CHANGE(IDC_EDIT_TRANSLATE, OnChangeEditTranslate)
	ON_EN_CHANGE(IDC_EDIT_COMMENT, OnChangeEditComment)
	ON_EN_CHANGE(IDC_EDIT_MAX_LENGTH, OnChangeEditMaxLength)
	ON_BN_CLICKED(IDC_BTN_FONT, OnBtnFont)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_SEARCH_CONVERT, OnBnClickedBtnSearchConvert)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTranslateDialog message handlers
void CTranslateDialog::OnCopy() 
{
	m_sTranslation = m_sOriginal;
	m_editTranslation.SetWindowText(m_sTranslation);
}
/////////////////////////////////////////////////////////////////////////////
void CTranslateDialog::OnButtonSuggestion() 
{
	m_sTranslation = m_sSuggestion;
	m_editTranslation.SetWindowText(m_sTranslation);
}
/////////////////////////////////////////////////////////////////////////////
void CTranslateDialog::OnOK() 
{
	if (CheckData())
	{
		CDialog::OnOK();
	}
	else
	{
		// do not exit
	}
}
/////////////////////////////////////////////////////////////////////////////
void CTranslateDialog::OnChangeEditTranslate() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	m_editTranslation.GetWindowText(m_sTranslation);
	m_uActualLength = m_sTranslation.GetLength();
	CString s;
	s.Format(_T("%d"),m_uActualLength);
	m_editActualLength.SetWindowText(s);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTranslateDialog::CheckData() 
{
	BOOL bReturn = TRUE;
	CString sChars;
	if (!CTranslateString::CheckFormatSpecifier(m_sOriginal, m_sTranslation))
	{
		AfxMessageBox(IDP_ERROR_FORMAT_SPECIFIER, MB_ICONSTOP);
		bReturn = FALSE;
	}
	else if (!CTranslateString::CheckSpecialStartChars(m_sOriginal, m_sTranslation, sChars))
	{
		CString sMsg;
		sMsg.Format(IDP_ERROR_SPECIAL_START_CHARS, sChars);
		AfxMessageBox(sMsg, MB_ICONSTOP);
		m_editTranslation.SetSel(0, 0, FALSE);
		m_editTranslation.SetFocus();
		bReturn = FALSE;
	}
	else if (!CTranslateString::CheckSpecialCharsToAvoid(m_sOriginal, m_sTranslation, sChars))
	{
		CString sMsg;
		sMsg.Format(IDP_ERROR_SPECIAL_CHARS_TO_AVOID, sChars);
		AfxMessageBox(sMsg, MB_ICONSTOP);
		m_editTranslation.SetSel(0, 0, FALSE);
		m_editTranslation.SetFocus();
		bReturn = FALSE;
	}
	else if (!CTranslateString::CheckMaxLength(m_sTranslation, m_uMaxLength))
	{
		if (IDCANCEL == AfxMessageBox(IDP_ERROR_MAX_LENGTH,
									  MB_ICONSTOP|MB_OKCANCEL|MB_DEFBUTTON2)
			)
		{
			// mark exceeding text in edit field
			// Ignore single '&' (Accelerator) in text,
			// but consider double '&&' as one character
			CString sText = m_sTranslation;
			sText.Replace(_T("&&"), _T("&"));
			int iIndex = sText.Find('&');
			int iMaxIndex = (int)m_uMaxLength;
			while (   iIndex != -1
				   && iIndex < iMaxIndex)
			{
				iMaxIndex++;
				iIndex = sText.Find('&', iIndex);
			}
			m_editTranslation.SetSel(iMaxIndex, -1, FALSE);
			m_editTranslation.SetFocus();
			bReturn = FALSE;
		}
	}

	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CTranslateDialog::OnChangeEditComment() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
}
/////////////////////////////////////////////////////////////////////////////
void CTranslateDialog::OnChangeEditMaxLength() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTranslateDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (m_bEnableEdit)
	{
		m_editMaxLength.SetReadOnly(FALSE);
		m_editComment.SetReadOnly(FALSE);
	}
	else 
	{
		if (m_uMaxLength == 0)
		{
			m_txtMaxLength.ShowWindow(SW_HIDE);
			m_editMaxLength.ShowWindow(SW_HIDE);
			m_txtActualLength.ShowWindow(SW_HIDE);
			m_editActualLength.ShowWindow(SW_HIDE);
		}
		if (m_sComment.IsEmpty())
		{
			m_editComment.ShowWindow(SW_HIDE);
		}
	}

	m_uActualLength = m_sTranslation.GetLength();
	CString s;
	s.Format(_T("%d"),m_uActualLength);
	m_editActualLength.SetWindowText(s);
	
	m_editTranslation.SetFocus();
	m_editTranslation.SetSel(0, -1, TRUE);

	if (m_bCheckDataAtStart)
	{
		// Means CheckData only
		// If user accepts e.g. max length exceed, close dialog immediatly,
		// so handle as if OK-Button was pressed
		// Make dialog visible first
		ShowWindow(SW_SHOW);
		OnOK();
	}

	theApp.SetFontForChildren(m_hWnd);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTranslateDialog::OnBtnFont() 
{
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_VIEW_EDITFONT, (LPARAM)m_hWnd);
}

void CTranslateDialog::OnBnClickedBtnSearchConvert()
{
	UpdateData();
	CTranslatorDlg dlg;
	dlg.m_sOrgText = m_sOriginal;
	if (dlg.DoModal() ==IDOK)
	{
		m_sTranslation = dlg.m_sTransText;
		UpdateData(FALSE);
	}
}
