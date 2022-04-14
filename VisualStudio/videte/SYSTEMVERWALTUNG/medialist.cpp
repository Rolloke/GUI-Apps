/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: medialist.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/medialist.cpp $
// CHECKIN:		$Date: 11.09.03 15:51 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 11.09.03 12:04 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "SystemVerwaltung.h"

#include "MediaGroup.h"
#include "MediaList.h"

//////////////////////////////////////////////////////////////////////////////
// CMediaList
/////////////////////////////////////////////////////////////////////////////
CMediaList::CMediaList()
{
}
/////////////////////////////////////////////////////////////////////////////
CMediaList::~CMediaList()
{
	Reset();
}
//////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CMediaList::AssertValid() const
{
	CObject::AssertValid();
}

void CMediaList::Dump(CDumpContext& dc) const
{
//	CObject::Dump(dc);
	dc << _T("CMediaList ") << m_Array.GetSize();
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
CMediaGroup* CMediaList::AddMediaGroup(LPCTSTR pName, int iNr, LPCTSTR pSMName, LPCTSTR sType)
{
	CMediaGroup* pMediaGroup;
	CSecID id(SECID_GROUP_MEDIA+1,0);

	int i=1;
	while (i)
	{
		pMediaGroup = GetGroupByID(id);
		if (!pMediaGroup)
			break;
		id.SetGroupID((WORD)(id.GetGroupID()+1)); 
	}
	
	pMediaGroup = new CMediaGroup(pName,iNr, pSMName,id,sType);
	m_Array.Add(pMediaGroup);

	return pMediaGroup;
}
/////////////////////////////////////////////////////////////////////////////
CMediaGroup* CMediaList::GetGroupByName(const CString& sName) const
{ 
	CMediaGroup* pMediaGroup;

	for (int iGrpNr=0;iGrpNr<GetSize();iGrpNr++) 
	{
		pMediaGroup = GetGroupAt(iGrpNr);
		if(sName == pMediaGroup->GetName())
		{
			return pMediaGroup;		// EXIT
		}
	}
	return NULL; 
}
/////////////////////////////////////////////////////////////////////////////
CMediaGroup* CMediaList::GetGroupBySMName(const CString& sSMName) const
{
	CMediaGroup* pMediaGroup;

	for (int iGrpNr=0;iGrpNr<GetSize();iGrpNr++) 
	{
		pMediaGroup = GetGroupAt(iGrpNr);
		if(sSMName == pMediaGroup->GetSMName())
		{
			return pMediaGroup;		// EXIT
		}
	}
	return NULL; 
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMediaList::DeleteGroup(const CString& sSMName)
{
	CMediaGroup* pMediaGroup;

	for (int iGrpNr=0;iGrpNr<GetSize();iGrpNr++) 
	{
		pMediaGroup = GetGroupAt(iGrpNr);
		if(sSMName == pMediaGroup->GetSMName())
		{
			WK_DELETE(pMediaGroup);
			m_Array.RemoveAt(iGrpNr);
			return TRUE; // EXIT
		}
	}
	return FALSE; 
}
/////////////////////////////////////////////////////////////////////////////
void CMediaList::Reset()
{
	int i,c;
	CMediaGroup* pMediaGroup;

	c = m_Array.GetSize();
	for (i=0;i<c;i++) 
	{
		pMediaGroup = (CMediaGroup*)m_Array.GetAt(i);
		WK_DELETE(pMediaGroup);

	} 
	m_Array.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
void CMediaList::Load(CWK_Profile& wkp)
{
	int i,c;
	CMediaGroup* pMediaGroup;

	Reset();

	c = wkp.GetInt(SEC_NAME_MEDIAGROUPS, ENT_NAME_MAXGROUPS, 0);
	
	for (i=0;i<c;i++)
	{
		pMediaGroup = new CMediaGroup();
		if (pMediaGroup->Load(i+1,wkp))
		{
			m_Array.Add(pMediaGroup);
		}
		else
		{
			delete pMediaGroup;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMediaList::Save(CWK_Profile& wkp, BOOL bGenerateHTML)
{
	int i,c;
	CMediaGroup* pMediaGroup;
	
	wkp.DeleteSection(SEC_NAME_MEDIAGROUPS);

	c = m_Array.GetSize();
	wkp.WriteInt(SEC_NAME_MEDIAGROUPS, ENT_NAME_MAXGROUPS, c);

	for (i=0;i<c;i++)
	{
		pMediaGroup = (CMediaGroup*)m_Array.GetAt(i);
		if (pMediaGroup)
		{
			pMediaGroup->Save(i+1,wkp);
		}
	}
	if (bGenerateHTML)
	{
		GenerateHTMLFile();
	}
}
//////////////////////////////////////////////////////////////////////////////
void CMediaList::GenerateHTMLFile()
{
/*
	CString sFileName;
	CStdioFile file;
	CFileException cfe;

	sFileName.Format(_T("%s\\media.htm"),theApp.m_sLocalWWW);

	if (file.Open(sFileName,CFile::modeCreate|CFile::modeWrite,&cfe))
	{
		CString sT;
		CString sL;
		CString sTime;

		file.WriteString(_T("<html>\n"));
		// Überschrift
		file.WriteString(_T("<head>\n"));
		sL.LoadString(IDS_MEDIA);
		sT.Format(_T("<title>%s</title>\n"),sL);
		file.WriteString(sT);
		file.WriteString(_T("</head>\n"));

		// Text
		file.WriteString(_T("<body bgcolor=\"#C0C0C0\">\n"));
		sT.Format(_T("<h3>%s</h3>\n"),sL);
		file.WriteString(sT);

		CMediaGroup* pMediaGroup;
		int i,c;
		c = m_Array.GetSize();
		for (i=0;i<c;i++)
		{
			pMediaGroup = (CMediaGroup*)m_Array.GetAt(i);
			if (pMediaGroup)
			{
				sL.Format(_T("<a href=\"%04lx.htm\"> %s </a><br>\n"),
					pMediaGroup->GetID().GetGroupID(),
					pMediaGroup->GetName());
				file.WriteString(sL);
				pMediaGroup->GenerateHTMLFile();
			}
		}
		file.WriteString(_T("</body>\n</html>\n"));
	}
	else
	{
		WK_TRACE_ERROR(_T("Cannot open media file %s, %s\n"),
						sFileName, GetLastErrorString(cfe.m_lOsError));
	}
*/
}
//////////////////////////////////////////////////////////////////////////////
CMediaGroup* CMediaList::GetGroupByID(CSecID id) const
{
	for (int i=0;i<GetSize();i++) 
	{
		if (id==GetGroupAt(i)->GetID()) 
		{
			return (GetGroupAt(i));
		}
	}
	return (NULL);
}
//////////////////////////////////////////////////////////////////////////////
const CMedia* CMediaList::GetMediaByID(CSecID id) const
{
	for (int i=0;i<GetSize();i++) 
	{
		if (id.GetGroupID()==GetGroupAt(i)->GetID().GetGroupID()) 
		{
			return ( GetGroupAt(i)->GetMedia(id.GetSubID()) );
		}
	}
	return NULL;
}
