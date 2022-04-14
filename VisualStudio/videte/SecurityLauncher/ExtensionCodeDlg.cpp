// ExtensionCodeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SecurityLauncher.h"
#include "ExtensionCodeDlg.h"

// CExtensionCodeDlg dialog

IMPLEMENT_DYNAMIC(CExtensionCodeDlg, CSkinDialog)

CExtensionCodeDlg::CExtensionCodeDlg(CWnd* pParent /*=NULL*/)
	: CSkinDialog(CExtensionCodeDlg::IDD, pParent)
{
}

CExtensionCodeDlg::~CExtensionCodeDlg()
{
}
/////////////////////////////////////////////////////////////////////////////
void CExtensionCodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CExtensionCodeDlg, CSkinDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_EN_CHANGE(IDC_EDT_EXTENSION_CODE, OnEnChangeEdtExtensionCode)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CExtensionCodeDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();
	AutoCreateSkinEdit();
	AutoCreateSkinStatic();
	AutoCreateSkinButtons();

	CString s;
	s.LoadString(ID_EXTENSION_CODE);
	s.Replace(_T("..."), _T(""));
	SetWindowText(s);

	CWK_Profile prof(CWK_Profile::WKP_ABSOLUT,HKEY_LOCAL_MACHINE,_T(""), _T(""));
	s = prof.GetString(REGPATH_HKLM_COMPUTERNAME, REGKEY_COMPUTERNAME,_T(""));
	SetDlgItemText(IDC_EDT_SERIAL_NUMBER, s);

	GotoDlgCtrl(GetDlgItem(IDC_EDT_EXTENSION_CODE));

	return FALSE;  // return TRUE unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
