// MCLTCtl.cpp : Implementation of the CMCLTCtrl ActiveX Control class.

#include "stdafx.h"
#include "MCLT.h"
#include "MCLTCtl.h"
#include "MCLTPpg.h"
#include "IPCInputOCX.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CMCLTCtrl* m_pThis;
UINT CMCLTCtrl::gm_wTimerRes = 0;

IMPLEMENT_DYNCREATE(CMCLTCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CMCLTCtrl, COleControl)
	//{{AFX_MSG_MAP(CMCLTCtrl)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_MESSAGE(WM_REQUEST_PICTURES, DoRequestPicture)
	ON_MESSAGE(WM_DECODE_VIDEO, OnDecodeVideo)	
	ON_WM_TIMER()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CMCLTCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CMCLTCtrl)
	DISP_PROPERTY_EX(CMCLTCtrl, "Selected", GetSelected, SetNotSupported, VT_BOOL)
	DISP_PROPERTY_EX(CMCLTCtrl, "Shape", GetShape, SetShape, VT_I2)
	DISP_PROPERTY_EX(CMCLTCtrl, "Connect", GetNotSupported, SetConnect, VT_BSTR)
	DISP_PROPERTY_EX(CMCLTCtrl, "Camera", GetNotSupported, SetCamera, VT_I2)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CMCLTCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CMCLTCtrl, COleControl)
	//{{AFX_EVENT_MAP(CMCLTCtrl)
	EVENT_CUSTOM("Select", FireSelect, VTS_BOOL)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CMCLTCtrl, 1)
	PROPPAGEID(CMCLTPropPage::guid)
END_PROPPAGEIDS(CMCLTCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CMCLTCtrl, "MCLT.MCLTCtrl.1",
	0x739aea24, 0xa07c, 0x487a, 0x98, 0x45, 0x2f, 0x9f, 0xa7, 0x8e, 0x14, 0x1a)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CMCLTCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DMCLT =
		{ 0x73f1f421, 0x9a15, 0x4bdb, { 0xa5, 0xe, 0xe8, 0xfb, 0xa0, 0x70, 0xaa, 0x7a } };
