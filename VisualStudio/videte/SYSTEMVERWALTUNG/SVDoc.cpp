// SVDoc.cpp : implementation of the CSVDoc class
//

#include "stdafx.h"
#include "SystemVerwaltung.h"

#include "SVDoc.h"
#include "SVView.h"
#include "SVPage.h"
#include "NTLoginPage.h"
#include "CIPCInputSV.h"
#include "oemgui\oemgui.h"
#include "oemgui\DlgLoginNew.h"

#include "RemoteDialog.h"
#include <io.h>
#include <stdio.h> 
#include <sys\stat.h>
#include "DrivePage.h"
#include "SDIDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
	#define  TRACE_PROFILE FALSE
#else
	#define  TRACE_PROFILE TRUE
#endif


/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CSVDoc, CDocument)

BEGIN_MESSAGE_MAP(CSVDoc, CDocument)
	//{{AFX_MSG_MAP(CSVDoc)
	ON_COMMAND(ID_CONNECT, OnConnect)
	ON_UPDATE_COMMAND_UI(ID_CONNECT, OnUpdateConnect)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_COMMAND(ID_DISCONNECT, OnDisconnect)
	ON_UPDATE_COMMAND_UI(ID_DISCONNECT, OnUpdateDisconnect)
	ON_COMMAND(ID_FILE_LOAD_REGISTRY, OnFileLoadRegistry)
	ON_UPDATE_COMMAND_UI(ID_FILE_LOAD_REGISTRY, OnUpdateFileLoadRegistry)
	ON_COMMAND(ID_FILE_SAVE_AND_CLOSE, OnFileSaveAndClose)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AND_CLOSE, OnUpdateFileSaveAndClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVDoc construction/destruction

CSVDoc::CSVDoc()
{
	m_pProfile = new CWK_Profile();
	m_pProfile->SetTrace(TRACE_PROFILE);
	m_sCoCoIni = _T("cocoisa.ini");
	m_pDongle = new CWK_Dongle();
	m_dwFlags = theApp.m_pPermission->GetFlags();
	m_idHost = SECID_LOCAL_HOST;
	m_nRemoteCodePage = 0;
	m_bIsLoadedFile = FALSE;
	
	// Falls eine Datei 'Trivial.pwl' vorhanden ist, deren Inhalt in
	// die Registry kopieren und anschließend die Datei löschen.
	CopyTrivialListToRegistry(_T("Trivial.pwl"));
}

CSVDoc::~CSVDoc()
{
	if (m_idHost == SECID_NO_ID)
	{
		if (!m_sRegistryFile.IsEmpty())
		{
			m_pProfile->DeleteSection(NULL);
			m_sRegistryFile.Empty();
		}
	}

	if (m_idHost != SECID_LOCAL_HOST)
	{
		m_pProfile->DeleteSection(NULL);
		if (!m_sRegistryFile.IsEmpty())
		{
			DeleteFile(m_sRegistryFile);
		}
		if (!m_sCoCoIni.IsEmpty())
		{
			DeleteFile(m_sCoCoIni);
		}
	}


	WK_DELETE(m_pProfile);
	WK_DELETE(m_pDongle);
}

