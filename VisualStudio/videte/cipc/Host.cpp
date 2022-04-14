/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: Host.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/Host.cpp $
// CHECKIN:		$Date: 20.01.06 9:28 $
// VERSION:		$Revision: 85 $
// LAST CHANGE:	$Modtime: 19.01.06 21:26 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdcipc.h"

#include "wk.h"

#include "Host.h"
#include "Permission.h"
#include "CipcServerControl.h"
#include "ArchivInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const TCHAR szHost[] = _T("Hosts");
static const TCHAR szNumberOfHosts[] = _T("NumberOfHosts");

static const TCHAR szTCP[] = _T("tcp:");
static const TCHAR szDUN[] = _T("dun:");
static const TCHAR szPRES[] = _T("pres:");
static const TCHAR szB3[] = _T("b3:");
static const TCHAR szVCS[] = _T("pres:");
static const TCHAR szB6[] = _T("b6:");
static const TCHAR szTOBS[] = _T("tobs:");
static const TCHAR szSMS[] = _T("sms:");
static const TCHAR szEMail[] = _T("mailto:");
static const TCHAR szFAX[] = _T("fax:");
static const TCHAR szNumberChars[] = _T("01234567890 /-()+");
static const TCHAR szDigits[] = _T("1234567890");
//

/* CHost_Overview|
 <c CHost>, <c CHostArray> 

The CHost class encapsulates a Security 3.0 host, 
that is another computer at the end of an ISDN or network connection.
Every CHost has a name, number and id. All CHosts are collected in a 
CHostArray, one should access all CHosts using the array.
*/

