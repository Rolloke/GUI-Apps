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

#if !defined __CDIALOG_H_INCLUDED_
#define __CDIALOG_H_INCLUDED_

#ifndef STRICT
  #define STRICT
#endif
#include <WINDOWS.H>

#define ID_STRING_OFFSET  61000
#define NUM_CUSTOMCOLORS  16

#define NOKILLFOCUSNOTIFICATION 0x00000001
#define NOERRORWINDOW           0x00000002
#define REPORT_ID               0x00000004
#define IS_IN_MENU_LOOP         0x00000008
#define DLG_DOCK_ABLE           0x00000010
#define MODAL_W_CONTEXT_HELP    0x00000020
#define MODAL_W_MIN_BTN         0x00000040
#define MODAL_W_MAX_BTN         0x00000080

#define MBU_CAPTIONSTRING       0x10000000L
#define MBU_TEXTSTRING          0x20000000L
#define HID_BASE_HELP           0x00200000

#define EXIT_SIZE_MOVE_SET      0x00000001
#define EXIT_SIZE_MOVE_DOCK     0x00000002
#define EXIT_SIZE_MOVE_FORCE    0x00000004
#define EXIT_SIZE_MOVE_LEFT     0x00000010
#define EXIT_SIZE_MOVE_TOP      0x00000020
#define EXIT_SIZE_MOVE_RIGHT    0x00000040
#define EXIT_SIZE_MOVE_BOTTOM   0x00000080
#define EXIT_SIZE_MOVE_GET      0x00000100

class CEtsDialog
{
public:
   friend class CEtsPropertySheet;
   CEtsDialog();                                               // Konstruktor 1
   CEtsDialog(HINSTANCE hInstance, int nID, HWND hWndParent);  // Konstruktor 2
   virtual ~CEtsDialog();                                      // Destruktor
   bool Init(HINSTANCE hInstance, int nID, HWND hWndParent);   // Initialisierer
   int         DoModal();                                      // Erzeugt eine modalen Dialog
   HWND        Create();                                       // Erzeugt einen nichtmodalen Dialog
   HWND        GetWindowHandle(){return m_hWnd;};              // liefert den Windowhandle des Dialogfensters

   void     CreateErrorWnd(int, int, bool bSetFocus=false);    // erzeugt eine Fehlermeldung
	void     CreateErrorWnd(int, char*, bool bSetFocus=false);
	int      MessageBox(long, long , UINT uType);               // Ruft die API-Messageboxfunktion auf
	char    *LoadString(int, int *pnLen=NULL);                  // Lädt einen String aus den Resourcendaten
	RECT     AdaptMonitorPoints(POINT *, int);                  // Rechnet die Punkte für andere Monitore um

   static HWND GetTopLevelParent(HWND);
   static bool ChooseColorT(char *, COLORREF &, HWND, HANDLE); // ruft den Windows Farbdialog auf
   static void SaveCustomColors(HANDLE);
   static void LoadCustomColors(HANDLE);
   static void UnHookMsgFilterProc()
   {
      if (gm_hDll_Hook)
      {
         UnhookWindowsHookEx(gm_hDll_Hook);
      }
   }
   static void GetWindowThreadID(HWND hwnd)
   {
      gm_nWindowThreadProcessID = ::GetWindowThreadProcessId(hwnd, NULL);
   }

protected:
   virtual bool OnInitDialog(HWND hWnd);                       // zur initialisierung der Dialogfelder
   virtual int  OnOk(WORD nNotify);                            // Wird durch die OK-Taste ausgelöst
   virtual int  OnCommand(WORD, WORD, HWND);                   // verarbeitet WM_COMMAND´s an das Dialogfenster
   virtual int  OnMessage(UINT, WPARAM, LPARAM);               // verarbeitet Nachrichten an das Dialogfenster
   virtual int  OnCancel()     {return IDCANCEL;};             // Wird durch die Abbrechen-Taste ausgelöst
   virtual void OnEndDialog(int);                              // Beendet den Dialog

