/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: cpci.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicDll/cpci.h $
// CHECKIN:		$Date: 2.07.02 13:35 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 2.07.02 12:33 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CPCI_H__
#define __CPCI_H__

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

class CPCIDeviceList;	
class CIo;
class AFX_EXT_CLASS CPCIBus
{
	friend class CPCIDevice;

	public:
		// Default Constructor
		CPCIBus(void);			

		// Default Destructor
		~CPCIBus(void);

		BOOL IsValid() const;

		// Scant den PCI-Bus und erstellt eine Liste aller gefundenen Devices.
		CPCIDeviceList* ScanBus();

	protected:
		// Überprüft, ob PCI-Bios vorhanden ist.
		BOOL CheckPCI(void);

		WORD	m_wHardmech;
		WORD	m_wInterfaceLevel;
		BYTE	m_byLastNumber;
		BYTE	m_byBusNumber;
		BOOL	m_bPciPresent;

		CPCIDeviceList *m_pDeviceList; 
};

///////////////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CPCIDevice : public CObject
{
	friend class CPCIBus;
	public:

		// Liefert die VendorID des devices
		WORD GetVendorID() const {return ReadWord(0x00);};

		// Liefert die DeviceID des devices
		WORD GetDeviceID() const {return ReadWord(0x02);};
		
		// Liefert die CommandAndStatus des devices
		DWORD GetCommandAndStatus() const {return ReadDWord(0x04);};

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

		// Liefert Device Control des devices
		BYTE GetDeviceControl() const {return ReadByte(0x40);};
		
		// Setzt DeviceControl des devices
		void SetDeviceControl(BYTE byVal) const {WriteByte(0x40, byVal);};

		// Liefert PowerManagementSupport des devices
		DWORD GetPowerManagementSupport() const {return ReadDWord(0x50);};

		// Setzt PowerManagementSupport des devices
		void SetPowerManagementSupport(DWORD dwVal) const {WriteDWord(0x50, dwVal);};

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

///////////////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CPCIDeviceList : public CObList
{
	public:
		BOOL AddDevice(CPCIDevice* pDevice);
		// Fügt ein Device der Liste hinzu, dabei wird zuvor überprüft,
		// ob sich das Device schon in der Liste befindet
		// Liefert TRUE, wenn das Deviceobjekt neu in die Liste aufgenommen
		// wurde.

		CPCIDevice*	GetDevice(WORD wVendorID, WORD wDeviceID, WORD wSubSystemID) const;
		// Sucht innerhalb der Liste nach einem Device, welches durch seine
		// VendorID,DeviceID und SubSystemID spezifiziert wird. Befinden sich mehrere
		// Devices in der Liste, so wird der erste gefundene zurückgeliefert.
		// Wenn kein Device gefunden wird, so liefert die Funktion NULL

		CPCIDevice*	GetDevice(WORD wVendorID, WORD wDeviceID) const;
		// Sucht innerhalb der Liste nach einem Device, welches durch seine
		// VendorID und DeviceID spezifiziert wird. Befinden sich mehrere
		// Devices in der Liste, so wird der erste gefundene zurückgeliefert.
		// Wenn kein Device gefunden wird, so liefert die Funktion NULL

		CPCIDevice*	CPCIDeviceList::GetDeviceByIndex(WORD wVendorID, WORD wDeviceID, int nIndex) const;
		// Sucht innerhalb der Liste nach einem Device, welches durch seine
		// VendorID und DeviceID spezifiziert wird. Befinden sich mehrere
		// Devices in der Liste, so wird das Device Nummer 'nIndex' zurückgeliefert.
		// Wenn kein Device gefunden wird, so liefert die Funktion NULL

		CPCIDevice*	GetDevice(BYTE byBaseClass) const;
		// Sucht innerhalb der Liste nach einem Device, welches durch seine
		// BaseClass spezifiziert wird. Befinden sich mehrere
		// Devices in der Liste, so wird der erste gefundene zurückgeliefert.
		// Wenn kein Device gefunden wird, so liefert die Funktion NULL
		
		void Trace() const;
		// Gibt über ML_TRACE eine Liste aller gefundenen Devices aus.

};

#endif // __CPCI_H__
