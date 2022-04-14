// TimeZoneInformation.cpp: implementation of the CTimeZoneInformation class.
//
//////////////////////////////////////////////////////////////////////
#include "stdcipc.h"

#include "TimeZoneInformation.h"
#include "CIPCExtraMemory.h"
#include "wkclasses\wk_trace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTimeZoneInformation::CTimeZoneInformation()
{
	Bias = 0;

	ZeroMemory(&StandardName,sizeof(StandardName));
	StandardDate.wDay = 0;
	StandardDate.wDayOfWeek = 0;
	StandardDate.wHour = 0;
	StandardDate.wMilliseconds = 0;
	StandardDate.wMinute = 0;
	StandardDate.wMonth = 0;
	StandardDate.wSecond = 0;
	StandardDate.wYear = 0;
	StandardBias = 0;

	ZeroMemory(&DaylightName,sizeof(DaylightName));
	DaylightDate.wDay = 0;
	DaylightDate.wDayOfWeek = 0;
	DaylightDate.wHour = 0;
	DaylightDate.wMilliseconds = 0;
	DaylightDate.wMinute = 0;
	DaylightDate.wMonth = 0;
	DaylightDate.wSecond = 0;
	DaylightDate.wYear = 0;
	DaylightBias = 0;

	ZeroMemory(&DisplayName,sizeof(DisplayName));
}
//////////////////////////////////////////////////////////////////////
CTimeZoneInformation::CTimeZoneInformation(const TIME_ZONE_INFORMATION& tzi)
{
    Bias = tzi.Bias; 
	CopyMemory(&StandardName,&tzi.StandardName,sizeof(StandardName));
    StandardDate = tzi.StandardDate; 
    StandardBias = tzi.StandardBias; 
	CopyMemory(&DaylightName,&tzi.DaylightName,sizeof(DaylightName));
    DaylightDate = tzi.DaylightDate; 
    DaylightBias = tzi.DaylightBias; 
	// DisplayName
}
//////////////////////////////////////////////////////////////////////
CString CTimeZoneInformation::GetDisplayName()
{
	return DisplayName;
}

//////////////////////////////////////////////////////////////////////
void CTimeZoneInformation::SetDisplayName(const CString& sName)
{
	ZeroMemory(&DisplayName, sizeof(DisplayName));
	int iLen = sName.GetLength();
	if (iLen > sizeof(DisplayName))
	{
		WK_TRACE_WARNING(_T("CTimeZoneInformation::SetDisplayName Overflow\n"));
		iLen = sizeof(DisplayName);
	}
	for (int nI=0 ; nI<iLen ; nI++) 
		DisplayName[nI] = sName[nI];
}
//////////////////////////////////////////////////////////////////////
CTimeZoneInformation::~CTimeZoneInformation()
{
}

//////////////////////////////////////////////////////////////////////
DWORD CTimeZoneInformation::GetBubbleLength()
{
	CTimeZoneInformation tzi;
	DWORD dwLen = 1; // version byte
	dwLen += sizeof(TIME_ZONE_INFORMATION);
	dwLen += sizeof(tzi.DisplayName);
	return dwLen;
}
//////////////////////////////////////////////////////////////////////
CIPCExtraMemory* CTimeZoneInformation::BubbleFrom(CIPC* pCipc) const
{
	CIPCExtraMemory* pExtraMem = new CIPCExtraMemory();
	if (pExtraMem->Create(pCipc,GetBubbleLength()))
	{
		PBYTE pByte = (PBYTE)pExtraMem->GetAddressForWrite();
		WriteIntoBubble(pByte);
	}
	else
	{
		WK_DELETE(pExtraMem);		    
	}
	return pExtraMem;
}
//////////////////////////////////////////////////////////////////////
void CTimeZoneInformation::FromBubble(const CIPCExtraMemory *pExtraMem)
{
	PBYTE pByte = (PBYTE)pExtraMem->GetAddress();
	ReadFromBubble(pByte);
}
//////////////////////////////////////////////////////////////////////
void CTimeZoneInformation::WriteIntoBubble(BYTE*& pByte) const
{
	BYTE v = 0;
	CIPCExtraMemory::BYTE2Memory(pByte,v);
	DWORD dwLen = sizeof(TIME_ZONE_INFORMATION);
	CopyMemory(pByte,this,dwLen);
	pByte += dwLen;
	dwLen = sizeof(DisplayName);
	CopyMemory(pByte,&DisplayName,dwLen);
	pByte += dwLen;
	// add new members here
}
//////////////////////////////////////////////////////////////////////
void CTimeZoneInformation::ReadFromBubble(const BYTE*& pByte)
{
	BYTE v = 0;
	v = CIPCExtraMemory::Memory2BYTE(pByte);

	if (v==0)
	{
		DWORD dwLen = sizeof(TIME_ZONE_INFORMATION);
		CopyMemory(this,pByte,dwLen);
		pByte += dwLen;
		dwLen = sizeof(DisplayName);
		CopyMemory(&DisplayName,pByte,dwLen);
		pByte += dwLen;
	}
	else
	{
		int i = 0;
		WK_ASSERT(i);
	}
}
//////////////////////////////////////////////////////////////////////
CTimeZoneInformations::CTimeZoneInformations()
{
	m_bAutoDelete = TRUE;
}
//////////////////////////////////////////////////////////////////////
CTimeZoneInformations::CTimeZoneInformations(const CTimeZoneInformations& src)
{
	Lock();
	DeleteAll();
	for	(int i=0;i<src.GetSize();i++)
	{
		Add(new	CTimeZoneInformation(*src.GetAtFast(i)));
	}
	Unlock();
}
///////////////////////////////////////////////////////////////////////////////////////
const CTimeZoneInformations& CTimeZoneInformations::operator = (const CTimeZoneInformations& src)
{
	if (this != &src)
	{
		Lock();
		DeleteAll();
		for (int i=0;i<src.GetSize();i++)
		{
			Add(new CTimeZoneInformation(*src.GetAtFast(i)));
		}
		Unlock();
	}
	return *this;
}
//////////////////////////////////////////////////////////////////////
CTimeZoneInformations::~CTimeZoneInformations()
{
}

