#if !defined __BOX_POSITION_DLG_H_INCLUDED_
#define __BOX_POSITION_DLG_H_INCLUDED_

#include "CEtsDlg.h"
#include "BoxDscr.h"
#include "CEtsList.h"

class CBoxObj;
class CVector;
class CBoxPositionDlg: public CEtsDialog
{
public:
   CBoxPositionDlg();
   CBoxPositionDlg(HINSTANCE, int, HWND);
   void Constructor();
   ~CBoxPositionDlg();
   virtual bool OnInitDialog(HWND hWnd);
   virtual int  OnCancel();
   virtual int  OnOk(WORD);
   virtual int  OnCommand(WORD, WORD, HWND);
   virtual int  OnMessage(UINT, WPARAM, LPARAM);
   void         SetpBox(CBoxObj *);
   void         UpdateNewData();
   void         UpdateParams();
   bool         CheckBoxPosition(bool);
   void         CatchBox(CVector *, CVector *);
private:
   CARAWALK_BoxPos       m_Bp;
   int                   m_nCurSel;
   int                   m_nRotate;
   int                   m_nNoOfBoxes;
   int                   m_nNoOfBoxPositions;
   CEtsList              m_Boxes;
   CEtsList              m_BoxPos;
   int                  *m_pnBox;
   int                  *m_pnBoxPos;
};

#endif __BOX_POSITION_DLG_H_INCLUDED_
