// ActualOutputImage.cpp: implementation of the CActualOutputImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "actualimage.h"
#include "ActualOutputImage.h"

#include "wk_util.h"
#include "NotificationMessage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CActualOutputImage::CActualOutputImage(const char* shmName)
:CIPCOutputClient(shmName,FALSE)
{
	m_iCurrentCamera = 0;
	m_bIsReady = FALSE;
	m_bDeleteDir = TRUE;
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CActualOutputImage::~CActualOutputImage()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CActualOutputImage::OnConfirmInfoOutputs(WORD wGroupID, int iNumGroups, 
											int iNumRecords, 
											const CIPCOutputRecord records[])
{
	//build Directory string for saving pictures
	m_sDirectory.Format("%s\\%08lx\\ist",			
	CNotificationMessage::GetWWWRoot(),SECID_LOCAL_HOST);

	WK_TRACE("OnConfirmInfoOutputs\n");
	CIPCOutputClient::OnConfirmInfoOutputs(wGroupID,iNumGroups,iNumRecords,records);
		WK_TRACE("Number of records: %d\n", iNumRecords );
	if (GetNumberOfOutputs() > 0) {
		RequestNextCamera();
	}
	else{
		m_bIsReady = TRUE;
		WK_TRACE("output records = %d\n", iNumRecords );
	}
	// Stop
}
//////////////////////////////////////////////////////////////////////
void CActualOutputImage::RequestNextCamera()
{
	// delete directory only once
	if(m_bDeleteDir)
	{
		if(!DeleteDirRecursiv(m_sDirectory))
		{
			WK_TRACE("cannot delete directory \"%s\"\n", m_sDirectory);
		}
		else
		{
			WK_TRACE("directory \"%s\" deleted\n", m_sDirectory);
		}
		m_bDeleteDir = FALSE;
	}
	

	WK_TRACE("RequestNextCamera %d\n",m_iCurrentCamera);

	while (m_iCurrentCamera<GetNumberOfOutputs())  //get counts of relays + cameras
	{
		const CIPCOutputRecord& rec = GetOutputRecordFromIndex(m_iCurrentCamera);

		if (   rec.IsCamera()						//current outputrecord is camera
			&& rec.CameraHasColor() 
			&& rec.CameraDoesJpeg()
			)
		{
			WK_TRACE("requesting %08lx\n",rec.GetID().GetID());
			
			DoRequestNewJpegEncoding(rec.GetID(),	// request image from camera
									 RESOLUTION_HIGH,
									 COMPRESSION_5, 
									 1, 
									 IMAGETIMER);
			break;
		}
		else {
			// no camera, next output
			m_iCurrentCamera++;
		}
	}
	if (m_iCurrentCamera >= GetNumberOfOutputs()) {
		// force disconnect
		m_bIsReady = TRUE;
	}
}


//////////////////////////////////////////////////////////////////////
void CActualOutputImage::OnAddRecord(const class CIPCOutputRecord &pRec)	
{
}
//////////////////////////////////////////////////////////////////////
void CActualOutputImage::OnDeleteRecord(const class CIPCOutputRecord &pRec)
{
}
//////////////////////////////////////////////////////////////////////
void CActualOutputImage::OnUpdateRecord(const class CIPCOutputRecord &pRec)
{
}
//////////////////////////////////////////////////////////////////////
void CActualOutputImage::OnConfirmJpegEncoding(const CIPCPictureRecord &pict,DWORD dwUserData)
{
	WK_TRACE("onconfirm %08lx\n",pict.GetCamID().GetID());

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
		m_iCurrentCamera++;
		SaveImage(pict);
		RequestNextCamera();					//request pictures from next camera
	}
}
//////////////////////////////////////////////////////////////////////
void CActualOutputImage::SaveImage(const CIPCPictureRecord &pict)
{
	if(!WK_CreateDirectory(m_sDirectory))
	{
		WK_TRACE("cannot create directory \"%s\"\n",m_sDirectory);
	}
	else
	{
		WK_TRACE("directory \"%s\" created\n",m_sDirectory);
	}
	// save the image

	CFile file;
	CString sPathname;
	BOOL bSaved = FALSE;

	sPathname.Format("%s\\%08lx.jpg",m_sDirectory,pict.GetCamID().GetID());

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
			WK_TRACE("%s\n",GetLastErrorString(e->m_lOsError));
		}
		END_CATCH
	}

	if (bSaved)
	{
		WK_TRACE("image saved under %s\n",sPathname);
	}
	else
	{
		WK_TRACE("cannot save image under %s\n",sPathname);
	}
}
//////////////////////////////////////////////////////////////////////
	


