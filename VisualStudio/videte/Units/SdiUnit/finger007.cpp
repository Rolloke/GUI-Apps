// Finger007.cpp: implementation of the CFinger007 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SDIUnit.h"
#include "Finger007.h"
#include "starinterface.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFinger007::CFinger007(CStarInterface* pStarInterface,
					   short wWorkIndex,
					   LPCTSTR szBoardIndex,
					   LPCTSTR szCom)
{
	m_wWorkIndex = wWorkIndex;
	m_pStarInterface = pStarInterface;
	m_sCOM = szCom;
	m_sBoardIndex = szBoardIndex;

	PrepareCMD();
	/*
	TRACE(_T("GetControllerWeek=%s\n"),m_pStarInterface->GetControllerWeek());
	TRACE(_T("GetFinger=%s\n"),m_pStarInterface->GetFinger());
	TRACE(_T("GetFingerUseFlag=%s\n"),m_pStarInterface->GetFingerUseFlag());
	TRACE(_T("GetReader=%s\n"),m_pStarInterface->GetReader());*/

	m_pStarInterface->MasterCardUpload();
	TRACE(_T("GetMasterCard=%s\n"),m_pStarInterface->GetMasterCard());
	TRACE(_T("CommSpeedUpload=%s\n"),m_pStarInterface->CommSpeedUpload());
}
///////////////////////////////////////////////////////////////////////
CFinger007::~CFinger007()
{

}
///////////////////////////////////////////////////////////////////////
short CFinger007::GetBoardIndexShort() const
{
	short r = -1;

	_stscanf(m_sBoardIndex,_T("%02d"),&r);

	return r;
}
///////////////////////////////////////////////////////////////////////
void CFinger007::PrepareCMD()
{
	m_pStarInterface->SetWorkIndex(m_wWorkIndex);
	m_pStarInterface->SetBoardIndex(m_sBoardIndex);
}
///////////////////////////////////////////////////////////////////////
CString CFinger007::GetDate()
{
	PrepareCMD();
	CString s = m_pStarInterface->DateTimeUpload();
	CString d =	m_pStarInterface->GetControllerDate();

	return d;
}
///////////////////////////////////////////////////////////////////////
CString CFinger007::GetTime()
{
	PrepareCMD();
	CString s = m_pStarInterface->DateTimeUpload();
	CString d =	m_pStarInterface->GetControllerTime();

	return d;
}


