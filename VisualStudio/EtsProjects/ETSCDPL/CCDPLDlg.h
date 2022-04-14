#if !defined __CDPL_DLG_H_INCLUDED_
#define __CDPL_DLG_H_INCLUDED_

#include "CEtsDlg.h"
#include "CDPlayerCtrl.h"
#include "CEtsList.h"
#include "CDTitleDlg.h"
#include "MixerDlg.h"

class CCDPLDlg;
struct CCDPLDlgStruct
{
   int       nTitles;
   char    **ppszTitles;
   CCDPLDlg *pThis;
};

class CCDPLDlg: public CEtsDialog
{
public:
   CCDPLDlg();
   CCDPLDlg(HINSTANCE, int, HWND);
   ~CCDPLDlg();

   CDTitleDlg        m_TitleDlg;
   CMixerDlg         m_MixerDlg;
   CCDPlayerCtrl     m_CDPlayer;
   CCDPLDlgStruct   *m_pCDDlgStruct;
   
private:
   void Constructor();
   virtual bool   OnInitDialog(HWND hWnd);
   virtual int    OnOk(WORD);
   virtual void   OnEndDialog(int);
public:
   virtual int    OnMessage(UINT, WPARAM, LPARAM);               // verarbeitet Nachrichten an das Dialogfenster
   virtual int    OnCommand(WORD, WORD, HWND);

	static void ModifyStyle(HWND, DWORD, DWORD);

private:
   int     OnDrawItem(DRAWITEMSTRUCT *);
   void    OnTimer(UINT);
   LRESULT OnUpdateCdCtrls(WPARAM, LPARAM);
   void    OnOptionsCdDrive();

   UINT    SetTimerSave(UINT, UINT, void (CALLBACK *p)(HWND,UINT,UINT,DWORD));
   void    KillTimerSave(UINT);
   void    SetEjectBtnImage();
   void    AdaptWindowSize();

   static LRESULT CALLBACK SubClassProc(HWND, UINT, WPARAM, LPARAM);
   static BOOL CALLBACK    EnumChildWndFunc(HWND, LPARAM);

   CEtsList m_Timers;
   short    m_nDisplayDivX;
   bool     m_bHitDisplayDivX;
	bool     m_bUseMsgBoxForError;
	int      m_nRoundRectRadius;
   char     m_szCDWndText[MAX_PATH];
   WORD     m_wOldEjectIconID;
   WORD     m_wAlternateEjectIcon;

   HMIXER   m_hMixer;
   int      m_nUpdateControl;

   static char gm_szFormatCDTime[64];
   static char gm_szFormatCDTitle[64];

public:
	void InitTitles();
   static char gm_szImportPath[MAX_PATH];
   static bool gm_bOwnerDraw;
};

#endif //__CDPL_DLG_H_INCLUDED_