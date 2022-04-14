#if !defined __BOX_BASE_DATA_PAGE_H_INCLUDED_
#define __BOX_BASE_DATA_PAGE_H_INCLUDED_

#include "CEtsPropertyPage.h"

class CBoxBaseDataPage: public CEtsPropertyPage
{
public:
   CBoxBaseDataPage();

protected:
   virtual bool OnInitDialog(HWND hWnd);
   virtual int  OnCommand(WORD, WORD, HWND);

   virtual int  OnHelp(PSHNOTIFY*);
   virtual int  OnQueryCancel(PSHNOTIFY*, bool);
   virtual int  OnReset(PSHNOTIFY*);
   virtual int  OnSetActive(PSHNOTIFY*, bool);
   virtual int  OnWizardBack(PSHNOTIFY*, bool);
   virtual int  OnWizardFinish(PSHNOTIFY*, bool);
   virtual int  OnWizardNext(PSHNOTIFY*, bool);

   void InitDlgCtrls();
	void CheckBRTLMinMax(int);
	void CheckBoxType();
	void SetBassImpedance();
	void CheckWizardButtons();
   static BOOL CALLBACK SetHelpIDs(HWND, LPARAM);
};

#endif //__BOX_BASE_DATA_PAGE_H_INCLUDED_
