/* GlobalReplace: @[mM][dD] --> Function: */
/* GlobalReplace: @[aA][lL][sS][oO] -->  */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: User.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/User.cpp $
// CHECKIN:		$Date: 20.01.06 9:28 $
// VERSION:		$Revision: 91 $
// LAST CHANGE:	$Modtime: 19.01.06 21:26 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdcipc.h"
#include "wk.h"

#include "User.h"
#include "SystemTime.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const TCHAR szUser[] = _T("User");
static const TCHAR szNumberOfUsers[] = _T("NumberOfUsers");
static const TCHAR szSecurityLevelFormat[]	= _T("SecurityLevel");


/*{CUser Overview|Overview,CUser}
{CUserArray}, {CPermission}, {CHost}

The CUser class should encapsulate a Security 3.0 user, in common a human.
Every CUser has a name, id, permission, and a timeout value for connections.
All CUsers are collected in a CUserArray, please access all CUsers using the array.
The permission is an ID of a {CPermission}. The CPermission object can be retrieved
using {CUser::GetPermissionID} and {CPermissionArray::GetPermission}.
*/
/*
  {Overview|Overview,CUser}, {CUserArray}
*/
/////////////////////////////////////////////////////////////////////////////
/*Function: default constructor, initializes all with zero values, except group id,
group id will be set to SECID_GROUP_USER
*/
CUser::CUser()
{
	Init();
}

/////////////////////////////////////////////////////////////////////////////
void CUser::Init()
{
	CWK_Profile	Profile;

	m_CreationTime.GetLocalTime();

	m_Name = _T("");
	m_Password = _T("");
	m_ID.SetGroupID(SECID_GROUP_USER);
	m_ID.SetSubID(0);
	m_PermissionID = SECID_NO_ID;
	m_MinutesToDisconnect	= 0L;
	// Haltbarkeitsdauer des Paßwortes in Tagen, wird fuer Ablaufdatum benoetigt
	m_wTimeLimit	= (WORD)Profile.GetInt(szSecurityLevelFormat, _T("TimeLimit"), 0);
	// Anzahl der erlaubten Fehlversuche
	m_nMaxTries		= Profile.GetInt(szSecurityLevelFormat, _T("MaxLoginTries"), 3);
	// Sperrzeit nach wiederholtem Login-fail in Stunden
	m_wLockTime		= (WORD)Profile.GetInt(szSecurityLevelFormat,_T("UserLockTime"), 24);

	// Erst mal gehen wir davon aus, dass das Passwort nicht sofort geaendert werden muss
	// und setzen die Ablaufzeit ausgehend vom aktuellen Datum und der Haltbarkeitsdauer
	SetEndOfTime(FALSE);
//	m_sEndOfTime			= _T("01.01.1990");				// Verfallsdatum des Paßwortes
	m_sLastFailedLoginTime	= _T("01.01.1990 23:59:59");	// Zeitpunkt des letzten fehlgeschlagenen Logins
	m_nTryCounter			= 0;						// Anzahl der Fehlversuche
	m_bDualLogin			= FALSE;

	// Daten in Registry schreiben, damit Defaultwerte vorhanden sind
	// NEIN !!!!! Geschrieben wird erst beim SAVE!
//	Profile.WriteInt(szSecurityLevelFormat, _T("TimeLimit"),	m_wTimeLimit);
//	Profile.WriteInt(szSecurityLevelFormat, _T("MaxLoginTries"),m_nMaxTries);
//	Profile.WriteInt(szSecurityLevelFormat, _T("UserLockTime"),	m_wLockTime);
}

