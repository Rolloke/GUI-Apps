// CRisc.cpp: Implementierung der Klasse CRisc.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CRisc.h"
#include "..\\SavicDA\\SavicDirectAccess.h"
#include "CBT878.h"
#include "CIoMemory.h"
#include "SavicReg.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CRisc::CRisc(CBT878* pBT878)
{
	m_dwRiscPrgPhys = 0;
	m_dwTargetAddr	= 0;
	m_pIoMem = NULL;
	m_nPC	 = 0;

	m_dwRiscPrgSize = 80*1024;
	if (pBT878)
	{
		if (pBT878->BT878In(BT878_RISC_STRT_ADD, m_dwRiscPrgPhys))
		{
			if (m_dwRiscPrgPhys)
			{
				DWORD dwRiscPrg = DAMapPhysToLin(m_dwRiscPrgPhys, m_dwRiscPrgSize);
				m_pIoMem = new CIoMemory(dwRiscPrg);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
CRisc::~CRisc()
{
	if (m_pIoMem)
	{
		DWORD dwIoBase = m_pIoMem->GetBaseAddress();
		if (dwIoBase)
			DAUnmapIoSpace(dwIoBase, m_dwRiscPrgSize);

		delete m_pIoMem;
		m_pIoMem = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
void CRisc::Disassamble(DWORD dwLen)
{
	if (!m_pIoMem)
		return;
		
	int nCount = 0;
	while((DWORD)nCount++ <= dwLen/sizeof(DWORD))
	{
		if ((m_nPC < 0) || (m_nPC >= (int)m_dwRiscPrgSize))
		{
			TRACE(_T("Program counter out of range (%d)\n"), m_nPC);
			break;
		}

		DWORD dwVal =  m_pIoMem->Inputdw(m_nPC);
		BYTE byOp = (dwVal >> 28) & 0x0f;
		switch(byOp)
		{
			case OP_WRITE:
				HandleOpWrite();
				break;
			case OP_WRITE123:
				HandleOpWrite123();
				break;
			case OP_WRITE1S23:
				HandleOpWrite1S23();
				break;
			case OP_WRITEC:
				HandleOpWriteC();
				break;
			case OP_SKIP:
				HandleOpSkip();
				break;
			case OP_SKIP123:
				HandleOpSkip123();
				break;
			case OP_JUMP:
				HandleOpJump();
				break;
			case OP_SYNC:
				HandleOpSync();
				break;
		};
	}
}

//////////////////////////////////////////////////////////////////////
BOOL CRisc::HandleOpWrite()
{
	BOOL bResult = FALSE;
	CString sOutText;
	DWORD dwVal =  m_pIoMem->Inputdw(m_nPC);

	WORD wByteCount		= (dwVal >> 0)  & 0xfff;	// [11:0]
	BYTE byByteEnables	= (dwVal >> 12) & 0x0f;		// [15:12]
	BYTE byRiscStatus	= (dwVal >> 16) & 0xff;		// [16:23]
	BOOL bIRQ			= (dwVal >> 24) & 0x01;		// [24]
	BOOL bReserved		= (dwVal >> 25) & 0x01;		// [25]
	BOOL bEOL			= (dwVal >> 26) & 0x01;		// [26]
	BOOL bSOL			= (dwVal >> 27) & 0x01;		// [27]
	BYTE byOp			= (dwVal >> 28) & 0x0f;		// [31:28]
	DWORD dwTargetAddr	= m_pIoMem->Inputdw(m_nPC+4);
	m_dwTargetAddr		= dwTargetAddr + wByteCount;

	sOutText.Format(_T("0x%08x WRITE #%d @ 0x%08x [RiscStatus=0x%02x] %s %s %s\n"),
					 m_dwRiscPrgPhys+m_nPC,
					 wByteCount,
					 dwTargetAddr,
					 byRiscStatus,
					 bIRQ ? _T("IRQ") : _T(""),
					 bSOL ? _T("SOL") : _T(""),
					 bEOL ? _T("EOL") : _T(""));
	
	ML_TRACE(sOutText);
	Sleep(10);
	m_nPC += 2*sizeof(DWORD);
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CRisc::HandleOpWrite123()
{
	BOOL bResult = FALSE;
	CString sOutText;
	DWORD dwVal =  m_pIoMem->Inputdw(m_nPC);

	WORD wByteCount1	= (dwVal >> 0)  & 0xfff;	// [11:0]
	BYTE byByteEnables	= (dwVal >> 12) & 0x0f;		// [15:12]
	BYTE byRiscStatus	= (dwVal >> 16) & 0xff;		// [16:23]
	BOOL bIRQ			= (dwVal >> 24) & 0x01;		// [24]
	BOOL bReserved		= (dwVal >> 25) & 0x01;		// [25]
	BOOL bEOL			= (dwVal >> 26) & 0x01;		// [26]
	BOOL bSOL			= (dwVal >> 27) & 0x01;		// [27]
	BYTE byOp			= (dwVal >> 28) & 0x0f;		// [31:28]
	
	DWORD dwVal2		= m_pIoMem->Inputdw(m_nPC+1*sizeof(DWORD));
	DWORD dwTargetAddr1	= m_pIoMem->Inputdw(m_nPC+2*sizeof(DWORD));
	DWORD dwTargetAddr2	= m_pIoMem->Inputdw(m_nPC+3*sizeof(DWORD));
	DWORD dwTargetAddr3	= m_pIoMem->Inputdw(m_nPC+4*sizeof(DWORD));
	WORD  wByteCount2	= (dwVal2 >> 0) & 0xfff;	// [11:0]
	WORD  wByteCount3	= (dwVal2 >> 16) & 0xfff;	// [27:16]

	sOutText.Format(_T("0x%08x WRITE123 #(%d,%d,%d) @(%0x%08x,%0x%08x,%0x%08x) [RiscStatus=0x%02x] %s %s %s\n"),
					 m_dwRiscPrgPhys+m_nPC,
					 wByteCount1,
					 wByteCount2,
					 wByteCount3,
					 dwTargetAddr1,
					 dwTargetAddr2,
					 dwTargetAddr3,
					 byRiscStatus,
					 bIRQ ? _T("IRQ") : _T(""),
					 bSOL ? _T("SOL") : _T(""),
					 bEOL ? _T("EOL") : _T(""));
	ML_TRACE(sOutText);
	Sleep(10);

	m_nPC += 5*sizeof(DWORD);
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CRisc::HandleOpWrite1S23()
{
	BOOL bResult = FALSE;
	CString sOutText;
	DWORD dwVal =  m_pIoMem->Inputdw(m_nPC);

	WORD wByteCount1	= (dwVal >> 0)  & 0xfff;	// [11:0]
	BYTE byByteEnables	= (dwVal >> 12) & 0x0f;		// [15:12]
	BYTE byRiscStatus	= (dwVal >> 16) & 0xff;		// [16:23]
	BOOL bIRQ			= (dwVal >> 24) & 0x01;		// [24]
	BOOL bReserved		= (dwVal >> 25) & 0x01;		// [25]
	BOOL bEOL			= (dwVal >> 26) & 0x01;		// [26]
	BOOL bSOL			= (dwVal >> 27) & 0x01;		// [27]
	BYTE byOp			= (dwVal >> 28) & 0x0f;		// [31:28]
	
	DWORD dwVal2		= m_pIoMem->Inputdw(m_nPC+1*sizeof(DWORD));
	DWORD dwTargetAddr	= m_pIoMem->Inputdw(m_nPC+2*sizeof(DWORD));
	WORD  wByteCount2	= (dwVal2 >> 0) & 0xfff;	// [11:0]
	WORD  wByteCount3	= (dwVal2 >> 16) & 0xfff;	// [27:16]

	sOutText.Format(_T("0x%08x WRITE1S23 #(%d,%d,%d) @(%0x%08x) [RiscStatus=0x%02x] %s %s %s\n"),
					 m_dwRiscPrgPhys+m_nPC,
					 wByteCount1,
					 wByteCount2,
					 wByteCount3,
					 dwTargetAddr,
					 byRiscStatus,
					 bIRQ ? _T("IRQ") : _T(""),
					 bSOL ? _T("SOL") : _T(""),
					 bEOL ? _T("EOL") : _T(""));
	ML_TRACE(sOutText);
	Sleep(10);

	m_nPC += 3*sizeof(DWORD);
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CRisc::HandleOpWriteC()
{
	BOOL bResult = FALSE;
	CString sOutText;
	DWORD dwVal =  m_pIoMem->Inputdw(m_nPC);

	WORD wByteCount		= (dwVal >> 0)  & 0xfff;	// [11:0]
	BYTE byByteEnables	= (dwVal >> 12) & 0x0f;		// [15:12]
	BYTE byRiscStatus	= (dwVal >> 16) & 0xff;		// [16:23]
	BOOL bIRQ			= (dwVal >> 24) & 0x01;		// [24]
	BOOL bReserved		= (dwVal >> 25) & 0x01;		// [25]
	BOOL bEOL			= (dwVal >> 26) & 0x01;		// [26]
	BOOL bSOL			= (dwVal >> 27) & 0x01;		// [27]
	BYTE byOp			= (dwVal >> 28) & 0x0f;		// [31:28]

	sOutText.Format(_T("0x%08x WRITEC #%d @ 0x%08x [RiscStatus=0x%02x] %s %s %s\n"),
					 m_dwRiscPrgPhys+m_nPC,
					 wByteCount,
					 m_dwTargetAddr,
					 byRiscStatus,
					 bIRQ ? _T("IRQ") : _T(""),
					 bSOL ? _T("SOL") : _T(""),
					 bEOL ? _T("EOL") : _T(""));
	
	ML_TRACE(sOutText);
	Sleep(10);

	m_nPC += 1*sizeof(DWORD);
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CRisc::HandleOpSkip()
{
	BOOL bResult = FALSE;
	CString sOutText;
	DWORD dwVal =  m_pIoMem->Inputdw(m_nPC);

	WORD wByteCount		= (dwVal >> 0)  & 0xfff;	// [11:0]
	BYTE byByteEnables	= (dwVal >> 12) & 0x0f;		// [15:12]
	BYTE byRiscStatus	= (dwVal >> 16) & 0xff;		// [16:23]
	BOOL bIRQ			= (dwVal >> 24) & 0x01;		// [24]
	BOOL bReserved		= (dwVal >> 25) & 0x01;		// [25]
	BOOL bEOL			= (dwVal >> 26) & 0x01;		// [26]
	BOOL bSOL			= (dwVal >> 27) & 0x01;		// [27]
	BYTE byOp			= (dwVal >> 28) & 0x0f;		// [31:28]
	DWORD dwTargetAddr	= m_dwTargetAddr;
	m_dwTargetAddr		= dwTargetAddr + wByteCount;

	sOutText.Format(_T("0x%08x SKIP #%d @ 0x%08x [RiscStatus=0x%02x] %s %s %s\n"),
					 m_dwRiscPrgPhys+m_nPC,
					 wByteCount,
					 dwTargetAddr,
					 byRiscStatus,
					 bIRQ ? _T("IRQ") : _T(""),
					 bSOL ? _T("SOL") : _T(""),
					 bEOL ? _T("EOL") : _T(""));

	ML_TRACE(sOutText);
	Sleep(10);

	m_nPC += 2*sizeof(DWORD);
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CRisc::HandleOpSkip123()
{
	BOOL bResult = FALSE;
	CString sOutText;
	DWORD dwVal =  m_pIoMem->Inputdw(m_nPC);

	WORD wByteCount1	= (dwVal >> 0)  & 0xfff;	// [11:0]
	BYTE byByteEnables	= (dwVal >> 12) & 0x0f;		// [15:12]
	BYTE byRiscStatus	= (dwVal >> 16) & 0xff;		// [16:23]
	BOOL bIRQ			= (dwVal >> 24) & 0x01;		// [24]
	BOOL bReserved		= (dwVal >> 25) & 0x01;		// [25]
	BOOL bEOL			= (dwVal >> 26) & 0x01;		// [26]
	BOOL bSOL			= (dwVal >> 27) & 0x01;		// [27]
	BYTE byOp			= (dwVal >> 28) & 0x0f;		// [31:28]
	
	DWORD dwVal2		= m_pIoMem->Inputdw(m_nPC+1*sizeof(DWORD));
	WORD  wByteCount2	= (dwVal2 >> 0) & 0xfff;	// [11:0]
	WORD  wByteCount3	= (dwVal2 >> 16) & 0xfff;	// [27:16]

	sOutText.Format(_T("0x%08x SKIP123 #(%d,%d,%d) [RiscStatus=0x%02x] %s %s %s\n"),
					 m_dwRiscPrgPhys+m_nPC,
					 wByteCount1,
					 wByteCount2,
					 wByteCount3,
					 byRiscStatus,
					 bIRQ ? _T("IRQ") : _T(""),
					 bSOL ? _T("SOL") : _T(""),
					 bEOL ? _T("EOL") : _T(""));
	ML_TRACE(sOutText);
	Sleep(10);

	m_nPC += 2*sizeof(DWORD);
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CRisc::HandleOpJump()
{
	BOOL bResult = FALSE;
	CString sOutText;
	DWORD dwVal =  m_pIoMem->Inputdw(m_nPC);

	WORD wReserved		= (dwVal >> 0)  & 0xfff;	// [15:0]
	BYTE byRiscStatus	= (dwVal >> 16) & 0xff;		// [16:23]
	BOOL bIRQ			= (dwVal >> 24) & 0x01;		// [24]
	BYTE byReserved		= (dwVal >> 25) & 0x07;		// [27:25]
	BYTE byOp			= (dwVal >> 28) & 0x0f;		// [31:28]
	DWORD dwJumpAddr	= m_pIoMem->Inputdw(m_nPC+1*sizeof(DWORD));

	sOutText.Format(_T("0x%08x JUMP to 0x%08x [RiscStatus=0x%02x] %s\n"),
					 m_dwRiscPrgPhys+m_nPC,
					 dwJumpAddr,
					 byRiscStatus,
					 bIRQ ? _T("IRQ") : _T(""));
	ML_TRACE(sOutText);
	Sleep(10);

	m_nPC += 2*sizeof(DWORD);
	m_nPC = (int)(dwJumpAddr-m_dwRiscPrgPhys); 
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CRisc::HandleOpSync()
{
	BOOL bResult = FALSE;
	CString sOutText;
	DWORD dwVal =  m_pIoMem->Inputdw(m_nPC);

	BYTE byStatus		= (dwVal >> 0)  & 0x0f;		// [3:0]
	WORD wReserved		= (dwVal >> 4)  & 0x3ff;	// [14:4]
	BOOL bResync		= (dwVal >> 15) & 0x01;		// [15]
	BYTE byRiscStatus	= (dwVal >> 16) & 0xff;		// [16:23]
	BOOL bIRQ			= (dwVal >> 24) & 0x01;		// [24]
	BYTE byReserved		= (dwVal >> 25) & 0x07;		// [27:25]
	BYTE byOp			= (dwVal >> 28) & 0x0f;		// [31:28]
	DWORD dwReserved	= m_pIoMem->Inputdw(m_nPC+1*sizeof(DWORD));

	CString sStatus;
	switch(byStatus)
	{
		case 0x06:
			sStatus = _T("FM1");
			break;
		case 0x0e:
			sStatus = _T("FM3");
			break;
		case 0x04:
			sStatus = _T("VRE");
			break;
		case 0x0c:
			sStatus = _T("VRO");
			break;
		default:
			sStatus = _T("??");
	}

	sOutText.Format(_T("0x%08x SYNC [%s] %s [RiscStatus=0x%02x] %s\n"),
					 m_dwRiscPrgPhys+m_nPC,
					 sStatus,
					 bResync ? _T("RESYNC") : _T(""),
					 byRiscStatus,
					 bIRQ ? _T("IRQ") : _T(""));

	ML_TRACE(sOutText);
	Sleep(10);

	m_nPC += 2*sizeof(DWORD);
	return bResult;
}
