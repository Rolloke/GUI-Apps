/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: outputlist.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/outputlist.cpp $
// CHECKIN:		$Date: 10.05.06 17:42 $
// VERSION:		$Revision: 19 $
// LAST CHANGE:	$Modtime: 10.05.06 17:39 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "OutputGroup.h"
#include "OutputList.h"

#include "globals.h"

/////////////////////////////////////////////////////////////////////////////
// COutputList
COutputList::COutputList()
{
}
/////////////////////////////////////////////////////////////////////////////
COutputList::~COutputList()
{
	Reset();
}
/////////////////////////////////////////////////////////////////////////////
void COutputList::Reset()
{
	int i,c;
	COutputGroup* pOutputGroup;

	c = m_Array.GetSize();
	for (i=0;i<c;i++) 
	{
		pOutputGroup = (COutputGroup*)m_Array.GetAt(i);
		WK_DELETE(pOutputGroup);

	} 
	m_Array.RemoveAll();
}
/////////////////////////////////////////////////////////////////////////////
void COutputList::Load(CWK_Profile& wkp)
{
	int i,c;
	COutputGroup* pOutputGroup;

	Reset();

	c = wkp.GetInt(SEC_NAME_OUTPUTGROUPS,ENT_NAME_MAXGROUPS,0);
	
	for (i=0;i<c;i++)
	{
		pOutputGroup = new COutputGroup();
		if (pOutputGroup->Load(i+1,wkp))
		{
			m_Array.Add(pOutputGroup);
		}
		else
		{
			delete pOutputGroup;
		}
	}

	// Störungsmelder
	BOOL bSTM = wkp.GetInt(theApp.GetLauncherSection(), _T("UseRelais4"),FALSE);
	pOutputGroup = GetGroupBySMName(SM_COCO_OUTPUT_RELAY0);
	if ( pOutputGroup ) 
	{
		COutput* pOutput = pOutputGroup->GetOutput(3);
		if (pOutput)
		{
			pOutput->SetSTM(bSTM);
		}
	}
	else
	{
		pOutputGroup = GetGroupBySMName(SM_MICO_OUTPUT_RELAYS);
		if ( pOutputGroup ) 
		{
			COutput* pOutput = pOutputGroup->GetOutput(3);
			if (pOutput)
			{
				pOutput->SetSTM(bSTM);
			}
		}
	}

}
/////////////////////////////////////////////////////////////////////////////
COutputGroup* COutputList::AddOutputGroup(LPCTSTR pName, int iNr, LPCTSTR pSMName)
{
	COutputGroup* pOutputGroup;
	CSecID id(SECID_GROUP_OUTPUT+1,0);

	BOOL bLoop = TRUE;
	while (bLoop)
	{
		pOutputGroup = GetGroupByID(id);
		if (!pOutputGroup)
		{
			break;
		}
		id.SetGroupID((WORD)(id.GetGroupID()+1)); 
	}
	
	pOutputGroup = new COutputGroup(pName,iNr,pSMName,id);
	m_Array.Add(pOutputGroup);

	return pOutputGroup;
}
/////////////////////////////////////////////////////////////////////////////
void COutputList::Save(CWK_Profile& wkp, BOOL bGenerateHTML)
{
	int i,c;
	COutputGroup* pOutputGroup;
	
	wkp.DeleteSection(SEC_NAME_OUTPUTGROUPS);

	CString sSNKey;
	int j;
	DWORD dwComMask;
	DWORD dwBit = 1;
	dwComMask = wkp.GetInt(SECTION_COMMON,COMMON_COMM_PORT_INFO,0);
	for (dwBit = 1,j=1; dwBit; dwBit<<=1,j++)
	{
		if (dwComMask & dwBit)
		{
			sSNKey.Format(_T("COM%d"),j);
			wkp.DeleteSubSection(_T("CommUnit"),sSNKey);
		}
	}
	c = m_Array.GetSize();
	wkp.WriteInt(SEC_NAME_OUTPUTGROUPS,ENT_NAME_MAXGROUPS,c);

	for (i=0;i<c;i++)
	{
		pOutputGroup = (COutputGroup*)m_Array.GetAt(i);
		if (pOutputGroup)
			pOutputGroup->Save(i+1,wkp);
	}

	if (bGenerateHTML)
	{
		GenerateHTMLFile();
	}
}
/////////////////////////////////////////////////////////////////////////////
COutputGroup* COutputList::GetGroupByName(const CString& sName) const
{ 
	COutputGroup* pOutputGroup;

	for (int iGrpNr=0;iGrpNr<GetSize();iGrpNr++) 
	{
		pOutputGroup = GetGroupAt(iGrpNr);
		if(sName == pOutputGroup->GetName())
		{
			return pOutputGroup;		// EXIT
		}
	}
	return NULL; 
}
//////////////////////////////////////////////////////////////////////////////
COutputGroup* COutputList::GetGroupByID(CSecID id) const
{
	for (int i=0;i<GetSize();i++) 
	{
		if (id.GetGroupID()==GetGroupAt(i)->GetID().GetGroupID()) 
		{
			return (GetGroupAt(i));
		}
	}
	return (NULL);
}
//////////////////////////////////////////////////////////////////////////////
const COutput * COutputList::GetOutputByID(CSecID id) const
{
	if (id==SECID_NO_ID)
	{
		return NULL;
	}
	for (int i=0;i<GetSize();i++) 
	{
		if (id.GetGroupID()==GetGroupAt(i)->GetID().GetGroupID()) 
		{
			return ( GetGroupAt(i)->GetOutput(id.GetSubID()) );
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
COutputGroup* COutputList::GetGroupBySMName(const CString& sSMName) const
{
	COutputGroup* pOutputGroup;

	for (int iGrpNr=0;iGrpNr<GetSize();iGrpNr++) 
	{
		pOutputGroup = GetGroupAt(iGrpNr);
		if(sSMName == pOutputGroup->GetSMName())
		{
			return pOutputGroup;		// EXIT
		}
	}
	return NULL; 
}
/////////////////////////////////////////////////////////////////////////////
BOOL COutputList::DeleteGroup(const CString& sSMName)
{
	COutputGroup* pOutputGroup;

	for (int iGrpNr=0;iGrpNr<GetSize();iGrpNr++) 
	{
		pOutputGroup = GetGroupAt(iGrpNr);
		if(sSMName == pOutputGroup->GetSMName())
		{
			WK_DELETE(pOutputGroup);
			m_Array.RemoveAt(iGrpNr);
			return TRUE; // EXIT
		}
	}
	return FALSE; 
}
/////////////////////////////////////////////////////////////////////////////
void COutputList::GenerateHTMLFile()
{
	CString sFileName;
	CFile file;
	CFileException cfe;

	CString sDir = CNotificationMessage::GetWWWRoot();
	CString sHost,sPath;
	sHost.Format(_T("%08lx"),SECID_LOCAL_HOST);
	sFileName = sDir + _T('\\') + sHost + _T('\\') + _T("output.htm");

	if (file.Open(sFileName,CFile::modeCreate|CFile::modeWrite,&cfe))
	{
		CWK_String sText;
		CString sL;
		CString sTime;

		sL.LoadString(IDS_OUTPUT);
		sText.Format(HTML_TEXT_TOP, theApp.GetHTMLCodePage(), theApp.m_pszExeName, HTML_TYPE_CAMERAS, sL);
		sText.Write(file);

		sText.CleanUp();	// clean up before calling constructor!
		sText = _T("<body bgcolor=\"#C0C0C0\">") HTML_LINEFEED;
		sText.Write(file);

		sText.Format(_T("<h3>%s</h3>%s"), sL, HTML_LINEFEED);
		sText.Write(file);

		COutputGroup* pOutputGroup;
		int i,c;
		c = m_Array.GetSize();
		for (i=0;i<c;i++)
		{
			pOutputGroup = (COutputGroup*)m_Array.GetAt(i);
			if (pOutputGroup && pOutputGroup->HasCameras())
			{
				sText.Format(_T("<a href=\"%04lx.htm\"> %s </a><br>%s"),
					pOutputGroup->GetID().GetGroupID(), pOutputGroup->GetName(), HTML_LINEFEED);
				sText.Write(file);
				pOutputGroup->GenerateHTMLFile();
			}
		}

		sText.CleanUp();	// clean up before calling constructor!
		sText = HTML_TEXT_BOTTOM;
		sText.Write(file);
	}
	else
	{
		WK_TRACE_ERROR(_T("Cannot open input file %s, %s\n"),
						sFileName, GetLastErrorString(cfe.m_lOsError));
	}
}

