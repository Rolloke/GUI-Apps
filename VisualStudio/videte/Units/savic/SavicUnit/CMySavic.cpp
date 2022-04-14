// CMySaVic.cpp: implementation of the CMySaVic class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CMySavic.h"
#include "SavicUnitDlg.h"

/////////////////////////////////////////////////////////////////////////////
CMySaVic::CMySaVic(CSaVicUnitDlg* pWnd, const CString &sAppIniName, CPCIDevice* pDevice) : CCodec(sAppIniName, pDevice)
{
	m_pMainWnd = pWnd;
}

/////////////////////////////////////////////////////////////////////////////
void CMySaVic::OnReceivedImageData(const IMAGE* pImage) const
{
	if (m_pMainWnd)
		m_pMainWnd->OnReceivedImageData(pImage);
}

/////////////////////////////////////////////////////////////////////////////
void CMySaVic::OnReceivedEncoderError(int nErrorCode) const
{
	if (m_pMainWnd)
		m_pMainWnd->OnReceivedEncoderError(nErrorCode);
}

/////////////////////////////////////////////////////////////////////////////
void CMySaVic::OnReceivedAlarm(WORD wCurrentAlarmState) const
{
	if (m_pMainWnd)
		m_pMainWnd->OnReceivedAlarm(wCurrentAlarmState);
}

/////////////////////////////////////////////////////////////////////////////
void CMySaVic::OnReceivedCameraStatus(WORD wSource, BOOL bStatus) const
{
	if (m_pMainWnd)
		m_pMainWnd->OnReceivedCameraStatus(wSource, bStatus);
}

/////////////////////////////////////////////////////////////////////////////
void CMySaVic::OnReceivedMotionAlarm(WORD wSource, CMDPoints& Points) const
{
	if (m_pMainWnd)
		m_pMainWnd->OnReceivedMotionAlarm(wSource, Points);
}

/////////////////////////////////////////////////////////////////////////////
void CMySaVic::OnPowerOffButtonPressed() const
{
	if (m_pMainWnd)
		m_pMainWnd->OnPowerOffButtonPressed();
}

/////////////////////////////////////////////////////////////////////////////
void CMySaVic::OnResetButtonPressed() const
{
	if (m_pMainWnd)
		m_pMainWnd->OnResetButtonPressed();
}

/////////////////////////////////////////////////////////////////////////////
void CMySaVic::OnReceivedRunTimeError(WK_RunTimeErrorLevel Level, WK_RunTimeError RTError, const CString &sError, DWORD dwParam1, DWORD dwParam2, WK_RunTimeErrorSubcode errorSubcode) const
{
	CWK_RunTimeError RTerr(Level, RTError, sError, dwParam1, dwParam2, errorSubcode);
	RTerr.Send();
}

/////////////////////////////////////////////////////////////////////////////
void CMySaVic::OnReceivedMotionDIB(WORD wSource, HANDLE hDIB)
{
	if (m_pMainWnd)
		m_pMainWnd->OnReceivedMotionDIB(wSource, hDIB);
}

