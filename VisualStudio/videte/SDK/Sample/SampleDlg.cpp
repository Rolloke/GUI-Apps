/******************************************************************************
|* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
|* ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
|* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
|* PARTICULAR PURPOSE.
|* 
|* Copyright 2005 videte IT ® AG. All Rights Reserved.
|*-----------------------------------------------------------------------------
|* videteSDK / Sample
|*
|* PROGRAM: SampleDlg.cpp
|*
|* PURPOSE: Client dialog interface; implementation file.
******************************************************************************/

#include "stdafx.h"
#include "Sample.h"
#include "SampleDlg.h"
#include ".\sampledlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSampleDlg::CSampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSampleDlg::IDD, pParent)
	, m_sPassword(_T(""))
	, m_sPin(_T("0000"))
	, m_dwIPAdress(3232235599) //192.168.0.79
	, m_sTextMsg(_T(""))
	, m_sIPAdress()
	, m_sDBText(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//create the control interface 
	m_pCIPCControlInterface = new CIPCControlInterface;

	//initialize members
	m_pCIPCDatabaseSample				= NULL;
	m_pCIPCOutputSample					= NULL;
	m_pCIPCInputSample					= NULL;
	m_secIDCurrentCamera				= ((DWORD)0);
	m_CurrentPlayStatus					= PS_STOP;
	m_wCurrentArchive					= 0;
	m_wCurrentSequence					= 0;
	m_dwCurrentRecord					= 0;
	m_bIsDTS							= FALSE;

	//get the default parameters
	NETUNITPARAM netUnitParam			= m_pCIPCControlInterface->GetNetUnitParam();
}
CSampleDlg::~CSampleDlg()
{	
}
/////////////////////////////////////////////////////////////////////////////
// message map
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSampleDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED	(IDC_BUTTON_CONNECT, OnBnClickedButtonConnect)
	ON_BN_CLICKED	(IDOK, OnBnClickedOk)
	ON_NOTIFY		(LVN_ITEMCHANGED, IDC_LIST_ARCHIVES, OnLvnItemchangedListArchives)
	ON_NOTIFY		(LVN_ITEMCHANGED, IDC_LIST_SEQUENCES, OnLvnItemchangedListSequences)
	ON_WM_DESTROY	()
	ON_BN_CLICKED	(IDC_BUTTON_STEP_FORWARD, OnBnClickedButtonStepForward)
	ON_BN_CLICKED	(IDC_BUTTON_DISCONNECT, OnBnClickedButtonDisconnect)
	ON_BN_CLICKED	(IDC_BUTTON_STEP_BACKWARD, OnBnClickedButtonStepBackward)
	ON_NOTIFY		(LVN_ITEMCHANGED, IDC_LIST_CAMERAS, OnLvnItemchangedListCameras)
	ON_BN_CLICKED	(IDC_BUTTON_PLAY_FORWARD, OnBnClickedButtonPlayForward)
	ON_BN_CLICKED	(IDC_BUTTON_PLAY_BACKWARD, OnBnClickedButtonPlayBackward)
	ON_BN_CLICKED	(IDC_BUTTON_FAST_FORWARD, OnBnClickedButtonFastForward)
	ON_BN_CLICKED	(IDC_BUTTON_FAST_BACKWARD, OnBnClickedButtonFastBackward)
END_MESSAGE_MAP()

