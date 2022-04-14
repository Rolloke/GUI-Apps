// CKeyboardDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dvclient.h"
#include "CKeyboardDlg.h"
#include "CPanel.h"
#include ".\ckeyboarddlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define		INPUT_IP_ENTERED	2
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CKeyboardDlg, CTransparentDialog)
/////////////////////////////////////////////////////////////////////////////
// CKeyboardDlg dialog
CKeyboardDlg::CKeyboardDlg(CDisplay* pDisplay, const CString &sFormat, const CString &sInput, CWnd* pParent /*=NULL*/)
	: CTransparentDialog(CKeyboardDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKeyboardDlg)
	//}}AFX_DATA_INIT
	m_BaseColor		= CPanel::m_BaseColorBackGrounds;
	m_sInput		= sInput;
	m_sFormat		= sFormat;
	m_pDisplay		= pDisplay;
	m_pPanel		= pParent;
	m_bShift		= TRUE;
	m_bKBLang		= FALSE;
	m_hCursor		= NULL;
	m_hAccelTable	= LoadAccelerators(AfxGetInstanceHandle(),
										MAKEINTRESOURCE(IDR_ACCELERATOR_KEYBORD));
	m_bInitializedBtn = false;
	m_pCirconFlexe	= NULL;
	m_pAigu			= NULL;
	m_pGrave		= NULL;
	m_pEnje			= NULL;
	m_pDiaerese		= NULL;
	m_bOnlyAsscii	= FALSE;
	m_bIPallowed	= FALSE;
} 
/////////////////////////////////////////////////////////////////////////////
CKeyboardDlg::~CKeyboardDlg()
{
} 
/////////////////////////////////////////////////////////////////////////////
void CKeyboardDlg::DoDataExchange(CDataExchange* pDX)
{
	CTransparentDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeyboardDlg)
	DDX_Control(pDX, IDC_BUTTON_SPACE, m_ctrlButtonSpace);
	DDX_Control(pDX, IDC_BUTTON_PLUS, m_ctrlButtonPlus);
	DDX_Control(pDX, IDC_BUTTON_MINUS, m_ctrlButtonMinus);
	DDX_Control(pDX, IDC_BUTTON_SHIFT, m_ctrlButtonShift);
	DDX_Control(pDX, IDC_BUTTON_BACKSPACE, m_ctrlButtonBackSpace);
	DDX_Control(pDX, IDOK, m_ctrlButtonOK);
	DDX_Control(pDX, IDC_BUTTON_0, m_ctrlButton0);
	DDX_Control(pDX, IDC_BUTTON_1, m_ctrlButton1);
	DDX_Control(pDX, IDC_BUTTON_2, m_ctrlButton2);
	DDX_Control(pDX, IDC_BUTTON_3, m_ctrlButton3);
	DDX_Control(pDX, IDC_BUTTON_4, m_ctrlButton4);
	DDX_Control(pDX, IDC_BUTTON_5, m_ctrlButton5);
	DDX_Control(pDX, IDC_BUTTON_6, m_ctrlButton6);
	DDX_Control(pDX, IDC_BUTTON_7, m_ctrlButton7);
	DDX_Control(pDX, IDC_BUTTON_8, m_ctrlButton8);
	DDX_Control(pDX, IDC_BUTTON_9, m_ctrlButton9);
	DDX_Control(pDX, IDC_BUTTON_Q, m_ctrlButtonQ);
	DDX_Control(pDX, IDC_BUTTON_W, m_ctrlButtonW);
	DDX_Control(pDX, IDC_BUTTON_E, m_ctrlButtonE);
	DDX_Control(pDX, IDC_BUTTON_R, m_ctrlButtonR);
	DDX_Control(pDX, IDC_BUTTON_T, m_ctrlButtonT);
	DDX_Control(pDX, IDC_BUTTON_Z, m_ctrlButtonZ);
	DDX_Control(pDX, IDC_BUTTON_U, m_ctrlButtonU);
	DDX_Control(pDX, IDC_BUTTON_I, m_ctrlButtonI);
	DDX_Control(pDX, IDC_BUTTON_O, m_ctrlButtonO);
	DDX_Control(pDX, IDC_BUTTON_P, m_ctrlButtonP);
	DDX_Control(pDX, IDC_BUTTON_A, m_ctrlButtonA);
	DDX_Control(pDX, IDC_BUTTON_S, m_ctrlButtonS);
	DDX_Control(pDX, IDC_BUTTON_D, m_ctrlButtonD);
	DDX_Control(pDX, IDC_BUTTON_F, m_ctrlButtonF);
	DDX_Control(pDX, IDC_BUTTON_G, m_ctrlButtonG);
	DDX_Control(pDX, IDC_BUTTON_H, m_ctrlButtonH);
	DDX_Control(pDX, IDC_BUTTON_J, m_ctrlButtonJ);
	DDX_Control(pDX, IDC_BUTTON_K, m_ctrlButtonK);
	DDX_Control(pDX, IDC_BUTTON_L, m_ctrlButtonL);
	DDX_Control(pDX, IDC_BUTTON_Y, m_ctrlButtonY);
	DDX_Control(pDX, IDC_BUTTON_X, m_ctrlButtonX);
	DDX_Control(pDX, IDC_BUTTON_C, m_ctrlButtonC);
	DDX_Control(pDX, IDC_BUTTON_V, m_ctrlButtonV);
	DDX_Control(pDX, IDC_BUTTON_B, m_ctrlButtonB);
	DDX_Control(pDX, IDC_BUTTON_N, m_ctrlButtonN);
	DDX_Control(pDX, IDC_BUTTON_M, m_ctrlButtonM);
	DDX_Control(pDX, IDC_BUTTON_UE, m_ctrlButtonUE);
	DDX_Control(pDX, IDC_BUTTON_OE, m_ctrlButtonOE);
	DDX_Control(pDX, IDC_BUTTON_AE, m_ctrlButtonAE);
	DDX_Control(pDX, IDC_BUTTON_SC1, m_ctrlButtonSC1);
	DDX_Control(pDX, IDC_BUTTON_SC2, m_ctrlButtonSC2);
	DDX_Control(pDX, IDC_BUTTON_SC3, m_ctrlButtonSC3);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_BUTTON_KBLANG, m_btnKbLang);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CKeyboardDlg, CTransparentDialog)
	//{{AFX_MSG_MAP(CKeyboardDlg)
	ON_BN_CLICKED(IDC_BUTTON_BACKSPACE, OnButtonBackSpace)
	ON_BN_CLICKED(IDC_BUTTON_SHIFT, OnButtonShift)
	ON_BN_CLICKED(IDC_BUTTON_SPACE, OnButtonSpace)
	ON_BN_CLICKED(IDC_BUTTON_PLUS, OnButtonPlus)
	ON_BN_CLICKED(IDC_BUTTON_MINUS, OnButtonMinus)
	ON_WM_SETCURSOR()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_0, IDC_BUTTON_SC3, 	OnButtonRange)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_KBLANG, OnBnClickedButtonKblang)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CKeyboardDlg message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CKeyboardDlg::OnInitDialog() 
{
	CTransparentDialog::OnInitDialog();
	
	CreateTransparent(StyleBackGroundColorChangeBrushed, CPanel::m_BaseColorBackGrounds);

	m_DVButtons.Add(&m_ctrlButton0);
	m_DVButtons.Add(&m_ctrlButton1);
	m_DVButtons.Add(&m_ctrlButton2);
	m_DVButtons.Add(&m_ctrlButton3);
	m_DVButtons.Add(&m_ctrlButton4);
	m_DVButtons.Add(&m_ctrlButton5);
	m_DVButtons.Add(&m_ctrlButton6);
	m_DVButtons.Add(&m_ctrlButton7);
	m_DVButtons.Add(&m_ctrlButton8);
	m_DVButtons.Add(&m_ctrlButton9);

	m_DVButtons.Add(&m_ctrlButtonQ);
	m_DVButtons.Add(&m_ctrlButtonW);
	m_DVButtons.Add(&m_ctrlButtonE);
	m_DVButtons.Add(&m_ctrlButtonR);
	m_DVButtons.Add(&m_ctrlButtonT);
	m_DVButtons.Add(&m_ctrlButtonZ);
	m_DVButtons.Add(&m_ctrlButtonU);
	m_DVButtons.Add(&m_ctrlButtonI);
	m_DVButtons.Add(&m_ctrlButtonO);
	m_DVButtons.Add(&m_ctrlButtonP);
	m_DVButtons.Add(&m_ctrlButtonA);
	m_DVButtons.Add(&m_ctrlButtonS);
	m_DVButtons.Add(&m_ctrlButtonD);
	m_DVButtons.Add(&m_ctrlButtonF);
	m_DVButtons.Add(&m_ctrlButtonG);
	m_DVButtons.Add(&m_ctrlButtonH);
	m_DVButtons.Add(&m_ctrlButtonJ);
	m_DVButtons.Add(&m_ctrlButtonK);
	m_DVButtons.Add(&m_ctrlButtonL);
	m_DVButtons.Add(&m_ctrlButtonY);
	m_DVButtons.Add(&m_ctrlButtonX);
	m_DVButtons.Add(&m_ctrlButtonC);
	m_DVButtons.Add(&m_ctrlButtonV);
	m_DVButtons.Add(&m_ctrlButtonB);
	m_DVButtons.Add(&m_ctrlButtonN);
	m_DVButtons.Add(&m_ctrlButtonM);
	m_DVButtons.Add(&m_ctrlButtonBackSpace);
	m_DVButtons.Add(&m_ctrlButtonShift);
	m_DVButtons.Add(&m_btnKbLang);
	m_DVButtons.Add(&m_ctrlButtonSpace);
	m_DVButtons.Add(&m_ctrlButtonPlus);
	m_DVButtons.Add(&m_ctrlButtonMinus);
	m_DVButtons.Add(&m_ctrlButtonOK);

	m_DVButtons.Add(&m_ctrlButtonAE);
	m_DVButtons.Add(&m_ctrlButtonOE);
	m_DVButtons.Add(&m_ctrlButtonUE);
	m_DVButtons.Add(&m_ctrlButtonSC1);
	m_DVButtons.Add(&m_ctrlButtonSC2);
	m_DVButtons.Add(&m_ctrlButtonSC3);

	if (m_bIPallowed)
	{
		CString sKeys;
		sKeys.LoadString(IDS_KEYBOARD_UPPER_KEYS);
		if (sKeys.Find(_T(".")) != -1)
		{
			m_bShift = TRUE;
		}
		else
		{
			m_bShift = FALSE;
		}
	}
#ifndef _UNICODE
	CSkinButton*pBtn = &m_ctrlButtonUE;
	for (int i=0; i<6; i++)
	{
		pBtn[i].ShowWindow(SW_HIDE);
	}
	MoveButtons();
#endif
						 
	m_DVButtons.SetTextAllignment(DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX);
	m_DVButtons.SetBaseColor(m_BaseColor);
	if (CSkinDialog::UseGlobalFonts())
	{
		m_DVButtons.SetTextFont(CSkinDialog::GetDialogItemGlobalFont());
		if (m_pPanel->IsKindOf(RUNTIME_CLASS(CPanel)))
		{
			m_btnKbLang.SetWindowText(((CPanel*)m_pPanel)->GetCurrentLanguageAbbr());
		}
		m_btnKbLang.ShowWindow(SW_SHOW);
		m_btnKbLang.SetStyle(StyleCheck);
		m_bKBLang = TRUE;
	}
	else
	{
		m_btnKbLang.ShowWindow(SW_HIDE);
		m_bKBLang = TRUE;
	}

	m_ilEnter.Create(IDB_ENTER2, 16, 0, SKIN_COLOR_KEY);
	m_ctrlButtonOK.SetImageList(&m_ilEnter);

	m_ilBackSpace.Create(IDB_BACKSPACE, 16, 0, SKIN_COLOR_KEY);
	m_ctrlButtonBackSpace.SetImageList(&m_ilBackSpace);

	m_ilShift.Create(IDB_SHIFT, 16, 0, SKIN_COLOR_KEY);
	m_ctrlButtonShift.SetImageList(&m_ilShift);

	m_ctrlButtonShift.SetStyle(StyleCheck);
	
	m_hCursor = theApp.m_hArrow;

	if (m_pPanel)
	{
		CRect rcPanel;
		m_pPanel->GetWindowRect(rcPanel);

		CRect rcKeyboard;
		GetWindowRect(rcKeyboard);
		
		// Keyboard oberhalb des Panels positionieren
		int nX	= max(rcPanel.left, 0);
		int	nY	= rcPanel.top - rcKeyboard.Height();

		if (nX < 0 || nY < 0)
		{
			// Keyboard unterhalb des Panels positionieren
			nX	= rcPanel.left;
			nY	= rcPanel.bottom;
		}
		SetWindowPos(&wndTopMost, nX, nY, -1, -1, SWP_NOSIZE);
	}
	OnButtonPressed(_T(""));
	ChangeKeyBoard();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
//
void CKeyboardDlg::OnButtonRange(UINT nID)
{
	CWnd *pWnd = GetDlgItem(nID);
	if (pWnd)
	{
		CString str;
		ASSERT_KINDOF(CSkinButton, pWnd);
		CSkinButton *pBtn = (CSkinButton*)pWnd;

		pWnd->GetWindowText(str);
		if (str[0] == _T('&'))
		{
			str = str.Mid(1);
		}
		
		if (str[0] == _T('^'))
		{
			pBtn->SetStyle(StyleCheck);
			pBtn->SetCheck(TRUE);
			m_pCirconFlexe = pBtn;
			return;
		}
		else if (m_pCirconFlexe && m_pCirconFlexe->GetCheck())
		{
			switch (str[0])
			{
				case _T('e'): str.SetAt(0, _T('ê')); break;
				case _T('u'): str.SetAt(0, _T('û')); break;
				case _T('i'): str.SetAt(0, _T('î')); break;
				case _T('o'): str.SetAt(0, _T('ô')); break;
				case _T('a'): str.SetAt(0, _T('â')); break;
				case _T('E'): str.SetAt(0, _T('Ê')); break;
				case _T('U'): str.SetAt(0, _T('Û')); break;
				case _T('I'): str.SetAt(0, _T('Î')); break;
				case _T('O'): str.SetAt(0, _T('Ô')); break;
				case _T('A'): str.SetAt(0, _T('Â')); break;
				default: break;
			}
			m_pCirconFlexe->SetCheck(FALSE);
		}

		if (str[0] == _T('´'))
		{
			pBtn->SetStyle(StyleCheck);
			pBtn->SetCheck(TRUE);
			m_pAigu = pBtn;
			return;
		}
		else if (m_pAigu && m_pAigu->GetCheck())
		{
			switch (str[0])
			{
				case _T('e'): str.SetAt(0, _T('é')); break;
				case _T('u'): str.SetAt(0, _T('ú')); break;
				case _T('i'): str.SetAt(0, _T('í')); break;
				case _T('o'): str.SetAt(0, _T('ó')); break;
				case _T('a'): str.SetAt(0, _T('á')); break;
				case _T('y'): str.SetAt(0, _T('ý')); break;
				case _T('E'): str.SetAt(0, _T('É')); break;
				case _T('U'): str.SetAt(0, _T('Ú')); break;
				case _T('I'): str.SetAt(0, _T('Í')); break;
				case _T('O'): str.SetAt(0, _T('Ó')); break;
				case _T('A'): str.SetAt(0, _T('Á')); break;
				case _T('Y'): str.SetAt(0, _T('Ý')); break;
				default: break;
			}
			m_pAigu->SetCheck(FALSE);
		}
		
		if (str[0] == _T('`'))
		{
			pBtn->SetStyle(StyleCheck);
			pBtn->SetCheck(TRUE);
			m_pGrave = pBtn;
			return;
		}
		else if (m_pGrave && m_pGrave->GetCheck())
		{
			switch (str[0])
			{
				case _T('e'): str.SetAt(0, _T('è')); break;
				case _T('u'): str.SetAt(0, _T('ù')); break;
				case _T('i'): str.SetAt(0, _T('ì')); break;
				case _T('o'): str.SetAt(0, _T('ò')); break;
				case _T('a'): str.SetAt(0, _T('à')); break;
				case _T('E'): str.SetAt(0, _T('È')); break;
				case _T('U'): str.SetAt(0, _T('Ù')); break;
				case _T('I'): str.SetAt(0, _T('Ì')); break;
				case _T('O'): str.SetAt(0, _T('Ò')); break;
				case _T('A'): str.SetAt(0, _T('À')); break;
				default: break;
			}
			m_pGrave->SetCheck(FALSE);
		}

		if (str[0] == _T('~'))
		{
			pBtn->SetStyle(StyleCheck);
			pBtn->SetCheck(TRUE);
			m_pEnje = pBtn;
			return;
		}
		else if (m_pEnje && m_pEnje->GetCheck())
		{
			switch (str[0])
			{
				case _T('o'): str.SetAt(0, _T('õ')); break;
				case _T('a'): str.SetAt(0, _T('ã')); break;
				case _T('O'): str.SetAt(0, _T('Õ')); break;
				case _T('A'): str.SetAt(0, _T('Ã')); break;
				default: break;
			}
			m_pEnje->SetCheck(FALSE);
		}
		
		if (str[0] == _T('¨'))
		{
			pBtn->SetStyle(StyleCheck);
			pBtn->SetCheck(TRUE);
			m_pDiaerese = pBtn;
			return;
		}
		else if (m_pDiaerese && m_pDiaerese->GetCheck())
		{
			switch (str[0])
			{
				case _T('e'): str.SetAt(0, _T('ë')); break;
				case _T('u'): str.SetAt(0, _T('ü')); break;
				case _T('i'): str.SetAt(0, _T('ï')); break;
				case _T('o'): str.SetAt(0, _T('ö')); break;
				case _T('a'): str.SetAt(0, _T('ä')); break;
				case _T('E'): str.SetAt(0, _T('Ë')); break;
				case _T('U'): str.SetAt(0, _T('Ü')); break;
				case _T('I'): str.SetAt(0, _T('Ï')); break;
				case _T('O'): str.SetAt(0, _T('Ö')); break;
				case _T('A'): str.SetAt(0, _T('Ä')); break;
				default: break;
			}
			m_pDiaerese->SetCheck(FALSE);
		}
		OnButtonPressed(str);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CKeyboardDlg::OnBnClickedButtonKblang()
{
	m_bKBLang = !m_bKBLang;
	ChangeKeyBoard();
}
/////////////////////////////////////////////////////////////////////////////
void CKeyboardDlg::OnButtonShift() 
{
	m_bShift = !m_bShift;
	ChangeKeyBoard();
}
/////////////////////////////////////////////////////////////////////////////
void CKeyboardDlg::ChangeKeyBoard()
{
	m_ctrlButtonShift.SetCheck(!m_bShift);
	m_btnKbLang.SetCheck(m_bKBLang);
#ifdef _UNICODE
	int     i, k, nLen;
	CString sKeys, sTemp;
	if (m_bShift)
	{
		sKeys.LoadString(IDS_KEYBOARD_UPPER_KEYS);
	}
	else
	{
		sKeys.LoadString(IDS_KEYBOARD_LOWER_KEYS);
	}
	nLen = sKeys.GetLength();
	if (nLen!=0 && m_bKBLang)
	{
		for (k=0,i=IDC_BUTTON_Q; k<nLen && i<=IDC_BUTTON_M; k++, i++)
		{
			sTemp.Format(_T("&%c"), sKeys[k]);
			SetDlgItemText(i, sTemp);
		}
		bool bMove = true;
		CSkinButton*pBtn = &m_ctrlButtonUE;
		for (i=0; k<nLen && i<6; k++, i++)
		{
			if (m_bOnlyAsscii && !_istascii(sKeys[k]))
			{
				pBtn[i].ShowWindow(SW_HIDE);
			}
			else if (sKeys[k] == _T(' ')) // space character is to hide
			{
				pBtn[i].ShowWindow(SW_HIDE);
			}
			else
			{
				if (!m_sNotAllowedChars.IsEmpty())
				{
					if (m_sNotAllowedChars.Find(sKeys[k]) != -1)
					{
						pBtn[i].ShowWindow(SW_HIDE);
						continue;
					}
				}
				sTemp.Format(_T("&%c"), sKeys[k]);
				pBtn[i].SetWindowText(sTemp);
				pBtn[i].ShowWindow(SW_SHOW);
				bMove = false;
			}
		}
		for (; i<6; i++)
		{
			pBtn[i].ShowWindow(SW_HIDE);
		}

		if (!m_sNotAllowedChars.IsEmpty())
		{
			if (m_sNotAllowedChars.Find(_T('-')) != -1)
			{
				m_ctrlButtonMinus.ShowWindow(SW_HIDE);
			}
			if (m_sNotAllowedChars.Find(_T('+')) != -1)
			{
				m_ctrlButtonPlus.ShowWindow(SW_HIDE);
			}
			if (m_sNotAllowedChars.Find(_T(' ')) != -1)
			{
				m_ctrlButtonSpace.ShowWindow(SW_HIDE);
			}
		}
		if (!m_bInitializedBtn)
		{
			if (bMove)
			{
				MoveButtons();
			}
			m_bInitializedBtn = true;
		}
	}
	else
#endif
	{
		if (m_bShift)
		{
			m_ctrlButtonQ.SetWindowText(_T("&Q"));
			m_ctrlButtonW.SetWindowText(_T("&W"));
			m_ctrlButtonE.SetWindowText(_T("&E"));
			m_ctrlButtonR.SetWindowText(_T("&R"));
			m_ctrlButtonT.SetWindowText(_T("&T"));
			m_ctrlButtonZ.SetWindowText(_T("&Z"));
			m_ctrlButtonU.SetWindowText(_T("&U"));
			m_ctrlButtonI.SetWindowText(_T("&I"));
			m_ctrlButtonO.SetWindowText(_T("&O"));
			m_ctrlButtonP.SetWindowText(_T("&P"));
			m_ctrlButtonA.SetWindowText(_T("&A"));
			m_ctrlButtonS.SetWindowText(_T("&S"));
			m_ctrlButtonD.SetWindowText(_T("&D"));
			m_ctrlButtonF.SetWindowText(_T("&F"));
			m_ctrlButtonG.SetWindowText(_T("&G"));
			m_ctrlButtonH.SetWindowText(_T("&H"));
			m_ctrlButtonJ.SetWindowText(_T("&J"));
			m_ctrlButtonK.SetWindowText(_T("&K"));
			m_ctrlButtonL.SetWindowText(_T("&L"));
			m_ctrlButtonY.SetWindowText(_T("&Y"));
			m_ctrlButtonX.SetWindowText(_T("&X"));
			m_ctrlButtonC.SetWindowText(_T("&C"));
			m_ctrlButtonV.SetWindowText(_T("&V"));
			m_ctrlButtonB.SetWindowText(_T("&B"));
			m_ctrlButtonN.SetWindowText(_T("&N"));
			m_ctrlButtonM.SetWindowText(_T("&M"));
		}
		else
		{
			m_ctrlButtonQ.SetWindowText(_T("&q"));
			m_ctrlButtonW.SetWindowText(_T("&w"));
			m_ctrlButtonE.SetWindowText(_T("&e"));
			m_ctrlButtonR.SetWindowText(_T("&r"));
			m_ctrlButtonT.SetWindowText(_T("&t"));
			m_ctrlButtonZ.SetWindowText(_T("&z"));
			m_ctrlButtonU.SetWindowText(_T("&u"));
			m_ctrlButtonI.SetWindowText(_T("&i"));
			m_ctrlButtonO.SetWindowText(_T("&o"));
			m_ctrlButtonP.SetWindowText(_T("&p"));
			m_ctrlButtonA.SetWindowText(_T("&a"));
			m_ctrlButtonS.SetWindowText(_T("&s"));
			m_ctrlButtonD.SetWindowText(_T("&d"));
			m_ctrlButtonF.SetWindowText(_T("&f"));
			m_ctrlButtonG.SetWindowText(_T("&g"));
			m_ctrlButtonH.SetWindowText(_T("&h"));
			m_ctrlButtonJ.SetWindowText(_T("&j"));
			m_ctrlButtonK.SetWindowText(_T("&k"));
			m_ctrlButtonL.SetWindowText(_T("&l"));
			m_ctrlButtonY.SetWindowText(_T("&y"));
			m_ctrlButtonX.SetWindowText(_T("&x"));
			m_ctrlButtonC.SetWindowText(_T("&c"));
			m_ctrlButtonV.SetWindowText(_T("&v"));
			m_ctrlButtonB.SetWindowText(_T("&b"));
			m_ctrlButtonN.SetWindowText(_T("&n"));
			m_ctrlButtonM.SetWindowText(_T("&m"));
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CKeyboardDlg::OnButtonSpace() 
{
	OnButtonPressed(_T(" "));	
}

/////////////////////////////////////////////////////////////////////////////
void CKeyboardDlg::OnButtonPlus() 
{
	OnButtonPressed(_T("+"));	
}

/////////////////////////////////////////////////////////////////////////////
void CKeyboardDlg::OnButtonMinus() 
{
	OnButtonPressed(_T("-"));	
}

/////////////////////////////////////////////////////////////////////////////
void CKeyboardDlg::OnButtonPressed(const CString &sCharacter)
{
	if (!m_sNotAllowedChars.IsEmpty() && !sCharacter.IsEmpty())
	{
		int nFind = m_sNotAllowedChars.Find(sCharacter.GetAt(0));
		if (nFind != -1)
		{
			theApp.MessageBeep(MB_ICONWARNING);
			return;
		}
		else if (m_bOnlyAsscii)
		{
			if (!_istascii(sCharacter.GetAt(0)))
			{
				theApp.MessageBeep(MB_ICONWARNING);
				return;
			}
		}
	}
	CString sMsg;
	CString sChar = sCharacter;

	if (!m_bShift)
		sChar.MakeLower();
	
	if ( sCharacter.GetLength() &&
		(	m_bIPallowed == TRUE			// IP can be entered
		 || m_bIPallowed == INPUT_IP_ENTERED))
	{
		if (sCharacter.GetAt(0) == _T('.'))	// IP is entered
		{
			if (   m_bIPallowed == TRUE		// load format string
				&& m_sInput.GetLength() == 3)
			{
				CString sFormat;		// load FORMAT_IP
				sFormat.LoadString(IDS_FORMAT_IP);
				int nFind = m_sFormat.Find(_T(":"));
				if (nFind != -1)
				{
					m_sFormat = m_sFormat.Left(nFind) + sFormat;
					m_bIPallowed = INPUT_IP_ENTERED;// input is now IP address
					int i;
					for (i=IDC_BUTTON_Q; i<=IDC_BUTTON_M; i++)
					{
						GetDlgItem(i)->EnableWindow(FALSE);
					}
					CSkinButton*pBtn = &m_ctrlButtonUE;
					for (i=0; i<6; i++)
					{
						pBtn[i].EnableWindow(FALSE);
					}
					m_ctrlButtonShift.EnableWindow(FALSE);
					m_ctrlButtonSpace.EnableWindow(FALSE);
					m_ctrlButtonMinus.EnableWindow(FALSE);
				}
			}
			sChar.Empty();
		}
		else
		{
			if (!_istdigit(sCharacter.GetAt(0)))
			{
				m_bIPallowed = FALSE;
			}
		}
	}

	m_sInput += sChar;
	TRACE(_T("%s"), sChar);
//	m_sInput = m_sInput.Left(16);
	
	if (m_pDisplay->SetFormatDisplayText(m_sInput, m_sFormat) < m_sInput.GetLength())
		m_sInput = m_sInput.Left(m_sInput.GetLength()-1);
}

/////////////////////////////////////////////////////////////////////////////
void CKeyboardDlg::OnButtonBackSpace() 
{
	if (!m_sInput.IsEmpty())
	{
		m_sInput = m_sInput.Left(m_sInput.GetLength()-1);
		OnButtonPressed(_T(""));
	}
}

/////////////////////////////////////////////////////////////////////////////
CString CKeyboardDlg::GetInputString()
{
	if (m_bIPallowed == INPUT_IP_ENTERED)
	{
		CString sTemp = m_sInput;
		m_sInput.Format(_T("%s.%s.%s.%s"), sTemp.Mid(0,3), sTemp.Mid(3,3), sTemp.Mid(6,3), sTemp.Mid(9,3));
	}
	return m_sInput;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CKeyboardDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_hCursor)
	{
		if (SetCursor(m_hCursor) != NULL)
			return TRUE;		
	}	

	return CTransparentDialog::OnSetCursor(pWnd, nHitTest, message);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CKeyboardDlg::PreTranslateMessage(MSG* pMsg) 
{
	BOOL bHandled = FALSE;
	if (m_hAccelTable) 
	{
		if (::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg))
		{
			bHandled = TRUE;
		}
		else
		{
			if (pMsg->message == WM_KEYDOWN)
			{
				TRACE(_T("0x%x  0x%x\n"), pMsg->wParam, pMsg->lParam);
				if (pMsg->wParam == VK_SHIFT)
				{
					OnButtonShift();
					bHandled = TRUE;
				}
			}
		}
	}

	if (!bHandled)
	{
		bHandled = CTransparentDialog::PreTranslateMessage(pMsg);
	}
	return bHandled;
}
//////////////////////////////////////////////////////////////////////////
void CKeyboardDlg::MoveButtons()
{
	int   i, nWidth, nHeight, nCount, nToMove[] ={IDC_BUTTON_PLUS, IDC_BUTTON_MINUS, IDC_BUTTON_BACKSPACE, IDOK};
	CWnd *pWnd;
	CRect rc;
	m_ctrlButton0.GetClientRect(&rc);
	nWidth  = rc.Width();
	nHeight = rc.Height();
	nCount = sizeof(nToMove) / sizeof(int);
	for (i=0; i<nCount; i++)
	{
		pWnd = GetDlgItem(nToMove[i]);
		if (pWnd)
		{
			pWnd->GetWindowRect(&rc);
			ScreenToClient(&rc);
			if (nToMove[i] == IDOK)             rc.top  -= nHeight;
			if (nToMove[i] == IDC_BUTTON_MINUS) rc.left -= nWidth;
			pWnd->SetWindowPos(NULL, rc.left-nWidth, rc.top, 0, 0, SWP_NOSIZE|SWP_NOOWNERZORDER);
		}
	}
	GetWindowRect(&rc);
	SetWindowPos(NULL, 0, 0, rc.Width()-nWidth, rc.Height(), SWP_NOMOVE|SWP_NOOWNERZORDER);
}