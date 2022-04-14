// viewcamDlg.cpp : implementation file
//

#include "stdafx.h"
#include "viewcam.h"
#include "viewcamDlg.h"

#include "IPCOutputViewcam.h"
#include "IPCamera.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewcamDlg dialog

CViewcamDlg::CViewcamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CViewcamDlg::IDD, pParent)
{

		//{{AFX_DATA_INIT(CViewcamDlg)
	m_eTarget_Host_Address = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	i_PictureCount=0;
	i_CameraNr=1;
	m_pCIPCOutputViewcam=NULL;
	m_PictureCount=1;


	
}

CViewcamDlg::~CViewcamDlg()
{

	WK_DELETE(m_pCIPCOutputViewcam);


}


void CViewcamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewcamDlg)

	DDX_Control(pDX, IDC_PRPICTURES, m_prPictures);
	DDX_Control(pDX, IDC_LIST_CAMERA, m_lbCamera);
	DDX_Text(pDX, IDC_EDIT_ADDRESS, m_eTarget_Host_Address);
	DDX_Text(pDX, IDC_EDIT_PICTURE_COUNT, m_PictureCount);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CViewcamDlg, CDialog)
	//{{AFX_MSG_MAP(CViewcamDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONNECTBUTTON, OnConnectbutton)
	ON_BN_CLICKED(IDC_DISCONNECT, OnDisconnect)
	ON_LBN_SELCHANGE(IDC_LIST_CAMERA, OnSelchangeListCamera)
	ON_BN_CLICKED(IDC_BUTTON_GETPICTURE, OnButtonGetpicture)
	ON_EN_CHANGE(IDC_EDIT_ADDRESS, OnChangeEditAddress)
	ON_EN_SETFOCUS(IDC_EDIT_ADDRESS, OnSetfocusEditAddress)
	ON_EN_CHANGE(IDC_EDIT_PICTURE_COUNT, OnChangeEditPictureCount)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewcamDlg message handlers

BOOL CViewcamDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CViewcamDlg::OnPaint() 
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
HCURSOR CViewcamDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CViewcamDlg::OnConnectbutton() 
{
		
		

		// TODO: Add your control notification handler code here
		CConnectionRecord c;
		c.SetDestinationHost(m_eTarget_Host_Address);
		c.SetPermission("SuperVisor");
		CIPCFetchResult fr = NewFetchOutputConnection(c);
		
		if (fr.GetError() == CIPC_ERROR_OKAY)
		{
			TRACE("connect ok with %s\n",fr.GetShmName());
			m_pCIPCOutputViewcam = new CIPCOutputViewcam(fr.GetShmName());
			m_pCIPCOutputViewcam->m_MainViewcam=1;
			CWnd *GetPictureBut=GetDlgItem(IDC_DISCONNECT);
			GetPictureBut->ShowWindow(SW_SHOW);
			GetPictureBut=GetDlgItem(IDC_CONNECTBUTTON);
			GetPictureBut->ShowWindow(SW_HIDE);
			
			
		}
		else
		{
			TRACE("Connection not ok:%s",fr.GetErrorMessage());
			m_eTarget_Host_Address ="! CONNECTION ERROR !";
			UpdateData(FALSE);

		}

	
}

void CViewcamDlg::OnDisconnect() 
{

	WK_DELETE(m_pCIPCOutputViewcam);
//	for(icount=1;icount<=10;icount++)WK_DELETE(Camera[icount]);
	
	m_lbCamera.ResetContent();
	CWnd *GetPictureBut=GetDlgItem(IDC_DISCONNECT);
			GetPictureBut->ShowWindow(SW_HIDE);
			GetPictureBut=GetDlgItem(IDC_CONNECTBUTTON);
			GetPictureBut->ShowWindow(SW_SHOW);
		
	// TODO: Add your control notification handler code here
	
}

void CViewcamDlg::OnSelchangeListCamera() 
{
		CString s;
		TRACE("gewählt: %i\n",m_lbCamera.GetCurSel());
		m_lbCamera.GetText(m_lbCamera.GetCurSel(),s);
		TRACE("string: %s\n",s);


					

}

void CViewcamDlg::OnButtonGetpicture() 
{
	if(m_lbCamera.GetSelCount()!=0){          // Auswahl wurde in Liste getroffen
	i_PictureCount=0;
	i_CameraNr=1;
	LPINT ipCameraIndex=new int[m_lbCamera.GetCount()];//Ptr auf Selektierte Kameras im Puffer

													//Anzahl selektierter Kameras in Liste
	iSelectCamera = m_lbCamera.GetSelItems(m_lbCamera.GetCount(),ipCameraIndex);
	
	m_prPictures.SetRange(0,(m_PictureCount*iSelectCamera)*100);//ProgressBar
	m_prPictures.SetStep(100);
	
	
	TRACE("%i Kameras selektiert\n",iSelectCamera);
	
													//Für 1. Kamera in Liste Bilder anfordern
	DWORD dwCam = m_lbCamera.GetItemData(*ipCameraIndex);
	CSecID camID = dwCam;
	
	m_pCIPCOutputViewcam->DoRequestNewJpegEncoding(camID, 
										RESOLUTION_HIGH, 
										COMPRESSION_5,
										1,
										1000);
	
	TRACE(", ID: %08lx\n",camID);
	
	ip_CameraIndex=&ipCameraIndex;	//Pointer auf Pointer Kamera-Index zur Weitervervendung
									//in OnConfirmJpeg...
									//(** um dynamische Speicherverwaltung (Kameras) zu 
									//ermöglichen)
	}
}

void CViewcamDlg::OnChangeEditAddress() 
{
	UpdateData(TRUE);
		
}

void CViewcamDlg::OnSetfocusEditAddress() 
{
	m_eTarget_Host_Address ="";
	m_eTarget_Host_Address="tcp:134.245.76.51";
	UpdateData(FALSE);

}



void CViewcamDlg::OnChangeEditPictureCount() 
{
	
	UpdateData(TRUE);
	
}

//DEL void CViewcamDlg::OnSelectCompress() 
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL 			
//DEL }

//DEL void CViewcamDlg::OnEditchangeSelectCompress() 
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL 	
//DEL }

//DEL void CViewcamDlg::OnSelectResolution() 
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL 	
//DEL }
