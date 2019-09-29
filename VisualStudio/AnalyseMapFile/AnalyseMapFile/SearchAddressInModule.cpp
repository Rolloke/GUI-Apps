// SearchAddressInModule.cpp : implementation file
//

#include "stdafx.h"
#include "AnalyseMapFile.h"
#include "SearchAddressInModule.h"
#include ".\searchaddressinmodule.h"


// CSearchAddressInModule dialog

IMPLEMENT_DYNAMIC(CSearchAddressInModule, CDialog)
CSearchAddressInModule::CSearchAddressInModule(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchAddressInModule::IDD, pParent)
	, m_sModuleName(_T(""))
	, m_sAddress(_T(""))
	, m_sBaseAddress(_T(""))
	, m_sModuleSize(_T(""))
	, m_sOffset(_T(""))
	, m_sFunctionName(_T(""))
	, m_bShowDecimal(FALSE)
	, m_sSymbolSize(_T(""))
{
}

CSearchAddressInModule::~CSearchAddressInModule()
{
}

void CSearchAddressInModule::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MODULE_NAME, m_sModuleName);
	DDX_Text(pDX, IDC_EDT_BASE_ADDRESS, m_sBaseAddress);
	DDX_Text(pDX, IDC_EDT_MODULE_SIZE, m_sModuleSize);
	DDX_Text(pDX, IDC_EDT_OFFSET, m_sOffset);
	DDX_Text(pDX, IDC_EDT_FUNCTION_NAME, m_sFunctionName);
	DDX_Check(pDX, IDC_CK_SHOW_DECIMAL, m_bShowDecimal);
	DDX_Text(pDX, IDC_EDT_SYMBOL_SIZE, m_sSymbolSize);
	DDX_Text(pDX, IDC_EDT_ADDRESS, m_sAddress);

	if (pDX->m_bSaveAndValidate == TRUE)
	{
		if (!m_sAddress.IsEmpty() || !m_sOffset.IsEmpty())
		{
			m_sFunctionName.Empty();
		}
		else if (m_sAddress.IsEmpty() && m_sOffset.IsEmpty() && m_sFunctionName.IsEmpty())
		{
			AfxMessageBox(IDS_SEARCH_STRING_EMPTY, MB_ICONINFORMATION);
			pDX->Fail();
		}
	}
}


BEGIN_MESSAGE_MAP(CSearchAddressInModule, CDialog)
	ON_BN_CLICKED(IDC_BTN_BROWSE_MODULE_NAME, OnBnClickedBtnBrowseModuleName)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CLEAR_PARAMS, OnBnClickedClearParams)
	ON_BN_CLICKED(IDC_CK_SHOW_DECIMAL, OnBnClickedCkShowDecimal)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ADDRESS, OnDeltaposSpinAddress)
	ON_BN_CLICKED(IDC_CALC_ADDRESS, OnBnClickedCalcAddress)
END_MESSAGE_MAP()


// CSearchAddressInModule message handlers

void CSearchAddressInModule::OnBnClickedBtnBrowseModuleName()
{
   CFileDialog dialog(TRUE, _T("*"), m_sModuleName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Datei (*.*)|*.*||"), this);
   if (dialog.DoModal() == IDOK)
   {
		CDataExchange dx(this, FALSE);
		m_sModuleName = dialog.GetPathName();
		FindSymbols fs;
		fs.SetModule(m_sModuleName);
		theApp.FindModule(fs);
		if (fs.dwBaseAddress == 0)
		{
			theApp.AddModule(fs);
		}
		if (fs.dwBaseAddress)
		{
			m_sBaseAddress = theApp.DWORD64ToString(fs.dwBaseAddress);
			m_sModuleSize = theApp.DWORD64ToString(fs.dwModuleSize);
		}
		DDX_Text(&dx, IDC_EDIT_MODULE_NAME, m_sModuleName);
		DDX_Text(&dx, IDC_EDT_BASE_ADDRESS, m_sBaseAddress);
	   	DDX_Text(&dx, IDC_EDT_MODULE_SIZE, m_sModuleSize);
	}
}

void CSearchAddressInModule::OnBnClickedOk()
{
	if (UpdateData())
	{
		FindSymbols fs;
		fs.SetModule(m_sModuleName);
		ReadInputData(fs);
		if (   fs.sModule.IsEmpty()
			&& fs.dwAddress == 0 
			&& fs.dwOffset == 0)
		{
			fs.dwAddress = fs.dwBaseAddress;
		}
		if (theApp.FindSymbolInModule(fs))
		{
			m_sModuleName = fs.sModule;
			m_sFunctionName =  fs.sName;
			FormatOutputData(fs);
		}
		else
		{
			AfxMessageBox(IDS_NO_SYMBOL_FOUND, MB_ICONINFORMATION);
			if (!fs.sModule.IsEmpty())
			{
				m_sModuleName = fs.sModule;
				FormatOutputData(fs);
			}
		}
	}
}

void CSearchAddressInModule::OnBnClickedClearParams()
{
	m_sModuleName.Empty();
	m_sAddress.Empty();
	m_sBaseAddress.Empty();
	m_sModuleSize.Empty();
	m_sOffset.Empty();
	m_sFunctionName.Empty();
	m_sSymbolSize.Empty();
	UpdateData(FALSE);
}

void CSearchAddressInModule::OnBnClickedCkShowDecimal()
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_SHOW_DECIMAL, m_bShowDecimal);
	FindSymbols fs;
	ReadInputData(fs);
	FormatOutputData(fs);
}

