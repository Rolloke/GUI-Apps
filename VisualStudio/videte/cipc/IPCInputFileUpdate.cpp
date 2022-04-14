// IPCInputFileUpdate.cpp: implementation of the CIPCInputFileUpdate class.
//
//////////////////////////////////////////////////////////////////////

#include "stdcipc.h"
#include "IPCInputFileUpdate.h"

#include "CipcServerControlClientSide.h"
#include "IPCFetch.h"

#include "wk_dongle.h"
#include "cipcstringdefs.h"

#include "NotificationMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CIPCInputFileUpdate::m_bAllowGetRegistry = TRUE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCInputFileUpdate::CIPCInputFileUpdate(const CConnectionRecord& c,
										 LPCTSTR shmName, 
										 BOOL asMaster)
 : CIPCInput(shmName,asMaster), m_ConnectionRecord(c)
{
}

CIPCInputFileUpdate::~CIPCInputFileUpdate()
{

}
/*
	DoIndicateError Sub Codes
	0 .. unbenutzt
	1 .. löschen fehlgeschlagen
	2 .. löschen Verzeichnis fehlgeschlagen
	3 .. ausführen fehlgeschlagen
	4 .. Verzeichnis lesen fg
	5 .. Datei lesen fg
*/
void CIPCInputFileUpdate::OnRequestGetFile(int iDestination,
										   const CString &sFileName)
{
	if (iDestination==RFU_STRING_INTERFACE)
	{
		RequestGetFileStringInterface(sFileName);
		// makes DoConfirmFileUpdate or DoIndicateError
		return;
	}
	CString sName = MakeFullPathName(iDestination,sFileName);

	if (iDestination & RFU_DELETE)
	{
		// delete file

		CFile f;
		DWORD dwAttributes;
		
		dwAttributes = GetFileAttributes(LPCTSTR(sName));
		if (!(dwAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			if (DeleteFile(sName))
			{
				WK_TRACE(_T("Delete File %s SUCCESS\n"),sName);
				DoConfirmGetFile(CFU_DELETE_FILE, sName, NULL, 0);
			}
			else
			{
				WK_TRACE(_T("Delete File %s FAILED\n"),sName);
				DoIndicateError(INP_REQUEST_GET_FILE,SECID_NO_ID,CIPC_ERROR_GET_FILE,
							    1,sName);
			}
		}
		else
		{
			if (RemoveDirectory(sName))
			{
				WK_TRACE(_T("Delete Dir %s SUCCESS\n"),sName);
				DoConfirmGetFile(CFU_DELETE_FILE, sName, NULL, 0);
			}
			else
			{
				WK_TRACE_ERROR(_T("Delete Dir %s FAILED, %s\n"),sName,GetLastErrorString());
				DoIndicateError(INP_REQUEST_GET_FILE,SECID_NO_ID,CIPC_ERROR_GET_FILE,
								2,sName);
			}
		}
	}
	else if (iDestination & RFU_EXECUTE)
	{
		CFile fFile;

#ifdef _UNICODE
		CWK_String str(sFileName);
		if (31<WinExec(str,SW_SHOW))
#else
		if (31<WinExec(sFileName,SW_SHOW))
#endif
		{
			WK_TRACE(_T("start %s SUCCESS\n"), sFileName);
			DoConfirmGetFile(CFU_EXECUTE_FILE,sFileName,NULL,0);
		}
		else
		{
			WK_TRACE(_T("start %s FAILED\n"), sFileName);
			DoIndicateError(INP_REQUEST_GET_FILE,SECID_NO_ID,CIPC_ERROR_GET_FILE,
							3,sFileName);
		}
	}
	else
	{
		CFile f;
		BOOL bOpenOkay;
		BOOL bExists = DoesFileExist(sName);

		if (!bExists)
		{
			DWORD dwFileAttributes = GetFileAttributes(sName);
			if (dwFileAttributes != 0xffffffff)
			{
				bExists = dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? DFE_IS_DIRECTORY : TRUE;
			}
		}
		if (sName.GetAt(sName.GetLength()-1) == _T('\\'))
		{
			sName = sName.Left(sName.GetLength()-1);
		}

		
		if (!bExists)
		{
			sName.MakeLower();
//			int nFind = sName.Find(_T("\\camera.htm"));	// speziell kamera
			int	nFind = sName.Find(_T("\\8000"));		// allgemein wwwroot
			if (nFind != -1)
			{
				CString sWWWroot = CNotificationMessage::GetWWWRoot();
				sWWWroot.MakeLower();
				if (sName.Find(sWWWroot) == -1)
				{
//					nFind -= 9;
					sName = sWWWroot + sName.Mid(nFind);
					bExists = DoesFileExist(sName);
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("File Does not exist %s\n"), sName);
			}
		}

		if (bExists == DFE_IS_DIRECTORY)
		{
			// directory
			CWK_String sDir;
			HANDLE hF;
#ifdef _UNICODE
			BOOL bUnicode = (GetCodePage() == CODEPAGE_UNICODE) ? TRUE : FALSE;
#endif
			void* lpFindFileData; // use variable data format!!
			CPtrArray pA;
			BYTE* pData,*pBuf;
			int i,c;
			DWORD dwLen,dwFD;
			CString sFindFileName;

			sDir = sName;
			while (sDir[sDir.GetLength()-1]=='\\')
			{
				sDir = sDir.Left(sDir.GetLength()-1);
			}
			sDir += _T("\\*.*");

#ifdef _UNICODE
			if (bUnicode)
			{
				dwFD = sizeof(WIN32_FIND_DATAW);
				lpFindFileData = malloc(dwFD);
				hF = FindFirstFileW(sDir, (LPWIN32_FIND_DATAW)lpFindFileData);
				sFindFileName = ((LPWIN32_FIND_DATAW)lpFindFileData)->cFileName;
			}
			else
			{
				dwFD = sizeof(WIN32_FIND_DATAA);
				lpFindFileData = malloc(dwFD);
				hF = FindFirstFileA(sDir, (LPWIN32_FIND_DATAA)lpFindFileData);
				sFindFileName = ((LPWIN32_FIND_DATAA)lpFindFileData)->cFileName;
			}
#else
			dwFD = sizeof(WIN32_FIND_DATAA);
			lpFindFileData = malloc(dwFD);
			hF = FindFirstFileA(sDir, (LPWIN32_FIND_DATAA)lpFindFileData);
			sFindFileName = ((LPWIN32_FIND_DATAA)lpFindFileData)->cFileName;
#endif
			if (hF == INVALID_HANDLE_VALUE)
			{
				free(lpFindFileData);
			}
			else
			{
				if ( (_tcscmp(sFindFileName, _T("."))!=0) &&
					 (_tcscmp(sFindFileName, _T(".."))!=0))
				{
					pA.Add((LPVOID)lpFindFileData);
				}
				else
				{
					free(lpFindFileData);
				}

				BOOL bGo = TRUE;
				BOOL bReturn;
				
				while (bGo)
				{
#ifdef _UNICODE
					if (bUnicode)
					{
						lpFindFileData = malloc(dwFD);
						bReturn = FindNextFileW(hF,(LPWIN32_FIND_DATAW)lpFindFileData);
						sFindFileName = ((LPWIN32_FIND_DATAW)lpFindFileData)->cFileName;
					}
					else
					{
						lpFindFileData = malloc(dwFD);
						bReturn = FindNextFileA(hF,(LPWIN32_FIND_DATAA)lpFindFileData);
						sFindFileName = ((LPWIN32_FIND_DATAA)lpFindFileData)->cFileName;
					}
#else
					lpFindFileData = malloc(dwFD);
					bReturn = FindNextFileA(hF,(LPWIN32_FIND_DATAA)lpFindFileData);
					sFindFileName = ((LPWIN32_FIND_DATAA)lpFindFileData)->cFileName;
#endif
					if (!bReturn)
					{
						free(lpFindFileData);
						break;
					}
					if ( (_tcscmp(sFindFileName, _T("."))!=0) &&
						 (_tcscmp(sFindFileName, _T(".."))!=0))
					{
						pA.Add((LPVOID)lpFindFileData);
					}
					else
					{
						free(lpFindFileData);
					}
				}
				FindClose(hF);
			}
			c = pA.GetSize();
			if (c>0)
			{
				int nTotal = c, nStart = 0;
				int nMax = 50;
				BOOL bNotAllSent = TRUE;
				if (   iDestination & RFU_DIVIDE_IN_BITS // if the calling application can do
					&& c > nMax)				// and the number of file Datas is too high
				{
					c = nMax;					// divide them into smaller bits
				}

				dwLen = c*dwFD;
				pData = new BYTE[dwLen];

				do
				{
					pBuf = pData;
					dwLen = 0;
					if (c >= nTotal)			// at the end
					{
						c = nTotal;				// don´t go throug
						bNotAllSent = FALSE;	// and leave the loop afterwards
					}
					for (i=nStart; i<c; i++)
					{
						CopyMemory(pBuf,pA.GetAt(i),dwFD);
						pBuf += dwFD;
						dwLen += dwFD;
						free (pA.GetAt(i)); 
					}
					if (dwLen)
					{
						if (nStart > 0)			// mark following bits
						{						// so that the calling application inserts correctly
							((LPWIN32_FIND_DATAA)pData)->dwFileAttributes |= FILE_ATTR_CONTINUED_TRANSMISSION;
						}
						DoConfirmGetFile(CFU_DIRECTORY,sFileName,pData,dwLen);
					}
					nStart = c;					// take the next bit
					c += nMax;
				}while (bNotAllSent);

				WK_DELETE_ARRAY(pData);
				pA.RemoveAll();
			}
			else
			{
				WK_TRACE(_T("Failed to read directory %s\n"), sFileName);
				DoIndicateError(INP_REQUEST_GET_FILE,SECID_NO_ID,CIPC_ERROR_GET_FILE,
							    4,sFileName);
			}
		}
		else if (bExists)
		{
			// file open
			bOpenOkay = f.Open(sName,CFile::modeRead|CFile::shareDenyNone);
			if (bOpenOkay) 
			{
				DWORD dwLen = (DWORD)f.GetLength();
				BYTE *pData = new BYTE[dwLen];
				DWORD dwRead = 0;
				TRY
				{
					dwRead = f.Read(pData, dwLen);
				}
				WK_CATCH(_T("reading file for getfile\n"));
				
				if (dwRead!=dwLen) 
				{
					WK_TRACE(_T("File %s, could only read %d from %d\n"), sName, dwRead, dwLen);
				}
				DoConfirmGetFile(CFU_FILE, sFileName, pData, dwRead);
				WK_DELETE_ARRAY(pData);
			} 
			else 
			{
				// NOT YET
				WK_TRACE(_T("Failed to open %s\n"), sFileName);
				DoIndicateError(INP_REQUEST_GET_FILE,SECID_NO_ID,CIPC_ERROR_GET_FILE,
							    5,sFileName);
			}
		}
	}
}
/*
	DoIndicateError Sub Codes
	6 .. unbekannter String Interface Befehl bzw. Befehl in falschem Format
	7 .. reserved
	8 .. GetDiskFreeSpace fg
	9 .. GetOsVersion fg
	10 .. GetSystemInfo fg
	11 .. GetSoftwareVersion fg
	12 .. GetFileVersion no file
	13 .. GetFileVersion no info
	14 .. ExportRegistry fg
	15 .. WWWDir
	16 .. GetTimeZoneInformation
	17 .. Registry Parameter error
	18 .. Registry Execution error
*/
void CIPCInputFileUpdate::RequestGetFileStringInterface(const CString& sCommand)
{
	CString sC,sP;
	int p;

	p = sCommand.Find(' ');
	if (p==-1)
	{
		// invalid command or format
		DoIndicateError(INP_REQUEST_GET_FILE,SECID_NO_ID,CIPC_ERROR_GET_FILE,
						6,sCommand);
	}
	sC = sCommand.Left(p);
	sP = sCommand.Mid(p+1);

	WK_TRACE(_T("%s\n"), sC);
	if (0==sC.CompareNoCase(SI_GETLOGICALDRIVES))
	{
		SIGetLogicalDrives(sC);
	}
	else if (0==sC.CompareNoCase(SI_GETDISKFREESPACE))
	{
		SIGetDiskFreeSpace((char)sP[0],sC);
	}
	else if (0==sC.CompareNoCase(SI_GETOSVERSION))
	{
		SIGetOsVersion(sC);
	}
	else if (0==sC.CompareNoCase(SI_GETSYSTEMINFO))
	{
		SIGetSystemInfo(sC);
	}
	else if (0==sC.CompareNoCase(SI_GLOBALMEMORYSTATUS))
	{
		SIGlobalMemoryStatus(sC);
	}
	else if (0==sC.CompareNoCase(SI_GETCURRENTTIME))
	{
		SIGetCurrentTime(sC);
	}
	else if (0==sC.CompareNoCase(SI_GETSOFTWAREVERSION))
	{
		SIGetSoftwareVersion(sC);
	}
	else if (0==sC.CompareNoCase(SI_GETFILEVERSION))
	{
		SIGetFileVersion(sP,sC);
	}
	else if (0==sC.CompareNoCase(SI_GETWWWDIRECTORY))
	{
		CString sLocalWWW;
		sLocalWWW.Format(_T("%s\\%08lx"),CNotificationMessage::GetWWWRoot(),SECID_LOCAL_HOST);
		SIGetWWWDirectory(sLocalWWW,sP,sC);
	}
	else if (0==sC.CompareNoCase(SI_EXPORTREGISTRY))
	{
		if (m_bAllowGetRegistry)
		{
			SIExportRegistry(sC);
		}
		else
		{
			DoIndicateError(INP_REQUEST_GET_FILE,SECID_NO_ID,CIPC_ERROR_GET_FILE, 6,sCommand);
		}
	}
	else if (0==sC.CompareNoCase(SI_GETTIMEZONEINFORMATION))
	{
		SIGetTimeZoneInformation(sC);
	}
	else if (0==sC.CompareNoCase(SI_GETPRODUCTINFO))
	{
		SIGetproductInfo(sC);
	}
	else if (0==sC.CompareNoCase(SI_ENUMREGKEYS))
	{
		SIEnumRegKeys(sP);
	}
	else if (0==sC.CompareNoCase(SI_ENUMREGVALUES))
	{
		SIEnumRegValues(sP);
	}
	else if (0==sC.CompareNoCase(SI_GETREGKEY))
	{
		SIGetRegKey(sP);
	}
	else if (0==sC.CompareNoCase(SI_SETREGKEY))
	{
		SISetRegKey(sP);
	}
	else if (0==sC.CompareNoCase(SI_DELETEREGKEY))
	{
		SIDeleteRegKey(sP);
	}
	else
	{
		DoIndicateError(INP_REQUEST_GET_FILE,SECID_NO_ID,CIPC_ERROR_GET_FILE, 6,sCommand);
	}
}
/////////////////////////////////////////////////////////////////////////
void CIPCInputFileUpdate::SIGetLogicalDrives(const CString& sCommand)
{
	// no Parameter
	DWORD dwLD = GetLogicalDrives();
	DWORD dwBit = 1;
	CString sDrive,sDrives,sAnswer;
	char c = 'a';
	int i;

	while (dwBit!=0)
	{
		if (dwLD & dwBit)
		{
			sDrives += c;
		}
		dwBit <<= 1;
		c++;
	}
	for (i=0; i<sDrives.GetLength(); i++)
	{
		sDrive.Format(_T("%c:\\"),sDrives[i]);
		switch (GetDriveType(sDrive))
		{
		case DRIVE_REMOVABLE:
			// The drive can be removed from the drive.
			sDrive += _T("F,");
			break;
		case DRIVE_FIXED:
			// The disk cannot be removed from the drive
			sDrive += _T("H,");
			break;
		case DRIVE_REMOTE:
			// The drive is a remote (network) drive.
			sDrive += _T("N,");
			break;
		case DRIVE_CDROM:
			// The drive is a CD-ROM drive.
			sDrive += _T("C,");
			break;
		case DRIVE_RAMDISK:
			// The drive is a RAM disk.
			sDrive += _T("R,");
			break;
		default:
			sDrive += _T("E,");
			break;
		}
		sAnswer += sDrive;
 	}
#ifdef _UNICODE
	DoConfirmGetFile(CFU_GET_LOGICAL_DRIVES,sCommand,
					 LPCTSTR(sAnswer), sAnswer.GetLength() | STRING_DATA_POINTER);
#else
	DoConfirmGetFile(CFU_GET_LOGICAL_DRIVES,sCommand,
					 LPCTSTR(sAnswer), sAnswer.GetLength());
#endif
}
//////////////////////////////////////////////////////////////////////
// Enumerates the Registry subkeys of a given path
// sC: Registry path, example: HKEY_LOCAL_MACHINE\SOFTWARE\DVRT
// returns the keys as comma separated values 
void CIPCInputFileUpdate::SIEnumRegKeys(const CString&sC)
{
	CString sKey;
	int nError = 17;
	int n = sC.Find(_T("\\"));
	if (n != -1)
	{
		sKey = sC.Left(n);
		CString sSection = sC.Mid(n+1);
		HKEY hKey = CWK_Profile::KeyFromName(sKey);
		if (hKey)
		{
			CWK_Profile wkp(CWK_Profile::WKP_ABSOLUT, hKey, _T(""), _T(""));
			CStringArray saKeys;
			if (wkp.EnumRegKeys(sSection, saKeys))
			{
				CString sKeys;
				ConcatenateStrings(saKeys, sKeys, _T(','));
	#ifdef _UNICODE
				DoConfirmGetFile(CFU_ENUMREGKEYS, sC,
								LPCTSTR(sKeys), sKeys.GetLength() | STRING_DATA_POINTER);
	#else
				DoConfirmGetFile(CFU_ENUMREGKEYS, sC,
								LPCTSTR(sKeys), sKeys.GetLength());
	#endif
				return;
			}
			else
			{
				nError = 18;
			}
		}
	}

	DoIndicateError(INP_REQUEST_GET_FILE,SECID_NO_ID,CIPC_ERROR_GET_FILE,
					nError,sC);
}
//////////////////////////////////////////////////////////////////////
// Enumerates the Registry value keys of a given path
// sC: Registry path, example: HKEY_LOCAL_MACHINE\SOFTWARE\DVRT
// returns the value keys as comma separated values 
void CIPCInputFileUpdate::SIEnumRegValues(const CString&sC)
{
	CString sKey;
	int nError = 17;
	int n = sC.Find(_T("\\"));
	if (n != -1)
	{
		sKey = sC.Left(n);
		CString sSection = sC.Mid(n+1);
		HKEY hKey = CWK_Profile::KeyFromName(sKey);
		if (hKey)
		{
			CWK_Profile wkp(CWK_Profile::WKP_ABSOLUT, hKey, _T(""), _T(""));
			CStringArray saKeys;
			if (wkp.EnumRegEntries(sSection, saKeys))
			{
				CString sKeys;
				ConcatenateStrings(saKeys, sKeys, _T(','));
#ifdef _UNICODE
				DoConfirmGetFile(CFU_ENUMREGVALUES, sC,
								LPCTSTR(sKeys), sKeys.GetLength() | STRING_DATA_POINTER);
#else
				DoConfirmGetFile(CFU_ENUMREGVALUES, sC,
								LPCTSTR(sKeys), sKeys.GetLength());
#endif
				return;
			}
			else
			{
				nError = 18;
			}
		}
	}

	DoIndicateError(INP_REQUEST_GET_FILE,SECID_NO_ID,CIPC_ERROR_GET_FILE,
					nError,sC);
}
//////////////////////////////////////////////////////////////////////
// Requests the value of Registry key of a given path
// sC: Registry path and keyname separated by "::"
// example: HKEY_LOCAL_MACHINE\SOFTWARE\DVRT\SecurityLauncher::DisableHDWriteCache
// returns the valuedefinition separated by ':' as follows:
// Type:Size:Value
// Type and size are decimal numbers, Type: (REG_DWORD, REG_SZ, REG_MULTI_SZ, REG_BINARY)
// The Value depends on the type.
// REG_DWORD: decimal number
// REG_SZ: String
// REG_MULTI_SZ: comma separated strings
// REG_BINARY: Each byte as Hex value "%02x"
void CIPCInputFileUpdate::SIGetRegKey(const CString&sC)
{
	CString sKey;
	int nError = 17;
	int n = sC.Find(_T("\\"));
	if (n != -1)
	{
		sKey = sC.Left(n);
		CString sSection = sC.Mid(n+1);
		HKEY hKey = CWK_Profile::KeyFromName(sKey);
		n = sSection.Find(_T("::"));
		if (n != -1 && hKey)
		{
			CString sReturn;
			sKey = sSection.Mid(n+2);
			sSection = sSection.Left(n);
			CWK_Profile wkp(CWK_Profile::WKP_ABSOLUT, hKey, _T(""), _T(""));
			DWORD dwValue, dwSize, dwType = wkp.GetType(sSection, sKey, &dwSize);
			switch (dwType)
			{
				case REG_DWORD:
					dwValue = wkp.GetInt(sSection, sKey, 0);
					sReturn.Format(_T("%d:%d:%d"), dwType, dwSize, dwValue); 
					break;
				case REG_SZ:
					sReturn.Format(_T("%d:%d:"), dwType, dwSize); 
					sReturn += wkp.GetString(sSection, sKey, NULL);
					break;
				case REG_MULTI_SZ:
				{
					CStringArray saValues;
					if (wkp.GetMultiString(sSection, sKey, saValues))
					{
						CString sValues;
						ConcatenateStrings(saValues, sValues, _T(','));
						sReturn.Format(_T("%d:%d:%s"), dwType, dwSize, sValues);
					}
				}break;
//				case REG_BINARY:
				default:
				if (dwType == 0)
				{
					nError = 18;
					break;
				}
				else
				{
					BYTE*pValue = NULL;
					if (wkp.GetBinary(sSection, sKey, &pValue, (UINT*)&dwSize))
					{
						CString sValue(_T(' '), dwSize*2);
						CString sTemp;
						DWORD i, j;
						for (i=0, j=0; i<dwSize; i++)
						{
							sTemp.Format(_T("%02x"), pValue[i]);
							sValue.SetAt(j++, sTemp.GetAt(0));
							sValue.SetAt(j++, sTemp.GetAt(1));
						}
						sReturn.Format(_T("%d:%d:%s"), dwType, dwSize, sValue);
					}
					WK_DELETE(pValue);
				}break;
			}
			if (!sReturn.IsEmpty())
			{

#ifdef _UNICODE
				DoConfirmGetFile(CFU_GETREGKEY, sC,
							LPCTSTR(sReturn), sReturn.GetLength() | STRING_DATA_POINTER);
#else
				DoConfirmGetFile(CFU_GETREGKEY, sC,
							LPCTSTR(sReturn), sReturn.GetLength());
#endif
				return;
			}
		}
		else
		{
			nError = 18;
		}
	}

	DoIndicateError(INP_REQUEST_GET_FILE,SECID_NO_ID,CIPC_ERROR_GET_FILE,
					nError,sC);
}
//////////////////////////////////////////////////////////////////////
// Sets the value of Registry key of a given path
// sC: Registry path, Keyname and value separated by "::":
// example: HKEY_LOCAL_MACHINE\SOFTWARE\DVRT\SecurityLauncher::DisableHDWriteCache::Valuedefinition
// Valuedefinition separated by ':' -> Type:Value[:Binary]
// the Type is a decimal number: (REG_DWORD, REG_SZ, REG_MULTI_SZ, REG_BINARY)
// The Value depends on the type.
// REG_DWORD: decimal number
// REG_SZ: String
// REG_MULTI_SZ: comma separated strings
// REG_BINARY: Each byte as Hex value "%02x", Value contains the size
// returns Confirm or InicateError
void CIPCInputFileUpdate::SISetRegKey(const CString&sC)
{
	CString sKey;
	int nError = 17;
	int n = sC.Find(_T("\\"));
	if (n != -1)
	{
		sKey = sC.Left(n);
		CString sSection = sC.Mid(n+1);
		HKEY hKey = CWK_Profile::KeyFromName(sKey);
		n = sSection.Find(_T("::"));
		if (n != -1 && hKey)
		{
			sKey = sSection.Mid(n+2);
			sSection = sSection.Left(n);
			n = sKey.Find(_T("::"));
			if (n != -1)
			{
				CString sValueDefinition;
				BOOL bSuccess = FALSE;
				sValueDefinition = sKey.Mid(n+2);
				sKey = sKey.Left(n);
				DWORD dwValue=0, dwType=0;
				n = _stscanf(sValueDefinition, _T("%d:%d"), &dwType, &dwValue);
				CWK_Profile wkp(CWK_Profile::WKP_ABSOLUT, hKey, _T(""), _T(""));
				switch (dwType)
				{
					case REG_DWORD:
						bSuccess = wkp.WriteInt(sSection, sKey, dwValue);
						break;
					case REG_SZ:
						n = sValueDefinition.Find(_T(":"));
						if (n != -1)
						{
							bSuccess = wkp.WriteString(sSection, sKey, sValueDefinition.Mid(n+1));
						}
						break;
					case REG_MULTI_SZ:
						n = sValueDefinition.Find(_T(":"));
						if (n != -1)
						{
							CStringArray saValues;
							SplitString(sValueDefinition.Mid(n+1), saValues, _T(','));
							bSuccess = wkp.WriteMultiString(sSection, sKey, saValues);
						}
						break;
					case REG_BINARY:
						n = sValueDefinition.Find(_T(":"));
						if (n != -1) n = sValueDefinition.Find(_T(":"), n+1);
						if (n != -1)
						{
							// the value contains the size
                            BYTE*pValue = new BYTE[dwValue];
							if (pValue)
							{
								LPCTSTR szValue = sValueDefinition;
								DWORD i, j, dw;
								for (i=0, j=n+1; i<dwValue; i++, j+=2)
								{
									n = _stscanf(&szValue[j], _T("%02x"), &dw);
									pValue[i] = (BYTE)dw;
								}
								bSuccess = wkp.WriteBinary(sSection, sKey, pValue, (UINT)dwValue);
							}
						}
						break;
					default:
//						if (dwType == 0)
						{
							nError = 18;
						}
						break;
				}
				if (bSuccess)
				{
					DoConfirmGetFile(CFU_SETREGKEY, sC, NULL, 0);
					return;
				}
			}
		}
		else
		{
			nError = 18;
		}
	}

	DoIndicateError(INP_REQUEST_GET_FILE,SECID_NO_ID,CIPC_ERROR_GET_FILE,
					nError,sC);
}
//////////////////////////////////////////////////////////////////////
// Deletes a Registry value or a key from a given path
// sC: Registry path and optional value
// example: HKEY_LOCAL_MACHINE\SOFTWARE\DVRT\AudioUnit[::ExtendedSettings]
// returns Confirm or InicateError
void CIPCInputFileUpdate::SIDeleteRegKey(const CString&sC)
{
	CString sKey;
	int nError = 17;
	int n = sC.Find(_T("\\"));
	if (n != -1)
	{
		sKey = sC.Left(n);
		CString sSection = sC.Mid(n+1);
		HKEY hKey = CWK_Profile::KeyFromName(sKey);
		if (hKey)
		{
			CWK_Profile wkp(CWK_Profile::WKP_ABSOLUT, hKey, _T(""), _T(""));
			BOOL bSuccess = FALSE;
			n = sSection.Find(_T("::"));
			if (n != -1)
			{
				CString sReturn;
				sKey = sSection.Mid(n+2);
				sSection = sSection.Left(n);
				bSuccess = wkp.DeleteEntry(sSection, sKey);
			}
			else
			{
				bSuccess = wkp.DeleteSection(sSection);
			}

			if (bSuccess)
			{
				DoConfirmGetFile(CFU_DELETEREGKEY, sC, NULL, 0);
				return;
			}
			else
			{
				nError = 18;
			}
		}
	}

	DoIndicateError(INP_REQUEST_GET_FILE,SECID_NO_ID,CIPC_ERROR_GET_FILE,
					nError,sC);
}
//////////////////////////////////////////////////////////////////////
void CIPCInputFileUpdate::SIGetDiskFreeSpace(char cDrive, const CString& sCommand)
{
	// parameter is a drive letter
	CString sRoot;
	DWORD dwA[4];
	sRoot.Format(_T("%c:\\"),cDrive);
	if (GetDiskFreeSpace(LPCTSTR(sRoot), &dwA[0], &dwA[1], &dwA[2], &dwA[3]))
	{
		DoConfirmGetFile(CFU_GET_DISK_FREE_SPACE,sCommand,
						 &dwA, sizeof(dwA));
	}
	else
	{
		DoIndicateError(INP_REQUEST_GET_FILE,SECID_NO_ID,CIPC_ERROR_GET_FILE,
						8,sCommand);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCInputFileUpdate::SIGetOsVersion(const CString& sCommand)
{
	// no parameter
	OSVERSIONINFOA osvi;	// explicit define ansi version !!
	CString sRet;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
	if (GetVersionExA(&osvi))
	{
		DoConfirmGetFile(CFU_GET_OS_VERSION,sCommand,
						 &osvi, sizeof(OSVERSIONINFOA));
	}
	else
	{
		sRet = GetLastErrorString();
		DoIndicateError(INP_REQUEST_GET_FILE,SECID_NO_ID,CIPC_ERROR_GET_FILE,
						9,sRet);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCInputFileUpdate::SIGetSystemInfo(const CString& sCommand)
{
	// no parameter
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	DoConfirmGetFile(CFU_GET_SYSTEM_INFO,sCommand,
					 &si, sizeof(SYSTEM_INFO));
}
//////////////////////////////////////////////////////////////////////
void CIPCInputFileUpdate::SIGlobalMemoryStatus(const CString& sCommand)
{
	// no parameter
	MEMORYSTATUS ms;
	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);
	DoConfirmGetFile(CFU_GLOBAL_MEMORY_STATUS,sCommand,
					 &ms, sizeof(MEMORYSTATUS));
}
//////////////////////////////////////////////////////////////////////
void CIPCInputFileUpdate::SIGetCurrentTime(const CString& sCommand)
{
	// no parameter
	CTime t = CTime::GetCurrentTime();
	CString s = t.Format(_T("%d.%m.%y %H:%M:%S"));
#ifdef _UNICODE
	DoConfirmGetFile(CFU_GET_CURRENT_TIME,sCommand,
					 LPCTSTR(s), s.GetLength() | STRING_DATA_POINTER);
#else
	DoConfirmGetFile(CFU_GET_CURRENT_TIME,sCommand,
					 LPCTSTR(s), s.GetLength());
#endif
}
//////////////////////////////////////////////////////////////////////
void CIPCInputFileUpdate::SIGetSoftwareVersion(const CString& sCommand)
{
	CString sVersion;
	CWK_Profile wkp;
	sVersion = wkp.GetString(_T("SecurityLauncher"), _T("Version"), _T(""));
	if (sVersion.GetLength() < 2)
	{
		sVersion = wkp.GetString(_T("Version"), _T("Number"), _T(""));
	}
#ifdef _UNICODE
	DoConfirmGetFile(CFU_GET_SOFT_VERSION,sCommand,
					 LPCTSTR(sVersion), sVersion.GetLength() | STRING_DATA_POINTER);
#else
	DoConfirmGetFile(CFU_GET_SOFT_VERSION,sCommand,
					 LPCTSTR(sVersion), sVersion.GetLength());
#endif
}
//////////////////////////////////////////////////////////////////////
void CIPCInputFileUpdate::SIGetFileVersion(const CString& sFile,const CString& sCommand)
{
	// file name as parameter
	CString sP = sFile;
	CString sRet;
	DWORD dwSize;
	DWORD dwHandle=0;
	PBYTE pData;


	dwSize = GetFileVersionInfoSize(sP.GetBuffer(0),&dwHandle);
	sP.ReleaseBuffer();

	if (dwSize>0)
	{
		pData = new BYTE[dwSize];
		if (GetFileVersionInfo(sP.GetBuffer(0),0,dwSize,pData))
		{
			sP.ReleaseBuffer();
			DoConfirmGetFile(CFU_GET_FILE_VERSION,sP,
							 pData, dwSize);
		}
		else
		{
			sP.ReleaseBuffer();
			sRet = GetLastErrorString();
			DoIndicateError(INP_REQUEST_GET_FILE,SECID_NO_ID,CIPC_ERROR_GET_FILE,
						12,sRet);
		}
		WK_DELETE_ARRAY(pData);
	}
	else
	{
		sRet = GetLastErrorString();
		DoIndicateError(INP_REQUEST_GET_FILE,SECID_NO_ID,CIPC_ERROR_GET_FILE,
					13,sRet);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCInputFileUpdate::SIGetWWWDirectory(const CString& sDirectory,
											  const CString& sParameter,
											  const CString& sCommand)
{
	WIN32_FIND_DATA	found;
	HANDLE	hFound = NULL;
	int p;

	CString sSearch;
	CString sFile;

	sSearch  = sDirectory + _T("\\*.*");


	hFound = FindFirstFile(LPCTSTR(sSearch), &found);
	if (hFound != INVALID_HANDLE_VALUE)
	{
		for (BOOL bFound = TRUE; bFound; bFound=FindNextFile (hFound, &found)) 
		{
			sFile = found.cFileName;
			if (found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				// no recursion over dirs
			}
			else
			{
				BOOL bSend = TRUE;
				CString s = sParameter;
				p = s.Find(found.cFileName);
				if (p!=-1)
				{
					s = s.Mid(p);
					p = s.Find('(');
					if (p!=-1)
					{
						s = s.Mid(p);
						p = s.Find(')');
						if (p!=-1)
						{
							DWORD dwT=0;
							s = s.Left(p+1);
							if (1==_stscanf(s,_T("(%08lx)"),&dwT))
							{
								CTime t1((time_t)dwT);
								CTime t2(found.ftLastWriteTime);
								if (t1>=t2)
								{
									bSend = FALSE;
								}
							}
						}
					}
				}
				
				if (bSend)
				{
					sFile = sDirectory + '\\' + found.cFileName;
					SIDoConfirmWWWFile(sFile);
				}
			}
		}
	}
	FindClose(hFound);
}
//////////////////////////////////////////////////////////////////////
void CIPCInputFileUpdate::SIDoConfirmWWWFile(const CString& sPathname)
{
	CFile file;

	if (file.Open(sPathname,CFile::modeRead|CFile::shareDenyNone))
	{
		ULONGLONG ulSize = file.GetLength();
		WK_ASSERT(ULONG_MAX >= ulSize);
		DWORD dwLen = (DWORD)ulSize;
		BYTE *pData = new BYTE[dwLen];
		DWORD dwRead = 0;

		CFileStatus cfs;
		file.GetStatus(cfs);
		TRY
		{
			dwRead = file.Read(pData,dwLen);
			file.Close();
		}
		WK_CATCH(_T("reading file for getwwwfile"));
		
		if (dwRead!=dwLen) 
		{
			WK_TRACE(_T("File %s, could only read %d from %d\n"),sPathname,dwRead,dwLen);
		}
		else
		{
			CString sValue;
			CString sConfirm;
			ULONGLONG ulTime = cfs.m_mtime.GetTime();
			WK_ASSERT(ULONG_MAX >= ulTime);
			sValue.Format(_T("(%08lx)"),(DWORD)ulTime);
			sConfirm = sPathname + sValue;
			WK_TRACE(_T("confirming %s\n"),sConfirm);
			DoConfirmGetFile(CFU_WWW_FILE, sConfirm, pData, dwRead);
		}
		WK_DELETE_ARRAY(pData);
	} 
	else 
	{
		// NOT YET
		WK_TRACE(_T("Failed to open %s\n"),sPathname);
		DoIndicateError(INP_REQUEST_GET_FILE,SECID_NO_ID,CIPC_ERROR_GET_FILE,
						5,sPathname);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCInputFileUpdate::SIGetTimeZoneInformation(const CString& sCommand)
{
	// no parameter
	TIME_ZONE_INFORMATION ti;
	CString sRet;
	if (TIME_ZONE_ID_UNKNOWN!=GetTimeZoneInformation(&ti))
	{
		DoConfirmGetFile(CFU_TIME_ZONE_INFO,sCommand,
						 &ti, sizeof(TIME_ZONE_INFORMATION));
	}
	else
	{
		sRet = GetLastErrorString();
		DoIndicateError(INP_REQUEST_GET_FILE,SECID_NO_ID,CIPC_ERROR_GET_FILE,
						16,sRet);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCInputFileUpdate::SIGetproductInfo(const CString&sCommand)
{
	void *pi = NULL;
	int nSize = 0;
//	CWK_Dongle dongle;

	DoConfirmGetFile(CFU_PODUCT_INFO, sCommand, pi, nSize);
}
//////////////////////////////////////////////////////////////////////
void CIPCInputFileUpdate::SIExportRegistry(const CString& sCommand)
{
	CString sFile = WK_GetTempFile("sec");

	CWK_Profile wkp;
	CWK_Dongle dongle;
	CFile file;
#ifdef _UNICODE
	DWORD dwCodePage = 0;
	if (m_bAllowGetRegistry == 2)	// do not use Unicode
	{	// save the codepage
		dwCodePage = MAKELONG(CWK_String::GetCodePage(), 0);
	}
	else							// use Unicode
	{	// save the codepage
		dwCodePage = MAKELONG(CWK_String::GetCodePage(), 0x0001);
		wkp.SetCodePage(CODEPAGE_UNICODE);
	}
#endif
	dongle.Save(wkp);

	if (wkp.SaveAs(sFile,NULL))
	{
		// bubble it into the result
		BOOL bOpenOkay = file.Open(sFile,CFile::modeRead|CFile::shareDenyNone);
		if (bOpenOkay) 
		{
			ULONGLONG ulSize = file.GetLength();
			WK_ASSERT(ULONG_MAX >= ulSize);
			DWORD dwLen = (DWORD)ulSize;
			BYTE *pData = new BYTE[dwLen];
			DWORD dwRead = file.Read(pData,dwLen);
			if (dwRead==dwLen) 
			{
#ifdef _UNICODE
				// deliver the codepage params
				WK_TRACE(_T("confirming exported registry %s %d Bytes, CodePage:%d, Unicode:%d\n"),sFile, dwRead, LOWORD(dwCodePage), HIWORD(dwCodePage));
				DoConfirmGetFile(CFU_EXPORT_REGISTRY,sCommand, pData, dwRead, dwCodePage);
#else
				WK_TRACE(_T("confirming exported registry %s %d Bytes\n"),sFile,dwRead);
				DoConfirmGetFile(CFU_EXPORT_REGISTRY,sCommand, pData, dwRead);
#endif
			}
			else
			{
				DoIndicateError(INP_REQUEST_GET_FILE,SECID_NO_ID,CIPC_ERROR_GET_FILE,
							14,sCommand);
			}
			WK_DELETE_ARRAY(pData);
			file.Close();
		} 
		else 
		{
			DoIndicateError(INP_REQUEST_GET_FILE,SECID_NO_ID,CIPC_ERROR_GET_FILE,
							14,sCommand);
		}

		if (!DeleteFile(sFile))
		{
			WK_TRACE_ERROR(_T("cannot delete temp file %s,%s\n"),sFile,GetLastErrorString());
		}
	}

#ifdef _UNICODE
	// restore the old codepage
	wkp.SetCodePage(LOWORD(dwCodePage));
#endif
}
/////////////////////////////////////
BOOL CIPCInputFileUpdate::SIImportRegistry(const CString& sCommand, 
											 const void *pData,
											 DWORD dwDataLen)
{
	CString sTempFileName = WK_GetTempFile("sec");
	BOOL bRet = TRUE;

	CFile tempFile;

	if (tempFile.Open(sTempFileName,
		CFile::modeCreate|CFile::modeWrite))
	{
		WK_TRACE(_T("import registry temporary is %s\n"),sTempFileName);
		TRY
		{
			tempFile.Write(pData,dwDataLen);
			tempFile.Flush();
			tempFile.Close();
		}
		CATCH(CFileException, e)
		{
			CString sTrace;
			WK_TRACE_ERROR(_T("import registry temporary not saved %s %s\n"),sTempFileName,
				GetLastErrorString(e->m_lOsError));
			bRet = FALSE;
		}
		END_CATCH
	}
	else
	{
		bRet = FALSE;
		WK_TRACE_ERROR(_T("import registry temporary not saved\n"));
	}

	if (!bRet)
	{
		return FALSE;
	}

	CWK_Profile wkp;
	if (!wkp.Import(sTempFileName))
	{
		WK_TRACE_ERROR(_T("ImportRegistry failed <%s>\n"),sTempFileName);

		if (!DeleteFile(sTempFileName))
		{
			WK_TRACE_ERROR(_T("cannot delete temp file %s,%s\n"),
				sTempFileName,GetLastErrorString());
		}

		return FALSE;
	}

	DeleteFile(sTempFileName);

	// first confirm the import
	// then do all resets

	DoConfirmFileUpdate(_T("ImportRegistry ")+sCommand);

	return bRet;
}
/////////////////////////////////////////////////////////////
BOOL CIPCInputFileUpdate::SIReset(const CString& sCommand)
{
	BOOL bRet = TRUE;
	CStringArray sa;

	SplitString(sCommand,sa,',');

	CString sApp;

	for (int i=0;i<sa.GetSize();i++)
	{
		sApp = sa.GetAt(i);
		if (!sApp.IsEmpty())
		{
			if (WK_IS_RUNNING(sApp))
			{
				CConnectionRecord c(m_ConnectionRecord);
				c.SetTimeout(5*1000);
				c.SetDestinationHost(sApp);
				if (CIPCServerControlClientSide::FetchServerReset(c)) 
				{
					WK_TRACE(_T("<%s> resetted\n"),sApp);
				}
				else
				{
					bRet = FALSE;
				}
			}
		}
	}

	OnRequestResetApplication();

	return bRet;
}
/*
	DoIndicateError Sub Codes
	0 .. unbenutzt
	1 .. ausführen fehlgeschlagen
*/
void CIPCInputFileUpdate::OnRequestFileUpdate(int iDestination,
											  const CString &sFileName,
											  const void *pData,
											  DWORD dwDataLen,
											  BOOL bNeedsReboot)
{

	if ( (iDestination == RFU_STRING_INTERFACE) &&
		 (0==sFileName.Find(_T("ImportRegistry"))))
	{
		// for remote systemverwaltung

		// first save current settings
		CString sSave;
		CString sLine;
		int p;
		BOOL bOK = TRUE;

		GetModuleFileName(NULL,sSave.GetBuffer(_MAX_PATH),_MAX_PATH);
		sSave.ReleaseBuffer();

		p = sSave.ReverseFind('\\');
		sSave = sSave.Left(p+1) + REMOTE_REG_IMPORT_BACKUP;

		WK_TRACE(_T("save current settings under %s\n"),sSave);
		CWK_Profile wkp;
		bOK = wkp.SaveAs(sSave,NULL);

		if (bOK)
		{
			WK_TRACE(_T("current settings saved\n"));

			WK_TRACE(_T("delete current settings\n"));
			if (wkp.DeleteSection(NULL))
			{
				WK_TRACE(_T("current settings deleted\n"));
			}

			p = sFileName.Find(' ');
			if (p!=-1)
			{
				sLine = sFileName.Mid(p+1);
			}

			WK_TRACE(_T("importing new settings\n"));
			if (SIImportRegistry(sLine,pData,dwDataLen))
			{
				WK_TRACE(_T("new settings imported successfull\n"));
				
				// wait for sending the confirm
				// before resetting
				Sleep(100);

				SIReset(sLine);
			}
			else
			{
				WK_TRACE_ERROR(_T("importing new settings FAILED\n"));
				DoIndicateError(INP_REQUEST_FILE_UPDATE,
								SECID_NO_ID,
								CIPC_ERROR_FILE_UPDATE,
								1,sFileName);
				WK_TRACE(_T("importing old settings\n"));
				wkp.Import(sSave);
			}
		}
		else
		{
			DoIndicateError(INP_REQUEST_FILE_UPDATE,
							SECID_NO_ID,
							CIPC_ERROR_FILE_UPDATE,
							1,sFileName);
		}
		if (DeleteFile(sSave))
		{
			WK_TRACE(_T("deleted %s\n"),sSave);
		}
	}
	else
	{
		WK_TRACE(_T("Update %s REQUEST\n"), sFileName);
		CIPCFetch fetch;
		BOOL bDone = fetch.FetchUpdate(iDestination, 
									   sFileName, 
									   pData, 
									   dwDataLen, 
									   bNeedsReboot);
		if (bDone) 
		{
			WK_TRACE(_T("Update %s SUCCESS\n"), sFileName);
			DoConfirmFileUpdate(sFileName);
		}
		else 
		{
			WK_TRACE(_T("Update %s FAILED\n"), sFileName);
			DoIndicateError(INP_REQUEST_FILE_UPDATE,SECID_NO_ID,CIPC_ERROR_FILE_UPDATE,
						1,sFileName);
		}
	}
}
////////////////////////////////////////////////////////////////////////////
void CIPCInputFileUpdate::OnRequestResetApplication()
{
}
////////////////////////////////////////////////////////////////////////////
