/**************************************+****************************************

                                CModalDialog.cpp
                                ----------------

                 OFC-Basisklasse für alle modalen Dialogfelder 

                 (C) 1998-2001 ELAC Technische Software GmbH

                         VERSION 1.4 Stand 10.05.2001


inheritated from : nothing

                                                   programmed by Oliver Wesnigk
*******************************************************************************/

#include "CModalDialog.h"
#include "CToolBar.h"
#include "CUnitDialog.h"
#include "resource.h"

#include <CEtsHelp.h>                            // Hilfe- und Fehlerfenster



/*************************************+*****************************************
                                  Constructor

public Function: anlegen einer Instance ,Besitzer ist ein CWindow
*******************************************************************************/

CModalDialog::CModalDialog(int ID,CWindow * pOwner,int bef)
{
   m_pOwner         = pOwner;
   m_pDialog        = NULL;
   m_nID            = ID;
   m_bEndWithReturn = false;
   m_hIcon          = NULL;

   if(bef != NULL)
   {
      m_hIcon       = CToolBar::GetIcon(bef);
   }
}


/**************************************+****************************************
                                  Constructor

public Function: anlegen einer Instance ,Besitzer ist ein CModalDialog
*******************************************************************************/

CModalDialog::CModalDialog(int ID,CModalDialog * pOwner,int bef)
{
   m_pOwner         = NULL;
   m_pDialog        = pOwner;
   m_nID            = ID;
   m_bEndWithReturn = false;
   m_hIcon          = NULL;

   if(bef != NULL)
   {
      m_hIcon       = CToolBar::GetIcon(bef);
   }
}


/**************************************+****************************************
                                   Destructor

public Function: zerstören einer Instance
*******************************************************************************/

CModalDialog::~CModalDialog()
{
  m_pOwner  = NULL;
  m_pDialog = NULL;
  if(m_hIcon) DestroyIcon(m_hIcon);
}


/*************************************+*****************************************
                                 GetInstance

protected Function: Instancehandle der Anwendung besorgen
*******************************************************************************/

HINSTANCE CModalDialog::GetInstance() const
{
   if(m_pOwner !=NULL) return m_pOwner->GetInstance();
   if(m_pDialog!=NULL) return m_pDialog->GetInstance();
   return 0;
}


/*************************************+*****************************************
                                 GetOwnerWnd

protected Function: Besitzerfensterhandle besorgen
*******************************************************************************/

HWND CModalDialog::GetOwnerWnd() const
{
   if(m_pOwner !=NULL) return m_pOwner->GetWnd();
   if(m_pDialog!=NULL) return m_pDialog->GetWnd();
   return NULL;
}


/**************************************+****************************************
                                    DoModal

public Function: Dialogfeld anzeigen und modal ausführen
*******************************************************************************/

bool CModalDialog::DoModal()
{
   int ret;                                      // Rückgabewert des Dialogfeldes
                                                 // gibt es einen Besitzer ?
   if((m_pOwner==NULL)&&(m_pDialog==NULL)) return false;

                                                 // Da eine Dialogbox den Hintergrund in einer Bitmap speichert
                                                 // sollten bei diese Anwendung vorher die Mousestriche in den 
                                                 // Linealen nicht mehr angezeigt sein !
   ::SendMessage(GetOwnerWnd(),WM_MOUSELEAVE,0,0);

                                                 // es gibt jetzt zwar 2 * die WM_MOUSELEAVE Nachricht, ist aber egal !
   ret = DialogBoxParam(GetInstance(),           // Dialogfeld anzeigen und modal ausführen
                        MAKEINTRESOURCE(m_nID),
                        GetOwnerWnd(),
                        (DLGPROC) MessageFunction,
                        (LPARAM) this);

   if(ret==1) return true;
   else       return false;
}



bool CModalDialog::DoModal(HWND hWnd)
{
   int ret;                                      // Rückgabewert des Dialogfeldes

                                                 // es gibt jetzt zwar 2 * die WM_MOUSELEAVE Nachricht, ist aber egal !
   ret = DialogBoxParam(GetInstance(),           // Dialogfeld anzeigen und modal ausführen
                        MAKEINTRESOURCE(m_nID),
                        hWnd,
                        (DLGPROC) MessageFunction,
                        (LPARAM) this);

   if(ret==1) return true;
   else       return false;
}



/**************************************+****************************************
                                MessageFunction

private Function: Nachrichtenbearbeitung wird von WINDOOF aufgerufen
*******************************************************************************/

