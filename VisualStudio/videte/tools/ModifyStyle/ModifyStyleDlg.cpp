/////////////////////////////////////////////////////////////////////////////
//	Project:		Common
//
//	File:			$Workfile: ModifyStyleDlg.cpp $ : implementation file
//
//	Start:			01.06.95
//
//  Last change:	$Modtime: 9/05/05 1:44p $
//
//  by Author	:	$Author: Rolf.kary-ehlers $
//
//  Checked in:	$Date: 9/05/05 1:44p $
//
//  Version:		$Revision: 35 $
//
//	Company:		w+k Video Communication GmbH & Co.KG
//
//$Nokeywords:$
/////////////////////////////////////////////////////////////////////////////
// ModifyStyleDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "ModifyStyle.h"
#include "ModifyStyleDlg.h"
#include "Psapi.h"

#include <aclapi.h>
#include ".\modifystyledlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialogfeld für Anwendungsbefehl "Info"

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogfelddaten
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// Keine Nachrichten-Handler
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define WND_STYLE					 0
#define WND_STYLE_EX				 1
#define EDIT_STYLE					 2
#define BUTTON_STYLE				 3
#define STATIC_STYLE				 4
#define DIALOG_STYLE				 5
#define LISTBOX_STYLE				 6
#define COMBOBOX_STYLE				 7
#define SCROLLBAR_STYLE				 8
#define SYSLISTVIEW32_STYLE			 9
#define SYSLISTVIEW32_EX			10
#define SYSTREEVIEW32_STYLE			11
#define SYSTABCONTROL32_STYLE		12
#define SYSTABCONTROL32_EX_STYLE	13
#define TOOLBARWINDOW32_STYLE		14
#define MSCTLS_UPDOWN32_STYLE		15
#define CLASS_STYLE					16
#define PROCESS_COMBO_POS			16
#define CHILDREN_COMBO_POS			17

/////////////////////////////////////////////////////////////////////////////
// CModifyStyleDlg Dialogfeld
_TCHAR * CModifyStyleDlg::gm_szClasses[] =
{
   _T("Wnd"),               // Windowstyle             0
   _T("Ex"),                // WindowstyleEx           1
   _T("Edit"),              // EditWindowStyle         2
   _T("Button"),            // ButtonWindowStyle       3
   _T("Static"),            // StaticWindowStyle       4
   _T("#32770"),            // DialogWindowStyle       5
   _T("ListBox"),           // ListboxWindowStyle      6
   _T("ComboBox"),          // ComboBoxWindowStyle     7
   _T("ScrollBar"),         // ScrollbaWindowStyle     8
   _T("SysListView32"),     // ListViewWindowStyle     9
   _T("Ex"),                // ListViewWindowStyleEx  10
   _T("SysTreeView32"),     // TreeViewWindowStyle    11
   _T("SysTabControl32"),   // TabCtrlWindowStyle     12
   _T("Ex"),                // TabCtrlWindowStyleEx   13
   _T("ToolbarWindow32"),   // ToolBarWindowStyle     14
   _T("msctls_updown32"),   // UpDownCtrlWindowStyle  15
   NULL,
};

WindowStyles CModifyStyleDlg::gm_ListClassStyle[]=
{
   {CS_VREDRAW, _T("CS_VREDRAW")},
   {CS_HREDRAW, _T("CS_HREDRAW")},
   {CS_DBLCLKS, _T("CS_DBLCLKS")},
   {CS_OWNDC, _T("CS_OWNDC")},
   {CS_CLASSDC, _T("CS_CLASSDC")},
   {CS_PARENTDC, _T("CS_PARENTDC")},
   {CS_NOCLOSE, _T("CS_NOCLOSE")},
   {CS_SAVEBITS, _T("CS_SAVEBITS")},
   {CS_BYTEALIGNCLIENT, _T("CS_BYTEALIGNCLIENT")},
   {CS_BYTEALIGNWINDOW, _T("CS_BYTEALIGNWINDOW")},
   {CS_GLOBALCLASS, _T("CS_GLOBALCLASS")},
   {CS_IME, _T("CS_IME")},
#if(_WIN32_WINNT >= 0x0501)
   {CS_DROPSHADOW, _T("CS_DROPSHADOW")},
#endif /* _WIN32_WINNT >= 0x0501 */
   {0              , NULL}
};
WindowStyles CModifyStyleDlg::gm_ListStyle[]=
{
   {WS_POPUP       , _T("WS_POPUP")},
   {WS_CHILD       , _T("WS_CHILD/WS_CHILDWINDOW")},
   {WS_MINIMIZE    , _T("WS_MINIMIZE")},
   {WS_VISIBLE     , _T("WS_VISIBLE")},
   {WS_DISABLED    , _T("WS_DISABLED")},
   {WS_CLIPSIBLINGS, _T("WS_CLIPSIBLINGS")},
   {WS_CLIPCHILDREN, _T("WS_CLIPCHILDREN")},
   {WS_MAXIMIZE    , _T("WS_MAXIMIZE")},
   {WS_CAPTION     , _T("WS_CAPTION")},
   {WS_BORDER      , _T("WS_BORDER")},
   {WS_DLGFRAME    , _T("WS_DLGFRAME")},
   {WS_VSCROLL     , _T("WS_VSCROLL")},
   {WS_HSCROLL     , _T("WS_HSCROLL")},
   {WS_SYSMENU     , _T("WS_SYSMENU")},
   {WS_THICKFRAME  , _T("WS_THICKFRAME")},
   {WS_GROUP       , _T("WS_GROUP")},
   {WS_TABSTOP     , _T("WS_TABSTOP")},
   {WS_MINIMIZEBOX , _T("WS_MINIMIZEBOX")},
   {WS_MAXIMIZEBOX , _T("WS_MAXIMIZEBOX")},
   {WS_TILED       , _T("WS_TILED/WS_OVERLAPPED")},
   {WS_ICONIC      , _T("WS_ICONIC/WS_MINIMIZE")},
   {WS_SIZEBOX     , _T("WS_SIZEBOX/WS_THICKFRAME")},
   {WS_TILEDWINDOW , _T("WS_TILEDWINDOW/WS_OVERLAPPEDWINDOW")},
   {WS_POPUPWINDOW , _T("WS_POPUPWINDOW")},
   {0              , NULL}
};

WindowStyles CModifyStyleDlg::gm_ListStyleEx[] =
{
   {WS_EX_DLGMODALFRAME  , _T("WS_EX_DLGMODALFRAME")},
   {WS_EX_NOPARENTNOTIFY , _T("WS_EX_NOPARENTNOTIFY")},
   {WS_EX_TOPMOST        , _T("WS_EX_TOPMOST")},
   {WS_EX_ACCEPTFILES    , _T("WS_EX_ACCEPTFILES")},
   {WS_EX_TRANSPARENT    , _T("WS_EX_TRANSPARENT")},
   {WS_EX_MDICHILD       , _T("WS_EX_MDICHILD")},
   {WS_EX_TOOLWINDOW     , _T("WS_EX_TOOLWINDOW")},
   {WS_EX_WINDOWEDGE     , _T("WS_EX_WINDOWEDGE")},
   {WS_EX_CLIENTEDGE     , _T("WS_EX_CLIENTEDGE")},
   {WS_EX_CONTEXTHELP    , _T("WS_EX_CONTEXTHELP")},
#if(WINVER >= 0x0400)
   {WS_EX_RIGHT          , _T("WS_EX_RIGHT")},
   {WS_EX_LEFT           , _T("WS_EX_LEFT")},
   {WS_EX_RTLREADING     , _T("WS_EX_RTLREADING")},
   {WS_EX_LTRREADING     , _T("WS_EX_LTRREADING")},
   {WS_EX_LEFTSCROLLBAR  , _T("WS_EX_LEFTSCROLLBAR")},
   {WS_EX_RIGHTSCROLLBAR , _T("WS_EX_RIGHTSCROLLBAR")},
   {WS_EX_CONTROLPARENT  , _T("WS_EX_CONTROLPARENT")},
   {WS_EX_STATICEDGE     , _T("WS_EX_STATICEDGE")},
   {WS_EX_APPWINDOW      , _T("WS_EX_APPWINDOW")},
#endif /* WINVER >= 0x0400 */
#if(_WIN32_WINNT >= 0x0500)
   {WS_EX_OVERLAPPEDWINDOW, _T("WS_EX_OVERLAPPEDWINDOW")},
   {WS_EX_PALETTEWINDOW  , _T("WS_EX_PALETTEWINDOW")},
   {WS_EX_LAYERED        , _T("WS_EX_LAYERED")},
   {WS_EX_NOINHERITLAYOUT, _T("WS_EX_NOINHERITLAYOUT")},
   {WS_EX_LAYOUTRTL      , _T("WS_EX_LAYOUTRTL")},
   {WS_EX_COMPOSITED     , _T("WS_EX_COMPOSITED")},
   {WS_EX_NOACTIVATE     , _T("WS_EX_NOACTIVATE")},
#endif /* _WIN32_WINNT >= 0x0500 */
   {0                    , NULL}
};

WindowStyles CModifyStyleDlg::gm_ListStyleEdit[] =
{
   { ES_LEFT       , _T("ES_LEFT")},
   { ES_CENTER     , _T("ES_CENTER")},
   { ES_RIGHT      , _T("ES_RIGHT")},
   { ES_MULTILINE  , _T("ES_MULTILINE")},
   { ES_UPPERCASE  , _T("ES_UPPERCASE")},
   { ES_LOWERCASE  , _T("ES_LOWERCASE")},
   { ES_PASSWORD   , _T("ES_PASSWORD")},
   { ES_AUTOVSCROLL, _T("ES_AUTOVSCROLL")},
   { ES_AUTOHSCROLL, _T("ES_AUTOHSCROLL")},
   { ES_NOHIDESEL  , _T("ES_NOHIDESEL")},
   { ES_OEMCONVERT , _T("ES_OEMCONVERT")},
   { ES_READONLY   , _T("ES_READONLY")},
   { ES_WANTRETURN , _T("ES_WANTRETURN")},
   { ES_NUMBER     , _T("ES_NUMBER")},
   {0              , NULL}
};

WindowStyles CModifyStyleDlg::gm_ListStyleBtn[] =
{
   { BS_PUSHBUTTON     , _T("BS_PUSHBUTTON")},
   { BS_DEFPUSHBUTTON  , _T("BS_DEFPUSHBUTTON")},
   { BS_CHECKBOX       , _T("BS_CHECKBOX")},
   { BS_AUTOCHECKBOX   , _T("BS_AUTOCHECKBOX")},
   { BS_RADIOBUTTON    , _T("BS_RADIOBUTTON")},
   { BS_3STATE         , _T("BS_3STATE")},
   { BS_AUTO3STATE     , _T("BS_AUTO3STATE")},
   { BS_GROUPBOX       , _T("BS_GROUPBOX")},
   { BS_USERBUTTON     , _T("BS_USERBUTTON")},
   { BS_AUTORADIOBUTTON, _T("BS_AUTORADIOBUTTON")},
   { BS_OWNERDRAW      , _T("BS_OWNERDRAW")},
   { BS_LEFTTEXT       , _T("BS_LEFTTEXT")},
#if(WINVER >= 0x0400)
   { BS_TEXT           , _T("BS_TEXT")},
   { BS_ICON           , _T("BS_ICON")},
   { BS_BITMAP         , _T("BS_BITMAP")},
   { BS_LEFT           , _T("BS_LEFT")},
   { BS_RIGHT          , _T("BS_RIGHT")},
   { BS_CENTER         , _T("BS_CENTER")},
   { BS_TOP            , _T("BS_TOP")},
   { BS_BOTTOM         , _T("BS_BOTTOM")},
   { BS_VCENTER        , _T("BS_VCENTER")},
   { BS_PUSHLIKE       , _T("BS_PUSHLIKE")},
   { BS_MULTILINE      , _T("BS_MULTILINE")},
   { BS_NOTIFY         , _T("BS_NOTIFY")},
   { BS_FLAT           , _T("BS_FLAT")},
#endif /* WINVER >= 0x0400 */
   {0                  , NULL}
};

