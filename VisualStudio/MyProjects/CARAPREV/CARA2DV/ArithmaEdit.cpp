// ArithmaEdit.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CARA2DV.h"
#include "Aritmetic.h"
#include "ArithmaEdit.h"
#include "..\TreeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"

/////////////////////////////////////////////////////////////////////////////
// CArithmaEdit
CArithmaEdit::CArithmaEdit()
{
}

CArithmaEdit::~CArithmaEdit()
{
}


BEGIN_MESSAGE_MAP(CArithmaEdit, CEdit)
	//{{AFX_MSG_MAP(CArithmaEdit)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CArithmaEdit 
#define FIRST_LEVEL        0
#define UNARY_FUNCTION     1
#define BINARY_FUNCTION    2
#define OPERATORS          3
#define CONSTANTS          4

void CArithmaEdit::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   BEGIN("CArithmaEdit::OnLButtonDblClk");
   CTreeDlg Tdlg;
   char text[64];
   Tdlg.m_strWndTitle.LoadString(IDS_INSERT);
   ZeroMemory(&Tdlg.m_TvItem, sizeof(Tdlg.m_TvItem));
   Tdlg.m_pCallBack         = SetArithmaStrings;
   Tdlg.m_TvItem.mask       = TVIF_PARAM|TVIF_TEXT;
   Tdlg.m_TvItem.pszText    = text;
   Tdlg.m_TvItem.cchTextMax = 63;

   if ((Tdlg.DoModal() == IDOK) && (Tdlg.m_TvItem.lParam != FIRST_LEVEL))
   {
      if (Tdlg.m_TvItem.lParam == OPERATORS)
         text[1] = 0;
      SendMessage(EM_REPLACESEL, 1, (LPARAM)text);
      int  nStart, nEnd;
      SendMessage(EM_GETSEL, (WPARAM)&nStart, (LPARAM)&nEnd);
      switch (Tdlg.m_TvItem.lParam)
      {
         case UNARY_FUNCTION:  nEnd -= 1; break;
         case BINARY_FUNCTION: nEnd -= 2; break;
      }
      SendMessage(EM_SETSEL, (WPARAM)nEnd, (LPARAM)nEnd);
   }
}

