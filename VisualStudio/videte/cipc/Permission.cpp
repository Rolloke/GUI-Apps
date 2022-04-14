/* GlobalReplace: @[mM][dD] --> Function: */
/* GlobalReplace: @[aA][lL][sS][oO] -->  */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: Permission.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/Permission.cpp $
// CHECKIN:		$Date: 12.06.06 11:23 $
// VERSION:		$Revision: 69 $
// LAST CHANGE:	$Modtime: 12.06.06 11:16 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdcipc.h"

#include "wk.h"

#include "Permission.h"
#include "ArchivInfo.h"
#include "ConnectionRecord.h"
#include "CipcStringDefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const TCHAR szPermission[] = _T("Permission");
static const TCHAR szOutputs[] = _T("Outputs");
static const TCHAR szArchivs[] = _T("Archivs");
static const TCHAR szSuperVisor[] = _T("SuperVisor");
static const TCHAR szSpecialReceiver[] = _T("SpecialReceiver");
static const TCHAR szNumberOfPermissions[] = _T("NumberOfPermissions");
static const TCHAR szPriority[] = _T("Priority");
static const TCHAR szTimer[] = _T("Timer");

/*{CPermission Overview|Overview,CPermission}
 {members|CPermission},
{CPermissionArray}, {CUser}

The CPermission class should encapsulate a Security 3.0 permission, in common a group of humans
belongs to one permission.
Every CPermission has a name, id, and the security accessing rights.
All CPermissions are collected in a CPermissionArray, please access all CPermissions using the array.
The permission is an ID of a {CPermission}.
*/
/*
  {Overview,CPermission}, {CPermissionArray}
*/

/////////////////////////////////////////////////////////////////////////////
/*Function: default constructor, initializes all with zero values, except group id,
group id will be set to SECID_GROUP_PERMISSION
*/
/////////////////////////////////////////////////////////////////////////////
CPermission::CPermission()
{
	m_Name = _T("");
	m_Password = _T("");
	m_Comment = _T("");
	m_ID.SetGroupID(SECID_GROUP_PERMISSION);
	m_ID.SetSubID(0);
	m_Flags = 0L;
	m_Priority = 3;
}
/////////////////////////////////////////////////////////////////////////////
CPermission::CPermission(const CPermission& perm)
{
	m_Name = perm.GetName();
	m_Password = perm.GetPassword();
	m_Comment = perm.GetComment();
	m_ID = perm.GetID();
	m_Flags = perm.GetFlags();
	m_Priority = perm.GetPriority();

	m_OutputIDs.Copy(perm.m_OutputIDs);
	m_ArchivIDs.Copy(perm.m_ArchivIDs);
}
/////////////////////////////////////////////////////////////////////////////
CPermission::CPermission(CSecID id)
{
	m_ID = id;

	if (IsSuperVisor())
	{
		m_Name = szSuperVisor;
		m_Password = _T("");
		m_Comment = _T("");
		m_Flags = 0xFFFFFFFF;
		m_Priority = 1;
	}
	else if (IsSpecialReceiver())
	{
		m_Name = szSpecialReceiver;
		m_Password = _T("");
		m_Comment = _T("");
		m_Flags =
//				    WK_ALLOW_HARDWARE
//				  | WK_ALLOW_INPUT
//				  | WK_ALLOW_OUTPUT
//				  | WK_ALLOW_TIMER
//				  | WK_ALLOW_PROZESS
//				  | WK_ALLOW_LINK
//				  | WK_ALLOW_USER
//				  | WK_ALLOW_PERMISSION
				    WK_ALLOW_HOST
				  | WK_ALLOW_ARCHIV
				  | WK_ALLOW_BACKUP
//				  | WK_ALLOW_SDICONFIG
				  | WK_ALLOW_SHUTDOWN
				  | WK_ALLOW_DATA_EXPORT
//				  | WK_ALLOW_SETTINGS
//				  | WK_ALLOW_SECURITY
				  | WK_ALLOW_LOGVIEW
				  | WK_ALLOW_DATE_TIME
//				  | WK_ALLOW_IMAGECOMPARE
//				  | WK_ALLOW_ALARM_OFF_SPAN
					;
		m_Priority = 1;
	}
	else
	{
		m_Name = _T("");
		m_Password = _T("");
		m_Comment = _T("");
		m_Flags = 0L;
		m_Priority = 3;
	}
}
/////////////////////////////////////////////////////////////////////////////
CPermission::~CPermission()
{
}