WindowStyles CModifyStyleDlg::gm_ListStyleStatic[] =
{
   { SS_LEFT          , _T("SS_LEFT")},
   { SS_CENTER        , _T("SS_CENTER")},
   { SS_RIGHT         , _T("SS_RIGHT")},
   { SS_ICON          , _T("SS_ICON")},
   { SS_BLACKRECT     , _T("SS_BLACKRECT")},
   { SS_GRAYRECT      , _T("SS_GRAYRECT")},
   { SS_WHITERECT     , _T("SS_WHITERECT")},
   { SS_BLACKFRAME    , _T("SS_BLACKFRAME")},
   { SS_GRAYFRAME     , _T("SS_GRAYFRAME")},
   { SS_WHITEFRAME    , _T("SS_WHITEFRAME")},
   { SS_USERITEM      , _T("SS_USERITEM")},
   { SS_SIMPLE        , _T("SS_SIMPLE")},
   { SS_LEFTNOWORDWRAP, _T("SS_LEFTNOWORDWRAP")},
#if(WINVER >= 0x0400)
   { SS_OWNERDRAW     , _T("SS_OWNERDRAW")},
   { SS_BITMAP        , _T("SS_BITMAP")},
   { SS_ENHMETAFILE   , _T("SS_ENHMETAFILE")},
   { SS_ETCHEDHORZ    , _T("SS_ETCHEDHORZ")},
   { SS_ETCHEDVERT    , _T("SS_ETCHEDVERT")},
   { SS_ETCHEDFRAME   , _T("SS_ETCHEDFRAME")},
   { SS_TYPEMASK      , _T("SS_TYPEMASK")},
#endif /* WINVER >= 0x0400 */
#if(WINVER >= 0x0501)
   {SS_REALSIZECONTROL, _T("SS_REALSIZECONTROL")},
#endif /* WINVER >= 0x0501 */
   { SS_NOPREFIX      , _T("SS_NOPREFIX")},
#if(WINVER >= 0x0400)
   { SS_NOTIFY        , _T("SS_NOTIFY")},
   { SS_CENTERIMAGE   , _T("SS_CENTERIMAGE")},
   { SS_RIGHTJUST     , _T("SS_RIGHTJUST")},
   { SS_REALSIZEIMAGE , _T("SS_REALSIZEIMAGE")},
   { SS_SUNKEN        , _T("SS_SUNKEN")},
   { SS_ENDELLIPSIS   , _T("SS_ENDELLIPSIS")},
   { SS_PATHELLIPSIS  , _T("SS_PATHELLIPSIS")},
   { SS_WORDELLIPSIS  , _T("SS_WORDELLIPSIS")},
   { SS_ELLIPSISMASK  , _T("SS_ELLIPSISMASK")},
#endif /* WINVER >= 0x0400 */
   {0                 , NULL}
};

WindowStyles CModifyStyleDlg::gm_ListStyleDlg[] =
{
   { DS_ABSALIGN     , _T("DS_ABSALIGN")},
   { DS_SYSMODAL     , _T("DS_SYSMODAL")},
   { DS_LOCALEDIT    , _T("DS_LOCALEDIT")},
   { DS_SETFONT      , _T("DS_SETFONT")},
   { DS_MODALFRAME   , _T("DS_MODALFRAME")},
   { DS_NOIDLEMSG    , _T("DS_NOIDLEMSG")},
   { DS_SETFOREGROUND, _T("DS_SETFOREGROUND")},
#if(WINVER >= 0x0400)
   { DS_3DLOOK       , _T("DS_3DLOOK")},
   { DS_FIXEDSYS     , _T("DS_FIXEDSYS")},
   { DS_NOFAILCREATE , _T("DS_NOFAILCREATE")},
   { DS_CONTROL      , _T("DS_CONTROL")},
   { DS_CENTER       , _T("DS_CENTER")},
   { DS_CENTERMOUSE  , _T("DS_CENTERMOUSE")},
   { DS_CONTEXTHELP  , _T("DS_CONTEXTHELP")},
   { DS_SHELLFONT    , _T("DS_SHELLFONT")},
#endif /* WINVER >= 0x0400 */
#if(_WIN32_WCE >= 0x0500)
   {DS_USEPIXELS     , _T("DS_USEPIXELS")},
#endif
	{0                , NULL}
};

WindowStyles CModifyStyleDlg::gm_ListStyleLB[] =
{
   { LBS_NOTIFY           , _T("LBS_NOTIFY")},
   { LBS_SORT             , _T("LBS_SORT")},
   { LBS_NOREDRAW         , _T("LBS_NOREDRAW")},
   { LBS_MULTIPLESEL      , _T("LBS_MULTIPLESEL")},
   { LBS_OWNERDRAWFIXED   , _T("LBS_OWNERDRAWFIXED")},
   { LBS_OWNERDRAWVARIABLE, _T("LBS_OWNERDRAWVARIABLE")},
   { LBS_HASSTRINGS       , _T("LBS_HASSTRINGS")},
   { LBS_USETABSTOPS      , _T("LBS_USETABSTOPS")},
   { LBS_NOINTEGRALHEIGHT , _T("LBS_NOINTEGRALHEIGHT")},
   { LBS_MULTICOLUMN      , _T("LBS_MULTICOLUMN")},
   { LBS_WANTKEYBOARDINPUT, _T("LBS_WANTKEYBOARDINPUT")},
   { LBS_EXTENDEDSEL      , _T("LBS_EXTENDEDSEL")},
   { LBS_DISABLENOSCROLL  , _T("LBS_DISABLENOSCROLL")},
   { LBS_NODATA           , _T("LBS_NODATA")},
#if(WINVER >= 0x0400)
   { LBS_NOSEL            , _T("LBS_NOSEL")},
#endif /* WINVER >= 0x0400 */
   {LBS_COMBOBOX          , _T("LBS_COMBOBOX")},
   {LBS_STANDARD          , _T("LBS_STANDARD")},
   {0                     , NULL}
};

WindowStyles CModifyStyleDlg::gm_ListStyleCB[] =
{
   { CBS_SIMPLE           , _T("CBS_SIMPLE")},
   { CBS_DROPDOWN         , _T("CBS_DROPDOWN")},
   { CBS_DROPDOWNLIST     , _T("CBS_DROPDOWNLIST")},
   { CBS_OWNERDRAWFIXED   , _T("CBS_OWNERDRAWFIXED")},
   { CBS_OWNERDRAWVARIABLE, _T("CBS_OWNERDRAWVARIABLE")},
   { CBS_AUTOHSCROLL      , _T("CBS_AUTOHSCROLL")},
   { CBS_OEMCONVERT       , _T("CBS_OEMCONVERT")},
   { CBS_SORT             , _T("CBS_SORT")},
   { CBS_HASSTRINGS       , _T("CBS_HASSTRINGS")},
   { CBS_NOINTEGRALHEIGHT , _T("CBS_NOINTEGRALHEIGHT")},
   { CBS_DISABLENOSCROLL  , _T("CBS_DISABLENOSCROLL")},
#if(WINVER >= 0x0400)
   { CBS_UPPERCASE        , _T("CBS_UPPERCASE")},
   { CBS_LOWERCASE        , _T("CBS_LOWERCASE")},
#endif /* WINVER >= 0x0400 */
   {0                     , NULL}
};

WindowStyles CModifyStyleDlg::gm_ListStyleSBAR[] =
{
   { SBS_HORZ                   , _T("SBS_HORZ")},
   { SBS_VERT                   , _T("SBS_VERT")},
   { SBS_TOPALIGN               , _T("SBS_TOPALIGN")},
   { SBS_LEFTALIGN              , _T("SBS_LEFTALIGN")},
   { SBS_BOTTOMALIGN            , _T("SBS_BOTTOMALIGN")},
   { SBS_RIGHTALIGN             , _T("SBS_RIGHTALIGN")},
   { SBS_SIZEBOXTOPLEFTALIGN    , _T("SBS_SIZEBOXTOPLEFTALIGN")},
   { SBS_SIZEBOXBOTTOMRIGHTALIGN, _T("SBS_SIZEBOXBOTTOMRIGHTALIGN")},
   { SBS_SIZEBOX                , _T("SBS_SIZEBOX")},
#if(WINVER >= 0x0400)
   { SBS_SIZEGRIP               , _T("SBS_SIZEGRIP")},
#endif /* WINVER >= 0x0400 */
   {0                           , NULL}
};

WindowStyles CModifyStyleDlg::gm_ListStyleLV[] =
{
   { LVS_ICON           , _T("LVS_ICON")},
   { LVS_REPORT         , _T("LVS_REPORT")},
   { LVS_SMALLICON      , _T("LVS_SMALLICON")},
   { LVS_LIST           , _T("LVS_LIST")},
   { LVS_TYPEMASK       , _T("LVS_TYPEMASK")},
   { LVS_SINGLESEL      , _T("LVS_SINGLESEL")},
   { LVS_SHOWSELALWAYS  , _T("LVS_SHOWSELALWAYS")},
   { LVS_SORTASCENDING  , _T("LVS_SORTASCENDING")},
   { LVS_SORTDESCENDING , _T("LVS_SORTDESCENDING")},
   { LVS_SHAREIMAGELISTS, _T("LVS_SHAREIMAGELISTS")},
   { LVS_NOLABELWRAP    , _T("LVS_NOLABELWRAP")},
   { LVS_AUTOARRANGE    , _T("LVS_AUTOARRANGE")},
   { LVS_EDITLABELS     , _T("LVS_EDITLABELS")},
#if (_WIN32_IE >= 0x0300)
   { LVS_OWNERDATA      , _T("LVS_OWNERDATA")},
#endif
   { LVS_NOSCROLL       , _T("LVS_NOSCROLL")},
   { LVS_TYPESTYLEMASK  , _T("LVS_TYPESTYLEMASK")},
   { LVS_ALIGNTOP       , _T("LVS_ALIGNTOP")},
   { LVS_ALIGNLEFT      , _T("LVS_ALIGNLEFT")},
   { LVS_ALIGNMASK      , _T("LVS_ALIGNMASK")},
   { LVS_OWNERDRAWFIXED , _T("LVS_OWNERDRAWFIXED")},
   { LVS_NOCOLUMNHEADER , _T("LVS_NOCOLUMNHEADER")},
   { LVS_NOSORTHEADER   , _T("LVS_NOSORTHEADER")},
   {0                   , NULL}
};

WindowStyles CModifyStyleDlg::gm_ListStyleLVEx[] =
{
// EX styles for use with LVM_GETEXTENDEDSTYLE
   { LVS_EX_GRIDLINES       , _T("LVS_EX_GRIDLINES")},
   { LVS_EX_SUBITEMIMAGES   , _T("LVS_EX_SUBITEMIMAGES")},
   { LVS_EX_CHECKBOXES      , _T("LVS_EX_CHECKBOXES")},
   { LVS_EX_TRACKSELECT     , _T("LVS_EX_TRACKSELECT")},
   { LVS_EX_HEADERDRAGDROP  , _T("LVS_EX_HEADERDRAGDROP")},
   { LVS_EX_FULLROWSELECT   , _T("LVS_EX_FULLROWSELECT")},
   { LVS_EX_ONECLICKACTIVATE, _T("LVS_EX_ONECLICKACTIVATE")},
   { LVS_EX_TWOCLICKACTIVATE, _T("LVS_EX_TWOCLICKACTIVATE")},
#if (_WIN32_IE >= 0x0400)
   { LVS_EX_FLATSB          , _T("LVS_EX_FLATSB")},
   { LVS_EX_REGIONAL        , _T("LVS_EX_REGIONAL")},
   { LVS_EX_INFOTIP         , _T("LVS_EX_INFOTIP")},
   { LVS_EX_UNDERLINEHOT    , _T("LVS_EX_UNDERLINEHOT")},
   { LVS_EX_UNDERLINECOLD   , _T("LVS_EX_UNDERLINECOLD")},
   { LVS_EX_MULTIWORKAREAS  , _T("LVS_EX_MULTIWORKAREAS")},
#endif// End (_WIN32_IE >= 0x0400)
#if (_WIN32_IE >= 0x0500)
   { LVS_EX_LABELTIP        , _T("LVS_EX_LABELTIP")},
   { LVS_EX_BORDERSELECT    , _T("LVS_EX_BORDERSELECT")},
#endif  // End (_WIN32_IE >= 0x0500)
#if (_WIN32_WINNT >= 0x501)
   {LVS_EX_DOUBLEBUFFER     , _T("LVS_EX_DOUBLEBUFFER")},
   {LVS_EX_HIDELABELS       , _T("LVS_EX_HIDELABELS")},
   {LVS_EX_SINGLEROW        , _T("LVS_EX_SINGLEROW")},
   {LVS_EX_SNAPTOGRID       , _T("LVS_EX_SNAPTOGRID")},
   { LVS_EX_SIMPLESELECT    , _T("LVS_EX_SIMPLESELECT")},
#endif
   {0                       , NULL}
};