//////////////////////////////////////////////////////////////////////
BOOL CTimeZoneInformations::Load()
{
	CString sSubKey	= _T("");
	LONG	lRetVal		= 0;
	HKEY	hKey			= NULL;
	REGTIMEZONEINFORMATION regTzi;
	BOOL	bResult		= FALSE;

	OSVERSIONINFO os;
	ZeroMemory(&os, sizeof(os));
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&os);

	if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
		sSubKey = SKEY_NT;
	else
		sSubKey = SKEY_9X;

	
	Lock();
	DeleteAll(); 

	lRetVal = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sSubKey, 0, KEY_ALL_ACCESS, &hKey);
	if (lRetVal == ERROR_SUCCESS)
	{
		DWORD dwCurIdx = 0;
		DWORD dwDataLen = 32;
		DWORD dwValueLen = 32;
		LONG lResult = 0;
		CString	sKey = _T("");
		do
		{
			sKey.Format(_T("%d"), dwValueLen);
		 
			lResult = RegEnumKey(hKey, dwCurIdx, sKey.GetBuffer(255), dwDataLen);
			sKey.ReleaseBuffer();
			if (lResult == ERROR_SUCCESS)
			{
				HKEY hKey	= NULL;
				CString sTemp =  sSubKey + _T("\\") +sKey;
				LONG lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, sTemp, 0, KEY_QUERY_VALUE, &hKey);
				if(lResult == ERROR_SUCCESS)
				{
					DWORD dwType = 0;
					DWORD dwLen  = 0;
					CString sValue = _T("");

					CTimeZoneInformation* pTzi = new CTimeZoneInformation;
			
					// Read Displayname
					LONG lResult = RegQueryValueEx(hKey, _T("Display"), NULL, &dwType, NULL, &dwLen);
					if(lResult == ERROR_SUCCESS)
					{
						LONG lResult = RegQueryValueEx(hKey, _T("Display"), NULL, &dwType, (LPBYTE)sValue.GetBuffer(dwLen),&dwLen);
						sValue.ReleaseBuffer();
						if (lResult == ERROR_SUCCESS)
						{
#ifdef _UNICODE
							dwLen /= sizeof(WCHAR);
#endif
							dwLen = min(dwLen, sizeof(pTzi->DisplayName)/sizeof(WCHAR)); 
							for (int nI = 0; nI < (int)dwLen-1; nI++)
								pTzi->DisplayName[nI] = (WCHAR)sValue[nI]; 

							pTzi->DisplayName[dwLen-1] = '\0';

							// Sommerzeitname einlesen
							LONG lResult = RegQueryValueEx(hKey, _T("Dlt"), NULL, &dwType, NULL, &dwLen);
							if(lResult == ERROR_SUCCESS)
							{
								LONG lResult = RegQueryValueEx(hKey, _T("Dlt"), NULL, &dwType, (LPBYTE)sValue.GetBuffer(dwLen),&dwLen);			
								sValue.ReleaseBuffer();
								if (lResult == ERROR_SUCCESS)
								{
#ifdef _UNICODE
									dwLen /= sizeof(WCHAR);
#endif
									dwLen = min(dwLen, sizeof(pTzi->DaylightName)/sizeof(WCHAR)); 
									for (int nI = 0; nI < (int)dwLen-1; nI++)
										pTzi->DaylightName[nI] = (WCHAR)sValue[nI]; 
									pTzi->DaylightName[dwLen-1] = '\0';

									// Normalzeitname einlesen
									LONG lResult = RegQueryValueEx(hKey, _T("Std"), NULL, &dwType, NULL, &dwLen);
									if(lResult == ERROR_SUCCESS)
									{
										LONG lResult = RegQueryValueEx(hKey, _T("Std"), NULL, &dwType, (LPBYTE)sValue.GetBuffer(dwLen),&dwLen);			
										sValue.ReleaseBuffer();
										if (lResult == ERROR_SUCCESS)
										{
#ifdef _UNICODE
											dwLen /= sizeof(WCHAR);
#endif
											dwLen = min(dwLen, sizeof(pTzi->StandardName)/sizeof(WCHAR)); 
											for (int nI = 0; nI < (int)dwLen-1; nI++)
												pTzi->StandardName[nI] = (WCHAR)sValue[nI]; 

											// TimeZoneInformation einlesen
											dwLen = sizeof(regTzi);
											LONG lResult = RegQueryValueEx(hKey, _T("Tzi"), NULL, &dwType, (LPBYTE)&regTzi, &dwLen);			
											if (lResult == ERROR_SUCCESS)
											{
												pTzi->Bias = regTzi.Bias;
												pTzi->DaylightBias = regTzi.DaylightBias;
												pTzi->DaylightDate = regTzi.DaylightDate;
												pTzi->StandardBias = regTzi.StandardBias;
												pTzi->StandardDate = regTzi.StandardDate;
	
												Add(pTzi);
												// TRACE(_T("adding %s\n"),pTzi->GetDisplayName());
												bResult = TRUE;
											}
											else
											{
												WK_DELETE(pTzi);
											}
										}
										else
										{
											WK_DELETE(pTzi);
										}
									}
									else
									{
										WK_DELETE(pTzi);
									}
								}
								else
								{
									WK_DELETE(pTzi);
								}
							}
							else
							{
								WK_DELETE(pTzi);
							}
						}
						else
						{
							WK_DELETE(pTzi);
						}
					}
					else
					{
						WK_DELETE(pTzi);
					}
					RegCloseKey(hKey);
				}				
			}
			dwCurIdx ++;
		}
		while (lResult == ERROR_SUCCESS);	
		RegCloseKey(hKey);
 	}
   else
	{
      WK_TRACE_ERROR(_T("Could not open registry key\n"));
	}
   
   Sort();

   Unlock();

   return bResult;								   
}

