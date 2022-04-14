/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaDA
// FILE:		$Workfile: DirectAccess.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaDA/DirectAccess.h $
// CHECKIN:		$Date: 13.04.04 10:06 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 13.04.04 10:01 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __DIRECTACCESS_H__
#define __DIRECTACCESS_H__

#pragma warning(disable : 4244)  // disable 'conversion' conversion from 'type1' to 'type2' warning

#define DAEXP __declspec( dllexport )
////////////////////////////////////////////////////////////////
#define SETBIT(w,b)    ((DWORD)((DWORD)(w) | (1L << (b))))
#define CLRBIT(w,b)    ((DWORD)((DWORD)(w) & ~(1L << (b))))
#define TSTBIT(w,b)    ((BOOL)((DWORD)(w) & (1L << (b)) ? TRUE : FALSE))

// Versionsnummer von Tasha.Dll, TashaDA.dll, und Tasha.sys
#define TASHA_UNIT_VERSION				0x00040003

// BUS-Type
#define BUSTYPE_ISA					1
#define BUSTYPE_PCI					2

#define MAX_MSG_LEN						256
#define MAX_EVENTS						64

// Diese Eventarten gibt es
#define EVENT_INFO						0x0001		
#define EVENT_WARNING					0x0002
#define EVENT_ERROR						0x0003
#define EVENT_STAT_PEAK_INFO			0x0004
#define EVENT_STAT_PEAK_WARNING			0x0005
#define EVENT_STAT_PEAK_ERROR			0x0006

typedef ULONG	HIRQ;			// IRQ Handle
typedef DWORD	HDEVICE	;

typedef struct
{
	DWORD	dwInBufferSize;
	void*	pInBuffer;
	DWORD	dwOutBufferSize;
	void*	pOutBuffer;
	DWORD*	pBytesToReturn;
}IOCtlStruct;

// Io-Größenangabe 8Bit, 16Bit 32Bit or Buffer
#define IO_SIZE8	1
#define IO_SIZE16	2
#define IO_SIZE32	3
#define IO_SIZEX	4

typedef struct
{
	WORD  wIOSize;
	DWORD dwAddr;
	DWORD dwVal;
	WORD  wVal;
	BYTE  byVal;
	void* pBuffer;
	DWORD dwLen;
} IOSTRUCT;


typedef struct
{
	PVOID pPhysAddr;	// Physikalische Adresse
	PVOID pLinAddr;		// Lineare Adresse
	DWORD dwLen;
} MEMSTRUCT;

typedef struct
{
	int		  nCardIndex;
	MEMSTRUCT Buffer;
}FRAMEBUFFERSTRUCT;


// Der Rest dieses Headers wird in den Gerätetreibern nicht benötigt.
#ifndef DEVICEDRIVER
#include <Afxmt.h>

/////////////////////////////////////////////////////////////////////////////////////
// CIo Basisklasse. Kann nicht Instanziert werden
/////////////////////////////////////////////////////////////////////////////////////
typedef HANDLE (WINAPI *OPENVXDH) (HANDLE);

class DAEXP CIo
{
public:
	BYTE	Input(DWORD dwPort);									// 8Bit  Lesezugriff
	WORD	Inputw(DWORD dwPort);									// 16Bit Lesezugriff
	DWORD	Inputdw(DWORD dwPort);									// 32Bit Lesezugriff
	void	InputX(void* pDstBuffer, DWORD wIOAddr, DWORD dwLen);	// Buffer Lesezugriff
	
	void	Output(DWORD dwPort, BYTE byDatabyte);					// 8Bit	 Schreibzugriff
	void	Outputw(DWORD dwPort, WORD wDatabyte); 					// 16Bit Schreibzugriff
	void	Outputdw(DWORD dwPort, DWORD dwDatabyte); 				// 32Bit Schreibzugriff
	void	OutputX(void* pSrcBuffer, DWORD wIOAddr, DWORD dwLen);	// Buffer Schreibzugriff
	
	DWORD	GetIoBase(){return m_dwIOBase;}
	virtual ~CIo();

protected:
	CIo();
	CIo(DWORD dwIOBase);

