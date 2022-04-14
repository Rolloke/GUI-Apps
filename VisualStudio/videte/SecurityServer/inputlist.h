/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: inputlist.h $
// ARCHIVE:		$Archive: /Project/SecurityServer/inputlist.h $
// CHECKIN:		$Date: 11.04.05 12:53 $
// VERSION:		$Revision: 43 $
// LAST CHANGE:	$Modtime: 8.04.05 15:02 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef __INPUTLIST_H_
#define __INPUTLIST_H_

#include "CipcInputServer.h"
///////////////////////////////////////////////////////////////////////////////////////
typedef CIPCInputServer * CIPCInputServerPtr;
WK_PTR_ARRAY_CS(CIPCInputServerArrayPlain,CIPCInputServerPtr,CIPCInputServerArraySafe)
///////////////////////////////////////////////////////////////////////////////////////
class CInputList : public CIPCInputServerArraySafe
{
	// construction / destruction
public:
	CInputList();
	virtual ~CInputList();

	// attributes
public:
	CInput*					GetInputByID(CSecID id) const;
	CIPCInputServer*		GetGroupByID(CSecID id) const;
	CIPCInputServer*		GetGroupByShmName(const CString& shmName) const;
	inline CIPCInputServer*	GetGroupAt(int i) const;

	// operations
public:
	void StartThreads();
	void Load(CWK_Profile& wkp);
	void CreateMDOKActivations();
	void StartInitialProcesses();
	BOOL RecalcAlarmOffSpans(const CSystemTime& st);
	void InformClientsAlarmOffSpans();

private:
	CIPCInputServer* NewGroup(LPCTSTR  pName, int iNr, LPCTSTR pSMName, CSecID newID);
	BOOL AddInputGroup(const CString &str);
	BOOL ReadIo(CInput &oneInput,
				const CString & sSectioName, 
				int inputNr, int ioNr);

};
inline CIPCInputServer*	CInputList::GetGroupAt(int i) const 
{ 
	if (i<GetSize()) 
	{
		return GetAtFast(i);
	} 
	else 
	{
		return NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
#endif
