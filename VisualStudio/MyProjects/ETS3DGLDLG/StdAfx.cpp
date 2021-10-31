// stdafx.cpp : Quelltextdatei, die nur die Standard-Includes einbindet
//	ETS3DGLDLG.pch ist die vorcompilierte Header-Datei
//	stdafx.obj enthält die vorcompilierte Typinformation

#include "stdafx.h"

extern void AFXAPI AfxTextFloatFormat(CDataExchange* pDX, int nIDC,
	void* pData, double value, int nSizeGcvt);

BOOL DDX_TextVar(CDataExchange *pDX, int nID, int nAcc, float &fVar, bool bThrow)
{
   double dVar = (double)fVar;
   BOOL bReturn = DDX_TextVar(pDX, nID, nAcc, dVar, bThrow);
   fVar = (float)dVar;
   return bReturn;
}

BOOL DDX_TextVar(CDataExchange *pDX, int nID, int nAcc, double &dVar, bool bThrow)
{
	BOOL bOK = FALSE;       // assume failure
	TRY
	{
      if (pDX->m_bSaveAndValidate)
      {
         static const int nTextSize = 64;
         char text[nTextSize];
         ::GetDlgItemText(pDX->m_pDlgWnd->GetSafeHwnd(), nID, text, nTextSize);
         int i, c;

         char szDecimal[4];
         if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szDecimal, 4)==0)
            szDecimal[0] = ',';

         for (i=0; ((c=text[i])!=0) && (i<nTextSize); i++)
         {
            if (c == (int)'.') {text[i] = szDecimal[0]; continue;}// Punkt ist immer erlaubt
            if (!isdigit(c) &&                                 // Nummern
                (c != (int)szDecimal[0]) &&                    // Dezimalzeichen
                (c != (int)'-') && (c != (int)'+') &&          // Vorzeichen
                (c != (int)'e') &&                             // Exponent
                (c != (int)' '))                               // Leerzeichen
            {
               pDX->m_bSaveAndValidate = false;
               ::AfxTextFloatFormat(pDX, nID, &dVar, dVar, nAcc);
               pDX->m_bSaveAndValidate = true;
               pDX->Fail();
            }
         }
         dVar = atof(text);
      }
      else if (!pDX->m_bSaveAndValidate && _isnan(dVar))
      {
         CString str("--");
         DDX_Text(pDX, nID, str);
      }
      else
      {
         if (nAcc == FLT_DIG) nAcc++;
         ::AfxTextFloatFormat(pDX, nID, &dVar, dVar, nAcc);
      }
		bOK = TRUE;         // it worked
	}
	CATCH(CUserException, e)
	{
		// validation failed - user already alerted, fall through
      if (bThrow) pDX->Fail();
		ASSERT(!bOK);
		// Note: DELETE_EXCEPTION_(e) not required
	}
	AND_CATCH_ALL(e)
	{
		// validation failed due to OOM or other resource failure
      if (bThrow) pDX->Fail();
		e->ReportError(MB_ICONEXCLAMATION, AFX_IDP_INTERNAL_FAILURE);
		ASSERT(!bOK);
//		DELETE_EXCEPTION(e);
	}
	END_CATCH_ALL

	return bOK;
}


