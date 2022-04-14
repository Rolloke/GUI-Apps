/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CameraControlRecord.h $
// ARCHIVE:		$Archive: /Project/Units/CommUnit/CameraControlRecord.h $
// CHECKIN:		$Date: 20.10.05 13:28 $
// VERSION:		$Revision: 18 $
// LAST CHANGE:	$Modtime: 20.10.05 13:16 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __CameraControlRecord_H
#define __CameraControlRecord_H

#include "wk.h"
#include "SecID.h"
#include "CipcInput.h"
#include "CameraControl.h"
#include "CRS232.h"

/////////////////////////////////////////////////////////////////////////////
#define _MAX_CAMERACOMMAND 32
/////////////////////////////////////////////////////////////////////////////
class CCameraCommandRecord;
class CRS232;
UINT RepeatProc(LPVOID pParam);
/////////////////////////////////////////////////////////////////////////////
class CCameraControlRecord
{
// construction/destruction
public:
	CCameraControlRecord();
	virtual ~CCameraControlRecord();

	BOOL	Add(CameraControlType type, CSecID secID, DWORD camID);
	void	Destroy();

// access
public:
//	CSecID				GetCamID() const;
	BOOL				HasSecID(CSecID secID);
	CameraControlType	GetType() const;
	CRS232*				GetRS232();

// overrides
public:
	virtual BOOL	Create(CameraControlType type, int iComm);
	virtual void	OnNewCommand(CCameraCommandRecord* pCCommandR);
	virtual void	OnReceivedData(LPVOID pBuffer,DWORD dwLen){};

// operations
public:
	void	ReceivedDataVCLMaxCom(LPVOID pBuffer,DWORD dwLen);
protected:
private:
	void	SendCommand();

// implementation
private:
	void	CommandBufferSensormaticTouchTracker(CCameraCommandRecord* pCCommandR);
	CameraControlCmd CommandBufferPanasonicWV_RM70(CCameraCommandRecord* pCCommandR);
	void	CommandBufferErnitecBDR510(CCameraCommandRecord* pCCommandR);
	void	CommandBufferElbex(CCameraCommandRecord* pCCommandR);
	void	CommandBufferPieper(CCameraCommandRecord* pCCommandR);
	void	CommandBufferVidev(CCameraCommandRecord* pCCommandR);
	void	CommandBufferCanon(CCameraCommandRecord* pCCommandR);
	void	CommandBufferVCLCamera(CCameraCommandRecord* pCCommandR);
	void	CommandBufferVCLMaxCom(CCameraCommandRecord* pCCommandR);
	void	CommandBufferFastrax(CCameraCommandRecord* pCCommandR);

// data
private:
	CDWordArray			m_secIDs;
	CDWordArray			m_camIDs;
	CDWordArray			m_Flags;

	CameraControlType	m_type;
	CRS232*				m_pCom;

	CWinThread*			m_pRepeatThread;
	BYTE				m_CommandBuffer[_MAX_CAMERACOMMAND];
	int					m_iBufferLength;
	volatile BOOL		m_bRun;
	volatile BOOL		m_bRepeat;
	CCriticalSection	m_csRepeat;
	int					m_iRepeatTime;
	CCameraCommandRecord*m_pRepeatCmd;

	CCriticalSection	m_csMaxCom;
	CByteArray			m_byaVCLMaxCom;
	BYTE				m_byMaxComLastCameraAddress;

	static	UINT RepeatProc(LPVOID pParam);
};
/////////////////////////////////////////////////////////////////////////////
inline CameraControlType CCameraControlRecord::GetType() const
{
	return m_type;
}
/////////////////////////////////////////////////////////////////////////////
inline 	CRS232* CCameraControlRecord::GetRS232()
{
	return m_pCom;
}
/////////////////////////////////////////////////////////////////////////////
WK_PTR_ARRAY(CCameraControlRecordArray,CCameraControlRecord*)
/////////////////////////////////////////////////////////////////////////////
#endif