/////////////////////////////////////////////////////////////////////////////
int CTimeZoneInformations::FindByTzi(const TIME_ZONE_INFORMATION &tzi)
{												   
	int	nIndex = -1;

	Lock();	 							   	  
								   
	for (int nI = 0; nI < GetSize() && (nIndex == -1); nI++)
	{
		CTimeZoneInformation* pCtzi = GetAtFast(nI);
		if ((tzi.Bias						== pCtzi->Bias) &&
			(tzi.DaylightBias				== pCtzi->DaylightBias) &&
			(tzi.DaylightDate.wDay			== pCtzi->DaylightDate.wDay) &&
			(tzi.DaylightDate.wDayOfWeek	== pCtzi->DaylightDate.wDayOfWeek) &&
			(tzi.DaylightDate.wHour			== pCtzi->DaylightDate.wHour) &&
			(tzi.DaylightDate.wMilliseconds == pCtzi->DaylightDate.wMilliseconds) &&
			(tzi.DaylightDate.wMinute		== pCtzi->DaylightDate.wMinute) &&
			(tzi.DaylightDate.wMonth		== pCtzi->DaylightDate.wMonth) &&
			(tzi.DaylightDate.wSecond		== pCtzi->DaylightDate.wSecond) &&
			(tzi.DaylightDate.wYear			== pCtzi->DaylightDate.wYear) &&	
			(wcscmp(tzi.DaylightName, pCtzi->DaylightName) == 0) &&
			(tzi.StandardBias				== pCtzi->StandardBias) &&
			(tzi.StandardDate.wDay			== pCtzi->StandardDate.wDay) &&
			(tzi.StandardDate.wDayOfWeek	== pCtzi->StandardDate.wDayOfWeek) &&
			(tzi.StandardDate.wHour			== pCtzi->StandardDate.wHour) &&
			(tzi.StandardDate.wMilliseconds == pCtzi->StandardDate.wMilliseconds) &&
			(tzi.StandardDate.wMinute		== pCtzi->StandardDate.wMinute) &&
			(tzi.StandardDate.wMonth		== pCtzi->StandardDate.wMonth) &&
			(tzi.StandardDate.wSecond		== pCtzi->StandardDate.wSecond) &&
			(tzi.StandardDate.wYear			== pCtzi->StandardDate.wYear) &&
			(wcscmp(tzi.StandardName, pCtzi->StandardName) == 0))
		{
			nIndex = nI;
		}
	}

	Unlock();
									    
	return nIndex;
}							    

