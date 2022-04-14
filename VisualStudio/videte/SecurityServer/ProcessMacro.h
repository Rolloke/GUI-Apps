// FILE: $Workfile: ProcessMacro.h $ from $Archive: /Project/SecurityServer/ProcessMacro.h $
// VERSION: $Date: 1.03.06 16:32 $ $Revision: 50 $
// LAST:	$Modtime: 1.03.06 13:58 $ by $Author: Uwe.freiwald $
//$Nokeywords:$

#ifndef _CProcessMacro_H
#define _CProcessMacro_H

class CPermission;

// it's a DWORD
#define CPM_TIME_INFINITY 0xffffffff
#define P_MAX_ARC		  32

enum ProcessMacroType {
	PMT_INVALID,
	PMT_RECORD,
	PMT_ALARM_CALL,
	PMT_RELAY,
	PMT_BACKUP,
	PMT_ACTUAL_IMAGE,
	PMT_IMAGE_CALL,
	PMT_CHECK_CALL,
	PMT_GUARD_TOUR,
	PMT_VIDEO_OUT,
	PMT_AUDIO,
	PMT_PTZ_PRESET,
	PMT_ALARM_LIST,
	PMT_UPLOAD,
};
LPCTSTR NameOfEnum(ProcessMacroType type);

const int iNumMaxArchivesPerProcessMacro=15;
const int NUM_PRIORITIES=5;	// stored with 1..5 internal 0..4

class CProcessMacro 
{
	// construction/destruction
public:
	CProcessMacro(ProcessMacroType t, 
			      const CString& sComment,
				  CSecID id);
	virtual ~CProcessMacro();

	// attributes
public:
	BOOL IsValidMacro() const;	// CAVEAT prints TRACE messages
	//
	CString CreateDumpString() const;	// creates a string representation of the macro
	//
	inline CSecID GetID() const;
	inline const CString& GetName() const;
	inline Resolution GetResolution() const;
	inline Compression GetCompression() const;
	inline ProcessMacroType GetType() const;
	//
	inline const CWordArray& GetIDs() const;	
	inline const CStringArray& GetCameras() const;
	inline const BYTE *GetArchiveIDs() const;	// format for SavePicture
	// record values:
	inline DWORD GetRecordTime() const;
	inline DWORD GetNumPictures() const;
	inline DWORD GetRecordSlice() const;
	inline DWORD GetPicsPerSlice() const;
	//
	inline BOOL  IsIntervall() const;
	inline BOOL  IsKeepAlive() const;
	inline DWORD GetOverallTime() const;
	inline DWORD GetPauseTime() const;
	inline DWORD GetNumExpectedPictures() const;
	const  CPermission* GetPermission() const;
	inline DWORD GetConnectionTime() const;

	inline CSecID  GetBackupArchiveID() const;
	inline CString GetBackupDestination() const;
	inline BOOL	   GetBackupDeleteAtSuccess() const;
	inline DWORD   GetBackupTimeSpan() const;
	inline BOOL	   GetSetTime() const;

	// Guard tour settings
	inline DWORD   GetDwellTime() const;
	inline DWORD   GetNrOfTries() const;
	inline BOOL	   GetGuardTourSave() const;
	inline BOOL	   GetGuardTourFullScreen() const;
	inline BOOL	   GetGuardTourExclusiveCam() const;
	inline BOOL	   GetGuardTourDisconnect() const;

	// Video out settings;
	inline int	   GetVideoOutType() const;	   
	inline int	   GetVideoOutPort() const;	   
	inline WORD	   GetVideoOutGroupID() const;

	// Upload
	inline CString GetServerName() const;
	inline CString GetLogin() const;
	inline CString GetPassword() const;
	inline CString GetFilename() const;

	// operations
public:
	void SetPauseTime(DWORD dwTime);
	void SetOverallTime(DWORD dwTime);
	void SetConnectionTime(DWORD dwTime);
	void SetRecordValues(DWORD dwRecordTimeInMS, DWORD dwNumPictures);
	void SetIDs(const CWordArray & ids);
	void SetIsIntervall(BOOL bIntervall);
	void SetKeepAlive(BOOL bKeepAlive);
	//
	void SetPictureParameter(Compression comp, Resolution res);
	void SetResolution(Resolution res);
	//
	void SetPermission(const CPermission *pPermission);
	//
	void SetBackup(CSecID idBackupArchive, const CString& sDestination,
				   BOOL bDeleteAtSuccess, DWORD dwBackupTimespan);
	//
	void SetSetTime(BOOL bSetTime);
	// guard tour set
	void SetGuardTourValues(DWORD dwDwellTime,
							DWORD dwNrOfTries,
							BOOL bFullscreen,
							BOOL bExclusive,
							BOOL bSave,
							BOOL bDisconnect,
							const CWordArray& idHosts,
							const CStringArray& sCameras);
	void SetVideoOutValues(int iVideoPort,
						   int iSubtype,
						   DWORD dwDwellTime,
						   WORD wGroupID,
						   const CString& sCameras);
	void SetUploadValues(const CString& sServerName,
						 const CString& sLogin,
						 const CString& sPassword,
						 const CString& sFilename);

	static ProcessMacroType TypeFromString(const CString &s);

private:
	// general
	CSecID			 m_ID;
	ProcessMacroType m_Type;
	CString			 m_sComment;

	// picture parameter
	Compression	m_Compression;
	Resolution	m_Resolution;
	//
	BOOL	m_bKeepAlive;
	BOOL	m_bIsIntervall;

	DWORD	m_dwOverallTime;	// can be infinity
	DWORD	m_dwPauseTime;
	// record values:
	DWORD	m_dwRecordTime;
	DWORD	m_dwNumPictures;
	DWORD	m_dwRecordSlice;
	DWORD	m_dwPicsPerSlice;
	DWORD	m_dwConnectionTime;
	//
	CWordArray m_IDs;
	BYTE *m_pArchiveIDsAsBytes;
	//
	const CPermission *m_pPermission;
	//
	DWORD m_dwNumExpectedPictures;
	
	// backup process
	CSecID	m_ArchivID;			// backup archiv ID
	BOOL	m_bDeleteAtSuccess;  // delete the backup sequences by success
	CString m_sDestination;
	DWORD   m_dwBackupTimeSpan;

	// check call
	BOOL    m_bSetTime;

	// guard tour
	BOOL	m_bGTFullscreen;
	BOOL	m_bGTExclusive;
	BOOL	m_bGTSave;
	BOOL	m_bGTDisconnect;
	CStringArray m_sGTCameras;

	// video out
	int		m_iVideoOutSubtype;
	int		m_iVideoOutPort;
	WORD	m_wGroupID;

	// upload
	CString m_sServerName;
	CString m_sLogin;
	CString m_sPassword;
	CString m_sFilename;
};
#include "ProcessMacro.inl"
//////////////////////////////////////////////////////////////////////////////
typedef CProcessMacro * CProcessMacroPtr;
WK_PTR_ARRAY_CS(CProcessMacroArrayPlain,CProcessMacroPtr,CProcessMacroArrayCS)
//////////////////////////////////////////////////////////////////////////////
class CProcessMacros : public CProcessMacroArrayCS
{
public:
	CProcessMacros();
	// attributes
public:
	const CProcessMacro * GetMacroByID(CSecID id) const;
	
	// operations
public:
	void Load(CPermissionArray& permissions);
	void AddDefaultMacros();

};
#endif
