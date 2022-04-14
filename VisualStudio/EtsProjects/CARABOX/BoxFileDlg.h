#if !defined __BOX_VIEW_DLG_H_INCLUDED_
#define __BOX_VIEW_DLG_H_INCLUDED_

#define  STRICT
#include <WINDOWS.H>

#include "CEtsFileDlg.h"

class CBoxPropertySheet;

class CBoxFileDlg: public CEtsFileDlg
{
public:
   CBoxFileDlg();
   CBoxFileDlg(HINSTANCE, int, HWND);
   ~CBoxFileDlg();
   void SetPropertySheet(CBoxPropertySheet*p){m_pPropertySheet=p;}
   bool m_bOpen;

protected:
   virtual bool OnInitDialog(HWND hWnd);
   virtual UINT OnNotify(OFNOTIFY*);
   virtual int  OnCommand(WORD, WORD, HWND);

private:
	void ShowBoxFile(char*);
   void Constructor();

   CBoxPropertySheet *m_pPropertySheet;
};

#endif //__BOX_VIEW_DLG_H_INCLUDED_