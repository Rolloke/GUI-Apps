#include "stdafx.h"
#include "UploadThread.h"
#include "UploadProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


BOOL CUploadThread::m_bTraceFTP = FALSE;
//////////////////////////////////////////////////////////////////////////
CUploadItem::CUploadItem(const CIPCPictureRecord& pict,
			const CString& sServername,
			const CString& sLogin,
			const CString& sPassword,
			const CString& sCameraname,
			const CString& sFilename)
{
	m_pPicture = new CIPCPictureRecord(pict);
	m_sServername = sServername;
	m_sLogin = sLogin;
	m_sPassword = sPassword;
	m_sCameraname = sCameraname;
	m_sFilename = sFilename;
}
//////////////////////////////////////////////////////////////////////////
CUploadItem::~CUploadItem()
{
	WK_DELETE(m_pPicture);
}

//////////////////////////////////////////////////////////////////////////
CUploadThread::CUploadThread(): CWK_Thread(_T("Upload"))
{
	m_dwStatTime = 12*60*60*1000;
}
//////////////////////////////////////////////////////////////////////////
CUploadThread::~CUploadThread()
{
	m_Queue.SafeDeleteAll();
}
////////////////////////////////////////////////////
BOOL CUploadThread::Run(LPVOID lpContext)
{
	CUploadItem* pItem = NULL;

	m_Queue.Lock();
	if (m_Queue.GetCount())
	{
		pItem = m_Queue.GetAtFast(0);
		m_Queue.RemoveAt(0);
	}
	m_Queue.Unlock();

	if (pItem)
	{
		Upload(pItem);
		WK_DELETE(pItem);
	}

	return TRUE;
}
////////////////////////////////////////////////////
void CUploadThread::AddPicture(const CIPCPictureRecord& pict, CUploadProcess* pProcess)
{
	CUploadItem* pItem = new CUploadItem(pict,
		pProcess->GetMacro().GetServerName(),
		pProcess->GetMacro().GetLogin(),
		pProcess->GetMacro().GetPassword(),
		pProcess->GetCameraName(),
		pProcess->GetMacro().GetFilename()
		);
	m_Queue.SafeAdd(pItem);
}
////////////////////////////////////////////////////
BOOL CUploadThread::Upload(const CUploadItem* pItem)
{
	PBYTE pBuffer = NULL;
	DWORD dwLen = 0;
	BOOL bRet = FALSE;

	if (pItem->m_pPicture->GetCompressionType() == COMPRESSION_JPEG)
	{
		if (pItem->m_pPicture->GetResolution() == RESOLUTION_2CIF)
		{
			m_JpegDecoder.DecodeJpegFromMemory((PBYTE)pItem->m_pPicture->GetData(),pItem->m_pPicture->GetDataLength());
			HANDLE hDIB = m_JpegDecoder.CreateDIB();
			if (hDIB)
			{
				dwLen = 256*1024;
				pBuffer = new BYTE[dwLen];
				bRet = m_JpegEncoder.EncodeJpegToMemory(hDIB, pBuffer,dwLen);
				GlobalFree(hDIB);		
			}
		}
		else
		{
			dwLen = pItem->m_pPicture->GetDataLength();
			pBuffer = new BYTE[dwLen];
			CopyMemory(pBuffer,pItem->m_pPicture->GetData(),dwLen);
		}
	}
	else if (   pItem->m_pPicture->GetCompressionType() == COMPRESSION_YUV_422
		|| pItem->m_pPicture->GetCompressionType() == COMPRESSION_RGB_24)
	{
		BYTE* pData = (BYTE*)pItem->m_pPicture->GetData();
		LPBITMAPINFO lpBMI = (LPBITMAPINFO)pData;
		m_JpegDecoder.SetDIBData(lpBMI);
		HANDLE hDIB = m_JpegDecoder.CreateDIB();
		if (hDIB)
		{
			dwLen = 256*1024;
			pBuffer = new BYTE[dwLen];
			bRet = m_JpegEncoder.EncodeJpegToMemory(hDIB, pBuffer,dwLen);
			GlobalFree(hDIB);		
		}
	}
	else if (pItem->m_pPicture->GetCompressionType() == COMPRESSION_MPEG4)
	{
		m_Mpeg4Decoder.DecodeMpeg4FromMemory((PBYTE)pItem->m_pPicture->GetData(),pItem->m_pPicture->GetDataLength());
		CDib* pDib = m_Mpeg4Decoder.GetDib();
		HANDLE hDIB = pDib->GetHDIB();
		if (hDIB)
		{
			dwLen = 256*1024;
			pBuffer = new BYTE[dwLen];
			bRet = m_JpegEncoder.EncodeJpegToMemory(hDIB,pBuffer,dwLen);
			GlobalFree(hDIB);
		}
		WK_DELETE(pDib);
	}

	if (pBuffer)
	{
		CString sServer;
		CString sFile;
		CString sDirectory;

		sServer = pItem->m_sServername;

		if (0==sServer.Find(_T("ftp://")))
		{
			sServer = sServer.Mid(6);
		}
		int p = sServer.ReverseFind(_T('/'));
		if (p>0)
		{
			sDirectory = sServer.Mid(p+1);
			sServer = sServer.Left(p);
		}
		CString sFileName = pItem->m_sFilename;
		CString sName = pItem->m_sCameraname;
		sFile.Format(_T("%08lx.jpg"),pItem->m_pPicture->GetCamID().GetID());
		if (!sFileName.IsEmpty())
		{
			CSystemTime t = pItem->m_pPicture->GetTimeStamp();
			// nnnYYYYMMDD
			// Replace YY with current year;
			if (-1!=sFileName.Find(_T("YYYY")))
			{
				CString sYear;
				sYear.Format(_T("%04d"),t.wYear);
				sFileName.Replace(_T("YYYY"),sYear);
			}
			else if (-1!=sFileName.Find(_T("YY")))
			{
				CString sYear;
				sYear.Format(_T("%02d"),t.wYear%100);
				sFileName.Replace(_T("YY"),sYear);
			}
			if (-1!=sFileName.Find(_T("MM")))
			{
				CString sMonth;
				sMonth.Format(_T("%02d"),t.wMonth);
				sFileName.Replace(_T("MM"),sMonth);
			}
			if (-1!=sFileName.Find(_T("DD")))
			{
				CString sDay;
				sDay.Format(_T("%02d"),t.wDay);
				sFileName.Replace(_T("DD"),sDay);
			}
			if (-1!=sFileName.Find(_T("hh")))
			{
				CString s;
				s.Format(_T("%02d"),t.wHour);
				sFileName.Replace(_T("hh"),s);
			}
			if (-1!=sFileName.Find(_T("mm")))
			{
				CString s;
				s.Format(_T("%02d"),t.wMinute);
				sFileName.Replace(_T("mm"),s);
			}
			if (-1!=sFileName.Find(_T("ss")))
			{
				CString s;
				s.Format(_T("%02d"),t.wSecond);
				sFileName.Replace(_T("ss"),s);
			}
			// und jetzt alle nnn durch den Kameranamen ersetzen
			CString sCopy;
			for (int i=0,j=0;i<sFileName.GetLength();i++)
			{
				if (sFileName[i]==_T('n'))
				{
					if (j<sName.GetLength())
					{
						sCopy += sName[j++];
					}
				}
				else
				{
					sCopy += sFileName[i];
				}
			}
			sFileName = sCopy;
			CString sForbidden = _T("\\/:*?\"<>|");
			for (int i=0;i<sForbidden.GetLength();i++)
			{
				sFile.Replace(sForbidden[i],_T(''));
			}


			sFile = sFileName + _T(".jpg");
			sFile.Replace(_T(' '),_T('_'));
		}

		CInternetSession is;
		CFtpConnection* pFTP = NULL;
		try
		{
			if (m_bTraceFTP)
			{
				WK_TRACE(_T("ftp upload %s -> %s %s\n"),sFile,sServer,sDirectory);
			}
			pFTP = is.GetFtpConnection(sServer,pItem->m_sLogin,pItem->m_sPassword);
			if (pFTP)
			{
				if (!sDirectory.IsEmpty())
				{
					pFTP->SetCurrentDirectory(sDirectory);
				}
				CInternetFile* pFile = pFTP->OpenFile(sFile,GENERIC_WRITE);
				if (   pFile
					&& pBuffer)
				{
					pFile->Write(pBuffer,dwLen);
					pFile->Close();
					delete pFile;
				}
				bRet = TRUE;
				pFTP->Close();
				delete pFTP;
			}
		}
		catch (CInternetException* pEx) 
		{
			CString s;
			pEx->GetErrorMessage(s.GetBufferSetLength(1024),1024);
			s.ReleaseBuffer();
			CString sMessage;
			sMessage.Format(_T("FTP upload failed %s -> %s %s %s\n"),sFile,sServer,sDirectory,s);
			CWK_RunTimeError e(WAI_SECURITY_SERVER,REL_WARNING,RTE_FTP_ERROR,(LPCTSTR)sMessage);
			e.Send();
			pEx->Delete();
			bRet = FALSE;
		}
		WK_DELETE_ARRAY(pBuffer);
	}

	return bRet;
}