	// Diese Funktionen müssen überschrieben werden.
	virtual BYTE	Input8(DWORD dwPort) const = 0;										// 8Bit  Lese-Zugriff
	virtual WORD	Input16(DWORD dwPort) const = 0;									// 16Bit Lese-Zugriff
	virtual DWORD	Input32(DWORD dwPort) const = 0;									// 32Bit Lese-Zugriff
	virtual void	InputNN(void* pDstBuffer, DWORD wIOAddr, DWORD dwLen) const = 0;	// Buffer Lesezugriff

	virtual void	Output8(DWORD dwPort, BYTE byDatabyte) const = 0;					// 8Bit	 Schreib-Zugriff
	virtual void	Output16(DWORD dwPort, WORD wDatabyte) const = 0;					// 16Bit Schreib-Zugriff
	virtual void	Output32(DWORD dwPort, DWORD dwDatabyte) const = 0;					// 32it Schreib-Zugriff
	virtual void	OutputNN(void* pSrcBuffer, DWORD wIOAddr, DWORD dwLen) const = 0;	// Buffer Schreibzugriff

private:
	static CCriticalSection m_csIo;					// Synchronisationsobjekt
	DWORD m_dwIOBase;

};												

/////////////////////////////////////////////////////////////////////////////////////
// CIoPort. Fürt IO-Port Zugriffe durch
/////////////////////////////////////////////////////////////////////////////////////
class DAEXP CIoPort : public CIo
{
public:
	CIoPort(DWORD dwIOBase);
	virtual ~CIoPort();

protected:
	virtual BYTE	Input8(DWORD dwIOAddr) const;						// 8Bit  Port-Lesezugriff
	virtual WORD	Input16(DWORD dwIOAddr) const;						// 16Bit Port-Lesezugriff
	virtual DWORD	Input32(DWORD dwIOAddr) const;						// 32Bit Port-Lesezugriff
	virtual void	InputNN(void* pDstBuffer, DWORD wIOAddr, DWORD dwLen) const;	// Memory-Buffer-Lesezugriff

	virtual void	Output8(DWORD dwIOAddr, BYTE byDatabyte) const;		// 8Bit	 Port-Schreibzugriff
	virtual void	Output16(DWORD dwIOAddr, WORD wDatabyte) const;		// 16Bit Port-Schreibzugriff
	virtual void	Output32(DWORD dwIOAddr, DWORD dwDatabyte) const;	// 32Bit Port-Schreibzugriff
	virtual void	OutputNN(void* pSrcBuffer, DWORD wIOAddr, DWORD dwLen) const;// Memory-Buffer-Schreibzugriff

private:

};	 

/////////////////////////////////////////////////////////////////////////////////////
// CIoMemory. Fürt IO-Memory Zugriffe durch
/////////////////////////////////////////////////////////////////////////////////////
class DAEXP CIoMemory	: public CIo
{
public:
	CIoMemory(DWORD dwIOBase);
	virtual ~CIoMemory();

protected:
	virtual BYTE	Input8(DWORD wIOAddr) const;								// 8Bit  Memory-Lesezugriff
	virtual WORD	Input16(DWORD wIOAddr) const;								// 16Bit Memory-Lesezugriff
	virtual DWORD	Input32(DWORD wIOAddr) const;								// 32Bit Memory-Lesezugriff
	virtual void	InputNN(void* pDstBuffer, DWORD wIOAddr, DWORD dwLen) const;	// Memory-Buffer-Lesezugriff

	virtual void	Output8(DWORD wIOAddr, BYTE byDatabyte) const;				// 8Bit	 Memory-Schreibzugriff
	virtual void	Output16(DWORD wIOAddr, WORD wDatabyte) const; 				// 16Bit Memory-Schreibzugriff
	virtual void	Output32(DWORD wIOAddr, DWORD dwDatabyte) const;			// 32Bit Memory-Schreibzugriff
	virtual void	OutputNN(void* pSrcBuffer, DWORD wIOAddr, DWORD dwLen) const;// Memory-Buffer-Schreibzugriff

private:
};												


#define PCI_CONFIG_ADDRESS	0xcf8
#define PCI_CONFIG_DATA		0xcfc

