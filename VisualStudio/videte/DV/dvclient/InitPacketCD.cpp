// InitPacketCD.cpp: implementation of the CInitPacketCD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InitPacketCD.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInitPacketCD::CInitPacketCD()
{
	//actual version of PacketCD:		250.035
	//actual version of Formatierer:	2, 0, 0, 38
	//if the versions changes, check the new ID`s and the new buttontexts with spy++

	m_dwMaxCount = 30*1000;

//initializes variables for function _T("Format")
	CSystemTime sysTime;
	CString sDateTime;
	sysTime.GetLocalTime();
	sDateTime.Format(_T("%02d_%02d_%4d, %02d:%02d:%02d"),
			sysTime.GetDay(),
			sysTime.GetMonth(),
			sysTime.GetYear(),
			sysTime.GetHour(),
			sysTime.GetMinute(),
			sysTime.GetSecond()
			);
	m_sWndTxtPacketCD	= _T("PacketCD Formatierer");
	m_sFormate_ButtonTxt= _T("&Formatiere");
	m_sFertig_ButtonTxt	= _T("&Fertig stellen");
	m_sNewCDName		= sDateTime;

	m_wEdit_FieldID		= 0x0425;
	m_wFormate_ButtonID	= 0x3024;
	m_wFertig_ButtonID	= 0x3025;

	m_hPacketCDWnd				= NULL;
	m_hPacketCDChildWnd			= NULL;

//initializes variables for function _T("Finalize")
	m_hFirstFinalizeDlg				= NULL;
	m_sTxtFirstFinalizeDlg			= _T("PacketCD");
	m_sTxtSecondFinalizeDlg			= _T("Finalisiere UDF CD");
	m_wYesButtonID_FirstFinalizeDlg	= 0x0006;
}

CInitPacketCD::~CInitPacketCD()
{

}

//////////////////////////////////////////////////////////////////////
void CInitPacketCD::SetHandleFirstFinalizeDlg(HWND hWnd)
{
	m_hFirstFinalizeDlg = hWnd;
}

//////////////////////////////////////////////////////////////////////
void CInitPacketCD::SetHandlePacketCDWnd(HWND hWnd)
{
	m_hPacketCDWnd = hWnd;
}

//////////////////////////////////////////////////////////////////////
void CInitPacketCD::SetHandlePacketCDChildWnd(HWND hWnd)
{
	m_hPacketCDChildWnd = hWnd;
}