const IID BASED_CODE IID_DMCLTEvents =
		{ 0xbe2e041, 0xe0f4, 0x40f8, { 0x82, 0x75, 0xf6, 0xb6, 0xf8, 0x16, 0xfe, 0x45 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwMCLTOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CMCLTCtrl, IDS_MCLT, _dwMCLTOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CMCLTCtrl::CMCLTCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CMCLTCtrl

BOOL CMCLTCtrl::CMCLTCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_MCLT,
			IDB_MCLT,
			afxRegApartmentThreading,
			_dwMCLTOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CMCLTCtrl::CMCLTCtrl - Constructor

CMCLTCtrl::CMCLTCtrl()
{
	InitializeIIDs(&IID_DMCLT, &IID_DMCLTEvents);
	m_bSelected = FALSE;
	m_nShape = 0;
	m_pCIPCInputOCX = NULL;
	m_pCIPCOutputOCX = NULL;
	m_sPin = _T("0000");

	HINSTANCE hInstance = AfxLoadLibrary(_T("dvuesp.dll"));
	if (hInstance)
	{
		typedef LPCTSTR (APIENTRY* GET_LANGUAGE_NAME_PTR)(void);
		FARPROC theProc = ::GetProcAddress(hInstance, "GetLanguageName");
		if (theProc)
		{
			m_sFoundLanguage = ((GET_LANGUAGE_NAME_PTR)theProc)();
			m_nShape = 4;
		}
		AfxFreeLibrary(hInstance);
	}

	//create the control interface 
	m_pCIPCControlInterface = new CIPCControlInterface;

	//get the default parameters
	NETUNITPARAM netUnitParam			= m_pCIPCControlInterface->GetNetUnitParam();


	m_pThis					= this;
	m_pPictureRecord		= NULL;
	m_pMPEG4Decoder			= NULL;
	m_pJpeg					= NULL;
	m_pCIPCOutputOCX		= NULL;
	m_secIDCurrentCamera	= DWORD(0);
	m_uGOPTimer				= 0;
	m_iPictureRequests		= 0;
	m_nCurrentPicture		= -1;
	m_bFrameWasDropped		= FALSE;
	m_bUseMMTimer			= TRUE;
	m_bGOPTimerThreadPriorIsSet = FALSE;
	m_CompressionType		= COMPRESSION_UNKNOWN;
	m_dwCurrentCamera = 1;
	// TODO: Initialize your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CMCLTCtrl::~CMCLTCtrl - Destructor

CMCLTCtrl::~CMCLTCtrl()
{
	//close all connections to the server
	CloseConnection();

	DELETE_PTR(m_pCIPCInputOCX);
	DELETE_PTR(m_pCIPCOutputOCX);
	DELETE_PTR(m_pCIPCControlInterface);
	// TODO: Cleanup your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CMCLTCtrl::OnDraw - Drawing function

void CMCLTCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	// TODO: Replace the following code with your own drawing code.
	//pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	//pdc->Ellipse(rcBounds);

	BOOL DrawPicture = TRUE;
	if(DrawPicture)
	{
		//device context for painting
		CPaintDC dc(this); 

		CAutoCritSec acs(&m_csPictureRecord);
		if(m_pPictureRecord)
		{
			CWindowDC dc(this);
			CRect rcCtrl;

			CompressionType ct = COMPRESSION_UNKNOWN;
			ct = m_pPictureRecord->GetCompressionType();

			if (ct == COMPRESSION_MPEG4)
			{
				if(m_pMPEG4Decoder)
				{
					GetClientRect(&rcCtrl);
					m_pMPEG4Decoder->OnDraw(&dc, rcCtrl);
				}
			}
			else if(ct == COMPRESSION_JPEG)
			{
				if(m_pJpeg)
				{
					GetClientRect(&rcCtrl);
					m_pJpeg->OnDraw(&dc, rcCtrl);
				}
			}
		}
	}
	else
	{
		CPen pen;
		CBrush foreBrush, backBrush;
		CPoint points[3];
		pdc->SaveDC();
		pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		backBrush.CreateSolidBrush(TranslateColor(AmbientBackColor()));
		foreBrush.CreateSolidBrush(GetSelected() ? RGB(255, 0, 0) : RGB(0, 255, 0));

		pdc->FillRect(rcBounds, &backBrush);
		pdc->SelectObject(&pen);
		pdc->SelectObject(&foreBrush);



		if(m_sFoundLanguage.IsEmpty())
		{
			m_nShape = 3;
			m_sFoundLanguage = _T("weiter gehts");
		}
		switch (m_nShape)
		{
		case 0:
			pdc->Ellipse(rcBounds);
			break;
		case 1:
			pdc->Rectangle(rcBounds);
			break;
		case 2:
			points[0].x = rcBounds.left;
			points[0].y = rcBounds.bottom - 1;
			points[1].x = (rcBounds.left + rcBounds.right -1) /2;
			points[1].y = rcBounds.top;
			points[2].x = rcBounds.right - 1;
			points[2].y = rcBounds.bottom - 1;
			pdc->Polygon(points, 3);
			break;
		case 3:
			{
				CRect Rect(0,20,200,200);
				pdc->DrawText(_T("DLL nicht geladen"), Rect,DT_CENTER);
			}
			break;
		case 4:
			{
				CRect Rect(0,20,200,200);
				CString s;
				s.Format(_T("DLL geladen: %s"),m_sFoundLanguage);
				pdc->DrawText(s, Rect,DT_CENTER);
			}
			break;
		}
		CRect Rect(0,20,300,300);
		CString s;
		s.Format(_T("%s\n\n%s"),m_sConnected, m_sTextMsg);
		pdc->DrawText(s, Rect,DT_CENTER);
		pdc->RestoreDC(-1);
	}


}


/////////////////////////////////////////////////////////////////////////////
// CMCLTCtrl::DoPropExchange - Persistence support

void CMCLTCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	PX_Short(pPX, _T("Shape"), m_nShape, 0);
}


/////////////////////////////////////////////////////////////////////////////
// CMCLTCtrl::OnResetState - Reset control to default state

void CMCLTCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CMCLTCtrl::AboutBox - Display an "About" box to the user

void CMCLTCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_MCLT);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CMCLTCtrl message handlers



BOOL CMCLTCtrl::GetSelected() 
{
	// TODO: Add your property handler here

	return TRUE;
}

short CMCLTCtrl::GetShape()
{
	// TODO: Add your property handler here

	return m_nShape;
}

void CMCLTCtrl::SetShape(short nNewValue) 
{
	/*
	if(AmbientUserMode()
	{
		ThrowError(CTL_E_SETNOTSUPPORTEDATRUNTIME);
	}
	else if (nNewValue < 0 || nNewValue > 2)
	{
		ThrowError(CTL_E_INVALIDPROPERTYVALUE);
	}
	else
	*/
	{
		m_nShape = nNewValue;
		SetModifiedFlag();
		InvalidateControl();
	}
}
void CMCLTCtrl::SetConnect(CString sNew) 
{
	m_sConnected.Format(_T("%s"), sNew);

	m_sPassword = _T("idip40");
	//if(CheckPassword(m_sConnected))
	{
		m_sConnected = _T("password correct");
		ConnectToOutput();
		//ConnectToInput();
	}
	/*else
	{
		m_sConnected.Format(_T("password incorrect"));
	}
	*/
	TRACE(m_sConnected);
	SetShape(2);
}

void CMCLTCtrl::SetCamera(short sCamNr) 
{
	//on first request both cameras, the previous and the new are the same

	PostMessage(WM_REQUEST_PICTURES, sCamNr, m_dwCurrentCamera);
	m_dwCurrentCamera = sCamNr;
}
BOOL CMCLTCtrl::CheckPassword(CString sHash) 
{
	BOOL bRet = FALSE;
	CString sIdip40 = "f943c42b486a56c62c516edd6f01935e";
	if(sHash == sIdip40)
	{
		m_sPassword = _T("idip40");
		bRet = TRUE;
	}
	return bRet;
}
void CMCLTCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	
COleControl::OnLButtonDown(nFlags, point);
	m_bSelected = !m_bSelected;
	InvalidateControl();
	FireSelect(m_bSelected);
}
DWORD CMCLTCtrl::GetControlFlags()
{
	return COleControl::GetControlFlags() | pointerInactive;
}

