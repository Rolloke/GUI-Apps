// LabelContainer.h: Schnittstelle für die Klasse CLabelContainer.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LabelCONTAINER_H__C553DC82_282A_11D1_9DB9_B051418EA04A__INCLUDED_)
#define AFX_LabelCONTAINER_H__C553DC82_282A_11D1_9DB9_B051418EA04A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Label.h"

#define CL_LABELUNPICKED   0x01
#define CL_TRACKPICKRECT   0x02
#define CL_LABELPICKED     0x04
#define CL_NEWLABELPICKED  0x08
#define CL_LABELCHANGED    0x10

class CFileHeader;

class AFX_EXT_CLASS CLabelContainer : public CLabel
{
   DECLARE_SERIAL(CLabelContainer)
public:
   CLabelContainer();
   virtual ~CLabelContainer();
   // Zeichen-Funktionen
   virtual  bool  Draw(CDC *, int bAtr = 1);
   virtual  bool  DrawShape(CDC *, bool);
   // Verschiebe- und Editierfunktionen
   virtual  void  MoveLabel(CPoint);
   virtual  void  ScaleLabel(CSize, CSize);
   virtual  void  SetPoint(CPoint);
   virtual  int   IsOnLabel(CDC*, CPoint *, CRect*);
   virtual  CRect GetRect(CDC*, UINT nFlags = 0);
   virtual  bool  GetRgn(int n_cm, CDC *, CRgn *, UINT nFlags = 0);
   virtual  void  Serialize(CArchive &);
#ifdef _DEBUG
   	virtual void AssertValid() const;
#endif

   void  GetChecksum(CFileHeader *);
   // Einfüge-und Lösch-funktionen
   bool     InsertLabel(CLabel *, int nPos=-1);
   bool     InsertList(CLabelContainer *, bool bReverse = true);
   bool     RemoveLabel(CLabel *);
   bool     RemoveList(CLabelContainer *);
   void     RemoveAll();
   void     DeleteAll();
   void     SetDeleteContent(bool bDel){m_State.deletecontent=bDel;};
   bool     GetDeleteContent(){return m_State.deletecontent;};
   // Erfragefunktionen
   CLabel  *GetLabel(int pos);
   int      GetCount() {return m_List.GetCount();};
   bool     IsEmpty()  {return ((m_List.IsEmpty()) ? true : false);};
   bool     IsLast(CLabel*);
   bool     IsFirst(CLabel *);
   int      GetIndex(CLabel *);
   // Pickfunktion
   int      PickLabel(CPoint *, CLabelContainer *, CDC *, bool, int, CRgn*, bool bDrSh=false);
   int      OnPickRect(CPoint *, CLabelContainer *, CDC *,  int, CRgn *, bool bDrSh=false);
   int      EditLabelPoints(CPoint *,CPoint *, CDC *pDC, CRect *pClipRect=NULL);
   CLabel*  HitLabel(CPoint *, CDC *, int *pnDist=NULL);
   void     ResetStates();
   bool     SetMatchPoint(CPoint *p);
   void     SetPicked(bool);
   // Listen Positionsfunktionen
   bool     SwapLabels(POSITION, POSITION, int n_cm=0, CDC *pDC=NULL, CRgn *prgn=NULL);
   bool     SwapDown(CLabel *pl, CDC *pDC=NULL, CRgn *prgn=NULL);
   bool     SwapUp(CLabel *pl, CDC *pDC=NULL, CRgn *prgn=NULL);
   bool     SwapFirst(CLabel *pl, CDC *pDC=NULL, CRgn *prgn=NULL);
   bool     SwapLast(CLabel *pl, CDC *pDC=NULL, CRgn *prgn=NULL);

   int      ProcessWithLabels(CRuntimeClass*, void *, int (*pCallBack)(CLabel*, void*));

   static int CountLabel(       CLabel *, void *);
   static int CountVisibleLabel(CLabel *, void *);
   static int GetIndexFromLabel(CLabel *, void *);
   static int GetLabelFromIndex(CLabel *, void *);
   static int HitLabel(         CLabel *, void *);
   static int ClipLabel(        CLabel *, void *);
   static int CheckScaling(     CLabel *, void *);
   static int GetFirstLabel(    CLabel *, void *);
   static int GetNextLabel(     CLabel *, void *);

private:
   CLabel      *m_pL;
   CLabelList   m_List;
};


#endif // !defined(AFX_LabelCONTAINER_H__C553DC82_282A_11D1_9DB9_B051418EA04A__INCLUDED_)
