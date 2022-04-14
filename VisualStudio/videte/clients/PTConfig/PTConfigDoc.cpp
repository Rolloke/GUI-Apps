// PTConfigDoc.cpp : implementation of the CPTConfigDoc class
//

#include "stdafx.h"
#include "PTConfig.h"

#include "PTConfigDoc.h"
#include "PTInputClient.h"
#include "PTOutputClient.h"
#include "CIPCServerControlClientSide.h"
#include "PostFetch.h"
#include "ConnectDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPTConfigDocs g_theDocs;	// global instead of static member to avoid declaration problems

/////////////////////////////////////////////////////////////////////////////
// CPTConfigDoc

int CPTConfigDoc::m_iInstances = 0;
IMPLEMENT_DYNCREATE(CPTConfigDoc, CDocument)

BEGIN_MESSAGE_MAP(CPTConfigDoc, CDocument)
	//{{AFX_MSG_MAP(CPTConfigDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPTConfigDoc construction/destruction

CPTConfigDoc::CPTConfigDoc()
{
	m_pInput = NULL;
	m_pOutput = NULL;
	m_state = PCS_CALLING;
	m_sState = "verbinden...";
	m_iInstances++;
	
	g_theDocs.Add(this);
	m_onlineStartTime = CTime::GetCurrentTime();
}

CPTConfigDoc::~CPTConfigDoc()
{
	m_state = PCS_DISCONNECTED;
	WK_DELETE(m_pInput);
	WK_DELETE(m_pOutput);
	m_iInstances--;

	g_theDocs.Remove(this);
}

BOOL CPTConfigDoc::OnNewDocument()
{
	if (m_iInstances>1) {
		// deny second document 
		// NOT YET real MDI
		return FALSE;
	}

	if (!CDocument::OnNewDocument())
		return FALSE;

	CConnectDialog dlg;
	dlg.m_sRemoteNumber="14";
	if (dlg.DoModal()==IDOK) {
		CConnectionRecord connection;
		CString sHost;
		sHost="pres:";
		sHost += dlg.m_sRemoteNumber;
		m_sRemoteHost = sHost;
		connection.SetDestinationHost(sHost);
		connection.SetTimeout(10000);	// OOPS
		// NOT YET any passwords
		//void SetPermission(const CString &sPermission);
		//void SetPassword(const CString &sPassword);
		//void SetSourceHost(const CString &sSource);
		// void SetOptions(DWORD dwOptions);
		CString sTitle;
		sTitle = dlg.m_sRemoteNumber;
		sTitle += " - B3 Konfiguration";
		SetTitle(sTitle);

		PostFetchInputConnection(connection, THE_MESSAGE,  (DWORD) this);
	} else {
		return FALSE;
	}

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CPTConfigDoc serialization

void CPTConfigDoc::Serialize(CArchive& ar)
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
// CPTConfigDoc diagnostics

#ifdef _DEBUG
void CPTConfigDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPTConfigDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPTConfigDoc commands


const CString &CPTConfigDoc::GetState() const
{
	return m_sState;
}

void CPTConfigDoc::ForceUpdate()
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND,WM_USER,(DWORD)this);
}
void CPTConfigDoc::AppendState(const CString &sState)
{
	m_sState += " ";
	m_sState += sState;
	ForceUpdate();
}


void CPTConfigDoc::HandleFetchResult(const CPostFetchResult &postResult)
{
#if 0
	DWORD dwStartCount = GetTickCount();
	for (int i=0;i<100;i++) {
		WK_TRACE("FooMessage %i \n",i);
		for (int j=0;j<50;j++) {
			WK_STAT_LOG("Foo",j,"Goo");
		}
	}

	WK_TRACE("FooDone 5000 traces took %dms\n", GetTickCount()-dwStartCount);
#endif


	CIPCFetchResult result = postResult.m_result;
	if (result.IsOkay()) {
		if (postResult.m_type==FCT_INPUT){
			WK_TRACE("connecting input\n");
			m_pInput = new CPTInputClient(this, result.GetShmName());

			CConnectionRecord connection;
			connection.SetDestinationHost(m_sRemoteHost);
			connection.SetTimeout(10000);	// OOPS

			PostFetchOutputConnection(connection,THE_MESSAGE, (DWORD) this);
		} else if (postResult.m_type==FCT_OUTPUT){
			WK_TRACE("connecting\n");
			m_pOutput = new CPTOutputClient(this, result.GetShmName());

			m_pInput->StartThread(TRUE);
			m_pOutput->StartThread(TRUE);

			m_onlineStartTime = CTime::GetCurrentTime();
		} else {
			WK_TRACE("Invalid PostFetchResult %d\n",postResult.m_type);
		}
	} else {
		// NOT YET GetError() and GetErrorString();
		CString sMsg;
		sMsg = "Verbindung konnte nicht hergestellt werden.\n[";
		sMsg += result.GetErrorMessage();
		sMsg += "]";
		m_sState = sMsg;
		m_state = PCS_FAILED;
		ForceUpdate();
	}
}

BOOL CPTConfigDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	// TODO: Add your specialized creation code here
	
	return TRUE;
}

BOOL CPTConfigDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDocument::OnSaveDocument(lpszPathName);
}
