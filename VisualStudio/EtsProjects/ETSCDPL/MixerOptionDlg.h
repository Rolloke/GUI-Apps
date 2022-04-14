#if !defined __MIXER_OPTION_DLG_H_INCLUDED_
#define __MIXER_OPTION_DLG_H_INCLUDED_

#define  STRICT
#include <WINDOWS.H>
#include "CEtsDlg.h"

class CMixerOptionDlg: public CEtsDialog
{

public:
   CMixerOptionDlg();
   CMixerOptionDlg(HINSTANCE, int, HWND);
   ~CMixerOptionDlg();


private:
   virtual bool   OnInitDialog(HWND hWnd);
   virtual int    OnOk(WORD);
   virtual void   OnEndDialog(int);
   virtual int    OnCommand(WORD, WORD, HWND);
   virtual int    OnMessage(UINT, WPARAM, LPARAM);               // verarbeitet Nachrichten an das Dialogfenster

   void  Constructor();

private:

public:
	void EnumMixerKeys(char*);
};

#endif //__MIXER_OPTION_DLG_H_INCLUDED_