/*********************************************************************************************
Description   : This function is called by the framework to exchange and validate dialog data.

Parameters:   
pDX	: (I): Pointer to the data exchence object. (CDataExchange*)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text		(pDX, IDC_EDIT_PASSWORD, m_sPassword);
	DDX_Text		(pDX, IDC_EDIT_PIN, m_sPin);
	DDX_IPAddress	(pDX, IDC_IPADDRESS, m_dwIPAdress);
	DDX_Control		(pDX, IDC_EDIT_PIN, m_ctrlPin);
	DDX_Text		(pDX, IDC_ERROR_CODE, m_sTextMsg);
	DDX_Control		(pDX, IDC_LIST_ARCHIVES, m_ListArchives);
	DDX_Control		(pDX, IDC_LIST_SEQUENCES, m_ListSequences);
	DDX_Control		(pDX, IDC_BUTTON_STEP_FORWARD, m_ctrlButtonStepForward);
	DDX_Control		(pDX, IDC_BUTTON_STEP_BACKWARD, m_ctrlButtonStepBackward);
	DDX_Control		(pDX, IDC_BUTTON_PLAY_FORWARD, m_ctrlButtonPlayForward);
	DDX_Control		(pDX, IDC_BUTTON_PLAY_BACKWARD, m_ctrlButtonPlayBackward);
	DDX_Control		(pDX, IDC_LIST_CAMERAS, m_ListCameras);
	DDX_Control		(pDX, IDC_LIVE_PICTURE, m_ctrlLivePicture);
	DDX_Control		(pDX, IDC_PLAY_PICTURE, m_ctrlPlayPicture);
	DDX_Control		(pDX, IDC_LIST_RELAYS, m_ListRelays);
	DDX_Control		(pDX, IDC_LIST_DETECTORS, m_ListDetectors);
	DDX_Control		(pDX, IDC_BUTTON_FAST_FORWARD, m_ctrlButtonFastForward);
	DDX_Control		(pDX, IDC_BUTTON_FAST_BACKWARD, m_ctrlButtonFastBackward);
	DDX_Text		(pDX, IDC_PICTURE_INFO, m_sPictureInfo);
	DDX_Text		(pDX, IDC_STATIC_DB_TEXT, m_sDBText);
	DDX_Control		(pDX, IDC_PICTURE_INFO, m_ctrlPictureInfo);
	DDX_Control		(pDX, IDC_STATIC_DB_TEXT, m_ctrlDBText);
}

/*********************************************************************************************
Description   : This function initializes the dialog.

Parameters:   none

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
BOOL CSampleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_ctrlPlayPicture.m_pSampleDlg = this;
	m_ctrlLivePicture.m_pSampleDlg = this;

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_ctrlPin.SetLimitText(4);	//limit the pin edit ctrl to 4 characters
	InitControls();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/*********************************************************************************************
Description   : This function draws the dialog.

Parameters:   none

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::OnPaint() 
{
	CDialog::OnPaint();
}

/*********************************************************************************************
Description   : The system calls this function to obtain the cursor to display 
				while the user drags the minimized window.

Parameters:   none

Return Value:  (HCURSOR)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
HCURSOR CSampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*********************************************************************************************
Description   : This function creates connections (database, input and output) to the server.

Parameters:   none

Return Value:  (HCURSOR)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::OnBnClickedButtonConnect()
{
	//create connection to the database of the server
	if(ConnectToDatabase())
	{
		TRACE(_T(" Database connected\n"));

		//initialize all list controls
		m_ListCameras.DeleteAllItems();
		m_ListRelays.DeleteAllItems();
		m_ListDetectors.DeleteAllItems();
		m_ListArchives.DeleteAllItems();
		m_ListSequences.DeleteAllItems();

		//create connection to the input (alarm detectors) of the server
		if(ConnectToInput())
		{
			TRACE(_T(" Input connected\n"));
			
			//create connection to the output (cameras, relays) of the server
			if(ConnectToOutput())
			{
				TRACE(_T(" Output connected\n"));
			}
		}
	}
	UpdateData(FALSE);
}

/*********************************************************************************************
Description   : This function closes all connection to the server.

Parameters:   none

Return Value:  (HCURSOR)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::OnBnClickedButtonDisconnect()
{
	//close all connections to the server
	CloseConnection();

	//delete all pointers
	DELETE_PTR(m_pCIPCDatabaseSample);
	DELETE_PTR(m_pCIPCOutputSample);
	DELETE_PTR(m_pCIPCInputSample);
}

/*********************************************************************************************
Description   : This function sets all necessary data for a connection to the server.

Parameters:   none

Return Value:  
c		: (O): Initialized connection record. (CConnectionRecord)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
CConnectionRecord CSampleDlg::SetParamsConnectionRecord()
{
	//get the IP address of the server as string
	m_sIPAdress = GetIPAdress();

	//create a connection record with necessary data
	CConnectionRecord c;
	c.SetPassword(m_sPassword);			//password of the permission "SuperVisor" if exists
	c.SetPermission(_T("SuperVisor"));	//permission
	c.SetDestinationHost(m_sIPAdress);	//IP address of the server as string
	c.SetFieldValue(_T("PIN"),m_sPin);	//PIN number for connections to a DTS system

	DWORD dwOptions = 0;

	//SCO_WANT_RELAYS: inform the sample when the status of a relay has changed
	dwOptions |= SCO_WANT_RELAYS;	

	//SCO_CAN_GOP: send GOP's (group of pictures) on MPEG4 pictures to the sample
	//if not set, each MPEG4 picture will be send to the sample one by one
	dwOptions |= SCO_CAN_GOP;

	c.SetOptions(dwOptions);

	return c;
}

/*********************************************************************************************
Description   : This function transforms the DWORD IP address into a text IP adress.

Parameters:   none

Return Value:  
sIPAdress	: (O): The transformed IP address. (CString)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
CString CSampleDlg::GetIPAdress()
{
#define	MASK	0x000000FF

	DWORD	dwIPAdress = m_dwIPAdress;
	CString sIPAdress;
	DWORD dw1, dw2, dw3, dw4;
	dw4 = dwIPAdress&0x000000ff;
	dw3 = (dwIPAdress >>= 8)&MASK;
	dw2 = (dwIPAdress >>= 8)&MASK;
	dw1 = (dwIPAdress >>= 8)&MASK;

	sIPAdress.Format(_T("tcp:%d.%d.%d.%d"), dw1, dw2, dw3, dw4);
	return sIPAdress;
}

/*********************************************************************************************
Description   : This function starts exiting the main dialog.

Parameters:   none

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::OnBnClickedOk()
{
	OnOK();
}

/*********************************************************************************************
Description   : This function initializes all controls.

Parameters:   none

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::InitControls()
{
	CRect rect;
	int w = 0;
	m_ListCameras.GetClientRect(rect);
	w = rect.Width();
	m_ListCameras.InsertColumn(0,_T("ID"),LVCFMT_LEFT,2*w/5);
	m_ListCameras.InsertColumn(1,_T("Name"),LVCFMT_LEFT,w/5);
	m_ListCameras.InsertColumn(2,_T("Type"),LVCFMT_LEFT,w/5);
	m_ListCameras.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	m_ListRelays.GetClientRect(rect);
	w = rect.Width();
	m_ListRelays.InsertColumn(0,_T("ID"),LVCFMT_LEFT,2*w/5);
	m_ListRelays.InsertColumn(1,_T("Name"),LVCFMT_LEFT,w/5);
	m_ListRelays.InsertColumn(2,_T("Contact"),LVCFMT_LEFT,w/5);
	m_ListRelays.InsertColumn(3,_T("Status"),LVCFMT_LEFT,w/5);
	m_ListRelays.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	m_ListDetectors.GetClientRect(rect);
	w = rect.Width();
	m_ListDetectors.InsertColumn(0,_T("ID"),LVCFMT_LEFT,2*w/5);
	m_ListDetectors.InsertColumn(1,_T("Name"),LVCFMT_LEFT,w/5);
	m_ListDetectors.InsertColumn(2,_T("Status"),LVCFMT_LEFT,w/5);
	m_ListDetectors.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	m_ListArchives.GetClientRect(rect);
	w = rect.Width();
	m_ListArchives.InsertColumn(0,_T("A-No"),LVCFMT_LEFT,2*w/5);
	m_ListArchives.InsertColumn(1,_T("Name"),LVCFMT_LEFT,w/5);
	m_ListArchives.InsertColumn(2,_T("Type"),LVCFMT_LEFT,w/5);
	m_ListArchives.InsertColumn(3,_T("Size (MBytes)"),LVCFMT_LEFT,w/5);
	m_ListArchives.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	m_ListSequences.GetClientRect(rect);
	w = rect.Width();
	m_ListSequences.InsertColumn(0,_T("S-No"),LVCFMT_LEFT,w/5);
	m_ListSequences.InsertColumn(1,_T("Name"),LVCFMT_LEFT,w/5);
	m_ListSequences.InsertColumn(2,_T("Count"),LVCFMT_LEFT,w/5);
	m_ListSequences.InsertColumn(3,_T("Size (Bytes)"),LVCFMT_LEFT,w/5);
	m_ListSequences.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
}

/*********************************************************************************************
Description   : This function fills the camera control with a list of all live cameras
				of the connected server.

Parameters:   
pa_cameras		: (I): Pointer Array of all live cameras.	(const CIPCOutputRecordArray&)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::FillListCtrl_Cameras(const CIPCOutputRecordArray& pa_cameras)
{
	m_ListCameras.DeleteAllItems();
	for (int i=0;i<pa_cameras.GetSize();i++)
	{
		DWORD dwID = 0;
		CString sCameraID;
		CString sType(_T("fixed"));
		CString sName;
		WORD wFlags = pa_cameras.GetAtFast(i)->GetFlags();

		sName = pa_cameras.GetAtFast(i)->GetName();
		dwID = pa_cameras.GetAtFast(i)->GetID().GetID();
		sCameraID.Format(_T("%d"), dwID);

		if ((wFlags & OREC_IS_ENABLED) != 0)
		{
			//camera is connected
			if((wFlags & OREC_CAMERA_IS_FIXED) == 0)
			{
				//camera is a pant tilt zoom camera
				sType = "PTZ";
			}
		}
		else
		{
			sType = "not connected";
		}

		int j=m_ListCameras.InsertItem(m_ListCameras.GetItemCount(),sCameraID);
		m_ListCameras.SetItemText(j,1,sName);
		m_ListCameras.SetItemText(j,2,sType);
		m_ListCameras.SetItemData(j,dwID);
	}
	for (i=0;i<m_ListCameras.GetHeaderCtrl()->GetItemCount();i++)
	{
		m_ListCameras.SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);
	}
}

/*********************************************************************************************
Description   : This function fills the relay control with a list of all relays
				of the connected server.

Parameters:
pa_relays		: (I): Pointer Array of all relays.		(const CIPCOutputRecordArray&)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::FillListCtrl_Relays(const CIPCOutputRecordArray& pa_relays)
{
	m_ListRelays.DeleteAllItems();
	for (int i=0;i<pa_relays.GetSize();i++)
	{
		DWORD dwID = 0;
		CString sCameraID;
		CString sType(_T("normally open"));
		CString sStatus(_T("open"));
		CString sName;
		WORD wFlags = pa_relays.GetAtFast(i)->GetFlags();

		sName = pa_relays.GetAtFast(i)->GetName();
		dwID = pa_relays.GetAtFast(i)->GetID().GetID();
		sCameraID.Format(_T("%d"), dwID);

        if ((wFlags & OREC_RELAY_CLOSED) != 0)
		{
			//relay is closed
			sStatus = "closed";
		}
		if((wFlags & OREC_RELAY_IS_EDGE) == 0)
		{
			//relay is an edge relay
			sType = "break";
		}
		int j=m_ListRelays.InsertItem(m_ListRelays.GetItemCount(),sCameraID);
		m_ListRelays.SetItemText(j,1,sName);
		m_ListRelays.SetItemText(j,2,sType);
		m_ListRelays.SetItemText(j,3,sStatus);
		m_ListRelays.SetItemData(j,dwID);
	}
	for (i=0;i<m_ListRelays.GetHeaderCtrl()->GetItemCount();i++)
	{
		m_ListRelays.SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);
	}
}

/*********************************************************************************************
Description   : This function fills the detector control with a list of all detectors
				of the connected server.

Parameters: 
detectors		: (I): Pointerarray of all detectors.	(const CIPCInputRecordArray&)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::FillListCtrl_Detectors(const CIPCInputRecordArray& detectors)
{
	m_ListDetectors.DeleteAllItems();
	for (int i=0;i<detectors.GetSize();i++)
	{
		DWORD dwID = 0;
		CString sCameraID;
		CString sStatus(_T("not signaled"));
		CString sName;

		CIPCInputRecord* pRec = detectors.GetAtFast(i);
		sName = pRec->GetName();
		dwID = pRec->GetID().GetID();
		sCameraID.Format(_T("%d"), dwID);

		if(pRec->GetIsActive()) //detectors currently detects an alarm
		{
			//alarm detector is signaled
			sStatus = "signaled";
		}
		
		int j=m_ListDetectors.InsertItem(m_ListDetectors.GetItemCount(),sCameraID);
		m_ListDetectors.SetItemText(j,1,sName);
		m_ListDetectors.SetItemText(j,2,sStatus);
		m_ListDetectors.SetItemData(j,dwID);
	}
	for (i=0;i<m_ListDetectors.GetHeaderCtrl()->GetItemCount();i++)
	{
		m_ListDetectors.SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);
	}

}

/*********************************************************************************************
Description   : This function gets all archives from the DatabaseSample and fills the 
				archive control with a list of all archives	of the connected server.

Parameters:   none

Return Value:	(void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::FillListCtrl_Archives()
{
	if(m_pCIPCDatabaseSample)
	{
		for (int i=0;i<m_pCIPCDatabaseSample->GetRecords().GetSize();i++)
		{
			CIPCArchivRecord* pAR = m_pCIPCDatabaseSample->GetRecords().GetAt(i);
			CString s;
			s.Format(_T("%d"),pAR->GetID(),pAR->GetID());
			int j = m_ListArchives.InsertItem(m_ListArchives.GetItemCount(),s);
			m_ListArchives.SetItemText(j,1,pAR->GetName());
			if (pAR->IsAlarm())
			{
				//archive is an alarm archive
				s = _T("Alarm");
			}
			else if (pAR->IsAlarmList())
			{
				//archive is an alarm list (DTS)
				s = _T("Alarmlist");
			}
			else if (pAR->IsBackup())
			{
				//archive is a backup archive
				s = _T("Backup");
			}
			else if (pAR->IsRing())
			{
				//archive is a ring archive
				s = _T("Ring");
			}
			else if (pAR->IsSafeRing())
			{
				//archive is a prering archive
				s = _T("Prering");
			}
			else if (pAR->IsSearch())
			{
				//archive is a search result archive
				s = _T("Search");
			}
			else
			{
				s = _T("Unknown");
			}
			m_ListArchives.SetItemText(j,2,s);
			s.Format(_T("%d"),CIPCInt64(pAR->BenutzterPlatz()).GetInMB());
			m_ListArchives.SetItemText(j,3,s);
			m_ListArchives.SetItemData(j,pAR->GetID());
		}

		for (i=0;i<m_ListArchives.GetHeaderCtrl()->GetItemCount();i++)
		{
			m_ListArchives.SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);
		}
	}
}

/*********************************************************************************************
Description   : This function fills the sequence control with a list of all 
				sequences of the specific archive.

Parameters:  
wArchiveNr	: (I): Archive number.											(WORD)
&sequences	: (I): Reference to a pointer list of the archives sequences.	(CIPCSequenceRecords)

Return Value:	(void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::FillListCtrl_Sequences(WORD wArchiveNr, 
										CIPCSequenceRecords& sequences)
{
	DeleteListCtrlSequences(wArchiveNr);

	for (int i=0;i<sequences.GetSize();i++)
	{
		CIPCSequenceRecord* pR = sequences.GetAt(i);
		if (pR->GetArchiveNr()==wArchiveNr)
		{
			CString s;
			s.Format(_T("%d"),pR->GetSequenceNr());
			int j=m_ListSequences.InsertItem(m_ListSequences.GetItemCount(),s);
			m_ListSequences.SetItemText(j,1,pR->GetName());
			s.Format(_T("%d"),pR->GetNrOfPictures());
			m_ListSequences.SetItemText(j,2,s);
			m_ListSequences.SetItemText(j,3,pR->GetSize().Format(TRUE));
			m_ListSequences.SetItemData(j,MAKELONG(pR->GetSequenceNr(),pR->GetArchiveNr()));
		}
	}
	for (i=0;i<m_ListSequences.GetHeaderCtrl()->GetItemCount();i++)
	{
		m_ListSequences.SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);
	}
}

/*********************************************************************************************
Description   : This function is called after an archive in the archive control was
				selected. A request of the archives sequences will be send to the server.

Parameters:  
pNMHDR	: (I): Pointer to the list control.			(NMHDR *)
pResult	: (O): Pointer to the result (not used).	(LRESULT *)

Return Value:	(void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::OnLvnItemchangedListArchives(NMHDR *pNMHDR, 
											  LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	int i = pNMLV->iItem;
	DWORD dwData = (DWORD)m_ListArchives.GetItemData(i);
	WORD wArchivNr = (WORD)dwData;
	if (LVIS_SELECTED & pNMLV->uNewState)
	{
		//request the sequence list of the selected archive
		m_pCIPCDatabaseSample->DoRequestSequenceList(wArchivNr);
	}
	else
	{
		DeleteListCtrlSequences(wArchivNr);
	}

	*pResult = 0;
}

/*********************************************************************************************
Description   : This function is called after a sequence in the sequence control was
				selected. All play buttons and the first picture of the selected sequence will
				will be shown.
Parameters:  
pNMHDR	: (I): Pointer to the list control.			(NMHDR *)
pResult	: (O): Pointer to the result (not used).	(LRESULT *)

Return Value:	(void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::OnLvnItemchangedListSequences(NMHDR *pNMHDR, 
											   LRESULT *pResult)
{
	for (int i=0;i<m_ListSequences.GetItemCount();i++)
	{
		if (LVIS_SELECTED & m_ListSequences.GetItemState(i,LVIS_SELECTED))
		{
			DWORD dwData = (DWORD)m_ListSequences.GetItemData(i);
			WORD wArchiveNr = HIWORD(dwData);
			WORD wSequenceNr = LOWORD(dwData);
			m_wCurrentArchive = wArchiveNr;
			m_wCurrentSequence = wSequenceNr;
			m_dwCurrentRecord = 1; 
			
			if(m_ctrlPlayPicture)
			{
				//activate the play buttons
				ActivatePlayButtons(PS_STOP);

				//show play and message control
				m_ctrlPlayPicture.ShowWindow(SW_SHOW);
				m_ctrlPictureInfo.ShowWindow(SW_SHOW);
				m_ctrlDBText.ShowWindow(SW_SHOW);

				//navigate to the first picture of the current sequence
				m_ctrlPlayPicture.Navigate(PS_STOP, m_wCurrentArchive, m_wCurrentSequence, 0, m_dwCurrentRecord);
			}
			break;
		}
	}

	*pResult = 0;
}

/*********************************************************************************************
Description   : This function is called after a live camera in the camera control was
				selected. The live pictures of the selected live camera will be requested.
Parameters:  
pNMHDR	: (I): Pointer to the list control.			(NMHDR *)
pResult	: (O): Pointer to the result (not used).	(LRESULT *)

Return Value:	(void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::OnLvnItemchangedListCameras(NMHDR *pNMHDR, 
											 LRESULT *pResult)
{
	for (int i=0;i<m_ListCameras.GetItemCount();i++)
	{
		if (LVIS_SELECTED & m_ListCameras.GetItemState(i,LVIS_SELECTED))
		{
			DWORD dwData = (DWORD)m_ListCameras.GetItemData(i);
			CString s = m_ListCameras.GetItemText(i,1);
			CSecID secIDNewCamera(dwData);
	
            if(m_pCIPCOutputSample)
			{
				DWORD dwCurrentCamera = m_secIDCurrentCamera.GetID();
				DWORD dwNewCamera = dwData;

				if(m_secIDCurrentCamera == DWORD(0))
				{
					//on first request both cameras, the previous and the new are the same
					dwCurrentCamera = dwNewCamera;
				}

				//request the new camera
				m_ctrlLivePicture.PostMessage(WM_REQUEST_PICTURES, dwNewCamera, dwCurrentCamera);
				m_secIDCurrentCamera = secIDNewCamera;
			}
			break;
		}
	}
	*pResult = 0;
}

/*********************************************************************************************
Description   : This function deletes the sequence list of the specified archive.

Parameters:  
wArchiveNr	: (I): The archive which sequence list has to be deleted.	(WORD)

Return Value:	(void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::DeleteListCtrlSequences(WORD wArchiveNr)
{
	if (m_ListSequences.GetItemCount()>0)
	{
		for (int i=m_ListSequences.GetItemCount()-1;i>=0;i--)
		{
			DWORD dwData = (DWORD)m_ListSequences.GetItemData(i);
			if (wArchiveNr == HIWORD(dwData))
			{
				m_ListSequences.DeleteItem(i);
			}
		}
	}
}

/*********************************************************************************************
Description   : This function updates and redraws the dialog to show new text messages.

Parameters:  
pDC	: (I): The device context to be updated.	(CDC*)

Return Value:	(void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::OnDraw(CDC* pDC)
{
	//update to show new text messages
	UpdateData(FALSE);
}

/*********************************************************************************************
Description   : This function stops the GOP-timer, stops the video requests,
				hides all play buttons and text messages on the main dialog, deletes all
				contains of all controls on the main dialog and closes the connection 
				to the server.

Parameters:  none

Return Value:	(void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::CloseConnection()
{
	//kill the GOP multimedia timer
	m_ctrlLivePicture.KillGOPTimer();

	if(m_pCIPCOutputSample)
	{
		//stop requesting pictures
		m_pCIPCOutputSample->DoRequestStopVideo(m_secIDCurrentCamera,0);
	}
	
	//hide all picture and text message controls
	m_ctrlPlayPicture.ShowWindow(SW_HIDE);
	m_ctrlLivePicture.ShowWindow(SW_HIDE);
	m_ctrlPictureInfo.ShowWindow(SW_HIDE);
	m_ctrlDBText.ShowWindow(SW_HIDE);

	//delete all lists
	m_ListArchives.DeleteAllItems();
	m_ListSequences.DeleteAllItems();
	m_ListCameras.DeleteAllItems();
	m_ListRelays.DeleteAllItems();
	m_ListDetectors.DeleteAllItems();

	//deactivate all play buttons
	ActivatePlayButtons(m_CurrentPlayStatus, FALSE);

	if(m_pCIPCControlInterface)
	{
		//close all connections to the server
		m_pCIPCControlInterface->CloseAllGateways();
	}

	CString sHost = m_sIPAdress.Right(m_sIPAdress.GetLength()-4);
	m_sTextMsg.Format(_T("Connection to host < %s > closed."), sHost);
	
	UpdateData(FALSE);
}

/*********************************************************************************************
Description   : The framework calls this member function to inform the dialog that it is 
				being destroyed. Deletes all used pointers.
				
Parameters:  none

Return Value:	(void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::OnDestroy()
{
	//close connections to the server and hide all controls
	CloseConnection();

	//delete pointers
	DELETE_PTR(m_pCIPCDatabaseSample);
	DELETE_PTR(m_pCIPCOutputSample);
	DELETE_PTR(m_pCIPCInputSample);
	DELETE_PTR(m_pCIPCControlInterface);
	CDialog::OnDestroy();
}

/*********************************************************************************************
Description   : This function is called after clicking the button step forward (step f.).
				The new playstatus will be set and next picture will be requested from the server.

Parameters:  none

Return Value:	(void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::OnBnClickedButtonStepForward()
{
	//navigate to the next database picture (current picture + 1)
	m_CurrentPlayStatus = ActivatePlayButtons(PS_STOP);
	m_ctrlPlayPicture.Navigate(m_CurrentPlayStatus, m_wCurrentArchive, m_wCurrentSequence, m_dwCurrentRecord, 1);
}

/*********************************************************************************************
Description   : This function is called after clicking the button step backward (step b.).
				The new playstatus will be set and next picture will be requested from the server.

Parameters:  none

Return Value:	(void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::OnBnClickedButtonStepBackward()
{	
	//navigate to the next database picture (current picture - 1)
	m_CurrentPlayStatus = ActivatePlayButtons(PS_STOP);
	m_ctrlPlayPicture.Navigate(m_CurrentPlayStatus, m_wCurrentArchive, m_wCurrentSequence, m_dwCurrentRecord, -1);
}

/*********************************************************************************************
Description   : This function is called after clicking the button play forward (play f.).
				The new playstatus will be set and next picture will be requested from the server.

Parameters:  none

Return Value:	(void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::OnBnClickedButtonPlayForward()
{	
	//navigate to the next database picture (current picture + 1)
	//the next picture will automatically requested on playstatus PS_PLAY_FORWARD
	m_CurrentPlayStatus = ActivatePlayButtons(PS_PLAY_FORWARD);
	m_ctrlPlayPicture.Navigate(m_CurrentPlayStatus, m_wCurrentArchive, m_wCurrentSequence, m_dwCurrentRecord, 1);
}

/*********************************************************************************************
Description   : This function is called after clicking the button play backward (play b.).
				The new playstatus will be set and next picture will be requested from the server.

Parameters:  none

Return Value:	(void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::OnBnClickedButtonPlayBackward()
{	
	//navigate to the next database picture (current picture - 1)
	//the previous picture will automatically requested on playstatus PS_PLAY_BACKWARD
	m_CurrentPlayStatus = ActivatePlayButtons(PS_PLAY_BACKWARD);
	m_ctrlPlayPicture.Navigate(m_CurrentPlayStatus, m_wCurrentArchive, m_wCurrentSequence, m_dwCurrentRecord, -1);
}

/*********************************************************************************************
Description   : This function is called after clicking the button fast forward (fast f.).
				The new playstatus will be set and next picture will be requested from the server.

Parameters:  none

Return Value:	(void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::OnBnClickedButtonFastForward()
{
	//navigate to the next database picture (current picture + 10)
	//the next picture will automatically requested on playstatus PS_FAST_FORWARD
	m_CurrentPlayStatus = ActivatePlayButtons(PS_FAST_FORWARD);
	m_ctrlPlayPicture.Navigate(m_CurrentPlayStatus, m_wCurrentArchive, m_wCurrentSequence, m_dwCurrentRecord, 10);
}

/*********************************************************************************************
Description   : This function is called after clicking the button fast backward (fast b.).
				The new playstatus will be set and next picture will be requested from the server.

Parameters:  none

Return Value:	(void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::OnBnClickedButtonFastBackward()
{
	//navigate to the next database picture (current picture - 10)
	//the previous picture will automatically requested on playstatus PS_FAST_BACKWARD
	m_CurrentPlayStatus = ActivatePlayButtons(PS_FAST_BACKWARD);
	m_ctrlPlayPicture.Navigate(m_CurrentPlayStatus, m_wCurrentArchive, m_wCurrentSequence, m_dwCurrentRecord, -10);
}

/*********************************************************************************************
Description   : This function enables or disabled all play buttons depending on the 
				current playstatus. If bVisible is FALSE all play button will be hided.

Parameters:
ps			: (I): The current playstatus.			(PlayStatus)
bVisible	: (I): If FALSE hide all play buttons,	
				   If TRUE show all play buttons.	(BOOL)

Return Value:  
psReturn	: (O): The new playstatus. (PlayStatus)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
PlayStatus CSampleDlg::ActivatePlayButtons(PlayStatus ps, 
										   BOOL bVisible /* = TRUE */)
{
	int iVisible = 0;
	bVisible ? iVisible = SW_SHOW : iVisible = SW_HIDE;

	//show or hide all play buttons

	m_ctrlButtonStepForward.ShowWindow(iVisible);
	m_ctrlButtonStepBackward.ShowWindow(iVisible);
	m_ctrlButtonPlayForward.ShowWindow(iVisible);
	m_ctrlButtonPlayBackward.ShowWindow(iVisible);
	m_ctrlButtonFastForward.ShowWindow(iVisible);
	m_ctrlButtonFastBackward.ShowWindow(iVisible);

	PlayStatus psReturn = ps;
	if(m_CurrentPlayStatus == ps)
	{
		psReturn = PS_STOP;
	}

	//enable or disable the play buttons depending on the current play status
	//e.g. if playstatus is PS_PLAY_FORWARD, all play buttons 
	//except the button "play f." are disabled until the play status PS_STOP is reached
	switch (psReturn)
	{
		case PS_STOP:
			m_ctrlButtonStepForward.EnableWindow(TRUE);
			m_ctrlButtonStepBackward.EnableWindow(TRUE);
			m_ctrlButtonPlayForward.EnableWindow(TRUE);
			m_ctrlButtonPlayBackward.EnableWindow(TRUE);
			m_ctrlButtonFastForward.EnableWindow(TRUE);
			m_ctrlButtonFastBackward.EnableWindow(TRUE);
			break;
		
		case PS_PLAY_FORWARD:
			m_ctrlButtonStepForward.EnableWindow(FALSE);
			m_ctrlButtonStepBackward.EnableWindow(FALSE);
			m_ctrlButtonPlayForward.EnableWindow(TRUE);
			m_ctrlButtonPlayBackward.EnableWindow(FALSE);
			m_ctrlButtonFastForward.EnableWindow(FALSE);
			m_ctrlButtonFastBackward.EnableWindow(FALSE);
			break;

		case PS_PLAY_BACKWARD:
			m_ctrlButtonStepForward.EnableWindow(FALSE);
			m_ctrlButtonStepBackward.EnableWindow(FALSE);
			m_ctrlButtonPlayForward.EnableWindow(FALSE);
			m_ctrlButtonPlayBackward.EnableWindow(TRUE);
			m_ctrlButtonFastForward.EnableWindow(FALSE);
			m_ctrlButtonFastBackward.EnableWindow(FALSE);
			break;

		case PS_FAST_FORWARD:
			m_ctrlButtonStepForward.EnableWindow(FALSE);
			m_ctrlButtonStepBackward.EnableWindow(FALSE);
			m_ctrlButtonPlayForward.EnableWindow(FALSE);
			m_ctrlButtonPlayBackward.EnableWindow(FALSE);
			m_ctrlButtonFastForward.EnableWindow(TRUE);
			m_ctrlButtonFastBackward.EnableWindow(FALSE);
			break;

		case PS_FAST_BACKWARD:
			m_ctrlButtonStepForward.EnableWindow(FALSE);
			m_ctrlButtonStepBackward.EnableWindow(FALSE);
			m_ctrlButtonPlayForward.EnableWindow(FALSE);
			m_ctrlButtonPlayBackward.EnableWindow(FALSE);
			m_ctrlButtonFastForward.EnableWindow(FALSE);
			m_ctrlButtonFastBackward.EnableWindow(TRUE);
			break;

		default:
			m_ctrlButtonStepForward.EnableWindow(TRUE);
			m_ctrlButtonStepBackward.EnableWindow(TRUE);
			m_ctrlButtonPlayForward.EnableWindow(TRUE);
			m_ctrlButtonPlayBackward.EnableWindow(TRUE);
			m_ctrlButtonFastForward.EnableWindow(TRUE);
			m_ctrlButtonFastBackward.EnableWindow(TRUE);
	}

	UpdateWindow();
	return psReturn;
}

