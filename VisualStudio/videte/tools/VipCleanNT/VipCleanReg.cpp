// VipCleanReg.cpp: implementation of the CVipCleanReg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VipCleanNT.h"
#include "VipCleanReg.h"
#include "shlwapi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVipCleanReg::CVipCleanReg()
{

}

CVipCleanReg::~CVipCleanReg()
{
	
}





BOOL CVipCleanReg::SetKey(CString KeyAdress, CString KeyName, BYTE *pValue, DWORD dwlen, DWORD Regtype)
{
	HKEY phkResult = NULL;
	LONG rvalue = 0;
	//Schlüssel öffnen

	rvalue = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,         // handle to open key
		KeyAdress,  // address of name of subkey to open
		0,							// reserved
		KEY_WRITE, // security access mask
		&phkResult    // address of handle to open key
		);
	
	if (rvalue == ERROR_SUCCESS)
	{
		if(Regtype==REG_SZ)
		{
			rvalue= RegSetValueEx(
				phkResult,           // handle to key to set value for
				KeyName, // name of the value to set
				NULL,      // reserved
				Regtype,      //Type CString
				pValue,  // address of value data
				dwlen+1         // size of value data+ \0
				);
		}
		else if(Regtype==REG_DWORD)
		{
			DWORD a = _ttoi((LPCTSTR)pValue);
			
			//a=a + ttoi(t);
			
			rvalue= RegSetValueEx(
				phkResult,           // handle to key to set value for
				KeyName, // name of the value to set
				NULL,      // reserved
				REG_DWORD,      //Type DWORD
				(BYTE*)&a,  // address of value data
				sizeof(DWORD)         // size of value data+ \0
				);
		}
		if (rvalue==ERROR_SUCCESS)
		{
			RegCloseKey(phkResult);
			return TRUE;
		}
	}
	else
	{
		//Schlüssel existiert nicht, also anlegen
		DWORD dw;
		rvalue = RegCreateKeyEx(HKEY_LOCAL_MACHINE, KeyAdress, 0, REG_NONE,
								0, KEY_ALL_ACCESS, NULL, &phkResult, &dw);
		if (rvalue == ERROR_SUCCESS)
		{
			if(Regtype==REG_SZ)
			{
				rvalue= RegSetValueEx(
					phkResult,           // handle to key to set value for
					KeyName, // name of the value to set
					NULL,      // reserved
					Regtype,      //Type CString
					pValue,  // address of value data
					dwlen+1         // size of value data+ \0
					);
			}
			
			else if(Regtype==REG_DWORD)
			{
				DWORD a = _ttoi((LPCTSTR)pValue);
				
				//a=a + ttoi(t);
				
				rvalue= RegSetValueEx(
									phkResult,           // handle to key to set value for
									KeyName, // name of the value to set
									NULL,      // reserved
									REG_DWORD,      //Type DWORD
									(BYTE*)&a,  // address of value data
									sizeof(DWORD)         // size of value data+ \0
									);
			}
			if (rvalue==ERROR_SUCCESS)
			{
				RegCloseKey(phkResult);
				return TRUE;
			}
		}
	}
	RegCloseKey(phkResult);
	return false;
}

CString CVipCleanReg::GetKey(CString KeyAdress, CString KeyName, DWORD Regtype, HKEY MainKey /* = HKEY_LOCAL_MACHINE */)
{
	HKEY phkResult;
	DWORD dwType,dwLen;
	LONG rvalue;
	CString strValue;
//	LPBYTE test=NULL;
	BYTE value;
	
	//Schlüssel öffnen
	rvalue = RegOpenKeyEx(
		MainKey,         // handle to open key
		KeyAdress,  // address of name of subkey to open
		0,							// reserved
		KEY_QUERY_VALUE, // security access mask
		&phkResult    // address of handle to open key
		);
	//Länge des Inhaltes holen
	if(rvalue == ERROR_SUCCESS)
	{
		rvalue = RegQueryValueEx(phkResult, 
			KeyName, 
			NULL, 
			&dwType, 
			NULL,
			&dwLen);
		//Inhalt (OEM) holen;
		if(rvalue == ERROR_SUCCESS)
		{
			if (Regtype==REG_SZ)
			{
				rvalue = RegQueryValueEx(phkResult, 
					KeyName, 
					NULL, 
					&dwType,
					(LPBYTE)strValue.GetBuffer(dwLen/sizeof(TCHAR)),&dwLen);
				strValue.ReleaseBuffer();
				RegCloseKey(phkResult);
				return strValue;
			}
			
			else if(Regtype == REG_DWORD)
			{
				rvalue = RegQueryValueEx(phkResult, 
					KeyName, 
					NULL, 
					&dwType,
					&value, 
					&dwLen);
				
				RegCloseKey(phkResult);
				strValue.Format(_T("%i"),value);
				return strValue;
			}
		}
	}
	
	RegCloseKey(phkResult);
	return _T("0");
}