WindowStyles CModifyStyleDlg::gm_ListStyleTV[] =
{
   { TVS_HASBUTTONS     , _T("TVS_HASBUTTONS")},
   { TVS_HASLINES       , _T("TVS_HASLINES")},
   { TVS_LINESATROOT    , _T("TVS_LINESATROOT")},
   { TVS_EDITLABELS     , _T("TVS_EDITLABELS")},
   { TVS_DISABLEDRAGDROP, _T("TVS_DISABLEDRAGDROP")},
   { TVS_SHOWSELALWAYS  , _T("TVS_SHOWSELALWAYS")},
#if (_WIN32_IE >= 0x0300)
   { TVS_RTLREADING     , _T("TVS_RTLREADING")},
   { TVS_NOTOOLTIPS     , _T("TVS_NOTOOLTIPS")},
   { TVS_CHECKBOXES     , _T("TVS_CHECKBOXES")},
   { TVS_TRACKSELECT    , _T("TVS_TRACKSELECT")},
#endif
#if (_WIN32_IE >= 0x0400)
   { TVS_SINGLEEXPAND   , _T("TVS_SINGLEEXPAND")},
   { TVS_INFOTIP        , _T("TVS_INFOTIP")},
   { TVS_FULLROWSELECT  , _T("TVS_FULLROWSELECT")},
   { TVS_NOSCROLL       , _T("TVS_NOSCROLL")},
   { TVS_NONEVENHEIGHT  , _T("TVS_NONEVENHEIGHT")},
#endif
#if (_WIN32_IE >= 0x500)
   { TVS_NOHSCROLL      , _T("TVS_NOHSCROLL")},
#endif
   {0                   , NULL}
};

WindowStyles CModifyStyleDlg::gm_ListStyleTC[] =
{
#if (_WIN32_IE >= 0x0300)
   { TCS_SCROLLOPPOSITE   , _T("TCS_SCROLLOPPOSITE")},
   { TCS_BOTTOM           , _T("TCS_BOTTOM")},
   { TCS_RIGHT            , _T("TCS_RIGHT")},
   { TCS_MULTISELECT      , _T("TCS_MULTISELECT")},
#endif
#if (_WIN32_IE >= 0x0400)
   { TCS_FLATBUTTONS      , _T("TCS_FLATBUTTONS")},
#endif
   { TCS_FORCEICONLEFT    , _T("TCS_FORCEICONLEFT")},
   { TCS_FORCELABELLEFT   , _T("TCS_FORCELABELLEFT")},
#if (_WIN32_IE >= 0x0300)
   { TCS_HOTTRACK         , _T("TCS_HOTTRACK")},
   { TCS_VERTICAL         , _T("TCS_VERTICAL")},
#endif
   { TCS_TABS             , _T("TCS_TABS")},
   { TCS_BUTTONS          , _T("TCS_BUTTONS")},
   { TCS_SINGLELINE       , _T("TCS_SINGLELINE")},
   { TCS_MULTILINE        , _T("TCS_MULTILINE")},
   { TCS_RIGHTJUSTIFY     , _T("TCS_RIGHTJUSTIFY")},
   { TCS_FIXEDWIDTH       , _T("TCS_FIXEDWIDTH")},
   { TCS_RAGGEDRIGHT      , _T("TCS_RAGGEDRIGHT")},
   { TCS_FOCUSONBUTTONDOWN, _T("TCS_FOCUSONBUTTONDOWN")},
   { TCS_OWNERDRAWFIXED   , _T("TCS_OWNERDRAWFIXED")},
   { TCS_TOOLTIPS         , _T("TCS_TOOLTIPS")},
   { TCS_FOCUSNEVER       , _T("TCS_FOCUSNEVER")},
   {0                   , NULL}
};

WindowStyles CModifyStyleDlg::gm_ListStyleTCEx[] =
{
#if (_WIN32_IE >= 0x0400)
// EX styles for use with TCM_GETEXTENDEDSTYLE
   { TCS_EX_FLATSEPARATORS, _T("TCS_EX_FLATSEPARATORS")},
   { TCS_EX_REGISTERDROP  , _T("TCS_EX_REGISTERDROP")},
#endif
   {0                   , NULL}
};

WindowStyles CModifyStyleDlg::gm_ListStyleTB[] =
{   
   { TBS_AUTOTICKS     , _T("TBS_AUTOTICKS")},
   { TBS_VERT          , _T("TBS_VERT")},
   { TBS_HORZ          , _T("TBS_HORZ")},
   { TBS_TOP           , _T("TBS_TOP")},
   { TBS_BOTTOM        , _T("TBS_BOTTOM")},
   { TBS_LEFT          , _T("TBS_LEFT")},
   { TBS_RIGHT         , _T("TBS_RIGHT")},
   { TBS_BOTH          , _T("TBS_BOTH")},
   { TBS_NOTICKS       , _T("TBS_NOTICKS")},
   { TBS_ENABLESELRANGE, _T("TBS_ENABLESELRANGE")},
   { TBS_FIXEDLENGTH   , _T("TBS_FIXEDLENGTH")},
   { TBS_NOTHUMB       , _T("TBS_NOTHUMB")},
#if (_WIN32_IE >= 0x0300)
   { TBS_TOOLTIPS      , _T("TBS_TOOLTIPS")},
#endif
#if (_WIN32_IE >= 0x0500)
   { TBS_REVERSED      , _T("TBS_REVERSED")},
#endif
#if (_WIN32_IE >= 0x0501)
   { TBS_DOWNISLEFT      , _T("TBS_DOWNISLEFT")},
#endif
   {0                  , NULL}
};

WindowStyles CModifyStyleDlg::gm_ListStyleUDC[] =
{   
   { UDS_WRAP       , _T("UDS_WRAP")},
   { UDS_SETBUDDYINT, _T("UDS_SETBUDDYINT")},
   { UDS_ALIGNRIGHT , _T("UDS_ALIGNRIGHT")},
   { UDS_ALIGNLEFT  , _T("UDS_ALIGNLEFT")},
   { UDS_AUTOBUDDY  , _T("UDS_AUTOBUDDY")},
   { UDS_ARROWKEYS  , _T("UDS_ARROWKEYS")},
   { UDS_HORZ       , _T("UDS_HORZ")},
   { UDS_NOTHOUSANDS, _T("UDS_NOTHOUSANDS")},
#if (_WIN32_IE >= 0x0300)
   { UDS_HOTTRACK   , _T("UDS_HOTTRACK")},
#endif
   {0               , NULL}
};


long CModifyStyleDlg::gm_lOldBtnWndFc = 0;

/*
typedef struct _MODULEINFO {
    LPVOID lpBaseOfDll;
    DWORD SizeOfImage;
    LPVOID EntryPoint;
} MODULEINFO, *LPMODULEINFO;
*/
struct ProcessData
{
   CString    szName;
   CString    szPath;
   MODULEINFO mi;
   HANDLE     hHandle;
   CPtrList   modules;
	DWORD      dwID;
};

struct ChildrenData
{
	ChildrenData(HWND hwnd, _TCHAR*szName)
	{
		hwndChild = hwnd;
		sName     = szName;
	}
	HWND hwndChild;
	CString sName;
};


HMODULE  g_hPSapi = NULL;
BOOL  (WINAPI*g_pfnEnumProcesses)(DWORD*, DWORD, DWORD*)              = NULL;
BOOL  (WINAPI*g_pfnEnumProcessModules)(HANDLE,HMODULE*,DWORD,LPDWORD) = NULL;
DWORD (WINAPI*g_pfnGetModuleBaseName)(HANDLE,HMODULE,LPTSTR,DWORD)    = NULL;
DWORD (WINAPI*g_pfnGetMappedFileName)(HANDLE,LPVOID,LPTSTR,DWORD)     = NULL;
BOOL  (WINAPI*g_pfnGetModuleInformation)(HANDLE,HMODULE,LPMODULEINFO,DWORD) = NULL;
DWORD (WINAPI*g_pfnGetModuleFileNameEx)(HANDLE,HMODULE,LPTSTR,DWORD)  = NULL;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

CModifyStyleDlg::CModifyStyleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModifyStyleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModifyStyleDlg)
	m_strWindowInfo = _T("");
	m_nPosX = 0;
	m_nPosY = 0;
	m_nSizeX = 0;
	m_nSizeY = 0;
	m_bTopMost = FALSE;
	m_strWndTxt = _T("");
	m_bFindDisabled = FALSE;
	m_bHideWindow = FALSE;
	m_bSetPos = FALSE;
	m_bSetSize = FALSE;
	m_nMessage = 0;
	m_nLPARAM = 0;
	m_uWPARAM = 0;
	//}}AFX_DATA_INIT
	// Beachten Sie, dass LoadIcon unter Win32 keinen nachfolgenden DestroyIcon-Aufruf benötigt
	m_hIcon       = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pStyle      = NULL;
	m_hModWnd     = NULL;
	m_hParentOfModWnd = NULL;
	m_hSearchCursor = NULL;
	m_nGetStyle   = GWL_STYLE;
	if (g_hPSapi == NULL)
	{
		g_hPSapi = LoadLibrary(_T("PSAPI.DLL"));                    // Dll laden
		if (g_hPSapi >= (HANDLE)32)
		{
			g_pfnEnumProcesses        = (BOOL  (WINAPI*)(DWORD*, DWORD, DWORD*))             GetProcAddress((HINSTANCE)g_hPSapi, "EnumProcesses");
			g_pfnEnumProcessModules   = (BOOL  (WINAPI*)(HANDLE,HMODULE*,DWORD,LPDWORD))     GetProcAddress((HINSTANCE)g_hPSapi, "EnumProcessModules");
			g_pfnGetModuleInformation = (BOOL  (WINAPI*)(HANDLE,HMODULE,LPMODULEINFO,DWORD)) GetProcAddress((HINSTANCE)g_hPSapi, "GetModuleInformation");
#ifdef _UNICODE
			g_pfnGetModuleBaseName    = (DWORD (WINAPI*)(HANDLE,HMODULE,LPTSTR,DWORD))       GetProcAddress((HINSTANCE)g_hPSapi, "GetModuleBaseNameW");
			g_pfnGetMappedFileName    = (DWORD (WINAPI*)(HANDLE,LPVOID,LPTSTR,DWORD))        GetProcAddress((HINSTANCE)g_hPSapi, "GetMappedFileNameW");
			g_pfnGetModuleFileNameEx  = (DWORD (WINAPI*)(HANDLE,HMODULE,LPTSTR,DWORD))       GetProcAddress((HINSTANCE)g_hPSapi, "GetModuleFileNameExW");
#else
			g_pfnGetModuleBaseName    = (DWORD (WINAPI*)(HANDLE,HMODULE,LPTSTR,DWORD))       GetProcAddress((HINSTANCE)g_hPSapi, "GetModuleBaseNameA");
			g_pfnGetMappedFileName    = (DWORD (WINAPI*)(HANDLE,LPVOID,LPTSTR,DWORD))        GetProcAddress((HINSTANCE)g_hPSapi, "GetMappedFileNameA");
			g_pfnGetModuleFileNameEx  = (DWORD (WINAPI*)(HANDLE,HMODULE,LPTSTR,DWORD))       GetProcAddress((HINSTANCE)g_hPSapi, "GetModuleFileNameExA");
#endif
		}
	}
	m_bShowChildren = false;
	m_nProcess      = -1;
	m_nChild			 = -1;
	m_bSetPos       = FALSE;
	m_bSetSize      = FALSE;
	m_bShowAllProcesses = false;
	m_bProcessesCreated = false;
}

CModifyStyleDlg::~CModifyStyleDlg()
{
   ::FreeLibrary(g_hPSapi);
   g_hPSapi = NULL;
   DeleteProcessData();
}

LRESULT CModifyStyleDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
   if (m_hSearchCursor)
   {
      if (message == WM_CAPTURECHANGED)
      {
         TRACE(_T("WM_CAPTURECHANGED\n"));
      }
   }
   return CDialog::WindowProc(message, wParam, lParam);
}

LRESULT CALLBACK CModifyStyleDlg::BtnWndFc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   if (gm_lOldBtnWndFc)
   {
      if (nMsg == WM_SETCURSOR)
      {
         CModifyStyleDlg *pThis = (CModifyStyleDlg*)::GetWindowLong(hwnd, GWL_USERDATA);
         if (pThis && pThis->m_hSearchCursor)
            return ::SendMessage(::GetParent(hwnd), nMsg, wParam, lParam);
      }
      else if (nMsg == WM_LBUTTONDOWN)
      {
         CModifyStyleDlg *pThis = (CModifyStyleDlg*)::GetWindowLong(hwnd, GWL_USERDATA);
         pThis->OnFindWindow();
      }
      else if ((nMsg == WM_MOUSEMOVE)|| (nMsg == WM_LBUTTONUP))
      {
         CPoint pt;
         CModifyStyleDlg *pThis = (CModifyStyleDlg*)::GetWindowLong(hwnd, GWL_USERDATA);
         if (pThis && pThis->m_hSearchCursor)
         {
            HWND hwndP = ::GetParent(hwnd);
            POINTSTOPOINT(pt, MAKEPOINTS(lParam));
            ::MapWindowPoints(hwnd, hwndP, &pt, 1);
            lParam = MAKELONG(pt.x,pt.y);
            return ::SendMessage(hwndP, nMsg, wParam, lParam);
         }
      }
      return CallWindowProc((WNDPROC)gm_lOldBtnWndFc, hwnd, nMsg, wParam, lParam);
   }
   return 0;
}
 
void CModifyStyleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModifyStyleDlg)
	DDX_Control(pDX, IDC_COMBO_STYLES, m_cComboStyles);
	DDX_Control(pDX, IDC_STYLE_LIST, m_cStyleList);
	DDX_Text(pDX, IDC_TXT_WND_INFO, m_strWindowInfo);
	DDX_Text(pDX, IDC_EDT_POS_X, m_nPosX);
	DDV_MinMaxInt(pDX, m_nPosX, -4000, 4000);
	DDX_Text(pDX, IDC_EDT_POS_Y, m_nPosY);
	DDV_MinMaxInt(pDX, m_nPosY, -4000, 4000);
	DDX_Text(pDX, IDC_EDT_SIZE_X, m_nSizeX);
	DDV_MinMaxInt(pDX, m_nSizeX, 1, 3000);
	DDX_Text(pDX, IDC_EDT_SIZE_Y, m_nSizeY);
	DDV_MinMaxInt(pDX, m_nSizeY, 1, 3000);
	DDX_Check(pDX, IDC_CK_TOP_MOST, m_bTopMost);
	DDX_Text(pDX, IDC_EDT_WND_TXT, m_strWndTxt);
	DDX_Check(pDX, IDC_CK_HIDE, m_bHideWindow);
	DDX_Control(pDX, IDC_WND_TREE, m_WindowTree);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModifyStyleDlg, CDialog)
	//{{AFX_MSG_MAP(CModifyStyleDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SET_STYLE, OnSetStyle)
	ON_CBN_SELCHANGE(IDC_COMBO_STYLES, OnSelchangeComboStyles)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_STYLE_LIST, OnGetdispinfoStyleList)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CLICK, IDC_STYLE_LIST, OnClickStyleList)
	ON_BN_CLICKED(IDC_KILL_PROCESS, OnKillProcess)
	ON_BN_CLICKED(IDC_GET_PROCESS, OnGetProcess)
	ON_NOTIFY(NM_DBLCLK, IDC_STYLE_LIST, OnDblclkStyleList)
	ON_NOTIFY(NM_RCLICK, IDC_STYLE_LIST, OnRclickStyleList)
	ON_BN_CLICKED(IDC_CK_FIND_DISABLED, OnCkFindDisabled)
	ON_BN_CLICKED(IDC_GET_TEXT, OnGetText)
	ON_NOTIFY(HDN_ITEMCLICK, IDC_STYLE_LIST, OnItemclickStyleList)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_STYLE_LIST, OnColumnclickStyleList)
	ON_BN_CLICKED(IDC_CK_POS, OnCkPos)
	ON_BN_CLICKED(IDC_CK_SIZE, OnCkSize)
	ON_COMMAND(ID_VIEW_PROCESSES, OnViewProcesses)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROCESSES, OnUpdateViewProcesses)
	ON_COMMAND(ID_VIEW_CHILDWINDOWS, OnViewChildwindows)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CHILDWINDOWS, OnUpdateViewChildwindows)
	ON_COMMAND(ID_VIEW_STYLE, OnViewStyle)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STYLE, OnUpdateViewStyle)
	ON_COMMAND(ID_VIEW_STYLE_EX, OnViewStyleEx)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STYLE_EX, OnUpdateViewStyleEx)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_VIEW_PROCESS_MODULES, OnViewProcessModules)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROCESS_MODULES, OnUpdateViewProcessModules)
	ON_COMMAND(ID_VIEW_ALL_PROCESSES, OnViewAllProcesses)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ALL_PROCESSES, OnUpdateViewAllProcesses)
	ON_MESSAGE(WM_NCPAINT, OnNcPaint)
	ON_BN_CLICKED(IDC_BTN_POST_MESSAGE, OnBtnPostMessage)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
	ON_COMMAND(ID_FILE_SAVE_WND_TEXT, OnFileSaveWndText)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_WND_TEXT, OnUpdateFileSaveWndText)
	ON_COMMAND(ID_VIEW_WINDOW_HIRARCHIE, OnViewWindowHirarchie)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WINDOW_HIRARCHIE, OnUpdateViewWindowHirarchie)
	ON_UPDATE_COMMAND_UI(ID_START_PROCESS, OnUpdateStartProcess)
	ON_COMMAND(ID_START_PROCESS, OnStartProcess)
	ON_NOTIFY(TVN_SELCHANGED, IDC_WND_TREE, OnTvnSelchangedWndTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModifyStyleDlg Nachrichten-Handler

BOOL CModifyStyleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Hinzufügen des Menübefehls "Info..." zum Systemmenü.

	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{	
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
   
	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden
	
   m_cStyleList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

   CString string;
   string.LoadString(IDS_STYLE_CODE);
   m_cStyleList.InsertColumn(0, string, LVCFMT_LEFT, 100);
   string.LoadString(IDS_STYLE_NAME);
   m_cStyleList.InsertColumn(1, string, LVCFMT_LEFT, 230);
	m_nHeader = IDS_STYLE_CODE;

   m_ImageList.Create(IDB_LIST_IMAGES, 13, 3, ILC_COLOR16);
   CImageList *pIL = m_cStyleList.SetImageList(&m_ImageList, LVSIL_SMALL);
   if (pIL)
   {
      pIL->DeleteImageList();
   }
   CWnd *pWnd = GetDlgItem(IDC_FIND_WINDOW);
   if (pWnd)
   {
      if (::GetWindowLong(pWnd->m_hWnd, GWL_USERDATA)==0)
      {
         gm_lOldBtnWndFc = ::SetWindowLong(pWnd->m_hWnd, GWL_WNDPROC, (long)BtnWndFc);
         ::SetWindowLong(pWnd->m_hWnd, GWL_USERDATA, (long)this);
      }
      m_hSearchCursor = AfxGetApp()->LoadCursor(IDC_SEARCH_CURSOR);
      pWnd->SendMessage(BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hSearchCursor);
      m_hSearchCursor = NULL;
   }

   m_hModWnd = m_hWnd;
   SetWindowinfo(m_hModWnd);
   m_cComboStyles.SetCurSel(0);
   OnSelchangeComboStyles();
   ::SetWindowPos(m_hWnd, HWND_TOPMOST, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
   EnableToolTips(true);
	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CModifyStyleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// Wollen Sie Ihrem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch für Sie erledigt.

void CModifyStyleDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext für Zeichnen

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Symbol in Client-Rechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// Die Systemaufrufe fragen den Cursorform ab, die angezeigt werden soll, während der Benutzer
//  das zum Symbol verkleinerte Fenster mit der Maus zieht.
HCURSOR CModifyStyleDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CModifyStyleDlg::OnSetStyle() 
{
   if (m_hModWnd && ::IsWindow(m_hModWnd) && UpdateData(true))
   {
      HWND hwnd = (m_bTopMost!=0) ? HWND_TOPMOST : HWND_NOTOPMOST;

      if ((m_iClass ==  9) && (m_nGetStyle == LVM_GETEXTENDEDLISTVIEWSTYLE))
      {
         ::SendMessage(m_hModWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, 0xffffffff, m_nStyle);
          m_nStyle = ::SendMessage(m_hModWnd, m_nGetStyle, 0, 0);
      }
      else if ((m_iClass == 12) && (m_nGetStyle == TCM_GETEXTENDEDSTYLE))
      {
         ::SendMessage(m_hModWnd, TCM_SETEXTENDEDSTYLE, 0xffffffff, m_nStyle);
         m_nStyle = ::SendMessage(m_hModWnd, m_nGetStyle, 0, 0);
      }
      else if (m_nGetStyle)
      {
         ::SetWindowLong(m_hModWnd, m_nGetStyle, m_nStyle);
         m_nStyle = ::GetWindowLong(m_hModWnd, m_nGetStyle);
      }
	  else
	  {
		  // SetClassLong ?
		 m_nStyle = ::GetClassLong(m_hModWnd, GCL_STYLE);
	  }
      if (m_hParentOfModWnd)
      {
         ::MoveWindow(m_hModWnd, m_nPosX, m_nPosY, m_nSizeX, m_nSizeY, true);
      }
      else
      {
			UINT nFlags = SWP_ASYNCWINDOWPOS;
			if (m_bHideWindow) nFlags |= SWP_HIDEWINDOW;
			else               nFlags |= SWP_SHOWWINDOW|SWP_FRAMECHANGED;
			if (!m_bSetPos)    nFlags |= SWP_NOMOVE;
			if (!m_bSetSize)   nFlags |= SWP_NOSIZE;
         ::SetWindowPos(m_hModWnd, hwnd, m_nPosX, m_nPosY, m_nSizeX, m_nSizeY, nFlags);
      }
		if (m_bSetTextViaClipboard)
		{
			if (!m_strWndTxt.IsEmpty() && OpenClipboard())
			{
				EmptyClipboard();
				HLOCAL hLMem = GlobalAlloc(GHND, (m_strWndTxt.GetLength()+1)*sizeof(_TCHAR));
				if (hLMem)
				{
					_tcscpy((_TCHAR*)GlobalLock(hLMem), m_strWndTxt);
					GlobalUnlock(hLMem);
#ifdef _UNICODE
					if (::SetClipboardData(CF_UNICODETEXT, hLMem) == NULL)
#else
					if (::SetClipboardData(CF_TEXT, hLMem) == NULL)
#endif
					{
						LocalFree(hLMem);
						hLMem = NULL;
					}
				}
				CloseClipboard();
				if (hLMem)
				{
					::SendMessage(m_hModWnd, EM_SETSEL, 0, (LPARAM)-1);
					::SendMessage(m_hModWnd, WM_PASTE, 0, 0);
				}
			}
		}
		else
		{
			::SetWindowText(m_hModWnd, m_strWndTxt);
		}
      ::UpdateWindow(m_hModWnd);
      ::InvalidateRect(m_hModWnd, NULL, true);
      m_cStyleList.InvalidateRect(NULL);
   }
}

void CModifyStyleDlg::OnSelchangeComboStyles() 
{
	int nSel = m_cComboStyles.GetCurSel();
	if (nSel != CB_ERR)
	{
		m_nGetStyle   = GWL_STYLE;
		switch (nSel)
		{
			case WND_STYLE:					m_pStyle = gm_ListStyle;       break;
			case WND_STYLE_EX:				m_pStyle = gm_ListStyleEx; m_nGetStyle = GWL_EXSTYLE; break;
			case EDIT_STYLE:				m_pStyle = gm_ListStyleEdit;   break;
			case BUTTON_STYLE:				m_pStyle = gm_ListStyleBtn;    break;
			case STATIC_STYLE:				m_pStyle = gm_ListStyleStatic; break;
			case DIALOG_STYLE:				m_pStyle = gm_ListStyleDlg;    break;
			case LISTBOX_STYLE:				m_pStyle = gm_ListStyleLB;     break;
			case COMBOBOX_STYLE:			m_pStyle = gm_ListStyleCB;     break;
			case SCROLLBAR_STYLE:			m_pStyle = gm_ListStyleSBAR;   break;
			case SYSLISTVIEW32_STYLE:		m_pStyle = gm_ListStyleLV;     break;
			case SYSLISTVIEW32_EX:			m_pStyle = gm_ListStyleLVEx;m_nGetStyle = LVM_GETEXTENDEDLISTVIEWSTYLE;  break;
			case SYSTREEVIEW32_STYLE:		m_pStyle = gm_ListStyleTV;     break;
			case SYSTABCONTROL32_STYLE:		m_pStyle = gm_ListStyleTC;     break;
			case SYSTABCONTROL32_EX_STYLE:	m_pStyle = gm_ListStyleTCEx;m_nGetStyle = TCM_GETEXTENDEDSTYLE;   break;
			case TOOLBARWINDOW32_STYLE:		m_pStyle = gm_ListStyleTB;     break;
			case MSCTLS_UPDOWN32_STYLE:		m_pStyle = gm_ListStyleUDC;    break;
			case CLASS_STYLE:				m_pStyle = gm_ListClassStyle;m_nGetStyle = 0; break;
			default:m_pStyle = NULL;               break;
		}

		if (m_hModWnd && ::IsWindow(m_hModWnd))
		{
			if (((m_iClass ==  9) && (m_nGetStyle == LVM_GETEXTENDEDLISTVIEWSTYLE)) ||
				((m_iClass == 12) && (m_nGetStyle == TCM_GETEXTENDEDSTYLE)))
			{
				m_nStyle = ::SendMessage(m_hModWnd, m_nGetStyle, 0, 0);
			}
			else if (m_nGetStyle)
			{
				m_nStyle = ::GetWindowLong(m_hModWnd, m_nGetStyle);
			}
			else
			{
				m_nStyle = ::GetClassLong(m_hModWnd, GCL_STYLE);
			}
		}

		CString str;
		if (m_pStyle)
		{
			int i;
			for (i=0; m_pStyle[i].pszStyle != NULL; i++)
			{
				ChangeListHeader(IDS_STYLE_CODE);
			}
			m_cStyleList.SetItemCount(i);
			m_cStyleList.InvalidateRect(NULL);
			str.LoadString(IDS_BTNTXT_CLOSE_WINDOW);
			SetDlgItemText(IDC_KILL_PROCESS, str);
		}
	}
}

void CModifyStyleDlg::OnGetdispinfoStyleList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   if (m_pStyle)
   {
      if(pDispInfo->item.mask & LVIF_TEXT)
      {
         switch (pDispInfo->item.iSubItem)
         {
            case 0:
               wsprintf(pDispInfo->item.pszText, _T("%08x"), m_pStyle[pDispInfo->item.iItem].nStyle);
               break;
            case 1:
				_tcsncpy(pDispInfo->item.pszText, m_pStyle[pDispInfo->item.iItem].pszStyle, pDispInfo->item.cchTextMax);
               break;
         }      
      }
      if (pDispInfo->item.mask & LVIF_IMAGE)
      {
         long i, nBits = 0;
         long nStyle = m_pStyle[pDispInfo->item.iItem].nStyle;
         for (i=1; i!=0; i<<=1)
         {
            if (nStyle & i) nBits++;
            if (nBits>1) break;
         }
         if (nBits == 1) pDispInfo->item.iImage = ((m_nStyle & nStyle) !=      0) ? 1 : 0;
         else            pDispInfo->item.iImage = ((m_nStyle & nStyle) == nStyle) ? 1 : 0;
      }
   }
   else if (m_bShowChildren && (m_nProcess != -1) && (m_nProcess < m_Process.GetCount()))
   {
      POSITION pos = m_Process.FindIndex(m_nProcess);
      ProcessData *pP = NULL;
      if (pos) pP = (ProcessData*) m_Process.GetAt(pos);
      if (pP)
      {
         pos = pP->modules.FindIndex(pDispInfo->item.iItem);
      }
      if (pos) pP = (ProcessData*) pP->modules.GetAt(pos);
      if((pDispInfo->item.mask & LVIF_TEXT) && (pP))
      {
         switch (pDispInfo->item.iSubItem)
         {
            case 0:
               _tcsncpy(pDispInfo->item.pszText, pP->szName, pDispInfo->item.cchTextMax);
               break;
            case 1:
               _tcsncpy(pDispInfo->item.pszText, pP->szPath, pDispInfo->item.cchTextMax);
               break;
         }      
      }
   }
   else if (pDispInfo->item.iItem < m_Process.GetCount())
   {
      POSITION pos = m_Process.FindIndex(pDispInfo->item.iItem);
      ProcessData *pP = NULL;
      if (pos) pP = (ProcessData*) m_Process.GetAt(pos);
      if(pDispInfo->item.mask & LVIF_TEXT)
      {
         switch (pDispInfo->item.iSubItem)
         {
            case 0:
               _tcsncpy(pDispInfo->item.pszText, pP->szName, pDispInfo->item.cchTextMax);
               break;
            case 1:
               _tcsncpy(pDispInfo->item.pszText, pP->szPath, pDispInfo->item.cchTextMax);
               break;
         }      
      }
   }
	else if (pDispInfo->item.iItem < m_Children.GetCount())
	{
      POSITION pos = m_Children.FindIndex(pDispInfo->item.iItem);
      ChildrenData *pC = NULL;
      if (pos) pC = (ChildrenData*) m_Children.GetAt(pos);
      if(pDispInfo->item.mask & LVIF_TEXT)
      {
         switch (pDispInfo->item.iSubItem)
         {
            case 0:
				{
               _stprintf(pDispInfo->item.pszText, _T("%x"), pC->hwndChild);
				}break;
            case 1:
               _tcsncpy(pDispInfo->item.pszText, pC->sName, pDispInfo->item.cchTextMax);
               break;
         }      
      }
	}

	*pResult = 0;
}

struct HitChildStruct
{
   HitChildStruct(HWND hwndP, CPoint&pt)
   {
      hwndReturn = NULL;
      hwndParent = hwndP;
      ptCursor   = pt;
      rcFirst.SetRectEmpty();
   }  

   HWND   hwndReturn;
   HWND   hwndParent;
   CRect  rcFirst;
   CPoint ptCursor;
};

BOOL CALLBACK CModifyStyleDlg::HitChildRect(HWND hwnd, LPARAM lParam)
{
   CRect rcCurrent;
   HitChildStruct *pS = (HitChildStruct*)lParam;
   ::GetWindowRect(hwnd, rcCurrent);
//   ::ScreenToClient(pS->hwndParent, &((POINT*)&rcCurrent)[0]);
//   ::ScreenToClient(pS->hwndParent, &((POINT*)&rcCurrent)[1]);

   if (!pS->rcFirst.IsRectEmpty())
   {
      if (rcCurrent.PtInRect(pS->ptCursor) && 
          pS->rcFirst.PtInRect(rcCurrent.TopLeft()) &&
          pS->rcFirst.PtInRect(rcCurrent.BottomRight()))
      {
         pS->hwndReturn = hwnd;
      }
      return 1;
   }
   if (rcCurrent.PtInRect(pS->ptCursor))
   {
      pS->hwndReturn = hwnd;
      pS->rcFirst    = rcCurrent;
   }
   return 1;
}

void CModifyStyleDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
   if (m_hSearchCursor)
   {
      SetCursor(m_hSearchCursor);
      CPoint ptScreen(point);
      if (!(nFlags & 0x00010000))
         ClientToScreen(&ptScreen);
      HWND hModWnd = ::WindowFromPoint(ptScreen);
      if (hModWnd)
      {
         HWND hParent = ::GetParent(hModWnd);
         _TCHAR szClass[MAX_PATH];
         ::GetClassName(hParent, szClass, MAX_PATH);
         if (m_bFindDisabled || (_tcscmp(szClass, gm_szClasses[5])==0)) // Dialog
         {
            HitChildStruct hcs(hParent, ptScreen);
            if (hParent) ::EnumChildWindows(hParent, HitChildRect, (LPARAM)&hcs);
            HWND hChild  = hcs.hwndReturn;
            if (hChild) hModWnd = hChild;
         }
      }
      SetWindowinfo(hModWnd);
   }
	
	CDialog::OnMouseMove(nFlags, point);
}

void CModifyStyleDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
   if (m_hSearchCursor)
   {
      m_hSearchCursor = NULL;
      ReleaseCapture();
      CPoint ptScreen(point);
      ClientToScreen(&ptScreen);
      HWND hModWnd = ::WindowFromPoint(ptScreen);
      if (hModWnd)
      {
         HWND hParent = ::GetParent(hModWnd);
         _TCHAR szClass[MAX_PATH];
         ::GetClassName(hParent, szClass, MAX_PATH);
         if (m_bFindDisabled || (_tcscmp(szClass, gm_szClasses[5])==0)) // Dialog
         {
            HitChildStruct hcs(hParent, ptScreen);
            if (hParent) ::EnumChildWindows(hParent, HitChildRect, (LPARAM)&hcs);
            HWND hChild  = hcs.hwndReturn;
            if (hChild && (hChild != hModWnd))
            {
               m_hParentOfModWnd = hModWnd;
               hModWnd = hChild;
            }
            else
            {
               m_hParentOfModWnd = ::GetParent(hModWnd);
            }
         }
         else
         {
            m_hParentOfModWnd = ::GetParent(hModWnd);
         }
         m_hModWnd = hModWnd;
      }
      SetWindowinfo(m_hModWnd);
      OnSelchangeComboStyles();
      m_cStyleList.InvalidateRect(NULL);
   }	
	CDialog::OnLButtonUp(nFlags, point);
}

