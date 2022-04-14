/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: cvideoinlay.cpp $
// ARCHIVE:		$Archive: /Project/Units/SaVic/SavicDll/cvideoinlay.cpp $
// CHECKIN:		$Date: 20.06.03 12:01 $
// VERSION:		$Revision: 35 $
// LAST CHANGE:	$Modtime: 20.06.03 11:35 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CVideoInlay.h"
#include "CPCI.h"
#include "CIoMemory.h"
#include "..\\SavicDA\\SavicDirectAccess.h"
#include "DirectDraw.h"
#include "CCodec.h"
#include "CBT878.h"
#include "CRisc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
CVideoInlay::CVideoInlay(CBT878* pBT878)
{
	m_bOK					= FALSE;
	m_pBT878				= pBT878;
	m_wRBits				= 0;
	m_wGBits				= 0;
	m_wBBits				= 0;
	m_nDisplayType			= DISPLAYTYPE_UNKNOWN;
	m_pDirectDraw			= NULL;

	m_dwScreenBaseAddr		= 0;
	m_dwBytesPerLine		= 0;
	m_dwBytesPerPixel		= 0;
	m_nHRes					= 0;
	m_nVRes					= 0;
	if (m_pBT878->GetVideoFormat() == VFMT_PAL_CCIR)
	{
		m_nCaptureSizeH = 720;	// Außmaße de Videoinlays
		m_nCaptureSizeV = 576;	//		"
	}
	else
	{
		m_nCaptureSizeH = 720;
		m_nCaptureSizeV = 480;
	}

	m_nBitsPerPixel			= 0;
	m_nPlanes				= 0;
	m_wPllFreq				= 0;
	m_pRiscProgram			= new DWORD[2048];
	m_hDevice				= NULL;
}

