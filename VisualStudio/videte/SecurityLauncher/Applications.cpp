// Applications.cpp: implementation of the CApplications class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SecurityLauncher.h"
#include "Applications.h"
#include "MeszMez.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CApplications::CApplications()
{
	m_pDongle = NULL;
	m_nMode	  = LM_NORMAL;
}
//////////////////////////////////////////////////////////////////////
CApplications::~CApplications()
{
	DeleteAll();
}
//////////////////////////////////////////////////////////////////////
void CApplications::DeleteAll() 
{ 
	for (int i=0;i<GetSize();i++) 
	{ 
		CApplication*pApp = GetAt(i);
		WK_DELETE(pApp);
	} 
	RemoveAll(); 
};
//////////////////////////////////////////////////////////////////////
CApplication* CApplications::GetApplication(const CString& sPath)
{
	CString exe;

	for (int i=0; i<GetSize();i++)
	{
		CApplication* pApp = GetAt(i);
		if (pApp)
		{
			if (pApp->GetId()==WAI_COCOUNIT)
			{
				if (IsFileForCocoUnit(sPath))
				{
					return pApp;
				}
			}
			else if (pApp->GetId()==WAI_MICOUNIT)
			{
				if (IsFileForMicoUnit(sPath))
				{
					return pApp;
				}
			}
			else
			{
				exe = pApp->GetExePathName();
				if (exe.CompareNoCase(sPath)==0)
				{
					return pApp;
				}
			}
		}
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////
CApplication* CApplications::GetApplication(WK_ApplicationId applicationId)
{
	if ((applicationId==WAI_INVALID)||(applicationId==WAI_LAUNCHER))
	{
		return NULL;
	}

	for (int i=0; i<GetSize();i++)
	{
		CApplication* pApp = GetAt(i);
		if (pApp && pApp->GetId()==applicationId)
		{
			return pApp;
		}
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////
BOOL CApplications::InitAppsAndDongle(CWK_Dongle* pDongle, CMeszMez* pMeszMez, int nMode, BOOL bUseUH)
{
	CApplication* pApp = NULL;

	m_pDongle = pDongle;
	m_nMode	  = nMode;

	InternalProductCode ipc = m_pDongle->GetProductCode();
	CWK_Profile wkp;

	if (m_nMode == LM_NORMAL)
	{
		// create app records, an object for every application to control

		// WinClk
		if (pMeszMez->IsDCF())
		{
			pApp = new CApplication();
			pApp->Create(_T("WinClk"),FALSE,2000);
			Add(pApp);
		}

		// COCO
		if (m_pDongle->RunQUnit())
		{
			pApp = new CApplication();
			pApp->Create(WAI_QUNIT,TRUE,20000);
			Add(pApp);
		}
		// COMM
		if (m_pDongle->RunCommUnit())
		{
			pApp = new CApplication();
			pApp->Create(WAI_COMMUNIT,FALSE,2000);
			Add(pApp);
		}
		// GEMOS
		if (m_pDongle->RunGemosUnit())
		{
			pApp = new CApplication();
			pApp->Create(WAI_GEMOS_UNIT,FALSE,2000);
			Add(pApp);
		}
		// IP Cam
		if (m_pDongle->RunIPCameraUnit())
		{
			pApp = new CApplication();
			pApp->Create(WAI_IP_CAMERA_UNIT,TRUE,15000);
			Add(pApp);
		}

		// JACOB1
		if (m_pDongle->RunJaCobUnit1())
		{
			pApp = new CApplication();
			pApp->Create(WAI_JACOBUNIT_1,TRUE,15000, 80, 80);
			Add(pApp);
		}

		// JACOB2
		if (m_pDongle->RunJaCobUnit2())
		{
			pApp = new CApplication();
			pApp->Create(WAI_JACOBUNIT_2, TRUE, 15000, 80, 80);
			Add(pApp);
		}

		// JACOB3
		if (m_pDongle->RunJaCobUnit3())
		{
			pApp = new CApplication();
			pApp->Create(WAI_JACOBUNIT_3, TRUE, 15000, 80, 80);
			Add(pApp);
		}

		// JACOB4
		if (m_pDongle->RunJaCobUnit4())
		{
			pApp = new CApplication();
			pApp->Create(WAI_JACOBUNIT_4, TRUE, 15000, 80, 80);
			Add(pApp);
		}

		// SAVIC1
		if (m_pDongle->RunSaVicUnit1())
		{
			pApp = new CApplication();
			pApp->Create(WAI_SAVICUNIT_1,TRUE,20000, 80, 80);
			Add(pApp);
		}

		// SAVIC2
		if (m_pDongle->RunSaVicUnit2())
		{
			pApp = new CApplication();
			pApp->Create(WAI_SAVICUNIT_2, TRUE, 20000, 80, 80);
			Add(pApp);
		}

		// SAVIC3
		if (m_pDongle->RunSaVicUnit3())
		{
			pApp = new CApplication();
			pApp->Create(WAI_SAVICUNIT_3, TRUE, 20000, 80, 80);
			Add(pApp);
		}

		// SAVIC4
		if (m_pDongle->RunSaVicUnit4())
		{
			pApp = new CApplication();
			pApp->Create(WAI_SAVICUNIT_4, TRUE, 20000, 80, 80);
			Add(pApp);
		}

		// TASHA1
		if (m_pDongle->RunTashaUnit1())
		{
			pApp = new CApplication();
			pApp->Create(WAI_TASHAUNIT_1,TRUE,25000, 80, 80);
			Add(pApp);
		}

		// TASHA2
		if (m_pDongle->RunTashaUnit2())
		{
			pApp = new CApplication();
			pApp->Create(WAI_TASHAUNIT_2, TRUE, 25000, 80, 80);
			Add(pApp);
		}

		// TASHA3
		if (m_pDongle->RunTashaUnit3())
		{
			pApp = new CApplication();
			pApp->Create(WAI_TASHAUNIT_3, TRUE, 25000, 80, 80);
			Add(pApp);
		}

		// TASHA4
		if (m_pDongle->RunTashaUnit4())
		{
			pApp = new CApplication();
			pApp->Create(WAI_TASHAUNIT_4, TRUE, 25000, 80, 80);
			Add(pApp);
		}

		// Audio 1 und 2
		if (m_pDongle->RunAudioUnit())
		{
			pApp = new CApplication();
			pApp->Create(WAI_AUDIOUNIT_1,TRUE,1000,40,40);
			Add(pApp);

			pApp = new CApplication();
			pApp->Create(WAI_AUDIOUNIT_2,TRUE,1000,40,40);
			Add(pApp);

			pApp = new CApplication();
			pApp->Create(WAI_AUDIOUNIT_3,TRUE,1000,40,40);
			Add(pApp);

			pApp = new CApplication();
			pApp->Create(WAI_AUDIOUNIT_4,TRUE,1000,40,40);
			Add(pApp);
		}

		HINSTANCE hInst = LoadLibrary(_T("Capi2032.dll"));
		BOOL bCapi2032 = FALSE;
		if (hInst)
		{
			FreeLibrary(hInst);
			bCapi2032 = TRUE;
			// ISDN
			if (m_pDongle->RunISDNUnit())
			{
				pApp = new CApplication();
				pApp->Create(WAI_ISDN_UNIT,FALSE,2000);
				Add(pApp);
			}

			// ISDN 2
			if (m_pDongle->RunISDNUnit2())
			{
				pApp = new CApplication();
				pApp->Create(WAI_ISDN_UNIT_2,FALSE,2000);
				Add(pApp);
			}

			// SMS
			if (m_pDongle->RunSMSUnit())
			{
				pApp = new CApplication();
				pApp->Create(WAI_SMS_UNIT,FALSE,4000);
				Add(pApp);
			}

			// Presence
			if (m_pDongle->RunPTUnit())
			{
				pApp = new CApplication();
				pApp->Create(WAI_PRESENCE_UNIT,FALSE,2000);
				Add(pApp);
			}

			// Presence 2
			if (m_pDongle->RunPTUnit2())
			{
				pApp = new CApplication();
				pApp->Create(WAI_PRESENCE_UNIT_2,FALSE,2000);
				Add(pApp);
			}
		}
		// TeleObserver 
		if (m_pDongle->RunTOBSUnit())
		{
			pApp = new CApplication();
			pApp->Create(WAI_TOBS_UNIT,FALSE,2000);
			Add(pApp);
		}

		
		CSocket socket;
		BOOL bTCPIP = FALSE;
		if (socket.Create(0, SOCK_STREAM, _T("127.0.0.1")))
		{
			// TCP/IP
			bTCPIP = TRUE;
			if (m_pDongle->RunSocketUnit())
			{
				pApp = new CApplication();
				pApp->Create(WAI_SOCKET_UNIT,FALSE,4000);
				Add(pApp);
			}

			// HTTP
			if (m_pDongle->RunHTTP())
			{
				pApp = new CApplication();
				pApp->Create(WAI_HTTP,TRUE,2000);
				Add(pApp);
			}
		}


		// EMail
        if (bCapi2032 || bTCPIP)
		{
			if (m_pDongle->RunEMailUnit())
			{
				pApp = new CApplication();
				pApp->Create(WAI_EMAIL_UNIT,TRUE,2000);
				Add(pApp);
			}
		}

		// SDI
		if (m_pDongle->RunSDIUnit())
		{
			pApp = new CApplication();
			pApp->Create(WAI_SDIUNIT,FALSE,2000);
			Add(pApp);
		}

		// SIM
		if (m_pDongle->RunSimUnit())
		{
			pApp = new CApplication();
			pApp->Create(WAI_SIMUNIT,TRUE,2000);
			Add(pApp);
		}

		// AKU
		if (m_pDongle->RunAKUUnit())
		{
			pApp = new CApplication();
			pApp->Create(WAI_AKUUNIT,TRUE,2000);
			Add(pApp);
		}

		// ICPCon Unit
//		if (   m_pDongle->RunICPCONUnit()
//			|| wkp.GetInt(_T("ICPConUnit\\Settings"), _T("SwitchPanel"), 0))
		{
			pApp = new CApplication();
			pApp->Create(WAI_ICPCON_UNIT,TRUE,4000);
			Add(pApp);
		}

		// Die YutaUnit wird nicht verdongled
		pApp = new CApplication();
		pApp->Create(WAI_YUTA_UNIT,TRUE,10000);
		Add(pApp);

		// ACDC - aber nur unter NT, 2000 und XP
		if (   m_pDongle->RunCDRWriter()
			//&& theApp.IsWinNT()			//15.01.2004 TKR nun doch unter 98/ME
			)
		{
			pApp = new CApplication();
			pApp->Create(WAI_AC_DC,TRUE,2000);
			Add(pApp);
		}

		// DBS
		if (m_pDongle->RunDatabaseServer())
		{
			pApp = new CApplication();
			pApp->Create(WAI_DATABASE_SERVER,FALSE,10000,100);
			Add(pApp);
		}

		// ICPCON Client
		if (m_pDongle->RunICPCONClient())
		{
			pApp = new CApplication();
			pApp->Create(WAI_ICPCON_CLIENT,TRUE,1000);
			Add(pApp);
		}

		if (m_pDongle->RunServer())
		{
			// SERVER
			pApp = new CApplication();
			pApp->Create(WAI_SECURITY_SERVER,FALSE,10000);
			Add(pApp);
		}

		// SDIConfig
		if (m_pDongle->RunSDIConfig())
		{
			pApp = new CApplication();
			pApp->Create(WAI_SDICONFIG,FALSE,3000,5,0,FALSE);
			Add(pApp);
		}

		// SV
		if (m_pDongle->RunSupervisor())
		{
			pApp = new CApplication();
			pApp->Create(WAI_SUPERVISOR,FALSE,3000,5,0,FALSE);
			Add(pApp);
		}

		// Product View
		if (   (ipc != IPC_TOBS_RECEIVER)
			&& (ipc != IPC_DTS_RECEIVER)
			)
		{
			pApp = new CApplication();
			pApp->Create(WAI_PRODUCT_VIEW,TRUE,3000,5,0,FALSE);
			Add(pApp);
		}

		// Product Version
		if (   (ipc != IPC_TOBS_RECEIVER)
			&& (ipc != IPC_DTS_RECEIVER)
			)
		{
			pApp = new CApplication();
			pApp->Create(WAI_PRODUCT_VERSION,TRUE,3000,5,0,FALSE);
			Add(pApp);
		}

		// Log View
		pApp = new CApplication();
		pApp->Create(WAI_LOG_VIEW,FALSE,3000,5,0,FALSE);
		Add(pApp);

		if (bUseUH)
		{
			pApp = new CApplication();
			pApp->Create(WAI_UPDATE_HANDLER,FALSE,1000,5,0,FALSE);
			Add(pApp);
		}

		// idipClient
		if (   m_pDongle->RunDatabaseClient()
			|| m_pDongle->RunExplorer())
		{
			pApp = new CApplication();
			pApp->Create(WAI_IDIP_CLIENT,FALSE,5000);
			Add(pApp);
		}
		if (m_pDongle->IsTransmitter())
		{
			if (wkp.GetInt(REG_PATH_PC_HEALTH, STAND_ALONE, -1) != -1)
			{
				pApp = new CApplication();
				pApp->Create(WAI_HEALTH_CONTROL,FALSE,5000);
				Add(pApp);
			}
		}
	}
	else if (m_nMode == LM_BACKUP)
	{
		// MICO
		if (m_pDongle->RunMiCoUnit())
		{
			pApp = new CApplication();
			pApp->Create(WAI_MICOUNIT,TRUE,15000);
			Add(pApp);
		}
		// DBBackup
		// DBC
		if (m_pDongle->RunCDRWriter())
		{
			WK_TRACE(_T("Modus CD Sicherung\n"));
			if (m_pDongle->RunDatabaseServer())
			{
				pApp = new CApplication();
				pApp->Create(WAI_DATABASE_SERVER,FALSE,10000,100);
				Add(pApp);
			}
			if (m_pDongle->RunDatabaseClient())
			{
				pApp = new CApplication();
				pApp->Create(WAI_DATABASE_CLIENT,FALSE,3000,5,0,FALSE);
				Add(pApp);
			}
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CApplications::StartAll()
{
	CApplication* pApp;
	int i,c;
	c = GetSize();
	for (i=0;i<c;i++)
	{
		pApp = GetAt(i);
		if (pApp && pApp->KeepAlive())
		{
			pApp->InitialStart();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CApplications::TerminateAll()
{
	CApplication* pApp = NULL;
	int i,c;

	c = GetSize();
	for (i=c-1;i>=0;i--)
	{
		pApp = GetAt(i);
		if (pApp)
		{
			pApp->Terminate();
		}
	}

	// Wait for last App to REAL terminate
	// Otherwise WK_IS_RUNNING in StartAll sometimes returns TRUE!
	DWORD dwTime = 300;
	BOOL bRealTerminated = FALSE;
	DWORD dwTime1 = GetTickCount();
	DWORD dwTime2;
	while(!bRealTerminated && dwTime)
	{
		bRealTerminated = TRUE;
		for (i=c-1;i>=0;i--)
		{
			pApp = GetAt(i);
			if (pApp && WK_IS_RUNNING(GetAppnameFromId(pApp->GetId())))
			{
				WK_TRACE(_T("### TerminateAll %s still running\n"), NameOfEnum(pApp->GetId()));
				bRealTerminated = FALSE;
			}
		}
		if (!bRealTerminated)
		{
			Sleep(100);
			dwTime--;

		}
	}
	dwTime2 = GetTickCount();
	CString s;
	if (bRealTerminated)
	{
		s.Format(_T("TerminateAll OK %d ms"), dwTime2-dwTime1);
	}
	else
	{
		s.Format(_T("TerminateAll timeout %d ms"), dwTime2-dwTime1);
	}
	WK_TRACE(_T("%s\n"), s);
}
/////////////////////////////////////////////////////////////////////////////
int CApplications::GetNrOfBChannels(WK_ApplicationId waiExcept)
{
	CApplication* pApp;
	int i,c;

	c = 0;
	for (i=0;i<GetSize();i++)
	{
		pApp = GetAt(i);
		if (pApp && pApp->GetId() != waiExcept)
		{
			c+=pApp->GetNrOfBChannels();
		}
	}
	return c;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplications::IsFileForMicoUnit(const CString& exe)
{
	CString sMicoDir;
	CString sCompare;

	CApplication* pMico = GetApplication(WAI_MICOUNIT);

	if (pMico)
	{
		sMicoDir = pMico->GetWorkingDir();
		sMicoDir += _T('\\');
		sMicoDir.MakeLower();

		sCompare = sMicoDir + _T("mico.vxd");
		if (sCompare.CompareNoCase(exe)==0)
			return TRUE;

		if (-1!=exe.Find(_T("micopci.sys")))
			return TRUE;

		if (-1!=exe.Find(_T("micoda.dll")))
			return TRUE;

		if (-1!=exe.Find(_T("micopci.vxd")))
			return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CApplications::IsFileForCocoUnit(const CString& exe)
{
	CString sCocoDir;
	CString sCompare;

	CApplication* pCoco = GetApplication(WAI_COCOUNIT);

	if (pCoco==NULL)
	{
		return FALSE;
	}

	sCocoDir = pCoco->GetWorkingDir();
	sCocoDir += _T('\\');
	sCocoDir.MakeLower();

	sCompare = sCocoDir + _T("cocosrv.exe");
	if (sCompare.CompareNoCase(exe)==0)
		return TRUE;

	sCompare = sCocoDir + _T("cocounit.exe");
	if (sCompare.CompareNoCase(exe)==0)
		return TRUE;

	sCompare = sCocoDir + _T("avp3api.dll");
	if (sCompare.CompareNoCase(exe)==0)
		return TRUE;

	sCompare = sCocoDir + _T("avpalmpi.dll");
	if (sCompare.CompareNoCase(exe)==0)
		return TRUE;

	sCompare = sCocoDir + _T("avpaudpi.dll");
	if (sCompare.CompareNoCase(exe)==0)
		return TRUE;

	sCompare = sCocoDir + _T("avpcpi.dll");
	if (sCompare.CompareNoCase(exe)==0)
		return TRUE;

	sCompare = sCocoDir + _T("avpfrs.dll");
	if (sCompare.CompareNoCase(exe)==0)
		return TRUE;

	sCompare = sCocoDir + _T("avphdar.dll");
	if (sCompare.CompareNoCase(exe)==0)
		return TRUE;
	
	sCompare = sCocoDir + _T("avpscpi.dll");
	if (sCompare.CompareNoCase(exe)==0)
		return TRUE;
	
	sCompare = sCocoDir + _T("avpvidpi.dll");
	if (sCompare.CompareNoCase(exe)==0)
		return TRUE;

	sCompare = sCocoDir + _T("ccodec32.dll");
	if (sCompare.CompareNoCase(exe)==0)
		return TRUE;
	
	sCompare = sCocoDir + _T("convrt32.dll");
	if (sCompare.CompareNoCase(exe)==0)
		return TRUE;
	
	sCompare = sCocoDir + _T("mapper31.dll");
	if (sCompare.CompareNoCase(exe)==0)
		return TRUE;

	sCompare = sCocoDir + _T("megrav2.dll");
	if (sCompare.CompareNoCase(exe)==0)
		return TRUE;

	sCompare = sCocoDir + _T("pcidebug.dll");
	if (sCompare.CompareNoCase(exe)==0)
		return TRUE;
	
	sCompare = sCocoDir + _T("coco263.mcs");
	if (sCompare.CompareNoCase(exe)==0)
		return TRUE;

	sCompare = sCocoDir + _T("vconf263.drs");
	if (sCompare.CompareNoCase(exe)==0)
		return TRUE;

	return FALSE;
}
