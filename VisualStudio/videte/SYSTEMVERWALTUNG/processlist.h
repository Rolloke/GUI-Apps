/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: processlist.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/processlist.h $
// CHECKIN:		$Date: 16.02.06 15:36 $
// VERSION:		$Revision: 31 $
// LAST CHANGE:	$Modtime: 16.02.06 13:56 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _PROZESS_LIST_H
#define _PROZESS_LIST_H

#include "globals.h"

class CProcessList;
//////////////////////////////////////////////////////////////////////////////
class CProcess                                             
{
public:
	enum ProcessType
	{
		P_NOTYPE,
		P_SAVE,
		P_CALL,
		P_BACKUP,
		P_ACTUAL_IMAGE,
		P_IMAGE_CALL,
		P_CHECK_CALL,
		P_GUARD_TOUR,
		P_VIDEO_OUT,
		P_PTZ_PRESET,
		P_ALARM_LIST,
		P_UPLOAD,
	};

public:
	enum VOutSubType
	{
		VOut_NoSubType = -1,
		VOut_Client = 0,
		VOut_Sequencer = 1,
		VOut_Activation = 2
	};

	// construction/destruction
public:
	CProcess();
	~CProcess();
	
protected:
	CProcess(CSecID id);

	// attributes
public:
	static CString		VOutSubType2String(VOutSubType type);

	inline const DWORD	  GetID() const;
	inline const CString& GetName() const;
	inline const ProcessType GetType() const;

	inline const BOOL     IsCall() const;
	inline const BOOL     IsSave() const;
	inline const BOOL     IsBackup() const;
	inline const BOOL     IsActualImage() const;
	inline const BOOL     IsImageCall() const;
	inline const BOOL     IsCheckCall() const;
	inline const BOOL     IsGuardTour() const;
	inline const BOOL     IsVideoOut() const;
	inline const BOOL     IsPTZPreset() const;
	inline const BOOL     IsAlarmList() const;
	inline const BOOL	  IsUpload() const;

	inline const BOOL     IsHold() const;
	inline const BOOL     IsInterval() const;
	inline const DWORD    GetDuration() const;
	inline const DWORD    GetPause() const;
	inline const DWORD    GetCallTime() const;
	inline const DWORD    GetConnectionTime() const;
	inline const WORD     GetNrPict() const;
	inline const DWORD    GetRecordTime() const;
	inline const int	  ImageByType() const;
	inline const CSecID   GetPermission() const;
	inline const Resolution  GetResolution() const;
	inline const Compression GetCompression() const;
	
	inline const int  GetNrOfCall() const;
	inline const WORD GetCall(int i) const;
				 BOOL HasCall(WORD wCall) const;
	inline const int  GetNrOfArchiv() const;
	inline const WORD GetArchiv(int i) const;

	inline const CSecID  GetBackupArchiv() const;
	inline const CString GetDestination() const;
	inline const BOOL    GetDeleteAtSuccess() const;
	inline const DWORD   GetBackupTimeSpan() const;
	inline const BOOL    GetSetTime() const;

	inline const BOOL	GetFullScreen() const;
	inline const BOOL	GetSave() const;
	inline const BOOL	GetExclusive() const;
	inline const BOOL	GetDisconnect() const;
	inline const DWORD	GetSwitchTime() const;

	inline const VOutSubType	GetVOutSubType() const;
	inline const CSecID			GetVOutGroupID() const;
	inline const DWORD			GetVOutOutput() const;
	inline const CString		GetCameras() const;

	inline const CString	GetServerName() const;
	inline const CString	GetLogin() const;
	inline const CString	GetPassword() const;
	inline const CString    GetFileName() const;

	// operations
public:
	BOOL Load(int i, CWK_Profile& wkp);
	BOOL Save(int i, CWK_Profile& wkp);
	void SetName(const CString& sName);
	inline void SetPermission(CSecID pID);
	void SetCall(const CString& sName,
				 DWORD dwDuration, 
				 DWORD dwPause,
				 DWORD dwCallTime,
				 BOOL  bHold,
				 BOOL  bInterval,
				 CSecID	idPermission,
				 DWORD dwConnectionTime);

