#if !defined __CLIGHT_DLG_H_INCLUDED_
#define __CLIGHT_DLG_H_INCLUDED_

#include "CEtsDlg.h"
#include "Ets3DGL.h"

class CLightdlg: public CEtsDialog
{
public:
   CLightdlg();
   CLightdlg(HINSTANCE, int, HWND);
   void Constructor();
   virtual bool OnInitDialog(HWND hWnd);
   virtual int  OnCancel();
   virtual int  OnOk(WORD);
   virtual int  OnCommand(WORD, WORD, HWND);
   virtual int  OnMessage(UINT, WPARAM, LPARAM);
   void    SetLightParam(CGlLight *pLight) {m_pLp = pLight;};
   void    SetAmbientColor(COLORREF color);
   void    UpdateParams();
   int     OnLightColor();
   int     OnLightDlgSpot();
private:
   CGlLight   m_Lp;
   CGlLight  *m_pLp;
   int        m_nCurSel;
   int        m_nLightIntens;
   COLORREF   m_LightColor;
   float      m_pfAmbient[3];
};

#endif // __CLIGHT_DLG_H_INCLUDED_
