// QuadPage.cpp : implementation file
//

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "QuadPage.h"
#include "ComConfigurationDialog.h"
#include ".\quadpage.h"


static TCHAR BASED_CODE szQuad[] = _T("Quad");

#define DEVICE_IDENTIFIER _T("Quad Switch")

// CQuadPage dialog
///////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CQuadPage, CSVPage)
CQuadPage::CQuadPage(CSVView* pParent)	: CSVPage(CQuadPage::IDD)
{
	m_pParent = pParent;
	m_pOutputs = pParent->GetDocument()->GetOutputs();
	m_pComParameters = NULL;
	m_nInitToolTips = TOOLTIPS_SIMPLE;

	Create(IDD,m_pParent);
}
///////////////////////////////////////////////////////////////////////////////////
CQuadPage::~CQuadPage()
{
	WK_DELETE(m_pComParameters);
}
///////////////////////////////////////////////////////////////////////////////////
void CQuadPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TYP, m_ctrlType);
	DDX_Control(pDX, IDC_COMBO_COM, m_ctrlCOM);
	DDX_Control(pDX, IDC_COMBO_V1, m_ctrlVideo[0]);
	DDX_Control(pDX, IDC_COMBO_V2, m_ctrlVideo[1]);
	DDX_Control(pDX, IDC_COMBO_V3, m_ctrlVideo[2]);
	DDX_Control(pDX, IDC_COMBO_V4, m_ctrlVideo[3]);
	DDX_Control(pDX, IDC_BUTTON_COM_SETTINGS, m_btnCOMSettings);
	DDX_Control(pDX, IDC_COMBO_O1, m_ctrlOut[0]);
	DDX_Control(pDX, IDC_COMBO_O2, m_ctrlOut[1]);
	DDX_Control(pDX, IDC_COMBO_O3, m_ctrlOut[2]);
	DDX_Control(pDX, IDC_COMBO_O4, m_ctrlOut[3]);
}
///////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CQuadPage, CSVPage)
	ON_CBN_SELCHANGE(IDC_COMBO_TYP, OnCbnSelchangeComboTyp)
	ON_CBN_SELCHANGE(IDC_COMBO_COM, OnCbnSelchangeComboCom)
	ON_CBN_SELCHANGE(IDC_COMBO_V1, OnCbnSelchangeComboV1)
	ON_CBN_SELCHANGE(IDC_COMBO_V2, OnCbnSelchangeComboV2)
	ON_CBN_SELCHANGE(IDC_COMBO_V3, OnCbnSelchangeComboV3)
	ON_CBN_SELCHANGE(IDC_COMBO_V4, OnCbnSelchangeComboV4)
	ON_BN_CLICKED(IDC_BUTTON_COM_SETTINGS, OnBnClickedButtonComSettings)
	ON_CBN_SELCHANGE(IDC_COMBO_O1, OnCbnSelchangeComboOut)
	ON_CBN_SELCHANGE(IDC_COMBO_O2, OnCbnSelchangeComboOut)
	ON_CBN_SELCHANGE(IDC_COMBO_O3, OnCbnSelchangeComboOut)
	ON_CBN_SELCHANGE(IDC_COMBO_O4, OnCbnSelchangeComboOut)
