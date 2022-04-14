/* GlobalReplace: GetGrpAt --> GetGroupAt */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: inputlist.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/inputlist.h $
// CHECKIN:		$Date: 14.06.05 14:51 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 14.06.05 12:13 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef __INPUTLIST_H_
#define __INPUTLIST_H_

#include "Input.h"
#include "InputGroup.h"

class CInputList : public CObject
{
	// construction
public:
	CInputList();
	~CInputList();

	// attributes
public:
	// input
	const CInput*	GetInputByID(CSecID id) const;
	
	// group
	inline int GetSize() const;
		   int GetNrOfActiveInputs() const;
	inline CInputGroup*	GetGroupAt(int i) const;
	CInputGroup*	GetGroupByID(CSecID id) const;
	CInputGroup* 	GetGroupByName(const CString& sName) const;	
	CInputGroup* 	GetGroupBySMName(const CString& sSMName) const;	

	// operations
public:
	// delete an existing group
	BOOL			DeleteGroup(const CString& sSMName,BOOL bExactMatch=TRUE);
	BOOL			DeleteActivation(CSecID id);
	// create a new group
	CInputGroup* 	AddInputGroup(LPCTSTR pName, int iNr, LPCTSTR pSMName);

	// serialization
	void Reset();
	void Load(CWK_Profile& wkp);
	void Save(CWK_Profile& wkp, BOOL bGenerateHTML);
	void GenerateHTMLFile();

#ifdef _DEBUG
public:
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	CInputGroupArray	m_Array;
};
/////////////////////////////////////////////////////////////////////////////
inline int CInputList::GetSize() const
{
	return m_Array.GetSize(); 
}
/////////////////////////////////////////////////////////////////////////////
inline CInputGroup*	CInputList::GetGroupAt(int i) const
{
	return (GetSize() && i>=0) ? (CInputGroup*)m_Array.GetAt(i):NULL; 
}
/////////////////////////////////////////////////////////////////////////////
#endif