LRESULT CALLBACK CModalDialog::MessageFunction(HWND hDlg,int nMessage,WPARAM wParam,LPARAM lParam)
{
   CModalDialog * dialog;                        // wird this pointer für das aktive Dialogfeld

   if(nMessage==WM_INITDIALOG)                   // wird das Dialogfeld erstellt ?
   {
      dialog = (CModalDialog *) lParam;          // type cast of this pointer
      dialog->m_hDlg = hDlg;                     // Fensterhandle des Dialogfeldes speichern
      SetWindowLong(hDlg,GWL_USERDATA,lParam);   // this pointer als FensterID speichern

      if(dialog->m_hIcon)
      {
         SendMessage(hDlg,WM_SETICON,ICON_SMALL,(LPARAM) dialog->m_hIcon);
      }

      return dialog->OnInitDialog();             // Initfunktion des Dialogfeldes aufrufen
   }

                                                 // besorge den this-pointer für das Dialogfeld
   dialog = (CModalDialog *) GetWindowLong(hDlg,GWL_USERDATA);

   if(dialog==NULL) return FALSE;                // wenn this pointer nicht vorhanden abhauen


   if(nMessage==WM_SYSCOMMAND)
   {
      if((wParam&0xFFF0)==SC_KEYMENU)
      {
         dialog->UnitConvert();
         return TRUE;
      }
   }

   switch(nMessage)
   {
      case WM_COMMAND:                           // Kommandos bearbeiten              
           return dialog->DoCommandMessage(HIWORD(wParam),LOWORD(wParam),lParam);
 
      case WM_HELP:                              // Kontexthilfe bearbeiten
           return dialog->DoHelpMessage(nMessage,wParam,lParam);

      default:                                   // alle anderen Nachrichten weiterleiten
           return dialog->OnMessage(nMessage,wParam,lParam);
   }
}


/*************************************+*****************************************
                               DoCommandMessage

private Function: Kommandos bearbeiten oder weiterleiten
*******************************************************************************/

LRESULT CModalDialog::DoCommandMessage(int flags,int ID,LPARAM lParam)
{
   HWND from;

   switch(ID)
   {
      case IDOK:
           if(flags==BN_CLICKED)
           {
              if((!m_bEndWithReturn)&&((from=GetFocus())!=NULL))
              {  
                 if(from!=GetDlgItem(IDOK))
                 {
                    PostMessage(m_hDlg,WM_NEXTDLGCTL,0,0);
                    break;
                 }
              }
              bool ret = OnCommand(flags,ID,lParam);
              if(!ret) break;
              EndDialog(IDOK);
           }
           break;

      case IDCANCEL:
           OnCommand(flags,ID,lParam);
           EndDialog(IDCANCEL);
           break;

      default: 
           return OnCommand(flags,ID,lParam);
   }

   return (TRUE);
}


/*************************************+*****************************************
                                DoHelpMessage

private Function: Kontexthilfe bearbeiten oder weiterleiten
*******************************************************************************/

LRESULT CModalDialog::DoHelpMessage(int nMessage,WPARAM wParam,LPARAM lParam)
{
   LPHELPINFO hi = (LPHELPINFO) lParam;

   if(hi->iContextType==HELPINFO_WINDOW)
   {
      char szText[1024];
      RECT rc;
                                                 // laden des Hilfetextes 64 weniger wegen eventuellen szAdd.String
      if(LoadString(GetInstance(),hi->iCtrlId,szText,1024-64))
      {
         if(CM_EditControll((HWND)hi->hItemHandle))
         {
            char szAdd[64];

            if(LoadString(GetInstance(),IDC_UNIT_E_CM,szAdd,64))
            {
               strcat(szText,szAdd);
            }
         }
                                                 // überprüfe ob Mausposition innerhalb des Controls liegt
                                                 // denn Kontexthilfe kann auch mit F1 ausgelößt werden
                                                 // und dann ist das Fenster irgendwo (diese wurde in Version 1.0 vergessen !)
         GetWindowRect(GetDlgItem(hi->iCtrlId),&rc);
          
         if(PtInRect(&rc,hi->MousePos))
         {                                       // Hilfefenster an Mausposition erzeugen
            CEtsHelp::CreateContextWnd(szText,hi->MousePos.x,hi->MousePos.y);
         }
         else
         {                                       // Hilfefenster an Controlposition erzeugen
            CEtsHelp::CreateContextWnd(szText,rc.left + 3,rc.top + 3);
         }
      }
      else                                       // Hilfetext wurde nicht gefunden, dann Nachricht weiterleiten
      {
         return OnMessage(nMessage,wParam,lParam);
      }
   }
   return (TRUE);
}


/*************************************+*****************************************
                          CreateSetButtonColorBitmap                                 

protected Function:
*******************************************************************************/

