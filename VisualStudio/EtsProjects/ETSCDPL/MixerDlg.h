#if !defined __MIXER_DLG_H_INCLUDED_
#define __MIXER_DLG_H_INCLUDED_

#define  STRICT
#include <WINDOWS.H>
#include "CEtsDlg.h"
#include "CEtsList.h"

#define MIXER              "Mixer"
#define MIXER_DEVICE       "Device"
#define MIXER_DEVICES      "Devices"
#define MIXER_NAME         "Name"
#define MIXER_CTRL_FLAG    "Flag"
#define MIXER_INIT         "Init"
#define MIXER_HWND         "HWND"
#define CONTROL_VOLUME     "Volume"
#define CONTROL_MUTE       "Mute"
#define CONTROL_SOUND      "Sound%s"

#define IF_CONTROL_ACTIVE  0x00000001
#define VOLUME_ACTIVE      0x00000002
#define MUTE_ACTIVE        0x00000004
#define METER_ACTIVE       0x00000008
#define MUX_ACTIVE         0x00000010
#define EQU_ACTIVE         0x00000020
#define SET_VOLUME_FULL    0x00010000
#define SET_MUTED          0x00040000
#define SET_EQU_LINEAR     0x00080000

#define CONTROL_NO_UPDATE_ONCE 1
#define CONTROL_UPDATE         2
#define CONTROL_NO_UPDATE      3

#define INIT_LINES             0x0001        // Initialisiere die Audio Lines für die Wiedergabe
#define STORE_LINES            0x0004        // Sichern der Einstellungen der Audio Lines beim starten der ersten Instanz
#define RESTORE_LINES          0x0008        // Rücksichern der Audio Lines nach schließen der letzten Instanz
#define WAVE_LINES             0x0010        // alle benötigten Regler für Wave Line anzeigen
#define CD_LINES               0x0020        // alle benötigten Regler für CD Line anzeigen

#define DEST_CTRL_FLAG         0x0100        // Registry Flag: Destinations können von der Registry kontrolliert werden
#define DEST_ACTIVE            0x0200        // Registry Flag: Destination ist aktiv

struct MixerControlLine
{
   MIXERCONTROLDETAILS mxcd;
   MIXERCONTROL        mxctl;
   HWND  hwndSliderLeft;
   HWND  hwndSliderRight;
   HWND  hwndMute;
   HWND  hwndSingle;
   BOOL  bSingle;
   DWORD dwLineID,
         dwLineFlags,
         dwComponentType;
   union ControlDetails
   {
      MIXERCONTROLDETAILS_UNSIGNED unSigned[1];
      MIXERCONTROLDETAILS_BOOLEAN  boolean[1];
      MIXERCONTROLDETAILS_LISTTEXT listext[1];
   } cd;
};

class CMixerDlg: public CEtsDialog
{

public:
   CMixerDlg();
   CMixerDlg(HINSTANCE, int, HWND);
   ~CMixerDlg();

   bool   OpenMixerDev(HWND hwndParent=NULL, bool bRestore=false);
	void   SetWaveVolume(long);
   void   GetControlValues(MixerControlLine*);
	void   CloseMixerDev();
	long   GetWaveVolume();
	int    GetWaveCtrlID();
	HMIXER GetWaveMixer();
   bool   IsOpened() {return (m_hMixer != NULL) ? true : false;};
	DWORD  ModifyInitFlag(DWORD dwSet=0, DWORD dwRemove=0);

private:
   virtual bool   OnInitDialog(HWND hWnd);
   virtual int    OnOk(WORD);
   virtual void   OnEndDialog(int);
   virtual int    OnCommand(WORD, WORD, HWND);
   virtual int    OnMessage(UINT, WPARAM, LPARAM);               // verarbeitet Nachrichten an das Dialogfenster

   void  Constructor();
	void  ReportMMError(MMRESULT, bool);
	void  VolumeControl(HWND, bool);
	void* CreateMCL(MIXERCONTROL&, DWORD);
   BOOL  MixAppGetComponentName(LPMIXERLINE, LPTSTR);

	static void DeleteMCL(void*);
	static int  FindControlID(const void*, const void*);

private:
	static int SortWnd(const void*, const void*);
	static BOOL CALLBACK EnumSliderSysColorChange(HWND, LPARAM);
	HMIXER   m_hMixer;
	HMIXER   m_hWaveMixer;
   char     m_szWindowText[64];
   CEtsList m_Controls;
   MixerControlLine *m_pWaveCtrl;
   int      m_nUpdateControl;

public:
	void CountMixerCtrInst(HWND, bool);
   UINT     m_nMixerDev;
   UINT     m_nForWhat;
   DWORD    m_dwInitLines;
};

#endif //__MIXER_DLG_H_INCLUDED_