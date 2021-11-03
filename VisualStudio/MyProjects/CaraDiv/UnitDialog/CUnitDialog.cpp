/*******************************************************************************
                                 CUnitDialog.h
                                 -------------

                         Dialogbox zum Einheitenumwandeln


                   (c) 2001 ELAC Technische Software GmbH Kiel

                          Version 1.0 Stand:  10.05.2001


inheritated from: CModalDialog
                                                    programmed by Oliver Wesnigk
*******************************************************************************/

#include "CUnitDialog.h"
#include "resource.h"
#include <stdio.h>
#include <multimon.h>                            // wie ein Popup Fenster aus diesen Monitor beschränken
#include "CWin2k.h"


bool CUnitDialog::gm_bLocked = false;            // noch ist keine Instance von diesem Dialog vorhanden


CUnitDialog::CUnitDialog(CWindow * pWindow,bool bStd,int bef):CModalDialog(IDD_UNITCONVERTDIALOG,pWindow,bef)
{
   m_cm             = 0;
   m_ptPos.x        = 1;
   m_ptPos.y        = 1;
   m_hFett          = NULL;
   m_bEndWithReturn = true;
   
   gm_bLocked       = true;                      // weitere Instancen sperren
}

CUnitDialog::CUnitDialog(CModalDialog * pDialog,bool bStd,int bef):CModalDialog(IDD_UNITCONVERTDIALOG,pDialog,bef)
{
   m_cm             = 0;
   m_ptPos.x        = 1;
   m_ptPos.y        = 1;
   m_hFett          = NULL;
   m_bEndWithReturn = true;

   gm_bLocked       = true;                      // weitere Instancen sperren
}

CUnitDialog::~CUnitDialog()
{
   if(m_hFett) DeleteObject(m_hFett);

   gm_bLocked       = false;                     // weitere Instancen zulassen
}



#define WS_EX_LAYERED           0x00080000       // Für Win2k Transparent (LayerWindows)
#define LWA_ALPHA               0x00000002

#define SPI_GETMENUANIMATION    0x1002           // Win98 und Win2k und höher
#define SPI_GETMENUFADE         0x1012           // Win2k und höher

bool CUnitDialog::OnInitDialog()
{
   static const LOGFONT lfa={-12,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,OUT_DEVICE_PRECIS,CLIP_DEFAULT_PRECIS,PROOF_QUALITY,FF_MODERN,"Arial"};

   m_hFett  = ::CreateFontIndirect(&lfa);

   if(m_hFett)
   {
      SendDlgItemMessage(IDC_UNIT_S_CM,WM_SETFONT,(WPARAM) m_hFett,0);
   }

   SetFocus(GetDlgItem(IDC_UNIT_E_INCH));

   Update();
                                                 // Position auf dem Monitor bestimmen
   int  width;
   int  height;
   int  maxwidth;
   int  maxheight;
   int  minx;
   int  miny;

   {                                             // get Workarea mit alter API !
      RECT rcWork;                               // gilt nur für den ersten Monitor

      SystemParametersInfo(SPI_GETWORKAREA,0,&rcWork,0);

      maxwidth  = rcWork.right;
      maxheight = rcWork.bottom;
      minx      = rcWork.left;
      miny      = rcWork.top;
   }

   {                                             // Dialogboxgrösse besorgen
      RECT rc;
      GetWindowRect(GetWnd(),&rc);
      width  = rc.right - rc.left;
      height = rc.bottom - rc.top;
   }

   {                                             // Monitorinformationen finden
      HMONITOR    hMonitor = MonitorFromPoint(m_ptPos,MONITOR_DEFAULTTONEAREST);

      if(hMonitor)                               // der Unterschied hir zu ETSHELP ist, das hier die Workarea verwendet werden muß
      {                                          // da das Dialogfeld nicht TOPMOST ist !
         MONITORINFO sMonInfo;

         sMonInfo.cbSize = sizeof(MONITORINFO);

         if(GetMonitorInfo(hMonitor,&sMonInfo))
         {
            maxwidth  = sMonInfo.rcWork.right;
            maxheight = sMonInfo.rcWork.bottom;
            minx      = sMonInfo.rcWork.left;
            miny      = sMonInfo.rcWork.top;
         }
      }
   }
                                                 // left,top clipping
   if(m_ptPos.x <= minx) m_ptPos.x = minx + 1;
   if(m_ptPos.y <= miny) m_ptPos.y = miny + 1; 
                                                 // right, bottom clipping
   if(m_ptPos.x+width >= maxwidth)
   {
      m_ptPos.x = maxwidth - width -1;
   }
   if(m_ptPos.y+height >= maxheight)
   {
      m_ptPos.y = maxheight - height - 1;
   }

   MoveWindow(GetWnd(),m_ptPos.x,m_ptPos.y,width,height,false);

   SendDlgItemMessage(IDC_UNIT_E_INCH,EM_SETSEL,0,-1); // alles im Editcontroll selectieren

   SendDlgItemMessage(IDC_UNIT_E_CM  ,EM_LIMITTEXT,(WPARAM) 5,(LPARAM) 0);
   SendDlgItemMessage(IDC_UNIT_E_INCH,EM_LIMITTEXT,(WPARAM) 7,(LPARAM) 0);
   SendDlgItemMessage(IDC_UNIT_E_FEET,EM_LIMITTEXT,(WPARAM) 6,(LPARAM) 0);
   SendDlgItemMessage(IDC_UNIT_E_YARD,EM_LIMITTEXT,(WPARAM) 6,(LPARAM) 0);


   if(CWin2k::IsSetLayeredWindowAttributes())    // Ist Win2k
   {
      BOOL on;
                                                 // Menü Animationen erlaubt ?
      if(SystemParametersInfo(SPI_GETMENUANIMATION,NULL,&on,0))
      {
         if(on)
         {                                       // Fade eingeschaltet ?
            if(SystemParametersInfo(SPI_GETMENUFADE,NULL,&on,0))
            {
               if(on)                            // Ja, fade in durchführen
               {
                  SetWindowLong(GetWnd(),GWL_EXSTYLE,GetWindowLong(GetWnd(),GWL_EXSTYLE)|WS_EX_LAYERED);
                  CWin2k::SetLayeredWindowAttributes(GetWnd(),0,(255*10)/100,LWA_ALPHA);
                  m_nAlpha = 10;
                  SetTimer(GetWnd(),0x11223344,40,NULL);
               }
            }
         }
      }
   }

   return false;
}

