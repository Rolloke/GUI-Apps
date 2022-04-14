// CIPCMediaVideo.h: interface for the CIPCMediaVideo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCMEDIAAUDIOUNIT_H__72262C36_3D40_4CD7_8537_A30AF43FD7D4__INCLUDED_)
#define AFX_IPCMEDIAAUDIOUNIT_H__72262C36_3D40_4CD7_8537_A30AF43FD7D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _DEBUG
#include <CIPCMedia.h>
#include <MediaSampleRecord.h>

class CUSBCameraUnitDlg;

class CIPCMediaVideo : public CIPCMedia  
{
	// @access construction / destruction
public:
	CIPCMediaVideo(LPCTSTR, BOOL, CUSBCameraUnitDlg*);
	virtual ~CIPCMediaVideo();
	// Client-Server Information
public:
	CSecID GetSecID() const;
	// Media Reset:
	virtual void OnRequestReset(WORD wGroupID);
	// Media Hardware:
	virtual void OnRequestHardware(CSecID secID, DWORD dwIDRequest);
	// Media Encoding
	virtual void OnRequestStartMediaEncoding(CSecID mediaID, DWORD dwFlags, DWORD dwUserData);
	virtual void OnRequestStopMediaEncoding(CSecID mediaID, DWORD dwFlags);
	virtual void OnIndicateMediaData(const CIPCMediaSampleRecord& media, CSecID mediaID, DWORD dwUserData);
	// Media Decoding
	virtual void OnRequestMediaDecoding(const CIPCMediaSampleRecord&, CSecID mediaID, DWORD dwFlags);
	virtual void OnRequestStopMediaDecoding(CSecID mediaID, DWORD dwFlags);
	// Media Values
	virtual void OnRequestSetValue(CSecID , const CString &, const CString &, DWORD);
	virtual void OnRequestValues(CSecID, DWORD, DWORD, DWORD, const CIPCExtraMemory*);
	virtual void OnConfirmValues(CSecID, DWORD, DWORD, DWORD dwValue  , const CIPCExtraMemory*pData);
	// Connection
	virtual void OnReadChannelFound();
	
	BOOL IsConnected();
private:
	// no implementation, to avoid implicit generation!
	CIPCMediaVideo(const CIPCMediaVideo& src);		
	// no implementation, to avoid implicit generation!
	void operator=(const CIPCMediaVideo& src);	

	CUSBCameraUnitDlg *m_pDlg;
	CSecID         m_SecID;
};
#endif // DEBUG
#endif // !defined(AFX_IPCMEDIAAUDIOUNIT_H__72262C36_3D40_4CD7_8537_A30AF43FD7D4__INCLUDED_)
