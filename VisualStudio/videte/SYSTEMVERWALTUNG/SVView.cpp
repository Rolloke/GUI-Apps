// SVView.cpp : implementation of the CSVView class
//


#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "SVDoc.h"
#include "SVView.h"
#include "oemgui\oemgui.h"

#include "SVTree.h"
#include "SVPage.h"
#include "SVPropertySheet.h"

#include "PropPageVariable.h"

// System
#include "HardwarePage.h"
	#include "QUnitPage.h"
	#include "JaCobPage.h"
	#include "SaVicPage.h"
	#include "TashaPage.h"
	#include "CameraParameterPage.h"
	#include "AudioPage.h"
	#include "ICPCONConfigPage.h"
	#include "STMPage.h"
	#include "YutaPage.h"
	#include "JoystickPage.h"
	#include "QuadPage.h"
// Verbindungen
	#include "ISDNPage.h"
	#include "PTUnitPage.h"
	#include "SocketPage.h"
	#include "SMSPage.h"
	#include "EMailPage.h"
// Datenbank
	#include "DrivePage.h"
	#include "ArchivPage.h"
	#include "DatabaseFieldPage.h"
#include "SoftwarePage.h"
	#include "SimPage.h"
	#include "TOBSPage.h"
	#include "RS232Page.h"
	#include "GemosPage.h"
	#include "MESZPage.h"
	#include "SDIPage.h"
	#include "AppPage.h"
	#include "NTLoginPage.h"
#include "MultiMonitorPage.h"
#include "LanguagePage.h"
#include "MaintenancePage.h"	// Wartung
#include "MiscellaneousPage.h"	// Diverse Einstellungen
#include "InputPage.h"			// Melder
#include "OutputPage.h"			// Kameras
#include "RelayPage.h"			// Relais
#include "AudioChannelPage.h"	// Audio
#include "HostPage.h"
#include "SecurityPage.h"
#include "PermissionPage.h"
#include "UserPage.h"
#include "TimerEditorPage.h"
#include "NotificationPage.h"
// Prozesse
	#include "SavePage.h"
	#include "CallPage.h"
	#include "BackupProcessPage.h"
	#include "ActualImagePage.h"
	#include "ImageCallPage.h"
	#include "CheckCallPage.h"
	#include "GuardTourPage.h"
	#include "VideoOutPage.h"
	#include "UploadPage.h"
#include "ActivationPage.h"

#include "DefaultTreeKnode.h"

#include "CIPCServerControlClientSide.h"
#include "mainfrm.h"

#include "DesignaWizardDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define HKLM_HW_PROCESSOR	_T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0")

		//#define SV_TREE_WIDTH	200

/////////////////////////////////////////////////////////////////////////////
// CSVView

IMPLEMENT_DYNCREATE(CSVView, CView)

BEGIN_MESSAGE_MAP(CSVView, CView)
	//{{AFX_MSG_MAP(CSVView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_EDIT_NEW, OnEditNew)
	ON_UPDATE_COMMAND_UI(ID_EDIT_NEW, OnUpdateEditNew)
	ON_COMMAND(ID_EDIT_CANCEL, OnEditCancel)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CANCEL, OnUpdateEditCancel)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_COMMAND(ID_EDIT_SERVERINIT, OnEditServerinit)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SERVERINIT, OnUpdateEditServerinit)
	ON_COMMAND(ID_VIEW_TOOLTIPS, OnViewTooltips)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLTIPS, OnUpdateViewTooltips)
	ON_COMMAND(ID_WIZZ_FIRST, OnWizFirst)
	ON_UPDATE_COMMAND_UI(ID_WIZZ_FIRST, OnUpdateWizFirst)
	ON_COMMAND_RANGE(ID_WIZ_ROOM_SURVEILLANCE_MD, ID_WIZ_UVVK_RECORDING, OnWizRoomSurveillance)
	ON_UPDATE_COMMAND_UI(ID_WIZ_ROOM_SURVEILLANCE_MD, OnUpdateWizRoomSurveillanceMd)
	ON_UPDATE_COMMAND_UI(ID_WIZ_PERMANENT_RECORDING, OnUpdateWizPermanentRecording)
	ON_UPDATE_COMMAND_UI(ID_WIZ_UVVK_RECORDING, OnUpdateWizUvvkRecording)
	ON_COMMAND(ID_WIZ_DESIGNAPM100RECORDING, OnWizDesignaPM100Recording)
	ON_UPDATE_COMMAND_UI(ID_WIZ_DESIGNAPM100RECORDING, OnUpdateWizDesignaPM100Recording)
	ON_COMMAND(ID_WIZ_DESIGNAPMABACUSRECORDING, OnWizDesignaPMAbacusRecording)
	ON_UPDATE_COMMAND_UI(ID_WIZ_DESIGNAPMABACUSRECORDING, OnUpdateWizDesignaPMAbacusRecording)
	ON_MESSAGE(WM_USER, OnUserMessage)
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_MESSAGE(WM_SELECT_PAGE, OnSelectPage)
	ON_MESSAGE(WM_SET_WIZARD_SIZE, OnSetWizardSize)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVView construction/destruction
CSVView::sRoomSurveillance::sRoomSurveillance()
{
	m_bSetNames		 = TRUE;
	m_dwBoardActive	 = 0;
	m_dwBoardMD		 = 0;
	m_nInitialPageID = 0;
	m_bUVVK			 = 0;
	m_bMD			 = TRUE;
}

// extern TCHAR szJaCobMDFormat[];

CSVView::CSVView()
{
	m_pSVTree = new CSVTree(this);
	m_pSVPage = NULL;
	m_bResizing = FALSE;
	m_bClearing = FALSE;
	m_bShowToolTips = TRUE;
	m_lTreeWidth = GetSystemMetrics(SM_CXFULLSCREEN) / 4;

	ClearHTREEITEMs();

	m_bServerInit = FALSE;
	m_bInitApps = FALSE;

	m_pSVWizard = NULL;
	m_pRoomSurveillanceMd = NULL;
	m_lWizardHeight = 0;
}

CSVView::~CSVView()
{
	delete m_pSVTree;
	WK_DELETE(m_pSVWizard);
	WK_DELETE(m_pRoomSurveillanceMd);
}

void CSVView::ClearHTREEITEMs()
{
	m_SelectedItem = NULL;
	m_DeletedItem  = NULL;

	m_hSystemPage = NULL;
		m_hHardwarePage = NULL;
			m_hQUnitPage = NULL;
			m_hIPCameraPage = NULL;
			
			m_hJaCobPage1 = NULL;
			m_hJaCobPage2 = NULL;
			m_hJaCobPage3 = NULL;
			m_hJaCobPage4 = NULL;
			
			m_hSaVicPage1 = NULL;
			m_hSaVicPage2 = NULL;
			m_hSaVicPage3 = NULL;
			m_hSaVicPage4 = NULL;
			
			m_hTashaPage1 = NULL;
			m_hTashaPage2 = NULL;
			m_hTashaPage3 = NULL;
			m_hTashaPage4 = NULL;
			
			ZeroMemory(m_hAudioPage, sizeof(HTREEITEM)*SM_NO_OF_AUDIO_UNITS);
			m_hICPCONPage = NULL;
			m_hYUTAPage = NULL;
			m_hJoystickPage = NULL;
			m_hQuadPage = NULL;
		m_hLinkPage = NULL;
			m_hISDNPage1 = NULL;
			m_hISDNPage2 = NULL;
			m_hPTUnitPage1 = NULL;
			m_hPTUnitPage2 = NULL;
			m_hTOBSUnitPage = NULL;
			m_hSocketPage = NULL;
			m_hSMSPage = NULL;
			m_hEMailPage = NULL;
	m_hDatabase = NULL;
		m_hDrives = NULL;
		m_hArchivs = NULL;
		m_hDatabaseFields = NULL;

		m_hSoftwarePage = NULL;
			m_hSTMPage = NULL;
			m_hSimPage = NULL;
			m_hRS232Page = NULL;
			m_hMESZPage = NULL;
			m_hSDIPage = NULL;
			m_hAutoLogoutPage = NULL;
			m_hNTLoginPage = NULL;
			m_hGemosPage = NULL;
		m_hMultiMonitorPage = NULL;
		m_hLanguagePage = NULL;
		m_hMaintenancePage = NULL;
		m_hMiscellaneousPage = NULL;
	m_hInputPage = NULL;
	m_hCameraPage = NULL;
	m_hRelaisPage = NULL;
   m_hAudioChannel = NULL;
		m_hAudioPageIn = NULL;
		m_hAudioPageOut = NULL;
	m_hSecurityPage = NULL;
		m_hPermissionPage = NULL;
		m_hUserPage = NULL;
	m_hTimerPage = NULL;
	m_hHostPage = NULL;
	m_hNotificationPage = NULL;
	m_hProcesses = NULL;
		m_hSaveProcessPage = NULL;
		m_hCallProcessPage = NULL;
		m_hBackupProcessPage = NULL;
		m_hActualImagePage = NULL;
		m_hImageCallPage = NULL;
		m_hCheckCallPage = NULL;
		m_hGuardTourPage = NULL;
		m_hVideoOutPage  = NULL;
		m_hUploadPage = NULL;
	m_hActivationPage = NULL;
}

BOOL CSVView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
//	cs.style |= DS_CONTROL | WS_CHILD;	// gf dass reicht nicht

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSVView drawing

void CSVView::OnDraw(CDC* pDC)
{
//	CSVDoc* pDoc = GetDocument();
//	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CSVView printing

BOOL CSVView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSVView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSVView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CSVView diagnostics

#ifdef _DEBUG
void CSVView::AssertValid() const
{
	CView::AssertValid();
}

void CSVView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSVDoc* CSVView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSVDoc)));
	return (CSVDoc*)m_pDocument;
}

CSVPage* CSVView::GetSVPage()
{
	ASSERT(m_pSVPage != NULL);
	return m_pSVPage;
}

CSVTree * CSVView::GetSVTree()
{
	ASSERT(m_pSVTree!= NULL);
	return m_pSVTree;
}

#endif //_DEBUG
CSVPropertySheet * CSVView::GetSVWizard()
{
	ASSERT(m_pSVWizard!= NULL);
	return m_pSVWizard;
}


