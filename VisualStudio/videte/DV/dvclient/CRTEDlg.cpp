// CRTEDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dvclient.h"
#include "CRTEDlg.h"
#include "wkclasses\WK_RuntimeErrors.h"
#include "CPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
									         
extern CDVClientApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRTEDlg dialog


/////////////////////////////////////////////////////////////////////////////
CRTEDlg::CRTEDlg(CWnd* pParent /*=NULL*/)
	: CTransparentDialog(CRTEDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRTEDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pPanel	= pParent;
	m_BaseColor = CPanel::m_BaseColorBackGrounds;
	CString sHomePath = theApp.GetHomeDir();

#ifdef _DEBUG
	sHomePath = _T("C:\\DV");
#endif	
	m_RTErrors.LoadFromFile(sHomePath+_T("\\")+RTE_FILENAME);

	m_nStartPos = max(m_RTErrors.GetSize()-1, 0);
	m_hCursor = NULL;
	m_sConfirm.LoadString(IDS_CONFIRM_RTE);
}

/////////////////////////////////////////////////////////////////////////////
CRTEDlg::~CRTEDlg()
{
	m_RTErrors.SafeDeleteAll();
}

/////////////////////////////////////////////////////////////////////////////
void CRTEDlg::DoDataExchange(CDataExchange* pDX)
{
	CTransparentDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRTEDlg)
	DDX_Control(pDX, IDC_RTE_PRINT, m_ctrlPrint);
	DDX_Control(pDX, IDC_RTE_TIMESTAMP, m_ctrlTimestampDisplay);
	DDX_Control(pDX, IDC_RTE_CONF_DISPLAY, m_ctrlConfirmDisplay);
	DDX_Control(pDX, IDC_RTE_CONFIRM, m_ctrlConfirm);
	DDX_Control(pDX, IDC_RTE_OK, m_ctrlOK);
	DDX_Control(pDX, IDC_RTE_UP, m_ctrlUp);
	DDX_Control(pDX, IDC_RTE_DOWN, m_ctrlDown);
	DDX_Control(pDX, IDC_RTE_DEL, m_ctrlDel);
	DDX_Control(pDX, IDC_RTE_DEL_ALL, m_ctrlDelAll);
	DDX_Control(pDX, IDC_RTE_DISPLAY, m_ctrlDisplay);
	DDX_Control(pDX, IDC_RTE_BORDER, m_ctrlBorder);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CRTEDlg, CTransparentDialog)
	//{{AFX_MSG_MAP(CRTEDlg)
	ON_BN_CLICKED(IDC_RTE_DEL, OnRteDel)
	ON_BN_CLICKED(IDC_RTE_DOWN, OnRteDown)
	ON_BN_CLICKED(IDC_RTE_UP, OnRteUp)
	ON_BN_CLICKED(IDC_RTE_CONFIRM, OnRteConfirm)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_RTE_OK, OnCloseDlg)
	ON_WM_SETCURSOR()
	ON_BN_CLICKED(IDC_RTE_DEL_ALL, OnRteDelAll)
	ON_BN_CLICKED(IDC_RTE_PRINT, OnRtePrint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRTEDlg message handlers

/////////////////////////////////////////////////////////////////////////////
BOOL CRTEDlg::OnInitDialog() 
{
	CTransparentDialog::OnInitDialog();
	
	CreateTransparent(StyleBackGroundColorChangeBrushed, CPanel::m_BaseColorBackGrounds);
	
	m_ctrlTimestampDisplay.OpenStockDisplay(this, Display2);
	m_ctrlDisplay.OpenStockDisplay(this, Display2);
	m_ctrlConfirmDisplay.OpenStockDisplay(this, Display2);

	// Kontrast erhöhen
	DSPCOL DisplayColors;
	DisplayColors.colText		= SKIN_COLOR_BLACK;
	DisplayColors.colBackGround = CPanel::m_BaseColorDisplays; // Background Green Color
	
	// OEM Spezifische Einstellungen
	// Alarmcom und ProtectionOne bekommen eine andere Displayfarbe
	if ((m_pPanel && ((CPanel*)m_pPanel)->GetOem() == OemAlarmCom) ||
		(m_pPanel && ((CPanel*)m_pPanel)->GetOem() == OemPOne))
		DisplayColors.colBackGround	= SKIN_COLOR_BLUE; // Background Blue Color

	DisplayColors.colBorder		= DisplayColors.colBackGround;
	DisplayColors.dwFlags = DSP_COL_TEXT | DSP_COL_BACKGROUNND | DSP_COL_BORDER;
	m_ctrlTimestampDisplay.SetDisplayColors(DisplayColors);
	m_ctrlDisplay.SetDisplayColors(DisplayColors);
	m_ctrlConfirmDisplay.SetDisplayColors(DisplayColors);

	m_ctrlBorder.CreateFrame(this);
	m_ctrlBorder.EnableShadow(TRUE);
		
	FRAMECOL FrameColors;
	FrameColors.dwFlags = FRAME_COL_BACKGROUNND;
	FrameColors.colBackGround = DisplayColors.colBackGround;
	m_ctrlBorder.SetFrameColors(FrameColors);
 
	m_ctrlDisplay.SetTextAllignment(DT_LEFT|DT_WORDBREAK);
		  
	m_RTEButtons.Add(&m_ctrlOK);
	m_RTEButtons.Add(&m_ctrlDel);
	m_RTEButtons.Add(&m_ctrlDelAll);
	m_RTEButtons.Add(&m_ctrlUp);
	m_RTEButtons.Add(&m_ctrlDown);
	m_RTEButtons.Add(&m_ctrlConfirm);
 	m_RTEButtons.Add(&m_ctrlPrint);

	m_ctrlUp.EnableActionOnButtonDown();
	m_ctrlDown.EnableActionOnButtonDown();
	m_ctrlUp.EnableAutoRepeat();
	m_ctrlDown.EnableAutoRepeat();

	m_RTEButtons.SetBaseColor(m_BaseColor);

	m_ilUp.Create(IDB_UP, 16, 0, SKIN_COLOR_KEY);
	m_ctrlUp.SetImageList(&m_ilUp);

	m_ilDown.Create(IDB_DOWN, 16, 0, SKIN_COLOR_KEY);
	m_ctrlDown.SetImageList(&m_ilDown);

	m_ilOK.Create(IDB_ENTER, 16, 0, SKIN_COLOR_KEY);
	m_ctrlOK.SetImageList(&m_ilOK);

	m_ilDel.Create(IDB_DEL, 16, 0, SKIN_COLOR_KEY);
	m_ctrlDel.SetImageList(&m_ilDel);
	
	m_ilDelAll.Create(IDB_DEL_ALL, 41, 0, SKIN_COLOR_KEY);
	m_ctrlDelAll.SetImageList(&m_ilDelAll);

	m_ilConfirm.Create(IDB_CONFIRM, 16, 0, SKIN_COLOR_KEY);
	m_ctrlConfirm.SetImageList(&m_ilConfirm);

	m_ilPrint.Create(IDB_PRINT, 24, 0, SKIN_COLOR_KEY);
	m_ctrlPrint.SetImageList(&m_ilPrint);

	m_hCursor = theApp.m_hArrow;

	if (m_pPanel)
	{
		// Drucker vorhanden?
		if (!((CPanel*)m_pPanel)->IsPrinterAvailable())
			m_ctrlPrint.ShowWindow(SW_HIDE);
		// Drucker ausgewählt?
		if (((CPanel*)m_pPanel)->GetDefaultPrinter() == 0)
			m_ctrlPrint.EnableWindow(FALSE);
	}

	ShowRTErrors();

	CRect rcPanel;
	if (m_pPanel)
	{
		m_pPanel->GetWindowRect(rcPanel);

		CRect rcRTEDlg;
		GetWindowRect(rcRTEDlg);
		
		// RTEDialog oberhalb des Panels positionieren
		int nX	= rcPanel.left;
		int	nY	= rcPanel.top - rcRTEDlg.Height();

		if (nX < 0 || nY < 0)
		{
			// RTEDialog unterhalb des Panels positionieren
			nX	= rcPanel.left;
			nY	= rcPanel.bottom;
		}
		SetWindowPos(&wndTopMost, nX, nY, -1, -1, SWP_NOSIZE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CRTEDlg::OnRteDel() 
{
	if (m_nStartPos < m_RTErrors.GetSize())
	{
		CWK_RunTimeError* pRTE = m_RTErrors.GetAtFast(m_nStartPos);
		if (pRTE)
		{
			CString sErrorText	= pRTE->GetFormattedTimeStamp() + _T(" ") + pRTE->GetErrorText();
			WK_TRACE(_T("Runtimeerror <%s> deleted by User\n"), sErrorText);
			
			m_RTErrors.RemoveAt(m_nStartPos);
			m_RTErrors.WriteToFile(theApp.GetHomeDir()+_T("\\")+RTE_FILENAME);
			m_nStartPos = max(min(m_nStartPos, m_RTErrors.GetSize()-1),0);
			
			ShowRTErrors();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CRTEDlg::OnRteDelAll() 
{
	WK_TRACE(_T("Deleting all Runtimeerrors by User\n"));
	
	m_RTErrors.DeleteAll();

	m_RTErrors.WriteToFile(theApp.GetHomeDir()+_T("\\")+RTE_FILENAME);
	m_nStartPos = 0;
	ShowRTErrors();
}

/////////////////////////////////////////////////////////////////////////////
void CRTEDlg::OnRteDown() 
{
	m_nStartPos--;
	m_nStartPos = max(m_nStartPos, 0);

	ShowRTErrors();
}

/////////////////////////////////////////////////////////////////////////////
void CRTEDlg::OnRteUp() 
{
	m_nStartPos++;
	m_nStartPos = min(m_nStartPos, m_RTErrors.GetSize()-1);

	ShowRTErrors();
}

/////////////////////////////////////////////////////////////////////////////
void CRTEDlg::ShowRTErrors()
{  
	CString sError		= _T("");
	CString sTimestamp	= _T("");
	CString sConfirm	= _T("");

	if (m_nStartPos < m_RTErrors.GetSize())
	{
		CWK_RunTimeError* pRTE = m_RTErrors.GetAtFast(m_nStartPos);
	
		// Übersichtliche Fehlermeldung bauen
		CString sMsg;
		sMsg.Format(IDS_RTE, m_nStartPos+1);

		sTimestamp.Format(_T("%s %s"), sMsg, pRTE->GetFormattedTimeStamp());
		sError		= pRTE->GetErrorText();

		// Ist die Fehlermeldung schon quittiert?
		int nIndex = sError.Find(RTE_CONFIRMED, 0);
		if (nIndex != -1)
		{
			// RTE_CONFIRMED entfernen und Quittierungsmeldung ausgeben
			sError = sError.Left(nIndex); 
			sConfirm = m_sConfirm;		
			m_ctrlConfirm.EnableWindow(FALSE);
		}
		else
		{
			m_ctrlConfirm.EnableWindow(TRUE);
		}
	}
	
	m_ctrlTimestampDisplay.SetDisplayText(sTimestamp);
	m_ctrlDisplay.SetDisplayText(sError);
	m_ctrlConfirmDisplay.SetDisplayText(sConfirm);		

	if (m_nStartPos <= 0)
		m_ctrlDown.EnableWindow(FALSE);
	else
		m_ctrlDown.EnableWindow(TRUE);

	if (m_nStartPos >= m_RTErrors.GetSize()-1)
		m_ctrlUp.EnableWindow(FALSE);
	else
		m_ctrlUp.EnableWindow(TRUE);

	if (m_RTErrors.GetSize() == 0)
	{
		m_ctrlDel.EnableWindow(FALSE);
		m_ctrlDelAll.EnableWindow(FALSE);
		m_ctrlConfirm.EnableWindow(FALSE);
		m_ctrlPrint.EnableWindow(FALSE);
	}

	// Piezo evtl. neu setzen
	CWnd* pWnd = FindWindow(NULL, _T("DVStarter"));
	if (WK_IS_WINDOW(pWnd))
   		pWnd->PostMessage(WM_CHECK_RTE);
}

/////////////////////////////////////////////////////////////////////////////
void CRTEDlg::OnRteConfirm() 
{
	if (m_nStartPos < m_RTErrors.GetSize())
	{
		CWK_RunTimeError* pRTE = m_RTErrors.GetAtFast(m_nStartPos);
		if (pRTE)
		{
			CString sErrorText	= pRTE->GetFormattedTimeStamp() + _T(" ") + pRTE->GetErrorText();
			if (sErrorText.Find(RTE_CONFIRMED, 0) == -1)
			{	   
				WK_TRACE(_T("Runtimeerror <%s> was confirmed\n"), sErrorText);

				// Runtimeerror quittieren.
				pRTE->SetErrorText(pRTE->GetErrorText() + RTE_CONFIRMED);
				m_RTErrors.WriteToFile(theApp.GetHomeDir()+_T("\\")+RTE_FILENAME);

				ShowRTErrors();
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CRTEDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	CPoint CurPoint(0,0);

	GetCursorPos(&CurPoint);
	CWnd* pWnd = WindowFromPoint(CurPoint);
	CString s;
	if (WK_IS_WINDOW(pWnd))
	{
		s.LoadString(pWnd->GetDlgCtrlID());
	}
	if (m_pPanel)
		((CPanel*)m_pPanel)->SetTooltipText(s);
	
	CTransparentDialog::OnMouseMove(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CRTEDlg::OnCloseDlg() 
{
	CTransparentDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CRTEDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_hCursor)
	{
		if (SetCursor(m_hCursor) != NULL)
			return TRUE;		
	}	
	return CTransparentDialog::OnSetCursor(pWnd, nHitTest, message);
}

/////////////////////////////////////////////////////////////////////////////
void CRTEDlg::OnRtePrint() 
{
	CString sError     = _T("");
	CString sTimestamp = _T("");
	CString sConfirm   = _T("");
	CString sRteNr     = _T("");
	CString sMsg       = _T("");

	CPrintDialog dlg(FALSE);
	dlg.m_pd.Flags &= ~PD_RETURNDC;
	if (dlg.GetDefaults())
	{
		DEVMODE *pDM = dlg.GetDevMode();
		pDM->dmOrientation  = DMORIENT_PORTRAIT;
		pDM->dmPrintQuality = DMRES_DRAFT;
		::GlobalUnlock(dlg.m_pd.hDevMode);

		HDC hDCPrint = dlg.CreatePrinterDC();
		if (hDCPrint)
		{
			CDC dcPrint;
			dcPrint.Attach(hDCPrint);
			dcPrint.m_bPrinting = TRUE;

			DOCINFO di;
			ZeroMemory(&di, sizeof(DOCINFO));
			di.cbSize = sizeof(DOCINFO);
			di.lpszDocName = _T("logevents");

			if (dcPrint.StartDoc(&di) != SP_ERROR)
			{
				if (dcPrint.StartPage() >= 0)
				{	
					CFont font;
//					font.CreatePointFont(8*10, _T("Arial"), &dcPrint);
					font.CreatePointFont(8*10, GetVarFontFaceName(), &dcPrint);
					CFont* pOldFont = (CFont*)dcPrint.SelectObject(&font);

					CSize  szPrint(dcPrint.GetDeviceCaps(HORZRES), dcPrint.GetDeviceCaps(VERTRES));
					CPoint ptOffset(dcPrint.GetDeviceCaps(PHYSICALOFFSETX), dcPrint.GetDeviceCaps(PHYSICALOFFSETY)); 
					CRect  rcPage(ptOffset, szPrint);
					rcPage.DeflateRect(200,200,200, 200);
/*
					CRect rcPage(0, 0, dcPrint.GetDeviceCaps(HORZRES), dcPrint.GetDeviceCaps(VERTRES));
*/
					rcPage.DeflateRect(200,200,200, 200);
					dcPrint.DPtoLP(&rcPage);
					int nYPos = 0;
					for (int nI = 0; nI < m_RTErrors.GetSize(); nI++)
					{
						CWK_RunTimeError* pRTE = m_RTErrors.GetAtFast(nI);

						// Übersichtliche Fehlermeldung bauen
						sRteNr.Format(IDS_RTE, nI+1);
						sTimestamp	= pRTE->GetFormattedTimeStamp();
						sError		= pRTE->GetErrorText();

						// Ist die Fehlermeldung schon quittiert?
						int nIndex = sError.Find(RTE_CONFIRMED, 0);
						if (nIndex != -1)
						{
							// RTE_CONFIRMED entfernen und Quittierungsmeldung ausgeben
							sError = sError.Left(nIndex); 
							sConfirm = m_sConfirm;		
						}
						sMsg.Format(_T("%s %s  '%s'  %s"), sRteNr, sTimestamp, sError, sConfirm);
			
						// Text ausgeben.
						CRect rcText(rcPage.left, rcPage.top+nYPos, rcPage.right, rcPage.top+nYPos);
						dcPrint.DrawText(sMsg, rcText, DT_CALCRECT | DT_LEFT|DT_WORDBREAK);
						dcPrint.DrawText(sMsg, rcText, DT_LEFT|DT_WORDBREAK);

						// Eine Zeile weiter
						nYPos += rcText.Height(); 
						
						// Seitenende erreicht?
						if (nYPos >= rcPage.Height()-rcText.Height())
						{
							nYPos = rcText.Height(); 
							dcPrint.EndPage();
							dcPrint.StartPage();
						}
					}
					dcPrint.EndPage();
					dcPrint.SelectObject(pOldFont);
					pOldFont->DeleteObject();

				}
				dcPrint.EndDoc();
			}
			dcPrint.Detach();
			DeleteDC(hDCPrint);
		}
	}
}