   void     OnApply();                                         // wird durch die "Übernehmen"-Taste ausgelöst
   void     SetChanged(bool);                                  // Enabled die "Übernehmen"-Taste
   BOOL     IsChanged();                                       // fragt den Zustand der "Übernehmen"-Taste ab

	bool     AutoInitBtnBmp(int cx=0,int cy=0);                 // Initialisiert Buttons mit Bitmaps der gleichen ID
	void     DeleteAutoBtnBmp();                                // löscht Buttonbitmapobjekte
	void     ExecuteDlgInit(HRSRC);                             // Initialisiert Dialog Listenfelder aus Resourcendaten
	void     ExecuteDlgInit(void*);
   LONG     SendDlgItemMessage(int, UINT, WPARAM, LPARAM);     // Sendet eine Nachricht zu einem Dialogfeld
   void     KillTimer(UINT);                                   // Löscht den Timer und die letze Timermeldung

   bool     SetDlgItemDouble(int, double, int);                // Wandelt eine double-Zahl in Fenstertext um
   bool     SetComboBoxStrings(int, int);                      // setzt ComboBoxeinträge durch eine String-Resource
   BOOL     SetDlgItemText(int, LPCTSTR, int nMaxLen=0);       // Setzt den Fenstertext eines Diaolgfeldes
   BOOL     SetDlgItemInt(int, int, bool);                     // Wandelt eine Integer-Zahl in Fenstertext
	bool     CheckDlgRadioBtn(int, int);                        // Setzt den zustand von Autoradiobuttons
	void     EnableGroupItems(HWND, bool, long);                // Enabled/Disabled Dialogelemente innerhalb einer Gruppe
	bool     SetNumeric(int, bool, bool);                            // Setzt ein Editfeld auf Numeric Modus

   BOOL     GetDlgItemText(int, LPTSTR, bool *pbChanged=NULL, int nMaxLen=0);// liefert den Fenstertext eines Dialogfeldes
   bool     GetDlgItemDouble(int, double&, double dDefault=0, bool *pbChanged=NULL); // Wandelt Fenstertext in eine double-Zahl um
   int      GetDlgSpinInt(int);                                // liefert die Position eines Spin-Controls
	char    *GetDlgItemText(HWND, char *pszOld=NULL, bool *pbChanged=NULL);// liefert den Text eines Fensters
   BOOL     GetDlgItemInt(int, int &, bool, int nDefault=0, bool *pbChanged=NULL);// Wandelt Fenstertext in eine Integer-Zahl
   HWND     GetDlgItem(int);                                   // liefert das Windowhandle eines Dialogfeldes
	int      GetDlgRadioBtn(int);                               // liefert den Index eines Buttons in einer Autoradiobuttongruppe
   // Überprüfung des Bereichs und evtl. Ausgabe einer Fehlermeldung
   bool     CheckMinMaxInt(int, int, int, bool, int&, bool bMsg=true);
   bool     CheckMinMaxDouble(int, double, double, int, double&, bool bMsg=true);

   HWND      m_hWnd;          // Handle des Dialogfensters
   HWND      m_hWndParent;    // Handle des Elterfensters
   HINSTANCE m_hInstance;     // Instance Handel für die Dialogresourcen
   int       m_nID;           // ID der Dialogfeldresource
   bool      m_bModal;        // Anzeiger für Modal/Nichtmodal
public:
   UINT      m_nFlags;
protected:
	void ChangeModalStyles(bool);
   HWND      m_hWndForeground;// 
   static BOOL CALLBACK    DialogProc(HWND, UINT, WPARAM, LPARAM);
	static BOOL CALLBACK    EnumGroupItems(HWND,  LPARAM);
	static BOOL CALLBACK    EnumRadioItems(HWND,  LPARAM);
	static BOOL CALLBACK    EnumBmpButtons(HWND,  LPARAM);


private :
	void  RemoveDockProc();
	LRESULT OnExitSizeMove(WPARAM, HWND);