/////////////////////////////////////////////////////////////////////////////
// CSVView message handlers
int CSVView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	CRect rect(0,0,200,200);
	if (!m_pSVTree->Create(WS_CHILD|WS_VISIBLE|WS_BORDER|
							TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS,
							rect,this,1))
	{
		return -1;
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::InitSystem()
{
	// HARDWARE
	if (GetDocument()->GetFlags() & WK_ALLOW_HARDWARE)
	{
		CString s;

		// SYSTEM
		s.LoadString(IDS_SYSTEM);
		m_hSystemPage = m_pSVTree->InsertItem(s,IMAGE_SYSTEM,IMAGE_SYSTEM);
		m_pSVTree->SetItemData(m_hSystemPage, IDS_SYSTEM_DESCR);
		if (   GetDocument()->GetDongle()->RunIPCameraUnit() 
			|| GetDocument()->GetDongle()->RunQUnit()
			|| GetDocument()->GetDongle()->RunJaCobUnit1()
			|| GetDocument()->GetDongle()->RunSaVicUnit1()
			|| GetDocument()->GetDongle()->RunAKUUnit()
			|| GetDocument()->GetDongle()->RunICPCONUnit()
			|| GetDocument()->GetDongle()->RunUSBCameraUnit()
			|| GetDocument()->GetDongle()->RunAudioUnit()
			|| GetDocument()->GetDongle()->RunTashaUnit1()
			)
		{
			s.LoadString(IDS_HARDWARE);
			m_hHardwarePage = m_pSVTree->InsertItem(s,IMAGE_HARDWARE,IMAGE_HARDWARE,m_hSystemPage);
			m_pSVTree->SetItemData(m_hHardwarePage, IDS_HARDWARE_DESCR);

			if (GetDocument()->GetDongle()->RunQUnit())
			{

				//check if an expansion of the dongle features was done before
				//check if the new nr of QUnit cameras is higher than the current nr
				int iNrOfQCamerasFromDongle = GetDocument()->GetDongle()->GetNrOfCameras();

				COutputGroup* pOutputGroup = NULL;
				COutputList*  m_pOutputList	= NULL;
				m_pOutputList = GetDocument()->GetOutputs();
				if(m_pOutputList)
				{
					pOutputGroup = m_pOutputList->GetGroupBySMName(SM_Q_OUTPUT_CAMERA);
					if (pOutputGroup) 
					{
						if(pOutputGroup->GetSize() < iNrOfQCamerasFromDongle)
						{
							//save new QUnit dongle informations to registry
							CQUnitPage tempQUnitDlg(this);
							tempQUnitDlg.SaveNewDongleFeaturesAfterExpansion();
						}
					}
				}

				s.LoadString(IDS_Q_UNIT);
				m_hQUnitPage = m_pSVTree->InsertItem(s,IMAGE_Q_UNIT,IMAGE_Q_UNIT,m_hHardwarePage);
			}

			if (GetDocument()->GetDongle()->RunIPCameraUnit() && GetDocument()->IsLocal())
			{
				s.LoadString(IDS_IP_CAMERA_UNIT);
				m_hIPCameraPage = m_pSVTree->InsertItem(s,IMAGE_IP_CAMERA,IMAGE_IP_CAMERA,m_hHardwarePage);
			}
		
			if (GetDocument()->GetDongle()->RunJaCobUnit1())
			{
				s.LoadString(IDS_JACOB1);
				m_hJaCobPage1 = m_pSVTree->InsertItem(s,IMAGE_JACOB,IMAGE_JACOB,m_hHardwarePage);
			}

			if (GetDocument()->GetDongle()->RunJaCobUnit2())
			{
				s.LoadString(IDS_JACOB2);
				m_hJaCobPage2 = m_pSVTree->InsertItem(s,IMAGE_JACOB,IMAGE_JACOB,m_hHardwarePage);
			}

			if (GetDocument()->GetDongle()->RunJaCobUnit3())
			{
				s.LoadString(IDS_JACOB3);
				m_hJaCobPage3 = m_pSVTree->InsertItem(s,IMAGE_JACOB,IMAGE_JACOB,m_hHardwarePage);
			}

			if (GetDocument()->GetDongle()->RunJaCobUnit4())
			{
				s.LoadString(IDS_JACOB4);
				m_hJaCobPage4 = m_pSVTree->InsertItem(s,IMAGE_JACOB,IMAGE_JACOB,m_hHardwarePage);
			}
			
			if (GetDocument()->GetDongle()->RunSaVicUnit1())
			{
				s.LoadString(IDS_SAVIC1);
				m_hSaVicPage1 = m_pSVTree->InsertItem(s,IMAGE_SAVIC,IMAGE_SAVIC,m_hHardwarePage);
			}

			if (GetDocument()->GetDongle()->RunSaVicUnit2())
			{
				s.LoadString(IDS_SAVIC2);
				m_hSaVicPage2 = m_pSVTree->InsertItem(s,IMAGE_SAVIC,IMAGE_SAVIC,m_hHardwarePage);
			}

			if (GetDocument()->GetDongle()->RunSaVicUnit3())
			{
				s.LoadString(IDS_SAVIC3);
				m_hSaVicPage3 = m_pSVTree->InsertItem(s,IMAGE_SAVIC,IMAGE_SAVIC,m_hHardwarePage);
			}

			if (GetDocument()->GetDongle()->RunSaVicUnit4())
			{
				s.LoadString(IDS_SAVIC4);
				m_hSaVicPage4 = m_pSVTree->InsertItem(s,IMAGE_SAVIC,IMAGE_SAVIC,m_hHardwarePage);
			}

			if (GetDocument()->GetDongle()->RunTashaUnit1())
			{
				CString s1;
				s.LoadString(IDS_TASHA1);
				s1.Format(_T("%s %d"),s,TASHA1);
				m_hTashaPage1 = m_pSVTree->InsertItem(s1,IMAGE_TASHA,IMAGE_TASHA,m_hHardwarePage);
			}
			if (GetDocument()->GetDongle()->RunTashaUnit2())
			{
				CString s1;
				s.LoadString(IDS_TASHA1);
				s1.Format(_T("%s %d"),s,TASHA2);
				m_hTashaPage2 = m_pSVTree->InsertItem(s1,IMAGE_TASHA,IMAGE_TASHA,m_hHardwarePage);
			}
			if (GetDocument()->GetDongle()->RunTashaUnit3())
			{
				CString s1;
				s.LoadString(IDS_TASHA1);
				s1.Format(_T("%s %d"),s,TASHA3);
				m_hTashaPage3 = m_pSVTree->InsertItem(s1,IMAGE_TASHA,IMAGE_TASHA,m_hHardwarePage);
			}
			if (GetDocument()->GetDongle()->RunTashaUnit4())
			{
				CString s1;
				s.LoadString(IDS_TASHA1);
				s1.Format(_T("%s %d"),s,TASHA4);
				m_hTashaPage4 = m_pSVTree->InsertItem(s1,IMAGE_TASHA,IMAGE_TASHA,m_hHardwarePage);
			}
			
			if (GetDocument()->IsLocal())	// nur lokal ändern
			{
				if (   GetDocument()->GetDongle()->RunAudioUnit()
					&& CAudioPage::IsUnitActive(GetDocument()->GetProfile(), -1))
				{
					s.Format(IDS_AUDIO, 1);
					m_hAudioPage[0] = m_pSVTree->InsertItem(s,IMAGE_AUDIO, IMAGE_AUDIO, m_hHardwarePage);
					for (int i=1; i<SM_NO_OF_AUDIO_UNITS; i++)
					{
						if (   CAudioPage::IsUnitActive(GetDocument()->GetProfile(), i) 
							 && CAudioPage::CanActivateUnit(GetDocument()->GetProfile(), i+1))
						{
							s.Format(IDS_AUDIO, i+1);
							m_hAudioPage[i] = m_pSVTree->InsertItem(s, IMAGE_AUDIO, IMAGE_AUDIO, m_hHardwarePage);
						}
					}
				}

				if (GetDocument()->GetDongle()->RunICPCONUnit() 
					&& (    GetDocument()->GetDongle()->RunAnyRelaisUnit()
						||  GetDocument()->GetFlags() & WK_ALLOW_INPUT
						)
					)
				{
					s.LoadString(IDS_ICPCON_MODULES);
					m_hICPCONPage = m_pSVTree->InsertItem(s,IMAGE_ICPCON,IMAGE_ICPCON,m_hHardwarePage);
				}

				if (GetDocument()->GetFlags() & WK_ALLOW_INPUT)
				{
					if (!theApp.IsNT40())
					{
						s.LoadString(IDS_YUTA_MODULES);
						m_hYUTAPage = m_pSVTree->InsertItem(s,IMAGE_YUTA,IMAGE_YUTA,m_hHardwarePage);
					}
				}
				if (!theApp.IsNT40())
				{
					s.LoadString(IDS_JOYSTICK);
					m_hJoystickPage =  m_pSVTree->InsertItem(s,IMAGE_JOYSTICK, IMAGE_JOYSTICK, m_hHardwarePage);
				}

				if (GetDocument()->GetDongle()->IsTransmitter())
				{
					s.LoadString(IDS_QUAD);
					m_hQuadPage =  m_pSVTree->InsertItem(s,IMAGE_QUAD, IMAGE_QUAD, m_hHardwarePage);
				}
			}

			if (   GetDocument()->GetDongle()->RunMiCoUnit() 
				|| GetDocument()->GetDongle()->RunCoCoUnit()
				|| GetDocument()->GetDongle()->RunJaCobUnit1()
				|| GetDocument()->GetDongle()->RunSaVicUnit1()
				|| GetDocument()->GetDongle()->RunTashaUnit1())
			{
				s.LoadString(IDS_STM);
				m_hSTMPage = m_pSVTree->InsertItem(s,IMAGE_STM,IMAGE_STM,m_hHardwarePage);
			}
		}

		// Linkunits
		if (   GetDocument()->GetDongle()->RunAnyLinkUnit() 
			|| GetDocument()->GetDongle()->RunAnyBoxUnit()
			)
		{
			s.LoadString(IDS_LINK);
			m_hLinkPage = m_pSVTree->InsertItem(s,IMAGE_LINK,IMAGE_LINK,m_hSystemPage);
			m_pSVTree->SetItemData(m_hLinkPage, IDS_LINK_DESCR);

			if (GetDocument()->GetDongle()->RunISDNUnit())
			{
				s.LoadString(IDS_ISDN1);
				m_hISDNPage1 = m_pSVTree->InsertItem(s,IMAGE_ISDN,IMAGE_ISDN,m_hLinkPage);
			}
			if (GetDocument()->GetDongle()->RunISDNUnit2())
			{
				s.LoadString(IDS_ISDN2);
				m_hISDNPage2 = m_pSVTree->InsertItem(s,IMAGE_ISDN,IMAGE_ISDN,m_hLinkPage);
			}
		
			if (GetDocument()->GetDongle()->RunPTUnit())
			{
				s.LoadString(IDS_PTUNIT1);
				m_hPTUnitPage1 = m_pSVTree->InsertItem(s,IMAGE_PTUNIT,IMAGE_PTUNIT,m_hLinkPage);
			}
			if (GetDocument()->GetDongle()->RunPTUnit2())
			{
				s.LoadString(IDS_PTUNIT2);
				m_hPTUnitPage2 = m_pSVTree->InsertItem(s,IMAGE_PTUNIT,IMAGE_PTUNIT,m_hLinkPage);
			}
			if (GetDocument()->GetDongle()->RunTOBSUnit())
			{
				s.LoadString(IDS_TOBS);
				m_hTOBSUnitPage = m_pSVTree->InsertItem(s,IMAGE_TOBS,IMAGE_TOBS,m_hLinkPage);
			}
			if (   GetDocument()->GetDongle()->RunSocketUnit()
				&& !GetDocument()->GetDongle()->IsDemo())
			{
				s.LoadString(IDS_SOCKET);
				m_hSocketPage = m_pSVTree->InsertItem(s,IMAGE_SOCKET,IMAGE_SOCKET,m_hLinkPage);
			}
			if (GetDocument()->GetDongle()->RunSMSUnit())
			{
				s.LoadString(IDS_SMS);
				m_hSMSPage = m_pSVTree->InsertItem(s,IMAGE_SMS,IMAGE_SMS,m_hLinkPage);
			}
			if (GetDocument()->GetDongle()->RunEMailUnit())
			{
				s.LoadString(IDS_EMAIL);
				m_hEMailPage = m_pSVTree->InsertItem(s,IMAGE_EMAIL,IMAGE_EMAIL,m_hLinkPage);
			}
		}

		// SOFTWARE
		s.LoadString(IDS_SOFTWARE);
		m_hSoftwarePage = m_pSVTree->InsertItem(s,IMAGE_SOFTWARE,IMAGE_SOFTWARE,m_hSystemPage);

		if (GetDocument()->GetDongle()->RunSimUnit())
		{
			s.LoadString(IDS_SIM);
			m_hSimPage = m_pSVTree->InsertItem(s,IMAGE_SIM,IMAGE_SIM,m_hSoftwarePage);
		}
		if (GetDocument()->GetDongle()->RunCommUnit())
		{
			s.LoadString(IDS_RS232);
			m_hRS232Page = m_pSVTree->InsertItem(s,IMAGE_RS232,IMAGE_RS232,m_hSoftwarePage);
		}
		if (GetDocument()->GetDongle()->RunGemosUnit())
		{
			s.LoadString(IDS_GEMOS);
			m_hGemosPage = m_pSVTree->InsertItem(s,IMAGE_GEMOS,IMAGE_GEMOS,m_hSoftwarePage);
		}
		if (   !theApp.m_pPermission->IsSpecialReceiver()
			&& !GetDocument()->GetDongle()->IsDemo())
		{
			s.LoadString(IDS_MESZ);
			m_hMESZPage = m_pSVTree->InsertItem(s,IMAGE_MESZ,IMAGE_MESZ,m_hSoftwarePage);
		}
		if (!GetDocument()->GetDongle()->IsDemo())
		{
			s.LoadString(IDS_AUTOLOGOUT);
			m_hAutoLogoutPage = m_pSVTree->InsertItem(s,IMAGE_AUTOLOGOUT,IMAGE_AUTOLOGOUT,m_hSoftwarePage);
		}

		if (   GetDocument()->IsLocal()
			&& theApp.IsNT()
			&& !GetDocument()->GetDongle()->IsDemo()
			&& !theApp.m_pPermission->IsSpecialReceiver()
			)
		{
			s.LoadString(IDS_NTLOGON);
			m_hNTLoginPage = m_pSVTree->InsertItem(s,IMAGE_LOCK,IMAGE_LOCK,m_hSoftwarePage);
		}

		if (   GetDocument()->IsLocal()
			&& !theApp.IsNT40()
			&& GetDocument()->GetDongle()->AllowMultiMonitor())
		{
			CMonitorInfo mi;
			if (mi.GetNrOfMonitors() > 1)
			{
				s.LoadString(IDS_MULTI_MONITOR);
				m_hMultiMonitorPage = m_pSVTree->InsertItem(s,IMAGE_MULTI_MONITOR,IMAGE_MULTI_MONITOR,m_hSystemPage);
			}
		}
		s.LoadString(IDS_LANGUAGE);
		m_hLanguagePage =  m_pSVTree->InsertItem(s,IMAGE_LANGUAGE, IMAGE_LANGUAGE,m_hSystemPage);

		if (   GetDocument()->IsLocal()
			&& GetDocument()->GetDongle()->IsTransmitter()
			&& GetDocument()->GetDongle()->GetOemCode() != CWK_Dongle::OEM_ALARMCOM) // Siemens BT has still no maintenance
		{
			s.LoadString(IDS_MAINTENANCE);
			m_hMaintenancePage =  m_pSVTree->InsertItem(s,IMAGE_MAINTENANCE, IMAGE_MAINTENANCE,m_hSystemPage);
		}

		s.LoadString(IDS_MISCELLANEOUS);
		m_hMiscellaneousPage =  m_pSVTree->InsertItem(s,IMAGE_MISCELLANEOUS, IMAGE_MISCELLANEOUS, m_hSystemPage);
	}
}

//#define MAX_VIRTUAL_ALARM 19 gf abhaengig von Version!!!
/////////////////////////////////////////////////////////////////////////////
void CSVView::InitInput()
{
	// ACHTUNG!!!
	// virtuelle Alarme koennen mit steigender Version nur hinzugefuegt,
	// aber niemals entfernt werden!!!
	if (GetDocument()->GetFlags() & WK_ALLOW_INPUT)
	{
		CString s;
		s.LoadString(IDS_INPUT);
		m_hInputPage = m_pSVTree->InsertItem(s,IMAGE_INPUT,IMAGE_INPUT);
		m_pSVTree->SetItemData(m_hInputPage, IDS_INPUT_DESCR);

		// Virtuelle Alarmmelder anlegen
		// Erstmal schauen, ob schon eine Gruppe vorhanden
		CInputList *m_pInputList = GetDocument()->GetInputs();
		CInputGroup* pInputGroup = m_pInputList->GetGroupBySMName(SM_LAUNCHER_INPUT);

		// Aktuelle Liste der virtuellen Alarme nach Versionsstand anlegen
		// ACHTUNG: Reihenfolge NIE aendern
		CString sVersion = GetDocument()->GetProfile().GetString(_T("Version"),_T("Number"),_T(""));
		CStringArray sInputNameArray;
		CInputGroup::GetVirtualNames(sVersion,sInputNameArray);


		// gesamte Anzahl der virtuellen Alarme
		int iAlarms = sInputNameArray.GetSize();

		// wenn keine alte Gruppe vorhanden, oder die Anzahl von der aktuellen abweicht
		// neue Gruppe anlegen, alte Aktivierungen gehen dann verloren!
		if (!pInputGroup || pInputGroup->GetSize() != iAlarms)
		{
			if (pInputGroup)
			{
				pInputGroup->SetSize((WORD)iAlarms);
			}
			else
			{
				pInputGroup = m_pInputList->AddInputGroup(NULL, iAlarms, SM_LAUNCHER_INPUT);
			}
			
			CString sTmp;
			sTmp.LoadString(IDS_VA_ALWAYS_ALARM);
			for (int i = 0; i < iAlarms; i++)
			{
				pInputGroup->GetInput(i)->SetName(sInputNameArray.GetAt(i));
				// VA-Daueralarm soll auf Öffner stehen.
				if (sInputNameArray.GetAt(i) == sTmp)
					pInputGroup->GetInput(i)->SetEdge(TRUE);
				// Inputs sind standardmaessig ACTIVATE_OFF, setzen also unnoetig
			}
			m_pInputList->Save(GetDocument()->GetProfile(),
							   GetDocument()->IsLocal());
		}
		OnInputChanged();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::InitOutput()
{
	if (GetDocument()->GetFlags() & WK_ALLOW_OUTPUT)
	{
		CString s;
		if (GetDocument()->GetDongle()->RunAnyCameraUnit())
		{
			s.LoadString(IDS_OUTPUT);
			m_hCameraPage = m_pSVTree->InsertItem(s,IMAGE_CAMERA,IMAGE_CAMERA);
			m_pSVTree->SetItemData(m_hCameraPage, IDS_OUTPUT_DESCR);
			OnCameraChanged();
		}
		if (GetDocument()->GetDongle()->RunAnyRelaisUnit())
		{
			s.LoadString(IDS_RELAY);
			m_hRelaisPage = m_pSVTree->InsertItem(s,IMAGE_RELAY,IMAGE_RELAY);
			m_pSVTree->SetItemData(m_hRelaisPage, IDS_RELAY_DESCR);
			OnRelaisChanged();
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CSVView::InitAudio(HTREEITEM hInsertAfter/*=TVI_LAST*/)
{
	// Audio
	CWK_Profile &wkp = GetDocument()->GetProfile();
	if (   GetDocument()->GetDongle()->RunAudioUnit()
		&& CAudioPage::IsUnitActive(wkp, -1)
		&& (GetDocument()->GetFlags() & WK_ALLOW_HARDWARE)
		)
	{
		CString sIn, sOut, sKey;
		sIn.Format(_T("%s\\%s"), AUDIO_UNIT, SEC_MEDIA_TYPE_INPUT);
		sOut.Format(_T("%s\\%s"), AUDIO_UNIT, SEC_MEDIA_TYPE_OUTPUT);
		int i, nInputChannels = 0, nOutputChannels = 0;
		for (i=1; i<=SM_NO_OF_AUDIO_UNITS; i++)
		{
			if (CAudioPage::IsUnitActive(wkp, i))
			{
				sKey.Format(_T("%s%d"), AU_CHANNEL, i);
				nInputChannels += wkp.GetInt(sIn, sKey, 0);
				nOutputChannels += wkp.GetInt(sOut, sKey, 0);
			}
		}
		
		if (m_hAudioChannel == NULL)
		{
			sIn.LoadString(IDS_AUDIO_CHANNELS);
			m_hAudioChannel = m_pSVTree->InsertItem(sIn, IMAGE_AUDIO, IMAGE_AUDIO, TVI_ROOT, hInsertAfter);
			m_pSVTree->SetItemData(m_hAudioChannel, IDS_AUDIO_DESCR);
		}

		if (nInputChannels != 0 && m_hAudioPageIn == NULL)
		{
			sIn.LoadString(IDS_AUDIO_INPUT);
			m_hAudioPageIn = m_pSVTree->InsertItem(sIn,IMAGE_AUDIO_IN, IMAGE_AUDIO_IN, m_hAudioChannel);
		}
		else if (nInputChannels == 0 && m_hAudioPageIn != NULL)
		{
			m_pSVTree->DeleteItem(m_hAudioPageIn);
			m_hAudioPageIn = NULL;
		}

		if (nOutputChannels != 0 && m_hAudioPageOut == NULL)
		{
			sOut.LoadString(IDS_AUDIO_OUTPUT);
			m_hAudioPageOut = m_pSVTree->InsertItem(sOut,IMAGE_AUDIO_OUT, IMAGE_AUDIO_OUT, m_hAudioChannel);
		}
		else if (nOutputChannels == 0 && m_hAudioPageOut != NULL)
		{
			m_pSVTree->DeleteItem(m_hAudioPageOut);
			m_hAudioPageOut = NULL;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::InitHost()
{
	// HOSTS
	if (GetDocument()->GetFlags() & WK_ALLOW_HOST)
	{
		CString s;
		s.LoadString(IDS_HOSTS);
		m_hHostPage = m_pSVTree->InsertItem(s,IMAGE_HOST,IMAGE_HOST);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::InitSecurity()
{
	CString s;
	// SECURITY
	if (GetDocument()->GetFlags() & WK_ALLOW_SECURITY)
	{
		s.LoadString(IDS_SAFETY);
		m_hSecurityPage = m_pSVTree->InsertItem(s,IMAGE_SECURITY,IMAGE_SECURITY);
	}

	// PERMISSION
	if (GetDocument()->GetFlags() & WK_ALLOW_PERMISSION)
	{
		s.LoadString(IDS_PERMISSIONPAGE);
		m_hPermissionPage = m_pSVTree->InsertItem(s,IMAGE_PERM,IMAGE_PERM);
	}

	// USER
	if (GetDocument()->GetFlags() & WK_ALLOW_USER)
	{
		s.LoadString(IDS_USERPAGE);
		m_hUserPage = m_pSVTree->InsertItem(s,IMAGE_USER,IMAGE_USER);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::InitTimer()
{
	// TIMER
	if (   GetDocument()->GetFlags() & WK_ALLOW_TIMER	// permissions
		&& GetDocument()->GetDongle()->AllowTimers())	// dongle
	{
		if (   GetDocument()->GetDongle()->IsTransmitter() 
			|| GetDocument()->GetDongle()->AllowLocalProcesses()
			|| GetDocument()->GetDongle()->IsLocal())
		{
			CString s;
			s.LoadString(IDS_TIMERPAGE);
			m_hTimerPage = m_pSVTree->InsertItem(s,IMAGE_TIMER,IMAGE_TIMER);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::InitNotification()
{
	if (GetDocument()->GetFlags() & WK_ALLOW_PROZESS)
	{
		if (   GetDocument()->GetDongle()->AllowLocalProcesses()
			|| GetDocument()->GetDongle()->IsTransmitter()
			|| GetDocument()->GetDongle()->IsLocal())
		{
			CString s;
			s.LoadString(IDS_NOTIFICATIONPAGE);
			m_hNotificationPage = m_pSVTree->InsertItem(s,IMAGE_NOTIFICATION,IMAGE_NOTIFICATION);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::InitDatabase()
{
	if (GetDocument()->GetFlags() & WK_ALLOW_ARCHIV)
	{
		CString s;
		s.LoadString(IDS_DATABASE);
		m_hDatabase = m_pSVTree->InsertItem(s,IMAGE_DATABASE,IMAGE_DATABASE);
		m_pSVTree->SetItemData(m_hDatabase, IDS_DATABASE_DESCR);
		s.LoadString(IDS_DRIVES);
		m_hDrives = m_pSVTree->InsertItem(s,IMAGE_DRIVES,IMAGE_DRIVES,m_hDatabase);
		s.LoadString(IDS_ARCHIVES);
		m_hArchivs = m_pSVTree->InsertItem(s,IMAGE_ARCHIVS,IMAGE_ARCHIVS,m_hDatabase);
		if (!GetDocument()->GetDongle()->IsDemo())
		{
			if (   GetDocument()->GetDongle()->RunSDIUnit()
				|| GetDocument()->GetDongle()->IsReceiver())
			{
				s.LoadString(IDS_DATABASE_FIELDS);
				m_hDatabaseFields = m_pSVTree->InsertItem(s,IMAGE_DATABASE,IMAGE_DATABASE,m_hDatabase);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::InitSDI()
{
	if (   GetDocument()->GetDongle()->RunSDIUnit()
		&& GetDocument()->GetFlags() & WK_ALLOW_ARCHIV
		)
	{
		CString s = COemGuiApi::GetApplicationNameOem(WAI_SDIUNIT);
		m_hSDIPage = m_pSVTree->InsertItem(s,IMAGE_SDI,IMAGE_SDI);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::InitProcesses()
{
	CString s;

	// PROCESS
	if (GetDocument()->GetFlags() & WK_ALLOW_PROZESS)
	{
		if (   GetDocument()->GetDongle()->AllowLocalProcesses()
		 	 || GetDocument()->GetDongle()->IsTransmitter()
			 || GetDocument()->GetDongle()->IsLocal())
		{
			s.LoadString(IDS_PROCESSES);
			m_hProcesses = m_pSVTree->InsertItem(s,IMAGE_PROCESS,IMAGE_PROCESS);
			m_pSVTree->SetItemData(m_hProcesses, IDS_PROCESSES_DESCR);

			s.LoadString(IDS_SAVEPROCESSPAGE);
			m_hSaveProcessPage = m_pSVTree->InsertItem(s,IMAGE_SAVE,IMAGE_SAVE,m_hProcesses);

			s.LoadString(IDS_CALLPROCESSPAGE);
			m_hCallProcessPage = m_pSVTree->InsertItem(s,IMAGE_CALL,IMAGE_CALL,m_hProcesses);

			if (GetDocument()->GetDongle()->RunCDRWriter())
			{
				s.LoadString(IDS_BACKUPPROCESSPAGE);
				m_hBackupProcessPage = m_pSVTree->InsertItem(s,IMAGE_BACKUP,IMAGE_BACKUP,m_hProcesses);
			}
			CString sVersion = GetDocument()->GetProfile().GetString(_T("Version"),_T("Number"),_T(""));
			if (sVersion >= _T("4.1"))
			{
				s.LoadString(IDS_CHECK_CALL);
				m_hCheckCallPage = m_pSVTree->InsertItem(s,IMAGE_CHECK_CALL,IMAGE_CHECK_CALL,m_hProcesses);
				// CheckCall process has to be initialized
				CCheckCallPage(this);
#ifdef _DEBUG
				s.LoadString(IDS_IMAGE_CALL);
				m_hImageCallPage = m_pSVTree->InsertItem(s,IMAGE_IMAGE_CALL,IMAGE_IMAGE_CALL,m_hProcesses);
#endif
			}
			if (sVersion >= _T("4.6"))
			{
				s.LoadString(IDS_GUARD_TOUR);
				m_hGuardTourPage = m_pSVTree->InsertItem(s,IMAGE_GUARD_TOUR,IMAGE_GUARD_TOUR,m_hProcesses);
			}
			if (   (sVersion >= _T("4.6.1"))
				&& (   (GetDocument()->GetDongle()->RunJaCobUnit1())
					|| (GetDocument()->GetDongle()->RunSaVicUnit1())
					|| (GetDocument()->GetDongle()->RunTashaUnit1())
					|| (GetDocument()->GetDongle()->RunQUnit())
					)
				)
			{
				s.LoadString(IDS_VIDEO_OUT);
				m_hVideoOutPage = m_pSVTree->InsertItem(s,IMAGE_VIDEO_OUT,IMAGE_VIDEO_OUT,m_hProcesses);
			}
#ifndef _DEBUG
			if (sVersion >= _T("5.0.6"))
#endif
			{
				s.LoadString(IDS_PROZESS_UPLOAD);
				m_hUploadPage = m_pSVTree->InsertItem(s,IMAGE_UPLOAD,IMAGE_UPLOAD,m_hProcesses);
			}
		}
	}

	// ACTIVATION
	if (GetDocument()->GetFlags() & WK_ALLOW_LINK)
	{
		if (   GetDocument()->GetDongle()->AllowLocalProcesses()
			 || GetDocument()->GetDongle()->IsTransmitter()
			 || GetDocument()->GetDongle()->IsLocal())
		{
			s.LoadString(IDS_ACTIVATIONPAGE);
			m_hActivationPage = m_pSVTree->InsertItem(s,IMAGE_ACTIVATION,IMAGE_ACTIVATION);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::InitTree()
{
	m_bClearing = TRUE;
	m_pSVTree->DeleteAllItems();

	ClearHTREEITEMs();

	InitSystem();

	InitDatabase();

	InitSDI();

	if (!GetDocument()->GetDongle()->IsDemo())
	{
		InitInput();
		InitOutput();
		InitAudio();
		InitHost();
		InitSecurity();
		InitTimer();
		InitNotification();
		InitProcesses();
	}
	
	m_bServerInit = FALSE;
	m_bClearing = FALSE;
	// select the first item in the tree
	m_pSVTree->SelectItem(m_pSVTree->GetRootItem());

   if (theApp.m_bUpdateLanguageDependencies)
   {
		CSVDoc* pDoc = GetDocument();
		CInputList* pInputList = pDoc->GetInputs();
		pInputList->Save(GetDocument()->GetProfile(), FALSE);
/*
		CInputGroup* pInputGroup = pInputList->GetGroupBySMName(SM_LAUNCHER_INPUT);
		if (pInputGroup)
		{
			m_pSVPage = new CInputPage(this,pInputGroup);
			m_pSVPage->SaveChanges();
		}
		m_pSVPage->DestroyWindow();
		m_pSVPage = NULL;
*/
		if (GetDocument()->GetDongle()->RunSDIUnit())
		{
			m_pSVPage = new CDatabaseFieldPage(this);
			m_pSVPage->SendMessage(WM_COMMAND, IDC_BTN_DBF_DEFAULT_VALUES_ALL);
			m_pSVPage->SaveChanges();
			m_pSVPage->SetModified(FALSE, FALSE);
			m_pSVPage->DestroyWindow();
			m_pSVPage = NULL;
		}

		m_pSVPage = new CNotificationPage(this);
		m_pSVPage->SaveChanges();
		m_pSVPage->SetModified(FALSE, FALSE);
		m_pSVPage->DestroyWindow();
		m_pSVPage = NULL;

		m_pSVPage = new CHostPage(this);
		m_pSVPage->SaveChanges();
		m_pSVPage->SetModified(FALSE, FALSE);
		m_pSVPage->DestroyWindow();
		m_pSVPage = NULL;

		theApp.GetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT);
   }
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::Clear()
{
	m_bClearing = TRUE;
	if (m_pSVPage && m_pSVPage->m_hWnd)
	{
		m_pSVPage->DestroyWindow();
		m_pSVPage = NULL;
	}
	m_pSVTree->DeleteAllItems();
	ClearHTREEITEMs();
	m_bClearing = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	m_pSVTree->SetImageList(theApp.GetSmallImage(),LVSIL_NORMAL);
	
	InitTree();

}
/////////////////////////////////////////////////////////////////////////////
void CSVView::TreeSelchanged(HTREEITEM hNewSelItem)
{
	if (m_bClearing)
	{
		return;
	}

	if (hNewSelItem == m_SelectedItem)
	{
		// nothing todo if nothing changed
		return;
	}

	// first close old page if exist
	if (m_pSVPage)
	{
		BOOL bReturn = TRUE;
		if (m_pSVPage->IsModified())
		{
			if (m_pSVPage->IsDataValid()) 
			{
				int iReturn = AfxMessageBox(IDP_SAVE_CHANGES, MB_YESNOCANCEL);
				switch (iReturn)
				{
					case IDYES:
						m_pSVPage->OnOK();
						if ((m_DeletedItem!=NULL) && (hNewSelItem == m_DeletedItem))
						{
							m_DeletedItem = NULL;
							return;
						}
						bReturn = FALSE;
						break;
					case IDNO:
						m_pSVPage->OnCancel();
						if ((m_DeletedItem!=NULL) && (hNewSelItem == m_DeletedItem))
						{
							m_DeletedItem = NULL;
							return;
						}
						bReturn = FALSE;
						break;
					case IDCANCEL:
						bReturn = TRUE;
					default:
						break;
				}
			}
		}
		else
		{
			// not modified
			bReturn = FALSE;
		}
		if (bReturn)
		{
			m_pSVTree->SelectItem(m_SelectedItem);
			return;
		}
		else
		{
			m_pSVPage->DestroyWindow();
			m_pSVPage = NULL;
		}
	}

	m_SelectedItem = hNewSelItem;

	if (hNewSelItem == m_hHostPage)	{
		m_pSVPage = new CHostPage(this);
	}
	else if (hNewSelItem == m_hSecurityPage)	{
		m_pSVPage = new CSecurityPage(this);
	}
	else if (hNewSelItem == m_hPermissionPage)	{
		m_pSVPage = new CPermissionPage(this);
	}
	else if (hNewSelItem == m_hUserPage)	{
		m_pSVPage = new CUserPage(this);
	}
	else if ((m_pSVTree->GetParentItem(hNewSelItem) == m_hInputPage) && 
			 (m_hInputPage!=NULL))
	{
		CString sGroup = m_pSVTree->GetItemText(hNewSelItem);
		CSVDoc* pDoc = GetDocument();
		CInputList* pInputList = pDoc->GetInputs();
		CInputGroup* pInputGroup = pInputList->GetGroupByName(sGroup);
		if (pInputGroup)
		{
			m_pSVPage = new CInputPage(this,pInputGroup);
		}
	}
	else if (hNewSelItem == m_hSimPage)	{
		m_pSVPage = new CSimPage(this);
	}
	else if (hNewSelItem == m_hISDNPage1)	{
		m_pSVPage = new CISDNPage(this, ISDN1);
	}
	else if (hNewSelItem == m_hISDNPage2)	{
		m_pSVPage = new CISDNPage(this, ISDN2);
	}
	else if (hNewSelItem == m_hPTUnitPage1)	{
		m_pSVPage = new CPTUnitPage(this, PT1);
	}
	else if (hNewSelItem == m_hTOBSUnitPage) {
		m_pSVPage = new CTOBSPage(this);
	}
	else if (hNewSelItem == m_hPTUnitPage2)	{
		m_pSVPage = new CPTUnitPage(this, PT2);
	}
	else if (hNewSelItem == m_hSocketPage)	{
		m_pSVPage = new CSocketPage(this);
	}
	else if (hNewSelItem == m_hSMSPage)	{
		m_pSVPage = new CSMSPage(this);
	}
	else if (hNewSelItem == m_hEMailPage)	{
		m_pSVPage = new CEMailPage(this);
	}
	else if (hNewSelItem == m_hRS232Page)	{
		m_pSVPage = new CRS232Page(this);
	}
	else if (hNewSelItem == m_hGemosPage)	{
		m_pSVPage = new CGemosPage(this);
	}
	else if (hNewSelItem == m_hAutoLogoutPage)	{
		m_pSVPage = new CAppPage(this);
	}
	else if (hNewSelItem == m_hNTLoginPage)	{
		m_pSVPage = new CNTLoginPage(this);
	}
	else if (hNewSelItem == m_hSDIPage)	{
		m_pSVPage = new CSDIPage(this);
	}
	else if (hNewSelItem == m_hMESZPage)	{
		m_pSVPage = new CMESZPage(this);
	}
//	else if (hNewSelItem == m_hHardwarePage)	{
//		m_pSVPage = new CHardwarePage(this);
//	}
	else if (hNewSelItem == m_hQUnitPage)	{
		m_pSVPage = new CQUnitPage(this);
	}
	else if (hNewSelItem == m_hIPCameraPage)	{
		m_pSVPage = new CCameraParameterPage(this);
	}
	else if (hNewSelItem == m_hJaCobPage1){
		m_pSVPage = new CJaCobPage(this, JACOB1);
	}
	else if (hNewSelItem == m_hJaCobPage2)	{
		m_pSVPage = new CJaCobPage(this, JACOB2);
	}
	else if (hNewSelItem == m_hJaCobPage3)	{
		m_pSVPage = new CJaCobPage(this, JACOB3);
	}
	else if (hNewSelItem == m_hJaCobPage4)	{
		m_pSVPage = new CJaCobPage(this, JACOB4);
	}
	else if (hNewSelItem == m_hSaVicPage1){
		m_pSVPage = new CSaVicPage(this, SAVIC1);
	}
	else if (hNewSelItem == m_hSaVicPage2)	{
		m_pSVPage = new CSaVicPage(this, SAVIC2);
	}
	else if (hNewSelItem == m_hSaVicPage3)	{
		m_pSVPage = new CSaVicPage(this, SAVIC3);
	}
	else if (hNewSelItem == m_hSaVicPage4)	{
		m_pSVPage = new CSaVicPage(this, SAVIC4);
	}
	else if (hNewSelItem == m_hTashaPage1){
		m_pSVPage = new CTashaPage(this, TASHA1);
	}
	else if (hNewSelItem == m_hTashaPage2){
		m_pSVPage = new CTashaPage(this, TASHA2);
	}
	else if (hNewSelItem == m_hTashaPage3){
		m_pSVPage = new CTashaPage(this, TASHA3);
	}
	else if (hNewSelItem == m_hTashaPage4){
		m_pSVPage = new CTashaPage(this, TASHA4);
	}
	else if (hNewSelItem == m_hICPCONPage) {
		m_pSVPage = new CICPCONConfigPage(this);
	}
	else if (hNewSelItem == m_hYUTAPage) {
		m_pSVPage = new CYutaPage(this);
	}
	else if (hNewSelItem == m_hJoystickPage) {
		m_pSVPage = new CJoystickPage(this);
	}
	else if (hNewSelItem == m_hQuadPage) {
		m_pSVPage = new CQuadPage(this);
	}
	else if (hNewSelItem == m_hSTMPage) {
		m_pSVPage = new CSTMPage(this);
	}
	else if (hNewSelItem == m_hSaveProcessPage)	{
		m_pSVPage = new CSavePage(this);
	}
	else if (hNewSelItem == m_hUploadPage)	{
		m_pSVPage = new CUploadPage(this);
	}
	else if (hNewSelItem == m_hCallProcessPage)	{
		m_pSVPage = new CCallPage(this);
	}
	else if (hNewSelItem == m_hBackupProcessPage)	{
		m_pSVPage = new CBackupProcessPage(this);
	}
	else if (hNewSelItem == m_hActualImagePage)	{
		m_pSVPage = new CActualImagePage(this);
	}
	else if (hNewSelItem == m_hImageCallPage)	{
		m_pSVPage = new CImageCallPage(this);
	}
	else if (hNewSelItem == m_hCheckCallPage)	{
		m_pSVPage = new CCheckCallPage(this);
	}
	else if (hNewSelItem == m_hGuardTourPage)	{
		m_pSVPage = new CGuardTourPage(this);
	}
	else if (hNewSelItem == m_hNotificationPage) {
		m_pSVPage = new CNotificationPage(this);
	}
	else if (hNewSelItem == m_hDrives) {
		m_pSVPage = new CDrivePage(this);
	}
	else if (hNewSelItem == m_hArchivs) {
		m_pSVPage = new CArchivPage(this);
	}
	else if (hNewSelItem == m_hDatabaseFields) {
		m_pSVPage = new CDatabaseFieldPage(this);
	}
	else if (hNewSelItem == m_hMultiMonitorPage) {
		m_pSVPage = new CMultiMonitorPage(this);
	}
	else if (hNewSelItem == m_hLanguagePage) {
		m_pSVPage = new CLanguagePage(this);
	}
	else if (hNewSelItem == m_hMaintenancePage) {
		m_pSVPage = new CMaintenancePage(this);
	}
	else if (hNewSelItem == m_hMiscellaneousPage) {
		m_pSVPage = new CMiscellaneousPage(this);
	}
	else if (   (m_pSVTree->GetParentItem(hNewSelItem) == m_hCameraPage) 
			  && (m_hCameraPage!=NULL))
	{
		CString sGroup = m_pSVTree->GetItemText(hNewSelItem);
		CSVDoc* pDoc = GetDocument();
		COutputList* pOutputList = pDoc->GetOutputs();
		COutputGroup* pOutputGroup = pOutputList->GetGroupByName(sGroup);
		if (pOutputGroup)
		{
			if (pOutputGroup->HasCameras())
			{
				m_pSVPage = new COutputPage(this,pOutputGroup);
			}
		}
	}
	else if (   (m_pSVTree->GetParentItem(hNewSelItem) == m_hRelaisPage) 
			  && (m_hRelaisPage!=NULL))
	{
		CString sGroup = m_pSVTree->GetItemText(hNewSelItem);
		CSVDoc* pDoc = GetDocument();
		COutputList* pOutputList = pDoc->GetOutputs();
		COutputGroup* pOutputGroup = pOutputList->GetGroupByName(sGroup);
		if (pOutputGroup)
		{
			if (pOutputGroup->HasRelais())
			{
				m_pSVPage = new CRelayPage(this,pOutputGroup);
			}
		}
	}
	else if (hNewSelItem == m_hAudioPageIn)
	{
		m_pSVPage = new CAudioChannelPage(this, true);
	}
	else if (hNewSelItem == m_hAudioPageOut)
	{
		m_pSVPage = new CAudioChannelPage(this, false);
	}
	else if (hNewSelItem == m_hTimerPage)
	{
		m_pSVPage = new CTimerEditorPage(this);
	}
	else if (hNewSelItem == m_hVideoOutPage)
	{
		m_pSVPage = new CVideoOutPage(this);
	}
	else if (hNewSelItem == m_hActivationPage)
	{
		m_pSVPage = new CActivationPage(this);
	}
	else
	{
		for (int i=0; i<SM_NO_OF_AUDIO_UNITS; i++)
		{
			if (hNewSelItem == m_hAudioPage[i])	
			{
				m_pSVPage = new CAudioPage(this, i+1);
			}
		}

		if (m_pSVPage == NULL)
		{
			HTREEITEM hToSelect = NULL;
			CString str = m_pSVTree->GetItemText(hNewSelItem);
			if (str.IsEmpty())
			{
				if (m_pSVTree->ItemHasChildren(hNewSelItem))
				{
					m_pSVTree->Expand(hNewSelItem,TVE_EXPAND);
				}
				hToSelect = m_pSVTree->GetNextItem(hNewSelItem,TVGN_NEXTVISIBLE);
				if (hToSelect==NULL)
				{
					hToSelect = m_pSVTree->GetRootItem();
				}
				// this call causes a recursion of TreSelChanged
				m_pSVTree->SelectItem(hToSelect);
				// so return here to prevent from
				// executing further code
				return;
			}
			else
			{
				m_pSVPage = new CDefaultTreeKnode(this);
				CDefaultTreeKnode *pDefTN = (CDefaultTreeKnode*)m_pSVPage;
				pDefTN->m_strGroup = str;
				pDefTN->m_hSelected = hNewSelItem;
				int nData = m_pSVTree->GetItemData(hNewSelItem);
				if (nData) pDefTN->m_strDscr.LoadString(nData);
				else
				{
					CString strVersion;
					strVersion.LoadString(IDS_VERSION);
					pDefTN->m_strGroup += _T(", ") + strVersion;

					pDefTN->m_strDscr  = COemGuiApi::CalcProductName() + _T("\n\n") + strVersion + _T(": ");
					pDefTN->m_strDscr += GetDocument()->GetDongle()->GetVersionString();
				}

				if (m_pSVTree->ItemHasChildren(hNewSelItem))
				{
					int i=0;
					hToSelect = m_pSVTree->GetNextItem(hNewSelItem, TVGN_CHILD);
					while (hToSelect)
					{
						pDefTN->m_strItem[i++] = m_pSVTree->GetItemText(hToSelect);
						hToSelect = m_pSVTree->GetNextItem(hToSelect, TVGN_NEXT);
						if (i == MAX_CHILDITEMS) break;
					}
					if (i < MAX_CHILDITEMS && m_pSVTree->GetParentItem(hNewSelItem))
					{
						pDefTN->m_strItem[i] = TREEITEM_UP;
					}
				}
				if (GetDocument()->IsLocal())
				{
					if (m_SelectedItem == m_hSystemPage)
					{
						CWK_Profile wkpSystem(CWK_Profile::WKP_ABSOLUT, HKEY_LOCAL_MACHINE,_T(""),_T(""));
						_TCHAR szBuffer[MAX_PATH];
						CString str;
						DWORD dwSize = MAX_PATH;
						pDefTN->m_strDscr += _T("\n\n");
						// operating system
						str.LoadString(IDS_SYSTEM);
						pDefTN->m_strDscr += str + _T(":\n");
						str.Format(_T("  %s\n\n"), COemGuiApi::GetOSVersion());
						pDefTN->m_strDscr += str;

						// registration information
						str.LoadString(IDS_REGISTERED);
						pDefTN->m_strDscr += str + _T("\n");
						str = wkpSystem.GetString(REGPATH_HKLM_NT_CURRENTVERSION, REGKEY_REGISTEREDOWNER, NULL);
						if (!str.IsEmpty())
						{
							pDefTN->m_strDscr += _T("  ") + str + _T("\n");
						}
						str = wkpSystem.GetString(REGPATH_HKLM_NT_CURRENTVERSION, REGKEY_REGISTEREDORGANIZATION, NULL);
						if (!str.IsEmpty())
						{
							pDefTN->m_strDscr += _T("  ") + str + _T("\n");
						}
						str = wkpSystem.GetString(REGPATH_HKLM_NT_CURRENTVERSION, REGKEY_PRODUCTID, NULL);
						if (!str.IsEmpty())
						{
							pDefTN->m_strDscr += _T("  ") + str + _T("\n");
						}

						// computername and domain
						str.LoadString(IDS_NAME);
						pDefTN->m_strDscr += _T("\n") + str + _T(":\n");
						if (theApp.GetComputerNameEx(ComputerNameNetBIOS, szBuffer, &dwSize))
						{
							str.Format(_T("  %s"), szBuffer);
							pDefTN->m_strDscr += str;
						}
						dwSize = MAX_PATH;
						if (theApp.GetComputerNameEx(ComputerNameDnsDomain, szBuffer, &dwSize) && dwSize>1)
						{
							pDefTN->m_strDscr += _T(" @ ");
							pDefTN->m_strDscr += szBuffer;
						}
					}
					if (m_SelectedItem == m_hHardwarePage)
					{
						CString str;
						pDefTN->m_strDscr += _T("\n\n");
						str.LoadString(IDS_HARDWARE);
						pDefTN->m_strDscr += str + _T(":\n");
						CWK_Profile wkpSystem(CWK_Profile::WKP_ABSOLUT, HKEY_LOCAL_MACHINE,_T(""),_T(""));
						str = wkpSystem.GetString(HKLM_HW_PROCESSOR, _T("ProcessorNameString"), NULL);
						str.TrimLeft();
						pDefTN->m_strDscr += _T("  ") + str + _T("\n");
						DWORD dwSpeed = wkpSystem.GetInt(HKLM_HW_PROCESSOR, _T("~MHz"), 0);
						if (dwSpeed > 1000)
						{
							str.Format(_T("  %.2f GHz\n"), 0.001*dwSpeed);
						}
						else
						{
							str.Format(_T("  %d MHz\n"), dwSpeed);
						}
						pDefTN->m_strDscr += str;
						CMemoryStatus ms;
						GlobalMemoryStatus(&ms);
						str.Format(_T("  %d MB RAM\n"), (ms.dwTotalPhys >> 20) + 1);
						pDefTN->m_strDscr += str;
					}
				}				
				pDefTN->Create();
			}
		}
	}

	if (m_pSVPage && m_pSVPage->m_hWnd && IsWindow(m_pSVPage->m_hWnd))
	{
		m_pSVPage->GetClientRect(m_PageRect);
	}

	Resize();
	
	if (m_pSVPage)
	{
		m_pSVPage->ShowWindow(SW_SHOW);
		m_pSVPage->SetFocus();
		m_pSVPage->ActivateTooltips(m_bShowToolTips);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnInputChanged()
{
	if (m_hInputPage)
	{
		// INPUT is allowed
		CSVDoc* pDoc = GetDocument();
		HTREEITEM hInputChild,hInputDelete, hItem;
		CString   sName;
		CInputGroup* pInputGroup;
		CInputList* pInputList = pDoc->GetInputs();
		
		// first remove all invalid items
		hInputChild = m_pSVTree->GetChildItem(m_hInputPage);
		while (hInputChild)
		{
			sName = m_pSVTree->GetItemText(hInputChild);
			pInputGroup = pInputList->GetGroupByName(sName);
			hInputDelete = hInputChild;
			hInputChild  = m_pSVTree->GetNextSiblingItem(hInputChild);
			if (pInputGroup==NULL)
			{
				m_DeletedItem = hInputDelete;
				m_pSVTree->DeleteItem(hInputDelete);
				hInputDelete = NULL;
			}
		}

		// second add new ones
		for (int i=0;i<pInputList->GetSize();i++)
		{
			pInputGroup = pInputList->GetGroupAt(i);
			if (pInputGroup)
			{
				// browse if it's already in tree
				BOOL bFound = FALSE;
				hInputChild = m_pSVTree->GetChildItem(m_hInputPage);
				while (hInputChild)
				{
					sName = m_pSVTree->GetItemText(hInputChild);
					if (sName == pInputGroup->GetName())
					{
						bFound = TRUE;
						break;
					}
					hInputChild  = m_pSVTree->GetNextSiblingItem(hInputChild);
				}
				if (!bFound)
				{
					// insert it
					hItem = m_pSVTree->InsertItem(pInputGroup->GetName(),
										IMAGE_INPUT,IMAGE_INPUT,m_hInputPage);
					m_pSVTree->SetItemData(hItem, pInputGroup->GetID().GetID());
				}
			}
		}

	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnCameraChanged()
{
	if (m_hCameraPage)
	{
		// Output is allowed
		int i;
		CSVDoc* pDoc = GetDocument();
		HTREEITEM hOutputChild,hOutputDelete, hItem;
		CString   sName;
		COutputGroup* pOutputGroup;
		COutputList* pOutputList = pDoc->GetOutputs();
		
		// camera
		// first remove all invalid items
		hOutputChild = m_pSVTree->GetChildItem(m_hCameraPage);
		while (hOutputChild)
		{
			sName = m_pSVTree->GetItemText(hOutputChild);
			pOutputGroup = pOutputList->GetGroupByName(sName);

			hOutputDelete = hOutputChild;
			hOutputChild  = m_pSVTree->GetNextSiblingItem(hOutputChild);
			if (pOutputGroup==NULL)
			{
				m_DeletedItem = hOutputDelete;
				m_pSVTree->DeleteItem(hOutputDelete);
				hOutputDelete = NULL;
			}
		}

		// second add new ones
		for (i=0;i<pOutputList->GetSize();i++)
		{
			pOutputGroup = pOutputList->GetGroupAt(i);
			if (pOutputGroup && pOutputGroup->HasCameras())
			{
				// browse if it's already in tree
				BOOL bFound = FALSE;
				hOutputChild = m_pSVTree->GetChildItem(m_hCameraPage);
				while (hOutputChild)
				{
					sName = m_pSVTree->GetItemText(hOutputChild);
					if (sName == pOutputGroup->GetName())
					{
						bFound = TRUE;
						break;
					}
					hOutputChild  = m_pSVTree->GetNextSiblingItem(hOutputChild);
				}
				if (!bFound)
				{
					// insert it
					hItem = m_pSVTree->InsertItem(pOutputGroup->GetName(),
										IMAGE_CAMERA,IMAGE_CAMERA,m_hCameraPage);
					m_pSVTree->SetItemData(hItem, pOutputGroup->GetID().GetID());
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnRelaisChanged()
{
	if (m_hRelaisPage)
	{
		// Output is allowed
		int i;
		CSVDoc* pDoc = GetDocument();
		HTREEITEM hOutputChild,hOutputDelete;
		CString   sName;
		COutputGroup* pOutputGroup;
		COutputList* pOutputList = pDoc->GetOutputs();

		// relais
		hOutputChild = m_pSVTree->GetChildItem(m_hRelaisPage);
		while (hOutputChild)
		{
			sName = m_pSVTree->GetItemText(hOutputChild);
			pOutputGroup = pOutputList->GetGroupByName(sName);

			hOutputDelete = hOutputChild;
			hOutputChild  = m_pSVTree->GetNextSiblingItem(hOutputChild);
			if (pOutputGroup==NULL)
			{
				m_DeletedItem = hOutputDelete;
				m_pSVTree->DeleteItem(hOutputDelete);
				hOutputDelete = NULL;
			}
		}

		// second add new ones
		for (i=0;i<pOutputList->GetSize();i++)
		{
			pOutputGroup = pOutputList->GetGroupAt(i);
			if (pOutputGroup && pOutputGroup->HasRelais())
			{
				// browse if it's already in tree
				BOOL bFound = FALSE;
				hOutputChild = m_pSVTree->GetChildItem(m_hRelaisPage);
				while (hOutputChild)
				{
					sName = m_pSVTree->GetItemText(hOutputChild);
					if (sName == pOutputGroup->GetName())
					{
						bFound = TRUE;
						break;
					}
					hOutputChild  = m_pSVTree->GetNextSiblingItem(hOutputChild);
				}
				if (!bFound)
				{
					// insert it
					m_pSVTree->InsertItem(pOutputGroup->GetName(),
										IMAGE_RELAY,IMAGE_RELAY,m_hRelaisPage);
				}
			}
		}

	}
}
//////////////////////////////////////////////////////////////////////////
void CSVView::OnHardwareChanged()
{
	if (m_hHardwarePage)
	{
		CWK_Profile &wkp = GetDocument()->GetProfile();
		CString s;
		int i;

		if (CAudioPage::IsUnitActive(wkp, -1))
		{
			for (i=1; i<SM_NO_OF_AUDIO_UNITS; i++)
			{
				if (m_hAudioPage[i])
				{
					if (   !CAudioPage::IsUnitActive(wkp, i) 
						|| !CAudioPage::CanActivateUnit(wkp, i+1))
					{
						m_pSVTree->DeleteItem(m_hAudioPage[i]);
						m_hAudioPage[i] = NULL;
					}
				}
				else
				{
					if (   CAudioPage::IsUnitActive(wkp, i) 
						&& CAudioPage::CanActivateUnit(wkp, i+1))
					{
						s.Format(IDS_AUDIO, i+1);
						ASSERT(m_hAudioPage[i] == NULL);
						m_hAudioPage[i] = m_pSVTree->InsertItem(s, IMAGE_AUDIO, IMAGE_AUDIO, m_hHardwarePage, m_hAudioPage[i-1]);
					}
				}
			}
		}
	}
	InitAudio();
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	Resize();
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::Resize()
{
	CRect pr;

	if (m_bResizing)
	{
		// never call this twice
		// prevent us from recursive calls
		return;
	}

	m_bResizing = TRUE;
	GetClientRect(pr);

	// Wizard
	if (m_pSVWizard && m_pSVWizard->m_hWnd)
	{
		pr.top       = pr.bottom - m_lWizardHeight;
		pr.right     = m_lTreeWidth;
		m_pSVWizard->MoveWindow(&pr);
		pr.bottom    = pr.top;
		pr.top       = 0;
		if (!::IsWindowVisible(m_pSVWizard->m_hWnd))
		{
			m_pSVWizard->ShowWindow(SW_SHOW);
		}
	}

	// TREE
	if (m_pSVTree && m_pSVTree->m_hWnd)
	{
//		pr.right = SV_TREE_WIDTH;
		pr.right = m_lTreeWidth;
		m_pSVTree->MoveWindow(pr);
	}

	// PAGE
	if (m_pSVPage && m_pSVPage->m_hWnd)
	{
		int xdiff,ydiff;

		GetClientRect(pr);
//		pr.left = SV_TREE_WIDTH+1;
		pr.left = m_lTreeWidth+1;
		xdiff = m_PageRect.Width() - pr.Width();
		ydiff = m_PageRect.Height() - pr.Height();
		if ((xdiff>0) || (ydiff>0))
		{
			CRect mr,vr,sr,tr;
			CMainFrame* pMF = (CMainFrame*)AfxGetMainWnd();

			if (xdiff<0)
			{
				xdiff = 0;
			}
			if (ydiff<0)
			{
				ydiff = 0;
			}
			
			// get rects
			pMF->GetWindowRect(mr);
			pMF->GetStatusBar()->GetWindowRect(sr);
			pMF->GetToolBar()->GetWindowRect(tr);
			GetWindowRect(vr);
			pMF->ScreenToClient(vr);
			pMF->ScreenToClient(tr);
			pMF->ScreenToClient(sr);

			// calc new rects
			mr.right += xdiff;
			pr.right += xdiff;
			vr.right += xdiff;
			tr.right += xdiff;
			sr.right += xdiff;
			mr.bottom += ydiff;
			pr.bottom += ydiff;
			vr.bottom += ydiff;
			tr.bottom += ydiff;
			sr.bottom += ydiff;
			
			// set rects
			m_pSVPage->MoveWindow(pr);
			MoveWindow(vr);
			pMF->GetStatusBar()->MoveWindow(sr);
			pMF->GetToolBar()->MoveWindow(tr);
			pMF->MoveWindow(mr);
			pMF->UpdateWindow();

			if (ydiff>0)
			{
				if (m_pSVTree && m_pSVTree->m_hWnd)
				{
					GetClientRect(pr);
//					pr.right = SV_TREE_WIDTH;
					pr.right = m_lTreeWidth;
					m_pSVTree->MoveWindow(pr);
				}
			}
		}
		else
		{
			m_pSVPage->MoveWindow(pr);
		}
	}
	m_bResizing = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnFileSave() 
{
	if (m_pSVPage)
	{
		m_pSVPage->OnOK();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	if (m_pSVWizard && m_pSVWizard->m_hWnd)
	{
		pCmdUI->Enable(m_pSVWizard->AllowSave());
	}
	else if (m_pSVPage && m_pSVPage->m_hWnd)
	{
		pCmdUI->Enable(m_pSVPage->IsModified());
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnEditCancel() 
{
   if (m_pSVPage && m_pSVPage->m_hWnd)
	{
		m_pSVPage->OnCancel();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnUpdateEditCancel(CCmdUI* pCmdUI) 
{
	if (m_pSVWizard && m_pSVWizard->m_hWnd)
	{
		pCmdUI->Enable(m_pSVWizard->AllowCancel());
	}
	else if (m_pSVPage && m_pSVPage->m_hWnd)
	{
		pCmdUI->Enable(m_pSVPage->IsModified());
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnEditNew() 
{
	if (m_pSVPage && m_pSVPage->m_hWnd)
	{
		if (m_pSVPage->CanNew())
		{
			m_pSVPage->OnNew();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnUpdateEditNew(CCmdUI* pCmdUI) 
{
	if (m_pSVWizard && m_pSVWizard->m_hWnd)
	{
		pCmdUI->Enable(m_pSVWizard->AllowNew());
	}
	else if (m_pSVPage && m_pSVPage->m_hWnd)
	{
		pCmdUI->Enable(m_pSVPage->CanNew());
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnEditDelete() 
{
   if (m_pSVPage && m_pSVPage->m_hWnd)
	{
		if (m_pSVPage->CanDelete())
		{
			m_pSVPage->OnDelete();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnUpdateEditDelete(CCmdUI* pCmdUI) 
{
   if (m_pSVWizard && m_pSVWizard->m_hWnd)
   {
		pCmdUI->Enable(m_pSVWizard->AllowDelete());
   }
   else if (m_pSVPage && m_pSVPage->m_hWnd)
	{
		pCmdUI->Enable(m_pSVPage->CanDelete());
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVView::CanCloseFrame(BOOL bInit /*= TRUE*/)
{
	if (m_pSVPage && m_pSVPage->m_hWnd)
	{
		if (m_pSVPage->IsModified())
		{
			int r = AfxMessageBox(IDP_SAVE_CHANGES, MB_YESNOCANCEL);
			switch (r)
			{
				case IDYES:
					m_pSVPage->OnOK();
					break;
				case IDNO:
					m_pSVPage->OnCancel();
					m_bServerInit = FALSE;
					break;
				case IDCANCEL:
				default:
					return FALSE;
					break;
			}
		}
		if (m_pSVPage->IsModified())
		{
			return FALSE;
		}
		if (bInit)
		{
			if (WK_IS_RUNNING(WK_APP_NAME_SERVER) && m_bServerInit)
			{
				GetDocument()->InitServer();
			}
			GetDocument()->InitApps();
		}
		m_bInitApps = FALSE;
	}
	return TRUE;
}
///////////////////////////////////////////////////////////////
void CSVView::OnEditServerinit() 
{
	CanCloseFrame();
}
///////////////////////////////////////////////////////////////
void CSVView::OnUpdateEditServerinit(CCmdUI* pCmdUI) 
{
	BOOL bEnable =    (m_bServerInit || m_bInitApps) 
				   && m_pSVPage!=NULL
				   && GetDocument()->IsLocal()
				   && (WK_IS_RUNNING(WK_APP_NAME_SERVER) || WK_IS_RUNNING(WK_APP_NAME_DB_SERVER)) ? TRUE : FALSE;

	if (bEnable && m_pSVWizard && m_pSVWizard->m_hWnd)
	{
		bEnable = m_pSVWizard->AllowInit();
	}

	pCmdUI->Enable(bEnable);
}
///////////////////////////////////////////////////////////////
void CSVView::AddInitApp(const CString& sAppname)
{
	m_bInitApps = TRUE;
	GetDocument()->AddInitApp(sAppname);
}
///////////////////////////////////////////////////////////////
void CSVView::OnViewTooltips() 
{
	m_bShowToolTips = !m_bShowToolTips;
	if (m_pSVPage && m_pSVPage->m_hWnd)
	{
		m_pSVPage->ActivateTooltips(m_bShowToolTips);
	}
}
///////////////////////////////////////////////////////////////
void CSVView::OnUpdateViewTooltips(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bShowToolTips);
}
///////////////////////////////////////////////////////////////
BOOL CSVView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
//	TRACE(_T("code:%d\n"), ((NMHDR*)lParam)->code);
	return CView::OnNotify(wParam, lParam, pResult);
}
///////////////////////////////////////////////////////////////
LRESULT CSVView::OnSelectPage(WPARAM wParam, LPARAM lParam)
{
	HTREEITEM hItem = NULL;
	if (lParam & SELECT_FROM_SEC_ID)
	{
		hItem = FindItem(TVI_ROOT, wParam, NULL);
		if (lParam & SELECT_TEST_ONLY) return (hItem != 0) ? 1 : 0;
		else if (hItem) m_pSVTree->SelectItem(hItem);
	}
	else
	{
		CString str;
		if (lParam & SELECT_FROM_STRING_ID)
		{
			str.LoadString(wParam);
		}
		else if (lParam & SELECT_FROM_STRING)
		{
			str = (_TCHAR*)wParam;
			if (str == TREEITEM_UP)
			{
				m_pSVTree->SelectItem(m_pSVTree->GetParentItem(m_pSVTree->GetSelectedItem()));
				return 0;
			}
		}
		if (!str.IsEmpty())
		{
			hItem = FindItem(TVI_ROOT, 0, str);
			if (lParam & SELECT_TEST_ONLY) return (hItem != 0) ? 1 : 0;
 			else if (hItem) m_pSVTree->SelectItem(hItem);
		}
   }
   return (hItem != 0) ? 1 : 0;
}
///////////////////////////////////////////////////////////////
LRESULT CSVView::OnSetWizardSize(WPARAM wParam, LPARAM lParam)
{
   if (wParam)
   {
		CRect *pr = (CRect*)wParam;

		m_lTreeWidth    = pr->right;
		m_lWizardHeight = pr->bottom;
   }
   else
   {
		m_lTreeWidth    = GetSystemMetrics(SM_CXFULLSCREEN) / 4;
		m_lWizardHeight = 0;
		Resize();
   }
   return 0;
}
///////////////////////////////////////////////////////////////
HTREEITEM CSVView::FindItem(HTREEITEM hItem, DWORD dwID, LPCTSTR pstr)
{
   CString str;
   if (hItem != TVI_ROOT)
   {
		if ((dwID != 0) && (dwID == m_pSVTree->GetItemData(hItem)))
		{
			return hItem;
		}
		if (pstr != NULL)
		{
			str = m_pSVTree->GetItemText(hItem);
			if (str == pstr)
			{
				return hItem;
			}
		}
   }

   HTREEITEM hChild = m_pSVTree->GetNextItem(hItem, TVGN_CHILD);
   while (hChild)
   {
		hItem = FindItem(hChild, dwID, pstr);
		if (hItem) return hItem;
		hChild = m_pSVTree->GetNextItem(hChild, TVGN_NEXT);
   }

   return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnUpdateWizFirst(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable((m_pSVWizard && m_pSVWizard->m_hWnd) ? false : true);
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnWizFirst() 
{
	ASSERT(m_pSVWizard==NULL);
	m_pSVWizard = new CSVPropertySheet(IDS_WIZARD_CAPTION, this);

	if (m_pSVWizard->m_hWnd == NULL)
	{
		m_pSVWizard->InsertPage(new CPropPageVariable)->Init(IDS_ARCHIVES, IDS_ARCHIVES);
		m_pSVWizard->InsertPage(new CPropPageVariable)->Init(IDS_TIMERPAGE, IDS_TIMERPAGE, ALLOW_NEW_DATA|ALLOW_DELETE_DATA);
		m_pSVWizard->InsertPage(new CPropPageVariable)->Init(IDS_SAVEPROCESSPAGE,IDS_SAVEPROCESSPAGE);
		m_pSVWizard->InsertPage(new CPropPageVariable)->Init(IDS_ACTIVATIONPAGE,IDS_ACTIVATIONPAGE);

		m_pSVWizard->Create(this);

		Resize();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnUpdateWizRoomSurveillanceMd(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((m_pSVWizard && m_pSVWizard->m_hWnd) && GetDocument()->IsLocal() ? FALSE : TRUE);
}
/////////////////////////////////////////////////////////////////////////////
#define INDEX_OFFSET_SAVIC	4
#define INDEX_OFFSET_TASHA	8
#define INDEX_Q_CAM			12
#define INDEX_IP_CAM		13

void CSVView::OnWizRoomSurveillance(UINT nID) 
{
	CSVDoc*       pDoc         = GetDocument();
	ASSERT(m_pSVWizard==NULL);
	m_pSVWizard = new CSVPropertySheet(IDS_WIZARD_CAPTION, this);

	if ((pDoc->GetFlags() & WK_ALLOW_INPUT) && (m_pSVWizard->m_hWnd == NULL))
	{
		int i, nStartID = IDS_JACOB1, nCards = 4;

		CDWordArray dwaStardIDs;
		CString str;

		WK_DELETE(m_pRoomSurveillanceMd);
		sRoomSurveillance*pRS = new sRoomSurveillance;
		m_pRoomSurveillanceMd = pRS;
		switch (nID)
		{
			case ID_WIZ_PERMANENT_RECORDING:	pRS->m_bMD = FALSE; break;
			case ID_WIZ_UVVK_RECORDING:			pRS->m_bUVVK = TRUE; break;
		}

		CPropPageVariable *pPPV;

		for (i=0; i<nCards; i++)
		{
			if (nStartID == IDS_TASHA1)
			{
				CString sLoad;
				sLoad.LoadString(nStartID);
				str.Format(_T("%s %d"), sLoad, i+1);
			}
			else
			{
				str.Empty();
			}
			
			if (  ( str.IsEmpty() && OnSelectPage(nStartID+i, SELECT_FROM_STRING_ID|SELECT_TEST_ONLY))
				||(!str.IsEmpty() && OnSelectPage((LPARAM)LPCTSTR(str), SELECT_FROM_STRING|SELECT_TEST_ONLY)))
			{
				// 1.Wizardseite aktiviert die Hardware
				pPPV = m_pSVWizard->InsertPage(new CPropPageVariable);
				pPPV->Init(nStartID+i, IDS_WIZ_JACOB_ACTIVATE_JACOB,
					ENABLE_PAGE, (LPARAM)pRS, NULL,
					WizardHWBActivateSetActive,
					WizardHWBActivateNext);
				if (!str.IsEmpty())
				{
					pPPV->SetPageTextID(str);
				}
				dwaStardIDs.Add(nStartID+i);
				switch (nStartID)
				{
					case IDS_JACOB1: case IDS_SAVIC1:
					// case IDS_IP_CAMERA_UNIT: ?
					// 2.Wizardseite aktiviert die MD
					pPPV = m_pSVWizard->InsertPage(new CPropPageVariable);
					pPPV->Init(nStartID+i, IDS_WIZ_JACOB_ACTIVATE_MD, 
						ENABLE_PAGE, (LPARAM)pRS, NULL,
						WizardHWBActivateMDSetActive,
						WizardHWBActivateMDNext); 
					if (!str.IsEmpty())
					{
						pPPV->SetPageTextID(str);
					}
					break;
				}
			}
			if (i == nCards-1)
			{
				i=-1;
				if (nStartID == IDS_JACOB1)
				{
					nStartID = IDS_SAVIC1;
					nCards = 4;
				}
				else if (nStartID == IDS_SAVIC1)
				{
					nStartID = IDS_TASHA1;
					nCards = 4;
				}
				else if (nStartID == IDS_TASHA1)
				{
					nStartID = IDS_Q_UNIT;
					nCards = 1;
				}
				else if (nStartID == IDS_Q_UNIT)
				{
					nStartID = IDS_IP_CAMERA_UNIT;
					nCards = 1;
					break;	 // TODO! RKE: remove break to enable IP Camera wizard
				}
				else
				{
					break;
				}
			}
		}

		CString sOutput, sInput;
		int nNumber;
		
		for (i=0; i<dwaStardIDs.GetCount(); i++)
		{
			nStartID = dwaStardIDs.GetAt(i);
			nNumber = 0;
			switch(nStartID)
			{
				case IDS_JACOB4: nNumber++;
				case IDS_JACOB3: nNumber++;
				case IDS_JACOB2: nNumber++;
				case IDS_JACOB1:
					sOutput = SM_MICO_OUTPUT_CAMERAS;
					sInput  = SM_MICO_MD_INPUT;
					break;
				case IDS_SAVIC4: nNumber++;
				case IDS_SAVIC3: nNumber++;
				case IDS_SAVIC2: nNumber++;
				case IDS_SAVIC1:
					sOutput = SM_SAVIC_OUTPUT_CAMERAS;
					sInput  = SM_SAVIC_MD_INPUT;
					break;
				case IDS_TASHA1+3: nNumber++;
				case IDS_TASHA1+2: nNumber++;
				case IDS_TASHA1+1: nNumber++;
				case IDS_TASHA1:
					sOutput = SM_TASHA_OUTPUT_CAMERAS;
					sInput  = SM_TASHA_MD_INPUT;
					break;
				case IDS_Q_UNIT:
					sOutput = SM_Q_OUTPUT_CAMERA;
					sInput  = SM_Q_MD_INPUT;
					break;
				case IDS_IP_CAMERA_UNIT:
					sOutput = SM_IP_CAMERA_OUTPUT_CAMERA;
					sInput  = SM_IP_CAMERA_MD_INPUT;
					break;
			}
			if (nNumber>0)
			{
				sOutput += (_TCHAR)('1' + nNumber);
				sInput  += (_TCHAR)('1' + nNumber);
			}
			pRS->m_OutputSM.Add(sOutput);
			pRS->m_InputSM.Add(sInput);
			// 3.Wizardseite aktiviert die Seite für Kameras
			pPPV = m_pSVWizard->InsertPage(new CPropPageVariable);
			pPPV->Init(i, IDS_WIZ_ACTIVATE_CAMERAS,
				ENABLE_PAGE|PAGEID_IS_SEC_ID, (LPARAM)pRS, 
				WizardActivateCamerasInit,
				WizardActivateCamerasSetActive,
				WizardActivateCamerasNext);
			
			// 4.Wizardseite aktiviert die Seite für MD Melder
			pPPV = m_pSVWizard->InsertPage(new CPropPageVariable);
			pPPV->Init(i, IDS_WIZ_ACTIVATE_MD_INPUT, 
				ENABLE_PAGE|PAGEID_IS_SEC_ID, (LPARAM)pRS,
				WizardActivateMDInputInit,
				WizardActivateMDInputSetActive);
		}		

		// 5.Wizardseite aktiviert die Laufwerke
		if (OnSelectPage(IDS_DRIVES, SELECT_FROM_STRING_ID|SELECT_TEST_ONLY))
		{
			pPPV = m_pSVWizard->InsertPage(new CPropPageVariable);
			pPPV->Init(IDS_DRIVES, IDS_WIZ_ACTIVATE_DRIVES, ENABLE_PAGE|ALLOW_NEW_DATA|ALLOW_DELETE_DATA,
				NULL, NULL, 
				WizardInitDrivesSetActive
//				, WizardActivateDrivesNext	// evtl prüfen ob es ein Speicherlaufwerk gibt
				);
		}

		// 6.Wizardseite initialisiert die Archive
		if (OnSelectPage(IDS_ARCHIVES, SELECT_FROM_STRING_ID|SELECT_TEST_ONLY))
		{
			pPPV = m_pSVWizard->InsertPage(new CPropPageVariable);
			pPPV->Init(IDS_ARCHIVES, IDS_WIZ_INIT_MD_ARCHIVES, ENABLE_PAGE|ALLOW_NEW_DATA|ALLOW_DELETE_DATA, (LPARAM)pRS, NULL
				, WizardInitMDArchivesSetActive);
		}
		// 7.Wizardseite aktiviert die Seite für Prozesse
		if (OnSelectPage(IDS_SAVEPROCESSPAGE, SELECT_FROM_STRING_ID|SELECT_TEST_ONLY))
		{
			switch (nID)
			{
				case ID_WIZ_PERMANENT_RECORDING:
					pPPV = m_pSVWizard->InsertPage(new CPropPageVariable);
					pPPV->Init(IDS_SAVEPROCESSPAGE, IDS_WIZ_ACTIVATE_PERMANENT_SAVE_PROCESS,
						ENABLE_PAGE|ALLOW_NEW_DATA|ALLOW_DELETE_DATA, (LPARAM)pRS, NULL,
						WizardActivateSaveProcessSetActive,
						WizardActivateSaveProcessNext);
					break;
				case ID_WIZ_UVVK_RECORDING:
					pPPV = m_pSVWizard->InsertPage(new CPropPageVariable);
					pPPV->Init(IDS_SAVEPROCESSPAGE, IDS_WIZ_ACTIVATE_UVVK_ALARM_SAVE_PROCESS,
						ENABLE_PAGE|ALLOW_NEW_DATA|ALLOW_DELETE_DATA, (LPARAM)pRS, NULL,
						WizardActivateSaveProcessSetActive,
						WizardActivateSaveProcessNext);

					pPPV = m_pSVWizard->InsertPage(new CPropPageVariable);
					pPPV->Init(IDS_SAVEPROCESSPAGE, IDS_WIZ_ACTIVATE_UVVK_OK_SAVE_PROCESS,
						ENABLE_PAGE|ALLOW_NEW_DATA|ALLOW_DELETE_DATA, (LPARAM)pRS, NULL,
						WizardActivateSaveProcessSetActive,
						WizardActivateSaveProcessNext);
					break;
				default:
					pPPV = m_pSVWizard->InsertPage(new CPropPageVariable);
					pPPV->Init(IDS_SAVEPROCESSPAGE, IDS_WIZ_ACTIVATE_MD_SAVE_PROCESS,
						ENABLE_PAGE|ALLOW_NEW_DATA|ALLOW_DELETE_DATA, (LPARAM)pRS, NULL,
						WizardActivateSaveProcessSetActive,
						WizardActivateSaveProcessNext);
			}
		}
		// 8.Wizardseite aktiviert die Seite für Aktivierungen
		if (OnSelectPage(IDS_ACTIVATIONPAGE, SELECT_FROM_STRING_ID|SELECT_TEST_ONLY))
		{
			pPPV = m_pSVWizard->InsertPage(new CPropPageVariable);
			pPPV->Init(IDS_ACTIVATIONPAGE, IDS_WIZ_ACTIVATE_MD_ACTIVATIONS,
				ENABLE_PAGE|ALLOW_NEW_DATA|ALLOW_DELETE_DATA|INIT_ON_FINISH, (LPARAM)pRS, NULL,
				WizardActivateMDActivateSetActive);
		}
		m_pSVWizard->Create(this);

		Resize();

   }
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK CheckBoxSurveil(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WNDPROC pPrevWndFunc = (WNDPROC)GetWindowLong(hWnd, GWL_USERDATA);
	if (uMsg == WM_LBUTTONUP)
	{
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_SAVE, (LPARAM)hWnd);
	}
	return CallWindowProc(pPrevWndFunc, hWnd, uMsg, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVView::WizardHWBActivateSetActive(CPropPageVariable*pThis, LPARAM)
{
	CSVPropertySheet*pSheet = (CSVPropertySheet*)pThis->GetParent();
	CSVPage         *pPage  = pSheet->GetSVView()->GetSVPage();
	CWnd            *pWnd   = NULL;
	int nID = 0;

	if (pPage == NULL)
	{
		return 0;
	}

	pPage->SetSelection(0);
	switch (pThis->GetPageID())
	{
		case IDS_JACOB1: case IDS_JACOB2: case IDS_JACOB3: case IDS_JACOB4:
			nID = IDC_CHECK_JACOB;
			break;
		case IDS_SAVIC1: case IDS_SAVIC2: case IDS_SAVIC3: case IDS_SAVIC4:
			nID = IDC_CHECK_SAVIC;
			break;
		case IDS_TASHA1: case IDS_TASHA1+1: case IDS_TASHA1+2: case IDS_TASHA1+3:
			nID = IDC_CHECK_TASHA;
			break;
		case IDS_Q_UNIT:
			nID = IDC_CHECK_Q;
			break;
		case IDS_IP_CAMERA_UNIT:
			nID = IDC_CK_IP_CAM_UNIT;
			break;
	}
	pPage->SetSelection(nID);

	pWnd = pPage->GetDlgItem(nID);
	if (pWnd)
	{
		LONG lUserData = GetWindowLong(pWnd->m_hWnd, GWL_USERDATA);
		if (lUserData == 0)
		{
			LONG lOldProc = SetWindowLong(pWnd->m_hWnd, GWL_WNDPROC, (LONG)(WNDPROC)CheckBoxSurveil);
			SetWindowLong(pWnd->m_hWnd, GWL_USERDATA, lOldProc);
		}
	}	

	return 1;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVView::WizardHWBActivateNext(CPropPageVariable*pThis, LPARAM lParam)
{
	CSVPropertySheet*pSheet = (CSVPropertySheet*)pThis->GetParent();
	CSVPage         *pPage  = pSheet->GetSVView()->GetSVPage();
	CWnd            *pWnd   = NULL;
	int nID = 0, nIndex = -1, nIDCheckMD = 0;

	sRoomSurveillance *pRS  = (sRoomSurveillance*)lParam;
	if (pPage == NULL)
	{
		return 0;
	}

	pPage->SetSelection(0);
	switch (pThis->GetPageID())
	{
		case IDS_JACOB1: case IDS_JACOB2: case IDS_JACOB3: case IDS_JACOB4:
			nID = IDC_CHECK_JACOB;
			nIndex = pThis->GetPageID()-IDS_JACOB1;		// 0,..,3
			nIDCheckMD = IDC_CHECK_MD_ON;
			break;
		case IDS_SAVIC1: case IDS_SAVIC2: case IDS_SAVIC3: case IDS_SAVIC4:
			nID = IDC_CHECK_SAVIC;						// 4,..,7
			nIndex = pThis->GetPageID()-IDS_SAVIC1 + INDEX_OFFSET_SAVIC;
			nIDCheckMD = IDC_CHECK_MD_ON;
			break;
		case IDS_TASHA1: case IDS_TASHA1+1: case IDS_TASHA1+2: case IDS_TASHA1+3:
			nID = IDC_CHECK_TASHA;						// 8,..,11
			nIndex = pThis->GetPageID()-IDS_TASHA1 + INDEX_OFFSET_TASHA;
			break;
		case IDS_Q_UNIT:
			nID = IDC_CHECK_Q;
			nIndex = INDEX_Q_CAM;	// rke: only one card, fixed index
			break;
		case IDS_IP_CAMERA_UNIT:
			//nID = IDC_CHECK_Q;
			nIndex = INDEX_IP_CAM;	// rke: only one card, fixed index
			break;
	}
	pWnd = pPage->GetDlgItem(nID);
	if (pWnd)
	{
//		ASSERT_KINDOF(CButton, pWnd);
		if (pWnd->SendMessage(BM_GETCHECK) != BST_CHECKED)
		{
			pRS->m_dwBoardActive &= ~(1 << nIndex);
		}
		else
		{
			pRS->m_dwBoardActive |= (1 << nIndex);
		}
		pPage->SetSelection(nIDCheckMD);
	}

	return 1;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVView::WizardHWBActivateMDSetActive(CPropPageVariable*pThis, LPARAM lParam)
{
	CSVPropertySheet*pSheet = (CSVPropertySheet*)pThis->GetParent();
	CSVView         *pView  = pSheet->GetSVView();
	CSVPage         *pPage  = pView->GetSVPage();
	if (pPage == NULL)
	{
		return 0;
	}

	int nIndex = -1;
	sRoomSurveillance *pRS  = (sRoomSurveillance*)lParam;

	switch (pThis->GetPageID())
	{
		case IDS_JACOB1: case IDS_JACOB2: case IDS_JACOB3: case IDS_JACOB4:
			nIndex = pThis->GetPageID()-IDS_JACOB1;
			break;
		case IDS_SAVIC1: case IDS_SAVIC2: case IDS_SAVIC3: case IDS_SAVIC4:
			nIndex = pThis->GetPageID()-IDS_SAVIC1 + INDEX_OFFSET_SAVIC;
			break;
		case IDS_IP_CAMERA_UNIT:
			nIndex = INDEX_IP_CAM;
			break;
	}

	if (!(pRS->m_dwBoardActive & (1 << nIndex)))
	{
		pSheet->PressLastButton();
		return 0;
	}

	pPage->SetSelection(0);
	switch (pThis->GetPageID())
	{
		case IDS_JACOB1: case IDS_JACOB2: case IDS_JACOB3: case IDS_JACOB4:
			pPage->SetSelection(IDC_CHECK_MD_ON);
			break;
		case IDS_SAVIC1: case IDS_SAVIC2: case IDS_SAVIC3: case IDS_SAVIC4:
			pPage->SetSelection(IDC_MD_CONFIG);
			break;
		case IDS_IP_CAMERA_UNIT:
//			pPage->SetSelection(IDC_MD_CONFIG);
			break;
	}

	return 1;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVView::WizardHWBActivateMDNext(CPropPageVariable*pThis, LPARAM lParam)
{
	CSVPropertySheet*pSheet = (CSVPropertySheet*)pThis->GetParent();
	CSVView         *pView  = pSheet->GetSVView();
	CSVPage         *pPage  = pView->GetSVPage();
	CWnd            *pWnd   = NULL;

	int nIndex;
	sRoomSurveillance *pRS  = (sRoomSurveillance*)lParam;
	if (pPage == NULL)
	{
		return 0;
	}
	if (IsBetween(pThis->GetPageID(),IDS_JACOB1, IDS_JACOB4))
	{
		nIndex = pThis->GetPageID()-IDS_JACOB1;
	}
	else if (IsBetween(pThis->GetPageID(),IDS_SAVIC1, IDS_SAVIC4))
	{
		nIndex = pThis->GetPageID()-IDS_SAVIC1 + 4;
	}
	else
	{
		nIndex = INDEX_IP_CAM;
	}
	
	if (!(pRS->m_dwBoardActive & (1 << nIndex)))
	{
		return 0;
	}

	pWnd = pPage->GetDlgItem(IDC_CHECK_MD_ON);
	if (pWnd)
	{
		ASSERT_KINDOF(CButton, pWnd);
		if (((CButton*)pWnd)->GetCheck() != 1)
		{
			((CButton*)pWnd)->SetCheck(1);
			pPage->SendMessage(WM_COMMAND, MAKELONG(IDC_CHECK_MD_ON,BN_CLICKED), (LPARAM)pThis->m_hWnd);
		}
		pPage->SetSelection(IDC_CHECK_MD_ON);
	}

	if (pPage && pPage->IsModified())
	{
		pPage->OnOK();
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVView::WizardActivateCamerasInit(CPropPageVariable*pThis, LPARAM lParam)
{
	sRoomSurveillance *pRS  = (sRoomSurveillance*)lParam;
	CButton				*pBtn;
	CRect				rc, rcBtn;
	int					nWidth;

	int nPageID = pThis->GetPageID();
	if (HIWORD(nPageID) == 0 && IsInArray(nPageID, pRS->m_OutputSM.GetSize()))
	{
		CSVPropertySheet*pSheet = (CSVPropertySheet*)pThis->GetParent();
		CSVView         *pView  = pSheet->GetSVView();
		CSVDoc          *pDoc   = pView->GetDocument();
		COutputList		*pOutputs = pDoc->GetOutputs();
		COutputGroup	*pGroup  = pOutputs->GetGroupBySMName(pRS->m_OutputSM.GetAt(nPageID));
		if (pGroup)
		{
			nPageID = pGroup->GetID().GetID();
			pThis->SetPageID(nPageID);
			if (pRS->m_nInitialPageID == 0)
			{
				pRS->m_nInitialPageID = nPageID;
			}
		}
		else
		{
			return 0;
		}
	}
	if (pRS->m_nInitialPageID == nPageID)
	{
		pThis->GetDlgItem(IDC_STAT_PPVAR_TEXT)->GetWindowRect(&rc);
		pThis->ScreenToClient(&rc);
		nWidth = rc.Width();
		rcBtn.top	 = rc.bottom +  5;
		rcBtn.bottom = rcBtn.top + 20;
		rcBtn.left	 = rc.left;
		rcBtn.right  = rcBtn.left + nWidth - 5;
		CString str;
		if (str.LoadString(IDS_AUTO_INIT_AND_ACTIVATE))
		{
			pBtn = new CButton();
			if (pBtn)
			{
				pBtn->Create(str, WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_DEFPUSHBUTTON|BS_CHECKBOX|BS_AUTOCHECKBOX, rcBtn, pThis, IDC_DEF_ITEM0);
				pBtn->SetFont(pThis->GetFont());
				pBtn->SetCheck(pRS->m_bSetNames ? BST_CHECKED : BST_UNCHECKED);
			}
		}
		rcBtn.left  += nWidth;
		rcBtn.right  = rcBtn.left + nWidth - 5;
	}	
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVView::WizardActivateCamerasSetActive(CPropPageVariable*pThis, LPARAM)
{
	CSVPropertySheet*pSheet = (CSVPropertySheet*)pThis->GetParent();
	CSVView         *pView  = pSheet->GetSVView();
	CSVPage         *pPage  = pView->GetSVPage();
	if (pPage)
	{
		pPage->SetSelections(IDC_STATIC_TYPE_CAMERA);
	}
	
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVView::WizardActivateCamerasNext(CPropPageVariable*pThis, LPARAM lParam)
{
	sRoomSurveillance *pRS  = (sRoomSurveillance*)lParam;
	CButton *pBtn = (CButton*)pThis->GetDlgItem(IDC_DEF_ITEM0);
	if (pBtn)
	{
		pRS->m_bSetNames = pBtn->GetCheck() == BST_CHECKED ? TRUE : FALSE;
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVView::WizardActivateMDInputInit(CPropPageVariable*pThis, LPARAM lParam)
{
	sRoomSurveillance *pRS  = (sRoomSurveillance*)lParam;

	if (pRS->m_bMD == FALSE || pRS->m_bUVVK)
	{
		return 0;
	}

	int nPageID = pThis->GetPageID();
	if (   HIWORD(nPageID) == 0 
		&& IsBetween(nPageID, 0, pRS->m_InputSM.GetSize()-1)
		&& IsBetween(nPageID, 0, pRS->m_OutputSM.GetSize()-1))
	{
		CSVPropertySheet*pSheet		= (CSVPropertySheet*)pThis->GetParent();
		CSVView         *pView		= pSheet->GetSVView();
		CSVDoc          *pDoc		= pView->GetDocument();
		COutputList		*pOutputs	= pDoc->GetOutputs();
		COutputGroup	*pOutGroup	= pOutputs->GetGroupBySMName(pRS->m_OutputSM.GetAt(nPageID));
		CInputList		*pInputs	= pDoc->GetInputs();
		CInputGroup		*pInGroup	= pInputs->GetGroupBySMName(pRS->m_InputSM.GetAt(nPageID));
		if (pInGroup && pOutGroup)
		{
			pThis->SetPageID(pInGroup->GetID().GetID());
			pRS->m_Outputs.SetAt(pThis, pOutGroup);
		}
		else
		{
			return 0;
		}
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVView::WizardActivateMDInputSetActive(CPropPageVariable*pThis, LPARAM lParam)
{
	sRoomSurveillance *pRS  = (sRoomSurveillance*)lParam;
	CSVPropertySheet*pSheet       = (CSVPropertySheet*)pThis->GetParent();
	CSVView         *pView        = pSheet->GetSVView();
	CSVPage         *pPage        = pView->GetSVPage();
	COutputGroup    *pOutputGroup = NULL;
	void *p;
	if (pRS->m_Outputs.Lookup(pThis, p))
	{
		pOutputGroup = (COutputGroup*)p;
	}
	
	if (pPage && pOutputGroup)
	{
		pPage->SetSelections(IDC_STATIC_TYPE_INPUT);
		int i, nDlgs  = pPage->GetNoOfSubDlgs();
		nDlgs = min(nDlgs, pOutputGroup->GetSize());
		bool bChanged = false;
		for (i=0; i<nDlgs; i++)
		{
			CWK_Dialog *pDlg    = pPage->GetSubDlg(i);
			COutput    *pOutput = pOutputGroup->GetOutput(i);
			if (pDlg && pOutput)
			{
				if (pOutput->GetExOutputType() != EX_OUTPUT_OFF)	// ist die Kamera aktiviert
				{
					CWnd *pWnd = pDlg->GetDlgItem(IDC_SPIN_TYPE_INPUT);
					ASSERT_KINDOF(CSpinButtonCtrl, pWnd);			// und der MD-Melder inaktiv
					if (((CSpinButtonCtrl*)pWnd)->GetPos() == INPUT_OFF)
					{
						((CSpinButtonCtrl*)pWnd)->SetPos(INPUT_MELDER);// MD-Melder aktivieren
						pWnd = pDlg->GetDlgItem(IDC_EDGE);
						if (pWnd)
						{
							ASSERT_KINDOF(CButton, pWnd);
							((CButton*)pWnd)->SetCheck(0);			// MD-Melder als Schließer definieren
						}
						bChanged = true;
					}
					if (pRS->m_bSetNames)
					{
						pWnd = pDlg->GetDlgItem(IDC_NAME_INPUT);
						if (pWnd)
						{
							ASSERT_KINDOF(CEdit, pWnd);
							pWnd->SetWindowText(pOutput->GetName()+_T(" MD"));// Namen der Kamera für den Melder vergeben
						}
						bChanged = true;
					}
				}
			}
		}
		if (bChanged)
		{
			pPage->SetModified();
		}
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVView::WizardActivateSaveProcessSetActive(CPropPageVariable*pThis, LPARAM lParam)
{
	CSVPropertySheet*pSheet       = (CSVPropertySheet*)pThis->GetParent();
	sRoomSurveillance *pRS	  = (sRoomSurveillance*)lParam;
	CSVView         *pView        = pSheet->GetSVView();
	CSVPage         *pPage        = pView->GetSVPage();
	CSVDoc          *pDoc         = pView->GetDocument();
	CProcessList    *pProcessList = pDoc->GetProcesses();
	CProcess        *pProcess     = NULL;
	Resolution       res          = RESOLUTION_2CIF;
	Compression      comp         = COMPRESSION_3;
	WORD             nPict        = 5;
	DWORD            nRecT        = 1000,
	                 nPaus        = 0,
	                 nDura        = 2000;
	BOOL             bHold        = TRUE,
	                 bInterval    = TRUE;

	switch (pThis->GetTextID())
	{
		case IDS_WIZ_ACTIVATE_MD_SAVE_PROCESS:
			if (pRS->m_bUVVK || pRS->m_bMD == FALSE) return 0;
			break;
		case IDS_WIZ_ACTIVATE_PERMANENT_SAVE_PROCESS:
			if (pRS->m_bMD || pRS->m_bUVVK) return 0;
			nPict        = 5;
			nRecT        = 1000;
			nPaus        = 0;
			nDura        = 0;
			bHold        = FALSE;
			bInterval    = TRUE;		
			break;
		case IDS_WIZ_ACTIVATE_UVVK_OK_SAVE_PROCESS:
			if (pRS->m_bUVVK == FALSE) return 0;
			nPict        = 1;
			nRecT        = 1000;
			nPaus        = 0;
			nDura        = 0;
			bHold        = FALSE;
			bInterval    = TRUE;		
			break;
		case IDS_WIZ_ACTIVATE_UVVK_ALARM_SAVE_PROCESS:
			if (pRS->m_bUVVK == FALSE) return 0;
			nPict        = 2;
			nRecT        = 1000;
			nPaus        = 0;
			nDura        = 0;
			bHold        = FALSE;
			bInterval    = TRUE;		
			break;
	}

	pPage->SetSelection(IDC_TXT_NR);

	CWnd *pWnd = pPage->GetDlgItem(IDC_SAVE_LIST_PROCESS);
	if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CListBox)))
	{
		CListBox *pList = (CListBox*) pWnd;
		int i, nP = pList->GetCount();
		for (i=0; i<nP; i++)
		{
			pProcess = (CProcess*)pList->GetItemData(i);
			if (pProcess && pProcess->IsSave())
			{
				if (
//					(pProcess->GetNrPict()      == nPict) &&				// 1 Bild
					(pProcess->GetRecordTime()  == nRecT) &&				// in einer Sekunde (1000 ms)
					(pProcess->GetPause()       == nPaus) &&				// Pausenzeit 0
//					(pProcess->GetDuration()    == nDura) &&				// Gesamtdauer 2 Sekunden (2000 ms)
					(pProcess->GetResolution()  == res  ) &&				// Auflösung fein
					(pProcess->GetCompression() == comp ) &&				// Kompression mittel
					(pProcess->IsHold()         == bHold) &&				// Volle Zeitdauer
					(pProcess->IsInterval()     == bInterval)				// Intervallprozess
					)
				{
				if (pThis->GetTextID() == IDS_WIZ_ACTIVATE_UVVK_OK_SAVE_PROCESS)
					pRS->m_ProcessIDprering = pProcess->GetID();
				else
					pRS->m_ProcessID = pProcess->GetID();
					break;													// Prozess gefunden
				}
			}
		}

		if (i < nP)																	// Prozess selektieren
		{
			pList->SetCurSel(i);
			pPage->SendMessage(WM_COMMAND, MAKELONG(IDC_SAVE_LIST_PROCESS, LBN_SELCHANGE), (LPARAM)pList->m_hWnd);
		}
		else																			// neuen Prozess erzeugen
		{
			CString strName;
			strName.Format(_T("%dfps"), MulDiv(nPict, nRecT, 1000));
			pProcess = pProcessList->AddProcess();
			pProcess->SetSave(strName, nDura, nPaus, nPict, nRecT, res, comp, bHold, bInterval);
			pProcessList->Save(pDoc->GetProfile());
			pPage->SetModified();
			pPage->OnCancel();
			pList->SetCurSel(pList->GetCount()-1);
			pPage->SendMessage(WM_COMMAND, MAKELONG(IDC_SAVE_LIST_PROCESS, LBN_SELCHANGE), (LPARAM)pList->m_hWnd);
			pPage->SetModified(FALSE, TRUE);
		}
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVView::WizardActivateSaveProcessNext(CPropPageVariable*pThis, LPARAM lParam)
{
	CSVPropertySheet*pSheet       = (CSVPropertySheet*)pThis->GetParent();
	sRoomSurveillance *pRS	  = (sRoomSurveillance*)lParam;
	CSVView         *pView        = pSheet->GetSVView();
	CSVPage         *pPage        = pView->GetSVPage();
	CProcess        *pProcess     = NULL;
	CWnd *pWnd = pPage->GetDlgItem(IDC_SAVE_LIST_PROCESS);
	if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CListBox)))
	{
		CListBox *pList = (CListBox*) pWnd;
		int nCurSel = pList->GetCurSel();
		if (nCurSel != LB_ERR)
		{
			pProcess = (CProcess*)pList->GetItemData(nCurSel);
			if (pProcess)
			{
				if (pThis->GetTextID() == IDS_WIZ_ACTIVATE_UVVK_OK_SAVE_PROCESS)
					pRS->m_ProcessIDprering = pProcess->GetID();
				else
					pRS->m_ProcessID = pProcess->GetID();
			}
		}
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVView::WizardInitDrivesSetActive(CPropPageVariable*pThis, LPARAM lParam)
{
	CSVPropertySheet*pSheet       = (CSVPropertySheet*)pThis->GetParent();
	CSVView         *pView        = pSheet->GetSVView();
	CSVPage         *pPage        = pView->GetSVPage();
	pPage->SetSelection(IDC_RADIO_DATABASE);
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVView::WizardInitMDArchivesSetActive(CPropPageVariable*pThis, LPARAM lParam)
{
	sRoomSurveillance *pRS  = (sRoomSurveillance*)lParam;
	CSVPropertySheet*pSheet       = (CSVPropertySheet*)pThis->GetParent();
	CSVView         *pView        = pSheet->GetSVView();
	CSVPage         *pPage        = pView->GetSVPage();
	CSVDoc          *pDoc         = pView->GetDocument();
	COutputList		*pOutputs	  = pDoc->GetOutputs();
	CArchivInfoArray*pArchivs  = pDoc->GetArchivs();
	COutputGroup	*pGroup;
	COutput			*pOutput;
	CArchivInfo		*pAI = NULL, *pAIUVV, *pAItemp;
	int i, nGroup, nOutput;
	BOOL bSomeCreated = FALSE, bFound;

	pPage->SetSelection(IDC_STATIC_SIZE);

	if (pRS->m_bSetNames)
	{
		for (nGroup=0; nGroup<pOutputs->GetSize(); nGroup++)
		{
			pGroup = pOutputs->GetGroupAt(nGroup);
			for (nOutput=0; nOutput<pGroup->GetSize(); nOutput++)
			{
				pOutput = pGroup->GetOutput(nOutput);
				if (pOutput->IsCamera())
				{
					CString sTemp = pOutput->GetName();
					CString sCamName;
					pAI = NULL;
					pAIUVV = NULL;
					bFound = FALSE;
					if (pRS->m_bUVVK)
					{
						for (i=0; i<pArchivs->GetSize(); i++)
						{
							pAItemp = pArchivs->GetAt(i);
							if (   pAItemp->GetName() == sTemp)
							{
								bFound = TRUE;
								if (pAItemp->GetType() == ALARM_ARCHIV)
								{
									pAI = pAItemp;
									sCamName = sTemp;
								}
								else if (pAItemp->GetType() == SICHERUNGS_RING_ARCHIV)
								{
									pAIUVV = pAItemp;
								}
							}
						}
						if (bFound) continue;
						if (pAI == NULL)
						{
							sCamName = sTemp;
							pAI = pArchivs->AddArchivInfo();
							pAI->SetTyp(ALARM_ARCHIV);
							pAI->SetName(sCamName);
							pAI->SetSizeMB(400);	// ca. 12 * vorring
							bSomeCreated = TRUE;
						}
						if (pAIUVV == NULL)
						{
							pAIUVV = pArchivs->AddArchivInfo();
							pAIUVV->SetName(pAI->GetName());
							pAIUVV->SetTyp(SICHERUNGS_RING_ARCHIV);
							pAIUVV->SetSafeRingFor(pAI->GetID().GetSubID());
							pAIUVV->SetSizeMB(35); // ca. 1 fps, 40 KB, 15 min
							bSomeCreated = TRUE;
						}
						pRS->m_Archives.SetAt(pOutput, MAKELONG(pAI->GetID().GetSubID(), pAIUVV->GetID().GetSubID()));
					}
					else	// not UVVK
					{
						for (i=0; i<pArchivs->GetSize(); i++)
						{
							pAItemp = pArchivs->GetAt(i);
							if (pAItemp->GetName() == sTemp)
							{
								bFound = TRUE;
								if (pAItemp->GetType() == RING_ARCHIV)
								{
									pAI = pAItemp;
									sCamName = sTemp;
									break;
								}
							}
						}
						if (bFound) continue;
						if (pAI == NULL)
						{
							sCamName = sTemp;
							pAI = pArchivs->AddArchivInfo();
							pAI->SetName(sCamName);
							bSomeCreated = TRUE;
						}
						pRS->m_Archives.SetAt(pOutput, pAI->GetID().GetSubID());
					}
				}
			}
		}
		if (bSomeCreated)
		{
			pArchivs->Save(pDoc->GetProfile());
			pPage->SetModified();
			pPage->OnCancel();
			pPage->SetModified(FALSE, TRUE);
		}
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVView::WizardActivateMDActivateSetActive(CPropPageVariable*pThis, LPARAM lParam)
{
	sRoomSurveillance *pRS  = (sRoomSurveillance*)lParam;

	if (pRS->m_bSetNames)
	{
		CSVPropertySheet*pSheet     = (CSVPropertySheet*)pThis->GetParent();
		CSVView         *pView      = pSheet->GetSVView();
		CSVPage         *pPage      = pView->GetSVPage();
		CSVDoc          *pDoc       = pView->GetDocument();
		COutputList		*pOutputs	= pDoc->GetOutputs();
		CInputList		*pInputs	= pDoc->GetInputs();
		CInputGroup		*pInputGroup;
		CInput			*pInput, *pAlwaysAlarm = NULL;
		CInputToOutput	*pIn2Out;
		COutputGroup	*pOutputGroup;
		COutput			*pOutput;
		BOOL			bFound = FALSE,
						bSomeCreated = FALSE;
		WORD			wArchive;
		int nGroup, nPut, nIn2Out;

		pInputGroup = pInputs->GetGroupBySMName(SM_LAUNCHER_INPUT);
		if (pInputGroup)
		{
			CString sTmp;
			sTmp.LoadString(IDS_VA_ALWAYS_ALARM);
			for (nPut=0; nPut<pInputGroup->GetSize(); nPut++)
			{
				pAlwaysAlarm = pInputGroup->GetInput(nPut);
				if (pAlwaysAlarm->GetName() == sTmp)
				{
					break;
				}
			}
		}

		for (nGroup=0; nGroup<pRS->m_InputSM.GetCount(); nGroup++)
		{
			pOutputGroup = pOutputs->GetGroupBySMName(pRS->m_OutputSM.GetAt(nGroup));
			pInputGroup	 = pInputs->GetGroupBySMName(pRS->m_InputSM.GetAt(nGroup));
			if (pInputGroup && pOutputGroup)
			{
				for (nPut=0; nPut<pInputGroup->GetSize() && nPut<pOutputGroup->GetSize(); nPut++)
				{
					pOutput = pOutputGroup->GetOutput(nPut); 
					if (pOutput->GetExOutputType() != EX_OUTPUT_OFF)
					{
						pInput  = pInputGroup->GetInput(nPut);
						if (pInput->GetIDActivate() == SECID_ACTIVE_OFF && pAlwaysAlarm)
						{
							pInput = pAlwaysAlarm;
							pAlwaysAlarm->SetIDActivate(SECID_ACTIVE_ON);
						}	
						bFound = FALSE;
						for (nIn2Out=0; nIn2Out<pInput->GetNumberOfInputToOutputs(); nIn2Out++)
						{
							pIn2Out = pInput->GetInputToOutput(nIn2Out);
							if (pIn2Out->GetOutputOrMediaID() == pOutput->GetID())
							{
								bFound = TRUE;
							}
						}
						if (!bFound)
						{
							DWORD dwArchive = 0;
							pIn2Out = pInput->AddInputToOutput();
							if (pIn2Out)
							{
								pRS->m_Archives.Lookup(pOutput, dwArchive);
								wArchive = LOWORD(dwArchive);
								pIn2Out->SetArchiveID(CSecID(SECID_GROUP_ARCHIVE, wArchive));
								pIn2Out->SetOutputID(pOutput->GetID());
								pIn2Out->SetInputState(IO_ALARM);
								pIn2Out->SetTimerID(SECID_NO_ID);
								pIn2Out->SetPriority(3);
								pIn2Out->SetProcessID(pRS->m_ProcessID);
								pIn2Out->SetNotificationID(SECID_NO_ID);
								bSomeCreated = TRUE;
							}
							if (pRS->m_bUVVK && dwArchive)
							{
								pIn2Out = pInput->AddInputToOutput();
								wArchive = HIWORD(dwArchive);
								pIn2Out->SetArchiveID(CSecID(SECID_GROUP_ARCHIVE, wArchive));
								pIn2Out->SetOutputID(pOutput->GetID());
								pIn2Out->SetInputState(IO_OK);
								pIn2Out->SetTimerID(SECID_NO_ID);
								pIn2Out->SetPriority(3);
								pIn2Out->SetProcessID(pRS->m_ProcessIDprering);
								pIn2Out->SetNotificationID(SECID_NO_ID);
							}
						}
					}
				}
			}
		}
		if (bSomeCreated)
		{
			pDoc->GetInputs()->Save(pDoc->GetProfile(), FALSE);
			pPage->SetModified();
			pPage->OnCancel();
			pPage->SetModified(FALSE, TRUE);
		}
	}	
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnUpdateWizPermanentRecording(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(theApp.m_bProfessional);
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnUpdateWizUvvkRecording(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(theApp.m_bProfessional);
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnIdleSVView()
{
	if (m_pSVPage && m_pSVPage->m_hWnd)
	{
		m_pSVPage->OnIdleSVPage();
	}
}
//////////////////////////////////////////////////////////////////////////
LRESULT CSVView::OnUserMessage(WPARAM wParam,  LPARAM lParam)
{
	switch (wParam)
	{
		case USER_MSG_UPDATE_PARENT_TREE:
		{
			if (lParam & (1<<IMAGE_INPUT   )) OnInputChanged();
			if (lParam & (1<<IMAGE_RELAY   )) OnRelaisChanged();
			if (lParam & (1<<IMAGE_CAMERA  )) OnCameraChanged();
			if (lParam & (1<<IMAGE_HARDWARE)) OnHardwareChanged();
			if (lParam)
			{
				if (m_pSVPage && m_pSVPage->IsKindOf(RUNTIME_CLASS(CDefaultTreeKnode)))
				{
					HTREEITEM hOld = m_SelectedItem;
					m_SelectedItem = NULL;
					TreeSelchanged(hOld);
				}
			}
		}break;
		case USER_MSG_RELOAD_SETTINGS:
			GetDocument()->OnNewDocument();
			break;
		case USER_MSG_REINIT_TREE:
			Clear();
			InitTree();
			break;
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
void CSVView::OnDestroy()
{
	if (m_pSVWizard && m_pSVWizard->m_hWnd)
	{
		m_pSVWizard->DestroyWindow();
	}
	CView::OnDestroy();
}
//////////////////////////////////////////////////////////////////////////
void CSVView::EmptyWizardPointer()
{
	m_pSVWizard = NULL;
}
//////////////////////////////////////////////////////////////////////////
void CSVView::OnWizDesignaPM100Recording()
{
	if (   m_pSVPage
		&& m_pSVPage->IsKindOf(RUNTIME_CLASS(COutputPage)))
	{
		CSVDoc* pDoc = GetDocument();
		COutputPage* pOutputPage = (COutputPage*)m_pSVPage;
		CSecID camID = pOutputPage->GetSelectedCamera();
		const COutput* pOutput = pDoc->GetOutputs()->GetOutputByID(camID);
		if (pOutput)
		{
			TRACE(_T("run Designa PM 100 Wizard on camera %s\n"),pOutput->GetName());
			// modaler Dialog für BFR TCC und Alarme.

			CDesignaWizardDialog dlg;

			dlg.m_sCameraName = pOutput->GetName();

			if (IDOK==dlg.DoModal())
			{
				if (pDoc->CreateDesigna(SDICT_DESIGNA_PM_100_ALARM,
										camID,dlg.m_dwBFR,dlg.m_dwTCC,dlg.m_PM100Alarm,dlg.m_PM100XML))
				{
					m_bServerInit = TRUE;
					AddInitApp(WK_APP_NAME_DB_SERVER);
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CSVView::OnUpdateWizDesignaPM100Recording(CCmdUI *pCmdUI)
{
	CSVDoc* pDoc = GetDocument();

	pCmdUI->Enable(   pDoc->GetDongle()->AllowSDIParkmanagment() 
				   && m_pSVPage
				   && (pDoc->GetFlags() & WK_ALLOW_INPUT)
				   && (pDoc->GetFlags() & WK_ALLOW_PROZESS)
				   && m_pSVPage->IsKindOf(RUNTIME_CLASS(COutputPage)));
}
//////////////////////////////////////////////////////////////////////////
void CSVView::OnUpdateWizDesignaPMAbacusRecording(CCmdUI *pCmdUI)
{
	CSVDoc* pDoc = GetDocument();

	pCmdUI->Enable(   pDoc->GetDongle()->AllowSDIParkmanagment() 
		&& m_pSVPage
		&& (pDoc->GetFlags() & WK_ALLOW_INPUT)
		&& (pDoc->GetFlags() & WK_ALLOW_PROZESS)
		&& m_pSVPage->IsKindOf(RUNTIME_CLASS(COutputPage)));
}
//////////////////////////////////////////////////////////////////////////
void CSVView::OnWizDesignaPMAbacusRecording()
{
	// TODO: Add your command handler code here
	if (   m_pSVPage
		&& m_pSVPage->IsKindOf(RUNTIME_CLASS(COutputPage)))
	{
		CSVDoc* pDoc = GetDocument();
		COutputPage* pOutputPage = (COutputPage*)m_pSVPage;
		CSecID camID = pOutputPage->GetSelectedCamera();
		const COutput* pOutput = pDoc->GetOutputs()->GetOutputByID(camID);
		if (pOutput)
		{
			TRACE(_T("run Designa PM Abacus Wizard on camera %s\n"),pOutput->GetName());
			// modaler Dialog für BFR TCC und Alarme.

			CDesignaWizardDialog dlg;

			dlg.m_sCameraName = pOutput->GetName();
			dlg.m_SDICT = SDICT_DESIGNA_PM_ABACUS;

			if (IDOK==dlg.DoModal())
			{
				if (pDoc->CreateDesigna(SDICT_DESIGNA_PM_ABACUS,
										camID,dlg.m_dwBFR,dlg.m_dwTCC,dlg.m_PM100Alarm,dlg.m_PM100XML))
				{
					m_bServerInit = TRUE;
					AddInitApp(WK_APP_NAME_DB_SERVER);
				}
			}
		}
	}
}