#define PCI_FUNCTION_ID		0xb1
#define PCI_BIOS_PRESENT	0x01
#define FIND_PCI_DEVICE		0x02
#define READ_CONFIG_BYTE	0x08
#define READ_CONFIG_WORD	0x09
#define READ_CONFIG_DWORD	0x0a

#define WRITE_CONFIG_BYTE	0x0b
#define WRITE_CONFIG_WORD	0x0c
#define WRITE_CONFIG_DWORD	0x0d

#define SUCCESSFUL			0x00
#define DEVICE_NOT_FOUND	0x86
#define BAD_VENDOR_ID		0x83

// Baseclass Defintitionen
#define BC_MASS_STORAGE_CONTROLLER		0x01
#define BC_NETWORK_CONTROLLER			0x02
#define BC_DISPLAY_CONTROLLER			0x03
#define BC_MULTIMEDIA_DEVICE			0x04
#define BC_MEMORY_CONTROLLER			0x05
#define BC_PCI_BRIDGE_DEVICE			0x06
#define BC_COMMUNICATION_CONTROLLER		0x07
#define BC_GENERIC_SYSTEM_PERIPHERAL	0x08
#define BC_INPUT_DEVICE					0x09
#define BC_DOCKING_STATION				0x0a
#define BC_PROZESSOR					0x0b
#define BC_SERIAL_BUS_CONTROLLER		0x0c

/////////////////////////////////////////////////////////////////////////////////////
// CPCIDevice. Kapselt ein PCI-Device
/////////////////////////////////////////////////////////////////////////////////////
class DAEXP CPCIDevice : public CObject
{
	friend class CPCIBus;
	public:

		// Liefert die VendorID des devices
		WORD GetVendorID() const {return ReadWord(0x00);};

		// Liefert die DeviceID des devices
		WORD GetDeviceID() const {return ReadWord(0x02);};
		
		// Liefert die Class des devices
		BYTE GetClass() const {return ReadByte(0x0b);};
		
		// Liefert die SubClass des devices
		BYTE GetSubClass() const {return ReadByte(0x0a);};

		// Liefert die SubSystemID des devices
		WORD GetSubsystemID()const {return ReadWord(0x2e);};

		// Liefert die SubSystemID des devices
		WORD GetSubsystemVendorID() const {return ReadWord(0x2c);};

		// Liefert die wIndex Membaseadresse des Devices
		DWORD GetMemBase(DWORD wIndex) const {return ReadDWord(0x10 + sizeof(DWORD) * wIndex);};

 		// Liefert die IRQLine des  Devices
		BYTE GetIRQLine() const {return ReadByte(0x3c);};

		// Liefert Busnummer und die Device/Funktionsnummer des Devices
		BOOL GetBusDevAndFunction(BYTE  &byBusNum, BYTE &byDevAndFunc) const;

		// Liest Daten aus dem Configuration space
		BYTE  ReadByte(WORD wAt) const;
		WORD  ReadWord(WORD wAt) const;
		DWORD ReadDWord(WORD wAt) const;

		// Schreibt Daten in das Configuration space
		void WriteByte(WORD wAt,  BYTE byData) const;
		void WriteWord(WORD wAt,  WORD wData) const;
		void WriteDWord(WORD wAt, DWORD dwData) const;

		~CPCIDevice(void);

	private:
		CPCIDevice(BYTE byBusNumber, BYTE nyDevAndFunc);

		BYTE	m_byBusNumber;
		BYTE	m_byDeviceAndFunction;
		CIo		*m_pIoAddress;
		CIo		*m_pIoData;
};

/////////////////////////////////////////////////////////////////////////////////////
// CPCIDeviceList. Eine Liste aller gefundenen PCI-Devices
/////////////////////////////////////////////////////////////////////////////////////
class DAEXP CDeviceList : public CObList
{
	public:
		BOOL AddDevice(CPCIDevice* pDevice);
		// Fügt ein Device der Liste hinzu, dabei wird zuvor überprüft,
		// ob sich das Device schon in der Liste befindet
		// Liefert TRUE, wenn das Deviceobjekt neu in die Liste aufgenommen
		// wurde.

