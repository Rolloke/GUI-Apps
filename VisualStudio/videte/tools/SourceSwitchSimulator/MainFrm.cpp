// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Simulator.h"

#include "MainFrm.h"
#include "CDecoder.h"
#include "CSignal.h"
#include "CMux.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(IDM_START, OnStart)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_Values[0].Init("Switch", 8);
	m_Values[1].Init("Signal1", 1);
	m_Values[2].Init("Signal2", 2);
	m_Values[3].Init("Signal3", 3);
	m_Values[4].Init("MuxOut1", 4);
	m_Values[5].Init("MuxOut2", 5);
	m_Values[6].Init("Decode1Out", 6);
	m_Values[7].Init("Decode2Out", 7);
	m_Values[8].Init("Zaoran", 8);
}

CMainFrame::~CMainFrame()
{
	m_Values[0].SafeDeleteAll();
	m_Values[1].SafeDeleteAll();
	m_Values[2].SafeDeleteAll();
	m_Values[3].SafeDeleteAll();
	m_Values[4].SafeDeleteAll();
	m_Values[5].SafeDeleteAll();
	m_Values[6].SafeDeleteAll();
	m_Values[7].SafeDeleteAll();
	m_Values[8].SafeDeleteAll();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
/*	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
*/
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	
//	cs.dwExStyle |= WS_EX_CLIENTEDGE;
//	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	// forward focus to the view window
//	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
//	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
//		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CMainFrame::OnStart() 
{
	int		nCam	= 0;
	int		nFE		= 0;
	int		nMaxCam	= 3;
	int		nMaxFE	= 2;
	int		nSwitchTime = 0;

	BOOL	bSwitch	= FALSE;

	m_Values[0].SafeDeleteAll();
	m_Values[1].SafeDeleteAll();
	m_Values[2].SafeDeleteAll();
	m_Values[3].SafeDeleteAll();
	m_Values[4].SafeDeleteAll();
	m_Values[5].SafeDeleteAll();
	m_Values[6].SafeDeleteAll();
	m_Values[7].SafeDeleteAll();
	m_Values[8].SafeDeleteAll();

	m_Signal[0].Init(19, 0,  ODD, 0, 5, RGB(255,0,0));
	m_Signal[1].Init(20, 5,  EVEN, 0, 5, RGB(0,255,0));
	m_Signal[2].Init(21, 10, ODD, 0, 5, RGB(0,0,255));

	m_Mux.Init(2);

	m_Mux.SwitchSignalToOutput(m_Signal[0], 0);
	m_Mux.SwitchSignalToOutput(m_Signal[1], 1);
	m_Decoder[0].In(m_Mux.Out(0));
	m_Decoder[1].In(m_Mux.Out(1));

	// Simulationszeitraum: 1000ms
	for (int nTime = 1; nTime < 1000; nTime++)
	{
		// Hat der Zoran seine Umschalterlaubnis gegeben, und sind mindestens 20ms vergangen?
		if (bSwitch==2) // && (nTime - nSwitchTime > 25))
		{
			bSwitch = 0;
	
			// Frontend und Kamera wechseln
			nFE = (nFE + 1) % nMaxFE;
			nCam= (nCam + 1) % nMaxCam;
			
			// Analog am inaktiven Frontend umschalten
			m_Mux.SwitchSignalToOutput(m_Signal[nCam], nFE);
			m_Decoder[0].In(m_Mux.Out(0));
			m_Decoder[1].In(m_Mux.Out(1));
			m_Values[0].Add(new CValue(5, UNDEFINE, 0, RGB(0,0,0)));
		}

		m_Signal[0].Clock();
		m_Signal[1].Clock();
		m_Signal[2].Clock();
		m_Decoder[0].Clock();
		m_Decoder[1].Clock();

		// Daten in die Fifos schreiben
		m_Fifo[0].Push(m_Decoder[0].Out().Out());
		m_Fifo[1].Push(m_Decoder[1].Out().Out());

		m_Values[0].Add(new CValue(0, UNDEFINE, 0, RGB(0,0,0)));
		m_Values[1].Add(m_Signal[0].Out());
		m_Values[2].Add(m_Signal[1].Out());
		m_Values[3].Add(m_Signal[2].Out());
		m_Values[4].Add(m_Mux.Out(0).Out());
		m_Values[5].Add(m_Mux.Out(1).Out());
		m_Values[6].Add(m_Decoder[0].Out().Out());
		m_Values[7].Add(m_Decoder[1].Out().Out());
		m_Values[8].Add(m_Fifo[(nFE + 1) % nMaxFE].Pop());
		
		// Liegt am Ausgang des aktiven FE ein Syncsignal? Dann darf digital umgeschaltet werden
		CValue* pValue = m_Values[8].GetAt(m_Values[8].GetSize()-1);
		if (pValue && pValue->GetInt() == HI_LEVEL)
		{
			bSwitch++;
			nSwitchTime = nTime;
		}
	}

	InvalidateRect(NULL);
	UpdateWindow();
}

void CMainFrame::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rcClient;
	GetClientRect(rcClient);
	
	int nRowHeight = rcClient.Height() / 10;
	int nYOffset = rcClient.bottom-nRowHeight/2;
/*	
	for (int nJ = 0; nJ< m_Values[0].GetSize(); nJ+=101)
	{
		double dx = (double)rcClient.Width() / (double)m_Values[0].GetSize();
		CPen pen(PS_DASH, 1, RGB(192,192,192));
		dc.SelectObject(pen);
		int nX = (int)((double)nJ*dx)-1;
		dc.MoveTo(CPoint(nX,rcClient.bottom));
		dc.LineTo(CPoint(nX,rcClient.top));
	}		
*/
	for (int nI = 0; nI < 9; nI++)
	{
		dc.TextOut(0, nYOffset-10, m_Values[nI].GetName());
		if (m_Values[nI].GetSize() > 0)
		{
			double dx = (double)rcClient.Width() / (double)m_Values[nI].GetSize();
			
			dc.MoveTo(CPoint(0,nYOffset));
			for (int nJ = 0; nJ < m_Values[nI].GetSize(); nJ++)
			{
				CValue* pValue = m_Values[nI].GetAt(nJ);
				if (pValue)
				{
					int nX = (int)((double)nJ*dx);
					int nY = nYOffset - 10 * pValue->GetInt();

					CPen pen(PS_SOLID, 1, pValue->GetColor());
					dc.SelectObject(pen);

					dc.LineTo(CPoint(nX, nY));
					if (pValue->GetInt() == HI_LEVEL)
					{
#if (0)
						CString sMsg;
						sMsg.Format("%d", pValue->GetSyncCounter());	
						dc.TextOut(nX-4, nYOffset, sMsg);
#else
						if (pValue->GetField() == ODD)
							dc.TextOut(nX+2, nY, "O");
						if (pValue->GetField() == EVEN)
							dc.TextOut(nX+2, nY, "E");
#endif
					}
					if ((nI == 0) && (pValue->GetInt() == HI_LEVEL))
					{
						CPen pen(PS_DASH, 1, RGB(192,192,192));
						dc.SelectObject(pen);
						dc.MoveTo(CPoint(nX,rcClient.bottom));
						dc.LineTo(CPoint(nX,rcClient.top));
						dc.MoveTo(CPoint(nX,nYOffset));
					}
				
				}
				else
				{
					CPen pen(PS_DASH, 1, RGB(192,192,192));
					dc.SelectObject(pen);
					int nX = (int)((double)nJ*dx);
					int nY = nYOffset;
					dc.LineTo(CPoint(nX, nY));
				}
			}

			nYOffset -= nRowHeight;
		}
	}
		
	// TODO: Add your message handler code here
	
	// Do not call CFrameWnd::OnPaint() for painting messages
}