BOOL CVipCleanReg::DelFiles(CString sdir)
{
	CFileFind test;
	CString name;
	BOOL bIsDot, bIsDir;
	
	BOOL ok=test.FindFile(sdir+_T("\\*.*"));
	
	while(ok)
	{
		ok=test.FindNextFile();
		name=test.GetFileName();
		TRACE(_T("%s\n"),name);

		bIsDot=test.IsDots();
		bIsDir=test.IsDirectory();

		if(!bIsDot  && !bIsDir)
		{
			SetFileAttributes(sdir+_T("\\")+name,FILE_ATTRIBUTE_NORMAL);
			BOOL bDeleted = FALSE;
			bDeleted = DeleteFile(sdir+_T("\\")+name);
		}
		if((!test.IsDots())&&(test.IsDirectory()))
		{
			DelFiles(sdir+_T("\\")+name);
			_tchdir(sdir);
			_trmdir(name);
		}
		
	}
	
	
	return TRUE;
}

BOOL CVipCleanReg::DelDir()
{
	
	DelFiles(_T("c:\\dv\\mdmask1"));
	DelFiles(_T("c:\\dv\\mdmask2"));

	DelFiles(_T("c:\\Log"));

	RemoveDirectory(_T("c:\\dv\\mdmask1"));
	RemoveDirectory(_T("c:\\dv\\mdmask2"));
	DeleteFile(_T("c:\\dv\\rterrors.dat"));
	
	char drive;
	CString path;
	
	/* If we can switch to the drive, if exists. */
	for( drive = _T('c'); drive <= _T('z'); drive++ )
	{
		path.Format(_T("%c:\\dvs"),drive);
		DelFiles(path);
		RemoveDirectory(path);
	}
	
	
	
	
	return TRUE;
	
}

BOOL CVipCleanReg::CheckTemp()
{
	if (_tchdir(_T("c:\\temp"))==-1)
		return FALSE;
	
	return TRUE;
}

BOOL CVipCleanReg::CreateTempDir()
{
	if(_tmkdir(_T("c:\\temp"))==-1)
		return FALSE;
	return TRUE;
}

BOOL CVipCleanReg::RemoveTempDir()
{
	DelFiles(_T("c:\\temp"));
	if(_trmdir(_T("c:\\temp"))==-1)
		return FALSE;
	return TRUE;
}

int CVipCleanReg::GetNTLoaderTimeout()
{
	CFileStatus status;					//Schreibschutz entfernen;
	int itimeout = 0;
	//TODO TKR checken, warum boot.ini neuen Status setzen nicht mehr klappt
/*	CFile::GetStatus( _T("c:\\boot.ini"), status );

	status.m_attribute = status.m_attribute | 0x01;
	status.m_attribute = status.m_attribute ^ 0x01;
	
	WK_TRACE(_T(" vor SetStatus()\n"));
	CFile::SetStatus( _T("c:\\boot.ini"), status );
	WK_TRACE(_T(" nach SetStatus()\n"));
	
	char pbuf[1024];
	int iPos,jPos,itimeout;
	
	CString sBuffer;
	CFile *pFile= new CFile( _T("c:\\boot.ini"),CFile::modeReadWrite);
	pFile->Read(pbuf,1024);	
	
	WK_TRACE(_T(" nach Read()\n"));
	sBuffer=(LPCTSTR)pbuf;
	iPos= sBuffer.Find(_T("[boot loader]"));
	iPos= sBuffer.Find(_T("timeout"),iPos);
	iPos= sBuffer.Find(_T("="),iPos);
	
	jPos= sBuffer.Find(_T("\r\n"),iPos);
	
	itimeout = ttoi((LPCTSTR)sBuffer.Mid(iPos+1,jPos-iPos));
	status.m_attribute = status.m_attribute ^ 0x01;
	pFile->Close();
	WK_TRACE(_T(" nach Close()\n"));
	delete pFile;
*/
	return itimeout;
	
}

void CVipCleanReg::SetNTLoaderTimeout(int /*iTimeout*/)
{

//TODO TKR checken, warum boot.ini neuen Status setzen nicht mehr klappt
/*
	CFileStatus status;					//Schreibschutz entfernen;
	CFile::GetStatus( _T("c:\\boot.ini"), status );
	
	WK_TRACE(_T("vor SetStatus()\n"));
//	status.m_attribute = status.m_attribute | 0x01;
//	status.m_attribute = status.m_attribute ^ 0x01;
	status.m_attribute = 0x0;
	

	CFile::SetStatus( _T("c:\\boot.ini"), status );
	WK_TRACE(_T("nach SetStatus()\n"));
	
	char pbuf[1024];
	int iPos,jPos;
	
	CString sBuffer,sleft,sright,sNewBuffer;
	CFile *pFile= new CFile( _T("c:\\boot.ini"),CFile::modeReadWrite);
	pbuf[pFile->Read(pbuf,1024)]=_T('\0');	
	
	sBuffer=(LPCTSTR)pbuf;
	iPos= sBuffer.Find(_T("[boot loader]"));
	iPos= sBuffer.Find(_T("timeout"),iPos);
	iPos= sBuffer.Find(_T("="),iPos);
	
	jPos= sBuffer.Find(_T("\r\n"),iPos);
	sleft= sBuffer.Left(iPos+1);
	sright= sBuffer.Mid(jPos);
	sNewBuffer.Format(_T("%s%i%s"),sleft,iTimeout,sright);
	pFile->Close();
	delete pFile;
	DeleteFile(_T("c:\\boot.ini"));
	pFile= new CFile( _T("c:\\boot.ini"),CFile::modeCreate|CFile::modeReadWrite);
	pFile->Write((LPCTSTR)sNewBuffer,sNewBuffer.GetLength()+1);
	
	
	pFile->Close();
	
	CFile::GetStatus( _T("c:\\boot.ini"), status );
	status.m_attribute = status.m_attribute | 0x01;
	delete pFile;
*/
}

