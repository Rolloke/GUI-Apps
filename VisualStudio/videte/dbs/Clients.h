// Clients.h: interface for the CClients class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENTS_H__AB5B237A_3834_11D2_B58E_00C095EC9EFA__INCLUDED_)
#define AFX_CLIENTS_H__AB5B237A_3834_11D2_B58E_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Client.h"

class CSequence;
/////////////////////////////////////////////////////////////////////////////
typedef CClient* CClientPtr;
WK_PTR_ARRAY_CS(CClientArray,CClientPtr,CClientArrayCS)
/////////////////////////////////////////////////////////////////////////////
class CClients : public CClientArrayCS
{
	// construction / destruction
public:
	CClients();
	virtual ~CClients();

	// attributes
public:
	CClient* GetClient(DWORD dwID);

	// operations
public:
	CClient* AddClient();
	void	 OnIdle();

	// indications
public:
	void	DoIndicateDeleteSequence(const CSequence& seq);
	void	DoIndicateNewSequence(const CSequence& seq,
								  WORD  wPrevSequenceNr,
								  DWORD dwNrOfRecords);

	void	DoIndicateNewArchiv(CArchiv& arc);
	void	DoIndicateDeleteArchiv(WORD wArchivNr);
	void	DoIndicateDrives();
	void	DoIndicateNewArchivName(DWORD dwClientID, CArchiv& a);
};

#endif // !defined(AFX_CLIENTS_H__AB5B237A_3834_11D2_B58E_00C095EC9EFA__INCLUDED_)
