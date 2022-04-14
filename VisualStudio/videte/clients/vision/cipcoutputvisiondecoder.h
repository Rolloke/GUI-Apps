/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCOutputVisionDecoder.h $
// ARCHIVE:		$Archive: /Project/Clients/Vision/CIPCOutputVisionDecoder.h $
// CHECKIN:		$Date: 25.02.04 12:31 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 25.02.04 11:26 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
	
#ifndef _CIPCOUTPUTVISIONDECODER_H_
#define _CIPCOUTPUTVISIONDECODER_H_

class CVisionDoc;
//////////////////////////////////////////////////////////////////
class CIPCOutputVisionDecoder : public CIPCOutput
{
	// construction / destruction
public:
	CIPCOutputVisionDecoder(LPCTSTR shmName, CVisionDoc* pDoc);
	~CIPCOutputVisionDecoder();

	// overridables
public:
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnConfirmInfoOutputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCOutputRecord records[]);
	virtual void OnRequestDisconnect();

	virtual void OnConfirmGetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						const CString &sValue,
						DWORD dwServerData);

	virtual void OnConfirmMpegDecoding(WORD wGroupID, DWORD dwUserData);

public:
	CString		m_sMaxRect;
	COLORREF	m_crKeyColor;
private:
	CVisionDoc*	m_pVisionDoc;
};
//////////////////////////////////////////////////////////////////
#endif	// _ExplorerDecoder_H_