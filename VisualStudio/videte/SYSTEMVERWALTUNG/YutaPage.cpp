// YutaPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "YutaPage.h"

#include "OutputList.h"
#include "OutputGroup.h"
#include "InputList.h"
#include "InputGroup.h"
#include "SVDoc.h"
#include "SVView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CYutaPage dialog
IMPLEMENT_DYNAMIC(CYutaPage, CSVPage)


#define BUTTON_IS_NOT_INITIALISED 0x00001000

CYutaPage::CYutaPage(CSVView* pParent /*=NULL*/)
	: CSVPage(CYutaPage::IDD)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CYutaPage)
	//}}AFX_DATA_INIT
	m_bYutaUnit[0] = FALSE;
	int i;
	for (i=1; i<NO_OF_YUTA_UNITS; i++)
	{
		m_bYutaUnit[i] = BUTTON_IS_NOT_INITIALISED;
		m_bDeleteActivations[i] = FALSE;
	}
	Create(IDD,(CWnd*)m_pParent);
}


void CYutaPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CYutaPage)
	DDX_Control(pDX, IDC_CK_YUTA_UNIT, m_cUtaUnitOne);
	//}}AFX_DATA_MAP
	int i;
	for (i=0; i<NO_OF_YUTA_UNITS; i++)
	{
		if (m_bYutaUnit[i] == BUTTON_IS_NOT_INITIALISED) break;
		DDX_Check(pDX, IDC_CK_YUTA_UNIT+i, m_bYutaUnit[i]);
	}
}


BEGIN_MESSAGE_MAP(CYutaPage, CDialog)
	//{{AFX_MSG_MAP(CYutaPage)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CK_YUTA_UNIT, IDC_CK_YUTA_UNIT+NO_OF_YUTA_UNITS, OnCkYutaUnit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CYutaPage message handlers
void CYutaPage::SaveChanges()
{
	CWK_Profile wkpBase;//(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sBaseDVRT, _T(""));
	if (UpdateData(TRUE))
	{
		int   i, nCount = 0;
		DWORD m_dwChanged = 0;
		CString sSMInputName;
		CString sSMOutputName;
		CString sModuleName;
		for (i=0; i<NO_OF_YUTA_UNITS; i++)
		{
			sSMInputName.Format(SM_YUTA_INPUT, i+1);
			sSMOutputName.Format(SM_YUTA_OUTPUT, i+1);
			if (m_bYutaUnit[i])
			{
				nCount++;
				if (m_pInputs->GetGroupBySMName(sSMInputName) == NULL)
				{
					m_pInputs->AddInputGroup(NULL, 16, sSMInputName);
					m_dwChanged |= 1<<IMAGE_INPUT;
				}
				if (m_pOutputs->GetGroupBySMName(sSMOutputName) == NULL)
				{
					m_pOutputs->AddOutputGroup(NULL, 8, sSMOutputName);
					m_dwChanged |= 1<<IMAGE_RELAY;
				}
			}
			else
			{
				if (m_bDeleteActivations[i])
				{
					CheckActivations(i, true);
				}
				if (m_pInputs->GetGroupBySMName(sSMInputName))
				{
					m_dwChanged |= 1<<IMAGE_INPUT;
					m_pInputs->DeleteGroup(sSMInputName);
				}
				if (m_pOutputs->GetGroupBySMName(sSMOutputName))
				{
					m_pOutputs->DeleteGroup(sSMOutputName);
					m_dwChanged |= 1<<IMAGE_RELAY;
				}
			}
		}
		m_pInputs->Save(wkpBase, FALSE);
		m_pOutputs->Save(wkpBase, FALSE);

		m_pParent->PostMessage(WM_USER, USER_MSG_UPDATE_PARENT_TREE, m_dwChanged);
		sSMInputName.Format(_T("%s\\Modules\\"), WK_APP_NAME_YUTA_UNIT);
		sModuleName.Format(_T("%s.exe"), WK_APP_NAME_YUTA_UNIT);
		wkpBase.WriteString(theApp.GetModuleSection(),WK_APP_NAME_YUTA_UNIT, (nCount > 0) ? sModuleName : _T(""));
	}
}

void CYutaPage::CancelChanges()
{
	if (IsModified())
	{	
		int i;
		CString strText;
		for (i=0; i<NO_OF_YUTA_UNITS; i++)
		{
			strText.Format(SM_YUTA_INPUT, i+1);
			CInputGroup * pInputGroup = m_pInputs->GetGroupBySMName(strText);
			m_bYutaUnit[i] = pInputGroup ? TRUE : FALSE;
			m_bDeleteActivations[i] = FALSE;
		}
		SetModified(FALSE, FALSE);
		UpdateData(FALSE);
	}
}

