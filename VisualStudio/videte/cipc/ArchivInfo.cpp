/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ArchivInfo.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/ArchivInfo.cpp $
// CHECKIN:		$Date: 22.06.06 21:46 $
// VERSION:		$Revision: 40 $
// LAST CHANGE:	$Modtime: 22.06.06 21:43 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdcipc.h"

#include "SecID.h"
#include "Permission.h"
#include "wkclasses\wk_profile.h"
#include "wkclasses\wk_trace.h"
#include "ArchivInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const TCHAR szArchive[]		= _T("Archive");
static const TCHAR szNumArchive[]	= _T("ArchivCount");

/*{Overview,CArchivInfo}
 {CArchivInfo},{CArchivInfoArray} 
*/

/*
  
	{overview|Overview,CArchivInfo},
	{CArchivInfoArray}
*/

/////////////////////////////////////////////////////////////////////////////
/*Function: NYD*/
CArchivInfo::CArchivInfo()
{
//	m_sName;
	m_ID.SetGroupID(SECID_GROUP_ARCHIVE);
	m_ID.SetSubID(0);
	Init();
}
/////////////////////////////////////////////////////////////////////////////
/*Function: NYD*/
CArchivInfo::CArchivInfo(CSecID id)
{
//	m_sName;
	m_ID = id;
	Init();
}
/////////////////////////////////////////////////////////////////////////////
void CArchivInfo::Init()
{
	m_dwSizeMB = START_SIZE_MB;
	m_wSafeRingFor = 0;
	m_ArchivTyp = RING_ARCHIV;
	m_dwMaxStorageTime = 0;
	m_bDynamicSize = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: NYD*/
CArchivInfo::~CArchivInfo()
{
}
/////////////////////////////////////////////////////////////////////////////
/*Function: NYD*/
void CArchivInfo::SetName(const CString& sName)
{
	m_sName = sName;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: NYD*/
BOOL CArchivInfo::Load(int i,CWK_Profile& wkp)
{
	CString sTemp;
	CString sID;
	WORD	  wSubID;
	_TCHAR  szBuf[12];
	int	  p;

	sTemp = wkp.GetString(szArchive, _itot(i,szBuf,10), _T(""));

	if (sTemp.IsEmpty())
		return FALSE;

	if (isdigit(sTemp[0]))
	{
		// old style
		p = sTemp.Find('\\');
		if (p==-1)
			return FALSE;
		 
		sID   = sTemp.Left(p);
		DWORD dwTmp;
		_stscanf(sID, _T("%04lx"),&dwTmp);
		wSubID = (WORD)dwTmp;
		m_ID.SetSubID(wSubID);
		m_sName = sTemp.Mid(p+1);

		if (m_sName.IsEmpty())
			return FALSE;

		// read old db settings too
		CString sSection;
		CString sValue;
		
		sSection.Format(_T("ArchivManagment\\%04lx"), m_ID.GetSubID());

		sValue = wkp.GetString(sSection,_T("Typ"),_T(""));
		m_ArchivTyp = GetArchivType(sValue);
		if (m_ArchivTyp == NO_ARCHIV)
		{
			return FALSE;
		}

		m_dwSizeMB = wkp.GetInt(sSection,_T("SizeInMB"),0);
		if (m_dwSizeMB==0)
		{
			return FALSE;
		}

		m_wSafeRingFor = (WORD)wkp.GetInt(sSection,_T("SafeRingFor"),0);
	}
	else
	{
		// new style
		m_ID = (DWORD)wkp.GetHexFromString(sTemp,_T("\\ID"),SECID_NO_ID);
		m_sName = wkp.GetStringFromString(sTemp,_T("\\CO"),_T(""));
		m_ArchivTyp = (ArchivType)wkp.GetHexFromString(sTemp,_T("\\TYP"),0);
		m_dwSizeMB = (DWORD)wkp.GetHexFromString(sTemp,_T("\\SIZE"),0);
		m_wSafeRingFor = (WORD)wkp.GetHexFromString(sTemp,_T("\\SRF"),0);
		m_sFixedDrive = wkp.GetStringFromString(sTemp,_T("\\FD"),_T(""));
		m_dwMaxStorageTime = wkp.GetNrFromString(sTemp,_T("\\MST"),0);
		m_bDynamicSize = wkp.GetNrFromString(sTemp,_T("\\DYN"),0);

		if (/*(m_sName.IsEmpty()) || */
			(m_ArchivTyp == NO_ARCHIV) ||
			(m_dwSizeMB == 0))
		{
			WK_TRACE_ERROR(_T("archive info invalid %d,%s\n"),i,sTemp);
			return FALSE;
		}
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: NYD*/
void CArchivInfo::Save(int i,CWK_Profile& wkp)
{
	// only save new style
	CString sValue;
	CString sKey;

	sKey.Format(_T("%d"),i);
	sValue.Format(_T("\\ID%08lx\\CO%s\\TYP%08x\\SIZE%08lx\\SRF%04x\\FD%s\\MST%d\\DYN%d"),
		m_ID.GetID(),
		m_sName,
		(DWORD)m_ArchivTyp,
		m_dwSizeMB,
		m_wSafeRingFor,
		m_sFixedDrive,
		m_dwMaxStorageTime,
		m_bDynamicSize);
	wkp.WriteString(szArchive,sKey,sValue);
}

/**/
/////////////////////////////////////////////////////////////////////////////
/*{Overview,CArchivInfoArray}
 {members|CArchivInfoArray},{CArchivInfo} 
*/
/*
  
	{overview|Overview,CArchivInfoArray},
	{CArchivInfo}
*/

/////////////////////////////////////////////////////////////////////////////
/*Function:T{performs DeleteAll()!} destructor, performs DeleteAll()!*/
CArchivInfoArray::~CArchivInfoArray()
{
	DeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
DWORD CArchivInfoArray::GetSizeMB()
{
	DWORD dwRet = 0;

	int i;

	for (i=0; i<GetSize(); i++) 
	{
		dwRet += GetAtFast(i)->GetSizeMB(); 
	} 
	return dwRet;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CArchivInfoArray::GetSizeMBForFixedDrive(const CString& sRoot)
{
	DWORD dwRet = 0;

	int i;

	for (i=0; i<GetSize(); i++)
	{
		if ( sRoot == GetAtFast(i)->GetFixedDrive() )
		{
			dwRet += GetAtFast(i)->GetSizeMB(); 
		}
	} 
	return dwRet;
}
/////////////////////////////////////////////////////////////////////////////

/*Function: NYD*/
void CArchivInfoArray::Load(CWK_Profile& wkp)
{
	DeleteAll();

	CString sTemp = wkp.GetString(szArchive,_T("0"),_T(""));
	BOOL bOldFormat = FALSE;

	if (!sTemp.IsEmpty())
	{
		bOldFormat = isdigit(sTemp[0]);
	}

	int c = wkp.GetInt(szArchive,szNumArchive,0);
	
	CArchivInfo* pArchiv;
	for (int i=0;i<c;i++)
	{
		pArchiv = new CArchivInfo();
		if (pArchiv->Load(i,wkp))
		{
			Add(pArchiv);
		}
		else
		{
			delete pArchiv;
		}
	}

	if (bOldFormat)
	{
		WK_TRACE(_T("archive format converted\n"));
		Save(wkp);
		wkp.DeleteSection(_T("ArchivManagment"));
	}
}
/////////////////////////////////////////////////////////////////////////////
/*Function: NYD*/
void CArchivInfoArray::Save(CWK_Profile& wkProfile)
{
	int i,c;
	CArchivInfo* pArchiv;

	c = GetSize();

	wkProfile.DeleteSection(szArchive);

	wkProfile.WriteInt(szArchive,szNumArchive,c);

	for (i=0;i<c;i++)
	{
		pArchiv = GetAtFast(i);
		if (pArchiv)
		{
			pArchiv->Save(i,wkProfile);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
/*Function: NYD*/
CArchivInfo* CArchivInfoArray::AddArchivInfo()
{
	CArchivInfo* pArchivInfo;
	WORD wID = 1;
	int i,bLoop,bFoundInDeleted;

	bLoop = TRUE;
	bFoundInDeleted = FALSE;

	while (bLoop)
	{
		pArchivInfo = GetArchivInfo(CSecID(SECID_GROUP_ARCHIVE,wID));
		bFoundInDeleted = FALSE;
		for (i=0;i<m_DeletedIDs.GetSize();i++)
		{
			if (wID==m_DeletedIDs.GetAt(i))
			{
				bFoundInDeleted = TRUE;
				break;
			}
		}

		if (pArchivInfo || (wID==LOCAL_SEARCH_RESULT_ARCHIV_NR) || bFoundInDeleted)
		{
			wID++;
		}
		else
		{
			bLoop = FALSE;
		}
	}
	
	pArchivInfo = new CArchivInfo(CSecID(SECID_GROUP_ARCHIVE,wID));
	Add(pArchivInfo);

	return pArchivInfo;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: NYD*/
void CArchivInfoArray::DeleteArchivInfo(CArchivInfo* pArchivInfo)
{
	int i,c;
	CArchivInfo* pSearch;

	c = GetSize();

	for (i=0;i<c;i++)
	{
		pSearch = GetAtFast(i);
		if (pArchivInfo==pSearch)
		{
			// found
			RemoveAt(i);
			m_DeletedIDs.Add(pArchivInfo->GetID().GetSubID());
			WK_DELETE(pArchivInfo);
			return;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
/*Function: NYD*/
CArchivInfo* CArchivInfoArray::GetArchivInfo(const CSecID& id)
{
	int i,c;
	CArchivInfo* pSearch;

	c = GetSize();

	for (i=0;i<c;i++)
	{
		pSearch = GetAtFast(i);
		if (pSearch->GetID()==id)
		{
			// found
			return pSearch;
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: NYD*/
void CArchivInfoArray::DeleteAll()
{
	for (int i=0;i<GetSize();i++) 
	{ 
		if (GetAtFast(i)) 
		{ 
			delete GetAtFast(i); 
		} 
	} 
	RemoveAll(); 
}