/////////////////////////////////////////////////////////////////////////////
void CSVDoc::Load()
{
	CString s = theApp.m_pPermission->GetName();

	m_Permissions.DeleteAll();
	m_Permissions.Load(GetProfile());

	for (int i=0;i<m_Permissions.GetSize();i++)
	{
		if (0==s.CompareNoCase(m_Permissions.GetAt(i)->GetName()))
		{
			m_dwFlags = m_Permissions.GetAt(i)->GetFlags();
			break;
		}
	}

	m_Hosts.DeleteAll();
	m_Hosts.Load(GetProfile());

	m_Users.DeleteAll();
	m_Users.Load(GetProfile());

	m_Archivs.DeleteAll();
	m_Archivs.Load(GetProfile());

	if (IsLocal())
	{
		CArchivInfo* pArchivInfo = m_Archivs.GetArchivInfo(CSecID(SECID_GROUP_ARCHIVE,
																	LOCAL_SEARCH_RESULT_ARCHIV_NR));
		if (pArchivInfo==NULL)
		{
			CString sName;
			sName.LoadString(IDS_SEARCH_RESULT);
			pArchivInfo = new CArchivInfo();
			pArchivInfo->SetName(sName);
			pArchivInfo->SetArchivNr(LOCAL_SEARCH_RESULT_ARCHIV_NR);
			pArchivInfo->SetSizeMB(START_SIZE_MB);
			pArchivInfo->SetTyp(RING_ARCHIV);
			m_Archivs.Add(pArchivInfo);
			m_Archivs.Save(GetProfile());
		}
	}
	
	
	// CAVEAT order is important for style conversion
	// process before inputs/activation
	// ditto for archives
	m_Processes.DeleteAll();
	m_Processes.Load(GetProfile());

	// in and outputs delete itself
	m_Inputs.Load(GetProfile());
	m_Outputs.Load(GetProfile());
	m_Medias.Load(GetProfile());

	m_Timers.DeleteAll();
	CTime ct=CTime::GetCurrentTime();
	m_Timers.Load(ct,GetProfile(),TRUE);	// keep empty timers OOPS realy keep her
	m_Timers.SetAutoDelete(TRUE);

	m_Notifications.DeleteAll();
	m_Notifications.Load(GetProfile());
#ifdef _DEBUG
// Add a dummy system notification for test purposes
	int c = m_Notifications.GetSize();
	if (c == 0)
	{
		CNotificationMessage* pNM = m_Notifications.AddNotificationMessage();
		CString str;
		str.LoadString(IDS_SYSTEM);
		pNM->SetName(str);
		pNM->SetMessage(str);
	}
#endif

	LoadDatabaseFields();
	LoadSDIControls();

	if (IsLocal())
	{
		// check for first time generation
		// of html files for cameras
		// Referenzbilder
		CString sOutputHTML;
		sOutputHTML.Format(_T("%s\\%08lx\\output.htm"),
			CNotificationMessage::GetWWWRoot(),SECID_LOCAL_HOST);
		if (!DoesFileExist(sOutputHTML))
		{
			m_Outputs.Save(GetProfile(),TRUE);
		}
		LoadDrives();

		// HEDU 011098
		// one time save  new style is required
		// drops archives from process
		// write archive in Activations
		// hosts for calling process remain
		int iConverted = m_pProfile->GetInt(theApp.GetLauncherSection(), _T("ActivationsConverted"), 0);
		if (iConverted==0) 
		{
			WK_TRACE(_T("Writing new style activations/processMacros\n"));
			m_Inputs.Save(GetProfile(),TRUE);
			GetProfile().WriteInt(theApp.GetLauncherSection(), _T("ActivationsConverted"), 1);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::LoadDrives(BOOL bUpdate/*=FALSE*/)
{
	CDrivePage *pDrivePage = NULL;
	if (bUpdate)
	{
		CSVPage*pPage = GetView()->GetSVPage();
		if (pPage && pPage->IsKindOf(RUNTIME_CLASS(CDrivePage)))
		{
			pDrivePage = (CDrivePage*)pPage;
		}
	}

	if (pDrivePage)
	{
		pDrivePage->UpdateDrives();
	}
	else
	{
		m_Drives.Init(GetProfile());
	}
}
/*
void CSVDoc::SetATMDatabaseFields()
{
	CString s = szDontUse;
	CIPCField* pField = NULL;
	s.LoadString(IDS_DTP_DATE);
	m_DatabaseFields.AddNoDuplicates(new CIPCField(CIPCField::m_sfDate,s,8,'D'));
	s.LoadString(IDS_DTP_TIME);
	m_DatabaseFields.AddNoDuplicates(new CIPCField(CIPCField::m_sfTime,s,6,'T'));
	s.LoadString(IDS_GA_NR);
	m_DatabaseFields.AddNoDuplicates(new CIPCField(CIPCField::m_sfGaNr,s,6,'C'));
	s.LoadString(IDS_TA_NR);
	m_DatabaseFields.AddNoDuplicates(new CIPCField(CIPCField::m_sfTaNr,s,4,'C'));
	s.LoadString(IDS_KTO_NR);
	m_DatabaseFields.AddNoDuplicates(new CIPCField(CIPCField::m_sfKtNr,s,16,'C'));
	s.LoadString(IDS_BANKLEITZAHL);
	m_DatabaseFields.AddNoDuplicates(new CIPCField(CIPCField::m_sfBcNr,s,16,'C'));
	s.LoadString(IDS_VALUE);
	m_DatabaseFields.AddNoDuplicates(new CIPCField(CIPCField::m_sfAmnt,s,8,'C'));
	s.LoadString(IDS_CURRENCY);
	m_DatabaseFields.AddNoDuplicates(new CIPCField(CIPCField::m_sfCurr,s,3,'C'));
}*/
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::LoadDatabaseFields()
{
	m_DatabaseFields.SafeDeleteAll();
	m_DatabaseFields.Load(SECTION_DATABASE_FIELDS, GetProfile());
	// check for old style before 4.6.2
}
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::LoadSDIControls()
{
	m_SDIControls.RemoveAll();
	
	// check for COM Registry entries
	CString sSection,sComment;
	SDIControlType iType;
	DWORD dwCommPortMask = GetProfile().GetInt(SECTION_COMMON,COMMON_COMM_PORT_INFO,0);

	for (int iCom=1, dw=1; dw ; dw<<=1,iCom++) 
	{
		if (dw & dwCommPortMask) 
		{
			sSection.Format(szSectionFormat, iCom);
			iType = (SDIControlType)GetProfile().GetInt(sSection, szEntryType, SDICT_UNKNOWN);
			if (iType != SDICT_UNKNOWN)
			{
				m_SDIControls.Add(iType);
			}
		}
	}
	for (int iIP=1 ; ;iIP++) 
	{
		sSection.Format(szSectionUDP, iIP);
		iType = (SDIControlType)GetProfile().GetInt(sSection, szEntryType, SDICT_UNKNOWN);
		if (iType==SDICT_UNKNOWN) 
		{
			break;
		}
		else
		{
			m_SDIControls.Add(iType);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	Load();

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::OnCloseDocument()
{
	if (theApp.m_bUpdateLanguageDependencies)
	{
		int i, nCount;
		// HTML pages
		m_Inputs.GenerateHTMLFile();
		nCount = m_Inputs.GetSize();
		for (i=0; i<nCount; i++)
		{
			m_Inputs.GetGroupAt(i)->GenerateHTMLFile();
		}

		m_Outputs.GenerateHTMLFile();
		nCount = m_Outputs.GetSize();
		for (i=0; i<nCount; i++)
		{
			m_Outputs.GetGroupAt(i)->GenerateHTMLFile();
		}

		m_Medias.GenerateHTMLFile();
		nCount = m_Medias.GetSize();
		for (i=0; i<nCount; i++)
		{
			m_Medias.GetGroupAt(i)->GenerateHTMLFile();
		}
	}

	CDocument::OnCloseDocument();
}
/////////////////////////////////////////////////////////////////////////////
// CSVDoc serialization

void CSVDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSVDoc diagnostics

#ifdef _DEBUG
void CSVDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSVDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
CSVView* CSVDoc::GetView()
{
	CView* pView;
	POSITION pos = GetFirstViewPosition();
	if (pos) 
	{
		pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CSVView))) 
		{
			return (CSVView*)pView;
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CWK_Profile& CSVDoc::GetProfile()
{
	return *m_pProfile;
}
/////////////////////////////////////////////////////////////////////////////
CWK_Dongle* CSVDoc::GetDongle()
{
	return m_pDongle;
}
/////////////////////////////////////////////////////////////////////////////
// CSVDoc commands
BOOL CSVDoc::CanCloseFrame(CFrameWnd* pFrame) 
{
	CSVView* pView = GetView();

	if (pView)
	{
		if (IsLocal())
		{
			return pView->CanCloseFrame();
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::Connect(CSecID idHostID)
{
	GetView()->Clear();

	CRemoteDialog rdlg(idHostID,TRUE,AfxGetMainWnd());

	if (IDOK==rdlg.DoModal())
	{
		m_sHost = rdlg.m_sHost;
		WK_DELETE(m_pProfile);
		WK_DELETE(m_pDongle);
		m_idHost = idHostID;
		m_sRegistryFile = rdlg.m_sRegistryFile;
		m_sCoCoIni = rdlg.m_sCoCoISAini;
		m_nRemoteCodePage = rdlg.m_nRemoteCodePage;

		CString sTemp;
		int p;
        p = m_sRegistryFile.ReverseFind(_T('\\'));
		sTemp = m_sRegistryFile.Mid(p+1);
		p = sTemp.Find(_T('.'));
		sTemp = sTemp.Left(p);

		m_pProfile = new CWK_Profile(CWK_Profile::WKP_REGPATH,
									 HKEY_LOCAL_MACHINE,
									 sTemp,_T(""));
		m_pProfile->SetTrace(TRACE_PROFILE);

		UINT nOldCP = CWK_String::GetCodePage();
		CWK_String::SetCodePage((UINT)LOWORD(m_nRemoteCodePage));
		m_pProfile->SetCodePage((UINT)LOWORD(m_nRemoteCodePage));
		if (m_pProfile->Import(m_sRegistryFile))
		{
			CWK_String::SetCodePage(nOldCP);
			m_pProfile->SetCodePage(nOldCP);

			m_pDongle = new CWK_Dongle();
			m_pDongle->Load(*m_pProfile);
			Load();

			m_RemoteDrives.DeleteAll();
			CIPCDrives& drives = rdlg.GetDrives();
			for (int i=0;i<drives.GetSize();i++)
			{
				m_RemoteDrives.Add(new CIPCDrive(*drives.GetAt(i)));
				CancelChangesRemoteDrives();
			}

			CString s;
			s = theApp.m_pszAppName;
			s += _T(" - ") + m_sHost;
			GetView()->Clear();
			GetView()->InitTree();
			AfxGetMainWnd()->SetWindowText(s);
		}
		else
		{
			m_sRegistryFile.Empty();
			CWK_String::SetCodePage(nOldCP);
			ReLoadLocal();
		}
	}
	else
	{
		ReLoadLocal();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::OnConnect() 
{
	COEMConnectionDialog cdlg(m_Hosts,AfxGetMainWnd());

	cdlg.m_sFilterTypes = _T("MINI B3,MINI B6,SMS,FAX,E-Mail,TeleObserver");
	if (IDOK==cdlg.DoModal())
	{
		Connect(cdlg.m_idHost);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::OnUpdateConnect(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_sRegistryFile.IsEmpty()
				   && theApp.m_bRunAnyLinkUnit
				   && m_pDongle
				   && !m_pDongle->IsDemo()
				   && IsLocal()
				   && theApp.m_pPermission 
		           && (theApp.m_pPermission->GetFlags() & WK_ALLOW_SETTINGS)
				   && (theApp.m_pPermission->IsSpecialReceiver() == FALSE)
				   );
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVDoc::SaveChanges()
{
	BOOL bCanSave = TRUE;
	CSVView* pView = GetView();
	if (pView)
	{
//			bCanSave = pView->CanCloseFrame();
	  	CSVPage* pSVPage = pView->GetSVPage();
		if (pSVPage)
		{
			if (pSVPage->IsModified())
			{
				int r = AfxMessageBox(IDP_SAVE_CHANGES, MB_YESNOCANCEL);
				switch (r)
				{
					case IDYES:
						pSVPage->OnOK();
						bCanSave = TRUE;
						break;
					case IDNO:
						bCanSave = TRUE;
						break;
					case IDCANCEL:
					default:
						bCanSave = FALSE;
						break;
				}
			}
		}
	}
	return bCanSave;
}
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::OnFileSaveAs() 
{
	CString sOwnHostName = m_Hosts.GetLocalHostName();
	if (   sOwnHostName.IsEmpty()
		|| sOwnHostName == CHostArray::GetLocalHostNameStandard()
		)
	{
		AfxMessageBox(IDP_NO_LOCAL_HOSTNAME,MB_ICONSTOP);
	}
	else
	{
		if (SaveChanges())
		{
			COemFileDialog dlg(AfxGetMainWnd());
			CString sSave,sSettings;
			sSave.LoadString(IDS_SAVE);
			sSettings.LoadString(IDS_SETTINGS);
			dlg.SetProperties(FALSE,sSave,sSave);
			if (m_bIsLoadedFile)
			{
				CString sInitial = m_sRegistryFile;
				int nFind = sInitial.ReverseFind(_T('\\'));
				if (nFind != -1) sInitial = sInitial.Left(nFind);
				dlg.SetInitialDirectory(sInitial);
			}
			else
			{
				dlg.SetInitialDirectory(theApp.m_sOpenDir);
			}
			dlg.AddFilter(sSettings,_T("svs"));
			if (IDOK == dlg.DoModal())
			{
				CString sFileName = dlg.GetPathname();
				BeginWaitCursor();
				CNTLoginPage ntlp;
				ntlp.LoadSettings();

				m_pProfile->WriteString(REGKEY_WINLOGON, REGKEY_AUTOADMINLOGON, ntlp.m_bAutoLogin ? _T("1") : _T("0"));
				m_pProfile->WriteString(REGKEY_WINLOGON, REGKEY_DEFAULTUSERNAME, ntlp.m_sUser);
				m_pProfile->WriteString(REGKEY_WINLOGON, REGKEY_DEFAULTDOMAINNAME, ntlp.m_sDomain);
				m_pProfile->WriteString(REGKEY_WINLOGON, REGKEY_DEFAULTPASSWORD, ntlp.m_sPassword);

				m_pProfile->SetCodePage(CODEPAGE_UNICODE);
				m_pProfile->SaveAs(sFileName,_T("DVRT"));
				m_pProfile->SetCodePage(CWK_String::GetCodePage());
				
				m_pProfile->DeleteSection(REGKEY_WINLOGON);

				EndWaitCursor();
				if (m_bIsLoadedFile)
				{
					m_sRegistryFile = sFileName;
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   theApp.m_pPermission 
		           && (theApp.m_pPermission->GetFlags() & WK_ALLOW_SETTINGS)
				   );
}
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::OnFileOpen() 
{
	AfxMessageBox(IDP_OPEN_FILE);
	COemFileDialog dlg(AfxGetMainWnd());
	CString sOpen,sSettings;

	sOpen.LoadString(IDS_OPEN);
	sSettings.LoadString(IDS_SETTINGS);
	dlg.SetProperties(TRUE,sOpen,sOpen);
	dlg.SetInitialDirectory(theApp.m_sOpenDir);
	dlg.AddFilter(sSettings,_T("svs"));
	if (IDOK==dlg.DoModal())
	{
		CString sFileName = dlg.GetPathname();
		BeginWaitCursor();
		CWK_Profile temp(CWK_Profile::WKP_REGPATH,
						 HKEY_LOCAL_MACHINE,
						 _T("SVSTemp"),_T(""));
		temp.Import(sFileName);
		CHostArray ha;
		ha.Load(temp);
		temp.DeleteSection(NULL);
		BOOL bOK = TRUE;
		if (m_Hosts.GetLocalHostName() != ha.GetLocalHostName())
		{
			// Achtung, User warnen
			int iReturn = AfxMessageBox(IDP_REGISTRY_IMPORT_WARNING,
										MB_ICONSTOP|MB_YESNO|MB_DEFBUTTON2);
			if (IDYES != iReturn) 
			{
				bOK = FALSE;
			}
		}
		if (bOK) 
		{
			CString sTempReg = WK_GetTempFile(_T("REG"));

			m_pProfile->SaveAs(sTempReg, WK_PROFILE_ROOT);

			m_pProfile->DeleteSection(NULL);
			if (m_pProfile->Import(sFileName))
			{
				if (IsLocal())
				{
					WK_DELETE(m_pProfile);	// Schlüssel neu anlegen
					m_pProfile = new CWK_Profile();
					m_pProfile->SetTrace(TRACE_PROFILE);
				}
				CNTLoginPage ntlp;
				ntlp.m_bAutoLogin = m_pProfile->GetString(REGKEY_WINLOGON, REGKEY_AUTOADMINLOGON   ,_T("0")) == _T("1");
				ntlp.m_sUser      = m_pProfile->GetString(REGKEY_WINLOGON, REGKEY_DEFAULTUSERNAME  ,_T(""));
				ntlp.m_sDomain    = m_pProfile->GetString(REGKEY_WINLOGON, REGKEY_DEFAULTDOMAINNAME,_T(""));
				ntlp.m_sPassword  = m_pProfile->GetString(REGKEY_WINLOGON, REGKEY_DEFAULTPASSWORD  ,_T(""));
				m_pProfile->DeleteSection(REGKEY_WINLOGON);
				ntlp.SaveSettings();
				if (IsLocal())
				{
					BOOL bVersion4_7 = FALSE;
					BOOL bOldROmode		= CWK_Profile::SetReadOnlyMode(TRUE);
					HKEY hKey			= m_pProfile->GetSectionKey(REG_KEY_VISION_SETTINGS);
					if (hKey)					// 4.7 settings imported?
					{
						::RegCloseKey(hKey);
						bVersion4_7 = TRUE;
					}
					CWK_Profile::SetReadOnlyMode(bOldROmode);
					if (bVersion4_7)
					{
						m_pProfile->WriteString(theApp.GetModuleSection(), WK_APP_NAME_IDIP_CLIENT, WK_APP_NAME_IDIP_CLIENT _T(".exe"));
					}
				}

				CSVView* pView = GetView();
				
				Load();
				pView->Clear();					// Update this application
				pView->InitTree();

				if (IsLocal())
				{
					pView->m_bServerInit = TRUE;// update system
					pView->AddInitApp(WK_APP_NAME_IDIP_CLIENT);
					theApp.StartApplication(WAI_IDIP_CLIENT, LAUNCHER_RELOAD_SETTINGS);
					pView->AddInitApp(WK_APP_NAME_DB_SERVER);
					if (!m_pProfile->GetString(theApp.GetModuleSection(), WK_APPNAME_EMAILUNIT, NULL).IsEmpty())
					{
						pView->AddInitApp(WK_APPNAME_EMAILUNIT);
					}
					if (!m_pProfile->GetString(theApp.GetModuleSection(), WK_APPNAME_GEMOSUNIT, NULL).IsEmpty())
					{
						pView->AddInitApp(WK_APPNAME_GEMOSUNIT);
					}
					if (!m_pProfile->GetString(theApp.GetModuleSection(), WK_APP_NAME_ISDN, NULL).IsEmpty())
					{
						pView->AddInitApp(WK_APP_NAME_ISDN);
					}
					if (!m_pProfile->GetString(theApp.GetModuleSection(), WK_APP_NAME_PTUNIT, NULL).IsEmpty())
					{
						pView->AddInitApp(WK_APP_NAME_PTUNIT);
					}
					if (!m_pProfile->GetString(theApp.GetModuleSection(), WK_APPNAME_SMSUNIT, NULL).IsEmpty())
					{
						pView->AddInitApp(WK_APPNAME_SMSUNIT);
					}
					if (!m_pProfile->GetString(theApp.GetModuleSection(), WK_APP_NAME_SOCKETS, NULL).IsEmpty())
					{
						pView->AddInitApp(WK_APP_NAME_SOCKETS);
					}
					if (!m_pProfile->GetString(theApp.GetModuleSection(), WK_APPNAME_TOBSUNIT, NULL).IsEmpty())
					{
						pView->AddInitApp(WK_APPNAME_TOBSUNIT);
					}
					CString sInitApps;
					int i=0;
					for (i=0; i<m_saInitApps.GetCount(); i++)
					{
						sInitApps += m_saInitApps.GetAt(i);
						if (i<m_saInitApps.GetCount()-1)
						{
							sInitApps += _T(", ");
						}
					}
					WK_TRACE(_T("Import of %s successfull, initialising %s\n"), sFileName, sInitApps);
				}
			}
			else if (IsLocal())
			{
				CString sExecute = _T("wait:regedt32.exe /s ");		// mit Regedt32 /s = Silent
				sExecute += sTempReg;
				ExecuteProgram(sExecute, theApp.m_sApplicationPath);// importieren
				WK_TRACE(_T("Import of %s failed, loading old settings saved in %s\n"), sFileName, sTempReg);
			}
			DeleteFile(sTempReg);
		}

		EndWaitCursor();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::OnUpdateFileOpen(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(theApp.m_pPermission 
		           && (theApp.m_pPermission->GetFlags() & WK_ALLOW_SETTINGS)
				   && !m_bIsLoadedFile
				   && m_sRegistryFile.IsEmpty()
				   && IsLocal()
				   );
}
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::OnFileLoadRegistry()
{
	COemFileDialog dlg(AfxGetMainWnd());
	CString sOpen,sSettings;

	sOpen.LoadString(IDS_OPEN);
	sSettings.LoadString(IDS_SETTINGS);
	dlg.SetProperties(TRUE,sOpen,sOpen);
	dlg.SetInitialDirectory(theApp.m_sOpenDir);
	dlg.AddFilter(sSettings,_T("svs"));
	if (IDOK==dlg.DoModal())
	{
		WK_DELETE(m_pProfile);
		WK_DELETE(m_pDongle);
		m_pProfile = new CWK_Profile(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, _T("SVSTemp"),_T(""));
		m_sRegistryFile = dlg.GetPathname();
		m_bIsLoadedFile = TRUE;
		BeginWaitCursor();
		if (m_pProfile->Import(m_sRegistryFile))
		{
			m_pDongle = new CWK_Dongle();
			m_pDongle->Load(*m_pProfile);
			Load();
			EndWaitCursor();
			m_idHost = SECID_NO_ID;
			CSVView* pView = GetView();
			pView->Clear();					// Update this application
			pView->InitTree();
			AfxGetMainWnd()->SetWindowText(CString(theApp.m_pszAppName) + _T(" - ") + m_sRegistryFile);
		}
		else
		{
			m_sRegistryFile.Empty();
			m_bIsLoadedFile = FALSE;
			ReLoadLocal();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::OnUpdateFileLoadRegistry(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_sRegistryFile.IsEmpty() && IsLocal());
}
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::OnFileSaveAndClose()
{
	if (!m_sRegistryFile.IsEmpty())
	{
		if (SaveChanges())
		{
			BeginWaitCursor();
			CNTLoginPage ntlp;
			ntlp.LoadSettings();

			m_pProfile->WriteString(REGKEY_WINLOGON, REGKEY_AUTOADMINLOGON, ntlp.m_bAutoLogin ? _T("1") : _T("0"));
			m_pProfile->WriteString(REGKEY_WINLOGON, REGKEY_DEFAULTUSERNAME, ntlp.m_sUser);
			m_pProfile->WriteString(REGKEY_WINLOGON, REGKEY_DEFAULTDOMAINNAME, ntlp.m_sDomain);
			m_pProfile->WriteString(REGKEY_WINLOGON, REGKEY_DEFAULTPASSWORD, ntlp.m_sPassword);

			m_pProfile->SetCodePage(CODEPAGE_UNICODE);
			m_pProfile->SaveAs(m_sRegistryFile,_T("DVRT"));
			m_pProfile->SetCodePage(CWK_String::GetCodePage());
			
			m_pProfile->DeleteSection(NULL);

			m_sRegistryFile.Empty();
			m_bIsLoadedFile = FALSE;

			ReLoadLocal();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::OnUpdateFileSaveAndClose(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_sRegistryFile.IsEmpty() && m_bIsLoadedFile);
}
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::OnDisconnect() 
{
	if (GetView()->CanCloseFrame(FALSE))
	{
		CRemoteDialog rdlg(m_idHost,FALSE,AfxGetMainWnd());

		CString sCommand;

		sCommand = _T("ImportRegistry ");

		for (int i=0;i<m_saInitApps.GetSize();i++)
		{
			sCommand += m_saInitApps.GetAt(i);
			if (i<m_saInitApps.GetSize()-1)
			{
				sCommand += _T(',');
			}
		}

		rdlg.m_sCommand = sCommand;
		rdlg.m_sRegistryFile = m_sRegistryFile;
		rdlg.m_nRemoteCodePage = m_nRemoteCodePage;
		UINT nOldCP = CWK_String::GetCodePage();
		CWK_String::SetCodePage((UINT)LOWORD(m_nRemoteCodePage));
		m_pProfile->SetCodePage((UINT)LOWORD(m_nRemoteCodePage));
		m_pProfile->SaveAs(m_sRegistryFile,_T("DVRT"));
		CWK_String::SetCodePage(nOldCP);
		m_pProfile->SetCodePage(nOldCP);

		rdlg.m_sCoCoISAini = m_sCoCoIni;

		if (IDOK==rdlg.DoModal())
		{
			WK_TRACE(_T("remote sending registry OK\n"));
		}
		else
		{
			WK_TRACE(_T("remote sending registry cancelled\n"));
		}

		m_pProfile->DeleteSection(NULL);

		DWORD dwCPBits = COemGuiApi::GetCodePageBits();
		if (m_nRemoteCodePage & INPUT_CPF_GLOBAL_FONTS_CHANGED)
		{
			if (m_nRemoteCodePage & INPUT_CPF_USES_GLOBAL_FONTS)
			{
				if (dwCPBits)
				{
					SetFontFaceNamesFromCodePageBits(dwCPBits);
				}				
			}
			else
			{
				CSkinDialog::DoUseGlobalFonts(FALSE);
			}
		}

		if (m_nRemoteCodePage & INPUT_CPF_KB_LAYOUT_CREATED)
		{
			theApp.CreateKeyboardLayouts(KBLANGID_LOCALE, COemGuiApi::GetLocaleID());
		}
		else if (m_nRemoteCodePage & INPUT_CPF_KB_LAYOUT_CHANGED && dwCPBits)
		{
			theApp.CreateKeyboardLayouts(KBLANGID_ENU, COemGuiApi::GetLocaleID());
		}

		ReLoadLocal();
		// clean up
		m_RemoteDrives.SafeDeleteAll();
		if (DeleteFile(m_sRegistryFile))
		{
			m_sRegistryFile.Empty();
			m_nRemoteCodePage = 0;
		}
		else
		{
			WK_TRACE_ERROR(_T("cannot delete temp registry file %s %s\n"),
				m_sRegistryFile,GetLastErrorString());
		}
		if (!m_sCoCoIni.IsEmpty())
		{
			if (DeleteFile(m_sCoCoIni))
			{
				m_sCoCoIni.Empty();
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot delete coco isa file %s %s\n"),
					m_sCoCoIni,GetLastErrorString());
			}
		}
		m_sCoCoIni = _T("cocoisa.ini");

		m_nRemoteCodePage = 0;
	}
}
void CSVDoc::ReLoadLocal()
{
	m_idHost = SECID_LOCAL_HOST;
	WK_DELETE(m_pProfile);
	WK_DELETE(m_pDongle);
	m_pProfile = new CWK_Profile();
	m_pProfile->SetTrace(TRACE_PROFILE);
	m_pDongle = new CWK_Dongle();
	Load();
	GetView()->Clear();
	GetView()->InitTree();
	AfxGetMainWnd()->SetWindowText(theApp.m_pszAppName);
}
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::OnUpdateDisconnect(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(theApp.m_bRunAnyLinkUnit && !IsLocal() && m_idHost != SECID_NO_ID);
}
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::AddInitApp(const CString& sAppname)
{
	int i,c;

	c = m_saInitApps.GetSize();

	for (i=0;i<c;i++)
	{
		if (sAppname == m_saInitApps.GetAt(i))
		{
			// already in array 
			return;
		}
	}

	m_saInitApps.Add(sAppname);
}
///////////////////////////////////////////////////////////////
void CSVDoc::InitApps()
{
	if ( theApp.m_pUser ) 
	{
		int i,c;
		CString sApp;

		c = m_saInitApps.GetSize();

		for (i=c-1;i>=0;i--)
		{
			sApp = m_saInitApps.GetAt(i);
			if (!sApp.IsEmpty())
			{
				if (WK_IS_RUNNING(sApp))
				{
					CConnectionRecord c(sApp,
										*theApp.m_pUser,
										theApp.m_pPermission->GetName(),
										theApp.m_pPermission->GetPassword()
										);
					c.SetTimeout(5*1000);
					if (CIPCServerControlClientSide::FetchServerReset(c)) 
					{
						m_saInitApps.RemoveAt(i);
					}
				}
				else
				{
					m_saInitApps.RemoveAt(i);
				}
			}
		}
	}
}
///////////////////////////////////////////////////////////////
void CSVDoc::InitServer()
{
	if ( theApp.m_pUser ) 
	{
		CConnectionRecord c(LOCAL_LOOP_BACK,
							*theApp.m_pUser,
							theApp.m_pPermission->GetName(),
							theApp.m_pPermission->GetPassword()
							);
		if (CIPCServerControlClientSide::FetchServerReset(c)) 
		{
			GetView()->m_bServerInit = FALSE;
			return;
		}
	}
	AfxMessageBox(IDS_RESET_FAILED, MB_OK); 
}
///////////////////////////////////////////////////////////////
DWORD CSVDoc::GetCommPortInfo()
{
//	if (IsLocal())
//	{
//		DWORD dwInfo = ::GetCommPortInfo();
//		GetProfile().WriteInt(SECTION_COMMON,COMMON_COMM_PORT_INFO,dwInfo);
//		return dwInfo;
//	}
//	else
//	{
		return GetProfile().GetInt(SECTION_COMMON, COMMON_COMM_PORT_INFO,0);
//	}
}

/////////////////////////////////////////////////////////////////////////////////
BOOL CSVDoc::CopyTrivialListToRegistry(const CString &sFile)
{
	CString sWinDir, sBuffer;
	CString sFilePath		= _T("");
	CString sLineNr			= _T("");
	CStdioFile fileRead;
	int	nI					= 0;

	GetWindowsDirectory(sWinDir.GetBuffer(MAX_PATH), MAX_PATH);
	sWinDir.ReleaseBuffer();
	sFilePath.Format(_T("%s\\%s"), sWinDir, sFile);

	if (fileRead.Open(sFilePath, CFile::modeRead|CFile::shareExclusive) == FALSE)
	{
		return FALSE;
	}
	
	GetProfile().DeleteSection(_T("TPWL"));
	while(fileRead.ReadString(sBuffer))
	{
		sLineNr.Format(_T("%05d"), nI++);
		GetProfile().WriteString(_T("TPWL"), sLineNr, sBuffer);
	}

	fileRead.Close();

	// Schreibschutz entfernen
	CFileStatus fileStatus;
	CFile::GetStatus(sFilePath, fileStatus);
	fileStatus.m_attribute = CFile::normal;
	TRY
	{
		CFile::SetStatus(sFilePath, fileStatus);
	}
	WK_CATCH(_T("cannot set status\n"));

	// Datei löschen.
	TRY
	{
		CFile::Remove(sFilePath);
	}
	WK_CATCH(_T("cannot delete file\n"));

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::OnIdle()
{
	CSVView* pView = GetView();
	if (pView)
	{
		pView->OnIdleSVView();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::CancelChangesRemoteDrives()
{
	m_Drives.SafeDeleteAll();
	for (int i=0 ; i<m_RemoteDrives.GetSize() ; i++)
	{
		m_Drives.Add(new CIPCDrive(*m_RemoteDrives.GetAt(i)));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::SaveChangesRemoteDrives()
{
	m_RemoteDrives.SafeDeleteAll();
	for (int i=0 ; i<m_Drives.GetSize() ; i++)
	{
		m_RemoteDrives.Add(new CIPCDrive(*m_Drives.GetAt(i)));
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVDoc::CheckActivationsWithProcess(CSecID id)
{
	// first check wether there is any Activation (CInputToOutput) with or 
	// process macro
	CInputGroup* pInputGroup;
	CInput* pInput;
	CInputToOutput* pInputToOutput;
	int i,j,k,c,d,e;
	BOOL bShowMB = TRUE;

	c = m_Inputs.GetSize();
	for (i=0;i<c;i++)
	{
		pInputGroup = m_Inputs.GetGroupAt(i);
		d = pInputGroup->GetSize();
		for (j=0;j<d;j++)
		{
			pInput = pInputGroup->GetInput(j);
			e = pInput->GetNumberOfInputToOutputs();
			for (k=e-1;k>=0;k--)
			{
				pInputToOutput = pInput->GetInputToOutput(k);
				if (pInputToOutput->GetProcessID() == id)
				{
					// we still have one
					if (bShowMB)
					{
						if (IDYES==AfxMessageBox(IDP_DELETE_PROCESS_WITH_ACTIVATION,MB_YESNO))
						{
							bShowMB = FALSE;
						}
						else
						{
							return FALSE;
						}
					}
					pInput->DeleteInputToOutput(pInputToOutput);
				}
			} // inputstooutputs
		} // inputs
	}//groups
	if (bShowMB==FALSE)
	{
		m_Inputs.Save(GetProfile(),IsLocal());
	}
	return TRUE;
}

#ifdef _UNICODE
bool CSVDoc::UseBinaryFormat()
{
	return m_nRemoteCodePage & INPUT_CPF_USE_STRING_FORMAT ? false: true;
}
#endif
/////////////////////////////////////////////////////////////////////////////
CInputToOutput* CSVDoc::CreateMDRecording(CSecID idCam)
{
	CInputToOutput*  pReturn = NULL;

	COutputGroup* pOutputGroup = m_Outputs.GetGroupByID(idCam);
	const COutput* pOutput = m_Outputs.GetOutputByID(idCam);

	int iNrPict = 2;
	CInputGroup* pMDInputGroup = NULL;
	CInput* pMDInput = NULL;
	CProcess* pMDProcess = NULL;

	// 1. Aufzeichnung mit MD für die Kamera
	// 1.1 passender MD Melder an
	if (pOutputGroup)
	{
		CString shmName = pOutputGroup->GetSMName();
		shmName.Replace(_T("OutputCamera"),_T("MDInput"));
		pMDInputGroup = m_Inputs.GetGroupBySMName(shmName);
		if (pMDInputGroup)
		{
			pMDInput = pMDInputGroup->GetInput(idCam.GetSubID());
		}
		if (-1!=pOutputGroup->GetSMName().Find(_T("TashaUnit")))
		{
			iNrPict = 25;
		}
		else if (-1!=pOutputGroup->GetSMName().Find(_T("QUnit")))
		{
			iNrPict = 5;
		}
	}
	if (pMDInput)
	{
		if (pMDInput->GetIDActivate() == SECID_ACTIVE_OFF)
		{
			WK_TRACE(_T("Designa Wizard set MD Detector on %s\n"),pOutput->GetName());
			pMDInput->SetIDActivate(SECID_ACTIVE_ON);
			pMDInput->SetName(pOutput->GetName() +_T(" MD"));
		}
	}
	// 1.2 passendes ProcessMacro fuer MD
	for (int i=0;i<m_Processes.GetSize();i++)
	{
		CProcess* pProcess = m_Processes.GetAtFast(i);
		if (pProcess->GetType() == CProcess::P_SAVE)
		{
			if (   pProcess->IsHold()
				&& pProcess->IsInterval()
				&& pProcess->GetRecordTime()==1000
				&& pProcess->GetDuration()>=2000
				&& pProcess->GetNrPict()==iNrPict)
			{
				TRACE(_T("PROCESS found %s\n"),pProcess->GetName());
				pMDProcess = pProcess;
				break;
			}
		}
	}
	if (pMDProcess == NULL)
	{
		pMDProcess = m_Processes.AddProcess();
		CString s;
		int iMainTime = 2;
		s.Format(_T("%d fps %d s"),iNrPict,iMainTime);
		pMDProcess->SetSave(s,1000*iMainTime,0,
			(WORD)iNrPict,1000,RESOLUTION_2CIF,COMPRESSION_1,TRUE,TRUE);
		WK_TRACE(_T("Designa Wizard set MD Detector on %s\n"),pOutput->GetName());
	}
	// 1.3 passende Aktivierung suchen!

	BOOL bFound = FALSE;

	for (int i=0;i<pMDInput->GetNumberOfInputToOutputs()&&!bFound;i++)
	{
		CInputToOutput* pActivation = pMDInput->GetInputToOutput(i);
		if (pActivation)
		{
			CSecID idProcess = pActivation->GetProcessID();
			CProcess* pProcess = m_Processes.GetProcess(idProcess);
			if (   pProcess
				&& pProcess->GetType() == CProcess::P_SAVE)
			{
				// schon ein MD Speicherprozess da!
				pReturn = pActivation;
				bFound = TRUE;
			}
		}
	}
	if (!bFound)
	{
		// keine Aktivierung
		// Archiv anlegen
		CSecID idArchive;

		for (int i=0;i<m_Archivs.GetSize();i++)
		{
			CArchivInfo* pArchive = m_Archivs.GetAtFast(i);
			if (   pArchive
				&& pArchive->GetName() == pOutput->GetName())
			{
				// archive schon da
				idArchive = pArchive->GetID();
				break;
			}
		}
		if (!idArchive.IsArchiveID())
		{
			// archive anlegen
			CArchivInfo* pArchiveInfo = m_Archivs.AddArchivInfo();
			pArchiveInfo->SetName(pOutput->GetName());
			pArchiveInfo->SetTyp(RING_ARCHIV);
			idArchive = pArchiveInfo->GetID();
		}
		
		CInputToOutput* pActivation = pMDInput->AddInputToOutput();
		pActivation->SetOutputID(idCam);
		pActivation->SetInputState(IO_ALARM);
		pActivation->SetProcessID(pMDProcess->GetID());
		pActivation->SetArchiveID(idArchive);

		pReturn = pActivation;
	}

	return pReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVDoc::CreateDesigna(SDIControlType sdict,
						   CSecID idCam, DWORD dwBFR, DWORD dwTCC,
						   const CDWordArray& dwAlarms,const CDWordArray& dwXMLs)
{
	BOOL bRet = FALSE;
	if (   idCam.IsOutputID()
		&& dwBFR>0
		&& dwTCC>0
		&& dwAlarms.GetSize()+dwXMLs.GetSize()>0)
	{
		CInputToOutput* pActivation = NULL;

		// 1. MD Recording for the camera!
		pActivation = CreateMDRecording(idCam);

		// 2. Alarmliste anlegen!
		CSecID idAlarmlist = CreateAlarmList();

		CProcess* p1fpsProcess = NULL;
		// passendes ProcessMacro fuer 1fps
		for (int i=0;i<m_Processes.GetSize();i++)
		{
			CProcess* pProcess = m_Processes.GetAtFast(i);
			if (pProcess->GetType() == CProcess::P_SAVE)
			{
				if (   pProcess->IsHold()
					&& !pProcess->IsInterval()
					&& pProcess->GetRecordTime()==1000
					&& pProcess->GetNrPict()==1)
				{
					p1fpsProcess = pProcess;
					break;
				}
			}
		}
		if (p1fpsProcess == NULL)
		{
			p1fpsProcess = m_Processes.AddProcess();
			CString s;
			s.Format(_T("1 fps"));
			p1fpsProcess->SetSave(s,0,0,(WORD)1,1000,RESOLUTION_2CIF,COMPRESSION_1,TRUE,FALSE);
			WK_TRACE(_T("Designa Wizard add save process %s\n"),p1fpsProcess->GetName());
		}

		// 3. Designa PM 100 Melder anlegen
		// und je Melder 2 Aktivierungen für die Alarmliste!
		CreateSDIControls(sdict,idCam,dwBFR,dwTCC, dwAlarms,dwXMLs,
			pActivation->GetArchiveID(),idAlarmlist,p1fpsProcess->GetID());
		// 
		bRet = TRUE;
	}
	else
	{
		TRACE(_T("illegal parameters for CreateDesigna\n"));
	}

	if (bRet)
	{
		m_Outputs.Save(GetProfile(),FALSE);
		m_Inputs.Save(GetProfile(),FALSE);
		m_Processes.Save(GetProfile());
		m_Archivs.Save(GetProfile());
		m_DatabaseFields.Save(SECTION_DATABASE_FIELDS,GetProfile());
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
CSecID CSVDoc::CreateAlarmList()
{
	CSecID idArchive;

	for (int i=0;i<m_Archivs.GetSize();i++)
	{
		CArchivInfo* pArchive = m_Archivs.GetAtFast(i);
		if (   pArchive
			&& pArchive->GetType() == ALARM_LIST_ARCHIV)
		{
			// archive schon da
			idArchive = pArchive->GetID();
			break;
		}
	}
	if (!idArchive.IsArchiveID())
	{
		// archive anlegen
		CArchivInfo* pArchiveInfo = m_Archivs.AddArchivInfo();
		CString s;
		s.LoadString(IDS_ALARM_LIST);
		pArchiveInfo->SetName(s);
		pArchiveInfo->SetTyp(ALARM_LIST_ARCHIV);
		idArchive = pArchiveInfo->GetID();
		WK_TRACE(_T("Designa Wizard add alarm list archive %s\n"),pArchiveInfo->GetName());
		m_Archivs.Save(GetProfile());
	}

	return idArchive;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVDoc::CreateSDIControls(SDIControlType sdict,
							   CSecID idCam,
							   DWORD dwBFR, 
							   DWORD dwTCC,
							   const CDWordArray& dwAlarms,
							   const CDWordArray& dwXMLs,
							   CSecID idRecording,
							   CSecID idAlarmList,
							   CSecID idSaveProcess)
{
	BOOL bRet = FALSE;

	int nNoOfUDP = GetProfile().GetInt(szSectionSDI, szEntryNoOfUDP, 0);
	int iPM100AlarmIndex = 0;
	int iPM100XMLIndex = 0;
	int iPMAbacusIndex = 0;
	CString sSection;

	for (int i=1;i<=nNoOfUDP;i++)
	{
		sSection.Format(_T("%s\\UDP%d"),szSectionSDI,i);
		int iType = GetProfile().GetInt(sSection, szEntryType, SDICT_UNKNOWN);

		if (iType == SDICT_DESIGNA_PM_100_ALARM)
		{
			// already exists PM 100 Alarm
			iPM100AlarmIndex = i;
		}
		else if (iType == SDICT_DESIGNA_PM_ABACUS)
		{
			// already exists PM 100 Alarm
			iPMAbacusIndex = i;
		}
		else if (iType == SDICT_DESIGNA_PM_100_XML)
		{
			// already exists PM 100 XML
			iPM100XMLIndex = i;
		}
	}

	if (dwAlarms.GetSize()>0)
	{
		BOOL bNew = FALSE;

		if (sdict == SDICT_DESIGNA_PM_100_ALARM)
		{
			if (iPM100AlarmIndex == 0)
			{
				// create SDI Control with Designa PM 100 Alarm
				nNoOfUDP++;
				iPM100AlarmIndex = nNoOfUDP;
				sSection.Format(_T("%s\\UDP%d"),szSectionSDI,iPM100AlarmIndex);
				GetProfile().WriteInt(sSection,szEntryType,SDICT_DESIGNA_PM_100_ALARM);
				GetProfile().WriteInt(sSection,szEntryPort,2100);
				GetProfile().WriteString(sSection,szEntryComment,CSDIControl::UIStringOfEnum(SDICT_DESIGNA_PM_100_ALARM));
				WK_TRACE(_T("Designa Wizard add PM 100 Alarm Protocol \n"));
				bNew = TRUE;
			}
			sSection.Format(_T("%s\\UDP%d"),szSectionSDI,iPM100AlarmIndex);
			CreateDesignaAlarms(idCam,dwBFR,dwTCC,dwAlarms,sSection,SDICT_DESIGNA_PM_100_ALARM,2100,idRecording,idAlarmList,idSaveProcess,bNew);
			// exitiert die Inputgruppe schon?
		}
		else if (sdict == SDICT_DESIGNA_PM_ABACUS)
		{
			if (iPMAbacusIndex == 0)
			{
				// create SDI Control with Designa PM 100 Alarm
				nNoOfUDP++;
				iPMAbacusIndex = nNoOfUDP;
				sSection.Format(_T("%s\\UDP%d"),szSectionSDI,iPMAbacusIndex);
				GetProfile().WriteInt(sSection,szEntryType,SDICT_DESIGNA_PM_ABACUS);
				GetProfile().WriteInt(sSection,szEntryPort,10500);
				GetProfile().WriteString(sSection,szEntryIPaddress,_T("224.0.1.41"));
				GetProfile().WriteString(sSection,szEntryComment,CSDIControl::UIStringOfEnum(SDICT_DESIGNA_PM_ABACUS));
				WK_TRACE(_T("Designa Wizard add PM Abacus Alarm Protocol \n"));
				bNew = TRUE;
			}
			sSection.Format(_T("%s\\UDP%d"),szSectionSDI,iPMAbacusIndex);
			CreateDesignaAlarms(idCam,dwBFR,dwTCC,dwAlarms,sSection,SDICT_DESIGNA_PM_ABACUS,10500,idRecording,idAlarmList,idSaveProcess,bNew);
			// exitiert die Inputgruppe schon?
		}
	}
	if (dwXMLs.GetSize()>0)
	{
		BOOL bNew = FALSE;
		if (sdict == SDICT_DESIGNA_PM_100_ALARM)
		{
			if (iPM100XMLIndex == 0)
			{
				// create SDI Control with Designa PM 100 Alarm
				nNoOfUDP++;
				bNew = TRUE;
				iPM100XMLIndex = nNoOfUDP;
				sSection.Format(_T("%s\\UDP%d"),szSectionSDI,iPM100XMLIndex);
				GetProfile().WriteInt(sSection,szEntryType,SDICT_DESIGNA_PM_100_XML);
				GetProfile().WriteInt(sSection,szEntryPort,2140);
				GetProfile().WriteString(sSection,szEntryComment,CSDIControl::UIStringOfEnum(SDICT_DESIGNA_PM_100_XML));
				WK_TRACE(_T("Designa Wizard add PM 100 XML Protocol \n"));
			}
			sSection.Format(_T("%s\\UDP%d"),szSectionSDI,iPM100XMLIndex);
			CreateDesignaAlarms(idCam,dwBFR,dwTCC,dwXMLs,sSection,
				SDICT_DESIGNA_PM_100_XML,2140,idRecording,idAlarmList,idSaveProcess,bNew);
		}
	}
	GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_SDIUNIT,_T("SDIUnit.exe"));
	GetProfile().WriteInt(szSectionSDI, szEntryNoOfUDP, nNoOfUDP);

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVDoc::CreateDesignaAlarms(CSecID idCam,
							   DWORD dwBFR, 
							   DWORD dwTCC,
							   const CDWordArray& dwAlarms,
							   const CString& sSection,
							   SDIControlType ct,
							   int iPortNr,
							   CSecID idRecording,
							   CSecID idAlarmList,
							   CSecID idSaveProcess,
							   BOOL bNew)
{
	CString sBFR,sTCC;

	sBFR.Format(_T("%d"),dwBFR);
	sTCC.Format(_T("%d"),dwTCC);
	// passende alarme aus dwAlarms erzeugen!
	CSDIAlarmNumbers sdiAlarmNumbers;
	sdiAlarmNumbers.Load(GetProfile(),sSection);
	for (int i=0;i<dwAlarms.GetSize();i++)
	{
		DWORD dwAlarm = dwAlarms[i];
		BOOL bFound = FALSE;

		for (int j=0;j<sdiAlarmNumbers.GetSize()&&!bFound;j++)
		{
			bFound =    sdiAlarmNumbers.GetBFR(j) == (int)dwBFR
				&& sdiAlarmNumbers.GetTCC(j) == (int)dwTCC
				&& sdiAlarmNumbers.GetAlarmNr(j) == (int)dwAlarm;
		}
		if (!bFound)
		{
			CString sNr,sAlarm,sComment;
			sNr.Format(_T("%d"),sdiAlarmNumbers.GetSize()+1);
			sAlarm.Format(_T("%d"),dwAlarm);

			if (ct == SDICT_DESIGNA_PM_100_ALARM)
			{
				sComment = CSDIControl::GetPM100AlarmDescription(dwAlarm);
			}
			else if (ct == SDICT_DESIGNA_PM_ABACUS)
			{
				sComment = CSDIControl::GetPMAbacusDescription(dwAlarm);
			}
			else if (ct == SDICT_DESIGNA_PM_100_XML)
			{
				sComment = CSDIControl::GetPM100XMLDescription(dwAlarm);
			}
			
			sComment += _T(" BFR ")+sBFR+_T(" TCC ")+sTCC;
			WK_TRACE(_T("Designa Wizard add PM alarm %s\n"),sComment);
			sdiAlarmNumbers.AddDesignaAlarm(sNr,sBFR,sTCC,sAlarm,_T(""),sComment);
		}
	}
	sdiAlarmNumbers.Save(GetProfile(),sSection);

	CString sSMName;
	sSMName.Format(_T("%sUDP%d"), SM_SDIUNIT_INPUT, iPortNr);
	CInputGroup* pInputGroup = m_Inputs.GetGroupBySMName(sSMName);
	CString sGroupName;
	if (pInputGroup)
	{
		sGroupName = pInputGroup->GetName();
	}
	else
	{
		sGroupName = CSDIControl::UIStringOfEnum(ct,TRUE);
	}
	
	
	SDIControlType eInitial = ct;
	if (bNew)
	{
		eInitial = SDICT_UNKNOWN;
	}

	pInputGroup = CSDIDialog::SaveChangesInputGroup(eInitial,ct,
		&m_Inputs,&m_DatabaseFields,sdiAlarmNumbers,NULL,sSMName,
		sGroupName,sGroupName,sdiAlarmNumbers.GetSize());

	if (pInputGroup)
	{
		for (int i=0;i<pInputGroup->GetSize();i++)
		{
			CInput* pInput = pInputGroup->GetInput(i);

			if (   pInput->GetExtra(0) == dwBFR
				&& pInput->GetExtra(1) == dwTCC)
			{
				if (pInput->GetIDActivate() == SECID_ACTIVE_OFF)
				{
					WK_TRACE(_T("Designa Wizard set Detector on %s\n"),pInput->GetName());
					pInput->SetIDActivate(SECID_ACTIVE_ON);
				}

				// create the 2 activations
				CInputToOutput* pActivation = NULL;
				BOOL bFound = FALSE;
				for (int j=0;j<pInput->GetNumberOfInputToOutputs()&&!bFound;j++)
				{
					pActivation = pInput->GetInputToOutput(j);
					if (   pActivation->GetProcessID() == SECID_PROCESS_ALARM_LIST
						&& pActivation->GetArchiveID() == idAlarmList)
					{
						bFound = TRUE;
					}
				}
				if (!bFound)
				{
					pActivation = pInput->AddInputToOutput();
					pActivation->SetArchiveID(idAlarmList);
					pActivation->SetInputState(IO_ALARM);
					pActivation->SetProcessID(SECID_PROCESS_ALARM_LIST);
					pActivation->SetOutputID(idCam);
					WK_TRACE(_T("Designa Wizard add Alarm list Activation on %s\n"),pInput->GetName());
				}

				bFound = FALSE;

				for (int j=0;j<pInput->GetNumberOfInputToOutputs()&&!bFound;j++)
				{
					pActivation = pInput->GetInputToOutput(j);
					if (   pActivation->GetProcessID() == idSaveProcess
						&& pActivation->GetArchiveID() == idRecording)
					{
						bFound = TRUE;
					}
				}
				if (!bFound)
				{
					pActivation = pInput->AddInputToOutput();
					pActivation->SetArchiveID(idRecording);
					pActivation->SetInputState(IO_ALARM);
					pActivation->SetProcessID(idSaveProcess);
					pActivation->SetOutputID(idCam);
					WK_TRACE(_T("Designa Wizard add Recording Data Activation on %s\n"),pInput->GetName());
				}
			}
		}
	}
	else
	{
		TRACE(_T("no Designa input group ??\n"));
	}

	GetView()->PostMessage(WM_USER, USER_MSG_UPDATE_PARENT_TREE, 1<<IMAGE_INPUT);

	return TRUE;
}

