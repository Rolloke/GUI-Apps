// SocketUnitDoc.h : interface of the CSocketUnitDoc class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CSocketUnitDoc_H
#define _CSocketUnitDoc_H

class CControlSocket;	// forward declaration
class CIPCPipeSocket;
class CServerControlSocket;
class CDataSocket;


#define VIDETE_CIPC_PORT    1927
#define VIDETE_OLD_PORT		888


#include "CipcPipeSocket.h"
#include "ConnectionThread.h"

class CHostTableDialog;

class CSocketUnitDoc : public CDocument
{
protected: // create from serialization only
	CSocketUnitDoc();
	DECLARE_DYNCREATE(CSocketUnitDoc)

// Attributes
public:
	inline CConnectionThreads& GetConnectionThreads();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSocketUnitDoc)
	public:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSocketUnitDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSocketUnitDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

		//
public:
	void ProcessPendingAccept(CControlSocket *pControl);
	void RemoveMe(CIPCPipeSocket *pPipe);
	virtual BOOL SaveModified( );	// MFC 
	void CloseAllConnections();
	void CheckConnections();


	void AddNewThread(CConnectionThread *pThread);
	void RemoveThread(CConnectionThread *pThread);

	void StopAllThreads();

	CServerControlSocket *m_pCipcControl;

	CIPCPipesArray m_ServerPipes;
	CIPCPipesArray m_ClientPipes;

	virtual void PreCloseFrame( CFrameWnd* pFrame );

private:
	BOOL ScanHostList(CDataSocket* pSocket);

private:
	// data:
	CControlSocket *m_pControlSocket;
	CConnectionThreads m_threads;
	CCriticalSection m_csPipes;
	CIPCPipesArray m_RemovedPipes;
	//
	DWORD m_dwLastReconnectTry;

};
inline CConnectionThreads& CSocketUnitDoc::GetConnectionThreads()
{
	return m_threads;
}


#endif

