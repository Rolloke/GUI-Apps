#ifndef __CRELAY_H__ 
#define __CRELAY_H__

#include "CIo.h"
 
class CRelay
{
	public:
		CRelay(WORD wIOBase, WORD wExtCard);
		// Konstruktor
		//		wIOBase  = Basisadresse des Boards
		//		wExtCard = Nummer des Boards (0=MiCo, 1...3=CoVi1...CoVi3)

		void SetRelay(BYTE byRelay);
		// Setzt/Löscht die 4 Relais (4Bit Maske)

		BYTE GetRelay();
		// Liefert den den augenblicklichen Status der 4 Relays (4Bit Maske)
		
	private:
		WORD m_wIOBase;			// Basisadresse des Boards
		WORD m_wExtCard;		// Nummer des Boards (0=MiCo, 1...3=CoVi1...CoVi3)
		CIo  m_IoAccess;	    // I/O-Objekt über das die I/O-Zugriffe erfolgen
};
		
#endif // __CRELAY_H__


