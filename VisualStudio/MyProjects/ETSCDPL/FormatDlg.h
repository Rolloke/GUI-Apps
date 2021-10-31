#if !defined __FORMAT_DLG_H_INCLUDED_
#define __FORMAT_DLG_H_INCLUDED_

#define  STRICT
#include <WINDOWS.H>
#include "CEtsDlg.h"
#include "CEtsList.h"

struct FormatStruct
{
   bool bChecked;
   char szFormat[1];
};

class CFormatDlg: public CEtsDialog
{
public:
   CFormatDlg();
   CFormatDlg(HINSTANCE, int, HWND);
   ~CFormatDlg();
   void ReadFormats();

   static int FindString(const void *, const void *);

private:

   virtual bool   OnInitDialog(HWND hWnd);
   virtual int    OnOk(WORD);
   virtual void   OnEndDialog(int);
   virtual int    OnCommand(WORD, WORD, HWND);
   virtual int    OnMessage(UINT, WPARAM, LPARAM);               // verarbeitet Nachrichten an das Dialogfenster

   void Constructor();
	void SetListSize(int);
	void WriteFormats();
	void AddFormat(char*, bool bChecked=false);

   int  OnGetdispinfoFormatList(LV_DISPINFO*);
   int  OnBeginlabeleditFormatList(LV_DISPINFO*);
   int  OnEndlabeleditFormatList(LV_DISPINFO*);
   int  OnClickFormatList(NMHDR*);
   int  OnKeyDownFormatList(NMLVKEYDOWN*);
   LRESULT OnEditCtrlKeyUp(WPARAM, LPARAM);

	static void DeleteFormat(void*);
   static LRESULT CALLBACK EditCtrlWndProc(HWND, UINT, WPARAM, LPARAM);
   static long gm_lOldEditCtrlWndProc;

public:
   CEtsList    m_Formats;
   HICON       m_hIcon;
   int         m_nCurrentItem;
};

#endif //__FORMAT_DLG_H_INCLUDED_