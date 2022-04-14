/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: media.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/media.cpp $
// CHECKIN:		$Date: 16.01.04 16:05 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 16.01.04 11:41 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "Media.h"
#include "MediaGroup.h"


//////////////////////////////////////////////////////////////////////////////
// CMedia
//////////////////////////////////////////////////////////////////////////////
CMedia::CMedia(WORD wGroupID, WORD wSubID)
{
	m_id = CSecID(wGroupID,wSubID);
   m_bIsAudio   = false;
	m_bIsVideo   = false;
	m_bIsDefault = false;
	m_bIsDisabled= false;
}
//////////////////////////////////////////////////////////////////////////////
const CMedia& CMedia::operator=(const CMedia& inp)
{
	m_id = inp.m_id;
	m_sName = inp.m_sName;
	m_sType = inp.m_sType;
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
CMedia::~CMedia()
{
}
//////////////////////////////////////////////////////////////////////////////
BOOL CMedia::Load(CWK_Profile& wkp)
{
	CString sSection;
	CString sMediaDefine;
	m_bIsAudio = false;
	m_bIsVideo = false;

	sSection.Format(_T("%s\\Group%04X"),SEC_NAME_MEDIAGROUPS, m_id.GetGroupID());
	sMediaDefine = wkp.GetString(sSection,m_id.GetSubID()+1,_T(""));

	if (sMediaDefine.IsEmpty())
	{
		return FALSE;
	}

	// name
	m_sName = wkp.GetStringFromString(sMediaDefine, INI_COMMENT,_T(""));
	m_sType = wkp.GetStringFromString(sMediaDefine, INI_TYPE,_T(""));

	CString sSpecInfo = wkp.GetStringFromString(sMediaDefine, INI_SPECIFIC_INFO, _T(""));
	if      (sSpecInfo == MEDIA_IS_AUDIO) m_bIsAudio = true;
	else if (sSpecInfo == MEDIA_IS_VIDEO) m_bIsVideo = true;
	m_bIsDefault  = wkp.GetNrFromString(sMediaDefine, INI_DEFAULT, 0) ? true : false;
	m_bIsDisabled = wkp.GetNrFromString(sMediaDefine, INI_DISABLED, 0) ? true : false;

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
BOOL CMedia::Save(CWK_Profile& wkp)
{
	CString sSection;
	CString sMediaDefine;
	CString sSpecInfo;
	if      (m_bIsAudio) sSpecInfo = MEDIA_IS_AUDIO;
	else if (m_bIsVideo) sSpecInfo = MEDIA_IS_VIDEO;

	sSection.Format(_T("%s\\Group%04X"),SEC_NAME_MEDIAGROUPS,m_id.GetGroupID());
	sMediaDefine.Format(_T("%s%s%s%s%s%s%s%d%s%d"), 
					INI_COMMENT,       m_sName,
	            INI_SPECIFIC_INFO, sSpecInfo,
					INI_TYPE,          m_sType,
					INI_DEFAULT,       m_bIsDefault,
					INI_DISABLED,      m_bIsDisabled);

	wkp.WriteStringIndex(sSection,m_id.GetSubID()+1,sMediaDefine);

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
void CMedia::GenerateHTMLFile(CMediaGroup* pMediaGroup)
{
/*
	CString sFileName;
	sFileName.Format(_T("%s\\%lx.htm"),theApp.m_sLocalWWW,m_id.GetID());

	if (m_tInstructionStampNew == m_tInstructionStamp)
	{
		if (DoesFileExist(sFileName))
		{
			return;
		}
	}
	m_tInstructionStamp = m_tInstructionStampNew;

	CStdioFile file;
	CFileException cfe;


	if (file.Open(sFileName,CFile::modeCreate|CFile::modeWrite,&cfe))
	{
		CString sT;
		CString sL;
		CString sTime;

		file.WriteString(_T("<html>\n"));
		// Überschrift
		file.WriteString(_T("<head>\n"));
		sL.LoadString(IDS_INFORMATION);
		sT.Format(_T("<title>%s %s</title>\n"),sL,m_sName);
		file.WriteString(sT);
		file.WriteString(_T("</head>\n"));

		// Text
		file.WriteString(_T("<body bgcolor=\"#C0C0C0\">\n"));
		sT.Format(_T("<h3>%s %s</h3>\n"),sL,m_sName);
		file.WriteString(sT);

		if (!m_sInformation.IsEmpty())
		{
			sT.Format(_T("%s<br>\n"),m_sInformation);
			file.WriteString(sT);
		}
		
		sL.LoadString(IDS_EDITOR);
		sTime = m_tInstructionStamp.Format(_T("%d.%m.%Y %H:%M:%S"));
		sT.Format(sL,m_sEditor,sTime);
		file.WriteString(sT);

		file.WriteString(_T("<hr>\n"));
		sL.Format(_T("<a href=\"%04lx.htm\">%s</a><br>\n"),
			m_id.GetGroupID(),pMediaGroup->GetName());
		file.WriteString(sL);

		file.WriteString(_T("</body>\n"));
		file.WriteString(_T("</html>\n"));
	}
	else
	{
		WK_TRACE_ERROR(_T("Cannot open instruction file %s, %s\n"),
						sFileName, GetLastErrorString(cfe.m_lOsError));
	}
*/
}
