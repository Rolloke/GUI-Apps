/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: Server.h $
// ARCHIVE:		$Archive: /Project/Clients/Recherche/Server.h $
// CHECKIN:		$Date: 3.03.03 11:04 $
// VERSION:		$Revision: 44 $
// LAST CHANGE:	$Modtime: 3.03.03 10:12 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#if !defined(AFX_SERVER_H__AFB9AF41_2139_11D2_8C1B_00C095EC9EFA__INCLUDED_)
#define AFX_SERVER_H__AFB9AF41_2139_11D2_8C1B_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CIPCDatabaseRecherche.h"
#include "IPCDataCarrierRecherche.h"
#include "QueryParameter.h"
#include "QueryResult.h"

class CRechercheDoc;
class CServers;
class CDataBaseInfoDlg;
class CConnectingDialog;

#define SERVERID_NOID 0xFFFF


class CServer  
{
	// construction / destruction
public:
	CServer(CRechercheDoc* pDoc, CServers* pServers, WORD wID);
	virtual ~CServer();

	// attributes
public:
	inline WORD				GetID() const;
	inline CSecID			GetHostID() const;
	inline const CString&	GetName() const;
				 CString	GetFullName() const;
	inline const CString&	GetKind() const;
	
	inline BOOL	IsDV() const;
	inline BOOL CanRectangleQuery() const;
	inline BOOL CanSplittingBackup() const;
	inline BOOL	IsLocal() const; 
		   BOOL	IsConnected() const;
		   BOOL	IsDataCarrierReady() const;
		   BOOL	IsDisconnected() const;
	inline BOOL	IsSearching() const;
	
	inline int GetMajorVersion() const;
	inline int GetMinorVersion() const;
	inline int GetSubVersion() const;
	inline int GetBuildNr() const;

	inline const CString&	GetError() const;

	inline CIPCDatabaseRecherche* GetDatabase() const;
	inline CIPCDataCarrierRecherche* GetDataCarrier() const;
	inline CRechercheDoc* GetDocument() const;
	inline CDataBaseInfoDlg* GetInfoDialog() const;

	CString	GetArchivName(WORD wArchiv);
	CString GetFieldName(const CString& sField);

	inline const CIPCFields& GetFields() const;
	inline CQueryArchives& GetQueryArchives();
	inline CIPCDrives& GetDrives();

	inline BOOL	IsInBackupMode() const;

	// operations
public:
	inline  void SetDV();
			void SetVersion(const CString& sVersion);
	void	OnIdle();
	BOOL	Connect(const CString& sName,
					const CString& sNumber,
					CSecID idHost);
	BOOL	Disconnect();
	void    ResetConnectionAutoLogout();
	void	CancelConnecting();

	void    OnConfirmFieldInfo(int iNumRecords, const CIPCField data[]);
	void	AddNewArchiv(const CIPCArchivRecord& data);
	void	UpdateArchiv(const CIPCArchivRecord& data);
	void	AddArchivFile(const CIPCArchivFileRecord& data);
	void	AddNewSequence(const CIPCSequenceRecord& seq);
	void	AddRemoved(WORD wArchiv, WORD wSequence);

	void	OpenSequence(WORD wArchiv, WORD wSequence, DWORD dwRecordNr = 1);
	void	DeleteArchivFiles();
	void	AddDeleteArchivFile(WORD wArchiv, WORD wSequence);
	void	AddToOpenFile(const CIPCArchivFileRecord& data);

	void    InitFields3x();

	void	RequestRecord(WORD wArchiv, 
						  WORD wSequence, 
						  DWORD dwCurrentRecord,
						  DWORD dwNewRecord,
						  DWORD dwCamID);

	void	StartSearch();
	void	StopSearch();
	void    ClearSearch();
	void	StopSearchCopy();

	void	Lock();
	void	Unlock();

	void	ShowInfoDialog();


	CIPCArchivFileRecord* GetCIPCArchivFileRecord(WORD wArchiv,WORD wSequence);
	CIPCArchivRecord*	  GetCIPCArchivRecord(WORD wArchiv);

