/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCServerControlIdipClient.h $
// ARCHIVE:		$Archive: /Project/Clients/IdipClient/CIPCServerControlIdipClient.h $
// CHECKIN:		$Date: 4.06.04 9:08 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 4.06.04 8:46 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _CIPCSERVERCONTROLIDIPCLIENT_H_
#define _CIPCSERVERCONTROLIDIPCLIENT_H_

class CIdipClientDoc;

class CIPCServerControlIdipClient : public CIPCServerControl
{
	// construction / destruction
public:
	CIPCServerControlIdipClient(LPCTSTR szShmName, CIdipClientDoc* pDoc);
    virtual ~CIPCServerControlIdipClient();
   
	// overrides
public:
	virtual void OnRequestAlarmConnection(const CConnectionRecord &c);
	virtual void OnRequestServerReset(const CConnectionRecord &c);

private:
	CIdipClientDoc* m_pDoc;
};

#endif // _CIPCSERVERCONTROLIDIPCLIENT_H_