//////////////////////////////////////////////////////////////////////
BOOL CVideoInlay::Init(CCodec* pCodec)
{
	CPCIBus		PCIBus;
	CPCIDeviceList	*pDevList	= NULL;
	CPCIDevice	*pDevice	= NULL;
	WORD		wVendorID	= 0;
	WORD		wDeviceID	= 0;

	if (!m_hDevice && pCodec)
		m_hDevice = pCodec->GetDevice();

	// Informationen über das Ausgabedevice erfragen
	HDC hDC				= CreateIC(_T("DISPLAY"), NULL, NULL, NULL);
	m_nHRes				= GetDeviceCaps(hDC, HORZRES);				// Pixel pro Zeile
	m_nVRes				= GetDeviceCaps(hDC, VERTRES);				// Anzahl der Zeilen
	m_nBitsPerPixel		= GetDeviceCaps(hDC, BITSPIXEL);			// Bits pro Pixel
	m_nPlanes			= GetDeviceCaps(hDC, PLANES);				// Anzahl der Planes
	m_dwBytesPerPixel	= (m_nBitsPerPixel + 7) / 8;				// Bytes pro Pixel
	m_dwBytesPerLine	= m_nHRes * m_dwBytesPerPixel;				// Bytes pro Zeile
	DeleteDC(hDC);

	// Werden 15 oder 16 Bits benutzt.
	CheckIf15Or16BitsPerPixel(m_nBitsPerPixel);

	switch (m_nBitsPerPixel)
	{
		case 15:
			m_wRBits		= 5;
			m_wGBits		= 5;
			m_wBBits		= 5;
			m_wOutputFormat = BT878_RGB15;
			ML_TRACE(_T("Videoinlay: RGB555\n"));
			break;
		case 16:
			m_wRBits		= 5;
			m_wGBits		= 6;
			m_wBBits		= 5;
			m_wOutputFormat = BT878_RGB16;
			ML_TRACE(_T("Videoinlay: RGB565\n"));
			break;
		case 24:
			m_wRBits		= 8;
			m_wGBits		= 8;
			m_wBBits		= 8;		
			m_wOutputFormat = BT878_RGB24;
			ML_TRACE(_T("Videoinlay: RGB888 (24 Bit)\n"));
			break;
		case 32:
			m_wRBits		= 8;
			m_wGBits		= 8;
			m_wBBits		= 8;		
			m_wOutputFormat = BT878_RGB32;
			ML_TRACE(_T("Videoinlay: RGB888 (32 Bit)\n"));
			break;
		default:
			ML_TRACE_ERROR(_T("CVideoInlay\t Unbekanntes Pixelformat\n"));
			break;
	}		   

	// Existiert schon ein DirectDraw Objekt, dann dieses zunächst zerstören.
	if (m_pDirectDraw)
		WK_DELETE(m_pDirectDraw);
	
	// DirectDraw Objekt anlegen
	m_pDirectDraw = new CDirectDraw(m_nCaptureSizeH, m_nCaptureSizeV);
	if (m_pDirectDraw)
	{
		// Umrechnung von ColorRef->Keycolor.
		COLORREF col = RGB(255, 0, 255);
		DWORD r	= ((DWORD)GetRValue(col)>>(8-m_wRBits));
		DWORD g	= ((DWORD)GetGValue(col)>>(8-m_wGBits));
		DWORD b	= ((DWORD)GetBValue(col)>>(8-m_wBBits));
		DWORD dwKeyCol = (DWORD)((r<<(m_wGBits+m_wBBits)) | (g<<m_wBBits) | b);

		// Versuche ein VideoOverlay zu etablieren
		if (m_pDirectDraw->CreateOverlaySurface(dwKeyCol))
		{
			ML_TRACE(_T("Using Video-Overlay\n"));
			m_dwScreenBaseAddr	= (DWORD)m_pDirectDraw->LockOverlaySurface();
			m_dwScreenBaseAddr	= DAMapLinToPhys(m_dwScreenBaseAddr);
			m_pDirectDraw->UnlockOverlaySurface();
			m_nDisplayType		= VIDEO_OVERLAY;
			m_wOutputFormat		= BT878_YUV2;
			m_nBitsPerPixel		= 16; // YUV422 hat 16Bit/Pixel
			m_dwBytesPerPixel	= (m_nBitsPerPixel + 7) / 8;	// Bytes pro Pixel
			m_dwBytesPerLine	= m_nHRes * m_dwBytesPerPixel;	// Bytes pro Zeile
		}
		// Versuche ein VideoInlay zu etablieren
		else if (m_pDirectDraw->CreatePrimarySurface())
		{
			ML_TRACE(_T("Using Video-Inlay (Primary Surface)\n"));
			m_dwScreenBaseAddr	= (DWORD)m_pDirectDraw->LockPrimarySurface();
			m_dwScreenBaseAddr	= DAMapLinToPhys(m_dwScreenBaseAddr);
			m_pDirectDraw->UnlockPrimarySurface();
			m_nDisplayType		= VIDEO_PRIMARY;
		}
	}

	// DirectDraw lieferte kein gültiges Surface. Ermittel Bildschirmadresse
	// über das Bios, um trotzdem ein VideoInlay zu ermöglichen.
	if (m_nDisplayType == DISPLAYTYPE_UNKNOWN)
	{
		// PCI-Bus nach Devices scannen
		pDevList = PCIBus.ScanBus();
		if (!pDevList)
		{
			ML_TRACE_ERROR(_T("Kein PCI-Device gefunden\n"));
			return FALSE;
		}

		// PCI-Displaycontroller suchen
		pDevice = pDevList->GetDevice(BC_DISPLAY_CONTROLLER);
		if (!pDevice)
		{
			ML_TRACE(_T("No PCI Displaycontroller found\n"));
			return FALSE;
		}
		m_dwScreenBaseAddr = pDevice->GetMemBase(0);
		wVendorID = pDevice->GetVendorID();
		wDeviceID = pDevice->GetDeviceID();

		m_dwScreenBaseAddr &= 0xffffff00;		// original value of mask: 0xff000000

		ML_TRACE(_T("PCI/AGP Displaycontroller found: VendorID=%x DeviceID=%x Framebufferaddr=0x%8x\n"),
							wVendorID, wDeviceID, m_dwScreenBaseAddr); 

		m_nDisplayType  = VIDEO_INLAY;
		ML_TRACE(_T("Using Video-Inlay\n"));
	}

	if (m_dwScreenBaseAddr == 0)
	{
		ML_TRACE_ERROR(_T("Ungültige Framebuffer Adresse\n"));
		return FALSE;
	}

	if (m_pBT878)
	{
		// Color Output format setzen
		m_pBT878->BT878Out(BT878_COLOR_FMT, m_wOutputFormat<<4 | m_wOutputFormat);
		
		// Dither, NoVBIOdd, NoVBIEven, VDFCOdd, VDFCEven		
		m_pBT878->BT878Out(BT878_CAP_CTL, 0x03);

		// Interrupts einschalten
		DWORD dwReg = 0;
//		dwReg = SETBIT(dwReg, BT878_INST_STAT_FMTCHG_BIT);
//		dwReg = SETBIT(dwReg, BT878_INST_STAT_VSYNC_BIT);
//		dwReg = SETBIT(dwReg, BT878_INST_STAT_HSYNC_BIT);
//		dwReg = SETBIT(dwReg, BT878_INST_STAT_OFLOW_BIT);	 // Dauer-Interrupt, sobald Kamerasignal!?
//		dwReg = SETBIT(dwReg, BT878_INST_STAT_HLOCK_BIT);
//		dwReg = SETBIT(dwReg, BT878_INST_STAT_VPRES_BIT);
//		dwReg = SETBIT(dwReg, BT878_INST_STAT_I2CDONE_BIT);
//		dwReg = SETBIT(dwReg, BT878_INST_STAT_GPINT_BIT);    // Dauer-Interrupt!!
		dwReg = SETBIT(dwReg, BT878_INST_STAT_RISCI_BIT);
		dwReg = SETBIT(dwReg, BT878_INST_STAT_FBUS_BIT);	 // Kommen sehr häufig
		dwReg = SETBIT(dwReg, BT878_INST_STAT_FTRGT_BIT);
		dwReg = SETBIT(dwReg, BT878_INST_STAT_FDSR_BIT);
		dwReg = SETBIT(dwReg, BT878_INST_STAT_PPERR_BIT);
		dwReg = SETBIT(dwReg, BT878_INST_STAT_RIPERR_BIT);
		dwReg = SETBIT(dwReg, BT878_INST_STAT_PABORT_BIT);
		dwReg = SETBIT(dwReg, BT878_INST_STAT_OCERR_BIT);
		dwReg = SETBIT(dwReg, BT878_INST_STAT_SCERR_BIT);
		m_pBT878->BT878Out(BT878_INT_MASK, dwReg);
	}
	
	m_bOK	= TRUE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
CVideoInlay::~CVideoInlay()
{
	// Interrupts Ausschalten
	DWORD dwReg = 0;
	m_pBT878->BT878Out(BT878_INT_MASK, dwReg);
	
	DisableRisc();

	WK_DELETE(m_pDirectDraw);
	WK_DELETE(m_pRiscProgram);
	
	m_bOK	= FALSE;

}

//////////////////////////////////////////////////////////////////////
BOOL CVideoInlay::IsValid() const
{		  
	return m_bOK;
}

//////////////////////////////////////////////////////////////////////
BOOL CVideoInlay::SetOutputWindow(const CRect &rc)
{
	DWORD	dwTopAddr		= 0;
	DWORD	dwBotAddr		= 0;
	TRACE(_T("x=%d y=%d w=%d, h=%d\n"), rc.left, rc.top, rc.Width(), rc.Height());

	int		nX			= rc.left;
	int		nY			= rc.top;
	int		nW			= rc.Width();
	int		nH			= rc.Height();

	if (!IsValid())
		return FALSE;

	nW = min(nW, m_nCaptureSizeH);
	nH = min(nH, m_nCaptureSizeV);

	DisableRisc();

	// Bild auf den sichtbaren Bereich beschränken
	nW = nW + min(nX, 0);
	nX = min(max(nX, 0), m_nHRes);
	nY = min(max(nY, 0), m_nVRes);

	if ((m_nDisplayType == VIDEO_OVERLAY) && m_pDirectDraw)
	{
		// DirectDraw übernimmt die Skallierung
		m_pDirectDraw->UpdateOverlay(rc);
		nX = 0;
		nY = 0;
		nW = m_nCaptureSizeH;
		nH = m_nCaptureSizeV;
	}

	// Das Bild skalieren.
	m_pBT878->SetHActiveE(nW);
	m_pBT878->SetHActiveO(nW);
	if (m_pBT878->GetVideoFormat() == VFMT_PAL_CCIR)
	{
		m_pBT878->SetHScaleE((int)(((922.0/(float)nW) - 1.0) * 4096));
		m_pBT878->SetHScaleO((int)(((922.0/(float)nW) - 1.0) * 4096));
		m_pBT878->SetHDelayE((WORD)(186.0/922.0*(float)nW) & 0x3FE);
		m_pBT878->SetHDelayO((WORD)(186.0/922.0*(float)nW) & 0x3FE);
		m_pBT878->SetVScaleE((WORD)((0x10000 - (576.0/(float)nH-1.0)*512.0)) & 0x1fff);
		m_pBT878->SetVScaleO((WORD)((0x10000 - (576.0/(float)nH-1.0)*512.0)) & 0x1fff);
	}
	else if (m_pBT878->GetVideoFormat() == VFMT_NTSC_CCIR)
	{
		m_pBT878->SetHScaleE((int)(((754.0/(float)nW) - 1.0) * 4096));
		m_pBT878->SetHScaleO((int)(((754.0/(float)nW) - 1.0) * 4096));
		m_pBT878->SetHDelayE((WORD)(135.0/754.0*(float)nW) & 0x3FE);
		m_pBT878->SetHDelayO((WORD)(135.0/754.0*(float)nW) & 0x3FE);
		m_pBT878->SetVScaleE((WORD)((0x10000 - (480.0/(float)nH-1.0)*512.0)) & 0x1fff);
		m_pBT878->SetVScaleO((WORD)((0x10000 - (480.0/(float)nH-1.0)*512.0)) & 0x1fff);
	}
	
	// Riscprogramm erzeugen.
	CreateRiscPrg(CRect(nX, nY, nX+nW, nY+nH));
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CVideoInlay::Activate()
{
	if (!IsValid())
		return FALSE;

	return EnableRisc();
}

//////////////////////////////////////////////////////////////////////
BOOL CVideoInlay::Deactivate()
{
	if (!IsValid())
		return FALSE;

	return DisableRisc();
}

//////////////////////////////////////////////////////////////////////
// This routine tries to determine if the number of bits per pixel is either 15 or 16.
// First, a bitmap compatible to the system screen bitmap is created. Then the physical
// data format of green and violet pixels is compared to the expected values for 15 and
// 16 bit.
void CVideoInlay::CheckIf15Or16BitsPerPixel(int &nBitsPerPixel) const
{
	HBITMAP hBitmap			= NULL;
	HDC		hMemDC			= NULL;
	WORD	BitmapData[256] ={0};
	
	hBitmap	= CreateCompatibleBitmap(GetDC(NULL), 8, 8);
	hMemDC	= CreateCompatibleDC(NULL);

	if (!hBitmap || !hMemDC)
		return;

	SelectObject(hMemDC,  hBitmap);

	::SetPixel(hMemDC, 0, 0, RGB(0, 255, 0));
	::SetPixel(hMemDC, 1, 0, RGB(255, 0, 255));
	::GetBitmapBits(hBitmap, 32, BitmapData);

	if (BitmapData[0] == 0x03e0 && BitmapData[1] == 0x7c1f)			// Patterns for 15 bit depth.
		nBitsPerPixel = 15;
	else if (BitmapData[0] == 0x07e0 && BitmapData[1] == 0xf81f)	// Patterns for 16 bit depth.
		nBitsPerPixel = 16;

	DeleteDC(hMemDC);
	DeleteObject(hBitmap);
}

/////////////////////////////////////////////////////////////////////////////
void CVideoInlay::CreateRiscPrg(const CRect &rc)   
{
	DisableRisc();
//	Sleep(50);

	DWORD* rp = m_pRiscProgram;
	DWORD dwRiscStartAddrPhys = 0;
	m_pBT878->BT878In(BT878_RISC_STRT_ADD, dwRiscStartAddrPhys);

	DWORD dwPhysDisplay = m_dwScreenBaseAddr;

	*(rp++)=(BT878_RISC_SYNC|BT878_FIFO_STATUS_FM1);
	*(rp++)=0;

#if (0)
	// Odd frame
	for (int nY=0; nY < rc.Height()/2; nY++)
	{
		dwPhysDisplay = m_dwScreenBaseAddr + (rc.top + nY) * m_dwBytesPerLine + (DWORD)rc.left * m_dwBytesPerPixel;
		(*rp++)=BT878_RISC_WRITE|BT878_RISC_SOL|BT878_RISC_EOL|m_dwBytesPerPixel*rc.Width(); 
		(*rp++)=dwPhysDisplay;
	}
	*(rp++)=BT878_RISC_SYNC|BT878_RISC_RESYNC|BT878_RISC_IRQ|BT878_FIFO_STATUS_VRE|0xf1<<16;
	*(rp++)=0;

	*(rp++)=(BT878_RISC_SYNC|BT878_FIFO_STATUS_FM1);
	*(rp++)=0;
/*
	// Even frame
	for (nY=0; nY < rc.Height()/2; nY++)
	{
		dwPhysDisplay = m_dwScreenBaseAddr + (rc.top + rc.Height()/2+nY) * m_dwBytesPerLine + (DWORD)rc.left * m_dwBytesPerPixel;
		(*rp++)=BT878_RISC_WRITE|BT878_RISC_SOL|BT878_RISC_EOL|m_dwBytesPerPixel*rc.Width(); 
		(*rp++)=dwPhysDisplay;
//		(*rp++)=BT878_RISC_SKIP|BT878_RISC_SOL|BT878_RISC_EOL|m_dwBytesPerPixel*rc.Width(); 
	}
	*(rp++)=BT878_RISC_SYNC|BT878_RISC_RESYNC|BT878_RISC_IRQ|BT878_FIFO_STATUS_VRO|0xf2<<16;
*/	*(rp++)=BT878_RISC_SYNC|BT878_RISC_RESYNC|BT878_FIFO_STATUS_VRO|0xf2<<16;
	*(rp++)=0;
	*(rp++)=BT878_RISC_JUMP;
	*(rp++)=dwRiscStartAddrPhys;
 	*(rp++)=0;  /* NULL WORD */
#else
	// Odd frame
	for (int nY=0; nY < rc.Height()/2; nY++)
	{
		dwPhysDisplay = m_dwScreenBaseAddr + (rc.top + 2*nY+0) * m_dwBytesPerLine + (DWORD)rc.left * m_dwBytesPerPixel;
		(*rp++)=BT878_RISC_WRITE|BT878_RISC_SOL|BT878_RISC_EOL|m_dwBytesPerPixel*rc.Width(); 
		(*rp++)=dwPhysDisplay;
	}
	*(rp++)=BT878_RISC_SYNC|BT878_RISC_RESYNC|BT878_RISC_IRQ|BT878_FIFO_STATUS_VRE|0xf0<<16;
	*(rp++)=0;

	*(rp++)=(BT878_RISC_SYNC|BT878_FIFO_STATUS_FM1);
	*(rp++)=0;
	
	// Even frame
	for (nY=0; nY < rc.Height()/2; nY++)
	{
		dwPhysDisplay = m_dwScreenBaseAddr + (rc.top + 2*nY+1) * m_dwBytesPerLine + (DWORD)rc.left * m_dwBytesPerPixel;
		(*rp++)=BT878_RISC_WRITE|BT878_RISC_SOL|BT878_RISC_EOL|m_dwBytesPerPixel*rc.Width(); 
		(*rp++)=dwPhysDisplay;
	}
	*(rp++)=BT878_RISC_SYNC|BT878_RISC_RESYNC|BT878_RISC_IRQ|BT878_FIFO_STATUS_VRO|0xf0<<16;
	*(rp++)=0;
	*(rp++)=BT878_RISC_JUMP;
	*(rp++)=dwRiscStartAddrPhys;
 	*(rp++)=0;  /* NULL WORD */
#endif
	DWORD dwLen = ((DWORD)rp - (DWORD)m_pRiscProgram);
	DATransferRiscProgram(m_hDevice, m_pRiscProgram, dwLen);

	EnableRisc();

//	CRisc Risc(m_pBT878);
//	Risc.Disassamble(dwLen);
}	

// Mit eingesschalteter Optimierung erzeugt der MS-Compiler Code, mit dem das Inlay nicht
// funktioniert. Das Riscprogramm startet nicht. Mit ausgeschalteter Optimierung, oder mit
// dem Intel-Compiler funktioniert es. Das untens stehende pragma schaltet alle optimierungen
// Innerhalb der Funktion 'CVideoInlay::EnableRisc()' aus. Dann geht es!
#pragma	optimize("", off)
/////////////////////////////////////////////////////////////////////////////
BOOL CVideoInlay::EnableRisc()
{
	BOOL bResult = FALSE;

	// Der Start des DMA-Transfer muß synchron zum Bildsignal erfolgen, deshalb wird
	// hier zunächst auf den Beginn des nächsten Bildes gewartet und dann der DMA-Transfer
	// aktiviert.
	if (m_pBT878)
	{
		WORD wReg = 0;
		BYTE byStatus = 0;
		BOOL b0, b1 = FALSE;

		if (m_pBT878->BT878In(BT878_GPIO_DMA_CTL, wReg))
		{
			wReg = SETBIT(wReg, BT878_GPIO_DMA_CTL_FIFO_ENABLE_BIT);
			wReg = SETBIT(wReg, BT878_GPIO_DMA_CTL_RISC_ENABLE_BIT);
			DWORD dwTC = 0;

			if (m_pBT878->BT878In(BT878_DSTATUS, byStatus))
			{
				b0 = TSTBIT(byStatus, BT878_DSTATUS_FIELD_BIT);
				dwTC = GetTickCount();
				do
				{
					if (m_pBT878->BT878In(BT878_DSTATUS, byStatus))
						b1 = TSTBIT(byStatus, BT878_DSTATUS_FIELD_BIT);
				}
				while((b0 == b1) && GetTimeSpan(dwTC) < 250);
				
				if (b0 != b1)
					bResult = m_pBT878->BT878Out(BT878_GPIO_DMA_CTL, wReg);
				else
					ML_TRACE(_T("EnableRisc waiting for even field timeout\n"));
			}
		
			ML_TRACE(_T("b0=%d b1=%d Time=%lu\n"), b0, b1, GetTickCount()-dwTC);
		}
	}

	return bResult;
}
#pragma	optimize("", on) // Optimierungen wieder auf Ursprüngliche Werte

/////////////////////////////////////////////////////////////////////////////
BOOL CVideoInlay::DisableRisc()
{
	BOOL bResult = FALSE;
	if (m_pBT878)
	{
		WORD wReg = 0;
		if (m_pBT878->BT878In(BT878_GPIO_DMA_CTL, wReg))
		{
			wReg = CLRBIT(wReg, BT878_GPIO_DMA_CTL_FIFO_ENABLE_BIT);
			wReg = CLRBIT(wReg, BT878_GPIO_DMA_CTL_RISC_ENABLE_BIT);
			if (m_pBT878->BT878Out(BT878_GPIO_DMA_CTL, wReg))
				bResult = TRUE;
		}
	}

	return bResult;
}

/* If Bt848a or Bt849, use PLL for PAL/SECAM and crystal for NTSC*/

/* Frequency = (F_input / PLL_X) * PLL_I.PLL_F/PLL_C 
   PLL_X = Reference pre-divider (0=1, 1=2) 
   PLL_C = Post divider (0=6, 1=4)
   PLL_I = Integer input 
   PLL_F = Fractional input 
   
   F_input = 28.636363 MHz: 
   PAL (CLKx2 = 35.46895 MHz): PLL_X = 1, PLL_I = 0x0E, PLL_F = 0xDCF9, PLL_C = 0
*/
#define BT878_PLL_X (1<<7)

void CVideoInlay::SetPllFreq(unsigned int fin, unsigned int fout)
{
	unsigned char fl, fh, fi;

	/* prevent overflows */
	fin/=4;
	fout/=4;

	fout*=12;
	fi=fout/fin;

	fout=(fout%fin)*256;
	fh=fout/fin;

	fout=(fout%fin)*256;
	fl=fout/fin;

	m_pBT878->BT878Out(BT878_PLL_F_LO, fl);
	m_pBT878->BT878Out(BT878_PLL_F_HI, fh);
	m_pBT878->BT878Out(BT878_PLL_XCI,  fi|BT878_PLL_X);
	Sleep(500);
	m_pBT878->BT878Out(BT878_TGCTRL,0x08);
}
