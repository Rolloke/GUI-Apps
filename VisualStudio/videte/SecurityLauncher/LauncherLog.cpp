/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: LauncherLog.cpp $
// ARCHIVE:		$Archive: /Project/SecurityLauncher/LauncherLog.cpp $
// CHECKIN:		$Date: 2.03.05 14:34 $
// VERSION:		$Revision: 12 $
// LAST CHANGE:	$Modtime: 2.03.05 14:31 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "SecurityLauncher.h"
#include "LogZip.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
//static TCHAR BASED_CODE szLogDir[] = _T("C:\\Log");
//static TCHAR BASED_CODE szLogFile[] = _T("C:\\Log\\Launcher.log");
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::WriteToLog(CString& sLine)
{
	CString sLogFile;
	sLogFile = GetLogPath()  + "\\Launcher.log";
	if (m_LogFile.Open(sLogFile,
		CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite))//|CFile::shareExclusive))
	{
		DWORD dwLen = 0;
		TRY
		{
			m_LogFile.SeekToEnd();
			m_LogFile.WriteString(sLine);
			dwLen = m_LogFile.GetLength();
			m_LogFile.Close();
		}
		CATCH (CFileException, e)
		{
		}
		END_CATCH

		if (dwLen > (512*1024))
		{
			CString sLogFile;
			SYSTEMTIME st;
			GetLocalTime(&st);
			CString sNew;
			CString sTime; 
			sTime.Format("%02d_%02d_%02d_%02d.log",st.wDay,st.wMonth,st.wHour,st.wMinute);
			sNew = GetLogPath();
			sNew += "\\UPLauncher.log";
			sNew += sTime;
			sLogFile = GetLogPath() + "\\Launcher.log";

			if (rename(sLogFile, sNew)==0)
			{
				CLogZip::CompressLogFile(sNew);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::Trace(const CString& sText)
{
	CString s;
	SYSTEMTIME st;
	GetLocalTime(&st);
	s.Format("%02d.%02d.%04d, %02d:%02d.%02d ",st.wDay,st.wMonth,st.wYear,st.wHour,st.wMinute,st.wSecond);
	s += sText;
	int p = s.GetLength();
	
	if (s[p-1]!='\n')
	{
		s+='\n';
	}

#ifndef _DEBUG
	WriteToLog(s);
#else
	TRACE(s);
#endif
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::Trace(LPCSTR szText, const CString& sText)
{
	CString s = szText;
	s += sText;
	Trace(s);
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::Trace(const CString& sText, LPCSTR szText)
{
	CString s = sText;
	s += szText;
	Trace(s);
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::StatLog(int i, const CString& sValue)
{
	//ClientLog@18.03,10:20:38@0@Connection|uwe.wk.de
	CString s,l;
	SYSTEMTIME st;
	GetLocalTime(&st);
	s.Format("LauncherLog@%d.%d,%d:%d:%d",st.wDay,st.wMonth,st.wHour,st.wMinute,st.wSecond);
	l.Format("%s@%d@%s\n",s,i,sValue);
	WriteToLog(l);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSecurityLauncherApp::DeleteLogFile()
{
	CString sLog;
	CString sLogFile;
	CWK_Profile wkp;
	sLog = wkp.GetString("Log","LogPath","c:\\log");
	if (sLog.IsEmpty())
	{
		sLog = "c:\\log";
	}
	sLogFile = sLog + "\\Launcher.log";

	BOOL bDelete = TRUE;
	TRY
	{
		CFile::Remove(sLogFile);
	}
	CATCH(CFileException, e)
	{
		bDelete = FALSE;
	}
	END_CATCH

	return bDelete;
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::Trace(LPCSTR szText)
{
	CString s = szText;
	Trace(s);
}
/////////////////////////////////////////////////////////////////////////////
// legt das Verzeichnis C:\LOG an, falls noch nicht vorhanden
// 
BOOL CSecurityLauncherApp::CreateCheckLogDir()
{
	CString sLog;
	CString sLogFile;
	CWK_Profile wkp;
	sLog = wkp.GetString("Log","LogPath","c:\\log");
	if (sLog.IsEmpty())
	{
		sLog = "c:\\log";
	}
	WIN32_FIND_DATA FindFileData;
	HANDLE h = FindFirstFile(sLog, &FindFileData);
	if (h == INVALID_HANDLE_VALUE)
	{
		if (!CreateDirectory(sLog,NULL))
		{
			if (m_WatchDog.PiezoSetFlag(PF_CREATE_DIRECTORY))
			{
				// cannot trace no log dir
			}
			return FALSE;
		}
	}
	else
	{
		FindClose(h);
	}
	return TRUE;
}
static char szDaysOfWeek[7][4] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
static char szTimeFormat[] = "%s,%02d.%02d.%04d %02d:%02d.%02d,%04d";
/////////////////////////////////////////////////////////////////////////////
CString	SystemTimeToString(const SYSTEMTIME& st)
{
	CString ret;
	CString sDayOfWeek;

	sDayOfWeek = szDaysOfWeek[st.wDayOfWeek];

	ret.Format(szTimeFormat,(const char*)sDayOfWeek,
							st.wDay,
							st.wMonth,
							st.wYear,
							st.wHour,
							st.wMinute,
							st.wSecond,
							st.wMilliseconds);

	return ret;
}
/////////////////////////////////////////////////////////////////////////////
BOOL StringToSystemTime(const CString& s,SYSTEMTIME& st)
{
	CString sT;
	CString sOne;
	int p;
	BOOL bFound = FALSE;

	sT = s;
	// Day of week
	p = sT.Find(',');
	if (p==-1)
	{
		return FALSE;
	}
	sOne = sT.Left(p);
	if (sOne.IsEmpty())
	{
		return FALSE;
	}
	for (WORD i=0;i<7;i++)
	{
		if (0==sOne.CompareNoCase(szDaysOfWeek[i]))
		{
			st.wDayOfWeek = i;
			bFound = TRUE;
			break;
		}
	}
	if (!bFound)
	{
		return FALSE;
	}
	sT = sT.Mid(p+1);

	// day
	p = sT.Find('.');
	if (p==-1)
	{
		return FALSE;
	}
	sOne = sT.Left(p);
	if (sOne.IsEmpty())
	{
		return FALSE;
	}
	st.wDay = (WORD)atoi((const char*)sOne);
	sT = sT.Mid(p+1);

	// month
	p = sT.Find('.');
	if (p==-1)
	{
		return FALSE;
	}
	sOne = sT.Left(p);
	if (sOne.IsEmpty())
	{
		return FALSE;
	}
	st.wMonth = (WORD)atoi((const char*)sOne);
	sT = sT.Mid(p+1);

	// year
	p = sT.Find(' ');
	if (p==-1)
	{
		return FALSE;
	}
	sOne = sT.Left(p);
	if (sOne.IsEmpty())
	{
		return FALSE;
	}
	st.wYear = (WORD)atoi((const char*)sOne);
	sT = sT.Mid(p+1);

	// hour
	p = sT.Find(':');
	if (p==-1)
	{
		return FALSE;
	}
	sOne = sT.Left(p);
	if (sOne.IsEmpty())
	{
		return FALSE;
	}
	st.wHour = (WORD)atoi((const char*)sOne);
	sT = sT.Mid(p+1);

	// minute
	p = sT.Find('.');
	if (p==-1)
	{
		return FALSE;
	}
	sOne = sT.Left(p);
	if (sOne.IsEmpty())
	{
		return FALSE;
	}
	st.wMinute = (WORD)atoi((const char*)sOne);
	sT = sT.Mid(p+1);

	// second
	p = sT.Find(',');
	if (p==-1)
	{
		return FALSE;
	}
	sOne = sT.Left(p);
	if (sOne.IsEmpty())
	{
		return FALSE;
	}
	st.wSecond = (WORD)atoi((const char*)sOne);
	sT = sT.Mid(p+1);

	// millisecond
	sOne = sT;
	if (sOne.IsEmpty())
	{
		return FALSE;
	}
	st.wMilliseconds = (WORD)atoi((const char*)sOne);

	return TRUE;
}
#pragma warning(disable: 4244) // warning C4244: '=' : conversion from 'int' to 'unsigned short', possible loss of data
/////////////////////////////////////////////////////////////////////////////
void CalcTimeDiff(const SYSTEMTIME& stYoung,
					 const SYSTEMTIME& stOld,
					 SYSTEMTIME& stResult)
{
	WORD wUT = 0;
	int res;

	// ms
	res = stYoung.wMilliseconds - stOld.wMilliseconds - wUT;
	wUT = 0;
	if (res<0)
	{
		res += 1000;
		wUT = 1;
	}
	stResult.wMilliseconds = res;

	// s
	res = stYoung.wSecond - stOld.wSecond - wUT;
	wUT = 0;
	if (res<0)
	{
		res += 60;
		wUT = 1;
	}
	stResult.wSecond = res;

	// min
	res = stYoung.wMinute - stOld.wMinute - wUT;
	wUT = 0;
	if (res<0)
	{
		res += 60;
		wUT = 1;
	}
	stResult.wMinute = res;

	// h
	res = stYoung.wHour - stOld.wHour - wUT;
	wUT = 0;
	if (res<0)
	{
		res += 24;
		wUT = 1;
	}
	stResult.wHour = res;

	// d
	res = stYoung.wDay - stOld.wDay - wUT;
	wUT = 0;
	if (res<0)
	{
		res += 31;
		wUT = 1;
	}
	stResult.wDay = res;

	// month
	res = stYoung.wMonth - stOld.wMonth - wUT;
	wUT = 0;
	if (res<0)
	{
		res += 12;
		wUT = 1;
	}
	stResult.wMonth = res;

	// year
	res = stYoung.wYear - stOld.wYear - wUT;
	wUT = 0;
	if (res<0)
	{
		res = 0;
	}
	stResult.wYear = res;
	stResult.wDayOfWeek = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::TraceSystemInformation()
{
	TIME_ZONE_INFORMATION ti;
	CString s;
	CString sn,dn;

	GetTimeZoneInformation(&ti);
	sn = ti.StandardName;
	dn = ti.DaylightName;
	Trace("TimeZoneInformation");
	s.Format("Bias = %d",ti.Bias);
	Trace(s);
	s.Format("StandardName = %s",sn);
	Trace(s);
	s.Format("DaylightName = %s",dn);
	Trace(s);
	s.Format("DaylightBias = %d",ti.DaylightBias);
	Trace(s);

	HDC hDC = GetDC(NULL);
	s.Format("Screen Resolution is %dx%dx%dbpp",
			GetSystemMetrics(SM_CXSCREEN),
			GetSystemMetrics(SM_CYSCREEN),
			GetDeviceCaps(hDC,BITSPIXEL));
	ReleaseDC(NULL,hDC);
	Trace(s);


}
