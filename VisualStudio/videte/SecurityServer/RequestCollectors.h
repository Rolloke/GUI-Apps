/* GlobalReplace: Optimiziation --> Optimization */
/* GlobalReplace: TraceOptimize --> TraceRequestOptimization */
// RequestCollector.h: interface for the CRequestCollector class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CRequestCollectors_h
#define CRequestCollectors_h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wk.h"
#include "PictureDef.h"
#include "RequestCollector.h"	// for PictureRecord

// @doc
// @class collect video encoding request for multiple processes
WK_PTR_ARRAY_CS(CRequestCollectorsPlain,CRequestCollector*,CRequestCollectorsCS)

class CRequestCollectors : public CRequestCollectorsCS
{
	// construction / destruction
public:
	CRequestCollectors(CIPCOutputServer* pParent);
	CRequestCollectors(CIPCAudioServer* pParent);
	virtual ~CRequestCollectors();

	// attributes
public:
	inline  CIOGroup*		  GetIOGroup() const;
	inline  CIPCOutputServer* GetCIPCOutputServer() const;
	inline  CIPCAudioServer*  GetCIPCAudioServer() const;

	// operations
public:
	int  AddNewRequest(CProcess *pProcess, int iNumPics);

	int  AddNewAudioRequest(CProcess *pProcess);
	int  AddNewVideoRequest(CProcess *pProcess);
	int  ActualizeRequest(CProcess *pProcess);

	int  RemoveAudioRequest(CProcess *pProcess);
	int  RemoveVideoRequest(CProcess *pProcess);

	BOOL InsertRequest(CRequestCollector *pRequest,int iNumPics, int iPos);
	void OptimizeRequests(int iNumCameras, int iNumSwitches);
	void SendRequestsToUnit(CIPCOutputServer *pUnit);

	BOOL OnIndicateVideo(const CIPCPictureRecord &pict,DWORD dwMDX,DWORD dwMDY,DWORD dwUserID);
	BOOL OnIndicateMediaData(const CIPCMediaSampleRecord& rec, CSecID secID, DWORD dwUserData);

	void DumpIDs(LPCTSTR szName);
	void Trace();

	void DoRequest(CRequestCollector* pRequest, BOOL bStart);
	void DoTashaRequest(CRequestCollector* pRequest, BOOL bStart);
	void DoQRequest(CRequestCollector* pRequest, BOOL bStart);
	void DoIPRequest(CRequestCollector* pRequest, BOOL bStart);

	void CalcQRequests(BOOL bDecreaseFPS);

	void CheckForTimeout();
	void CheckProcesses();
	void CheckIncreaseFramerate();
	void CheckDecreaseFramerate();

	// implementation
protected:
	inline  CRequestCollector* GetPreviousRequest(int ix) const;	// non idle requests
	inline  CRequestCollector* GetNextRequest(int ix) const;	// non idle requests
	static	void			   DumpRequests(const CRequestCollectorsCS* pRCs,const CString& sPrefix,int iFPS);
			void			   SortRequests(int iNumSlots);
			void			   MergeContinousJobs();
			void			   AdjustIdleRequests(int iRemovedTrailingIdleRequests);
			void			   PostOptimize(int iNumSlots);

	// data member
public:
	// static configurable data
	static BOOL ms_bDisableRequestOptimization;
	static BOOL ms_bDisableRequestMultiplexing;
	static BOOL ms_bTraceRequestOptimization;
	static BOOL ms_bTraceRequestMultiplexing;

	// data member
private:
	CIPCOutputServer* m_pCIPCOutputServer;
	CIPCAudioServer*  m_pCIPCAudioServer;
	int				  m_iFps;
	int				  m_iNumCameraSwitches;
	int				  m_iNumTotalPicturesRequested;
	int				  m_iNumTotalCameras;

	volatile int	  m_iFpsTasha[2];
	CCriticalSection  m_csFpsTasha;

	friend class CProcessScheduler;
};
////////////////////////////////////////////////////////
inline  CIPCOutputServer *CRequestCollectors::GetCIPCOutputServer() const
{
	return m_pCIPCOutputServer;
}
////////////////////////////////////////////////////////
inline  CIPCAudioServer *CRequestCollectors::GetCIPCAudioServer() const
{
	return m_pCIPCAudioServer;
}
////////////////////////////////////////////////////////
inline CIOGroup* CRequestCollectors::GetIOGroup() const
{
	if (m_pCIPCOutputServer)
	{
		return m_pCIPCOutputServer;
	}
	if (m_pCIPCAudioServer)
	{
		return m_pCIPCAudioServer;
	}
	return NULL;
}
////////////////////////////////////////////////////////
inline CRequestCollector * CRequestCollectors::GetPreviousRequest(int ix) const
{
	CRequestCollector *pResult = NULL;

	for (int i=ix;i>=0 && i<GetSize() && pResult==NULL;i--) {
		pResult = GetAtFast(i);
		if (pResult->IsIdleRequest()) {
			pResult = NULL;
		} else {
			// found the previous non idle job
		}
	}

	return pResult;
}
////////////////////////////////////////////////////////
inline CRequestCollector * CRequestCollectors::GetNextRequest(int ix) const
{
	CRequestCollector *pResult = NULL;

	for (int i=ix;i>=0 && i<GetSize() && pResult==NULL;i++) {
		pResult = GetAtFast(i);
		if (pResult->IsIdleRequest()) {
			pResult = NULL;
		} else {
			// found the previous non idle job
		}
	}

	return pResult;
}


#endif // !defined(AFX_REQUESTCOLLECTOR_H__21954F36_6341_11D2_B955_00C095ECA33E__INCLUDED_)
