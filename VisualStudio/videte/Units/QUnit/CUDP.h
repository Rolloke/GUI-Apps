//#pragma once
#ifndef __CUDP_H__
#define __CUDP_H__

#include "CMDpoints.h"
#include "Picturedef.h"
#include "systemtime.h"

//////////////////////////////////////////////////////////////////////
struct CMN_HWIDnSN {
	DWORD HWID;
	BYTE SN[16];
};

//////////////////////////////////////////////////////////////////////
typedef struct {
	int				nCamera;			// Kameranummer (0...MAXCAMERAS-1)
	int				nWidth;				// Reale horizontale Auflösung in Pixeln
	int				nHeight;			// Reale vertikale Auflösung in Pixeln
	float			fFramerate;			// Aktuell ermittelte Framerate
	CSystemTime		TimeStamp;			// Timestamp
	CompressionType	ct;					// Derzeit immer COMPRESSION_YUV_422
	CMDPoints*		pPMDoints;			// Pointer auf die Bewegungskoordinaten
	LPBITMAPINFO	lpBMI;				// Pointer auf die Bitmap
} QIMAGE;

// Status der beiden LEDs
typedef enum
{
	eLED1_On,
	eLED1_Blink,
	eLED2_On,
	eLED2_Off
}eLEDState;

typedef struct
{
	DWORD			dwUserData;
	Resolution		Resolution;
	int				iFPS;
	BOOL			bEncoderState;
}VideoStream;

#define MAX_BOARDS  4
#define MAX_CHANNEL_PER_BOARD 32

#define MAX_CAMERAS (MAX_BOARDS*MAX_CHANNEL_PER_BOARD)

// #error Use your company's own data for the following variable and delete this directive(line).


class CQUnitDlg;
class CIPCActivityMask;
class CMotionDetection;
class CUDP
{
public:
	CUDP(CQUnitDlg* pMainDlg, const CString& sIniFile, int nImgFormat=CMN5_VIDEO_FORMAT_PAL_B, int nWidth=704, int nHeight=288);
	virtual ~CUDP(void);

	BOOL IsValid();
	
	BOOL GetDriverInformation();
	// Ermittelt alle notwendigen Informationen der installierten UDP-Boards.
	// Wie z.B. die Anzahl der vorhandenen Boards. Anzahl der Channel pro Boards usw...

	BOOL LoadConfig();
	// Liest Einstellungen wie Helligkeit, Kontrast, Farbsättigung aus der Registry und setzt diese.
	
	int GetAvailableCameras();
	// Liefert die Gesamtanzahl der verfügbaren Kameras
	
	int GetAvailableRelays();
	// Liefert die Gesamtanzahl der verfügbaren Relay

	int GetAvailableInputs();
	// Liefert die Gesamtanzahl der verfügbaren Alarmeingänge

	BOOL SetActiveCamera(int nActiveCamera);
	// Setzt die aktive Kamera

	int GetActiveCamera();
	// Fragt ab, welche kamera aktiv ist.

	virtual BOOL DoRequestSetBrightness(int nCamera, int nValue);
	// Setzt die Helligkeit (0...255) der gewünschten Kamera
	
	virtual BOOL OnConfirmSetBrightness(int nCamera, int nValue);
	// Helligkeit (0...255) der gewünschten Kamera wurde gesetzt

	virtual BOOL DoRequestSetContrast(int nCamera, int nValue);
	// Setzt den Kontrast (0...255) der gewünschten Kamera

	virtual BOOL OnConfirmSetContrast(int nCamera, int nValue);
	// Kontrast (0...255) der gewünschten Kamera wurde gesetzt

	virtual BOOL DoRequestSetSaturation(int nCamera, int nValue);
	// Setzt die Farbsättigung (0...255) der gewünschten Kamera

	virtual BOOL OnConfirmSetSaturation(int nCamera, int nValue);
	// Farbsättigung (0...255) der gewünschten Kamera wurde gesetzt

	virtual BOOL DoRequestSetHue(int nCamera, int nValue);
	// Setzt die Phase (0...255) der gewünschten Kamera
	
	virtual BOOL OnConfirmSetHue(int nCamera, int nValue);
	// Phase (0...255) der gewünschten Kamera wurde gesetzt

	virtual BOOL DoRequestGetBrightness(int nCamera, int& nValue);
	// Fragt die eingestellte Helligkeit der gewünschten Kamera ab.

	virtual BOOL OnConfirmGetBrightness(int nCamera, int nValue);
	// Helligkeitsabfrage (0...255) der gewünschten Kamera beendet.

	virtual BOOL DoRequestGetContrast(int nCamera, int& nValue);
	// Fragt den eingestellte Kontrast der gewünschten Kamera ab.

	virtual BOOL OnConfirmGetContrast(int nCamera, int nValue);
	// Kontrastabfrage (0...255) der gewünschten Kamera beendet.

	virtual BOOL DoRequestGetSaturation(int nCamera, int& nValue);
	// Fragt die eingestellte Farbsättigung der gewünschten Kamera ab.

