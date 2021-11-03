#if !defined __FILTER_DLG_H_INCLUDED_
#define __FILTER_DLG_H_INCLUDED_

#define  STRICT
#include <WINDOWS.H>

#include "CEtsDlg.h"
#include "BoxStructures.h"

class CFilterDlg: public CEtsDialog
{
public:
   CFilterDlg();
   CFilterDlg(HINSTANCE, int, HWND);

   FilterBox m_Filter;
   double    m_dMinFrq, m_dMaxFrq;
   bool      m_bFrqChanged;

private:
   void Constructor();
   virtual bool   OnInitDialog(HWND hWnd);
   virtual int    OnOk(WORD);
   virtual int    OnCommand(WORD, WORD, HWND);
   virtual int    OnMessage(UINT, WPARAM, LPARAM);
   virtual void   OnEndDialog(int);

	LRESULT OnDrawItem(DRAWITEMSTRUCT *);
   static BOOL CALLBACK SetHelpIDs(HWND, LPARAM);

   bool     m_bHP, m_bTP;
   int      m_nTPMinOrder, m_nTPMaxOrder,
            m_nHPMinOrder, m_nHPMaxOrder;
protected:
   void CheckFilterType(int, int, int, int, double&, int&, int&, int&);
};

#endif //__FILTER_DLG_H_INCLUDED_