int CArithmaEdit::SetArithmaStrings(CTreeDlg *pDlg, int nReason)
{
   if (nReason == ONOK)
   {
      return (pDlg->m_TvItem.hItem != NULL) ? 1 : 0;
   }
   if (nReason != ONINITDIALOG) return 0;
   BEGIN("SetArithmaStrings");
   TV_INSERTSTRUCT tv_is;
   HTREEITEM       hUnaryFkt;
   HTREEITEM       hBinaryFkt;
   HTREEITEM       hConstants;
   HTREEITEM       hOperator;
   int             i;
   CString         strUnaryFkt;
   CString         strBinaryFkt;
   CString         strOperators;
   CString         strConstants;
   CString         strRest;
   CString         strExponent;
   CString         strLogAnd;
   CString         strLogOr;
   CString         strLogNeg;
   CString         strNeg;
   CString         strLogGreaterThan;
   CString         strLogLessThan;
   CString         strLogEqual;

   strUnaryFkt.LoadString(IDS_ARE_UNARY_FKT);
   strBinaryFkt.LoadString(IDS_ARE_BINARY_FKT);
   strOperators.LoadString(IDS_ARE_OPERATOR);
   strConstants.LoadString(IDS_ARE_CONSTANTS);
   strRest.LoadString(IDS_ARE_REST);
   strExponent.LoadString(IDS_ARE_EXPONENT);
   strLogAnd.LoadString(IDS_ARE_LOG_AND);
   strLogOr.LoadString(IDS_ARE_LOG_OR);
   strLogNeg.LoadString(IDS_ARE_LOG_NEG);
   strNeg.LoadString(IDS_ARE_NEG);
   strLogGreaterThan.LoadString(IDS_ARE_LOG_GREATER_THAN);
   strLogLessThan.LoadString(IDS_ARE_LOG_LESS_THAN);
   strLogEqual.LoadString(IDS_ARE_LOG_EQUAL);


   if (!pDlg->m_TreeCtrl.DeleteAllItems()) return 0;

   memset(&tv_is, 0, sizeof(TV_INSERTSTRUCT));
   tv_is.hParent      = TVI_ROOT;
   tv_is.hInsertAfter = TVI_LAST;
   tv_is.item.mask    = TVIF_PARAM|TVIF_TEXT;
   tv_is.item.lParam  = FIRST_LEVEL;
   
   tv_is.item.pszText = (char*)LPCTSTR(strUnaryFkt);
   hUnaryFkt  = pDlg->m_TreeCtrl.InsertItem(&tv_is);

   tv_is.item.pszText = (char*)LPCTSTR(strBinaryFkt);
   hBinaryFkt = pDlg->m_TreeCtrl.InsertItem(&tv_is);

   tv_is.item.pszText = (char*)LPCTSTR(strOperators);
   hOperator  = pDlg->m_TreeCtrl.InsertItem(&tv_is);

   tv_is.item.pszText = (char*)LPCTSTR(strConstants);
   hConstants         = pDlg->m_TreeCtrl.InsertItem(&tv_is);


   tv_is.hParent      = hUnaryFkt;
   tv_is.item.lParam  = UNARY_FUNCTION;
   char text[64];
   tv_is.item.pszText = text;
   for (i=0; CArithmetic::gm_UnaryFkt[i] != NULL; i++)
   {
      wsprintf(text, "%s()", CArithmetic::gm_UnaryFkt[i]);
      REPORT(text);
      pDlg->m_TreeCtrl.InsertItem(&tv_is);
   }

   tv_is.hParent      = hBinaryFkt;
   tv_is.item.lParam  = BINARY_FUNCTION;
   tv_is.item.pszText = text;
   for (i=0; CArithmetic::gm_BinaryFkt[i] != NULL; i++)
   {
      wsprintf(text, "%s(,)", CArithmetic::gm_BinaryFkt[i]);
      REPORT(text);
      pDlg->m_TreeCtrl.InsertItem(&tv_is);
   }

   tv_is.hParent      = hOperator;
   tv_is.item.lParam  = OPERATORS;
   tv_is.item.pszText = text;

   char *Operators[] = {"+", "-", "*", "/",
                        (char*)LPCTSTR(strRest),
                        (char*)LPCTSTR(strExponent),
                        (char*)LPCTSTR(strLogAnd),
                        (char*)LPCTSTR(strLogOr),
                        (char*)LPCTSTR(strLogGreaterThan),
                        (char*)LPCTSTR(strLogLessThan),
                        (char*)LPCTSTR(strLogEqual),
                        (char*)LPCTSTR(strLogNeg),
                        (char*)LPCTSTR(strNeg), NULL};

   for (i=0; Operators[i] != NULL; i++)
   {
      tv_is.item.pszText = (char*) Operators[i];
      REPORT(tv_is.item.pszText);
      pDlg->m_TreeCtrl.InsertItem(&tv_is);
   }

   tv_is.hParent      = hConstants;
   tv_is.item.lParam  = CONSTANTS;
   for (i=0; CArithmetic::gm_constants[i].name != NULL; i++)
   {
      tv_is.item.pszText = (char*)CArithmetic::gm_constants[i].name;
      REPORT(tv_is.item.pszText);
      pDlg->m_TreeCtrl.InsertItem(&tv_is);
   }
   return 1;
}

void CArithmaEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   switch(nChar)
   {
      case VK_F2:
      {
         UINT nFlags=0;
         CPoint point(0,0);
         OnLButtonDblClk(nFlags, point);
         return;
      }
   }
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}
