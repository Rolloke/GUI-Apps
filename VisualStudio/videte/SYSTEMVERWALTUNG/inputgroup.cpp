/* GlobalReplace: WK_GetProfileString --> wkp.GetString */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: inputgroup.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/inputgroup.cpp $
// CHECKIN:		$Date: 8.06.06 15:58 $
// VERSION:		$Revision: 40 $
// LAST CHANGE:	$Modtime: 8.06.06 15:32 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "InputGroup.h"

#include "globals.h"

///////////////////////////////////////////////////////////////////////////////////////
CInputGroup::CInputGroup()
{
	// for serialization only
}
///////////////////////////////////////////////////////////////////////////////////////
CInputGroup::CInputGroup(LPCTSTR pName, int iSize, LPCTSTR pSMName, CSecID SecID)
{
	CInput* pInput;
	WORD i;
	CString sName;
	m_GroupID		= SecID;	
	m_sSMName		= pSMName;
	if (pName == NULL)
	{
		CWK_Profile wkp;
		m_sName = GetGroupName(wkp, sName);
	}
	else
	{
		m_sName	= pName;
	}

	m_Array.SetSize(iSize);	
	for (i=0;i<m_Array.GetSize();i++) 
	{
		pInput = new CInput(m_GroupID.GetGroupID(),(WORD)i);
		sName.Format(_T("%s %02d"),m_sName,i+1);
		pInput->SetName(sName);
		if (m_sSMName==SM_SIMUNIT_INPUT)
		{
			pInput->SetEdge(TRUE);
		}
		if (0 == m_sSMName.Find(SM_SDIUNIT_INPUT))
		{
			pInput->SetCanEdge(FALSE);
		}
		m_Array.SetAt(i, pInput);
	}
}
///////////////////////////////////////////////////////////////////////////////////////
CInputGroup::~CInputGroup()
{
	Reset();
}
//////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CInputGroup::AssertValid() const
{
	CObject::AssertValid();
}

