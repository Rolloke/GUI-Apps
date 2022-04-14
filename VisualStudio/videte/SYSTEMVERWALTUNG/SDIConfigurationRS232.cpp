// SDIConfigurationRS232.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "SDIConfigurationDialog.h"
#include "SDIConfigurationRS232.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSDIConfigurationRs232
CSDIConfigurationRS232::CSDIConfigurationRS232(CSDIConfigurationDialog* pParent, 
											  int iCom,
											  BOOL bTraceAscii/*=TRUE*/,
											  BOOL bTraceHex/*=FALSE*/,
											  BOOL bTraceEvents/*=FALSE*/)
	: CWK_RS232(iCom, bTraceAscii, bTraceHex, bTraceEvents)
{
	m_pSDIConfigurationDialog = pParent;
	m_bReadInData = FALSE;
	m_bStatusRequest = FALSE;
	ClearAllData();
}
/////////////////////////////////////////////////////////////////////////////
CSDIConfigurationRS232::~CSDIConfigurationRS232()
{
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationRS232::OnDataComplete()
{
	AddData(m_sDataComplete);
	m_pSDIConfigurationDialog->PostMessage(SDI_SAMPLE_READ_IN, 0, 0);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationRS232::StartReadInData()
{
	m_bReadInData = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationRS232::StopReadInData()
{
	m_bReadInData = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CString CSDIConfigurationRS232::GetAndRemoveData()
{
	m_CS.Lock();
	CString sData;
	if ( m_sDataArray.GetSize() > 0 ) {
		sData = m_sDataArray.GetAt(0);
		m_sDataArray.RemoveAt(0);
	}
	m_CS.Unlock();
	return sData;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationRS232::AddData(const CString& sData)
{
	m_CS.Lock();
	m_sDataArray.Add(sData);
	m_CS.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationRS232::OnTimeOutTransparent()
{
	m_pSDIConfigurationDialog->PostMessage(SDI_SAMPLE_READ_IN, (LPARAM)(_T("")), 0);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationRS232::ClearAllData()
{
	m_iReceivedTotal = 0;
	m_iLenDataActual = 0;
	m_byXORCheckSum = 0;
	m_iDataLen = 0;
	m_sBuffer.Empty();
	m_sDataComplete.Empty();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationRS232::ClearActualData()
{
	m_sBuffer.Empty();
	m_iLenDataActual = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationRS232::SendACK()
{
	Write( (LPVOID)m_byaACK.GetData(), m_byaACK.GetSize() );
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationRS232::SendNAK()
{
	Write( (LPVOID)m_byaNAK.GetData(), m_byaNAK.GetSize() );
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationRS232::RespondStatusRequest()
{
	// Jede Statusanforderung muss beantwortet werden
	// Vorerst wird >Alles ok< vorgespiegelt
	// OOPS todo muss evtl. noch veraendert werden,
	// z.B. um Kameraausfaelle anzuzeigen
	// OOPS Response wird z.Z. nur einmal bei Programmstart berechnet, um Zeit zu sparen

	Write( (LPVOID)m_byaStatusResponse.GetData(), m_byaStatusResponse.GetSize() );

	m_bStatusRequest = FALSE;
}
