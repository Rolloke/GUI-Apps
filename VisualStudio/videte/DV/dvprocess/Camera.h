// Camera.h: interface for the CCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERA_H__4B70B0C3_092F_11D4_A0CD_004005A19028__INCLUDED_)
#define AFX_CAMERA_H__4B70B0C3_092F_11D4_A0CD_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Output.h"

#define ALARMCALL_ACTIVITY	0x01
#define ALARMCALL_EXTERN	0x02
#define ALARMCALL_SUSPECT	0x04

class CCamera;
class CMedia;
class COutputClient;

class CCameraH26xEncoder : public CH26xEncoder
{
public:
	CCameraH26xEncoder(CCamera* pCamera);
	virtual ~CCameraH26xEncoder();

public:
	virtual void OnEncodedFrame();

private:
	CCamera* m_pCamera;
};

class CCameraMPEGEncoder : public CMPEG4Encoder
{
public:
	CCameraMPEGEncoder(CCamera* pCamera);
	virtual ~CCameraMPEGEncoder();

public:
	virtual void OnEncodedFrame();

private:
	CCamera* m_pCamera;
};

class CPreAlarmPictures : public CIPCPictureRecords
{
	// construction / destruction
public:
	CPreAlarmPictures(int iSize);
	virtual ~CPreAlarmPictures();

public:
	void AddNewPicture(const CIPCPictureRecord& pict);
	void FlushPictures(CCamera* pCamera, WORD wArchivNr);
	void SetPreAlarmSize(int iSize);

protected:
	
private:
	int m_iPreAlarmSize;
	int m_iNrOfIFrames;
};

class CClientCounter
{
public:
	CClientCounter(DWORD dwClientID)
	{
		m_dwClientID = dwClientID;
		m_lCounter = 0;
	}

public:
	inline DWORD GetClientID() const;
	inline LONG  GetCounter() const;
	inline void  Increment();
	inline void  Decrement();
	inline void  Reset();


private:
	DWORD m_dwClientID;
	volatile LONG m_lCounter;
};
///////////////////////////////////////////////////////////
inline DWORD CClientCounter::GetClientID() const
{
	return m_dwClientID;
}
///////////////////////////////////////////////////////////
inline LONG CClientCounter::GetCounter() const
{
	return m_lCounter;
}
///////////////////////////////////////////////////////////
inline void CClientCounter::Increment()
{
	m_lCounter++;
}
///////////////////////////////////////////////////////////
inline void CClientCounter::Decrement()
{
	m_lCounter--;
}
///////////////////////////////////////////////////////////
inline void CClientCounter::Reset()
{
	m_lCounter = 0;
}
/////////////////////////////////////////////////////////////////////////////
typedef CClientCounter* CClientCounterPtr;
WK_PTR_ARRAY_CS(CClientCounterArray,CClientCounterPtr,CClientCounterArrayCS);
/////////////////////////////////////////////////////////////////////////////
class CClientCounters : public CClientCounterArrayCS
{
public:
	LONG GetMinCounter();
	void Increment(DWORD dwClientID);
	void Decrement(DWORD dwClientID);
	void Reset();
};


class CCamera : public COutput  
{
	// construction / destruction
public:
	CCamera(COutputGroup* pGroup, CSecID id);
	virtual ~CCamera();

	// attributes
public:
	inline CIPCPictureRecords& GetPictures();
	inline CIPCPictureRecord* GetLivePicture();
	
	inline CInput*  GetMotionDetector();
	inline CInput*  GetExternDetector();
	inline CInput*  GetSuspectDetector();
	inline CInput*  GetAudioDetector();

	inline CMedia*  GetMedia();

	inline WORD		GetPreAlarmNr() const;
	inline WORD		GetAlarmNr() const;
	inline WORD		GetMDAlarmNr() const;
	inline WORD		GetAlarmListNr() const;
	inline DWORD	GetFramesPerDay() const;
	inline DWORD	GetFramesPerWeek() const;

	inline DWORD    GetTickConfirm() const;
	inline int	    GetTickConfirmDifference() const;
		   int		GetIntervall();
	inline CSecID   GetCurrentArchiveID() const;
	
