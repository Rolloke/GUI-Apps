#include "stdafx.h"
#include "DVProcess.h"
#include "gopsendthread.h"
#include "IPCOutputDVClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
CGOPItem::CGOPItem(CSecID camID, CCamera* pCamera) 
{
	m_camID = camID;
	m_pCamera = pCamera;
	m_dwIndex = 0;
	m_dwLastSend = WK_GetTickCount();
	CSystemTime s;
	s.GetLocalTime();
	m_liLastSend = (LARGE_INTEGER)s;
}
//////////////////////////////////////////////////////////////////////
void CGOPItem::AddPictureRecord(const CIPCPictureRecord& pict,DWORD dwMDX, DWORD dwMDY)
{
	Lock();
	if (GetSize() == 0)
	{
		m_dwLastSend = WK_GetTickCount()-40;
		m_liLastSend = pict.GetTimeStamp();
	}
	Add(new CIPCPictureRecord(pict));
	m_dwMDXs.Add(dwMDX);
	m_dwMDYs.Add(dwMDY);
	Unlock();
}
//////////////////////////////////////////////////////////////////////
CIPCPictureRecord* CGOPItem::GetNextPicture(DWORD dwTick, DWORD& dwX,DWORD& dwY)
{
	// wieviel Zeit ist vergangen
	CIPCPictureRecord* pReturn = NULL;

	Lock();
	if (GetSize())
	{
		CIPCPictureRecord* pPict = GetAtFast(0);
		DWORD dwElapsed = WK_GetTimeSpan(m_dwLastSend,dwTick);
		LARGE_INTEGER t = pPict->GetTimeStamp(m_dwIndex); 
		LARGE_INTEGER stDiff;
		stDiff.QuadPart = t.QuadPart-m_liLastSend.QuadPart;
		// diff in 100 Nanosekunden
		DWORD dwDiffMS = (DWORD)((__int64)stDiff.QuadPart / (__int64)10000);
		int iDiff = 0;
		BOOL bSend = FALSE;

		if (dwDiffMS>dwElapsed)
		{
			iDiff = dwDiffMS-dwElapsed;
			bSend = iDiff<10;
		}
		else
		{
			bSend = TRUE;
		}
		// mit 10 ms genauigkeit senden
//		TRACE(_T("time diff %d %08lx\n"),iDiff,pPict->GetCamID());
		if (bSend)
		{
			CIPCSavePictureType type = (m_dwIndex==0)?SPT_FULL_PICTURE:SPT_DIFF_PICTURE;
			pReturn = new CIPCPictureRecord(pPict->GetCIPC(),
											pPict->GetData(m_dwIndex),
											pPict->GetDataLength(m_dwIndex),
											pPict->GetCamID(),
											pPict->GetResolution(),
											pPict->GetCompression(),
											pPict->GetCompressionType(),
											pPict->GetTimeStamp(),
											pPict->GetJobTime(),
											NULL,0,type);
			dwX = m_dwMDXs[0];
			dwY = m_dwMDYs[0];
			m_dwIndex++;
			if (m_dwIndex == pPict->GetNumberOfPictures())
			{
				RemoveAt(0);
				WK_DELETE(pPict);
				m_dwIndex = 0;
				m_dwMDXs.RemoveAt(0);
				m_dwMDYs.RemoveAt(0);
			}
			m_liLastSend.QuadPart = t.QuadPart;
			m_dwLastSend = dwTick;
		}
	}

	Unlock();

	return pReturn;
}
//////////////////////////////////////////////////////////////////////
CGOPSendThread::CGOPSendThread() : CWK_Thread(_T("CGOPSendThread"))
{
	m_pParent = NULL;
}
//////////////////////////////////////////////////////////////////////
CGOPSendThread::~CGOPSendThread()
{
	m_GOPs.SafeDeleteAll();
}
//////////////////////////////////////////////////////////////////////
void CGOPSendThread::SetParent(CIPCOutputDVClient* pParent)
{
	m_pParent = pParent;
}
//////////////////////////////////////////////////////////////////////
void CGOPSendThread::AddPictureRecord(CCamera* pCamera,const CIPCPictureRecord& pict, DWORD dwMDX, DWORD dwMDY)
{
	BOOL bFound = FALSE;
	CGOPItem* pGOPItem = NULL;

	m_GOPs.Lock();
	for (int i=0;i<m_GOPs.GetSize();i++)
	{
		pGOPItem = m_GOPs.GetAtFast(i);
		if (pGOPItem->GetCamID() == pict.GetCamID())
		{
			bFound = TRUE;
			break;
		}
	}
	if (!bFound)
	{
		pGOPItem = new CGOPItem(pict.GetCamID(),pCamera);
		m_GOPs.Add(pGOPItem);
	}
	pGOPItem->AddPictureRecord(pict,dwMDX,dwMDY);
	m_GOPs.Unlock();
	Sleep(0);
}
//////////////////////////////////////////////////////////////////////
BOOL CGOPSendThread::Run(LPVOID lpContext)
{
//	TRACE(_T("CGOPSendThread::Run\n"));
	if (m_pParent)
	{
		DWORD dwTick = WK_GetTickCount();
		CGOPItem* pGOPItem = NULL;
		CIPCPictureRecord* pPict = NULL;

		m_GOPs.Lock();
		for (int i=0;i<m_GOPs.GetSize();i++)
		{
			pGOPItem = m_GOPs.GetAtFast(i);
			if (pGOPItem)
			{
				DWORD x,y;
				x = 0;
				y = 0;
				pPict = pGOPItem->GetNextPicture(dwTick,x,y);
				if (pPict)
				{
					m_pParent->SendPicture(pGOPItem->GetCamera(),*pPict,x,y,FALSE);
					WK_DELETE(pPict);
				}
			}
		}
		m_GOPs.Unlock();

	}
	return TRUE;
}
