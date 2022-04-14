/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: inputlist.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/inputlist.cpp $
// CHECKIN:		$Date: 22.06.05 13:01 $
// VERSION:		$Revision: 18 $
// LAST CHANGE:	$Modtime: 22.06.05 9:39 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "SystemVerwaltung.h"

#include "InputGroup.h"
#include "InputList.h"
#include "globals.h"

//////////////////////////////////////////////////////////////////////////////
// CInputList
/////////////////////////////////////////////////////////////////////////////
CInputList::CInputList()
{
}
/////////////////////////////////////////////////////////////////////////////
CInputList::~CInputList()
{
	Reset();
}
//////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CInputList::AssertValid() const
{
	CObject::AssertValid();
}

void CInputList::Dump(CDumpContext& dc) const
{
//	CObject::Dump(dc);
	dc << _T("CInputList ") << m_Array.GetSize();
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
CInputGroup* CInputList::AddInputGroup(LPCTSTR pName, int iNr, LPCTSTR pSMName)
{
	CInputGroup* pInputGroup;
	CSecID id(SECID_GROUP_INPUT+1,0);

	int i=1;
	while (i)
	{
		pInputGroup = GetGroupByID(id);
		if (!pInputGroup)
			break;
		id.SetGroupID((WORD)(id.GetGroupID()+1)); 
	}
	
	pInputGroup = new CInputGroup(pName,iNr, pSMName,id);
	m_Array.Add(pInputGroup);

	return pInputGroup;
}
/////////////////////////////////////////////////////////////////////////////
CInputGroup* CInputList::GetGroupByName(const CString& sName) const
{ 
	CInputGroup* pInputGroup;

	for (int iGrpNr=0;iGrpNr<GetSize();iGrpNr++) 
	{
		pInputGroup = GetGroupAt(iGrpNr);
		if(sName == pInputGroup->GetName())
		{
			return pInputGroup;		// EXIT
		}
	}
	return NULL; 
}
/////////////////////////////////////////////////////////////////////////////
CInputGroup* CInputList::GetGroupBySMName(const CString& sSMName) const
{
	CInputGroup* pInputGroup;

	for (int iGrpNr=0;iGrpNr<GetSize();iGrpNr++) 
	{
		pInputGroup = GetGroupAt(iGrpNr);
		if(sSMName == pInputGroup->GetSMName())
		{
			return pInputGroup;		// EXIT
		}
	}
	return NULL; 
}
/////////////////////////////////////////////////////////////////////////////
BOOL CInputList::DeleteGroup(const CString& sSMName,BOOL bExactMatch/*=TRUE*/)
{
	CInputGroup* pInputGroup = NULL;

	for (int iGrpNr=GetSize()-1;iGrpNr>=0;iGrpNr--) 
	{
		pInputGroup = GetGroupAt(iGrpNr);
		if((   sSMName == pInputGroup->GetSMName())
			|| (!bExactMatch && -1!=pInputGroup->GetSMName().Find(sSMName))
			)
		{
			WK_DELETE(pInputGroup);
			m_Array.RemoveAt(iGrpNr);
		}
	}
	return FALSE; 
}
/////////////////////////////////////////////////////////////////////////////
void CInputList::Reset()
{
	int i,c;
	CInputGroup* pInputGroup;

	c = m_Array.GetSize();
	for (i=0;i<c;i++) 
	{
		pInputGroup = (CInputGroup*)m_Array.GetAt(i);
		WK_DELETE(pInputGroup);

	} 
	m_Array.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
void CInputList::Load(CWK_Profile& wkp)
{
	int i,c;
	CInputGroup* pInputGroup;

	Reset();

	c = wkp.GetInt(SEC_NAME_INPUTGROUPS,ENT_NAME_MAXGROUPS,0);
	
	for (i=0;i<c;i++)
	{
		pInputGroup = new CInputGroup();
		if (pInputGroup->Load(i+1,wkp))
		{
			m_Array.Add(pInputGroup);
		}
		else
		{
			delete pInputGroup;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CInputList::Save(CWK_Profile& wkp, BOOL bGenerateHTML)
{
	int i,c;
	CInputGroup* pInputGroup;
	
	wkp.DeleteSection(SEC_NAME_INPUTGROUPS);

	c = m_Array.GetSize();
	wkp.WriteInt(SEC_NAME_INPUTGROUPS,ENT_NAME_MAXGROUPS,c);

	for (i=0;i<c;i++)
	{
		pInputGroup = (CInputGroup*)m_Array.GetAt(i);
		if (pInputGroup)
		{
			pInputGroup->Save(i+1,wkp);
		}
	}
	if (bGenerateHTML)
	{
		GenerateHTMLFile();
	}
}
//////////////////////////////////////////////////////////////////////////////
void CInputList::GenerateHTMLFile()
{
	CString sFileName;
	CFile file;
	CFileException cfe;

	sFileName.Format(_T("%s\\input.htm"),theApp.m_sLocalWWW);

	if (file.Open(sFileName,CFile::modeCreate|CFile::modeWrite,&cfe))
	{
		CWK_String sText;
		CString sL;
		CString sTime;

		sL.LoadString(IDS_INPUT);
		sText.Format(HTML_TEXT_TOP, theApp.GetHTMLCodePage(), theApp.m_pszExeName, HTML_TYPE_INPUTS, sL);
		sText.Write(file);

		sText.CleanUp();	// clean up before calling constructor!
		sText = _T("<body bgcolor=\"#C0C0C0\">") HTML_LINEFEED;
		sText.Write(file);

		sText.Format(_T("<h3>%s</h3>%s"), sL, HTML_LINEFEED);
		sText.Write(file);

		CInputGroup* pInputGroup;
		int i,c;
		c = m_Array.GetSize();
		for (i=0;i<c;i++)
		{
			pInputGroup = (CInputGroup*)m_Array.GetAt(i);
			if (pInputGroup)
			{
				sText.Format(_T("<a href=\"%04lx.htm\">%s</a><br>%s"),
					pInputGroup->GetID().GetGroupID(), pInputGroup->GetName(), HTML_LINEFEED);
				sText.Write(file);
				pInputGroup->GenerateHTMLFile();
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
//////////////////////////////////////////////////////////////////////////////
CInputGroup* CInputList::GetGroupByID(CSecID id) const
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
const CInput* CInputList::GetInputByID(CSecID id) const
{
	for (int i=0;i<GetSize();i++) 
	{
		if (id.GetGroupID()==GetGroupAt(i)->GetID().GetGroupID()) 
		{
			return ( GetGroupAt(i)->GetInput(id.GetSubID()) );
		}
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////////
int CInputList::GetNrOfActiveInputs() const
{
	int ret = 0;
	for (int i=0;i<GetSize();i++) 
	{
		ret += GetGroupAt(i)->GetNrOfActiveInputs();
	}
	return ret;
}
//////////////////////////////////////////////////////////////////////////////
BOOL CInputList::DeleteActivation(CSecID id)
{
	BOOL ret = FALSE;
	for (int i=0;i<GetSize();i++) 
	{
		ret |= GetGroupAt(i)->DeleteActivation(id);
	}
	return ret;
}

