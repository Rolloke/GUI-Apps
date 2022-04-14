// SDIConfigurationMicrolockRS232.cpp: implementation of the CSDIConfigurationMicrolockRS232 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "systemverwaltung.h"
#include "SDIConfigurationMicrolockRS232.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
CSDIConfigurationMicrolockRS232::CSDIConfigurationMicrolockRS232(CSDIConfigurationDialog* pParent,
																  int iCom,
																  BOOL bTraceAscii /*= TRUE*/,
																  BOOL bTraceHex /*= FALSE*/,
																  BOOL bTraceEvents /*= FALSE*/)
	: CSDIConfigurationRS232(pParent, iCom, bTraceAscii, bTraceHex, bTraceEvents)
{
	m_sEndOfLine += ASCII_CR;
	m_sEndOfLine += ASCII_LF;
	m_iEndOfLineLen = m_sEndOfLine.GetLength();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationMicrolockRS232::OnReceivedData(LPBYTE pData, DWORD dwLen)
{
	if (m_bReadInData == FALSE) {
		return;
	}

	DWORD dw = 0;
	while (pData && dw<dwLen)
	{
		// Alle druckbaren Zeichen in den Buffer sichern
		if (   isprint(pData[dw])
			|| pData[dw] == ASCII_CR
			|| pData[dw] == ASCII_LF
			)
		{
			m_sDataComplete += pData[dw];
		}
		else
		{
			CString sMsg;
			sMsg.Format(_T("COM%i received 0x%02x %s"), GetCOMNumber(), pData[dw], GetAsciiSymbol(pData[dw]));
			WK_STAT_PEAK(_T("AsciiInvalid"), 1, sMsg);
			// Wird hier ignoriert
		}
		// Wenn ein Trennzeichen kommt, ist ein Datensatz komplett
		if (m_sDataComplete.Right(m_iEndOfLineLen) == m_sEndOfLine)
		{
			int iCompleteLen = m_sDataComplete.GetLength();
			m_sDataComplete = m_sDataComplete.Left(iCompleteLen - m_iEndOfLineLen);
			OnDataComplete();
			ClearAllData();
		}
		dw++;
	} // while
}