/*********************************************************************************************
Description   : This function creates a connection to the connected servers database. All
				database pictures will be requested and received through this connection.
				The function CIPCDatabaseSample::OnConfirmConnection() will be called to
				indicate that the connection was established.

Parameters:		none

Return Value:  
bRet	: (O): If TRUE the connection was established,
			   if FALSE no connection was established. (BOOL)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
BOOL CSampleDlg::ConnectToDatabase()
{
	BOOL bRet = FALSE;
	if (!m_pCIPCDatabaseSample)
	{
		UpdateData(TRUE);	//update the data in the main dialog window

		//TODO UF  faken, dass die SocketUnit läuft
		WK_ALONE(WK_APP_NAME_SOCKETS);

		CIPCFetch fetch;
		CConnectionRecord c;

		//get all necessary data for the connection
		c = SetParamsConnectionRecord();

		//connect to the servers database. 
		//The CIPCFetchResult contains the connection data
		CIPCFetchResult frDatabase = fetch.FetchDatabase(c);
		if (frDatabase.IsOkay())
		{
			//create a CIPCDatabaseSample object. This object communicates with the servers database
			m_pCIPCDatabaseSample = new CIPCDatabaseSample(m_ctrlPlayPicture, frDatabase.GetShmName(), frDatabase.GetCodePage());
			
			//create a text message
			m_sTextMsg.Empty();
			CString sHost = m_sIPAdress.Right(m_sIPAdress.GetLength()-4);
			m_sTextMsg.Format(_T("Connection to host < %s > established."), sHost);

			//if connection is to a DTS system, save this informations here
			m_bIsDTS = (frDatabase.GetOptions() & SCO_IS_DV) > 0;
			bRet = TRUE;
		}
		else
		{
			//in case of a connection error show a text message
			CIPCError  Error = frDatabase.GetError();
			m_sTextMsg.Format(_T("Error:  %s"), NameOfEnum(Error));
		}
	}
	return bRet;
}

/*********************************************************************************************
Description   : This function creates a connection to the connected servers output. All
				live cameras (videos) and relays will be requested and received 
				through this connection. 
				The function CIPCOutputSample::OnConfirmConnection() will be called to
				indicate that the connection was established.

Parameters:		none

Return Value:  
bRet	: (O): If TRUE the connection was established,
			   if FALSE no connection was established. (BOOL)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
BOOL CSampleDlg::ConnectToOutput()
{
	BOOL bRet = FALSE;

	if (!m_pCIPCOutputSample)
	{
		//TODO UF  faken, dass die SocketUnit läuft
		WK_ALONE(WK_APP_NAME_SOCKETS);
		CIPCFetch fetch;
		CConnectionRecord c;

		//get all necessary data for the connection
		c = SetParamsConnectionRecord();

		//connect to the servers output (cameras, relays). 
		//The CIPCFetchResult contains the connection data
		CIPCFetchResult frOutput = fetch.FetchOutput(c);

		if (frOutput.IsOkay())
		{
			//create a CIPCOutputSample object. This object communicates with the servers output
			m_pCIPCOutputSample = new CIPCOutputSample(m_ctrlLivePicture, frOutput.GetShmName(), frOutput.GetCodePage());
			
			//create a text message
			m_sTextMsg.Empty();
			CString sHost = m_sIPAdress.Right(m_sIPAdress.GetLength()-4);
			m_sTextMsg.Format(_T("Connection to host < %s > established."), sHost);
			bRet = TRUE;
		}
		else
		{
			//in case of a connection error show a text message
			CIPCError  Error = frOutput.GetError();
			m_sTextMsg.Format(_T("Error:  %s"), NameOfEnum(Error));
		}
	}
	return bRet;
}

/*********************************************************************************************
Description   : This function creates a connection to the connected servers input. All
				inputs of the server (alarm detectors) will be requested and received 
				through this connection.
				The function CIPCInputSample::OnConfirmConnection() will be called to
				indicate that the connection was established.

Parameters:		none

Return Value:  
bRet	: (O): If TRUE the connection was established,
			   if FALSE no connection was established. (BOOL)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
BOOL CSampleDlg::ConnectToInput()
{
	BOOL bRet = FALSE;

	if (!m_pCIPCInputSample)
	{
		//TODO UF  faken, dass die SocketUnit läuft
        WK_ALONE(WK_APP_NAME_SOCKETS);
		
		CIPCFetch fetch;
		CConnectionRecord c;
		c = SetParamsConnectionRecord();

		//connect to the servers input (alarm detectors). 
		//The CIPCFetchResult contains the connection data
		CIPCFetchResult frInput = fetch.FetchInput(c);

		if (frInput.IsOkay())
		{
			//create a CIPCInputSample object. This object communicates with the servers input
			m_pCIPCInputSample = new CIPCInputSample(this, frInput.GetShmName(), frInput.GetCodePage());
			
			//create a text message
			m_sTextMsg.Empty();
			CString sHost = m_sIPAdress.Right(m_sIPAdress.GetLength()-4);
			m_sTextMsg.Format(_T("Connection to host < %s > established."), sHost);
			bRet = TRUE;
		}
		else
		{
			//in case of a connection error show a text message
			CIPCError  Error = frInput.GetError();
			m_sTextMsg.Format(_T("Error:  %s"), NameOfEnum(Error));
		}
	}
	return bRet;
}

/*********************************************************************************************
Description   : This function sets the new text message with informations about 
				the currently shown database picture. This message includes the
				archive, sequence and record number and the record count of all pictures
				within the current sequence.

Parameters:		
sTextMessage	: (O): The new message. (CString)

Return Value:  none

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::SetDBTextMsg(CString sTextMessage)
{
	m_sDBText = sTextMessage;
}

/*********************************************************************************************
Description   : This function sets the new text message with informations about 
				the currently shown database picture. This message includes the
				whole time stamp of the picture.

Parameters:	
sTextMessage	: (O): The new message. (CString)

Return Value:	none  

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::SetPictureInfo(CString sTextMessage)
{
	m_sPictureInfo = sTextMessage;
}

/*********************************************************************************************
Description   : This function sets the archive, sequence and record number of the 
				currently shown database picture. The SampleDlg needs this information for
				navigation to the next picture after clicking a play button.

Parameters:	
wArchiveNr	: (O): The new message. (WORD)
wSequenceNr	: (O): The new message. (WORD)
dwRecordNr	: (O): The new message. (DWORD)

Return Value:	none	 


Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
void CSampleDlg::SetCurrentRecord(WORD wArchiveNr, 
								  WORD wSequenceNr, 
								  DWORD dwRecordNr)
{
	//safe the current data of the database picture 
	m_wCurrentArchive = wArchiveNr;
	m_wCurrentSequence = wSequenceNr;
	m_dwCurrentRecord = dwRecordNr;
}
