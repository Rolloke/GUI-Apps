// QueryResult.cpp: implementation of the CQueryResult class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IdipClient.h"
#include "QueryResult.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CQueryResult::CQueryResult(DWORD dwUserID,WORD wServer,
						   WORD wArchive, 
						   WORD wSequenceNr,
						   DWORD dwRecordNr,
						   int iNumFields,
						   const CIPCField fields[])
{
	m_dwUserID = dwUserID;
	m_wServer = wServer;
	m_wArchiveNr = wArchive;
	m_wSequenceNr = wSequenceNr;
	m_dwRecordNr = dwRecordNr;
	m_Fields.FromArray(iNumFields,fields);
}
//////////////////////////////////////////////////////////////////////
CQueryResult::CQueryResult(WORD wServer, const CIPCPictureRecord &pictData)
{
	m_wServer = wServer;
	m_wArchiveNr = 0;
	m_wSequenceNr = 0;
	m_dwRecordNr = 0;
	m_Fields.FromString(pictData.GetInfoString());
}
//////////////////////////////////////////////////////////////////////
CQueryResult::~CQueryResult()
{
	m_Fields.SafeDeleteAll();
}
//////////////////////////////////////////////////////////////////////
BOOL CQueryResult::IsEqual(WORD wServer,WORD wArchivNr,WORD wSequenceNr, DWORD dwRecordNr)
{
	return (wServer == m_wServer) &&
		   (wArchivNr == m_wArchiveNr) &&
		   (wSequenceNr == m_wSequenceNr) &&
		   (dwRecordNr == m_dwRecordNr);
}
