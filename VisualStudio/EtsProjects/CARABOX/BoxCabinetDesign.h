#if !defined __BOX_CABINET_DESIGN_PAGE_H_INCLUDED_
#define __BOX_CABINET_DESIGN_PAGE_H_INCLUDED_

#include "CEtsPropertyPage.h"
#include "BoxStructures.h"
#include "CCabinet.h"

class CBoxCabinetDesignPage: public CEtsPropertyPage
{
public:
   CBoxCabinetDesignPage();
   virtual ~CBoxCabinetDesignPage();

protected:
   virtual bool OnInitDialog(HWND hWnd);
   virtual int  OnCommand(WORD, WORD, HWND);
   virtual int  OnMessage(UINT, WPARAM, LPARAM);

   virtual int  OnHelp(PSHNOTIFY*);
   virtual int  OnReset(PSHNOTIFY*);
   virtual int  OnKillActive(PSHNOTIFY*, bool);
   virtual int  OnSetActive(PSHNOTIFY*, bool);
   virtual int  OnWizardBack(PSHNOTIFY*, bool);
   virtual int  OnWizardFinish(PSHNOTIFY*, bool);
   virtual int  OnWizardNext(PSHNOTIFY*, bool);

	int   OnKillFocusCabinetSizes(WORD, HWND);
   int   OnSelChangeChassisList(WORD, HWND);
	bool  OnSelChangeWall(WORD nID, HWND hwndControl);
	bool  OnSelChangeCabinet(WORD nID, HWND hwndControl);

   bool  CheckCurrentChassisPosition(bool bInit=false);
   bool  IsChassisOnWallSegments(ChassisData*);
	bool  AddCabinetsToChassis(ChassisData*, int);

	void  SetVolumeString();
	void  CheckWizardButtons();
	void  InitCabinetSizes();
	void  SetCurrentWall(int nNewCabWall=0);

	LRESULT OnDrawItem(DRAWITEMSTRUCT *);

   void  InitCabinetData();
   void  InitChassisData(bool bSetWall=true);
	void  InitChassisPos();
	void  InitChassisPosList();
	void  OnNewCabinet(HWND);
	bool  SetChassisLstSel(int nWall);
	void  Invalidate3D(int);
	bool  SetCurrentCabinet(int, bool bResetCurrent=true);
	void  UpdateCabinetList();

   void OnPosViewDraw(HDC, PAINTSTRUCT*);
   void OnPosViewPrepareDC(HDC);
	void OnPosViewMouseMove(DWORD, int, int);
   void OnPosViewLBtnDown(DWORD, int, int);
   void OnPosViewLBtnUp(DWORD, int, int);
   LRESULT OnPosViewKeyDown(WPARAM, LPARAM);
	void MoveActualChassis(int, int, bool bTransform=true);

 	static int DrawChassisPositions( void *, WPARAM, LPARAM);
 	static int CheckChassisPositions(void *, WPARAM, LPARAM);
   static int HitChassisPosition(   void *, WPARAM, LPARAM);
   static LRESULT CALLBACK PositionViewSubClassProc(HWND, UINT, WPARAM, LPARAM);
   static BOOL CALLBACK SetHelpIDs(HWND, LPARAM);

private:

   ChassisData *m_pCurrent;

   bool         m_bViewFocus;

   CVector2D    m_vrcWall[NO_OF_RECT_POINTS];
   POINT       *m_pptSegments;
   CVector2D   *m_pvSegments;
   int          m_nSizeOfSegmentArray;
   RECT         m_rcView;
   int          m_nWall;
   HWND         m_hWndPosView;
   POINT        m_ptOld;

   static LONG  gm_lOldPositionViewSubClassProc;
   static LONG  gm_lOldStaticCtrlSCProc;
   const static short gm_nCabinetSizes[CARABOX_CABSIZES][3];
};

#endif //__BOX_CABINET_DESIGN_PAGE_H_INCLUDED_