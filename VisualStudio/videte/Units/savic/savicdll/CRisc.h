// CRisc.h: Schnittstelle für die Klasse CRisc.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRISC_H__2846C034_334B_4DF1_ABF2_B0C2FB6D43F8__INCLUDED_)
#define AFX_CRISC_H__2846C034_334B_4DF1_ABF2_B0C2FB6D43F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define OP_WRITE		1
#define OP_WRITE123		9
#define OP_WRITE1S23	11
#define OP_WRITEC		5
#define OP_SKIP			2
#define OP_SKIP123		10
#define OP_JUMP			7
#define OP_SYNC			8

class CIo;
class CBT878;
class CRisc  
{
public:
	CRisc(CBT878* pBT878);
	
	virtual ~CRisc();
	void Disassamble(DWORD dwLen);

protected:
	BOOL HandleOpWrite();
	BOOL HandleOpWrite123();
	BOOL HandleOpWrite1S23();
	BOOL HandleOpWriteC();
	BOOL HandleOpSkip();
	BOOL HandleOpSkip123();
	BOOL HandleOpJump();
	BOOL HandleOpSync();

	
private:
	DWORD		m_dwRiscPrgPhys;	// physikalische Adresse des Riscprogrammes
	DWORD		m_dwRiscPrgSize;	// Größe des gemappten Speichers für das Riscprogramm
	DWORD		m_dwTargetAddr;		// Aktuelle DMA-Zieladresse
	int			m_nPC;				// ProgrammCounter
	CIo*		m_pIoMem;
};

#endif // !defined(AFX_CRISC_H__2846C034_334B_4DF1_ABF2_B0C2FB6D43F8__INCLUDED_)
