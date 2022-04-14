/* GlobalReplace: pApp\-\> --> theApp. */
// ProcessListView.cpp : implementation file
//

#include "stdafx.h"
#include "sec3.h"
#include "ProcessListView.h"
#include "Process.h"
#include "input.h"
#include "OutputList.h"
#include "ProcessMacro.h"

#include "CIPCOutputServerClient.h"
#include "CipcDatabaseClientServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static CProcessListView* g_spProcessListView = NULL;
/////////////////////////////////////////////////////////////////////////////
// CProcessListView

IMPLEMENT_DYNCREATE(CProcessListView, CListView)

CProcessListView::CProcessListView()
{
	g_spProcessListView = this;
}

CProcessListView::~CProcessListView()
{
	m_UpdateProcessRecords.SafeDeleteAll();
}


BEGIN_MESSAGE_MAP(CProcessListView, CListView)
	//{{AFX_MSG_MAP(CProcessListView)
	ON_MESSAGE(WM_USER,OnUser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessListView diagnostics

/////////////////////////////////////////////////////////////////////////////
// CProcessListView message handlers

BOOL CProcessListView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~LVS_ICON;
	cs.style |= LVS_LIST;
	return CListView::PreCreateWindow(cs);
}
////////////////////////////////////////////////////////////////////////
LRESULT CProcessListView::OnUser(WPARAM wParam, LPARAM lParam)
{
	CUpdateProcessRecord* pRec = NULL;
	
	m_UpdateProcessRecords.Lock();
	while (m_UpdateProcessRecords.GetSize())
	{
		pRec = m_UpdateProcessRecords.GetAt(0);
		m_UpdateProcessRecords.RemoveAt(0);
		m_UpdateProcessRecords.Unlock();
		InsertUpdateProcessRecord(pRec);
		m_UpdateProcessRecords.Lock();
	}
	m_UpdateProcessRecords.Unlock();

	return 0L;
}
/////////////////////////////////////////////////////////////////////////////////
void CProcessListView::InsertUpdateProcessRecord(CUpdateProcessRecord* pRec)
{
	if (pRec)
	{
		CListCtrl &list = GetListCtrl();

		int iCount = list.GetItemCount();
		if (pRec->m_bActive) 
		{
			BOOL bFound = FALSE;
			// first look if already there
			for (int i=0;i<list.GetItemCount()&&!bFound;i++)
			{
				DWORD dwData = list.GetItemData(i);
				bFound = (dwData == (DWORD)pRec->m_pProcess) && (dwData!=0);
				if (bFound)
				{
					break;
				}
			}
			if (bFound)
			{
				list.SetItemText(i,0,(LPCTSTR)(pRec->m_sText));
			}
			else
			{
				int ix = list.InsertItem(iCount,(LPCTSTR)(pRec->m_sText));
				list.SetItemData(ix,(DWORD)pRec->m_pProcess);	// can be NULL for static text
			}
		} 
		else 
		{
			BOOL bRemoved=FALSE;
			for (int i=0;i<iCount && bRemoved==FALSE;i++) {
				DWORD dwData = list.GetItemData(i);
				if (dwData==(DWORD)pRec->m_pProcess) {
					list.DeleteItem(i);
					bRemoved=TRUE;
				}
			}
		}
		WK_DELETE(pRec);
	}
}



void UpdateProcessListView(BOOL bActive, CProcess *pProcess)
{
	// NOT YET special format for relais processes
	CString sText;

	CUpdateProcessRecord *pRec = new CUpdateProcessRecord;
	pRec->m_pProcess = pProcess;
	pRec->m_bActive = bActive;

	if (pProcess)
	{
		pRec->m_sText = pProcess->GetDescription(bActive);
	}

	if (WK_IS_WINDOW(g_spProcessListView))
	{
		g_spProcessListView->m_UpdateProcessRecords.SafeAdd(pRec);
		g_spProcessListView->PostMessage(WM_USER);
	}
	else 
	{
		WK_DELETE(pRec);
	}
}

void UpdateProcessListView(const CString & sMsg)
{
	CString sText;

	CUpdateProcessRecord *pRec = new CUpdateProcessRecord;
	pRec->m_pProcess = NULL;
	pRec->m_bActive = TRUE;
	CSystemTime st;
	st.GetLocalTime();
	CString sTime = st.GetDateTime();

	CString sOutput;
	sText.Format(_T("%s (%s)"),(LPCTSTR)sMsg,(LPCTSTR)sTime);
	pRec->m_sText = sText;
	
	if (WK_IS_WINDOW(g_spProcessListView))
	{
		g_spProcessListView->m_UpdateProcessRecords.SafeAdd(pRec);
		g_spProcessListView->PostMessage(WM_USER);
	}
	else 
	{
		WK_DELETE(pRec);
	}
}

