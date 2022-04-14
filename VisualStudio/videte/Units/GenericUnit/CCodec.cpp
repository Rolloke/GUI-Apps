/////////////////////////////////////////////////////////////////////////////
// PROJECT:		GenericDll
// FILE:		$Workfile: CCodec.cpp $
// ARCHIVE:		$Archive: /Project/Units/GenericUnit/CCodec.cpp $
// CHECKIN:		$Date: 20.01.06 12:40 $
// VERSION:		$Revision: 26 $
// LAST CHANGE:	$Modtime: 20.01.06 12:33 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CCodec.h"
#include "resource.h"

#include "GenericUnitDlg.h"
#include "CMotionDetection.h"
#include "JaCobProfile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
UINT AlarmPollThread(LPVOID pData)
{
	CCodec*	pCodec	 = (CCodec*)pData;  // Pointer auf's Codec-Objekt

	if (!pCodec)
	{
		WK_TRACE_ERROR("AlarmPollThread\tpCodec = NULL\n");
		return 0;
	}
	// Poll bis Mainprozeß terminiert werden soll.
	while (pCodec->m_bRun)
	{
		Sleep(250);
		pCodec->PollAlarm();
	}

	WK_TRACE("Exit AlarmPollThread\n");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
UINT EncoderDateThread(LPVOID pData)
{
	CCodec*	pCodec	 = (CCodec*)pData;  // Pointer auf's Codec-Objekt

	if (!pCodec)
	{
		WK_TRACE_ERROR("PollEncoderDateThread\tpCodec = NULL\n");
		return 0;
	}

	// Poll bis Mainprozeß terminiert werden soll.
	while (pCodec->m_bRun)
	{
		pCodec->ReadData();
	}

	WK_TRACE("Exit PollEncoderDateThread\n");
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
CCodec::CCodec(CGenericUnitDlg *pMainWnd)
{
	CWK_Profile		prof;

	m_wSource				= 0;
	m_wFormat				= 0;
	m_dwBPF					= 0;
	m_dwProzessID			= 0;
	m_dwReqPics				= 0;
	m_wAlarmState			= 0;
	m_pMainWnd				= pMainWnd;
	m_pAlarmPollThread		= NULL;
	m_pEncoderDataThread	= NULL;
	m_pMotionDetection		= NULL;
	m_bRun					= FALSE;
	m_nEncoderState			= ENCODER_OFF;
	m_wSourceType			= SRCTYPE_FBAS;
	m_wVideoFormat			= VFMT_PAL_CCIR;
	m_nStoreField			= EVEN_FIELD;

	// Für jede Kamera ein Jpegfile einlesen
	CString sFileName;
	for (int nI = VIDEO_SOURCE0; nI < MAX_CAMERAS; nI++)
	{
		m_nFrameCounter[nI] = 0;
		m_dwLastGrab[nI]	= GetTickCount();
		m_wBrightness[nI]	= (MAX_BRIGHTNESS - MIN_BRIGHTNESS) / 2;
		m_wContrast[nI]		= (MAX_CONTRAST - MIN_CONTRAST) / 2;
		m_wSaturation[nI]	= (MAX_SATURATION - MIN_SATURATION) / 2;
		m_wHue[nI]			= (MAX_HUE - MIN_HUE) / 2;
	}

	if (m_pMainWnd)
	{
		// Gibt an ob das Odd, Even, oder beide Halbbilder verwendet werden sollen.
		m_nStoreField = prof.GetInt(m_pMainWnd->GetIniPath()+"\\General", "StoreField", m_nStoreField);
		prof.WriteInt(m_pMainWnd->GetIniPath()+"\\General", "StoreField", m_nStoreField);
	}

	if (ReadConfigurationInt("MotionDetection", "Activate", 1, m_pMainWnd->GetIniPath()) == 1)
	{
		// MotionDetection-Klasse anlegen
		if (!m_pMotionDetection)
		{
			m_pMotionDetection	= new CMotionDetection(m_pMainWnd->GetIniPath());
			if (!m_pMotionDetection)
				return;
		}

		if (m_pMotionDetection)
			m_pMotionDetection->Init(this);
	}
	m_bRun = TRUE;

	// EncoderData Thread anlegen.
	m_pEncoderDataThread = AfxBeginThread(EncoderDateThread, this);
	if (m_pEncoderDataThread)
	{
		m_pEncoderDataThread->m_bAutoDelete = FALSE;
		m_pEncoderDataThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
	}
	// Alarm-Poll Thread anlegen.
	m_pAlarmPollThread = AfxBeginThread(AlarmPollThread, this);
	if (m_pAlarmPollThread)
	{
		m_pAlarmPollThread->m_bAutoDelete = FALSE;
		m_pAlarmPollThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
	}
}

/////////////////////////////////////////////////////////////////////////////
CCodec::~CCodec()
{
	m_bRun = FALSE;

    // Warte bis 'EcoderDataThread' beendet ist.
	if (m_pEncoderDataThread)
		WaitForSingleObject(m_pEncoderDataThread->m_hThread, 2000);
	WK_DELETE(m_pEncoderDataThread); //Autodelete = FALSE;

    // Warte bis 'AlarmPollThread' beendet ist.
	if (m_pAlarmPollThread)
		WaitForSingleObject(m_pAlarmPollThread->m_hThread, 2000);
	WK_DELETE(m_pAlarmPollThread); //Autodelete = FALSE;

	WK_TRACE("CCodec::~CCodec()\n");
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::SetInputSource(SOURCE_SELECT_STRUCT &CurrSrcSel, SOURCE_SELECT_STRUCT &NextSrcSel)
{
	WORD wOldSource = GetInputSource();

	if (CurrSrcSel.wSource == VIDEO_SOURCE_CURRENT)
		CurrSrcSel.wSource = m_wSource;

#if (1)
	// Die Umschaltung soll 60 bzw 80ms dauern
	Sleep((m_nStoreField == BOTH_FIELDS) ? 60 : 80);
#else
	Sleep(0);
#endif
	m_csCodec.Lock();
	m_wSource		= CurrSrcSel.wSource;
	m_dwProzessID	= CurrSrcSel.dwProzessID;
	m_dwReqPics		= CurrSrcSel.dwPics;
	
	MotionDetection();

	m_csCodec.Unlock();

	return wOldSource;
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::GetInputSource()
{
	return m_wSource;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SetSourceType(WORD wSourceType)
{
	m_wSourceType = wSourceType;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::GetSourceType() const
{
	return m_wSourceType;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SetVideoFormat(WORD wVideoFormat)
{
	m_wVideoFormat = wVideoFormat;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::GetVideoFormat() const
{
	return m_wVideoFormat;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SetBrightness(WORD wSource, WORD wBrightness)
{
	m_wBrightness[wSource] = wBrightness;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::GetBrightness(WORD wSource)
{
	return m_wBrightness[wSource];
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SetContrast(WORD wSource, WORD wContrast)
{
	m_wContrast[wSource] = wContrast;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::GetContrast(WORD wSource)
{
	return m_wContrast[wSource];
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SetSaturation(WORD wSource, WORD wSaturation)
{
	m_wSaturation[wSource] = wSaturation;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::GetSaturation(WORD wSource)
{
	return 	m_wSaturation[wSource];
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::SetHue(WORD wSource, WORD wHue)
{
	m_wHue[wSource] = wHue;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::GetHue(WORD wSource)
{
	return m_wHue[wSource];
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::GetCurrentAlarmState()
{	
	WORD wResult = 0;

	// Simulation zufälliger Alarme
	int rnd1 = rand();
	// Mit einer Wahrscheinlichkeit von 30%
	if (rnd1 <  (0.30 * (float)RAND_MAX))
	{
		int rnd2 = rand() * 16 / RAND_MAX;
		wResult = SETBIT(wResult, rnd2);
	}
	return wResult;
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::SetAlarmEdge(WORD wEdge)
{
	WORD wOldEdge = 0;

	wOldEdge = GetAlarmEdge();

	return wOldEdge;
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::GetAlarmEdge()
{
	WORD wRet = 0;

	return wRet;
}

/////////////////////////////////////////////////////////////////////////////
BYTE CCodec::GetRelais()
{
	BYTE byRet = 0;

	return byRet;
}

/////////////////////////////////////////////////////////////////////////////
BYTE CCodec::SetRelais(BYTE byRelais)
{
	BYTE byOldRelais = 0; 

	byOldRelais = GetRelais(); 

	return byOldRelais;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::EncoderStart()
{
	m_nEncoderState = ENCODER_ON;
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::EncoderStop()
{
	m_nEncoderState = ENCODER_OFF;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::EncoderSetFormat(WORD wFormat)
{
	WORD wOldFormat = EncoderGetFormat();

	m_wFormat = wFormat;

	return wOldFormat;
}

/////////////////////////////////////////////////////////////////////////////
WORD CCodec::EncoderGetFormat() const
{
	return 	m_wFormat;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCodec::EncoderSetBPF(DWORD dwBPF)
{
	DWORD dwOldBPF = EncoderGetBPF();

	m_dwBPF = dwBPF;

	return dwOldBPF;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CCodec::EncoderGetBPF() const
{
	return m_dwBPF;
}

/////////////////////////////////////////////////////////////////////////////
int CCodec::GetEncoderState() const
{
	return m_nEncoderState;
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::ReadData()
{
	JPEG *pJpeg	= NULL;
	static dwTC = 0;

	m_csCodec.Lock();

	if (GetEncoderState() == ENCODER_ON)
	{
		if (m_dwReqPics > 0)
		{
			pJpeg = ReadJpegFile(FALSE);
			//pJpeg = &m_Jpeg[m_wSource];
			if (pJpeg)
			{
				CSystemTime TimeStamp;
				TimeStamp.GetLocalTime();
				pJpeg->TimeStamp.bValid		= TRUE;
				pJpeg->TimeStamp.wYear		= TimeStamp.wYear;	
				pJpeg->TimeStamp.wMonth		= TimeStamp.wMonth;
				pJpeg->TimeStamp.wDay		= TimeStamp.wDay;
				pJpeg->TimeStamp.wHour		= TimeStamp.wHour;
				pJpeg->TimeStamp.wMinute	= TimeStamp.wMinute;
				pJpeg->TimeStamp.wSecond	= TimeStamp.wSecond;
				pJpeg->TimeStamp.wMSecond	= TimeStamp.wMilliseconds;
				pJpeg->dwProzessID			= m_dwProzessID;
				pJpeg->wSource				= m_wSource;
				dwTC = GetTickCount();

				if (m_pMainWnd)
				{
					m_pMainWnd->OnReceivedJpegData(pJpeg);
					m_dwReqPics--;
					WK_DELETE(pJpeg->Buffer.pLinAddr);
					WK_DELETE(pJpeg);
				}
			}
		}
	}
	m_csCodec.Unlock();
#if (1)
	// Alle 20 bzw. alle 40ms ein Bild schicken.
	Sleep((m_nStoreField == BOTH_FIELDS) ? 20 : 40);
#else
	Sleep(5);
#endif
	MotionDetection();
}


/////////////////////////////////////////////////////////////////////////////
BOOL CCodec::PollAlarm()
{
	WORD wCurrentAlarmState		= 0;
	BOOL  bRetFlag				= FALSE;

	// CurrentAlarmstatusregister lesen.
	wCurrentAlarmState = GetCurrentAlarmState();
	
	if (wCurrentAlarmState != m_wAlarmState)
	{										  
		m_wAlarmState = wCurrentAlarmState;
					
		if (m_pMainWnd)
			m_pMainWnd->OnReceivedAlarm(wCurrentAlarmState);

		bRetFlag = TRUE;
	}

	return bRetFlag;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CCodec::ActivateMotionDetection(WORD wSource)
{
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::DeactivateMotionDetection(WORD wSource)
{
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::SetMDMaskSensitivity(WORD wSource, const CString &sLevel)
{
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::SetMDAlarmSensitivity(WORD wSource, const CString &sLevel)
{
}

/////////////////////////////////////////////////////////////////////////////
CString CCodec::GetMDMaskSensitivity(WORD wSource)
{
	return CSD_NORMAL;
}

/////////////////////////////////////////////////////////////////////////////
CString CCodec::GetMDAlarmSensitivity(WORD wSource)
{
	return CSD_NORMAL;
}

//////////////////////////////////////////////////////////////////////
DWORD CCodec::ScanForCameras(DWORD dwScanMask)
{
	DWORD dwMask = 0xff;

	return dwMask;
}

//////////////////////////////////////////////////////////////////////
JPEG* CCodec::ReadJpegFile(BOOL bCurrentFrame)
{
	m_csCodec.Lock();

	// Für jede Kamera ein Jpegfile einlesen
	CString sFileName;	       

	JPEG* pJpeg = new JPEG;

	ZeroMemory(pJpeg, sizeof(JPEG));				
	TRY
	{
		sFileName.Format("Camera%02d\\J_%06d.jpg", m_wSource, m_nFrameCounter[m_wSource]);
		
		if (DoesFileExist(sFileName))
		{
			CFile file(sFileName, CFile::modeRead | CFile::shareDenyNone);	

			DWORD dwLen = file.GetLength();
			if (dwLen > 0)	     
			{
				pJpeg->Buffer.pLinAddr	= new BYTE[dwLen];
				pJpeg->Buffer.dwLen		= dwLen;
				pJpeg->dwLen			= dwLen;
				file.Read(pJpeg->Buffer.pLinAddr, dwLen);
				m_nFrameCounter[m_wSource]++;
			}
		}
		else
		{
			m_nFrameCounter[m_wSource] = 0;
		}
	}
	CATCH( CFileException, e )
	{
		m_nFrameCounter[m_wSource] = 0;
	}
	END_CATCH

	m_csCodec.Unlock();

	return pJpeg;
}

//////////////////////////////////////////////////////////////////////
void CCodec::MotionDetectionConfiguration()
{
	if (m_pMotionDetection)
		m_pMotionDetection->OpenConfigDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CCodec::MotionDetection()
{
	CPoint Point(0,0);

	// MotionDetection durchführen
	if (m_pMotionDetection && m_pMotionDetection->MotionCheck(m_wSource, Point))
	{
		if (m_pMainWnd)
			m_pMainWnd->OnReceivedMotionAlarm(m_wSource, Point);
	}
}
