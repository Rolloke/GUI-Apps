// SDIDialog.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "SDIDialog.h"
#include "ComConfigurationDialog.h"
#include "IPConfigurationDialog.h"
#include "SDIConfigurationDialog.h"
#include "SDIConfigurationDialogIBM.h"
#include "SDIConfigurationDialogNCR.h"
#include "SDIConfigurationDialogMicrolock.h"
#include "SDIConfigurationDialogACT.h"
#include "SDIConfigurationDialogNCTDiva.h"
#include "SDIStarInterfaceDlg.h"
#include "SdiAlarmNumberDialog.h"

#include "SDIPage.h"
#include "wkclasses\SDIControl.h"

#include "Input.h"
#include "InputList.h"
#include "InputGroup.h"
#include "InputToOutput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_TXT_PORT_NAME AFX_IDW_CONTROLBAR_FIRST

/////////////////////////////////////////////////////////////////////////////
// CSDIDialog dialog
CSDIDialog::CSDIDialog(CSVView* pView, CSDIPage* pPage, int iPortNr, eSdiInterface nInterface/*=0*/)
	: CWK_Dialog(CSDIDialog::IDD)
{
	m_pView = pView;
	m_pPage = pPage;

	//{{AFX_DATA_INIT(CSDIDialog)
	m_iIndexCBProtocol = -1;
	m_sComment = _T("");
	m_sCurrency = _T("");
	//}}AFX_DATA_INIT

	m_nInterface = nInterface;
	m_bOn = (nInterface == SDI_INTERFACE_COM) ? FALSE : TRUE;
	SetLocationNo(iPortNr);

	m_eProtocol = SDICT_UNKNOWN;
	m_pCSDIProtocolArray = NULL;
	m_pCSDIProtocolArray = new CSDIProtocolArray(m_sSection);
	m_pComParameters = NULL;
	// m_AlarmTypeArray

	Create(IDD, m_pPage);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::SetLocationNo(int nLocNo)
{
	switch (m_nInterface)
	{
		case SDI_INTERFACE_COM:
			m_iPortNr = nLocNo;
			m_sPort.Format(szComFormat, m_iPortNr);
			m_sSection.Format(szSectionFormat, m_iPortNr);
			break;
		case SDI_INTERFACE_UDP:
			m_sPort.Format(szUDPFormat, nLocNo);
			m_sSection.Format(szSectionUDP, nLocNo);
			break;
		default: ASSERT(FALSE); break;
	}
}
/////////////////////////////////////////////////////////////////////////////
CSDIDialog::~CSDIDialog() 
{
	WK_DELETE(m_pCSDIProtocolArray);
	WK_DELETE(m_pComParameters);
	m_StarDevices.DeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::PostNcDestroy() 
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIDialog::StretchElements()
{
	return TRUE;
}
void CSDIDialog::SwitchOn(BOOL bOn)
{
	m_bOn = bOn;
	CDataExchange dx(this, FALSE);
	DDX_Check(&dx, IDC_CHECK_SDI, m_bOn);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::InitProtocolListbox()
{
	// ACHTUNG!!! Listbox-Eintraege koennen fehlen,
	// daher SDIControlType als Data verankern
	int iIndex = -1;
	CString sEntry;
	SDIControlType eProtocol;
	CComParameters ComParams(m_sSection);
	CString sVersion = m_pPage->GetProfile().GetString(_T("Version"),_T("Number"),_T(""));
	// Geldautomaten
	if (   m_pPage->GetDongle().AllowSDICashSystems() 
		&& m_nInterface == SDI_INTERFACE_COM) 
	{
		// Ascom
		if (sVersion>=_T("4.1"))
		{
			eProtocol = SDICT_ASCOM;
			CSDIControl::GetStandardComParams(eProtocol, ComParams);
			sEntry.Format(_T("%s (%s)"),
							CSDIControl::UIStringOfEnum(eProtocol),
							ComParams.GetParamsString() );
			iIndex = m_cbProtocol.AddString(sEntry);
			m_cbProtocol.SetItemData(iIndex, eProtocol);
		}
		// IBM
		eProtocol = SDICT_IBM;
		CSDIControl::GetStandardComParams(eProtocol, ComParams);
		sEntry.Format(_T("%s (%s)"),
						CSDIControl::UIStringOfEnum(eProtocol),
						ComParams.GetParamsString() );
		iIndex = m_cbProtocol.AddString(sEntry);
		m_cbProtocol.SetItemData(iIndex, eProtocol);
		// NCR
		eProtocol = SDICT_NCR_VER_1_1;
		CSDIControl::GetStandardComParams(eProtocol, ComParams);
		sEntry.Format(_T("%s (%s)"),
						CSDIControl::UIStringOfEnum(eProtocol),
						ComParams.GetParamsString() );
		iIndex = m_cbProtocol.AddString(sEntry);
		m_cbProtocol.SetItemData(iIndex, eProtocol);
		// NCR neues Protokoll
		eProtocol = SDICT_NCR_VER_1_2;
		CSDIControl::GetStandardComParams(eProtocol, ComParams);
		sEntry.Format(_T("%s (%s)"),
						CSDIControl::UIStringOfEnum(eProtocol),
						ComParams.GetParamsString() );
		iIndex = m_cbProtocol.AddString(sEntry);
		m_cbProtocol.SetItemData(iIndex, eProtocol);
		// SNI Chase Schnittstelle
		eProtocol = SDICT_SNI_DOS_CHASE;
		CSDIControl::GetStandardComParams(eProtocol, ComParams);
		sEntry.Format(_T("%s (%s)"),
						CSDIControl::UIStringOfEnum(eProtocol),
						ComParams.GetParamsString() );
		iIndex = m_cbProtocol.AddString(sEntry);
		m_cbProtocol.SetItemData(iIndex, eProtocol);
		// SNI IBM Emulation
		eProtocol = SDICT_SNI_IBM_EMU;
		CSDIControl::GetStandardComParams(eProtocol, ComParams);
		sEntry.Format(_T("%s (%s)"),
						CSDIControl::UIStringOfEnum(eProtocol),
						ComParams.GetParamsString() );
		iIndex = m_cbProtocol.AddString(sEntry);
		m_cbProtocol.SetItemData(iIndex, eProtocol);
		// SNI Dos COM
		if (sVersion>=_T("4.1"))
		{
			eProtocol = SDICT_SNI_DOS_COM;
			CSDIControl::GetStandardComParams(eProtocol, ComParams);
			sEntry.Format(_T("%s (%s)"),
							CSDIControl::UIStringOfEnum(eProtocol),
							ComParams.GetParamsString() );
			iIndex = m_cbProtocol.AddString(sEntry);
			m_cbProtocol.SetItemData(iIndex, eProtocol);
		}
		if (sVersion>=_T("4.2"))
		{
			eProtocol = SDICT_LANDAU_DTP;
			CSDIControl::GetStandardComParams(eProtocol, ComParams);
			sEntry.Format(_T("%s (%s)"),
							CSDIControl::UIStringOfEnum(eProtocol),
							ComParams.GetParamsString() );
			iIndex = m_cbProtocol.AddString(sEntry);
			m_cbProtocol.SetItemData(iIndex, eProtocol);
		}
	}
	// Zutrittskontrollen
	if ( m_pPage->GetDongle().AllowSDIAccessControl()
		&& m_nInterface == SDI_INTERFACE_COM) 
	{
		// Engel Multipass
		eProtocol = SDICT_ENGEL_MULTIPASS;
		CSDIControl::GetStandardComParams(eProtocol, ComParams);
		sEntry.Format(_T("%s (%s)"),
						CSDIControl::UIStringOfEnum(eProtocol),
						ComParams.GetParamsString() );
		iIndex = m_cbProtocol.AddString(sEntry);
		m_cbProtocol.SetItemData(iIndex, eProtocol);
		if (sVersion>=_T("4.1"))
		{
			// portachec 400
			eProtocol = SDICT_KEBA_PC400;
			CSDIControl::GetStandardComParams(eProtocol, ComParams);
			sEntry.Format(_T("%s (%s)"),
							CSDIControl::UIStringOfEnum(eProtocol),
							ComParams.GetParamsString() );
			iIndex = m_cbProtocol.AddString(sEntry);
			m_cbProtocol.SetItemData(iIndex, eProtocol);
		}

		// portachec 2000
		eProtocol = SDICT_KEBA_PC2000;
		CSDIControl::GetStandardComParams(eProtocol, ComParams);
		sEntry.Format(_T("%s (%s)"),
						CSDIControl::UIStringOfEnum(eProtocol),
						ComParams.GetParamsString() );
		iIndex = m_cbProtocol.AddString(sEntry);
		m_cbProtocol.SetItemData(iIndex, eProtocol);

		// Pasador
		eProtocol = SDICT_KEBA_PASADOR;
		CSDIControl::GetStandardComParams(eProtocol, ComParams);
		sEntry.Format(_T("%s (%s)"),
						CSDIControl::UIStringOfEnum(eProtocol),
						ComParams.GetParamsString() );
		iIndex = m_cbProtocol.AddString(sEntry);
		m_cbProtocol.SetItemData(iIndex, eProtocol);

		// TDSi Multilock
		if (sVersion>=_T("4.6"))
		{
			eProtocol = SDICT_ACCESS_MICROLOCK;
			CSDIControl::GetStandardComParams(eProtocol, ComParams);
			sEntry.Format(_T("%s (%s)"),
							CSDIControl::UIStringOfEnum(eProtocol),
							ComParams.GetParamsString() );
			iIndex = m_cbProtocol.AddString(sEntry);
			m_cbProtocol.SetItemData(iIndex, eProtocol);
		}

		// ACT Access Control
//		if (sVersion>=_T("4.6"))
		{
			eProtocol = SDICT_ACCESS_ACT;
			CSDIControl::GetStandardComParams(eProtocol, ComParams);
			sEntry.Format(_T("%s (%s)"),
							CSDIControl::UIStringOfEnum(eProtocol),
							ComParams.GetParamsString() );
			iIndex = m_cbProtocol.AddString(sEntry);
			m_cbProtocol.SetItemData(iIndex, eProtocol);
		}
//#ifdef _DEBUG
		if (sVersion>=_T("5.0.2"))
		{
			eProtocol = SDICT_STAR_INTERFACE;
			CSDIControl::GetStandardComParams(eProtocol, ComParams);
			sEntry.Format(_T("%s (%s)"),
				CSDIControl::UIStringOfEnum(eProtocol),
				ComParams.GetParamsString() );
			iIndex = m_cbProtocol.AddString(sEntry);
			m_cbProtocol.SetItemData(iIndex, eProtocol);
		}
//#endif
	}
	// Industrielle Anlagen
	if (   m_pPage->GetDongle().AllowSDIIndustrialEquipment()
		&& m_nInterface == SDI_INTERFACE_COM) 
	{
		// German Parcel
		eProtocol = SDICT_GERMAN_PARCEL;
		CSDIControl::GetStandardComParams(eProtocol, ComParams);
		sEntry.Format(_T("%s (%s)"),
						CSDIControl::UIStringOfEnum(eProtocol),
						ComParams.GetParamsString() );
		iIndex = m_cbProtocol.AddString(sEntry);
		m_cbProtocol.SetItemData(iIndex, eProtocol);
		
		// Barcode Scanner
		eProtocol = SDICT_SCANNER;
		CSDIControl::GetStandardComParams(eProtocol, ComParams);
		sEntry.Format(_T("%s (%s)"), CSDIControl::UIStringOfEnum(eProtocol), ComParams.GetParamsString() );
		iIndex = m_cbProtocol.AddString(sEntry);
		m_cbProtocol.SetItemData(iIndex, eProtocol);

		// NCT DIVA Warenwirtschaftssystem
		eProtocol = SDICT_NCT_DIVA;
		CSDIControl::GetStandardComParams(eProtocol, ComParams);
		sEntry.Format(_T("%s (%s)"), CSDIControl::UIStringOfEnum(eProtocol), ComParams.GetParamsString() );
		iIndex = m_cbProtocol.AddString(sEntry);
		m_cbProtocol.SetItemData(iIndex, eProtocol);

		// Point Of Sale
		if (sVersion>=_T("5.1.2"))
		{
			eProtocol = SDICT_POS;
			sEntry.Format(_T("%s (%s)"), CSDIControl::UIStringOfEnum(eProtocol), ComParams.GetParamsString() );
			iIndex = m_cbProtocol.AddString(sEntry);
			m_cbProtocol.SetItemData(iIndex, eProtocol);
		}
}
	if (m_pPage->GetDongle().AllowSDIParkmanagment())
	{
		// Designa PM 100
		if (m_nInterface == SDI_INTERFACE_UDP)
		{
			eProtocol = SDICT_DESIGNA_PM_100_ALARM;
			sEntry.Format(_T("%s"), CSDIControl::UIStringOfEnum(eProtocol));
			iIndex = m_cbProtocol.AddString(sEntry);
			m_cbProtocol.SetItemData(iIndex, eProtocol);

			eProtocol = SDICT_DESIGNA_PM_100_XML;
			sEntry.Format(_T("%s"), CSDIControl::UIStringOfEnum(eProtocol));
			iIndex = m_cbProtocol.AddString(sEntry);
			m_cbProtocol.SetItemData(iIndex, eProtocol);

			// Designa PM Abacus
			eProtocol = SDICT_DESIGNA_PM_ABACUS;
			sEntry.Format(_T("%s"), CSDIControl::UIStringOfEnum(eProtocol));
			iIndex = m_cbProtocol.AddString(sEntry);
			m_cbProtocol.SetItemData(iIndex, eProtocol);
		}

		if (m_nInterface == SDI_INTERFACE_COM)
		{
			// Schneider Intercom
			eProtocol = SDICT_SCHNEIDER_INTERCOM;
			sEntry.Format(_T("%s (%s)"), CSDIControl::UIStringOfEnum(eProtocol), ComParams.GetParamsString() );
			iIndex = m_cbProtocol.AddString(sEntry);
			m_cbProtocol.SetItemData(iIndex, eProtocol);
		}
	}

	if (sVersion>=_T("5.1.2"))
	{
		// idip XML
		eProtocol = SDICT_IDIP_XML;
		if (m_nInterface == SDI_INTERFACE_COM)
		{
			sEntry.Format(_T("%s (%s)"), CSDIControl::UIStringOfEnum(eProtocol), ComParams.GetParamsString() );
		}
		else
		{
			sEntry.Format(_T("%s"), CSDIControl::UIStringOfEnum(eProtocol));
		}
		iIndex = m_cbProtocol.AddString(sEntry);
		m_cbProtocol.SetItemData(iIndex, eProtocol);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::EnableControls()
{
	BOOL bShow = FALSE;
	m_staticPort.EnableWindow(m_bOn);
	if (m_nInterface == SDI_INTERFACE_COM)
	{
		bShow = CSDIControl::IsComConfigurable(m_eProtocol);
		m_btnConfigurateCom.ShowWindow(m_bOn && bShow ? SW_SHOW : SW_HIDE);
		m_btnConfigurateCom.EnableWindow(m_bOn && bShow);
	}
	else
	{
		GetDlgItem(IDC_COM_SDI)->EnableWindow(m_bOn);
		GetDlgItem(IDC_CHECK_SDI)->ShowWindow(SW_HIDE);
	}

	m_cbProtocol.EnableWindow(m_bOn);
	bShow = CSDIControl::IsProtocolConfigurable(m_eProtocol);
	m_btnConfigurateProtocol.ShowWindow(m_bOn && bShow ? SW_SHOW : SW_HIDE);
	m_btnConfigurateProtocol.EnableWindow(m_bOn && bShow);

	bShow = CSDIControl::IsCashSystem(m_eProtocol);
	m_cbCurrency.ShowWindow(m_bOn && bShow ? SW_SHOW : SW_HIDE);
	m_cbCurrency.EnableWindow(m_bOn && bShow);
	m_editComment.EnableWindow(m_bOn);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::ClearData()
{
	m_eProtocol = SDICT_UNKNOWN;
	m_eInitialProtocol = m_eProtocol;
	m_iIndexCBProtocol = -1;
	m_sComment = _T("");
	m_sCurrency = _T("");
	m_pCSDIProtocolArray->DeleteAll();
	WK_DELETE(m_pComParameters);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::ProtocolChanged()
{
	// Das ProtocolArray muss geloescht werden,
	// sonst hat man falsche Einstellungen vorgegeben!
	m_pCSDIProtocolArray->DeleteAll();
	m_AlarmNumbers.RemoveAll();
	// Neues Protokoll holen
	m_eProtocol = (SDIControlType)m_cbProtocol.GetItemData(m_iIndexCBProtocol);
	// Setze Standardwerte für einige konfigurierbare Protokolle
	SetDefaultValues();
	// Die ComParams muessen auf Standard gesetzt werden,
	// sonst hat man u.U. falsche Einstellungen vorgegeben!
	if (m_pComParameters) 
	{
		CSDIControl::GetStandardComParams(m_eProtocol, *m_pComParameters);											
	}
	m_sCurrency = _T("");
	m_sComment = _T("");
	UpdateData(FALSE);
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::SetDefaultValues()
{
	CSDIConfigurationDialog* pDlg = NULL;
/*	if (NULL==m_pComParameters) {
		m_pComParameters = new CComParameters(m_sSection);
		CSDIControl::GetStandardComParams(m_eProtocol, *m_pComParameters);											
		m_pComParameters->LoadFromRegistry( m_pPage->GetProfile() );
	}
*/
	CSDIProtocolArray protocolArray(*m_pCSDIProtocolArray);
	switch (m_eProtocol)
	{
		case SDICT_ACCESS_MICROLOCK:
			pDlg = new CSDIConfigurationDialogMicrolock(m_eProtocol,
														m_iPortNr,
														m_pCSDIProtocolArray,
														&m_AlarmTypeArray,
														m_pComParameters,
														m_pView);
			break;
		case SDICT_ACCESS_ACT:
			pDlg = new CSDIConfigurationDialogACT(m_eProtocol,
												  m_iPortNr,
												  m_pCSDIProtocolArray,
												  &m_AlarmTypeArray,
												  m_pComParameters,
												  m_pView);
			break;
		case SDICT_NCT_DIVA:
			pDlg = new CSDIConfigurationDialogNCTDiva(m_eProtocol,
													  m_iPortNr,
													  m_pCSDIProtocolArray,
													  &m_AlarmTypeArray,
													  m_pComParameters,
													  m_pView);
			break;
		default:
			break;
	}
	if (pDlg)
	{
		pDlg->SetDefaultValues();
	}
	WK_DELETE(pDlg);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::SetModified()
{
	m_pPage->SetModified();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIDialog::IsDataValid()
{
	BOOL bReturn = TRUE;

	UpdateData();

	if (m_bOn) 
	{
		CString sCom, sFormat, sMsg;
		switch (m_nInterface)
		{
			case SDI_INTERFACE_COM:
				sCom.Format(szComFormat, m_iPortNr);
				break;
			case SDI_INTERFACE_UDP:
				sCom.Format(szUDPFormat, m_iPortNr);
				if (!IsBetween(m_iPortNr, 1, 65535))
				{
					bReturn = FALSE;
					AfxMessageBox(AFX_IDP_PARSE_INT, MB_OK);
					GotoDlgCtrl(GetDlgItem(IDC_COM_SDI));
				}break;
			case SDI_INTERFACE_TCP:
				sCom.Format(szTCPIPFormat, m_iPortNr);
				if (!IsBetween(m_iPortNr, 1, 65535))
				{
					bReturn = FALSE;
					AfxMessageBox(AFX_IDP_PARSE_INT, MB_OK);
					GotoDlgCtrl(GetDlgItem(IDC_COM_SDI));
				}break;
		}
		if (!bReturn)
		{
			return bReturn;
		}
		// Erst pruefen, ob Protokoll ausgewaehlt ist
		if (m_iIndexCBProtocol==LB_ERR) 
		{
			// Wenn kein Protokoll ausgewaehlt ist,
			// kann diese Schnittstelle nicht aktiviert werden
			bReturn = FALSE;
			sFormat.LoadString(IDS_NO_SDI_TYPE_SELECTED);
			sMsg.Format(sFormat, sCom);
			AfxMessageBox(sMsg, MB_ICONSTOP);
			m_cbProtocol.SetFocus();
		}
		// Ist keine gueltige Waehrung angegeben und ist es ein GA
		else if ( CSDIControl::IsCurrencyValid(m_sCurrency) == FALSE 
					&& CSDIControl::IsCashSystem(m_eProtocol) == TRUE ) 
		{
			// Hat sie die richtige Laenge
			if ( m_sCurrency.GetLength() == 3 ) {
				// Warnung ausgeben
				sFormat.LoadString(IDS_CURRENCY_UNKNOWN);
				sMsg.Format(sFormat, sCom);
				int iReturn = AfxMessageBox(sMsg, MB_YESNO|MB_ICONQUESTION);
				switch (iReturn) 
				{
				case IDYES:
					break;
				case IDNO:
				default:
					bReturn = FALSE;
					m_cbCurrency.SetFocus();
					break;
				}
			}
			else 
			{
				// Ist komplett falsch
				bReturn = FALSE;
				sFormat.LoadString(IDS_WRONG_CURRENCY);
				sMsg.Format(sFormat, sCom);
				AfxMessageBox(sMsg, MB_ICONSTOP);
				m_cbCurrency.SetFocus();
			}
		}
		// Oder wurde kein sinnvoller Name eingegeben (leer oder nur Leerzeichen)
		else if ( m_sComment.IsEmpty()
				  || m_sComment.GetLength()==m_sComment.SpanIncluding(_T(" ")).GetLength() )
		{
			// Wenn kein Name eingegeben wurde,
			// Standartnamen einsetzen und User benachrichtigen
			SDIControlType eProtocol = (SDIControlType)m_cbProtocol.GetItemData(m_iIndexCBProtocol);
			m_sComment = CSDIControl::UIStringOfEnum(eProtocol,TRUE);
			m_sComment += _T(" ") + sCom;
			UpdateData(FALSE);
			sFormat.LoadString(IDS_NO_SDI_NAME);
			sMsg.Format(sFormat, sCom, m_sComment);
			int iReturn = AfxMessageBox(sMsg, MB_YESNO|MB_ICONQUESTION);
			switch (iReturn)
			{
			case IDNO:
				bReturn = FALSE;
				m_editComment.SetFocus();
				m_editComment.SetSel(0, -1);
				break;
			case IDYES:
			default:
				break;
			}
		}
		else if (   CSDIControl::HasAlarmNumbers(m_eProtocol)
				 && m_AlarmNumbers.GetCount() == 0)
		{
			AfxMessageBox(IDS_NO_ALARMNUMBERS);
			PostMessage(WM_COMMAND, IDC_BUTTON_CONFIGURATE_PROTOCOL, (LPARAM)m_hWnd);
			bReturn = FALSE;
		}
	}

	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
CString CSDIDialog::GetShmName()
{
	CString sSMName;
	if (m_nInterface == SDI_INTERFACE_COM)
	{
		sSMName.Format(_T("%s%d"), SM_SDIUNIT_INPUT, m_iPortNr);
	}
	else
	{
		if (m_iInitialPortNo != m_iPortNr)
		{
			sSMName.Format(_T("%sUDP%d"), SM_SDIUNIT_INPUT, m_iInitialPortNo);
			m_pPage->m_pInputList->DeleteGroup(sSMName, FALSE);
		}
		sSMName.Format(_T("%sUDP%d"), SM_SDIUNIT_INPUT, m_iPortNr);
	}
	return sSMName;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::SaveChanges()
{
	WK_TRACE_USER(_T("InterfaceSerialData-Einstellungen %s wurden geändert\n"), m_sPort);
	UpdateData();

	CWK_Profile& wkp = m_pPage->GetProfile();
	CString sSMName = GetShmName();

	// Ist die Schnittstelle ueberhaupt aktiviert
	if (m_bOn)
	{
		// erstmal Registry loeschen, damit keine Leichen herumliegen
		// loescht die Section SDIUnit\COMx
		wkp.DeleteSection(m_sSection);

		if (m_nInterface != SDI_INTERFACE_COM)
		{
			wkp.WriteInt(m_sSection, szEntryPort, m_iPortNr);
			if (!m_sIPaddress.IsEmpty())
			{
				wkp.WriteString(m_sSection, szEntryIPaddress, m_sIPaddress);
			}
			if (!m_sSubnetMask.IsEmpty())
			{
				wkp.WriteString(m_sSection, szEntrySubnetMask, m_sSubnetMask);
			}
		}
		
		// Wenn die COM-Parameter veraendert wurden, dieses sichern
		if (   CSDIControl::IsComConfigurable(m_eProtocol)
			&& m_pComParameters) 
		{
			m_pComParameters->SaveToRegistry( wkp ) ;
		}
		// Wenn es ein einstellbares Protokoll ist, dieses sichern
		if ( CSDIControl::IsProtocolConfigurable(m_eProtocol) ) 
		{
			m_pCSDIProtocolArray->SaveToRegistry( wkp ) ;
		}

		wkp.WriteInt(m_sSection, szEntryType, m_eProtocol);
		wkp.WriteString(m_sSection, szEntryComment, m_sComment);
		wkp.WriteString(m_sSection, szEntryCurrency, m_sCurrency);

		// Neuen Gruppennamen zusammenbasteln
		CString sNewGroupName = m_sComment;
		/*
		if (sNewGroupName.Find(m_sCom) == -1) 
		{
			sNewGroupName += _T(" ") + m_sCom;
		}*/
		int iInputs = CSDIControl::GetNumberOfInputs(m_eProtocol);
		if (CSDIControl::HasAlarmNumbers(m_eProtocol))
		{
			m_AlarmNumbers.Save(wkp, m_sSection);
			iInputs = m_AlarmNumbers.GetSize();
		}
		if (m_eProtocol == SDICT_STAR_INTERFACE)
		{
			for (int i=0;i<m_StarDevices.GetSize();i++)
			{
				CSDIStarDevice* pSI = m_StarDevices.GetAtFast(i);
				CString sSMStarInterface;
				CString sGroupNameStarInterface;
				sSMStarInterface.Format(_T("%s_%02d"),sSMName,pSI->GetID());
				sGroupNameStarInterface.Format(_T("%s %s"),sNewGroupName,pSI->GetName());
				SaveChangesInputGroup(m_eInitialProtocol,
									  m_eProtocol,
									  m_pPage->m_pInputList, 
									  m_pPage->m_pDatabaseFields,
									  m_AlarmNumbers,
									  m_pCSDIProtocolArray,
									  sSMStarInterface,
									  sGroupNameStarInterface, 
									  m_sInitialComment,
									  iInputs);
				m_sInitialComment == sGroupNameStarInterface;
				m_pPage->m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);
			}
			m_StarDevices.Save(wkp,m_sSection+_T("\\Devices"));
		}
		else
		{
			CInputGroup* pInputGroup = 
			SaveChangesInputGroup(m_eInitialProtocol,
								  m_eProtocol,
								  m_pPage->m_pInputList,
								  m_pPage->m_pDatabaseFields,
								  m_AlarmNumbers,
								  m_pCSDIProtocolArray,
								  sSMName,
								  sNewGroupName, 
								  m_sInitialComment,
								  iInputs);
			SetAlarmTypes(pInputGroup);
			m_sInitialComment = sNewGroupName;
			m_pPage->m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);
		}
	}
	else
	{	// ...wenn nicht, alles loeschen
		m_pPage->m_pInputList->DeleteGroup(sSMName,FALSE);
		wkp.DeleteSection(m_sSection);
		ClearData();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIDialog::IsEqual(SDIControlType eProtocol,
						 CInput* pInput,
						 CSDIAlarmNumbers& alarmNumbers,
						 int iAlarmNumberIndex)
{
	BOOL bRet = FALSE;
	switch (eProtocol) 
	{
	case SDICT_DESIGNA_PM_100_ALARM:
	case SDICT_DESIGNA_PM_100_XML:
	case SDICT_DESIGNA_PM_ABACUS:
		/*
		TRACE(_T("%d==%d,%d==%d,%d==%d\n"),pInput->GetExtra(0),(DWORD)alarmNumbers.GetBFR(iAlarmNumberIndex)
			,pInput->GetExtra(1),(DWORD)alarmNumbers.GetTCC(iAlarmNumberIndex)
			,pInput->GetExtra(2),(DWORD)alarmNumbers.GetAlarmNr(iAlarmNumberIndex));*/
		{
			bRet =  (   pInput->GetExtra(0) == (DWORD)alarmNumbers.GetBFR(iAlarmNumberIndex)
				     && pInput->GetExtra(1) == (DWORD)alarmNumbers.GetTCC(iAlarmNumberIndex)
				     && pInput->GetExtra(2) == (DWORD)alarmNumbers.GetAlarmNr(iAlarmNumberIndex));
		}
		break;
	case SDICT_SCHNEIDER_INTERCOM:
		{
			CString sSubscriberNr = alarmNumbers.GetSubscriberNr(iAlarmNumberIndex);
			CString sExtra1;
			sExtra1.Format(_T("%d"),pInput->GetExtra(0));
			bRet = sExtra1 == sSubscriberNr;
		}
		break;
	default:
		break;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::UpdateInputs(SDIControlType eProtocol,
							  CInputGroup* pInputGroup,
							  CSDIAlarmNumbers& alarmNumbers)
{
	if (   eProtocol == SDICT_DESIGNA_PM_100_ALARM
		|| eProtocol == SDICT_DESIGNA_PM_ABACUS
		|| eProtocol == SDICT_SCHNEIDER_INTERCOM
		|| eProtocol == SDICT_DESIGNA_PM_100_XML)
	{
		int iMax = 0;
		int i = 0;
		for (i=0;i<alarmNumbers.GetSize();i++)
		{
			int iNum = alarmNumbers.GetNr(i);
			if (iNum>iMax)
			{
				iMax = iNum;
			}
		}
		if (iMax>pInputGroup->GetSize())
		{
			pInputGroup->SetSize((WORD)iMax);
		}

		for (i=0;i<alarmNumbers.GetSize();i++)
		{
			int iNum = alarmNumbers.GetNr(i);
			// input schon da
			CInput* pInput = pInputGroup->GetInput(iNum-1);
			if (!IsEqual(eProtocol,pInput,alarmNumbers,i))
			{
				// changed or new
				pInput->DeleteAllInputToOutputs();
				switch (eProtocol) 
				{
				case SDICT_DESIGNA_PM_100_ALARM:
				case SDICT_DESIGNA_PM_100_XML:
				case SDICT_DESIGNA_PM_ABACUS:
					pInput->SetExtra(0,alarmNumbers.GetBFR(i));
					pInput->SetExtra(1,alarmNumbers.GetTCC(i));
					pInput->SetExtra(2,alarmNumbers.GetAlarmNr(i));
					break;
				case SDICT_SCHNEIDER_INTERCOM:
					{
						DWORD dwSubscriberNr = 0;
						_stscanf(alarmNumbers.GetSubscriberNr(i),_T("%d"),&dwSubscriberNr);
						pInput->SetExtra(0,dwSubscriberNr);
					}
					break;
				default:
					ASSERT(0);
					break;
				}
			}
		}
		for (i=0;i<pInputGroup->GetSize();i++)
		{
			BOOL bFound = FALSE;
			CInput* pInput = pInputGroup->GetInput(i);
			int iNr = i+1;
			for (int j=0;j<alarmNumbers.GetSize()&&!bFound;j++)
			{
				int iNum = alarmNumbers.GetNr(j);
				bFound = (iNum == iNr);
			}
			if (!bFound)
			{
				pInput->DeleteAllInputToOutputs();
				pInput->SetIDActivate(SECID_ACTIVE_OFF);
			}
		}
	}
}
CIPCField* CreateField(int nResourceID, LPCTSTR szName, WORD wLen, LPCTSTR szValue=NULL, DWORD dwFlags = 0)
{
	CString s;

	if (nResourceID && s.LoadString(nResourceID))
	{
		s.Replace(_T(":"),_T(""));
		s.Replace(_T("."),_T(""));
		s.Replace(_T("&"),_T(""));
	}
	else if (szValue)
	{
		s = szValue;
	}
	CIPCField*pField = new CIPCField(szName,s,wLen,'C');
	pField->SetFlags(dwFlags);
	return pField;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::AddDatabaseFields(CIPCFields* pDatabaseFields,SDIControlType eType)
{
	CIPCFields newFields;
	DWORD dwFlags = FIELD_FLAG_NO_EDIT_VALUE|FIELD_FLAG_NO_EDIT_TYPE|FIELD_FLAG_NO_EDIT_MAXLEN;
	// rke: possible values
	//  FIELD_FLAG_CAN_DEACTIATE
	//  FIELD_FLAG_UNUSED	 use also FIELD_FLAG_CAN_DEACTIATE !
	//  FIELD_FLAG_NO_EDIT_NAME
	//  FIELD_FLAG_NO_EDIT_VALUE
	//  FIELD_FLAG_NO_EDIT_TYPE
	//  FIELD_FLAG_NO_EDIT_MAXLEN

	switch (eType) 
	{
	case SDICT_DESIGNA_PM_100_ALARM:
		{
			newFields.Add(CreateField(IDS_DESIGNA_BFR,FIELD_DBD_BFR,3, NULL, dwFlags));
			newFields.Add(CreateField(IDS_DESIGNA_TCC,FIELD_DBD_TCC,3, NULL, dwFlags));
			newFields.Add(CreateField(IDS_ALARM_NR,FIELD_DBD_ANR,5, NULL, dwFlags));
			newFields.Add(CreateField(IDS_COMMENT,FIELD_DBD_COMMENT,30, NULL, dwFlags));
		}
		break;
	case SDICT_SCHNEIDER_INTERCOM:
		{
			newFields.Add(CreateField(IDS_SUBSCRIBER,FIELD_DBD_SUBSCRIBER,10, NULL, dwFlags));
			newFields.Add(CreateField(IDS_COMMENT,FIELD_DBD_COMMENT,30, NULL, dwFlags));
		}
		break;
	case SDICT_DESIGNA_PM_100_XML:
	case SDICT_DESIGNA_PM_ABACUS:
		{
			CString s;
			s.LoadString(IDS_DTP_DATE);
			newFields.Add(new CIPCField(CIPCField::m_sfDate,s,8,'D'));
			s.LoadString(IDS_DTP_TIME);
			newFields.Add(new CIPCField(CIPCField::m_sfTime,s,6,'T'));

			newFields.Add(CreateField(IDS_DESIGNA_BFR,FIELD_DBD_BFR,3, NULL, dwFlags));
			newFields.Add(CreateField(IDS_DESIGNA_TCC,FIELD_DBD_TCC,3, NULL, dwFlags));
			newFields.Add(CreateField(IDS_ALARM_NR,FIELD_DBD_ANR,5, NULL, dwFlags));
			newFields.Add(CreateField(IDS_COMMENT,FIELD_DBD_COMMENT,30, NULL, dwFlags));
			newFields.Add(CreateField(0, FIELD_DBD_TSN, 25, _T("Ticket Nr"), dwFlags));
		}
		break;
	case SDICT_POS:
		{
			CString s;
			s = CSDIControl::UIStringOfEnum(eType,TRUE);
			CIPCField*pField = new CIPCField(FIELD_DBD_POS,s,64,'C');
			pField->SetFlags(dwFlags);
			newFields.Add(pField);
		}
		break;
	default:
		{
			CString s = szDontUse;
			if (CSDIControl::HasDate(eType))
			{
				s.LoadString(IDS_DTP_DATE);
				newFields.Add(new CIPCField(CIPCField::m_sfDate,s,8,'D'));
			}
			if (CSDIControl::HasTime(eType))
			{
				s.LoadString(IDS_DTP_TIME);
				newFields.Add(new CIPCField(CIPCField::m_sfTime,s,6,'T'));
			}
			if (CSDIControl::HasAccount(eType))
			{
				s.LoadString(IDS_KTO_NR);
				newFields.Add(new CIPCField(CIPCField::m_sfKtNr,s,16,'C'));
			}
			if (CSDIControl::HasWSID(eType))
			{
				s.LoadString(IDS_GA_NR);
				newFields.Add(new CIPCField(CIPCField::m_sfGaNr,s,6,'C'));
			}
			if (CSDIControl::HasTAN(eType))
			{
				s.LoadString(IDS_TA_NR);
				newFields.Add(new CIPCField(CIPCField::m_sfTaNr,s,4,'C'));
			}
			if (CSDIControl::HasBankCode(eType))
			{
				s.LoadString(IDS_BANKLEITZAHL);
				newFields.Add(new CIPCField(CIPCField::m_sfBcNr,s,16,'C'));
			}
			if (CSDIControl::HasValue(eType))
			{
				s.LoadString(IDS_VALUE);
				newFields.Add(new CIPCField(CIPCField::m_sfAmnt,s,8,'C'));
			}
			if (CSDIControl::HasCurrency(eType))
			{
				s.LoadString(IDS_CURRENCY);
				newFields.Add(new CIPCField(CIPCField::m_sfCurr,s,3,'C'));
			}
		}
		break;
	}
	if (newFields.GetSize())
	{
		pDatabaseFields->AddNoDuplicates(newFields);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::DeleteDatabaseFields(SDIControlType eType)
{
	BOOL bFound = FALSE;
	for (int i=0;i<m_pPage->m_SDIDialogs.GetSize() && !bFound;i++)
	{
		CSDIDialog* pSDIDialog = m_pPage->m_SDIDialogs.GetAtFast(i);
		if (   WK_IS_WINDOW(pSDIDialog)
			&& pSDIDialog!=this)
		{
			bFound = pSDIDialog->m_eProtocol == eType;
			if (   eType == SDICT_DESIGNA_PM_100_ALARM
				&& pSDIDialog->m_eProtocol == SDICT_DESIGNA_PM_100_XML)
			{
				bFound = TRUE;
			}
			if (   eType == SDICT_DESIGNA_PM_100_XML
				&& pSDIDialog->m_eProtocol == SDICT_DESIGNA_PM_100_ALARM)
			{
				bFound = TRUE;
			}
		}
	}
	if (!bFound)
	{
		BOOL bDeleted = TRUE;
		switch (eType) 
		{
		case SDICT_DESIGNA_PM_100_ALARM:
			{
				CIPCField* pField;

				pField = m_pPage->m_pDatabaseFields->GetCIPCField(FIELD_DBD_BFR);
				m_pPage->m_pDatabaseFields->Delete(pField);
				pField = m_pPage->m_pDatabaseFields->GetCIPCField(FIELD_DBD_TCC);
				m_pPage->m_pDatabaseFields->Delete(pField);
				pField = m_pPage->m_pDatabaseFields->GetCIPCField(FIELD_DBD_ANR);
				m_pPage->m_pDatabaseFields->Delete(pField);
			}
			break;
		case SDICT_DESIGNA_PM_100_XML:
		case SDICT_DESIGNA_PM_ABACUS:
			{
				CIPCField* pField;
				pField = m_pPage->m_pDatabaseFields->GetCIPCField(FIELD_DBD_BFR);
				m_pPage->m_pDatabaseFields->Delete(pField);
				pField = m_pPage->m_pDatabaseFields->GetCIPCField(FIELD_DBD_TCC);
				m_pPage->m_pDatabaseFields->Delete(pField);
				pField = m_pPage->m_pDatabaseFields->GetCIPCField(FIELD_DBD_ANR);
				m_pPage->m_pDatabaseFields->Delete(pField);
				pField = m_pPage->m_pDatabaseFields->GetCIPCField(FIELD_DBD_TSN);
				m_pPage->m_pDatabaseFields->Delete(pField);
			}
			break;
		case SDICT_SCHNEIDER_INTERCOM:
			{
				CIPCField* pField;
				pField = m_pPage->m_pDatabaseFields->GetCIPCField(FIELD_DBD_SUBSCRIBER);
				m_pPage->m_pDatabaseFields->Delete(pField);
			}
			break;
		case SDICT_POS:
			{
				CIPCField* pField;
				pField = m_pPage->m_pDatabaseFields->GetCIPCField(FIELD_DBD_POS);
				m_pPage->m_pDatabaseFields->Delete(pField);
			}
			break;
		default:
			bDeleted = FALSE;
			break;
		}
		if (bDeleted)
		{
			m_pPage->m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
CInputGroup* CSDIDialog::SaveChangesInputGroup(SDIControlType& eInitialProtocol,
									   SDIControlType eProtocol,
									   CInputList* pInputList,
									   CIPCFields* pDatabaseFields,
									   CSDIAlarmNumbers& alarmNumbers,
									   CSDIProtocolArray* pSDIProtocolArray,
									   const CString& sSMName, 
									   const CString& sGroupName,
									   const CString& sInitialComment,
									   int iInputs)
{
	// Daten der Inputgruppe aktualisieren, wenn bereits vorhanden...
	CInputGroup* pInputGroup = NULL;
	pInputGroup = pInputList->GetGroupBySMName(sSMName);
	
	if (pInputGroup) 
	{
		// Wenn sich das Protokoll aendert, muss die Gruppe neu angelegt werden
		if (eInitialProtocol != eProtocol) 
		{
			eInitialProtocol = eProtocol;
			pInputList->DeleteGroup(sSMName);
			pInputGroup = pInputList->AddInputGroup(sGroupName,iInputs,sSMName);
			AddDatabaseFields(pDatabaseFields, eProtocol);
		}
		// vergleiche die Liste der Inputs mit der
		// m_AlarmNumbers
		UpdateInputs(eProtocol,pInputGroup,alarmNumbers);

		// Wenn sich der Kommentar aendert, muessen der Gruppenname
		// und die jeweiligen Inputnamen geaendert werden
		if (pInputGroup->GetName() != sGroupName) 
		{
			SaveChangesComment(pInputGroup, sInitialComment, sGroupName);
		}
	}
	// ...sonst neue Gruppe anlegen
	else 
	{
		pInputGroup = pInputList->AddInputGroup(sGroupName,iInputs,sSMName);
		UpdateInputs(eProtocol,pInputGroup,alarmNumbers);
		AddDatabaseFields(pDatabaseFields, eProtocol);
	}

	// Einige GA haben bestimmte Melderbezeichnungen
	SetSpecialInputNames(eProtocol,pInputGroup,alarmNumbers,pSDIProtocolArray);

	return pInputGroup;
}
//////////////////////////////////////////////////////////////////////////////
void CSDIDialog::SetAlarmTypes(CInputGroup* pInputGroup)
{
	// Input ggf. aktivieren und Name aendern, wenn gewuenscht
	CString sName;
	CSDIAlarmType* pAlarmType = NULL;
	CInput* pInput = NULL;
	// m_AlarmTypeArray kann auch leer sein
	for (int i=0 ; i<m_AlarmTypeArray.GetSize() ; i++)
	{
		pAlarmType = m_AlarmTypeArray.GetAt(i);
		pInput = pInputGroup->GetInput( pAlarmType->GetAlarm() );
		if (pInput)
		{
			pInput->SetIDActivate(SECID_ACTIVE_ON);
			sName = pInput->GetName();
			if ( sName.Find( pAlarmType->GetAlarmTypeString() ) == -1 )
			{
				sName += _T(" ") + pAlarmType->GetAlarmTypeString();
				pInput->SetName(sName);
			}
		}
		else 
		{
			TRACE(_T("no input found for sdi\n"));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::SaveChangesComment(CInputGroup* pInputGroup, 
									CString sInitialComment,
									CString sNewGroupName)
{
	if (pInputGroup) 
	{
		CString sOldGroupName = pInputGroup->GetName();
		// m_sInitialComment erst am Ende neu setzen!
		// Neuen Gruppennamen setzen
		pInputGroup->SetName(sNewGroupName);
		// Im Inputnamen den alten Gruppennamen oder Kommentar ersetzen
		// oder neuen Kommentar dem bisherigen Inputnamen voransetzen
		CString sOldName, sNewName;
		CInput* pInput = NULL;
		int iIndex = -1;
// Es sollen alle Inputnamen geaendert werden
//		iInputs = pInputGroup->GetNrOfActiveInputs();
		for (int i=0 ; i<pInputGroup->GetSize() ; i++)
		{
			pInput = pInputGroup->GetInput(i);
			sOldName = pInput->GetName();
			// Alten Gruppennamen suchen
			iIndex = sOldName.Find(sOldGroupName);
			if ( iIndex != -1 )
			{
				// Alten Gruppenname gefunden, ersetzen
				sNewName = sOldName.Left(iIndex)
						+ sNewGroupName
						+ sOldName.Mid(iIndex+sOldGroupName.GetLength());
				pInput->SetName(sNewName);
			}
			else 
			{
				// Alten Kommentar suchen
				iIndex = sOldName.Find(sInitialComment);
				if ( iIndex != -1 ) {
					// Alten Kommentar gefunden, ersetzen
					sNewName = sOldName.Left(iIndex)
							+ sNewGroupName
							+ sOldName.Mid(iIndex+sInitialComment.GetLength());
					pInput->SetName(sNewName);
				}
				else 
				{
					// Weder alten Gruppennamen noch alten Kommentar gefunden,
					// also neuen Kommentar voransetzen
					sNewName += sNewGroupName + _T(" ") + sOldName;
					pInput->SetName(sNewName);
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::SetSpecialInputNames(SDIControlType eProtocol, 
									  CInputGroup* pInputGroup,
									  CSDIAlarmNumbers& alarmNumbers,
									  CSDIProtocolArray* pSDIProtocolArray)
{
	if (pInputGroup) 
	{
		switch(eProtocol) 
		{
			case SDICT_SNI_DOS_CHASE:
				SetSpecialInputNamesSNI(pInputGroup);
				break;
			case SDICT_NCR_VER_1_1:
			case SDICT_NCR_VER_1_2:
				SetSpecialInputNamesNCR(pInputGroup);
				break;
			case SDICT_ACCESS_MICROLOCK:
			case SDICT_ACCESS_ACT:
			case SDICT_DESIGNA_PM_100_ALARM:
			case SDICT_DESIGNA_PM_100_XML:
			case SDICT_DESIGNA_PM_ABACUS:
			case SDICT_SCHNEIDER_INTERCOM:
				SetSpecialInputNamesGeneric(pInputGroup,alarmNumbers,pSDIProtocolArray);
				break;
			case SDICT_STAR_INTERFACE:
				SetSpecialInputNamesStarInterface(pInputGroup);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::SetSpecialInputNamesNCR(CInputGroup* pInputGroup)
{
	if (pInputGroup) 
	{
		CInput* pInput = NULL;
		CString sOldName, sNewName, sSpecial;
		int iIndex = -1;
		for (int i=0 ; i<pInputGroup->GetSize() ; i++) {
			pInput = pInputGroup->GetInput(i);
			sOldName = pInput->GetName();
			if (i == 0) {
				// Diagnose
				sSpecial.LoadString(IDS_DIAGNOSIS);
				// Spezielle Bezeichnung suchen
				iIndex = sOldName.Find(sSpecial);
				if ( iIndex == -1 ) {
					// Nicht gefunden, anhaengen
					sNewName = sOldName + _T(" ") + sSpecial;
					pInput->SetName(sNewName);
				}
			}
			else if (i == 1) {
				// Gleiche TAN oder Nullen
				sSpecial.LoadString(IDS_SAME_TAN_OR_ZERO);
				// Spezielle Bezeichnung suchen
				iIndex = sOldName.Find(sSpecial);
				if ( iIndex == -1 ) {
					// Nicht gefunden, anhaengen
					sNewName = sOldName + _T(" ") + sSpecial;
					pInput->SetName(sNewName);
				}
			}
			else if (i == 2) {
				// Neue TAN
				sSpecial.LoadString(IDS_NEW_TAN);
				// Spezielle Bezeichnung suchen
				iIndex = sOldName.Find(sSpecial);
				if ( iIndex == -1 ) {
					// Nicht gefunden, anhaengen
					sNewName = sOldName + _T(" ") + sSpecial;
					pInput->SetName(sNewName);
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::SetSpecialInputNamesSNI(CInputGroup* pInputGroup)
{
	if (pInputGroup) {
		CInput* pInput = NULL;
		CString sOldName, sNewName, sSpecial;
		int iIndex = -1;
		for (int i=0 ; i<pInputGroup->GetSize() ; i++) {
			pInput = pInputGroup->GetInput(i);
			sOldName = pInput->GetName();
			if (i == 0) {
				// Raum
				sSpecial.LoadString(IDS_ROOM);
				// Spezielle Bezeichnung suchen
				iIndex = sOldName.Find(sSpecial);
				if ( iIndex == -1 ) {
					// Nicht gefunden, anhaengen
					sNewName = sOldName + _T(" ") + sSpecial;
					pInput->SetName(sNewName);
				}
			}
			else if (i == 1) {
				// Person
				sSpecial.LoadString(IDS_PORTRAIT);
				// Spezielle Bezeichnung suchen
				iIndex = sOldName.Find(sSpecial);
				if ( iIndex == -1 ) {
					// Nicht gefunden, anhaengen
					sNewName = sOldName + _T(" ") + sSpecial;
					pInput->SetName(sNewName);
				}
			}
			else if (i == 2) {
				// Geldfach
				sSpecial.LoadString(IDS_HAND_TO_MONEY);
				// Spezielle Bezeichnung suchen
				iIndex = sOldName.Find(sSpecial);
				if ( iIndex == -1 ) {
					// Nicht gefunden, anhaengen
					sNewName = sOldName + _T(" ") + sSpecial;
					pInput->SetName(sNewName);
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::SetSpecialInputNamesGeneric(CInputGroup* pInputGroup,
											 CSDIAlarmNumbers& alarmNumbers,
											 CSDIProtocolArray* pSDIProtocolArray
											 )
{
	if (pInputGroup)
	{
		CString sGroupName;
		if (alarmNumbers.GetSize())
		{
			CInput* pInput = NULL;
			CString sOldName, sNewName, sSpecial;
			int iNr = 0;
			for (int i=0; i<alarmNumbers.GetSize(); i++) 
			{
				iNr = alarmNumbers.GetNr(i);
				pInput = pInputGroup->GetInput(iNr-1);
				if (pInput)
				{
					pInput->SetName(pInputGroup->GetName() 
									+ _T(" ") + 
									alarmNumbers.GetComment(i));
				}
			}
		}
		else if (pSDIProtocolArray)
		{
			CInput* pInput = NULL;
			CString sOldName, sNewName, sSpecial;
			int iIndex = -1;
			for (int i=0 ; i<pInputGroup->GetSize() ; i++) 
			{
				pInput = pInputGroup->GetInput(i);
				sOldName = pInput->GetName();
				if (i < pSDIProtocolArray->GetSize())
				{ // take name of protocol
					sSpecial = pSDIProtocolArray->GetAt(i)->GetName();
					// Spezielle Bezeichnung suchen
					iIndex = sOldName.Find(sSpecial);
					if ( iIndex == -1 )
					{
						// Nicht gefunden, umbenennen
						sNewName = pInputGroup->GetName() + _T(" ") + sSpecial;
						pInput->SetName(sNewName);
					}
				}
				else
				{ // no more real detectors implemented, set default name
					sNewName.Format(_T("%s %i"), pInputGroup->GetName(), i+1);
					pInput->SetName(sNewName);
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::SetSpecialInputNamesStarInterface(CInputGroup* pInputGroup)
{
	if (pInputGroup)
	{
		CString sGroupName = pInputGroup->GetName();
		for (int i=0;i<pInputGroup->GetSize();i++)
		{
			CInput* pInput = pInputGroup->GetInput(i);
			if (pInput)
			{
				CString sName;
				sName .Format(_T("%02d"),i);

				switch (i)
				{
				case 0:
					sName.LoadString(IDS_VALID_ID);
					break;
				case 1:
					sName.LoadString(IDS_INVALID_ID);
					break;
				case 2:
					sName.LoadString(IDS_INVALID_TIME);
					break;
				case 3:
					sName.LoadString(IDS_ANTI_PASSBACK_IN);
					break;
				case 4:
					sName.LoadString(IDS_ANTI_PASSBACK_OUT);
					break;
				case 5:
					sName.LoadString(IDS_INVALID_PASSWORD);
					break;
				case 6:
					sName.LoadString(IDS_DOOR_ERROR);
					break;
				case 7:
					sName.LoadString(IDS_INVALID_FINGER);
					break;
				case 8:
					sName.LoadString(IDS_DURESS_MODE);
					break;
				}
				pInput->SetName(sGroupName+_T(" ")+sName);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::CancelChanges()
{
	TRACE(_T("CancelChanges %s\n"),m_sSection);
	ClearData();

	m_eProtocol		   = (SDIControlType)m_pPage->GetProfile().GetInt(m_sSection, szEntryType, SDICT_UNKNOWN);
	m_eInitialProtocol = m_eProtocol;
	m_sCurrency		   = m_pPage->GetProfile().GetString(m_sSection, szEntryCurrency, _T(""));
	m_sComment		   = m_pPage->GetProfile().GetString(m_sSection, szEntryComment, _T(""));
	m_sInitialComment  = m_sComment;

	if (m_nInterface == SDI_INTERFACE_COM)
	{
		m_bOn = (m_eProtocol!=SDICT_UNKNOWN);
	}
	else
	{
		m_iPortNr		 = m_pPage->GetProfile().GetInt(m_sSection, szEntryPort, 0);
		m_iInitialPortNo = m_iPortNr;
		m_sIPaddress	 = m_pPage->GetProfile().GetString(m_sSection, szEntryIPaddress, 0);
		m_sSubnetMask	 = m_pPage->GetProfile().GetString(m_sSection, szEntrySubnetMask, 0);
	}

	m_iIndexCBProtocol = -1;
	SDIControlType eProtocol = SDICT_UNKNOWN;
	BOOL bFound = FALSE;
	for (int i=0 ; !bFound && i<m_cbProtocol.GetCount() ; i++) 
	{
		eProtocol = (SDIControlType)m_cbProtocol.GetItemData(i);
		if (eProtocol == m_eProtocol) 
		{
			bFound = TRUE;
			m_iIndexCBProtocol = i;
			m_pCSDIProtocolArray->LoadFromRegistry( m_pPage->GetProfile() );
		}
	}
	switch (m_eProtocol)
	{
	case SDICT_STAR_INTERFACE:
		{
			CString sSection = m_sSection+_T("\\Devices"); 
			CWK_Profile& wkp = m_pPage->GetProfile(); 
			m_StarDevices.Load(wkp,sSection);
		}
		break;
	case SDICT_DESIGNA_PM_100_ALARM:
	case SDICT_DESIGNA_PM_100_XML:
	case SDICT_SCHNEIDER_INTERCOM:
	case SDICT_DESIGNA_PM_ABACUS:
		{
			CWK_Profile& wkp = m_pPage->GetProfile(); 
			m_AlarmNumbers.Load(wkp, m_sSection);
			m_AlarmNumbers.SetControlType(m_eProtocol);
		} 
		break;
	}
	UpdateData(FALSE);
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSDIDialog)
	DDX_Control(pDX, IDC_COMBO_CURRENCY, m_cbCurrency);
	DDX_Control(pDX, IDC_BUTTON_CONFIGURATE_COM, m_btnConfigurateCom);
	DDX_Control(pDX, IDC_COMBO_PROT, m_cbProtocol);
	DDX_Control(pDX, IDC_EDIT_COMMENT, m_editComment);
	DDX_Control(pDX, IDC_BUTTON_CONFIGURATE_PROTOCOL, m_btnConfigurateProtocol);
	DDX_Check(pDX, IDC_CHECK_SDI, m_bOn);
	DDX_CBIndex(pDX, IDC_COMBO_PROT, m_iIndexCBProtocol);
	DDX_Text(pDX, IDC_EDIT_COMMENT, m_sComment);
	DDX_CBString(pDX, IDC_COMBO_CURRENCY, m_sCurrency);
	//}}AFX_DATA_MAP
	switch (m_nInterface)
	{
		case SDI_INTERFACE_COM:
			DDX_Control(pDX, IDC_COM_SDI, m_staticPort);
			DDX_Text(pDX, IDC_COM_SDI, m_sPort);
			break;
		case SDI_INTERFACE_UDP:
			DDX_Control(pDX, IDC_TXT_PORT_NAME, m_staticPort);
			DDX_Text(pDX, IDC_COM_SDI, m_iPortNr);
			break;
		case SDI_INTERFACE_TCP:
			DDX_Control(pDX, IDC_TXT_PORT_NAME, m_staticPort);
			DDX_Text(pDX, IDC_COM_SDI, m_sIPaddress);
			DDV_Validate_IP_adr(pDX, m_sIPaddress);
			break;
	}
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSDIDialog, CWK_Dialog)
	//{{AFX_MSG_MAP(CSDIDialog)
	ON_BN_CLICKED(IDC_CHECK_SDI, OnCheckOn)
	ON_BN_CLICKED(IDC_BUTTON_CONFIGURATE_COM, OnButtonConfigurateCom)
	ON_CBN_SELCHANGE(IDC_COMBO_PROT, OnSelchangeComboProtocol)
	ON_EN_CHANGE(IDC_EDIT_COMMENT, OnChangeEdit)
	ON_EN_CHANGE(IDC_COM_SDI, OnChangeEdit)
	ON_BN_CLICKED(IDC_BUTTON_CONFIGURATE_PROTOCOL, OnButtonConfigurateProtocol)
	ON_CBN_SELCHANGE(IDC_COMBO_CURRENCY, OnSelchangeComboCurrency)
	ON_CBN_EDITCHANGE(IDC_COMBO_CURRENCY, OnEditchangeComboCurrency)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSDIDialog message handlers
BOOL CSDIDialog::OnInitDialog() 
{
	if (m_nInterface != SDI_INTERFACE_COM)
	{
		CRect rc;
		int nLeft = 5;
		CWnd *pWnd = GetDlgItem(IDC_CHECK_SDI);
		if (pWnd)
		{
			pWnd->GetWindowRect(&rc);
			ScreenToClient(&rc);
			nLeft = rc.left;
		}

		pWnd = GetDlgItem(IDC_COM_SDI);
		if (pWnd)
		{
			pWnd->GetWindowRect(&rc);
			ScreenToClient(&rc);
			rc.right = nLeft + rc.Width();
			rc.left = nLeft;
			pWnd->MoveWindow(&rc);
			pWnd->SetDlgCtrlID(IDC_TXT_PORT_NAME);
			pWnd->SetWindowText(_T("UDP"));
		}

		pWnd = GetDlgItem(IDC_BUTTON_CONFIGURATE_COM);
		if (pWnd)
		{
			pWnd->GetWindowRect(&rc);
			ScreenToClient(&rc);
			nLeft = rc.left;
			rc.left = rc.right - 15;
			pWnd->MoveWindow(&rc);
			pWnd->SetWindowText(_T("..."));
			rc.right = rc.left - 1;
			rc.left = nLeft;
			CEdit *pEdit = new CEdit;
			pEdit->CreateEx(WS_EX_NOPARENTNOTIFY|WS_EX_CLIENTEDGE,
				WC_EDIT, _T(""),
				WS_VISIBLE|WS_CHILD|ES_AUTOHSCROLL|WS_TABSTOP|ES_READONLY,
				rc, this, IDC_COM_SDI);
		}
	}

	CWK_Dialog::OnInitDialog();

	if (CSkinDialog::SetChildWindowFont(m_hWnd))
	{
		if (m_ToolTip.m_hWnd)
		{
			m_ToolTip.SetFont(CSkinDialog::GetDialogItemGlobalFont(), 0);
		}
	}

	InitProtocolListbox();

	//CancelChanges();
	
	return TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::OnCancel() 
{
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::OnOK() 
{
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::OnCheckOn() 
{
	SetModified();
	UpdateData();
	if (!m_bOn)
	{
		ClearData();
		UpdateData(FALSE);
	}
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::OnButtonConfigurateCom() 
{
	if (m_nInterface == SDI_INTERFACE_COM)
	{
		BOOL bFirstTime = FALSE;
		if (NULL==m_pComParameters) 
		{
			bFirstTime = TRUE;
			m_pComParameters = new CComParameters(m_sSection);
			CSDIControl::GetStandardComParams(m_eProtocol, *m_pComParameters);											
			m_pComParameters->LoadFromRegistry( m_pPage->GetProfile() );
		}
		CComConfigurationDialog dlg(m_sPort, m_pComParameters);
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
	else
	{
		CIPConfigurationDialog dlg;
		dlg.m_sIPaddress = m_sIPaddress;
		dlg.m_iPort      = m_iPortNr;
//		dlg.m_sSubnetMask = m_sSubnetMask;
		if (dlg.DoModal() == IDOK)
		{
			m_sIPaddress = dlg.m_sIPaddress;
			m_iPortNr    = dlg.m_iPort;
			SetDlgItemInt(IDC_COM_SDI, m_iPortNr, FALSE);
//			m_sSubnetMask = dlg.m_sSubnetMask;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::OnSelchangeComboProtocol() 
{
	// Vorherigen Index sichern
	int iIndexCBProtocolOld = m_iIndexCBProtocol;
	UpdateData();
	// Wenn sich der Index und damit das Protokoll geandert hat
	if (iIndexCBProtocolOld != m_iIndexCBProtocol) 
	{
		SetModified();
		ProtocolChanged();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::OnButtonConfigurateProtocol() 
{
	CDialog* pDlg = NULL;
	CSDIStarInterfaceDlg* pSDIStarInterfaceDlg = NULL;
	CSDIAlarmNumberDialog* pSDIAlarmNumberDialog = NULL;

	if (NULL==m_pComParameters) 
	{
		m_pComParameters = new CComParameters(m_sSection);
		CSDIControl::GetStandardComParams(m_eProtocol, *m_pComParameters);											
		m_pComParameters->LoadFromRegistry( m_pPage->GetProfile() );
	}

	CSDIProtocolArray protocolArray(*m_pCSDIProtocolArray);
	switch (m_eProtocol) 
	{
		case SDICT_IBM:
		case SDICT_SNI_IBM_EMU:
		case SDICT_ASCOM:
			pDlg = new CSDIConfigurationDialogIBM(m_eProtocol,
													m_iPortNr,
													&protocolArray,
													&m_AlarmTypeArray,
													m_pComParameters,
													m_pView);
			break;
		case SDICT_NCR_VER_1_2:
			pDlg = new CSDIConfigurationDialogNCR(m_eProtocol,
													m_iPortNr,
													&protocolArray,
													&m_AlarmTypeArray,
													m_pComParameters,
													m_pView);
			break;
		case SDICT_ACCESS_MICROLOCK:
			pDlg = new CSDIConfigurationDialogMicrolock(m_eProtocol,
														m_iPortNr,
														&protocolArray,
														&m_AlarmTypeArray,
														m_pComParameters,
														m_pView);
			break;
		case SDICT_ACCESS_ACT:
			pDlg = new CSDIConfigurationDialogACT(m_eProtocol,
												  m_iPortNr,
												  &protocolArray,
												  &m_AlarmTypeArray,
												  m_pComParameters,
												  m_pView);
			break;
		case SDICT_NCT_DIVA:
			pDlg = new CSDIConfigurationDialogNCTDiva(m_eProtocol,
													  m_iPortNr,
													  &protocolArray,
													  &m_AlarmTypeArray,
													  m_pComParameters,
													  m_pView);
			break;
		case SDICT_STAR_INTERFACE:
			{
				pSDIStarInterfaceDlg = new CSDIStarInterfaceDlg(m_eProtocol,
																m_iPortNr,
																&protocolArray,
																&m_AlarmTypeArray,
																m_pComParameters,
																m_pView);
				for (int i=0;i<m_StarDevices.GetSize();i++)
				{
					CSDIStarDevice* pSD = m_StarDevices.GetAtFast(i);
					CSDIStarDevice* pSDNew = new CSDIStarDevice(pSD->GetID(),pSD->GetName(),pSD->GetType());
					pSDIStarInterfaceDlg->m_Devices.Add(pSDNew);
				}
				pDlg = pSDIStarInterfaceDlg;
			}

			break;
		case SDICT_DESIGNA_PM_100_ALARM:
		case SDICT_DESIGNA_PM_100_XML:
		case SDICT_SCHNEIDER_INTERCOM:
		case SDICT_DESIGNA_PM_ABACUS:
			{
				CString sShmName = GetShmName();
				CInputGroup* pInputGroup = m_pPage->m_pInputList->GetGroupBySMName(sShmName);
				pSDIAlarmNumberDialog = new CSDIAlarmNumberDialog(m_eProtocol, &m_AlarmNumbers,pInputGroup);
				pDlg = pSDIAlarmNumberDialog;
			}
			break;
		default:
			ASSERT(0);
			break;
	}
	if (pDlg) 
	{
		if (IDOK == pDlg->DoModal())
		{
			WK_DELETE(m_pCSDIProtocolArray);
			m_pCSDIProtocolArray = new CSDIProtocolArray(protocolArray);
			if (pSDIStarInterfaceDlg)
			{
				m_StarDevices.DeleteAll();
				for (int i=0;i<pSDIStarInterfaceDlg->m_Devices.GetSize();i++)
				{
					CSDIStarDevice* pSD = pSDIStarInterfaceDlg->m_Devices.GetAtFast(i);
					m_StarDevices.Add(new CSDIStarDevice(pSD->GetID(),pSD->GetName(),pSD->GetType()));
				}
			}
			SetModified();
		}

		if (pSDIStarInterfaceDlg)
		{
			WK_DELETE(pSDIStarInterfaceDlg);
			pDlg = NULL;
		}
		if (pSDIAlarmNumberDialog)
		{
			WK_DELETE(pSDIAlarmNumberDialog);
			pDlg = NULL;
		}

		WK_DELETE(pDlg);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::OnSelchangeComboCurrency() 
{
	UpdateData();
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::OnEditchangeComboCurrency() 
{
	UpdateData();
	if (m_sCurrency.GetLength() > 3) {
		MessageBeep((UINT)-1);
		m_sCurrency = m_sCurrency.Left(3);
		m_cbCurrency.SetWindowText(m_sCurrency);
		m_cbCurrency.SetEditSel( m_sCurrency.GetLength(), -1 );
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::OnChangeEdit() 
{
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDialog::OnDestroy() 
{
	if (m_nInterface != SDI_INTERFACE_COM)
	{
		CWnd *pWnd = GetDlgItem(IDC_COM_SDI);
		WK_DELETE(pWnd);
	}
	CDialog::OnDestroy();
}
