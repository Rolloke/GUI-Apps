/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: RechercheView.cpp $
// ARCHIVE:		$Archive: /Project/Clients/Recherche/RechercheView.cpp $
// CHECKIN:		$Date: 16.01.04 13:35 $
// VERSION:		$Revision: 72 $
// LAST CHANGE:	$Modtime: 16.01.04 13:26 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$

#include "stdafx.h"
#include "Recherche.h"

#include "Mainfrm.h"
#include "RechercheDoc.h"
#include "RechercheView.h"
#include "AlarmListWindow.h"
#include "BackupDialogOld.h"
#include "BackupDialogNew.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRechercheView

IMPLEMENT_DYNCREATE(CRechercheView, CView)

BEGIN_MESSAGE_MAP(CRechercheView, CView)
	//{{AFX_MSG_MAP(CRechercheView)
	ON_WM_SIZE()
	ON_COMMAND(ID_VIEW_1, OnView1)
	ON_UPDATE_COMMAND_UI(ID_VIEW_1, OnUpdateView1)
	ON_COMMAND(ID_VIEW_2_2, OnView22)
	ON_UPDATE_COMMAND_UI(ID_VIEW_2_2, OnUpdateView22)
	ON_COMMAND(ID_VIEW_3_3, OnView33)
	ON_UPDATE_COMMAND_UI(ID_VIEW_3_3, OnUpdateView33)
	ON_COMMAND(ID_VIEW_ORIGINAL, OnViewOriginal)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ORIGINAL, OnUpdateViewOriginal)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_COMMAND(ID_VIEW_TOGGLE, OnViewToggle)
	ON_COMMAND(ID_SMALL_CLOSE, OnSmallClose)
	ON_UPDATE_COMMAND_UI(ID_SMALL_CLOSE, OnUpdateSmallClose)
	ON_COMMAND(ID_FILE_OPEN_DISK, OnFileOpenDisk)
	ON_COMMAND(ID_SMALL_CLOSE_ALL, OnSmallCloseAll)
	ON_UPDATE_COMMAND_UI(ID_SMALL_CLOSE_ALL, OnUpdateSmallCloseAll)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN_DISK, OnUpdateFileOpenDisk)
	ON_COMMAND(ID_SEQUENCE_OVER, OnSequenceOver)
	ON_UPDATE_COMMAND_UI(ID_SEQUENCE_OVER, OnUpdateSequenceOver)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRechercheView construction/destruction

CRechercheView::CRechercheView()
{
	m_iXWin = 1;
	m_pActiveSmallWindow = NULL;
	m_pMapWindow = NULL;
	m_pBackupDialog		  = NULL;
	m_b1to1 = FALSE;
	m_bSequenceOver = TRUE;
//	m_bSequenceRealtime = FALSE;
	m_sOpenDirectory = _T("c:\\out");
}

CRechercheView::~CRechercheView()
{
	CSmallWindow* pSW;
	for (int i=0;i<m_SmallWindows.GetSize();i++)
	{
		pSW = m_SmallWindows[i];
		WK_DELETE(pSW);
	}
	m_SmallWindows.RemoveAll();
}

BOOL CRechercheView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CRechercheView drawing

