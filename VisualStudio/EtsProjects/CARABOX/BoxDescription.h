#if !defined __BOX_DESCRIPTION_PAGE_H_INCLUDED_
#define __BOX_DESCRIPTION_PAGE_H_INCLUDED_

#include "CEtsPropertyPage.h"

class CBoxDescriptionPage: public CEtsPropertyPage
{
public:
   CBoxDescriptionPage();
   virtual bool OnInitDialog(HWND hWnd);
   virtual int  OnCommand(WORD, WORD, HWND);

   virtual int  OnHelp(PSHNOTIFY*);
   virtual int  OnReset(PSHNOTIFY*);
   virtual int  OnSetActive(PSHNOTIFY*, bool);
   virtual int  OnWizardBack(PSHNOTIFY*, bool);
   virtual int  OnWizardFinish(PSHNOTIFY*, bool);
   virtual int  OnWizardNext(PSHNOTIFY*, bool);
protected:
	void InitDlgCtrls();
   static BOOL CALLBACK SetHelpIDs(HWND, LPARAM);
};

#endif //__BOX_DESCRIPTION_PAGE_H_INCLUDED_