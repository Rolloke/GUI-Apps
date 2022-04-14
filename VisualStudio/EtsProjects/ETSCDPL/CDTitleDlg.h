#if !defined __CDTITLE_DLG_H_INCLUDED_
#include "CDPlayerCtrl.h"	// Hinzugefügt von der Klassenansicht
#define __CDTITLE_DLG_H_INCLUDED_

#define  STRICT
#include <WINDOWS.H>
#include <shellapi.h>
#include "CEtsDlg.h"
#include "CEtsList.h"

union MMTime;

class CDTitleDlg: public CEtsDialog
{
public:
   CDTitleDlg();
   CDTitleDlg(HINSTANCE, int, HWND);
   ~CDTitleDlg();

	const char* GetInterpret();
   void  SetInterpret(char*);
	void  AddTitle(char*);
	void  FreeTitles();
   void  FreeInterpret();
	bool  ReadMediaInfo(char *pszPath=NULL);
   bool  WriteMediaInfo(char *pszPath=NULL);
	int   GetCDInfoPath(char*, int);
	void  SetCDInfoPath(char*);
	void  SetTime(int, MMTime);
	MMTime GetTime(int);

 	LRESULT OnDropFiles(HDROP, HWND hwndParent=NULL);

private:
   virtual bool   OnInitDialog(HWND hWnd);
   virtual int    OnOk(WORD);
   virtual void   OnEndDialog(int);
   virtual int    OnCommand(WORD, WORD, HWND);
   virtual int    OnMessage(UINT, WPARAM, LPARAM);               // verarbeitet Nachrichten an das Dialogfenster

   void Constructor();

   int  OnGetdispinfoCdTitleList(LV_DISPINFO*);
   int  OnBeginlabeleditCdTitleList(LV_DISPINFO*);
   int  OnEndlabeleditCdTitleList(LV_DISPINFO*);
   int  OnKeyDownFormatList(NMLVKEYDOWN*);
   int  OnDblClkCdTitleList(NMITEMACTIVATE*);
   LRESULT OnEditCtrlKeyUp(WPARAM, LPARAM);

   static void DeleteTitle(void *);
	static char *ReadString(HANDLE);
   static bool WriteString(HANDLE, char *);
   static LRESULT CALLBACK EditCtrlWndProc(HWND, UINT, WPARAM, LPARAM);

	char *      m_pszInterpret;
   MMTime     *m_pTimes;
   static long gm_lOldEditCtrlWndProc;
   static char gm_szFormatCDTime[64];

public:
	void UpdateContent();
	void FindCommonPath();
	void InitInterpret();
   char        m_szMediaIdent[32];
   CEtsList    m_Titles;
   int         m_nTracks;
   bool        m_bEditable;
   HICON       m_hIcon;
   int         m_nCurrentItem;
   int         m_nIDList;
   int         m_nIDInterpret;
};

#endif //__CDTITLE_DLG_H_INCLUDED_