/////////////////////////////////////////////////////////////////////////////
/*Function: default constructor, initializes all with zero values, except group id,
group id will be set to SECID_GROUP_HOST
*/
/////////////////////////////////////////////////////////////////////////////
CHost::CHost()
{
	Init();
}
/////////////////////////////////////////////////////////////////////////////
// Function: NYD
CHost::CHost(LPCTSTR pszName, LPCTSTR pszNumber)
{
	Init();
	m_sName = pszName;
	m_sNumber = pszNumber;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: protected constructor for CHostArray, sets the UserID*/
CHost::CHost(CSecID id)
{
	Init();
	m_ID = id;
}
/////////////////////////////////////////////////////////////////////////////
void CHost::Init()
{
//	m_sName;
//	m_sNumber;
	m_ID.SetGroupID(SECID_GROUP_HOST);
	m_ID.SetSubID(0);
	m_bPINRequired = FALSE;
//	m_sPIN;
	m_dwAutoDisconnectTime = 0;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented*/
CHost::~CHost()
{
}

/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented*/
void CHost::SetName(const CString& sName)
{
	m_sName = sName;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented*/
void CHost::SetNumber(const CString& sNumber)
{
	m_sNumber = sNumber;
}
/////////////////////////////////////////////////////////////////////////////
const CString CHost::GetLastNumber() const
{
	int p = m_sNumber.ReverseFind(ROUTING_CHAR);
	CString s;

	if (p==-1)
	{
		s = m_sNumber;
	}
	else
	{
		s = m_sNumber.Mid(p+1);
	}
	return s;
}
/////////////////////////////////////////////////////////////////////////////
CString CHost::GetCleanedUpNumber(const CString& sCheck)
{
	CStringArray sa;
	SplitRouteFromString(sCheck,sa);
	CString sRet,sOne,sPrefix,sNumber;

	for (int i=0;i<sa.GetSize();i++)
	{
		sOne = sa.GetAt(i);
		//Anpassen bei neuen Link-Modules
		if (IsTCPIP(sOne) || IsDUN(sOne))
		{
			// nothing to do
		}
		else 
		{
			int p = sOne.Find(':');
			if (p!=-1)
			{
				sPrefix = sOne.Left(p+1);
				sNumber = sOne.Mid(p+1);
			}
			else
			{
				sPrefix.Empty();
				sNumber = sOne;
			}
			sOne = sPrefix + StringOnlyCharsInSet(sNumber,szDigits);
		}

		if (!sRet.IsEmpty())
		{
			sRet += ROUTING_CHAR;
		}
		sRet += sOne;
	}

	return sRet;
}
/////////////////////////////////////////////////////////////////////////////
const CString CHost::GetCleanedUpNumber() const
{
	return GetCleanedUpNumber(m_sNumber);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHost::IsISDN() const
{
	return IsISDN(GetLastNumber());
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHost::IsTCPIP() const
{
	return IsTCPIP(GetLastNumber());
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHost::IsDUN() const
{
	return IsDUN(GetLastNumber());
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHost::IsB3() const
{
	return IsB3(GetLastNumber());
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHost::IsB6() const
{
	return IsB6(GetLastNumber());
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHost::IsSMS() const
{
	return IsSMS(GetLastNumber());
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHost::IsEMail() const
{
	return IsEMail(GetLastNumber());
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHost::IsFAX() const
{
	return IsFAX(GetLastNumber());
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHost::IsTOBS() const
{
	return IsTOBS(GetLastNumber());
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHost::IsISDN(const CString& sNumber)
{
	if (sNumber.IsEmpty())
	{
		return FALSE;
	}
	else
	{
		return isdigit(sNumber[0]);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHost::IsTCPIP(const CString& sNumber)
{
	return 0==sNumber.Find(szTCP);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHost::IsDUN(const CString& sNumber)
{
	return 0==sNumber.Find(szDUN);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHost::IsB3(const CString& sNumber)
{
	return    (0==sNumber.Find(szPRES)) 
		   || (0==sNumber.Find(szB3));
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHost::IsB6(const CString& sNumber)
{
	return    (0==sNumber.Find(szVCS)) 
		   || (0==sNumber.Find(szB6));
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHost::IsSMS(const CString& sNumber)
{
	return 0 == sNumber.Find(szSMS);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHost::IsEMail(const CString& sNumber)
{
	return 0 == sNumber.Find(szEMail);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHost::IsFAX(const CString& sNumber)
{
	return 0 == sNumber.Find(szFAX);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHost::IsTOBS(const CString& sNumber)
{
	return 0 == sNumber.Find(szTOBS);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHost::IsRouting() const
{
	return -1!=m_sNumber.Find(ROUTING_CHAR);
}
////////////////////////////////////////////////////////////////////////////
// the following routing combinations
// are valid
//
// TCP  SMS
// TCP  ISDN
// TCP  B3
// TCP  B6
// TCP  ISDN TCP
// TCP  Tobs
// TCP  Email
// TCP  Fax
// ISDN	TCP
// ISDN TCP  ISDN
// ISDN TCP  SMS
// ISDN TCP  B3
// ISDN TCP  B6
//
BOOL CHost::IsRoutingValid() const
{
	BOOL bRet = TRUE;
	CStringArray sa;

	SplitRouteFromString(m_sNumber,sa);

	int c = sa.GetSize();

	if (   (c<2)
		|| (c>3)
	   )
	{
		bRet = FALSE;
	}
	else
	{

		for (int i=0;i<c;i++)
		{
			if (!IsNumberValid(sa[i]))
			{
				bRet = FALSE;
				break;
			}
		}

		if (bRet)
		{
			// check the first routing host
			// must be isdn or tcp
			if (IsTCPIP(sa[0]))
			{
				// the second must not be tcp
				if (IsTCPIP(sa[1]))
				{
					bRet = TRUE;
				}
				else
				{
					if (c==2)
					{
						bRet = TRUE;
					}
					else
					{
						bRet = IsISDN(sa[1]) && IsTCPIP(sa[2]);
					}
				}
			}
			else if (IsISDN(sa[0]))
			{
				// the second must be tcp
				if (!IsTCPIP(sa[1]))
				{
					bRet = FALSE;
				}
				else
				{
					if (c==2)
					{
						bRet = TRUE;
					}
					else
					{
						bRet = !IsTCPIP(sa[2]);
					}
				}
			}
			else
			{
				bRet = FALSE;
			}
		}
	}

	return bRet;
}
////////////////////////////////////////////////////////////////////////////
BOOL CHost::IsNumberValid(const CString& sNumber)
{
	int p = sNumber.Find(':');
	if (p==-1)
	{
		// should be ISDN and only numbers
		return StringContainsOnly(sNumber, szNumberChars);
	}
	CString sTyp = sNumber.Left(p+1);
	CString s = sNumber.Mid(p+1);
	if (   (sTyp == szTCP)
		|| (sTyp == szEMail)
		|| (sTyp == szDUN)
		)
	{
		// should be TCP and everything is allowed except ! and #
		return StringContainsNo(s,_T("#!\\"));
	}
	else if (   (sTyp == szPRES)
			 || (sTyp == szB3)
			 || (sTyp == szTOBS)
			 || (sTyp == szFAX)
			 || (sTyp == szVCS)
			 || (sTyp == szB6)
			 || (sTyp == szSMS)
			 )
	{
		return StringContainsOnly(s,szNumberChars);
	}

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////
BOOL CHost::IsValid() const
{
	if (m_sNumber.IsEmpty())
	{
		return FALSE;
	}
	if (IsRouting())
	{
		return IsRoutingValid();
	}
	else
	{
		return IsNumberValid(m_sNumber);
	}
}
////////////////////////////////////////////////////////////////////////////
// Function: NYD
const CString CHost::GetTyp() const
{
	CString r;

	if (IsTCPIP())
	{
		r = _T("TCP/IP");
	}
	else if (IsSMS())
	{
		r = _T("SMS");
	}
	else if (IsB3())
	{
		r = _T("PT200");
	}
	else if (IsB6())
	{
		r = _T("MIDI B6");
	}
	else if (IsTOBS())
	{
		r = _T("TeleObserver");
	}
	else if (IsFAX())
	{
		r = _T("FAX");
	}
	else if (IsEMail())
	{
		r = _T("E-Mail");
	}
	else if (IsDUN())
	{
		r = _T("DUN");
	}
	else
	{
		r = _T("ISDN");
	}

	return r;
}
/////////////////////////////////////////////////////////////////////////////
// Function: NYD
const CString CHost::GetClearNumber() const
{
	CString r;
	int p;

	p = m_sNumber.Find(ROUTING_CHAR);
	if (p==-1)
	{
		p = m_sNumber.Find(':');
		if (p!=-1)
		{
			r = m_sNumber.Mid(p+1);
		}
		else
		{
			r = m_sNumber;
		}
	}
	else
	{
		r = m_sNumber;
	}
	return r;
}
/////////////////////////////////////////////////////////////////////////////
// Function: NYD
BOOL CHost::Load(int i,CWK_Profile& wkp)
{
	CString sTemp, sID, sOemConverted;
	WORD	  wSubID;
	CString sKey;
	int	  p;

	sKey.Format(_T("%d"), i);

#ifdef _UNICODE
	DWORD dwSize, dwType = wkp.GetType(szHost, sKey, &dwSize);
	if (dwType == REG_SZ)
	{
		sTemp         = wkp.GetString(szHost, sKey, _T(""), TRUE);	// uses ansi function
		sOemConverted = wkp.GetString(szHost, sKey, _T(""));	// uses unicode function
	}
	else if (dwType == REG_BINARY && dwSize > 0)
	{
		int nLength = dwSize / sizeof(_TCHAR);
		LPCTSTR sBuffer = sTemp.GetBufferSetLength(nLength+1);	// no zero termination is saved
		wkp.GetBinary(szHost, sKey, (BYTE**)&sBuffer, (UINT*)&dwSize);
		sTemp.ReleaseBuffer(nLength);
	}
	else
	{
		sTemp.Empty();
	}
#else
	sTemp = wkp.GetString(szHost, sKey,_T(""));
#endif

	if (sTemp.IsEmpty())
		return FALSE;

	if (_istdigit(sTemp[0]))
	{
		// old style
		p = sTemp.Find('\\');
		if (p==-1)
			return FALSE;
		 
		sID   = sTemp.Left(p);
		DWORD dwTmp;
		_stscanf(sID,_T("%04lx"),&dwTmp);
		wSubID = (WORD) dwTmp;
		m_ID.SetSubID(wSubID);
		sTemp = sTemp.Mid(p+1);

		p = sTemp.Find('\\');
		if (p==-1)
			return FALSE;
		m_sName = sTemp.Left(p);
		m_sNumber = sTemp.Mid(p+1);

		CArchivInfoArray archives;
		
		archives.Load(wkp);
		for (int i=0;i<archives.GetSize();i++)
		{
			CArchivInfo* pArchivInfo = archives.GetAtFast(i);
			if (pArchivInfo->GetName() == (m_sName + _T(" Suche")))
			{
				m_idSearchArchive = pArchivInfo->GetID();
			}
			else if (pArchivInfo->GetName() == (m_sName + _T(" Normal")))
			{
				m_idNormalArchive = pArchivInfo->GetID();
			}
			else if (pArchivInfo->GetName() == (m_sName + _T(" Alarm")))
			{
				m_idAlarmArchive = pArchivInfo->GetID();
			}
		}
	}
	else
	{
		m_ID = (DWORD)wkp.GetHexFromString(sTemp,_T("\\ID"),SECID_NO_ID);
		if (sOemConverted.IsEmpty())
		{
			m_sName = wkp.GetStringFromString(sTemp,_T("\\CO"),_T(""));
		}
		else
		{
			m_sName = wkp.GetStringFromString(sOemConverted,_T("\\CO"),_T(""));
		}
		m_sNumber = wkp.GetStringFromString(sTemp,_T("\\ADRESS"),_T(""));

		m_idAlarmArchive = (DWORD)wkp.GetHexFromString(sTemp,_T("\\AA"),SECID_NO_ID);
		m_idSearchArchive = (DWORD)wkp.GetHexFromString(sTemp,_T("\\SA"),SECID_NO_ID);
		m_idNormalArchive = (DWORD)wkp.GetHexFromString(sTemp,_T("\\NA"),SECID_NO_ID);

		m_bPINRequired = (BOOL)wkp.GetNrFromString(sTemp,_T("\\PR"),0);
		m_dwAutoDisconnectTime = wkp.GetNrFromString(sTemp,_T("ADT"),0);
		CString sPin = wkp.GetStringFromString(sTemp,_T("\\PI"),_T(""));
#ifdef _UNICODE
		wkp.Decode(sPin, dwType == REG_SZ ? TRUE : FALSE);
#else
		wkp.Decode(sPin);
#endif
		m_sPIN = sPin;
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// Function: NYD
#ifdef _UNICODE
void CHost::Save(int i, CWK_Profile& wkp, bool bUseBinaryFormat)
#else
void CHost::Save(int i, CWK_Profile& wkp)
#endif
{
	// only save new style
	CString sValue;
	CString sKey;

	CString sPin = m_sPIN;
	wkp.Encode(sPin);

	sKey.Format(_T("%d"),i);
	sValue.Format(_T("\\ID%08lx\\CO%s\\ADRESS%s\\AA%08lx\\SA%08lx\\NA%08lx\\PR%d\\ADT%d\\PI%s"),
		m_ID.GetID(),
		m_sName,
		m_sNumber,
		m_idAlarmArchive.GetID(),
		m_idSearchArchive.GetID(),
		m_idNormalArchive.GetID(),
		m_bPINRequired,
		m_dwAutoDisconnectTime,
		sPin);
#ifdef _UNICODE
	if (bUseBinaryFormat)
	{
		// no zero termination is saved
		wkp.WriteBinary(szHost, sKey, (BYTE*)LPCTSTR(sValue), sValue.GetLength()*sizeof(_TCHAR));
	}
	else
	{
		wkp.WriteString(szHost,sKey,sValue, TRUE);
	}
#else
	wkp.WriteString(szHost,sKey,sValue);
#endif

/*
	CString sTemp;
	char    szID[10];
	char    szBuf[12];

	_stprintf(szID, _T("%04x")),m_ID.GetSubID());
	sTemp = szID;
	sTemp += '\\';
	sTemp += m_sName;
	sTemp += '\\';
	sTemp += m_sNumber;

	wkProfile.WriteString(szHost,_itot(i,szBuf,10),sTemp);
	*/
}

// Function: 
// a static member, since the link units only have strings
//
// Splits a path into separate strings.
// If it's not a path, there is only one string
void CHost::SplitRouteFromString(const CString &sPathIn, CStringArray &result)
{
	result.RemoveAll();
	SplitString(sPathIn,result,ROUTING_CHAR);
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/*

 CHostArray_Overview|
 <c CHostArray>, <c CHost>

Kind of {WK_PTR_ARRAY} of {CHost} records.
Hosts should be added using {CHostArray::AddHost}, and deleted with 
<mf CHostArray.DeleteAll>
or <mf CHostArray.DeleteHost>. So all host ID's are under the array's control and unique
for every station.
Accessing CHost should be done with <mf CHostArray.GetHost>.

 See CTypedPtrArray for base functions.
*/
/////////////////////////////////////////////////////////////////////////////
// Function: NYD
CHostArray::CHostArray() : m_LocalHost(CSecID((DWORD)(SECID_LOCAL_HOST)))
{
#ifdef _UNICODE
	m_bUseBinaryFormat = TRUE;
#endif
}
/////////////////////////////////////////////////////////////////////////////
// Function: NYD
CHostArray::~CHostArray()
{
	DeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
// Function: NYD
LPCTSTR CHostArray::GetLocalHostNameStandard()
{
	return _T("Standard");
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented*/
void CHostArray::Load(CWK_Profile& wkProfile)
{
	DeleteAll();

	BOOL bOldFormat = FALSE;
	CString sTemp, sKey = _T("0");
#ifdef _UNICODE
	DWORD dwSize, dwType = wkProfile.GetType(szHost, sKey, &dwSize);
	if (dwType == REG_SZ) 
	{
		sTemp = wkProfile.GetString(szHost, sKey, _T(""), TRUE);
	}
	else if (dwType == REG_BINARY && dwSize > 0)
	{
		int nLength = dwSize / sizeof(_TCHAR);
		LPCTSTR sBuffer = sTemp.GetBufferSetLength(nLength+1);	// no zero termination is saved
		wkProfile.GetBinary(szHost, sKey, (BYTE**)&sBuffer, (UINT*)&dwSize);
		sTemp.ReleaseBuffer(nLength);
	}
	else
	{
		sTemp.Empty();
	}
#else
	sTemp = wkProfile.GetString(szHost, sKey, _T(""));
#endif
	if (!sTemp.IsEmpty())
	{
		if (_istdigit(sTemp[0]))
		{
			bOldFormat = TRUE;
		}
	}

	int c = wkProfile.GetInt(szHost,szNumberOfHosts,0);
	
	CHost* pHost;
	for (int i=0;i<c;i++)
	{
		pHost = new CHost();
		if (pHost->Load(i,wkProfile))
		{
			Add(pHost);
		}
		else
		{
			delete pHost;
		}

	}

	if (!m_LocalHost.Load(c, wkProfile))
	{
		CString sName = wkProfile.GetString(szHost,_T("OwnID"),GetLocalHostNameStandard());
		m_LocalHost.SetName(sName);
		m_LocalHost.SetNumber(_T("0"));
		WK_TRACE(_T("Local host %s initialized\n"), sName);
		Save(wkProfile);
		bOldFormat = FALSE; // already saved
	}

	if (bOldFormat)
	{
		WK_TRACE(_T("host format converted\n"));
		Save(wkProfile);
	}
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented*/
void CHostArray::Save(CWK_Profile& wkProfile)
{
	int i,c;
	CHost* pHost;

	c = GetSize();

	wkProfile.DeleteSection(szHost);
	
	wkProfile.WriteString(szHost, _T("OwnID"), m_LocalHost.GetName());

	wkProfile.WriteInt(szHost,szNumberOfHosts,c);

	for (i=0;i<c;i++)
	{
		pHost = GetAtFast(i);
		if (pHost)
		{
#ifdef _UNICODE
			pHost->Save(i,wkProfile, m_bUseBinaryFormat ? true : false);
#else
			pHost->Save(i,wkProfile);
#endif
		}
	}
#ifdef _UNICODE
	m_LocalHost.Save(c,wkProfile, m_bUseBinaryFormat? true : false);
#else
	m_LocalHost.Save(c,wkProfile);
#endif
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented*/
CHost* CHostArray::AddHost()
{
	CHost* pHost;
	CSecID id(SECID_GROUP_HOST,0);
	
	do 
	{
		pHost = GetHost(id);
		if (pHost) {
			id = id.GetID() + 1; 
		}
	} while (pHost);
	
	pHost = new CHost(id);
	Add(pHost);

	return pHost;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented*/
void CHostArray::DeleteHost(CHost* pHost)
{
	int i,c;
	CHost* pSearch;

	c = GetSize();

	for (i=0;i<c;i++)
	{
		pSearch = GetAtFast(i);
		if (pHost==pSearch)
		{
			// found
			RemoveAt(i);
			WK_DELETE(pHost);
			return;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented*/
CHost* CHostArray::GetHost(const CSecID& id)
{
	int i,c;
	CHost* pSearch;

	if (id==m_LocalHost.GetID())
	{
		return &m_LocalHost;
	}

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
/*Function: not yet documented*/
void CHostArray::Sort()
{
	int iCur, iMin, iNext, nCount;
	CHost* swap;
	
	nCount = GetSize();
	for (iCur=0; iCur<nCount; iCur++)
	{
		iMin = iCur;
		for (iNext=iCur; iNext<nCount; iNext++)
		{
			if (GetAtFast(iNext)->GetName() < GetAtFast(iMin)->GetName())
			{
				iMin = iNext;
			}
		}
		// wenn ein element kleiner als das aktuelle ist, tausche die elemente.
		if (iMin>iCur)
		{
			swap = GetAtFast(iCur);
			SetAt(iCur, GetAtFast(iMin));
			SetAt(iMin, swap);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CHostArray::Add(CHost*pHost)
{
	ASSERT(pHost !=	NULL);
	if (pHost->m_ID == SECID_NO_ID)
	{
		pHost->m_ID = CSecID(SECID_GROUP_HOST, (WORD)GetSize());
	}
	CPtrArray::Add(pHost);
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented*/
CHost* CHostArray::GetLocalHost()
{
	return &m_LocalHost;
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented*/
CString CHostArray::GetLocalHostName() const
{
	return m_LocalHost.GetName();
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented*/
CSecID CHostArray::GetLocalHostID() const
{
	return m_LocalHost.GetID();
}
/////////////////////////////////////////////////////////////////////////////
/*Function: not yet documented*/
void CHostArray::SetLocalHostName(CString sName)
{
	m_LocalHost.SetName(sName);
}

/*Function: Checks for a valid host. Either the hostname is known,
or the permission is open (has no password)
*/
BOOL CHostArray::CheckHost(const CString &sHost,
						   const CPermission *pPermission) const
{
	if (pPermission==NULL) {
		return FALSE;	// EXIT
	}

	if (GetLocalHostName()==sHost) {	// OOPS duplicate names ?
		return TRUE;
	}

	for (int i=0;i<GetSize();i++) 
	{ 
		CHost *pHost = GetAtFast(i);
		CString sName = pHost->GetName();
		if (sName==sHost) {
			return TRUE;	// EXIT host found
		}
	} 
	// host unknown
	CString sPassword = pPermission->GetPassword();
	if (sPassword.IsEmpty()) {
		// permission without passwd accept even unknown hosts
		return TRUE;		// EXIT
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////
CHost* CHostArray::GetHostForArchive(const CSecID& id)
{
	if (id==m_LocalHost.GetAlarmArchive())
	{
		return &m_LocalHost;
	}
	if (id==m_LocalHost.GetNormalArchive())
	{
		return &m_LocalHost;
	}
	if (id==m_LocalHost.GetSearchArchive())
	{
		return &m_LocalHost;
	}

	CHost* pSearch;

	for (int i=0;i<GetSize();i++)
	{
		pSearch = GetAtFast(i);
		if (pSearch->GetAlarmArchive()==id)
		{
			// found
			return pSearch;
		}
		if (pSearch->GetNormalArchive()==id)
		{
			// found
			return pSearch;
		}
		if (pSearch->GetSearchArchive()==id)
		{
			// found
			return pSearch;
		}
	}
	return NULL;
}
