// IPBookDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IPBook.h"
#include "IPBookDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIPBookDlg dialog

CIPBookDlg::CIPBookDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIPBookDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIPBookDlg)
	m_sIPList = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CIPBookDlg::~CIPBookDlg()
{
	 delete m_pSocket;
}


void CIPBookDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIPBookDlg)
	
	DDX_Control(pDX, IDC_IPLIST, m_CtrlIPList);
	DDX_LBString(pDX, IDC_IPLIST, m_sIPList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIPBookDlg, CDialog)
	//{{AFX_MSG_MAP(CIPBookDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(NM_CLICK, IDC_IPLIST, OnClickIplist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPBookDlg message handlers

BOOL CIPBookDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_pSocket = new CLocalIPGang(this);
	m_pSocket->Create(UDP);
//	m_pdcGraph = m_CtrlGraph.GetDC();
	
//	BOOL a=m_fileLog.Open("Bandwidth.log",CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_pSocket->SetMulticastAddress("234.5.6.7");
	m_pSocket->SetMulticastPort(815);
	CString sHeader;
	int iColWidth;
	LVCOLUMN LVColumn;

	CRect rect;
	m_CtrlIPList.GetClientRect(rect);

	iColWidth = (int)(rect.Width() / 5);

	sHeader = "IP-Address";

	LVColumn.mask       = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT;
	LVColumn.pszText    = (LPTSTR)(const char*)sHeader;		//Column Header
	LVColumn.fmt        = LVCFMT_LEFT;						//Column Alignment
	LVColumn.cchTextMax = sHeader.GetLength();				//Column Header size
	LVColumn.cx			= iColWidth;						//Column width

	m_CtrlIPList.InsertColumn(0,&LVColumn);

	sHeader = "Hostname";
	LVColumn.pszText    = (LPTSTR)(const char*)sHeader;		//Column Header
	LVColumn.cchTextMax = sHeader.GetLength();				//Column Header size

	m_CtrlIPList.InsertColumn(1,&LVColumn);

	sHeader = "Bandwidth";
	LVColumn.pszText    = (LPTSTR)(const char*)sHeader;		//Column Header
	LVColumn.cchTextMax = sHeader.GetLength();				//Column Header size

	m_CtrlIPList.InsertColumn(2,&LVColumn);

	sHeader = "Max";
	LVColumn.pszText    = (LPTSTR)(const char*)sHeader;		//Column Header
	LVColumn.cchTextMax = sHeader.GetLength();				//Column Header size
	LVColumn.cx			= (iColWidth/2);

	m_CtrlIPList.InsertColumn(3,&LVColumn);

	sHeader = "Min";
	LVColumn.pszText    = (LPTSTR)(const char*)sHeader;		//Column Header
	LVColumn.cchTextMax = sHeader.GetLength();				//Column Header size
	LVColumn.cx			= (iColWidth/2);

	m_CtrlIPList.InsertColumn(4,&LVColumn);

	sHeader = "Counts";
	LVColumn.pszText    = (LPTSTR)(const char*)sHeader;		//Column Header
	LVColumn.cchTextMax = sHeader.GetLength();				//Column Header size
	LVColumn.cx			= 85;

	m_CtrlIPList.InsertColumn(5,&LVColumn);




	LONG dw = ListView_GetExtendedListViewStyle(m_CtrlIPList.m_hWnd);
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(m_CtrlIPList.m_hWnd,dw);

	SetTimer(1,30000,NULL);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIPBookDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIPBookDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CIPBookDlg::AddAddress(CString sIP, CString sHost)
{


   
	LVFINDINFO info;
	int nIndex;
	CRect rect;
	info.flags = LVFI_PARTIAL|LVFI_STRING;
	info.psz = (LPTSTR)(const char*)sIP;


    if((nIndex=m_CtrlIPList.FindItem(&info)) == -1)
	{

		int i;
		int c = m_CtrlIPList.GetItemCount();
		i=m_CtrlIPList.InsertItem (LVIF_TEXT|LVIF_STATE,
								c,
								sIP,
								LVIS_SELECTED|LVIS_FOCUSED,
								0,0,0);

		

		m_CtrlIPList.SetItemText(i,1,sHost);

	}	

	SetForegroundWindow( );
	

}

void CIPBookDlg::OnTimer(UINT nIDEvent) 
{

	m_pSocket->SendRefresh();
	m_pSocket->CheckTimeouts();

	CDialog::OnTimer(nIDEvent);
}