   static UINT CALLBACK    CCHookProc(HWND, UINT, WPARAM, LPARAM);
   static LRESULT CALLBACK MsgFilterProc(int code, WPARAM wParam,  LPARAM lParam);
   static BOOL CALLBACK    MsgFilterEnum(HWND, LPARAM);

   static LRESULT CALLBACK DockingParentSC(HWND, UINT, WPARAM, LPARAM);
   long m_lOlpParentSubClass;
   int  m_nWhere;
   HWND m_hwndDock;

   static HHOOK            gm_hDll_Hook;
   static int              gm_nWindowThreadProcessID;
   static int              gm_nNonModalDlg;
   static HWND             gm_hWndTopLevelParent;

public:
#ifdef INCLUDE_ETS_HELP
	static void Destructor();
   static UINT gm_nmode;
#else
	static void SetHelpPath(char*);
#endif

   static int              gm_nIDAPPLY;                        // die ID der "Übernehmen"-Taste; im Konstructor initialisieren
   static int              gm_nIDFmtMinMaxDouble;              // FormatID's für Message String
   static int              gm_nIDFmtMinMaxInt;
   static int              gm_nIDFmtNoNumber;
   static COLORREF         gm_CustomColors[16];
   static char *           gm_pszHelp;

};

template <class T> void swap(T& x, T& y)
{
   T Temp;
   Temp = x;
          x = y;
              y = Temp;
}

/******************************************************************************
* Name       : Init bzw. CEtsDialog-Konstruktor                               *
* Zweck      : Setzen der Dialogresource und des Instancehandles um die       *
*              Dialogbox zu laden. Setzen des Parentfensters                  *
* Aufruf     : Init(hInstance, nID, hWndParent);                              *
* Parameter  :                                                                *
* hInstance (E): HANDLE der Instanz, die die Dialogresource enthält(HINSTANCE)* 
* nID       (E): ID der Dialogfeldresource.                            (UINT) *
* hWndParent(E): Parentfenster                                         (HWND) *
* Funktionswert : (true, false)                                        (bool) *
******************************************************************************/

/******************************************************************************
* Name       : DoModal                                                        *
* Zweck      : Aufruf der Dialogbox als Modales Dialogfenster                 *
* Aufruf     : DoModal();                                                     *
* Parameter  : keine                                                          *
* Funktionswert : (0, IDOK, IDCANCEL) Fehler, Ok, Abbruch               (int) *
******************************************************************************/

/******************************************************************************
* Name       : Create                                                         *
* Zweck      : Aufruf der Dialogbox als nichtmodales Dialogfenster.           *
* Aufruf     : Create()                                                       *
* Parameter  : keine                                                          *
* Funktionswert : Fensterhandle des Dialogfensters                   (HWND)   *
******************************************************************************/

/******************************************************************************
* Name       : SetChanged                                                     *
* Zweck      : Setzt den Zustand des Buttons mit der ID-Nummer                *
*              CEtsDialog::gm_nIDAPPLY, um den Zustand (nicht) geändert zu    *
*              speichern. Die Membervariable gm_nIDAPPLY sollte im Konstruktor*
*              mit dem ResourcenID-Wert des Apply-Now-Buttons initialisiert   *
*              werden.                                                        *
* Aufruf     : SetChanged(bChanged);                                          *
* Parameter  :                                                                *
* bChanged(E): true  : Der Applybutton wird Enabled                    (bool) *
*              false : Der Applybutton wird Disabled                          *
* Funktionswert : keiner                                                      *
******************************************************************************/

/******************************************************************************
* Name       : IsChanged                                                      *
* Zweck      : Fragt den Zustand des Applybuttons ab, um den Zustand (nicht)  *
*              geändert zu ermitteln. Die Membervariable gm_nIDAPPLY sollte   *
*              im Konstruktor mit dem ResourcenID-Wert des Apply-Now-Buttons  *
*              initialisiert werden.                                                        *
* Aufruf     : IsChanged();                                                   *
* Parameter  : keine                                                          *
* Funktionswert : (true, false)                                      (BOOL)   *
******************************************************************************/

