/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: mediagroup.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/mediagroup.h $
// CHECKIN:		$Date: 11.09.03 15:51 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 11.09.03 11:50 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __MEDIAGROUP_H_
#define __MEDIAGROUP_H_

#include "media.h"

class CMediaList;

class CMediaGroup : public CObject
{
	// construction, destruction
public:
	CMediaGroup(LPCTSTR pName, int iNr, LPCTSTR pSMName, CSecID SecID, LPCTSTR sType);
	~CMediaGroup();

protected:
	CMediaGroup();
	// attributes
public:
	inline CString GetName() const;
	inline CString GetSMName() const;
		   int GetNrOfActiveMedias() const;
	inline int 	   GetSize() const;
	inline CSecID  GetID() const;
	inline CMedia*	GetMedia(int iNr);
	inline const CString&	GetType() const;

	// operations
public:	
		   void SetSize(WORD iSize);
		   BOOL SetMedia(CMedia* pMedia);
	inline void SetName(const CString& sName);
	inline void SetID(WORD w);
	inline void SetType(const CString &sType);

	// serialization
public:
	void	Reset();
	BOOL	Load(int iNr, CWK_Profile& wkp);
	BOOL	Save(int iNr, CWK_Profile& wkp);
	void	GenerateHTMLFile();

#ifdef _DEBUG
public:
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	CMediaArray	m_Array;		// the medias
	CString		m_sName;		// name of the group
	CString		m_sSMName;		// shared memory name for media
	CSecID		m_GroupID;		// the id of the group
	CString	   m_sType;

	friend class CMediaList;
};
////////////////////////////////////////////////////////////////////////
inline CMedia* CMediaGroup::GetMedia(int i)
{
	return (CMedia*)m_Array.GetAt(i); 
}
////////////////////////////////////////////////////////////////////////
inline CString CMediaGroup::GetSMName() const
{
	return m_sSMName; 
}
////////////////////////////////////////////////////////////////////////
inline CString CMediaGroup::GetName() const
{
	return m_sName; 
}
////////////////////////////////////////////////////////////////////////
inline void CMediaGroup::SetName(const CString& sName)
{
	m_sName = sName;
}
////////////////////////////////////////////////////////////////////////
inline int CMediaGroup::GetSize() const
{
	return m_Array.GetSize(); 
}
////////////////////////////////////////////////////////////////////////
inline CSecID CMediaGroup::GetID() const
{
	return m_GroupID; 
}
////////////////////////////////////////////////////////////////////////
inline void	CMediaGroup::SetID(WORD w)
{
	CSecID id(w,0);
	m_GroupID = id; 
}
//////////////////////////////////////////////////////////////////////
const CString &CMediaGroup::GetType() const
{
	return (m_sType);
}
//////////////////////////////////////////////////////////////////////
inline void CMediaGroup::SetType(const CString &sType)
{
	m_sType= sType;
}
////////////////////////////////////////////////////////////////////////
typedef CMediaGroup* CMediaGroupPtr;
WK_PTR_ARRAY(CMediaGroupArray,CMediaGroupPtr);
////////////////////////////////////////////////////////////////////////
#endif
