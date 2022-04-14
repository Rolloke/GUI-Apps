/* GlobalReplace: WK_GetProfileString --> wkp.GetString */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: mediagroup.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/mediagroup.cpp $
// CHECKIN:		$Date: 20.01.06 11:29 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 20.01.06 10:25 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "MediaGroup.h"

///////////////////////////////////////////////////////////////////////////////////////
CMediaGroup::CMediaGroup()
{
	// for serialization only
}
///////////////////////////////////////////////////////////////////////////////////////
CMediaGroup::CMediaGroup(LPCTSTR pName, int iSize, LPCTSTR pSMName, CSecID SecID, LPCTSTR sType)
{
	CMedia* pMedia;
	WORD i;
	CString sName;
	m_sName			= pName;
	m_GroupID		= SecID;	
	m_sSMName		= pSMName;
	m_sType			= sType;

	m_Array.SetSize(iSize);	
	for (i=0;i<m_Array.GetSize();i++) 
	{
		pMedia = new CMedia(m_GroupID.GetGroupID(),(WORD)i);
		sName.Format(_T("%s %02d"),m_sName,i+1);
		pMedia->SetName(sName);
		m_Array.SetAt(i, pMedia);
	}
}
///////////////////////////////////////////////////////////////////////////////////////
CMediaGroup::~CMediaGroup()
{
	Reset();
}
//////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CMediaGroup::AssertValid() const
{
	CObject::AssertValid();
}

