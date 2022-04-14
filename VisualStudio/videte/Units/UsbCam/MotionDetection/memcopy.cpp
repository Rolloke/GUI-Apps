// MemCopy.cpp: implementation of the CMemCopy class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemCopy.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define CACHEBLOCK 2048			// number of BYTES in a chunk 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CMemCopy::CMemCopy()
{
	m_bMMX		= HasMMX();
	m_bSFenc2	= SupportCheckForSFence();

#ifdef _DEBUG
	if (!m_bMMX)
	{
		WK_TRACE(_T("No MMX Instructions supported\n"));
	}
	if (!m_bSFenc2)
	{
		WK_TRACE(_T("No SFENCE Instruction supported\n"));
	}
#endif

}

//////////////////////////////////////////////////////////////////////
CMemCopy::~CMemCopy()
{
}


//////////////////////////////////////////////////////////////////////
BOOL CMemCopy::HasMMX()
{
	BOOL bResult = TRUE;
	DWORD dwCPUID = 0;
	_asm
	{
		mov	EAX, 1					; request for feature flags
		CPUID						; 0Fh, 0A2h   CPUID Instruction
		mov dwCPUID, EDX
		test	EDX, 00800000h		; is MMX technology Bit(bit 23)in feature
									; flags equal to 1
		jnz	Found
		mov bResult, 0
		jmp End
	Found:
		mov bResult, 1
	End:
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////
// Überprüft, ob der SFENCE-Befehl unterstützt wird.
// Dies ist z.B. bei den VIA C3 Prozessoren nicht der Fall.
BOOL CMemCopy::SupportCheckForSFence()
{
	_try
	{
		__asm
		{
			sfence	// Löst dieser Befehl eine Exception aus?
		}
	}
	_except(EXCEPTION_EXECUTE_HANDLER)
	{
		if (_exception_code()==STATUS_ILLEGAL_INSTRUCTION)
			return FALSE;	// Streaming SIMD Extensions not supported
	}

	// Streaming SIMD Extensions are supported by OS
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
void CMemCopy::FastCopy(void* pDest, void* pSrc, DWORD dwLen)
{
	if (!m_bMMX || !m_bSFenc2)
		memcpy(pDest, pSrc, dwLen);
	else if ((dwLen % 64) == 0)
		FastCopy1(pDest, pSrc, dwLen);
	else
		memcpy(pDest, pSrc, dwLen);
}

//////////////////////////////////////////////////////////////////////
void CMemCopy::FastCopy1(void* pDest, void* pSrc, DWORD dwLen)
{  
	__int64  RegBuffer[8];

	// Suche die Prefetch-größe, bei dem dwLen ein vielfaches der Prefetch-größe ist
	// (Es werden nur 2er Potenzen untersucht, ab 64Byte Größe (Also 64, 128, 256, 512...)
	DWORD dwBufferSize = 0;
	for (int nI = 6; nI<32;nI++)
	{
		if (TSTBIT(dwLen, nI))
		{
			dwBufferSize = SETBIT(dwBufferSize, nI);
			break;
		}
	}
//	ML_TRACE(_T("dwLen=%lu dwBufferSize=%lu\n"), dwLen, dwBufferSize);
	
	if (dwBufferSize)
	{
		dwBufferSize /= 8;				// Numbers of QWORDS
		dwLen /= 8;						// Numbers of QWORDS
		__asm{

			mov esi, pSrc				// source array
			mov edi, pDest				// destination array
			mov ecx, dwLen				// number of QWORDS (8bytes) (assumes len / CACHEBLOCK is an integer)
			lea esi, [esi+ecx*8]		// end of source
			lea edi, [edi+ecx*8]		// end of destination
			neg ecx						// use anegative offset as a combo pointer-and-loop-counter

			// Alle MMX-Register sichern
			movq RegBuffer[0],  mm0
			movq RegBuffer[8],  mm1
			movq RegBuffer[16], mm2
			movq RegBuffer[24], mm3
			movq RegBuffer[32], mm4
			movq RegBuffer[40], mm5
			movq RegBuffer[48], mm6
			movq RegBuffer[56], mm7

			mainloop:
				mov eax, dwBufferSize			// note: prefetch loop is unrolled 2X
				shr eax, 4						// Buffersize / 16
				add ecx, dwBufferSize			// move up to end of block

				prefetchloop:
					mov ebx, [esi+ecx*8-64]		// read one address in this cache line...
					mov ebx, [esi+ecx*8-128]	// ... and one in the previous line
					sub ecx, 16					// 16 QWORDS = 2 64-byte cache lines
					dec eax
					jnz prefetchloop

				mov eax, dwBufferSize
				shr eax, 3						// Buffersize / 8

				writeloop:
					movq mm0, qword ptr [esi+ecx*8+0]		// reads from cache!
					movq mm1, qword ptr [esi+ecx*8+8]		// reads from cache!
					movq mm2, qword ptr [esi+ecx*8+16]		// reads from cache!
					movq mm3, qword ptr [esi+ecx*8+24]		// reads from cache!
					movq mm4, qword ptr [esi+ecx*8+32]		// reads from cache!
					movq mm5, qword ptr [esi+ecx*8+40]		// reads from cache!
					movq mm6, qword ptr [esi+ecx*8+48]		// reads from cache!
					movq mm7, qword ptr [esi+ecx*8+56]		// reads from cache!

					movntq qword ptr [edi+ecx*8+0], mm0		// streaming store
					movntq qword ptr [edi+ecx*8+8], mm1		// streaming store
					movntq qword ptr [edi+ecx*8+16], mm2	// streaming store
					movntq qword ptr [edi+ecx*8+24], mm3	// streaming store
					movntq qword ptr [edi+ecx*8+32], mm4	// streaming store
					movntq qword ptr [edi+ecx*8+40], mm5	// streaming store
					movntq qword ptr [edi+ecx*8+48], mm6	// streaming store
					movntq qword ptr [edi+ecx*8+56], mm7	// streaming store

					add ecx, 8
					dec eax
					jnz writeloop

				or ecx, ecx 	// assumes integer number of cacheblocks
				jnz mainloop
			
			// Alle MMX-Register wieder herstellen
			movq mm0, RegBuffer[0]
			movq mm1 ,RegBuffer[8]
			movq mm2 ,RegBuffer[16]
			movq mm3 ,RegBuffer[24]
			movq mm4 ,RegBuffer[32]
			movq mm5 ,RegBuffer[40]
			movq mm6 ,RegBuffer[48]
			movq mm7 ,RegBuffer[56]

			sfence
			emms
		}
	}
}		