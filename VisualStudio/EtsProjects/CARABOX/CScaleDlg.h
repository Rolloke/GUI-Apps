#if !defined __PROGRESS_DLG_H_INCLUDED_
#define __PROGRESS_DLG_H_INCLUDED_

#define  STRICT
#include <WINDOWS.H>

#include "CEtsDlg.h"

#define SCALE_HEIGHT      0
#define SCALE_WIDTH       1
#define SCALE_DEPTH       2
#define SCALE_ANGLE_PHI   3
#define SCALE_ANGLE_THETA 4

class CScaleDlg: public CEtsDialog
{
public:
   CScaleDlg();
   CScaleDlg(HINSTANCE, int, HWND);

   int m_nScaleMode;
   int m_nWhat;
   int m_nScale;
   char m_szText[256];

private:
   void Constructor();
   virtual bool   OnInitDialog(HWND hWnd);
   virtual int    OnOk(WORD);
   virtual void   OnEndDialog(int);
   virtual int    OnCommand(WORD, WORD, HWND);

   int   m_nMode1;
   int   m_nMode2;
   bool  m_bShow;
   bool  m_bInvDir;
};

#endif //__PROGRESS_DLG_H_INCLUDED_