void CRechercheView::OnDraw(CDC* pDC)
{
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	CSmallWindow* pSW = GetActiveSmallWindow();
	if (WK_IS_WINDOW(pSW))
	{
		if (pSW->IsImageWindow())
		{
			CImageWindow* pIW = (CImageWindow*)pSW;
			pIW->OnPrint(pDC,pInfo);
		}
		else if (pSW->IsAlarmListWindow())
		{
			CAlarmListWindow* pALW = (CAlarmListWindow*)pSW;
			pALW->OnPrint(pDC,pInfo);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// CRechercheView printing

BOOL CRechercheView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	CSmallWindow* pSW = GetActiveSmallWindow();
	if (WK_IS_WINDOW(pSW))
	{
		if (pSW->IsAlarmListWindow())
		{
			CAlarmListWindow* pALW = (CAlarmListWindow*)pSW;
			return pALW->OnPreparePrinting(pInfo);
		}
	}

	return DoPreparePrinting(pInfo);
}

void CRechercheView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CRechercheView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CRechercheView diagnostics

#ifdef _DEBUG
void CRechercheView::AssertValid() const
{
	CView::AssertValid();
}

void CRechercheView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRechercheDoc* CRechercheView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRechercheDoc)));
	return (CRechercheDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
void CRechercheView::ShowMapWindow()
{
	if (WK_IS_WINDOW(m_pMapWindow))
	{
		m_pMapWindow->BringWindowToTop();
	}
	else
	{
		CRect rect;
		GetClientRect(rect);
		
		m_pMapWindow = new CHtmlWindow();
		m_pMapWindow->Create(rect,1,this);
		m_pMapWindow->NavigateTo(theApp.GetMapURL());
		AddSmallWindow(m_pMapWindow);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CRechercheView message handlers

void CRechercheView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	CString sMapURL;
	sMapURL = theApp.GetMapURL();
	if (!sMapURL.IsEmpty())
	{
		if (DoesFileExist(sMapURL))
		{
			ShowMapWindow();
		}
	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	WORD cause	  = LOWORD(lHint);
	WORD wServerID = HIWORD(lHint);
	CRechercheDoc* pDoc = GetDocument();
	CServer* pServer = pDoc->GetServer(wServerID);


	switch(cause)
	{
	case VDH_OPEN_FILE:
		AddDisplayWindow(pServer,(CIPCArchivFileRecord*)pHint);
		break;
	case VDH_OPEN_SEQUENCE:
		AddDisplayWindow(pServer,(CIPCSequenceRecord*)pHint);
		break;
	case VDH_REMOVE_SERVER:
		DeleteSmallWindows((WORD)pHint);
		break;
	case VDH_REMOVE_FILE:
		{
			DWORD dw = (DWORD)pHint;
			WORD wArchivNr = LOWORD(dw);
			WORD wSequenceNr = HIWORD(dw);
			DeleteSmallWindow(wServerID,CSecID(wArchivNr,wSequenceNr));
		}
		break;
	case VDH_DEL_ARCHIV:
		DeleteSmallWindows(wServerID,(WORD)pHint);
		break;
	case VDH_BACKUP:
		OnArchivBackup((CServer*)pHint);
		break;
	case VDH_BACKUP_ADD_SEQUENCE:
		if (WK_IS_WINDOW(m_pBackupDialog))
		{
			m_pBackupDialog->AddSequence((CIPCSequenceRecord*)pHint);
		}
		break;
	case VDH_BACKUP_DEL_SEQUENCE:
		if (WK_IS_WINDOW(m_pBackupDialog))
		{
			m_pBackupDialog->DeleteSequence((CIPCSequenceRecord*)pHint);
		}
		break;
	case VDH_BACKUP_ADD_ARCHIVE:
		if (WK_IS_WINDOW(m_pBackupDialog))
		{
			m_pBackupDialog->AddArchive((CIPCArchivRecord*)pHint);
		}
		break;
	case VDH_BACKUP_DEL_ARCHIVE:
		if (WK_IS_WINDOW(m_pBackupDialog))
		{
			CIPCArchivRecord* pArchivRecord = (CIPCArchivRecord*)pHint;
			m_pBackupDialog->DeleteArchive(pArchivRecord->GetID(),TRUE);
		}
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	SetWindowSize();
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::AddSmallWindow(CSmallWindow* pSW)
{
	m_SmallWindows.SafeAdd(pSW);

	ShowHideRectangle();

//	SortSmallWindows();
	if (m_SmallWindows.GetSize()>m_iXWin*m_iXWin)
	{	

		if (m_iXWin<3)
		{
			m_iXWin++;
			//when more than 4 sequences are open
			//and actual view was 1x1
			if(  (m_SmallWindows.GetSize()>m_iXWin*m_iXWin) 
				&&(m_iXWin<3))
			{
				m_iXWin++;
			}
		}
	}
	SetWindowSize();

}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::SortSmallWindows()
{
	int iCur, iMin, iNext;
	CSmallWindow* swap;

	m_SmallWindows.Lock();
	for (iCur=0; iCur<m_SmallWindows.GetSize(); iCur++)
	{
		iMin = iCur;
		for (iNext=iCur; iNext<m_SmallWindows.GetSize(); iNext++)
		{
			if (*m_SmallWindows.GetAt(iNext) < *m_SmallWindows.GetAt(iMin))
			{
				iMin = iNext;
			}
		}
		// wenn ein element kleiner als das aktuelle ist, tausche die elemente.
		if (iMin>iCur)
		{
			swap = m_SmallWindows.GetAt(iCur);
			m_SmallWindows.SetAt(iCur,m_SmallWindows.GetAt(iMin));
			m_SmallWindows.SetAt(iMin,swap);
		}
	}
	m_SmallWindows.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::AddDisplayWindow(CServer* pServer, CIPCSequenceRecord* pSequence)
{
	CDisplayWindow* pDW = GetDisplayWindow(pServer->GetID(),
										   CSecID(pSequence->GetArchiveNr(),
										   pSequence->GetSequenceNr()));
	
	if (pDW)
	{
		// already open
	}
	else
	{
		CRect rect;

		GetClientRect(rect);
		rect.right /= m_iXWin;
		rect.bottom /= m_iXWin;
		CSmallWindow* pSW = NULL;

		if (pSequence->IsAlarmList())
		{
			pSW = new CAlarmListWindow(pServer, pSequence);
		}
		else
		{
			pSW = new CDisplayWindow(pServer, *pSequence);
		}

		if (pSW)
		{
			if (!pSW->Create(rect, this))
			{
				delete pSW;
				return;
			}
			AddSmallWindow(pSW);
			ActivateSmallWindow(pSW);

			DWORD dwUserData = MAKELONG(pServer->GetID(),
								pSequence->GetSequenceNr()); // low,high
			if (pSequence->IsAlarmList())
			{
				pServer->GetDatabase()->DoRequestRecords(pSequence->GetArchiveNr(),pSequence->GetSequenceNr(),0,0);

			}
			else
			{
				pServer->GetDatabase()->DoRequestCameraNames(pSequence->GetArchiveNr(),
					dwUserData);
			}
//			pDW->Navigate(1);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::AddDisplayWindow(CServer* pServer, CIPCArchivFileRecord* pFile)
{
	CDisplayWindow* pDW = GetDisplayWindow(pServer->GetID(),
										   CSecID(pFile->GetArchivNr(),
										   pFile->GetDirNr()));
	
	if (pDW)
	{
		// already open
	}
	else
	{
		CRect rect;

		GetClientRect(rect);
		rect.right /= m_iXWin;
		rect.bottom /= m_iXWin;

		pDW = new CDisplayWindow(pServer, pFile);

		if (pDW)
		{
			if (!pDW->Create(rect, this))
			{
				delete pDW;
				return;
			}
			AddSmallWindow(pDW);
			ActivateSmallWindow(pDW);
			pDW->Navigate(1);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
CSmallWindow* CRechercheView::GetActiveSmallWindow()
{
	return m_pActiveSmallWindow;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRechercheView::ActivateSmallWindow(CSmallWindow* pSW)
{
	// activate/display SmallWindow
	// because of new Picture Data, or Instruction 
	if (WK_IS_WINDOW(m_pActiveSmallWindow))
	{
		m_pActiveSmallWindow->SetActive(FALSE);
	}
	m_pActiveSmallWindow = pSW;
	if (WK_IS_WINDOW(m_pActiveSmallWindow))
	{
		m_pActiveSmallWindow->SetActive();
	}
	pSW->ShowWindow(SW_SHOW);

	// now hide the old on same Position but which ?
	int i,index,square;

	square = m_iXWin*m_iXWin;
	if (m_SmallWindows.GetSize()<=square)
	{
		return FALSE;
	}

	if (pSW->IsWindowVisible())
	{
		return FALSE;
	}

	m_SmallWindows.Lock();
	index = -1;
	// first found myself
	for (i=0;i<m_SmallWindows.GetSize();i++)
	{
		if (m_SmallWindows[i]==pSW)
		{
			index = i;
			break;
		}
	}
	if (index!=-1)
	{
		for (i=0;i<m_SmallWindows.GetSize();i++)
		{
			if ((i%square) == (index%square) )
			{
				m_SmallWindows[i]->ShowWindow(SW_HIDE);
			}
		}
	}

	m_SmallWindows.Unlock();

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CSmallWindows*	CRechercheView::GetSmallWindows()
{
	CSmallWindows* pSW = &m_SmallWindows;
	return pSW;
}
/////////////////////////////////////////////////////////////////////////////
CDisplayWindow*	CRechercheView::GetDisplayWindow(WORD wServerID, WORD wArchiv, WORD wSequence)
{
	return GetDisplayWindow(wServerID,CSecID(wArchiv,wSequence));
}
/////////////////////////////////////////////////////////////////////////////
CDisplayWindow*	CRechercheView::GetDisplayWindow(WORD wServerID, CSecID id)
{
	CSmallWindow* pSW = NULL;
	
	m_SmallWindows.Lock();
	for (int i=0;i<m_SmallWindows.GetSize();i++) 
	{
		pSW = m_SmallWindows.GetAt(i);
		if (   WK_IS_WINDOW(pSW) 
			&& (pSW->IsDisplayWindow()) 
			&& (pSW->GetServerID()==wServerID)	
			)
		{
			CDisplayWindow* pDW = (CDisplayWindow*)pSW;
			if (id.GetSubID()==0)
			{
				if (pDW->GetID().GetGroupID()==id.GetGroupID())
				{
					m_SmallWindows.Unlock();
					return pDW;
				}
			}
			else
			{
				if (pDW->GetID()==id)
				{
					m_SmallWindows.Unlock();
					return pDW;
				}
			}
		}
	}
	m_SmallWindows.Unlock();
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CAlarmListWindow* CRechercheView::GetAlarmListWindow(WORD wServerID, WORD wArchiv, WORD wSequence)
{
	CSmallWindow* pSW = NULL;
	CSecID id(wArchiv,wSequence);
	
	m_SmallWindows.Lock();
	for (int i=0;i<m_SmallWindows.GetSize();i++) 
	{
		pSW = m_SmallWindows.GetAt(i);
		if (   WK_IS_WINDOW(pSW) 
			&& (pSW->IsAlarmListWindow()) 
			&& (pSW->GetServerID()==wServerID)	
			)
		{
			CAlarmListWindow* pALW = (CAlarmListWindow*)pSW;
			if (pALW->GetID()==id)
			{
				m_SmallWindows.Unlock();
				return pALW;
			}
		}
	}
	m_SmallWindows.Unlock();
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::CheckSmallWindowsFullScreen()
{
	if (m_SmallWindows.GetSize() == 0)
	{
		CMainFrame* pMF = (CMainFrame*)AfxGetMainWnd();
		if (WK_IS_WINDOW(pMF) && pMF->IsFullScreen())
		{
			pMF->ChangeFullScreenModus();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::DeleteSmallWindow(WORD wServerID, CSecID id)
{
	CSmallWindow* pSW = NULL;
	BOOL bFound = FALSE;
	BOOL bDeleted = FALSE;
	
	m_SmallWindows.Lock();

	for (int i=0;i<m_SmallWindows.GetSize();i++) 
	{
		pSW = m_SmallWindows.GetAt(i);
		if (pSW && (pSW->GetServerID()==wServerID))
		{
			if (pSW->GetID()==id)
			{
				if (pSW->PreDestroyWindow(this))
				{
					if (pSW == m_pActiveSmallWindow)
					{
						m_pActiveSmallWindow = NULL;
					}

					pSW->DestroyWindow();
					WK_DELETE(pSW);
					m_SmallWindows.RemoveAt(i);
					bDeleted = TRUE;

					ShowHideRectangle();
				}
				bFound = TRUE;
				break;
			}
		}
	}

	// activate last recherche-window
	if (m_pActiveSmallWindow == NULL)
	{
		if (m_SmallWindows.GetSize() > 0)
		{
			pSW = m_SmallWindows.GetAt(m_SmallWindows.GetSize()-1);
			ActivateSmallWindow(pSW);
		}
	}

	m_SmallWindows.Unlock();


	if (bDeleted)
	{
		if (m_SmallWindows.GetSize()<=(m_iXWin-1)*(m_iXWin-1))
		{
			if (m_iXWin>1)
			{
				m_iXWin--;
			}
		}
		SetWindowSize();
	}

	CheckSmallWindowsFullScreen();
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::DeleteHtmlWindow(CHtmlWindow* pHW)
{
	int i;
	CSmallWindow* pSW = NULL;
	BOOL bFound = FALSE;
	
	m_SmallWindows.Lock();
	for (i=0;i<m_SmallWindows.GetSize();i++) 
	{
		pSW = m_SmallWindows.GetAt(i);
		if (pSW == pHW)
		{
			pSW->DestroyWindow();
			WK_DELETE(pSW);
			m_SmallWindows.RemoveAt(i);
			bFound = TRUE;
			m_pMapWindow = NULL;
			break;
		}
	}
	m_SmallWindows.Unlock();
	if (bFound)
	{
		if (m_SmallWindows.GetSize()<=(m_iXWin-1)*(m_iXWin-1))
		{
			if (m_iXWin>1)
			{
				m_iXWin--;
			}
		}
		SetWindowSize();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::DeleteSmallWindows(WORD wServerID)
{
	int i;
	CSmallWindow* pSW = NULL;
	
	m_SmallWindows.Lock();
	for (i=m_SmallWindows.GetSize()-1;i>=0;i--) 
	{
		pSW = m_SmallWindows.GetAt(i);
		if (pSW && (pSW->GetServerID()==wServerID))
		{
			if (pSW == m_pActiveSmallWindow)
			{
				m_pActiveSmallWindow = NULL;
			}
			pSW->DestroyWindow();
			WK_DELETE(pSW);
			m_SmallWindows.RemoveAt(i);

			ShowHideRectangle();

		}
	}
	m_SmallWindows.Unlock();
	if (m_SmallWindows.GetSize()<=(m_iXWin-1)*(m_iXWin-1))
	{
		if (m_iXWin>1)
		{
			m_iXWin--;
		}
	}
	SetWindowSize();

	if (WK_IS_WINDOW(m_pBackupDialog))
	{
		if (m_pBackupDialog->GetServer()->GetID() == wServerID)
		{
			m_pBackupDialog->DestroyWindow();
			OnBackupClosed();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::DeleteSmallWindows(WORD wServerID, WORD wArchivID)
{
	int i;
	CSmallWindow* pSW = NULL;
	
	m_SmallWindows.Lock();
	for (i=m_SmallWindows.GetSize()-1;i>=0;i--) 
	{
		pSW = m_SmallWindows.GetAt(i);
		if (pSW->IsDisplayWindow())
		{
			CDisplayWindow* pDW = (CDisplayWindow*)pSW;
			if (	pDW 
				&& (pDW->GetServerID()==wServerID)
				&& (pDW->GetArchivNr()==wArchivID)
				)
			{
				if (pSW == m_pActiveSmallWindow)
				{
					m_pActiveSmallWindow = NULL;
				}
				pDW->DestroyWindow();
				WK_DELETE(pDW);
				m_SmallWindows.RemoveAt(i);

				ShowHideRectangle();

			}
		}
	}
	m_SmallWindows.Unlock();
	if (m_SmallWindows.GetSize()<=(m_iXWin-1)*(m_iXWin-1))
	{
		if (m_iXWin>1)
		{
			m_iXWin--;
		}
	}
	SetWindowSize();
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::SetSmallWindowView1(CSmallWindow* pDW)
{
	int i;

	if (m_iXWin != 1)
	{		
		m_iXWin = 1;
		SetWindowSize();
	}

	for (i=0;i<m_SmallWindows.GetSize();i++)
	{
		if (m_SmallWindows[i]==pDW)
		{
			m_SmallWindows[i]->ShowWindow(SW_SHOW);
		}
		else
		{
			if ((i>m_iXWin*m_iXWin) || (m_iXWin== 1))
			{
				m_SmallWindows[i]->ShowWindow(SW_HIDE);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::SetXWin(int i)
{
	m_iXWin = i;
	SetWindowSize();

	if(m_iXWin == 1)
	{
		for (i=0;i<m_SmallWindows.GetSize();i++)
		{
			if(m_SmallWindows[i]->IsActive())
			{
				if(i!=0)
				{
					m_SmallWindows[0]->ShowWindow(SW_HIDE);
				}
				m_SmallWindows[i]->RedrawWindow();
			}
		}

	}
	else

	{
		for (i=0;i<m_SmallWindows.GetSize();i++)
		{
			if (m_SmallWindows[i]->IsWindowVisible())
			{
				m_SmallWindows[i]->RedrawWindow();
			}
		}
	}


}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::SetWindowSize()
{
	CRect r;
	GetClientRect(&r);
	int x,y,i;
	// dimension of the view window itself
	int viewWidth = r.Width();
	int viewHeight = r.Height();
	// dimension for the child windows, one per camera
	int childWidth,childHeight;

	childWidth = viewWidth / m_iXWin;
	childHeight = viewHeight / m_iXWin;

	for (i=0;i<m_SmallWindows.GetSize();i++) 
	{
		x = (i-(i/m_iXWin)*m_iXWin) * childWidth;
		y = (i/m_iXWin-(i/(m_iXWin*m_iXWin))*m_iXWin) * childHeight;


		if (i<m_iXWin*m_iXWin)
		{
			if (!m_SmallWindows[i]->IsWindowVisible())
			{
				m_SmallWindows[i]->ShowWindow(SW_SHOW);
			}
		}
		m_SmallWindows[i]->MoveWindow(x, y, childWidth, childHeight);
		m_SmallWindows[i]->OnSetWindowSize();
	}

	if (WK_IS_WINDOW(m_pBackupDialog))
	{
		m_pBackupDialog->MoveWindow(r);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnView1() 
{
	int i;
	BOOL b = FALSE;
	int iActive = 0;

	for (i=0;i<m_SmallWindows.GetSize();i++)
	{
		if (!(m_SmallWindows[i]->IsActive()))
		{
			m_SmallWindows[i]->ShowWindow(SW_HIDE);
		}
		else
		{
			b = TRUE;
			iActive = i;
		}
	}

	if (!b)
	{
		m_SmallWindows[0]->ShowWindow(SW_SHOW);
	}
	else
	{
		m_SmallWindows[iActive]->ShowWindow(SW_SHOW);
	}

	SetXWin(1);
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnUpdateView1(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_SmallWindows.GetSize()>0);
	pCmdUI->SetCheck(m_iXWin==1);
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnView22() 
{
	int i,c;

	c = min(4,m_SmallWindows.GetSize());
	if (m_iXWin>2)
	{
		for (i=4;i<m_SmallWindows.GetSize();i++)
		{
			m_SmallWindows[i]->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		for (i=0;i<c;i++)
		{
			m_SmallWindows[i]->ShowWindow(SW_SHOW);
		}
	}

	SetXWin(2);
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnUpdateView22(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_SmallWindows.GetSize()>1);
	pCmdUI->SetCheck(m_iXWin==2);
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnView33() 
{
	int i,c;

	c = min(9,m_SmallWindows.GetSize());
	if (m_iXWin>3)
	{
		for (i=9;i<m_SmallWindows.GetSize();i++)
		{
			m_SmallWindows[i]->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		for (i=0;i<c;i++)
		{
			m_SmallWindows[i]->ShowWindow(SW_SHOW);
		}
	}

	SetXWin(3);
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnUpdateView33(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_SmallWindows.GetSize()>4);
	pCmdUI->SetCheck(m_iXWin==3);
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnViewOriginal() 
{
	m_b1to1 = !m_b1to1;

	int i;
	CSmallWindow* pSW = NULL;
	
	for (i=0;i<m_SmallWindows.GetSize();i++) 
	{
		pSW = m_SmallWindows.GetAt(i);
		if (pSW && pSW->IsDisplayWindow())
		{
			CDisplayWindow* pDW = (CDisplayWindow*)pSW;
			pDW->Set1to1(m_b1to1);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnUpdateViewOriginal(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_SmallWindows.GetSize());
	pCmdUI->SetCheck(m_b1to1);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRechercheView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (WK_IS_WINDOW(m_pActiveSmallWindow))
	{
		if (m_pActiveSmallWindow->OnCmdMsg(nID,nCode,pExtra,pHandlerInfo))
		{
			return TRUE;
		}
	}
	
	return CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnFileOpenDisk() 
{
	COemFileDialog dlg(this);
	CString sOpen,sJPX,sJPEG,sBMP;

	sOpen.LoadString(IDS_OPEN);
	dlg.SetProperties(TRUE,sOpen,sOpen);
	dlg.SetInitialDirectory(_T("a:"));

#ifndef _DTS
	sJPX.LoadString(IDS_JPX);
	dlg.AddFilter(sJPX,_T("jpx"));
#endif
	
	sJPEG.LoadString(IDS_JPEG);
	dlg.AddFilter(sJPEG, _T("jpg"));

	sBMP.LoadString(IDS_DIB);
	dlg.AddFilter(sBMP, _T("bmp"));

	if (IDOK==dlg.DoModal())
	{
		CString sFileName = dlg.GetPathname();
		CRect rect;
		CDibWindow* pDW;
		int p;
		
		p = sFileName.ReverseFind(_T('\\'));
		m_sOpenDirectory = sFileName.Left(p);
		GetClientRect(rect);
		rect.right /= m_iXWin;
		rect.bottom /= m_iXWin;

		pDW = new CDibWindow();

		if (pDW)
		{
			if (!pDW->Create(rect, this))
			{
				delete pDW;
				return;
			}
			if (!pDW->LoadFile(sFileName))
			{
				pDW->DestroyWindow();
				delete pDW;
				return;
			}
			AddSmallWindow(pDW);
			ActivateSmallWindow(pDW);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnFileOpen() 
{
	COemFileDialog dlg(this);
	CString sOpen,sDIB,sJPX,sJPG;

	sOpen.LoadString(IDS_OPEN);
	dlg.SetProperties(TRUE,sOpen,sOpen);
	dlg.SetInitialDirectory(m_sOpenDirectory);
	
#ifndef _DTS
	sJPX.LoadString(IDS_JPX);
	dlg.AddFilter(sJPX,_T("jpx"));
#endif
	
	sJPG.LoadString(IDS_JPEG);
	dlg.AddFilter(sJPG,_T("jpg"));

	sDIB.LoadString(IDS_DIB);
	dlg.AddFilter(sDIB,_T("bmp"));
	
	if (IDOK==dlg.DoModal())
	{
		CString sFileName = dlg.GetPathname();
		CRect rect;
		CDibWindow* pDW;
		int p;
		
		p = sFileName.ReverseFind(_T('\\'));
		m_sOpenDirectory = sFileName.Left(p);
		GetClientRect(rect);
		rect.right /= m_iXWin;
		rect.bottom /= m_iXWin;

		pDW = new CDibWindow();

		if (pDW)
		{
			if (!pDW->Create(rect, this))
			{
				delete pDW;
				return;
			}
			if (!pDW->LoadFile(sFileName))
			{
				pDW->DestroyWindow();
				delete pDW;
				return;
			}
			AddSmallWindow(pDW);
			ActivateSmallWindow(pDW);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnUpdateFilePrint(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_SmallWindows.GetSize()>0);
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnViewToggle() 
{
	if (m_iXWin == 1)
	{
		if (m_SmallWindows.GetSize()>1)
		{
			OnView22();
		}
	}
	else if (m_iXWin == 2)
	{
		if (m_SmallWindows.GetSize()>4)
		{
			OnView33();
		}
		else
		{
			OnView1();
		}
	}
	else if (m_iXWin == 3)
	{
		OnView1();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnSmallClose() 
{
	CSmallWindow* pSW = GetActiveSmallWindow();

	if (WK_IS_WINDOW(pSW))
	{
		pSW->SendMessage(WM_COMMAND,ID_SMALL_CLOSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnUpdateSmallClose(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_SmallWindows.GetSize()>0);
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnArchivBackup(CServer* pServer)
{
	if (!theApp.m_bAllowBackup)
	{
		return;
	}
	if (pServer)
	{
		BeginWaitCursor();
		
		OnSmallCloseAll();

		if (pServer->CanSplittingBackup())
		{
			m_pBackupDialog = new CBackupDialogNew(pServer,this);
		}
		else
		{
			m_pBackupDialog = new CBackupDialogOld(pServer,this);
		}

		CRect r;
		GetClientRect(r);
		m_pBackupDialog->MoveWindow(r);
		m_pBackupDialog->ShowWindow(SW_SHOW);
		
		EndWaitCursor();
	}
	else
	{
		if (WK_IS_WINDOW(m_pBackupDialog))
		{
			m_pBackupDialog->DestroyWindow();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnBackupClosed()
{
	GetDocument()->UpdateMyViews(this,VDH_BACKUP,NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::CloseAllWindows() 
{
	m_SmallWindows.Lock();
	for (int i=m_SmallWindows.GetSize()-1;i>=0;i--)
	{
		m_SmallWindows.GetAt(i)->SendMessage(WM_COMMAND,ID_SMALL_CLOSE);
	}
	m_SmallWindows.RemoveAll();
	m_SmallWindows.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnSmallCloseAll() 
{
	CloseAllWindows();
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnUpdateSmallCloseAll(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_SmallWindows.GetSize()>0);
}
/////////////////////////////////////////////////////////////////////////////
CBackupDialog* CRechercheView::GetBackupDialog()
{
	return m_pBackupDialog;
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnUpdateFileOpenDisk(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pBackupDialog==NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnIdle()
{
	for (int i=0;i<m_SmallWindows.GetSize();i++)
	{
		m_SmallWindows.GetAt(i)->OnIdle();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRechercheView::Is1x1()
{
	return (m_iXWin == 1);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CRechercheView::UseSingleWindow()
{
	return m_bSequenceOver;
}

/////////////////////////////////////////////////////////////////////////////
/*BOOL CRechercheView::CanPlayRealtime()
{
	return m_bSequenceRealtime;
}
*/
/////////////////////////////////////////////////////////////////////////////
//if QueryDialog is on Desktop, show or hide Checkbox for Rectangle-Search
void CRechercheView::ShowHideRectangle()
{
	//gehe alle offenen SmallWindows durch, ist ein Rechteck vorhanden, dann im
	//QueryDialog entsprechend die Bereichsuche enablen und den Dateibaum entsprechend anzeigen
	CQueryDialog* pQDlg = GetDocument()->GetObjectView()->GetQueryDialog();
	CSmallWindow* pSW = NULL;
	int iSize = m_SmallWindows.GetSize();
	BOOL bShowHideRect = FALSE;
	if(iSize)
	{
		CDisplayWindow* pDW = NULL;
		for(int i=0; i < iSize; i++)
		{
			pSW = m_SmallWindows.GetAt(i);
			if(pSW)
			{
				pDW = (CDisplayWindow*)pSW;
				if(pDW && !pDW->GetRectToQuery().IsRectEmpty())
				{
					bShowHideRect = TRUE;
					break;
				}
			}
		}
	}

	if(pQDlg)
	{
		pQDlg->ShowHideRectangle(bShowHideRect);
	}
}
/////////////////////////////////////////////////////////////////////////////
CDisplayWindow* CRechercheView::GetYoungestDisplayWindow(CSystemTime& t)
{
	CDisplayWindow* pRet = NULL;
	CSystemTime n,d;


	m_SmallWindows.Lock();

	for (int i=0;i<m_SmallWindows.GetSize();i++)
	{
		if (m_SmallWindows.GetAt(i)->IsDisplayWindow())
		{
			CSmallWindow* pSW = m_SmallWindows.GetAt(i);
			if (pSW->IsDisplayWindow())
			{
				CDisplayWindow* pDW = (CDisplayWindow*)pSW;
				if (pDW->GetTimeStamp(d))
				{
					if (d>n)
					{
						n = d;
						pRet = pDW;
					}
				}
			}
		}
	}


	m_SmallWindows.Unlock();

	t = n;

	return pRet;
}
/////////////////////////////////////////////////////////////////////////////
CDisplayWindow* CRechercheView::GetOldestDisplayWindow(CSystemTime& t)
{
	CDisplayWindow* pRet = NULL;
	CSystemTime n,d;

	n.GetLocalTime();

	m_SmallWindows.Lock();

	for (int i=0;i<m_SmallWindows.GetSize();i++)
	{
		if (m_SmallWindows.GetAt(i)->IsDisplayWindow())
		{
			CSmallWindow* pSW = m_SmallWindows.GetAt(i);
			if (pSW->IsDisplayWindow())
			{
				CDisplayWindow* pDW = (CDisplayWindow*)pSW;
				if (pDW->GetTimeStamp(d))
				{
					if (d<n)
					{
						n = d;
						pRet = pDW;
					}
				}
			}
		}
	}


	m_SmallWindows.Unlock();

	t = n;

	return pRet;
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnSequenceOver() 
{
	m_bSequenceOver = !m_bSequenceOver;
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheView::OnUpdateSequenceOver(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bSequenceOver);
	
}
