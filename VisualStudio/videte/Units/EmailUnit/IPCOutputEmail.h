// IPCOutputEmail.h: interface for the CIPCOutputEmail class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCOUTPUTEMAIL_H__BE414929_1103_11D4_A9EC_004005A26A3B__INCLUDED_)
#define AFX_IPCOUTPUTEMAIL_H__BE414929_1103_11D4_A9EC_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEmailUnitDlg;

class CIPCOutputEmail : public CIPCOutputClient   
{

public:

	CIPCOutputEmail(LPCTSTR shmName, CEmailUnitDlg* pDlg, CSecID id
#ifdef _UNICODE
		, WORD wCodePage
#endif
	);
	virtual ~CIPCOutputEmail();


	inline CIPCPictureRecord* GetPicture() const;


	inline BOOL CameraIsJpeg();
	inline BOOL CameraIsMpeg();
	CString GetCamName();
	CString GetJobTime();

	virtual void OnDeleteRecord(const CIPCOutputRecord &pRec);
	virtual void OnAddRecord(const CIPCOutputRecord &rec);
	virtual void OnUpdateRecord(const CIPCOutputRecord &pRec);
	virtual void OnConfirmJpegEncoding(const CIPCPictureRecord &pict,
										DWORD dwUserData,
										CSecID idArchive);
	virtual void OnIndicateH263Data(const CIPCPictureRecord &pict, 
										DWORD dwJobData,
										CSecID idArchive);
	virtual void OnIndicateVideo(const CIPCPictureRecord& pict,
		DWORD dwMDX,
		DWORD dwMDY,
		DWORD dwUserData,
		CSecID idArchive);


private:

	CSystemTime m_JobTime;
	BOOL m_bCameraIsJpeg;
	BOOL m_bCameraIsMpeg;
	CString m_sCamName;
	CSecID m_CamID;
	CEmailUnitDlg* m_pDlg;
	CIPCPictureRecord* m_pPicture;
};
//////////////////////////////////////////////////////////////////////////////////////
inline CIPCPictureRecord* CIPCOutputEmail::GetPicture() const
{
	return m_pPicture;
}
//////////////////////////////////////////////////////////////////////////////////////
inline BOOL CIPCOutputEmail::CameraIsJpeg()
{
	return m_bCameraIsJpeg;	
}
//////////////////////////////////////////////////////////////////////////////////////
inline BOOL CIPCOutputEmail::CameraIsMpeg()
{
	return m_bCameraIsMpeg;	
}
//////////////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_IPCOUTPUTEMAIL_H__BE414929_1103_11D4_A9EC_004005A26A3B__INCLUDED_)