void CIPBookDlg::OnDestroy() 
{

//	m_pSocket->LeaveGang();
//	DWORD dwRet = WaitForSingleObject(m_pSocket->m_DestroyEvent,5000);
//	m_fileLog.Close();
	CDialog::OnDestroy();
}

void CIPBookDlg::RemoveAddress(CString sIP, CString sHost)
{
	LVFINDINFO info;
	int nIndex;

	info.flags = LVFI_PARTIAL|LVFI_STRING;
	info.psz = (LPTSTR)(const char*)sIP;


    if((nIndex=m_CtrlIPList.FindItem(&info)) != -1)
	{
		m_CtrlIPList.DeleteItem(nIndex);
	}
}

void CIPBookDlg::SetLocalIP(CString sIP)
{
	SetWindowText("IPBook on "+ sIP);
}



void CIPBookDlg::SetBandwidth(CString sIP, int iThroughput)
{
	LVFINDINFO info;
	int nIndex;
	CRect rect;
	info.flags = LVFI_PARTIAL|LVFI_STRING;
	info.psz = (LPTSTR)(const char*)sIP;
	CString sThroughput;
	int icount=0;
	SYSTEMTIME ActTime;
	
	CTime time(CTime::GetCurrentTime());

	time.GetAsSystemTime( ActTime);
	BYTE *pbTime = (BYTE*)&ActTime;
	CString sLine;

	unsigned long lIP = inet_addr((const char*)sIP);
  	BYTE *pbIP =(BYTE*)&lIP;

	BYTE *pbThroughPut = (BYTE*)&iThroughput;


//	m_fileLog.Write(pbIP,sizeof(unsigned long)); 
//	m_fileLog.Write(pbTime,sizeof(SYSTEMTIME));
//	m_fileLog.Write(pbThroughPut,sizeof(SYSTEMTIME));

    if((nIndex=m_CtrlIPList.FindItem(&info)) != -1)
	{

		char ctext[4];
		int imin=0,imax=0;
		CString sText;

		m_CtrlIPList.GetItemText(nIndex,4,ctext,5);
		imin = atoi(ctext);
		m_CtrlIPList.GetItemText(nIndex,3,ctext,5);
		imax = atoi(ctext);
		m_CtrlIPList.GetItemText(nIndex,5,ctext,5);
		icount = atoi(ctext);


		if(icount==0)
		{
			icount=1;
			sText.Format("%i",icount);
			m_CtrlIPList.SetItemText(nIndex,5,sText);
		}
		else
		{
			icount++;
			sText.Format("%i",icount);
			m_CtrlIPList.SetItemText(nIndex,5,sText);
		}



		if(imin==0)
		{
			sText.Format("%i",iThroughput/1000);
			m_CtrlIPList.SetItemText(nIndex,4,sText);
			sText.Empty();
		}
		else
		{
			if((iThroughput/1000)<imin)
			{
				sText.Format("%i",iThroughput/1000);
				m_CtrlIPList.SetItemText(nIndex,4,sText);
				sText.Empty();
			}

		}
		if(imax==0)
		{
			sText.Format("%i",iThroughput/1000);
			m_CtrlIPList.SetItemText(nIndex,3,sText);
		}
		else
		{
			if((iThroughput/1000)>imax)
			{
				sText.Format("%i",iThroughput/1000);
				m_CtrlIPList.SetItemText(nIndex,3,sText);
			}

		}




		sThroughput.Format("%i kb/sec",iThroughput/1000);
		m_CtrlIPList.SetItemText(nIndex,2,sThroughput);





	}	


}

void CIPBookDlg::OnMouseMove(UINT nFlags, CPoint point) 
{

	
	CDialog::OnMouseMove(nFlags, point);
}

void CIPBookDlg::OnClickIplist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/*CPen pen;
	COLORREF color=RGB(255,0,255);

	pen.CreatePen(PS_SOLID,0,color);
	POSITION pos = m_CtrlIPList.GetFirstSelectedItemPosition();
	if (pos == NULL)
		TRACE0("No items were selected!\n");
	else
	{

	   GetDC()->SelectObject(pen);
	   int nItem = m_CtrlIPList.GetNextSelectedItem(pos);
	   CString sContent=m_CtrlIPList.GetItemText( nItem,0 );
	   GetDC()->MoveTo (0,100);
	   BOOL a=GetDC()->LineTo(20,300+100);
	   //Invalidate();
	}
   	  */
	*pResult = 0;
}

void CIPBookDlg::OnListening(CWinSocket *pSocket, UINT uPort)
{

}