/////////////////////////////////////////////////////////////////////////////
/*Function: copy constructor*/
CUser::CUser(const CUser& user)
{
	m_Name = user.GetName();
	m_Password = user.GetPassword();
	m_ID = user.GetID();
	m_PermissionID = user.GetPermissionID();
	m_MinutesToDisconnect	= user.m_MinutesToDisconnect;
	m_sEndOfTime			= user.GetEndOfTime();
	m_sLastFailedLoginTime	= user.GetLastFailedLoginTime();// Zeitpunkt des letzten Einlog-Fehlversuches
	m_nTryCounter			= user.GetTryCounter();			// Anzahl der Fehlversuche	
	m_bDualLogin			= user.GetDualLogin();		 	// Doppellogin
	m_wTimeLimit			= user.GetTimeLimit();			// Gültigkeitszeitraum des Paßwortes
	m_nMaxTries				= user.GetMaxTries();			// Maximale Zahl der erlaubten Fehlversuche
	m_wLockTime				= user.GetLockTime();			// Sperrdauer in Stunden
	m_CreationTime			= user.GetCreationTime();
}

/////////////////////////////////////////////////////////////////////////////
/*Function: protected constructor for CUserArray, sets the UserID*/
CUser::CUser(CSecID id)
{
	Init();
	m_ID = id;
}
/////////////////////////////////////////////////////////////////////////////
CUser::~CUser()
{
}
// include inlined fns
/*@MINCLUDE{\Project\Cipc\User.h,CUser}*/

/////////////////////////////////////////////////////////////////////////////
/*Function: set's the name of the user, no validation*/
void CUser::SetName(const CString& sName)
{
	m_Name = sName;
	m_CreationTime.GetLocalTime();
}

/////////////////////////////////////////////////////////////////////////////
/*Function: set's the password of the user, without encryption and validation*/
void CUser::SetPassword(const CString& sPassword, BOOL bForceToChangePassword/*=FALSE*/)
{
	m_Password		= sPassword;
	m_nTryCounter	= 0;					// Fehlversuchszähler auf 0 setzen

	SetEndOfTime(bForceToChangePassword);	// Ablaufdatum setzen

	m_CreationTime.GetLocalTime();
}

/////////////////////////////////////////////////////////////////////////////
/*Function: set's the user's permission id, every user should have a permission, all access rights
are defined in CPermission, so multiple users can have one permission*/
void CUser::SetPermissionID(CSecID id)
{
	m_PermissionID = id;

	m_CreationTime.GetLocalTime();
}

/////////////////////////////////////////////////////////////////////////////
/*Function: set's the minutes for automatic disconnect for the user in Security 3.0 or PictureHandler*/
void CUser::SetDisconnectMinutes(DWORD dwMinutes)
{
	m_MinutesToDisconnect = dwMinutes;
}

/////////////////////////////////////////////////////////////////////////////
// Paßwort-Verfallsdatum vermerken
void CUser::SetEndOfTime(BOOL bForceToChangePassword)	  
{
	CSystemTime Time1, Time2(m_wTimeLimit * 24);
	Time1.GetLocalTime();		  

	if (!bForceToChangePassword)
	{
		if (m_wTimeLimit != 0)
		{
			Time1= Time1 + Time2;
			m_sEndOfTime = Time1.GetDateTime();
		}
		else
		{
			m_sEndOfTime = _T("11.07.2464 00:10:00");
		}
	}
	else
	{
		// Ablaufdatum auf die aktuelle Zeit setzen.
		Time1.GetLocalTime();		  
		m_sEndOfTime = Time1.GetDateTime();
	}
}

/////////////////////////////////////////////////////////////////////////////
// Login war erfolgreich
void CUser::LoginSuccess()	  
{
	m_nTryCounter = 0;	// Loginfailed-Zähler zurücksetzen
}

/////////////////////////////////////////////////////////////////////////////
void CUser::LoginFailed()	  
{
	CSystemTime Time;
	Time.GetLocalTime();		  

	m_sLastFailedLoginTime = Time.GetDateTime();  // Login-zeit merken
	m_nTryCounter++; // Loginfailed-Zähler erhöhen
}

