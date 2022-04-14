// FieldDialog.cpp : implementation file
//

#include "stdafx.h"
#include "recherche.h"

#include "FieldDialog.h"
#include "ImageWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFieldDialog dialog


CFieldDialog::CFieldDialog(CImageWindow* pImageWindow)
	: CWK_Dialog(CFieldDialog::IDD, pImageWindow)
{
	m_pImageWindow = pImageWindow;
	m_dwSelectedCamera = (DWORD)(-1);
	m_lLastChar = -1;
	//{{AFX_DATA_INIT(CFieldDialog)
	m_sCamera = _T("");
	m_sResolution = _T("");
	m_sDate = _T("");
	m_sTime = _T("");
	m_sCompression = _T("");
	//}}AFX_DATA_INIT
	Create(IDD,pImageWindow);

	CRect cr,vr;
	m_pImageWindow->GetClientRect(cr);
	m_pImageWindow->GetVideoClientRect(vr);
	cr.left = vr.right;
	MoveWindow(vr);

	ShowWindow(SW_SHOW);
}

void CFieldDialog::PostNcDestroy() 
{
	m_pImageWindow->m_pFieldDialog = NULL;
	delete this;
}


void CFieldDialog::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFieldDialog)
	DDX_Control(pDX, IDC_RICHEDIT_DATA, m_RichEditData);
	DDX_Control(pDX, IDC_COMBO_CAMERA, m_comboCamera);
	DDX_Text(pDX, IDC_CAMERA, m_sCamera);
	DDX_Text(pDX, IDC_RESOLUTION, m_sResolution);
	DDX_Text(pDX, IDC_DATE, m_sDate);
	DDX_Text(pDX, IDC_TIME, m_sTime);
	DDX_Text(pDX, IDC_COMPRESSION, m_sCompression);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFieldDialog, CWK_Dialog)
	//{{AFX_MSG_MAP(CFieldDialog)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_COMBO_CAMERA, OnSelchangeComboCamera)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFieldDialog message handlers

