// IPCamera.h: interface for the CIPCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCAMERA_H__A98C1D16_F2F9_11D2_A8CD_004005A26A3B__INCLUDED_)
#define AFX_IPCAMERA_H__A98C1D16_F2F9_11D2_A8CD_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CViewcamDlg;
class CIPCamera : public CIPCOutputClient  
{
public:
	virtual void OnConfirmJpegEncoding(const CIPCPictureRecord &pict,DWORD dwUserData);
	CIPCamera(const char* shmName);
	virtual ~CIPCamera();

};

#endif // !defined(AFX_IPCAMERA_H__A98C1D16_F2F9_11D2_A8CD_004005A26A3B__INCLUDED_)