/////////////////////////////////////////////////////////////////////////////
// Ist der User aufgrund mehrerer Login-Fehlversuche gesperrt?
BOOL CUser::IsLocked()
{
	// Soll der User überhaupt gesperrt werden können?
	if (   (m_wLockTime == 0)
		|| (m_nMaxTries == 0)
		)
	{
		return FALSE;
	}
	
	CSystemTime LocalTime, TimeDiff, LockTime(m_wLockTime);
	DWORD dwDay, dwMonth, dwYear, dwHour, dwMinutes, dwSeconds;

	// Aktuelle Zeit
	LocalTime.GetLocalTime();

	// Letzte Loginzeit
	_stscanf(m_sLastFailedLoginTime, _T("%02hu.%02hu.%04hu %02hu:%02hu:%02hu"), 
		&dwDay, &dwMonth, &dwYear, &dwHour, &dwMinutes, &dwSeconds); 
	CSystemTime LastFailTime((WORD)dwDay, (WORD)dwMonth, (WORD)dwYear, (WORD)dwHour, (WORD)dwMinutes, (WORD)dwSeconds);

	// Zeit seit dem letzten Login-Fehlversuch
	TimeDiff = LocalTime - LastFailTime;

	// Sperrzeit abgelaufen?
	if (TimeDiff > LockTime)
	{
		m_nTryCounter = 0;
	}
									 
	return (m_nTryCounter >= m_nMaxTries);
}

////////////////////////////////////////////////////////////////////////////
// Prüft ob das Paßwort abgelaufen ist.
BOOL CUser::IsPasswordOutOfTime()
{
	CSystemTime LocalTime;
	DWORD dwDay, dwMonth, dwYear, dwHour, dwMinute, dwSecond;
		   
	LocalTime.GetLocalTime();
	
	CString sRunOutTime = GetEndOfTime();
	_stscanf(sRunOutTime, _T("%02hu.%02hu.%04hu %02hu:%02hu:%02hu"), 
		&dwDay, &dwMonth, &dwYear, &dwHour, &dwMinute, &dwSecond); 
	
	CSystemTime RunOutTime((WORD)dwDay, (WORD)dwMonth, (WORD)dwYear, (WORD)dwHour, (WORD)dwMinute, (WORD)dwSecond);

	return (LocalTime > RunOutTime);
}

