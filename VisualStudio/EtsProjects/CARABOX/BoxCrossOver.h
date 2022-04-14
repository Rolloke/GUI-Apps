#if !defined __BOX_CROSS_OVER_PAGE_H_INCLUDED_
#define __BOX_CROSS_OVER_PAGE_H_INCLUDED_

#include "CEtsPropertyPage.h"

#define CUT_OFF_FREQUENCIES 4

struct FilterBox;

class CBoxCrossOverPage: public CEtsPropertyPage
{
public:
   CBoxCrossOverPage();

protected:
	void SetControlValues();
   virtual bool OnInitDialog(HWND hWnd);
   virtual int  OnCommand(WORD, WORD, HWND);
   virtual int  OnMessage(UINT, WPARAM, LPARAM);

   virtual int  OnHelp(PSHNOTIFY*);
   virtual int  OnReset(PSHNOTIFY*);
   virtual int  OnQueryCancel(PSHNOTIFY*, bool);
   virtual int  OnSetActive(PSHNOTIFY*, bool);
   virtual int  OnWizardBack(PSHNOTIFY*, bool);
   virtual int  OnWizardFinish(PSHNOTIFY*, bool);
   virtual int  OnWizardNext(PSHNOTIFY*, bool);

	LRESULT OnMeasureItem(MEASUREITEMSTRUCT*);
	LRESULT OnDrawItem(DRAWITEMSTRUCT *);

	bool  GetCutOffFrequencies(int, int, int);
	void  CheckCrossOverType();
	bool  CallFilterDialog(FilterBox&);
	void  InitTT2Filter();
	bool  CheckFrequency(int, double);
	void  CheckFilterButtons();
	double*GetPtr1WGFrequ(int);
	void  CheckWizardButtons();
	void  GetControlFrequencies();
	void  InitStandardFrequencies();
	void  InitFilter(DWORD dwInit=0x000000ff);

private:

   static BOOL CALLBACK SetHelpIDs(HWND, LPARAM);

   double m_pdFrequencies[CUT_OFF_FREQUENCIES];
   bool   m_bSymbolicBtn;
   const static double gm_d2WayFrq;
   const static double gm_d3WayFrq[2];
   const static double gm_d4WayFrq[3];
   const static double gm_d5WayFrq[4];
   const static double gm_d2_3WayFrq;
   const static double gm_d3_4WayFrq;

   const static int    gm_nEditFloat[];
   const static int    gm_nGrp[];
   const static int    gm_nRadio[];
};

#endif //__BOX_CROSS_OVER_PAGE_H_INCLUDED_