/////////////////////////////////////////////////////////////////////////////
int CTimeZoneInformations::FindByDaylightName(const WCHAR* pDaylightName)
{
	int	nIndex = -1;
	Lock();

	for (int nI = 0; nI < GetSize() && (nIndex == -1); nI++)
	{
		CTimeZoneInformation* pCtzi = GetAtFast(nI);
		
		if (wcscmp(pDaylightName, pCtzi->DaylightName) == 0)
			nIndex = nI;
	}

	Unlock();

	return nIndex;
}

/////////////////////////////////////////////////////////////////////////////
int CTimeZoneInformations::FindByStandardName(const WCHAR* pStandardName)
{
	int	nIndex = -1;
	Lock();

//	TRACE(_T("Standardname %s gesucht\n"), pStandardName);
	for (int nI = 0; nI < GetSize() && (nIndex == -1); nI++)
	{
		CTimeZoneInformation* pCtzi = GetAtFast(nI);

//		TRACE(_T("Standardname %s\n"), pCtzi->StandardName);
		if (wcscmp(pStandardName, pCtzi->StandardName) == 0)
			nIndex = nI;
	}

	Unlock();

	return nIndex;
}

/////////////////////////////////////////////////////////////////////////////
int CTimeZoneInformations::FindByDisplayName(const WCHAR* pDisplayName)
{
	int	nIndex = -1;
	Lock();

	for (int nI = 0; nI < GetSize() && (nIndex == -1); nI++)
	{
		CTimeZoneInformation* pCtzi = GetAtFast(nI);
		
		if (wcscmp(pDisplayName, pCtzi->DisplayName) == 0)
			nIndex = nI;
	}

	Unlock();

	return nIndex;
}

/////////////////////////////////////////////////////////////////////////////
void CTimeZoneInformations::Sort()
{
	int iCur, iMin, iNext, nCount;
	CTimeZoneInformation* swap;
	
	Lock();
	
	nCount = GetSize();
	for (iCur=0; iCur<nCount; iCur++)
	{
		iMin = iCur;
		for (iNext=iCur; iNext<nCount; iNext++)
		{
			if (GetAtFast(iNext)->GetDisplayName() < GetAtFast(iMin)->GetDisplayName())
			{
				iMin = iNext;
			}
		}
		// wenn ein element kleiner als das aktuelle ist, tausche die elemente.
		if (iMin>iCur)
		{
			swap = GetAtFast(iCur);
			SetAt(iCur,GetAtFast(iMin));
			SetAt(iMin,swap);
		}
	}

	Unlock();
}
/////////////////////////////////////////////////////////////////////////////
CIPCExtraMemory* CTimeZoneInformations::BubbleFrom(CIPC* pCipc)
{
	Lock();
	DWORD dwBubbleLen = CTimeZoneInformation::GetBubbleLength() * GetSize() + sizeof(DWORD);
	CIPCExtraMemory* pExtraMem = new CIPCExtraMemory();
	if (pExtraMem->Create(pCipc,dwBubbleLen))
	{
		PBYTE pByte = (PBYTE)pExtraMem->GetAddressForWrite();
		DWORD dwSize = GetSize();
		CIPCExtraMemory::DWORD2Memory(pByte,dwSize);
		for (int i=0;i<GetSize();i++)
		{
			GetAtFast(i)->WriteIntoBubble(pByte);
		}
		DWORD dwDelta=pByte-(BYTE*)pExtraMem->GetAddressForWrite();
		WK_ASSERT(dwDelta==dwBubbleLen);
	}
	else
	{
		WK_DELETE(pExtraMem);
	}
	Unlock();
	return pExtraMem;
}
/////////////////////////////////////////////////////////////////////////////
void CTimeZoneInformations::FromBubble(int iNumRecords,const CIPCExtraMemory *pExtraMem)
{
	Lock();
	PBYTE pByte = (PBYTE)pExtraMem->GetAddress();
	int c = CIPCExtraMemory::Memory2DWORD(pByte);
	WK_ASSERT(c == iNumRecords);

	for (int i=0;i<c;i++)
	{
		CTimeZoneInformation* pTZI = new CTimeZoneInformation;
		pTZI->ReadFromBubble(pByte);
		Add(pTZI);
	}

	Unlock();
}
