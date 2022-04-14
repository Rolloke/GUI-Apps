/* GlobalReplace: GetMaxGroups --> GetSize */
/* GlobalReplace: GetGrpAt --> GetGroupAt */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: outputlist.h $
// ARCHIVE:		$Archive: /Project/SecurityServer/outputlist.h $
// CHECKIN:		$Date: 11.06.04 13:39 $
// VERSION:		$Revision: 40 $
// LAST CHANGE:	$Modtime: 11.06.04 13:21 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _OUTPUTLIST_H
#define _OUTPUTLIST_H

#include "output.h"

#include "CipcOutputServer.h"
///////////////////////////////////////////////////////////////////////////////////////
typedef CIPCOutputServer * CIPCOutputServerPtr;
WK_PTR_ARRAY_CS(CIPCOutputServerArrayPlain,CIPCOutputServerPtr,CIPCOutputServerArraySafe)
/////////////////////////////////////////////////////////////////////////////
// COutputList 
/////////////////////////////////////////////////////////////////////////////
class COutputList : public CIPCOutputServerArraySafe
{
	// construction/destruction
public:
	COutputList();
	virtual 	~COutputList();

	// attributes
public:
	CIPCOutputServer* GetGroupAt(int i);
	CIPCOutputServer* GetGroupByID(CSecID id);
	CIPCOutputServer* GetGroupShmName(const CString& sShmName);
	COutput*		  GetOutputByID(CSecID id);

	// operations
public:
	void Load(CWK_Profile& wkp);
	void Save(CWK_Profile& wkp);
	void StartThreads();
	void StartConnections();
	void DeleteUnnecessaryReferenzImages();
	void WriteNamesIni();
	void OnLastClientDisconnected();

	// implementation
protected:
	CIPCOutputServer* 	NewGroup(const CString &sName, int iNr, 
								 const CString &sShmName, CSecID newID);
	BOOL		AddOutputGroup(LPCTSTR);

};
/////////////////////////////////////////////////////////////////////////////
#endif // OUTPUTLIST_H