	virtual BOOL OnConfirmGetSaturation(int nCamera, int nValue);
	// Farbsättigungabfrage (0...255) der gewünschten Kamera beendet.

	virtual BOOL DoRequestGetHue(int nCamera, int& nValue);
	// Fragt die eingestellte Phase der gewünschten Kamera ab.

	virtual BOOL OnConfirmGetHue(int nCamera, int nValue);
	// Phasenabfrage (0...255) der gewünschten Kamera beendet.

	virtual BOOL DoRequestGetVideoState(int nCamera, BOOL& bPresent);
	// Liefert bPresent=TRUE, wenn die Kamera ein Videosignal liefert, ansonsten bPresent=FALSE

	virtual BOOL OnConfirmGetVideoState(int nCamera, BOOL bPresent);

	virtual BOOL DoRequestSetRelay(DWORD dwMask);
	// Schaltet die Relays über die Bitmaske dwMask (0=Relay angezogen, 1=Relay abgefallen)

	virtual BOOL OnConfirmSetRelay(DWORD dwMask);
	// Relays wurden geschaltet.

	virtual BOOL DoRequestGetRelay(DWORD& dwCompleteMask);
	// Liefert den Status der Relays bitkodiert.

	virtual BOOL OnConfirmGetRelay(DWORD dwCompleteMask);
	// Status der Relayzustände

	virtual BOOL DoRequestSetAlarmEdge(DWORD dwEdgeMask);
	// Konfiguriert über die Bitmaske dwEdgeMask, ob ein Alarmeingang als Öffner oder Schließer arbeitet.

	virtual BOOL OnConfirmSetAlarmEdge(DWORD dwEdgeMask);
	// Alarmedge wurde gesetzt.

	virtual DWORD DoRequestGetAlarmEdge();
	// Liefert die momentane Edgemake zurück
	
	virtual BOOL OnConfirmGetAlarmEdge(DWORD dwEdgeMask);
	// Aktuell eingestelle Edgemaske

	DWORD GetCurrentAlarmState();
	// Liefert den aktuellen Zustand aller Alarmeingänge.

	virtual BOOL DoRequestSetFramerate(int nCamera, int nFps);
	// Setzt fie Framerate der gewünschten Kamera
	
	virtual BOOL OnConfirmSetFramerate(int nCamera, int nFps);
	// Framerate der gewünschten Kamera wurde gesetzt

	virtual BOOL DoRequestGetFramerate(int nCamera, int& nFps);
	// Liefert die eingestellte Framerate der gewünschten Kamera

	virtual BOOL OnConfirmGetFramerate(int nCamera, int nFps);
	// Aktuelle Framerate der kamera

	virtual BOOL DoRequestSetImageSize(int nCamera, CSize ImageSize);
	// Setzt die Auflösung des Kanals.

	virtual BOOL OnConfirmSetImageSize(int nCamera, CSize ImageSize);
	// Auflösung wurde gesetzt

	virtual CSize DoRequestGetImageSize(int nCamera);
	// Liefert die Auflösung des Kanals

	virtual BOOL OnConfirmGetImageSize(int nCamera, CSize ImageSize);
	// Antwort auf DoRequestGetImageResolution 

	__int64 ScanForCameras();
	// Liefert eine 64Bit Maske der erkannten Videosignale.

	virtual BOOL SetLEDState(eLEDState ledState);
	// Setzt den Status der beiden externen LEDs (TODO: Derzeit nicht implementiert.)
	// LED1: eLED1_On
	//		 eLED1_Blink
	// LED2: eLED2_On
	// LED2: eLED2_Off

	BOOL SetCameraToAnalogOut(int nCamera);
	// Schaltet die gewünschte Kamera auf den gewünschten externen Ausgang
	// nCamera = Kameranummer

	virtual BOOL ActivateMotionDetection(int nCamera);
	// Aktiviert für die Kamera nCamera die MotionDetection

	virtual BOOL DeactivateMotionDetection(int nCamera);
	// Deaktiviert für die Kaera nCamera die MotionDetection

	virtual BOOL SetMDMaskSensitivity(int nCamera, const CString &sLevel);
	// Setzt die Empfindlichkeit der adaptiven Maske in 3 Stufen.
	// nCamera = Kamera, deren Empfindlichkeit gesetzt werden soll
	// sLevel = High, Normal, Low

	virtual BOOL SetMDAlarmSensitivity(int nCamera, const CString &sLevel);
	// Setzt die Empfindlichkeit der Motiondetection in 3 Stufen.
	// nCamera = Kamera, deren Empfindlichkeit gesetzt werden soll
	// wLevel = High, Normal, Low

	virtual CString GetMDMaskSensitivity(int nCamera);
	// Liefert die Empfindlichkeit der adaptiven Maske in 3 Stufen.
	// nCamera = Kamera, deren Empfindlichkeit gesetzt werden soll

	virtual CString GetMDAlarmSensitivity(int nCamera);
	// Liefert die Empfindlichkeit der Motiondetection in 3 Stufen.
	// nCamera = Kamera, deren Empfindlichkeit gesetzt werden soll