END_MESSAGE_MAP()
///////////////////////////////////////////////////////////////////////////////////
BOOL CQuadPage::IsDataValid()
{
	int iType = m_ctrlType.GetCurSel();
	return iType!=-1;
}
///////////////////////////////////////////////////////////////////////////////////
void CQuadPage::SaveChanges()
{
	CWK_Profile& wkp = GetProfile();

	wkp.DeleteSection(szQuad);
	SetCommPortUsed(GetProfile(), REMOVE_DEVICE, DEVICE_IDENTIFIER);

	int iCom, iCurSel = m_ctrlType.GetCurSel();
	QuadControlType qct = (QuadControlType)m_ctrlType.GetItemData(iCurSel);
	if (qct>QCT_UNKNOWN)
	{
		wkp.WriteInt(szQuad,_T("Type"),qct);
		iCurSel = m_ctrlCOM.GetCurSel();
		iCom = m_ctrlCOM.GetItemData(iCurSel);
		wkp.WriteInt(szQuad,_T("COM"), iCom);
		SetCommPortUsed(GetProfile(), iCom, DEVICE_IDENTIFIER);
		for (int i=0;i<4;i++)
		{
			CString sKey;

			sKey.Format(_T("Card%d"),i);
			iCurSel = m_ctrlVideo[i].GetCurSel();
			wkp.WriteInt(szQuad,sKey,m_ctrlVideo[i].GetItemData(iCurSel));

			sKey.Format(_T("Out%d"),i);
			iCurSel = m_ctrlOut[i].GetCurSel();
			wkp.WriteInt(szQuad,sKey,m_ctrlOut[i].GetItemData(iCurSel));
		}
		if (m_pComParameters)
		{
			m_pComParameters->SaveToRegistry( wkp ) ;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////
void SetCurSelItemData(CComboBox& cb, DWORD dwItemData)
{
	for (int i=0;i<cb.GetCount();i++)
	{
		if (dwItemData == cb.GetItemData(i))
		{
			cb.SetCurSel(i);
			break;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////
void CQuadPage::CancelChanges()
{
	CWK_Profile& wkp = GetProfile();
	QuadControlType qct = (QuadControlType)wkp.GetInt(szQuad,_T("Type"),QCT_UNKNOWN);
	if (qct != QCT_UNKNOWN)
	{
		SetCurSelItemData(m_ctrlType,qct);
		int iCOM = wkp.GetInt(szQuad,_T("COM"),0);
		SetCurSelItemData(m_ctrlCOM,iCOM);
		if (iCOM>0)
		{
			if (m_pComParameters == NULL)
			{
				m_pComParameters = new CComParameters(szQuad);
			}
			m_pComParameters->LoadFromRegistry(wkp);
		}
		
		for (int i=0;i<4;i++)
		{
			CString sKey;

			sKey.Format(_T("Card%d"),i);
			int iCard = wkp.GetInt(szQuad,sKey,SECID_NO_ID);
			SetCurSelItemData(m_ctrlVideo[i],iCard);
			OnSelChangeCard(i,TRUE);

			sKey.Format(_T("Out%d"),i);
			int iOut = wkp.GetInt(szQuad,sKey,-1);
			SetCurSelItemData(m_ctrlOut[i],iOut);
		}
	}
}
///////////////////////////////////////////////////////////////////////////
BOOL CQuadPage::OnInitDialog()
{
	CSVPage::OnInitDialog();
	CString s;

	s.LoadString(IDS_NOT_SELECTED);
	int index = m_ctrlType.AddString(s);
	m_ctrlType.SetItemData(index,QCT_UNKNOWN);

	for (QuadControlType qct = QCT_SANTEC_SQV_80;;qct=(QuadControlType)(qct+1))
	{
		s = NameOfEnumQuadRealName(qct);
		if (!s.IsEmpty())
		{
			int index = m_ctrlType.AddString(s);
			m_ctrlType.SetItemData(index,qct);
		}
		else
		{
			break;
		}
	}
	CString sDevice;
	BOOL bCanBeUsed = TRUE;
	DWORD dwComMask = m_pParent->GetDocument()->GetCommPortInfo();
	DWORD dwBit;
	int i;
	for (dwBit = 1, i=1; dwBit && dwBit<=dwComMask; dwBit<<=1,i++)
	{
		if (dwComMask & dwBit)
		{
			sDevice = IsCommPortUsed(GetProfile(), i);
			if (!sDevice.IsEmpty() && sDevice != DEVICE_IDENTIFIER)
			{
				bCanBeUsed = FALSE;
			}
			if (bCanBeUsed)
			{
				CString s;
				s.Format(_T("COM%d"),i);
				int index = m_ctrlCOM.AddString(s);
				m_ctrlCOM.SetItemData(index,i);
			}
		}
	}

	for (int i=0;i<4;i++)
	{
		for (int j=0;j<m_pOutputs->GetSize();j++)
		{
			COutputGroup* pGroup = m_pOutputs->GetGroupAt(j);
			if (pGroup->HasCameras())
			{
				int index = m_ctrlVideo[i].AddString(pGroup->GetName());
				m_ctrlVideo[i].SetItemData(index,pGroup->GetID().GetID());
			}

		}
	}

	CancelChanges();
	Enable();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
//////////////////////////////////////////////////////////////////
void CQuadPage::Enable()
{
	int iCurSelType = m_ctrlType.GetCurSel();
	QuadControlType qct = (QuadControlType)m_ctrlType.GetItemData(iCurSelType);

	BOOL bEnable = qct>QCT_UNKNOWN;

	for (int i=0;i<4;i++)
	{
		m_ctrlVideo[i].EnableWindow(bEnable);
		int iCurSelVideo = m_ctrlVideo[i].GetCurSel();
		m_ctrlOut[i].EnableWindow(iCurSelVideo!=-1);
	}
	m_ctrlCOM.EnableWindow(bEnable);
	m_btnCOMSettings.EnableWindow(m_ctrlCOM.GetCurSel()!=-1);

}
//////////////////////////////////////////////////////////////////
void CQuadPage::OnCbnSelchangeComboTyp()
{
	int iCurSelType = m_ctrlType.GetCurSel();
	QuadControlType qct = (QuadControlType)m_ctrlType.GetItemData(iCurSelType);
	if (qct == QCT_UNKNOWN)
	{
		for (int i=0;i<4;i++)
		{
			m_ctrlVideo[i].SetCurSel(-1);
			m_ctrlOut[i].SetCurSel(-1);
		}
		m_ctrlCOM.SetCurSel(-1);
	}
	Enable();
	SetModified();
}
//////////////////////////////////////////////////////////////////
void CQuadPage::OnCbnSelchangeComboCom()
{
	Enable();
	SetModified();
}
//////////////////////////////////////////////////////////////////
void CQuadPage::OnCbnSelchangeComboV1()
{
	OnSelChangeCard(0,FALSE);
}
//////////////////////////////////////////////////////////////////
void CQuadPage::OnCbnSelchangeComboV2()
{
	OnSelChangeCard(1,FALSE);
}
//////////////////////////////////////////////////////////////////
void CQuadPage::OnCbnSelchangeComboV3()
{
	OnSelChangeCard(2,FALSE);
}
//////////////////////////////////////////////////////////////////
void CQuadPage::OnCbnSelchangeComboV4()
{
	OnSelChangeCard(3,FALSE);
}
//////////////////////////////////////////////////////////////////
void CQuadPage::OnSelChangeCard(int iCard,BOOL bFromLoad)
{
	if (iCard>=0&&iCard<4)
	{
		Enable();
		int iCurSelOut = m_ctrlOut[iCard].GetCurSel();
		if (!bFromLoad)
		{
			SetModified();
		}
		int iCurSel = m_ctrlVideo[iCard].GetCurSel();
		m_ctrlOut[iCard].ResetContent();
		if (iCurSel!=-1)
		{
			COutputGroup* pGroup = m_pOutputs->GetGroupByID(m_ctrlVideo[iCard].GetItemData(iCurSel));
			if (   pGroup
				&& pGroup->HasCameras())
			{
				int iCount = 2;
				if (-1!=pGroup->GetSMName().Find(_T("Tasha")))
				{
					iCount = 4;
				}
				InsertVideoOut(iCard,iCount);
			}
		}
		if (iCurSelOut!=-1)
		{
			m_ctrlOut[iCard].SetCurSel(iCurSelOut);
		}

	}
}
//////////////////////////////////////////////////////////////////
void CQuadPage::InsertVideoOut(int iCard, int iCount)
{
	m_ctrlOut[iCard].ResetContent();
	for (int i=0;i<iCount;i++)
	{
		CString s;
		s.Format(_T("VOUT %d"),i+1);
		int iIndex = m_ctrlOut[iCard].AddString(s);
		m_ctrlOut[iCard].SetItemData(iIndex,i);
	}

}
//////////////////////////////////////////////////////////////////
void CQuadPage::OnBnClickedButtonComSettings()
{
	int iCurSelCOM = m_ctrlCOM.GetCurSel();
	int iCurSelType = m_ctrlType.GetCurSel();
	if (   iCurSelCOM!=-1
		&& iCurSelType!=-1)
	{
		CString sCom;
		sCom.Format(_T("COM%d"),m_ctrlCOM.GetItemData(iCurSelCOM));
		BOOL bFirstTime = FALSE;
		if (NULL==m_pComParameters)
		{
			bFirstTime = TRUE;
			m_pComParameters = new CComParameters(szQuad);
			QuadControlType qct = (QuadControlType)m_ctrlType.GetItemData(iCurSelType);
			switch (qct)
			{
			case QCT_SANTEC_SQV_80:
				m_pComParameters->SetBaudRate(CBR_1200);
				m_pComParameters->SetDataBits(8);
				m_pComParameters->SetParity(NOPARITY);
				m_pComParameters->SetStopBits(ONESTOPBIT);
				break;
			}
		}

		CComConfigurationDialog dlg(sCom, m_pComParameters);
		if ( IDOK == dlg.DoModal() ) 
		{
			SetModified();
		}
		else 
		{
			if (bFirstTime) 
			{
				WK_DELETE(m_pComParameters);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////
void CQuadPage::OnCbnSelchangeComboOut()
{
	SetModified();
}
//////////////////////////////////////////////////////////////////
BOOL CQuadPage::GetToolTip(UINT nID, CString&sText)
{
	switch(nID)
	{
		case IDC_COMBO_V1: case IDC_COMBO_V2: case IDC_COMBO_V3: case IDC_COMBO_V4:
			sText.LoadString(IDC_COMBO_V1);
			break;
		case IDC_COMBO_O1: case IDC_COMBO_O2: case IDC_COMBO_O3: case IDC_COMBO_O4:
			sText.LoadString(IDC_COMBO_O1);
			break;
		case IDC_BUTTON_COM_SETTINGS:
			sText.LoadString(IDC_BTN_CAM_COM_PARAMS);
			break;
	}
	return !sText.IsEmpty();
}
