// ExpansionCodeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ExpansionCode.h"
#include "ExpansionCodeDlg.h"
#include ".\expansioncodedlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDIP_TRANSMITTER 1
#define IDIP_RECEIVER	 2
#define DTS_TRANSMITTER  3

#define DLG_ITEMS_DISTANCE			1000
#define DLG_ITEMS_IDIP_TRANSMITTER	2000
#define DLG_ITEMS_IDIP_RECEIVER		3000
#define DLG_ITEMS_DTS_TRANSMITTER	4000


/////////////////////////////////////////////////////////////////////////////
// CExpansionCodeDlg dialog

CExpansionCodeDlg::CExpansionCodeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExpansionCodeDlg::IDD, pParent)
	, m_bIdipTimer(false)
	, m_bIdipPTZ(false)
	, m_bIdipMonitor2(false)
	, m_bIdipMaps(false)
	, m_bIdipReset(false)
	, m_iQCams(0)
{
	//{{AFX_DATA_INIT(CExpansionCodeDlg)
	m_bISDN = FALSE;
	m_bLAN = TRUE;
	m_sCode = _T("");
	m_sSerial = _T("");
	m_bBackup = TRUE;
	m_bDVD = TRUE;
	m_iIdipHost=0;

	m_iNrQCameras = 0; //erster Eintrag => 4 Kameras
	m_iNrIPCameras = -1;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExpansionCodeDlg)
	DDX_Check(pDX, IDC_DTS_CHECK_ISDN, m_bISDN);
	DDX_Check(pDX, IDC_DTS_CHECK_LAN, m_bLAN);
	DDX_Text(pDX, IDC_EDIT_CODE, m_sCode);
	DDX_Text(pDX, IDC_EDIT_SERIAL, m_sSerial);
	DDX_Check(pDX, IDC_DTS_CHECK_CDRW, m_bBackup);
	DDX_Check(pDX, IDC_DTS_CHECK_DVD, m_bDVD);
	DDX_Check(pDX, IDC_CHECK_IDIP_RECEIVER_TIMER, m_bIdipTimer);
	DDX_Check(pDX, IDC_CHECK_IDIP_RECEIVER_PTZ, m_bIdipPTZ);
	DDX_Check(pDX, IDC_CHECK_IDIP_RECEIVER_MONITOR2, m_bIdipMonitor2);
	DDX_Check(pDX, IDC_CHECK_IDIP_RECEIVER_MAPS, m_bIdipMaps);
	DDX_Control(pDX, IDC_GRP_OPTIONS, m_Static_Options);	
	DDX_Radio(pDX, IDC_DTS_RADIO_QCAM_4, m_iQCams);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CExpansionCodeDlg, CDialog)
	//{{AFX_MSG_MAP(CExpansionCodeDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_DTS_CHECK_ISDN, OnCheckIsdn)
	ON_BN_CLICKED(IDC_DTS_CHECK_LAN, OnCheckLan)
	ON_EN_CHANGE(IDC_EDIT_SERIAL, OnChangeEditSerial)
	ON_BN_CLICKED(IDC_DTS_CHECK_CDRW, OnCheckCdrw)
	ON_BN_CLICKED(IDC_DTS_CHECK_DVD, OnCheckDvd)
	ON_BN_CLICKED(IDC_CHECK_IDIP_RECEIVER_TIMER, OnCheckIdipTimer)
	ON_BN_CLICKED(IDC_CHECK_IDIP_RECEIVER_PTZ, OnCheckIdipPTZ)
	ON_BN_CLICKED(IDC_CHECK_IDIP_RECEIVER_MONITOR2, OnCheckIdipDualhead)
	ON_BN_CLICKED(IDC_CHECK_IDIP_RECEIVER_MAPS, OnCheckIdipMaps)
	ON_BN_CLICKED(IDC_RADIO_IDIP_PL, OnCheckIdipPLReceiver)
	ON_BN_CLICKED(IDC_RADIO_IDIP_XL, OnCheckIdipXLReceiver)
	ON_BN_CLICKED(IDC_RADIO_DTS, OnCheckDTSTransmitter)
	ON_CBN_SELCHANGE(IDC_COMBO_IDIP_RECEIVER_NR_HOST, OnCbnSelchangeComboHost)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_DTS_RADIO_QCAM_8, OnBnClickedRadioQCam8)
	ON_BN_CLICKED(IDC_DTS_RADIO_QCAM_16, OnBnClickedRadioQCam16)
	ON_BN_CLICKED(IDC_DTS_RADIO_QCAM_4, OnBnClickedRadioQcam4)
	ON_BN_CLICKED(IDC_RADIO_IDIP_TRANSMITTER, OnCheckIdipXLTransmitter)
	ON_CBN_SELCHANGE(IDC_DONGLE_COMBO_NR_Q_CAMERAS, OnCbnSelchangeDongleComboNrQCameras)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_DONGLE_CHECK_BACKUP, IDC_DONGLE_CHECK_HTTP, 	OnDongleDlgItemRange)
	ON_CBN_SELCHANGE(IDC_DONGLE_COMBO_NR_IP_CAMERAS, OnCbnSelchangeDongleComboNrIpCameras)
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExpansionCodeDlg message handlers

