// InputDialog.cpp : implementation file
//

#include "stdafx.h"
#include "globals.h"
#include "systemverwaltung.h"

#include "SVDoc.h"
#include "SVView.h"

#include "AudioChannel.h"

#include "AudioChannelPage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define AUDIO_OFF  0
#define AUDIO_ON   1
#define AUDIO_I_ON 1
#define AUDIO_O_ON 2

int CAudioChannelDialog::gm_nList[3] = {IMAGE_NULL, IMAGE_AUDIO_IN, IMAGE_AUDIO_OUT};
/////////////////////////////////////////////////////////////////////////////
// CAudioChannelDialog dialog
CAudioChannelDialog::CAudioChannelDialog(CAudioChannelPage* pPage)
	: CWK_Dialog(CAudioChannelDialog::IDD)
{
	m_pChannelPage = pPage;
	m_pChannel     = NULL;
	m_nStereoChannel = 0;

	//{{AFX_DATA_INIT(CAudioChannelDialog)
	m_sName        = _T("");
	m_bActive = FALSE;
	m_bDefault       = FALSE;
	//}}AFX_DATA_INIT
	m_nNo = 1;
	m_bDrawLine = FALSE;
	m_nEditCtrl = IDC_AUC_EDT_NAME;
   m_bSetPos = false;
}
//////////////////////////////////////////////////////////////////////////
void CAudioChannelDialog::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAudioChannelDialog)
	DDX_Control(pDX, IDC_STATIC_TYPE_AUDIO, m_StaticTypeAudio);
	DDX_Control(pDX, IDC_SPIN_TYPE_AUDIO, m_SpinTypeAudio);
	DDX_Check(pDX, IDC_AUC_RD_DEFAULT, m_bDefault);
	//}}AFX_DATA_MAP

	DDX_Text(pDX, m_nEditCtrl, m_sName);
	
	if (pDX->m_bSaveAndValidate)
	{
		if (m_bActive)
		{
			if (m_sName.IsEmpty())
			{
				AfxMessageBox(IDP_NOEMPTY_FIELDS);
				pDX->Fail();
			}
		}
	}
	else if(m_pChannel)	// Info
	{
		DDX_Text(pDX, IDC_NR_INPUT, m_nNo);
		DDX_Text(pDX, IDC_AUC_TXT_CHANNEL, m_pChannel->m_sChannel);
		DDX_Text(pDX, IDC_AUC_TXT_LINE_CHANNEL, m_sStereoChannel);
		DDX_Text(pDX, IDC_AUC_TXT_UNIT, m_pChannel->m_sDeviceName);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAudioChannelDialog::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAudioChannelDialog, CWK_Dialog)
	//{{AFX_MSG_MAP(CAudioChannelDialog)
	ON_EN_CHANGE(IDC_AUC_EDT_NAME, OnChangeName)
	ON_BN_CLICKED(IDC_AUC_RD_DEFAULT, OnAucRdDefault)
	ON_EN_CHANGE(IDC_AUC_EDT_NAME2, OnChangeName)
	ON_EN_CHANGE(IDC_EDIT_TYPE_AUDIO, OnChangeEditTypeAudio)
	//}}AFX_MSG_MAP
