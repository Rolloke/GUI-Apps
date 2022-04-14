/********************************************************************/
/*                                                                  */
/*                     Library CCurve                               */
/*                                                                  */
/*     Schnittstelle zum Programm ETS2DV zum Darstellen von Kurven  */
/*     Enthält die Klassen: CCurve, CMathCurve, CPlot, CFileHeader  */
/*                                                                  */
/*     programmed by Rolf Kary-Ehlers                               */
/*                                                                  */
/*     Version 2.2            04.04.2002                            */
/*                                                                  */
/********************************************************************/
// Curve.cpp: Implementierung der Klasse CCurve.
//
//////////////////////////////////////////////////////////////////////

// für alle Programme, die nicht MFC einbinden, müssen die
// folgende Zeile auskommentieren.
//#include "stdafx.h"

#ifdef CARA_MFC_HEADER
   #include "CaraWinApp.h"
   #include "CARAPREV.h"
   #include "resource.h"
#endif

#ifndef INVLOG10_2
   #define INVLOG10_2  3.3219280948873623478703194294894
#endif

#include <cmath>
#include <float.h>
#include "Curve.h"
#include "FileHeader.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
UINT   CCurve::gm_nInstances = 0;
HANDLE CCurve::gm_hHeap      = 0;
double CCurve::gm_dDummy     = 0;

#ifdef CARA_MFC_HEADER
   extern bool     g_bDoReport;
#else
   #include "Debug.h"
#endif

CFloatPrecision::CFloatPrecision()
{
   m_nOldFp = _controlfp(_PC_64, _MCW_PC);
}

CFloatPrecision::~CFloatPrecision()
{
   _controlfp(m_nOldFp, _MCW_PC);
}

CCurve::CCurve()
{
   Init();
   m_Parameter.xDivision = 1;
   m_Parameter.yDivision = 1;
   m_dOffset             = 0.0;
   m_dOrg                = 0.0;
   m_dStep               = 1.0;
   m_pGetCrvParamFkt     = NULL;
   m_pGetCrvParam        = NULL;
}

CCurve::CCurve(CCurve *pc)
{
   Init();
   if (pc)
   {
      m_Parameter = pc->m_Parameter;
      m_dOffset   = pc->m_dOffset;
      m_dOrg      = pc->m_dOrg;
      m_dStep     = pc->m_dStep;
   }
}

void* CCurve:: alloc(size_t nSize, void *pOld)
{
   void * pointer = NULL;
#ifdef CARA_MFC_HEADER
/*
   DWORD dwSize = CCaraWinApp::GetTotalPhysicalMem();
   if (nSize >= dwSize)
   {
      if (nSize >= CCaraWinApp::GetTotalVirtualMem())
      {
         if (pOld) free(pOld);
         return NULL;
      }
      CString strMsg;
      strMsg.Format(IDS_FORMAT_LARGE_MEM_REQUEST, nSize >> 20, dwSize >> 20);
      if (AfxMessageBox(strMsg, MB_YESNO|MB_ICONEXCLAMATION) == IDNO)
      {
         if (pOld) free(pOld);
         return NULL;
      }
   }
*/
#else
   static const char *szErrorText[] = 
   {                             // Error
      "Heap already exists !",   // 1
      "Heap creation failure !", // 2
      "Out of Memory !"          // 3
   };
#endif
   try
   {
      if(gm_nInstances==0)
      {
         if(gm_hHeap!=0) throw (int) 1;
         gm_hHeap = HeapCreate(0, 1048576,0); // growable Heap
         if(gm_hHeap==0) throw (int) 2;
      }
      if (pOld) pointer = HeapReAlloc(gm_hHeap,0, pOld, nSize);
      else      pointer = HeapAlloc(gm_hHeap,0, nSize);

      if(pointer==NULL) throw (int) 3;

      if (pOld==NULL) gm_nInstances++;
   }
   catch (int nError)
   {
#ifdef CARA_MFC_HEADER
      nError += IDE_HEAP_EXISTS-1;
      AfxMessageBox(nError, MB_OK|MB_ICONEXCLAMATION);
#else
      MessageBox((HWND) NULL, szErrorText[nError-1], "Error", MB_OK|MB_ICONEXCLAMATION);
#endif
   }
   return pointer;
}

