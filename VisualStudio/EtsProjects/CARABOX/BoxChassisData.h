#if !defined __BOX_CHASSIS_DATA_PAGE_H_INCLUDED_
#define __BOX_CHASSIS_DATA_PAGE_H_INCLUDED_

#include "CEtsPropertyPage.h"
#include "BoxStructures.h"
#include <commctrl.h>

class CBoxChassisDataPage: public CEtsPropertyPage
{
public:
   CBoxChassisDataPage();

protected:
   virtual bool OnInitDialog(HWND hWnd);
   virtual int  OnCommand(WORD, WORD, HWND);
   virtual int  OnMessage(UINT, WPARAM, LPARAM);

   virtual int  OnHelp(PSHNOTIFY*);
   virtual int  OnReset(PSHNOTIFY*);
   virtual int  OnSetActive(PSHNOTIFY*, bool);
   virtual int  OnWizardBack(PSHNOTIFY*, bool);
   virtual int  OnWizardFinish(PSHNOTIFY*, bool);
   virtual int  OnWizardNext(PSHNOTIFY*, bool);

   int   OnSelChangeChassisList(WORD, HWND);
	int   OnSetFocusChassisList(WORD, HWND);
	int   OnKillFocusChassisCount(WORD, HWND);
	void  CheckCurrentChassisPosition();
	void  CheckWizardButtons();
	void  SetMembraneTypeFromIndex(ChassisData*, int);

	LRESULT OnDrawItem(DRAWITEMSTRUCT *);

	void  InitChassisData();
   void  CheckChassisForm();
	void  CheckChassisType();
	void  CheckCrossOverType();
	void  CountAllChassis();
	void  UpdateChassisList();
   static BOOL CALLBACK SetHelpIDs(HWND, LPARAM);

private:

   bool   m_bListChanged;
   ChassisData *m_pCurrent;

	static int CountChassisTypes(void *, WPARAM, LPARAM);
	static int CheckChassisData(void *, WPARAM, LPARAM);
   const static long gm_nChassisCountEdt[CARABOX_CHASSISTYPES][3];
};

#endif //__BOX_CHASSIS_DATA_PAGE_H_INCLUDED_