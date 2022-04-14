/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: AudioChannelPage.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/AudioChannelPage.cpp $
// CHECKIN:		$Date: 1/30/06 4:47p $
// VERSION:		$Revision: 16 $
// LAST CHANGE:	$Modtime: 1/30/06 4:20p $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "systemverwaltung.h"
#include "globals.h"

#include "SVDoc.h"
#include "SVView.h"

#include "AudioChannel.h"
#include "AudioChannelPage.h"
#include "AudioPage.h"
#include "IPCControlAudioUnit.h"

#ifdef _WK_CLASSES_DLL
	#define COMPILE_MULTIMON_STUBS
	#pragma warning(push)
	#pragma warning(disable: 4706)
		#include "multimon.h"
	#pragma warning(pop)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define MAX_CHANNELS_ON_PAGE	12

/////////////////////////////////////////////////////////////////////////////
// CAudioChannelPage property page
IMPLEMENT_DYNAMIC(CAudioChannelPage, CSVPage)

/////////////////////////////////////////////////////////////////////////////
CAudioChannelPage::CAudioChannelPage(CSVView* pParent, bool bInputs) : CSVPage(CAudioChannelPage::IDD)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CAudioChannelPage)
	m_iScroll = 0;
	//}}AFX_DATA_INIT
	LONG    lResult;
	DWORD    dwLen, dwIndex;
	HKEY    hKey = NULL;
	TCHAR    pszName[MAX_PATH];
	
	m_bRecursive = false;
	m_bInputs    = bInputs;
	m_nChannels  = 0;
	m_nUnits     = 0;

	CWK_Profile &prof = m_pParent->GetDocument()->GetProfile();
	CString sCapture, sKey, sType = m_bInputs ? SEC_MEDIA_TYPE_INPUT : SEC_MEDIA_TYPE_OUTPUT;
	CString sBase     = prof.GetSection() + _T("\\");

	int i;
	for (i=1; i<=SM_NO_OF_AUDIO_UNITS; i++)
	{
		if (CAudioPage::IsUnitActive(prof, i))
		{
			sKey.Format(AUDIO_UNIT_DEVICE, i);
			sKey = sBase + sKey;
			CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, sKey, _T(""));
			if (m_bInputs)
			{
				sCapture  = wkp.GetString(AU_DIRECT_SHOW, AUDIO_INPUT_DEVICE_GUID , NULL);
			}
			else
			{
				sCapture  = wkp.GetString(AU_DIRECT_SHOW, AUDIO_OUTPUT_DEVICE_GUID , NULL);
			}
			if (sCapture.IsEmpty())
			{
				break;
			}
			sKey = _T("Software\\") + sBase + AUDIO_UNIT +  _T("\\") + sType +  _T("\\") + sCapture;
			
			lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sKey, 0, KEY_READ, &hKey);
			if (lResult == ERROR_SUCCESS)
			{
				for (dwIndex=0; ; dwIndex++)
				{
					dwLen   = MAX_PATH;
					lResult = RegEnumKeyEx(hKey, dwIndex, pszName, &dwLen, NULL, NULL, 0, NULL);
					if (lResult != ERROR_SUCCESS) break;
					CAudioChannel *pChannel = new CAudioChannel(pszName, dwIndex, 0, i, sCapture, m_bInputs);
					m_Channels.Add(pChannel);
					pChannel->Load(prof);
					m_nChannels++;
					if (pChannel->IsStereo()) m_nChannels++;
				}
			}

			m_nUnits++;
		}
	}

	Create(IDD,m_pParent);
}
/////////////////////////////////////////////////////////////////////////////
CAudioChannelPage::~CAudioChannelPage()
{
}
/////////////////////////////////////////////////////////////////////////////
void CAudioChannelPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAudioChannelPage)
	DDX_Control(pDX, IDC_TXTNR, m_staticInputNr);
	DDX_Control(pDX, IDC_SCROLLBAR, m_ScrollBar);
	DDX_Scroll(pDX, IDC_SCROLLBAR, m_iScroll);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAudioChannelPage, CSVPage)
	//{{AFX_MSG_MAP(CAudioChannelPage)
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
BOOL CAudioChannelPage::OnInitDialog() 
{
	CreateChannelDialogs();

	CSVPage::OnInitDialog();
	if (m_bInputs)
	{
		GetDlgItem(IDC_AUCP_CHANNEL)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_AUCP_CHANNEL_OUT)->ShowWindow(SW_SHOW);
	}

	if (m_nChannels > MAX_CHANNELS_ON_PAGE)
	{
		m_ScrollBar.SetScrollRange(0, m_nChannels-1);
		SetPageSize(m_ScrollBar, MAX_CHANNELS_ON_PAGE);
	}
	else
	{
		m_ScrollBar.ShowWindow(SW_HIDE);
	}

	SetModified(FALSE,FALSE);
	Resize();

	return TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CAudioChannelPage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (m_bRecursive) return;
	m_bRecursive = true;
	int oldScroll = m_iScroll;
	CSVPage::OnVScroll(m_ScrollBar, nSBCode, nPos, m_nChannels, m_iScroll);
	CWK_Dialog::OnVScroll(nSBCode, nPos, pScrollBar);

	if (oldScroll != m_iScroll)
	{
		Reset(oldScroll - m_iScroll);
	}
	m_bRecursive = false;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioChannelPage::OnSize(UINT nType, int cx, int cy) 
{
	CSVPage::OnSize(nType, cx, cy);
	
	Resize();
}
/////////////////////////////////////////////////////////////////////////////
void CAudioChannelPage::OnDestroy() 
{
	m_Channels.DeleteAll();
	CSVPage::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
int CAudioChannelPage::GetNoOfSubDlgs()
{
	return m_ChannelDlgs.GetSize();
}
/////////////////////////////////////////////////////////////////////////////
CWK_Dialog *CAudioChannelPage::GetSubDlg(int nDlg)
{
	if ((nDlg >=0) && (nDlg < m_ChannelDlgs.GetSize()))
	{
		return m_ChannelDlgs.GetAt(nDlg);
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
void CAudioChannelPage::CheckDefault(int nUnit, int nChannel, int nStereoChannel)
{
	CAudioChannel *pC;
	int i, nCount = m_Channels.GetSize();
	for (i=0; i<nCount; i++)
	{
		pC = m_Channels.GetAt(i);
		if (pC->GetUnit() == nUnit)
		{
			if (pC->GetChannel() != nChannel)
			{
				pC->SetDefault(false);
			}
			else if (pC->GetStereoChannel() != nStereoChannel)
			{
				pC->SetDefault(false);
			}
		}
	}
	nCount = m_ChannelDlgs.GetSize();
	for (i=0; i<nCount; i++)
	{
		m_ChannelDlgs.GetAt(i)->ChannelToControl();
	}	
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAudioChannelPage::IsDataValid()
{
	int i, c = m_ChannelDlgs.GetSize();
	BOOL bSaved = FALSE;

	for (i=0;i<c;i++)
	{
		bSaved = m_ChannelDlgs.GetAt(i)->ControlToChannel();
		if (!bSaved) break;
	}

	return bSaved;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAudioChannelPage::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioChannelPage::SaveChanges()
{
	WK_TRACE_USER(_T("Audiokanal-Einstellungen wurden geändert\n"));
	if (m_Channels.GetSize())
	{
		int   i, c;
		BOOL  bAnyEnabled = FALSE;
		CAudioChannel*pC = m_Channels.GetAt(0);
		CWK_Profile &prof = m_pParent->GetDocument()->GetProfile();
		CString sMedia, sInput, sInputName;

		CMedia      * pMedia = NULL;
		CMediaList  * pMediaList  = m_pParent->GetDocument()->GetMedias();
		sMedia.Format(SM_AudioUnitMedia, pC->GetUnit());
		CMediaGroup * pMediaGroup = pMediaList->GetGroupBySMName(sMedia);
		if (pMediaGroup)
		{
			int i, nSize = pMediaGroup->GetSize();
			for (i=0; i<nSize; i++)
			{
				if (m_bInputs && pMediaGroup->GetMedia(i)->GetType() == SEC_MEDIA_TYPE_INPUT)
				{
					pMedia = pMediaGroup->GetMedia(i);
					break;
				}
				else if (!m_bInputs && pMediaGroup->GetMedia(i)->GetType() == SEC_MEDIA_TYPE_OUTPUT)
				{
					pMedia = pMediaGroup->GetMedia(i);
					break;
				}
			}
		}
		
		c = m_Channels.GetSize();
		for (i=0; i<c; i++)
		{
			pC = m_Channels.GetAt(i);
			pC->Save(prof);
			if (pC->IsActive(pC->GetStereoChannel()))
			{
				bAnyEnabled = TRUE;
			}
			if (pC->IsDefault())
			{
				sInputName = pC->GetName(pC->GetStereoChannel());
				if (pMedia)
				{
					pMedia->SetName(sInputName);
				}
			}
		}

		if (pMedia)
		{
			pMedia->SetDisabled(!bAnyEnabled);
		}
		if (m_bInputs)
		{
			sInput.Format(SM_AudioUnitInput, pC->GetUnit());
			CInputList  * pInputList  = m_pParent->GetDocument()->GetInputs();
			CInputGroup * pInputGroup = pInputList->GetGroupBySMName(sInput);
			if (pInputGroup)
			{
				CInput *pInput = pInputGroup->GetInput(0);
				if (pInput && bAnyEnabled)
				{
					pInput->SetName(sInputName);
					pInputList->Save(prof, FALSE);
				}
			}
		}
		pMediaList->Save(prof, FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioChannelPage::CreateChannelDialogs()
{
	CAudioChannel* pChannel = NULL;
	CAudioChannelDialog* pChannelDlg;
	int nC=0, i;
	bool bNext = false;	// bNext und bLeft dienen zur Initialisierung des Dialogs
	bool bLeft = true;
	
	for (i=0; i<MAX_CHANNELS_ON_PAGE && i<m_nChannels ; i++)
	{
		pChannel = m_Channels.GetAt(nC);
		pChannelDlg = new CAudioChannelDialog(this);
		m_ChannelDlgs.Add(pChannelDlg);
		pChannelDlg->SetChannel(pChannel, bLeft, bNext, nC);
		pChannelDlg->m_nNo = i+1;
		pChannelDlg->CreateDlg();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioChannelPage::Reset(BOOL bDirection)
{
	int i, nDlgs, nD, nC, iScroll = m_iScroll;
	BOOL bSaved = FALSE;
	CAudioChannel* pChannel = NULL, *pLast = NULL;
	CAudioChannelDialog* pChannelDlg;

	nDlgs = m_ChannelDlgs.GetSize();
	
	// nur die Kanäle, die "verschwinden", müssen gesichert werden
	if (bDirection > 0)
	{
		nD = nDlgs-bDirection;
		for (i=nDlgs-1; i>=nD; i--)
		{
			bSaved = m_ChannelDlgs.GetAt(i)->ControlToChannel();
			if (!bSaved) break;
		}
	}
	else
	{
		nD = -bDirection;
		for (i=0; i<nD; i++)
		{
			bSaved = m_ChannelDlgs.GetAt(i)->ControlToChannel();
			if (!bSaved) break;
		}
	}
	
	if (bSaved)
	{
		bool bNext = false;	// bNext und bLeft dienen zur Initialisierung des Dialogs
		bool bLeft = true;
		pChannelDlg = m_ChannelDlgs.GetAt(0);
		for (i=0, nD=0, nC=0; i<m_nChannels; i++)
		{
			pChannel = m_Channels.GetAt(nC);
			if (i >= iScroll)
			{
				pChannelDlg = m_ChannelDlgs.GetAt(nD);
				pChannelDlg->SetChannel(pChannel, bLeft, bNext, nC);
				pChannelDlg->m_nNo = i+1;
				if (nD > 0 && pLast && pLast->GetUnit() != pChannel->GetUnit())
				{
					m_ChannelDlgs.GetAt(nD-1)->m_bDrawLine = TRUE;
					m_ChannelDlgs.GetAt(nD-1)->ShowHide();
				}
				pChannelDlg->m_bDrawLine = FALSE;
				pChannelDlg->ChannelToControl();
				if (++nD >= nDlgs)
				{
					break;
				}
			}
			else	// wird als Dummy aufgerufen:
			{		// verändert die Variablen bLeft, bNext und nC
				pChannelDlg->SetChannel(pChannel, bLeft, bNext, nC);
			}
			pLast = pChannel;
		}
	}
	else
	{
		m_iScroll += bDirection;
	}
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
void CAudioChannelPage::Resize()
{
	if (m_staticInputNr.m_hWnd)
	{
		CRect rect;
		CAudioChannelDialog* pChannelDlg;
		int i,c,height,off;
		CRect scrollrect;

		c = m_ChannelDlgs.GetSize();

		m_staticInputNr.GetWindowRect(rect);
		ScreenToClient(rect);
		off = rect.bottom + 10;

		if (c>0)
		{
			pChannelDlg = m_ChannelDlgs.GetAt(0);
			pChannelDlg->GetClientRect(rect);
			rect.top += off;
			rect.bottom += off;
			pChannelDlg->MoveWindow(rect);
			pChannelDlg->ShowWindow(SW_SHOW);
			height = rect.Height() + 5;
			scrollrect.left = rect.right + 5;
			scrollrect.right = scrollrect.left + GetSystemMetrics(SM_CXVSCROLL);
			scrollrect.top = rect.top;
			
			for (i=1;i<c;i++)
			{
				pChannelDlg = m_ChannelDlgs.GetAt(i);
				pChannelDlg->GetClientRect(rect);
				rect.top += height*i+off;
				rect.bottom += height*i+off;
				pChannelDlg->MoveWindow(rect);
				pChannelDlg->ShowWindow(SW_SHOW);
			}
			scrollrect.bottom = rect.bottom;
			m_ScrollBar.MoveWindow(scrollrect);
		}
		m_ScrollBar.ShowWindow((m_nChannels>MAX_CHANNELS_ON_PAGE) ? SW_SHOW : SW_HIDE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioChannelPage::CancelChanges() 
{
	CWK_Profile &prof = m_pParent->GetDocument()->GetProfile();
	int i, nCount = m_Channels.GetSize();
	for (i=0; i<nCount; i++)
	{
		m_Channels.GetAt(i)->Load(prof);
	}
	nCount = m_ChannelDlgs.GetSize();
	for (i=0; i<nCount; i++)
	{
		m_ChannelDlgs.GetAt(i)->ChannelToControl();
	}	
}

BOOL CAudioChannelPage::CheckDeactivateChannel()
{
	BOOL bAnyEnabled = FALSE;
	int i, c = m_Channels.GetSize();
	if (c)
	{
		CWK_Profile &prof = m_pParent->GetDocument()->GetProfile();
		CAudioChannel*pC = NULL;
		for (i=0; i<c; i++)
		{
			pC = m_Channels.GetAt(i);
			if (pC->IsActive(pC->GetStereoChannel()))
			{
				bAnyEnabled = TRUE;
			}
		}
		
		if (!bAnyEnabled)
		{
			CString sInput;
			sInput.Format(SM_AudioUnitInput, pC->GetUnit());
			CInputList  * pInputList  = m_pParent->GetDocument()->GetInputs();
			CInputGroup * pInputGroup = pInputList->GetGroupBySMName(sInput);
			if (pInputGroup)
			{
				CInput *pInput = pInputGroup->GetInput(0);
				if (pInput)
				{
					CString sMsg, sMsg2, sFormat;
					if (pInput->GetNumberOfInputToOutputs())
					{
						sFormat.LoadString(IDS_DELETE_INPUT);
						sMsg2.Format(sFormat, pInput->GetName());
					}
					sFormat.LoadString(IDS_DEACTIVATE_INPUT);
					sMsg.Format(sFormat, sMsg2);

					if (IDYES == AfxMessageBox(sMsg, MB_YESNO))
					{
						pInput->DeleteAllInputToOutputs();
						pInput->SetIDActivate(SECID_ACTIVE_OFF);
						pInputList->Save(prof, FALSE);
						return TRUE;
					}
				}
			}
		}
	}
	return bAnyEnabled;
}
