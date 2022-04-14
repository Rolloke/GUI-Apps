// CIPCOutputActualImage.h: interface for the CIPCOutputActualImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCOUTPUTACTUALIMAGE_H__648371E4_FA43_11D2_B5D9_004005A19028__INCLUDED_)
#define AFX_CIPCOUTPUTACTUALIMAGE_H__648371E4_FA43_11D2_B5D9_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CIPCOutputClient.h"

class CActualImageProcess;

class CIPCOutputActualImage : public CIPCOutputClient  
{
public:
	CIPCOutputActualImage(CActualImageProcess* pProcess, LPCTSTR shmName);
	virtual ~CIPCOutputActualImage();

	// attributes
public:
	inline BOOL IsReady() const;

	// overrides
public:
	virtual void OnConfirmInfoOutputs(WORD wGroupID, int iNumGroups, 
								int iNumRecords, 
								const CIPCOutputRecord records[]);
	virtual void OnAddRecord(const class CIPCOutputRecord &pRec);
	virtual void OnDeleteRecord(const class CIPCOutputRecord &pRec);
	virtual void OnUpdateRecord(const class CIPCOutputRecord &pRec);
	virtual void OnConfirmJpegEncoding(const CIPCPictureRecord &pict,DWORD dwUserData);

	// implementation
protected:
	void RequestNextCamera();
	void SaveImage(const CIPCPictureRecord &pict);

	// data member
private:
	int m_iCurrentCamera;
	BOOL m_bIsReady;
	CActualImageProcess* m_pProcess;
};

inline BOOL CIPCOutputActualImage::IsReady() const
{
	return m_bIsReady;
}

#endif // !defined(AFX_CIPCOUTPUTACTUALIMAGE_H__648371E4_FA43_11D2_B5D9_004005A19028__INCLUDED_)