		CPCIDevice*	GetDevice(WORD wVendorID, WORD wDeviceID, WORD wSubSystemID) const;
		// Such innerhalb der Liste nach einem Device, welches durch seine
		// VendorID und DeviceID spezifiziert wird. Befinden sich mehrere
		// Devices in der Liste, so wird der erste gefundene zurückgeliefert.
		// Wenn kein Device gefunden wird, so liefert die Funktion NULL

		CPCIDevice*	GetDevice(WORD wVendorID, WORD wDeviceID) const;
		// Such innerhalb der Liste nach einem Device, welches durch seine
		// VendorID und DeviceID spezifiziert wird. Befinden sich mehrere
		// Devices in der Liste, so wird der erste gefundene zurückgeliefert.
		// Wenn kein Device gefunden wird, so liefert die Funktion NULL

		CPCIDevice*	GetDevice(BYTE byBaseClass) const;
		// Such innerhalb der Liste nach einem Device, welches durch seine
		// BaseClass spezifiziert wird. Befinden sich mehrere
		// Devices in der Liste, so wird der erste gefundene zurückgeliefert.
		// Wenn kein Device gefunden wird, so liefert die Funktion NULL
		
		void Trace() const;
		// Gibt über ML_TRACE eine Liste aller gefundenen Devices aus.

};

/////////////////////////////////////////////////////////////////////////////////////
// CPCIBus. Scant den PCI-Bus
/////////////////////////////////////////////////////////////////////////////////////
class DAEXP CPCIBus
{
	friend class CPCIDevice;

	public:
		// Default Constructor
		CPCIBus(void);			

		// Default Destructor
		~CPCIBus(void);

		BOOL IsValid() const;

		// Scant den PCI-Bus und erstellt eine Liste aller gefundenen Devices.
		CDeviceList* ScanBus();

	protected:
		// Überprüft, ob PCI-Bios vorhanden ist.
		BOOL CheckPCI(void);

		WORD	m_wHardmech;
		WORD	m_wInterfaceLevel;
		BYTE	m_byLastNumber;
		BYTE	m_byBusNumber;
		BOOL	m_bPciPresent;

		CDeviceList *m_pDeviceList; 
};

/////////////////////////////////////////////////////////////////////////////////////
// Dies sind die eigendlichen Zugriffsfunktionen. Es ist auch möglich diese
// direkt zuverwenden und nicht die CIo-Klassen.
/////////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

DAEXP BOOL		DAIsValid();
DAEXP DWORD		DAGetDriverVersion();
DAEXP DWORD		DAGetTashaDAVersion();

// Gibt die Adresse eines statischen 2Mbyte Buffers zurück, der in DriverEntry alloziert wurde.
DAEXP BOOL		DAGetFramebuffer(int nCardIndex, MEMSTRUCT& mem);
DAEXP BOOL		DAGetTransferbuffer(int nCardIndex, MEMSTRUCT& mem);
DAEXP BOOL		DAWritePort8(DWORD dwAddr,  BYTE  byVal);
DAEXP BOOL		DAWritePort16(DWORD dwAddr, WORD  wVal);
DAEXP BOOL		DAWritePort32(DWORD dwAddr, DWORD dwVal);
DAEXP BYTE		DAReadPort8(DWORD dwAddr);
DAEXP WORD		DAReadPort16(DWORD dwAddr);
DAEXP DWORD		DAReadPort32(DWORD dwAddr);
DAEXP BOOL		DAWriteMemory8(DWORD dwAddr,  BYTE  byVal);
DAEXP BOOL		DAWriteMemory16(DWORD dwAddr, WORD  wVal);
DAEXP BOOL		DAWriteMemory32(DWORD dwAddr, DWORD dwVal);
DAEXP BOOL		DAWriteMemoryX(void* pSrcBuffer, DWORD dwAddr, DWORD dwLen);
DAEXP BYTE		DAReadMemory8(DWORD dwAddr);
DAEXP WORD		DAReadMemory16(DWORD dwAddr);
DAEXP DWORD		DAReadMemory32(DWORD dwAddr);
DAEXP BOOL		DAReadMemoryX(void* pDestBuffer, DWORD dwAddr, DWORD dwLen);

#ifdef __cplusplus
}
#endif

#endif // DEVICEDRIVER
#endif // __DIRECTACCESS_H__