void CMediaGroup::Dump(CDumpContext& dc) const
{
//	CObject::Dump(dc);
	dc << _T("CMediaGroup ") << m_sName;
}
#endif //_DEBUG
///////////////////////////////////////////////////////////////////////////////////////
void CMediaGroup::SetSize(WORD iSize)
{
	CString sName;
	CMedia* pMedia;
	WORD oldSize = (WORD)m_Array.GetSize();
	WORD i;

	if (iSize < oldSize)
	{
		for (i=(WORD)(oldSize-1);i>=iSize;i--)
		{
			pMedia = (CMedia*)m_Array.GetAt(i);
			WK_DELETE(pMedia);
			m_Array.RemoveAt(i);
		}
	}
	else if (iSize > oldSize)
	{
		m_Array.SetSize(iSize);	
		for (i=oldSize;i<m_Array.GetSize();i++) 
		{
			pMedia = new CMedia(m_GroupID.GetGroupID(),(WORD)i);
			sName.Format(_T("%s %02d"),m_sName,i+1);
			pMedia->SetName(sName);
			m_Array.SetAt(i, pMedia);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////
BOOL CMediaGroup::SetMedia(CMedia* pMedia)
{
	int index = pMedia->GetID().GetSubID();

	if (pMedia && (index < m_Array.GetSize()))
	{
		CMedia *pOldMedia = (CMedia*)m_Array.GetAt(index);
		if (pOldMedia) 
		{ 
			*pOldMedia = *pMedia; // just copy the data
		}
		else 
		{
			WK_DELETE(pOldMedia);
			m_Array.SetAt(index, pMedia);
		}
		return TRUE;

	}
	return FALSE;
}
///////////////////////////////////////////////////////////////////////////////////////
void CMediaGroup::Reset()
{
	int i,c;
	CMedia* pMedia;

	c = m_Array.GetSize();
	for (i=0;i<c;i++) 
	{
		pMedia = (CMedia*)m_Array.GetAt(i);
		WK_DELETE(pMedia);

	} 
	m_Array.RemoveAll();
}
///////////////////////////////////////////////////////////////////////////////////////
BOOL CMediaGroup::Load(int iNr, CWK_Profile& wkp)
{
	CString sGroupDefine;
	CMedia*	pMedia;  
	int i,c;

	sGroupDefine = wkp.GetString(SEC_NAME_MEDIAGROUPS,iNr,_T(""));

// number of medias
	c = (int)wkp.GetNrFromString(sGroupDefine, INI_NR, 0L);
	if (c<0) 
	{
		return FALSE;
	}

	// name and shared memory name
	m_sName = wkp.GetStringFromString(sGroupDefine, INI_COMMENT,_T(""));
	m_sSMName = wkp.GetStringFromString(sGroupDefine, INI_SMNAME, _T(""));

	// SecID
	WORD wID = (WORD)wkp.GetHexFromString(sGroupDefine, INI_ID, -1);	
	m_GroupID.Set(wID,0);

	m_sType = wkp.GetStringFromString(sGroupDefine, INI_TYPE   , _T(""));

	// medias
	CString sName;
	for (i=0;i<c;i++)
	{
		pMedia = new CMedia(wID,(WORD)(i));
		if (pMedia->Load(wkp))
		{
			m_Array.Add(pMedia);
		}
		else
		{
			sName.Format(_T("%s %02d"),m_sName,i+1);
			pMedia->SetName(sName);
			m_Array.Add(pMedia);
		}
	}
	
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////
BOOL CMediaGroup::Save(int iNr,CWK_Profile& wkp)
{
	int i,c;
	CMedia* pMedia;
	// store group record in _T("MediaGroups")
	CString sGroupEntry;
	sGroupEntry.Format(_T("%s%x%s%d%s%s%s%s%s%s"),
						INI_ID     , m_GroupID.GetGroupID(),
						INI_NR     , m_Array.GetSize(),
						INI_SMNAME , m_sSMName,
						INI_COMMENT, m_sName,
						INI_TYPE,    m_sType);

	wkp.WriteStringIndex(SEC_NAME_MEDIAGROUPS, iNr, sGroupEntry); 

	c = m_Array.GetSize();
	for (i=0;i<c;i++)
	{
		pMedia = (CMedia*)m_Array.GetAt(i);
		if (pMedia)
		{
			pMedia->Save(wkp);
		}
	}

	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////
void CMediaGroup::GenerateHTMLFile()
{
/*
	CString sFileName;
	CStdioFile file;
	CFileException cfe;

	sFileName.Format(_T("%s\\%04lx.htm"),theApp.m_sLocalWWW,m_GroupID.GetGroupID());

	if (file.Open(sFileName,CFile::modeCreate|CFile::modeWrite,&cfe))
	{
		CString sT;
		CString sL;
		CString sTime;

		file.WriteString(_T("<html>\n"));
		// Überschrift
		file.WriteString(_T("<head>\n"));
		sL.LoadString(IDS_MEDIAGROUP);
		sT.Format(_T("<title>%s %s</title>\n"),sL,m_sName);
		file.WriteString(sT);
		file.WriteString(_T("</head>\n"));

		// Text
		file.WriteString(_T("<body bgcolor=\"#C0C0C0\">\n"));
		sT.Format(_T("<h3>%s %s</h3>\n"),sL,m_sName);
		file.WriteString(sT);

		CMedia* pMedia;
		int i,c;
		c = m_Array.GetSize();
		for (i=0;i<c;i++)
		{
			pMedia = (CMedia*)m_Array.GetAt(i);
			if (pMedia)
			{
				sL.Format(_T("<a href=\"%08lx.htm\"> %s </a><br>\n"),
					pMedia->GetID().GetID(),
					pMedia->GetName());
				file.WriteString(sL);
				pMedia->GenerateHTMLFile(this);
			}
		}
		file.WriteString(_T("<hr>\n"));
		sT.LoadString(IDS_MEDIA);
		sL.Format(_T("<a href=\"media.htm\"> %s </a><br>\n"),sT);
		file.WriteString(sL);
		file.WriteString(_T("</body>\n"));
		file.WriteString(_T("</html>\n"));
	}
	else
	{
		WK_TRACE_ERROR(_T("Cannot open group file %s, %s\n"),
						sFileName, GetLastErrorString(cfe.m_lOsError));
	}
*/
}