void CModifyStyleDlg::SetWindowinfo(HWND hwnd)
{
	if (hwnd)
	{
		CString str;
		HWND hwndP = ::GetParent(hwnd);
		_TCHAR szClass[MAX_PATH];
		_TCHAR szText[MAX_PATH] = _T("");
		DWORD dwThreadId, m_dwProcID, dwCtrlID, dwHelpID, dwStyle, dwExStyle;
		::GetClassName( hwnd, szClass, MAX_PATH);
		for (int i=0; gm_szClasses[i] != NULL; i++)
		{
			if (_tcscmp(szClass, gm_szClasses[i]) == 0)
			{
				m_cComboStyles.SetCurSel(i);
				m_iClass = i;
				break;
			}
		}

		if (gm_szClasses[i] == NULL)
		{
			m_cComboStyles.SetCurSel(0);
		}

		::GetWindowText(hwnd, szText , MAX_PATH);
		m_bSetTextViaClipboard = false;
		if (szText[0] == 0)
		{
			m_strWndTxt.Empty();
			if (OpenClipboard())
			{
				EmptyClipboard();
				CloseClipboard();
			}
			::SendMessage(hwnd, EM_SETSEL, 0, (LPARAM)-1);
			::SendMessage(hwnd, WM_COPY, 0, 0);
			if (OpenClipboard())
			{
#ifdef _UNICODE
				_TCHAR * text= (_TCHAR*)::GetClipboardData(CF_UNICODETEXT);
#else
				_TCHAR * text= (_TCHAR*)::GetClipboardData(CF_TEXT);
#endif
				if (text)
				{
					m_strWndTxt = text;
					m_bSetTextViaClipboard = true;
				}
				CloseClipboard();
			}
		}
		else
		{
			m_strWndTxt = szText;
		}

		dwCtrlID    = ::GetWindowLong(hwnd, GWL_ID);
		dwStyle     = ::GetWindowLong(hwnd, GWL_STYLE);
		dwExStyle   = ::GetWindowLong(hwnd, GWL_EXSTYLE);
		dwHelpID    = ::GetWindowContextHelpId(hwnd);
		dwThreadId  = ::GetWindowThreadProcessId(hwnd, &m_dwProcID);
		GetDlgItem(IDC_KILL_PROCESS)->EnableWindow(true);
		m_strWindowInfo.Format(IDS_WND_INFO, hwnd, dwThreadId, m_dwProcID, dwStyle, dwExStyle, dwCtrlID, dwCtrlID, dwHelpID, dwHelpID, szClass);
		if (m_hParentOfModWnd == NULL)
		{
			m_strWindowInfo += _T("\r\nFileName : ") + GetFileNameFromWindowHandle(hwnd) + _T("\r\n");
		}
		if (hwndP)
		{
			::GetClassName( hwndP, szClass, MAX_PATH);
			::GetWindowText(hwndP, szText , MAX_PATH);
			dwCtrlID   = ::GetWindowLong(hwndP, GWL_ID);
			dwHelpID   = ::GetWindowContextHelpId(hwndP);
			dwThreadId = ::GetWindowThreadProcessId(hwndP, &m_dwProcID);
			dwStyle    = ::GetWindowLong(hwndP, GWL_STYLE);
			dwExStyle  = ::GetWindowLong(hwndP, GWL_EXSTYLE);
			str.Format(IDS_WND_INFO, hwndP, dwThreadId, m_dwProcID, dwStyle, dwExStyle, dwCtrlID, dwCtrlID, dwHelpID, dwHelpID, szClass);
			m_strWindowInfo += _T("\r\n\r\nParent : ");
			m_strWindowInfo += str;
		}
		m_nStyle =::GetWindowLong(hwnd, m_nGetStyle);
		CRect rc;
		::GetWindowRect(hwnd, &rc);
		if (m_hParentOfModWnd)
		{
			CWnd::FromHandle(m_hParentOfModWnd)->ScreenToClient(&rc);
			GetDlgItem(IDC_CK_TOP_MOST)->EnableWindow(false);
			GetDlgItem(IDC_CK_HIDE)->EnableWindow(false);
		}
		else
		{
			GetDlgItem(IDC_CK_TOP_MOST)->EnableWindow(true);
			CDataExchange dx(this, FALSE);
			m_bTopMost = dwExStyle & WS_EX_TOPMOST ? TRUE : FALSE;
			DDX_Check(&dx, IDC_CK_TOP_MOST, m_bTopMost);
			GetDlgItem(IDC_CK_HIDE)->EnableWindow(true);
		}

		m_nPosX  = rc.left;
		m_nPosY  = rc.top;
		m_nSizeX = rc.Width();
		m_nSizeY = rc.Height();
		UpdateData(false);
	}
}

