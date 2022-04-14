#pragma once
#include <vector>

#include "ResizeDlgItems.h"
#include "ToolTips.h"

struct ETSDIV_DIV;
class CCopyCSVDlg
{
public:
   CCopyCSVDlg();
   ~CCopyCSVDlg();

   int      DoModal(HWND hWndParent);
   HWND        Create(HWND hWndParent);                                       // Erzeugt einen nichtmodalen Dialog
protected:
   bool     OnInitDialog(HWND hWnd);
   void     OnEndDialog(int nResult);
   int      OnOk(WORD nNotify);
   int      OnCommand(WORD nID, WORD nNotifyCode, HWND hwndControl);
   void     PrepareGraphDC(HWND hWnd, HDC hdc);
   void     OnDrawGraph(HWND hWnd, HDC hdc);
   void     OnLButtonDownGraph(HWND hWnd, UINT nFlags, POINT pt);
   void     OnRButtonDownGraph(HWND hWnd, UINT nFlags, POINT pt);
   void     OnRButtonUpGraph(HWND hWnd, UINT nFlags, POINT pt);
   void     OnMouseMoveGraph(HWND hWnd, UINT nFlags, POINT pt);
   BOOL     OnMessage(UINT, WPARAM, LPARAM);
   BOOL     OnSize(UINT, short, short);

   void     ChangeModalStyles(bool);
   void     FormatMessage(const char *szFmt, ...);

private:
   static BOOL CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
   static LRESULT CALLBACK DrawGraphProc(HWND, UINT, WPARAM, LPARAM);
   static HWND GetTopLevelParent(HWND);
   static void GetWindowThreadID(HWND hwnd)
   {
      gm_nWindowThreadProcessID = ::GetWindowThreadProcessId(hwnd, NULL);
   }
   static LRESULT CALLBACK MsgFilterProc(int code, WPARAM wParam,  LPARAM lParam);
   static BOOL CALLBACK    MsgFilterEnum(HWND, LPARAM);
public:
   static void UnHookMsgFilterProc()
   {
      if (gm_hDll_Hook)
      {
         UnhookWindowsHookEx(gm_hDll_Hook);
      }
   }
   static int  Round(double dval);
   static bool CalculateLinDivision(ETSDIV_DIV *pdiv, double& dStep, double& dStart);

public:
   BSTR     m_pSelected;
   HWND      m_hWnd;          // Handle des Dialogfensters
   static int   gm_nModal;
private:
   HWND      m_hWndParent;    // Handle des Elterfensters
   UINT      m_nFlags;
   std::vector<POINT> m_ptCurve;
   RECT      m_rcZooming;
   RECT      m_rcZoom;
   int       m_nZoom;
   int       m_nZooming;
   int       m_nPoints;
   double    m_dFaktor;
   double    m_dOffset;
   double    m_dStart;
   double    m_dStep;
   int       m_nSteps;
   int       m_nStartX;
   int       m_nStepX;
   int       m_nStepsX;
   bool      m_bModal;        // Anzeiger für Modal/Nichtmodal
   VARIANT   m_Selected;
   CResizeDlgItems mResize;
   CToolTips mToolTips;
   static WNDPROC gm_oldDrawGraphProc;
   static HHOOK gm_hDll_Hook;
   static int   gm_nWindowThreadProcessID;
   static HWND  gm_hWndTopLevelParent;
   static int   gm_nCallCount;
};
