/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCFetchResult.h $
// ARCHIVE:		$Archive: /Project/CIPC/CIPCFetchResult.h $
// CHECKIN:		$Date: 23.03.04 15:15 $
// VERSION:		$Revision: 9 $
// LAST CHANGE:	$Modtime: 22.03.04 12:46 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$

#ifndef _CIPCFetchResult_H
#define _CIPCFetchResult_H

#include "SecID.h"
#include "Cipc.h"

class AFX_EXT_CLASS CIPCFetchResult 
{
	// construction/destruction
public:
	CIPCFetchResult();
	CIPCFetchResult(const CString &sShmName, 
					CIPCError cipcError, 
					int iErrorCode,
					const CString& sErrorOrVersion,
					DWORD dwOptions,
					CSecID assignedID
#ifdef _UNICODE
					, WORD wCodePage
#endif
					);
	CIPCFetchResult(const CIPCExtraMemory &bubble);

	// attributes
public:
	// result data:
	BOOL			IsOkay()	const;
	CIPCError		GetError() const;
	int				GetErrorCode() const;
	const CString&	GetErrorMessage() const;
	const CString&	GetServerVersion() const;
	const CString&	GetBitrate() const;

	// connection data:
	const CString&	GetShmName() const;
	CSecID			GetAssignedID() const;
	DWORD			GetOptions() const;
#ifdef _UNICODE
	WORD GetCodePage();
#endif

	// operations
public:
	void			 SetOptions(DWORD dwTicks);
	CIPCExtraMemory* CreateBubble(CIPC *pCipc);

	// implementation
private:
	void Init();
	DWORD GetMinBubbleSize();

	// data member
private:
	CString   m_sShmName;
	DWORD     m_dwOptions;
	WORD      m_wCodePage;
	WORD      m_wReserved;
	//
	CIPCError m_error;
	int       m_iErrorCode;
	CString   m_sErrorOrVersion;
	CSecID    m_assignedID;
};


#endif
