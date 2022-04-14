/////////////////////////////////////////////////////////////////////////////
// PROJECT:		MiCoTst
// FILE:		$Workfile: CZoran.h $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/MiCoTst/CZoran.h $
// CHECKIN:		$Date: 5.08.98 10:03 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 5.08.98 10:02 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CZORAN_H__ 
#define __CZORAN_H__

//#include "cipc.h"
#include "zr050.h"
#include "zoranio.h"

class Z050_Coder;
class ZoranIo;

class CZoran
{
	public:
		CZoran(WORD wIOBase);
		~CZoran();
		BOOL IsValid();

		BOOL Open();
		BOOL Close();

		void SetInputSource(WORD wFrontEnd, WORD wExtCard, WORD wSource, BYTE byPortY, BYTE byPortC, WORD wFormat, DWORD dwBPF);
		void SetFeVideoFormat(WORD wFormat);
		void SetBeVideoFormat(WORD wFormat);

		BOOL StartEncoding();
		BOOL StopEncoding();
		BOOL StartDecoding();
		BOOL StopDecoding();
		int  GetEncoderState();
		int	 GetDecoderState();
		void SetFormat(WORD wFormat);
		void SetFramerate(WORD wFramerate);
		void SetBPF(DWORD wBPF);
		
		DWORD WriteData(LPCSTR pData, DWORD dwDataLen);
       
       	// testroutinen
		BOOL TestingEncoder();
		BOOL Recording();
		BOOL Playing();
		
		BOOL PollingVideoIRQ();
		BOOL TestingZR36055();
		BOOL TestingInternalMemory();
		BOOL TestingCodebuffer();
		BOOL TestingIRQ(const WORD *wIRQArray, WORD wIRQCount);
				
	private:
		BOOL Reset();
		void EnableBrdIRQ();
		void DisableBrdIRQ();
		void SetIRQNumber(WORD wIRQ);
		void ChipSelectVideoInAnalogOut(WORD wFrontEnd, WORD wExtCard, BYTE byData);
		BYTE ChipSelectVideoInAnalogIn(WORD wFrontEnd, WORD wExtCard);
		void ChipSelectVideoInDigitalOut(BYTE byData);
		BYTE ChipSelectVideoInDigitalIn();

		void ZR36055InitForEncoding();
		void ZR36050InitForEncoding();
		void ZR36055InitForDecoding();
		void ZR36050InitForDecoding();

		void WriteToZR36050(WORD wAdr, BYTE byValue);
		BYTE ReadFromZR36050(WORD wAdr);

		void InstallISR(WORD wIrq);
		void RemoveISR(WORD wIrq);

		Z050_Coder *m_pzCoder;
		ZoranIo	   *m_pZIo;
        WORD*	m_pBuffer;
		BOOL	m_bOK;
		CIo		m_IO;
		BYTE	m_byMiCoID;
		int		m_nEncoderState;
		int		m_nDecoderState;
		DWORD	m_dwDecodedFrames;
		DWORD	m_dwEncodedFrames;
		WORD	m_wFormat;
		WORD	m_wFramerate;
		DWORD	m_dwBPF;
		WORD	m_wIOBase;
		WORD	m_wFeVideoFormat;
		WORD	m_wFeHStart[4];
		WORD	m_wFeHEnd[4];
		WORD	m_wFeHTotal[4];
		WORD	m_wFeVStart[4];
		WORD	m_wFeVEnd[4];
		WORD	m_wFeHSStart[4];
		WORD	m_wFeHSEnd[4];
		WORD	m_wFeVTotal[4];
		WORD	m_wFeBlankStart[4];
		WORD	m_wFeNMCU[4];

		WORD	m_wBeVideoFormat;
		WORD	m_wBeHStart;
		WORD	m_wBeHEnd;
		WORD	m_wBeHTotal;
		WORD	m_wBeVStart;
		WORD	m_wBeVEnd;
		WORD	m_wBeHSStart;
		WORD	m_wBeHSEnd;
		WORD	m_wBeVTotal;
		WORD	m_wBeNMCU;
		WORD	m_wBeBlankStart;

		WORD	m_wYBackgnd;
		WORD	m_wUVBackgnd;

		DWORD	m_dwTCVData;
		DWORD	m_dwTCVNet;
		WORD	m_wSF;
};
#endif // __CZORAN_H__