// StandAlone.cpp : implementation file

#include "stdafx.h"
#include "mmsystem.h"
#include "CMiCo.h"
#include "MiCoUnitApp.h"
#include "MiCoUnitDlg.h"
#include "Settings.h"
		   
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
BOOL CMiCoUnitDlg::UpdateDlg()
{
	if (!IsValid())
		return FALSE;

	if (!m_pMiCo)
		return TRUE;

	WORD wSource	= 0;
	WORD wExtCard	= 0;
	WORD wEdge		= 0;
	WORD wAck		= 0;
	WORD wRelais	= 0;

	m_pMiCo->GetInputSource(wExtCard, wSource);

	WORD  wFeVideoFormat= m_pMiCo->GetFeVideoFormat();
	WORD  wFeSourceType = m_pMiCo->GetFeSourceType();
	WORD  wBeVideoFormat= m_pMiCo->GetBeVideoFormat();
	WORD  wBeVideoType  = m_pMiCo->GetBeVideoType();
	WORD  wFeFilter		= m_pMiCo->GetFeFilter();
	
	WORD  wFormat		= m_pMiCo->EncoderGetFormat();
	DWORD wBPF			= m_pMiCo->EncoderGetBPF();

	wEdge				= m_pMiCo->GetAlarmEdge(wExtCard);
	wAck				= m_pMiCo->GetAlarmAck(wExtCard);
	wRelais				= m_pMiCo->GetRelais(wExtCard);

	switch (wFormat)
	{
		case MICO_ENCODER_HIGH:
			CheckRadioButton(IDC_HIGH_RES, IDC_LOW_RES, IDC_HIGH_RES);
			break;
		case MICO_ENCODER_LOW:
			CheckRadioButton(IDC_HIGH_RES, IDC_LOW_RES, IDC_LOW_RES);
			break;
	}
	
   switch(wBPF)
   {
		case MICO_BPF_1:
			CheckRadioButton(IDC_COMPRESS_1, IDC_COMPRESS_5, IDC_COMPRESS_1);
			break;
		case MICO_BPF_2:
			CheckRadioButton(IDC_COMPRESS_1, IDC_COMPRESS_5, IDC_COMPRESS_2);
			break;
		case MICO_BPF_3:
			CheckRadioButton(IDC_COMPRESS_1, IDC_COMPRESS_5, IDC_COMPRESS_3);
			break;
		case MICO_BPF_4:
			CheckRadioButton(IDC_COMPRESS_1, IDC_COMPRESS_5, IDC_COMPRESS_4);
			break;
		case MICO_BPF_5:
			CheckRadioButton(IDC_COMPRESS_1, IDC_COMPRESS_5, IDC_COMPRESS_5);
			break;
	}

	switch(wSource)
	{
		case MICO_SOURCE0:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE7, IDC_SOURCE1);
			break;
		case MICO_SOURCE1:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE7, IDC_SOURCE2);
			break;
		case MICO_SOURCE2:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE7, IDC_SOURCE3);
			break;
		case MICO_SOURCE3:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE7, IDC_SOURCE4);
			break;
		case MICO_SOURCE4:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE7, IDC_SOURCE5);
			break;
		case MICO_SOURCE5:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE7, IDC_SOURCE6);
			break;
		case MICO_SOURCE6:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE7, IDC_SOURCE7);
			break;
		case MICO_SOURCE7:
			CheckRadioButton(IDC_SOURCE1, IDC_SOURCE7, IDC_SOURCE8);
			break;
	}

	switch(wExtCard)
	{
		case MICO_EXTCARD0:
			CheckRadioButton(IDC_EXTCARD0, IDC_EXTCARDMEGRA, IDC_EXTCARD0);
			break;
		case MICO_EXTCARD1:
			CheckRadioButton(IDC_EXTCARD0, IDC_EXTCARDMEGRA, IDC_EXTCARD1);
			break;
		case MICO_EXTCARD2:
			CheckRadioButton(IDC_EXTCARD0, IDC_EXTCARDMEGRA, IDC_EXTCARD2);
			break;
		case MICO_EXTCARD3:
			CheckRadioButton(IDC_EXTCARD0, IDC_EXTCARDMEGRA, IDC_EXTCARD3);
			break;
	}

	switch(wFeVideoFormat)
	{
	 	case MICO_PAL_CCIR:
			CheckRadioButton(IDC_FE_PAL_CCIR, IDC_FE_NTSC_SQUARE, IDC_FE_PAL_CCIR);
			break;
		case MICO_NTSC_CCIR:
			CheckRadioButton(IDC_FE_PAL_CCIR, IDC_FE_NTSC_SQUARE, IDC_FE_NTSC_CCIR);
			break;
	 	case MICO_PAL_SQUARE:
			CheckRadioButton(IDC_FE_PAL_CCIR, IDC_FE_NTSC_SQUARE, IDC_FE_PAL_SQUARE);
			break;
		case MICO_NTSC_SQUARE:
			CheckRadioButton(IDC_FE_PAL_CCIR, IDC_FE_NTSC_SQUARE, IDC_FE_NTSC_SQUARE);
			break;
	}

	switch(wFeSourceType)
	{
	 	case MICO_FBAS:
			CheckRadioButton(IDC_FE_FBAS, IDC_FE_SVHS, IDC_FE_FBAS);
			break;
		case MICO_SVHS:
			CheckRadioButton(IDC_FE_FBAS, IDC_FE_SVHS, IDC_FE_SVHS);
			break;
	}

	switch(wBeVideoFormat)
	{
	 	case MICO_PAL_CCIR:
			CheckRadioButton(IDC_BE_PAL_CCIR, IDC_BE_NTSC_SQUARE, IDC_BE_PAL_CCIR);
			break;
		case MICO_NTSC_CCIR:
			CheckRadioButton(IDC_BE_PAL_CCIR, IDC_BE_NTSC_SQUARE, IDC_BE_NTSC_CCIR);
			break;
	 	case MICO_PAL_SQUARE:
			CheckRadioButton(IDC_BE_PAL_CCIR, IDC_BE_NTSC_SQUARE, IDC_BE_PAL_SQUARE);
			break;
		case MICO_NTSC_SQUARE:
			CheckRadioButton(IDC_BE_PAL_CCIR, IDC_BE_NTSC_SQUARE, IDC_BE_NTSC_SQUARE);
			break;
	}

	switch(wBeVideoType)
	{	
		case MICO_COMPOSITE:
			CheckRadioButton(IDC_BE_COMPOSITE, IDC_BE_RGB, IDC_BE_COMPOSITE);
			break;
	 	case MICO_RGB:
			CheckRadioButton(IDC_BE_COMPOSITE, IDC_BE_RGB, IDC_BE_RGB);
			break;
	}

	if (wFeFilter & MICO_LNOTCH)
		CheckDlgButton(IDC_FE_LNOTCH, 1);
	else
		CheckDlgButton(IDC_FE_LNOTCH, 0);

	if (wFeFilter & MICO_LDEC)
		CheckDlgButton(IDC_FE_LDEC, 1);
	else
		CheckDlgButton(IDC_FE_LDEC, 0);

	
	if (wEdge & 0x01)
		CheckDlgButton(IDC_EDGE0, 1);
	else
		CheckDlgButton(IDC_EDGE0, 0);

	if (wEdge & 0x02)
		CheckDlgButton(IDC_EDGE1, 1);
	else
		CheckDlgButton(IDC_EDGE1, 0);

	if (wEdge & 0x04)
		CheckDlgButton(IDC_EDGE2, 1);
	else
		CheckDlgButton(IDC_EDGE2, 0);

	if (wEdge & 0x08)
		CheckDlgButton(IDC_EDGE3, 1);
	else
		CheckDlgButton(IDC_EDGE3, 0);

	if (wEdge & 0x10)
		CheckDlgButton(IDC_EDGE4, 1);
	else
		CheckDlgButton(IDC_EDGE4, 0);

	if (wEdge & 0x20)
		CheckDlgButton(IDC_EDGE5, 1);
	else
		CheckDlgButton(IDC_EDGE5, 0);

	if (wEdge & 0x40)
		CheckDlgButton(IDC_EDGE6, 1);
	else
		CheckDlgButton(IDC_EDGE6, 0);

	if (wEdge & 0x80)
		CheckDlgButton(IDC_EDGE7, 1);
	else
		CheckDlgButton(IDC_EDGE7, 0);


	if (wAck & 0x01)
		CheckDlgButton(IDC_ACK0, 1);
	else
		CheckDlgButton(IDC_ACK0, 0);

	if (wAck & 0x02)
		CheckDlgButton(IDC_ACK1, 1);
	else
		CheckDlgButton(IDC_ACK1, 0);

	if (wAck & 0x04)
		CheckDlgButton(IDC_ACK2, 1);
	else
		CheckDlgButton(IDC_ACK2, 0);

	if (wAck & 0x08)
		CheckDlgButton(IDC_ACK3, 1);
	else
		CheckDlgButton(IDC_ACK3, 0);

	if (wAck & 0x10)
		CheckDlgButton(IDC_ACK4, 1);
	else
		CheckDlgButton(IDC_ACK4, 0);

	if (wAck & 0x20)
		CheckDlgButton(IDC_ACK5, 1);
	else
		CheckDlgButton(IDC_ACK5, 0);

	if (wAck & 0x40)
		CheckDlgButton(IDC_ACK6, 1);
	else
		CheckDlgButton(IDC_ACK6, 0);

	if (wAck & 0x80)
		CheckDlgButton(IDC_ACK7, 1);
	else
		CheckDlgButton(IDC_ACK7, 0);


	if (wRelais & 0x01)
		CheckDlgButton(IDC_RELAIS0, 1);
	else
		CheckDlgButton(IDC_RELAIS0, 0);

	if (wRelais & 0x02)
		CheckDlgButton(IDC_RELAIS1, 1);
	else
		CheckDlgButton(IDC_RELAIS1, 0);

	if (wRelais & 0x04)
		CheckDlgButton(IDC_RELAIS2, 1);
	else
		CheckDlgButton(IDC_RELAIS2, 0);
	
	if (wRelais & 0x08)
		CheckDlgButton(IDC_RELAIS3, 1);
	else
		CheckDlgButton(IDC_RELAIS3, 0);

	DWORD dwHardwareStateState = m_pMiCo->GetHardwareState();

	if (TSTBIT(dwHardwareStateState,0))
		GetDlgItem(IDC_EXTCARD0)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_EXTCARD0)->EnableWindow(FALSE);

	if (TSTBIT(dwHardwareStateState,1))
		GetDlgItem(IDC_EXTCARD1)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_EXTCARD1)->EnableWindow(FALSE);
	
	if (TSTBIT(dwHardwareStateState,2))
		GetDlgItem(IDC_EXTCARD2)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_EXTCARD2)->EnableWindow(FALSE);

	if (TSTBIT(dwHardwareStateState,3))
		GetDlgItem(IDC_EXTCARD3)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_EXTCARD3)->EnableWindow(FALSE);
	

	if (m_nEncoderState == MICO_ENCODER_ON)
	{
		GetDlgItem(IDC_ENC_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_ENC_STOP)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_ENC_START)->EnableWindow(TRUE);
		GetDlgItem(IDC_ENC_STOP)->EnableWindow(FALSE);
	}	
	
	if (m_nDecoderState == MICO_DECODER_ON)
	{
		GetDlgItem(IDC_DEC_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_DEC_STOP)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_DEC_START)->EnableWindow(TRUE);
		GetDlgItem(IDC_DEC_STOP)->EnableWindow(FALSE);
	}	

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnBePalCCIR()
{
	if (m_pMiCo)
		m_pMiCo->SetBeVideoFormat(MICO_PAL_CCIR);	
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnBeNtscCCIR() 
{
	if (m_pMiCo)
		m_pMiCo->SetBeVideoFormat(MICO_NTSC_CCIR);	
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnBePalSquare()
{
	if (m_pMiCo)
		m_pMiCo->SetBeVideoFormat(MICO_PAL_SQUARE);	
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnBeNtscSquare() 
{
	if (m_pMiCo)
		m_pMiCo->SetBeVideoFormat(MICO_NTSC_SQUARE);	
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnBeComposite() 
{
	if (m_pMiCo)
		m_pMiCo->SetBeVideoType(MICO_COMPOSITE);	
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnBeRgb() 
{
	if (m_pMiCo)
		m_pMiCo->SetBeVideoType(MICO_RGB);	
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnDlgSettings() 
{
	m_pSettings = new CSettings(m_pMiCo, this);
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnExtcard0() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);	
		m_pMiCo->SetInputSource(MICO_EXTCARD0, wSource, m_wFormat, m_dwBPF, 0);	
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnExtcard1() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);	
		m_pMiCo->SetInputSource(MICO_EXTCARD1, wSource, m_wFormat, m_dwBPF, 0);	
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnExtcard2() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);	
		m_pMiCo->SetInputSource(MICO_EXTCARD2, wSource, m_wFormat, m_dwBPF, 0);	
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnExtcard3() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);	
		m_pMiCo->SetInputSource(MICO_EXTCARD3, wSource, m_wFormat, m_dwBPF, 0);	
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnFeFbas() 
{
	if (m_pMiCo)
		m_pMiCo->SetFeSourceType(MICO_FBAS);	
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnFeSvhs() 
{
	if (m_pMiCo)
		m_pMiCo->SetFeSourceType(MICO_SVHS);	
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnFePalCCIR() 
{
	if (m_pMiCo)
		m_pMiCo->SetFeVideoFormat(MICO_PAL_CCIR);	
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnFeNtscCCIR() 
{
	if (m_pMiCo)
		m_pMiCo->SetFeVideoFormat(MICO_NTSC_CCIR);	
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnFePalSquare() 
{
	if (m_pMiCo)
		m_pMiCo->SetFeVideoFormat(MICO_PAL_SQUARE);	
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnFeNtscSquare() 
{
	if (m_pMiCo)
		m_pMiCo->SetFeVideoFormat(MICO_NTSC_SQUARE);	
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnFeLNotch() 
{
	if (m_pMiCo)
		m_pMiCo->SetFeFilter(m_pMiCo->GetFeFilter() ^MICO_LNOTCH);	
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnFeLDec() 
{
	if (m_pMiCo)
		m_pMiCo->SetFeFilter(m_pMiCo->GetFeFilter() ^MICO_LDEC);	
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnSource1() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);	
		m_pMiCo->SetInputSource(wExtCard, MICO_SOURCE0, m_wFormat, m_dwBPF, 0);	
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnSource2() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);	
		m_pMiCo->SetInputSource(wExtCard, MICO_SOURCE1, m_wFormat, m_dwBPF, 0);	
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnSource3() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);	
		m_pMiCo->SetInputSource(wExtCard, MICO_SOURCE2, m_wFormat, m_dwBPF, 0);	
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnSource4() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);	
		m_pMiCo->SetInputSource(wExtCard, MICO_SOURCE3, m_wFormat, m_dwBPF, 0);	
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnSource5() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);	
		m_pMiCo->SetInputSource(wExtCard, MICO_SOURCE4, m_wFormat, m_dwBPF, 0);	
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnSource6() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);	
		m_pMiCo->SetInputSource(wExtCard, MICO_SOURCE5, m_wFormat, m_dwBPF, 0);	
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnSource7() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);	
		m_pMiCo->SetInputSource(wExtCard, MICO_SOURCE6, m_wFormat, m_dwBPF, 0);	
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnSource8() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);	
		m_pMiCo->SetInputSource(wExtCard, MICO_SOURCE7, m_wFormat, m_dwBPF, 0);	
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnEdge7() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);
		
		BYTE byAlarmEdge = m_pMiCo->GetAlarmEdge(wExtCard);
		byAlarmEdge = byAlarmEdge^0x80;
		m_pMiCo->SetAlarmEdge(wExtCard, byAlarmEdge);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnEdge6() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);

		BYTE byAlarmEdge = m_pMiCo->GetAlarmEdge(wExtCard);
		byAlarmEdge = byAlarmEdge^0x40;
		m_pMiCo->SetAlarmEdge(wExtCard, byAlarmEdge);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnEdge5() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);

		BYTE byAlarmEdge = m_pMiCo->GetAlarmEdge(wExtCard);
		byAlarmEdge = byAlarmEdge^0x20;
		m_pMiCo->SetAlarmEdge(wExtCard, byAlarmEdge);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnEdge4() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);

		BYTE byAlarmEdge = m_pMiCo->GetAlarmEdge(wExtCard);
		byAlarmEdge = byAlarmEdge^0x10;
		m_pMiCo->SetAlarmEdge(wExtCard, byAlarmEdge);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnEdge3() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);

		BYTE byAlarmEdge = m_pMiCo->GetAlarmEdge(wExtCard);
		byAlarmEdge = byAlarmEdge^0x08;
		m_pMiCo->SetAlarmEdge(wExtCard, byAlarmEdge);
	}
}
	    
