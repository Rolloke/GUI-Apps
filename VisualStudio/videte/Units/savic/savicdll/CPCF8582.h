/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: CPCF8582.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicDll/CPCF8582.h $
// CHECKIN:		$Date: 10.12.02 15:33 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 10.12.02 15:32 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPCF8582_H__2E661CE3_F0EB_11D3_8E46_004005A11E32__INCLUDED_)
#define AFX_CPCF8582_H__2E661CE3_F0EB_11D3_8E46_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Aufbau der EEProms:
// -------------------
//  0x11 0x07 0x19 0x64								Header
//	0xYY 0xYY 0xYY 0xYY	0xYY 0xYY 0xYY 0xYY			16 Bytes Seriennummer + '\0'
//	0xYY 0xYY 0xYY 0xYY	0xYY 0xYY 0xYY 0xYY 0x00		"	
//	0xXX 0xXX										2 Byte Dongleinformationen

#define EE_MH_OFFSET	0
#define EE_MH_LEN		4
#define EE_SN_OFFSET	4
#define EE_SN_LEN		16
#define EE_DO_OFFSET	21
#define EE_DO_LEN		2
#define EE_SVI_OFFSET	0xfc
#define EE_SVI_LEN		4
#define MAGIC_HEADER	0x11071964

#define EE_DONGLE_BIT_ISDN		 0
#define EE_DONGLE_BIT_TCP		 1
#define EE_DONGLE_BIT_BACKUP	 2
#define EE_DONGLE_BIT_BACKUP_DVD 3

class CI2C;
class CPCF8582  
{
public:
	CPCF8582(DWORD dwIOBase);
	virtual ~CPCF8582();

	BOOL ClearEEProm();
	// Löscht das EEProm (Adr:0x00...0xFB)
	// Liefert im Fehlerfall FALSE

	BOOL	WriteSubsystemID(WORD wSubsystemID);
	// Schreibt die SubsystemID an die Adresse 0xfc,0xfd
	// Liefert im Fehlerfall FALSE

	BOOL	ReadSubsystemID(WORD& wSubsystemID);
	// Liest die SubsystemID aus dem EEProm
	// Liefert im Fehlerfall FALSE

	BOOL	WriteSN(const CString &sSN);
	// Schreibt die Seriennummer in das EEProm
	// sSN = Seriennummer
	// Liefert im Fehlerfall FALSE
	// Anmerkung: Die Seriennummer muß 16 Stellen besitzen.

	BOOL	ReadSN(CString& sSN);
	// Liest die 16 stellige Seriennummer aus dem EEProm
	// Liefert im Fehlerfall FALSE
	
	BOOL	ReadDongleInformation(WORD& wDongle);
	// Liest 16Bit Dongle-Informationen aus dem EEProm
	// Bit 0	->	1=ISDN Erlaubt, 0=ISDN nicht erlaubt
	// Bit 1	->	1=TCP  Erlaubt, 0=TCP nciht erlaubt

	BOOL WriteDongleInformation(WORD wDongle);
	// Schreibt 16Bit Dongle-Informationen in das EEProm
	// Bit 0	->	1=ISDN Erlaubt, 0=ISDN nicht erlaubt
	// Bit 1	->	1=TCP  Erlaubt, 0=TCP nciht erlaubt
	
	BOOL	ReadMagicHeader(DWORD& Header);
	// Überprüft, ob das EEProm einen bestimmten Header besitzt.
	// Ist dies nicht der Fall, so wird FALSE zurückgeliefert.

	BOOL WriteToEEProm(WORD wAddr, BYTE byVal);
	// Schreibt das Byte 'byVal' an die Position 'wAddr' des EEProms.
	// Liefert im Fehlerfall FALSE

	BOOL ReadFromEEProm(WORD wAddr, BYTE& byVal);
	// Liest ein Byte von der Position 'wAddr' aus dem EEProm.
	// Liefert im Fehlerfall FALSE

	BOOL DumpEEProm();
	// Liest alle 256Bytes des EEProms aus und schreibt sie in Logfile.

private:
	CCriticalSection	m_csPCF8582;
	CI2C*				m_pCI2C;
};

#endif // !defined(AFX_CPCF8582_H__2E661CE3_F0EB_11D3_8E46_004005A11E32__INCLUDED_)