/******************************************************************************
* Name       : OnInitDialog                                                   *
* Zweck      : Initialisierung der Dialogboxparameter. Setzt Fensterhandle.   *
*              Bei einer nicht modalen Dialogbox wird eine Hookfunktion als   *
*              Messagefilter installiert, die die Dialog-Box-Messages für den *
*              nichtmodalen Dialog, der den Eingabefocus hat herausfiltert.   *
*              * siehe MsgFilterProc !!!                                      *
*              Wird diese Funktion überladen, muß sie am Anfang der           *
*              überladenen Funktion folgendermaßen aufgerufen werden :        *
*              CEtsDialog::OnInitDialog(hWnd);                                   *
* Definition : virtual bool OnInitDialog(HWND hWnd);                          *
* Aufruf     : OnInitDialog(hWnd);                                            *
* Parameter  :                                                                *
* hWnd   (E) : Fensterhandle der Dialogbox                           (HWND)   *
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/

/******************************************************************************
* Name       : OnEndDialog                                                    *
* Zweck      : Aufräumen der Dialogboxparameter und Variablen.                *
*              Wird diese Funktion überladen, muß sie am Ende der überladenen *
*              Funktion folgendermaßen aufgerufen werden :                    *
*              CEtsDialog::OnEndDialog(nResult);                                 *
* Definition : virtual void OnEndDialog(int);                                 *
* Aufruf     : OnEndDialog(nResult);                                          *
* Parameter  :                                                                *
* nResult (E): Rückgabewert der Dialogbox                               (int) *
* Funktionswert : keiner                                                      *
******************************************************************************/

/******************************************************************************
* Name       : OnOk                                                           *
* Zweck      : Posten einer WM_COMMAND Message an das Parentfenster mit der   *
*              ID der Dialogboxresource, einem NotificationCode und dem       *
*              Fensterhandle der Dialogbox. Ist ein Applybutton vorhanden     *
*              wird nur dann gesendet, wenn dieser Enabled war.               *
*              Wird diese Funktion überladen, kann sie am Ende der überladenen*
*              Funktion folgendermaßen aufgerufen werden :                    *
*              return CEtsDialog::OnOk(nNotify);                                 *
* Definition : virtual int OnOk(WORD);                                        *
* Aufruf     : Durch WM_COMMAND mit IDOK bzw.                                 *
*              CEtsDialog::gm_nIDAPPLY (durch die Funktion OnApply())            *
* Parameter  :                                                                *
* Funktionswert : (IDOK, ...)                                           (int) *
* Liefert diese Funktion IDOK, so wird die Dialogbox beendet.                 *
******************************************************************************/

/******************************************************************************
* Name       : OnCommand                                                      *
* Zweck      : Verarbeitung der WM_COMMAND Nachrichten an die Dialogbox       *
*              Zum Empfangen der Nachrichten muß diese Funktion überladen     *
*              werden.                                                        *
* Definition : virtual int OnCommand(WORD, WORD, HWND);                       *
* Aufruf     : OnCommand(nID , nNotifyCode, hwndControl);                     *
*              durch die Dialogfunktion bei einer WM_COMMAND-Nachricht        *
* Parameter  :                                                                *
* nID        (E): Control ID des Absenders.                            (WORD) *
* nNotifyCode(E): Notificationcode                                     (WORD) *
* hwndControl(E): Fensterhandle des Absenders                          (HWND) *
* Funktionswert : Nachricht verarbeitet (0, 1)                         (int)  *
******************************************************************************/