BOOL CModifyStyleDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CModifyStyleDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 500)
	{
		POSITION pos = m_Process.GetHeadPosition();
		if (pos)
		{
			ProcessData*pPD = (ProcessData*) m_Process.GetAt(pos);
			FILETIME ftCreate, ftExit, ftKernel, ftUser;
			GetProcessTimes(pPD->hHandle, &ftCreate, &ftExit, &ftKernel, &ftUser);
			TRACE(_T("t:%d, %d\n"),ftKernel.dwLowDateTime, ftUser.dwLowDateTime);
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CModifyStyleDlg::OnDestroy() 
{
	m_ImageList.DeleteImageList();
	CDialog::OnDestroy();
}

void CModifyStyleDlg::OnClickStyleList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   CPoint ptCursor, ptScreen;
   UINT Flags;
   LV_ITEM lvItem = { LVIF_STATE|LVIF_PARAM|LVIF_IMAGE, -1, 0, 0, LVIS_SELECTED, NULL, 0, 0, NULL};
   GetCursorPos(&ptCursor);
   ptScreen = ptCursor;
   m_cStyleList.ScreenToClient(&ptCursor);
   ptCursor.x = 2;

   lvItem.iItem = m_cStyleList.HitTest(ptCursor, &Flags);
   if ((lvItem.iItem != -1) && (Flags & LVHT_ONITEMICON))
   {
      if (m_pStyle)
      {
         if (m_pStyle[lvItem.iItem].nStyle & m_nStyle) m_nStyle &= ~m_pStyle[lvItem.iItem].nStyle;
         else                                          m_nStyle |=  m_pStyle[lvItem.iItem].nStyle;
         m_cStyleList.InvalidateRect(NULL);
      }
      else if (lvItem.iItem < m_Process.GetCount())
      {
         GetDlgItem(IDC_KILL_PROCESS)->EnableWindow(true);
         m_nProcess = lvItem.iItem;
      }
      else if (lvItem.iItem < m_Children.GetCount())
      {
         m_nChild = lvItem.iItem;
      }
   }

	*pResult = 0;
}

void CModifyStyleDlg::OnDblclkStyleList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (m_pStyle == NULL)
   {
		ToggleProcess();
		
      if ((m_nChild!= -1) && (m_nChild < m_Children.GetCount()))
		{
			POSITION pos = m_Children.FindIndex(m_nChild);
			ChildrenData *pC = NULL;
         if (pos) pC = (ChildrenData*) m_Children.GetAt(pos);
			m_hParentOfModWnd = ::GetParent(pC->hwndChild);
			m_hModWnd         = pC->hwndChild;
			ChangeListHeader(IDS_STYLE_CODE);
			SetWindowinfo(pC->hwndChild);
			OnSelchangeComboStyles();
			m_cStyleList.InvalidateRect(NULL);
		}
   }
	
	*pResult = 0;
}

void CModifyStyleDlg::ToggleProcess()
{
   if ((m_nProcess != -1) && (m_nProcess < m_Process.GetCount()))
   {
      m_bShowChildren = ! m_bShowChildren;
      if (m_bShowChildren)
      {
         POSITION pos = m_Process.FindIndex(m_nProcess);
         ProcessData *pP = NULL;
         if (pos) pP = (ProcessData*) m_Process.GetAt(pos);
         m_cStyleList.SetItemCount(pP->modules.GetCount());
         GetDlgItem(IDC_KILL_PROCESS)->EnableWindow(false);
			ChangeListHeader(IDS_PROCESS_MODULES, pP->szName);
      }
      else
      {
         m_cStyleList.SetItemCount(m_Process.GetCount());
         GetDlgItem(IDC_KILL_PROCESS)->EnableWindow(true);
			ChangeListHeader(IDS_PROCESS_NAME);
      }
      m_cStyleList.InvalidateRect(NULL);
   }
}

void CModifyStyleDlg::OnRclickStyleList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   CPoint ptCursor, ptScreen;
   UINT Flags;
   LV_ITEM lvItem = { LVIF_STATE|LVIF_PARAM|LVIF_IMAGE, -1, 0, 0, LVIS_SELECTED, NULL, 0, 0, NULL};
   GetCursorPos(&ptCursor);
   ptScreen = ptCursor;
   m_cStyleList.ScreenToClient(&ptCursor);
   ptCursor.x = 2;

   lvItem.iItem = m_cStyleList.HitTest(ptCursor, &Flags);
   if ((lvItem.iItem != -1) && (Flags & LVHT_ONITEMICON))
   {
      if (m_pStyle)
      {
         CString str;
         str.Format(IDS_ASK_COPY_STRING, m_pStyle[lvItem.iItem].pszStyle);
         if (AfxMessageBox(str, MB_YESNO|MB_ICONQUESTION) == IDYES)
         {
            int nErrorg = 0;
            HGLOBAL hGlobal = NULL;
            try
            {
               if (!OpenClipboard())                throw (int)1;
			   EmptyClipboard();
               hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, (_tcslen(m_pStyle[lvItem.iItem].pszStyle)+1)*sizeof(_TCHAR));
               _TCHAR * pszString = (_TCHAR*) ::GlobalLock(hGlobal);
               if (pszString == NULL)                       throw (int)2;
               _tcscpy(pszString, m_pStyle[lvItem.iItem].pszStyle);
               ::GlobalUnlock(hGlobal);
#ifdef _UNICODE
               if (!::SetClipboardData(CF_UNICODETEXT, pszString)) throw (int)4;
#else
               if (!::SetClipboardData(CF_TEXT, pszString)) throw (int)4;
#endif
            }
            catch (int nError)
            {
               str.Format(IDS_ERROR_COPY_STRING, nError);
               AfxMessageBox(str, MB_OK|MB_ICONERROR);
               nErrorg = nError;
            }
            CloseClipboard();
            if (nErrorg)
            {
               if (hGlobal) GlobalFree(hGlobal);
            }
         }
      }
   }
	*pResult = 0;
}

void CModifyStyleDlg::OnKillProcess() 
{
   if (m_pStyle)
   {
      if (m_hModWnd && ::IsWindow(m_hModWnd))
      {
         CString  str;
         str.Format(IDS_ASK_CLOSE_WND, m_strWndTxt);
         if (AfxMessageBox(str, MB_YESNO|MB_ICONQUESTION) == IDYES)
         {
            if (!::DestroyWindow(m_hModWnd))
            {
               ::PostMessage(m_hModWnd, WM_CLOSE, 0, 0);
               AfxMessageBox(IDS_WM_CLOSE_SENT, MB_OK|MB_ICONEXCLAMATION);
            }
            else
            {
               AfxMessageBox(IDS_WINDOW_CLOSED, MB_OK|MB_ICONEXCLAMATION);
            }
         }
      }
   }
   else
   {
      int nSel = m_cStyleList.GetSelectionMark();
      if (nSel != -1)
      {
         POSITION pos = m_Process.FindIndex(nSel);
         if (pos)
         {
            ProcessData *pDx, *pD = (ProcessData*)m_Process.GetAt(pos);
				
            CString  str;
            str.Format(IDS_ASK_KILL_PROCESS, pD->szName);
            if (AfxMessageBox(str, MB_YESNO|MB_ICONQUESTION) == IDYES)
            {
               if (::TerminateProcess(pD->hHandle, 0xffffffff))
               {
                  m_Process.RemoveAt(pos);
                  CloseHandle(pD->hHandle);
                  while (pD->modules.GetCount())
                  {
                     pDx = (ProcessData*)pD->modules.RemoveHead();
                     delete pDx;
                  }
                  delete pD;
                  m_cStyleList.SetItemCount(m_Process.GetCount());
                  m_cStyleList.InvalidateRect(NULL);
                  AfxMessageBox(IDS_PROCESS_KILLED, MB_OK|MB_ICONEXCLAMATION);
               }
            }
         }
      }
   }
}

void AdjustDacl(HANDLE h, DWORD dwDesiredAccess)
{
    wprintf( L"Attempting to adjust process DACL..." );

    // the WORLD Sid is trivial to form programmatically (S-1-1-0)
    SID world = { SID_REVISION, 1, SECURITY_WORLD_SID_AUTHORITY, 0 };

    EXPLICIT_ACCESS ea =
    {
        dwDesiredAccess,
        SET_ACCESS,
        NO_INHERITANCE,
        {
            0, NO_MULTIPLE_TRUSTEE,
            TRUSTEE_IS_SID,
            TRUSTEE_IS_USER,
            reinterpret_cast<_TCHAR*>( &world )
        }
    };
    ACL* pdacl = 0;
    DWORD err = SetEntriesInAcl( 1, &ea, 0, &pdacl );
    if ( err ) return;

    err = SetSecurityInfo( h, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION,
                           0, 0, pdacl, 0 );
    if ( err ) return;


    LocalFree( pdacl );
}

bool EnablePrivilege( HANDLE htok, LPCTSTR pszPriv,
                      TOKEN_PRIVILEGES& tpOld )
{
    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if ( !LookupPrivilegeValue( 0, pszPriv, &tp.Privileges[0].Luid ) )
		 return false;

    // htok must have been opened with the following permissions:
    // TOKEN_QUERY (to get the old priv setting)
    // TOKEN_ADJUST_PRIVILEGES (to adjust the priv)
    DWORD cbOld = sizeof tpOld;
    if ( !AdjustTokenPrivileges( htok, FALSE, &tp, cbOld, &tpOld, &cbOld ) )
		 return false;

    // Note that AdjustTokenPrivileges may succeed, and yet
    // some privileges weren't actually adjusted.
    // You've got to check GetLastError() to be sure!
    return ( ERROR_NOT_ALL_ASSIGNED != GetLastError() );
}
 
// Corresponding restoration helper function
void RestorePrivilege( HANDLE htok, const TOKEN_PRIVILEGES& tpOld )
{
    if ( !AdjustTokenPrivileges( htok, FALSE,
                                 const_cast<TOKEN_PRIVILEGES*>(&tpOld),
                                 0, 0, 0 ) )
		return;
}