//	ON_WM_VSCROLL()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudioChannelDialog message handlers
void CAudioChannelDialog::OnOK() 
{
}
/////////////////////////////////////////////////////////////////////////////
void CAudioChannelDialog::OnCancel() 
{
}
/////////////////////////////////////////////////////////////////////////////
void CAudioChannelDialog::PostNcDestroy() 
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAudioChannelDialog::OnInitDialog() 
{
	CWK_Dialog::OnInitDialog();
	if (CSkinDialog::SetChildWindowFont(m_hWnd))
	{
		if (m_ToolTip.m_hWnd)
		{
			m_ToolTip.SetFont(CSkinDialog::GetDialogItemGlobalFont(), 0);
		}
	}

	RemoveFromResize(IDC_NR_INPUT);
	RemoveFromResize(IDC_EDIT_TYPE_AUDIO);
	RemoveFromResize(IDC_SPIN_TYPE_AUDIO);
	RemoveFromResize(IDC_STATIC_TYPE_AUDIO);

	m_StaticTypeAudio.ModifyStyle(0, SS_ICON, 0);
	m_SpinTypeAudio.SetRange32(1, 0);
	ChannelToControl();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
//////////////////////////////////////////////////////////////////////////
void CAudioChannelDialog::OnChangeName() 
{
	// TODO! RKE: Implement CSkinEdit::SetForbiddenCharacters(CString s);
	GetDlgItemText(m_nEditCtrl, m_sName);
	int nLength = m_sName.GetLength();
	if (nLength && m_sName.GetAt(nLength-1) == _T('\\'))
	{
		m_sName.SetAt(nLength-1, 0);
		SetDlgItemText(m_nEditCtrl, m_sName);
	}

	m_pChannelPage->SetModified();
}
//////////////////////////////////////////////////////////////////////////
void CAudioChannelDialog::OnChange() 
{
	m_pChannelPage->SetModified();
}
//////////////////////////////////////////////////////////////////////////
void CAudioChannelDialog::OnAucRdDefault() 
{
	OnChange();
	if (m_pChannel)
	{
		if (!m_bDefault)
		{
			m_pChannel->SetDefault(true);
			m_pChannel->SetStereoChannel(m_nStereoChannel);
			m_pChannelPage->CheckDefault(m_pChannel->GetUnit(), m_pChannel->GetChannel(), m_nStereoChannel);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
// Implementation
void CAudioChannelDialog::ChannelToControl()
{
	if (m_pChannel)
	{
		m_sName    = m_pChannel->GetName(m_nStereoChannel);
		m_bDefault = false;
		if (m_pChannel->GetStereoChannel() == m_nStereoChannel)
		{
			m_bDefault = m_pChannel->IsDefault();
		}
		m_bActive   = m_pChannel->IsActive(m_nStereoChannel);
		if (m_pChannel->IsInput())
		{
			m_nEditCtrl = IDC_AUC_EDT_NAME;
		}
		else
		{
			m_nEditCtrl = IDC_AUC_EDT_NAME2;
		}
		GetDlgItem(m_nEditCtrl)->ShowWindow(SW_SHOW);
		m_bSetPos = true;
		int nPos = m_bActive ? AUDIO_ON : AUDIO_OFF;
		if (m_SpinTypeAudio.SetPos(nPos) == nPos)
		{
			OnChangeEditTypeAudio();
		}
	}
	UpdateData(FALSE);
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAudioChannelDialog::ControlToChannel()
{
	BOOL bUpdate = UpdateData();
	if (bUpdate && m_pChannel)
	{
		m_bActive = (m_SpinTypeAudio.GetPos() == AUDIO_OFF) ? FALSE : TRUE;
		m_pChannel->SetName(  m_nStereoChannel, m_sName);
		m_pChannel->SetActive(m_nStereoChannel, m_bActive ? true : false);
	}
	return bUpdate;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioChannelDialog::ShowHide()
{
	GetDlgItem(IDC_AUC_EDT_NAME)->EnableWindow(m_bActive);
	GetDlgItem(IDC_AUC_EDT_NAME2)->EnableWindow(m_bActive);
	GetDlgItem(IDC_AUC_RD_DEFAULT)->EnableWindow(m_bActive);
	GetDlgItem(IDC_LINE_ONE)->ShowWindow(m_bDrawLine ? SW_SHOW : SW_HIDE);
}
/////////////////////////////////////////////////////////////////////////////
void CAudioChannelDialog::CreateDlg()
{
	Create(CAudioChannelDialog::IDD, m_pChannelPage);
}
/////////////////////////////////////////////////////////////////////////////
void CAudioChannelDialog::SetChannel(CAudioChannel* pChannel, bool &bLeft, bool &bNext, int &nCount)
{
	m_pChannel = pChannel;
	if (m_pChannel)
	{
		if (m_pChannel->IsStereo())
		{
			m_sStereoChannel.LoadString(bLeft ? IDS_LEFT : IDS_RIGHT);
			m_nStereoChannel = bLeft ? 0 : 1;
			if (bLeft)
			{
				bLeft = false;
				bNext = false;
			}
			else
			{
				bLeft = true;
				bNext = true;
			}
		}
		else
		{
			m_sStereoChannel = _T("--");
			m_nStereoChannel = 0;
			bNext = true;
		}
	}
	else
	{
		bNext = true;
	}
	if (bNext)
	{
		nCount++;
	}
}

void CAudioChannelDialog::OnChangeEditTypeAudio() 
{
	if (m_SpinTypeAudio.m_hWnd == NULL) return; 

	int nPos = m_SpinTypeAudio.GetPos();
	if (nPos >= AUDIO_OFF && nPos <= AUDIO_ON)
	{
		m_bActive = (nPos == AUDIO_ON) ? TRUE : FALSE;
		if (m_pChannel)
		{
			m_pChannel->SetActive(m_nStereoChannel, m_bActive ? true : false);
			if (m_pChannel->IsInput())										// Input
			{
				if (!m_bSetPos && !m_bActive)
				{
					if (!m_pChannelPage->CheckDeactivateChannel())
					{
						m_bActive = TRUE;
						m_bSetPos = true;
						m_SpinTypeAudio.SetPos(AUDIO_ON);
						return;
					}
				}
			}
			else																	// Output
			{
				if (m_bActive)
				{
					nPos = AUDIO_O_ON;										// only for the icon
				}
			}
		}
		
		if (m_bSetPos)
		{
			m_bSetPos = false;
		}
		else
		{
			OnChange();
		}

		HICON hIcon = ::ImageList_GetIcon(HIMAGELIST(*((CSVApp*)AfxGetApp())->GetSmallImage()), gm_nList[nPos], ILD_TRANSPARENT);
		m_StaticTypeAudio.SetIcon(hIcon);
		ShowHide();
	}
}