	virtual BOOL GetMask(int nCamera, CIPCActivityMask& mask);
	// Liefert für eine bestimmte Kamera die MD-Maske
	// nCamera = Kamera, deren Maske angefordert wird.
	// Der Typ der Maske (permanent/dynamisch) muß in mask vorbelegt sein.

	virtual BOOL SetMask(int nCamera, const CIPCActivityMask& mask);
	// Setzt für eine bestimmte Kamera die MD-Maske
	// nCamera = Kamera, deren Maske gesetzt werden soll..

	CSize GetImageSize(int nCamera);
	// Liefert die geometrische Auflösung des Kanals.

	void OnDlgMdConfig(HWND hWnd);
	// Öffnet den Konfigurationsdialog der MotionDetection.

	BOOL UpdateFramerateCalculation(int nActiveCamera, int nActiveFps);
	// Setzt die Aktive Kamera auf 'ActiveFps'-fps und verteilt den Rest der 100fps gleichmäßig auf 
	// die übrigen aktiven Kameras.

	BOOL CheckResolution(int nBoard, CSize ImageSize);
	// Prüft, ob die geforderte Auflösung unterstützt wird.
private:
	BOOL InitializeDriver();

	BOOL GetActCodeFromModelID(DWORD hwid,UCHAR * sn);
	
	BOOL CalculateFramerate(int nCamera, float& fFramerate);
	// Ermittelt die Framerate anhand der Zeit für die letzten 25 Bilder.

	BOOL ResetFramerates();
	// Setzt die Frameraten aller Boards anhand der zuletzt gesetzten Frameraten. Dies ist z.B. 
	// nach einer Änderung der Auflösung notwendig.

	BOOL InitConversionTables();
	// Legt die Tabellen an, für die Umwandlung der nCamera in nBoardID/nChannel und
	// nBoardID/nChannel zurück in nCamera.

	BOOL GetBoardIDandChannelFromCamera(int nCamera, int& nBoardID, int& nChannel);
	// Liefert anhand der Kameranummer (0...MAX_CAMERAS-1) die BoardNummer (0...CAP_SYSTEM_MAXBOARD-1)
	// und die Kanalnummer (0...16 bzw. 0...31)
	
	BOOL GetCameraFromBoardIDandChannel(int nBoardID, int nChannel, int& nCamera);
	// Liefert anhand der BoardNummer(0...CAP_SYSTEM_MAXBOARD-1) und Kanalnummer (0...16 bzw. 0...31)
	// die Kameranummer (0...MAX_CAMERAS-1)

	BOOL TriggerWatchDog();
	BOOL TriggerPiezo();

	BOOL SendTestImage(int nCamera);

	virtual void OnReceivedMotionAlarm(int nCamera, CMDPoints& Points) const;

	BOOL StartCaptureThread();
	BOOL StopCaptureThread();

	BOOL StartExternEventThread();
	BOOL StopExternEventThread();

	BOOL OnCapture(CAP5_DATA_INFO& ImgInfo);
	BOOL OnSensor(CMN5_SENSOR_STATUS_INFO& SensorStatusInfo);
	BOOL OnVideoState(CMN5_VIDEO_STATUS_INFO& VideoStatusInfo);

	static UINT OnCaptureThreadProcedure(LPVOID pData);
	static UINT OnExternEventThreadProcedure(LPVOID pData);

	BOOL					m_bRunCaptureThread;
	BOOL					m_bRunExternEventThread;
	BOOL					m_bValid;
	CQUnitDlg*				m_pMainDlg;
	CMotionDetection*		m_pMotionDetection;

	CString					m_sIniFile;
	int						m_nVideoFormat;
	int						m_nColorFormat;
	int						m_nImgWidth;
	int						m_nImgHeight;

	int						m_nActiveCamera;
	int						m_nMaxChannelFps;
	int						m_nMaxBoardFps;
	DWORD					m_dwCompleteEdgeMask;
	DWORD					m_dwCompleteSensorMask;

	HANDLE					m_hWatchDogTriggerEvent;		// WatchdogTriggerEvent.
	HANDLE					m_hPiezoEvent;					// Störmelder an/aus

	CWinThread*				m_pCaptureThreadProc;
	CWinThread*				m_pExternEventThreadProc;			// Wait for extern Event
	CEvent					m_evStopCaptureThread;

	CMN5_SYSTEM_INFO		m_SystemInfo;
	CAP5_BOARD_INFO*		m_pBdInfo;

	BOOL					m_bVideoStatus[MAX_CAMERAS];

	BOOL					m_bConversionTablesReady;
	int						m_nCamera[MAX_BOARDS][MAX_CHANNEL_PER_BOARD];
	int						m_nBoardID[MAX_CAMERAS];
	int						m_nChannel[MAX_CAMERAS];
	BYTE*					m_pFpsList[MAX_BOARDS];
	int						m_nAvailableCameras;
	int						m_nAvailableRelays;
	int						m_nAvailableInputs;

	float					m_fCurrentFramerate[MAX_CAMERAS];
	DWORD					m_dwTestImageLen;
	BYTE*					m_lpTestImage;
};
#endif // __CUDP_H__