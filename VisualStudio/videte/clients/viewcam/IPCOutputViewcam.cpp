// IPCOutputViewcam.cpp: implementation of the CIPCOutputViewcam class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "viewcam.h"
#include "IPCOutputViewcam.h"
#include "viewcamDlg.h"
#include "IPCamera.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCOutputViewcam::CIPCOutputViewcam(const char* shmName)
:CIPCOutputClient(shmName,FALSE)
{
	m_pViewcamDlg = (CViewcamDlg*)AfxGetMainWnd();
	m_MainViewcam=0;

 	StartThread();


}

CIPCOutputViewcam::~CIPCOutputViewcam()
{
 	StopThread();

}

void CIPCOutputViewcam::OnAddRecord(const CIPCOutputRecord &pRec)
{
	int index;
	if(m_MainViewcam==1){
	TRACE("Add Cam: %s\n", pRec.GetName());
	CSecID id = pRec.GetID();
    index = m_pViewcamDlg->m_lbCamera.AddString(pRec.GetName());
	m_pViewcamDlg->m_lbCamera.SetItemData(index,id.GetID());
	}
}

void CIPCOutputViewcam::OnUpdateRecord(const CIPCOutputRecord &pRec)
{
	if(m_MainViewcam==1){	
	// Es werden nur die Kameras upgedatet, die noch enabled sind
		if(!pRec.IsEnabled())
		{
			int a = m_pViewcamDlg->m_lbCamera.FindStringExact(-1,pRec.GetName());
			if(a != LB_ERR)
			{
				m_pViewcamDlg->m_lbCamera.DeleteString(a);
			}
			
		}
	}
	}
void CIPCOutputViewcam::OnDeleteRecord(const CIPCOutputRecord &pRec)
{
	if(m_MainViewcam==1){
		// Es werden nur die Kameras upgedatet, die noch enabled sind
		if(!pRec.IsEnabled())
		{
			int a = m_pViewcamDlg->m_lbCamera.FindStringExact(-1,pRec.GetName());
			if(a != LB_ERR)
			{
				m_pViewcamDlg->m_lbCamera.DeleteString(a);
			}
			
		}		
	}

}

void CIPCOutputViewcam::OnConfirmInfoOutputs(WORD wGroupID, 
											 int iNumGroups, 
											 int numRecords, 
											 const CIPCOutputRecord records[])
{
	CIPCOutputClient::OnConfirmInfoOutputs(wGroupID, iNumGroups, numRecords, records);

	// hier habe ich alle kameras und relais, im CIPC Thread(!)
}

static int g_PictureCount=0,g_CameraNr=1;

void CIPCOutputViewcam::OnConfirmJpegEncoding(const CIPCPictureRecord &pict,DWORD dwUserData)

{
 if(g_CameraNr <= m_pViewcamDlg->iSelectCamera)
 {
	CString sFileName;
	CString sNamePic = ".jpg";
	TRACE("picture from cam id %08lx\n",pict.GetCamID().GetID());
	m_pViewcamDlg->m_prPictures.StepIt();

	// Speichern der Einzelbilder
	sFileName.Format("%08lx-%d.jpg",pict.GetCamID().GetID(),g_PictureCount+1);
	CFile fpicture(sFileName,CFile::modeCreate|CFile::modeWrite);

	fpicture.Write(pict.GetData(),pict.GetDataLength());
	fpicture.Close();

	g_PictureCount++;									//Anzahl der zu empfangenen Bilder erreicht(g_i)
	if (g_PictureCount==m_pViewcamDlg->m_PictureCount) 			
	{

														//Stop Bildersendung für akt.Kamera
		DoRequestNewJpegEncoding(pict.GetCamID().GetID(),
								RESOLUTION_NONE,COMPRESSION_NONE, 0, 0);
		
		g_PictureCount=0;
		g_CameraNr++;
		LPINT *lp_HelpPointer = (*m_pViewcamDlg).ip_CameraIndex;
		(*lp_HelpPointer)++;
	
	
										//Nächste abzufragende KameraID ermitteln
		DWORD dwCam = m_pViewcamDlg->m_lbCamera.
										GetItemData(**lp_HelpPointer);
		CSecID camID = dwCam;

	
										//Rekursiver Aufruf von OnConfirm.. über DoRequest..
	
		
		if(g_CameraNr <= m_pViewcamDlg->iSelectCamera) 
		{
			DoRequestNewJpegEncoding(	camID,	
										RESOLUTION_HIGH, 
										COMPRESSION_7,
										1,
										1000);
		}
		else //Ende aller Aufzeichnungen
		{
			g_PictureCount=0,g_CameraNr=1;
			m_pViewcamDlg->m_prPictures.SetPos(0);
		}
		
	}
 }	

}

