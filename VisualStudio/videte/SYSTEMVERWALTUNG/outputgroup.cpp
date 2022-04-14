/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: outputgroup.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/outputgroup.cpp $
// CHECKIN:		$Date: 17.08.06 8:19 $
// VERSION:		$Revision: 30 $
// LAST CHANGE:	$Modtime: 16.08.06 15:11 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "OutputGroup.h"

#include "globals.h"

#define TYPE_RELAY	1
#define TYPE_CAMERA	2
///////////////////////////////////////////////////////////////////////////////////////
COutputGroup::COutputGroup()
{
	m_nType = 0;
}
///////////////////////////////////////////////////////////////////////////////////////
COutputGroup::COutputGroup(LPCTSTR pName, int iSize, LPCTSTR pSMName, CSecID SecID)
{
	COutput* pOutput;
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
	m_nType = 0;
	CheckType();

	m_Array.SetSize(iSize);	
	for (i=0;i<m_Array.GetSize();i++) 
	{
		pOutput = new COutput(m_GroupID.GetGroupID(),(WORD)i);
		sName.Format(_T("%s %02d"),m_sName,i+1);
		pOutput->SetName(sName);
		m_Array.SetAt(i, pOutput);
	}
}
///////////////////////////////////////////////////////////////////////////////////////
COutputGroup::~COutputGroup()
{
	Reset();
}
///////////////////////////////////////////////////////////////////////////////////////
BOOL COutputGroup::HasCameras()
{
	return m_nType == TYPE_CAMERA ? TRUE : FALSE;
}
///////////////////////////////////////////////////////////////////////////////////////
BOOL COutputGroup::HasRelais()
{
	return m_nType == TYPE_RELAY ? TRUE : FALSE;
}
///////////////////////////////////////////////////////////////////////////////////////
void COutputGroup::CheckType()
{
	if (-1 != m_sSMName.Find(SM_IP_CAMERA_OUTPUT_RELAY))
	{
		m_nType = TYPE_RELAY;
	}
	else if (-1!=m_sSMName.Find(_T("amera")))
	{
		m_nType = TYPE_CAMERA;
	}
	else if (-1 != m_sSMName.Find(_T("elay")))
	{
		m_nType = TYPE_RELAY;
	}
	else if (-1 != m_sSMName.Find(SM_ICPCONUnitOutput))
	{
		m_nType = TYPE_RELAY;
	}
	else if (-1 != m_sSMName.Find(SM_SIMUNIT_OUTPUT))
	{
		m_nType = TYPE_RELAY;
	}
	else if (0 == _tcsncmp(m_sSMName, SM_YUTA_OUTPUT, _tcslen(SM_YUTA_OUTPUT)-2))
	{
		m_nType = TYPE_RELAY;
	}
}
///////////////////////////////////////////////////////////////////////////////////////
BOOL COutputGroup::CanVideoOut()
{
	// Until now (22.07.2003) all camera cards have video out
	return m_nType == TYPE_CAMERA ? TRUE : FALSE;
//	if (-1!=m_sSMName.Find(_T("amera")))
//	{
//		return TRUE;
//	}
//	return FALSE;
}
///////////////////////////////////////////////////////////////////////////////////////
void COutputGroup::SetSize(WORD iSize)
{
	CString sName;
	COutput* pOutput;
	WORD oldSize = (WORD)m_Array.GetSize();
	WORD i;

	if (iSize < oldSize)
	{
		for (i=(WORD)(oldSize-1);i>=iSize;i--)
		{
			pOutput = (COutput*)m_Array.GetAt(i);
			WK_DELETE(pOutput);
			m_Array.RemoveAt(i);
		}
	}
	else if (iSize > oldSize)
	{
		m_Array.SetSize(iSize);	
		for (i=oldSize;i<m_Array.GetSize();i++) 
		{
			pOutput = new COutput(m_GroupID.GetGroupID(),(WORD)i);
			sName.Format(_T("%s %02d"),m_sName,i+1);
			pOutput->SetName(sName);
			m_Array.SetAt(i, pOutput);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////
BOOL COutputGroup::SetOutput(COutput* pOutput)
{
	int index = pOutput->GetID().GetSubID();

	if (pOutput && (index < m_Array.GetSize()))
	{
		COutput *pOldOutput = (COutput*)m_Array.GetAt(index);
		if (pOldOutput) 
		{ 
			*pOldOutput = *pOutput; // just copy the data
		}
		else 
		{
			WK_DELETE(pOldOutput);
			m_Array.SetAt(index, pOutput);
		}
		return TRUE;

	}
	return FALSE;
}
///////////////////////////////////////////////////////////////////////////////////////
void COutputGroup::Reset()
{
	int i,c;
	COutput* pOutput;

	c = m_Array.GetSize();
	for (i=0;i<c;i++) 
	{
		pOutput = (COutput*)m_Array.GetAt(i);
		WK_DELETE(pOutput);

	} 
	m_Array.RemoveAll();
}
///////////////////////////////////////////////////////////////////////////////////////
CString COutputGroup::GetGroupName(CWK_Profile& wkp, const CString& sGroupDefine)
{
	CString sCamera,sRelais,sName,sNr;
	int iNr = 0;

	sCamera.LoadString(IDS_ACTIVATION_OUTPUT);
	sRelais.LoadString(IDS_RELAY);

	sNr = StringOnlyCharsInSet(m_sSMName,_T("0123456789"));
	_stscanf(sNr,_T("%d"),&iNr);
	if (iNr == 0) 
	{
		iNr++;
	}

	if (m_sSMName == SM_SIMUNIT_OUTPUT)
	{
		sName = _T("SimUnit ")+sRelais;
	}
	else if (0 == m_sSMName.Find(SM_TASHA_OUTPUT_CAMERAS))
	{
		sName.Format(_T("Tasha%d %s"),iNr,sCamera);
	}
	else if (0 == m_sSMName.Find(SM_TASHA_OUTPUT_RELAYS))
	{
		sName.Format(_T("Tasha%d %s"),iNr,sRelais);
	}
	else if (m_sSMName == SM_AKU_OUTPUT)
	{
		sName = _T("Relais Plus 8 ");
	}
	else if (m_sSMName == SM_COCO_OUTPUT_CAM0)
	{
		sName = _T("CoCo ")+sCamera;
	}
	else if (m_sSMName == SM_COCO_OUTPUT_RELAY0)
	{
		sName = _T("CoCo ")+sRelais;
	}
	else if (0 == m_sSMName.Find(SM_MICO_OUTPUT_CAMERAS))
	{
		sName.Format(_T("JaCob%d %s"),iNr,sCamera);
	}
	else if (0 == m_sSMName.Find(SM_MICO_OUTPUT_RELAYS))
	{
		sName.Format(_T("JaCob%d %s"),iNr,sRelais);
	}
	else if (0 == m_sSMName.Find(SM_SAVIC_OUTPUT_CAMERAS))
	{
		sName.Format(_T("SaVic%d %s"),iNr,sCamera);
	}
	else if (0 == m_sSMName.Find(SM_SAVIC_OUTPUT_RELAYS))
	{
		sName.Format(_T("SaVic%d %s"),iNr,sRelais);
	}
	else if (0 == m_sSMName.Find(SM_Q_OUTPUT_CAMERA))
	{
		sName.Format(_T("Q %s"),sCamera);
	}
	else if (0 == m_sSMName.Find(SM_Q_OUTPUT_RELAY))
	{
		sName.Format(_T("Q %s"),sRelais);
	}
	else if (0 == _tcsncmp(m_sSMName, SM_YUTA_OUTPUT, _tcslen(SM_YUTA_OUTPUT)-2))
	{
		sName.Format(IDS_YUTA_RELAIS, iNr);
	}

	if (sName.IsEmpty())
	{
		ASSERT(!sGroupDefine.IsEmpty());
		sName = wkp.GetStringFromString(sGroupDefine, INI_COMMENT,_T(""));
	}

	return sName;
}
///////////////////////////////////////////////////////////////////////////////////////
BOOL COutputGroup::Load(int iNr,CWK_Profile& wkp)
{
	CString sGroupDefine;
	COutput*	pOutput;  
	int i,c;

	sGroupDefine = wkp.GetString(SEC_NAME_OUTPUTGROUPS,iNr,_T(""));

	// number of inputs
	c = (int)wkp.GetNrFromString(sGroupDefine, INI_NR, 0L);
	if (c<0) 
	{
		return FALSE;
	}

	// name and shared memory name
	m_sSMName = wkp.GetStringFromString(sGroupDefine, INI_SMNAME, _T(""));	
	m_sName = GetGroupName(wkp,sGroupDefine);
	CheckType();
	// SecID
	WORD wID = (WORD)wkp.GetHexFromString(sGroupDefine, INI_ID, -1);	
	m_GroupID.Set(wID,0);

	// outputs
	CString sName;
	for (i=0;i<c;i++)
	{
		pOutput = new COutput(wID,(WORD)(i));
		sName.Format(_T("%s %02d"),m_sName,i+1);
		if (pOutput->Load(wkp))
		{
			m_Array.Add(pOutput);
		}
		else
		{
			pOutput->SetName(sName);
			m_Array.Add(pOutput);
		}
		if (pOutput->GetExOutputType() == EX_OUTPUT_OFF)
		{
			pOutput->SetName(sName);
		}
	}
	
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////
BOOL COutputGroup::Save(int iNr,CWK_Profile& wkp)
{
	int i,c;
	COutput* pOutput;
	// store group record in _T("OutputGroups")
	CString sGroupEntry;
	sGroupEntry.Format(_T("%s%x%s%d%s%s%s%s"),
						INI_ID     , m_GroupID.GetGroupID(),
						INI_NR     , m_Array.GetSize(),
						INI_SMNAME , m_sSMName,
						INI_COMMENT, m_sName);

	wkp.WriteStringIndex(SEC_NAME_OUTPUTGROUPS, iNr, sGroupEntry); 

	c = m_Array.GetSize();
	for (i=0;i<c;i++)
	{
		pOutput = (COutput*)m_Array.GetAt(i);
		if (pOutput)
			pOutput->Save(wkp);
	}
	return TRUE;

}
///////////////////////////////////////////////////////////////////////////////////////
void COutputGroup::GenerateHTMLFile()
{
	CString sFileName;
	CFile file;
	CFileException cfe;

	CString sDir = CNotificationMessage::GetWWWRoot();
	CString sHost,sGroup,sPath;
	sHost.Format(_T("%08lx"),SECID_LOCAL_HOST);
	sGroup.Format(_T("%04lx"),m_GroupID.GetGroupID());
	sFileName = sDir + _T('\\') + sHost + _T('\\') + sGroup + _T(".htm");

	if (file.Open(sFileName,CFile::modeCreate|CFile::modeWrite,&cfe))
	{
		CWK_String sText;
		CString sL;
		CString sTime;

		sText.Format(HTML_TEXT_TOP, theApp.GetHTMLCodePage(), theApp.m_pszExeName, HTML_TYPE_CAMERAS, m_sName);
		sText.Write(file);

		sText.CleanUp();	// clean up before calling constructor!
		sText = _T("<body bgcolor=\"#C0C0C0\">") HTML_LINEFEED;
		sText.Write(file);

		sText.Format(_T("<h3>%s</h3>%s"), m_sName, HTML_LINEFEED);
		sText.Write(file);

		COutput* pOutput;
		int i,c;
		c = m_Array.GetSize();
		for (i=0;i<c;i++)
		{
			pOutput = (COutput*)m_Array.GetAt(i);
			if (pOutput==NULL)
			{
				continue;
			}
			if (   pOutput->GetExOutputType() == EX_OUTPUT_CAMERA_FBAS
				|| pOutput->GetExOutputType() == EX_OUTPUT_CAMERA_SVHS)
			{
				sText.Format(_T("<a href=\"%08lx.htm\"> %s </a><br>%s"),
					pOutput->GetID().GetID(), pOutput->GetName(), HTML_LINEFEED);
				sText.Write(file);
				pOutput->GenerateHTMLFileCamera(this);
			}
		}

		sText.CleanUp();	// clean up before calling constructor!
		sText = _T("<hr>") HTML_LINEFEED;
		sText.Write(file);

		sL.LoadString(IDS_OUTPUT);
		sText.Format(_T("<a href=\"output.htm\"> %s </a><br>%s"), sL, HTML_LINEFEED);
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
