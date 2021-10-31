/*******************************************************************************
                                 CEtsDialog
  Zweck:
   Basisklasse zur Erzeugung und Steuerung von Dialogfeldern
  Header:            Bibliotheken:        DLL´s:
                                          ETSHELP.dll
  Copyright:
   (C) 1999 ELAC Technische Software GmbH
  VERSION 
   1.00 Stand 10.09.1999
                                                   programmed by Rolf Ehlers
*******************************************************************************/

#if !defined __CETS_FILE_DLG_H_INCLUDED_
#define __CETS_FILE_DLG_H_INCLUDED_

#include "CEtsDlg.h"


class CEtsFileDlg : public CEtsDialog
{
public:
	void SetTemplateName(UINT);
   CEtsFileDlg();
   CEtsFileDlg(HINSTANCE hInstance, int nID, HWND hWndParent);  // Konstruktor 2
   virtual ~CEtsFileDlg();
   void Constructor();

   virtual bool OnInitDialog(HWND hWnd);                       // zur initialisierung der Dialogfelder

   // Aufruf des Dialogs
   BOOL GetSaveFileName();
	BOOL GetOpenFileName();
   // Parameter setzen erfragen
	void ModifyFlags(DWORD dwSet, DWORD dwRemove);
	bool SetMaxPathLength(int);
	bool SetFilter(int nID);
	bool SetFilter(char *, int, bool bAlloc=true);
	void SetInitialDir(LPCTSTR);
	void SetDefaultExt(LPCTSTR);
	void SetFileTitle(LPCTSTR pszFileTitle);
	void SetFile(LPCTSTR pszFile);
	bool SetTitle(int nID);
   const char * GetFilePathName() {return m_OF.lpstrFile;};
   char * GetPathName();
   const char * GetFileTitle();
   int  GetOffset()       {return m_OF.nFileOffset;};
   int  GetFilterIndex()  {return m_OF.nFilterIndex;};
   int  GetMaxFileTitle() {return m_OF.nMaxFileTitle;};
   int  GetMaxCustFilter(){return m_OF.nMaxCustFilter;};
   int  GetFileExtension(){return m_OF.nFileExtension;};

	static void CopyValidChars(LPTSTR, LPCTSTR, int);
	static bool IsValidChar(int);
	static LRESULT CALLBACK EditFileNameProc(HWND, UINT, WPARAM, LPARAM);

protected:
   virtual UINT OnNotify(OFNOTIFY*);

   OPENFILENAME m_OF;

private:
   static UINT CALLBACK OFNHookProc(HWND, UINT, WPARAM, LPARAM);
};


#endif //__CETS_FILE_DLG_H_INCLUDED_