/******************************************************************************
* Name       : OnMessage                                                      *
* Zweck      : Verarbeitung der Nachrichten an die Dialogbox.                 *
*              Wird diese Funktion überladen, so muß sie, wenn die Nachricht  *
*              nicht verarbeitet wurde, am Ende der überladenen Funktion      *
*              folgendermaßen aufgerufen werden:                              *
*              CEtsDialog::OnMessage(nMsg, wParam, lParam);                      *
* Definition : virtual int OnMessage(UINT, WPARAM, LPARAM);                   *
* Aufruf     : OnMessage(nMsg, wParam, lParam);                               *
*              durch die Dialogfunktion zur Verarbeitung der noch nicht       *
*              verarbeiteten Nachrichten.                                     *
* Parameter  :                                                                *
* nMsg   (E) : Message ID                                            (UINT)   *
* wParam (E) : erster Parameter der Nachricht                        (LPARAM) *
* lParam (E) : zweiter Parameter der Nachricht                       (WPARAM) *
* Funktionswert : (0, 1) Nachricht verarbeitet                       (int)    *
******************************************************************************/

/******************************************************************************
* Name       : SetComboBoxStrings                                             *
* Zweck      : Setzen der Combobox Strings durch einen Resourcenstring, der   *
*              mehrere durch "\n" getrennte Strings enthalten kann.           *
* Aufruf     : SetComboBoxStrings(nIDListBox, nIDString);                     *
* Parameter  :                                                                *
* nIDListBox(E): Resourcen-ID der Listbox                               (int) *
* nIDString (E): Resourcen-ID des Strings                               (int) *
* Funktionswert : (true, false)                                         (bool)*
******************************************************************************/

/******************************************************************************
* Name       : SetDlgItemDouble                                               *
* Zweck      : Konvertiert eine double-Zahl in einen String und setzt diesen  *
*              als Windowtext des Dialogfeldes.                               *
* Aufruf     : SetDlgItemDouble(nIDDlgItem, dValue, nDec);                    *
* Parameter  :                                                                *
* nIDDlgItem(E): ID des Dialog Items                                 (int)    *
* dValue  (E): zu konvertierende Zahl                                (double) *
* nDec    (E): Anzahl der Stellen hinter dem Komma (0,..,9)          (int)    *
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/

/******************************************************************************
* Name       : GetDlgItemDouble                                               *
* Zweck      : Konvertiert den Windowtext eines Dialogfeldes in eine          *
*              double-Zahl.                                                   *
* Aufruf     : GetDlgItemDouble(nIDDlgItem, dValue);                          *
* Parameter  :                                                                *
* nIDDlgItem(E): ID des Dialog Items                                 (int)    *
* dValue  (A): Konvertierte Zahl                                     (doubel&)*
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/

/******************************************************************************
* Name       : CheckMinMaxInt                                                 *
* Zweck      : Überprüft den Wertebereich einer Integer-Zahl, gibt eine       *
*              Fehlermeldung aus, setzt die Zahl auf einen gültigen Wert im   *
*              vorgegebenen Wertebereich und setzt den geänderten Wert im     *
*              Dialogfeld.                                                    *
* Aufruf     : CheckMinMaxInt(nIDDlgItem, nMin, nMax, bSigned, nValue);       *
* Parameter  :                                                                *
* nIDDlgItem(E): ID des Dialog Items                                 (int)    *
* nMin, nMax(E): Minimaler und Maximaler Wert des Bereichs           (int)    *
* bSigned   (E): Vorzeichen (true, false)                            (bool)   *
* nValue   (EA): zu überprüfende Zahl                                (int&)   *
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/

/******************************************************************************
* Name       : CheckMinMaxDouble                                              *
* Zweck      : Überprüft den Wertebereich einer Double-Zahl, gibt eine        *
*              Fehlermeldung aus, setzt die Zahl auf einen gültigen Wert im   *
*              vorgegebenen Wertebereich und setzt den geänderten Wert im     *
*              Dialogfeld.                                                    *
* Aufruf     : CheckMinMaxInt(nIDDlgItem, dMin, dMax, nDec, nValue);          *
* Parameter  :                                                                *
* nIDDlgItem(E): ID des Dialog Items                                 (int)    *
* nMin, nMax(E): Minimaler und Maximaler Wert des Bereichs           (int)    *
* nDec      (E): Anzahl der Kommastellen im Dialogfeldtext           (int)    *
* dValue   (EA): zu überprüfende Zahl                                (double&)*
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/

/******************************************************************************
* Name       : GetDlgSpinInt                                                  *
* Zweck      : liefert die Position eines Spinbuttons                         *
* Aufruf     : GetDlgSpinInt(nIDDlgItem);                                     *
* Parameter  :                                                                *
* nIDDlgItem(E): Resourcen-ID des Spincontrols                          (int) *
* Funktionswert : Position des Spincontrols                                   *
******************************************************************************/

