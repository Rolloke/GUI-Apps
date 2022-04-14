// CIPCOutputActualImage.cpp: implementation of the CIPCOutputActualImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CIPCOutputActualImage.h"
#include "ActualImageProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCOutputActualImage::CIPCOutputActualImage(CActualImageProcess* pProcess,
											 LPCTSTR shmName)
	:CIPCOutputClient(shmName,FALSE)
{
	m_pProcess = pProcess;
	m_iCurrentCamera = 0;
	m_bIsReady = FALSE;
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCOutputActualImage::~CIPCOutputActualImage()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputActualImage::OnConfirmInfoOutputs(WORD wGroupID, int iNumGroups, 
											int iNumRecords, 
											const CIPCOutputRecord records[])
{
	WK_TRACE(_T("OnConfirmInfoOutputs\n"));
	CIPCOutputClient::OnConfirmInfoOutputs(wGroupID,iNumGroups,iNumRecords,records);
	
	RequestNextCamera();
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputActualImage::RequestNextCamera()
{
	while (m_iCurrentCamera<GetNumberOfOutputs())  //get counts of relays + cameras
	{
		const CIPCOutputRecord& rec = GetOutputRecordFromIndex(m_iCurrentCamera);
		m_iCurrentCamera++;

		if (   rec.IsCamera()						//current outputrecord is camera
			&& rec.CameraHasColor() 
			&& rec.CameraDoesJpeg()
			&& rec.IsReference()
			)
		{
			WK_TRACE(_T("requesting %08lx\n"),rec.GetID().GetID());
			Resolution res = RESOLUTION_2CIF;
			Compression comp = COMPRESSION_5;

			comp = m_pProcess->GetMacro().GetCompression();
			res = m_pProcess->GetMacro().GetResolution();

			DoRequestNewJpegEncoding(rec.GetID(),	// request image from camera
									 res,
									 comp, 
									 1, 
									 1000);
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputActualImage::OnAddRecord(const class CIPCOutputRecord &pRec)
{
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputActualImage::OnDeleteRecord(const class CIPCOutputRecord &pRec)
{
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputActualImage::OnUpdateRecord(const class CIPCOutputRecord &pRec)
{
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputActualImage::OnConfirmJpegEncoding(const CIPCPictureRecord &pict,DWORD dwUserData)
{
	WK_TRACE(_T("onconfirm %08lx\n"),pict.GetCamID().GetID());
	SaveImage(pict);
	if (m_iCurrentCamera>=GetNumberOfOutputs())	//last outputrecord reached,stop sending
												//pictures
	{
		DoRequestNewJpegEncoding(SECID_NO_ID,
								 RESOLUTION_NONE,
								 COMPRESSION_NONE, 
								 0, 
								 0);
		// force disconnect
		m_bIsReady = TRUE;
	}
	else 
	{
		RequestNextCamera();					//request pictures from next camera
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputActualImage::SaveImage(const CIPCPictureRecord &pict)
{
	CString sDirectory;

	sDirectory.Format(_T("%s\\%08lx\\ist"),			//build Directory for saving pictures
		CNotificationMessage::GetWWWRoot(),SECID_LOCAL_HOST);

	if(!WK_CreateDirectory(sDirectory))
	{
		WK_TRACE(_T("cannot create directory %s\n"),sDirectory);
	}
	// save the image

	CFile file;
	CString sPathname;
	BOOL bSaved = FALSE;

	sPathname.Format(_T("%s\\%08lx.jpg"),sDirectory,pict.GetCamID().GetID());

	if (file.Open(sPathname,CFile::modeCreate|CFile::modeWrite))
	{
		TRY
		{
			file.Write(pict.GetData(),pict.GetDataLength());
			file.Flush();
			file.Close();
			bSaved = TRUE;
		}
		CATCH(CFileException, e)
		{
			WK_TRACE(_T("%s\n"),GetLastErrorString(e->m_lOsError));
		}
		END_CATCH
	}

	if (bSaved)
	{
		WK_TRACE(_T("image saved under %s\n"),sPathname);
	}
	else
	{
		WK_TRACE(_T("cannot save image under %s\n"),sPathname);
	}
}
//////////////////////////////////////////////////////////////////////

