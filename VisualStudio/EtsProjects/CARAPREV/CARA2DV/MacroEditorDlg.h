#ifndef AFX_MACROEDITORDLG_H__35D25004_1578_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_MACROEDITORDLG_H__35D25004_1578_11D2_9DB9_0000B458D891__INCLUDED_

// MacroEditorDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld MacroEditorDlg 
typedef enum 
{
   ZERO_COMMAND  =0,
   MAX_CONTROL   =200,
   SPACE         = 1,
   MAX_SEPARATOR =21,
   NEWMAKROLINE  =22,
   NO_OF_CURVES  =23,
   DESCRIPTION   =24,
   UNITX         =25,
   UNITY         =26,
   VALUEX        =27,
   VALUEY        =28,
   NO_OF_VALUES  =29,
   NEWLINE       =30,
   REPEAT        =31,
   UNTIL         =32,
   ENDLOOP       =33,
   EQUAL         =34,
   TUEDELCHEN    =35,
   INDEX         =36,
   NO_OF_LOOPS   =37,
   DECIMAL_POINT =38,
   DECIMAL_COMMA =39,
   LOCUS_LINEAR  =40,
   LOCUS_LOG     =41,
   XLOGARITHMIC  =42,
   YLOGARITHMIC  =43,
   FIND_STRING   =44,
   TABULATOR     =45,
   COMMENT       =50,
   AUTOSIZE      =51,
   UNITLOCUS     =52,
   START_VALUE   =53,
   STEP_VALUE    =54,
   OFFSETY       =55,
   VALUELOCUS    =56,
   XPOLAR        =57,
   ANGLE_GON     =58,
   ANGLE_RAD     =59,
   FORMAT_DOUBLE =60,
   FORMAT_FLOAT  =61,
   FORMAT_SHORT  =62,
   SINGLE_FORMAT =63,
   STEP_BY       =64,
   SOMECHARACTER =80,
   SET_VALUE     =100,
}e_MacroCommand;

struct SControl
{
   e_MacroCommand nCommand;
   short nNumber;
   void *ptr;
   DWORD dwDebugPos;
};

#define ALPHA          1
#define NUMERIC        2
#define ALPHA_STRING   4

#define DEBUG_TM_RUN   0
#define DEBUG_TM_BREAK 1
#define DEBUG_TM_STEP  2

#define DEBUG_TMSET_SELECT  1
#define DEBUG_TMSET_RUN     2
#define DEBUG_TMSET_SET_TXT 3

class CTreeDlg;
class CTextEdit;

class MacroEditorDlg : public CDialog
{
   struct KeyWords
   {
      char *szKey;
      int   nCode;
   };
// Konstruktion
public:
	MacroEditorDlg(CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~MacroEditorDlg();                       // StandardDestruktor

private:
// Dialogfelddaten
   //{{AFX_DATA(MacroEditorDlg)
   enum { IDD = IDD_CURVE_TEXT_MACRO };
   CComboBox	m_MakroList;
   CEdit	m_MakroEdit;
   //}}AFX_DATA
public:
   volatile BOOL m_nRun;
   CTextEdit*m_pTexEdit;
   void SaveToRegistry();
   void LoadFromRegistry();
   void ReloadMacroLists();

private:
// private Daten
   bool m_bChanged;
   int  m_nEditNo;
// private Funktionen
   void LoadMakroText(int );
   bool SaveMakroText(bool bRequest = true);

   static int   Keyword(LPCTSTR str, int& nKeyLength);
   static const KeyWords MacroEditorDlg::gm_Keys[];

public:
	static void GetCommand(SControl *pControl, CString&);
// öffentliche Daten
   CStringArray   m_Makros;
   CStringArray   m_Descriptions;
// öffentliche Funktionen
   static long CheckSyntax(CString&, SControl *, bool bTest=false);
   static int  GetIndex(SControl*, int, int);
   static int  ExtractString(char *, int, CString&, char*strCut=NULL, char*strInclude=NULL);
   static bool ExtractdValue(char *, bool, double &);
   static int  GetnValue(char *, int &);

// Überschreibungen
   // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
   //{{AFX_VIRTUAL(MacroEditorDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
   //}}AFX_VIRTUAL

// Implementierung
protected:
	LRESULT OnLButtonDblClkEdit(WPARAM, LPARAM);
	static int InitTree(CTreeDlg*, int);
	static LRESULT CALLBACK EditCallback(HWND, UINT, WPARAM, LPARAM);
   static long gm_lEditCallBack;
	static void TreeHelp(CTreeDlg*, HELPINFO*);

   // Generierte Nachrichtenzuordnungsfunktionen
   //{{AFX_MSG(MacroEditorDlg)
   afx_msg void OnDeleteMacro();
   afx_msg void OnNewMacro();
   afx_msg void OnSelchangeMakroList();
   virtual void OnCancel();
   afx_msg void OnChangeMakroEdit();
   virtual BOOL OnInitDialog();
   afx_msg void OnEditchangeMakroList();
   afx_msg void OnSaveMakro();
   afx_msg LRESULT OnHelp(WPARAM, LPARAM);
   afx_msg LRESULT OnUser(WPARAM, LPARAM);
	afx_msg void OnHelpButton();
   afx_msg void OnBnClickedMacroDebug();
   afx_msg void OnBnClickedRdStep();
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_MACROEDITORDLG_H__35D25004_1578_11D2_9DB9_0000B458D891__INCLUDED_