/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
void CPermission::SetName(const CString& sName)
{
	if (   !IsSuperVisor()
		&& !IsSpecialReceiver()
		)
	{
		m_Name = sName;
	}
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
void CPermission::SetPassword(const CString& sPassword)
{
	m_Password = sPassword;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
void CPermission::SetComment(const CString& sComment)
{
	m_Comment = sComment;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
void CPermission::SetPriority(int iPriority)
{
	m_Priority = iPriority;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
void CPermission::SetTimer(CSecID id)
{
	m_idTimer = id;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPermission::Load(int i, CWK_Profile& wkProfile)
{
	CString sTemp,sID,sSection, sKey, sOemConverted;
	WORD	 wSubID;
	DWORD	 dwID;
	int	 p;
	sKey.Format(_T("%d"), i);

	if (i!=-1)
	{
#ifdef _UNICODE
		DWORD dwSize, dwType = wkProfile.GetType(szPermission, sKey, &dwSize);
		if (dwType == REG_SZ)
		{	
			sTemp         = wkProfile.GetString(szPermission, sKey, _T(""), TRUE);	// uses ansi
			sOemConverted = wkProfile.GetString(szPermission, sKey, _T(""));		// uses unicode
		}
		else if (dwType == REG_BINARY && dwSize > 0)
		{
			int nLength = dwSize / sizeof(_TCHAR);
			LPCTSTR sBuffer = sTemp.GetBufferSetLength(nLength+1);	// no zero termination is saved
			wkProfile.GetBinary(szPermission, sKey, (BYTE**)&sBuffer, (UINT*)&dwSize);
			sTemp.ReleaseBuffer(nLength);
		}
		else
		{
			sTemp.Empty();
		}
#else
		sTemp = wkProfile.GetString(szPermission, sKey, _T(""));
#endif

		if (sTemp.IsEmpty())
			return FALSE;

		p = sTemp.Find('\\');
		if (p==-1)
			return FALSE;

		sID   = sTemp.Left(p);
		DWORD dwTmp;
		_stscanf(sID,_T("%04lx"),&dwTmp);
		wSubID = (WORD)dwTmp;
		m_ID.SetSubID(wSubID);
		sTemp = sTemp.Mid(p+1);
		if (!sOemConverted.IsEmpty())
		{
			sOemConverted = sOemConverted.Mid(p+1);
		}
		
		p = sTemp.Find('\\');
		if (p==-1)
			return FALSE;
		if (sOemConverted.IsEmpty())
		{
			m_Name = sTemp.Left(p);
		}
		else
		{
			m_Name = sOemConverted.Left(p);
		}
		sTemp = sTemp.Mid(p+1);

		p = sTemp.Find('\\');
		if (p==-1)
			return FALSE;

		sSection = sTemp.Left(p);
		sTemp = sTemp.Mid(p+1);
		if (!sSection.IsEmpty())
		{
#ifdef _UNICODE
			wkProfile.Decode(sSection, dwType == REG_SZ ? TRUE : FALSE);
#else
			wkProfile.Decode(sSection);
#endif
		}
		m_Password = sSection;
		_stscanf(sTemp,_T("%08lx"),&m_Flags);

		sSection = szPermission;
		sSection += '\\';
		sSection += sID;
		
		m_Priority = wkProfile.GetInt(sSection,szPriority,3);
		m_idTimer = (DWORD)wkProfile.GetInt(sSection,szTimer,SECID_NO_ID);

		DeleteAll();
		sTemp = wkProfile.GetString(sSection,szOutputs,_T(""));

		if (!sTemp.IsEmpty())
		{
			while (sTemp.GetLength()>=8)
			{	
				sID = sTemp.Left(8);
				sTemp = sTemp.Mid(8);
				_stscanf(sID,_T("%08lx"),&dwID);
				m_OutputIDs.Add(dwID);
			}
		}

		sTemp = wkProfile.GetString(sSection,szArchivs,_T(""));

		if (!sTemp.IsEmpty())
		{
			BYTE bArchiv = 0;
			BOOL bFoundSearchArchiv = FALSE;
			while (sTemp.GetLength()>=8)
			{	
				sID = sTemp.Left(8);
				sTemp = sTemp.Mid(8);
				_stscanf(sID,_T("%08lx"),&dwID);
				bArchiv = (BYTE)(dwID & 0xFF);
				if (bArchiv==LOCAL_SEARCH_RESULT_ARCHIV_NR)
				{
					bFoundSearchArchiv = TRUE;
				}
				m_ArchivIDs.Add(dwID);
			}
			// Archiv Suchergebnisse
			if (!bFoundSearchArchiv)
			{
				CSecID sID(SECID_GROUP_ARCHIVE,LOCAL_SEARCH_RESULT_ARCHIV_NR);
				m_ArchivIDs.Add(sID.GetID());
				SetArchivFlags(sID,WK_ALLOW_PICTURE|WK_ALLOW_DATA|WK_ALLOW_DELETE);
			}
		}
	}
	else
	{
		// load supervisor
		// supervisor
#ifdef _UNICODE
//		sTemp = wkProfile.GetString(szPermission,szSuperVisor, _T(""), TRUE);
		DWORD dwSize, dwType = wkProfile.GetType(szPermission, szSuperVisor, &dwSize);
		if (dwType == REG_SZ)
		{	// uses ansi
			sTemp = wkProfile.GetString(szPermission, szSuperVisor, _T(""), TRUE);
		}
		else if (dwType == REG_BINARY && dwSize > 0)
		{
			int nLength = dwSize / sizeof(_TCHAR);
			LPCTSTR sBuffer = sTemp.GetBufferSetLength(nLength+1);	// no zero termination is saved
			wkProfile.GetBinary(szPermission, szSuperVisor, (BYTE**)&sBuffer, (UINT*)&dwSize);
			sTemp.ReleaseBuffer(nLength);
		}
		else
		{
			sTemp.Empty();
		}
#else
		sTemp = wkProfile.GetString(szPermission, szSuperVisor, _T(""));
#endif
		if (!sTemp.IsEmpty())
		{
#ifdef _UNICODE
			wkProfile.Decode(sTemp, dwType == REG_SZ ? TRUE : FALSE);
#else
			wkProfile.Decode(sTemp);
#endif
		}
		m_Password = sTemp;
	}
	
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
void CPermission::Save(int i,CWK_Profile& wkProfile, bool bUseBinaryFormat)
#else
void CPermission::Save(int i,CWK_Profile& wkProfile)
#endif
{
	CString sTemp;
	CString sID;
	CString sSection;
	_TCHAR  szID[10];
	_TCHAR  szBuf[12];
	int j,c;

	if (i!=-1)
	{
		_stprintf(szID,_T("%04x"), m_ID.GetSubID());
		sID = szID;
		
		sTemp = sID;
		sTemp += '\\';
		sTemp += m_Name;
		sTemp += '\\';
		sSection = m_Password;
		if (!sSection.IsEmpty())
		{
			wkProfile.Encode(sSection);
		}
		sTemp += sSection;
		_stprintf(szID,_T("%08x"),m_Flags);
		sTemp += '\\';
		sTemp += szID;

#ifdef _UNICODE
		if (bUseBinaryFormat)
		{
			// no zero termination is saved
			wkProfile.WriteBinary(szPermission, _itot(i,szBuf,10), (BYTE*)LPCTSTR(sTemp), sTemp.GetLength()*sizeof(_TCHAR));
		}
		else
		{
			wkProfile.WriteString(szPermission, _itot(i,szBuf,10), sTemp, TRUE);
		}
#else
		wkProfile.WriteString(szPermission,_itot(i,szBuf,10), sTemp);
#endif
		sSection = szPermission;
		sSection += '\\';
		sSection += sID;
		
		wkProfile.WriteInt(sSection,szPriority,m_Priority);
		wkProfile.WriteInt(sSection,szTimer,m_idTimer.GetID());

		sTemp.Empty();
		c = m_OutputIDs.GetSize();
		DWORD *pdwOutputIDs = m_OutputIDs.GetData();
		for (j=0;j<c;j++)
		{
//			_stprintf(szID,_T("%08x"),m_OutputIDs.GetAt(j));
			_stprintf(szID,_T("%08x"), pdwOutputIDs[j]);
			sTemp += szID;
		}
		wkProfile.WriteString(sSection,szOutputs,sTemp);

		sTemp.Empty();
		c = m_ArchivIDs.GetSize();
		DWORD *pdwArchivIDs = m_ArchivIDs.GetData();
		for (j=0;j<c;j++)
		{
//			_stprintf(szID,_T("%08x"),m_ArchivIDs.GetAt(j));
			_stprintf(szID,_T("%08x"), pdwArchivIDs[j]);
			sTemp += szID;
		}
		wkProfile.WriteString(sSection,szArchivs,sTemp);
	}
	else
	{
		// supervisor
		sTemp = m_Password;
		if (!sTemp.IsEmpty())
		{
			wkProfile.Encode(sTemp);
		}
#ifdef _UNICODE
		if (bUseBinaryFormat)
		{
			// no zero termination is saved
			wkProfile.WriteBinary(szPermission, szSuperVisor, (BYTE*)LPCTSTR(sTemp), sTemp.GetLength()*sizeof(_TCHAR));
		}
		else
		{
			wkProfile.WriteString(szPermission, szSuperVisor, sTemp, TRUE);
		}
#else
		wkProfile.WriteString(szPermission,szSuperVisor,sTemp);
#endif
	}
}
/////////////////////////////////////////////////////////////////////////////

/*Function: not yet documented */
void CPermission::AddOutput(CSecID id)
{
	DWORD dw = id.GetID();
	m_OutputIDs.Add(dw);
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
void CPermission::DeleteOutput(CSecID id)
{
	DWORD dw = id.GetID();
	int i,c;

	c = m_OutputIDs.GetSize();
	DWORD *pdwOutputIDs = m_OutputIDs.GetData();
	for (i=0;i<c;i++)
	{
		if (pdwOutputIDs[i]==dw)
		{
			m_OutputIDs.RemoveAt(i);
			return;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
void CPermission::AddArchiv(CSecID id, UINT uFlags)
{
	DWORD dw = id.GetID();

	dw = dw | (uFlags<<12);
	m_ArchivIDs.Add(dw);
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
void CPermission::SetArchivFlags(CSecID id, UINT uFlags)
{
	DWORD dw = id.GetID();
	// UNUSED DWORD dwFlags = uFlags;
	DWORD dwSearch;
	int i,c;

	c = m_ArchivIDs.GetSize();
	DWORD *pdwArchivIDs = m_ArchivIDs.GetData();
	for (i=0;i<c;i++)
	{
//		dwSearch = (m_ArchivIDs.GetAt(i)) & 0xFFFF0FFF;
		dwSearch = (pdwArchivIDs[i]) & 0xFFFF0FFF;
		if (dwSearch==dw)
		{
			dw = dw | (uFlags<<12);
			m_ArchivIDs.SetAt(i,dw);
			return;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
void CPermission::DeleteArchiv(CSecID id)
{
	DWORD dw = id.GetID();
	DWORD dwSearch;
	int i,c;

	c = m_ArchivIDs.GetSize();
	DWORD *pdwArchivIDs = m_ArchivIDs.GetData();
	for (i=0;i<c;i++)
	{
//		dwSearch = (m_ArchivIDs.GetAt(i)) & 0xFFFF0FFF;
		dwSearch = (pdwArchivIDs[i]) & 0xFFFF0FFF;
		if (dwSearch==dw)
		{
			m_ArchivIDs.RemoveAt(i);
			return;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
int	CPermission::GetNumberOfArchivs()
{
	return m_ArchivIDs.GetSize();
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
CSecID CPermission::GetArchiv(int nIndex)
{
	return m_ArchivIDs.GetAt(nIndex) & 0xFFFF0FFF;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
int	CPermission::GetNumberOfOutputs()
{
	return m_OutputIDs.GetSize();
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
CSecID CPermission::GetOutput(int nIndex)
{
	return m_OutputIDs.GetAt(nIndex);
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
BOOL CPermission::IsOutputAllowed(CSecID id)
{
	DWORD dw = id.GetID();
	int i,c;

	if (IsSuperVisor())
		return TRUE;

	c = m_OutputIDs.GetSize();
	DWORD *pdwOutputIDs = m_OutputIDs.GetData();

	for (i=0;i<c;i++)
	{
//		if (m_OutputIDs.GetAt(i)==dw)
		if (pdwOutputIDs[i]==dw)
		{
			return TRUE;
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented
 
   WK_ALLOW_PICTURE
 
   WK_ALLOW_DATA
 
   WK_ALLOW_DELETE

   */
BOOL CPermission::IsArchivAllowed(CSecID id, UINT uFlags)
{
	DWORD dw = id.GetID();
	DWORD dwFlags = uFlags;
	DWORD dwSearch;
	int i,c;

	if (   IsSuperVisor()
		|| IsSpecialReceiver()
		)
	{
		return TRUE;
	}

	c = m_ArchivIDs.GetSize();
	DWORD *pdwArchivIDs = m_ArchivIDs.GetData();
	for (i=0;i<c;i++)
	{
//		dwSearch = m_ArchivIDs.GetAt(i);
		dwSearch = pdwArchivIDs[i];
		if ((dwSearch & 0xFFFF0FFF)==dw)
		{
			dwSearch = (dwSearch & 0x0000F000) >> 12;
			return (dwSearch & dwFlags) == dwFlags;
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented
 
   WK_ALLOW_PICTURE
 
   WK_ALLOW_DATA
 
   WK_ALLOW_DELETE

   */
UINT CPermission::GetArchivFlags(CSecID id)
{
	DWORD dw = id.GetID();
	UINT uFlags;
	DWORD dwSearch;
	int i,c;

	if (   IsSuperVisor()
		|| IsSpecialReceiver()
		)
	{
		return WK_ALLOW_PICTURE|WK_ALLOW_DATA|WK_ALLOW_DELETE;
	}

	c = m_ArchivIDs.GetSize();
	const DWORD* pdwArchivIDs = m_ArchivIDs.GetData();
	for (i=0;i<c;i++)
	{
//		dwSearch = m_ArchivIDs.GetAt(i);
		dwSearch = pdwArchivIDs[i];
		if ((dwSearch & 0xFFFF0FFF)==dw)
		{
			uFlags = (UINT)((dwSearch & 0x0000F000) >> 12);
			return uFlags;
		}
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
void CPermission::DeleteAllArchivs()
{
	m_ArchivIDs.RemoveAll();
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
void CPermission::DeleteAllOutputs()
{
	m_OutputIDs.RemoveAll();
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
void CPermission::DeleteAll()
{
	m_OutputIDs.RemoveAll();
	m_ArchivIDs.RemoveAll();
}
/////////////////////////////////////////////////////////////////////////////
/*

*/
/*
{CPermissionArray Overview|Overview,CPermissionArray}
 {members|CPermissionArray}, {CPermission}

Kind of {WK_PTR_ARRAY} of {CPermission} records.
Permissions should be added using {CPermission::AddPermission}, and deleted with {CPermissionArray::DeleteAll}
or {CPermissionArray::DeletePermission}. So all Permission ID's are under the array's control and unique
for every station.
Accessing CPermission should be done with {CPermissionArray::GetPermission}.

 See CTypedPtrArray for base functions.
*/
/*  {overview|Overview,CPermissionArray},
{CPermission}
*/
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
CPermissionArray::CPermissionArray()
{
	// add Supervisor permission always as first entry!
	// this is needed for access
	Add(new CPermission(CSecID(SECID_SUPERVISOR_PERMISSION)));

	// add other special permissions
	CWK_Dongle dongle;
	InternalProductCode ipc = dongle.GetProductCode();
	if (   ipc == IPC_DTS_RECEIVER
		|| ipc == IPC_TOBS_RECEIVER
		)
	{
		Add(new CPermission(CSecID(SECID_SPECIAL_RECEIVER_PERMISSION)));
	}
#ifdef _UNICODE
	m_bUseBinaryFormat = TRUE;
#endif
}
/////////////////////////////////////////////////////////////////////////////
CPermissionArray::~CPermissionArray()
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
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
void CPermissionArray::Load(CWK_Profile& wkProfile)
{
	int i,c;
	CPermission* pPermission;

	DeleteAll();

	c = wkProfile.GetInt(szPermission,szNumberOfPermissions,0);

	pPermission = GetAtFast(0);
	pPermission->Load(-1,wkProfile);
	
	for (i=0;i<c;i++)
	{
		pPermission = new CPermission();
		if(pPermission->Load(i,wkProfile))
		{
			Add(pPermission);
		}
		else
		{
			delete pPermission;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
void CPermissionArray::Save(CWK_Profile& wkProfile)
{
	int i,c;
	CPermission* pPermission;

	c = GetSize();

	wkProfile.DeleteSection(szPermission);

	wkProfile.WriteInt(szPermission,szNumberOfPermissions,c-1);

	pPermission = GetAtFast(0);
#ifdef _UNICODE
	pPermission->Save(-1, wkProfile, m_bUseBinaryFormat ? true : false);
#else
	pPermission->Save(-1, wkProfile);
#endif
	for (i=1;i<c;i++)
	{
		pPermission = GetAtFast(i);
		if (pPermission)
		{
#ifdef _UNICODE
			pPermission->Save(i-1,wkProfile, m_bUseBinaryFormat ? true : false);
#else
			pPermission->Save(i-1,wkProfile);
#endif
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
CPermission* CPermissionArray::AddPermission()
{
	CPermission* pPermission;
	CSecID id(SECID_GROUP_PERMISSION,1);
	
	do
	{
		pPermission = GetPermission(id);
		if (pPermission) 
		{
			id = id.GetID() + 1; 
		}
	} while (pPermission);
	
	pPermission = new CPermission(id);
	Add(pPermission);

	return pPermission;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
void CPermissionArray::DeletePermission(CPermission* pPermission)
{
	int i,c;
	CPermission* pSearch;

	if (   pPermission->IsSuperVisor()
		|| pPermission->IsSpecialReceiver()
		)
		return;

	c = GetSize();

	for (i=0;i<c;i++)
	{
		pSearch = GetAtFast(i);
		if (pPermission==pSearch)
		{
			// found
			RemoveAt(i);
			WK_DELETE(pPermission);
			return;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
CPermission* CPermissionArray::GetPermission(const CSecID& id)
{
	int i,c;
	CPermission* pSearch;

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
/*Function: not yet documented */
CPermission* CPermissionArray::GetPermission(const CString& sName)
{
	int i,c;
	CPermission* pSearch;

	c = GetSize();

	for (i=0;i<c;i++)
	{
		pSearch = GetAtFast(i);
		if (sName==pSearch->GetName())
		{
			// found
			return pSearch;
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
CPermission* CPermissionArray::GetSuperVisor()
{
	CPermission* pSuperVisor = GetAtFast(0);
	return pSuperVisor;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
CPermission* CPermissionArray::GetSpecialReceiver()
{
	CPermission *pPermission = NULL;
	for (int i=0;i<GetSize();i++) 
	{
		pPermission = GetAtFast(i);
		if (pPermission->IsSpecialReceiver())
		{
			return pPermission;		// EXIT
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented */
void CPermissionArray::DeleteAll()
{
	// save special permissions
	CPermission* pSuperVisor = GetSuperVisor();
	CPermission* pSpecialReceiver = GetSpecialReceiver();
	CPermission* pPermission = NULL;
	for (int i=1;i<GetSize();i++) 
	{
		pPermission = GetAtFast(i);
		if (pPermission && pPermission != pSpecialReceiver)
		{ 
			delete GetAtFast(i); 
		} 
	} 
	RemoveAll(); 
	// add saved special permissions
	Add(pSuperVisor);
	if (pSpecialReceiver)
	{
		Add(pSpecialReceiver);
	}
}
/////////////////////////////////////////////////////////////////////////////
/*Function: Checks for a valid permission.
 Returns a valid permission 
 if the name is known 
 and @CW{sPassword} matches the local password or the
		local password is empty.
*/
CPermission* CPermissionArray::CheckPermission(const CConnectionRecord& c) const
{
	if (c.GetPermission() == szSpecialReceiver)
	{
		CString sPIN;

		// there must be a PIN field to prevent a security leak
		if (c.GetFieldValue(CSD_PIN,sPIN))
		{
			CPermission* pSpecialReceiver = new CPermission(CSecID(SECID_SPECIAL_RECEIVER_PERMISSION));
			return pSpecialReceiver;
		}
	}
	for (int i=0;i<GetSize();i++) 
	{
		CPermission *pOnePermission = GetAtFast(i);
		CString sOneName = pOnePermission->GetName();
		if (sOneName==c.GetPermission()) 
		{
			CString sOnePassword = pOnePermission->GetPassword();
			// same password or local password is empty
			if (   sOnePassword == c.GetPassword()
				|| sOnePassword.IsEmpty()) 
			{
				return pOnePermission;		// EXIT
			}
		}
	}
	return NULL;
}
//---------------------------------------------------------------------------
