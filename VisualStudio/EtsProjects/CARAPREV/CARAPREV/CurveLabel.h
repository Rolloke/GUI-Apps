// CurveLabel.h: Schnittstelle für die Klasse CCurveLabel.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CURVELabel_H__A46AD202_F83A_11D0_9DB9_B051418EA04A__INCLUDED_)
#define AFX_CURVELabel_H__A46AD202_F83A_11D0_9DB9_B051418EA04A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#define WAVE_FORMAT_PCM     1

struct SWaveFile
{
   char  sRIFF[4];
   long  nFileLength;
   char  sWAVEfmt_[8];
   long  nHeaderLength;
};

class CWaveParameter
{
public:
   CWaveParameter()
   {
      ZeroMemory(this, sizeof(*this));
      bSigned = TRUE;
   }

   SWaveFile*     pWaveFile;
   WAVEFORMATEX*  pWaveFmtEx;
   HANDLE         hFile;
   HANDLE         hFileMapping;
   BYTE*          pData;
   int            nReference;
   BOOL           bSigned;
};

//#include "Label.h"
#include "MathCurve.h"
#include "TextLabel.h"  // Hinzugefügt von ClassView

class CPlotLabel;
class CFileHeader;

class AFX_EXT_CLASS CCurveLabel : public CMathCurve, public CTextLabel 
{
   DECLARE_SERIAL(CCurveLabel)
public:
   CCurveLabel();
   CCurveLabel(CCurveLabel*);
   virtual ~CCurveLabel()
   {
      #ifdef CHECK_LABEL_DESTRUCTION
      TRACE("CCurveLabel::Destructor\n");
      #endif
   };
   // Zeichenfunktionen
   virtual bool   Draw(CDC *, int bAtr = 1);
   virtual bool   DrawShape(CDC *, bool);
   void           DrawCursor(CDC *, bool, SCurve *pdpCursor=NULL);
	void           DrawLocusValue(CDC*, int);

   void           SetCursorDrawn(bool b){m_bCursorDrawn = b;};
   bool           IsCursorDrawn()      {return m_bCursorDrawn;};

   // Pickfunktionen
   virtual int    IsOnLabel(CDC *, CPoint*, CRect *pR = NULL);
   virtual CRect  GetRect(CDC*, UINT nFlags = 0);
#ifdef _DEBUG
   	virtual void AssertValid() const;
#endif
   virtual void   MoveLabel(CPoint);
   virtual void   SetPoint(CPoint);
   virtual void   Serialize(CArchive &);

   void           GetChecksum(CFileHeader *);

   SCurve         GetDPoint(int n=-1);
   bool           SetMatchPoint(CPoint *);
   bool           SetMatchPoint(int n);
   virtual void   SetDescription(const TCHAR*);
   // Dateizugriffsfunktionen
   void           Read(HANDLE);
   void           Write(HANDLE);
   // Zugriffsfunktionen für die Änderung der Kurvenwerte
   void           SetDPoint(SCurve, CPoint, int n= -1);
   void           UndoChanges();
   void           SaveEditUndo(CArchive &);
   void           RestoreEditUndo(CArchive &);
   bool           GetUndoRgn(int , CDC *, CRgn *);
   // Zugriffsfunktion für die Darstellungsparameter
   void           SetPoint(CPoint p, int n) {CLabel::SetPoint(p,n);};
   void           SetLineStartPoint()       {m_nLineEditPoint=m_nAct; m_dpLinePoint = GetCurveValue(m_nAct);};
   bool           UpdateLine(CArchive *par = NULL);
   bool           GetLinePoints(SCurve&, SCurve&);
   bool           IsEditingLine()   {return m_nLineEditPoint != -1;};
   CPoint         GetPoint(int n=-1);
   // Zugriffsfunktion für die Darstellungsparameter
   void           SetTransformation(CPlotLabel *ptran);
   CPlotLabel    *GetTransformation() {return m_pTran;};
   void           SetLogPen(LOGPEN *);
   LOGPEN         GetLogPen();
   void           SetUndoAllValues();
   int            GetFormatEx();
   static int     WaveParamFkt(CF_GET_CURVE_PARAMS*pP);
//   void           ResetTextPos();

private:
	bool m_bCursorDrawn;
   void          Init();
   CPlotLabel   *m_pTran;
   CPen         *m_pCurvePen;
   int           m_nLineEditPoint;
   int           m_nUndoPoint;
   SCurve        m_dpLinePoint;
   SCurve        m_dpUndoPoints[3];
public:
   int           m_nChannel;
protected:
	LPTSTR ExtendCurveText();
};

#endif // !defined(AFX_CURVELabel_H__A46AD202_F83A_11D0_9DB9_B051418EA04A__INCLUDED_)