void CModifyStyleDlg::OnGetProcess() 
{
   if (g_hPSapi)
   {
      const DWORD dwSize = 1024;
      DWORD  i, j, nM, nP, dwNeeded = 0;
      DWORD  dwProcess[dwSize];
      HANDLE hProcess;
      int    nlen = 255;
      _TCHAR szBaseName[256] = _T("");
	  GetUserName(szBaseName, (DWORD*)&nlen);
	  CString sUser = szBaseName;
      ProcessData *pP = NULL;
      DeleteProcessData();
	  BOOL bResult = g_pfnEnumProcesses(&dwProcess[0], dwSize*sizeof(DWORD), &dwNeeded);
      nP = dwNeeded / sizeof(DWORD);
      for (i=0; i<nP; i++)
      {
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, dwProcess[i]);
		if (!hProcess && 	m_bShowAllProcesses)
		{
			DWORD dwError = GetLastError();
			if (dwError == 5)
			{
				// oh well, we need to grant ourselves PROCESS_TERMINATE
				HANDLE hpWriteDAC = OpenProcess( WRITE_DAC, FALSE, dwProcess[i]);
				if (!hpWriteDAC )
				{
						// hmm, we don't have permissions to modify the DACL...
						// time to take ownership...
					HANDLE htok;
					TOKEN_PRIVILEGES tpOld;
					if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &htok ))
					{
						continue;
					}

					if (!EnablePrivilege(htok, SE_TAKE_OWNERSHIP_NAME, tpOld))
					{
						continue;
					}

					// SeTakeOwnershipPrivilege allows us to open objects with
					// WRITE_OWNER, but that's about it, so we'll update the owner,
					// and dupe the handle so we can get WRITE_DAC permissions.
					HANDLE hpWriteOwner = OpenProcess( WRITE_OWNER, FALSE, dwProcess[i]);
					if (!hpWriteOwner)
					{
						CloseHandle(htok);
						continue;
					}

					BYTE buf[512]; // this should always be big enough
					DWORD cb = sizeof buf;
					if (!GetTokenInformation( htok, TokenUser, buf, cb, &cb ))
					{
						continue;
					}

					if (SetSecurityInfo(hpWriteOwner, SE_KERNEL_OBJECT, OWNER_SECURITY_INFORMATION, reinterpret_cast<TOKEN_USER*>(buf)->User.Sid, 0, 0, 0 ) != 0)
					{
						continue;
					}
					// now that we're the owner, we've implicitly got WRITE_DAC
					// permissions, so ask the system to reevaluate our request,
					// giving us a handle with WRITE_DAC permissions
					if (!DuplicateHandle(GetCurrentProcess(), hpWriteOwner, GetCurrentProcess(), &hpWriteDAC, WRITE_DAC, FALSE, 0 ))
					{
						continue;
					}
						// not truly necessary in this app,
						// but included for completeness
					RestorePrivilege( htok, tpOld );
					CloseHandle(htok);
					CloseHandle(hpWriteDAC);
                    hpWriteDAC = NULL;
				}

				if ( hpWriteDAC )
				{
					// we've now got a handle that allows us WRITE_DAC permission
					DWORD dwDesiredAccess = PROCESS_ALL_ACCESS;
					AdjustDacl(hpWriteDAC, dwDesiredAccess);
					// now that we've granted ourselves permission to terminate
					// the process, ask the system to reevaluate our request,
					// giving us a handle with PROCESS_TERMINATE permissions
					DuplicateHandle(GetCurrentProcess(), hpWriteDAC, GetCurrentProcess(), &hProcess, dwDesiredAccess, FALSE, 0);
					CloseHandle(hpWriteDAC);
				}
			}
		}
         if (hProcess)
         {
			HMODULE hModules[dwSize];
			nlen = g_pfnGetModuleBaseName(hProcess, NULL,  szBaseName, 256);
			if (!m_bShowAllProcesses)
			{
				HANDLE hToken;
				BYTE bBufer[100];
				TOKEN_OWNER *pto = (TOKEN_OWNER*)bBufer;

				SID_NAME_USE SidType;
				DWORD dwSize = 0, dwResult = 0;
				_TCHAR lpName[MAX_PATH];
				_TCHAR lpDomain[MAX_PATH];

				if (OpenProcessToken(hProcess, TOKEN_QUERY, &hToken ))
				{
					dwSize = 100;
					if (GetTokenInformation(hToken, TokenOwner, pto, dwSize, &dwSize))
					{
						LookupAccountSid( NULL, pto->Owner, lpName, &dwSize, lpDomain, &dwSize, &SidType);
						if (sUser != lpName)
						{
							nlen = 0;
						}
					}
					CloseHandle(hToken);
				}
			}
            if (nlen)
            {
               pP = new ProcessData;
               pP->szName  = szBaseName;
               pP->hHandle =  hProcess;
					pP->dwID    =  dwProcess[i];
               nlen = g_pfnGetModuleInformation(hProcess, NULL, &pP->mi, sizeof(MODULEINFO));
               nlen = g_pfnGetModuleFileNameEx(hProcess, NULL, szBaseName, 256); 
               if (nlen) pP->szPath = szBaseName;
               m_Process.AddTail(pP);
               m_pStyle = NULL;
            }
			else
			{
				CloseHandle(hProcess);
				continue;
			}
            nlen = g_pfnEnumProcessModules(hProcess, &hModules[0], dwSize*sizeof(HMODULE),&dwNeeded);
            if (nlen) nM = dwNeeded / sizeof(HMODULE);
            else nM = 0;
            
            for (j=1; j<nM; j++)
            {
               nlen = g_pfnGetModuleBaseName(hProcess, hModules[j], szBaseName, 256); 
               if (nlen)
               {
                  ProcessData *pPx;
                  pPx = new ProcessData;
                  pPx->szName  = szBaseName;
                  pPx->hHandle = hModules[j];
                  nlen = g_pfnGetModuleInformation(hProcess, hModules[j], &pPx->mi, sizeof(MODULEINFO));
                  nlen = g_pfnGetModuleFileNameEx( hProcess, hModules[j], szBaseName, 256); 
                  if (nlen) pPx->szPath = szBaseName;
                  pP->modules.AddTail(pPx);
               }
            }
		 }
      }

      if (m_pStyle == NULL)
      {
		 ChangeListHeader(IDS_PROCESS_NAME);
         m_cStyleList.SetItemCount(m_Process.GetCount());
         m_cStyleList.InvalidateRect(NULL);
         SendDlgItemMessage(IDC_COMBO_STYLES, CB_SETCURSEL, -1, 0);
//		 SetTimer(500, 1000, NULL);
      }
   }
}

void CModifyStyleDlg::DeleteProcessData()
{
   ProcessData *pD, *pDx;
   while (m_Process.GetCount())
   {
      pD = (ProcessData*)m_Process.RemoveHead();
      CloseHandle(pD->hHandle);
      while (pD->modules.GetCount())
      {
         pDx = (ProcessData*)pD->modules.RemoveHead();
         delete pDx;
      }
      delete pD;
   }
	ChildrenData *pC;
   while (m_Children.GetCount())
   {
      pC = (ChildrenData*)m_Children.RemoveHead();
      delete pC;
   }
}

void CModifyStyleDlg::OnFindWindow() 
{
   if (m_hSearchCursor == NULL)
   {
      m_hSearchCursor = AfxGetApp()->LoadCursor(IDC_SEARCH_CURSOR);
      CWnd *pWnd = m_cComboStyles.SetCapture();
      m_hParentOfModWnd = NULL;
   }
}

LRESULT CModifyStyleDlg::OnNcPaint(WPARAM wParam, LPARAM lParam)
{
   Default();
/*   
   HDC hdc;
   hdc = ::GetWindowDC(m_hWnd);
   TextOut(hdc, 5, 5, _T("Test"), 4);
   ::ReleaseDC(m_hWnd, hdc);
*/
   return 0;
}

BOOL CModifyStyleDlg::OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
   // need to handle both ANSI and UNICODE versions of the message
   TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;
   CString strTipText;
   UINT nID = pNMHDR->idFrom;
   if (pNMHDR->code == TTN_NEEDTEXT && (pTTT->uFlags & TTF_IDISHWND))
   {
      // idFrom is actually the HWND of the tool
      nID = ::GetDlgCtrlID((HWND)nID);
   }

   if (nID != 0)
   {// will be zero on a separator
      if ((nID == IDC_STYLE_LIST) && (m_pStyle == NULL))
         nID = IDC_PROCESS_LIST;

	  pTTT->lpszText = MAKEINTRESOURCE(nID);
      pTTT->hinst = AfxGetResourceHandle();
      return(TRUE);
   }

   _tcsncpy(pTTT->szText, strTipText, sizeof(pTTT->szText));

   *pResult = 0;

   return TRUE;    // message was handled
}

void CModifyStyleDlg::OnCkFindDisabled() 
{
	CDataExchange dx(this, true);
	DDX_Check(&dx, IDC_CK_FIND_DISABLED, m_bFindDisabled);
}

void CModifyStyleDlg::OnGetText() 
{
	if (m_hModWnd && ::IsWindow(m_hModWnd))
	{
		_TCHAR szText[256];
		ListView_GetItemText(m_hModWnd, 0, 0, szText, 256);
	}
}
CString CModifyStyleDlg::GetFileNameFromWindowHandle(HWND hWnd)
{
	CString sFileName;
	if (hWnd)
	{
		DWORD dwPID = 0;
		GetWindowThreadProcessId(hWnd, &dwPID);
		if (dwPID != 0)
		{
			HANDLE  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
			if (hProcess)
			{
				HMODULE hModule = NULL;
				DWORD dwNeeded  = sizeof(hModule);
				if (EnumProcessModules(hProcess, &hModule, sizeof(hModule), &dwNeeded))
				{
					_TCHAR szFileName[_MAX_PATH] = {0};
					if (GetModuleFileNameEx(hProcess, hModule, szFileName, sizeof(szFileName)))
					{
						sFileName = szFileName;
					}
				}
				CloseHandle(hProcess);
			}
		}
	}
	return sFileName;
}				

BOOL CALLBACK CModifyStyleDlg::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	CModifyStyleDlg *pThis = (CModifyStyleDlg*)lParam;
	_TCHAR szText[MAX_PATH] = _T("");
	::GetClassName(hwnd, szText, MAX_PATH-1);
	_tcscat(szText, _T(": "));
	int nLen = _tcslen(szText);
	::GetWindowText(hwnd, &szText[nLen], MAX_PATH-nLen);
	pThis->m_Children.AddHead(new ChildrenData(hwnd, szText));
	return 1;
}

BOOL CALLBACK CModifyStyleDlg::EnumFirstLevelChildren(HWND hwnd, LPARAM lParam)
{
	CModifyStyleDlg *pThis = (CModifyStyleDlg*)lParam;
	if (::GetParent(hwnd) == pThis->m_hParentOfModWnd)
	{
		CString sWndName, sClassName;
		int nLen = ::GetClassName(hwnd, sClassName.GetBufferSetLength(MAX_PATH), MAX_PATH);
		sClassName.ReleaseBuffer(nLen);
		CWnd::FromHandle(hwnd)->GetWindowText(sWndName);
		sClassName += _T(":") + sWndName;
		HTREEITEM hNew = pThis->m_WindowTree.InsertItem(sClassName, pThis->m_hCurrentTreeItem);
		pThis->m_WindowTree.SetItemData(hNew, (DWORD_PTR)hwnd);
		HTREEITEM	hCurrentTI  = pThis->m_hCurrentTreeItem;
		HWND		hCurrentWnd = pThis->m_hParentOfModWnd;
		pThis->m_hCurrentTreeItem = hNew;
		pThis->m_hParentOfModWnd  = hwnd;
		EnumChildWindows(hwnd, EnumFirstLevelChildren, (LPARAM)pThis);
		pThis->m_hCurrentTreeItem = hCurrentTI;
		pThis->m_hParentOfModWnd  = hCurrentWnd;
	}
	return TRUE;
}

BOOL CALLBACK CModifyStyleDlg::EnumDesktopWindowsProc(HWND hwnd, LPARAM lParam)
{
	DWORD dwThreadID, dwProcessID, *pdwArray = (DWORD*)lParam;
	dwThreadID = ::GetWindowThreadProcessId(hwnd, &dwProcessID);
	if (pdwArray[0] == dwProcessID)
	{
		pdwArray[1] = (DWORD) hwnd;
		return 0;
	}
	return 1;
}

void CModifyStyleDlg::ChangeListHeader(int nHeader, LPCTSTR szName)
{
	if (nHeader != m_nHeader)
	{
		CString string;
		LVCOLUMN lvc = {LVCF_TEXT, 0, 0, NULL, 0, 0, 0, 0};
		m_nHeader = nHeader;
		if (szName)
		{
			string = szName;
		}
		else
		{
			string.LoadString(nHeader);
		}
		lvc.pszText = (_TCHAR*)LPCTSTR(string);
		lvc.cchTextMax = string.GetLength();
		m_cStyleList.SetColumn(0, &lvc);

		switch (nHeader)
		{
			case IDS_STYLE_CODE:      nHeader = IDS_STYLE_NAME; break;
			case IDS_PROCESS_NAME:    nHeader = IDS_PROCESS_PATH; break;
			case IDS_CHILD_HANDLE:    nHeader = IDS_CHILD_NAME; break;
			case IDS_PROCESS_MODULES: nHeader = IDS_MODULE_PATH; break;
		}
		string.LoadString(nHeader);
		lvc.pszText = (_TCHAR*)LPCTSTR(string);
		lvc.cchTextMax = string.GetLength();
		m_cStyleList.SetColumn(1, &lvc);
	}
}

void CModifyStyleDlg::OnItemclickStyleList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
		
	*pResult = 0;
}

void CModifyStyleDlg::OnColumnclickStyleList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if (m_nHeader == IDS_STYLE_CODE)
	{
      HWND hParent = ::GetParent(m_hModWnd);
		if (hParent)
		{
         m_hModWnd         = hParent;
         m_hParentOfModWnd = ::GetParent(m_hModWnd);
			SetWindowinfo(m_hModWnd);
			OnSelchangeComboStyles();
			m_cStyleList.InvalidateRect(NULL);
      }
	}
	else if (m_nHeader == IDS_PROCESS_MODULES)
	{
		OnDblclkStyleList(pNMHDR, pResult);
	}
	else if (m_nHeader == IDS_PROCESS_NAME)
	{
		int nPos = m_cStyleList.GetSelectionMark();
		if (nPos != -1)
		{
			POSITION pos = m_Process.FindIndex(m_nProcess);
			ProcessData *pP = NULL;
			if (pos) pP = (ProcessData*) m_Process.GetAt(pos);
			if (pP)
			{
				DWORD dwArray[2] = {pP->dwID, 0};
				::EnumDesktopWindows(NULL, EnumDesktopWindowsProc, (LPARAM)dwArray);
				if (dwArray[1])
				{
					m_hModWnd         = (HWND)dwArray[1];
					m_hParentOfModWnd = ::GetParent(m_hModWnd);
					while (m_hParentOfModWnd)
					{
						m_hModWnd = m_hParentOfModWnd;
						m_hParentOfModWnd = ::GetParent(m_hModWnd);
					}
					SetWindowinfo(m_hModWnd);
					OnSelchangeComboStyles();
					m_cStyleList.InvalidateRect(NULL);
				}
			}
		}
	}
	
	*pResult = 0;
}