void _stdcall CModalDialog::CreateSetButtonColorBitmap(int nID,COLORREF Color,HBITMAP& hBitmap) const
{
   HWND    hWnd;
   HDC     hdc;
   HDC     memdc;
   RECT    rc;
   HBITMAP hOld;
   HBRUSH  hB;
   HBRUSH  hR;
   HPEN    hPF;

   if(hBitmap) DeleteObject(hBitmap);

   hWnd  = GetDlgItem(nID);
   memdc = CreateCompatibleDC(NULL);
   hdc   = GetDC(NULL);

   GetClientRect(hWnd,&rc);
   hBitmap = CreateCompatibleBitmap(hdc,rc.right,rc.bottom);

   ReleaseDC(NULL,hdc);

   hOld = (HBITMAP) SelectObject(memdc,hBitmap);
   hB   = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
   hR   = CreateSolidBrush(Color);
   hPF  = CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNTEXT));

   SelectObject(memdc,hB);
   PatBlt(memdc,0,0,rc.right,rc.bottom,PATCOPY);
   SelectObject(memdc,hR);
   SelectObject(memdc,hPF);
   Rectangle(memdc,3,3,rc.right-3,rc.bottom-3);
   SelectObject(memdc,GetStockObject(BLACK_PEN));
   SelectObject(memdc,GetStockObject(BLACK_BRUSH));
   SelectObject(memdc,hOld);
   DeleteObject(hR);
   DeleteObject(hB);
   DeleteObject(hPF);   
   DeleteDC(memdc);

   ::SendMessage(hWnd,BM_SETIMAGE,IMAGE_BITMAP,(LPARAM) hBitmap);
}


/*************************************+*****************************************
                                 SetCheckBox

protected Function:
*******************************************************************************/

void _stdcall CModalDialog::SetCheckBox(int Id,int value) const
{
   int flag;

   if(value!=0) flag = 1;
   else         flag = 0;

   SendDlgItemMessage(Id,BM_SETCHECK,flag,0);
}


/*************************************+*****************************************
                                 SetCheckBox

protected Function:
*******************************************************************************/

void _stdcall CModalDialog::SetCheckBox(int Id,bool value) const
{
   int flag;

   if(value) flag = 1;
   else      flag = 0;

   SendDlgItemMessage(Id,BM_SETCHECK,flag,0);
}


/*************************************+*****************************************
                                 GetCheckBox

protected Function:
*******************************************************************************/

bool _stdcall CModalDialog::GetCheckBox(int Id) const
{
   int state=SendDlgItemMessage(Id,BM_GETCHECK,0,0);

   if(state&BST_CHECKED) return true;
   else                  return false;
}


/*************************************+*****************************************
                              SendDlgItemMessage

protected Function:
*******************************************************************************/

int _stdcall CModalDialog::SendDlgItemMessage(int id,int nMessage,WPARAM wParam,LPARAM lParam) const
{
   return ::SendDlgItemMessage(m_hDlg,id,nMessage,wParam,lParam);
}


/*************************************+*****************************************
                                EnableDlgItem

protected Function:
*******************************************************************************/

void _stdcall CModalDialog::EnableDlgItem(int id) const
{
   EnableWindow(GetDlgItem(id),TRUE);
}


/*************************************+*****************************************
                                DisableDlgItem

protected Function:
*******************************************************************************/

void _stdcall CModalDialog::DisableDlgItem(int id) const
{
   EnableWindow(GetDlgItem(id),FALSE);
}


/*************************************+*****************************************
                            DisableDlgItemAndValue

protected Function:
*******************************************************************************/

void _stdcall CModalDialog::DisableDlgItemAndValue(int id) const
{   
   EnableWindow(GetDlgItem(id),FALSE);
   ::SetDlgItemText(m_hDlg,id,"");
}


/*************************************+*****************************************
                                SetDlgItemText

protected Function:
*******************************************************************************/

void _stdcall CModalDialog::SetDlgItemText(int id,const char * szText) const
{
   ::SetDlgItemText(m_hDlg,id,szText);
}


/*************************************+*****************************************
                                SetDlgItemText

protected Function:
*******************************************************************************/

void _stdcall CModalDialog::SetDlgItemText(int id,int number) const
{
   ::SetDlgItemInt(m_hDlg,id,number,TRUE);
}


/*************************************+*****************************************
                                 GetDlgItem

protected Function:
*******************************************************************************/

HWND _stdcall CModalDialog::GetDlgItem(int id) const
{
   return ::GetDlgItem(m_hDlg,id);
}


/*************************************+*****************************************
                                SetDlgFocusTo

protected Function:
*******************************************************************************/

