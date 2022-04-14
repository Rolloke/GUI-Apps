// MDProcess.cpp: implementation of the CMDProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sec3.h"
#include "CipcDatabaseClientServer.h"
#include "input.h"
#include "outputlist.h"
#include "MDProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMDProcess::CMDProcess(CProcessScheduler* pScheduler,CActivation *pActivation, CompressionType ct)
 : CRecordingProcess(pScheduler,pActivation,ct,-1)
{
	m_dwNrOfIFrames = 0;
#ifdef TRACE_LOCKS
	m_Pictures.m_nCurrentThread = theApp.m_bTraceLocks ? 0 : -1;
#endif

}
//////////////////////////////////////////////////////////////////////
CMDProcess::~CMDProcess()
{
	m_Pictures.SafeDeleteAll();
}
//////////////////////////////////////////////////////////////////////
void CMDProcess::PictureData(const CIPCPictureRecord& pict,DWORD dwMDX, DWORD dwMDY,DWORD dwUserID)
{
	/*
	TRACE(_T("MD OK picture %s, %s,%04d %s %d\n"),
		GetActivation()->GetInput()->GetName(),
		pict.GetTimeStamp().GetDateTime(),
		pict.GetTimeStamp().GetMilliseconds(),
		SPT2TCHAR(pict.GetPictureType()),
		m_Pictures.GetSize()
		);
*/
	m_Pictures.Lock(_T(__FUNCTION__));

	if (   pict.GetPictureType() == SPT_FULL_PICTURE
		|| pict.GetPictureType() == SPT_GOP_PICTURE
		|| m_Pictures.GetSize() > 0)
	{
		CIPCPictureRecord* pPict = new CIPCPictureRecord(pict);
		// ist hier unnötig, da die Bubble sowieso noch länger für andere Prozesse benötigt wird.
		//		pPict->ReleaseBubble();
		m_Pictures.Add(pPict);
		if (   pict.GetPictureType() == SPT_FULL_PICTURE
			|| pict.GetPictureType() == SPT_GOP_PICTURE)
		{
			m_dwNrOfIFrames++;
			if ((DWORD)m_Pictures.GetSize()>GetMacro().GetPicsPerSlice())
			{
				// the first one should be I-Frame
				pPict = m_Pictures.GetAtFast(0);
				if (   pPict->GetPictureType() == SPT_FULL_PICTURE
					|| pPict->GetPictureType() == SPT_GOP_PICTURE)
				{
					m_dwNrOfIFrames--;
				}
				else
				{
					TRACE("first image in pre alarm not I-Frame\n");
				}
				m_Pictures.RemoveAt(0);
				WK_DELETE(pPict);
				while (   m_Pictures.GetSize()
					&& (pPict = m_Pictures.GetAtFast(0))->GetPictureType() == SPT_DIFF_PICTURE)
				{
					m_Pictures.RemoveAt(0);
					WK_DELETE(pPict);
				}
			}
		}
	}
	else
	{
		// P-Frame and Buffer empty
	}
	m_Pictures.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CMDProcess::OnStateChanging(SecProcessState newState,BOOL bShutDown)
{
	if (   newState==SPS_TERMINATED
		&& !bShutDown)
	{
		FlushMDPictures();
	}
}
//////////////////////////////////////////////////////////////////////
void CMDProcess::FlushMDPictures()
{
	// connected to database ?
	WORD wArchivNr = GetActivation()->GetArchiveID().GetSubID();
	CIPCFields fields;
	AddDefaultFields(fields);

	m_Pictures.Lock(_T(__FUNCTION__));
	for (int i=0; i<m_Pictures.GetSize();i++)
	{
		CIPCPictureRecord* pPict = m_Pictures.GetAtFast(i);
		theApp.SavePicture(GetMacro().GetCompression(),*pPict,fields,wArchivNr);
//		const CIPCPictureRecord& pict = *m_Pictures[i];
//		theApp.SavePicture(GetMacro().GetCompression(),pict,fields,wArchivNr);
	}
	m_Pictures.DeleteAll();
	m_dwNrOfIFrames = 0;
	m_Pictures.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CMDProcess::OnUnitDisconnected()
{
	m_Pictures.Lock(_T(__FUNCTION__));
	Lock(_T(__FUNCTION__));
	m_Pictures.DeleteAll();
	Unlock();
	m_Pictures.Unlock();
}
