/*******************************************************************************
                                 CEtsDialog
  Zweck:
   Basisklasse zur Erzeugung und Steuerung von Dialogfeldern
  Header:            Bibliotheken:        DLL´s:
                                          ETSHELP.dll
  Copyright:
   (C) 1999 ELAC Technische Software GmbH
  VERSION 
   1.00 Stand 10.09.1999
                                                   programmed by Rolf Ehlers
*******************************************************************************/

#if !defined __CETS_PROPERTY_PAGE_H_INCLUDED_
#define __CETS_PROPERTY_PAGE_H_INCLUDED_

#include "CEtsDlg.h"

class CEtsPropertyPage : public CEtsDialog
{
public:
   friend class CEtsPropertySheet;
   CEtsPropertyPage();
   virtual ~CEtsPropertyPage();
protected:
   // EtsDialogMessages
   virtual bool OnInitDialog(HWND hWnd);                       // zur initialisierung der Dialogfelder
   virtual int  OnMessage(UINT, WPARAM, LPARAM);               // verarbeitet Nachrichten an das Dialogfenster
   virtual void OnEndDialog(int);                              // Beendet den Dialog
   // EtsPropertyPageMessages
   virtual int  OnApply(PSHNOTIFY*, bool);
   virtual int  OnGetObject(NMHDR*); 
   virtual int  OnHelp(PSHNOTIFY*);
   virtual int  OnKillActive(PSHNOTIFY*, bool);
   virtual int  OnQueryCancel(PSHNOTIFY*, bool);
   virtual int  OnReset(PSHNOTIFY*);
   virtual int  OnSetActive(PSHNOTIFY*, bool);
   virtual int  OnWizardBack(PSHNOTIFY*, bool);
   virtual int  OnWizardFinish(PSHNOTIFY*, bool);
   virtual int  OnWizardNext(PSHNOTIFY*, bool);

   bool SwitchDlgItem(int nIDFocusTest, HWND hWndNext, bool bNext);
   void SetParent(CEtsPropertySheet *);

   static UINT CALLBACK PropSheetPageProc(HWND, UINT, LPPROPSHEETPAGE);

   CEtsPropertySheet *m_pParent;
public:
	bool HasPageButtonFocus(PSHNOTIFY*);
	void SetPageButtonFocus(PSHNOTIFY*);
   PROPSHEETPAGE     *m_pPropSheetPage;
};


#endif //__CETS_PROPERTY_PAGE_H_INCLUDED_