	inline DWORD	GetDwellTime() const;
	inline BYTE		GetAlarmCallHandling() const;
	inline Compression GetCompression() const;
	inline DWORD    GetFPS() const;
	inline BOOL		GetTerminateVideoSignal() const;

	inline BOOL		IsPTZ() const;
	inline CameraControlType GetType() const;
	inline void		SetType(CameraControlType eType);
	inline DWORD	GetPTZID() const;
	inline void		SetPTZID(DWORD dwPTZID);
	inline int		GetComPort() const;
	inline void		SetComPort(int iComPort);
	inline int		GetNrOfTashaClients() const;

		   BOOL		IsInputActive() const;
		   void		GetMDCoordinates(DWORD& dwX,DWORD& dwY);
		   BOOL		IsLastSaveOld();
	inline CSystemTime GetLastSave() const;
		   BOOL		IsRecording() const;

	virtual void	FromString(const CString& s);
	virtual CString ToString();
	virtual CString Format();

	// operations
public:
	inline void SetInputs(CInput* pExtern, CInput* pSuspect, CInput* pMotion);
	inline void SetAudioInput(CInput* pAudio);
	inline void SetMedia(CMedia* pMedia);

	inline void	SetPreAlarmNr(WORD wPreAlarmNr);
	inline void	SetAlarmNr(WORD wAlarmNr);
	inline void	SetMDAlarmNr(WORD wMDAlarmNr);
	inline void	SetAlarmListNr(WORD wAlarmListNr);
	inline void SetDWellTime(DWORD dwDWellTimeInSeconds);
	inline void	SetAlarmCallHandling(BYTE bAlarmCallHandling);
	inline void SetCompression(Compression comp);
		   void SetFPS(DWORD dwFPS);
	inline void SetTerminateVideoSignal(BOOL bTerminateVideoSignal);

    virtual void SetActive(BOOL bActive);
	virtual void SetTimerActive(BOOL bTimerActive);
	virtual void SetMode(OutputMode mode);
			void SetMDFlag();
			void SaveReferenceImage();


	void NewPicture(const CIPCPictureRecord& pict,DWORD x, DWORD y);
	void NewTashaClientPicture(const CIPCPictureRecord& pict,DWORD x, DWORD y);

	void NewDIBPicture(WORD wXRes, WORD wYRes, const CIPCPictureRecord& pict);
	void FlushMDPictures();
	void SavePicture(const CIPCPictureRecord& pict,								
					 CIPCFields& fields,
					 WORD wArchiveNr);
	void ResetFramesPerDay();
	void ResetFramesPerWeek();
	void DeletePictures();

	void AddH263ClientQCIF(DWORD dwClientID);
	void DeleteH263ClientQCIF(DWORD dwClientID);
	void OnEncodedFrame(CompressionType ct);

	void AddMPEG4Client(COutputClient* pClient,Resolution res);
	void DeleteMPEG4Client(DWORD dwClientID);

	int  GetJPEGQuality();
	void AddCompressedJPEGSize(DWORD dwSize);
	
	virtual void Disable();
	virtual void Enable();

	// implementation
protected:
//	virtual void Disable();
	DWORD GetAverageSwitchtime();
	DWORD GetSwitchtimeVarianz();
	void  ResetUVVStatistics();
	void EncodeH263(const CIPCPictureRecord& pict);
	void EncodeMPEG4(const CIPCPictureRecord& pict);
	void DoTashaClientRequest();

	// data member
private:
	BOOL		m_bMDTrackFlag;

	CInput*		m_pExternDetector;
	CInput*		m_pSuspectDetector;
	CInput*		m_pMotionDetector;
	CInput*		m_pAudioDetector;
	CMedia*		m_pMedia;
	
	DWORD		m_dwTickConfirm;
	int			m_iTickDifference;
	CDWordArray m_dwSwitchTimes;
	int			m_iCurrentSwitchTimeIndex;

	DWORD		m_dwTashaLowBitrateConfirm;

	WORD		m_wPreAlarmNr;
	WORD		m_wAlarmNr;
	WORD		m_wMDAlarmNr;
	WORD		m_wAlarmListNr;
	CSecID		m_idCurrentArchive;

