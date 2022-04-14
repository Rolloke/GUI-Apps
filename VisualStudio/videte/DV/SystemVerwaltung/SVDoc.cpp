// SVDoc.cpp : implementation of the CSVDoc class
//

#include "stdafx.h"
#include "SystemVerwaltung.h"

#include "SVDoc.h"
#include "SVView.h"

#include <io.h>
#include <stdio.h> 
#include <sys\stat.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSVDoc

IMPLEMENT_DYNCREATE(CSVDoc, CDocument)

BEGIN_MESSAGE_MAP(CSVDoc, CDocument)
	//{{AFX_MSG_MAP(CSVDoc)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVDoc construction/destruction

CSVDoc::CSVDoc()
{
	m_pProfile = new CWK_Profile();
	m_pDongle = new CWK_Dongle();
	m_dwFlags = theApp.m_pPermission->GetFlags();
	m_idHost = SECID_LOCAL_HOST;
}

CSVDoc::~CSVDoc()
{
	WK_DELETE(m_pProfile);
	WK_DELETE(m_pDongle);
}

/////////////////////////////////////////////////////////////////////////////
void CSVDoc::Load()
{
	CString s = theApp.m_pPermission->GetName();

	m_Hosts.DeleteAll();
	m_Hosts.Load(GetProfile());

	m_Archivs.DeleteAll();
	m_Archivs.Load(GetProfile());

	CArchivInfo* pArchivInfo = m_Archivs.GetArchivInfo(CSecID(SECID_GROUP_ARCHIVE,
																LOCAL_SEARCH_RESULT_ARCHIV_NR));
	if (pArchivInfo==NULL)
	{
		pArchivInfo = new CArchivInfo();
		CString sName;
		sName.LoadString(IDS_SEARCH_RESULT);
		pArchivInfo->SetName(sName);
		pArchivInfo->SetArchivNr(LOCAL_SEARCH_RESULT_ARCHIV_NR);
		pArchivInfo->SetSizeMB(START_SIZE_MB);
		pArchivInfo->SetTyp(RING_ARCHIV);
		m_Archivs.Add(pArchivInfo);
		m_Archivs.Save(GetProfile());
	}
	m_Drives.Init(GetProfile());
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

	if (WK_IS_WINDOW(pView))
	{
		return pView->CanCloseFrame();
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::OnFileSaveAs() 
{
	CString sOwnHostName = m_Hosts.GetLocalHostName();
	if (sOwnHostName.IsEmpty()
		|| sOwnHostName == CHostArray::GetLocalHostNameStandard()) {
			AfxMessageBox(IDP_NO_LOCAL_HOSTNAME,MB_ICONSTOP);
	}
	else {
		COemFileDialog dlg(AfxGetMainWnd());
		CString sSave,sSettings;
		sSave.LoadString(IDS_SAVE);
		sSettings.LoadString(IDS_SETTINGS);
		dlg.SetProperties(FALSE,sSave,sSave);
		dlg.SetInitialDirectory(_T("c:\\out"));
		dlg.AddFilter(sSettings,_T("svs"));
		if (IDOK==dlg.DoModal()) {
			CString sFileName = dlg.GetPathname();
			BeginWaitCursor();
			m_pProfile->SaveAs(sFileName,_T("DVRT"));
			EndWaitCursor();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVDoc::OnFileOpen() 
{
	COemFileDialog dlg(AfxGetMainWnd());
	CString sOpen,sSettings;

	sOpen.LoadString(IDS_OPEN);
	sSettings.LoadString(IDS_SETTINGS);
	dlg.SetProperties(TRUE,sOpen,sOpen);
	dlg.SetInitialDirectory(_T("c:\\out"));
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
			if (IDYES != iReturn) {
				bOK = FALSE;
			}
		}
		if (bOK) {
			m_pProfile->Import(sFileName);
			Load();
			GetView()->Clear();
			GetView()->InitTree();
		}
		EndWaitCursor();
	}
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

