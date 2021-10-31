/*******************************************************************************
                                 CEtsPropertySheet
  Zweck:
   Basisklasse zur Erzeugung und Steuerung von PropertySheets
  Header:            Bibliotheken:        DLL´s:
                                          ETSHELP.dll
  Copyright:
   (C) 1999 ELAC Technische Software GmbH
  VERSION 
   1.00 Stand 10.09.1999
                                                   programmed by Rolf Ehlers
*******************************************************************************/

#if !defined __CETS_PROPERTY_SHEET_H_INCLUDED_
#define __CETS_PROPERTY_SHEET_H_INCLUDED_

#ifndef STRICT
  #define STRICT
#endif
#include <WINDOWS.H>

class CEtsPropertyPage;

#define PSN_VALIDATE            (PSN_FIRST-1)
#define PSN_CANCEL              (PSN_FIRST-3)

class CEtsPropertySheet
{
   friend class CEtsPropertyPage;
public:
   CEtsPropertySheet();
   virtual ~CEtsPropertySheet();
   // PropertySheet properties
	void SetCaption(UINT);
	void SetCaption(char*);
	void SetNoOfPages(int);
	void SetInstanceHandle(HINSTANCE);
	void SetWizard(bool);
	void SetHelp(bool);
	void SetStartPage(int);
	void SetStartPage(LPCTSTR);
	void ExtendTitle(bool);
	void SetApplyButton(bool);
	void SetFinishBtn(bool);
	void SetWizardContextHelp(bool);
	void SetWatermarkBmp(LPCSTR);
	void SetHeaderBmp(LPCSTR);
   // PropertyPage properties
	bool SetPageDlgProc(UINT, DLGPROC, LPARAM);
	bool SetPropertyPage(UINT, CEtsPropertyPage*);
	bool SetPageResource(UINT,LPCDLGTEMPLATE pResource);
	bool SetPageTemplate(UINT,LPCSTR);
	bool SetPageIcon(UINT,LPCSTR);
	bool SetPageIcon(UINT,HICON);
	bool SetPageTitle(UINT, LPCSTR);
	bool SetPageHelp(UINT, bool);
   // Call the PropertySheet
	HWND Create(HWND);
	int  DoModal(HWND);

	bool IsModified();
	void SetModified(bool);
	HWND GetWndOkBtn();
   HWND GetWndNextBtn();
   HWND GetWndBackBtn();
   HWND GetWndFinishBtn();
   HWND GetWndHelpBtn();
   HWND GetWndTabControl();

protected:
	virtual bool OnInitSheet(HWND hWnd);
   virtual int  OnMessage(UINT, WPARAM, LPARAM);
	void FreePages();
	void FreeCaption();
	CEtsPropertyPage * GetActivePage();

   static int CALLBACK PropertySheetProc(HWND,UINT,LPARAM);
   static LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
   static BOOL CALLBACK    NotifyChildPages(HWND, LPARAM);
   
   PROPSHEETHEADER m_PS_Header;
   HWND m_hWnd;
   bool m_bCaptionAllocated;

private:
   static LONG gm_nStdWndProc;
};


#endif //__CETS_PROPERTY_SHEET_H_INCLUDED_