#if !defined __BOX_PROPERTIES_DLG_H_INCLUDED_
#define __BOX_PROPERTIES_DLG_H_INCLUDED_

#include "CEtsDlg.h"
#include "BoxDscr.h"
#include "CEtsList.h"


class CBoxObj;
class CVector;
class CMaterial;
class CCaraWalkDll;

class CBoxPropertiesDlg: public CEtsDialog
{
public:
	void GetMaterialText(CMaterial*, char*);
	void InitComboBox();
	void InitMaterialParam(CMaterial *);
   CBoxPropertiesDlg();
   CBoxPropertiesDlg(HINSTANCE, int, HWND);
   void Constructor();
   ~CBoxPropertiesDlg();
   virtual bool OnInitDialog(HWND hWnd);
   virtual int  OnCancel();
   virtual int  OnOk(WORD);
   virtual int  OnCommand(WORD, WORD, HWND);
   virtual int  OnMessage(UINT, WPARAM, LPARAM);
   virtual void OnEndDialog(int);                              // Beendet den Dialog

   void         SetpBox(CBoxObj *);
   void         UpdateNewData();

   CCaraWalkDll *m_pCaraWalk;

private:
   int        m_nCurSel;
   int        m_nPolygon;
   int        m_nMaterial;
   CEtsList   m_Boxes;
   CBoxObj   *m_pSelBox;
   CMaterial *m_pMaterial;

};

#endif __BOX_PROPERTIES_DLG_H_INCLUDED_
