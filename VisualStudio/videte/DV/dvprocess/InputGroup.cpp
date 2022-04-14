// InputGroup.cpp: implementation of the CInputGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVProcess.h"
#include "InputGroup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CInputGroup::CInputGroup(CInputList* pInputList, 
						 LPCTSTR shmName, 
						 int wGroup, 
						 int iSize, 
						 int iNrExternal,
						 BOOL bMotion)
  : CIPCInput(shmName, TRUE)
{
	m_pInputList = pInputList;
	WK_TRACE(_T("CInputGroup to %s %d ext, %d suspect\n"),GetShmName(),iNrExternal,iSize-iNrExternal);

	Lock();
	m_bMD = bMotion;
	m_iHardware		= 1;			
	m_dwState		= 0L;			// Der aktuelle Status der Eing. als Bitmask
	m_dwFree		= 0L;
	m_wGroupID = (WORD)wGroup;
	m_sSection.Format(_T("InputGroups\\Group%04lx"),GetID());
	SetSize(iSize);
	CInput* pInput;
	CInput::InputMode mode;
	for (WORD s=0;s<iSize;s++)
	{
		if (bMotion)
		{
			mode = CInput::IM_MD;
		}
		else
		{
			if (s<iNrExternal)
			{
				mode = CInput::IM_EXTERN;
			}
			else
			{
				mode = CInput::IM_SUSPECT;
			}
		}
		pInput = new CInput(this,CSecID(m_wGroupID,s),mode);
		SetAt(s,pInput);
		setbit(m_dwFree, s, 1); 
	}
	Unlock();
	SetConnectRetryTime(25);
	m_bIsJaCob = FALSE;
	m_bIsSaVic = FALSE;
	m_bIsTasha = FALSE;
	m_bIsQ = FALSE;

	CString sName(shmName);

	sName.MakeLower();

	if (-1!=sName.Find(_T("savic")))
	{
		m_bIsSaVic = TRUE;
	}
	else if (-1!=sName.Find(_T("mico")))
	{
		m_bIsJaCob = TRUE;
	}
	else if (-1!=sName.Find(_T("tasha")))
	{
		m_bIsTasha = TRUE;
	}
	else if (-1!=sName.Find(_T("qunit")))
	{
		m_bIsQ = TRUE;
	}
}
//////////////////////////////////////////////////////////////////////
CInputGroup::~CInputGroup()
{
	TRACE(_T("~CInputGroup %s\n"), GetShmName());
	StopThread();
	SafeDeleteAll();
}
//////////////////////////////////////////////////////////////////////
BOOL CInputGroup::IsJacob() const
{
	return m_bIsJaCob;
}
//////////////////////////////////////////////////////////////////////
BOOL CInputGroup::IsSavic() const
{
	return m_bIsSaVic;
}
//////////////////////////////////////////////////////////////////////
BOOL CInputGroup::IsTasha() const
{
	return m_bIsTasha;
}
//////////////////////////////////////////////////////////////////////
BOOL CInputGroup::IsQ() const
{
	return m_bIsQ;
}
//////////////////////////////////////////////////////////////////////
int	CInputGroup::GetOutstandingPicts()
{
	int iSum = 0;
	Lock();
	for (int i=0;i<GetSize();i++)
	{
		CInput* pInput = GetAtFast(i);
		iSum += pInput->GetOutstandingPicts();
	}
	Unlock();
	return iSum;
}
//////////////////////////////////////////////////////////////////////
void CInputGroup::Load(CWK_Profile& wkp)
{
	CString sEntry;
	CInput* pInput = NULL;

	for (int i=0;i<GetSize();i++)
	{
		pInput = GetAtFast(i);
		sEntry = wkp.GetString(GetSection(),pInput->GetKey(),_T(""));
		pInput->FromString(sEntry);
	}
}
//////////////////////////////////////////////////////////////////////
void CInputGroup::Save(CWK_Profile& wkp,BOOL bShutdown)
{
	CInput* pInput = NULL;
	for (int i=0;i<GetSize();i++)
	{
		pInput = GetAtFast(i);
		wkp.WriteString(GetSection(),
						pInput->GetKey(),
						pInput->ToString(bShutdown));
	}
}
//////////////////////////////////////////////////////////////////////
void CInputGroup::CheckActive()
{
	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->CheckActive();
	}
}
//////////////////////////////////////////////////////////////////////
void CInputGroup::ClearActive()
{
	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->ClearActive();
	}
}
//////////////////////////////////////////////////////////////////////
void CInputGroup::OnReadChannelFound()
{
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////
void CInputGroup::OnConfirmConnection()
{
	WK_TRACE(_T("connected to %s\n"), GetShmName());
	CIPC::OnConfirmConnection();
	DoRequestSetGroupID(m_wGroupID);
}
//////////////////////////////////////////////////////////////////////
void CInputGroup::OnConfirmSetGroupID(WORD wGroupID)
{
	CString sShm = GetShmName();
	if (sShm == SM_COMMUNIT_INPUT)
	{
		DoRequestReset(m_wGroupID);
	}
	DoRequestHardware(m_wGroupID);
}
//////////////////////////////////////////////////////////////////////
void CInputGroup::OnConfirmHardware(WORD wGroupID,int iErrorCode)
{
	m_iHardware = iErrorCode;

	CString sShm = GetShmName();
	if (   m_iHardware==0
		&& sShm != SM_COMMUNIT_INPUT)
	{
		// TODO opener, closer alarm input
		DoRequestSetEdge(m_wGroupID, 0x00);
	}

}
//////////////////////////////////////////////////////////////////////
void CInputGroup::OnConfirmEdge(WORD wGroupID,DWORD edgeBitmask)	// 1 positive, 0 negative
{
	DoRequestSetFree(m_wGroupID, m_dwFree);
}
//////////////////////////////////////////////////////////////////////
void CInputGroup::OnConfirmFree(WORD wGroupID,DWORD openBitmask)	// 1 open, 0 closed
{
	DoRequestReset(m_wGroupID);
}
//////////////////////////////////////////////////////////////////////
void CInputGroup::OnConfirmReset(WORD wGroupID)
{
	m_dwState = 0;
	DoRequestAlarmState(m_wGroupID);
}
//////////////////////////////////////////////////////////////////////
void CInputGroup::OnRequestDisconnect()
{
	m_iHardware = -1;
}
//////////////////////////////////////////////////////////////////////
void CInputGroup::OnConfirmAlarmState(WORD wGroupID,DWORD inputMask)
{
	m_dwState = inputMask;
	if (IsQ())
	{
		m_dwState = 0;
	}


	WK_TRACE(_T("Initial input states %s %08lx\n"),GetShmName(),m_dwState);
	// reset all inputs
	DWORD dwOneBit=1;
	
	for (WORD i=0;i<GetSize();i++, dwOneBit <<= 1) 
	{
		CInput *pInput = GetAtFast(i);
		// set the new state
		BOOL bActive = (m_dwState & dwOneBit)!=0;
		TRACE(_T("%s is %s\n"),pInput->GetName(),bActive?_T("active"):_T("inactive"));
		if (bActive)
		{
			pInput->SetActive(bActive);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CInputGroup::OnIndicateAlarmState(WORD wGroupID,
									   DWORD inputMask, 
									   DWORD changeMask,
									   LPCTSTR szInfoString)
{
	if (!theApp.IsUpAndRunning())
	{
		return;
	}

	if (!m_bMD)
	{
		IndicateAlarmToClients(inputMask,changeMask);
	}

	Lock();
	// calc changes from own data and cmp with received changes
	DWORD myChangeMask = m_dwState ^ inputMask;
	if (myChangeMask!=changeMask) 
	{
		// OOPS %x vs %s
		WK_TRACE(_T("%s: different change masks my %s != unit %s\n"),GetShmName(),BinaryString(myChangeMask,GetSize()),BinaryString(changeMask,GetSize()));
		// CAVEAT uses OWN mask instead of received mask
		changeMask = myChangeMask;	
	}
	m_dwState = inputMask;	// Bitmask-Status der Eingänge

	DWORD dwOneBit = 1;
	for (WORD i=0;i<GetSize();i++, dwOneBit <<= 1) 
	{
		if (changeMask & dwOneBit)	
		{
			CInput *pInput = GetAtFast(i);
			// set the new state
			pInput->SetActive((m_dwState & dwOneBit)!=0);
		}
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CInputGroup::OnIndicateAlarmFieldsState(WORD wGroupID,
											 DWORD inputMask, 	// 1 high, 0 low
											 DWORD changeMask,	// 1 changed, 0 unchanged
											 int iNumFields,
											 const CIPCField fields[])
{
	if (!theApp.IsUpAndRunning())
	{
		return;
	}

	if (!m_bMD)
	{
		IndicateAlarmToClients(inputMask,changeMask);
	}

	Lock();
	// calc changes from own data and cmp with received changes
	DWORD myChangeMask = m_dwState ^ inputMask;
	if (myChangeMask!=changeMask) 
	{
		// RKE da die Bewegungsalarme nur für jedes x-te Bild gesendet werden, so sind die Changemasks
		// unterschiedlich.
		// Das ist auch der Fall, wenn Alarme aus Performancegründen nicht abgesetzt werden können.
		// Ausserdem wird die dann eigene ChangeMask verwendet, so dass nach einem Fehler die Maske
		// aus dem Tritt gekommen ist.
//		WK_TRACE(_T("%s: different change masks my %s != unit %s\n"),GetShmName(),BinaryString(myChangeMask,GetSize()),BinaryString(changeMask,GetSize()));
		// CAVEAT uses OWN mask instead of received mask
//		changeMask = myChangeMask;	
	}
	m_dwState = inputMask;	// Bitmask-Status der Eingänge

	DWORD dwOneBit = 1;
	CIPCFields myFields;
	myFields.FromArray(iNumFields,fields);
	for (int j=0;j<myFields.GetSize();j++)
	{
		CString sName = myFields[j]->GetName();
		sName.Replace(_T("DBD_"),_T("DVD_"));
		myFields[j]->SetName(sName);
	}
	for (WORD i=0;i<GetSize();i++, dwOneBit <<= 1) 
	{
		if (changeMask & dwOneBit)	
		{
			CInput *pInput = GetAtFast(i);

			// set the new state
			pInput->SetActive((m_dwState & dwOneBit)!=0,myFields);
		}
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CInputGroup::OnIndicateAlarmFieldsUpdate(CSecID id,
												int iNumFields,
												const CIPCField fields[])
{
	if (id.GetSubID()<GetSize())
	{
		CInput *pInput = GetAt(id.GetSubID());

		if (   pInput
			&& pInput->IsActive()) 
		{
//			TRACE(_T("OnIndicateAlarmFieldsUpdate %d\n"),id.GetSubID());
			CIPCFields& myFields = pInput->GetFields();
			myFields.Lock();
			myFields.DeleteAll();
			myFields.FromArray(iNumFields,fields);
			for (int j=0;j<myFields.GetSize();j++)
			{
				CString sName = myFields[j]->GetName();
				sName.Replace(_T("DBD_"),_T("DVD_"));
				myFields[j]->SetName(sName);
			}
			myFields.Unlock();
			pInput->SetOutstandingPicts();
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CInputGroup::OnIndicateAlarmNr(CSecID id,
								    BOOL bAlarm,
								    DWORD dwData1,
								    DWORD dwData2)
{
	if (!theApp.IsUpAndRunning())
	{
		return;
	}

	Lock();

	WORD wNr = id.GetSubID();

	if (wNr<GetSize())
	{
		CInput *pInput = GetAtFast(id.GetSubID());
		
		// aktuelle Bitmaske der Stati aktualisieren
		DWORD dwBit = 1 << pInput->GetUnitID().GetSubID();

		if (bAlarm)
		{
			m_dwState |= dwBit;
		}
		else
		{
			m_dwState &= ~dwBit;
		}

		// set the new state
		pInput->SetActive(bAlarm,dwData1,dwData2);
	}
	
//	TRACE(_T("alarm %08lx, %08lx,%08lx\n"),id.GetID(),dwData1,dwData2);

	Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CInputGroup::OnConfirmGetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwServerData)
{
	CInputClients& clients = theApp.GetInputClients();

	clients.Lock();

	for (int i=0;i<clients.GetSize();i++) 
	{
		CInputClient* pClient = clients.GetAtFast(i);
		if (pClient->IsConnected())
		{
			pClient->GetCIPC()->DoConfirmGetValue(id, sKey, sValue, dwServerData);
		}
	}	// end of client loop

	clients.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CInputGroup::IndicateAlarmToClients(DWORD inputMask, 	// 1 high, 0 low
										 DWORD changeMask)	// 1 changed, 0 unchanged
{
	// inform all clients
	CInputClients& clients = theApp.GetInputClients();

	clients.Lock();

	for (int i=0;i<clients.GetSize();i++) 
	{
		CInputClient* pClient = clients.GetAtFast(i);
		if (pClient->IsConnected())
		{
			pClient->GetCIPC()->DoIndicateAlarmState(GetID(),inputMask,changeMask,NULL);
		}
	}	// end of client loop

	clients.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CInputGroup::IsInputActive(CSecID id)
{
	WORD wNr = id.GetSubID();
	DWORD dwBit = 1 << wNr;

	if (m_dwState & dwBit)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
CInput* CInputGroup::GetInputByUnitID(CSecID id)
{
	CInput* pInput = NULL;
	Lock();
	for (int j=0;j<GetSize();j++)
	{
		pInput = GetAtFast(j);
		if (pInput->GetUnitID() == id)
		{
			break;
		}
		pInput = NULL;
	}

	Unlock();
	return pInput;
}

