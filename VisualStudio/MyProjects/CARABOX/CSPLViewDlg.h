#if !defined __FILTER_DLG_H_INCLUDED_
#define __FILTER_DLG_H_INCLUDED_

#define  STRICT
#include <WINDOWS.H>

#include "CEtsDlg.h"
#include "BoxStructures.h"

#define SPL_VIEW_2D_FRQRESPONSE  0
#define SPL_VIEW_3D_POLAR_MAGN   1
#define SPL_VIEW_3D_POLAR_PHASE  2
#define SPL_VIEW_2D_POLAR        3

class CSPLViewDlg: public CEtsDialog
{
public:
   CSPLViewDlg();
   CSPLViewDlg(HINSTANCE, int, HWND);

   int    m_nWhat;
   double m_dDistance;
   int    m_nDistance;

private:
   void Constructor();
   virtual bool   OnInitDialog(HWND hWnd);
   virtual int    OnOk(WORD);
   virtual int    OnCommand(WORD, WORD, HWND);

protected:
};

#endif //__FILTER_DLG_H_INCLUDED_