void CModifyStyleDlg::OnCkPos() 
{
	CDataExchange dx(this, true);
	DDX_Check(&dx, IDC_CK_POS, m_bSetPos);
	GetDlgItem(IDC_EDT_POS_X)->EnableWindow(m_bSetPos);
	GetDlgItem(IDC_EDT_POS_Y)->EnableWindow(m_bSetPos);
}

void CModifyStyleDlg::OnCkSize() 
{
	CDataExchange dx(this, true);
	DDX_Check(&dx, IDC_CK_SIZE, m_bSetSize);
	GetDlgItem(IDC_EDT_SIZE_X)->EnableWindow(m_bSetSize);
	GetDlgItem(IDC_EDT_SIZE_Y)->EnableWindow(m_bSetSize);
}

void CModifyStyleDlg::OnViewProcesses() 
{
	SetTreeVisible(FALSE);
	m_bShowAllProcesses = false;
	m_pStyle = NULL;
	m_bShowChildren = false;
	OnGetProcess();
	GetDlgItem(IDC_KILL_PROCESS)->EnableWindow(false);
	CString str;
	str.LoadString(IDS_BTNTXT_PROCESS);
	SetDlgItemText(IDC_KILL_PROCESS, str);
}

void CModifyStyleDlg::OnViewAllProcesses() 
{
	SetTreeVisible(FALSE);
	m_bShowAllProcesses = true;
	m_pStyle = NULL;
	m_bShowChildren = false;
	OnGetProcess();
	GetDlgItem(IDC_KILL_PROCESS)->EnableWindow(false);
	CString str;
	str.LoadString(IDS_BTNTXT_PROCESS);
	SetDlgItemText(IDC_KILL_PROCESS, str);
}

void CModifyStyleDlg::OnUpdateViewProcesses(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(((m_Process.GetCount() != 0) && !m_bShowChildren) ? TRUE:FALSE);
}

void CModifyStyleDlg::OnViewChildwindows() 
{
	SetTreeVisible(FALSE);
	if (m_hModWnd && ::IsWindow(m_hModWnd))
	{
		m_pStyle = NULL;
		DeleteProcessData();
		::EnumChildWindows(m_hModWnd, EnumWindowsProc, (LPARAM)this);
		ChangeListHeader(IDS_CHILD_HANDLE);
		m_cStyleList.SetItemCount(m_Children.GetCount());
		m_cStyleList.InvalidateRect(NULL);
	}
}

void CModifyStyleDlg::OnUpdateViewChildwindows(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio((m_Children.GetCount() != 0) ? TRUE:FALSE);
}

void CModifyStyleDlg::OnViewStyle() 
{
	SetTreeVisible(FALSE);
	if (m_hModWnd && ::IsWindow(m_hModWnd))
	{
		int nSel = m_cComboStyles.GetCurSel();
		switch (nSel)
		{
			case  1: nSel =  0; break;
			case 10: nSel =  9; break;
			case 13: nSel = 12; break;
			default: nSel =  0; break;
		}
		m_cComboStyles.SetCurSel(nSel);
		OnSelchangeComboStyles();
	}
}

void CModifyStyleDlg::OnUpdateViewStyle(CCmdUI* pCmdUI) 
{
	int nSel = m_cComboStyles.GetCurSel();
	switch (nSel)
	{
		case  1: pCmdUI->SetRadio(TRUE); break;
		case 10: pCmdUI->SetRadio(TRUE); break;
		case 13: pCmdUI->SetRadio(TRUE); break;
		default: pCmdUI->SetRadio(FALSE); break;
	}
}

void CModifyStyleDlg::OnViewStyleEx() 
{
	SetTreeVisible(FALSE);
	if (m_hModWnd && ::IsWindow(m_hModWnd))
	{
	   int nSel = m_cComboStyles.GetCurSel();
		switch (nSel)
		{
			case  0: nSel =  1; break;
			case  9: nSel = 10; break;
			case 12: nSel = 13; break;
			default: nSel =  1; break;
		}
		m_cComboStyles.SetCurSel(nSel);
		OnSelchangeComboStyles();
	}
}

void CModifyStyleDlg::OnUpdateViewStyleEx(CCmdUI* pCmdUI) 
{
	int nSel = m_cComboStyles.GetCurSel();
	switch (nSel)
	{
		case  1: pCmdUI->SetRadio(FALSE); break;
		case 10: pCmdUI->SetRadio(FALSE); break;
		case 13: pCmdUI->SetRadio(FALSE); break;
		case -1: pCmdUI->SetRadio(FALSE); break;
		default: pCmdUI->SetRadio(TRUE); break;
	}
}

void CModifyStyleDlg::OnFileSave() 
{
	int i, nCount = m_cStyleList.GetItemCount();
	if (nCount > 0)
	{
		CFileDialog dialog(FALSE, _T("TXT"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Text Datei (*.txt)|*.txt||"), this);
		if (dialog.DoModal()	== IDOK)
		{
			CFile file;
			if (file.Open(dialog.GetPathName(), CFile::modeCreate|CFile::modeWrite))
			{
				CArchive ar(&file, CArchive::store);
				CString str;
				if (m_pStyle)
				{
					str = m_strWndTxt + _T("\r\n");
					ar.WriteString(str);
					str = m_strWindowInfo + _T("\r\n");
					ar.WriteString(str);
					m_cComboStyles.GetWindowText(str);
					str+=  _T(":\r\n");
					ar.WriteString(str);
					for (i=0; i<nCount; i++)
					{
						bool bCheck = false;
						long n, nBits = 0;
						long nStyle = m_pStyle[i].nStyle;
						for (n=1; n!=0; n<<=1)
						{
							if (nStyle & n) nBits++;
							if (nBits>1) break;
						}
						if (nBits == 1) bCheck = ((m_nStyle & nStyle) !=      0) ? 1 : 0;
						else            bCheck = ((m_nStyle & nStyle) == nStyle) ? 1 : 0;
						if (bCheck)
						{
							str = m_cStyleList.GetItemText(i, 1) + _T("\r\n");
							ar.WriteString(str);
						}
					}
				}
				else if (m_Process.GetCount())
				{
					if (m_bShowChildren)
					{
						LVCOLUMN column;
						_TCHAR szText[64];
						column.mask = LVCF_TEXT;
						column.cchTextMax = 63;
						column.pszText = szText;
						m_cStyleList.GetColumn(0, &column);
						str = szText;
						str+=  _T("\r\n");
						ar.WriteString(str);
					}
					for (i=0; i<nCount; i++)
					{
						if (m_cStyleList.GetCheck(i))
						{
							str = m_cStyleList.GetItemText(i, 0) + _T(": ") + m_cStyleList.GetItemText(i, 1) + _T("\r\n");
							ar.WriteString(str);
						}
					}
				}
				else if (m_Children.GetCount())
				{
					str = m_strWndTxt + _T("\r\n");
					ar.WriteString(str);
					for (i=0; i<nCount; i++)
					{
						str.Format(_T("%d: %s (%s)\r\n"), i+1,m_cStyleList.GetItemText(i, 1), m_cStyleList.GetItemText(i, 0));
						ar.WriteString(str);
					}
				}
			}
		}
		
	}
}

void CModifyStyleDlg::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((m_cStyleList.GetItemCount() != 0) ? TRUE:FALSE);	
}

void CModifyStyleDlg::OnViewProcessModules() 
{
	SetTreeVisible(FALSE);
	ToggleProcess();
}

void CModifyStyleDlg::OnUpdateViewProcessModules(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(((m_Process.GetCount() != 0) && m_bShowChildren) ? TRUE:FALSE);
}

void CModifyStyleDlg::OnUpdateViewAllProcesses(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

void CModifyStyleDlg::OnBtnPostMessage() 
{
	CDataExchange dx (this, true);

	DDX_Text(&dx, IDC_EDT_MESSAGE, m_nMessage);
	DDX_Text(&dx, IDC_EDT_LPARAM, m_nLPARAM);
	DDX_Text(&dx, IDC_EDT_WPARAM, m_uWPARAM);
	if (m_hModWnd)
	{
		::PostMessage(m_hModWnd, m_nMessage, m_uWPARAM, m_nLPARAM);
	}
}

void CModifyStyleDlg::OnFileSaveWndText()
{
	if (::IsWindow(m_hModWnd))
	{
		CFileDialog dialog(FALSE, _T("TXT"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Text Datei (*.txt)|*.txt||"), this);
		if (dialog.DoModal()	== IDOK)
		{
			CFile file;
			if (file.Open(dialog.GetPathName(), CFile::modeCreate|CFile::modeWrite))
			{
				CArchive ar(&file, CArchive::store);
				CString str;
				if (m_iClass == SYSLISTVIEW32_STYLE)
				{
					CListCtrl list;
					list.m_hWnd = m_hModWnd;
					int nItems = list.GetItemCount();
//					int iColumn, nColumns;
//					LVCOLUMN column;
//					for (iColumn=0; ; iColumn++)
//					{
//						list.GetColumn(iColumn, &column);
//					}
//					str = list.GetItemText(0, 0);
//					_TCHAR szText[256];
//					LVITEM lvi = {0};
//					lvi.mask = LVIF_TEXT;
//					lvi.iItem = 0;
//					lvi.iSubItem = 0;
//					CMemFile file;
//
//					lvi.pszText = (_TCHAR*) CoTaskMemAlloc(256 * sizeof(_TCHAR));
//					lvi.cchTextMax = 255;
//					list.GetItem(&lvi);
//					DWORD dwError = GetLastError();
//					CoTaskMemFree((void*)lvi.pszText);
//					list.m_hWnd = NULL;
				}
			}
		}
	}
}

void CModifyStyleDlg::OnUpdateFileSaveWndText(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);//::IsWindow(m_hModWnd));
}

void CModifyStyleDlg::OnViewWindowHirarchie()
{
	HWND hwnd, hwndParent = m_hModWnd;
	do
	{
		hwnd = hwndParent;
		hwndParent = ::GetParent(hwnd);
	}
	while (hwndParent);

	if (hwnd)
	{
		SetTreeVisible(TRUE);
		HTREEITEM hItem = m_WindowTree.GetNextItem(TVI_ROOT, TVGN_CHILD);
		if (hItem)
		{
			hwndParent = (HWND)m_WindowTree.GetItemData(hItem);
			if (hwndParent == hwnd)
			{
				return;
			}
		}
		hwndParent = m_hParentOfModWnd;
		m_hParentOfModWnd = hwnd;
		CString sWndName;
		CWnd::FromHandle(m_hParentOfModWnd)->GetWindowText(sWndName);
		m_WindowTree.DeleteAllItems();
		m_hCurrentTreeItem = m_WindowTree.InsertItem(sWndName);
		m_WindowTree.SetItemData(m_hCurrentTreeItem, (DWORD_PTR)hwnd);
		EnumChildWindows(m_hParentOfModWnd, EnumFirstLevelChildren, (LPARAM)this);
		m_hParentOfModWnd = hwndParent;
	}
}

void CModifyStyleDlg::SetTreeVisible(BOOL bTree)
{
	m_WindowTree.ShowWindow(bTree ? SW_SHOW : SW_HIDE);
	m_cStyleList.ShowWindow(bTree ? SW_HIDE : SW_SHOW);
}

void CModifyStyleDlg::OnUpdateViewWindowHirarchie(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_hModWnd != NULL);
}

void CModifyStyleDlg::OnStartProcess()
{
	CFileDialog dialog(TRUE, _T("EXE"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Ausführbare Datei (*.exe)|*.exe||"), this);
	if (dialog.DoModal() == IDOK)
	{
//		CreateProcess(NULL, dialog.GetFileName(), NULL, NULL, TRUE, 
	}	
}

void CModifyStyleDlg::OnUpdateStartProcess(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

void CModifyStyleDlg::OnTvnSelchangedWndTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	m_hModWnd = (HWND)m_WindowTree.GetItemData(pNMTreeView->itemNew.hItem);
	SetWindowinfo(m_hModWnd);
	*pResult = 0;
}

void CModifyStyleDlg::OnCancel()
{
	DestroyWindow();
}

void CModifyStyleDlg::PostNcDestroy()
{
	delete this;
}

LRESULT CModifyStyleDlg::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam)
{
	UpdateDialogControls(this, FALSE);
	return TRUE;
}
