/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: Activation.cpp $
// ARCHIVE:		$Archive: /Project/SecurityServer/Activation.cpp $
// CHECKIN:		$Date: 9.11.05 20:35 $
// VERSION:		$Revision: 17 $
// LAST CHANGE:	$Modtime: 9.11.05 20:22 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "Activation.h"
#include "ProcessMacro.h"

#include "Process.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////
void CActivation::Init(int iIONr)
{ 
	m_iIONr = iIONr;
	m_activation = TOA_NONE;
	m_pInput=NULL;
	// m_idOutput
	// m_idProcessMacro
	// m_idTimer
	m_dwPriority = IO_DEFAULTPRIOR; 
	m_pActiveProcess = NULL;
	m_pProcessMacro = NULL;
	m_pTmpMacro = NULL;
	m_PTZCommand = CCC_INVALID;
} 
//////////////////////////////////////////////////////////////////
CActivation::CActivation(int iIONr)
{
	Init(iIONr);
}
//////////////////////////////////////////////////////////////////
CActivation::CActivation(CProcessMacro *pMacro,
						 CProcess *pProcess)
{
	Init(-1);
	m_activation = TOA_USER;
	m_pProcessMacro = pMacro;	//  set both one ist const
	m_pTmpMacro = pMacro;		// this one is only for client processes
	m_idProcessMacro = pMacro->GetID();
	SetActiveProcess( pProcess );
}
//////////////////////////////////////////////////////////////////
void CActivation::SetTypeOfActivation(TypeOfActivation toa)
{
	m_activation = toa;
}
//////////////////////////////////////////////////////////////////
void CActivation::SetInput(CInput *pInput)
{
	m_pInput = pInput;
}
//////////////////////////////////////////////////////////////////
void CActivation::SetOutputID(CSecID id)
{
	m_idOutput = id;
}
//////////////////////////////////////////////////////////////////
void CActivation::SetActiveProcess(CProcess *pProcess)
{
	if (   m_pActiveProcess
		&& pProcess
		&& m_pActiveProcess != pProcess)
	{
		WK_TRACE(_T("IBL: still have active process in activation %s %08lx\n"),
			m_pActiveProcess->GetDescription(FALSE),GetCurrentThreadId());
	}
	m_pActiveProcess = pProcess;
}
//////////////////////////////////////////////////////////////////
CActivation::~CActivation()
{
	// process should be removed
	if (m_pActiveProcess) 
	{
		WK_TRACE(_T("OOPS, still have process in activation !?\n"));
	}
	WK_DELETE(m_pTmpMacro);
}
//////////////////////////////////////////////////////////////////
void CActivation::SetPriority(DWORD dwPriority)
{
	m_dwPriority = dwPriority;
}
//////////////////////////////////////////////////////////////////
void CActivation::SetArchiveID(CSecID id)
{
	m_archiveID = id;
}
//////////////////////////////////////////////////////////////////
void CActivation::SetNotificationID(CSecID id)
{
	m_idNotification = id;
}
//////////////////////////////////////////////////////////////////
void CActivation::SetMacro(const CProcessMacro* pMacro)
{
	m_pProcessMacro = pMacro;
	m_idProcessMacro = pMacro->GetID();
}
//////////////////////////////////////////////////////////////////
void CActivation::SetCameraControlCommand(CameraControlCmd ccc)
{
	m_PTZCommand = ccc;
}