void CYutaPage::Initialize()
{
	
}

BOOL CYutaPage::IsDataValid()
{
	return TRUE;
}

BOOL CYutaPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	CRect rc;
	CString strFormat, strText;
	CFont *pFont;
	int i, nVdist;
	DWORD dwStyle;

	dwStyle = m_cUtaUnitOne.GetButtonStyle();
	dwStyle |= WS_VISIBLE|WS_CHILD;
	m_cUtaUnitOne.GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_cUtaUnitOne.GetWindowText(strFormat);
	strText.Format(strFormat, 1);
	m_cUtaUnitOne.SetWindowText(strText);
	pFont = m_cUtaUnitOne.GetFont();
	nVdist = rc.Height() * 2;
	
	for (i=1; i<NO_OF_YUTA_UNITS; i++)
	{
		strText.Format(strFormat, i+1);
		CButton *pButton = new CButton;
		rc.OffsetRect(0, nVdist);
		pButton->Create(strText, dwStyle, rc, this, IDC_CK_YUTA_UNIT+i);
		pButton->SetFont(pFont);
		m_bYutaUnit[i] = FALSE;
	}
	
	m_pInputs  = m_pParent->GetDocument()->GetInputs();
	m_pOutputs = m_pParent->GetDocument()->GetOutputs();

	for (i=0; i<NO_OF_YUTA_UNITS; i++)
	{
		strText.Format(SM_YUTA_INPUT, i+1);
		CInputGroup * pInputGroup = m_pInputs->GetGroupBySMName(strText);
		m_bYutaUnit[i] = pInputGroup ? TRUE : FALSE;
	}

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CYutaPage::OnDestroy() 
{
	int i;
	for (i=1; i<NO_OF_YUTA_UNITS; i++)
	{
		CButton*pBtn = (CButton*)GetDlgItem(IDC_CK_YUTA_UNIT+i);
		WK_DELETE(pBtn);
	}
	CDialog::OnDestroy();
}

void CYutaPage::OnCkYutaUnit(UINT nID) 
{
	CDataExchange dx(this, TRUE);
	int nIndex = nID - IDC_CK_YUTA_UNIT;
	if (IsBetween(nIndex, 0, NO_OF_YUTA_UNITS-1))
	{
		DDX_Check(&dx, nID, m_bYutaUnit[nIndex]);
		if (m_bYutaUnit[nIndex])
		{
			m_bDeleteActivations[nIndex] = FALSE;
		}
		else
		{
			if (CheckActivations(nIndex, false))
			{
				dx.m_bSaveAndValidate = FALSE;
				m_bYutaUnit[nIndex] = TRUE;
				DDX_Check(&dx, nID, m_bYutaUnit[nIndex]);
				return;
			}
		}
	}
	SetModified();
}

BOOL CYutaPage::CheckActivations(int nIndex, bool bDelete)
{
	CString strText, sFormat, sNames;
	strText.Format(SM_YUTA_INPUT, nIndex+1);
	
	int i, nSize = 0;
	CInputGroup * pInputGroup = m_pInputs->GetGroupBySMName(strText);
	if (pInputGroup)
	{
		nSize = pInputGroup->GetSize();
		for (i=0; i<nSize; i++)
		{
			CInput *pInput = pInputGroup->GetInput(i);
			if (pInput->GetNumberOfInputToOutputs())
			{
				if (bDelete)
				{
					pInput->DeleteAllInputToOutputs();
				}
				else
				{
					sNames += _T(" * ") + pInput->GetName() + _T("\n");
				}
			}
		}
	}

	strText.Format(SM_YUTA_OUTPUT, nIndex+1);
	COutputGroup * pOutputGroup = m_pOutputs->GetGroupBySMName(strText);
	if (pOutputGroup)
	{
		nSize = pOutputGroup->GetSize();
		for (i=0; i<nSize; i++)
		{
			COutput *pOutput = pOutputGroup->GetOutput(i);
			if (bDelete)
			{
				CheckActivationsWithOutput(pOutput->GetID(), 0, SVP_CO_CHECK_SILENT);
			}
			else
			{
				if (!CheckActivationsWithOutput(pOutput->GetID(), 0, SVP_CO_CHECK_ONLY))
				{
					sNames += _T(" * ") + pOutput->GetName() + _T("\n");
				}
			}
		}
	}

	if (!sNames.IsEmpty())
	{
		sFormat.LoadString(IDS_EXISTING_ACTIVATION_YUTA);
		strText.Format(sFormat, sNames);
		if (IDYES == AfxMessageBox(strText, MB_YESNO))
		{
			m_bDeleteActivations[nIndex] = TRUE;
		}
		else
		{
			return TRUE;
		}
	}
	
	return FALSE;
}
