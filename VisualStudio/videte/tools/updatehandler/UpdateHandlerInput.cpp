/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: UpdateHandlerInput.cpp $
// ARCHIVE:		$Archive: /Project/Tools/UpdateHandler/UpdateHandlerInput.cpp $
// CHECKIN:		$Date: 12.05.06 21:11 $
// VERSION:		$Revision: 29 $
// LAST CHANGE:	$Modtime: 12.05.06 21:11 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "UpdateHandler.h"
#include "UpdateHandlerInput.h"
#include "mainfrm.h"
#include "DirContent.h"

extern CUpdateHandler theApp;

//////////////////////////////////////////////////////////////////////////////////////
CUpdateHandlerInput::CUpdateHandlerInput(LPCTSTR shmName
	, CSecID id
#ifdef _UNICODE
	, WORD wCodePage
#endif
	)
	: CIPCInput(shmName,FALSE)
{
	m_ID = id;
#ifdef _UNICODE
	m_wCodePage = wCodePage;
#endif
	m_dwBitrate = 65535;
	memset(&m_OSVersionInfo,0,sizeof(OSVERSIONINFO));
	m_OSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	memset(&m_SystemInfo,0,sizeof(SYSTEM_INFO));
	memset(&m_MemoryStatus,0,sizeof(MEMORYSTATUS));
	m_MemoryStatus.dwLength = sizeof(MEMORYSTATUS);

	m_sRemoteTemp = _T("c:\\");
	StartThread();
}
//////////////////////////////////////////////////////////////////////////////////////
CUpdateHandlerInput::~CUpdateHandlerInput()
{
	CDirContent* pDirContent=NULL;

	// Iterate through the entire map, dumping both name and age.
	while(m_DirectoryInfo.GetCount())
	{
		pDirContent = (CDirContent*) m_DirectoryInfo.RemoveHead();
		WK_DELETE(pDirContent);
	}

	StopThread();
}
//////////////////////////////////////////////////////////////////////////////////////
void CUpdateHandlerInput::OnReadChannelFound()
{
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////////////////////
void CUpdateHandlerInput::OnConfirmConnection()
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_INPUT_CONNECT);
	DoRequestBitrate();
	DoRequestInfoInputs(SECID_NO_GROUPID);
}
//////////////////////////////////////////////////////////////////////////////////////
void CUpdateHandlerInput::OnConfirmBitrate(DWORD dwBitrate)
{
	m_dwBitrate = (dwBitrate * 85) / 100;
	WK_TRACE(_T("Bitrate = %d\n"),m_dwBitrate);
}
//////////////////////////////////////////////////////////////////////////////////////
void CUpdateHandlerInput::OnRequestDisconnect()
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_INPUT_DISCONNECT);
}
//////////////////////////////////////////////////////////////////////////////////////
void CUpdateHandlerInput::SetHost(const CString& sHost)
{
	int i,c;

	m_sHost.Empty();
	c = sHost.GetLength();
	for (i=0;i<c;i++)
	{
		if (isalnum(sHost[i]))
		{
			m_sHost += sHost[i];
		}
	}
	if (m_sHost.IsEmpty())
	{
		m_sHost = _T("unbekannt");
	}

}
//////////////////////////////////////////////////////////////////////////////////////
void CUpdateHandlerInput::OnConfirmGetFile(
				int iDestination,
				const CString &sFileName,
				const void *pData,
				DWORD dwDataLen, DWORD dwCodePage)
{
#ifdef _UNICODE
	BOOL bUnicode = m_wCodePage == CODEPAGE_UNICODE ? TRUE : FALSE;
#endif
	if (iDestination<CFU_DIRECTORY)
	{
		CFile file;
		CString sDestination;
		int p;

		p = sFileName.ReverseFind(_T('\\'));
		if (p!=-1)
		{
			sDestination = sFileName.Mid(p+1);
		}
		else
		{
			sDestination = sFileName;
		}
		sDestination = theApp.GetDestination() + _T('\\') + m_sHost + _T('\\') + sDestination;

		CString sDir;
		sDir = theApp.GetDestination()+_T('\\')+m_sHost;
		if (!DoesFileExist(sDir))
		{
			CreateDirectory(sDir,NULL);
		}

		if (file.Open(sDestination,CFile::modeCreate|CFile::modeWrite))
		{
			WK_TRACE(_T("OnConfirmGetFile %s copied to %s\n"),sFileName,sDestination);
			file.Write(pData,dwDataLen);
			file.Flush();
			file.Close();
			m_sLastGot = sFileName;
			m_sLastWrittenTo = sDestination;
			AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_FILE_GOT);
		}
	}
	else if (iDestination==CFU_DELETE_FILE)
	{
		WK_TRACE(_T("OnConfirmDeleteFile %s\n"),sFileName);
		m_sLastDeleted = sFileName;
		AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_FILE_DELETED);
	}
	else if (iDestination==CFU_EXECUTE_FILE)
	{
		WK_TRACE(_T("OnConfirmExecuteFile %s\n"),sFileName);
		m_sLastExecuted = sFileName;
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_EXECUTED);
	}
	else if (iDestination==CFU_DIRECTORY)
	{
		if (m_wCodePage == CODEPAGE_UNICODE)
		{
			dwDataLen |= FLAG_FILE_DATA_IS_UNICODE;
		}
		CDirContent* pDirContent = new CDirContent(sFileName,pData,dwDataLen);
		CString sDir = sFileName;
		m_DirectoryInfo.AddTail(pDirContent);
		AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_DIRECTORY_ARRIVED,(LPARAM)(LPVOID)pDirContent);
	}
	else if (iDestination==CFU_GET_LOGICAL_DRIVES)
	{
		CWK_String sAnswer;
#ifdef _UNICODE
		sAnswer.InitFromMemory(bUnicode, pData, dwDataLen, m_wCodePage);
#else
		sAnswer.InitFromMemory(pData, dwDataLen);
#endif
		WK_TRACE(_T("OnConfirmGetFile GetLogicalDrives(%s,%s)\n"),sFileName,sAnswer);
		m_DriveInfo = sAnswer;
		AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_DRIVES);
	}
	else if (iDestination==CFU_GET_DISK_FREE_SPACE)
	{
		DWORD dwA[4];
		CopyMemory((void*)&dwA,pData,sizeof(dwA));
		WK_TRACE(_T("OnConfirmGetFile GetDiskFreeSpace(%s,%d,%d,%d,%d)\n"),
			sFileName,dwA[0],dwA[1],dwA[2],dwA[3]);
	}
	else if (iDestination==CFU_GET_OS_VERSION)
	{
		WK_TRACE(_T("OnConfirmGetFile GetOSVersion()\n"));
		CopyMemory(&m_OSVersionInfo,pData,dwDataLen);
	}
	else if (iDestination==CFU_GET_SYSTEM_INFO)
	{
		WK_TRACE(_T("OnConfirmGetFile GetSystemInfo()\n"));
		CopyMemory(&m_SystemInfo,pData,dwDataLen);
	}
	else if (iDestination==CFU_GET_CURRENT_TIME)
	{
		CWK_String sAnswer;
#ifdef _UNICODE
		sAnswer.InitFromMemory(bUnicode, pData, dwDataLen, m_wCodePage);
#else
		sAnswer.InitFromMemory(pData, dwDataLen);
#endif
		WK_TRACE(_T("OnConfirmGetFile GetCurrentTime(%s,%s)\n"),sFileName,sAnswer);
		m_sCurrentTime = sAnswer;
		AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_TIME_ARRIVED,0);
	}
	else if (iDestination==CFU_GET_SOFT_VERSION)
	{
		CWK_String sAnswer;
#ifdef _UNICODE
		sAnswer.InitFromMemory(bUnicode, pData, dwDataLen, m_wCodePage);
#else
		sAnswer.InitFromMemory(pData, dwDataLen);
#endif
		WK_TRACE(_T("OnConfirmGetFile GetSoftwareVersion(%s)\n"),sAnswer);
		m_SoftwareVersion = sAnswer;
	}
	else if (iDestination==CFU_GET_FILE_VERSION)
	{
		WK_TRACE(_T("OnConfirmGetFile GetFileVersion(%s)\n"),sFileName);
	}
	else if (iDestination==CFU_GLOBAL_MEMORY_STATUS)
	{
		WK_TRACE(_T("OnConfirmGetFile GlobalMemoryStatus()\n"));
		CopyMemory(&m_MemoryStatus,pData,dwDataLen);
	}
	else if (iDestination==CFU_EXPORT_REGISTRY)
	{
		WK_TRACE(_T("OnConfirmGetFile ExportRegistry()\n"));
		CFile file;
		CString sFile;

		if (m_sHost.IsEmpty())
		{
			sFile = GetLogPath() + _T("\\Unbekannte Gegenstation.reg");
		}
		else
		{
			sFile = GetLogPath() + _T("\\") + m_sHost + _T(".reg");
		}

		if (file.Open(sFile,CFile::modeCreate|CFile::modeWrite))
		{
			file.Write(pData,dwDataLen);
			file.Close();
		}
	}
	else if (iDestination==CFU_ENUMREGKEYS)
	{
		CWK_String sAnswer;
#ifdef _UNICODE
		sAnswer.InitFromMemory(bUnicode, pData, dwDataLen, m_wCodePage);
#else
		sAnswer.InitFromMemory(pData, dwDataLen);
#endif
		WK_TRACE(_T("OnConfirmGetFile EnumRegKeys (%s,%s)\n"),sFileName,sAnswer);
		m_DriveInfo = sFileName + _T("->") + sAnswer;
		AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_DRIVES);
	}
	else if (iDestination==CFU_ENUMREGVALUES)
	{
		CWK_String sAnswer;
#ifdef _UNICODE
		sAnswer.InitFromMemory(bUnicode, pData, dwDataLen, m_wCodePage);
#else
		sAnswer.InitFromMemory(pData, dwDataLen);
#endif
		m_RegInfo = sFileName + _T("->") + sAnswer;
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_REG_ENUM_VALUES);
	}
	else if (iDestination==CFU_GETREGKEY)
	{
		CWK_String sAnswer;
#ifdef _UNICODE
		sAnswer.InitFromMemory(bUnicode, pData, dwDataLen, m_wCodePage);
#else
		sAnswer.InitFromMemory(pData, dwDataLen);
#endif
		m_RegInfo = sFileName + _T("=>") + sAnswer;
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_REG_GET_VALUE);
	}
	else if (iDestination==CFU_SETREGKEY)
	{
		CString sPathValue, sAnswer;
		int n = sFileName.Find(_T("::"));
		if (n != -1) n = sFileName.Find(_T("::"), n+1);
		if (n != -1)
		{
			sPathValue = sFileName.Left(n);
			sAnswer = sFileName.Mid(n+2);
			n = sAnswer.Find(_T(":"));
			if (n != -1)
			{
				sAnswer = sAnswer.Left(n) + _T(":0") + sAnswer.Mid(n);
				m_RegInfo = sPathValue + _T("=>") + sAnswer;
				AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_REG_GET_VALUE);
			}
		}
		// only success notification
	}
	else if (iDestination==CFU_DELETEREGKEY)
	{
		m_sLastDeleted = sFileName;
		m_sLastDeleted.Replace(_T(" "), _T("\\"));
		AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_FILE_DELETED);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CUpdateHandlerInput::OnConfirmFileUpdate(const CString &sFileName)
{
	CString* psBuffer = new CString(sFileName);

	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_CONFIRM_FILE_UPDATE,(LPARAM)psBuffer);

	WK_TRACE(_T("OnConfirmFileUpdate(%s)\n"), sFileName);
}
//////////////////////////////////////////////////////////////////////////////////////
void CUpdateHandlerInput::OnIndicateError(DWORD dwCmd, CSecID id, 
								 CIPCError error, int iErrorCode,
								 const CString &sErrorMessage)
{
	WK_TRACE(_T("OnIndicateError(%s,%s,%d,%s)\n"),
			  NameOfCmd(dwCmd), NameOfEnum(error), iErrorCode, sErrorMessage);
	m_sLastError = sErrorMessage;
	AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_ERROR);
}
//////////////////////////////////////////////////////////////////////////////////////
const CDirContent* CUpdateHandlerInput::GetDirContent(const CString& sDir, POSITION &posFound) const
{
	CDirContent* pDirContent;
	POSITION pos;
	pos = m_DirectoryInfo.GetHeadPosition();

	while (pos)
	{
		posFound = pos;
		pDirContent = (CDirContent*)m_DirectoryInfo.GetNext(pos);
		if (pDirContent->GetDirectory() == sDir)
		{
			return pDirContent;
		}
	}
	posFound = NULL;
	return NULL;
}
//////////////////////////////////////////////////////////////////////////////////////
void	CUpdateHandlerInput::DeleteDirContent(CDirContent*pDI)
{
	POSITION pos = m_DirectoryInfo.Find(pDI);
	if (pos)
	{
		m_DirectoryInfo.RemoveAt(pos);
	}
	WK_DELETE(pDI);
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CUpdateHandlerInput::RemoveFileFromDirContent(const CString& sFile)
{
	CString sFileName;
	CString sDirName;
	int p;
	CDirContent* pDirContent = NULL;

	p = sFile.ReverseFind(_T('\\'));
	if (p==-1)
	{
		return FALSE;
	}
	sDirName = sFile.Left(p+1);
	sFileName = sFile.Mid(p+1);

	POSITION pos;
	pDirContent = (CDirContent*)GetDirContent(sDirName, pos);
	if (pDirContent)
	{
		pDirContent->Remove(sFileName);
	}
	
	pDirContent = (CDirContent*)GetDirContent(sFile, pos);
	if (pDirContent)
	{
		m_DirectoryInfo.RemoveAt(pos);
		WK_DELETE(pDirContent);
	}
	return TRUE;
}
