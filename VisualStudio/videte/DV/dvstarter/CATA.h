// Definitionen für Windows NT sowie 2000 und XP 
// aus dem DDK-Headerfile ntddscsi.h übernommen
#define FILE_DEVICE_CONTROLLER      0x00000004
#define IOCTL_SCSI_BASE             FILE_DEVICE_CONTROLLER
#define FILE_READ_ACCESS            0x0001
#define FILE_WRITE_ACCESS           0x0002
#define METHOD_BUFFERED             0
#define CTL_CODE(DeviceType, Function, Method, Access) (((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))

//---------------------------------------------------
// Definitionen und Strukturen für Windows NT
// aus dem DDK-Headerfile ntddscsi.h übernommen

// dokumentierter Funktionscode für SCSI
#define IOCTL_SCSI_PASS_THROUGH     CTL_CODE(IOCTL_SCSI_BASE, 0x0401, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define CDB10GENERIC_LENGTH         10
#define SCSI_IOCTL_DATA_OUT          0
#define SCSI_IOCTL_DATA_IN           1
#define SCSI_IOCTL_DATA_UNSPECIFIED  2
// undokomentierter Pseudo-SCSI-Befehl
#define SCSIOP_ATA_PASSTHROUGH      0xCC

//---------------------------------------------------
// Definitionen und Datenstruktur für Windows 2000 und XP
// aus dem DDK-Headerfile ntddscsi.h übernommen

#define IOCTL_IDE_PASS_THROUGH  CTL_CODE(IOCTL_SCSI_BASE, 0x040A, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define FILE_ANY_ACCESS         0
#define IOCTL_SCSI_RESCAN_BUS   CTL_CODE(IOCTL_SCSI_BASE, 0x0407, METHOD_BUFFERED, FILE_ANY_ACCESS)

//----------------------------------------------------------------------------
// Klassendefinition von CWinATA

class CWinAta  
{
public:
	CWinAta(UINT DeviceNum);
	~CWinAta();

	// Fehlercodes der Klasse CWinAta
	typedef enum 
	{
		aec_Ok               = 0, 
		aec_OSNotSupported   = 1, 
		aec_InvalidDevice    = 2, 
		aec_InvalidParameter = 3, 
		aec_OutOfMemory      = 4, 
		aec_CommandFailed    = 5, 
		aec_LAST
	} TWinAtaErrorCode;

	TWinAtaErrorCode GetAtaErrorCode(){return m_WinAtaErrorCode;};
	BOOL EnableWriteCache();
	BOOL DisableWriteCache();

private:
	// Registersatz des ATA-Hostadapter
	typedef struct
	{
		UCHAR Reg[7];
	} TATARegs, *PATARegs;

	// AtaErrorcode
	TWinAtaErrorCode m_WinAtaErrorCode;

	// Handle des geöffneten Gerätetreibers
	HANDLE m_hDevice;

	// Windows NT 4 verwendet eine andere Schnittstelle
	BOOL   m_bIsNT4;

	// SendCommand sendet ATA-Befehle
	BOOL SendCommand(PATARegs pATARegs, void *pBuf, UINT BufSize, UINT *pResultSize);

	// interne Funktionen, je nach Betriebssystem
	BOOL SendCommand_WinNT(PATARegs pATARegs, void *pBuf, UINT BufSize, UINT *pResultSize);
	BOOL SendCommand_Win2000(PATARegs pATARegs, void *pBuf, UINT BufSize, UINT *pResultSize);

	//---------------------------------------------------
	// Registerdefinition des ATA-Controllers laut Windows-Schnittstelle
	// aus DDK-Headerfile ntdddisk.h übernommen

	typedef struct
	{
		UCHAR bFeaturesReg;
		UCHAR bSectorCountReg;
		UCHAR bSectorNumberReg;
		UCHAR bCylLowReg;
		UCHAR bCylHighReg;
		UCHAR bDriveHeadReg;
		UCHAR bCommandReg;
		UCHAR bReserved;
	} IDEREGS, *PIDEREGS;
};
