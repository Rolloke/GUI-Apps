// VOutThread.h: interface for the CVOutThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VOUTTHREAD_H__D28DCE53_1D12_11D5_99C6_004005A19028__INCLUDED_)
#define AFX_VOUTTHREAD_H__D28DCE53_1D12_11D5_99C6_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVOutThread : public CWK_Thread  
{
	// construction / destruction
public:
	CVOutThread();
	virtual ~CVOutThread();

	// operations
public:
	void AddActivity(CSecID idCam, int iCard);

	// overrides
public:
	virtual	BOOL StartThread();
	virtual	BOOL Run(LPVOID lpContext);
	virtual	BOOL StopThread();

protected:
	BOOL Sequence(int i, int j, DWORD dwTick);
	BOOL Activity(int i, int j, DWORD dwTick);

private:
	DWORD m_dwVOutPortTime[NR_OF_CARDS][NR_OF_VOUT]; // in ms
	DWORD m_dwVOutPortTick[NR_OF_CARDS][NR_OF_VOUT];
	WORD  m_wCamSubID[NR_OF_CARDS][NR_OF_VOUT];

	CDWordArray			m_dwActivities[NR_OF_CARDS];
	CCriticalSection	m_csActivities[NR_OF_CARDS];
};

#endif // !defined(AFX_VOUTTHREAD_H__D28DCE53_1D12_11D5_99C6_004005A19028__INCLUDED_)
