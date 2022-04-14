#pragma once

#include "process.h"
class CProcessSchedulerVideoBase;

class CUploadProcess : public CProcess
{
public:
	CUploadProcess(CProcessSchedulerVideoBase* pScheduler,
					CActivation *pActivation, 
					CompressionType ct,
					int iNumPicturesRecorded);
	virtual ~CUploadProcess(void);

	// attributes
public:
	inline CString GetCameraName() const;

	// overrides
public:
	virtual void PictureData(const CIPCPictureRecord& pict,DWORD dwMDX, DWORD dwMDY, DWORD dwUserID);
	virtual void OnStateChanging(SecProcessState newState, BOOL bShutDown);

private:
	CString		   m_sCameraName;
};
inline CString CUploadProcess::GetCameraName() const
{
	return m_sCameraName;
}
