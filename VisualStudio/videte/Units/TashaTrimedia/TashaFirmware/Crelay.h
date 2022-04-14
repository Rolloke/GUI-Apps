/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: Crelay.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/Crelay.h $
// CHECKIN:		$Date: 11.12.01 9:23 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 11.12.01 9:14 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CRELAY_H__ 
#define __CRELAY_H__
 
class CRelay
{						  
public:
	CRelay();

	virtual ~CRelay();

	virtual void SetRelay(DWORD dwRelayMask);
	// Setzt/Löscht die Relais 				  

	virtual DWORD GetRelay() const;
	// Liefert den den augenblicklichen Status der Relays

	virtual void BeeperOn() const;
	// Schalten den Störmelder ein

	virtual void BeeperOff() const;
	// Schalten den Störmelder aus

protected:
	DWORD m_dwRelayStateMask;
};
		
#endif // __CRELAY_H__
