/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: CEEProm.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaDll/CEEProm.h $
// CHECKIN:		$Date: 28.04.04 12:44 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 28.04.04 11:03 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CEEPROM_H__2E661CE3_F0EB_11D3_8E46_004005A11E32__INCLUDED_)
#define AFX_CEEPROM_H__2E661CE3_F0EB_11D3_8E46_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Aufbau der EEProms:
// -------------------
//  0x11 0x07 0x19 0x64								Header
//	0xYY 0xYY 0xYY 0xYY	0xYY 0xYY 0xYY 0xYY			20 Bytes Seriennummer
//	0xYY 0xYY 0xYY 0xYY	0xYY 0xYY 0xYY 0xYY 0x00		"	
//	0xXX 0xXX										2 Byte Dongleinformationen
#define EE_BASE			(8*1024-256)

#define EE_MH_OFFSET	(EE_BASE + 0)
#define EE_MH_LEN		4
#define EE_SN_OFFSET	(EE_BASE + 4)
#define EE_SN_LEN		20
#define EE_DO_OFFSET	(EE_BASE + 24)
#define EE_DO_LEN		2
#define EE_SSID_OFFSET	(EE_BASE + 26)
#define EE_SSID_LEN		2
#define EE_SSVID_OFFSET	(EE_BASE + 28)
#define EE_SSVID_LEN	2
#define MAGIC_HEADER	0x11071964

#define EE_DONGLE_BIT_ISDN		 0
#define EE_DONGLE_BIT_TCP		 1
#define EE_DONGLE_BIT_BACKUP	 2
#define EE_DONGLE_BIT_BACKUP_DVD 3

class CCodec;
class CEEProm  
{
public:
	CEEProm(CCodec* pCodec);
	virtual ~CEEProm();

	BOOL ClearEEProm();
	// Löscht das EEProm (Adr:EE_BASE...EE_BASE+255)
	// Liefert im Fehlerfall FALSE

	BOOL WriteSubsystemID(WORD wSubsystemID, WORD wSubsystemVendorID);
	// Schreibt die SubsystemID und SubsystemVendorID ins EEProm
	// Liefert im Fehlerfall FALSE

	BOOL	ReadSubsystemID(WORD& wSubsystemID, WORD& wSubsystemVendorID);
	// Liest die SubsystemID und SubsystemVendorID aus dem EEProm
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

	BOOL DumpEEProm();
	// Liest alle 8KBytes des EEProms aus und schreibt sie in Logfile.

private:
	CCriticalSection	m_csEEPRom;
	CCodec*				m_pCodec;
};

#endif // !defined(AFX_CEEPROM_H__2E661CE3_F0EB_11D3_8E46_004005A11E32__INCLUDED_)
