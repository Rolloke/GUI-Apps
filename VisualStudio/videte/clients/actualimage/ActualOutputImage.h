// ActualOutputImage.h: interface for the CActualOutputImage class.
//
/////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTUALOUTPUTIMAGE_H__B9800ECE_F63C_11D2_8951_004005A1D890__INCLUDED_)
#define AFX_ACTUALOUTPUTIMAGE_H__B9800ECE_F63C_11D2_8951_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CIPCOutputClient.h"
#define IMAGETIMER 2000

class CActualOutputImage : public CIPCOutputClient  
{
	// construction/destruction
public:
	CActualOutputImage(const char* shmName);
	virtual ~CActualOutputImage();

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
	BOOL m_bDeleteDir;
	CString m_sDirectory;
	int m_iCurrentCamera;
	BOOL m_bIsReady;
};

inline BOOL CActualOutputImage::IsReady() const
{
	return m_bIsReady;
}

#endif // !defined(AFX_ACTUALOUTPUTIMAGE_H__B9800ECE_F63C_11D2_8951_004005A1D890__INCLUDED_)
