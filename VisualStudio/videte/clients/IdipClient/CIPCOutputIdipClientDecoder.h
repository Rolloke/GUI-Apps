/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCOutputIdipClientDecoder.h $
// ARCHIVE:		$Archive: /Project/Clients/IdipClient/CIPCOutputIdipClientDecoder.h $
// CHECKIN:		$Date: 21.04.04 15:26 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 21.04.04 14:37 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
	
#ifndef _CIPCOUTPUTIdipClientDECODER_H_
#define _CIPCOUTPUTIdipClientDECODER_H_

class CIdipClientDoc;
//////////////////////////////////////////////////////////////////
class CIPCOutputIdipClientDecoder : public CIPCOutput
{
	// construction / destruction
public:
	CIPCOutputIdipClientDecoder(LPCTSTR shmName, CIdipClientDoc* pDoc);
	~CIPCOutputIdipClientDecoder();

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
//	COLORREF	m_crKeyColor;
private:
	CIdipClientDoc*	m_pDoc;
};
//////////////////////////////////////////////////////////////////
#endif	// _CIPCOUTPUTIdipClientDECODER_H_