/******************************************************************************
* Name       : ChooseColorT                                                   *
* Zweck      : Aufruf des Common-Dialogs zur Farbauswahl                      *
* Aufruf     : ChooseColorT(pszHeading, color);                               *
* Parameter  :                                                                *
* pszHeading(E): Überschrift des Dialogfeldes                     (char*)     *
* color  (EA): RGB-Farbwert                                       (COLORREF&) *
* Funktionswert : (true, false)                                   (bool)      *
******************************************************************************/

/******************************************************************************
* Name       : SaveCustomColors, LoadCustomColors                             *
* Zweck      : Laden bzw. Speichern der Custom Colors in einer Datei.         *
* Aufruf     : SaveCustomColors(hFile); LoadCustomColors(hFile)               *
* Parameter  :                                                                *
* hFile   (E): Dateihandle                                          (HANDLE)  *
* Funktionswert : keiner                                                      *
******************************************************************************/

/******************************************************************************
* Name       : KillTimer                                                      *
* Zweck      : Beenden des Timers und Löschen der Timer Nachrichten.          *
* Aufruf     : KillTimer(nID);                                                *
* Parameter  :                                                                *
* nID     (E): ID des Timers                                         (UINT)   *
* Funktionswert : keiner                                                      *
******************************************************************************/

/******************************************************************************
* Name       : SendDlgItemMessage                                             *
* Zweck      : Senden einer Nachricht an den Dialogitem                       *
* Aufruf     : SendDlgItemMessage(nIDDlgItem, Msg, wParam, lParam);           *
* Parameter  :                                                                *
* nIDDlgItem(E): DialogitemID                                        (int)    *
* Msg       (E): Nachrichten ID                                      (UINT)   *
* wParam    (E): wParam der Nachricht                                (WPARAM) *
* lParam    (E): lParam der Nachricht                                (LPARAM) *
* Funktionswert : Rückgebewert von SendMessage(..)                   (LONG)   *
******************************************************************************/

/******************************************************************************
* Name       : SetDlgItemText                                                 *
* Zweck      : Setzen des Textes eines Dialogitemfensters                     *
* Aufruf     : SetDlgItemText(nIDDlgItem, lpString);                          *
* Parameter  :                                                                *
* nIDDlgItem(E): DialogitemID                                        (int)    *
* lpString  (E): Text                                                (LPCTSTR)*
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/

/******************************************************************************
* Name       : SetDlgItemInt                                                  *
* Zweck      : Setzen des Integerwertes eines Dialogitemfensters              *
* Aufruf     : SetDlgItemInt(nIDDlgItem, nValue, bSigned);                    *
* Parameter  :                                                                *
* nIDDlgItem(E): DialogitemID                                        (int)    *
* nValue    (E): Integerwert                                         (int)    *
* bSigned   (E): Vorzeichen (true, false)                            (bool)   *
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/

/******************************************************************************
* Name       : CheckDlgRadioBtn                                               *
* Definition : bool CheckDlgRadioBtn(int, int);                               *
* Zweck      : Markieren des Radiobuttons mit dem Index nIBtn in der          *
*              Radiobuttongruppe.                                             *
* Aufruf     : CheckDlgRadioBtn(nID, nIBtn);                                  *
* Parameter  :                                                                *
* nID     (E): Erster Dialogelement in einer Radiobuttongruppe. Dieses  (int) *
*              muß den Windowstil WS_GROUP haben. Alle anderen dürfen diesen  *
*              Stil nicht haben! Das nächste Dialogelement mit dem Stil       *
*              WS_GROUP beendet die Radiobuttongruppe.                        *
* nBItn   (E): Index des Buttons der markiert werden soll.                    *
* Funktionswert : (true, false)                                        (bool) *
******************************************************************************/