void CInputGroup::Dump(CDumpContext& dc) const
{
//	CObject::Dump(dc);
	dc << _T("CInputGroup ") << m_sName;
}
#endif //_DEBUG
///////////////////////////////////////////////////////////////////////////////////////
void CInputGroup::SetSize(WORD iSize)
{
	CString sName;
	CInput* pInput;
	WORD oldSize = (WORD)m_Array.GetSize();
	WORD i;

	if (iSize < oldSize)
	{
		for (i=(WORD)(oldSize-1);i>=iSize;i--)
		{
			pInput = (CInput*)m_Array.GetAt(i);
			WK_DELETE(pInput);
			m_Array.RemoveAt(i);
		}
	}
	else if (iSize > oldSize)
	{
		m_Array.SetSize(iSize);	
		for (i=oldSize;i<m_Array.GetSize();i++) 
		{
			pInput = new CInput(m_GroupID.GetGroupID(),(WORD)i);
			sName.Format(_T("%s %02d"),m_sName,i+1);
			pInput->SetName(sName);
			m_Array.SetAt(i, pInput);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////
BOOL CInputGroup::SetInput(CInput* pInput)
{
	int index = pInput->GetID().GetSubID();

	if (pInput && (index < m_Array.GetSize()))
	{
		CInput *pOldInput = (CInput*)m_Array.GetAt(index);
		if (pOldInput) 
		{ 
			*pOldInput = *pInput; // just copy the data
		}
		else 
		{
			WK_DELETE(pOldInput);
			m_Array.SetAt(index, pInput);
		}
		return TRUE;

	}
	return FALSE;
}
///////////////////////////////////////////////////////////////////////////////////////
void CInputGroup::Reset()
{
	int i,c;
	CInput* pInput;

	c = m_Array.GetSize();
	for (i=0;i<c;i++) 
	{
		pInput = (CInput*)m_Array.GetAt(i);
		WK_DELETE(pInput);

	} 
	m_Array.RemoveAll();
}
///////////////////////////////////////////////////////////////////////////////////////
CString CInputGroup::GetGroupName(CWK_Profile& wkp, const CString& sGroupDefine)
{
	CString sMelder,sName,sNr;
	int iNr = 0;
	
	
	sMelder.LoadString(IDS_ACTIVATION_INPUT);
	sNr = StringOnlyCharsInSet(m_sSMName,_T("0123456789"));
	_stscanf(sNr,_T("%d"),&iNr);
	if (iNr == 0) iNr++;

	if (m_sSMName == SM_SIMUNIT_INPUT)
	{
		sName = _T("SimUnit ")+sMelder;
	}
	else if (m_sSMName == SM_LAUNCHER_INPUT)
	{
		sName.LoadString(IDS_VIRTUAL_ALARM);
	}
	else if (0 == m_sSMName.Find(SM_TASHA_INPUT))
	{
		sName.Format(_T("Tasha%d %s"),iNr,sMelder);
	}
	else if (0 == m_sSMName.Find(SM_TASHA_MD_INPUT))
	{
		sName.Format(_T("Tasha%d MD %s"),iNr,sMelder);
	}
	else if (m_sSMName == SM_AKU_INPUT)
	{
		sName = _T("Relais Plus 8 ")+sMelder;
	}
	else if (m_sSMName == SM_COCO_INPUT0)
	{
		sName = _T("CoCo ")+sMelder;
	}
	else if (0 == m_sSMName.Find(SM_MICO_INPUT))
	{
		sName.Format(_T("JaCob%d %s"),iNr,sMelder);
	}
	else if (0 == m_sSMName.Find(SM_MICO_MD_INPUT))
	{
		sName.Format(_T("JaCob%d MD %s"),iNr,sMelder);
	}
	else if (0 == m_sSMName.Find(SM_SAVIC_INPUT))
	{
		sName.Format(_T("SaVic%d %s"),iNr,sMelder);
	}
	else if (0 == m_sSMName.Find(SM_SAVIC_MD_INPUT))
	{
		sName.Format(_T("SaVic%d MD %s"),iNr,sMelder);
	}
	else if (0 == m_sSMName.Find(SM_Q_INPUT))
	{
		sName.Format(_T("Q %s"),sMelder);
	}
	else if (0 == m_sSMName.Find(SM_Q_MD_INPUT))
	{
		sName.Format(_T("Q MD %s"),sMelder);
	}
	else if (0 == _tcsncmp(m_sSMName, SM_YUTA_INPUT, _tcslen(SM_YUTA_INPUT)-2))
	{
		sName.Format(IDS_YUTA_DETECTOR, iNr);
	}
	
	if (sName.IsEmpty())
	{
		ASSERT(!sGroupDefine.IsEmpty());
		sName = wkp.GetStringFromString(sGroupDefine, INI_COMMENT,_T(""));
	}

	return sName;
}
///////////////////////////////////////////////////////////////////////////////////////
void CInputGroup::GetVirtualNames(const CString& sVersion, CStringArray& sInputNameArray)
{
	CString sInputName;
	sInputName.LoadString(IDS_ARCHIVE_60_PERCENT_FULL);
	sInputNameArray.Add(sInputName);
	sInputName.LoadString(IDS_VA_ARCHIVE_FULL);
	sInputNameArray.Add(sInputName);
	sInputName.LoadString(IDS_VA_VIDEOSIGNAL);
	sInputNameArray.Add(sInputName);

	sInputName.LoadString(IDS_VA_OUTGOING_CALL_TCP);
	sInputNameArray.Add(sInputName);
	sInputName.LoadString(IDS_VA_INCOMING_CALL_TCP);
	sInputNameArray.Add(sInputName);
	sInputName.LoadString(IDS_VA_REJECT_TCP);
	sInputNameArray.Add(sInputName);

	sInputName.LoadString(IDS_VA_OUTGOING_CALL_ISDN1);
	sInputNameArray.Add(sInputName);
	sInputName.LoadString(IDS_VA_INCOMING_CALL_ISDN1);
	sInputNameArray.Add(sInputName);
	sInputName.LoadString(IDS_VA_REJECT_ISDN1);
	sInputNameArray.Add(sInputName);

	sInputName.LoadString(IDS_VA_OUTGOING_CALL_PT1);
	sInputNameArray.Add(sInputName);
	sInputName.LoadString(IDS_VA_INCOMING_CALL_PT1);
	sInputNameArray.Add(sInputName);
	sInputName.LoadString(IDS_VA_REJECT_PT1);
	sInputNameArray.Add(sInputName);

	sInputName.LoadString(IDS_VA_OUTGOING_CALL_ISDN2);
	sInputNameArray.Add(sInputName);
	sInputName.LoadString(IDS_VA_INCOMING_CALL_ISDN2);
	sInputNameArray.Add(sInputName);
	sInputName.LoadString(IDS_VA_REJECT_ISDN2);
	sInputNameArray.Add(sInputName);

	sInputName.LoadString(IDS_VA_OUTGOING_CALL_PT2);
	sInputNameArray.Add(sInputName);
	sInputName.LoadString(IDS_VA_INCOMING_CALL_PT2);
	sInputNameArray.Add(sInputName);
	sInputName.LoadString(IDS_VA_REJECT_PT2);
	sInputNameArray.Add(sInputName);

	// Folgende erst ab Version 4.1 >=
	if (sVersion>=_T("4.1"))
	{
		sInputName.LoadString(IDS_VA_CHECK_CALL_ERROR);
		sInputNameArray.Add(sInputName);

		sInputName.LoadString(IDS_VA_ALWAYS_ALARM);
		sInputNameArray.Add(sInputName);
	}
	// Folgende erst nach Version 4.1 >
	if (sVersion>_T("4.1"))
	{
		sInputName.LoadString(IDS_VA_CHECK_CALL_OK);
		sInputNameArray.Add(sInputName);
	}
	if (sVersion>_T("4.2"))
	{
		sInputName.LoadString(IDS_VA_ALARM_OFF_SPAN);
		sInputNameArray.Add(sInputName);
	}
	if (sVersion >= _T("4.5"))
	{
		sInputName.LoadString(IDS_VA_ALARM_EXTERNAL_DEVICE_ERROR);
		sInputNameArray.Add(sInputName);
	}

	// TODO! RKE: change to version 5.1
	if (sVersion >= _T("5.0.5")) 
	{
		sInputName.LoadString(IDS_VA_ALARM_TEMPERATURE);
		sInputNameArray.Add(sInputName);
		sInputName.LoadString(IDS_VA_ALARM_SMART);
		sInputNameArray.Add(sInputName);
		sInputName.LoadString(IDS_VA_ALARM_HARDDISK_FAILURE);
		sInputNameArray.Add(sInputName);
		sInputName.LoadString(IDS_VA_ALARM_FAN_SPEED);
		sInputNameArray.Add(sInputName);
	}
}
///////////////////////////////////////////////////////////////////////////////////////
BOOL CInputGroup::Load(int iNr, CWK_Profile& wkp)
{
	CString sGroupDefine;
	CInput*	pInput;  
	int i,c;
	WK_ApplicationId wai = WAI_INVALID;
	BOOL	bCanPushButton = FALSE;

	sGroupDefine = wkp.GetString(SEC_NAME_INPUTGROUPS,iNr,_T(""));

// number of inputs
	c = (int)wkp.GetNrFromString(sGroupDefine, INI_NR, 0L);
	if (c<0) 
	{
		return FALSE;
	}

	// name and shared memory name
	m_sSMName = wkp.GetStringFromString(sGroupDefine, INI_SMNAME, _T(""));
	m_sName = GetGroupName(wkp,sGroupDefine);

	// SecID
	WORD wID = (WORD)wkp.GetHexFromString(sGroupDefine, INI_ID, -1);	
	m_GroupID.Set(wID,0);

	// inputs
	CStringArray sInputNameArray;
	if (m_sSMName == SM_LAUNCHER_INPUT)
	{
		CString sVersion = wkp.GetString(_T("Version"),_T("Number"),_T(""));
		GetVirtualNames(sVersion,sInputNameArray);
		wai = WAI_LAUNCHER;
		if (sInputNameArray.GetCount() < c)
		{
			c = sInputNameArray.GetCount();
		}
	}
	else if (m_sSMName == SM_ICPCONUnitInput)
	{
		bCanPushButton = TRUE;
	}

	CString sName;
	for (i=0;i<c;i++)
	{
		pInput = new CInput(wID,(WORD)(i));
		sName.Format(_T("%s %02d"),m_sName,i+1);
		if (bCanPushButton)
		{
			pInput->SetCanPushButton(TRUE);
		}
		pInput->Load(wkp);
		m_Array.Add(pInput);
		if (   pInput->GetIDActivate() == SECID_ACTIVE_OFF
			&& pInput->GetName().IsEmpty())
		{
			pInput->SetName(sName);
		}
		if (wai == WAI_LAUNCHER)
		{
			pInput->SetName(sInputNameArray[i]);
		}
		else
		{
			if (pInput->GetIDActivate() == SECID_ACTIVE_OFF)
			{
				if (pInput->GetName().IsEmpty())
				{
					pInput->SetName(sName);
				}
			}
		}
//		if (   wai == WAI_TASHAUNIT_1		// Tascha Detector
//			&& i == c-1)					// the last one
//		{
//			if (pInput->GetIDActivate() == SECID_ACTIVE_OFF)
//			{
//				sName.LoadString(IDS_)
//				pInput->SetName(sName);
//			}
//		}
	}
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////
BOOL CInputGroup::Save(int iNr,CWK_Profile& wkp)
{
	int i,c;
	CInput* pInput;
	// store group record in _T("InputGroups")
	CString sGroupEntry;
	sGroupEntry.Format(_T("%s%x%s%d%s%s%s%s"),
						INI_ID     , m_GroupID.GetGroupID(),
						INI_NR     , m_Array.GetSize(),
						INI_SMNAME , m_sSMName,
						INI_COMMENT, m_sName);

	wkp.WriteStringIndex(SEC_NAME_INPUTGROUPS, iNr, sGroupEntry); 

	c = m_Array.GetSize();
	for (i=0;i<c;i++)
	{
		pInput = (CInput*)m_Array.GetAt(i);
		if (pInput)
		{
			pInput->Save(wkp);
		}
	}

	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////
void CInputGroup::GenerateHTMLFile()
{
	CString sFileName;
	CStdioFile file;
	CFileException cfe;

	sFileName.Format(_T("%s\\%04lx.htm"),theApp.m_sLocalWWW,m_GroupID.GetGroupID());

	if (file.Open(sFileName,CFile::modeCreate|CFile::modeWrite,&cfe))
	{
		CWK_String sText;
		CString sL;
		CString sTime;

		sL.LoadString(IDS_INPUTGROUP);
		sL += _T(" ") + m_sName;
		sText.Format(HTML_TEXT_TOP, theApp.GetHTMLCodePage(), theApp.m_pszExeName, HTML_TYPE_INPUTS, sL);
		sText.Write(file);

		// Text
		sText.CleanUp();	// clean up before calling constructor!
		sText = _T("<body bgcolor=\"#C0C0C0\">") HTML_LINEFEED;
		sText.Write(file);

		sText.Format(_T("<h3>%s</h3>%s"),sL, HTML_LINEFEED);
		sText.Write(file);

		CInput* pInput;
		int i,c;
		c = m_Array.GetSize();
		for (i=0;i<c;i++)
		{
			pInput = (CInput*)m_Array.GetAt(i);
			if (pInput)
			{
				sText.Format(_T("<a href=\"%08lx.htm\"> %s </a><br>%s"),
					pInput->GetID().GetID(), pInput->GetName(), HTML_LINEFEED);
				sText.Write(file);
				pInput->GenerateHTMLFile(this);
			}
		}

		sText.CleanUp();	// clean up before calling constructor!
		sText = _T("<hr>") HTML_LINEFEED;
		sText.Write(file);

		sL.LoadString(IDS_INPUT);
		sText.Format(_T("<a href=\"input.htm\"> %s </a><br>%s"), sL, HTML_LINEFEED);
		sText.Write(file);

		sText.CleanUp();	// clean up before calling constructor!
		sText = HTML_TEXT_BOTTOM;
		sText.Write(file);
	}
	else
	{
		WK_TRACE_ERROR(_T("Cannot open group file %s, %s\n"),
						sFileName, GetLastErrorString(cfe.m_lOsError));
	}
}
///////////////////////////////////////////////////////////////////////////////////////
int CInputGroup::GetNrOfActiveInputs() const
{
	int i,c,r;
	CInput* pInput;
	c = m_Array.GetSize();
	r = 0;
	for (i=0;i<c;i++)
	{
		pInput = (CInput*)m_Array.GetAt(i);
		if (pInput && (pInput->GetIDActivate()!=SECID_ACTIVE_OFF))
		{
			r++;
		}
	}
	return r;
}
///////////////////////////////////////////////////////////////////////////////////////
BOOL CInputGroup::IsMD() const
{
	if (   m_sSMName == SM_TASHA_MD_INPUT
		|| m_sSMName == SM_TASHA_MD_INPUT2
		|| m_sSMName == SM_TASHA_MD_INPUT3
		|| m_sSMName == SM_TASHA_MD_INPUT4
		|| m_sSMName == SM_MICO_MD_INPUT
		|| m_sSMName == SM_MICO_MD_INPUT2
		|| m_sSMName == SM_MICO_MD_INPUT3
		|| m_sSMName == SM_MICO_MD_INPUT4
		|| m_sSMName == SM_SAVIC_MD_INPUT
		|| m_sSMName == SM_SAVIC_MD_INPUT2
		|| m_sSMName == SM_SAVIC_MD_INPUT3
		|| m_sSMName == SM_SAVIC_MD_INPUT4
		)
	{
		return TRUE;
	}
	return FALSE;
}
///////////////////////////////////////////////////////////////////////////////////////
BOOL CInputGroup::DeleteActivation(CSecID id)
{
	BOOL ret = FALSE;
	CInput* pInput;
	for (int i=0;i<m_Array.GetSize();i++)
	{
		pInput = (CInput*)m_Array.GetAt(i);
		if (pInput)
		{
			ret |= pInput->DeleteActivation(id);
		}
	}
	return ret;
}
