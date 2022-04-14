

#include <stdafx.h>

#include "PostFetch.h"

#include "CIPCServerControlClientSide.h"

/* PostFetchXXX, ruft NewFetchXXX, kehrt aber sofort zurueck. Nicht blockierend!
Das result from NewFetchXXX wird geposted. lParam == CIPCFetchResult *pResult.
Das muss dann deleted werden!!!
*/


class CFetchThreadRecord 
{
public:
	CFetchThreadRecord(FetchCallType t,const CConnectionRecord &c, DWORD dwUserData)
	{
		m_type = t;
		m_connection = c;
		m_dwUserData = dwUserData;
	}
	//
	FetchCallType m_type;
	CConnectionRecord m_connection;
	DWORD m_dwUserData;
};

static UINT theThreadFunction(LPVOID pParam)
{
	CFetchThreadRecord *pRecord = (CFetchThreadRecord *)pParam;
	CIPCFetchResult result;
	switch (pRecord->m_type) {
		case FCT_INPUT:
			result = NewFetchInputConnection(pRecord->m_connection);
			break;
		case FCT_OUTPUT:
			result = NewFetchOutputConnection(pRecord->m_connection);
			break;
		default:
			;	// NOT YET
	}
	CPostFetchResult *pPostResult = new CPostFetchResult(
		pRecord->m_type,
		result,
		pRecord->m_dwUserData
		);
	AfxGetMainWnd()->PostMessage(WM_COMMAND,THE_MESSAGE,(DWORD)pPostResult);

	WK_DELETE(pRecord);

	return FALSE;	// terminate thread
}

void PostFetchInputConnection(const CConnectionRecord &c, DWORD dwUserData)
{
	CFetchThreadRecord *pRecord = new CFetchThreadRecord(FCT_INPUT,c ,dwUserData);
	CWinThread * pThread = AfxBeginThread(theThreadFunction, pRecord );
	// thread has autodelete
}

void PostFetchOutputConnection(const CConnectionRecord &c, DWORD dwUserData)
{
	CFetchThreadRecord *pRecord = new CFetchThreadRecord(FCT_OUTPUT,c ,dwUserData);
	CWinThread * pThread = AfxBeginThread(theThreadFunction, pRecord );
	// thread has autodelete
}