BOOL CVipCleanReg::GetDisplaySettings(DWORD &dwHeigth, 
									  DWORD &dwwidth,
									  DWORD &colour,
									  DWORD &dwHZ)
{
	CString Name;
	
	DEVMODE lpDevMode,DevModeReg;      
	
	BOOL a= EnumDisplaySettings(
								NULL,
								ENUM_CURRENT_SETTINGS,
								&lpDevMode
								);
	
	dwHeigth=lpDevMode.dmPelsHeight;
	dwwidth=lpDevMode.dmPelsWidth;
	colour=lpDevMode.dmBitsPerPel;
	dwHZ = lpDevMode.dmDisplayFrequency;
	
	BOOL b= EnumDisplaySettings(
								NULL,
								ENUM_REGISTRY_SETTINGS,
								&DevModeReg
								);
	
	return a && b && (lpDevMode.dmPelsHeight==DevModeReg.dmPelsHeight)
		&& (lpDevMode.dmPelsWidth==DevModeReg.dmPelsWidth)
		&& (lpDevMode.dmBitsPerPel==DevModeReg.dmBitsPerPel)
		&& (lpDevMode.dmDisplayFrequency==DevModeReg.dmDisplayFrequency);
}

BOOL CVipCleanReg::DelKey(CString SubKey)
{


	LONG		lResult;
	TCHAR		cSubKeyName[_MAX_PATH];
	DWORD		dwLen = _MAX_PATH;
	HKEY		hKey=NULL;
	HKEY		hDeleteKey=HKEY_LOCAL_MACHINE;
	FILETIME	ft;

	lResult = RegOpenKeyEx(hDeleteKey,(LPCTSTR)SubKey,0,KEY_WRITE|KEY_READ,&hKey);
	if (   (lResult != ERROR_SUCCESS)
		|| (hKey == NULL)
		)
	{
		return FALSE;
	}

	while (ERROR_SUCCESS == (lResult = RegEnumKeyEx(hKey,0,cSubKeyName,&dwLen,NULL,NULL,NULL,&ft)))
	{
        LPTSTR pszRegPathSubkey = new TCHAR[lstrlen(SubKey) + dwLen + 2];
        wsprintf( pszRegPathSubkey, _T("%s\\%s"), SubKey, cSubKeyName );

		// recurse call
		DelKey(pszRegPathSubkey);

        delete []pszRegPathSubkey;

		dwLen = _MAX_PATH;
	}
	RegCloseKey(hKey);

	lResult = RegDeleteKey(hDeleteKey,SubKey);

	if (lResult==ERROR_SUCCESS)
		return TRUE;

	else return FALSE;

}

BOOL CVipCleanReg::SetInt(const CString &sKey,const CString &sEntry, int nValue)
{
	BOOL	bResult = FALSE;
    HKEY	hKey = NULL;
	LONG	lResult = 0;

    // Create the driver Registry key.
    lResult = RegCreateKey(HKEY_LOCAL_MACHINE, sKey, &hKey);
	if(lResult == ERROR_SUCCESS)
	{
		// Add the appropriate values.
		lResult = RegSetValueEx( hKey, sEntry, 0, REG_DWORD, (const BYTE*)&nValue, sizeof(nValue));
		if(lResult == ERROR_SUCCESS)
			bResult = TRUE;
	
		RegCloseKey( hKey );
	}

	return bResult;
}

int CVipCleanReg::GetInt(const CString &sKey,const CString &sEntry, int /*nDefault*/)
{
    HKEY	hKey		= NULL;
	LONG	lResult		= 0;
	DWORD	dwType		= REG_DWORD;
	DWORD	dwLen		= sizeof(DWORD);
	CString	sValue		= _T("");
	DWORD	dwValue		= 0;

	lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, sKey, 0, KEY_QUERY_VALUE, &hKey);
	if(lResult == ERROR_SUCCESS)
		RegQueryValueEx(hKey, sEntry, NULL, &dwType, (LPBYTE)&dwValue,&dwLen);
	
	RegCloseKey(hKey);

	return (int)dwValue;
}
