// IPCOutputEmail.cpp: implementation of the CIPCOutputEmail class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EmailUnit.h"
#include "IPCOutputEmail.h"
#include "EmailUnitDlg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCOutputEmail::CIPCOutputEmail(LPCTSTR shmName, CEmailUnitDlg* pDlg, CSecID id
#ifdef _UNICODE
	, WORD wCodePage
#endif
								 )
	: CIPCOutputClient(shmName,FALSE)
{
	m_pDlg = pDlg;
	m_CamID = id;
	m_pPicture = NULL;
	m_bCameraIsMpeg = FALSE;
	m_bCameraIsJpeg = FALSE;

#ifdef _UNICODE
	m_wCodePage = wCodePage;
#endif
	StartThread();
}
//////////////////////////////////////////////////////////////////////////////////////
CIPCOutputEmail::~CIPCOutputEmail()
{
	StopThread();
	WK_DELETE(m_pPicture);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputEmail::OnUpdateRecord(const CIPCOutputRecord &pRec)
{

}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputEmail::OnAddRecord(const CIPCOutputRecord &rec)
{
	if (m_CamID == rec.GetID())
	{
		if (rec.CameraDoesJpeg() && rec.CameraHasColor())
		{
			WK_TRACE(_T("[CIPCOutputEmail::OnAddRecord]\n"));
			WK_TRACE(_T("Jpeg Bild angefordert\n"));
			// mico/jacob
			DoRequestNewJpegEncoding(m_CamID,RESOLUTION_2CIF,COMPRESSION_5,1,1000); 
		}
		else if (!rec.CameraDoesJpeg() && rec.CameraHasColor())
		{
			// coco
			WK_TRACE(_T("[CIPCOutputEmail::OnAddRecord]\n"));
			WK_TRACE(_T("H263 Bild angefordert\n"));
			DoRequestStartH263Encoding(m_CamID,
									   RESOLUTION_2CIF,
									   COMPRESSION_5,
									   1000);
		}
		else if (rec.CameraDoesMpeg())
		{
			DoRequestStartVideo(m_CamID, RESOLUTION_2CIF, COMPRESSION_5, COMPRESSION_MPEG4, 1, 1, 0x0815);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputEmail::OnDeleteRecord(const CIPCOutputRecord &pRec)
{

}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputEmail::OnIndicateVideo(const CIPCPictureRecord& pict,
							 DWORD dwMDX,
							 DWORD dwMDY,
							 DWORD dwUserData,
							 CSecID idArchive)
{
	if (m_pPicture == NULL)
	{
		OnConfirmJpegEncoding(pict, dwUserData, idArchive);
	}
	else
	{
		// no second image
	}

	DoRequestStopVideo(m_CamID, dwUserData);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputEmail::OnIndicateH263Data(const CIPCPictureRecord &pict, DWORD dwJobData,
										 CSecID idArchive)
{
	if (m_pPicture == NULL)
	{
		OnConfirmJpegEncoding(pict,dwJobData,idArchive);
	}
	else
	{
		// no second image
	}
	DoRequestStopH263Encoding(pict.GetCamID());
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputEmail::OnConfirmJpegEncoding(const CIPCPictureRecord &pict, DWORD dwUserID,
											CSecID idArchive)
{
	m_pPicture = new CIPCPictureRecord(pict);

	CIPCOutputRecord *ptrOutputRecord = GetOutputRecordPtrFromSecID(pict.GetCamID());

	m_sCamName      = ptrOutputRecord->GetName();			//Kameranamen holen
	m_JobTime  		= pict.GetTimeStamp();
															//Camera ist Coco wenn nicht Jpeg und Farbe
	m_bCameraIsJpeg = !(!(ptrOutputRecord->CameraDoesJpeg()) && (ptrOutputRecord->CameraHasColor()));	//auf Jpeg prüfen
	m_bCameraIsMpeg = ptrOutputRecord->CameraDoesMpeg();

	WK_TRACE(_T("[CIPCOutputEmail::OnConfirmJpegEncoding]\n"));
	WK_TRACE(_T("Jpeg Bild erhalten\n"));
												// trennen
	DelayedDelete();

	if (m_pDlg)
	{
										// message an dialog senden
		m_pDlg->PostMessage(WM_PICTURE);
		m_pDlg->m_ePictureHasArrived.SetEvent();
	}
}
//////////////////////////////////////////////////////////////////////////////////////
CString CIPCOutputEmail::GetCamName()
{
	return m_sCamName;
}
//////////////////////////////////////////////////////////////////////////////////////
CString CIPCOutputEmail::GetJobTime()
{
	CString sJobTime;
	m_JobTime.GetDay();
	sJobTime.Format(_T("%02d.%02d.%04d - %02d:%02d:%02d"),m_JobTime.GetDay(),
													  m_JobTime.GetMonth(),
													  m_JobTime.GetYear(),
													  m_JobTime.GetHour(),
													  m_JobTime.GetMinute(),
													  m_JobTime.GetSecond());
	return 	sJobTime;
}