/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnEdge2() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);

		BYTE byAlarmEdge = m_pMiCo->GetAlarmEdge(wExtCard);
		byAlarmEdge = byAlarmEdge^0x04;
		m_pMiCo->SetAlarmEdge(wExtCard, byAlarmEdge);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnEdge1() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);

		BYTE byAlarmEdge = m_pMiCo->GetAlarmEdge(wExtCard);
		byAlarmEdge = byAlarmEdge^0x02;
		m_pMiCo->SetAlarmEdge(wExtCard, byAlarmEdge);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnEdge0() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);

		BYTE byAlarmEdge = m_pMiCo->GetAlarmEdge(wExtCard);
		byAlarmEdge = byAlarmEdge^0x01;
		m_pMiCo->SetAlarmEdge(wExtCard, byAlarmEdge);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnAck0() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);

		BYTE byAlarmAck  = m_pMiCo->GetAlarmAck(wExtCard);
		byAlarmAck = byAlarmAck^0x01;
		m_pMiCo->SetAlarmAck(wExtCard, byAlarmAck);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnAck1() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);

		BYTE byAlarmAck  = m_pMiCo->GetAlarmAck(wExtCard);
		byAlarmAck = byAlarmAck^0x02;
		m_pMiCo->SetAlarmAck(wExtCard, byAlarmAck);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnAck2() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);

		BYTE byAlarmAck  = m_pMiCo->GetAlarmAck(wExtCard);
		byAlarmAck = byAlarmAck^0x04;
		m_pMiCo->SetAlarmAck(wExtCard, byAlarmAck);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnAck3() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);

		BYTE byAlarmAck  = m_pMiCo->GetAlarmAck(wExtCard);
		byAlarmAck = byAlarmAck^0x08;
		m_pMiCo->SetAlarmAck(wExtCard, byAlarmAck);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnAck4() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);

		BYTE byAlarmAck  = m_pMiCo->GetAlarmAck(wExtCard);
		byAlarmAck = byAlarmAck^0x10;
		m_pMiCo->SetAlarmAck(wExtCard, byAlarmAck);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnAck5() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);

		BYTE byAlarmAck  = m_pMiCo->GetAlarmAck(wExtCard);
		byAlarmAck = byAlarmAck^0x20;
		m_pMiCo->SetAlarmAck(wExtCard, byAlarmAck);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnAck6() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);

		BYTE byAlarmAck  = m_pMiCo->GetAlarmAck(wExtCard);
		byAlarmAck = byAlarmAck^0x40;
		m_pMiCo->SetAlarmAck(wExtCard, byAlarmAck);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnAck7() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);

		BYTE byAlarmAck  = m_pMiCo->GetAlarmAck(wExtCard);
		byAlarmAck = byAlarmAck^0x80;
		m_pMiCo->SetAlarmAck(wExtCard, byAlarmAck);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnRelais0() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);

		BYTE byRelais	= m_pMiCo->GetRelais(wExtCard);
		byRelais = byRelais^0x01;
		m_pMiCo->SetRelais(wExtCard, byRelais);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnRelais1() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);

		BYTE byRelais	= m_pMiCo->GetRelais(wExtCard);
		byRelais = byRelais^0x02;
		m_pMiCo->SetRelais(wExtCard, byRelais);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnRelais2() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);

		BYTE byRelais	= m_pMiCo->GetRelais(wExtCard);
		byRelais = byRelais^0x04;
		m_pMiCo->SetRelais(wExtCard, byRelais);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnRelais3() 
{
	if (m_pMiCo)
	{
		WORD wSource, wExtCard;
		m_pMiCo->GetInputSource(wExtCard, wSource);

		BYTE byRelais	= m_pMiCo->GetRelais(wExtCard);
		byRelais = byRelais^0x08;
		m_pMiCo->SetRelais(wExtCard, byRelais);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnDecStart() 
{
	m_dwDecodedFrames = 0L;

	if (m_pMiCo)
	{
		m_pMiCo->DecoderStart();	
		m_nDecoderState = MICO_DECODER_ON;
		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnDecStop() 
{
	if (m_pMiCo)
	{
		m_pMiCo->DecoderStop();	
		m_nDecoderState = MICO_DECODER_OFF;
		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnEncStart() 
{
	m_dwEncodedFrames = 0L;
	m_dwSourceSwitch  = 0L;

	if (m_pMiCo)
	{
		m_pMiCo->EncoderSetFormat(m_wFormat);
		m_pMiCo->EncoderSetBPF(m_dwBPF);

		if (m_pMiCo->EncoderStart())
			m_nEncoderState = MICO_ENCODER_ON;

		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnEncStop() 
{
	if (m_pMiCo)
	{
		if (m_pMiCo->EncoderStop())
			m_nEncoderState = MICO_ENCODER_OFF;

		UpdateDlg();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnTest()
{
	m_bOn = !m_bOn;
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnCompress1() 
{
	m_dwBPF = MICO_BPF_1;

	WORD wSource, wExtCard;

	if (m_pMiCo)
		m_pMiCo->GetInputSource(wExtCard, wSource);	
	if (m_pMiCo)
		m_pMiCo->SetInputSource(wExtCard, wSource, m_wFormat, m_dwBPF, 0);
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnCompress2() 
{
	m_dwBPF = MICO_BPF_2;
	WORD wSource, wExtCard;

	if (m_pMiCo)
		m_pMiCo->GetInputSource(wExtCard, wSource);	
	if (m_pMiCo)
		m_pMiCo->SetInputSource(wExtCard, wSource, m_wFormat, m_dwBPF, 0);
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnCompress3() 
{
	m_dwBPF = MICO_BPF_3;
	WORD wSource, wExtCard;

	if (m_pMiCo)
		m_pMiCo->GetInputSource(wExtCard, wSource);	
	if (m_pMiCo)
		m_pMiCo->SetInputSource(wExtCard, wSource, m_wFormat, m_dwBPF, 0);
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnCompress4() 
{
	m_dwBPF = MICO_BPF_4;
	WORD wSource, wExtCard;

	if (m_pMiCo)
		m_pMiCo->GetInputSource(wExtCard, wSource);	
	if (m_pMiCo)
		m_pMiCo->SetInputSource(wExtCard, wSource, m_wFormat, m_dwBPF, 0);
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnCompress5() 
{
	m_dwBPF = MICO_BPF_5;
	WORD wSource, wExtCard;

	if (m_pMiCo)
		m_pMiCo->GetInputSource(wExtCard, wSource);	
	if (m_pMiCo)
		m_pMiCo->SetInputSource(wExtCard, wSource, m_wFormat, m_dwBPF, 0);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMiCoUnitDlg::SetDlgText(UINT nID, const char *pszText)
{
	if (!pszText)
	{
		WK_TRACE_WARNING("MiCoUnitDlg::SetDlgText\tpszText=NULL\n");
		return FALSE;
	}

	if (!IsValid())
	{
		WK_TRACE_WARNING("MiCoUnitDlg::SetDlgText\tUnValid\n");
		return FALSE;
	}

	SetDlgItemText(nID, pszText);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnHighResolution() 
{
	m_wFormat = MICO_ENCODER_HIGH;
	WORD wSource, wExtCard;

	if (m_pMiCo)
		m_pMiCo->GetInputSource(wExtCard, wSource);	
	if (m_pMiCo)
		m_pMiCo->SetInputSource(wExtCard, wSource, m_wFormat, m_dwBPF, 0);
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnLowResolution() 
{
	m_wFormat = MICO_ENCODER_LOW;
	WORD wSource, wExtCard;

	if (m_pMiCo)
		m_pMiCo->GetInputSource(wExtCard, wSource);	
	if (m_pMiCo)
		m_pMiCo->SetInputSource(wExtCard, wSource, m_wFormat, m_dwBPF, 0);
}

