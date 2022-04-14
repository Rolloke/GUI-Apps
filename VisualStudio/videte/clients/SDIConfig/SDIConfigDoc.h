/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIConfigDoc.h $
// ARCHIVE:		$Archive: /Project/Clients/SDIConfig/SDIConfigDoc.h $
// CHECKIN:		$Date: 15.02.99 16:08 $
// VERSION:		$Revision: 16 $
// LAST CHANGE:	$Modtime: 15.02.99 16:06 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_SDICONFIGDOC_H__3716C0F0_5B7E_11D1_8143_EAF88A4ACC28__INCLUDED_)
#define AFX_SDICONFIGDOC_H__3716C0F0_5B7E_11D1_8143_EAF88A4ACC28__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
#include "wkclasses\SDIControl.h"

/////////////////////////////////////////////////////////////////////////////
class CSDIConfigView;
class CSDIConfigInput;

/////////////////////////////////////////////////////////////////////////////
class CSDIConfigDoc : public CDocument
{
// Construction
protected: // create from serialization only
	DECLARE_DYNCREATE(CSDIConfigDoc)
	CSDIConfigDoc();

// Attributes
public:
			CSDIConfigView*	GetView();
	virtual inline BOOL		WasExternProgramFound();
	inline	BOOL			IsConnecting();
	inline	BOOL			IsConnected();
	inline	BOOL			IsTransfering();
	inline	BOOL			IsConfiguring();
	inline	DWORD			GetFileSize();
	inline	DWORD			GetByteRate();
			void			GetConfigState(CString& sReturn);
			BOOL			IsExternBusy();


// Operations
public:
	BOOL	ResetConnecting(BOOL bSelf);
	BOOL	ResetConnected(BOOL bOK);
	BOOL	ResetTransfering(BOOL bOK);
	BOOL	ResetConfiguring(BOOL bOK);
	void	StartExternProgram();
	BOOL	SelectHosts();
	void	StartConnection(BOOL bLocal);
	void	OnConnection(CIPCFetchResult& FetchResult);
	void	EndConnection(BOOL bSelf);
	void	ControlWasDeleted(const CSecID id);
	void	AddStatusLine(const CString& sLine);
	void	AddStatusLine(UINT uID);
	void	ClearStatusText();

// Overrides
public:
	virtual	BOOL	IsControlRecordOk(SDIControlType ControlType);
	virtual void	OnIdle();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDIConfigDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSDIConfigDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void	ClearDoc();
	BOOL	TransferFile(CString& sSourcePath, CString& sDestPath);
	BOOL	GetFile(CString& sSourcePath, CString& sDestPath);
	void	AddStatusLineIntern(const CString& sLine);

// Generated message map functions
protected:
	//{{AFX_MSG(CSDIConfigDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Data
public:
protected:
	BOOL				m_bConnecting;		// Wird eine Verbindung hergestellt
	BOOL				m_bConnected;		// Besteht eine Verbindung
	BOOL				m_bTransfering;		// Wird eine Datei uebertragen
	BOOL				m_bConfiguring;		// Wird eine Konfiguration durchgefuehrt
	CString				m_sExecutableDir;	// Verzeichnis fuer das externe Programm
	CString				m_sExecutableFile;	// Das externe Programm
	CString				m_sConfigDir;		// Verzeichnis für die Konfigurationsdatei
	CString				m_sConfigFile;		// Konfigurationsdatei
	CString				m_sHost;			// Name der Gegenstation
	CString				m_sNumber;			// Nummer der Gegenstation
	CSDIConfigInput*	m_pInput;			// CIPCInput zum Server
	CSDIConfigInput*	m_pInputToDelete;	// Alter CIPCInput zum Server, ist zu loeschen
	CSecID				m_idConfigurate;	// ID des aktuell zu konfigurierenden Controls
	CCriticalSection	m_csConfigState;	// CS, um Zugriffe von Input und iew zu entkoppeln
	CString				m_sConfigState;		// Hier wird der aktuelle Status angehaengt
	HANDLE				m_hExternProcess;	// Handle des externen Konfigurationsprogrammes
	DWORD				m_dwFileSize;
	DWORD				m_dwByteRate;
};
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSDIConfigDoc::WasExternProgramFound()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSDIConfigDoc::IsConnecting()
{
	return m_bConnecting;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSDIConfigDoc::IsConnected()
{
	return m_bConnected;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSDIConfigDoc::IsTransfering()
{
	return m_bTransfering;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSDIConfigDoc::IsConfiguring()
{
	return m_bConfiguring;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CSDIConfigDoc::GetFileSize()
{
	return m_dwFileSize;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CSDIConfigDoc::GetByteRate()
{
	return m_dwByteRate;
}
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDICONFIGDOC_H__3716C0F0_5B7E_11D1_8143_EAF88A4ACC28__INCLUDED_)
