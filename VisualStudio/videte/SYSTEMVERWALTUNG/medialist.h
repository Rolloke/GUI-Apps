/* GlobalReplace: GetGrpAt --> GetGroupAt */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: medialist.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/medialist.h $
// CHECKIN:		$Date: 11.09.03 15:51 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 11.09.03 11:50 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __MEDIALIST_H_
#define __MEDIALIST_H_

#include "Media.h"
#include "MediaGroup.h"

class CMediaList : public CObject
{
	// construction
public:
	CMediaList();
	~CMediaList();

	// attributes
public:
	// media
	const CMedia*	GetMediaByID(CSecID id) const;
	
	// group
	inline int GetSize() const;
		   int GetNrOfActiveMedias() const;
	inline CMediaGroup*	GetGroupAt(int i) const;
	CMediaGroup*	GetGroupByID(CSecID id) const;
	CMediaGroup* 	GetGroupByName(const CString& sName) const;	
	CMediaGroup* 	GetGroupBySMName(const CString& sSMName) const;	

	// operations
public:
	// delete an existing group
	BOOL			DeleteGroup(const CString& sSMName);
	BOOL			DeleteActivation(CSecID id);
	// create a new group
	CMediaGroup* 	AddMediaGroup(LPCTSTR pName, int iNr, LPCTSTR pSMName, LPCTSTR sType);

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
	CMediaGroupArray	m_Array;
};
/////////////////////////////////////////////////////////////////////////////
inline int CMediaList::GetSize() const
{
	return m_Array.GetSize(); 
}
/////////////////////////////////////////////////////////////////////////////
inline CMediaGroup*	CMediaList::GetGroupAt(int i) const
{
	return (GetSize() && i>=0) ? (CMediaGroup*)m_Array.GetAt(i):NULL; 
}
/////////////////////////////////////////////////////////////////////////////
#endif
