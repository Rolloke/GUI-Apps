#if !defined __BOX_VIEW_DLG_H_INCLUDED_
#define __BOX_VIEW_DLG_H_INCLUDED_

#define  STRICT
#include <WINDOWS.H>

#include "CEtsFileDlg.h"

class CCaraSdbDlg;

class CSdbFileDlg: public CEtsFileDlg
{
public:
	int m_nPreSel;
   CSdbFileDlg();
   CSdbFileDlg(HINSTANCE, int, HWND);
   ~CSdbFileDlg();
   void SetCaraSdbDlg(CCaraSdbDlg*p){m_pCaraSdbDlg=p;}

	bool m_bDeleteZipFiles;
protected:
   virtual bool OnInitDialog(HWND hWnd);
   virtual UINT OnNotify(OFNOTIFY*);

private:
   void Constructor();

   CCaraSdbDlg *m_pCaraSdbDlg;
};

#endif //__BOX_VIEW_DLG_H_INCLUDED_