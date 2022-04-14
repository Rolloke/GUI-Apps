// PTZconfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "PTZconfigDlg.h"
#include ".\ptzconfigdlg.h"

#define PTZCDF_EXTENDED	0x00000001
#define PTZCDF_EDITABLE	0x00000002
#define PTZCDF_EDITED   0x00000004


///////////////////////////////////////////////////////////////
// CPTZconfigDlg dialog
IMPLEMENT_DYNAMIC(CPTZconfigDlg, CDialog)
CPTZconfigDlg::CPTZconfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPTZconfigDlg::IDD, pParent)
{
	m_pWKP = NULL;
	m_CameraType = CCT_UNKNOWN;
}
///////////////////////////////////////////////////////////////
CPTZconfigDlg::~CPTZconfigDlg()
{
}
///////////////////////////////////////////////////////////////
void CPTZconfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PTZ_CONFIG_LIST, m_List);
}
///////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CPTZconfigDlg, CDialog)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_PTZ_CONFIG_LIST, OnLvnBeginlabeleditPtzConfigList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_PTZ_CONFIG_LIST, OnLvnEndlabeleditPtzConfigList)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////
// CPTZconfigDlg message handlers
BOOL CPTZconfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CSkinDialog::SetChildWindowFont(m_hWnd);
	if (!m_sSection.IsEmpty() && m_CameraType != CCT_UNKNOWN && m_pWKP)
	{
		CString sText, sFormat;
		GetWindowText(sText); 
		sFormat.Format(_T("%s, COM%d"), NameOfEnumPTZRealName(m_CameraType), m_nCom);
		sText += sFormat;
		SetWindowText(sText);
		const DWORD nItems[42][3] = 
		{
			{PTZF_PAN_LEFT, IDS_LEFT, 0},	// 0
			{PTZF_PAN_RIGHT, IDS_RIGHT, 0},
			{PTZF_TILT_UP, IDS_UP, 0},
			{PTZF_TILT_DOWN, IDS_DOWN, 0},
			{PTZF_MOVE_DIAGONAL, IDS_DIAGONAL, 0},
			{PTZF_FOCUS_NEAR, IDS_FOCUS_NEAR, 0},
			{PTZF_FOCUS_FAR, IDS_FOCUS_FAR, 0},
			{PTZF_FOCUS_AUTO,IDS_FOCUS_AUTO, 0},
			{PTZF_IRIS_OPEN, IDS_IRIS_OPEN, 0},
			{PTZF_IRIS_CLOSE, IDS_IRIS_CLOSE, 0},
			{PTZF_IRIS_AUTO, IDS_IRIS_AUTO, 0},
			{PTZF_ZOOM_IN, IDS_ZOOM_IN, 0},
			{PTZF_ZOOM_OUT, IDS_ZOOM_OUT, 0},
			{PTZF_POS_HOME, IDS_PTZ_POSITION, 0},
			{PTZF_POS_1, IDS_PTZ_POSITION, 0},
			{PTZF_POS_2, IDS_PTZ_POSITION, 0},
			{PTZF_POS_3, IDS_PTZ_POSITION, 0},
			{PTZF_POS_4, IDS_PTZ_POSITION, 0},
			{PTZF_POS_5, IDS_PTZ_POSITION, 0},
			{PTZF_POS_6, IDS_PTZ_POSITION, 0},
			{PTZF_POS_7, IDS_PTZ_POSITION, 0},
			{PTZF_POS_8, IDS_PTZ_POSITION, 0},
			{PTZF_POS_9, IDS_PTZ_POSITION, 0},
			{PTZF_SET_POS, IDS_SET_POS, 0},
			{PTZF_EX_ERASE_PRESET, IDS_ERASE_PRESET, PTZCDF_EXTENDED},
			{PTZF_SPEED, IDS_SPEED, 0},
			{PTZF_CONFIG, IDS_CONFIG, 0},
			{PTZF_TURN_180, IDS_TURN_180, 0},
	//		{PTZF_ROLL_LEFT, 0, 0},
	//		{PTZF_ROLL_RIGHT, 0, 0},
	//		{PTZF_EXPOSURE_LONGER, 0, 0},
	//		{PTZF_EXPOSURE_SHORTER, 0, 0},
			{PTZF_EX_AUXILIARY_OUT1, IDS_AUX_OUT, PTZCDF_EXTENDED|PTZCDF_EDITABLE},
			{PTZF_EX_AUXILIARY_OUT2, IDS_AUX_OUT, PTZCDF_EXTENDED|PTZCDF_EDITABLE},
			{PTZF_EX_AUXILIARY_OUT3, IDS_AUX_OUT, PTZCDF_EXTENDED|PTZCDF_EDITABLE},
			{PTZF_EX_AUXILIARY_OUT4, IDS_AUX_OUT, PTZCDF_EXTENDED|PTZCDF_EDITABLE},
			{PTZF_EX_AUXILIARY_OUT5, IDS_AUX_OUT, PTZCDF_EXTENDED|PTZCDF_EDITABLE},
			{PTZF_EX_AUXILIARY_OUT6, IDS_AUX_OUT, PTZCDF_EXTENDED|PTZCDF_EDITABLE},
			{PTZF_EX_AUXILIARY_OUT7, IDS_AUX_OUT, PTZCDF_EXTENDED|PTZCDF_EDITABLE},
			{PTZF_EX_AUXILIARY_OUT8, IDS_AUX_OUT, PTZCDF_EXTENDED|PTZCDF_EDITABLE},
			{0, 0, 0}
		};

		DWORD	dwCameraCommands   = GetCameraControlFunctions(m_CameraType);
		DWORD	dwCameraCommandsEx = GetCameraControlFunctionsEx(m_CameraType);
		DWORD	dwActualCC = m_pWKP->GetInt(m_sSection, REG_PTZF_CCFUNCTIONS, dwCameraCommands);
		DWORD	dwActualCCEx = m_pWKP->GetInt(m_sSection, REG_PTZF_CCFUNCTIONS_EX, dwCameraCommandsEx);
		DWORD	dwFlag;
		BOOL	bCheck = TRUE;
		CString sHeading, sFmt;
		int		nItem, i=-1, nInserted, nImage;
		CRect	rc;
		m_List.GetClientRect(&rc);
		sHeading.LoadString(IDS_PTZ_HEADING);
		m_List.InsertColumn(0, sHeading, 0, rc.Width()-GetSystemMetrics(SM_CXHTHUMB));

		m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);

		for (nItem=0, nInserted=0; nItems[nItem][0] && nItems[nItem][1]; nItem++)
		{
			dwFlag = nItems[nItem][0];
			nImage = nItems[nItem][2];
			if (nImage & PTZCDF_EXTENDED)
			{
				if ((dwFlag & dwCameraCommandsEx) == 0)
				{
					continue;
				}
				bCheck = dwActualCCEx & dwFlag ? TRUE : FALSE;
			}
			else
			{
				if ((dwFlag & dwCameraCommands) == 0)
				{
					continue;
				}
				bCheck = dwActualCC & dwFlag ? TRUE : FALSE;
			}
			if (nImage & PTZCDF_EDITABLE)
			{
				sHeading = m_pWKP->GetString(m_sSection, GetFlagName(dwFlag, nImage & PTZCDF_EXTENDED ? TRUE : FALSE), NULL);
				if (!sHeading.IsEmpty())
				{
//					RKE: Multilaguage support
//					sHeading = COemGuiApi::ExtractNameForCurrentLanguage(sHeading);
					nImage |= PTZCDF_EDITED;
				}
			}
			if (sHeading.IsEmpty())
			{
				sHeading.LoadString(nItems[nItem][1]);
				if (nItems[nItem][1] == IDS_PTZ_POSITION)
				{
					if (i==-1)	i = 0;
					else		i++;
					sFmt.Format(sHeading, i);
					sHeading = sFmt;
				}
				else if (nItems[nItem][1] == IDS_AUX_OUT)
				{
					if (i==-1)	i = 1;
					else		i++;
					sFmt.Format(sHeading, i);
					sHeading = sFmt;
				}
				else
				{
					i = -1;
				}
			}
			m_List.InsertItem(LVIF_TEXT|LVIF_PARAM|LVIF_IMAGE, nInserted, sHeading, 0, 0, nImage, dwFlag);
			m_List.SetCheck(nInserted++, bCheck);
			sHeading.Empty();
		}
		return TRUE;  // return TRUE unless you set the focus to a control
	}
	else
	{
		EndDialog(0);
		return FALSE;
	}

}
///////////////////////////////////////////////////////////////
void CPTZconfigDlg::OnOK()
{
	int		nCount = m_List.GetItemCount();
	DWORD	dwActualCC = 0;
	DWORD	dwActualCCEx = 0;
	LVITEM  lvi;
	ZERO_INIT(lvi);
	lvi.mask = LVIF_PARAM|LVIF_IMAGE;

	for (lvi.iItem=0; lvi.iItem<nCount; lvi.iItem++)
	{
		lvi.iImage = 0;
		lvi.lParam = 0;
		if (m_List.GetCheck(lvi.iItem) && m_List.GetItem(&lvi))
		{
			if (lvi.iImage & PTZCDF_EXTENDED)
			{
				dwActualCCEx |= lvi.lParam;
			}
			else
			{
				dwActualCC |= lvi.lParam;
			}
		}
		if (lvi.iImage & PTZCDF_EDITED)
		{
			CString sFlagName = GetFlagName(lvi.lParam, lvi.iImage & PTZCDF_EXTENDED ? TRUE : FALSE);
			CString sName = m_List.GetItemText(lvi.iItem, 0);
			if (!sFlagName.IsEmpty())
			{
//				RKE: Multilaguage support
//				CString sNames = m_pWKP->GetString(m_sSection, sFlagName, NULL);
//				COemGuiApi::AddNameForCurrentLanguage(sNames, sName);
//				m_pWKP->WriteString(m_sSection, sFlagName, sNames);
				m_pWKP->WriteString(m_sSection, sFlagName, sName);
			}
		}
	}

	if (dwActualCCEx)
	{
		dwActualCC |= PTZF_EXTENDED;
	}

	m_pWKP->WriteInt(m_sSection, REG_PTZF_CCFUNCTIONS, dwActualCC);
	m_pWKP->WriteInt(m_sSection, REG_PTZF_CCFUNCTIONS_EX, dwActualCCEx);
	CDialog::OnOK();
}
///////////////////////////////////////////////////////////////
void CPTZconfigDlg::OnLvnBeginlabeleditPtzConfigList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	pDispInfo->item.mask |= LVIF_IMAGE;
	m_List.GetItem(&pDispInfo->item);
	if (pDispInfo->item.iImage & PTZCDF_EDITABLE)
	{
		*pResult = 0;
	}
	else
	{
		*pResult = 1;
	}
}
///////////////////////////////////////////////////////////////
void CPTZconfigDlg::OnLvnEndlabeleditPtzConfigList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		m_List.SetItemText(pDispInfo->item.iItem, 0, pDispInfo->item.pszText);
		pDispInfo->item.mask &= ~LVIF_TEXT;
		pDispInfo->item.mask |= LVIF_IMAGE;
		m_List.GetItem(&pDispInfo->item);
		pDispInfo->item.iImage |= PTZCDF_EDITED;
		m_List.SetItem(&pDispInfo->item);
	}
	*pResult = 0;
}
#define RETURN_NAME_OF_FLAG(x,y) if (y==x) return _T(#x);
///////////////////////////////////////////////////////////////
CString CPTZconfigDlg::GetFlagName(DWORD dwFlag, BOOL bExtended)
{
	if (bExtended)
	{
		RETURN_NAME_OF_FLAG(PTZF_EX_AUXILIARY_OUT1, dwFlag);
		RETURN_NAME_OF_FLAG(PTZF_EX_AUXILIARY_OUT2, dwFlag);
		RETURN_NAME_OF_FLAG(PTZF_EX_AUXILIARY_OUT3, dwFlag);
		RETURN_NAME_OF_FLAG(PTZF_EX_AUXILIARY_OUT4, dwFlag);
		RETURN_NAME_OF_FLAG(PTZF_EX_AUXILIARY_OUT5, dwFlag);
		RETURN_NAME_OF_FLAG(PTZF_EX_AUXILIARY_OUT6, dwFlag);
		RETURN_NAME_OF_FLAG(PTZF_EX_AUXILIARY_OUT7, dwFlag);
		RETURN_NAME_OF_FLAG(PTZF_EX_AUXILIARY_OUT8, dwFlag);
	}
	return _T("");
}
