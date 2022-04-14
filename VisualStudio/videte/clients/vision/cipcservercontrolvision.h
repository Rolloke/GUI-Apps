/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCServerControlVision.h $
// ARCHIVE:		$Archive: /Project/Clients/Vision/CIPCServerControlVision.h $
// CHECKIN:		$Date: 25.02.04 12:31 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 25.02.04 11:26 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$

#ifndef _CIPCSERVERCONTROLVISION_H_
#define _CIPCSERVERCONTROLVISION_H_

class CVisionDoc;

class CIPCServerControlVision : public CIPCServerControl
{
public:
	CIPCServerControlVision(LPCTSTR shmName, CVisionDoc* pDoc);
   ~CIPCServerControlVision();
   
	virtual void OnRequestAlarmConnection(const CConnectionRecord &c);

private:
	CVisionDoc* m_pVisionDoc;
};

#endif // _CExplorerControl_H_