	DWORD		m_iRecordedFramesPerDay;
	DWORD		m_iRecordedFramesPerWeek;
	DWORD		m_iCurrentRecordedFramesPerDay;
	DWORD		m_iCurrentRecordedFramesPerWeek;
	DWORD		m_iGrabbedFramesPerDay;
	DWORD		m_iGrabbedFramesPerWeek;
	DWORD		m_iCurrentGrabbedFramesPerDay;
	DWORD		m_iCurrentGrabbedFramesPerWeek;
	CSystemTime m_stLastSave;
	DWORD		m_dwDwellTime;
	BYTE		m_byAlarmCallBits;
	Compression m_Compression;
	DWORD       m_dwFPS;
	
	// PTZ data
	BOOL				m_bPTZ;
	CameraControlType	m_eType;
	DWORD				m_dwPTZID;
	int					m_iComPort;

	// video signal termination
	BOOL				m_bTerminateVideoSignal;

	// image cache
	CPreAlarmPictures m_PreAlarmPictures;
	CIPCPictureRecord* m_pLivePicture;
	
	// JPEG to H.263 transcoding
	CJpeg*			   m_pJpeg;

	CCameraH26xEncoder*	   m_pH26xEncoderQCIF;
	CDWordArray			   m_dwH263ClientsQCIF;
	volatile LONG		   m_iH263ClientRequestsQCIF;

	CCameraMPEGEncoder*	   m_pMPEG4EncoderQCIF;
	CClientCounters		   m_MPEG4ClientsQCIF;

	// MPEG4 Tasha ISDN/DSL
	CClientCounters	   m_TashaClients;
	Compression		   m_cTashaLowbandwidthCompression;
	Resolution		   m_rTashaLowbandwidthResolution;

