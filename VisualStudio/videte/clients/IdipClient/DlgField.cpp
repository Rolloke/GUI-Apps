// FieldDialog.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"

#include "DlgField.h"
#include "WndImageRecherche.h"
#include "WndDib.h"
#include ".\dlgfield.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgField dialog

CDlgField::CDlgField(CWndImageRecherche* pWndImage)
	: CSkinDialog(CDlgField::IDD, pWndImage)
{
	m_pWndImage = pWndImage;
	m_dwSelectedCamera = (DWORD)(-1);
	//{{AFX_DATA_INIT(CDlgField)
	m_sCamera = _T("");
	m_sResolution = _T("");
	m_sDate = _T("");
	m_sTime = _T("");
	m_sCompression = _T("");
	//}}AFX_DATA_INIT
	m_nMaxCharsWithoutSpace = 0;


	m_nInitToolTips = FALSE;
	if (UseOEMSkin())
	{
		CreateTransparent(StyleBackGroundPlaneColor, m_BaseColor);
		MoveOnClientAreaClick(false);
	}

#ifdef	DATA_CONTROL_AS_LINE
	ZERO_INIT(m_Bold);
	m_dwBoldLines = 0;
	m_dwOldBoldLines = 0;
#else
	m_lLastChar = -1;
#endif

	Create(IDD,pWndImage);
	CRect cr,vr;
	m_pWndImage->GetClientRect(cr);
	m_pWndImage->GetVideoClientRect(vr);
	cr.left = vr.right;
	MoveWindow(vr);


	ShowWindow(SW_SHOW);
}

void CDlgField::PostNcDestroy() 
{
	m_pWndImage->m_pDlgField = NULL;
	delete this;
}


void CDlgField::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgField)
	DDX_Control(pDX, IDC_COMBO_CAMERA, m_comboCamera);
	DDX_Text(pDX, IDC_CAMERA, m_sCamera);
	DDX_Text(pDX, IDC_RESOLUTION, m_sResolution);
	DDX_Text(pDX, IDC_DATE, m_sDate);
	DDX_Text(pDX, IDC_TIME, m_sTime);
	DDX_Text(pDX, IDC_COMPRESSION, m_sCompression);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgField, CSkinDialog)
	//{{AFX_MSG_MAP(CDlgField)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_COMBO_CAMERA, OnSelchangeComboCamera)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgField message handlers

