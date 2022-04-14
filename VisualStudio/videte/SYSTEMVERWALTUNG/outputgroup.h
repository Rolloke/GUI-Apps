/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: outputgroup.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/outputgroup.h $
// CHECKIN:		$Date: 14.09.04 16:18 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 14.09.04 16:15 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __COUTPUTGROUP_H_
#define __COUTPUTGROUP_H_

#include "output.h"

class COutputList;

class COutputGroup
{
	// construction, destruction
public:
	COutputGroup(LPCTSTR pName, int iNr,LPCTSTR pSMName, CSecID SecID);
	~COutputGroup();

protected:
	COutputGroup();

	// attributes
public:
	inline CString GetName() const;
	inline void    SetName(const CString&);
	inline CString GetSMName() const;
	inline int 	   GetSize() const;
	inline CSecID  GetID() const;

	inline COutput*	GetOutput(int iNr) const;


	BOOL			HasCameras();
	BOOL			HasRelais();
	BOOL			CanVideoOut();

	// operations
public:	
		   void SetSize(WORD iSize);
		   BOOL SetOutput(COutput* pOutput);
	inline void SetID(WORD w);

	// serialization
public:
	void	Reset();
	BOOL	Load(int iNr,CWK_Profile& wkp);
	BOOL	Save(int iNr,CWK_Profile& wkp);
	void	GenerateHTMLFile();

protected:
	CString GetGroupName(CWK_Profile& wkp, const CString& sGroupDefine);
	void	CheckType();

private:
	COutputArray	m_Array;		// the Outputs
	CString			m_sName;		// name of the group
	CString			m_sSMName;		// shared memory name for Output
	CSecID			m_GroupID;		// the id of the group
	int				m_nType;

	friend class COutputList;
};
////////////////////////////////////////////////////////////////////////
inline COutput* COutputGroup::GetOutput(int i) const
{
	return (COutput*)m_Array.GetAt(i); 
}
////////////////////////////////////////////////////////////////////////
inline CString COutputGroup::GetSMName() const
{
	return m_sSMName; 
}
////////////////////////////////////////////////////////////////////////
inline void COutputGroup::SetName(const CString& sName)
{
	m_sName = sName;
}
////////////////////////////////////////////////////////////////////////
inline CString COutputGroup::GetName() const
{
	return m_sName; 
}
////////////////////////////////////////////////////////////////////////
inline int COutputGroup::GetSize() const
{
	return m_Array.GetSize(); 
}
////////////////////////////////////////////////////////////////////////
inline CSecID COutputGroup::GetID() const
{
	return m_GroupID; 
}
////////////////////////////////////////////////////////////////////////
inline void	COutputGroup::SetID(WORD w)
{
	CSecID id(w,0);
	m_GroupID = id; 
}
////////////////////////////////////////////////////////////////////////
typedef COutputGroup* COutputGroupPtr;
WK_PTR_ARRAY(COutputGroupArray,COutputGroupPtr);
////////////////////////////////////////////////////////////////////////
#endif
