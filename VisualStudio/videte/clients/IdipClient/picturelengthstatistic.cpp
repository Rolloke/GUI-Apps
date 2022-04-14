// PictureLengthStatistic.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "PictureLengthStatistic.h"

#include "Server.h"
#include "CIPCOutputIdipClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static _TCHAR szBitrate[] = _T("Bitrate");
/////////////////////////////////////////////////////////////////////////////
// CPictureLengthStatistic
#define CAM_DIFF		5000 // every m_dwSyncTick milliseconds check synchronizing start with CAM_DIFF
#define PANIC_SYNC_DIFF	20000  // time span max to reduce bitrate in ms
#define MAX_SYNC_DIFF	1500  // time span max to reduce bitrate in ms
#define MIN_SYNC_DIFF	300  // time span max to increase bitrate in ms

#define MAX_TUNING		1000
#define MIN_TUNING		400
/////////////////////////////////////////////////////////////////////////////
CPictureLengthStatistic::CPictureLengthStatistic(CIPCOutputIdipClient* pOutput)
{
	m_pOutput = pOutput;
	m_bDelayed = FALSE;
	m_Tuning = theApp.m_dwInitialTuning;
	m_Delay  = 0;

	m_dwCurBitTick = CAM_DIFF;
	m_dwMedBitTick = 12 * CAM_DIFF;
	m_dwAllBitTick = 24 * CAM_DIFF;

	m_AllTime = 0;
	m_MedTime = 0;
	m_CurTime = 0;

	m_CurBitrate = 0;
	m_MedBitrate = 0;
	m_AllBitrate = 0;

	m_CurDataSum = 0;
	m_MedDataSum = 0;
	m_AllDataSum = 0;

	m_LastUpdated = 0;

	m_dwPics = 0;
}
//////////////////////////////////////////////////////////////////////////////////////
void CPictureLengthStatistic::Reset()
{
	m_Tuning = theApp.m_dwInitialTuning;
	m_Delay  = 0;

	m_bDelayed = FALSE;

	m_AllTime = GetTickCount();
	m_CurTime = m_AllTime;
	m_MedTime = m_AllTime;

	m_CurDataSum = 0;
	m_MedDataSum = 0;
	m_AllDataSum = 0;

	m_LastUpdated = 0;
	m_dwPics = 0;

	if (theApp.m_bTraceBitrate)
	{
		WK_STAT_LOG(szBitrate,0,_T("ResetBitrates"));
		WK_STAT_LOG(szBitrate,1,_T("ResetBitrates"));
		WK_STAT_LOG(szBitrate,0,_T("ResetBitrates"));
	}
}
/////////////////////////////////////////////////////////////////////////////
CPictureLengthStatistic::~CPictureLengthStatistic()
{
}
/////////////////////////////////////////////////////////////////////////////
void CPictureLengthStatistic::operator +=(int iPictureLength)
{
	AddPictureLength(iPictureLength);
}
/////////////////////////////////////////////////////////////////////////////
void CPictureLengthStatistic::AddPictureLength(int iPictureLength)
{
	// calculate bitrates for ruling delay with encoder bitrate
	if (m_AllTime!=0)
	{
		DWORD curTC = GetTickCount();
		m_dwPics++;

		if (GetTimeSpanSigned(m_LastUpdated, curTC) > PANIC_SYNC_DIFF)
		{
			Reset();
		}

		m_MedDataSum += iPictureLength;
		m_AllDataSum += iPictureLength;
		m_CurDataSum += iPictureLength;

		int nSpan = GetTimeSpanSigned(m_MedTime, curTC);
		if (nSpan > (int)m_dwMedBitTick)
		{
			m_MedBitrate = m_MedDataSum * 8000.0 / nSpan;
			if (theApp.m_bTraceBitrate)
			{
				WK_STAT_LOG(szBitrate,(int)m_MedBitrate,_T("60sBitrate"));
			}
			m_MedTime = curTC;
			m_MedDataSum = 0;
		}

		nSpan = GetTimeSpanSigned(m_CurTime, curTC);
		if (nSpan > (int)m_dwCurBitTick)
		{
			m_pOutput->DoRequestSync(curTC, 0, 0);
			TRACE(_T("PILS DoRequestBitrate\n"));
			m_pOutput->DoRequestBitrate();
			m_CurBitrate = m_CurDataSum * 8000.0 / nSpan;
			if (theApp.m_bTraceBitrate)
			{
				WK_STAT_LOG(szBitrate, (int)m_CurBitrate, _T("5sBitrate"));
				WK_STAT_LOG(szBitrate, MulDiv(m_dwPics, 1000, nSpan), _T("FPS"));
			}
			m_CurDataSum = 0;
			m_dwPics = 0;
			m_CurTime = curTC;
		}
		m_LastUpdated = curTC;
	}
	else
	{
		m_AllTime = GetTickCount();
		m_CurTime = m_AllTime;
		m_MedTime = m_AllTime;
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CPictureLengthStatistic::CalculateAverageBitrate()
{
	double sum = 0;
	int	n=0;
	DWORD curTC = GetTickCount();

	if (m_AllDataSum>0)
	{
		m_AllBitrate = (m_AllDataSum*8000) / GetTimeSpanSigned(m_AllTime, curTC);
		sum += m_AllBitrate;
		n++;
	}

	if (m_MedBitrate>0)
	{
		sum += m_MedBitrate;
		n++;
	}

	if (m_CurBitrate>0)
	{
		sum += m_CurBitrate;
		n++;
	}

	if (m_pOutput->GetBitrate()>0)
	{
		sum += m_pOutput->GetBitrate();
		n++;
	}

	if (n>0)
	{
		m_AveBitrate = sum / n;
	}
	else
	{
		m_AveBitrate = (m_pOutput->GetBitrate()* 9) / 10;
	}
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CPictureLengthStatistic::OnConfirmSync(DWORD dwTickConfirm, DWORD dwTickSend)
{
	DWORD curTC = GetTickCount();
	m_Delay  = curTC - dwTickSend;

	if ( ((m_Delay>MAX_SYNC_DIFF) || (m_Delay<MIN_SYNC_DIFF)) && (theApp.m_bCorrectBitrate))
	{
		if (theApp.m_bTraceDelay)
		{
			WK_STAT_LOG(szBitrate,m_Delay,_T("Delay"));
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CPictureLengthStatistic::CalcTuning()
{
	DWORD tuning;
	DWORD diff = 0;
	BOOL bTuned = FALSE;

	CalculateAverageBitrate();
	if(theApp.m_bTraceBitrate)
	{
		WK_STAT_LOG(szBitrate,(int)m_AllBitrate,_T("AllBitrate"));
		WK_STAT_LOG(szBitrate,(int)m_AveBitrate,_T("AverageBitrate"));
	}

	tuning = (DWORD)((m_AveBitrate * 1110) / m_pOutput->GetBitrate());

	if (m_Delay>0)
	{
		if (m_Delay>MAX_SYNC_DIFF)
		{
			tuning = (tuning * MAX_SYNC_DIFF*9) / (m_Delay*10);
			m_bDelayed = TRUE;
		}
		else if (m_Delay<MIN_SYNC_DIFF)
		{
			if (m_bDelayed)
			{
				tuning = (tuning * MIN_SYNC_DIFF*10) / (m_Delay*9);
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			m_bDelayed = FALSE;
			return FALSE;
		}
	}

	if (tuning>MAX_TUNING)
	{
		tuning = MAX_TUNING;
	}
	if (tuning<MIN_TUNING)
	{
		tuning = MIN_TUNING;
	}

	if (tuning>m_Tuning)
	{
		diff = tuning-m_Tuning;
	}
	else
	{
		diff = m_Tuning - tuning;
	}
	if (diff>10)
	{
		m_Tuning = tuning;
		bTuned = TRUE;
		m_dwCurBitTick = CAM_DIFF; // start again with time to regulate
	}
	else
	{
		if (m_dwCurBitTick < 2*CAM_DIFF)
		{
			m_dwCurBitTick += 100;
		}
	}

	return bTuned;
}