BOOL CExpansionCodeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	//activate Idip PL Options first
	((CButton*)GetDlgItem(IDC_RADIO_IDIP_PL))->SetCheck(1);

	//Anwahlkarten und 2 Monitorbetrieb nur bei XL freischalten
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_MAPS))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_MAPS))->EnableWindow(0);
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_MONITOR2))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_MONITOR2))->EnableWindow(0);

	ShowHideControls(IDIP_RECEIVER); //show idip/dts related controls
	((CComboBox*)GetDlgItem(IDC_COMBO_IDIP_RECEIVER_NR_HOST))->SetCurSel(0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CExpansionCodeDlg::OnPaint() 
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
/////////////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CExpansionCodeDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::OnCheckIsdn() 
{
	CalcNewCodeDTSTransmitter();
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::OnCheckLan() 
{
	CalcNewCodeDTSTransmitter();
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::OnChangeEditSerial() 
{
	if(    ((CButton*)GetDlgItem(IDC_RADIO_IDIP_PL))->GetCheck()
		|| ((CButton*)GetDlgItem(IDC_RADIO_IDIP_XL))->GetCheck())
	{
		CalcNewCodeIdipReceiver();
	}
	else if(((CButton*)GetDlgItem(IDC_RADIO_DTS))->GetCheck())
	{
		CalcNewCodeDTSTransmitter();
	}
	else if (((CButton*)GetDlgItem(IDC_RADIO_IDIP_TRANSMITTER))->GetCheck())
	{
		CalcNewCodeIdipTransmitter();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::CalcNewCodeDTSTransmitter()
{
	UpdateData();

	DWORD dwSerial = 0;
	DWORD dwDongle = 0;

	CString s,sSerial;

	sSerial = m_sSerial;

	if (!sSerial.IsEmpty())
	{
		sSerial.MakeLower();
		for (int i=0;i<sSerial.GetLength();i++)
		{
			if (   '0' <= sSerial[i]
				&& sSerial[i] <= '9')
			{
				s += sSerial[i];
			}
		}
		
		if (s.IsEmpty())
		{
			for (int i=0;i<sSerial.GetLength() && i<8;i++)
			{
				int iNum = sSerial[i] % 10;
				s += (char)('0'+iNum);
			}
		}
		
		m_sCode.Empty();
		
		if (1 == _stscanf(s,_T("%d"),&dwSerial))
		{
			if (m_bISDN)
			{
				dwDongle |= FLAG_ISDN;
			}
			if (m_bLAN)
			{
				dwDongle |= FLAG_LAN;
			}
			if (m_bBackup)
			{
				dwDongle |= FLAG_BACKUP;
			}
			if (m_bDVD)
			{
				dwDongle |= FLAG_DVD;
			}
			if (m_iQCams == 0) //4 Q Cameras
			{
				dwDongle &= ~FLAG_Q_CAM_8;
				dwDongle &= ~FLAG_Q_CAM_16;
			}
			if (m_iQCams == 1) //8 Q Cameras
			{
				dwDongle |= FLAG_Q_CAM_8;
				dwDongle &= ~FLAG_Q_CAM_16;
			}
			if (m_iQCams == 2) //16 Q Cameras
			{
				dwDongle |= FLAG_Q_CAM_16;
				dwDongle &= ~FLAG_Q_CAM_8;
			}			
			
			CRSA encoder(0,0);
			encoder.EncodeSerialDongle(dwSerial,dwDongle,m_sCode);
			m_sCode.MakeUpper();
			TRACE(_T("dongle encoded %d,%08lx %s\n"),dwSerial,dwDongle,m_sCode);
		}
		else
		{
			MessageBeep(0);
		}
		UpdateData(FALSE);
	}

#ifdef _DEBUG
	CRSA decoder(0,0);
	DWORD dwSerialDecode,dwDongleDecode;
	decoder.DecodeSerialDongle(m_sCode,dwSerialDecode,dwDongleDecode);
	TRACE(_T("dongle decoded %d,%08lx\n"),dwSerialDecode,dwDongleDecode);
#endif
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::CalcNewCodeIdipReceiver()
{
	UpdateData();

	DWORD dwSerial = 0;
	DWORD dwDongle = 0;

	CString s,sSerial;

	sSerial = m_sSerial;

	if (!sSerial.IsEmpty())
	{
		sSerial.MakeLower();
		for (int i=0;i<sSerial.GetLength();i++)
		{
			if (   '0' <= sSerial[i]
			&& sSerial[i] <= '9')
			{
				s += sSerial[i];
			}
		}

		if (s.IsEmpty())
		{
			for (int i=0;i<sSerial.GetLength() && i<8;i++)
			{
				int iNum = sSerial[i] % 10;
				s += (char)('0'+iNum);
			}
		}

		m_sCode.Empty();

		if (1 == _stscanf(s,_T("%d"),&dwSerial))
		{
			if (m_bIdipReset)
			{
				dwDongle |= FLAG_RESET;
			}
			if (m_bIdipTimer)
			{
				dwDongle |= FLAG_TIMER;
			}
			if (m_bIdipPTZ)
			{
				dwDongle |= FLAG_PTZ;
			}
			if (m_bIdipMonitor2)
			{
				dwDongle |= FLAG_MONITOR;
			}
			if (m_bIdipMaps)
			{
				dwDongle |= FLAG_MAPS;
			}

			switch (m_iIdipHost)
			{
			case 0:
				dwDongle |= FLAG_HOST5;
				break;
			case 1:
				dwDongle |= FLAG_HOST10;
				break;
			case 2:
				dwDongle |= FLAG_HOST20;
				break;
			case 3:
				dwDongle |= FLAG_HOST30;
				break;
			case 4:
				dwDongle |= FLAG_HOST40;
				break;
			case 5:
				dwDongle |= FLAG_HOST50;
				break;
			case 6:
				dwDongle |= FLAG_HOST60;
				break;
			case 7: //unbegrenzt
				dwDongle |= FLAG_HOST0;
				break;
			default:
				dwDongle |= FLAG_HOST5;
				break;
			}
			CRSA encoder(0,0);
			//use the first 7 digits of the serial numer (9 digits are incl checksum) because
			//9 digits could be to much (e.g. 711111221 is too big)
			DWORD dwSerialWithoutChecksum = dwSerial/100;
			encoder.EncodeSerialDongle(dwSerialWithoutChecksum,dwDongle,m_sCode);
			m_sCode.MakeUpper();
			TRACE(_T("dongle encoded %d,%08lx %s\n"),dwSerialWithoutChecksum,dwDongle,m_sCode);
		}
		else
		{
			MessageBeep(0);
		}
		UpdateData(FALSE);
	}

#ifdef _DEBUG
	CRSA decoder(0,0);
	DWORD dwSerialDecode,dwDongleDecode;
	decoder.DecodeSerialDongle(m_sCode,dwSerialDecode,dwDongleDecode);
	TRACE(_T("dongle decoded %d,%08lx\n"),dwSerialDecode,dwDongleDecode);
#endif

}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::CalcNewCodeIdipTransmitter()
{
	UpdateData();

	DWORD dwSerial = 0;
	DWORD dwDongle = 0;

	CString s,sSerial;

	sSerial = m_sSerial;

	if (!sSerial.IsEmpty())
	{
		sSerial.MakeLower();
		for (int i=0;i<sSerial.GetLength();i++)
		{
			if (   '0' <= sSerial[i]
			&& sSerial[i] <= '9')
			{
				s += sSerial[i];
			}
		}

		if (s.IsEmpty())
		{
			for (int i=0;i<sSerial.GetLength() && i<8;i++)
			{
				int iNum = sSerial[i] % 10;
				s += (char)('0'+iNum);
			}
		}


		m_sCode.Empty();

		if (1 == _stscanf(s,_T("%d"),&dwSerial))
		{
			if (((CButton*)GetDlgItem(IDC_DONGLE_CHECK_BACKUP))->GetCheck())
			{
				dwDongle |= DONGLE_FLAG_BACKUP;
			}

			if (((CButton*)GetDlgItem(IDC_DONGLE_CHECK_IP))->GetCheck())
			{
				dwDongle |= DONGLE_FLAG_IP;

				switch (m_iNrIPCameras)
				{
				case 0:
					dwDongle |= DONGLE_FLAG_IP_CAM_4;
					break;
				case 1:
					dwDongle |= DONGLE_FLAG_IP_CAM_8;
					break;
				case 2:
					dwDongle |= DONGLE_FLAG_IP_CAM_16;
					break;
				case 3:
					dwDongle |= DONGLE_FLAG_IP_CAM_24;
					break;
				case 4:
					dwDongle |= DONGLE_FLAG_IP_CAM_32;
					break;
				default:
					//dwDongle bleibt unverändert
					break;
				}
			}

			if (((CButton*)GetDlgItem(IDC_DONGLE_CHECK_ISDN))->GetCheck())
			{
				dwDongle |= DONGLE_FLAG_ISDN;
			}
			if (((CButton*)GetDlgItem(IDC_DONGLE_CHECK_PTZ))->GetCheck())
			{
				dwDongle |= DONGLE_FLAG_PTZ;
			}
			if (((CButton*)GetDlgItem(IDC_DONGLE_CHECK_SERIAL_ALARM))->GetCheck())
			{
				dwDongle |= DONGLE_FLAG_SERIAL_ALARM;
			}
			if (((CButton*)GetDlgItem(IDC_DONGLE_CHECK_GAA))->GetCheck())
			{
				dwDongle |= DONGLE_FLAG_GAA;
			}
			if (((CButton*)GetDlgItem(IDC_DONGLE_CHECK_ACCESS))->GetCheck())
			{
				dwDongle |= DONGLE_FLAG_ACCESS;
			}
			if (((CButton*)GetDlgItem(IDC_DONGLE_CHECK_SCANNER))->GetCheck())
			{
				dwDongle |= DONGLE_FLAG_SCANNER;
			}
			if (((CButton*)GetDlgItem(IDC_DONGLE_CHECK_PARK))->GetCheck())
			{
				dwDongle |= DONGLE_FLAG_PARK;
			}
			if (((CButton*)GetDlgItem(IDC_DONGLE_CHECK_ICPCON))->GetCheck())
			{
				dwDongle |= DONGLE_FLAG_ICPCON;
			}
			if (((CButton*)GetDlgItem(IDC_DONGLE_CHECK_HTTP))->GetCheck())
			{
				dwDongle |= DONGLE_FLAG_HTTP;
			}
			if (((CButton*)GetDlgItem(IDC_DONGLE_CHECK_QUNIT))->GetCheck())
			{
				dwDongle |= DONGLE_FLAG_Q;

				switch (m_iNrQCameras)
				{
				case 0:
					dwDongle |= DONGLE_FLAG_Q_CAM_4;
					break;
				case 1:
					dwDongle |= DONGLE_FLAG_Q_CAM_8;
					break;
				case 2:
					dwDongle |= DONGLE_FLAG_Q_CAM_16;
					break;
				case 3:
					dwDongle |= DONGLE_FLAG_Q_CAM_24;
					break;
				case 4:
					dwDongle |= DONGLE_FLAG_Q_CAM_32;
					break;
				default:
					dwDongle |= DONGLE_FLAG_Q_CAM_4;
					break;
				}
			}

			DWORD dwDongleCheckSum = CreateDongleChecksum(dwDongle);
            
			//cut the last 2 digits of the serial number (computer name) 
			//and add the checksum for the dongle
			dwSerial = dwSerial/100;
			dwSerial = dwSerial * 100 + dwDongleCheckSum;
			
			CRSA encoder(0,0);
			encoder.EncodeSerialDongle(dwSerial,dwDongle,m_sCode);
			m_sCode.MakeUpper();
			TRACE(_T("dongle encoded %d,%08lx %s\n"),dwSerial,dwDongle,m_sCode);
		}
		else
		{
			MessageBeep(0);
		}
		UpdateData(FALSE);
	}

#ifdef _DEBUG
	CRSA decoder(0,0);
	DWORD dwSerialDecode,dwDongleDecode;
	decoder.DecodeSerialDongle(m_sCode,dwSerialDecode,dwDongleDecode);
	TRACE(_T("dongle decoded %d,%08lx\n"),dwSerialDecode,dwDongleDecode);
#endif
}/////////////////////////////////////////////////////////////////////////////
DWORD CExpansionCodeDlg::CreateDongleChecksum(DWORD dwDongle)
{
	DWORD dwDongleCheckSum = 0;

	DWORD dwDummy1;
	int iShiftLeft = 28;
	int iShiftRight = 0;
	for (int i = 8; i > 0; i--)
	{
		dwDummy1 = dwDongle >> iShiftRight;
		dwDummy1 = dwDummy1 << iShiftLeft;
		dwDummy1 = dwDummy1 >> iShiftLeft;
		dwDongleCheckSum += dwDummy1;
		iShiftRight += 4;
	}

	if(dwDongleCheckSum > 99)
	{
		dwDongleCheckSum = 99;
	}
	return dwDongleCheckSum;
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::OnCheckCdrw() 
{
	if(!((CButton*)GetDlgItem(IDC_DTS_CHECK_CDRW))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_DTS_CHECK_DVD))->SetCheck(0);
	}
	CalcNewCodeDTSTransmitter();
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::OnCheckDvd() 
{
	((CButton*)GetDlgItem(IDC_DTS_CHECK_CDRW))->SetCheck(1);
	CalcNewCodeDTSTransmitter();	
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::OnCheckIdipTimer()
{
	CalcNewCodeIdipReceiver();
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::OnCheckIdipPTZ()
{
	CalcNewCodeIdipReceiver();
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::OnCheckIdipDualhead()
{
	CalcNewCodeIdipReceiver();
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::OnCheckIdipMaps()
{
	CalcNewCodeIdipReceiver();
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::OnCheckIdipPLReceiver()
{
	ShowHideControls(IDIP_RECEIVER);
	
	//0 => 5 Gegenstationen
	((CComboBox*)GetDlgItem(IDC_COMBO_IDIP_RECEIVER_NR_HOST))->SetCurSel(0);
	
	OnCbnSelchangeComboHost();	 //aktualisieren den ausgewählten Wert

	//Timer und PTZ sollen IMMER freigeschaltet sein (PL und XL)
	m_bIdipTimer = TRUE;
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_TIMER))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_TIMER))->EnableWindow(0);
	m_bIdipPTZ = TRUE;
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_PTZ))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_PTZ))->EnableWindow(0);

	//Anwahlkarten und 2 Monitorbetrieb nicht bei PL freischalten, nur bei XL
	m_bIdipMonitor2 = FALSE;
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_MAPS))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_MAPS))->EnableWindow(0);
	m_bIdipMaps = FALSE;
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_MONITOR2))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_MONITOR2))->EnableWindow(0);

	CalcNewCodeIdipReceiver();
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::OnCheckIdipXLReceiver()
{
	ShowHideControls(IDIP_RECEIVER);

	//1 => 10 Gegenstationen
	((CComboBox*)GetDlgItem(IDC_COMBO_IDIP_RECEIVER_NR_HOST))->SetCurSel(1);

	OnCbnSelchangeComboHost();	 //aktualisieren den ausgewählten Wert

	//Timer und PTZ sollen IMMER freigeschaltet sein (PL und XL)
	m_bIdipTimer = TRUE;
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_TIMER))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_TIMER))->EnableWindow(0);
	m_bIdipPTZ = TRUE;
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_PTZ))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_PTZ))->EnableWindow(0);

	//Anwahlkarten und 2 Monitorbetrieb nur bei XL freischalten
	m_bIdipMonitor2 = TRUE;
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_MAPS))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_MAPS))->EnableWindow(0);

	m_bIdipMaps = TRUE;
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_MONITOR2))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_MONITOR2))->EnableWindow(0);

	CalcNewCodeIdipReceiver();
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::OnCheckIdipXLTransmitter()
{

	ShowHideControls(IDIP_TRANSMITTER);

	//0 => 4 Kameras
	((CComboBox*)GetDlgItem(IDC_DONGLE_COMBO_NR_Q_CAMERAS))->SetCurSel(0);

	OnDongleDlgItemRange(IDC_DONGLE_COMBO_NR_Q_CAMERAS);	 //aktualisieren den ausgewählten Wert
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::OnCheckDTSTransmitter()
{
	ShowHideControls(DTS_TRANSMITTER);
	CalcNewCodeDTSTransmitter();
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::ShowHideControls(int nType)
{
	switch (nType)
	{
	case IDIP_RECEIVER:
		{
			ShowIdipReceiver();
		}
		break;
	case DTS_TRANSMITTER:
		{
			ShowDTSTransmitter();
		}
		break;
	case IDIP_TRANSMITTER:
		{
			ShowIdipTransmitter();
		}
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::ShowIdipReceiver()
{
	//first: hide all dlg items
	ShowHideDlgItems(IDC_GRP_OPTIONS, FALSE, DLG_ITEMS_IDIP_TRANSMITTER, TRUE);

	//second: show all idip receiver (PL/XL) dlg items
	ShowHideDlgItems(IDC_GRP_OPTIONS, TRUE, DLG_ITEMS_IDIP_RECEIVER);
	
	m_Static_Options.SetWindowText(_T("Optionen Idip Receiver"));
	GetDlgItem(IDC_TXT_LICENSE)->SetWindowText(_T("Lizenznummer:"));

	//Timer und PTZ sollen IMMER freigeschaltet sein (PL und XL)
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_TIMER))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_TIMER))->EnableWindow(0);
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_PTZ))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_CHECK_IDIP_RECEIVER_PTZ))->EnableWindow(0);
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::ShowDTSTransmitter()
{
	//first: hide all dlg items
	ShowHideDlgItems(IDC_GRP_OPTIONS, FALSE, DLG_ITEMS_DTS_TRANSMITTER, TRUE);

	//second: show all dts transmitter dlg items
	ShowHideDlgItems(IDC_GRP_OPTIONS, TRUE, DLG_ITEMS_DTS_TRANSMITTER);

	m_Static_Options.SetWindowText(_T("Optionen DTS Sender"));
	GetDlgItem(IDC_TXT_LICENSE)->SetWindowText(_T("Seriennummer:"));
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::ShowIdipTransmitter()
{
	//first: hide all dlg items
	ShowHideDlgItems(IDC_GRP_OPTIONS, FALSE, DLG_ITEMS_IDIP_TRANSMITTER, TRUE);
	
	//second: show all idip transmitter dlg items
	ShowHideDlgItems(IDC_GRP_OPTIONS, TRUE, DLG_ITEMS_IDIP_TRANSMITTER);
	
	m_Static_Options.SetWindowText(_T("Optionen Idip Sender"));
	GetDlgItem(IDC_TXT_LICENSE)->SetWindowText(_T("Seriennummer:"));
}
/////////////////////////////////////////////////////////////////////////
// bShow -> show/hide section, bEnable -> enable/disable section
void CExpansionCodeDlg::ShowHideDlgItems(int nDlg, BOOL bShow, int nGrpSection, BOOL bHideAll /*=FALSE*/)
{
	CWnd *pWnd = GetDlgItem(nDlg);
	int iShowHide;
	bShow ? iShowHide  = SW_SHOW :  iShowHide  = SW_HIDE;

	CRect rc, rcGrp;

	pWnd->GetWindowRect(&rcGrp);
	pWnd = GetWindow(GW_CHILD);
	while (pWnd)
	{
		CString s;
		pWnd->GetDlgItemText(pWnd->GetDlgCtrlID(), s);
		pWnd->GetWindowRect(&rc);
		if (rcGrp.PtInRect(rc.TopLeft()))
		{
			if(bHideAll)
			{
				pWnd->ShowWindow(iShowHide);
			}
			else
			{
				int iID = pWnd->GetDlgCtrlID();
				int iMinDlgItemID = nGrpSection;
				int iMaxDlgItemID = nGrpSection + DLG_ITEMS_DISTANCE;
				if(iID > iMinDlgItemID && iID < iMaxDlgItemID)
				{
					pWnd->ShowWindow(iShowHide);
				}
			}
		}
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}
	GetDlgItem(IDC_GRP_OPTIONS)->ShowWindow(SW_SHOW);
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::OnCbnSelchangeComboHost()
{
	int iCurSel = ((CComboBox*)GetDlgItem(IDC_COMBO_IDIP_RECEIVER_NR_HOST))->GetCurSel();

	m_iIdipHost=0;

	if(iCurSel >= 0 && iCurSel <= 7)
	{
		m_iIdipHost = iCurSel;
	}

	CalcNewCodeIdipReceiver();
}

/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::OnBnClickedOk()
{
if(0) //erzeuge Freischaltcodes 
		//es wird die Datei "serial1.cvs" ins lokale Verzeichnis dieses Programms kopiert
		//dann werden die Seriennummern ausgelesen, dann der Freischaltcode
		//für die Idip Receiver PL und XL erzeugt und mit der Seriennummer in die
		//date "serial1_new.csv" kopiert.
		//anschließend auch die tabelle serail4 nehmen
	{
		CString sTabelle1 = "serial1.csv";
		CString sTabelle1New = "serial1_new.csv";
		CString sNew;
		CString sSemikolon = ";";
		CStdioFile file;
		CStdioFile newfile(sTabelle1New, CFile::modeCreate | CFile::modeReadWrite);

		if (file.Open(sTabelle1,CFile::modeReadWrite))
		{
			CString sLine;
			int iCount = 0;

			while(file.ReadString(sLine)) 
			{
				m_sSerial.Empty();
				m_sCode.Empty();
				//TRACE(_T("Line: %s\n"), sLine);
				
				//setze gelesene Seriennummer ins Seriennummernfeld und lies 
				//den Freischaltcode für IdipPL mit 5 Gegenstationen (Basisversion) aus.
				//Den Freischaltcode dann in die Datei schreiben
				
				m_sSerial = sLine;

				for (int i=0; i<2; i++)
				{
					if(i==0)
					{
						//PL
						((CButton*)GetDlgItem(IDC_RADIO_IDIP_PL))->SetCheck(1);
						((CButton*)GetDlgItem(IDC_RADIO_IDIP_XL))->SetCheck(0);
						UpdateWindow();
						UpdateData(FALSE);
						OnCheckIdipPLReceiver(); //Berechne neuen Code, ist dann in m_sCode gespeichert
						
						sNew = m_sSerial + sSemikolon + m_sCode;
						//TRACE(_T("new Line: %s\n"), sNew);
					}

					if(i==1)
					{
						//XP
						((CButton*)GetDlgItem(IDC_RADIO_IDIP_PL))->SetCheck(0);
						((CButton*)GetDlgItem(IDC_RADIO_IDIP_XL))->SetCheck(1);

						UpdateWindow();
						UpdateData(FALSE);
						OnCheckIdipXLReceiver(); //Berechne neuen Code, ist dann in m_sCode gespeichert
						
						sNew = sNew + sSemikolon + m_sCode + "\n";
						//TRACE(_T("new Line: %s\n"), sNew);
					}
				}

				newfile.WriteString(sNew);
				sLine.Empty();

				iCount++;
			}
			file.Close();
			newfile.Close();
		}
	}
	OnOK();
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::OnBnClickedRadioQCam8()
{
	CalcNewCodeDTSTransmitter();	
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::OnBnClickedRadioQCam16()
{
	CalcNewCodeDTSTransmitter();	
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::OnBnClickedRadioQcam4()
{
	CalcNewCodeDTSTransmitter();	
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::EnableDisableDlgItem(int nDlgID, BOOL bEnable)
{
	((CButton*)GetDlgItem(nDlgID))->EnableWindow(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::OnCbnSelchangeDongleComboNrQCameras()
{
	int iCurSel = ((CComboBox*)GetDlgItem(IDC_DONGLE_COMBO_NR_Q_CAMERAS))->GetCurSel();
	if(iCurSel >= 0 && iCurSel <= 5)
	{
		m_iNrQCameras = iCurSel;
	}
	CalcNewCodeIdipTransmitter();
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::OnCbnSelchangeDongleComboNrIpCameras()
{
	int iCurSel = ((CComboBox*)GetDlgItem(IDC_DONGLE_COMBO_NR_IP_CAMERAS))->GetCurSel();
	if(iCurSel >= 0 && iCurSel <= 5)
	{
		m_iNrIPCameras = iCurSel;
	}
	CalcNewCodeIdipTransmitter();
}
/////////////////////////////////////////////////////////////////////////////
void CExpansionCodeDlg::OnDongleDlgItemRange(UINT nID)
{
	if(nID == IDC_DONGLE_CHECK_SERIAL_ALARM)
	{
		BOOL bEnable = FALSE;
		if(((CButton*)GetDlgItem(IDC_DONGLE_CHECK_SERIAL_ALARM))->GetCheck())
		{
			bEnable = TRUE;
		}

		EnableDisableDlgItem(IDC_DONGLE_CHECK_GAA, bEnable);
		EnableDisableDlgItem(IDC_DONGLE_CHECK_ACCESS, bEnable);
		EnableDisableDlgItem(IDC_DONGLE_CHECK_SCANNER, bEnable);
		EnableDisableDlgItem(IDC_DONGLE_CHECK_PARK, bEnable);
	}

	//enable/disable Auswahlbox QUnit Kameras / IP Kameras
	BOOL bEnableIP = ((CButton*)GetDlgItem(IDC_DONGLE_CHECK_IP))->GetCheck();
	EnableDisableDlgItem(IDC_DONGLE_COMBO_NR_IP_CAMERAS, bEnableIP);
	if(!bEnableIP)
	{
		//vorherige Auswahl löschen
		m_iNrIPCameras = -1;
		((CComboBox*)GetDlgItem(IDC_DONGLE_COMBO_NR_IP_CAMERAS))->SetCurSel(m_iNrIPCameras);
	}

	BOOL bEnableQ = ((CButton*)GetDlgItem(IDC_DONGLE_CHECK_QUNIT))->GetCheck();
	EnableDisableDlgItem(IDC_DONGLE_COMBO_NR_Q_CAMERAS, bEnableQ);
	
	CalcNewCodeIdipTransmitter();	
}

