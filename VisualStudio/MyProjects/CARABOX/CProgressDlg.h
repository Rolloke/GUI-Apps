#if !defined __PROGRESS_DLG_H_INCLUDED_
#define __PROGRESS_DLG_H_INCLUDED_

#define  STRICT
#include <WINDOWS.H>

#include "CEtsDlg.h"
#include "BoxStructures.h"

class CProgressDlg: public CEtsDialog
{
public:
   CProgressDlg();
   CProgressDlg(HINSTANCE, int, HWND);
   void SetPropertySheet(CBoxPropertySheet*p){m_pPropertySheet=p;}

private:
   void Constructor();
   virtual bool   OnInitDialog(HWND hWnd);
   virtual int    OnOk(WORD);
   virtual void   OnEndDialog(int);
   virtual int    OnCommand(WORD, WORD, HWND);

   CBoxPropertySheet *m_pPropertySheet;
};

#endif //__PROGRESS_DLG_H_INCLUDED_