void CFieldDialog::OnCancel() 
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CFieldDialog::OnOK() 
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CFieldDialog::Update()
{
	if (WK_IS_WINDOW(m_pImageWindow))
	{
		CSize s = m_pImageWindow->GetPictureSize();
		if (s.cx>0 && s.cy>0)
		{
			m_sResolution.Format(_T("%dx%d"),s.cx,s.cy);
		}
		else
		{
			m_sResolution.Empty();
		}

		int iCamNr = m_pImageWindow->GetCameraNr();
		if (iCamNr > 0)
		{
			m_sCamera.Format(IDS_CAMERA_NR, iCamNr);
		}
		else
		{
			m_sCamera = "";
		}
		m_sDate = m_pImageWindow->GetDate();
		m_sTime = m_pImageWindow->GetTime();
		m_sCompression = m_pImageWindow->GetCompression();

		UpdateData(FALSE);
		

		int i;
		CString name,value;

		if (m_pImageWindow->IsDibWindow())
		{
			m_RichEditData.SetWindowText(m_pImageWindow->FormatData(_T(":\r\n"),_T("\r\n")));
		}
		else if (m_pImageWindow->IsDisplayWindow())
		{
			CIPCFields& fields = m_pImageWindow->GetFields();
			fields.Lock();
			CIPCField* pField;
			// start on begin of rich edit control
			m_lLastChar = -1;
			for (i=0;i<fields.GetSize();i++)
			{
				pField = fields.GetAt(i);
#ifdef _DEBUG
				if (pField->GetName() == _T("Größe"))
				{
					name = pField->GetName();
				}
				else
#endif
				{
					name = theApp.GetMappedString(pField->GetName());
				}
				if (name.IsEmpty())
				{				 
					if	(   (pField->GetName() == CIPCField::m_sfStNm)
						 || (pField->GetName() == CIPCField::m_sfCaNm)
						 || (pField->GetName() == CIPCField::m_sfInNm)
						 || (pField->GetName() == CIPCField::m_sfCurr)
						)
					{
						if (m_pImageWindow->GetServer())
						{
							name = m_pImageWindow->GetServer()->GetFieldName(pField->GetName());
						}
					}
				}
				if (!name.IsEmpty())
				{
					value = pField->ToString();
					value.TrimLeft();
					value.TrimRight();
					if (!value.IsEmpty())
					{
						// now insert it to the control
						SetTextEntry(name, CFE_BOLD);
						SetTextEntry(value, 0);
					}
				}
			} // for
			fields.Unlock();
			ClearRestOfEntries();
		}

	}
}
/////////////////////////////////////////////////////////////////////////////////
void CFieldDialog::SetTextEntry(const CString &sText, DWORD dwMask)
{
	// set entry end to search for
	CString sEntryEnd = _T("\r\n");
	// save start char for search
	long lStartChar = m_lLastChar+1;
	// save text to replace with
	CString sReplaceText = sText;

	// find next szLineEnd
	FINDTEXTEX findText;
	CHARRANGE chrgSearch;
	chrgSearch.cpMin = lStartChar;
	chrgSearch.cpMax = -1;
	findText.chrg = chrgSearch;
	findText.lpstrText = (LPTSTR)(LPCTSTR)sEntryEnd;
	long lFoundChar = m_RichEditData.FindText(0, &findText);
	long lEndChar;
	if (lFoundChar != -1) {
		// szEntryEnd found, set end char
		lEndChar = lFoundChar + sEntryEnd.GetLength() - 1;
	}
	else{
		// no more szEntryEnd found, append one befor
		sReplaceText = sEntryEnd + sReplaceText;
		lEndChar = lStartChar + sReplaceText.GetLength() -1;
	}

/*	// find text and check, if it is the same
	// first search for text with sEntryEnd
	BOOL bReplace = FALSE;
	CString sTextToSearch = sText + sEntryEnd;
	findText.lpstrText = sTextToSearch.GetBuffer(0);
	long lFoundText = m_RichEditData.FindText(0, &findText);
	sTextToSearch.ReleaseBuffer(0);

	if (lFoundText != -1) {
		// sText found, do not replace next line
		bReplace = FALSE;
	}
	else{
		// text not found, try with text only
		sTextToSearch = sText;
		findText.lpstrText = sTextToSearch.GetBuffer(0);
		lFoundText = m_RichEditData.FindText(0, &findText);
		sTextToSearch.ReleaseBuffer(0);
		if (lFoundText != -1) {
			// check if it is the actual following text
			if (lFoundText == lStartChar) {
				// and no more chars following
				long lFollowingChar = lFoundText + sText.GetLength();
				long lTextLength = m_RichEditData.GetTextLength();
				if (lFollowingChar > lTextLength) {
					// text is last text, do not replace the entry
					bReplace = FALSE;
				}
				else {
					// more chars following, replace entry
					bReplace = TRUE;
				}
			}
		}
		else{
			// text not found, replace (or insert) next entry
			bReplace = TRUE;
		}
	}

	if (bReplace)
*/	{
		m_RichEditData.SetSel(lStartChar, lEndChar);

		CHARFORMAT chfmt;
		chfmt.dwEffects		= dwMask;
		chfmt.cbSize		= sizeof(chfmt);
		chfmt.dwMask		= CFM_BOLD;
		m_RichEditData.SetSelectionCharFormat(chfmt);

		// replace text if necessary
		CString sSelectedText = m_RichEditData.GetSelText();
		if (sSelectedText != sText) {
			m_RichEditData.ReplaceSel(sReplaceText);
		}
	}

	// save last char index
	m_lLastChar = m_lLastChar + sText.GetLength() + sEntryEnd.GetLength();
}
/////////////////////////////////////////////////////////////////////////////////
void CFieldDialog::ClearRestOfEntries()
{
  	m_RichEditData.SetSel(m_lLastChar+1, -1);
	m_RichEditData.ReplaceSel(_T(""));
}
/////////////////////////////////////////////////////////////////////////////
BOOL CFieldDialog::PreTranslateMessage(MSG* pMsg) 
{
	ASSERT(pMsg != NULL);
	ASSERT_VALID(this);
	ASSERT(m_hWnd != NULL);

	// don't call base class implementation 
	// it will eat frame window accelerators

	// don't translate dialog messages when in Shift+F1 help mode
	CFrameWnd* pFrameWnd = GetTopLevelFrame();
	if (pFrameWnd != NULL && pFrameWnd->m_bHelpMode)
		return FALSE;

	// since _T('IsDialogMessage') will eat frame window accelerators,
	//   we call all frame windows' PreTranslateMessage first
	pFrameWnd = GetParentFrame();   // start with first parent frame
	while (pFrameWnd != NULL)
	{
		// allow owner & frames to translate before IsDialogMessage does
		if (pFrameWnd->PreTranslateMessage(pMsg))
			return TRUE;

		// try parent frames until there are no parent frames
		pFrameWnd = pFrameWnd->GetParentFrame();
	}

	if (CWK_Dialog::PreTranslateMessage(pMsg))
	{
		return TRUE;
	}

	// filter both messages to dialog and from children
	return PreTranslateInput(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
void CFieldDialog::OnSize(UINT nType, int cx, int cy) 
{
	CWK_Dialog::OnSize(nType, cx, cy);
	
	CRect cr;
	CRect r;
	GetWindowRect(r);
	int x = r.Width();

	if (x>0)
	{
		if (WK_IS_WINDOW(&m_RichEditData))
		{
			m_RichEditData.GetWindowRect(cr);
			ScreenToClient(cr);
			cr.bottom = cy - 2;
			m_RichEditData.MoveWindow(cr);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CFieldDialog::OnInitDialog() 
{
	CWK_Dialog::OnInitDialog();

	m_comboCamera.ShowWindow(SW_HIDE);

	m_RichEditData.SetBackgroundColor(FALSE,GetSysColor(COLOR_BTNFACE));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CFieldDialog::CameraNames(int iNumFields,const CIPCField fields[])
{
	// in CIPC Thread
	m_CameraFields.FromArray(iNumFields,fields);
	PostMessage(WM_COMMAND,ID_CAMERA_NAMES);
}
/////////////////////////////////////////////////////////////////////////////
DWORD CFieldDialog::GetSelectedCamID() const
{
	return m_dwSelectedCamera;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CFieldDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (wParam==ID_CAMERA_NAMES)
	{
		OnCameraNames();
		return TRUE;
	}
	
	return CWK_Dialog::OnCommand(wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CFieldDialog::OnCameraNames()
{

	if (m_CameraFields.GetSize()>1)
	{
		m_comboCamera.ShowWindow(SW_SHOW);
		m_comboCamera.ResetContent();

		CString sL;
		sL.LoadString(IDS_ALL_CAMERAS);
		int ins = m_comboCamera.AddString(sL);
		m_comboCamera.SetItemData(ins,(DWORD)(-1));

		for (int i=0;i<m_CameraFields.GetSize();i++)
		{
			CIPCField* pField= m_CameraFields.GetAt(i);
			if (pField)
			{
				DWORD dwCamID=0;
				if (1 == _stscanf(pField->GetName(),_T("%08lx"),&dwCamID))
				{
					ins = m_comboCamera.AddString(pField->GetValue());
					m_comboCamera.SetItemData(ins,dwCamID);
				}
			}
		}

		m_comboCamera.SelectString(0,sL);
		m_dwSelectedCamera = (DWORD)(-1);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CFieldDialog::OnSelchangeComboCamera() 
{
	int iCurSel;
	iCurSel = m_comboCamera.GetCurSel();
	if (iCurSel!=CB_ERR)
	{
		m_dwSelectedCamera = m_comboCamera.GetItemData(iCurSel);
	}
}
