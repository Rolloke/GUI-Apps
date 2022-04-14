#ifndef _CUNITCONVERTDIALOG_
#define _CUINTCONVERTDIALOG_

/*******************************************************************************
                                 CUnitDialog.h
                                 -------------

                         Dialogbox zum Einheitenumwandeln


                   (c) 2001 ELAC Technische Software GmbH Kiel

                          Version 1.0 Stand:  10.05.2001


inheritated from: CModalDialog
                                                    programmed by Oliver Wesnigk
*******************************************************************************/


#include "CModalDialog.h"
#include "CGraphicsMath.h"

class CUnitDialog: public CModalDialog, public CGraphicsMath
{
public:

   CUnitDialog(CWindow* pCWindow,bool bStd=false,int bef=NULL);
   CUnitDialog(CModalDialog * pCDialog,bool bStd=false,int bef=NULL);
   ~CUnitDialog();

   virtual bool OnInitDialog();
   virtual bool OnCommand(int flags,int id,LPARAM lParam);
   virtual int  OnMessage(int,WPARAM,LPARAM);

   void Params(int& cm,bool);
   void SetPosition(RECT& rc){m_ptPos.x = rc.left;m_ptPos.y = rc.top; };

   static bool GetLocked() {return gm_bLocked;}; 

private:

   void   Convert(char * szZahl);
   void   Update();

   int    m_cm;
   HFONT  m_hFett;
   POINT  m_ptPos;
   int    m_nAlpha;

   static bool gm_bLocked;                       // es darf nur eine Instance hiervon geben
};

#endif