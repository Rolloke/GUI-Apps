// EditDoubleCtrl.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CARA2DV.h"
#include "CurveDialog.h"
#include "EditDoubleCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditDoubleCtrl

CEditDoubleCtrl::CEditDoubleCtrl()
{
//   m_cValue = Complex();
}

CEditDoubleCtrl::~CEditDoubleCtrl()
{
}


BEGIN_MESSAGE_MAP(CEditDoubleCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CEditDoubleCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CEditDoubleCtrl 

void CEditDoubleCtrl::DoDataExchange(CDataExchange* pDX) 
{
   CEdit *pEdit = GetEditControl();
   if (pEdit)
   {
      if (pDX->m_bSaveAndValidate)
      {
         CFloatPrecision fp;
         char szList[4]=",", szDecimal[4]=".";
         GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLIST, szList, 4);
         GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szDecimal, 4);
         char text[MAX_PATH];
         int nlen = pEdit->GetWindowText(text, MAX_PATH);
         char *pBraceOpen = strstr(text, "(");
         if (pBraceOpen)                                          // Komplex ?
         {
            char *pComma = strstr(pBraceOpen, &szList[0]);
            if (!pComma)                                          // doch nicht ?
            {
               CString str;
               str.Format(IDE_PARSE_COMPLEX_NUMBER, szList[0], szDecimal[0], szList[0], szDecimal[0]);
			      AfxMessageBox(str);
			      pDX->Fail();            // throws exception
            }
            char *pBraceClose = strstr(pComma, ")");
            if (!pBraceClose)                                     // doch nicht ?
            {
               CString str;
               str.Format(IDE_PARSE_COMPLEX_NUMBER, szList[0], szDecimal[0], szList[0], szDecimal[0]);
			      AfxMessageBox(str);
			      pDX->Fail();            // throws exception
            }
            pBraceClose[0] = 0;
            m_cValue.imagteil = atof(&pComma[1]);
	         if (m_cValue.imagteil == 0.0 && pComma[1] != '0')
            {
			      AfxMessageBox(AFX_IDP_PARSE_REAL);
			      pDX->Fail();            // throws exception
            }
            pComma[0] = 0;
            m_cValue.realteil = atof(&pBraceOpen[1]);
	         if (m_cValue.realteil == 0.0 && pBraceOpen[1] != '0')
            {
			      AfxMessageBox(AFX_IDP_PARSE_REAL);
			      pDX->Fail();            // throws exception
            }
         }
         else
         {
            DDX_Text(pDX, pEdit->GetDlgCtrlID(), m_cValue.realteil);
            m_cValue.imagteil = 0;
         }
      }
      else
      {
         if (m_cValue.imagteil != 0.0)
         {
            CString str;
            char szList[4];
            GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLIST, szList, 4);
            str.Format("(%g%c %g)", m_cValue.realteil, szList[0], m_cValue.imagteil);
         }
         else DDX_Text(pDX, pEdit->GetDlgCtrlID(), m_cValue.realteil);
      }
   }
	CListCtrl::DoDataExchange(pDX);
}

