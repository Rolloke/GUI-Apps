// DBCheckDoc.cpp : implementation of the CDBCheckDoc class
//

#include "stdafx.h"
#include "DBCheck.h"

#include "DBCheckDoc.h"
#include "MainFrm.h"
#include "LeftView.h"
#include "DBCheckView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDBCheckDoc

IMPLEMENT_DYNCREATE(CDBCheckDoc, CDocument)

BEGIN_MESSAGE_MAP(CDBCheckDoc, CDocument)
	//{{AFX_MSG_MAP(CDBCheckDoc)
	ON_COMMAND(ID_CONNECT_LOCAL, OnConnectLocal)
	ON_UPDATE_COMMAND_UI(ID_CONNECT_LOCAL, OnUpdateConnectLocal)
	ON_COMMAND(ID_CONNECT_REMOTE, OnConnectRemote)
	ON_UPDATE_COMMAND_UI(ID_CONNECT_REMOTE, OnUpdateConnectRemote)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBCheckDoc construction/destruction

CDBCheckDoc::CDBCheckDoc()
{
   m_Server.InitServer(this, theApp.GetNewServerID());
}

CDBCheckDoc::~CDBCheckDoc()
{

}

BOOL CDBCheckDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CDBCheckDoc serialization

void CDBCheckDoc::Serialize(CArchive& ar)
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
// CDBCheckDoc diagnostics

#ifdef _DEBUG
void CDBCheckDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDBCheckDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDBCheckDoc commands

void CDBCheckDoc::OnConnectLocal() 
{
	if (m_Server.IsLocal())
	{
		// already connected
		return;
	}

	if (WK_IS_RUNNING(WK_APP_NAME_DB_SERVER)==FALSE) 
	{
		// no chance without dbs.exe running
		return; 
	}
/*
	DWORD dwTick = GetTickCount();

	if ((dwTick - m_dwLastLocalTry) < 7500)
	{
		// try again in 7,5 seconds
		AfxGetMainWnd()->PostMessage(WM_USER);
		return;
	}

	m_dwLastLocalTry = dwTick;
*/
   CHostArray& ha = theApp.GetHosts();

	CSecID idHost = ha.GetLocalHost()->GetID();

   if (m_Server.GetHostID() != idHost)
   {
		CString sLocal;
		sLocal.LoadString(IDS_LOCAL_SERVER);
		sLocal += " " + ha.GetLocalHost()->GetName();
		m_Server.Connect(sLocal,LOCAL_LOOP_BACK,idHost);
//		m_dwLastLocalTry = GetTickCount();
   }
}

void CDBCheckDoc::OnUpdateConnectLocal(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(m_Server.GetHostID().GetID() == 0);
}

void CDBCheckDoc::OnConnectRemote() 
{
	COEMConnectionDialog dlg(theApp.GetHosts(),AfxGetMainWnd());

	dlg.m_sFilterTypes = "MINI B3,MINI B6,SMS,FAX,E-Mail,TeleObserver";

	if (IDOK==dlg.DoModal())
	{
		m_Server.Connect(dlg.m_sHost,dlg.m_sTelefon,dlg.m_idHost);
	}
}

void CDBCheckDoc::OnUpdateConnectRemote(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(m_Server.GetHostID().GetID() == 0);
}

void CDBCheckDoc::ActualizeDatabase(CIPCDatabaseDBCheck* pDatabase)
{
	UpdateMyViews(NULL,VDH_DATABASE,(CObject*)pDatabase);
}

void CDBCheckDoc::UpdateMyViews(CView* pSender, LPARAM lHint, CObject* pHint)
	// walk through all views
{
	CLeftView* pOV    = GetLeftView();
	ASSERT_VALID(pOV);
	if (pOV != pSender) pOV->OnUpdate(pSender, lHint, pHint);

   CDBCheckView* pRV = GetDBCheckView();
	ASSERT_VALID(pRV);
	if (pRV != pSender)
		pRV->OnUpdate(pSender, lHint, pHint);
}

CLeftView* CDBCheckDoc::GetLeftView()
{
	CView* pView;
	POSITION pos = GetFirstViewPosition();
	while (pos) 
	{
		pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CLeftView))) 
		{
			return (CLeftView*)pView;
			break;
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////

CDBCheckView* CDBCheckDoc::GetDBCheckView()
{
	CView* pView;
	POSITION pos = GetFirstViewPosition();
	while (pos) 
	{
		pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CDBCheckView))) 
		{
			return (CDBCheckView*)pView;
			break;
		}
	}
	return NULL;
}

void CDBCheckDoc::OnIdle()
{
	m_Server.OnIdle();
/*
	if (m_bAutoLogout)
	{
		AutoLogout();
	}
	Sleep(0);

	if (GetLocalServer()==NULL)
	{
		OnConnectLocal();
	}
*/
	CLeftView* pOV = GetLeftView();
	if (WK_IS_WINDOW(pOV))
 	{
 		pOV->Redraw();
 	}
/*
	CDBCheckView* pRV = GetDBCheckView();
	if (WK_IS_WINDOW(pRV))
	{
		pRV->OnIdle();
	}
*/
}


void CDBCheckDoc::ServerError()
{
   CString str;
   str.Format("Verbindung zu %s nicht möglich!\n\n%s", m_Server.GetName(), m_Server.GetError());
   AfxMessageBox(str, MB_OK|MB_ICONERROR);
   m_Server.Reset();
}
