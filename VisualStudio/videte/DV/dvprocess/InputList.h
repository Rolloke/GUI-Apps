// InputList.h: interface for the CInputList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUTLIST_H__43C9E4A3_8B84_11D3_99EB_004005A19028__INCLUDED_)
#define AFX_INPUTLIST_H__43C9E4A3_8B84_11D3_99EB_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InputGroup.h"

class CInputList : public CInputGroupArrayCS  
{
	// construction / destruction
public:
	CInputList();
	virtual ~CInputList();

	// attributes
public:
	CInput*	  GetInputByClientID(CSecID id);
	CInput*	  GetInputByUnitID(CSecID id);
	CInput*	  GetPTZInput();
	int		  GetOutstandingPicts();

	// operations
public:
	void Load(CWK_Profile& wkp);
	void Save(CWK_Profile& wkp, BOOL bShutdown);
	void StartCIPCThreads();
	void WaitForConnect();
	void ClearActive();
	void CheckActive();
	void CleanUp();

	void ResetAudioReferences();

	void DeleteGroup(const CString& shmName);
	void StartGroup(const CString& shmName);
};

#endif // !defined(AFX_INPUTLIST_H__43C9E4A3_8B84_11D3_99EB_004005A19028__INCLUDED_)
