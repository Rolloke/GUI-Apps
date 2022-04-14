// InputList.cpp: implementation of the CInputList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVProcess.h"
#include "InputList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CInputList::CInputList()
{
}
//////////////////////////////////////////////////////////////////////
CInputList::~CInputList()
{
}
//////////////////////////////////////////////////////////////////////
int	CInputList::GetOutstandingPicts()
{
	int iSum = 0;
	Lock();

	for (int i=0;i<GetSize();i++)
	{
		CInputGroup* pGroup = GetAtFast(i);
		iSum += pGroup->GetOutstandingPicts();
	}

	Unlock();
	
	return iSum;
}
//////////////////////////////////////////////////////////////////////
void CInputList::Load(CWK_Profile& wkp)
{
	Lock();

	for (int i=0;i<GetSize();i++)
	{
		CInputGroup* pGroup = GetAtFast(i);
		pGroup->Load(wkp);
	}

	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CInputList::Save(CWK_Profile& wkp, BOOL bShutdown)
{
	Lock();

	for (int i=0;i<GetSize();i++)
	{
		CInputGroup* pGroup = GetAtFast(i);
		pGroup->Save(wkp,bShutdown);
	}

	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CInputList::StartCIPCThreads()
{
	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->StartThread();
	}
}
//////////////////////////////////////////////////////////////////////
void CInputList::WaitForConnect()
{
	BOOL bConnected = FALSE;
	int c = 10;
	while ( (c-->0) && (!bConnected))
	{
		bConnected = TRUE;
		for (int i=0;i<GetSize();i++)
		{
			bConnected &= GetAtFast(i)->GetIPCState()==CIPC_STATE_CONNECTED;
		}
		Sleep(50);
	}
}
//////////////////////////////////////////////////////////////////////
void CInputList::CheckActive()
{
	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->CheckActive();
	}
}
//////////////////////////////////////////////////////////////////////
void CInputList::ClearActive()
{
	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->ClearActive();
	}
}
//////////////////////////////////////////////////////////////////////
CInput* CInputList::GetInputByClientID(CSecID id)
{
	Lock();

	for (int i=0;i<GetSize();i++)
	{
		CInputGroup* pInputGroup = GetAtFast(i);
		pInputGroup->Lock();
		for (int j=0;j<pInputGroup->GetSize();j++)
		{
			CInput* pInput = pInputGroup->GetAtFast(j);
			if (pInput->GetClientID() == id)
			{
				pInputGroup->Unlock();
				Unlock();
				return pInput;
			}
		}

		pInputGroup->Unlock();
	}

	Unlock();

	return NULL;
}
//////////////////////////////////////////////////////////////////////
CInput* CInputList::GetInputByUnitID(CSecID id)
{
	Lock();

	for (int i=0;i<GetSize();i++)
	{
		CInputGroup* pInputGroup = GetAtFast(i);
		pInputGroup->Lock();
		for (int j=0;j<pInputGroup->GetSize();j++)
		{
			CInput* pInput = pInputGroup->GetAtFast(j);
			if (pInput->GetUnitID() == id)
			{
				pInputGroup->Unlock();
				Unlock();
				return pInput;
			}
		}

		pInputGroup->Unlock();
	}

	Unlock();

	return NULL;
}
//////////////////////////////////////////////////////////////////////
CInput* CInputList::GetPTZInput()
{
	Lock();

	for (int i=0;i<GetSize();i++)
	{
		CInputGroup* pInputGroup = GetAtFast(i);
		pInputGroup->Lock();
		for (int j=0;j<pInputGroup->GetSize();j++)
		{
			CInput* pInput = pInputGroup->GetAtFast(j);
			if (pInput->IsPTZ())
			{
				pInputGroup->Unlock();
				Unlock();
				return pInput;
			}
		}

		pInputGroup->Unlock();
	}

	Unlock();

	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CInputList::CleanUp()
{
	SafeDeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
void CInputList::DeleteGroup(const CString& shmName)
{
	Lock();
	for (int i=0;i<GetSize();i++)
	{
		CInputGroup* pInputGroup = GetAtFast(i);
		if (pInputGroup->GetShmName() == shmName)
		{
			WK_DELETE(pInputGroup);
			RemoveAt(i);
			break;
		}
	}
	Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CInputList::StartGroup(const CString& shmName)
{
	for (int i=0;i<GetSize();i++)
	{
		CInputGroup* pInputGroup = GetAtFast(i);
		if (pInputGroup->GetShmName() == shmName)
		{
			pInputGroup->StartThread();
			BOOL bConnected = FALSE;
			int c = 10;
			while ( (c-->0) && (!bConnected))
			{
				bConnected = pInputGroup->GetIPCState()==CIPC_STATE_CONNECTED;
				Sleep(10);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CInputList::ResetAudioReferences()
{
	Lock();
	for (int i=0;i<GetSize();i++)
	{
		CInputGroup* pInputGroup = GetAtFast(i);
		for (int j=0;j<pInputGroup->GetSize();j++)
		{
			pInputGroup->GetAtFast(j)->SetMedia(NULL);
		}
	}
	Unlock();
}
