/////////////////////////////////////////////////////////////////////////////
// PROJECT:		CoCoUnit
// FILE:		$Workfile: cpydata.h $
// ARCHIVE:		$Archive: /Project/Units/CoCoUnit/cpydata.h $
// CHECKIN:		$Date: 5.08.98 9:43 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 5.08.98 9:43 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __cpydata_h__
#define __cpydata_h__

#ifndef WM_COPYDATA
#define WM_COPYDATA	0x004a

typedef struct tagCOPYDATASTRUCT {
	DWORD dwData;
	DWORD cbData;
	PVOID lpData;
} COPYDATASTRUCT, *PCOPYDATASTRUCT;
#endif //WM_COPYDATA

typedef struct tagPARAMSTRUCT {
	DWORD  dwData;
	PCSTR lpData;
	DWORD  dwLen;
	DWORD  dwExtra1;
	DWORD  dwExtra2;
	WORD   wCoCoID;
	WORD   wIntra;	// 1 = I-Bild, ansonsten 0
}PARAMSTRUCT, *PPARAMSTRUCT;

#ifdef __cplusplus
/////////////////////////////////////////////////////////////////////////////
// CWmCopyData::CWmCopyData
class CWmCopyData
{
public:
	CWmCopyData();

	~CWmCopyData();

	BOOL WriteData(HWND hWndTo, PARAMSTRUCT &Param);
	// Überträgt Daten von einer Applikation (hWndFrom) zu einer anderen (hWndTo)
	// hWndTo = Fensterhandle, desjenigen Fensters, an das die Daten geschickt werden
	//			sollen.
	// Param	= Referenz auf eine Struktur mit Zusatzparametern
	// Rückgabe: TRUE->Alles Ok
	
	BOOL ReadData(LPARAM lParam, PARAMSTRUCT &Param);
	// Diese Funktion muß bei eintreffen der Message 'WM_COPYDATA' aufgerufen werden.
	// lParam   = lParam beim Eintreffen der 'WM_COPYDATA'-Message
	// Rückgabe:	&Param	= Referenz auf eine Struktur mit Zusatzparametern
	// 				TRUE = Kein Fehler aufgetreten.
	// Notes:	  Diese Funktion MUß bei erhalt der Message WM_COPY_DATA aufgerufen
	//		      werden. Der zurückgelieferte Pointer ist temporär, d.h. er darf nicht 
	//			  gespeichert werden, um zu einem späteren Zeitpunkt auf dei Daten zuzugreifen.
	//			  Soll auf die Daten später zugegriffen werden, muß eine Kopie der Daten
	//			  angefertigt werden.

};
#endif // __cplusplus
#endif // __cpydata_h__