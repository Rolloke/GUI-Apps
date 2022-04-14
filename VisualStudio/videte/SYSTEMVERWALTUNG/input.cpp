/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: input.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/input.cpp $
// CHECKIN:		$Date: 16.08.06 14:21 $
// VERSION:		$Revision: 40 $
// LAST CHANGE:	$Modtime: 15.08.06 12:15 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "Input.h"
#include "InputGroup.h"

#include "MainFrm.h"
#include "SVDoc.h"

#include "ProcessList.h"
#include "globals.h"

//////////////////////////////////////////////////////////////////////////////
CInputToOutput::CInputToOutput(CInput* pInput)
{
	m_pInput		= pInput;
	m_idOutputOrMedia	= SECID_NO_ID;
	m_idProcess		= SECID_NO_ID;
	m_idTimer		= SECID_NO_ID;
	m_dwState		= IO_NOSTATE;
	m_dwPriority	= IO_DEFAULTPRIOR;
	m_PTZPreset = CCC_INVALID;

	m_bMarkForDelete = FALSE;
	// INIT m_archiveID
} 
//////////////////////////////////////////////////////////////////////////////
CInputToOutput::~CInputToOutput()
{
}
//////////////////////////////////////////////////////////////////////////////
void CInputToOutput::DeleteInputToOutputFromInput()
{
	if (m_pInput)
	{
		m_pInput->DeleteInputToOutput(this);	
	}
}
//////////////////////////////////////////////////////////////////////////////
// CInput
//////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CInput, CObject)

