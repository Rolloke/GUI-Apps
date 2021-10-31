// CaraBoxViewDlg.h: Schnittstelle für die Klasse CCaraBoxViewDlg.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CARABOXVIEWDLG_H__AC4576EF_6F40_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_CARABOXVIEWDLG_H__AC4576EF_6F40_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define INCLUDE_ETS_HELP
#include "CEtsDlg.h"
#include "CCaraWalk.h"

class CCaraBoxViewDll
{
public:
	bool Create();
	static unsigned __stdcall Run(void*);

   volatile HWND   m_hWndDlg; // Threadsicherheit
   HANDLE m_hThread;
   ULONG  m_nThreadID;
   HWND   m_hWndParent;
};

class CCaraBoxViewDlg : public CEtsDialog
{
public:
	CCaraBoxViewDlg();
	CCaraBoxViewDlg(HINSTANCE, int, HWND);
	~CCaraBoxViewDlg();

protected:
   virtual bool   OnInitDialog(HWND hWnd);
   virtual int    OnCancel();
   virtual int    OnOk(WORD);
   virtual int    OnCommand(WORD, WORD, HWND);
   virtual int    OnMessage(UINT, WPARAM, LPARAM);
   virtual void   OnEndDialog(int);

   LRESULT        OnSize(SIZE);
   LRESULT        OnTimer(WPARAM nIDEvent);
	LRESULT        OnSetBoxData(char*, LRESULT*);
	void           UpdateAngle();
   void           Constructor();

public:
	LRESULT OnInitMenuPopup(HMENU, UINT, BOOL);
   CCaraBoxViewDll * m_pThreadConnect;

private:
   CCaraWalk         *m_pBoxView;
   int                m_nButtonTimer;
   CARAWALK_BoxPos    m_BoxPos;
   HWND               m_hwndGLView;
   SIZE               m_szMin;
   RECT               m_rcBoxView;
   SIZE               m_szButton;
   int                m_nPi, m_nTheta, m_nPsi, m_nTop;
   bool               m_bBoxOk;
   bool               m_bTop;
   
   static LRESULT CALLBACK SubClass(HWND, UINT, WPARAM, LPARAM);
   static LRESULT (CALLBACK *gm_pOldWF)(HWND, UINT, WPARAM, LPARAM);
};

#endif // !defined(AFX_CARABOXVIEWDLG_H__AC4576EF_6F40_11D3_B6EC_0000B458D891__INCLUDED_)
