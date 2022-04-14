////////////////////////////////////////////////////////////////////////////
#ifndef __WK_MSG_H__
#define __WK_MSG_H__

/////////////////////////////////////////////////////////////////////////////
// Messages reserved for Apllications
#define APPLICATION_BASE			(WM_USER + 1)
// do not insert any messages here!
#define APPLICATION_MSG_END			(WM_USER + 99)

// Messages for DLLs and Libs
#define LIB_MSGBASE					(WM_USER + 100)
#define	WK_WINTHREAD_STOP_THREAD	(WM_USER + 100)
	// Network Dll
#define TM_NEW_CONNECTION			(WM_USER + 101)
#define TM_CONNECTION_ERROR			(WM_USER + 102)
#define TM_CONNECTIONS_CHANGE		(WM_USER + 103)
#define TM_QUIT_CONNECTION			(WM_USER + 104)
#define TM_NEW_DATA					(WM_USER + 105)
#define TM_CONNECTION_CLOSED		(WM_USER + 106)
#define TM_CONNECTION_ESTABLISHED	(WM_USER + 107)
#define TM_PING_ERROR				(WM_USER + 108)
#define TM_NEW_UDPDATA				(WM_USER + 109)
#define TM_NEW_COMMAND				(WM_USER + 110)
#define TM_ERROR_CREATING_SOCKET	(WM_USER + 111)
	// PushSource
#define PSM_TRIGGER_UNIT			(WM_USER + 120)
#define LIB_MSG_END					(WM_USER + 299)

// Messages für DataBase
#define DBC_MSGBASE				(WM_USER + 300)
#define DBC_CONFIRM_JPEG_DECODE	(WM_USER + 301)
#define DBC_CONFIRM_BITMAP_DUMP	(WM_USER + 302)
#define DBC_SEARCH_NEW_PICT		(WM_USER + 303)
#define DBC_NEW_DATABASE_INFO	(WM_USER + 304)
	// Message zum Öffnen der Datenbank Einstellungen
#define DBC_DATABASE_SETTINGS	(WM_USER + 305)
#define DBC_MSGB_END			(WM_USER + 349)

// Messages für Net App
#define WK_NET_BASE		WM_USER + 350
#define WK_NET_END		WM_USER + 399

// Messages für CoCo ISA
#define MEGRA_MSGBASE	WM_USER + 400
#define MEGRA_MSG_END	WM_USER + 449

// Messages für CoCo ISA
// Nicht verändern!! Wird im CoCoSrv.exe verwendet. Der CoCoSrv.exe ist ein 16Bit
// Programm und wird NICHT im Supermake kompiliert.
#define COCO_MSGBASE	WM_USER + 450
#define COCO_MSG_END	WM_USER + 499

// Messages für Mico
#define MICO_MSGBASE	WM_USER + 500
#define MICO_MSG_END	WM_USER + 549

// Messages für Aku
#define AKU_MSGBASE		WM_USER + 550
#define AKU_MSG_END		WM_USER + 599

// Messages für JaCob
#define JACOB_MSGBASE		WM_USER + 600
#define JACOB_OPEN_MD_DLG	WM_USER + 601
#define JACOB_MSG_END		WM_USER + 649

// Messages für Starter / Launcher
#define DVSTARTER_MSGBASE		WM_USER + 650
#define DVSTARTER_EXIT			WM_USER + 651
#define WM_SELFCHECK			WM_USER + 652
#define WM_CHECK_RTE			WM_USER + 653
#define WM_PIEZO_USE_CHANGED	WM_USER + 654
#define DVSTARTER_MSG_END		WM_USER + 749

//Messages für Clients
#define DVCLIENT_MSGBASE		WM_USER + 750
#define WM_CHECK_BACKUP_STATUS	WM_USER + 751
#define WM_REMOUNT				WM_USER + 752
#define WM_POWEROFF_PRESSED		WM_USER + 753
#define WM_RESET_PRESSED		WM_USER + 754
#define WM_PLAY_REALTIME		WM_USER + 755
#define WM_USER_LOGIN_ALARM		WM_USER + 756
#define WM_ISDN_B_CHANNELS		WM_USER + 757	
//	LOWORD(wParam): total number of used channels
//	HIWORD(wParam): ApplicationID of sending module
//	LOWORD(lParam): current changed channel according to ApplicationID
//	HIWORD(lParam): other changed channel (other application)
#define WM_RESET_AUTO_LOGOUT	WM_USER + 758 // resets auto logout eg. by user action

#define DVCLIENT_MSG_END		WM_USER + 849

// Common Messages
#define COMMON_MSGBASE			WM_USER + 850
#define WM_TRAYCLICKED			WM_USER + 851
#define WM_TRACE_TEXTA			WM_USER + 852	// (WPARAM)(char*)ModuleName, (LPARAM)(char*)MessageText
												// const static text        , text allocated with malloc/strdup...
#define WM_TRACE_TEXTW			WM_USER + 853	// (WPARAM)(wchar_t*)ModuleName, (LPARAM)(wchar_t*)MessageText
												// const static text         , text allocated with malloc/strdup...
#ifdef _UNICODE
 #define WM_TRACE_TEXT WM_TRACE_TEXTW
#else
 #define WM_TRACE_TEXT WM_TRACE_TEXTA
#endif

#define WM_LANGUAGE_CHANGED		WM_USER + 854	// (WPARAM) (UINT)uCodePage, (LPARAM)(DWORD)dwCodePageBits
#define WM_KEYBOARD_CHANGED		WM_USER + 855

#define COMMON_MSG_END			WM_USER + 899

// Messages für SaVic
#define SAVIC_MSGBASE			WM_USER + 900
#define SAVIC_OPEN_MD_DLG		WM_USER + 901
#define SAVIC_MSG_END			WM_USER + 949

// Messages für SecurityLauncher
#define LAUNCHER_MSGBASE		WM_USER + 1000
#define LAUNCHER_EXIT			WM_USER + 1000
#define LAUNCHER_APPLICATION	WM_USER + 1001
#define LAUNCHER_MSG_END		WM_USER + 1049

/////////////////////////////////////////////////////////////////////////////
#endif // __WK_MSG_H__