CInput::CInput(WORD wGroupID, WORD wSubID)
{
	m_id = CSecID(wGroupID,wSubID);
	m_wState = I_STATE_DEFAULT; 
	m_idActivate = SECID_ACTIVE_OFF;
	m_wState |= I_CAN_EDGE;
	m_wState |= I_STATE_TEMP_DEACT;

	m_tInstructionStamp = CTime::GetCurrentTime();
	m_tInstructionStampNew = m_tInstructionStamp;
	m_bDeleteInputToOutputs = FALSE;

}
//////////////////////////////////////////////////////////////////////////////
const CInput& CInput::operator=(const CInput& inp)
{
	m_id = inp.m_id;
	m_wState = inp.m_wState;
	m_idActivate = inp.m_idActivate;
	m_sName = inp.m_sName;

	m_dwExtras.RemoveAll();
	m_dwExtras.Append(inp.m_dwExtras);

	m_InputToOutputs.DeleteAll();
	for (int i=0;i<inp.m_InputToOutputs.GetSize();i++) 
	{
		m_InputToOutputs.Add(new CInputToOutput(*inp.m_InputToOutputs[i]));
	}
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CInput::~CInput()
{
	m_InputToOutputs.DeleteAll();
}
//////////////////////////////////////////////////////////////////////////////
CInputToOutput*	CInput::AddInputToOutput()
{
	CInputToOutput* pInputToOutput;

	pInputToOutput = new CInputToOutput(this);
	m_InputToOutputs.Add(pInputToOutput);
	return pInputToOutput;
}
//////////////////////////////////////////////////////////////////////////////
void CInput::DeleteInputToOutput(CInputToOutput* pInputToOutput)
{
	int i,c;
	CInputToOutput* pSearch;

	c = m_InputToOutputs.GetSize();

	for (i=0;i<c;i++)
	{
		pSearch = m_InputToOutputs.GetAt(i);
		if (pSearch==pInputToOutput)
		{
			// found
			m_InputToOutputs.RemoveAt(i);
			WK_DELETE(pSearch);
			return;
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CInput::DeleteAllInputToOutputs()
{
	m_InputToOutputs.DeleteAll();
}
//////////////////////////////////////////////////////////////////////
CInputToOutput* CInput::GetInputToOutput(int n) const
{
	CInputToOutput* pIO = NULL;
	if ( n < m_InputToOutputs.GetSize() ) {
		pIO = m_InputToOutputs[n];
	}
	return pIO;
}
//////////////////////////////////////////////////////////////////////////////
void CInput::SetExtra(int i, DWORD dwExtra)
{
	m_dwExtras.SetAtGrow(i,dwExtra);
}
//////////////////////////////////////////////////////////////////////////////
BOOL CInput::Load(CWK_Profile& wkp)
{
	CString sSection;
	CString sInputDefine;
	int i;

	sSection.Format(_T("%s\\Group%04x"),SEC_NAME_INPUTGROUPS,m_id.GetGroupID());
	sInputDefine = wkp.GetString(sSection,m_id.GetSubID()+1,_T(""));

	if (sInputDefine.IsEmpty())
	{
		WK_TRACE_ERROR(_T("empty input define\n"));
		return FALSE;
	}

	// name
	m_sName = wkp.GetStringFromString(sInputDefine, INI_COMMENT,_T(""));

	int iNumExtra = wkp.GetNrFromString(sInputDefine, INI_COUNT_EXTRA, 0);
	for (i=0;i<iNumExtra;i++)
	{
		CString sKey;
		sKey.Format(INI_EXTRA_NO, i);
		m_dwExtras.Add(wkp.GetHexFromString(sInputDefine,sKey,0));
	}


	// status
	if (wkp.GetNrFromString(sInputDefine, INI_EDGE, 0)) 
	{
		m_wState |= I_STATE_EDGE; 
	}
	else 
	{
		m_wState &= ~I_STATE_EDGE; 
	}

	if (CanPushButton())
	{
		if (wkp.GetNrFromString(sInputDefine, INI_PUSHBUTTON, 0)) 
		{
			m_wState |= I_STATE_PUSHBUTTON; 
		}
		else 
		{
			m_wState &= ~I_STATE_PUSHBUTTON; 
		}
	}

	BOOL bAlarmOffSpans = TRUE;
	CWnd *pWnd = theApp.GetMainWnd();
	if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CMainFrame)))
	{
		CDocument*pDoc = ((CMainFrame*)pWnd)->GetActiveDocument();
		if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CSVDoc)))
		{
			bAlarmOffSpans = ((CSVDoc*)pDoc)->GetDongle()->AllowAlarmOffSpans();
		}
	}
	BOOL bTempDeactivate = TRUE;
	if (bAlarmOffSpans)
	{
		// temporary deactivation allowed
		bTempDeactivate = (BOOL)wkp.GetNrFromString(sInputDefine, INI_TEMP_DEACT, GetTempDeactivate());
	}
	SetTempDeactivate(bTempDeactivate);

	DWORD tmpDW = wkp.GetHexFromString(sInputDefine, INI_ACTIVE, SECID_ACTIVE_OFF);
	CSecID tmpID;
	if (tmpDW==SECID_NO_ID) tmpID=SECID_ACTIVE_OFF;
	else if (tmpDW==0) tmpID=SECID_ACTIVE_OFF;
	else if (tmpDW==1) tmpID=SECID_ACTIVE_ON;
	else if (tmpDW==2) tmpID=SECID_ACTIVE_LOCK;
	else tmpID = tmpDW;
	m_idActivate = tmpID;

	// get ip and ds parameter
	m_sType     = wkp.GetStringFromString(sInputDefine, INI_INPUT_TYPE, NULL);
	if (!m_sType.IsEmpty())
	{
		m_sUrl     = wkp.GetStringFromString(sInputDefine, INI_URL, NULL);
		m_uPort    = wkp.GetNrFromString(sInputDefine, INI_PORT, 0);
		m_nInputNr = wkp.GetNrFromString(sInputDefine, INI_INPUT_NR, 0);
	}

	// activations = CInputToOutput
	CString sInputToOutputFolder;
	sInputToOutputFolder.Format(_T("%s\\InputsToOutputs"),sSection);
	CString sInputToOutputEntry, sData;
	CInputToOutput* pInputToOutput;

	for (i=0 ; ; i++ ) 
	{
		sInputToOutputEntry.Format(_T("%d %d"),m_id.GetSubID(),i);
		sData = wkp.GetString(sInputToOutputFolder, sInputToOutputEntry, _T(""));
		if ( sData.IsEmpty() ) 
		{
			break;
		}
		else 
		{
			pInputToOutput = new CInputToOutput(this);
			pInputToOutput->m_idOutputOrMedia = (DWORD)wkp.GetHexFromString(sData, _T("\\O"), SECID_NO_ID);
			pInputToOutput->m_dwState = (DWORD)wkp.GetHexFromString(sData, _T("\\S"), IO_NOSTATE);
			pInputToOutput->m_dwPriority = (DWORD)wkp.GetHexFromString(sData, _T("\\P"), IO_DEFAULTPRIOR);
			pInputToOutput->m_idTimer  = (DWORD)wkp.GetHexFromString(sData, _T("\\T"), SECID_NO_ID);
			pInputToOutput->m_idProcess = (DWORD)wkp.GetHexFromString(sData, _T("\\M"), SECID_NO_ID);
			pInputToOutput->m_idNotification = (DWORD)wkp.GetHexFromString(sData, _T("\\NM"), SECID_NO_ID);
			pInputToOutput->m_PTZPreset = (CameraControlCmd)wkp.GetHexFromString(sData,_T("\\CCC"),CCC_INVALID);

			// HEDU 011098
			CSecID archiveID;
			archiveID = ( (DWORD)wkp.GetHexFromString(sData,_T("\\AR"), SECID_NO_ID ));
			if (archiveID!=SECID_NO_ID) {	// found some archive in an activation
				TRACE(_T("Found new style archive %08x\n"),archiveID.GetID());
				pInputToOutput->SetArchiveID(archiveID);
			}

			// NOT YET only for storing processes
			if (pInputToOutput->GetArchiveID()==SECID_NO_ID)
			{
				// read from the processs (pre 4.0 format)
				CArchivInfoArray aia;
				CProcessList pl;
				
				aia.Load(wkp);
				pl.Load(wkp);

				// extract the archiveIDs from the attached process
				CProcess*	pProcess = pl.GetProcess(pInputToOutput->m_idProcess);
				if (pProcess && pProcess->IsSave()) 
				{
					int iNumArchives = pProcess->GetNrOfArchiv();
					if (iNumArchives==0) {
						WK_TRACE_ERROR(_T("No archive in process %s\n"),pProcess->GetName());
					} else {
						// there are at least one archive
						CSecID archiveID;
						archiveID = CSecID(SECID_GROUP_ARCHIVE,pProcess->GetArchiv(0));
						pInputToOutput->SetArchiveID(archiveID);
						if (iNumArchives>1) {
							// more than one archive
							CString sDroppedArchives;
							// loop over all dropped archives
							for (int j=1 ; j<pProcess->GetNrOfArchiv(); j++) 
							{
								WORD wID = pProcess->GetArchiv(j);
								CArchivInfo *pArchive = aia.GetAt(wID);
								if (pArchive) 
								{	// exclude Suchergebn.
									if (sDroppedArchives.GetLength()) 
									{
										sDroppedArchives += _T("\n");
									}
									sDroppedArchives += pArchive->GetName();
								}
							}

							CString sMsg;
							sMsg.Format(IDP_MULTIPLE_ARCHIVES_IN_PROCESS,pProcess->GetName(),sDroppedArchives);
							WK_TRACE_ERROR(_T("%s\n"),sMsg);
							AfxMessageBox(sMsg, MB_OK);
						}
					}
				}
			}


			m_InputToOutputs.Add(pInputToOutput);
		}
	}


	CString sTemp;
	CString sValue;

	sTemp.Format(_T("\\%02d"),m_id.GetSubID()+1);
	sSection += sTemp;

	m_sInformation = wkp.GetString(sSection,_T("Information"),_T(""));

	if (!m_sInformation.IsEmpty())
	{
		m_sEditor = wkp.GetString(sSection,_T("Editor"),_T(""));

		CString sValue;
		DWORD dwT;
		sValue = wkp.GetString(sSection,_T("TimeStamp"),_T(""));
		if (!sValue.IsEmpty())
		{
			_stscanf(sValue,_T("%08lx"),&dwT);
			m_tInstructionStamp = (time_t)dwT;
			m_tInstructionStampNew = m_tInstructionStamp; 
		}
	}
	m_bDeleteInputToOutputs = FALSE;
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
BOOL CInput::Save(CWK_Profile& wkp)
{
	CString sSection;
	CString sInputDefine;
	int i,c;

	sSection.Format(_T("%s\\Group%04x"),SEC_NAME_INPUTGROUPS,m_id.GetGroupID());
	sInputDefine.Format(_T("%s%s%s%x%s%i%s%i"), 
					INI_COMMENT, m_sName,
					INI_ACTIVE, m_idActivate.GetID(), 
					INI_EDGE, GetEdge(),
					INI_TEMP_DEACT, GetTempDeactivate()
					);

	if (CanPushButton())
	{
		CString s;
		s.Format(_T("%s%i"), INI_PUSHBUTTON, GetPushButton());
		sInputDefine += s;
	}

	if (!m_sType.IsEmpty())
	{
		CString sKey;
		sKey.Format(_T("%s%s%s%s%s%d%s%d"),
			INI_INPUT_TYPE, m_sType,
			INI_URL, m_sUrl,
			INI_PORT, m_uPort,
			INI_INPUT_NR, m_nInputNr);
		sInputDefine += sKey;
	}

	if (m_dwExtras.GetSize())
	{
		CString sKey;
		sKey.Format(_T("%s%d"), INI_COUNT_EXTRA, m_dwExtras.GetSize());
		sInputDefine += sKey;
	}

	for (i=0;i<m_dwExtras.GetSize();i++)
	{
		CString sKey;
		sKey.Format(INI_EXTRA_NO _T("%08lx"), i, m_dwExtras[i]);
		sInputDefine += sKey;
	}

	wkp.WriteStringIndex(sSection,m_id.GetSubID()+1,sInputDefine);

	if (m_bDeleteInputToOutputs)
	{
		DeleteAllInputToOutputs();
		m_bDeleteInputToOutputs = FALSE;
	}

	for (i=m_InputToOutputs.GetSize()-1;i>=0;i--)
	{
		CInputToOutput* pInputToOutput = m_InputToOutputs.GetAtFast(i);
		if (pInputToOutput->m_bMarkForDelete)
		{
			WK_DELETE(pInputToOutput);
			m_InputToOutputs.RemoveAt(i);
		}
	}

	c = m_InputToOutputs.GetSize(); 

	if (c>0)
	{
		// activations = CInputToOutput
		CString sInputToOutputFolder;
		CString sInputToOutputEntry, sInputToOutputValue;
		CInputToOutput* pInputToOutput;

		sInputToOutputFolder.Format(_T("%s\\InputsToOutputs"),sSection);

		for (i=0;i<c;i++)
		{
			pInputToOutput = (CInputToOutput*)m_InputToOutputs.GetAt(i); 
			sInputToOutputEntry.Format(_T("%d %d"),m_id.GetSubID(),i);
			// only store if there is some data
			{
				sInputToOutputValue.Format(_T("\\O%x\\T%x\\M%x\\S%x\\P%x\\AR%x\\NM%x\\CCC%x"), 
					pInputToOutput->m_idOutputOrMedia.GetID(), 
					pInputToOutput->m_idTimer, 
					pInputToOutput->m_idProcess.GetID(), 
					pInputToOutput->m_dwState, 
					pInputToOutput->m_dwPriority,
					pInputToOutput->m_idArchive.GetID(),
					pInputToOutput->m_idNotification.GetID(),
					(DWORD)pInputToOutput->m_PTZPreset
					);	
				wkp.WriteString(sInputToOutputFolder,	sInputToOutputEntry, sInputToOutputValue);
			}
		}
	}
	

	if (!m_sInformation.IsEmpty())
	{
		CString sTemp;
		CString sValue;

		sTemp.Format(_T("\\%02d"),m_id.GetSubID()+1);
		sSection += sTemp;

		wkp.WriteString(sSection,_T("Editor"),m_sEditor);
		wkp.WriteString(sSection,_T("Information"),m_sInformation);

		sValue.Format(_T("%08lx"),(DWORD)m_tInstructionStampNew.GetTime());
		wkp.WriteString(sSection,_T("TimeStamp"),sValue);
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
void CInput::GenerateHTMLFile(CInputGroup* pInputGroup)
{
	CString sFileName;
	sFileName.Format(_T("%s\\%lx.htm"),theApp.m_sLocalWWW,m_id.GetID());

	if (   theApp.m_bUpdateLanguageDependencies == FALSE
		&& m_tInstructionStampNew == m_tInstructionStamp)
	{
		if (DoesFileExist(sFileName))
		{
			return;
		}
	}
	m_tInstructionStamp = m_tInstructionStampNew;

	CFile file;
	CFileException cfe;

	if (file.Open(sFileName,CFile::modeCreate|CFile::modeWrite, &cfe))
	{
		CWK_String sText;
		CString sL;
		CString sTime;

		sL.LoadString(IDS_INFORMATION);
		sL += _T(" ") + m_sName;
		sText.Format(HTML_TEXT_TOP, theApp.GetHTMLCodePage(), theApp.m_pszExeName, HTML_TYPE_INPUT, sL);
		sText.Write(file);

		sText.CleanUp();	// clean up before calling constructor!
		sText = _T("<body bgcolor=\"#C0C0C0\">") HTML_LINEFEED;
		sText.Write(file);

		sText.Format(_T("<h3>%s</h3>%s"),sL, HTML_LINEFEED);
		sText.Write(file);

		if (!m_sInformation.IsEmpty())
		{
			sText.Format(_T("%s<br>%s"),m_sInformation, HTML_LINEFEED);
			sText.Write(file);
		}
		
		sL.LoadString(IDS_EDITOR);
		sTime = m_tInstructionStamp.Format(_T("%d.%m.%Y %H:%M:%S"));
		sText.Format(sL, m_sEditor, sTime);
		sText.Write(file);

		sText.CleanUp();	// clean up before calling constructor!
		sText = _T("<hr>") HTML_LINEFEED;
		sText.Write(file);

		sText.Format(_T("<a href=\"%04lx.htm\">%s</a><br>%s"),
			m_id.GetGroupID(),pInputGroup->GetName(), HTML_LINEFEED);
		sText.Write(file);

		sText.CleanUp();	// clean up before calling constructor!
		sText = HTML_TEXT_BOTTOM;
		sText.Write(file);
	}
	else
	{
		WK_TRACE_ERROR(_T("Cannot open instruction file %s, %s\n"),
						sFileName, GetLastErrorString(cfe.m_lOsError));
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CInput::DeleteActivation(CSecID id)
{
	CInputToOutput* pSearch;
	BOOL bRet = FALSE;

	for (int i=0;i<m_InputToOutputs.GetSize();i++)
	{
		pSearch = m_InputToOutputs.GetAt(i);
		if (pSearch->GetProcessID() == id)
		{
			// found
			m_InputToOutputs.RemoveAt(i);
			WK_DELETE(pSearch);
			bRet = TRUE;
		}
	}
	return bRet;
}