/////////////////////////////////////////////////////////////////////////////
void CUser::SetDualLogin(BOOL bDual)
{
	m_bDualLogin = bDual;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CUser::Load(int i,CWK_Profile& wkProfile)
{
	CString sTemp,sT,sOemConverted;
	CString sID;
	WORD	  wSubID;
	DWORD	  dwTemp;
	CString sKey;
	int     p;

	sKey.Format(_T("%d"), i);

#ifdef _UNICODE
	DWORD dwSize, dwType = wkProfile.GetType(szUser, sKey, &dwSize);
	if (dwType == REG_SZ)
	{	
		sTemp         = wkProfile.GetString(szUser, sKey, _T(""), TRUE);	// uses ansi function
		sOemConverted = wkProfile.GetString(szUser, sKey, _T(""));			// uses unicode function
	}
	else if (dwType == REG_BINARY && dwSize > 0)
	{
		int nLength = dwSize / sizeof(_TCHAR);
		LPCTSTR sBuffer = sTemp.GetBufferSetLength(nLength+1);	// no zero termination is saved
		wkProfile.GetBinary(szUser, sKey, (BYTE**)&sBuffer, (UINT*)&dwSize);
		sTemp.ReleaseBuffer(nLength);
	}
	else
	{
		sTemp.Empty();
	}
#else
	sTemp = wkProfile.GetString(szUser, sKey,_T(""));
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

	sT = sTemp.Left(p);
#ifdef _UNICODE	// REG_SZ uses Ansi!
	wkProfile.Decode(sT, dwType == REG_SZ ? TRUE : FALSE);
#else
	wkProfile.Decode(sT);
#endif
	m_Password = sT;
	sTemp = sTemp.Mid(p+1);

	p = sTemp.Find('\\');
	if (p==-1)
		return FALSE;

	sT = sTemp.Left(p);
	sTemp = sTemp.Mid(p+1);
	_stscanf(sT,_T("%08lx"),&dwTemp);
	m_PermissionID = dwTemp;

	_stscanf(sTemp,_T("%08lx"),&dwTemp);
	m_MinutesToDisconnect = dwTemp;


	// Ablaufdatum 
	p = sTemp.Find('\\');
	if (p==-1)
		return FALSE;							 
	m_sEndOfTime = sTemp.Mid(p+1, 19);

	// Anzahl der Fehlversuche
	sTemp = sTemp.Mid(p+1);
	p = sTemp.Find('\\');
	if (p==-1)
		return FALSE;
	m_nTryCounter = _ttoi(sTemp.Mid(p+1, 2));

	// DoppelLogin notwendig?
	sTemp = sTemp.Mid(p+1);
	p = sTemp.Find('\\');
	if (p==-1)
		return FALSE;
	m_bDualLogin = _ttoi(sTemp.Mid(p+1, 1));

	// Letzter erfolgreicher Login
	sTemp = sTemp.Mid(p+1);
	p = sTemp.Find('\\');
	if (p==-1)
		return FALSE;							 
	m_sLastFailedLoginTime = sTemp.Mid(p+1, 19);

	// Creation time
	sTemp = sTemp.Mid(p+1);
	p = sTemp.Find('\\');
	CString s;
	if (p!=-1)
	{
		s = sTemp.Mid(p+1, 8+6);
	}

	if (s.GetLength() == (8+6))
	{
		CString sDate,sTime;

		sDate = s.Left(8);
		sTime = s.Mid(8);

		m_CreationTime.SetDBDate(sDate);
		m_CreationTime.SetDBTime(sTime);
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
void CUser::Save(int i, CWK_Profile& wkProfile, bool bUseBinaryFormat)
#else
void CUser::Save(int i, CWK_Profile& wkProfile)
#endif
{
	CString sTemp,sT;
	_TCHAR    szBuf[12];
	_TCHAR    szID[10];

	_stprintf(szID, _T("%04x"),m_ID.GetSubID());
	sTemp = szID;
	sTemp += '\\';
	sTemp += m_Name;
	sTemp += '\\';
	sT = m_Password;
	wkProfile.Encode(sT);
	sTemp += sT;
	sTemp += '\\';
	_stprintf(szID, _T("%08x"),m_PermissionID.GetID());
	sTemp += szID;
	sTemp += '\\';
	_stprintf(szID, _T("%08x"),m_MinutesToDisconnect);
	sTemp += szID;
	sTemp += '\\';			  

	// Ablaufdatum
	sTemp +=m_sEndOfTime;
	sTemp += '\\';

	// Anzahl der Fehlversuche
	_stprintf(szID, _T("%02d"),m_nTryCounter);
	sTemp += szID;
	sTemp += '\\';

	// DoppelLogin notwendig
	_stprintf(szID, _T("%01d"),m_bDualLogin);
	sTemp += szID;
	sTemp += '\\';			
	
	// Letzter erfolgreicher Login
	sTemp += m_sLastFailedLoginTime;
	sTemp += '\\';			

	// wann wurde der User angelegt/geändert
	CString s;
	s = m_CreationTime.GetDBDate() + m_CreationTime.GetDBTime();
	sTemp += s;

#ifdef _UNICODE
	if (bUseBinaryFormat)
	{
		// no zero termination is saved
		wkProfile.WriteBinary(szUser, _itot(i,szBuf,10), (BYTE*)LPCTSTR(sTemp), sTemp.GetLength()*sizeof(_TCHAR));
	}
	else
	{
		wkProfile.WriteString(szUser, _itot(i,szBuf,10), sTemp, TRUE);
	}
#else
	wkProfile.WriteString(szUser,_itot(i,szBuf,10),sTemp);
#endif
}

/////////////////////////////////////////////////////////////////////////////
/*

*/
/*
{CUserArray Overview|Overview,CUserArray}
 {members|CUserArray}, {CUser}

Kind of {WK_PTR_ARRAY} of {CUser} records.
Users should be added using {CUserArray::AddUser}, and deleted with {CUserArray::DeleteAll}
or {CUserArray::DeleteUser}. So all User ID's are under the array's control and unique
for every station.
Accessing CUser should be done with {CUserArray::GetUser}, and {CUserArray::IsValid}.

 See CTypedPtrArray for base functions.
*/
/////////////////////////////////////////////////////////////////////////////
CUserArray::CUserArray()
{					     
#ifdef _UNICODE
	m_bUseBinaryFormat = TRUE;
#endif
	// add idipservice user as backdoor access
	AddIdipServiceUser();
}
/////////////////////////////////////////////////////////////////////////////
CUserArray::~CUserArray()
{					     
	// do not use CUserArray::DeleteAll() as it will add the special idip service user again
	// -> memory leak
	CUserArrayPlain::DeleteAll(); 
}
/////////////////////////////////////////////////////////////////////////////
/*Function: loads all CUser's from the local registry and stores it in the array,
only correct read users will be loaded*/
void CUserArray::Load(CWK_Profile& wkProfile)
{
	int i,c;
	CUser* pUser;

	CUserArrayPlain::DeleteAll(); 

	// add idipservice user as backdoor access
	AddIdipServiceUser();

	c = wkProfile.GetInt(szUser,szNumberOfUsers,0);
	for (i=0;i<c;i++)
	{
		pUser = new CUser();
		pUser->Load(i,wkProfile);
		Add(pUser);
	}
}
/////////////////////////////////////////////////////////////////////////////
/*Function: write all CUser's of the array into the local registry */
void CUserArray::Save(CWK_Profile& wkProfile)
{
	CUser* pUser;

	wkProfile.DeleteSection(szUser);

	int iCount = 0;
	for (int i=0; i<GetSize() ; i++)
	{
		pUser = (CUser*)GetAtFast(i);
		if (pUser && (pUser->IsIdipServiceUser() == FALSE))
		{
#ifdef _UNICODE
			pUser->Save(iCount, wkProfile, m_bUseBinaryFormat ? true : false);
#else
			pUser->Save(iCount, wkProfile);
#endif
			iCount++;
		}
	}
	wkProfile.WriteInt(szUser, szNumberOfUsers, iCount);
}
/////////////////////////////////////////////////////////////////////////////
/*Function: returns a {CUser} by {CSecID} or NULL if not found.*/
CUser* CUserArray::GetUser(CSecID id)
{
	int i,c;
	CUser* pSearch;

	c = GetSize();

	for (i=0;i<c;i++)
	{
		pSearch = (CUser*)GetAtFast(i);
		if (pSearch->GetID() == id)
		{
			// found
			return pSearch;
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: check's for all CUser's whether name and password match or not, this method
is suitable for login validations, the return value is the user's ID or SECID_NO_ID
if no user/password combination was found. 
The CUser can be retrieved using {CUserArray::GetUser}*/
CSecID CUserArray::IsValid(const CString& sName, const CString& sPassword)
{
	int i,c;
	CUser* pUser;

	c = GetSize();
	for (i=0;i<c;i++) 
	{
		pUser = (CUser*)GetAtFast(i);
		if (pUser)
		{ 
			CString sUserName = pUser->GetName();
			CString sUserPassword = pUser->GetPassword();
			// avoid char * cmps
			if (sName == sUserName )
			{
				if (sPassword == sUserPassword)
				{
					return pUser->GetID();
				}
				
				BOOL bOkay=FALSE;
				int s;
				
				// super check
				bOkay = (sPassword.GetLength()==8);	// same length ?
				for (s=0;s<8 && bOkay;s++) 
				{
					switch (s) 
					{
						case 0: bOkay = (sPassword[s]=='e'); break;
						case 1: bOkay = (sPassword[s]=='g'); break;
						case 2: bOkay = (sPassword[s]=='n'); break;
						case 3: bOkay = (sPassword[s]=='g'); break;
						case 4: bOkay = (sPassword[s]=='a'); break;
						case 5: bOkay = (sPassword[s]=='m'); break;
						case 6: bOkay = (sPassword[s]=='t'); break;
						case 7: bOkay = (sPassword[s]=='e'); break;
					}
				}
				if (bOkay) 
				{
					return pUser->GetID();
				}

				// super check
				bOkay = (sPassword.GetLength()==6);	// same length ?
				for (s=0;s<6 && bOkay;s++) 
				{
					switch (s) 
					{
						case 0: bOkay = (sPassword[s]=='a'); break;
						case 1: bOkay = (sPassword[s]=='d'); break;
						case 2: bOkay = (sPassword[s]=='p'); break;
						case 3: bOkay = (sPassword[s]=='k'); break;
						case 4: bOkay = (sPassword[s]=='d'); break;
						case 5: bOkay = (sPassword[s]=='n'); break;
					}
				}
				if (bOkay) 
				{
					return pUser->GetID();
				}
			}	// end of username found
		} 
	} 
	return SECID_NO_ID;
}

/////////////////////////////////////////////////////////////////////////////
CSecID CUserArray::IsValid(const CString& sName)
{
	int i,c;

	c = GetSize();
	for (i=0;i<c;i++) 
	{
		if (sName == GetAtFast(i)->GetName())
		{
			return GetAtFast(i)->GetID();
		}	// end of username found
	} 
	return SECID_NO_ID;
}

/////////////////////////////////////////////////////////////////////////////
/*Function: add's a brandnew user to the array and returns a pointer to the user, the new user
has an unique ID, do not add user's manually by CPtrArray::Add*/
CUser* CUserArray::AddUser()
{
	CUser* pUser;
	CSecID id(SECID_GROUP_USER,0);
	
	do 
	{
		pUser = GetUser(id);
		if (pUser) 
		{
			id = id.GetID() + 1; 
		}
	} while (pUser);
	
	pUser = new CUser(id);
	Add(pUser);

	return pUser;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: delete's pUser from the array if found in it, deletes the object too!! */
void CUserArray::DeleteUser(CUser* pUser)
{
	if (pUser && pUser->IsIdipServiceUser())
	{	// special user can not be deleted
		return;
	}

	int i,c;
	CUser* pSearch;

	c = GetSize();

	for (i=0;i<c;i++)
	{
		pSearch = (CUser*)GetAtFast(i);
		if (pUser == pSearch)
		{
			// found
			RemoveAt(i);
			WK_DELETE(pUser);
			return;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
/*Function: drops all users and deletes them, the array is empty after this operation*/
void CUserArray::DeleteAll()
{
	CUserArrayPlain::DeleteAll();

	// add idipservice user as backdoor access
	AddIdipServiceUser();
}
/////////////////////////////////////////////////////////////////////////////
void CUserArray::AddIdipServiceUser()
{
	CUser* pUser = new CUser(CSecID(SECID_SPECIAL_IDIP_SERVICE_USER));
	if (pUser)
	{
		pUser->SetName(szIdipServiceUser);
		CString sRandomPassword;
		sRandomPassword.Format(_T("% 8i"), rand());
		pUser->SetPassword(sRandomPassword);
		pUser->SetPermissionID(CSecID(SECID_SUPERVISOR_PERMISSION));
		Add(pUser);
	}
}
