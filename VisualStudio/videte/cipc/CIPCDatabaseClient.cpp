// CIPCDatabaseClient.cpp: implementation of the CCIPCDatabaseClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdcipc.h"

#include "CIPCDatabase.h"
#include "CIPCDatabaseClient.h"

/////////////////////////////
/// PART: CDatabaseClient ///
/////////////////////////////
/*

*/

/*{CIPCDatabaseClient Overview|Overview,CIPCDatbaseClient}
 {members|CIPCDatabaseClient}, {CIPCDatabase}
*/
/*Function: NOT YET DOCUMENTED */
CIPCDatabaseClient::CIPCDatabaseClient(LPCTSTR shmName)
: CIPCDatabase(shmName,FALSE)	// no master
{
	// m_archives
	m_bGotRecords=FALSE;
}

/*Function: NOT YET DOCUMENTED */
CIPCDatabaseClient::~CIPCDatabaseClient()
{
	m_archives.SafeDeleteAll();
}
/*{const CIPCArchivRecordArray &CIPCDatabaseClient::GetRecords() const}
access to the records of the database. It is a WK_PTR_ARRAY_CS,
a pointer array plus critical section.

Use {CIPCDatabaseClient::GotRecords})(), to check if the inital
communication took place.

 {CIPCDatabaseClient::OnConfirmInfo}
*/
/*{BOOL CIPCDatabaseClient::GotRecords() const}
returns TRUE, if records are received from the database server.
 {CIPCDatabaseClient::OnConfirmInfo}
*/
/*Function: initiates a {CIPC::DoRequestConnection}() */
void CIPCDatabaseClient::OnReadChannelFound()
{
	DoRequestConnection();
}
/*Function: initiates a {CIPCDatabase::DoRequestInfo}() */
void CIPCDatabaseClient::OnConfirmConnection()
{
	RefreshInfo();
}

/*Function: sends {CIPCDatabase::DoRequestInfo}.
	deletes all archivinfo.
	m_bGotRecords = FALSE; 
	until new info available.
*/
void CIPCDatabaseClient::RefreshInfo()
{
	m_bGotRecords = FALSE;
	DoRequestInfo();
}

/*Function: handles the answer of a {CIPCDatabase::DoRequestInfo}.
The records are stored internaly. Derived classes can access
via {CIPCDatabaseClient::GetRecords}.
*/
void CIPCDatabaseClient::OnConfirmInfo(int iNumRecords, const CIPCArchivRecord data[])
{
	m_archives.Lock();
	m_archives.DeleteAll();

	for (int i=0;i<iNumRecords;i++) 
	{
		m_archives.Add(new CIPCArchivRecord(data[i]));
	}

	m_bGotRecords=TRUE;
	m_archives.Unlock();
}
void CIPCDatabaseClient::OnIndicateNewArchiv(const CIPCArchivRecord& data)
{
	m_archives.Lock();
	BOOL bFound = FALSE;
	for (int i=0;i<m_archives.GetSize()&&!bFound;i++) 
	{
		bFound = m_archives.GetAtFast(i)->GetID() == data.GetID();
	}
	if (!bFound)
	{
		m_archives.Add(new CIPCArchivRecord(data));
	}
	m_archives.Unlock();
}

void CIPCDatabaseClient::OnIndicateRemoveArchiv(WORD wArchivNr)
{
	m_archives.Lock();
	m_archives.RemoveArchiv(wArchivNr);
	m_archives.Unlock();
}