	void SetSave(const CString& sName,
				 DWORD dwDuration,
				 DWORD dwPause,
				 WORD  wNrPict,
				 DWORD dwRecTime,
				 Resolution res,
				 Compression comp,
				 BOOL  bHold,
				 BOOL  bInterval);
	void SetBackup(const CString& sName, 
				   const CString& sDestination, 	
		           CSecID idArchive, 
				   BOOL bDeleteAtSuccess,
				   DWORD dwBackupTimeSpan);
	void SetActualImage(const CString& sName,Resolution res, Compression comp);
	void SetImageCall(const CString& sName,CSecID idPermission);
	void SetCheckCall(const CString& sName,CSecID idPermission,BOOL bSetTime);
	void SetHold(BOOL);
	void SetExclusive(BOOL);
	void SetDisconnect(BOOL);
	void SetSave(BOOL);
	void SetFullscreen(BOOL);
	void SetSwitchTime(DWORD);
	void SetCallTime(DWORD);
	void SetGuardTour(const CString&		sName,
					  const CSecID&			idPermission,
					  const DWORD&			dwTries,
					  const DWORD&			dwSwitchTime,
					  const BOOL&			bHold,
					  const BOOL&			bSave,
					  const BOOL&			bFullScreen,
					  const BOOL&			bExclusive,
					  const BOOL&			bDisconnect,
					  const CWordArray&		waHosts,
					  const CStringArray&	saCameras);
	BOOL GetGuardTourData(CString&		sName,
						  CSecID&		idPermission,
						  DWORD&		dwTries,
						  DWORD&		dwSwitchTime,
						  BOOL&			bHold,
						  BOOL&			bSave,
						  BOOL&			bFullScreen,
						  BOOL&			bExclusive,
						  BOOL&			bDisconnect,
						  CWordArray&	waHosts,
						  CStringArray&	saCameras);
	void SetVideoOut(const CString&			sName,
					 const VOutSubType&		subType,
					 const CSecID&			idGroupCard,
					 const DWORD&			dwOutput,
					 const CString&			sCameras,
					 const DWORD&			dwDwellTime);
	BOOL GetVideoOut(CString&		sName,
					 VOutSubType&	subType,
					 CSecID&		idGroupCard,
					 DWORD&			dwOutput,
					 CString&		sCameras,
					 DWORD&			dwDwellTime);

	BOOL SetUpload(const CString& sName,
					DWORD dwDuration,
					DWORD dwRecTime,
					Resolution res,
					Compression comp,
					BOOL  bHold,
					const CString& sServerName,
					const CString& sLogin,
					const CString& sPassword,
					const CString& sFilename);

	void ClearCalls();
	void ClearCall(WORD wCall);
	void AddCall(WORD wCall);
	void ClearArchivs();
	void AddArchiv(WORD wArc);

	void GetCallArray(CWordArray& waHosts) const;
	void GetCameraArray(CStringArray& saCameras) const;

	// implementation
private:
	void Init();
	static ProcessType	String2Type(const CString& s);
	static CString		Type2String(ProcessType type);

private:
	CSecID		m_id;			// unique identifier
	CString		m_sName;		// visible name
	ProcessType m_Type;			// type

	Resolution	m_eResolution;	// Aufloesung ders Kamerabildes
	Compression	m_eCompression;	// Kompression der Bilddaten

	CSecID	m_idPermission;		// Permission for call process
	CSecID	m_ArchivID;			// backup archiv ID
	BOOL    m_bSetTime;
	
	DWORD	m_dwMainTime;		// Prozessdauer in Sek. 0 = unendlich
	DWORD	m_dwPauseTime;		// Pausenzeit in Sek.
	DWORD	m_dwRecordOrCallTime;		// Aufnahme/Anrufzeit
	DWORD	m_dwConnectionTime;		// Verbindungsdauer

	WORD	m_wNrPict;			// Anzahl Bilder
	BOOL	m_bHold;			// volle Zeitdauer/Bilder halten
	BOOL	m_bInterval;		// Intervallprozess

	CWordArray m_waCall;		// zu rufende Gegenstationen
	CWordArray m_waArchiv;		// zu speichernde Archive

	BOOL	m_bDeleteAtSuccess;  // delete the backup sequences by success
	CString m_sDestination;
	DWORD	m_dwBackupTimeSpan;	// Timespan

	DWORD			m_dwSwitchTime;	// guard tour switch time or video out dwell time
	BOOL			m_bFullScreen;	// guard tour in Fullscreen
	BOOL			m_bSave;		// guard tour save pictures
	BOOL			m_bExclusive;	// guard tour show camera exclusive
	BOOL			m_bDisconnect;
	CStringArray	m_saCameras;	// guard tour cameras for hosts, same order sa m_waCall!

