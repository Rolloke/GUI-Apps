/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: IOGroup.cpp $
// ARCHIVE:		$Archive: /Project/SecurityServer/IOGroup.cpp $
// CHECKIN:		$Date: 2.04.04 11:57 $
// VERSION:		$Revision: 24 $
// LAST CHANGE:	$Modtime: 2.04.04 11:23 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "IOGroup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


LPCTSTR NameOfEnum(GroupMemberState state)
{
	switch (state)
	{
	NAME_OF_ENUM(GMS_ERROR);
	NAME_OF_ENUM(GMS_INACTIVE); 
	NAME_OF_ENUM(GMS_ACTIVE); 
	NAME_OF_ENUM(GMS_OFF);
	default:
		return _T("");
	}
}

CIOGroup::CIOGroup(LPCTSTR pName)
{
	m_bShutdownInProgress=FALSE;
	m_sName			= pName;
	m_iHardware		= 1;			
	m_wGroupID		= SECID_NO_GROUPID;	
	m_dwState		= 0L;			// Der aktuelle Status der Eing. als Bitmask
	m_dwFree		= 0L;
	m_dwVersion = 1;
}

CIOGroup::~CIOGroup()
{
	// DeleteAllXXX must be called before!
	ASSERT(m_Array.GetSize()==0);
}

void CIOGroup::InformClientsAlarmOffSpans()
{
	CInput* pInput;
	for (int i=0;i<GetSize();i++)
	{
		pInput = GetInputAtWritable(i);
		if (   pInput 
			&& (pInput->GetIDActivate() != SECID_ACTIVE_OFF)
		   )
		{
			pInput->InformClientsAlarmOffSpans();
		}
	}
}
BOOL CIOGroup::RecalcAlarmOffSpans(const CSystemTime& st)
{
	BOOL bRet = FALSE;
	CInput* pInput;
	for (int i=0;i<GetSize();i++)
	{
		pInput = GetInputAtWritable(i);
		if (   pInput 
			&& (pInput->GetIDActivate() != SECID_ACTIVE_OFF)
		   )
		{
			// it's ON
			bRet |= pInput->RecalcAlarmOffSpans(st);
		}
	}
	return bRet;
}

void CIOGroup::CreateMDOKActivations()
{
	CInput* pInput;
	for (int i=0;i<GetSize();i++)
	{
		pInput = GetInputAtWritable(i);
		if (   pInput 
			&& (pInput->GetIDActivate() != SECID_ACTIVE_OFF)
		   )
		{
			// it's a MD and it's ON
			pInput->CreateMDOKActivation();
		}
	}
}

void CIOGroup::DeleteAllOutputs()
{
	for (int i=0;i<m_Array.GetSize();i++) {
		delete (COutput *)m_Array.GetAt(i);
	}
	m_Array.RemoveAll();
}
void CIOGroup::DeleteAllInputs()
{
	for (int i=0;i<m_Array.GetSize();i++) {
		delete (CInput *)m_Array.GetAt(i);
	}
	m_Array.RemoveAll();
}
void CIOGroup::DeleteAllMedia()
{
	for (int i=0;i<m_Array.GetSize();i++) {
		delete (CMedia*)m_Array.GetAt(i);
	}
	m_Array.RemoveAll();
}

void CIOGroup::AddInput(CInput* pInput)
{
	m_Array.Add(pInput);
	setbit(m_dwFree, m_Array.GetSize()-1, (pInput->GetIDActivate()==SECID_ACTIVE_OFF) ? 0:1); 
}


void CIOGroup::AddOutput(COutput* pOutput)
{
	m_Array.Add(pOutput);
	setbit(m_dwFree, m_Array.GetSize()-1, (pOutput->GetOutputType()!=OUTPUT_OFF) ? 1:0); 
}

void CIOGroup::AddMedia(CMedia* pMedia)
{
	m_Array.Add(pMedia);
	setbit(m_dwFree, m_Array.GetSize()-1, (pMedia->GetType().IsEmpty() ? 0:1)); 
}
GroupMemberState CIOGroup::GetState(int iNr) const
{
	GroupMemberState result=GMS_ERROR;
	
	if (m_dwFree & (1L<<iNr)) 
	{	// on or off
		if (m_iHardware==0) 
		{	// hardware okay ? 
			if (m_dwState & (1L<<iNr))	
			{ // active or inactive
				result = GMS_ACTIVE;
			} 
			else 
			{
				result = GMS_INACTIVE;
			}
		} 
		else 
		{
			result = GMS_ERROR;
		}
	} 
	else 
	{
		result = GMS_OFF;
	}
	
	return result;
}

DWORD CIOGroup::GetFreeMask() const
{
	return m_dwFree;
}

void CIOGroup::SetShutdownInProgress() 
{ 
	m_bShutdownInProgress=TRUE; 
}