int CUnitDialog::OnMessage(int nMessage,WPARAM wParam,LPARAM lParam)
{
   if(nMessage == WM_TIMER)
   {
      if(m_nAlpha < 100)
      {
         m_nAlpha += 10;
         CWin2k::SetLayeredWindowAttributes(GetWnd(),0,(255*m_nAlpha)/100,LWA_ALPHA);
      }
      else
      {
         SetWindowLong(GetWnd(),GWL_EXSTYLE,GetWindowLong(GetWnd(),GWL_EXSTYLE)&~WS_EX_LAYERED);
         KillTimer(GetWnd(),0x11223344);
      }
      return TRUE;
   }
   return FALSE;
}

void CUnitDialog::Update()
{
   char szOut[128];

   SetDlgItemText(IDC_UNIT_E_CM,m_cm);

   float inch = ((float)m_cm) / 2.54f;
   float feet = ((float)m_cm) / 30.48f;
   float yard = feet / 3.0f;

   sprintf(szOut,"%4.2f",inch);
   Convert(szOut);
   SetDlgItemText(IDC_UNIT_E_INCH,szOut);

   sprintf(szOut,"%4.2f",feet);
   Convert(szOut);
   SetDlgItemText(IDC_UNIT_E_FEET,szOut);

   sprintf(szOut,"%4.2f",yard);
   Convert(szOut);
   SetDlgItemText(IDC_UNIT_E_YARD,szOut);
}


bool CUnitDialog::OnCommand(int flags,int id,LPARAM lParam)
{
   if(id==IDOK) return true;

   if(flags==EN_KILLFOCUS) 
   {
      char   szIn[128];
      char   szDecimal[4];
      double z;

      ::GetDlgItemText(GetWnd(),id,szIn,128);

      GetLocaleInfo(LOCALE_USER_DEFAULT,LOCALE_SDECIMAL, szDecimal, 4);

      if(szDecimal[0] != 46)
      {
         for(int i=0;i<lstrlen(szIn);i++)
         {
            if(szIn[i] == szDecimal[0]) {szIn[i] = 46;break;}
         }
      }

      z = atof(szIn);
      if(z < 0) z = 0;

      switch(id)
      {
         case IDC_UNIT_E_INCH:
              z *= 2.54;
              break;
         case IDC_UNIT_E_FEET:
              z *= 30.48;
              break;
         case IDC_UNIT_E_YARD:
              z *= 3 * 30.48;
              break;
         case IDC_UNIT_E_CM:
              if(z > 15000) z = 15000;
              break;
         default: return false;
      }

      m_cm = Rounding(z);
      if(m_cm > 15000) m_cm = 15000;

      Update();
      return true;
   }
   return false;
}


void CUnitDialog::Params(int& cm,bool bDirection)
{
   if(bDirection==SETPARAMS)
   {
      m_cm = cm;
   }
   else
   {
      cm = m_cm;
   }
}



void CUnitDialog::Convert(char * szZahl)
{
   char szDecimal[4];

   GetLocaleInfo(LOCALE_USER_DEFAULT,LOCALE_SDECIMAL,szDecimal, 4);

   if(szDecimal[0] == 46) return;                // keine Anpassung nötig

   for(int i=0;i<lstrlen(szZahl);i++)
   {
      if(szZahl[i] == 46)
      {
         szZahl[i] = szDecimal[0];
         break;
      }
   }
}