	VOutSubType		m_eSubType;			// SubType bei VideoOut
	CSecID			m_idGroupCard;		// Group ID Output bei Video out
	DWORD			m_dwVOutOutput;		// Output bei VideoOut
	CString			m_sCameras;
	
	CString			m_sServerName;
	CString			m_sLogin;
	CString			m_sPassword;
	CString			m_sFilename;

	friend class CProcessList;
};        
//////////////////////////////////////////////////////////////////////////////
inline const DWORD CProcess::GetID() const
{
	return m_id.GetID();
}
/////////////////////////////////////////////////////////////////////
inline const CString& CProcess::GetName() const
{
	return m_sName;
}
//////////////////////////////////////////////////////////////////////////////
inline const CProcess::ProcessType CProcess::GetType() const
{
	return m_Type;
}
//////////////////////////////////////////////////////////////////////////////
inline const BOOL CProcess::IsCall() const
{
	return m_Type == P_CALL;
}
//////////////////////////////////////////////////////////////////////////////
inline const BOOL CProcess::IsSave() const
{
	return m_Type == P_SAVE;
}
//////////////////////////////////////////////////////////////////////////////
inline const BOOL CProcess::IsBackup() const
{
	return m_Type == P_BACKUP;
}
//////////////////////////////////////////////////////////////////////////////
inline const BOOL CProcess::IsPTZPreset() const
{
	return m_Type == P_PTZ_PRESET;
}
//////////////////////////////////////////////////////////////////////////////
inline const BOOL CProcess::IsAlarmList() const
{
	return m_Type == P_ALARM_LIST;
}
//////////////////////////////////////////////////////////////////////////////
inline const BOOL CProcess::IsActualImage() const
{
	return m_Type == P_ACTUAL_IMAGE;
}
//////////////////////////////////////////////////////////////////////////////
inline const BOOL CProcess::IsImageCall() const
{
	return m_Type == P_IMAGE_CALL;
}
//////////////////////////////////////////////////////////////////////////////
inline const BOOL CProcess::IsCheckCall() const
{
	return m_Type == P_CHECK_CALL;
}
//////////////////////////////////////////////////////////////////////////////
inline const BOOL CProcess::IsGuardTour() const
{
	return m_Type == P_GUARD_TOUR;
}
//////////////////////////////////////////////////////////////////////////////
inline const BOOL CProcess::IsVideoOut() const
{
	return m_Type == P_VIDEO_OUT;
}
//////////////////////////////////////////////////////////////////////////////
inline const BOOL CProcess::IsUpload() const
{
	return m_Type == P_UPLOAD;
}
//////////////////////////////////////////////////////////////////////////////
inline const BOOL CProcess::IsHold() const
{
	return m_bHold;
}
//////////////////////////////////////////////////////////////////////////////
inline const BOOL CProcess::IsInterval() const
{
	return m_bInterval;
}
//////////////////////////////////////////////////////////////////////////////
inline const Resolution CProcess::GetResolution() const
{
	return m_eResolution;
}
//////////////////////////////////////////////////////////////////////////////
inline const Compression CProcess::GetCompression() const
{
	return m_eCompression;
}
//////////////////////////////////////////////////////////////////////////////
inline const DWORD CProcess::GetDuration() const
{
	return m_dwMainTime;
}
//////////////////////////////////////////////////////////////////////////////
inline const DWORD CProcess::GetPause() const
{
	return m_dwPauseTime;
}
//////////////////////////////////////////////////////////////////////////////
inline const DWORD CProcess::GetCallTime() const
{
	return m_dwRecordOrCallTime;
}
//////////////////////////////////////////////////////////////////////////////
inline const DWORD CProcess::GetConnectionTime() const
{
	return m_dwConnectionTime;
}
//////////////////////////////////////////////////////////////////////////////
inline const DWORD CProcess::GetRecordTime() const
{
	return m_dwRecordOrCallTime;
}
//////////////////////////////////////////////////////////////////////////////
inline const WORD CProcess::GetNrPict() const
{
	return m_wNrPict;
}
//////////////////////////////////////////////////////////////////////////////
inline const int CProcess::ImageByType() const
{	
	int iImage = IMAGE_NULL;
	switch (m_Type) 
	{
		case P_SAVE:	
			iImage = IMAGE_SAVE;	
			break;
		case P_CALL:	
			iImage = IMAGE_CALL;	
			break;
		case P_BACKUP:	
			iImage = IMAGE_BACKUP;	
			break;
		case P_ACTUAL_IMAGE:	
			iImage = IMAGE_ACTUAL_IMAGE;	
			break;
		case P_IMAGE_CALL:	
			iImage = IMAGE_IMAGE_CALL;	
			break;
		case P_CHECK_CALL:	
			iImage = IMAGE_CHECK_CALL;	
			break;
		case P_GUARD_TOUR:
			iImage = IMAGE_GUARD_TOUR;
			break;
		case P_VIDEO_OUT:
			iImage = IMAGE_VIDEO_OUT;
			break;
		case P_UPLOAD:
			iImage = IMAGE_UPLOAD;
			break;
	}
	return iImage;
}
/////////////////////////////////////////////////////////////////////////////
inline const CSecID CProcess::GetPermission() const
{
	return m_idPermission;
}
/////////////////////////////////////////////////////////////////////////////
inline void CProcess::SetPermission(CSecID pID)
{
	m_idPermission = pID;
}
/////////////////////////////////////////////////////////////////////////////
inline const int  CProcess::GetNrOfCall() const
{
	return m_waCall.GetSize();
}
/////////////////////////////////////////////////////////////////////////////
inline const WORD CProcess::GetCall(int i) const
{
	return m_waCall.GetAt(i);
}
/////////////////////////////////////////////////////////////////////////////
inline const int CProcess::GetNrOfArchiv() const
{
	return m_waArchiv.GetSize();
}
/////////////////////////////////////////////////////////////////////////////
inline const WORD CProcess::GetArchiv(int i) const
{
	return m_waArchiv.GetAt(i);
}
/////////////////////////////////////////////////////////////////////////////
inline const CSecID CProcess::GetBackupArchiv() const
{
	return m_ArchivID;
}
/////////////////////////////////////////////////////////////////////////////
inline const BOOL CProcess::GetDeleteAtSuccess() const
{
	return m_bDeleteAtSuccess;
}
/////////////////////////////////////////////////////////////////////////////
inline const DWORD CProcess::GetBackupTimeSpan() const
{
	return m_dwBackupTimeSpan;
}
/////////////////////////////////////////////////////////////////////////////
inline const BOOL CProcess::GetSetTime() const
{
	return m_bSetTime;
}
/////////////////////////////////////////////////////////////////////////////
inline const BOOL CProcess::GetFullScreen() const
{
	return m_bFullScreen;
}
/////////////////////////////////////////////////////////////////////////////
inline const BOOL CProcess::GetSave() const
{
	return m_bSave;
}
/////////////////////////////////////////////////////////////////////////////
inline const BOOL CProcess::GetExclusive() const
{
	return m_bExclusive;
}
/////////////////////////////////////////////////////////////////////////////
inline const BOOL CProcess::GetDisconnect() const
{
	return m_bDisconnect;
}
/////////////////////////////////////////////////////////////////////////////
inline const DWORD CProcess::GetSwitchTime() const
{
	return m_dwSwitchTime;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString CProcess::GetDestination() const
{
	return m_sDestination;
}
/////////////////////////////////////////////////////////////////////////////
inline const CProcess::VOutSubType CProcess::GetVOutSubType() const
{
	return m_eSubType;
}
/////////////////////////////////////////////////////////////////////////////
inline const DWORD CProcess::GetVOutOutput() const
{
	return m_dwVOutOutput;
}
/////////////////////////////////////////////////////////////////////////////
inline const CSecID CProcess::GetVOutGroupID() const
{
	return m_idGroupCard;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString CProcess::GetCameras() const
{
	return m_sCameras;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString CProcess::GetServerName() const
{
	return m_sServerName;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString CProcess::GetLogin() const
{
	return m_sLogin;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString CProcess::GetPassword() const
{
	return m_sPassword;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString CProcess::GetFileName() const
{
	return m_sFilename;
}
/////////////////////////////////////////////////////////////////////////////
//***************************************************************************
/////////////////////////////////////////////////////////////////////////////
WK_PTR_ARRAY(CProzessPtrArray,CProcess*);
/////////////////////////////////////////////////////////////////////////////
class CProcessList : public CProzessPtrArray
{
public:
	CProcessList();
	virtual ~CProcessList();

	CProcess*	AddProcess();
	void		DeleteProcess(CProcess* pProcess);
	
	CProcess*	GetProcess(CSecID id) const;


	void	Reset();
	// Alle Prozesse in der Liste werden gelöscht.
	
	void Load(CWK_Profile& wkp);
	void Save(CWK_Profile& wkp);

};
								
#endif