	CIPCSequenceRecord*   GetCIPCSequenceRecord(WORD wArchiv,WORD wSequenceNr);
	CIPCSequenceRecord*   GetNextCIPCSequenceRecord(const CIPCSequenceRecord& s);
	CIPCSequenceRecord*   GetPrevCIPCSequenceRecord(const CIPCSequenceRecord& s);
	CIPCSequenceRecord*   GetFirstSequenceRecord(WORD wArchiveNr);
	CIPCSequenceRecord*   GetLastSequenceRecord(WORD wArchiveNr);

	void OnConfirmDrives(int iNumDrives, const CIPCDrive drives[]);
	void AddErrorMessage(const CString& sMessage);

	void AddQueryResult(DWORD dwUserID, WORD wArchivNr,WORD wSequenceNr, DWORD dwRecordNr,
						int iNumFields, const CIPCField fields[]);
	BOOL IsQueryResult(WORD wArchivNr,WORD wSequenceNr, DWORD dwRecordNr);
	void CopyNextQueryResult(WORD wArchivNr,WORD wSequenceNr, DWORD dwRecordNr);

	void SetBackupMode(BOOL bBackup);
	inline void	SetDataCarrierReady(BOOL bReady);

	// implementation
protected:
	static	UINT	ConnectThreadProc(LPVOID pParam);
	static	CString	GetErrorMessage(const CIPCFetchResult& fr);

	void	ConnectDatabase();
	BOOL	IsDataCarrierConnected() const;
	void	TryToConnectDataCarrier();
	void	DoCopyQueryResult(CQueryResult* pQueryResult,WORD wArchivNr,WORD wSequenceNr, DWORD dwRecordNr);

	// data member
private:
	DWORD				m_dwQueryID;
	WORD				m_wID;		// unique id in Vision
	CSecID				m_idHost;	// unique Host id System wide
	CString				m_sName;	// Name of Host
	CString				m_sKind;	// ISDN / Net / B3
	BOOL				m_bLocal;	// local or remote
	BOOL				m_bDV;	// local or remote
	int m_iMajorVersion;
	int m_iMinorVersion;
	int m_iSubVersion;
	int m_iBuildNr;

	CRechercheDoc*		m_pDoc;			// application single doc ref
	CServers*			m_pServers;		// server array ref
	CAutoLogout			m_AutoLogout;	// my auto disconnect

	CIPCDatabaseRecherche*	m_pDatabase; // the database connection

	CCriticalSection	m_csSection;
	CString				m_sConnectionError;

	CConnectingDialog*	m_pConnectDialog;		// to cancel connect thread
	CIPCFetch			m_Fetch;
	CConnectionRecord*  m_pConnectionRecord;	// for connect thread
	CIPCFetchResult*	m_pDatabaseResult;
	CWinThread*			m_pConnectThread;
	BOOL				m_bConnectingCancelled;

	CIPCFields				m_Fields;
	CDataBaseInfoDlg*		m_pDataBaseInfoDlg;

	CIPCSequenceRecords		m_srSequences;
	CIPCSequenceRecords		m_srNewSequences;

	CIPCArchivFileRecords	m_arNewFiles;
	CIPCArchivFileRecords	m_arFiles;
	CIPCArchivFileRecords	m_arToOpenFiles;
	
	CDWordArray				m_arToDelete;
	CDWordArray				m_arRemoved;

	CIPCArchivRecordArray	m_arNewArchivs;
	CIPCArchivRecordArray	m_arUpdatedArchivs;

	CQueryArchives			m_QueryArchives;
	BOOL					m_bSearching;
	CQueryResultArrayCS		m_QueryResultsToCopy;
	BOOL					m_bFirstQueryResult;
	WORD					m_wQueryArchiveNr;
	WORD					m_wQuerySequenceNr;
	DWORD					m_dwQueryRecordNr;
	BOOL					m_bStopSearchCopy;

	CIPCDrives				m_Drives;

	CStringArray			m_saErrorMessages;
	
	BOOL					m_bBackupMode;

	static int				m_iSerialCounter;

	CIPCDataCarrierRecherche*	m_pDataCarrier; // the data carrier connection (backup)
	DWORD						m_dwLastConnect;
	BOOL						m_bDataCarrierReady;

	friend class CDataBaseInfoDlg;
	friend class CConnectingDialog;
};
#include "server.inl"
#endif // !defined(AFX_SERVER_H__AFB9AF41_2139_11D2_8C1B_00C095EC9EFA__INCLUDED_)
