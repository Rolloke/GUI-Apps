// HostsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dvclient.h"
#include "HostsDlg.h"
#include "cpanel.h"
#include "CKeyboardDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
static const TCHAR szIP_Mask[] = _T("___.___.___.___");
static const TCHAR szName_Mask[] = _T("________________");
static const TCHAR szDigits[] = _T("0123456789");

/////////////////////////////////////////////////////////////////////////////
// CHostsDlg dialog
CHostsDlg::CHostsDlg(CWnd* pParent /*=NULL*/)
	: CTransparentDialog(CHostsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHostsDlg)
	//}}AFX_DATA_INIT
	m_hCursor		= NULL;
	m_pPanel		= (CPanel*)pParent;
	m_iSelectedItem = -1;
	m_pSelectedHost = NULL;
	m_pKeyboardDlg	= NULL;
}
/////////////////////////////////////////////////////////////////////////////
CHostsDlg::~CHostsDlg()
{
	WK_DELETE(m_pKeyboardDlg);
	m_Hosts.DeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
void CHostsDlg::DoDataExchange(CDataExchange* pDX)
{
	CTransparentDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHostsDlg)
	DDX_Control(pDX, IDC_HOSTS_FRAME_TOOLTIP, m_frameToolTip);
	DDX_Control(pDX, IDC_HOSTS_DISPLAY_TOOLTIP, m_displayToolTip);
	DDX_Control(pDX, IDC_HOSTS_DISPLAY_NAME, m_displayName);
	DDX_Control(pDX, IDC_HOSTS_FRAME_NAME, m_frameName);
	DDX_Control(pDX, IDC_HOSTS_DISPLAY_IP, m_displayIP);
	DDX_Control(pDX, IDC_HOSTS_FRAME_IP, m_frameIP);
	DDX_Control(pDX, IDC_HOSTS_LIST, m_listHosts);
	DDX_Control(pDX, IDC_HOSTS_UP_START, m_btnUpStart);
	DDX_Control(pDX, IDC_HOSTS_UP, m_btnUp);
	DDX_Control(pDX, IDC_HOSTS_DOWN, m_btnDown);
	DDX_Control(pDX, IDC_HOSTS_DOWN_END, m_btnDownEnd);
	DDX_Control(pDX, IDC_HOSTS_NEW, m_btnNew);
	DDX_Control(pDX, IDC_HOSTS_DELETE, m_btnDelete);
	DDX_Control(pDX, IDC_HOSTS_OK, m_btnOK);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CHostsDlg, CTransparentDialog)
	//{{AFX_MSG_MAP(CHostsDlg)
	ON_BN_CLICKED(IDC_HOSTS_FRAME_NAME, OnEditName)
	ON_BN_CLICKED(IDC_HOSTS_FRAME_IP, OnEditIP)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_HOSTS_LIST, OnItemchangedHostsList)
	ON_BN_CLICKED(IDC_HOSTS_NEW, OnNew)
	ON_BN_CLICKED(IDC_HOSTS_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_HOSTS_DOWN, OnDown)
	ON_BN_CLICKED(IDC_HOSTS_DOWN_END, OnDownEnd)
	ON_BN_CLICKED(IDC_HOSTS_UP, OnUp)
	ON_BN_CLICKED(IDC_HOSTS_UP_START, OnUpStart)
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_HOSTS_OK, OnOK)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
BOOL CHostsDlg::ValidateIP(const CString &sIP)
{
	char	sRest[1024]  = {0};
	int		a,b,c,d;
	BOOL	bRet = FALSE;

	if (sIP.IsEmpty() || (sIP.GetLength() != 12))
		return FALSE;

	a=-1;
	b=-1;
	c=-1;
	d=-1;
	_stscanf(sIP, _T("%03d%03d%03d%03d%s"), &a,&b,&c,&d,&sRest[0]);
	if ((a<0) || (a>255) ||
		(b<0) || (b>255) ||
		(c<0) || (c>255) ||
		(d<0) || (d>255))
	{
		return FALSE;
	}
	else
	{
		if (((CString)sRest).IsEmpty())
			bRet = (sIP.SpanIncluding(_T("1234567890.")) == sIP);
		else 
			bRet = FALSE;
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CHostsDlg::EnableUpDownButtons()
{
	BOOL bEnable = FALSE;
	int iTopIndex = m_listHosts.GetTopIndex();
	int iVisibleCount = m_listHosts.GetCountPerPage();
	int iLastVisibleIndex = iTopIndex + iVisibleCount - 1;
	int iMaxIndex = m_listHosts.GetItemCount() - 1;
	bEnable = (iTopIndex > 0) ? TRUE : FALSE;
	m_btnUpStart.EnableWindow(bEnable);
	m_btnUp.EnableWindow(bEnable);
	bEnable = (iLastVisibleIndex < iMaxIndex) ? TRUE : FALSE;
	m_btnDown.EnableWindow(bEnable);
	m_btnDownEnd.EnableWindow(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CHostsDlg::WaitToReadErrorMessage()
{
	Beep(250, 250);
	Beep(440, 250);

//	Sleep(1500);
}
/////////////////////////////////////////////////////////////////////////////
// CHostsDlg message handlers
BOOL CHostsDlg::OnInitDialog()
{
	CTransparentDialog::OnInitDialog();
	
	m_hCursor = theApp.m_hArrow;

	CreateTransparent(StyleBackGroundColorChangeBrushed, CPanel::m_BaseColorBackGrounds);
	
	// Farben des Displays setzen
	m_displayName.OpenStockDisplay(this, Display2);
	m_displayIP.OpenStockDisplay(this, Display2);
	m_displayToolTip.OpenStockDisplay(this, Display2);
	m_Displays.Add(&m_displayName);
	m_Displays.Add(&m_displayIP);
	m_Displays.Add(&m_displayToolTip);
	DSPCOL DisplayColors;
	DisplayColors.colBackGround = CPanel::m_BaseColorDisplays;
	DisplayColors.colBorder		= DisplayColors.colBackGround;
	DisplayColors.colText		= SKIN_COLOR_BLACK;
	DisplayColors.colShadow		= SKIN_COLOR_GREEN_SHADOW;
	DisplayColors.dwFlags		= DSP_COL_TEXT | DSP_COL_BACKGROUNND;// | DSP_COL_BORDER | DSP_COL_SHADOW;
	m_Displays.SetColors(DisplayColors);
	m_displayIP.SetTextAllignment(DT_RIGHT);

	m_frameName.CreateFrame(this);
	m_frameName.EnableShadow(TRUE);
	m_frameIP.CreateFrame(this);
	m_frameIP.EnableShadow(TRUE);
	m_frameToolTip.CreateFrame(this);
	m_frameToolTip.EnableShadow(TRUE);

	// Farben des Frames setzen
	FRAMECOL FrameColors;
	FrameColors.colBackGround	= DisplayColors.colBackGround;
	FrameColors.dwFlags			= FRAME_COL_BACKGROUNND;
	m_frameName.SetFrameColors(FrameColors);
	m_frameIP.SetFrameColors(FrameColors);
	m_frameToolTip.SetFrameColors(FrameColors);

	m_frameName.EnableActionOnButtonUp();
	m_frameIP.EnableActionOnButtonUp();

	// Farben der Listbox setzen
	m_listHosts.SetBackgroundColorNormal(DisplayColors.colBackGround);
	m_listHosts.SetBackgroundColorHighlighted(SKIN_COLOR_GREEN_SHADOW);
	m_listHosts.SetTextColorHighlighted(DisplayColors.colText);
	m_listHosts.SetTextColorNormal(DisplayColors.colText);
/*
	m_listHosts.SetBackgroundColorHighlighted(RGB(max(GetRValue(DisplayColors.colBackGround)-30,0),
											     max(GetGValue(DisplayColors.colBackGround)-30,0),
												 max(GetBValue(DisplayColors.colBackGround)-30,0)));
*/
	m_listHosts.SetFont(*(m_listHosts.GetFont()));

	// Buttons initialisieren
	m_Buttons.Add(&m_btnUpStart);
	m_Buttons.Add(&m_btnUp);
	m_Buttons.Add(&m_btnDown);
	m_Buttons.Add(&m_btnDownEnd);
	m_Buttons.Add(&m_btnNew);
	m_Buttons.Add(&m_btnDelete);
	m_Buttons.Add(&m_btnOK);
	m_Buttons.SetBaseColor(CPanel::m_BaseColorButtons);
	
	m_ilUpStart.Create(IDB_UP_START, 16, 0, SKIN_COLOR_KEY);
	m_btnUpStart.SetImageList(&m_ilUpStart);
	m_ilUp.Create(IDB_UP, 16, 0, SKIN_COLOR_KEY);
	m_btnUp.SetImageList(&m_ilUp);
	m_ilDown.Create(IDB_DOWN, 16, 0, SKIN_COLOR_KEY);
	m_btnDown.SetImageList(&m_ilDown);
	m_ilDownEnd.Create(IDB_DOWN_END, 16, 0, SKIN_COLOR_KEY);
	m_btnDownEnd.SetImageList(&m_ilDownEnd);
	m_ilNew.Create(IDB_NEW, 16, 0, SKIN_COLOR_KEY);
	m_btnNew.SetImageList(&m_ilNew);
	m_ilDelete.Create(IDB_DEL, 16, 0, SKIN_COLOR_KEY);
	m_btnDelete.SetImageList(&m_ilDelete);
	m_ilOK.Create(IDB_ENTER, 16, 0, SKIN_COLOR_KEY);
	m_btnOK.SetImageList(&m_ilOK);

	m_sHost = _T("");
	m_sIP = _T("");
	m_displayName.SetDisplayText(m_sHost);
	m_displayIP.SetDisplayText(m_sIP);

	// Die zusätzliche Host-Liste laden
	CWK_Profile prof(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE,_T("DVRT\\DV\\ADDITIONALHOSTS"),_T(""));
	m_Hosts.Load(prof);

	// Liste initialisieren
	CRect rect;
	m_listHosts.GetClientRect(rect);
	int iWidth = rect.Width();
	int iFirstColumnWidth = min(iWidth/2, iWidth/2);
	m_listHosts.InsertColumnWithType(0, _T(""), SIDT_TEXT, LVCFMT_LEFT, iFirstColumnWidth, 0);
	m_listHosts.InsertColumnWithType(1, _T(""), SIDT_TEXT, LVCFMT_RIGHT, iWidth-iFirstColumnWidth, 0);

	int iRow;
	CHost* pHost = NULL;
	for (int i=0 ; i<m_Hosts.GetSize() ; i++)
	{
		pHost = m_Hosts.GetAtFast(i);
		iRow = m_listHosts.InsertItem(LVIF_TEXT|LVIF_PARAM, i, pHost->GetName(), 0, 0, 0, (LPARAM)pHost);
		m_listHosts.SetItem(iRow, 1, LVIF_TEXT, pHost->GetClearNumber(), 0,0,0,0);
	}
	if (m_listHosts.GetItemCount() > 0)
	{
		m_listHosts.SetItem(0, 0, LVIF_STATE,
							NULL, 0,
							LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED, 0);
		m_listHosts.ForceRedraw();
	}
	m_listHosts.PostMessage(WM_SETFOCUS, 0, 0);

	EnableUpDownButtons();

	CRect rcPanel;
	if (m_pPanel)
	{
		m_pPanel->GetWindowRect(rcPanel);

		CRect rcAlarmlistDlg;
		GetWindowRect(rcAlarmlistDlg);
		
		// Dialog oberhalb des Panels positionieren
		int nX	= rcPanel.right -rcAlarmlistDlg.Width();
		int	nY	= rcPanel.top - rcAlarmlistDlg.Height();

		if (nX < 0 || nY < 0)
		{
			// RTEDialog unterhalb des Panels positionieren
			nX	= rcPanel.left;
			nY	= rcPanel.bottom;
		}
		SetWindowPos(&wndTopMost, nX, nY, -1, -1, SWP_NOSIZE);
	}

	ShowWindow(SW_SHOW);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CHostsDlg::OnEditName()
{
	EditName();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHostsDlg::EditName()
{
	BOOL bReturn = FALSE;
	if (m_pSelectedHost && !m_pKeyboardDlg)
	{
		int iReturn = IDCANCEL;
		BOOL bLoop = TRUE;
		CString sName;
		while(bLoop)
		{
			m_pKeyboardDlg = new CKeyboardDlg(&m_displayName, szName_Mask, m_sHost, this);
			if (m_pKeyboardDlg)
			{
				m_displayName.EnableBlinking(500);
				iReturn = m_pKeyboardDlg->DoModal();
				m_displayName.DisableBlinking();
				if (iReturn == IDOK)
				{
					sName = m_pKeyboardDlg->GetInputString();
					if (!sName.IsEmpty())
					{
						bLoop = FALSE;
						bReturn = TRUE;
						m_sHost = sName;
						m_pSelectedHost->SetName(m_sHost);
						m_listHosts.SetItemText(m_iSelectedItem,0,m_sHost);
					}
					else // Abbruch, wenn Leerstring
					{
						bLoop = FALSE;
						bReturn = FALSE;
					}
				}
				else
				{
					// canceled
					bLoop = FALSE;
					bReturn = FALSE;
				}

			}
			WK_DELETE(m_pKeyboardDlg);
			m_displayName.SetDisplayText(m_sHost);
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CHostsDlg::OnEditIP()
{
	EditIP();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHostsDlg::EditIP()
{
	BOOL bReturn = FALSE;
	if (m_pSelectedHost && !m_pKeyboardDlg)
	{
		int iReturn = IDCANCEL;
		BOOL bLoop = TRUE;
		CString sIP;
		while(bLoop)
		{
			if (!m_sIP.IsEmpty())
			{
				// Für die Darstellung im Display den IP-Eintrag auf jeweils 3 Stellen pro Segment erweitern
				int a=0,b=0,c=0,d=0;
				_stscanf(m_sIP, _T("%d.%d.%d.%d.%s"), &a,&b,&c,&d);
				sIP.Format(_T("%03d.%03d.%03d.%03d"), a, b, c, d);
			}
			else
				sIP = m_sIP;

			// Virtielle Tastatur öffnen
			m_pKeyboardDlg = new CKeyboardDlg(&m_displayIP,
												szIP_Mask,
												StringOnlyCharsInSet(sIP, szDigits),
												this);
			if (m_pKeyboardDlg)
			{
				m_displayIP.EnableBlinking(500);
				iReturn = m_pKeyboardDlg->DoModal();
				m_displayIP.DisableBlinking();
				if (iReturn == IDOK)
				{
					sIP = m_pKeyboardDlg->GetInputString();
					if (!sIP.IsEmpty())
					{
						if (ValidateIP(sIP))
						{
							bLoop = FALSE;
							bReturn = TRUE;
							m_sIP.Format(_T("%d.%d.%d.%d"),	_ttoi(sIP.Mid(0, 3)),
														_ttoi(sIP.Mid(3, 3)),
														_ttoi(sIP.Mid(6, 3)),
														_ttoi(sIP.Mid(9, 3)));
							sIP = _T("tcp:") + m_sIP;
							m_pSelectedHost->SetNumber(sIP);
							m_listHosts.SetItemText(m_iSelectedItem,1,m_sIP);
						}
						else
						{
							WaitToReadErrorMessage();
						}
					}
					else // Abbruch, wenn Leerstring
					{
						bLoop = FALSE;
						bReturn = FALSE;
					}
				}
				else
				{
					bLoop = FALSE;
					bReturn = FALSE;
				}
			}
			WK_DELETE(m_pKeyboardDlg);
			m_displayIP.SetDisplayText(m_sIP);
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CHostsDlg::OnItemchangedHostsList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (pNMListView->iItem!=m_iSelectedItem)
	{
		CString s;
		UpdateData(TRUE);
		m_pSelectedHost = (CHost*)pNMListView->lParam;
		m_iSelectedItem = pNMListView->iItem;
		m_sHost = m_pSelectedHost->GetName();
		m_sIP	= m_pSelectedHost->GetClearNumber();
		m_displayName.SetDisplayText(m_sHost);
		m_displayIP.SetDisplayText(m_sIP);
	}
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CHostsDlg::OnUpStart() 
{
	m_listHosts.ScrollToStart();
	EnableUpDownButtons();
}
/////////////////////////////////////////////////////////////////////////////
void CHostsDlg::OnUp() 
{
	m_listHosts.ScrollPageUp();
	EnableUpDownButtons();
}
/////////////////////////////////////////////////////////////////////////////
void CHostsDlg::OnDown() 
{
	m_listHosts.ScrollPageDown();
	EnableUpDownButtons();
}
/////////////////////////////////////////////////////////////////////////////
void CHostsDlg::OnDownEnd() 
{
	m_listHosts.ScrollToEnd();
	EnableUpDownButtons();
}
/////////////////////////////////////////////////////////////////////////////
void CHostsDlg::OnNew()
{
	CHost* pHost = m_Hosts.AddHost();

	if (pHost)
	{
		m_sHost	= pHost->GetName();
		m_sIP	= pHost->GetNumber();
		int iNew = m_listHosts.InsertItem(LVIF_TEXT|LVIF_PARAM,
										  m_listHosts.GetItemCount(),
										  m_sHost,
										  0,
										  0,
										  0,
										  (LPARAM)pHost);
		if (iNew != -1)
		{
			m_listHosts.SetItem(iNew, 0, LVIF_STATE,
								NULL, 0,
								LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED, 0);
			m_listHosts.ForceRedraw();
			m_pSelectedHost = pHost;
			m_iSelectedItem = iNew;
			BOOL bOK = FALSE;
			if (EditName())
				if (EditIP())
				{
					bOK = TRUE;
					EnableUpDownButtons();
				}
			if (!bOK)
			{
				OnDelete();
			}
		}
		else
		{
			m_Hosts.DeleteHost(pHost);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CHostsDlg::OnDelete()
{
	if ( (m_iSelectedItem==-1) || (m_pSelectedHost==NULL))
	{
		return;
	}

	m_Hosts.DeleteHost(m_pSelectedHost);
	m_pSelectedHost = NULL;
	
	// calc new selection
	int	newCount = m_listHosts.GetItemCount()-1;	// pre-subtract before DeleteItem
	int i = m_iSelectedItem;	// new selection

	// range check
	if (i > newCount-1 ) {	// outside ?
		i=newCount-1;
	} else if (newCount==0) {	// last one ?
		i = -1;
	}

	if (m_listHosts.DeleteItem(m_iSelectedItem))	// remove from listCtrl
	{
		CString sTyp;
		m_iSelectedItem = i;
		if (m_iSelectedItem != -1)
		{
			m_listHosts.SetItemState(m_iSelectedItem,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
			m_pSelectedHost = (CHost*)m_listHosts.GetItemData(m_iSelectedItem);
			m_sHost = m_pSelectedHost->GetName();
			m_sIP = m_pSelectedHost->GetClearNumber();
		}
		else
		{
			m_sHost = _T("");
			m_sIP = _T("");
		}
		m_displayName.SetDisplayText(m_sHost);
		if (m_sIP.IsEmpty())
		{
			m_displayIP.SetDisplayText(m_sIP);
		}
		else
		{
			m_displayIP.SetDisplayText(m_sIP);
		}

		m_listHosts.SetFocus();
		EnableUpDownButtons();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CHostsDlg::OnOK()
{
	// Die  Host-Liste speichern
	CWK_Profile prof(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE,_T("DVRT\\DV\\ADDITIONALHOSTS"),_T(""));
	m_Hosts.Save(prof);

	CTransparentDialog::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
void CHostsDlg::OnCancel()
{
	// Simply ignore it
}
/////////////////////////////////////////////////////////////////////////////
void CHostsDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	BOOL bHandled = FALSE;
	CWnd* pWnd = ChildWindowFromPoint(point);
	if (   pWnd
		&& pWnd != this
		)
	{
		if (pWnd == &m_displayName)
		{
			EditName();
			bHandled = TRUE;
		}
		else if (pWnd == &m_displayIP)
		{
			EditIP();
			bHandled = TRUE;
		}
	}

	if (!bHandled)
	{
		CTransparentDialog::OnLButtonUp(nFlags, point);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHostsDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_hCursor)
	{
		if (SetCursor(m_hCursor) != NULL)
			return TRUE;		
	}	
	
	return CTransparentDialog::OnSetCursor(pWnd, nHitTest, message);
}