void CSearchAddressInModule::ReadInputData(FindSymbols&fs)
{
	fs.dwBaseAddress = theApp.StringToDWORD64(m_sBaseAddress);
	fs.dwAddress     = theApp.StringToDWORD64(m_sAddress);
	fs.dwOffset      = (DWORD)theApp.StringToDWORD64(m_sOffset);
	fs.dwModuleSize  = (DWORD)theApp.StringToDWORD64(m_sModuleSize);
	fs.dwSize        = (DWORD)theApp.StringToDWORD64(m_sSymbolSize);
}

void CSearchAddressInModule::FormatOutputData(FindSymbols& fs)
{
	if (fs.dwBaseAddress)
	{
		m_sBaseAddress = theApp.DWORD64ToString(fs.dwBaseAddress, m_bShowDecimal ? FORCE_DECIMAL:0);
	}
	else
	{
		m_sBaseAddress.Empty();
	}
	if (fs.dwModuleSize)
	{
		m_sModuleSize  = theApp.DWORD64ToString(fs.dwModuleSize, m_bShowDecimal ? FORCE_DECIMAL:0);
	}
	else
	{
		m_sModuleSize.Empty();
	}
	if (fs.dwOffset)
	{
		m_sOffset      = theApp.DWORD64ToString(fs.dwOffset, m_bShowDecimal ? FORCE_DECIMAL:0);
	}
	else
	{
		m_sOffset.Empty();
	}
	if (fs.dwAddress)
	{
		m_sAddress     = theApp.DWORD64ToString(fs.dwAddress, m_bShowDecimal ? FORCE_DECIMAL:0);
	}
	else
	{
		m_sAddress.Empty();
	}
	if (fs.dwSize)
	{
		m_sSymbolSize  = theApp.DWORD64ToString(fs.dwSize, m_bShowDecimal ? FORCE_DECIMAL:0);
	}
	else
	{
		m_sSymbolSize.Empty();
	}
	UpdateData(FALSE);
}
void CSearchAddressInModule::OnDeltaposSpinAddress(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	FindSymbols fs;
	fs.SetModule(m_sModuleName);
	ReadInputData(fs);
	if (   fs.sModule.IsEmpty()
		&& fs.dwAddress == 0 
		&& fs.dwOffset == 0)
	{
		fs.dwAddress = fs.dwBaseAddress;
		if (fs.dwBaseAddress == 0)
		{
			fs.dwOffset = 0xffffffff;
		}
		::SendMessage(pNMHDR->hwndFrom, UDM_SETPOS, 0, 0);
		::SendMessage(pNMHDR->hwndFrom, UDM_SETRANGE32, 0, MAXWORD);
	}
	else
	{
		DWORD *pdwAddress = (DWORD*)&fs.dwAddress;
		fs.dwOffset = 0xffffffff;
		pdwAddress[0] = 0;
		pdwAddress[1] = pNMUpDown->iPos;
	}

	if (theApp.FindSymbolInModule(fs))
	{
		m_sModuleName = fs.sModule;
		m_sFunctionName =  fs.sName;
		FormatOutputData(fs);
	}

	*pResult = 0;
}

void CSearchAddressInModule::OnBnClickedCalcAddress()
{
	if (UpdateData(2))
	{
		FindSymbols fs;
		ReadInputData(fs);
		if (fs.dwBaseAddress)
		{
			bool bOffset = fs.dwOffset != 0 ? true : false;
			bool bAddress = fs.dwAddress != 0 ? true : false;

			if (bOffset && !bAddress)
			{
				fs.dwAddress = fs.dwBaseAddress + fs.dwOffset;
				fs.dwOffset = 0;
			}
			else if (	!bOffset && bAddress 
				     && fs.dwAddress > fs.dwBaseAddress)
			{
				fs.dwOffset = (DWORD)(fs.dwAddress - fs.dwBaseAddress);
				fs.dwAddress = 0;
			}
			else if (bOffset && bAddress)
			{
				if (fs.dwAddress > fs.dwOffset)
				{
					fs.dwOffset = (DWORD)(fs.dwAddress - fs.dwOffset);
				}
				else
				{
					fs.dwOffset = (DWORD)(fs.dwOffset - fs.dwAddress);
				}
				fs.dwAddress = fs.dwBaseAddress + fs.dwOffset;
				fs.dwOffset = 0;
			}
			else
			{
				AfxMessageBox(IDS_CALCULATE_PARAMS, MB_ICONINFORMATION);
				return;
			}

			FormatOutputData(fs);
		}
		else
		{
			AfxMessageBox(IDS_NO_MODULE, MB_ICONINFORMATION);
		}
	}
}
