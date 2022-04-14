/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: inputgroup.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/inputgroup.h $
// CHECKIN:		$Date: 1/20/05 2:54p $
// VERSION:		$Revision: 9 $
// LAST CHANGE:	$Modtime: 1/20/05 2:20p $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef __INPUTGROUP_H_
#define __INPUTGROUP_H_

#include "input.h"

class CInputList;

class CInputGroup : public CObject
{
	// construction, destruction
public:
	CInputGroup(LPCTSTR pName, int iNr, LPCTSTR pSMName, CSecID SecID);
	~CInputGroup();

protected:
	CInputGroup();
	// attributes
public:
	inline CString GetName() const;
	inline CString GetSMName() const;
		   int	   GetNrOfActiveInputs() const;
	inline int 	   GetSize() const;
	inline CSecID  GetID() const;
	inline CInput* GetInput(int iNr);
		   BOOL	   IsMD() const;
	// operations
public:	
		   void SetSize(WORD iSize);
		   BOOL SetInput(CInput* pInput);
	inline void SetName(const CString& sName);
	inline void SetID(WORD w);
		   BOOL	DeleteActivation(CSecID id);
    static void GetVirtualNames(const CString& sVersion, CStringArray& sInputNameArray);

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

protected:
	CString GetGroupName(CWK_Profile& wkp, const CString& sGroupDefine);

private:
	CInputArray	m_Array;		// the inputs
	CString		m_sName;		// name of the group
	CString		m_sSMName;		// shared memory name for input
	CSecID		m_GroupID;		// the id of the group

	friend class CInputList;
};
////////////////////////////////////////////////////////////////////////
inline CInput* CInputGroup::GetInput(int i)
{
	return (CInput*)m_Array.GetAt(i); 
}
////////////////////////////////////////////////////////////////////////
inline CString CInputGroup::GetSMName() const
{
	return m_sSMName; 
}
////////////////////////////////////////////////////////////////////////
inline CString CInputGroup::GetName() const
{
	return m_sName; 
}
////////////////////////////////////////////////////////////////////////
inline void CInputGroup::SetName(const CString& sName)
{
	m_sName = sName;
}
////////////////////////////////////////////////////////////////////////
inline int CInputGroup::GetSize() const
{
	return m_Array.GetSize(); 
}
////////////////////////////////////////////////////////////////////////
inline CSecID CInputGroup::GetID() const
{
	return m_GroupID; 
}
////////////////////////////////////////////////////////////////////////
inline void	CInputGroup::SetID(WORD w)
{
	CSecID id(w,0);
	m_GroupID = id; 
}
////////////////////////////////////////////////////////////////////////
typedef CInputGroup* CInputGroupPtr;
WK_PTR_ARRAY(CInputGroupArray,CInputGroupPtr);
////////////////////////////////////////////////////////////////////////
#endif
