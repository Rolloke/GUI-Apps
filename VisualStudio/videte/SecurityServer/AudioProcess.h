// AudioProcess.h: interface for the CAudioProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUDIOPROCESS_H__D4FFE610_B4F8_4D9E_918B_11523A48E104__INCLUDED_)
#define AFX_AUDIOPROCESS_H__D4FFE610_B4F8_4D9E_918B_11523A48E104__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Process.h"

class CAudioProcess : public CProcess  
{
public:
	CAudioProcess(CProcessScheduler* pScheduler,
				  CSecID idClient,
				  CSecID mediaID,
				  DWORD dwFlags);
	CAudioProcess(CProcessScheduler* pScheduler,
				  CActivation *pActivation);
	virtual ~CAudioProcess();

	// attributes
public:
	inline virtual BOOL IsClientProcess() const;
	inline virtual BOOL IsAudioProcess() const;
	inline CSecID GetClientID() const;

	// overrides
public:
	virtual void MediaData(const CIPCMediaSampleRecord& rec, CSecID idMedia);

	// data member
protected:
	CSecID	m_idClient;
};
///////////////////////////////////////////////////////////////
inline BOOL CAudioProcess::IsClientProcess() const
{
	return m_idClient != SECID_NO_ID;
}
///////////////////////////////////////////////////////////////
inline CSecID CAudioProcess::GetClientID() const
{
	return m_idClient;
}
///////////////////////////////////////////////////////////////
inline BOOL CAudioProcess::IsAudioProcess() const
{
	return TRUE;
}

#endif // !defined(AFX_AUDIOPROCESS_H__D4FFE610_B4F8_4D9E_918B_11523A48E104__INCLUDED_)
