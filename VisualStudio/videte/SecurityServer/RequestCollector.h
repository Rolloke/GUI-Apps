// RequestCollector.h: interface for the CRequestCollector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REQUESTCOLLECTOR_H__21954F36_6341_11D2_B955_00C095ECA33E__INCLUDED_)
#define AFX_REQUESTCOLLECTOR_H__21954F36_6341_11D2_B955_00C095ECA33E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wk.h"
#include "PictureDef.h"
#include "CIPCOutputServer.h"	// for PictureRecord
#include "IPCAudioServer.h"	// for PictureRecord
#include "Process.h"

class CRequestCollectors;
// @doc
// @class collect video encoding request for multiple processes
class CRequestCollector  
{
	// construction / destruction
public:
	CRequestCollector(CRequestCollectors *pParent, const CProcess *pProcess=NULL,int iNumPics=0, DWORD dwID=0);
	CRequestCollector(const CRequestCollector &src);

	// attributes
public:
	inline CSecID			GetCamID() const;
	inline DWORD			GetID() const;
	inline int				GetNumPics() const;
	inline int				GetIFrame() const;
	inline CompressionType	GetCompressionType() const;
	inline Resolution		GetResolution() const;
	inline Compression		GetCompression() const;
	inline BOOL				IsIdleRequest() const;
	inline int				GetNrOfProcesses() const;
	inline DWORD			GetProcessID(int i) const;

	// compare camID, compression etc.... NOT iNumPics
	BOOL IsSimilarRequest(const CProcess *pProcess) const;
	BOOL IsSimilarRequest(const CRequestCollector *pRequest) const;

	BOOL IsTimedOut();
	BOOL CheckProcesses();

	// operations
public:
	void OnIndicateVideo(const CIPCPictureRecord &pict,DWORD dwMDX,DWORD dwMDY);
	void OnIndicateMediaData(const CIPCMediaSampleRecord& rec, CSecID secID);
	void AddProcessID(DWORD dwPID);
	BOOL RemoveProcessID(DWORD dwPID);
	BOOL FindProcess(DWORD dwPID);
	void RemoveAllProcessIDs();
	void ResetTimeStamp();
	
	void SetID(DWORD dwID);
	void SetCamID(CSecID id);
	void SetNumPics(int iNumPics);
	void SetParent(CRequestCollectors *pParent);
	
	BOOL ActualizeTashaRequest(CProcess *pProcess);
	BOOL ActualizeQRequest(CProcess *pProcess);
	BOOL CalcTashaRequests(DWORD iMaxFPS=25);
	int  GetMinFrameCounter();

	void  Trace();

	static DWORD GetIFrameDefault(BOOL bLowBandwidth,int iFPS);

private:
	CRequestCollectors*	m_pParent;
	CSecID				m_camID;	// Audio or Video unit id
	CSecID				m_id;		// own id, which is sent to the unit

	Resolution		m_Resolution;
	Compression		m_Compression;
	CompressionType m_CompressionType;
	int				m_iNumPics;
	DWORD			m_IFrameIntervall;
	DWORD			m_dwBitrateScale;
	
	volatile DWORD   m_dwTickLastPicture;
	CCriticalSection m_csTickLastPicture;
	
	CDWordArray		m_attachedProcesses; // the process id's

public:
	static WORD ms_wSubIDCounter;

};
///////////////////////////////////////////////////////////////////
inline DWORD CRequestCollector::GetID() const
{
	return m_id.GetID();
}
///////////////////////////////////////////////////////////////////
inline CSecID CRequestCollector::GetCamID() const
{
	return m_camID;
}
///////////////////////////////////////////////////////////////////
inline int CRequestCollector::GetNumPics() const
{
	return m_iNumPics;
}
///////////////////////////////////////////////////////////////////
inline CompressionType CRequestCollector::GetCompressionType() const
{
	return m_CompressionType;
}
///////////////////////////////////////////////////////////////////
inline BOOL CRequestCollector::IsIdleRequest() const
{
	return (m_attachedProcesses.GetSize()==0);
}
///////////////////////////////////////////////////////////////////
inline Resolution CRequestCollector::GetResolution() const
{
	return m_Resolution;
}
///////////////////////////////////////////////////////////////////
inline Compression CRequestCollector::GetCompression() const
{
	return m_Compression;
}
///////////////////////////////////////////////////////////////////
inline int CRequestCollector::GetIFrame() const
{
	return m_IFrameIntervall;
}
///////////////////////////////////////////////////////////////////
inline int CRequestCollector::GetNrOfProcesses() const
{
	return m_attachedProcesses.GetSize();
}
///////////////////////////////////////////////////////////////////
inline DWORD CRequestCollector::GetProcessID(int i) const
{
	return m_attachedProcesses[i];
}
///////////////////////////////////////////////////////////////////
#endif // !defined(AFX_REQUESTCOLLECTOR_H__21954F36_6341_11D2_B955_00C095ECA33E__INCLUDED_)