BOOL CMCLTCtrl::ConnectToInput()
{
	BOOL bRet = FALSE;

	if (!m_pCIPCInputOCX)
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
			//create a CIPCInputOCX object. This object communicates with the servers input
			m_pCIPCInputOCX = new CIPCInputOCX(this, frInput.GetShmName(), frInput.GetCodePage());

			//create a text message
			m_sTextMsg.Empty();
			m_sTextMsg.Format(_T("Connection to host established."));


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
BOOL CMCLTCtrl::ConnectToOutput()
{
	BOOL bRet = FALSE;

	if (!m_pCIPCOutputOCX)
	{
		//TODO UF  faken, dass die SocketUnit läuft
		WK_ALONE(WK_APP_NAME_SOCKETS);

		CIPCFetch fetch;
		CConnectionRecord c;
		c = SetParamsConnectionRecord();

		//connect to the servers output (cameras, relays). 
		//The CIPCFetchResult contains the connection data
		CIPCFetchResult frOutput = fetch.FetchOutput(c);

		if (frOutput.IsOkay())
		{
			//create a CIPCOutputOCX object. This object communicates with the servers output
			m_pCIPCOutputOCX = new CIPCOutputOCX(*this, frOutput.GetShmName(), frOutput.GetCodePage());

			//create a text message
			m_sTextMsg.Empty();
			m_sTextMsg.Format(_T("Connection to host established."));


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

CConnectionRecord CMCLTCtrl::SetParamsConnectionRecord()
{
	//get the IP address of the server as string
	m_sIPAdress = _T("tcp:192.168.0.235");

	//create a connection record with necessary data
	CConnectionRecord c;
	c.SetPassword(m_sPassword);			//password of the permission "SuperVisor" if exists
	c.SetPermission(_T("SuperVisor"));	//permission
	c.SetDestinationHost(m_sIPAdress);	//IP address of the server as string
	//c.SetFieldValue(_T("PIN"),m_sPin);	//PIN number for connections to a DTS system

	DWORD dwOptions = 0;

	//SCO_WANT_RELAYS: inform the sample when the status of a relay has changed
	dwOptions |= SCO_WANT_RELAYS;	

	//SCO_CAN_GOP: send GOP's (group of pictures) on MPEG4 pictures to the sample
	//if not set, each MPEG4 picture will be send to the sample one by one
	dwOptions |= SCO_CAN_GOP;

	c.SetOptions(dwOptions);

	return c;
}
void CMCLTCtrl::CloseConnection()
{
/*
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
*/

	if(m_pCIPCControlInterface)
	{
		//close all connections to the server
		m_pCIPCControlInterface->CloseAllGateways();
	}
/*
	CString sHost = m_sIPAdress.Right(m_sIPAdress.GetLength()-4);
	m_sTextMsg.Format(_T("Connection to host < %s > closed."), sHost);

	UpdateData(FALSE);
*/
}
void CMCLTCtrl::Fill_Detectors(const CIPCInputRecordArray& detectors)
{
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

		CString s;
		s.Format(_T("\n%s - %s - %d"),sName, sStatus, dwID);
		m_sTextMsg += s; 
	}
}
/*********************************************************************************************/
void CMCLTCtrl::IndicateRecords(WORD wGroupID, 
								   int iNumGroups, 
								   int numRecords, 
								   const CIPCOutputRecord records[], 
								   CIPCOutputOCX* pOutputOCX)
{
	if(pOutputOCX)
	{
		//initialize
		m_pCIPCOutputOCX = pOutputOCX;
	}

	//	if(m_pSampleDlg)
	{
		m_paCameras.DeleteAll();
		m_paRelays.DeleteAll();
		for (int i=0; i<numRecords; i++)
		{
			if(records[i].IsCamera())
			{
				//fill the camera pointer array
				m_paCameras.Add(new CIPCOutputRecord(records[i]));
			}
			else if(records[i].IsRelay())
			{
				//fill the relay pointer array
				m_paRelays.Add(new CIPCOutputRecord(records[i]));
			}
		}

		if(m_paCameras.GetSize()>0)
		{
			//fill the camera list control on the main dialog
			//m_pSampleDlg->FillListCtrl_Cameras(m_paCameras);
		}

		if(m_paRelays.GetSize()>0)
		{
			//fill the relay list control on the main dialog
			//m_pSampleDlg->FillListCtrl_Relays(m_paRelays);
		}
	}
}
/*********************************************************************************************/
void CMCLTCtrl::PictureData(const CIPCPictureRecord &pict, 
							   CSecID idArchive, 
							   DWORD dwX, 
							   DWORD dwY, 
							   int iFrameRate)
{
	
	if (!IsWindowVisible())
	{
		//make sure the live picture control is visible on main dialog
		ShowWindow(SW_SHOW);
	}

	DWORD dwCamID = 0;

	//use a critical section to make sure that the picture record is used only in this thread
	CAutoCritSec acs(&m_csPictureRecord);

	//check the compression type of the picture
	if(pict.GetCompressionType() == COMPRESSION_MPEG4)
	{

		BOOL bSetGOP = FALSE;

		//in this Sample we only receive MPEG4 pictures with a frame rate of 25.
		//the Sample will receive 5 GOP's (group of pictures). each GOP's contains one
		//e.g. a frame rate of 5 picture is also possible
		if (iFrameRate == 25 && pict.GetPictureType() == SPT_GOP_PICTURE)
		{
			//how many pictures are in the GOP
			int nNumPics = pict.GetNumberOfPictures();

			if (nNumPics > 1)
			{
				//we have more than one picture in the GOP
				if (m_iPictureRequests>0)
				{
					//decrease the number of requests
					m_iPictureRequests--;
				}

				if (m_pPictureRecord)
				{
					//a GOP already exists, add GOP to queue
					m_GOPs.Add(new CIPCPictureRecord(pict));
				}
				else									
				{
					//GOP not exists, put GOP to picture record
					m_pPictureRecord = new CIPCPictureRecord(pict);
					m_nCurrentPicture = 0;
				}
				if (m_pMPEG4Decoder == NULL)
				{
					//create a MPEG4 decoder
					m_pMPEG4Decoder = new CMPEG4Decoder();
				}

				DWORD dwTimeStep = pict.GetFrameTimeStep();
				double dTimeStepAvg = 40000.0;	// presume 25 fps

				if (m_uGOPTimer == 0)			// start timer
				{
					// round to the nearest integer
					dwTimeStep = (DWORD)((dTimeStepAvg / 1000.0) + 0.5);
					SetGOPTimer(dwTimeStep);
					m_nCurrentPicture = 0;
				}
				bSetGOP = TRUE;
			}
		}

		if (!bSetGOP)
		{
			//we have no GOP's, we have only one picture e.g. 5fps with one picture in one GOP
			if (m_uGOPTimer)
			{
				if(m_pPictureRecord)
				{
					//get camera ID of the current picture
					dwCamID = m_pPictureRecord->GetCamID().GetID();
					m_nCurrentPicture = -1;
					DELETE_PTR(m_pPictureRecord);

					KillGOPTimer();

					//request the next picture
					PostMessage(WM_REQUEST_PICTURES, dwCamID); 
				}
			}

			CAutoCritSec acs(&m_csPictureRecord);
			if (m_iPictureRequests>0)
			{
				m_iPictureRequests--;
			}

			DELETE_PTR(m_pPictureRecord);

			//save the current picture
			m_pPictureRecord = new CIPCPictureRecord(pict);

			BOOL bLowBandWidth = FALSE;
			if(m_pCIPCOutputOCX)
			{
				bLowBandWidth = m_pCIPCOutputOCX->IsLowBandwidth();
			}
			// doppelt hält besser
			// wenn der Decoder zu voll ist Bilder weglassen,
			// besser als aller RAM wech und Crash
			//TRACE("### m_pMPEG4Decoder: 0x%x\n", m_pMPEG4Decoder);
			if (m_pMPEG4Decoder == NULL)
			{
				//create a MPEG4 decoder
				m_pMPEG4Decoder = new CMPEG4Decoder();
			}

			//decode and show the current picture
			//if we have too much picture for decoding in the decoder drop a frame to ensure 
			//a minimized use of RAM space 
			if (   bLowBandWidth
				|| (   (m_pMPEG4Decoder->GetDecoderQueueLength()<=(DWORD)iFrameRate)
				&& (!m_bFrameWasDropped || (pict.GetPictureType() == SPT_FULL_PICTURE || pict.GetPictureType() == SPT_GOP_PICTURE))
				)
				)
			{
				if(m_pPictureRecord)
				{
					dwCamID = m_pPictureRecord->GetCamID().GetID();
				}

				//decode the current picture
				BOOL bDecoded = m_pMPEG4Decoder->DecodeMpeg4FromMemory((BYTE*)pict.GetData(), pict.GetDataLength());
				m_bFrameWasDropped = !bDecoded;
				acs.Unlock();

				//show the decoded picture
				PostMessage(WM_DECODE_VIDEO, dwCamID);
			}
			else
			{
				m_bFrameWasDropped = TRUE;
			}
		}
	}
	else if(pict.GetCompressionType() == COMPRESSION_JPEG)
	{
		//create a Jpeg decoder
		if (m_pJpeg == NULL)
		{
			m_pJpeg = new CJpeg();
		}

		if (m_pJpeg)
		{
			DELETE_PTR(m_pPictureRecord);

			//save the current picture
			m_pPictureRecord = new CIPCPictureRecord(pict);

			if(m_pPictureRecord)
			{
				//decode the current picture
				dwCamID = m_pPictureRecord->GetCamID().GetID();
				if (!m_pJpeg->DecodeJpegFromMemory((BYTE*)m_pPictureRecord->GetData(), m_pPictureRecord->GetDataLength()))
				{
					TRACE(_T("DecodeJpegFromMemory failed \n"));
				}

				//show the decoded picture
				PostMessage(WM_DECODE_VIDEO, dwCamID);

				//request the next Jpeg picture
				PostMessage(WM_REQUEST_PICTURES, dwCamID); 
			}
		}
	}
	
}
/*********************************************************************************************/
LRESULT CMCLTCtrl::DoRequestPicture(WPARAM wParam, LPARAM pParam)
{
	//wParam contains the new selected camera ID, 
	//pParam contains the last active camera ID which has to be switched off
	//on first call of this function wParam and pParam contains the same camera ID
	//if requesting pictures of the same camera pParam will be 0

	CSecID secIDNewCamera((DWORD)wParam);

	if(m_pCIPCOutputOCX)
	{
		if(pParam != 0)
		{
/*			//request pictures of a different camera
			m_secIDCurrentCamera = (DWORD)pParam;

			//stop requesting of picture from the last camera
			m_pCIPCOutputOCX->DoRequestStopVideo(m_secIDCurrentCamera,0);

			//start requesting picture of the new current camera
			SetCompressionTypeOfCamera(secIDNewCamera);
			m_pCIPCOutputOCX->RequestLivePictures(secIDNewCamera, m_CompressionType);
			m_secIDCurrentCamera = secIDNewCamera;

			//reset all variables to start with the new current camera
			m_iPictureRequests = 1;
			m_nCurrentPicture = -1;
			m_GOPs.SafeDeleteAll();
			DELETE_PTR(m_pMPEG4Decoder);
			DELETE_PTR(m_pJpeg);
			DELETE_PTR(m_pPictureRecord);
*/
		}
//		else
		{
//			if(m_secIDCurrentCamera == secIDNewCamera)
			{					
				//request picture of the same camera as before
				m_pCIPCOutputOCX->RequestLivePictures(secIDNewCamera, COMPRESSION_MPEG4);
				m_iPictureRequests++;
			}
		}
	}
	return 0;
}
/*********************************************************************************************/
void CMCLTCtrl::SetGOPTimer(UINT uElapse)
{
	if (m_bUseMMTimer)
	{
		TIMECAPS tc;
		MMRESULT mmr = 0;
		if (gm_wTimerRes == 0)
		{
			mmr = ::timeGetDevCaps(&tc, sizeof(TIMECAPS));
			if (mmr == TIMERR_NOERROR) 
			{
				gm_wTimerRes = min(max(tc.wPeriodMin, 5), tc.wPeriodMax);
				mmr = ::timeBeginPeriod(gm_wTimerRes);
			}

			if (mmr != TIMERR_NOERROR)
			{
				m_bUseMMTimer = FALSE;
			}

		}
		if (m_bUseMMTimer)
		{
			m_uGOPTimer = ::timeSetEvent(uElapse, gm_wTimerRes, GOPTimer, 0, TIME_PERIODIC);
		}
	}

	if (!m_bUseMMTimer)
	{
		m_uGOPTimer = CWnd::SetTimer(110, uElapse, NULL);
	}
}
/*********************************************************************************************/
void CMCLTCtrl::KillGOPTimer()
{
	m_GOPs.SafeDeleteAll();
	if (m_uGOPTimer)
	{
		UINT uIDtemp = m_uGOPTimer;

		// set to zero, to make event id invalid
		m_uGOPTimer = 0;				

		if (m_bUseMMTimer)
		{
			::timeKillEvent(uIDtemp);
		}
		else
		{
			CWnd::KillTimer(uIDtemp);
		}
	}
}
/*********************************************************************************************/
void CALLBACK CMCLTCtrl::GOPTimer(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{ 
	if(m_pThis)
	{
		if(!m_pThis->m_bGOPTimerThreadPriorIsSet)
		{
			//the thread priority of the GOPtimer has to be set to "normal"
			//if not, the GOPtimer will slow down the computer
			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
			m_pThis->m_bGOPTimerThreadPriorIsSet = TRUE;
		}

		if (m_pThis->m_uGOPTimer)
		{
			m_pThis->OnTimer(wTimerID);
		}
	}
}
/*********************************************************************************************/
LRESULT CMCLTCtrl::OnDecodeVideo(WPARAM wParam, LPARAM pParam)
{
	//draw the new live picture
	//InvalidateRect(NULL);
	InvalidateControl();
	TRACE(_T("### UpdateWindow\n"));
	return 0;
}
/*********************************************************************************************/
void CMCLTCtrl::OnTimer(UINT nIDEvent)
{
	CAutoCritSec acs(&m_csPictureRecord);
	if (nIDEvent == m_uGOPTimer)
	{
		if (m_nCurrentPicture == -1)
		{
			//no picture in GOP to decode, request next picture
			DWORD dwCamID = 0;
			if(m_pPictureRecord)
			{
				dwCamID = m_pPictureRecord->GetCamID().GetID();

				//request the next picture of the same camera as before
				
				PostMessage(WM_REQUEST_PICTURES, dwCamID);
			}
			return;
		}

		int nNumPictures = 0;
		BOOL bNumPicturesReduced = FALSE;
		if (m_pPictureRecord)
		{
			int nNumGOPS = (int)m_GOPs.GetSize();
			nNumPictures = m_pPictureRecord->GetNumberOfPictures();
			if (nNumGOPS > 1)
			{
				//more than one GOP, reduce
				nNumPictures = nNumPictures - nNumGOPS + 1;
				bNumPicturesReduced = TRUE;
			}

			if (m_iPictureRequests<5)
			{
				//do not more than 5 request
				//only start next request when we received the first picture of 5
				PostMessage(WM_REQUEST_PICTURES, m_pPictureRecord->GetCamID().GetID());
				m_iPictureRequests++;
			}

			if (m_nCurrentPicture >= nNumPictures)
			{
				//get the next picture from within the GOP
				GetNextPictureOfGOP();
			}
		}

		if (m_pPictureRecord)
		{
			BOOL dwCamID = m_pPictureRecord->GetCamID().GetID();
			if(!bNumPicturesReduced)
			{
				nNumPictures = m_pPictureRecord->GetNumberOfPictures();
			}
			if (m_pMPEG4Decoder == NULL)
			{
				//create MPEG4 decoder
				m_pMPEG4Decoder = new CMPEG4Decoder();
			}

			//decode the current live picture
			m_pMPEG4Decoder->DecodeMpeg4FromMemory((BYTE*)m_pPictureRecord->GetData(m_nCurrentPicture),
				m_pPictureRecord->GetDataLength(m_nCurrentPicture));

			//show the decoded picture in the live picture control
			PostMessage(WM_DECODE_VIDEO, dwCamID); 

			m_nCurrentPicture++;

			if (m_nCurrentPicture == nNumPictures)
			{
				//get the next picture from within the GOP
				GetNextPictureOfGOP();
			}
		}
	}
	acs.Unlock();
	COleControl::OnTimer(nIDEvent);
}
/*********************************************************************************************/
void CMCLTCtrl::GetNextPictureOfGOP()
{
	int nNumGops = (int)m_GOPs.GetSize();
	if (m_pPictureRecord)
	{
		//we already have a picture, request the next
		DWORD dwCamID = m_pPictureRecord->GetCamID().GetID();
		PostMessage(WM_REQUEST_PICTURES, dwCamID);
	}

	DELETE_PTR(m_pPictureRecord);

	if (nNumGops)
	{
		//get the next picture from within the GOP
		m_pPictureRecord = m_GOPs.GetAt(0);
		m_GOPs.RemoveAt(0);
		m_nCurrentPicture = 0;
	}
	else
	{
		m_nCurrentPicture = -1;
	}
}