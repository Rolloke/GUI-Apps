#ifndef _CLINELABEL_
#define _CLINELABEL_

#include "CLabel.h"
#include "CLinedialogparams.h"
#include "CMainWndFriend.h"
// eindeutige Klassen ID (Achtung immer nur 8 Bit)

#define CLineLabel_CID 6

class CTextLabel;

class CLineLabel : public CLabel,public CMainWndFriend
{
DYNACREATE(CLineLabel)

public:
   CLineLabel();
   CLineLabel(CLINEDIALOGPARAMS&,POINT&,POINT&);

   virtual      ~CLineLabel(){};
   virtual void Draw(HDC);
   virtual void Modify();
   virtual int  HitTest(POINT&);
   virtual int  HitTest(RECT&);

   virtual int  GetHelpText(int,TCHAR *,int);
   virtual bool Serialize(STREAM&);
   virtual bool ChangeProperties(CWindow *,HDC);

   CTextLabel * GetMessureText();

   void SetStyle(bool);   

private:

   CLINEDIALOGPARAMS m_ldp;
};
#endif