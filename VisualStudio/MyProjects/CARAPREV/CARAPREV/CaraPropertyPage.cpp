// CaraPropertyPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Resource.h"
#include "CaraPropertyPage.h"
#include "CaraViewProperties.h"

#include "afx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CCaraPropertyPage 
/*
extern void AFXAPI AfxTextFloatFormat(CDataExchange* pDX, int nIDC,
	void* pData, double value, int nSizeGcvt);
*/
IMPLEMENT_DYNCREATE(CCaraPropertyPage, CPropertyPage)

CCaraPropertyPage::CCaraPropertyPage()
{
}

CCaraPropertyPage::CCaraPropertyPage(UINT nIDD) : CPropertyPage(nIDD)
{
}

CCaraPropertyPage::~CCaraPropertyPage()
{
}

void CCaraPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCaraPropertyPage)
		// HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCaraPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCaraPropertyPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CCaraPropertyPage 

BOOL CCaraPropertyPage::OnKillActive() 
{
   CWnd *pParent = GetParent();
   CWnd *pFocus  = GetFocus();
   CWnd *pApply;
   bool  bSwitch  = false;
   bool  bRequest = true;
   if (pParent)
   {
      if (pFocus && (pFocus->GetParent() == pParent)) bSwitch = true, bRequest = false;
      pApply  = pParent->GetDlgItem(ID_APPLY_NOW);
      CPoint point;
      if (GetCursorPos((POINT*)&point))
      {
         ASSERT_KINDOF(CPropertySheet, pParent);
         pParent->ScreenToClient((POINT*)&point);
         if (point.y < 30) bSwitch = true;
      }
   }

   if ((pParent->GetDlgItem(IDOK)== pFocus)||
       (pApply                   == pFocus)||bSwitch)
   {
      return RequestSaveChanges(bRequest);
   }
   return 0;
}

BOOL CCaraPropertyPage::RequestSaveChanges(bool bRequest)
{
   CWnd *pParent = GetParent();
   if (pParent)
   {
      CWnd *pApply  = pParent->GetDlgItem(ID_APPLY_NOW);
      if (pApply && pApply->IsWindowEnabled())
      {
         if (UpdateData(true))
         {
            if (bRequest)
            {
               if (AfxMessageBox(IDS_SAVE_CHANGES, MB_YESNO|MB_ICONQUESTION)==IDYES)
               {
                  OnApply();
                  return CHANGE_PAGE|CHANGES_SAVED;
               }
               else
               {
                  m_bValuesChanged = true;
                  SetModified(false);
                  return CHANGE_PAGE|CHANGES_REJECTED;
               }
            }
            return CHANGE_PAGE;
         }
         else
         {
            UpdateData(false);
            return 0;
         }
      }
      else return CHANGE_PAGE;
   }
   return 0;
}

BOOL CCaraPropertyPage::OnApply() 
{
   CWnd *pParent  = GetParent();
   if (pParent && pParent->IsKindOf(RUNTIME_CLASS(CCaraViewProperties)))
   {
      if (((CCaraViewProperties*)pParent)->m_pView)
      {
         ((CCaraViewProperties*)pParent)->m_pView->InvalidateRect(NULL);
         ((CCaraViewProperties*)pParent)->m_pView->GetDocument()->SetModifiedFlag(true);
      }
   }
	return CPropertyPage::OnApply();
}

COLORREF CCaraPropertyPage::GetPreviewBkGndColor()
{
   CWnd *pParent  = GetParent();
   if (pParent && pParent->IsKindOf(RUNTIME_CLASS(CCaraViewProperties)))
   {
      return ((CCaraViewProperties*)pParent)->m_BkColor;
   }
   return RGB(255,255,255);
}
/*
BOOL CCaraPropertyPage::DDX_TextVar(CDataExchange *pDX, int nID, int nAcc, double &dVar, bool bThrow)
{
	BOOL bOK = FALSE;       // assume failure
	TRY
	{
      if (!pDX->m_bSaveAndValidate && _isnan(dVar))
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
*/

BOOL CCaraPropertyPage::UpdateData(BOOL bSaveAndValidate)
{
   return CPropertyPage::UpdateData(bSaveAndValidate);
}
