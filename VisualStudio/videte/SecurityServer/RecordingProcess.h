// RecordingProcess.h: interface for the CRecordingProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECORDINGPROCESS_H__9A914185_CE4C_11D2_B598_004005A19028__INCLUDED_)
#define AFX_RECORDINGPROCESS_H__9A914185_CE4C_11D2_B598_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Process.h"

class CRecordingProcess : public CProcess  
{
	// Construction/Destruction
public:
	CRecordingProcess(CProcessScheduler* pScheduler,
					  CActivation *pActivation, 
					  CompressionType ct,
					  int iNumPicturesRecorded);
	virtual ~CRecordingProcess();

	// attributes
public:
	inline BOOL GetDoStoring() const;

	// overrides
public:
			void SetDoStoring(BOOL bDoStoring);
			void CleanNotStoringProcesses();
			void AddNotStoringProcess(CProcess* pProcess);
	virtual void PictureData(const CIPCPictureRecord& pict,DWORD dwMDX, DWORD dwMDY, DWORD dwUserID);
	virtual void OnStateChanging(SecProcessState newState, BOOL bShutDown);

	// implementation
protected:
	void IncrementPictureCount(int iNumPics);

private:
	CString	   m_sRegistryName;
	int		   m_iFlush;
	BOOL	   m_bDoStoring;
	CProcesses m_NotStoringProcesses;
	BOOL	   m_bTrackingInfoAdded;
};
//////////////////////////////////////////////////////////////////////
inline BOOL CRecordingProcess::GetDoStoring() const
{
	return m_bDoStoring;
}

#endif // !defined(AFX_RECORDINGPROCESS_H__9A914185_CE4C_11D2_B598_004005A19028__INCLUDED_)