void CCurve::free(void* p)
{
#ifndef CARA_MFC_HEADER
   static const char *szErrorText[] = 
   {                                // Error
      "Destruction without Heap !", // 1
      "HeapFree()-Error",           // 2
      "Heap Destruction failure !"  // 3
   };
#endif

   try
   {
      if(gm_hHeap==0)                      throw (int) 1;
      else if(HeapFree(gm_hHeap, 0, p)==0) throw (int) 2;

      gm_nInstances--;

      if(gm_nInstances==0)
      {
         if(HeapDestroy(gm_hHeap)==0)      throw (int) 3;
         gm_hHeap = 0;
      }
   }
   catch (int nError)
   {
#ifdef CARA_MFC_HEADER
      nError += IDE_DESTRUCTION_WITHOUT_HEAP-1;
      AfxMessageBox(nError, MB_OK|MB_ICONEXCLAMATION);
#else
      MessageBox((HWND) NULL, szErrorText[nError-1], "Error", MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
#endif
   }
}

bool CCurve::HeapWatch()
{
   if ((gm_hHeap == 0) && (gm_nInstances > 0))
   {
#ifdef CARA_MFC_HEADER
      AfxMessageBox(IDE_MEMORY_LEAKS_CCURVE, MB_OK|MB_ICONEXCLAMATION);
#else
      MessageBox((HWND) NULL, "Detected Memory Leaks on CCurve-Heap !", "HeapWatch", MB_OK|MB_ICONEXCLAMATION);
#endif
   }
   if (gm_hHeap)
   {
#ifdef CARA_MFC_HEADER
      CString format;
      format.Format(IDE_MEMORY_OBJECTS_CCURVE, gm_nInstances);
      AfxMessageBox(format, MB_OK|MB_ICONEXCLAMATION);
#else
      char text[64];
      wsprintf(text, "Number of Created Objects on CCurve-Heap %d :  %d", gm_hHeap, gm_nInstances);
      MessageBox((HWND) NULL, text, "HeapWatch", MB_OK|MB_ICONEXCLAMATION);
#endif
      return true;
   }
   return false;
}

void CCurve::DestroyHeap()
{
   if(gm_hHeap && (HeapDestroy(gm_hHeap)==0))
   {
#ifdef CARA_MFC_HEADER
      AfxMessageBox(IDE_HEAP_DESTROY, MB_OK|MB_ICONEXCLAMATION);
#else
      MessageBox((HWND) NULL, "Heap Destruction failure !", "HeapWatch", MB_OK|MB_ICONEXCLAMATION);
#endif
   }
}

void CCurve::Init()
{
   memset(&m_Parameter, 0, sizeof(BSParameter));
   m_nNum                   = 0;
   m_pArray                 = NULL;
   m_pszDescription         = NULL;
   m_pszUnitx               = NULL;
   m_pszUnity               = NULL;
   m_pszUnitLocus           = NULL;
   m_LogPen.lopnStyle       = PS_SOLID;
   m_LogPen.lopnWidth.x     = 1;
   m_LogPen.lopnWidth.y     = 1;
   m_LogPen.lopnColor       = 0xFFFFFFFF;
   m_Parameter.penchanged   = 1;
   m_Parameter.writeprotect = true;
   m_Parameter.xNumMode     = 1;
   m_Parameter.yNumMode     = 1;
}

CCurve::~CCurve()
{
//   TRACE("CCurve::Destructor\n");
   Destroy();
   if (m_pszDescription) delete[] m_pszDescription;
   if (m_pszUnitx)       delete[] m_pszUnitx;
   if (m_pszUnity)       delete[] m_pszUnity;
   if (m_pszUnitLocus)   delete[] m_pszUnitLocus;
}

SCurve CCurve::GetCurveValue(int n)
{
   SCurve sc = {0,0};
   if ((n >= 0) && (n < m_nNum))
   {
      switch (m_Parameter.format)
      {
         case CF_DOUBLE: return ((SCurve*)m_pArray)[n];
         case CF_FLOAT:
            sc.x = (double)((SCurveFloat*)m_pArray)[n].x;
            sc.y = (double)((SCurveFloat*)m_pArray)[n].y;
            break;
         case CF_SHORT:
            sc.x = (double)((SCurveShort*)m_pArray)[n].x;
            sc.y = (double)((SCurveShort*)m_pArray)[n].y;
            break;
         case CF_SINGLE_DOUBLE:
            sc.x = GetX_Value(n);
            sc.y = ((double*)m_pArray)[n];
            break;
         case CF_SINGLE_FLOAT:
            sc.x = GetX_Value(n);
            sc.y = (double)((float*)m_pArray)[n];
            break;
         case CF_SINGLE_SHORT:
            sc.x = GetX_Value(n);
            sc.y = (double)((short*)m_pArray)[n];
            break;
         case CF_ARBITRARY:
         case CF_ARBITRARY_CONTR:
         if (m_pGetCrvParamFkt)
         {
            CF_GET_CURVE_PARAMS gcp = {CF_GET_VALUE, n, this, &sc, m_pGetCrvParam};
            VERIFY(m_pGetCrvParamFkt(&gcp)!=0);
         } break;
         default: 
            ASSERT(false); 
            break;
      }
   }
   else
   {
      TRACE("Index %d out of Range\n", n);
      ASSERT(false);
   }
   return sc;
}

double CCurve::GetX_Value(int n)
{
   if ((n >= 0) && (n < m_nNum))
   {
      switch (m_Parameter.format)
      {
         case CF_DOUBLE: return ((SCurve*)m_pArray)[n].x;
         case CF_FLOAT:  return ((double)((SCurveFloat*)m_pArray)[n].x);
         case CF_SHORT:  return ((double)((SCurveShort*)m_pArray)[n].x);
         case CF_SINGLE_DOUBLE: case CF_SINGLE_FLOAT: case CF_SINGLE_SHORT:
            switch(GetXDivision())
            {
               case CCURVE_POLAR:                                              // Winkelabstände sind auch linear
               case CCURVE_LINEAR:      return (m_dOrg +   n * m_dStep);
               case CCURVE_LOGARITHMIC: 
               case CCURVE_FREQUENCY:   return (m_dOrg * pow(m_dStep, n));     // Schrittweite logarithmisch (Faktor)
//               case CCURVE_LOGARITHMIC: return (m_dOrg * pow(10, n * m_dStep));// Schrittweite an Dekaden orientiert
//               case CCURVE_FREQUENCY:   return (m_dOrg * pow( 2, n * m_dStep));// Schrittweite an Oktaven orientiert
               default:
                  ASSERT(false);
                  break;
            } break;
         case CF_ARBITRARY:
         case CF_ARBITRARY_CONTR:
         if (m_pGetCrvParamFkt)
         {
            SCurve sc;
            CF_GET_CURVE_PARAMS gcp = {CF_GET_XVALUE, n, this, &sc, m_pGetCrvParam};
            VERIFY(m_pGetCrvParamFkt(&gcp)!=0);
            return sc.x;
         } break;
         default: ASSERT(false); break;
      }
   }
   TRACE("Index %d out of Range\n", n);
   ASSERT(false);
   return 0.0;
}

double CCurve::GetY_Value(int n)
{
   if ((n >= 0) && (n < m_nNum))
   {
      switch (m_Parameter.format)
      {
         case CF_DOUBLE:        return ((SCurve*)m_pArray)[n].y;
         case CF_FLOAT:         return ((double)((SCurveFloat*)m_pArray)[n].y);
         case CF_SHORT:         return ((double)((SCurveShort*)m_pArray)[n].y);
         case CF_SINGLE_DOUBLE: return ((double*)m_pArray)[n];
         case CF_SINGLE_FLOAT:  return (double)((float*)m_pArray)[n];
         case CF_SINGLE_SHORT:  return (double)((short*)m_pArray)[n];
         case CF_ARBITRARY:
         case CF_ARBITRARY_CONTR:
         if (m_pGetCrvParamFkt)
         {
            SCurve sc;
            CF_GET_CURVE_PARAMS gcp = {CF_GET_YVALUE, n, this, &sc, m_pGetCrvParam};
            VERIFY(m_pGetCrvParamFkt(&gcp)!=0);
            return sc.y;
         } break;
         default:
            ASSERT(false);
            break;
      }
   }
   TRACE("Index %d out of Range\n", n);
   ASSERT(false);
   return 0.0;
}

double& CCurve::SetX_Value(int n)
{
   if ((n >= 0) && (n < m_nNum))
   {
      switch (m_Parameter.format)
      {
         case CF_DOUBLE: return ((SCurve*)m_pArray)[n].x;
         case CF_SINGLE_DOUBLE: case CF_SINGLE_FLOAT: case CF_SINGLE_SHORT:
            if (n == 0) return m_dOrg;
         default:
            ASSERT(false);
            TRACE("Wrong Format\nUse Function {bool CCurve::SetX_Value(int n, double dVal)} !\n", n);
            return gm_dDummy;
      }
   }
   else
   {
      ASSERT(false);
      TRACE("Index %d out of Range\n", n);
      return gm_dDummy;                                        // Error: Wertzuweisung an Dummy
   }
}

bool CCurve::SetX_Value(int n, double dVal)
{
   if ((n >= 0) && (n < m_nNum))
   {
      switch (m_Parameter.format)
      {
         case CF_DOUBLE: ((SCurve*)m_pArray)[n].x      =               dVal;  return true;
         case CF_FLOAT:  ((SCurveFloat*)m_pArray)[n].x = (float)       dVal;  return true;
         case CF_SHORT:  ((SCurveShort*)m_pArray)[n].x = DoubleToShort(dVal); return true;
         case CF_SINGLE_DOUBLE: case CF_SINGLE_FLOAT: case CF_SINGLE_SHORT:
            if      (n == 0)
            {
               m_dOrg = dVal;
               return true;
//               if (fabs(m_dOrg - dVal) > 1e-13) m_dOrg = dVal;
            }
            else if ((n == 1) || (n == (m_nNum-1)))
            {
               switch (GetXDivision())
               {
                  case CCURVE_POLAR: case CCURVE_LINEAR:       // Winkelabstände sind auch linear
                     if (n == 1) m_dStep =  dVal - m_dOrg;
                     else        m_dStep = (dVal - m_dOrg) / (double)n;
                     return true;
                  case CCURVE_LOGARITHMIC:                     // Schrittweite logarithmisch (Faktor)
                  case CCURVE_FREQUENCY:
                     ASSERT(m_dOrg != 0.0);
                     if (n == 1)
                        m_dStep =     dVal / m_dOrg;
                     else
                        m_dStep = pow(dVal / m_dOrg, 1.0/(double)n);
                     return true;
/*
                  case CCURVE_LOGARITHMIC:                     // Schrittweite an Dekaden orientiert
                     ASSERT(GetX_Value(0) > 0.0);
                     dStep = log10(dVal / GetX_Value(0));
                     break;
                  case CCURVE_FREQUENCY:
                     ASSERT(GetX_Value(0) > 0.0);
                     dStep = log10(dVal / GetX_Value(0)) * INVLOG10_2;
                     break;
*/
                  default:
                     ASSERT(false);
                     break;
               }
//               if (n > 1) dStep /= (double)n;
//               if (fabs(m_dStep - dStep) > 1e-13) m_dStep = dStep;
            }
#ifdef _DEBUG
            else
            {
               switch (m_Parameter.format)
               {
                  case CF_SINGLE_DOUBLE: case CF_SINGLE_FLOAT: case CF_SINGLE_SHORT:
                  {
                     double dXVal = GetX_Value(n);
                     if (fabs(dVal - dXVal) > 1e-18)
                        TRACE("X-Value different : %.12f\n", dXVal - dVal);
                  } break;
                  default: break;
               }
            }
#endif //_DEBUG
         break;
         case CF_ARBITRARY:
         case CF_ARBITRARY_CONTR:
         if (m_pGetCrvParamFkt)
         {
            SCurve sc = {dVal, 0};
            CF_GET_CURVE_PARAMS gcp = {CF_SET_XVALUE, n, this, &sc, m_pGetCrvParam};
            VERIFY(m_pGetCrvParamFkt(&gcp)!=0);
         } break;
         default:
            ASSERT(false);
            return false;
      }
   }
   else
   {
      ASSERT(false);
      TRACE("Index %d out of Range\n", n);
   }
   return false;
}

double& CCurve::SetY_Value(int n)
{
   if ((n >= 0) && (n < m_nNum))
   {
      switch (m_Parameter.format)
      {
         case CF_DOUBLE: return ((SCurve*)m_pArray)[n].y;
         case CF_SINGLE_DOUBLE: return ((double*)m_pArray)[n];
         default:
            ASSERT(false);
            TRACE("Wrong Format\nUse function {bool CCurve::SetY_Value(int n, double dVal)} !\n", n);
            return gm_dDummy;
      }
   }
   else
   {
      ASSERT(false);
      TRACE("Index %d out of Range\n", n);
      return gm_dDummy;                                        // Error: Wertzuweisung an Dummy
   }
}

bool CCurve::SetY_Value(int n, double dVal)
{
   if ((n >= 0) && (n < m_nNum))
   {
      switch (m_Parameter.format)
      {
         case CF_DOUBLE:        ((SCurve*)m_pArray)[n].y      =               dVal;  return true;
         case CF_FLOAT:         ((SCurveFloat*)m_pArray)[n].y = (float)       dVal;  return true;
         case CF_SHORT:         ((SCurveShort*)m_pArray)[n].y = DoubleToShort(dVal); return true;
         case CF_SINGLE_DOUBLE: ((double*)m_pArray)[n]        =               dVal;  return true;
         case CF_SINGLE_FLOAT:  ((float *)m_pArray)[n]        = (float)       dVal;  return true;
         case CF_SINGLE_SHORT:  ((short *)m_pArray)[n]        = DoubleToShort(dVal); return true;
         case CF_ARBITRARY:
         case CF_ARBITRARY_CONTR:
         if (m_pGetCrvParamFkt)
         {
            SCurve sc = {0, dVal};
            CF_GET_CURVE_PARAMS gcp = {CF_SET_YVALUE, n, this, &sc, m_pGetCrvParam};
            VERIFY(m_pGetCrvParamFkt(&gcp)!=0);
         } break;
         default:
            ASSERT(false);
            return false;
      }
   }
   else
   {
      ASSERT(false);
      TRACE("Index %d out of Range\n", n);
   }
   return false;
}

bool CCurve::SetCurveValue(int n, SCurve &c)
{
   if ((n >= 0) && (n < m_nNum))
   {
      switch (m_Parameter.format)
      {
         case CF_SINGLE_DOUBLE: ((double*)m_pArray)[n] = c.y;                break;
         case CF_SINGLE_FLOAT:  ((float *)m_pArray)[n] = (float) c.y;        break;
         case CF_SINGLE_SHORT:  ((short *)m_pArray)[n] = DoubleToShort(c.y); break;
         case CF_DOUBLE:
            ((SCurve*)m_pArray)[n] = c;
            return true;
         case CF_FLOAT:  
            ((SCurveFloat*)m_pArray)[n].x = (float) c.x;
            ((SCurveFloat*)m_pArray)[n].y = (float) c.y;
            return true;
         case CF_SHORT:
            ((SCurveShort*)m_pArray)[n].x = DoubleToShort(c.x);
            ((SCurveShort*)m_pArray)[n].y = DoubleToShort(c.y);
            return true;
         case CF_ARBITRARY:
         case CF_ARBITRARY_CONTR:
         if (m_pGetCrvParamFkt)
         {
            CF_GET_CURVE_PARAMS gcp = {CF_SET_VALUE, n, this, &c, m_pGetCrvParam};
            VERIFY(m_pGetCrvParamFkt(&gcp)!=0);
            return true;
         } break;
         default: 
            ASSERT(false); 
            return false;
      }
      SetX_Value(n, c.x);
      return true;
   }
   else
   {
      ASSERT(false);
      TRACE("Index %d out of Range\n", n);
      return false;                                        // Error: Wertzuweisung an Dummy
   }
}

bool CCurve::SetSize(int n)
{
   if ((   m_Parameter.format == CF_ARBITRARY
	   || m_Parameter.format == CF_ARBITRARY_CONTR)
	   && m_pGetCrvParamFkt)
   {
       CF_GET_CURVE_PARAMS gcp = {CF_SET_SIZE, 0, this, NULL, m_pGetCrvParam};
	   return m_pGetCrvParamFkt(&gcp) != 0 ? true : false;
   }
   bool bNewSize = (m_pArray == NULL) ? true : false;          // neues Array allozieren ?
   if      (n == 0) {Destroy();  return true;}                 // Array löschen
   else if (n == m_nNum)         return true;                  // Gleiche Größe setzen, nicht notwendig aber auch kein Fehler.
   else if (n  < 0)
   {
      ASSERT(false);
      return false;                 // Negative Größe, Fehler
   }

   m_nNum   = n;
   m_pArray = alloc(GetArraySize(), m_pArray);
   if (!m_pArray)
   {
      m_nNum = 0;
//      ASSERT(false);
      return false;                                            // allocieren fehlgeschlagen
   }
   if (bNewSize) memset(m_pArray, 0, GetArraySize());          // wenn neu, mit 0 initialisieren

   if (m_nNum < 3) SetEditable(false);
   return true;
}

int CCurve::GetArraySize()
{
   switch(m_Parameter.format)
   {
      case CF_DOUBLE:        return (sizeof(SCurve     ) * m_nNum);
      case CF_FLOAT:         return (sizeof(SCurveFloat) * m_nNum);
      case CF_SHORT:         return (sizeof(SCurveShort) * m_nNum);
      case CF_SINGLE_DOUBLE: return (sizeof(double     ) * m_nNum);
      case CF_SINGLE_FLOAT:  return (sizeof(float      ) * m_nNum);
      case CF_SINGLE_SHORT:  return (sizeof(short      ) * m_nNum);
      case CF_ARBITRARY:
      case CF_ARBITRARY_CONTR:
      if (m_pGetCrvParamFkt)
      {
         CF_GET_CURVE_PARAMS gcp = {CF_GET_SIZE, 0, this, NULL, m_pGetCrvParam};
         return m_pGetCrvParamFkt(&gcp);
      }
      default:
         ASSERT(false);
         return 0;
   }
}

void CCurve::SetCrvParamFkt(GET_CRV_PARAM_FKT pFkt, void *pParam)
{
   m_pGetCrvParamFkt = pFkt;
   m_pGetCrvParam    = pParam;
}

void CCurve::Destroy()
{
   switch(m_Parameter.format)
   {
      case CF_ARBITRARY:
      case CF_ARBITRARY_CONTR:
		if (m_pGetCrvParamFkt)
		{
			CF_GET_CURVE_PARAMS gcp = {CF_DELETE_VALUES, 0, this, NULL, m_pGetCrvParam};
			m_pGetCrvParamFkt(&gcp);
		} break;
		default:
		if (m_pArray)
		{
			free(m_pArray);
			m_pArray = NULL;
		} break;
   }
   m_nNum = 0;
}

void CCurve::Write(HANDLE hfile)
{
   DWORD dwactbytes=0;
   auto   size = strlen("");
   int nFormat = m_Parameter.format;
   if (hfile==INVALID_HANDLE_VALUE)  return;

   WriteFile(hfile, &m_nNum,      sizeof(int),         &dwactbytes, NULL);
   WriteFile(hfile, &m_dOffset,   sizeof(double),      &dwactbytes, NULL);
   WriteFile(hfile, &m_Parameter, sizeof(BSParameter), &dwactbytes, NULL);
   WriteFile(hfile, &m_LogPen   , sizeof(LOGPEN)     , &dwactbytes, NULL);
   WriteFile(hfile, m_pArray,     GetArraySize()     , &dwactbytes, NULL);

   if (m_pszDescription) size = strlen(m_pszDescription);
   else                  size = 0;
   WriteFile(hfile, &size, sizeof(int), &dwactbytes, NULL);
   if (size) WriteFile(hfile, m_pszDescription, static_cast<DWORD>(size), &dwactbytes, NULL);

   if (m_pszUnitx) size = strlen(m_pszUnitx);
   else            size = 0;
   WriteFile(hfile, &size, sizeof(int), &dwactbytes, NULL);
   if (size) WriteFile(hfile, m_pszUnitx, static_cast<DWORD>(size), &dwactbytes, NULL);

   if (m_pszUnity) size = strlen(m_pszUnity);
   else            size = 0;
   WriteFile(hfile, &size, sizeof(int), &dwactbytes, NULL);
   if (size) WriteFile(hfile, m_pszUnity, static_cast<DWORD>(size), &dwactbytes, NULL);
   if (m_Parameter.locus)
   {
      if (m_pszUnitLocus) size = strlen(m_pszUnitLocus);
      else            size = 0;
      WriteFile(hfile, &size, sizeof(int), &dwactbytes, NULL);
      if (size) WriteFile(hfile, m_pszUnitLocus, static_cast<DWORD>(size), &dwactbytes, NULL);
      m_Parameter.format = CF_SINGLE_SHORT;
   }
   switch (m_Parameter.format)
   {
      case CF_SINGLE_DOUBLE: case CF_SINGLE_FLOAT: case CF_SINGLE_SHORT:
      {
         WriteFile(hfile, &m_dOrg ,   sizeof(double), &dwactbytes, NULL);
         double dStep = m_dStep;
         switch (GetXDivision())
         {
            case CCURVE_LOGARITHMIC: dStep = log10(m_dStep); break;
            case CCURVE_FREQUENCY:   dStep = log(m_dStep) / log(2.0); break;
         }
         WriteFile(hfile, &dStep,   sizeof(double), &dwactbytes, NULL);
      }break;
      default: break;
   }
   m_Parameter.format = nFormat;
}

bool CCurve::Read(HANDLE hfile)
{
   DWORD dwactbytes = 0;
   int   size, nNum;
   if (hfile==INVALID_HANDLE_VALUE)                                            return false;

   if (!ReadFile(hfile, &nNum,        sizeof(int),         &dwactbytes, NULL)) return false;
   if (!ReadFile(hfile, &m_dOffset,   sizeof(double),      &dwactbytes, NULL)) return false;
   if (!ReadFile(hfile, &m_Parameter, sizeof(BSParameter), &dwactbytes, NULL)) return false;
   if (!ReadFile(hfile, &m_LogPen   , sizeof(LOGPEN)     , &dwactbytes, NULL)) return false;
   if (nNum < 0)                                                               return false;
   int nFormat = m_Parameter.format;

   if (SetSize(nNum))
   {
      if (!ReadFile(hfile, m_pArray, GetArraySize(), &dwactbytes, NULL)) return false;
   }

   if (m_pszDescription) {delete[] m_pszDescription; m_pszDescription = NULL;}
   if (m_pszUnitx)       {delete[] m_pszUnitx;       m_pszUnitx       = NULL;}
   if (m_pszUnity)       {delete[] m_pszUnity;       m_pszUnity       = NULL;}

   if (!ReadFile(hfile, &size, sizeof(int), &dwactbytes, NULL))        return false;
   if (size)
   {
      m_pszDescription = new char[size+1];
      if (!ReadFile(hfile, m_pszDescription, size, &dwactbytes, NULL)) return false;
      m_pszDescription[size] = 0;
   }

   if (!ReadFile(hfile, &size, sizeof(int), &dwactbytes, NULL))        return false;
   if (size)
   {
      m_pszUnitx = new char[size+1];
      if (!ReadFile(hfile, m_pszUnitx, size, &dwactbytes, NULL))       return false;
      m_pszUnitx[size] = 0;
   }

   if (!ReadFile(hfile, &size, sizeof(int), &dwactbytes, NULL))        return false;
   if (size)
   {
      m_pszUnity = new char[size+1];
      if (!ReadFile(hfile, m_pszUnity, size, &dwactbytes, NULL))       return false;
      m_pszUnity[size] = 0;
   }
   if (m_Parameter.locus)
   {
      if (!ReadFile(hfile, &size, sizeof(int), &dwactbytes, NULL))     return false;
      if (size)
      {
         m_pszUnitLocus = new char[size+1];
         if (!ReadFile(hfile, m_pszUnitLocus, size, &dwactbytes, NULL)) return false;
         m_pszUnitLocus[size] = 0;
      }
      m_Parameter.format = CF_SINGLE_SHORT;
   }
   switch (m_Parameter.format)
   {
      case CF_SINGLE_DOUBLE: case CF_SINGLE_FLOAT: case CF_SINGLE_SHORT:
         if (!ReadFile(hfile, &m_dOrg , sizeof(double), &dwactbytes, NULL)) return false;
         if (!ReadFile(hfile, &m_dStep, sizeof(double), &dwactbytes, NULL)) return false;
         switch (GetXDivision())
         {
            case CCURVE_LOGARITHMIC: m_dStep = pow(10.0, m_dStep); break;
            case CCURVE_FREQUENCY:   m_dStep = pow( 2.0, m_dStep); break;
         }
         break;
      default: break;
   }
   m_Parameter.format = nFormat;
   return true;
}

void CCurve::GetChecksum(CFileHeader *pFH)
{
   auto size = strlen("");
   if (!pFH) return;
   pFH->CalcChecksum(&m_nNum,      sizeof(int));
   pFH->CalcChecksum(&m_dOffset,   sizeof(double));
   pFH->CalcChecksum(&m_Parameter, sizeof(BSParameter));
   pFH->CalcChecksum(&m_LogPen   , sizeof(LOGPEN));
   if (m_pArray) pFH->CalcChecksum(m_pArray, GetArraySize());

   if (m_pszDescription) size = strlen(m_pszDescription);
   else                  size = 0;
   pFH->CalcChecksum(&size,            sizeof(int));
   if (m_pszDescription) pFH->CalcChecksum(m_pszDescription, static_cast<long>(size));

   if (m_pszUnitx) size = strlen(m_pszUnitx);
   else            size = 0;
   pFH->CalcChecksum(&size,      sizeof(int));
   if (m_pszUnitx) pFH->CalcChecksum(m_pszUnitx, static_cast<long>(size));

   if (m_pszUnity) size = strlen(m_pszUnity);
   else            size = 0;
   pFH->CalcChecksum(&size,      sizeof(int));
   if (m_pszUnity) pFH->CalcChecksum(m_pszUnity, static_cast<long>(size));
}

bool CCurve::IsValid()
{
   return ((((m_nNum && m_pArray) || m_pszDescription)) ? true : false);
}


void CCurve::SetDescription(const char *s)
{
   if (m_pszDescription) delete[] m_pszDescription;
   m_pszDescription = NULL;
   if (s)
   {
      m_pszDescription = new char[strlen(s)+1];
      strcpy(m_pszDescription, s);
   }
}

void CCurve::SetXUnit(const char *s)
{
   if (m_pszUnitx) delete[] m_pszUnitx;
   m_pszUnitx = NULL;
   if (s)
   {
      m_pszUnitx = new char[strlen(s)+1];
      strcpy(m_pszUnitx, s);
   }
}

void CCurve::SetYUnit(const char *s)
{
   if (m_pszUnity) delete[] m_pszUnity;
   m_pszUnity = NULL;
   if (s)
   {
      m_pszUnity = new char[strlen(s)+1];
      strcpy(m_pszUnity, s);
   }
}

void CCurve::SetLocusUnit(const char *s)
{
   if (m_pszUnitLocus) delete[] m_pszUnitLocus;
   m_pszUnitLocus = NULL;
   if (s)
   {
      m_pszUnitLocus = new char[strlen(s)+1];
      strcpy(m_pszUnitLocus, s);
   }
}

double CCurve::GetXMax()
{
   int i;
   double dMax, dAct;
   bool bInterupted = IsInterupted();
   if (!m_pArray)  return 0;
   if (IsSorted()) return GetX_Value(m_nNum-1);
   dMax = GetX_Value(0);
   for (i=1; i<m_nNum; i++)
   {
      if (bInterupted)
      {
         dAct = GetY_Value(i);
         if (_isnan(dAct)) continue;
      }
      dAct = GetX_Value(i);
      if (dMax < dAct) dMax = dAct;
   }
   return dMax;
}

double CCurve::GetXMin()
{
   int i;
   double dMin, dAct;
   bool bInterupted = IsInterupted();
   if (!m_pArray)  return 0;
   if (IsSorted()) return GetX_Value(0);
   dMin = GetX_Value(0);
   for (i=1; i<m_nNum; i++)
   {
      if (bInterupted)
      {
         dAct = GetY_Value(i);
         if (_isnan(dAct)) continue;
      }
      dAct = GetX_Value(i);
      if (dMin > dAct) dMin = dAct;
   }
   return dMin;
}

double CCurve::GetYMax()
{
   int i;
   double dMax, dAct;
   if (!m_pArray)  return 0;
   bool bInterupted = IsInterupted();
   dMax = GetY_Value(0);
   for (i=1; i<m_nNum; i++)
   {
      dAct = GetY_Value(i);
      if (bInterupted && _isnan(dAct)) continue;
      if (dMax < dAct) dMax = dAct;
   }
   return dMax;
}

double CCurve::GetYMin()
{
   int i;
   double dMin, dAct;
   if (!m_pArray)  return 0;
   bool bInterupted = IsInterupted();
   dMin = GetY_Value(0);
   for (i=1; i<m_nNum; i++)
   {
      dAct = GetY_Value(i);
      if (bInterupted && _isnan(dAct)) continue;
      if (dMin > dAct) dMin = dAct;
   }
   return dMin;
}

void CCurve::CopyValues(CCurve *pc)
{
   if (pc && SetSize(pc->m_nNum))
   {
      if (pc->m_Parameter.format == m_Parameter.format)
      {
         memcpy(m_pArray, pc->m_pArray, GetArraySize());
      }
      else
      {
         CFloatPrecision _fp;
         for (int i=0; i<m_nNum; i++)
            SetCurveValue(i, pc->GetCurveValue(i));
      }
   }
}

void CCurve::TestXSorted()
{
   if (m_Parameter.locus != 0) return;
   if (m_Parameter.sorted) return;
   switch (m_Parameter.format)
   {
      case CF_SINGLE_DOUBLE: case CF_SINGLE_FLOAT: case CF_SINGLE_SHORT:
      {
         for (int i=0; i<m_nNum; i++)   // alle Werte duchgehen
         {
            if (_isnan(GetY_Value(i)))
            {
               m_Parameter.sorted     = false;
               m_Parameter.sortable   = false;
               m_Parameter.interupted = true;
               return;
            }
         }
         m_Parameter.sorted = true;
      }  break;
      default:
      {
         double dX1 = GetX_Value(0), dX2;
         if (_isnan(GetY_Value(0)))
         {
            m_Parameter.sorted     = false;
            m_Parameter.sortable   = false;
            m_Parameter.interupted = true;
            return;
         }
         int    i;
         for (i=1; i<m_nNum; i++)   // alle Werte duchgehen
         {
            if (_isnan(GetY_Value(i)))
            {
               m_Parameter.sorted     = false;
               m_Parameter.sortable   = false;
               m_Parameter.interupted = true;
               return;
            }
            dX2 = GetX_Value(i);
            if (dX1 > dX2) return;  // Ist der vorhergehende Wert größer
            dX1 = dX2;              // Shift
         }
         m_Parameter.sorted = true;
      }break;
   }
}
 
bool CCurve::AttachArray(void *pArray, int nCount, int nFormat, bool bTestXSorted)
{
   if (pArray   == NULL) return false;
   if (m_pArray != NULL) return false;
   switch (nFormat)
   {
      case CF_DOUBLE:
         if ((_isnan(((double*)pArray)[0]) != 0) ||
             (_isnan(((double*)pArray)[1]) != 0)) return false;
         break;
      case CF_FLOAT:
         if ((_isnan(((float *)pArray)[0]) != 0) || 
             (_isnan(((float *)pArray)[1]) != 0)) return false;
         break;
      case CF_SINGLE_DOUBLE:
         if  (_isnan(((double*)pArray)[0]) != 0)  return false;
         break;
      case CF_SINGLE_FLOAT:
         if  (_isnan(((float *)pArray)[0]) != 0)  return false;
         break;
      case CF_SINGLE_SHORT:
      case CF_SHORT:
      case CF_ARBITRARY: case CF_ARBITRARY_CONTR:
         break;
      default: return false;
   }
   SetFormat(nFormat);
   m_pArray           = pArray;
   m_nNum             = nCount;
   if (bTestXSorted) TestXSorted();
   return true;
}

void * CCurve::DetachArray()
{
   void *pArray = m_pArray;
   m_pArray = NULL;
   m_nNum   = 0;
   return pArray;
}

const void * CCurve::GetArrayPointer()
{
   return (const void*) m_pArray;
}

bool CCurve::SetFormat(int n)     
{
   if (m_pArray) return false; // Format kann nur im leeren Zustand gesetzt werden
   switch (n)
   {
      case CF_SINGLE_DOUBLE: case CF_SINGLE_FLOAT: case CF_SINGLE_SHORT:
         m_Parameter.sorted   = true;
         m_Parameter.sortable = false;
         break;
      case CF_DOUBLE: case CF_FLOAT: case CF_SHORT:
      case CF_ARBITRARY: case CF_ARBITRARY_CONTR:
         break;
      default:
         return false;
   }
   m_Parameter.format = n;
   return true;
}

short CCurve::DoubleToShort(double dVal)
{
   if (dVal > (double) 32766) return  32766;
   if (dVal < (double)-32767) return -32767;

   short sVal = (short) dVal;
   dVal -= (double) sVal;
   if (dVal >=  0.5) sVal++;
   if (dVal <= -0.5) sVal--;
   return sVal;
}

bool CCurve::ChangeFormat(int nFormat)
{
   int nOldFormat = GetFormat();
   int nChange = 0;
   switch (nFormat)
   {
      case CF_CHANGE_TO_SINGLE:
         if (nOldFormat >= CF_SINGLE_DOUBLE) // ist schon single format
            return false;
         else nFormat = nOldFormat+3;
         break;
      case CF_CHANGE_TO_DOUBLE:
         if (nOldFormat < CF_SINGLE_DOUBLE)  // ist schon double format
            return false;
         else nFormat = nOldFormat-3;
         break;
      case CF_CHANGE_TO_SHORT_TYPE:
         nChange++;
      case CF_CHANGE_TO_FLOAT_TYPE:
         nChange++;
      case CF_CHANGE_TO_DOUBLE_TYPE:
         if (nOldFormat >= CF_SINGLE_DOUBLE) // ist single format
            nFormat = nChange + 3;
         else                                // ist nicht single format
            nFormat = nChange;
         break;
   }

   if (nFormat != nOldFormat)
   {
      if (m_pArray != NULL)
      {
         int   nNum = m_nNum;
         void *pArr = DetachArray();
         if (SetFormat(nFormat) && SetSize(nNum))
         {
            CCurve curve(this);
            if (curve.AttachArray(pArr, nNum, nOldFormat))
            {
               CopyValues(&curve);
               return true;
            }
         }
         else
         {
            AttachArray(pArr, nNum, nOldFormat);
         }
      }
      else
      {
         SetFormat(nFormat);
         return true;
      }
   }
   return false;
}

bool CCurve::AreXvaluesEqualySpaced()
{
   switch (m_Parameter.format)
   {
      case CF_SINGLE_DOUBLE: case CF_SINGLE_FLOAT: case CF_SINGLE_SHORT:
         return true;
      default:         break;
   }

   if (!m_Parameter.sortable) return false; // unsortierbare nicht
   if (!m_Parameter.sorted)   return false; // unsortierte nicht
   if ( m_Parameter.locus )   return false; // Ortskurven nicht

   double dX1 = GetX_Value(0),
          dX2 = GetX_Value(1),
          dStep;
   int    i;

   switch (m_Parameter.xDivision)
   {
      case CCURVE_POLAR: case CCURVE_LINEAR:
      {
         CFloatPrecision _fp;
         dStep = dX2 - dX1;
         dX1   = dX2;               // Shift
         for (i=2; i<m_nNum; i++)   // alle Werte duchgehen
         {
            dX2 = GetX_Value(i);
            if (fabs((dX2 - dX1) - dStep) > 1e-13)
               return false;        // Ist der vorhergehende Wert größer
            dX1 = dX2;              // Shift
         }
      } break;
      case CCURVE_LOGARITHMIC: case CCURVE_FREQUENCY:
      {
         CFloatPrecision _fp;
         dStep = dX2 / dX1;
         dX1   = dX2;               // Shift
         for (i=2; i<m_nNum; i++)   // alle Werte duchgehen
         {
            dX2 = GetX_Value(i);
            dX1 = dX2 / dX1;
            if (fabs(dX1 - dStep) > 1e-13)
               return false;        // Ist der vorhergehende Wert größer
            dX1 = dX2;              // Shift
         }
      } break;
   }
   return true;
}

double CCurve::GetLocusValue(int n)
{
   if (m_Parameter.locus > 0)
   {
      int nOldDiv = m_Parameter.xDivision;
      int nFormat = m_Parameter.format;
      m_Parameter.xDivision = m_Parameter.locus;

      m_Parameter.format    = CF_SINGLE_SHORT;
      double dVal = GetX_Value(n);
      m_Parameter.xDivision = nOldDiv;
      m_Parameter.format    = nFormat;
      return dVal;
   }
   return 0;
}

bool CCurve::SetLocusValue(int n, double dValue)
{
   if (m_Parameter.locus > 0)
   {
      int nOldDiv = m_Parameter.xDivision;
      int nFormat = m_Parameter.format;
      m_Parameter.xDivision = m_Parameter.locus;
      m_Parameter.format    = CF_SINGLE_SHORT;
      bool bReturn = SetX_Value(n, dValue);
      m_Parameter.xDivision = nOldDiv;
      m_Parameter.format    = nFormat;
      return bReturn;
   }
   return false;
}

void CCurve::SetLocus(int nd)
{
   m_Parameter.locus = nd;
   if (m_Parameter.locus)
   {
      m_Parameter.sorted   = false;
      m_Parameter.sortable = false;
   }
}

void CCurve::RobAllParameter(CCurve& c)
{
   m_nNum           = c.m_nNum          , c.m_nNum           = 0;
   m_Parameter      = c.m_Parameter;
   m_pArray         = c.m_pArray        , c.m_pArray         = NULL;
   m_LogPen         = c.m_LogPen;
   m_pszUnitx       = c.m_pszUnitx      , c.m_pszUnitx       = NULL;
   m_pszUnity       = c.m_pszUnity      , c.m_pszUnity       = NULL;;
   m_pszDescription = c.m_pszDescription, c.m_pszDescription = NULL;
   m_dOffset        = c.m_dOffset;
   m_dOrg           = c.m_dOrg;
   m_dStep          = c.m_dStep;
}

bool CCurve::IsValue(int n)
{
   bool bIsVal = true;
   if ((n >= 0) && (n < m_nNum))
   {
      switch (m_Parameter.format)
      {
         case CF_DOUBLE:
            if (_isnan(((SCurve*)m_pArray)[n].y)) bIsVal = false;
            break;
         case CF_FLOAT:
            if (_isnan((double)((SCurveFloat*)m_pArray)[n].y)) bIsVal = false;
            break;
         case CF_SHORT:
            if (((SCurveShort*)m_pArray)[n].y == 0x8000) bIsVal = false;
            break;
         case CF_SINGLE_DOUBLE:
            if (_isnan(((double*)m_pArray)[n])) bIsVal = false;
            break;
         case CF_SINGLE_FLOAT:
            if (_isnan((double)((float*)m_pArray)[n])) bIsVal = false;
            break;;
         case CF_SINGLE_SHORT:
            if (((short*)m_pArray)[n] == 0x8000) bIsVal = false;
            break;
         case CF_ARBITRARY:
         case CF_ARBITRARY_CONTR:
         if (m_pGetCrvParamFkt)
         {
            SCurve sc;
            CF_GET_CURVE_PARAMS gcp = {CF_GET_YVALUE, n, this, &sc, m_pGetCrvParam};
            VERIFY(m_pGetCrvParamFkt(&gcp)!=0);
            if (_isnan(sc.y)) bIsVal = false;
         } break;
      }
   }
   return bIsVal;
}