	// JPEG Compression rate
	int			m_iJPEGQuality;
	DWORD		m_dwSum;
	CDWordArray	m_dwaLengths;
	int			m_iCurrentLengthIndex;
};
//////////////////////////////////////////////////////////////////////
inline CInput* CCamera::GetMotionDetector()
{
	return m_pMotionDetector;
}
//////////////////////////////////////////////////////////////////////
inline CInput* CCamera::GetSuspectDetector()
{
	return m_pSuspectDetector;
}
//////////////////////////////////////////////////////////////////////
inline CInput* CCamera::GetExternDetector()
{
	return m_pExternDetector;
}
//////////////////////////////////////////////////////////////////////
inline CInput* CCamera::GetAudioDetector()
{
	return m_pAudioDetector;
}
//////////////////////////////////////////////////////////////////////
inline CMedia* CCamera::GetMedia()
{
	return m_pMedia;
}
//////////////////////////////////////////////////////////////////////
inline void CCamera::SetInputs(CInput* pExtern, CInput* pSuspect, CInput* pMotion)
{
	m_pExternDetector  = pExtern;
	m_pSuspectDetector = pSuspect;
	m_pMotionDetector  = pMotion;
}
//////////////////////////////////////////////////////////////////////
inline void CCamera::SetAudioInput(CInput* pAudio)
{
	m_pAudioDetector = pAudio;
}
//////////////////////////////////////////////////////////////////////
inline void CCamera::SetMedia(CMedia* pMedia)
{
	m_pMedia = pMedia;
}
//////////////////////////////////////////////////////////////////////
inline CIPCPictureRecords& CCamera::GetPictures()
{
	return m_PreAlarmPictures;
}
//////////////////////////////////////////////////////////////////////
inline CIPCPictureRecord* CCamera::GetLivePicture()
{
	return m_pLivePicture;
}
//////////////////////////////////////////////////////////////////////
inline WORD CCamera::GetPreAlarmNr() const
{
	return m_wPreAlarmNr;
}
//////////////////////////////////////////////////////////////////////
inline WORD	CCamera::GetAlarmNr() const
{
	return m_wAlarmNr;
}
//////////////////////////////////////////////////////////////////////
inline WORD	CCamera::GetMDAlarmNr() const
{
	return m_wMDAlarmNr;
}
//////////////////////////////////////////////////////////////////////
inline WORD	CCamera::GetAlarmListNr() const
{
	return m_wAlarmListNr;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CCamera::GetFramesPerDay() const
{
	return m_iRecordedFramesPerDay;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CCamera::GetFramesPerWeek() const
{
	return m_iRecordedFramesPerWeek;
}
//////////////////////////////////////////////////////////////////////
inline void	CCamera::SetPreAlarmNr(WORD wPreAlarmNr)
{
    m_wPreAlarmNr = wPreAlarmNr;
}
//////////////////////////////////////////////////////////////////////
inline void	CCamera::SetAlarmNr(WORD wAlarmNr)
{
	m_wAlarmNr = wAlarmNr;
}
//////////////////////////////////////////////////////////////////////
inline void	CCamera::SetMDAlarmNr(WORD wMDAlarmNr)
{
	m_wMDAlarmNr = wMDAlarmNr;
}
//////////////////////////////////////////////////////////////////////
inline void	CCamera::SetAlarmListNr(WORD wAlarmListNr)
{
	m_wAlarmListNr = wAlarmListNr;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CCamera::GetDwellTime() const
{
	return m_dwDwellTime;
}
//////////////////////////////////////////////////////////////////////
inline void CCamera::SetDWellTime(DWORD dwDWellTimeInSeconds)
{
    m_dwDwellTime = dwDWellTimeInSeconds;
}
//////////////////////////////////////////////////////////////////////
inline CSystemTime CCamera::GetLastSave() const
{
	return m_stLastSave;
}
//////////////////////////////////////////////////////////////////////
inline BYTE	CCamera::GetAlarmCallHandling() const
{
	return m_byAlarmCallBits;
}
//////////////////////////////////////////////////////////////////////
inline void	CCamera::SetAlarmCallHandling(BYTE bAlarmCallHandling)
{
	m_byAlarmCallBits = bAlarmCallHandling;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CCamera::GetTickConfirm() const
{
	return m_dwTickConfirm;
}
//////////////////////////////////////////////////////////////////////
inline int CCamera::GetTickConfirmDifference() const
{
	return m_iTickDifference;
}
//////////////////////////////////////////////////////////////////////
inline Compression CCamera::GetCompression() const
{
	return m_Compression;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CCamera::GetFPS() const
{
	return m_dwFPS;
}
//////////////////////////////////////////////////////////////////////
inline void CCamera::SetCompression(Compression comp)
{
	m_Compression = comp;
}
//////////////////////////////////////////////////////////////////////
inline BOOL CCamera::IsPTZ() const
{
	return m_bPTZ;
}
//////////////////////////////////////////////////////////////////////
inline CameraControlType CCamera::GetType() const
{
	return m_eType;
}
//////////////////////////////////////////////////////////////////////
inline void CCamera::SetType(CameraControlType eType)
{
	m_eType = eType;
	m_bPTZ = (m_eType != CCT_UNKNOWN);
}
//////////////////////////////////////////////////////////////////////
inline DWORD CCamera::GetPTZID() const
{
	return m_dwPTZID;
}
//////////////////////////////////////////////////////////////////////
inline void CCamera::SetPTZID(DWORD dwPTZID)
{
	m_dwPTZID = dwPTZID;
}
//////////////////////////////////////////////////////////////////////
inline int CCamera::GetComPort() const
{
	return m_iComPort;
}
//////////////////////////////////////////////////////////////////////
inline void CCamera::SetComPort(int iComPort)
{
	m_iComPort = iComPort;
}
//////////////////////////////////////////////////////////////////////
inline CSecID CCamera::GetCurrentArchiveID() const
{
	return m_idCurrentArchive;
}
//////////////////////////////////////////////////////////////////////
inline int CCamera::GetNrOfTashaClients() const
{
	return m_TashaClients.GetSize();
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CCamera::GetTerminateVideoSignal() const
{
	return m_bTerminateVideoSignal;
}
//////////////////////////////////////////////////////////////////////
inline void CCamera::SetTerminateVideoSignal(BOOL bTerminateVideoSignal)
{
	m_bTerminateVideoSignal = bTerminateVideoSignal;
}

#endif // !defined(AFX_CAMERA_H__4B70B0C3_092F_11D4_A0CD_004005A19028__INCLUDED_)
