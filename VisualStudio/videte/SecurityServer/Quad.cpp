#include "stdafx.h"
#include "quad.h"

BOOL CQuad::m_bTraceQuad = FALSE;


CQuad::CQuad(int iCom, QuadControlType qt, CComParameters* pComParameters /*= NULL*/) : CWK_RS232(iCom,m_bTraceQuad)
{
	m_iCom = iCom;
	m_eType = qt;
	m_bStatus = 0;
	m_pComParameters = NULL;
	//m_GroupIDs[i];
	for (int i=0;i<4;i++)
	{
		m_iVOUT[i] = 0;
	}

	if (pComParameters)
	{
		m_pComParameters = new CComParameters(*pComParameters);
	}
	else
	{
		switch (m_eType)
		{
		case QCT_SANTEC_SQV_80:
			m_pComParameters = new CComParameters(_T("Quad"),CBR_1200,8,NOPARITY,ONESTOPBIT);
			break;
		case QCT_GANZ_QD_04P:
			m_pComParameters = new CComParameters(_T("Quad"),CBR_9600,8,NOPARITY,ONESTOPBIT);
			break;
		}
	}
}
////////////////////////////////////////////////////////////////////////
CQuad::~CQuad()		  
{
	Close();
	Sleep(1000);
	WK_DELETE(m_pComParameters);
}
////////////////////////////////////////////////////////////////////////
void CQuad::Load(CWK_Profile& wkp)
{
	if (m_pComParameters)
	{
		m_pComParameters->LoadFromRegistry(wkp);
	}
	for (int i=0;i<4;i++)
	{
		CString sKey;
		sKey.Format(_T("Card%d"),i);
		m_GroupIDs[i] = (CSecID)(DWORD)wkp.GetInt(_T("Quad"),sKey,SECID_NO_ID);
		sKey.Format(_T("Out%d"),i);
		m_iVOUT[i] = wkp.GetInt(_T("Quad"),sKey,-1);
	}
}
////////////////////////////////////////////////////////////////////////
BOOL CQuad::Create()
{
	switch (m_eType)
	{
	case QCT_SANTEC_SQV_80:
		{
			Open(m_iCom, *m_pComParameters);
			Sleep(100);
			CByteArray cmd;
			cmd.Add('H');
			CWK_RS232::Write(cmd.GetData(),cmd.GetSize());
			Sleep(10);
			CWK_RS232::Write(cmd.GetData(),cmd.GetSize());
		}
		break;
	case QCT_GANZ_QD_04P:
		{
			Open(m_iCom, *m_pComParameters);
			Sleep(100);
		}
		break;
	default:
		break;
	}
	return IsOpen();
}
////////////////////////////////////////////////////////////////////////
void CQuad::SwitchCamSingle(int iCam)
{
	if (IsQuad()
		|| !IsCameraOn(iCam))
	{
		CByteArray cmd;
		switch (m_eType)
		{
		case QCT_SANTEC_SQV_80:
			if (IsQuad())
			{
				cmd.Add('E');
			}
			cmd.Add((BYTE)('A'+iCam));
			break;
		case QCT_GANZ_QD_04P:
			cmd.Add((BYTE)('1'+iCam));
//			cmd.Add((BYTE)'\r');
			// alle cameras und quad bit weg
			m_bStatus &= 0xF0;
			m_bStatus |= (BYTE)(1 << iCam);
			m_bStatus |= 0x10;
			m_bStatus |= 0x20;
			break;
		}

		if (cmd.GetSize())
		{
			CWK_RS232::Write(cmd.GetData(),cmd.GetSize());
		}
	}
}
////////////////////////////////////////////////////////////////////////
int CQuad::GetVideoOut(CSecID idGroup, int iPort)
{
	for (int i=0;i<4;i++)
	{
		if (   idGroup.GetGroupID() == m_GroupIDs[i].GetGroupID()
			&& iPort == m_iVOUT[i])
		{
			return i;
		}
	}
	return -1;
}
////////////////////////////////////////////////////////////////////////
void CQuad::SwitchQuad()
{
	if (!IsQuad())
	{
		CByteArray cmd;
		switch (m_eType)
		{
		case QCT_SANTEC_SQV_80:
			cmd.Add('E');
			break;
		case QCT_GANZ_QD_04P:
			cmd.Add('Q');
//			cmd.Add((BYTE)'\r');
			m_bStatus &= ~0x10;
			m_bStatus |= 0x20;
			break;
		}
		if (cmd.GetSize())
		{
			CWK_RS232::Write(cmd.GetData(),cmd.GetSize());
		}
	}
}
////////////////////////////////////////////////////////////////////////
void CQuad::OnReceivedData(LPBYTE pData, DWORD dwLen)
{
	switch (m_eType)
	{
	case QCT_SANTEC_SQV_80:
		m_bStatus = pData[0];

		if (m_bTraceQuad)
		{
			WK_TRACE(_T("QT_SANTEC_SQV_80 Status %02x\n"),m_bStatus);
			WK_TRACE(_T("QT_SANTEC_SQV_80 Locked %d\n"),IsLocked());
			WK_TRACE(_T("QT_SANTEC_SQV_80 %s\n"),IsVCRMode()?_T("VCR"):_T("Live"));
			if (IsSequencing())
			{
				WK_TRACE(_T("QT_SANTEC_SQV_80 sequencing\n"));
			}
			else
			{
				WK_TRACE(_T("QT_SANTEC_SQV_80 %s\n"),IsQuad()?_T("Quad"):_T("Fullscreen"));
			}
			for (int i=0;i<4;i++)
			{
				WK_TRACE(_T("QT_SANTEC_SQV_80 Cam %d %s\n"),i,IsCameraOn(i)?_T("ON"):_T("OFF"));
			}
		}
		break;
	}
}
