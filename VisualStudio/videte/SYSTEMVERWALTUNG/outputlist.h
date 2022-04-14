/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: outputlist.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/outputlist.h $
// CHECKIN:		$Date: 26.02.04 14:55 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 26.02.04 9:27 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$

#ifndef _OUTPUTLIST_H
#define _OUTPUTLIST_H

#include "Output.h"
#include "OutputGroup.h"

/////////////////////////////////////////////////////////////////////////////
// COutputList 
/////////////////////////////////////////////////////////////////////////////
class COutputList
{
	// construction
public:
	COutputList();
	~COutputList();
	
	// attributes
public:
	// output
	const COutput*	GetOutputByID(CSecID id) const;
	
	// group
	inline int GetSize() const;
	inline COutputGroup*	GetGroupAt(int i) const;
	COutputGroup*	GetGroupByID(CSecID id) const;
	COutputGroup* 	GetGroupByName(const CString& sName) const;	
	COutputGroup* 	GetGroupBySMName(const CString& sSMName) const;	

	// operations
public:
	// delete an existing group
	BOOL			DeleteGroup(const CString& sSMName);
	// create a new group
	COutputGroup* 	AddOutputGroup(LPCTSTR pName, int iNr,LPCTSTR pSMName);

	// serialization
	void Reset();
	void Load(CWK_Profile& wkp);
	void Save(CWK_Profile& wkp, BOOL bGenerateHTML);
	void GenerateHTMLFile();

private:
	COutputGroupArray	m_Array;
};
/////////////////////////////////////////////////////////////////////////////
inline int COutputList::GetSize() const
{
	return m_Array.GetSize(); 
}
/////////////////////////////////////////////////////////////////////////////
inline COutputGroup*	COutputList::GetGroupAt(int i) const
{
	return (GetSize() && i>=0) ? (COutputGroup*)m_Array.GetAt(i):NULL; 
}
/////////////////////////////////////////////////////////////////////////////
#endif // OUTPUTLIST_H