// CExtensionCodeDlg message handlers
void CExtensionCodeDlg::OnBnClickedOk()
{
	if(AddNewDongleFeatures())
	{
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT);
	}
	OnOK();
}
/////////////////////////////////////////////////////////////////////////////
void CExtensionCodeDlg::OnBnClickedCancel()
{
	OnCancel();
}
/////////////////////////////////////////////////////////////////////////////
void CExtensionCodeDlg::OnEnChangeEdtExtensionCode()
{
	CString sExtension;
	GetDlgItemText(IDC_EDT_EXTENSION_CODE, sExtension);
	if(    !sExtension.IsEmpty()
		&& sExtension.GetLength() >= 12)
	{
		GetDlgItem(IDOK)->EnableWindow();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CExtensionCodeDlg::AddNewDongleFeatures()
{
	BOOL bRet = TRUE;
	DWORD dwSerial = 0;
	DWORD dwDongle = 0;
	CRSA rsa(0,0);
	CString sValue;
	CString sError(_T("new dongle features added to dongle successfully\n"));

	GetDlgItemText(IDC_EDT_EXTENSION_CODE, sValue);

	if (rsa.DecodeSerialDongle(sValue,dwSerial,dwDongle))
	{
		TRACE(_T("expansion code decode success: sValue: %s  dwSerial: %d  dwDongle: %08lx\n")
			,sValue,dwSerial,dwDongle);

		CWK_Profile prof(CWK_Profile::WKP_ABSOLUT,HKEY_LOCAL_MACHINE,_T(""), _T(""));
		CString sComputerName = prof.GetString(REGPATH_HKLM_COMPUTERNAME, REGKEY_COMPUTERNAME, _T("")), s;

		sComputerName.MakeLower();
		TRACE(_T("Orig ComputerName is %s\n"),sComputerName);
		for (int i=0;i<sComputerName.GetLength();i++)
		{
			if (   '0' <= sComputerName[i]
			&& sComputerName[i] <= '9')
			{
				s += sComputerName[i];
			}
		}
		if (s.IsEmpty())
		{
			for (int i=0;i<sComputerName.GetLength() && i<8;i++)
			{
				int iNum = sComputerName[i] % 10;
				s += (_TCHAR)('0'+iNum);
			}
		}
		s.TrimLeft();
		s.TrimRight();

		//shrink serial so that it fits to the serial number
		//the serial numbers last 2 digits are the checksum for the dongle informations
		s = s.Left(s.GetLength()-2);

		TRACE(_T("Clean Device Serial is <%s>\n"),s);
		CString sSerial;
		sSerial.Format(_T("%d"),dwSerial);
		sSerial.TrimLeft();
		sSerial.TrimRight();

		//extract the checksum for the dongle information from the serial number
		//the serial numbers last 2 digits are the checksum for the dongle informations
		DWORD dwDongleCheckSum = 0;
		_stscanf(sSerial.Right(2),_T("%d"),&dwDongleCheckSum);
		sSerial = sSerial.Left(sSerial.GetLength()-2);

		TRACE(_T("Clean Dongle Serial is <%s>\n"),sSerial);

		DWORD dw1,dw2;
		dw1 = dw2 = 0;
		_stscanf(sSerial,_T("%d"),&dw1);
		_stscanf(s,_T("%d"),&dw2);


		if (   dwSerial != 0
			&& !s.IsEmpty()
			&& (   s == sSerial 
			|| (   dw1 == dw2
			&& dw1!=0)

			)
			)
		{ 
			//test auf computername erfolgreich,
			WK_TRACE(_T("dongle matches computer name writing dongle values\n"));

			//check if dongle checksum fits the dongle
			if(CreateDongleChecksum(dwDongle) == dwDongleCheckSum)
			{
				CString sDongleDcf, sDongleCry, sTempDongleCry;
				CString sDir = theApp.GetWorkingDirectory();
				if(!sDir.IsEmpty())
				{
					sDongleCry.Format(_T("%s\\dongle.cry"), sDir);
					sDongleDcf.Format(_T("%s\\dongle.dcf"), theApp.GetTempDirectory());
					sTempDongleCry.Format(_T("%s\\tempdongle.cry"), theApp.GetTempDirectory());

					//for safety reason copy dongle.cry in case of something went wrong
					CopyFile(sDongleCry,sTempDongleCry,FALSE);
					
					//create dongle.dcf 
					if(WriteTempDCF(sDongleDcf))
					{
						CRSA rsa;
						if(rsa.IsValid())
						{
							SetNewDongleFeatures(dwDongle);

							//save new features incl old features to temp dongle.dcf
							if(WriteTempDCF(sDongleDcf))
							{
								//create new dongle.cry
								if (!rsa.Encode(sDongleDcf,sDongleCry))
								{
									bRet = FALSE;

									//recreate original dongle.cry because writing the new one failed
									CopyFile(sTempDongleCry,sDongleCry,FALSE);
									sError = _T("could not crypt new dongle\n");
								}
							}
							else
							{
								bRet = FALSE;
								sError = _T("could not write temporary dongle\n");
							}
						}
						else
						{
							bRet = FALSE;
							sError = _T("decoding (RSA) is not valid\n");
						}
					}
					else
					{
						bRet = FALSE;
						sError = _T("could not create temporary dongle\n");
					}
		
					//delete dongle.dcf
					DeleteFile(sDongleDcf);
					DeleteFile(sTempDongleCry);
				}
				else
				{
					bRet = FALSE;
					sError = _T("could not get working directory\n");
				}
			}
			else
			{
				bRet = FALSE;
				sError = _T("wrong expansion code was entered\n");
			}
		}
		else
		{
			bRet = FALSE;
			sError = _T("dongle doesn't match computer name\n");
		}
	}
	else
	{
		bRet = FALSE;
		sError = _T("cannot decode expansion code\n");
	}

	if(!bRet)
	{
		theApp.AddNewError(new CWK_RunTimeError(WAI_LAUNCHER,REL_ERROR,RTE_CONFIGURATION,sError));
		if (WK_IS_WINDOW(theApp.m_pMainWnd))
		{
			theApp.m_pMainWnd->PostMessage(WM_COMMAND, ID_ERROR_ARRIVED);
		}
		WK_TRACE_ERROR(_T("%s"), sError);
	}
	else
	{
		WK_TRACE(_T("%s"), sError);
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CExtensionCodeDlg::WriteTempDCF(const CString &sFilename) const
{
	BOOL bOkay=TRUE;

	CStdioFile f;
	bOkay = f.Open(sFilename,CFile::modeCreate| CFile::modeWrite);
	if (bOkay) {
		CString sValue;
		BOOL bValue;

		CTime ct=CTime::GetCurrentTime();
		sValue.Format(_T(";created %d.%d, %d:%d\n"),
			ct.GetDay(), ct.GetMonth(),
			ct.GetHour(), ct.GetMinute()
			);
		f.WriteString(sValue);

		// non-BOOL values first
		sValue.Format(_T("VersionNumber=%s\n"),GetVersionString());
		f.WriteString(sValue);

		sValue.Format(_T("Major=%d\n"), m_bMajorVersion );
		f.WriteString(sValue);

		sValue.Format(_T("Minor=%d\n"), m_bMinorVersion );
		f.WriteString(sValue);

		sValue.Format(_T("ProductCode=%d\n"),GetProductCode());
		f.WriteString(sValue);

		sValue.Format(_T("OEMCode=%d\n"),(WORD)GetOemCode());
		f.WriteString(sValue);

		// call the member function via x()
		// convert to string #x
#define TRANSFER(x) \
	bValue = x(); \
	sValue.Format(_T("%s=%d\n"), _T(#x), (int)bValue); \
	f.WriteString(sValue); \
	TRACE(_T("%s"),sValue);
		// CAVEAT all Transfers as ONE , so all will get the SAME
#include "AllDongleTransfers.h"

		sValue=_T("MagicNumber=234567\n");
		f.WriteString(sValue);
		sValue.Format(_T("NrOfHosts=%d\n"), m_iNrOfHosts);
		f.WriteString(sValue);
		sValue.Format(_T("NrOfCameras=%d\n"), m_iNrOfCameras);
		f.WriteString(sValue);
		sValue.Format(_T("NrOfIPCameras=%d\n"), m_iNrOfIPCameras);
		f.WriteString(sValue);
#undef TRANSFER

		f.Close();
	}
	return bOkay;
}
/////////////////////////////////////////////////////////////////////////////
void CExtensionCodeDlg::SetNewDongleFeatures(DWORD dwDongle)
{
	if(dwDongle & DONGLE_FLAG_BACKUP)
	{
		m_bRunCDRWriter = TRUE;
	}

	if(dwDongle & DONGLE_FLAG_Q)
	{
		int iNewNrOfQCams	= 4;
		int iNrOfQCams		= GetNrOfCameras();
		m_bRunQUnit			= TRUE;

		(dwDongle & DONGLE_FLAG_Q_CAM_4)	? iNewNrOfQCams = NR_OF_CAMERAS_4  : iNewNrOfQCams;
		(dwDongle & DONGLE_FLAG_Q_CAM_8)	? iNewNrOfQCams = NR_OF_CAMERAS_8  : iNewNrOfQCams;
		(dwDongle & DONGLE_FLAG_Q_CAM_16)	? iNewNrOfQCams = NR_OF_CAMERAS_16 : iNewNrOfQCams;
		(dwDongle & DONGLE_FLAG_Q_CAM_24)	? iNewNrOfQCams = NR_OF_CAMERAS_24 : iNewNrOfQCams;
		(dwDongle & DONGLE_FLAG_Q_CAM_32)	? iNewNrOfQCams = NR_OF_CAMERAS_32 : iNewNrOfQCams;

		if(iNewNrOfQCams > iNrOfQCams)
		{
			m_iNrOfCameras = iNewNrOfQCams;
		}
	}
	
	if(dwDongle & DONGLE_FLAG_IP)
	{
		int iNewNrOfIPCams	= 0;
		int iNrOfIPCams		= GetNrOfIPCameras();
		m_bRunIPCameraUnit	= TRUE;

		(dwDongle & DONGLE_FLAG_IP_CAM_4)	? iNewNrOfIPCams = NR_OF_CAMERAS_4  : iNewNrOfIPCams;
		(dwDongle & DONGLE_FLAG_IP_CAM_8)	? iNewNrOfIPCams = NR_OF_CAMERAS_8  : iNewNrOfIPCams;
		(dwDongle & DONGLE_FLAG_IP_CAM_16)	? iNewNrOfIPCams = NR_OF_CAMERAS_16 : iNewNrOfIPCams;
		(dwDongle & DONGLE_FLAG_IP_CAM_24)	? iNewNrOfIPCams = NR_OF_CAMERAS_24 : iNewNrOfIPCams;
		(dwDongle & DONGLE_FLAG_IP_CAM_32)	? iNewNrOfIPCams = NR_OF_CAMERAS_32 : iNewNrOfIPCams;

		if(iNewNrOfIPCams > iNrOfIPCams)
		{
			m_iNrOfIPCameras = iNewNrOfIPCams;
		}
	}

	if(dwDongle & DONGLE_FLAG_ISDN)
	{
		m_bRunISDNUnit = TRUE;
		m_bRunISDNUnit2 = TRUE;
		m_bRunSMSUnit = TRUE;
	}

	if(dwDongle & DONGLE_FLAG_PTZ)
	{
		m_bRunCommUnit = TRUE;
	}

	if(dwDongle & DONGLE_FLAG_SERIAL_ALARM)
	{
		m_bRunSDIUnit = TRUE;

		if(dwDongle & DONGLE_FLAG_GAA)
		{
			m_bAllowSDICashSystems = TRUE;
		}
		if(dwDongle & DONGLE_FLAG_ACCESS)
		{
			m_bAllowSDIAccessControl = TRUE;
		}
		if(dwDongle & DONGLE_FLAG_SCANNER)
		{
			m_bAllowSDIIndustrialEquipment = TRUE;
		}
		if(dwDongle & DONGLE_FLAG_PARK)
		{
			m_bAllowSDIParkmanagment = TRUE;
		}
	}

	if(dwDongle & DONGLE_FLAG_ICPCON)
	{
		m_bRunICPCONUnit = TRUE;
	}

	if(dwDongle & DONGLE_FLAG_HTTP)
	{
		m_bRunHTTP = TRUE;
	}
}
/////////////////////////////////////////////////////////////////////////////
DWORD CExtensionCodeDlg::CreateDongleChecksum(DWORD dwDongle)
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