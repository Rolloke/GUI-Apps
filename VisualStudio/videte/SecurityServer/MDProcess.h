// MDProcess.h: interface for the CMDProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDPROCESS_H__D3E5AB71_530A_11D4_9E14_004005A19028__INCLUDED_)
#define AFX_MDPROCESS_H__D3E5AB71_530A_11D4_9E14_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RecordingProcess.h"

class CMDProcess : public CRecordingProcess  
{
	// construction / destruction
public:
	CMDProcess(CProcessScheduler* pScheduler,CActivation *pActivation, CompressionType ct);
	virtual ~CMDProcess();

	// operations
public:
	virtual void OnStateChanging(SecProcessState newState, BOOL bShutDown);
	virtual void OnUnitDisconnected();
	inline virtual BOOL IsMDProcess() const;

	// overrides
public:
	virtual void PictureData(const CIPCPictureRecord& pict,DWORD dwMDX, DWORD dwMDY, DWORD dwUserID);

	// implementation
protected:
			void FlushMDPictures();

	// data member
private:
	CIPCPictureRecords m_Pictures;
	DWORD m_dwNrOfIFrames;
};
inline BOOL CMDProcess::IsMDProcess() const
{
	return TRUE;
}

#endif // !defined(AFX_MDPROCESS_H__D3E5AB71_530A_11D4_9E14_004005A19028__INCLUDED_)
