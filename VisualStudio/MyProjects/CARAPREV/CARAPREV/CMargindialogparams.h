#ifndef _CMARGINDIALOGPARAMS_
#define _CMARGINDIALOGPARAMS_


#define DONT_SCALE_LABEL  0
#define SCALE_LABEL       1
#define RESTORE_POSITIONS 2

struct SMarginDlgParam
{
   bool  bFrame;           // Rahmen um Druckbereich zeichnen

   RECT  rcPageLoMetric;   // zum zeichnen im MM_LOMETRIC MapMode
   RECT  rcDrawLoMetric;
   POINT OffsetLoMetric;

   int   nScaleLabel;      // zum Scalieren der Label
   RECT  rcOldDrawLoMetric;
   short dmOrientation;
   short dmPaperSize;

   POINT OffsetDev;        // In Devicekoordinaten
};

struct PRINTERPARAMETER
{
   int      nMaxCopies;
   bool     bPrintToFile;
   POINT    ptOffset;
   SIZE     szPage;
   SIZE     szDrawArea;
   double   dScaleX2Lom;
   double   dScaleY2Lom;
   double   dScaleX2Dev;
   double   dScaleY2Dev;
   RECT     rcMinMargin;
   RECT     rcMargin;
};

#endif
