// Sequencer.h: interface for the CSequencer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SEQUENCER_H
#define _SEQUENCER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CIPCOutputVision;

class CSequencer  
{
	// construction/destruction
public:
	CSequencer(CIPCOutputVision* pCIPCOutputVision);
	virtual ~CSequencer();

	// attributes
public:
	inline CSecID	GetActiveCamera() const;

	// operations
public:
	void ActivateCamera(CSecID id);
	void ReactivateCamera(CSecID id, BOOL bSwitched);
	void DeactivateCamera(CSecID id,BOOL bForceCoCoStop=FALSE);
	void Sequence();

	// data member
protected:
	CIPCOutputVision*	m_pCIPCOutputVision;

	CCriticalSection	m_CS;
	CDWordArray m_ActiveCameras;
	CSecID		m_idActiveCamera;
	DWORD		m_dwLastSwitch;
	DWORD		m_dwStart;
};
/////////////////////////////////////////////////////////////////////
inline CSecID CSequencer::GetActiveCamera() const
{
	return m_idActiveCamera;
}

#endif
