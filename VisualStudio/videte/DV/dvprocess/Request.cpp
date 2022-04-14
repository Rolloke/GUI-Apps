// Request.cpp: implementation of the CRequest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVProcess.h"
#include "Request.h"

#include "CameraGroup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

WORD  CRequest::m_staticID = 0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRequest::CRequest()
{
	m_ID = 0;
	m_iNextRequestIndex = 0;
	m_bIsSingleMD = FALSE;
}
//////////////////////////////////////////////////////////////////////
CRequest::~CRequest()
{

}
//////////////////////////////////////////////////////////////////////
void CRequest::CalcRequests(CCameraGroup* pCameraGroup, WORD wFirst)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CCamera* pCamera = NULL;

	// alle aktiven MD Kameras
	for (int i=0; i<pCameraGroup->GetSize();i++)
	{
		pCamera = (CCamera*)pCameraGroup->GetAtFast(i);
		if (   pCamera->IsActive()
			&& pCamera->IsMD())
		{
			m_CamSubIDs.Add(pCamera->GetSubID());
		}
	}
	m_bIsSingleMD = m_CamSubIDs.GetSize() == 1;
	int iNum = 1;
	if (pCameraGroup->IsJacob())
	{
		iNum = 2;
	}
	if (pCameraGroup->GetNrOfActiveCameras()>iNum)
	{
		// alle aktiven MD Kameras mit aktuellem Alarm
		for (i=0; i<pCameraGroup->GetSize();i++)
		{
			pCamera = (CCamera*)pCameraGroup->GetAtFast(i);
			if (   pCamera->IsActive()	
				&& pCamera->IsInputActive()
				&& pCamera->IsTimerActive()
				&& pCamera->IsMD())
			{
				AddCameraBalanced(pCamera->GetSubID());
			}
		}
	}

	// jetzt die UVV Kameras einsortieren
	DWORD dwTick = WK_GetTickCount();

	// durchschnittliche Schaltzeit berechnen und lieber
	// auf volle 10er aufrunden.
	DWORD dwAverageSwitchTime = pCameraGroup->GetAverageSwitchtime();
	dwAverageSwitchTime = dwAverageSwitchTime / 10;
	dwAverageSwitchTime *= 10;
	dwAverageSwitchTime += 10;

	for (i=0; i<pCameraGroup->GetSize();i++)
	{
		pCamera = (CCamera*)pCameraGroup->GetAtFast(i);
		if (   pCamera->IsActive()
			&& pCamera->IsUVV())
		{
			m_bIsSingleMD = FALSE;
			DWORD dwTickConfirm = pCamera->GetTickConfirm();
			if (dwTickConfirm == 0)
			{
				TRACE(_T("initial UVV request %s\n"),pCamera->GetName());
				AddCameraSorted(pCamera->GetSubID());
			}
			else
			{
				// ist gerade noch ein request in der pipeline
				// Zeitintervall Alarm=500 Vorring=1000
				int iIntervall = pCamera->GetIntervall();
				// wieviel MD Kameras haben wir bereits, und wieviel Zeit werden diese benötigen
				// die +1 ist fuer das noch offene Request
//				int iMDTime = dwAverageSwitchTime*m_CamSubIDs.GetSize();
				// Zeit die vergangen ist, seit von dieser Kamera ein Bild gemacht wurde
				int iDifference = WK_GetTimeSpan(dwTickConfirm,dwTick);
				if (wFirst != (WORD)(-1))
				{
					iDifference += dwAverageSwitchTime;
				}
				// Zeitkorrektur zum Sekundenraster
				// Zeitdifferenz korrigieren
				iDifference += pCamera->GetTickConfirmDifference();
//				TRACE(_T("D=%04d,I=%d,RT=%d,AV=%d\n"),iDifference,iIntervall,iMDTime,dwAverageSwitchTime);

				int iRequestTime = 0;
				if (wFirst == pCamera->GetSubID())
				{
					iRequestTime = iRequestTime + iIntervall - (int)dwAverageSwitchTime;
				}
				else
				{
					if (iIntervall > iDifference)
					{
						iRequestTime = iIntervall - iDifference;
					}
				}
				while (iRequestTime<=(m_CamSubIDs.GetSize()*(int)dwAverageSwitchTime))
				{
					int pos = iRequestTime/(int)dwAverageSwitchTime;
/*
					int res = iRequestTime%(int)dwAverageSwitchTime;
					pos += (2*res>(int)dwAverageSwitchTime) ? 1 : 0;
*/
					if (pos<0)
					{
						pos = 0;
					}
					if (pos<=m_CamSubIDs.GetSize())
					{
//							TRACE(_T("POS = %d\n"),pos);
						m_CamSubIDs.InsertAt(pos,pCamera->GetSubID());
					}
					else
					{
						TRACE(_T("ADD ?\n"));
						break;
					}
					iRequestTime += iIntervall;
				}
			}
		}
	}
	m_ID = ++m_staticID;
}
/////////////////////////////////////////////
void CRequest::AddCameraBalanced(WORD w)
{
	WORD s = (WORD)m_CamSubIDs.GetSize();
	int index = 0;
	WORD*pCamSubIDs = m_CamSubIDs.GetData();
	for (int j=0;j<m_CamSubIDs.GetSize();j++)
	{
//		if (m_CamSubIDs.GetAt(j) == w)
		if (pCamSubIDs[j] == w)
		{
			index = j;
			break;
		}
	}
	int insert = (index+(s/2)+1)%s;
	// niemals an erste stelle einfuegen
	if (insert != 0)
	{
		m_CamSubIDs.InsertAt(insert,w);
	}
	else
	{
		m_CamSubIDs.Add(w);
	}
}
/////////////////////////////////////////////
void CRequest::AddCameraSorted(WORD w)
{
	int index = 0;
	WORD*pCamSubIDs = m_CamSubIDs.GetData();
	for (int j=0;j<m_CamSubIDs.GetSize();j++)
	{
//		if (m_CamSubIDs.GetAt(j) < w)
		if (pCamSubIDs[j] < w)
		{
			index = j;
			break;
		}
	}
	int insert = index+1;
	// niemals an erste stelle einfuegen
	if (   (insert != 0)
		&& (m_CamSubIDs.GetSize()>0)
		)
	{
		m_CamSubIDs.InsertAt(insert,w);
	}
	else
	{
		m_CamSubIDs.Add(w);
	}
}
/////////////////////////////////////////////
CString CRequest::ToString()
{
	CString s(_T("<"));
	CString v;
	WORD*pCamSubIDs = m_CamSubIDs.GetData();
	for (int i=0;i<m_CamSubIDs.GetSize();i++)
	{
//		v.Format(_T("%d"),m_CamSubIDs.GetAt(i));
		v.Format(_T("%d"), pCamSubIDs[i]);
		s += v;
		if (i+1<m_CamSubIDs.GetSize())
		{
			s += _T(",");
		}
	}

	s += _T(">");
	return s;
}
/////////////////////////////////////////////
int CRequest::GetCameraIndex(WORD w)
{
	int index = -1;
	WORD*pCamSubIDs = m_CamSubIDs.GetData();
	for (int j=0;j<m_CamSubIDs.GetSize();j++)
	{
//		if (m_CamSubIDs.GetAt(j) == w)
		if (pCamSubIDs[j] == w)
		{
			index = j;
			break;
		}
	}
	return index;
}
