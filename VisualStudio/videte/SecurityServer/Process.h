/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: Process.h $
// ARCHIVE:		$Archive: /Project/SecurityServer/Process.h $
// CHECKIN:		$Date: 14.08.06 16:00 $
// VERSION:		$Revision: 103 $
// LAST CHANGE:	$Modtime: 14.08.06 13:39 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _CProcess_H
#define _CProcess_H

#include "Activation.h"
#include "ProcessMacro.h"

class CIPCOutputServerClient;
class CIPCExtraMemory;
class CIPCPictureRecord;
class CInput;
class COutput;
class CActivation;
class CSecTimer;
class CProcessScheduler;
class CIPCInputServerClient;

enum SecProcessType 
{
	SPT_NONE,
	SPT_ENCODE,
	SPT_DECODE,
	SPT_VIDEO,
	SPT_RELAY,
	SPT_ALARM_CALL,
	SPT_BACKUP,
	SPT_ACTUAL_IMAGE,
	SPT_IMAGE_CALL,
	SPT_CHECK_CALL,
	SPT_GUARD_TOUR,
	SPT_VIDEO_OUT,
	SPT_AUDIO,
	SPT_PTZ_PRESET,
	SPT_ALARM_LIST,
	SPT_UPLOAD,
};
enum SecProcessState 
{
	SPS_INITIALIZED,
	SPS_ACTIVE_RECORDING,
	SPS_ACTIVE_IDLE,
	SPS_PAUSED,
	SPS_TERMINATED
};
LPCTSTR NameOfEnum(SecProcessState state);
LPCTSTR NameOfEnum(SecProcessType type);

class CConnectionRecord;

class CProcess 
{
	// construction / destruction
public:
	CProcess(CProcessScheduler* pScheduler);
	virtual ~CProcess();

	// attributes
public:
	inline CSecID					GetID() const;
	inline SecProcessType			GetType() const;
	inline SecProcessState			GetState() const;
	inline BOOL						IsMarkedForTermination() const;
	inline BOOL						IsTerminated() const;
	inline CInput*					GetAlarmInput();
	inline const COutput*			GetRelay() const;
	inline const CProcessMacro&		GetMacro() const;
	inline const CActivation*		GetActivation() const;
	inline const CSecTimer*			GetTimer() const;
	inline CSecID					GetCamID() const;
	inline DWORD					GetTickInit() const;
	inline DWORD					GetTickTerminated() const;
	inline CProcessScheduler*		GetScheduler() const;
	inline CompressionType			GetCompressionType() const;

	//
	inline virtual BOOL IsRelayProcess() const;
	inline virtual BOOL IsBackupProcess() const;
	inline virtual BOOL IsRecordingProcess() const;
	inline virtual BOOL IsMDProcess() const;
	inline virtual BOOL IsActiveRecordingProcess() const;
	inline virtual BOOL IsActualImageProcess() const;
	inline virtual BOOL IsImageCallProcess() const;
	inline virtual BOOL IsCheckCallProcess() const;
	inline virtual BOOL IsGuardTourProcess() const;
	inline virtual BOOL IsVideoOutProcess() const;
	inline virtual BOOL IsAudioProcess() const;
	inline virtual BOOL IsPTZPresetProcess() const;
	inline virtual BOOL IsAlarmListProcess() const;
	inline virtual BOOL IsClientProcess() const;
	inline virtual BOOL IsClientRelayProcess() const;
	inline virtual BOOL IsClientEncodingProcess() const;
	inline virtual BOOL IsUploadProcess() const;
	inline virtual BOOL IsCallProcess() const;

	// overrides
public:
	virtual CString GetStateName() const;
	virtual CString	GetDescription(BOOL bActive);
	virtual CString Format();
	virtual CSecID  GetClientID() const;

	virtual void Run();
	virtual void OnPause();
	virtual void OnUnitDisconnected();
	virtual	BOOL Terminate(BOOL bShutDown=FALSE);
	virtual void OnStateChanging(SecProcessState newState,BOOL bShutDown);
	virtual void Restart();
	virtual void OnModified(BOOL bNewOutputID);

	virtual void PictureData(const CIPCPictureRecord& pict,DWORD dwMDX, DWORD dwMDY, DWORD dwUserID);
	virtual void MediaData(const CIPCMediaSampleRecord& rec, CSecID idMedia);
	
	// operations
public:
			void CalcMode(DWORD cti);
			void SetState(SecProcessState s, DWORD cti, BOOL bShutDown=FALSE);
	inline  void StatLog(int iValue);
	//
	BOOL Modify(CSecID camID, Resolution res, Compression comp, 
				int iNumPictures, DWORD dwClientTimeSlice, DWORD dwBitrateScale);

	// Tasha Drop counter
	DWORD	m_dwDropCounter;
	// unsorted
	//
	int m_iNumVirtualSeconds;


	inline int  GetPictureCount();

	inline void Lock(LPCTSTR sFkt=NULL);
	inline void Unlock();

	//
	int m_iNumPicturesRequested;

	// operations
public:
	void WriteProtocol(const CString& sLine);
	void AddAlarmFields(CIPCFields& fields, int iGOPLength);

	// implementation
protected:
	void AddDefaultFields(CIPCFields& fields);
	void InitTimer();
	void CleanUp();
	void DoStatLog(int iValue);	// called from inlined StatLog

	void InitProtocolFile();
	void ExitProtocolFile();

	// data member
protected:
	CProcessScheduler*	m_pProcessScheduler;
	CCriticalSection    m_criticalSection;
	// general:
	SecProcessState m_state;
	SecProcessType	m_type;
	CSecID			m_id;
	BOOL			m_bMarkedForTermination;
	// links:
	CActivation			*m_pActivation;
	const CSecTimer		*m_pTimer;
	// tick values, all set in CalcMode/Setmode
	DWORD m_tickInit;
	DWORD m_tickIntervalStart;
	DWORD m_tickSliceStart;
	DWORD m_tickTerminated;
	//
	BOOL m_bHadStatLog;
	//
	volatile int m_iNumPicturesReceived;
	volatile int m_iNumSlicePicturesReceived;
	CCriticalSection    m_csPics;
	//
	//
	const COutput *m_pRelay;
	//
	static WORD m_tmpProcessCounter;

	//
	CompressionType m_CompressionType;

	// static trace member
public:
	static int m_iTraceLevel;	// 0 default, 1 detailed, 99 nothing at all
	static BOOL m_bTraceProcessTermination;
	static BOOL m_bAdjustStartTime;
	static BOOL m_bTraceProcessRun;
	static BOOL m_bTraceProcesses;

private:
	CStdioFileU			 m_sfProtocolFile;
};
/////////////////////////////////////////////////////////////////////////////
#include "Process.inl"
/////////////////////////////////////////////////////////////////////////////
typedef CProcess * CProcessPtr;
WK_PTR_ARRAY_CS(CProcessArray,CProcessPtr,CProcessArraySafe)
/////////////////////////////////////////////////////////////////////////////
class CProcesses : public CProcessArraySafe
{
	// attributes
public:
	CProcesses();
	CProcess* GetByID(CSecID id);
};

#endif
