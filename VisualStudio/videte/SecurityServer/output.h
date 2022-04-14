/* GlobalReplace: Relais --> Relay */
/* GlobalReplace: m_bRelayState --> m_bRelayClosed */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: output.h $
// ARCHIVE:		$Archive: /Project/SecurityServer/output.h $
// CHECKIN:		$Date: 25.08.06 22:06 $
// VERSION:		$Revision: 34 $
// LAST CHANGE:	$Modtime: 25.08.06 22:01 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef __OUTPUT_H_
#define __OUTPUT_H_

#include "SecID.h"
#include "CIPCOutput.h"
#include "PictureDef.h"

class CIPCOutputServer;

#define PTZ_INTERFACE_HTTP			33
#define PTZ_INTERFACE_AXIS_RS232	34
#define PTZ_INTERFACE_AXIS_RS485	35
#define PTZ_INTERFACE_UDP_RS485		36

enum RelayControlType {
	RCT_INVALID=0,
	RCT_EDGE=1,
	RCT_STATE=2
};

class CJpegStatistics : public CStatistics
{
	// construction / destruction
public:
	CJpegStatistics();
	virtual ~CJpegStatistics();

	// attributes
public:
	int GetJPEGQuality() const;

	// operations
public:
	void SetJPEGQuality(int iJPEGQuality);

private:
	int	m_iJPEGQuality;
};

class COutput
{           
	// construction / destruction
public:                   
	COutput(CIPCOutputServer* pParent,WORD subID,SecOutputType type);
	virtual ~COutput();

	// attributes:
public:
	inline SecOutputType GetOutputType() const;
	inline const CString &GetName() const;

	inline CSecID GetID() const;
	inline CSecID GetCurrentArchiveID() const;

	inline BOOL IsFixed() const;
	inline BOOL IsReference() const;
	inline BOOL IsTerminated() const;
	inline CameraControlCmd GetPTZCmd() const;
	inline int GetPTZInterface() const;
	//
	inline RelayControlType  GetRelayControlType() const;
	inline DWORD GetRelayClosingTime() const;
	inline BOOL ClientMayKillRelayProcess() const;
	inline BOOL KeepClosedAfterDisconnect() const;

	int  GetJPEGQuality(Compression comp, Resolution res);
	void GetMDCoordinates(DWORD& dwX,DWORD& dwY);

	inline CIPCOutputServer* GetGroup();

	
	// operations
public:
	void SetFixed(BOOL bFixed);
	void SetReference(BOOL bReference);
	void SetTermination(BOOL bTerminated);
	void SetPTZ(CameraControlCmd ccc, int iPTZInterface);
	void ReadRelayAttributesFromRegistryString(const CString &s);
	void AddCompressedJPEGSize(DWORD dwSize,Compression comp, Resolution res);
	void SetCurrentArchiveID(CSecID id);
	void SetMDCoordinates(DWORD dwX,DWORD dwY);

public:	// public data:
	CString		m_sName;					// Kommentare für den Ausgang.
	BOOL		m_bRelayClosed;		// OOPS not fully used/reset

private:
	CIPCOutputServer* m_pParent;
	SecOutputType m_outputType;	// Typ des Ausgangs OTYP_RELAY, OTYP_CAMERA  
	CSecID	m_id;
	BOOL	m_bIsFixed;	// for cameras
	BOOL    m_bReference;
	BOOL	m_bTermination;
	CameraControlCmd	m_ePTZCmd;
	int		m_iPTZInterface;

	// relais attributes
	DWORD	m_dwRelayClosingTime;
	RelayControlType m_clientRelayControl;
	BOOL	m_bClientMayKillRelayProcess;
	BOOL	m_bKeepRelayClosedAfterClientDisconnect;

	CJpegStatistics* m_pJPEGStatistics;
	CSecID  m_idCurrentArchive;
	DWORD   m_dwX;
	DWORD   m_dwY;
};
////////////////////////////////////////////////////////////////////
#include "Output.inl"

#endif
