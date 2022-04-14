// EULALicenseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "eula.h"
#include "EULALicenseDlg.h"
#include ".\eulalicensedlg.h"
#include "cipc.h"


#define FLAG_TIMER		0x00000010
#define FLAG_PTZ		0x00000020
#define FLAG_MONITOR	0x00000040
#define FLAG_MAPS		0x00000080
#define FLAG_HOST0		0x00000400
#define FLAG_HOST5		0x00000100
#define FLAG_HOST10		0x00000200
#define FLAG_HOST20		0x00000800
#define FLAG_HOST30		0x00001000
#define FLAG_HOST40		0x00002000
#define FLAG_HOST50		0x00004000
#define FLAG_HOST60		0x00008000
#define FLAG_RESET		0x00010000

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEULALicenseDlg dialog
CEULALicenseDlg::CEULALicenseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEULALicenseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEULALicenseDlg)
	m_ValueLicense = _T("");
	m_ValueExpansion = (_T(""));
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bDTS = theApp.IsDTS();
}
/////////////////////////////////////////////////////////////////////////////
void CEULALicenseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEULALicenseDlg)
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_EDIT_LICENSE, m_ctrlEditLicense);
	DDX_Control(pDX, IDC_EDIT_EXPANSION, m_ctrlEditExpansion);
	DDX_Text(pDX, IDC_EDIT_LICENSE, m_ValueLicense);
	DDX_Text(pDX, IDC_EDIT_EXPANSION, m_ValueExpansion);
	//}}AFX_DATA_MAP

	DDX_Control(pDX, IDC_TEXT_TOP_EXPANSION, m_ctrlStaticExpansion);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CEULALicenseDlg, CDialog)
	//{{AFX_MSG_MAP(CEULALicenseDlg)
	ON_EN_CHANGE(IDC_EDIT_LICENSE, OnChangeEditLicense)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER,OnUser)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CEULALicenseDlg::OnOK() 
{
	UpdateData();

	//check serial number
	if((m_ValueLicense.GetLength() == 9) && IsSerialCorrect(m_ValueLicense))
	{
		//check expansion code
		DWORD dwDongleDecode = 0;
		m_dwDongleDecode = 0;
		
		if(m_bDTS)
		{
			SaveSerialToReg(m_ValueLicense); //Speicher die korrekte Lizenznummer
			CString sError;
			sError.Format(_T("DTS serial number ok\n"));
//			AfxMessageBox(sError);
		}
		else //nur bei IdipReceiver
		{
			SaveSerialToReg(m_ValueLicense); //Speicher die korrekte Lizenznummer

			if(IsExpansionCorrect(dwDongleDecode, m_ValueLicense, m_ValueExpansion)) 
			{
				//Freischaltcode muss korrekt zur Lizenz passen
				m_dwDongleDecode = dwDongleDecode;
				SaveExpansionToReg(m_ValueExpansion);

				//set expansions to dongle
				CryptDongle();
			}
			else
			{
				//Freischaltcode falsch, schalte also nur die Grundfreischaltung der PL
				m_dwDongleDecode = 0;
				m_dwDongleDecode |= FLAG_RESET;
				m_ValueExpansion.Empty();
				SaveExpansionToReg(m_ValueExpansion);
				CString sError;
				sError.Format(_T("expansion code incorrect, installation canceled.\n"));
				AfxMessageBox(sError);
			}
		}
	}
	else
	{
		m_ValueExpansion.Empty();
		m_ValueLicense.Empty();
		SaveSerialToReg(m_ValueLicense);//Speicher Leerstring als Lizenznummer
										//Leerstring ist Indikator für Wise-Installationsabbruch
		SaveExpansionToReg(m_ValueExpansion);
		CString sError;
		sError.Format(_T("serial number incorrect, installation canceled.\n"));
		AfxMessageBox(sError);
	}
	CDialog::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CEULALicenseDlg::SaveSerialToReg(CString sSerial) 
{
	BOOL bRet = FALSE;
	DWORD dwResult = 0;
	HKEY hNewKey;
	DWORD dwKeyCreated = 0;
	
	dwResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
							  SERIAL,
							  NULL,
							  NULL,
							  REG_OPTION_NON_VOLATILE,
							  KEY_ALL_ACCESS,
							  NULL,
							  &hNewKey,
							  &dwKeyCreated);
	if (dwResult == ERROR_SUCCESS)
	{
//		DWORD dwSize = sSerial.GetLength();
		dwResult = RegSetValueEx(hNewKey,
								 _T("Serial"),
								 NULL,
								 REG_SZ,
								 (LPBYTE)(LPCTSTR)sSerial,
								 (lstrlen(sSerial)+1)*sizeof(TCHAR));

		if(dwResult == ERROR_SUCCESS)
		{
			bRet = TRUE;
		}
	}
	RegCloseKey(hNewKey);
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CEULALicenseDlg::SaveExpansionToReg(CString sCode) 
{
	BOOL bRet = FALSE;
	DWORD dwResult = 0;
	HKEY hNewKey;
	DWORD dwKeyCreated = 0;

	dwResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
		SERIAL,
		NULL,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hNewKey,
		&dwKeyCreated);
	if (dwResult == ERROR_SUCCESS)
	{
		//		DWORD dwSize = sSerial.GetLength();
		dwResult = RegSetValueEx(hNewKey,
			_T("Code"),
			NULL,
			REG_SZ,
			(LPBYTE)(LPCTSTR)sCode,
			(lstrlen(sCode)+1)*sizeof(TCHAR));

		if(dwResult == ERROR_SUCCESS)
		{
			bRet = TRUE;
		}
	}
	RegCloseKey(hNewKey);
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CEULALicenseDlg::IsSerialCorrect(CString sSerial)
{
	BOOL bRet = FALSE;
	CString s(_T("000000000"));
	CString sInt, sCheck;
	int i1, i2, i3, i4, i5, i6, iPrim, iDummy, iCheck;
	i1=i2=i3=i4=i5=i6=iPrim=iDummy=iCheck=0;

	s.SetAt(0, sSerial.GetAt(0));
	s.SetAt(1, sSerial.GetAt(5));
	s.SetAt(2, sSerial.GetAt(3));
	s.SetAt(3, sSerial.GetAt(2));
	s.SetAt(4, sSerial.GetAt(1));
	s.SetAt(5, sSerial.GetAt(6));
	s.SetAt(6, sSerial.GetAt(4));
	s.SetAt(7, sSerial.GetAt(7));
	s.SetAt(8, sSerial.GetAt(8));

	sCheck = s.Right(2);
	sInt = s.Mid(1, 6);
	iPrim = _ttoi(sInt);

	i1 = iPrim/100000;
	iDummy = iPrim-i1*100000;
	i2 = iDummy/10000;
	iDummy = iPrim-i1*100000-i2*10000;
	i3 = iDummy/1000;
	iDummy = iPrim-i1*100000-i2*10000-i3*1000;
	i4 = iDummy/100;
	iDummy = iPrim-i1*100000-i2*10000-i3*1000-i4*100;
	i5 = iDummy/10;
	iDummy = iPrim-i1*100000-i2*10000-i3*1000-i4*100-i5*10;
	i6 = iDummy;
	iCheck = 2*i1 + 3*i2 + 4*i3 + 4*i4 + 3*i5 + 2*i6;
	iCheck %= 100;

	if(iCheck == _ttoi(sCheck) && RealIsPrim(iPrim) )
	{
		TRACE(_T("Serial korrekt: %s\n"), s);
		m_ValueLicense = sSerial;
		bRet = TRUE;
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
__int64 CEULALicenseDlg::even(__int64 n)
{
	return (n & 1) != 1;
}
//////////////////////////////////////////////////////////////////////
__int64 CEULALicenseDlg::RealIsPrim (__int64 p)
{
	__int64 i;
	
	i = 3;
	
	if (p==2)
	{
		return 1;
	}

	if (even(p))
	{
		return 0;
	}
	
	if (p<=1)
	{
		return 0;
	}
	
	while (i*i<=p)
	{
		if (p%i==0)
		{
			return 0;
		}
		i+=2;
	}
	return 1;
} 
/////////////////////////////////////////////////////////////////////////////
BOOL CEULALicenseDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CenterWindow(GetDesktopWindow());
	m_ctrlEditLicense.LimitText(9);

	if(m_bDTS)
	{
		//hide all expansion code fields
		m_ctrlEditExpansion.ShowWindow(SW_HIDE);
		m_ctrlStaticExpansion.ShowWindow(SW_HIDE);
		PostMessage(WM_USER,1,IDC_EDIT_LICENSE);
	}
	else
	{
		CString sSerial = GetSerialFromReg();
		if(sSerial.IsEmpty())
		{
			PostMessage(WM_USER,1,IDC_EDIT_LICENSE);
		}
		else
		{
			PostMessage(WM_USER,1,IDC_EDIT_EXPANSION);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CEULALicenseDlg::OnChangeEditLicense() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	UpdateData(TRUE);
	if((UINT)m_ValueLicense.GetLength() == m_ctrlEditLicense.GetLimitText())
	{
		m_ctrlEditExpansion.SetFocus();
		m_ctrlEditExpansion.SetSel(0, -1, FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CEULALicenseDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

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
CString CEULALicenseDlg::GetSerialFromReg() 
{
	CString sReturn = _T("");
	DWORD dwResult = 0;
	HKEY hNewKey;
//	DWORD dwKeyCreated = 0;
	CString sSubKey(_T("SOFTWARE\\dvrt\\Version"));

	dwResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				      sSubKey,
					  0,
					  KEY_ALL_ACCESS,
					  &hNewKey);

	if(dwResult == ERROR_SUCCESS)
	{
		DWORD dwType = 0;
		DWORD dwLen  = 0;
		CString sValue;

		LONG lResult = RegQueryValueEx(hNewKey,
									   _T("Serial"),
									   NULL, 
									   &dwType, 
									   NULL, 
									   &dwLen);
		if(lResult == ERROR_SUCCESS)
		{
			lResult = RegQueryValueEx(hNewKey, 
										   _T("Serial"),
										   NULL,
										   &dwType,
										   (LPBYTE)sValue.GetBuffer(dwLen),
										   &dwLen);	
			sValue.ReleaseBuffer();
			if (lResult == ERROR_SUCCESS)
			{
				sReturn = sValue;
			}
		}
	}
	RegCloseKey(hNewKey);
	return sReturn;
}
/////////////////////////////////////////////////////////////////////////////
CString CEULALicenseDlg::GetExpansionFromReg() 
{
	CString sReturn = _T("");
	DWORD dwResult = 0;
	HKEY hNewKey;
	//	DWORD dwKeyCreated = 0;
	CString sSubKey(_T("SOFTWARE\\dvrt\\Version"));

	dwResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		sSubKey,
		0,
		KEY_ALL_ACCESS,
		&hNewKey);

	if(dwResult == ERROR_SUCCESS)
	{
		DWORD dwType = 0;
		DWORD dwLen  = 0;
		CString sValue;

		LONG lResult = RegQueryValueEx(hNewKey,
			_T("Code"),
			NULL, 
			&dwType, 
			NULL, 
			&dwLen);
		if(lResult == ERROR_SUCCESS)
		{
			lResult = RegQueryValueEx(hNewKey, 
				_T("Code"),
				NULL,
				&dwType,
				(LPBYTE)sValue.GetBuffer(dwLen),
				&dwLen);	
			sValue.ReleaseBuffer();
			if (lResult == ERROR_SUCCESS)
			{
				sReturn = sValue;
			}
		}
	}

	RegCloseKey(hNewKey);
	return sReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CEULALicenseDlg::IsExpansionCorrect(DWORD& dwDongleDecode, CString sLicense, const CString sExpansion)
{
	BOOL bRet = FALSE;
	DWORD dwExpansionDecode = 0;
	CRSA decoder(0,0);

	if(sExpansion.IsEmpty())
	{
		return bRet;
	}

	if(decoder.DecodeSerialDongle(sExpansion,dwExpansionDecode,dwDongleDecode))
	{
		//expansion code is only the first 7 digits from whole serial (which is 9 digits incl. checksum)
		//9 digits could be too long for encode (e.g. 711111221 is too big)
		DWORD dwValueLicence = 0;
		_stscanf(sLicense,_T("%d"),&dwValueLicence);
		if(dwExpansionDecode == dwValueLicence/100)
		{
			bRet = TRUE;
		}
		TRACE(_T("dongle decoded %d,%08lx\n"),dwExpansionDecode,dwDongleDecode);
	}
	else
	{
		TRACE(_T("dongle NOT decoded %d,%08lx\n"),dwExpansionDecode,dwDongleDecode);
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CEULALicenseDlg::CryptDongle()
{
	CString sDongleDcf;
	
	LoadDongleDCF(sDongleDcf);
	if (DoesFileExist(sDongleDcf))
	{
		CEulaDongle dongle(sDongleDcf);
		dongle.WriteDCF(sDongleDcf, m_dwDongleDecode);
	}
	else
	{
		CString sError;
		sError.Format(_T("%s file not exists\n"),sDongleDcf);
//		AfxMessageBox(sError);
	}

#ifdef _DEBUG
	CRSA rsa;
	CString sDongleCry = _T("c:\\security\\dongle.cry");

	//Encode macht das Wise Script, aber nur in Release beim Setup des Receivers
	if (rsa.IsValid())
	{
		rsa.Encode(sDongleDcf,sDongleCry);
	}
#endif
}
/////////////////////////////////////////////////////////////
LRESULT CEULALicenseDlg::OnUser(WPARAM wParam, LPARAM lParam)
{
	if(lParam == IDC_EDIT_EXPANSION)
	{
		CString sSerial = GetSerialFromReg();
		SetDlgItemText(IDC_EDIT_LICENSE,sSerial);
		GotoDlgCtrl(GetDlgItem(IDC_EDIT_EXPANSION));
	}

	if(lParam == IDC_EDIT_LICENSE)
	{
		GotoDlgCtrl(GetDlgItem(IDC_EDIT_LICENSE));
		m_ctrlEditLicense.SetSel(-1, -1, FALSE);
	}
	return 0L;
}
/////////////////////////////////////////////////////////////
void CEULALicenseDlg::LoadDongleDCF(CString &sDcf)
{
	//dongle.dcf wird vom Wise-Script ins temporäre Verzeichnis kopiert
	CString sTempDirectory;
	CString sDongleDcf = _T("dongle.dcf");
	GetTempPath(_MAX_PATH,sTempDirectory.GetBufferSetLength(_MAX_PATH));
	sDcf.Format(_T("%s%s"),sTempDirectory, sDongleDcf);
	sTempDirectory.ReleaseBuffer();
#ifdef _DEBUG
	sDcf = _T("c:\\security\\dongle.dcf");
#endif
}
/////////////////////////////////////////////////////////////////////////////
BOOL CEULALicenseDlg::DoesFileExist(LPCTSTR szFileOrDirectory)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE h = FindFirstFile(szFileOrDirectory, &FindFileData);
	if (h == INVALID_HANDLE_VALUE){
		return FALSE;
	}
	FindClose(h);
	return TRUE;
}