void CDlgField::OnCancel() 
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgField::OnOK() 
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgField::Update()
{
	if (m_pWndImage)
	{
		CSize s = m_pWndImage->GetPictureSize();
		if (s.cx>0 && s.cy>0)
		{
			m_sResolution.Format(_T("%dx%d"),s.cx,s.cy);
		}
		else
		{
			m_sResolution.Empty();
		}

		int iCamNr = m_pWndImage->GetCameraNr();
		if (iCamNr > 0)
		{
			m_sCamera.Format(IDS_CAMERA_NR, iCamNr);
		}
		else
		{
			m_sCamera = "";
		}
		m_sDate = m_pWndImage->GetDate();
		m_sTime = m_pWndImage->GetTime();
		m_sCompression = m_pWndImage->GetCompression();

		UpdateData(FALSE);
		
		m_pWndImage->GetFields().Lock(_T(__FUNCTION__));
		CIPCFields fields(m_pWndImage->GetFields());
		m_pWndImage->GetFields().Unlock();
		CIPCField* pField;

		int i;
		CString name,value;

		if (m_pWndImage->GetType() == WST_DIB)
		{
			ASSERT_KINDOF(CWndDib, m_pWndImage);
#ifdef	DATA_CONTROL_AS_LINE
			CString sFormat = m_pWndImage->FormatData(_T(":<br>"),_T("<br>"));
			SplitString(sFormat, m_DataLines, _T("<br>"));
#else
			if (m_RichEditData.m_hWnd)
			{
				m_RichEditData.SetWindowText(m_pWndImage->FormatData(_T(":\r\n"),_T("\r\n")));
			}
			else if (m_HtmlData.m_hWnd)
			{
				m_HtmlData.SetWindowText(m_pWndImage->FormatData(_T(":<br>"),_T("<br>")));
			}
#endif
		}
		else if (m_pWndImage->IsWndPlay())
		{
			// start on begin of rich edit control
#ifdef	DATA_CONTROL_AS_LINE
			m_DataLines.RemoveAll();
			m_dwBoldLines = 0;
#else
			m_sStaticText.Empty();
			m_lLastChar = -1;
#endif
			for (i=0;i<fields.GetSize();i++)
			{
				pField = fields.GetAtFast(i);
#ifdef _DEBUG
				CString sTemp(pField->GetName());
				if (0 == sTemp.CompareNoCase(CString(_T("Größe"))))
				{
					name = sTemp;
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
						if (m_pWndImage->GetServer())
						{
							name = m_pWndImage->GetServer()->GetFieldName(pField->GetName());
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
#ifdef	DATA_CONTROL_AS_LINE
			InitDataLineControls();
#else
			ClearRestOfEntries();
#endif
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////
void CDlgField::SetTextEntry(const CString &sText, DWORD dwMask)
{
#ifdef	DATA_CONTROL_AS_LINE
	if (dwMask & CFE_BOLD)
	{
		m_dwBoldLines |= (1 << m_DataLines.GetSize());
	}
	m_DataLines.Add(sText);
#else
	if (m_RichEditData.m_hWnd)
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
		if (lFoundChar != -1) 
		{
			// szEntryEnd found, set end char
			lEndChar = lFoundChar + sEntryEnd.GetLength() - 1;
		}
		else
		{
			// no more szEntryEnd found, append one befor
			sReplaceText = sEntryEnd + sReplaceText;
			lEndChar = lStartChar + sReplaceText.GetLength() -1;
		}

		{
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
	else if (m_HtmlData.m_hWnd)
	{
		if (dwMask & CFE_BOLD)
		{
			CString sFmt;
			sFmt.Format(_T("<font color=0x%08x><b>"), CSkinButton::ChangeBrightness(m_HtmlData.GetTextColor(), -20, TRUE));
			m_sStaticText += sFmt + sText + _T("</b></font><br>");
		}
		else
		{
			m_sStaticText += sText + _T("<br>");
		}
	}
#endif
}
/////////////////////////////////////////////////////////////////////////////////
#ifdef	DATA_CONTROL_AS_LINE
BOOL CDlgField::CalculateHeight(CWnd*pWnd, const CString &sText, int nHeight, CRect &rc)
{
	BOOL bChanged  = FALSE;
	CRect rcTxt(CPoint(0, 0), CSize(rc.Width(), rc.Height()*5)), rcOld;
	CDC *pDC = pWnd->GetDC();
	CFont*pFont = pWnd->GetFont();
	int nOldLines, nNewLines;

	pWnd->GetWindowRect(&rcOld);
	pDC->SaveDC();
	pDC->SelectObject(pFont);
	pDC->DrawText(sText, &rcTxt, DT_LEFT|DT_CALCRECT|DT_WORDBREAK);
	if (nHeight)
	{
		nOldLines = rcOld.Height() / nHeight;
		nNewLines = (rcTxt.Height()+2) / nHeight;
		int nSpace = sText.Find(_T(" "));
		if (   nSpace == -1 && sText.GetLength() > m_nMaxCharsWithoutSpace
			|| nSpace > m_nMaxCharsWithoutSpace)
		{
			nNewLines++;
		}
		if (nOldLines != nNewLines)
		{
			bChanged += TRUE;
		}
		rc.bottom = rc.top + 2 + nHeight * nNewLines;	
	}
	else
	{
		rc = rcTxt;
	}
	pDC->RestoreDC(-1);
	pWnd->ReleaseDC(pDC);
	return bChanged;
}
/////////////////////////////////////////////////////////////////////////////////
void CDlgField::InitDataLineControls()
{
	// init control here
	int			nID, i, nTexts = m_DataLines.GetSize();
	CWnd*		pWnd;
	pWnd = GetDlgItem(IDC_COMPRESSION);
	if (pWnd)
	{
		CSkinEdit*  pEdit;
		CRect		rc, rcTemp;
		DWORD		dwStyle, dwStyleEx;
		pWnd->GetWindowRect(&rc);
		int nTextHeight = rc.Height();
		ScreenToClient(&rc);
		rc.bottom = rc.top + nTextHeight + 2;
		rc.OffsetRect(0, rc.Height());
		DWORD dwBoldMask = 0;
		BOOL bMoveWindow = FALSE;
		if (m_nMaxCharsWithoutSpace == 0)
		{
			TEXTMETRIC tm;
			CDC *pDC = pWnd->GetDC();
			pDC->SaveDC();
			pDC->SelectObject(&m_fLight);
			pDC->GetTextMetrics(&tm);
			pDC->RestoreDC(-1);
			int nCharWidth = tm.tmAveCharWidth+1;
			m_nMaxCharsWithoutSpace = rc.Width() / nCharWidth;
			if (nTextHeight < tm.tmHeight)
			{
				// TODO! RKE: ResizeWindows
//				nTextHeight = tm.tmHeight;
			}
			pWnd->ReleaseDC(pDC);

			pWnd->SetFont(&m_fLight);
			GetDlgItem(IDC_CAMERA)->SetFont(&m_fLight);
			GetDlgItem(IDC_RESOLUTION)->SetFont(&m_fLight);
			GetDlgItem(IDC_DATE)->SetFont(&m_fLight);
			GetDlgItem(IDC_TIME)->SetFont(&m_fLight);
		}

		dwStyle = WS_VISIBLE|WS_CHILD|ES_READONLY|ES_LEFT|ES_MULTILINE;
		dwStyleEx = 0;

		for (nID=AFX_IDW_CONTROLBAR_FIRST, i=0;
			nID<AFX_IDW_CONTROLBAR_LAST, i<nTexts; 
			nID++, i++)
		{
			const CString& sText = m_DataLines.GetAt(i);
			pWnd = GetDlgItem(nID);
			dwBoldMask = (1<<i);
			if (pWnd == NULL)
			{
				pEdit = new CSkinEdit();
				pEdit->CreateEx(dwStyleEx, WC_EDIT, sText, dwStyle, rc, this, nID);
				pWnd = pEdit;
				if (m_dwBoldLines & dwBoldMask)
				{
					pWnd->SetFont(&m_fBold);
					m_dwOldBoldLines |= dwBoldMask;
				}
				else
				{
					pWnd->SetFont(&m_fLight);
				}
				BOOL bCalcHeight = CalculateHeight(pWnd, sText, nTextHeight, rc);
				if (bCalcHeight)
				{
					bMoveWindow = TRUE;
				}
			}
			else
			{
				CString sOld;
				pWnd->GetWindowText(sOld);
				if (CalculateHeight(pWnd, sText, nTextHeight, rc))
				{
					bMoveWindow = TRUE;
				}
				if (sOld != sText)
				{
					pWnd->SetWindowText(sText);
					if (m_dwBoldLines != m_dwOldBoldLines)
					{
						if (m_dwBoldLines & dwBoldMask && (m_dwOldBoldLines & dwBoldMask) == 0)
						{
							pWnd->SetFont(&m_fBold);
							m_dwOldBoldLines |= dwBoldMask;
						}
						else if (m_dwOldBoldLines & dwBoldMask && (m_dwBoldLines & dwBoldMask) == 0)
						{
							pWnd->SetFont(&m_fLight);
						}
					}
				}
			}
			if (bMoveWindow && pWnd)
			{
				pWnd->MoveWindow(&rc);
			}
			int nHeight = rc.Height();
			rc.top    += nHeight;
			rc.bottom += nHeight;

		}
		dwBoldMask <<= 1;
		dwBoldMask--;
		m_dwOldBoldLines &= dwBoldMask;
	}
}
#else
/////////////////////////////////////////////////////////////////////////////
void CDlgField::ClearRestOfEntries()
{
	if (m_RichEditData.m_hWnd)
	{
  		m_RichEditData.SetSel(m_lLastChar+1, -1);
		m_RichEditData.ReplaceSel(_T(""));
	}
	else if (m_HtmlData.m_hWnd && !m_sStaticText.IsEmpty())
	{
		if (m_sStaticText.GetLength() > 5)
		{
			m_sStaticText = m_sStaticText.Left(m_sStaticText.GetLength()-4);
		}
		CString s;
		m_HtmlData.GetWindowText(s);
		if (s != m_sStaticText)
		{
			m_HtmlData.SetWindowText(m_sStaticText);
		}
	}
}
#endif
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgField::PreTranslateMessage(MSG* pMsg) 
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

	if (CSkinDialog::PreTranslateMessage(pMsg))
	{
		return TRUE;
	}

	// filter both messages to dialog and from children
	return PreTranslateInput(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgField::OnSize(UINT nType, int cx, int cy) 
{
	CSkinDialog::OnSize(nType, cx, cy);
	
	CRect cr;
	CRect r;
	GetWindowRect(r);
	int x = r.Width();

	if (x>0)
	{
#ifdef	DATA_CONTROL_AS_LINE
		// - move already created
		// - delete or hide invisible
#else
		CWnd wnd;
		if (m_RichEditData.m_hWnd)	wnd.m_hWnd = m_RichEditData.m_hWnd;
		else if (m_HtmlData.m_hWnd)	wnd.m_hWnd = m_HtmlData.m_hWnd;

		if (wnd.m_hWnd)
		{
			wnd.GetWindowRect(cr);
			ScreenToClient(cr);
			cr.bottom = cy - 2;
			wnd.MoveWindow(cr);
		}
		wnd.m_hWnd = NULL;
#endif
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgField::OnInitDialog() 
{
	CSkinDialog::OnInitDialog();
	AutoCreateSkinStatic();
	AutoCreateSkinEdit();

	m_comboCamera.ShowWindow(SW_HIDE);

#ifdef	DATA_CONTROL_AS_LINE
	CFont *pFont = GetFont();
	if (UseGlobalFonts())
	{
		pFont = GetDialogItemGlobalFont();
	}
	if (pFont)
	{
		LOGFONT lf;
		if (pFont->GetLogFont(&lf))
		{
			CopyMemory(&m_Bold, &lf, sizeof(LOGFONT));
			m_fLight.CreateFontIndirect(&m_Bold);
			m_Bold.lfWeight = FW_BOLD;
			m_fBold.CreateFontIndirect(&m_Bold);
		}
	}
/*
	// RKE: those texts are removed from ressource
	CWnd *pWnd = GetDlgItem(IDC_STATIC_TEXT);
	if (pWnd)
	{
		pWnd->DestroyWindow();
	}
	pWnd = GetDlgItem(IDC_RICHEDIT_DATA);
	if (pWnd)
	{
		pWnd->DestroyWindow();
	}
*/
#else
	// TODO! RKE: Create Controls if ever necessary
	if (UseOEMSkin())
	{
		m_HtmlData.Create(..);
	}
	else
	{
		m_RichEditData.Create(..);
	}

	if (m_RichEditData.m_hWnd)
	{
		m_RichEditData.SetBackgroundColor(FALSE, afxData.clrBtnFace);
		CWnd *pWnd = GetDlgItem(IDC_STATIC_TEXT);
		if (pWnd)
		{
			pWnd->ShowWindow(SW_HIDE);
		}
	}
	else if (m_HtmlData.m_hWnd)
	{
		m_HtmlData.ModifySkinStyle(0, SSS_DRAW_HTML);
		m_HtmlData.SetWindowText(_T(""));
		CWnd *pWnd = GetDlgItem(IDC_RICHEDIT_DATA);
		if (pWnd)
		{
			pWnd->ShowWindow(SW_HIDE);
		}
	}
#endif
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CDlgField::CameraNames(int iNumFields,const CIPCField fields[])
{
	// in CIPC Thread
	m_CameraFields.FromArray(iNumFields,fields);
	PostMessage(WM_COMMAND,ID_CAMERA_NAMES);
}
/////////////////////////////////////////////////////////////////////////////
DWORD CDlgField::GetSelectedCamID() const
{
	return m_dwSelectedCamera;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgField::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (wParam==ID_CAMERA_NAMES)
	{
		OnCameraNames();
		return TRUE;
	}
	
	return CSkinDialog::OnCommand(wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgField::OnCameraNames()
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
			CIPCField* pField= m_CameraFields.GetAtFast(i);
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
void CDlgField::OnSelchangeComboCamera() 
{
	int iCurSel;
	iCurSel = m_comboCamera.GetCurSel();
	if (iCurSel!=CB_ERR)
	{
		m_dwSelectedCamera = m_comboCamera.GetItemData(iCurSel);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgField::OnDestroy() 
{
	int nID;
	CWnd *pWnd;
	for (nID=AFX_IDW_CONTROLBAR_FIRST; nID<AFX_IDW_CONTROLBAR_LAST; nID++)
	{
		pWnd = GetDlgItem(nID);
		if (pWnd)
		{
			delete pWnd;
		}
		else
		{
			break;
		}
	}
	CSkinDialog::OnDestroy();
}