void _stdcall CModalDialog::SetDlgFocusTo(int id) const
{
   SetFocus(GetDlgItem(id));
}


/*************************************+*****************************************
                                GetDlgItemText

protected Function:
*******************************************************************************/

void _stdcall CModalDialog::GetDlgItemText(int id,char * szInput,int nLen) const
{
   ::GetDlgItemText(m_hDlg,id,szInput,nLen);
}


/*************************************+*****************************************
                                GetDlgItemValue

protected Function:
*******************************************************************************/

bool _stdcall CModalDialog::GetDlgItemValue(int flags,int id,int& nValue,int nMin,int nMax) const
{
   if(flags==EN_KILLFOCUS)
   {
      GetDlgItemValue(id,nValue,nMin,nMax);
      return true;
   }
   return false;
}


/*************************************+*****************************************
                                GetDlgItemValue

protected Function:
*******************************************************************************/

void _stdcall CModalDialog::GetDlgItemValue(int id,int& nValue,int nMin,int nMax) const
{
   bool bOutRange = false;
   int  value;

   value = GetDlgItemInt(m_hDlg,id,NULL,TRUE);

   if((nMin!=0)||(nMax!=0))
   {
      if(value < nMin) value=nMin,bOutRange=true;
      if(value > nMax) value=nMax,bOutRange=true;
   }

   if(bOutRange)
   {
      RECT rc;
      char szError[256];
      char szErrorOut[512];

      LoadString(GetInstance(),IDS_MODALDIALOG_RANGEERROR,szError,sizeof(szError));
      wsprintf(szErrorOut,szError,nMin,nMax);

      SetDlgItemText(id,value);
      SetDlgFocusTo(id);
      MessageBeep(MB_ICONEXCLAMATION);
      ::GetWindowRect(GetDlgItem(id),&rc);
      CEtsHelp::CreateErrorWnd(szErrorOut,rc.left + 3,rc.top + 3);
   }
   else
      if(value==0) SetDlgItemText(id,value);

   nValue = value;
}


void CModalDialog::ToDefault()
{
   char szTitle[512];
   char szNew[1024];
   char szDefault[512];

   SendMessage(GetWnd(),WM_GETTEXT,512,(LPARAM) szTitle);

   if(LoadString(GetInstance(),IDS_CMODALDIALOG,szDefault,512))
   {
      wsprintf(szNew,"%s %s",szDefault,szTitle);
      SendMessage(GetWnd(),WM_SETTEXT,0,(LPARAM) szNew);
   }

   SetCheckBox(IDC_C_STD,true);
   DisableDlgItem(IDC_C_STD);
   ShowWindow(GetDlgItem(IDC_B_STD),SW_HIDE);
}



void CModalDialog::UnitConvert()
{
   HWND hEdit = GetFocus();

   if(CUnitDialog::GetLocked()) return;          // UnitDialog schon offen ?

   if(CM_EditControll(hEdit))
   {
      char        szZahl[128];
      RECT        rcPos;
      CUnitDialog dialog = CUnitDialog(this);

      if(SendMessage(hEdit,WM_GETTEXT,(WPARAM) 128,(LPARAM)szZahl))
      {
         int cm = atoi(szZahl);

         dialog.Params(cm,SETPARAMS);
      }

      GetWindowRect(hEdit,&rcPos);
       
      rcPos.left+=3;
      rcPos.top +=3;

      dialog.SetPosition(rcPos);

      if(dialog.DoModal())
      {
         int cm;
         dialog.Params(cm,GETPARAMS);
         wsprintf(szZahl,"%d",cm);
         SendMessage(hEdit,WM_SETTEXT,0,(LPARAM) szZahl);               
      }
   }
}

bool CModalDialog::CM_EditControll(HWND hEdit)
{
   if(IsWindow(hEdit))
   {
      char szClass[128];

      if(GetClassName(hEdit,szClass,128))
      {
         if(strcmp("Edit",szClass)==NULL)        // ein Editcontroll ?
         {                                       // ist es ein Number Eingabefeld !
            if((GetWindowLong(hEdit,GWL_STYLE)&ES_NUMBER)==0) return false;
                                                 // Erlaubt die Dialogbox für diese Controll den UnitDialog ?
                                                 // !!!! virtual function default == yes !!!!
            if(!UnitConvertAllow(hEdit)) return false; 
                                                 // kein Input möglich !
                                                 // also keine Zusatzhilfe und kein UnitDialog !
            if(!IsWindowEnabled(hEdit)) return false; 

            return true;                         // OK
         }
      }
   }

   return false;                                 // nicht OK
}
