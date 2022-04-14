/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: IOGroup.h $
// ARCHIVE:		$Archive: /Project/SecurityServer/IOGroup.h $
// CHECKIN:		$Date: 20.04.05 12:04 $
// VERSION:		$Revision: 38 $
// LAST CHANGE:	$Modtime: 20.04.05 11:38 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __CIOGROUP_H_
#define __CIOGROUP_H_

#include "cipc.h"
#include "input.h"
#include "output.h"
#include "media.h"

enum GroupMemberState
{
	GMS_ERROR, 
	GMS_INACTIVE, 
	GMS_ACTIVE, 
	GMS_OFF
};
LPCTSTR NameOfEnum(GroupMemberState state);

WK_PTR_ARRAY(CVoidPtrArray, LPVOID)

class CIOGroup
{
	// construction / destruction
public:
	CIOGroup(LPCTSTR pName);
	virtual ~CIOGroup();

	// attributes
public:
	inline const CString &GetName() const;
	inline int 	 GetSize() const;
	inline DWORD GetVersion() const;
	inline int	 GetHardwareState() const;

	// iNr can be 0..31, or even a global number it is used as (iNr%32)
	inline const COutput*	GetOutputAt(int iNr) const;
	inline const CInput*	GetInputAt(int i) const;
	inline const CMedia*	GetMediaAt(int i) const;
	//
	inline COutput*		GetOutputAtWritable(int i) const;
	inline CInput*		GetInputAtWritable(int i) const;
	inline CMedia*		GetMediaAtWritable(int i) const;
	
	// operations
public:
	void AddOutput(COutput* pOutput);
	void AddInput(CInput* pInput);
	void AddMedia(CMedia* pMedia);

	void DeleteAllInputs();
	void DeleteAllOutputs();
	void DeleteAllMedia();

	void CreateMDOKActivations();
	BOOL RecalcAlarmOffSpans(const CSystemTime& st);
	void InformClientsAlarmOffSpans();

	// Status-Funktionen
	GroupMemberState	GetState(int i) const; 
	// Liefert den Status eines Gruppenmitgliedes, 0=aus oder ok, >0: ein oder Alarm
	// i:	Die Nummer des Gruppenmitgliedes [1...32]

	void	SetGroupID(WORD w) { m_wGroupID = w; }
	WORD 	GetGroupID() const { return m_wGroupID; }
	DWORD	GetFreeMask() const;
	//
	virtual void SetShutdownInProgress();
protected:
	WORD	m_wGroupID;		// 0x1000+ or 0x2000+ 
	int		m_iHardware;	// Hardwareflag, 0: ok, !=0 Fehler im Modul
	CString	m_sName;		// Der Name der Eingangsgruppe
	DWORD	m_dwState;		// Der aktuelle Status der max 32 Gruppenmitglieder.
	DWORD	m_dwFree;		// Der aktuelle Freigabe der 32 Gruppenmitglider
	volatile BOOL	m_bShutdownInProgress;
	DWORD	m_dwVersion;
private:
	CVoidPtrArray m_Array;
};
/////////////////////////////////////////////////////////////////////////////
inline COutput* CIOGroup::GetOutputAtWritable(int i) const 
{
	if (IsInArray(i, m_Array.GetSize()))
	{
		return (COutput*)m_Array.GetAtFast(i); 
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
inline const COutput* CIOGroup::GetOutputAt(int ix) const
{
	return GetOutputAtWritable(ix);
}
/////////////////////////////////////////////////////////////////////////////
inline CMedia* CIOGroup::GetMediaAtWritable(int i) const 
{
	if (IsInArray(i, m_Array.GetSize()))
	{
		return (CMedia*)m_Array.GetAtFast(i); 
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
inline const CMedia* CIOGroup::GetMediaAt(int ix) const
{
	return GetMediaAtWritable(ix);
}
/////////////////////////////////////////////////////////////////////////////
inline CInput* CIOGroup::GetInputAtWritable(int i) const 
{
	if (IsInArray(i, m_Array.GetSize()))
	{
		return (CInput*)m_Array.GetAtFast(i); 
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
inline const CInput* CIOGroup::GetInputAt(int ix) const
{
	return GetInputAtWritable(ix);
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CIOGroup::GetVersion() const
{
	return m_dwVersion;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString& CIOGroup::GetName() const 
{ 
	return m_sName; 
}
/////////////////////////////////////////////////////////////////////////////
inline int CIOGroup::GetSize() const	
{ 
	return m_Array.GetSize(); 
}
/////////////////////////////////////////////////////////////////////////////
inline int CIOGroup::GetHardwareState() const
{
	return m_iHardware;
}

#endif