/******************************************************************************
* Name       : EnableGroupItems                                               *
* Definition : void EnableGroupItems(HWND, bool, long);                       *
* Zweck      : Enablen oder disablen von Dialogelementen innerhalb einer      *
*              Gruppe (GROUPBOX-Control).                                     *
* Aufruf     : EnableGroupItems(hWndGroup, bEnable, lFlags);                  *
* Parameter  :                                                                *
* hWndGroup(E): WindowHandle der Gruppe                                       *
* bEnable  (E): (true, false)=>(Enable, Disable)                       (bool) *
* lFlags   (E): Windowstil Flags um Dialogelemente auszuschließen      (long) *
* Funktionswert : keiner                                                      *
******************************************************************************/

/******************************************************************************
* Name       : GetDlgItem                                                     *
* Zweck      : liefert den Fensterhandle des Dialogitems                      *
* Aufruf     : GetDlgItem(nID);                                               *
* Parameter  :                                                                *
* nID     (E): ID des Dialogitems                                    (int)    *
* Funktionswert : Fensterhandle des Dialogitems                      (HWND)   *
******************************************************************************/

/******************************************************************************
* Name       : GetDlgItemText                                                 *
* Zweck      : Ermittelt den Text eines Dialogitemfensters                    *
* Aufruf     : GetDlgItemText(nIDDlgItem, lpString, nMaxCount);               *
* Parameter  :                                                                *
* nIDDlgItem(E): DialogitemID                                        (int)    *
* lpString  (A): Text                                                (LPCTSTR)*
* nMaxCount (E): maximale Länge des Textstrings                      (int)    *
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/

/******************************************************************************
* Name       : GetDlgItemInt                                                  *
* Zweck      : liefert den Integerwert eines Dialogitemfensters               *
* Aufruf     : GetDlgItemInt(nIDDlgItem, nValue, bSigned);                    *
* Parameter  :                                                                *
* nIDDlgItem(E): DialogitemID                                        (int)    *
* nValue    (A): Integerwert                                         (int&)   *
* bSigned   (E): Vorzeichen (true, false)                            (bool)   *
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/

/******************************************************************************
* Name       : GetDlgRadioBtn                                                 *
* Definition : int GetDlgRadioBtn(nID);                                       *
* Zweck      : Ermittelt den Index des markierten Radiobuttons.               *
* Aufruf     : GetDlgRadioBtn(nID);                                           *
* Parameter  :                                                                *
* nID     (E): Erster Dialogelement in einer Radiobuttongruppe. Dieses  (int) *
*              muß den Windowstil WS_GROUP haben. Alle anderen dürfen diesen  *
*              Stil nicht haben! Das nächste Dialogelement mit dem Stil       *
*              WS_GROUP beendet die Radiobuttongruppe.                        *
* Funktionswert : Index des markierten Radiobuttons                  (int)    *
******************************************************************************/

/******************************************************************************
* Name       : Destructor                                                     *
* Zweck      : ruft den Destructor der CEtsHelp Instanz auf                   *
* Aufruf     : CEtsDialog::Destructor();                                      *
* Parameter  : keine                                                          *
* Funktionswert : keiner                                                      *
******************************************************************************/

#endif // __CDIALOG_H_INCLUDED_
