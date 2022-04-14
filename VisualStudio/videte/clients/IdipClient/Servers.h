// Servers.h: interface for the CServers class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERS_H__BC7F5CC3_6BAE_11D1_93E4_00C095EC9EFA__INCLUDED_)
#define AFX_SERVERS_H__BC7F5CC3_6BAE_11D1_93E4_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Server.h"

#define INVALID_TIME_OFFSET	 MIN_S16

/////////////////////////////////////////////////////////////////////////////
typedef CServer* CServerPtr;
WK_PTR_ARRAY_CS(CServerArray,CServerPtr,CServerArrayCS);
/////////////////////////////////////////////////////////////////////////////
class CIdipClientDoc;
/////////////////////////////////////////////////////////////////////////////
class CServers : public CServerArrayCS
{
	// construction / destruction
public:
	CServers();
	virtual ~CServers();

	// attributes
public:
	CServer*	GetLocalServer();
	CServer*	GetServer(WORD wID);
	CServer*	GetServer(CSecID idHost);
	CServer*	GetServer(const CString& sName);
	CServer*	GetAlarmServer(const CString& sName);
	int			GetNrOfRemoteServers();
	CServer*	GetRemoteServer();
	BOOL		IsAnyB3Connection();
	BOOL		IsAnyServerFetching();
	BOOL		IsSearching() const;
	
	// operations
public:
	void		OnIdle();
	CServer*	AddServer();

	void		Disconnect(WORD wServerID);
	void		ResetConnectionAutoLogout();
	void		PopupDisconnectMenu(CPoint pt);
	void		Trace();
	void		Reset();
	void		ResetPictureTimeOffsets();
	void		UpdateTimeOffsets();
	void		CheckDatabaseConnections();

	inline void SetDocument(CIdipClientDoc* pDoc);

	// data member
protected:
	CIdipClientDoc* m_pDoc;
};
/////////////////////////////////////////////////////////////////////////////
inline void CServers::SetDocument(CIdipClientDoc* pDoc)
{
	m_pDoc = pDoc;
}

#endif // !defined(AFX_SERVERS_H__BC7F5CC3_6BAE_11D1_93E4_00C095EC9EFA__INCLUDED_)
