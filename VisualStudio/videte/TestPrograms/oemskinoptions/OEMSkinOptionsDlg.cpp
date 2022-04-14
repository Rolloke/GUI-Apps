// OEMSkinOptionsDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "OEMSkinOptions.h"
#include "OEMSkinOptionsDlg.h"
#include ".\oemskinoptionsdlg.h"


//#include <Winternl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MY_TV_PARAM (TVIF_HANDLE|TVIF_PARAM|TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE)

#define SET_TREE_DATA	0x00000001
#define APPLY_TO_ALL	0x00000002

extern AFX_DATA_IMPORT AUX_DATA afxData;

enum eControlType
{
	IMAGE_PARENT,
	IMAGE_DIALOG,
	IMAGE_BUTTON,
	IMAGE_STATIC,
	IMAGE_SLIDER,
	IMAGE_GROUP_BOX,
	IMAGE_EDIT,
	IMAGE_PROGRESS,
	IMAGE_TREE,
	IMAGE_LIST_CTRL,
	IMAGE_LIST_BOX,
	IMAGE_COMBO,
	IMAGE_SYS_COLORS,
	IMAGE_MENU
};

enum eControlProperty
{
	IMAGE_PROP_INVALID,
	IMAGE_PROP_COLOR,
	IMAGE_PROP_SHAPE,
	IMAGE_PROP_SURFACE,
	IMAGE_PROP_BACKGROUND_STYLE
};

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialogfeld für Anwendungsbefehl 'Info'
class CAboutDlg : public CSkinDialog
{
public:
	CAboutDlg();

// Dialogfelddaten
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

// Implementierung
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
/////////////////////////////////////////////////////////////////////////////
CAboutDlg::CAboutDlg() : CSkinDialog(CAboutDlg::IDD)
{
}
/////////////////////////////////////////////////////////////////////////////
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAboutDlg, CSkinDialog)
END_MESSAGE_MAP()

UINT GetPrivateProfileHex(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nDefault, LPCTSTR lpFileName)
{
	_TCHAR szDefault[] = _T("");
	_TCHAR szRead[64];
	UINT nReturn = nDefault;
	DWORD dwReturn = GetPrivateProfileString(lpAppName, lpKeyName, szDefault, szRead, 64, lpFileName);
	if (dwReturn > 0)
	{
		_stscanf(szRead, _T("%x"), &nReturn);
	}
	return nReturn;	
}
/////////////////////////////////////////////////////////////////////////////
// COEMSkinOptionsDlg Dialogfeld
COEMSkinOptionsDlg::COEMSkinOptionsDlg(CWnd* pParent /*=NULL*/)
	: CSkinDialog(COEMSkinOptionsDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_sInifile = "OEM.ini";
#ifdef _DEBUG
	m_nInitToolTips = TOOLTIPS_SIMPLE;
#else
	m_nInitToolTips = FALSE;
#endif
	m_liOldIdleTime.LowPart = 0;
	m_liOldIdleTime.HighPart = 0;
	m_liOldSystemTime.LowPart = 0;
	m_liOldSystemTime.HighPart = 0;
}
/////////////////////////////////////////////////////////////////////////////
void COEMSkinOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	DDX_Control(pDX, IDC_TREE1, m_Tree);
	DDX_Control(pDX, IDC_SLIDER1, m_Slider);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
	DDX_Control(pDX, IDC_LIST2, m_ListCtrl);
	DDX_Control(pDX, IDC_COMBO1, m_Combo);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_DateTimeCtrl);
	DDX_Control(pDX, IDC_SPIN1, m_SpinButton);
	DDX_Control(pDX, IDC_SCROLLBAR1, m_Scrollbar);
	DDX_Control(pDX, IDCANCEL, m_BtnCancel);
	DDX_Control(pDX, IDC_TXT_OEM, m_Static);
	DDX_Control(pDX, IDC_GRP_OEM, m_GroupBox);
	DDX_Control(pDX, ID_BTN_ICON, m_btnIcon);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(COEMSkinOptionsDlg, CSkinDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_NOTIFY(NM_RCLICK, IDC_TREE1, OnNMRclickTree1)
	ON_COMMAND(ID_FILE_LOAD, OnFileLoad)
	ON_UPDATE_COMMAND_UI(ID_FILE_LOAD, OnUpdateFileLoad)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveas)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveas)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_TEST_ENABLEOK, OnTestEnableok)
	ON_UPDATE_COMMAND_UI(ID_TEST_ENABLEOK, OnUpdateTestEnableok)
	ON_WM_DESTROY()
	ON_BN_CLICKED(ID_BTN_ICON, OnBnClickedBtnIcon)
	ON_WM_PARENTNOTIFY()
END_MESSAGE_MAP()


typedef struct
{
    DWORD   dwUnknown1;
    ULONG   uKeMaximumIncrement;
    ULONG   uPageSize;
    ULONG   uMmNumberOfPhysicalPages;
    ULONG   uMmLowestPhysicalPage;
    ULONG   uMmHighestPhysicalPage;
    ULONG   uAllocationGranularity;
    PVOID   pLowestUserAddress;
    PVOID   pMmHighestUserAddress;
    ULONG   uKeActiveProcessors;
    BYTE    bKeNumberProcessors;
    BYTE    bUnknown2;
    WORD    wUnknown3;
} SYSTEM_BASIC_INFORMATION;

typedef struct
{
    LARGE_INTEGER   liIdleTime;
    DWORD           dwSpare[76];
} SYSTEM_PERFORMANCE_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemBasicInformation = 0,
    SystemPerformanceInformation = 2,
    SystemTimeInformation = 3,
    SystemProcessInformation = 5,
    SystemProcessorPerformanceInformation = 8,
    SystemInterruptInformation = 23,
    SystemExceptionInformation = 33,
    SystemRegistryQuotaInformation = 37,
    SystemLookasideInformation = 45
} SYSTEM_INFORMATION_CLASS;

typedef struct
{
    LARGE_INTEGER liKeBootTime;
    LARGE_INTEGER liKeSystemTime;
    LARGE_INTEGER liExpTimeZoneBias;
    ULONG         uCurrentTimeZoneId;
    DWORD         dwReserved;
} SYSTEM_TIME_INFORMATION;

typedef struct
_SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION {
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER Reserved1[2];
    ULONG Reserved2;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);

PROCNTQSI NtQuerySystemInformation;

#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

/////////////////////////////////////////////////////////////////////////////
// COEMSkinOptionsDlg Meldungshandler
BOOL COEMSkinOptionsDlg::OnInitDialog()
{
	m_dwFlags &= ~SDFLAGS_SKIN_SYSMENU;	// do not initialize, because this dialog does
	CSkinDialog::OnInitDialog();
	AutoCreateSkinButtons(NULL, NULL);
	AutoCreateSkinStatic();
	AutoCreateSkinEdit();
//	AutoCreateSkinSlider(0, 0, 0);
	AutoCreateSkinListBox();
	
	m_Edit.SetFilter(_T("\\/#"), CFilteredSkinEdit::eNotAllowed);
	m_Edit.SetWindowText(_T("m_Static"));

	m_Context.LoadMenu(IDR_CONTEXT);
	m_Context.ConvertMenu(TRUE);
	m_Static.ModifySkinStyle(0, SSS_DRAW_HTML);

    NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(
                                          GetModuleHandle(_T("ntdll")),
                                         "NtQuerySystemInformation"
                                         );


/*
	m_ListBox.AddString(_T("String 1"));
	m_ListBox.AddString(_T("String 2"));
	m_ListBox.AddString(_T("String 3"));
	m_ListBox.AddString(_T("String 4"));
	m_ListBox.AddString(_T("String 5"));
	m_ListBox.AddString(_T("String 6"));
	m_ListBox.AddString(_T("String 7"));

	m_ListBox.SetBackgroundColorNormal(SKIN_COLOR_VIDETE_CI);
	m_ListBox.SetTextColorNormal(SKIN_COLOR_GREY_MEDIUM_LIGHT);
	m_ListBox.SetBackgroundColorHighlighted(CSkinButton::ChangeBrightness(SKIN_COLOR_VIDETE_CI, 32, TRUE));
	m_ListBox.SetTextColorHighlighted(SKIN_COLOR_WHITE);
*/

	m_Combo.AddString(_T("String 1"));
	m_Combo.AddString(_T("String 2"));
	m_Combo.AddString(_T("String 3"));
	m_Combo.AddString(_T("String 4"));
	m_Combo.AddString(_T("String 5"));
	m_Combo.AddString(_T("String 6"));
	m_Combo.AddString(_T("String 7"));
	SetTimer(0x0815, 1000, NULL);

	m_Scrollbar.SetScrollRange(0, 10);
	m_Scrollbar.SetScrollPos(5);

	m_progress.AddGradienColor(0, RGB(0,255,0));
	m_progress.AddGradienColor(25, RGB(0,255,255));
	m_progress.AddGradienColor(50, RGB(0,0,255));
	m_progress.AddGradienColor(75, RGB(255,255,0));
	m_progress.AddGradienColor(85, RGB(255,128,0));
	m_progress.AddGradienColor(100, RGB(255,0,0));
	m_progress.SetRange(0, 100);
//	long lStyle = GetWindowLong(::GetDlgItem(m_hWnd, IDC_SLIDER1), GWL_STYLE);
	for (int n=0; n<=100; n+=10)
	{
		m_Slider.SetTic(n);
	}
	m_Slider.ModifyFlags(0, SKINSLIDER_USE_CUSTOM_DRAW);
//	SetSelection(m_Slider.GetDlgCtrlID());

	CRect rc;
	m_ListCtrl.GetClientRect(&rc);
	m_ListCtrl.InsertColumn(0, _T("Header 1"), 0, rc.Width()/2);
	m_ListCtrl.InsertColumn(0, _T("Header 2"), 0, rc.Width()/2-1);

	m_ListCtrl.InsertItem(0, _T("Item 1"));
	m_ListCtrl.InsertItem(1, _T("Item 2"));
	m_ListCtrl.InsertItem(2, _T("Item 3"));
	m_ListCtrl.InsertItem(3, _T("Item 4"));
	m_ListCtrl.InsertItem(4, _T("Item 5"));

	m_ListCtrl.SetItem(0, 1, LVIF_TEXT, _T("SubItem 1"), 0,0,0,0);
	m_ListCtrl.SetItem(1, 1, LVIF_TEXT, _T("SubItem 1"), 0,0,0,0);
	m_ListCtrl.SetItem(2, 1, LVIF_TEXT, _T("SubItem 1"), 0,0,0,0);
	m_ListCtrl.SetItem(3, 1, LVIF_TEXT, _T("SubItem 1"), 0,0,0,0);
	m_ListCtrl.SetItem(4, 1, LVIF_TEXT, _T("SubItem 1"), 0,0,0,0);

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
		((CSkinMenu*)pSysMenu)->ConvertMenu(TRUE);
		// OnDestroy of baseclass frees resources if this flag is set
		m_dwFlags |= SDFLAGS_SKIN_SYSMENU;	
	}

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden
	m_btnIcon.SetIcon(m_hIcon);

	HTREEITEM hDialog = m_Tree.InsertItem(MY_TV_PARAM, OEM_DIALOG, IMAGE_PARENT, IMAGE_DIALOG, 0, 0, 0, TVI_ROOT, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_BACKGROUND_STYLE, IMAGE_DIALOG, IMAGE_PROP_BACKGROUND_STYLE, 0, 0, GetBackgroundStyle(), hDialog, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_BASE_COLOR, IMAGE_DIALOG, IMAGE_PROP_COLOR, 0, 0, GetBaseColorC(), hDialog, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_TEXT_COLOR, IMAGE_DIALOG, IMAGE_PROP_COLOR, 0, 0, GetTextColorC(), hDialog, TVI_LAST);

	m_Tree.InsertItem(MY_TV_PARAM, OEM_HILIGHT_COLOR, IMAGE_DIALOG, IMAGE_PROP_COLOR, 0, 0, GetHilightColor(), hDialog, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_SHADOW_COLOR, IMAGE_DIALOG, IMAGE_PROP_COLOR, 0, 0, GetShadowColor(), hDialog, TVI_LAST);
/*
	m_Tree.InsertItem(MY_TV_PARAM, _T("Change only Brightness"), IMAGE_DIALOG, 0, 0, 0, 0, hDialog, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, _T("Max Color Change"), IMAGE_DIALOG, 0, 0, 0, 0, hDialog, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, _T("Brush Effect Factor"), IMAGE_DIALOG, 0, 0, 0, 0, hDialog, TVI_LAST);
*/
	HTREEITEM hButton = m_Tree.InsertItem(MY_TV_PARAM, OEM_BUTTON, IMAGE_PARENT, IMAGE_BUTTON, 0, 0, 0, TVI_ROOT, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_SHAPE, IMAGE_BUTTON, IMAGE_PROP_SHAPE, 0, 0, m_BtnCancel.GetShape(), hButton, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_SURFACE, IMAGE_BUTTON, IMAGE_PROP_SURFACE, 0, 0, m_BtnCancel.GetSurface(), hButton, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_BASE_COLOR, IMAGE_BUTTON, IMAGE_PROP_COLOR, 0, 0, m_BtnCancel.GetBaseColor(), hButton, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_TEXT_COLOR, IMAGE_BUTTON, IMAGE_PROP_COLOR, 0, 0, m_BtnCancel.GetTextColorNormal(), hButton, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_TEXT_COLOR_HILIGHTED, IMAGE_BUTTON, IMAGE_PROP_COLOR, 0, 0, m_BtnCancel.GetTextColorHighlighted(), hButton, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_TEXT_COLOR_DISABLED, IMAGE_BUTTON, IMAGE_PROP_COLOR, 0, 0, m_BtnCancel.GetTextColorDisabled(), hButton, TVI_LAST);

	HTREEITEM hStatic = m_Tree.InsertItem(MY_TV_PARAM, OEM_STATIC, IMAGE_PARENT, IMAGE_STATIC, 0, 0, 0, TVI_ROOT, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_BACKGROUND_COLOR, IMAGE_STATIC, IMAGE_PROP_COLOR, 0, 0, m_Static.GetBackgroundColor(), hStatic, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_TEXT_COLOR, IMAGE_STATIC, IMAGE_PROP_COLOR, 0, 0, m_Static.GetTextColor(), hStatic, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_HILIGHT_COLOR, IMAGE_STATIC, IMAGE_PROP_COLOR, 0, 0, m_Static.GetLineColorLight(), hStatic, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_SHADOW_COLOR, IMAGE_STATIC, IMAGE_PROP_COLOR, 0, 0, m_Static.GetLineColorDark(), hStatic, TVI_LAST);

	HTREEITEM hSlider= m_Tree.InsertItem(MY_TV_PARAM, OEM_SLIDER, IMAGE_PARENT, IMAGE_SLIDER, 0, 0, 0, TVI_ROOT, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_SHAPE, IMAGE_SLIDER, IMAGE_PROP_SHAPE, 0, 0, m_Slider.GetShape(), hSlider, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_SURFACE, IMAGE_SLIDER, IMAGE_PROP_SURFACE, 0, 0, m_Slider.GetSurface(), hSlider, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_BACKGROUND_COLOR, IMAGE_SLIDER, IMAGE_PROP_COLOR, 0, 0, m_Slider.GetBaseColor(), hSlider, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_TEXT_COLOR, IMAGE_SLIDER, IMAGE_PROP_COLOR, 0, 0, m_Slider.GetTextColorNormal(), hSlider, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_HILIGHT_COLOR, IMAGE_SLIDER, IMAGE_PROP_COLOR, 0, 0, m_Slider.GetColorLineLight(), hSlider, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_SHADOW_COLOR, IMAGE_SLIDER, IMAGE_PROP_COLOR, 0, 0, m_Slider.GetColorLineDark(), hSlider, TVI_LAST);

	HTREEITEM hGroupBox = m_Tree.InsertItem(MY_TV_PARAM, OEM_GROUP_BOX, IMAGE_PARENT, IMAGE_GROUP_BOX, 0, 0, 0, TVI_ROOT, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_TEXT_COLOR, IMAGE_GROUP_BOX, IMAGE_PROP_COLOR, 0, 0, m_GroupBox.GetTextColorNormal(), hGroupBox, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_TEXT_COLOR_DISABLED, IMAGE_GROUP_BOX, IMAGE_PROP_COLOR, 0, 0, m_GroupBox.GetTextColorDisabled(), hGroupBox, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_HILIGHT_COLOR, IMAGE_GROUP_BOX, IMAGE_PROP_COLOR, 0, 0, m_GroupBox.GetLineColorLight(), hGroupBox, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_SHADOW_COLOR, IMAGE_GROUP_BOX, IMAGE_PROP_COLOR, 0, 0, m_GroupBox.GetLineColorDark(), hGroupBox, TVI_LAST);

	HTREEITEM hEdit= m_Tree.InsertItem(MY_TV_PARAM, OEM_EDIT, IMAGE_PARENT, IMAGE_EDIT, 0, 0, 0, TVI_ROOT, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_BACKGROUND_COLOR, IMAGE_EDIT, IMAGE_PROP_COLOR, 0, 0, m_Edit.GetBackgroundColor(), hEdit, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_TEXT_COLOR, IMAGE_EDIT, IMAGE_PROP_COLOR, 0, 0, m_Edit.GetTextColor(), hEdit, TVI_LAST);

	HTREEITEM hProgress = m_Tree.InsertItem(MY_TV_PARAM, OEM_PROGRESS, IMAGE_PARENT, IMAGE_PROGRESS, 0, 0, 0, TVI_ROOT, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_BACKGROUND_COLOR, IMAGE_PROGRESS, IMAGE_PROP_COLOR, 0, 0, m_progress.GetBackGroundColor(), hProgress, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_PROGRESS_COLOR, IMAGE_PROGRESS, IMAGE_PROP_COLOR, 0, 0, m_progress.GetProgressColor(), hProgress, TVI_LAST);
	
	HTREEITEM hTree = m_Tree.InsertItem(MY_TV_PARAM, OEM_TREE, IMAGE_PARENT, IMAGE_TREE, 0, 0, 0, TVI_ROOT, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_BACKGROUND_COLOR, IMAGE_TREE, IMAGE_PROP_COLOR, 0, 0, m_Tree.GetBkColor(), hTree, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_TEXT_COLOR, IMAGE_TREE, IMAGE_PROP_COLOR, 0, 0, m_Tree.GetTextColor(), hTree, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_LINE_COLOR, IMAGE_TREE, IMAGE_PROP_COLOR, 0, 0, m_Tree.GetLineColor(), hTree, TVI_LAST);

	HTREEITEM hListCtrl= m_Tree.InsertItem(MY_TV_PARAM, OEM_LIST_CONTROL, IMAGE_PARENT, IMAGE_LIST_CTRL, 0, 0, 0, TVI_ROOT, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_HEADER_BUTTON_SHAPE, IMAGE_LIST_CTRL, IMAGE_PROP_SHAPE, 0, 0, m_ListCtrl.GetShape(), hListCtrl, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_HEADER_BUTTON_SURFACE, IMAGE_LIST_CTRL, IMAGE_PROP_SURFACE, 0, 0, m_ListCtrl.GetSurface(), hListCtrl, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_HEADER_BACKGROUND_COLOR, IMAGE_LIST_CTRL, IMAGE_PROP_COLOR, 0, 0, m_ListCtrl.GetHeaderBkColor(), hListCtrl, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_HEADER_TEXT_COLOR, IMAGE_LIST_CTRL, IMAGE_PROP_COLOR, 0, 0, m_ListCtrl.GetHeaderTextColor(), hListCtrl, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_BASE_COLOR, IMAGE_LIST_CTRL, IMAGE_PROP_COLOR, 0, 0, m_ListCtrl.GetBkColor(), hListCtrl, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_TEXT_BACKGROUND_COLOR, IMAGE_LIST_CTRL, IMAGE_PROP_COLOR, 0, 0, m_ListCtrl.GetTextBkColor(), hListCtrl, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_TEXT_COLOR, IMAGE_LIST_CTRL, IMAGE_PROP_COLOR, 0, 0, m_ListCtrl.GetTextColor(), hListCtrl, TVI_LAST);
	
	HTREEITEM hListBox = m_Tree.InsertItem(MY_TV_PARAM, OEM_LIST_BOX, IMAGE_PARENT, IMAGE_LIST_BOX, 0, 0, 0, TVI_ROOT, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_BACKGROUND_COLOR, IMAGE_LIST_BOX, IMAGE_PROP_COLOR, 0, 0, m_ListBox.GetBackgroundColorNormal(), hListBox, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_SELECTED_BACKGROUND, IMAGE_LIST_BOX, IMAGE_PROP_COLOR, 0, 0, m_ListBox.GetBackgroundColorHighlighted(), hListBox, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_TEXT_COLOR, IMAGE_LIST_BOX, IMAGE_PROP_COLOR, 0, 0, m_ListBox.GetTextColorNormal(), hListBox, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_SELECTED_TEXT, IMAGE_LIST_BOX, IMAGE_PROP_COLOR, 0, 0, m_ListBox.GetTextColorHighlighted(), hListBox, TVI_LAST);

	HTREEITEM hComboBox = m_Tree.InsertItem(MY_TV_PARAM, OEM_COMBO_BOX, IMAGE_PARENT, IMAGE_COMBO, 0, 0, 0, TVI_ROOT, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_SHAPE, IMAGE_COMBO, IMAGE_PROP_SHAPE, 0, 0, m_Combo.GetShape(), hComboBox, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_SURFACE, IMAGE_COMBO, IMAGE_PROP_SURFACE, 0, 0, m_Combo.GetSurface(), hComboBox, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_BACKGROUND_COLOR, IMAGE_COMBO, IMAGE_PROP_COLOR, 0, 0, m_Combo.GetBackgroundColor(), hComboBox, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_TEXT_COLOR, IMAGE_COMBO, IMAGE_PROP_COLOR, 0, 0, m_Combo.GetTextColor(), hComboBox, TVI_LAST);

	HTREEITEM hMenu = m_Tree.InsertItem(MY_TV_PARAM, OEM_MENU, IMAGE_PARENT, IMAGE_MENU, 0, 0, 0, TVI_ROOT, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_SHAPE, IMAGE_MENU, IMAGE_PROP_SHAPE, 0, 0, GetSkinMenu()->GetShape(), hMenu, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_SURFACE, IMAGE_MENU, IMAGE_PROP_SURFACE, 0, 0, GetSkinMenu()->GetSurface(), hMenu, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_BACKGROUND_COLOR, IMAGE_MENU, IMAGE_PROP_COLOR, 0, 0, GetSkinMenu()->GetBaseColor(), hMenu, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_TEXT_COLOR, IMAGE_MENU, IMAGE_PROP_COLOR, 0, 0, GetSkinMenu()->GetTextColorNormal(), hMenu, TVI_LAST);

	HTREEITEM hSystemColors = m_Tree.InsertItem(MY_TV_PARAM, OEM_SYSTEM_COLORS, IMAGE_PARENT, IMAGE_SYS_COLORS, 0, 0, 0, TVI_ROOT, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_WINDOW_FRAME			 , COLOR_WINDOWFRAME, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_WINDOWFRAME), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_BUTTON_FACE			 , COLOR_BTNFACE, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_BTNFACE), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_BUTTON_TEXT			 , COLOR_BTNTEXT, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_BTNTEXT), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_SHADOW				 , COLOR_BTNSHADOW, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_BTNSHADOW), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_HILIGHT				 , COLOR_BTNHIGHLIGHT, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_BTNHIGHLIGHT), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, _T("-----------"), 0, IMAGE_PROP_INVALID, 0, 0, 0, hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_MENU_BACKGROUND		 , COLOR_MENU, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_MENU), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_MENU_TEXT			 , COLOR_MENUTEXT, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_MENUTEXT), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_MENU_HILIGHT			 , COLOR_MENUHILIGHT, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_MENUHILIGHT), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_MENU_BAR				 , COLOR_MENUBAR, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_MENUBAR), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_SELECTED_BACKGROUND	 , COLOR_HIGHLIGHT, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_HIGHLIGHT), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_SELECTED_TEXT		 , COLOR_HIGHLIGHTTEXT, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_HIGHLIGHTTEXT), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_3D_SHADOW			 , COLOR_3DDKSHADOW, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_3DDKSHADOW), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_3D_HILIGHT			 , COLOR_3DLIGHT, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_3DLIGHT), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_APPLICATION_BACKGROUND, COLOR_WINDOW, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_WINDOW), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_APPLICATION_TEXT		 , COLOR_WINDOWTEXT, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_WINDOWTEXT), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, _T("-----------"), 0, IMAGE_PROP_INVALID, 0, 0, 0, hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_INFO_TEXT, COLOR_INFOTEXT, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_INFOTEXT), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_INFO_BACKGROUND, COLOR_INFOBK, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_INFOBK), hSystemColors, TVI_LAST);

	m_Tree.InsertItem(MY_TV_PARAM, OEM_BACKGROUND_COLOR, COLOR_BACKGROUND, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_BACKGROUND), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_APPLICATION_WORKSPACE, COLOR_APPWORKSPACE, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_APPWORKSPACE), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_GRAY_TEXT, COLOR_GRAYTEXT, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_GRAYTEXT), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, OEM_HOT_LIGHT, COLOR_HOTLIGHT, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_HOTLIGHT), hSystemColors, TVI_LAST);
	LoadOEM();

/*
	m_Tree.InsertItem(MY_TV_PARAM, _T("Window Frame"), COLOR_WINDOWFRAME, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_WINDOWFRAME), hSystemColors, TVI_LAST);

	m_Tree.InsertItem(MY_TV_PARAM, _T("Active Caption Background"), COLOR_ACTIVECAPTION, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_ACTIVECAPTION), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, _T("Active Caption Gradient Color"), COLOR_GRADIENTACTIVECAPTION, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_GRADIENTACTIVECAPTION), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, _T("Active Caption Text"), COLOR_CAPTIONTEXT, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_CAPTIONTEXT), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, _T("Active Border"), COLOR_ACTIVEBORDER, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_ACTIVEBORDER), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, _T("Inactive Caption"), COLOR_INACTIVECAPTION, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_INACTIVECAPTION), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, _T("Inactive Caption Gradient Color"), COLOR_GRADIENTINACTIVECAPTION, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_GRADIENTINACTIVECAPTION), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, _T("Inactive Caption Text"), COLOR_INACTIVECAPTIONTEXT, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_INACTIVECAPTIONTEXT), hSystemColors, TVI_LAST);
	m_Tree.InsertItem(MY_TV_PARAM, _T("Inactive Border"), COLOR_INACTIVEBORDER, IMAGE_PROP_COLOR, 0, 0, GetSysColor(COLOR_INACTIVEBORDER), hSystemColors, TVI_LAST);
*/
// Farben:
		//	COLOR_WINDOW: Hintergrundfarbe der Anwendungen
		//	COLOR_WINDOWTEXT: Textfarbe (auf dem Hintergrund) der Anwendungen
		//	COLOR_SCROLLBAR: Scrollbar Farbe
		//	COLOR_WINDOWFRAME: Rahmenfarbe der Fenster (Button Rahmen..) #
		//	COLOR_CAPTIONTEXT: Titeltextfarbe einer aktiven Anwendung
		//	COLOR_INACTIVECAPTIONTEXT:  Titeltextfarbe einer inaktiven Anwendung
		//	COLOR_ACTIVEBORDER: ?
		//	COLOR_INACTIVEBORDER: ?
		//	COLOR_BTNFACE: Farbe für Buttons, Dialoge, Toolbars, etc
		//	COLOR_GRAYTEXT: Disabled Text Farbe
		//	COLOR_BTNTEXT: Textfarbe für Buttons, TabCtrls, Statusbar

	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}
/////////////////////////////////////////////////////////////////////////////
void COEMSkinOptionsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CSkinDialog::OnSysCommand(nID, lParam);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.
void COEMSkinOptionsDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext zum Zeichnen

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Symbol in Clientrechteck zentrieren
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
		CSkinDialog::OnPaint();
	}
}
/////////////////////////////////////////////////////////////////////////////
// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
//  das minimierte Fenster mit der Maus zieht.
HCURSOR COEMSkinOptionsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAboutDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();
	AutoCreateSkinStatic();
	AutoCreateSkinButtons(NULL, NULL);

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void COEMSkinOptionsDlg::OnOK()
{
	OnFileSave();
	CSkinDialog::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
void COEMSkinOptionsDlg::OnTimer(UINT nIDEvent)
{
	if (NtQuerySystemInformation)
	{
		SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
		SYSTEM_TIME_INFORMATION        SysTimeInfo;
		SYSTEM_BASIC_INFORMATION       SysBaseInfo;
		double                         dbIdleTime;
		double                         dbSystemTime;
		LONG                           status;

		// get number of processors in the system
		status = NtQuerySystemInformation(SystemBasicInformation,&SysBaseInfo,sizeof(SysBaseInfo),NULL);
		if (status != NO_ERROR)
			return ;
    
        // get new system time
	    status = NtQuerySystemInformation(SystemTimeInformation,&SysTimeInfo,sizeof(SysTimeInfo),0);
        if (status!=NO_ERROR)
            return ;

        // get new CPU's idle time
        status = NtQuerySystemInformation(SystemPerformanceInformation,&SysPerfInfo,sizeof(SysPerfInfo),NULL);
        if (status != NO_ERROR)
            return ;

			// if it's a first call - skip it
		if (m_liOldIdleTime.QuadPart != 0)
		{
				// CurrentValue = NewValue - OldValue
				dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(m_liOldIdleTime);
				dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(m_liOldSystemTime);

				// CurrentCpuIdle = IdleTime / SystemTime
				dbIdleTime = dbIdleTime / dbSystemTime;

				// CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors
				dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)SysBaseInfo.bKeNumberProcessors + 0.5;

				UINT nPercent = (UINT)dbIdleTime;
				m_progress.SetPos(nPercent);
//				m_progress.SetPos(100);
		}

		// store new CPU's idle and system time
		m_liOldIdleTime = SysPerfInfo.liIdleTime;
		m_liOldSystemTime = SysTimeInfo.liKeSystemTime;
		
	}
/*
	int nLower, nUpper, nPos = m_progress.GetPos();
	m_progress.GetRange(nLower, nUpper);
	if (nPos == nUpper)
	{
		nPos = nLower;
	}
	else
	{
		nPos+=2;
	}
	m_progress.SetPos(nPos);
*/
/*
	CTime time;
	time = CTime::GetCurrentTime();
	CString str = time.Format(_T("%H:%M:%S"));
	CDialog::GetDlgItem(IDC_TXT_OEM5)->SetWindowText(str);
*/
	CSkinDialog::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void COEMSkinOptionsDlg::OnNMRclickTree1(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	CPoint ptCursor;
	GetCursorPos(&ptCursor);
	m_Tree.ScreenToClient(&ptCursor);
	UINT nFlags = 0;
	int nImage, nSelected;
	HTREEITEM hItem = m_Tree.HitTest(ptCursor, &nFlags);

	if (hItem && (TVHT_ONITEM & nFlags))
	{
		m_Tree.SelectItem(hItem);
		m_Tree.GetItemImage(hItem, nImage, nSelected);
		if (nImage == IMAGE_PARENT)
		{
			if (   nSelected == IMAGE_SYS_COLORS 
				&& AfxMessageBox(_T("Set SystemColors ?"), MB_YESNO, 0) == IDYES)
			{
				int nItems = 0;
				HTREEITEM hChild = m_Tree.GetNextItem(hItem, TVGN_CHILD);
				while (hChild)
				{
					m_Tree.GetItemImage(hChild, nImage, nSelected);
					if (IMAGE_PROP_COLOR == nSelected)
					{
						nItems++;
					}
					hChild = m_Tree.GetNextItem(hChild, TVGN_NEXT);
				}
				if (nItems)
				{
					int *nElements = new int[nItems];
                    COLORREF *cCols = new COLORREF[nItems];
					int i=0;
					hChild = m_Tree.GetNextItem(hItem, TVGN_CHILD);
					while (hChild)
					{
						m_Tree.GetItemImage(hChild, nImage, nSelected);
						if (IMAGE_PROP_COLOR == nSelected)
						{
							cCols[i] =(COLORREF) m_Tree.GetItemData(hChild);
							nElements[i] = nImage;
							i++;
						}
						hChild = m_Tree.GetNextItem(hChild, TVGN_NEXT);
					}
					SetSysColors(nItems, nElements, cCols);
					delete nElements;
					delete cCols;
				}
			}
		}
		else
		{
			DWORD dwData = (DWORD)m_Tree.GetItemData(hItem);
			switch (nSelected)
			{
				case IMAGE_PROP_COLOR:
					dwData = ChooseColor(dwData);
					break;
				case IMAGE_PROP_BACKGROUND_STYLE:
					dwData = ChooseBkGndStyle(dwData);
					break;
				case IMAGE_PROP_SHAPE:
					dwData = ChooseShape(dwData);
					break;
				case IMAGE_PROP_SURFACE:
					dwData = ChooseSurface(dwData);
					break;
				default:
					dwData = (DWORD)-1;
					break;
			}
			if (dwData != (DWORD)-1)
			{
				m_Tree.SetItemData(hItem, dwData);
				int nParent;
				m_Tree.GetItemImage(m_Tree.GetParentItem(hItem), nParent, nSelected);
				if (nSelected == IMAGE_SYS_COLORS) nImage = IMAGE_SYS_COLORS;
				ApplyProperty((eControlType) nImage, m_Tree.GetItemText(hItem), dwData, APPLY_TO_ALL);
			}
		}
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void COEMSkinOptionsDlg::ApplyProperty(eControlType nImage, CString sProperty, DWORD dwData, DWORD dwFlags)
{
	if (dwFlags == SET_TREE_DATA)
	{
		// TODO! RKE: Fehlerhaft!!
		HTREEITEM hItem = FindTreeItem(FindTreeItem(nImage), sProperty);
		if (hItem)
		{
			m_Tree.SetItemData(hItem, dwData);
			TRACE(_T("Set %s, %s\n"), m_Tree.GetItemText(m_Tree.GetParentItem(hItem)), m_Tree.GetItemText(hItem));
		}
		else
		{
			TRACE(_T("Did not find %d, %s\n"), nImage, sProperty);
		}
	}
	if (nImage == IMAGE_DIALOG)
	{
		if (sProperty == OEM_BACKGROUND_STYLE)
		{
			STYLE_BACKGROUND sBkGnd = (STYLE_BACKGROUND)dwData;
			if (sBkGnd == StyleBackGroundBitmap)
			{
				CreateTransparent(0); // later
			}
			else
			{
				CreateTransparent(sBkGnd, (COLORREF)-1);
			}
		}
		else if (sProperty == OEM_BASE_COLOR)
		{
			SetBaseColor(dwData);
			if (   (dwFlags & APPLY_TO_ALL) 
				&& AfxMessageBox(_T("Apply the base color to all other Dialog items?"), MB_YESNO|MB_ICONQUESTION) == IDYES)
			{
				COLORREF cHilight = CSkinButton::ChangeBrightness(dwData,  45, SET_TREE_DATA);
				COLORREF cSelection = CSkinButton::ChangeBrightness(dwData,  20, SET_TREE_DATA);
				COLORREF cShaddow = CSkinButton::ChangeBrightness(dwData, -45, SET_TREE_DATA);
				
				ApplyProperty(IMAGE_DIALOG, OEM_HILIGHT_COLOR, cHilight, SET_TREE_DATA);
				ApplyProperty(IMAGE_DIALOG, OEM_SHADOW_COLOR, cShaddow, SET_TREE_DATA);

				ApplyProperty(IMAGE_BUTTON, OEM_BASE_COLOR, dwData, SET_TREE_DATA);

				ApplyProperty(IMAGE_STATIC, OEM_BACKGROUND_COLOR, dwData, SET_TREE_DATA);
				ApplyProperty(IMAGE_STATIC, OEM_HILIGHT_COLOR, cHilight, SET_TREE_DATA);
				ApplyProperty(IMAGE_STATIC, OEM_SHADOW_COLOR, cShaddow, SET_TREE_DATA);

				ApplyProperty(IMAGE_LIST_BOX, OEM_BACKGROUND_COLOR, dwData, SET_TREE_DATA);
				ApplyProperty(IMAGE_LIST_BOX, OEM_SELECTED_BACKGROUND, dwData, SET_TREE_DATA);

				ApplyProperty(IMAGE_SLIDER, OEM_BACKGROUND_COLOR, dwData, SET_TREE_DATA);
				ApplyProperty(IMAGE_SLIDER, OEM_HILIGHT_COLOR, cHilight, SET_TREE_DATA);
				ApplyProperty(IMAGE_SLIDER, OEM_SHADOW_COLOR, cShaddow, SET_TREE_DATA);

				ApplyProperty(IMAGE_EDIT, OEM_BACKGROUND_COLOR, dwData, SET_TREE_DATA);
				ApplyProperty(IMAGE_PROGRESS, OEM_BACKGROUND_COLOR, dwData, SET_TREE_DATA);
				ApplyProperty(IMAGE_TREE, OEM_BACKGROUND_COLOR, dwData, SET_TREE_DATA);
				ApplyProperty(IMAGE_LIST_CTRL, OEM_HEADER_BACKGROUND_COLOR, dwData, SET_TREE_DATA);
				ApplyProperty(IMAGE_LIST_CTRL, OEM_TEXT_BACKGROUND_COLOR, dwData, SET_TREE_DATA);
				ApplyProperty(IMAGE_LIST_CTRL, OEM_BASE_COLOR, dwData, SET_TREE_DATA);
				ApplyProperty(IMAGE_COMBO, OEM_BACKGROUND_COLOR, dwData, SET_TREE_DATA);
				ApplyProperty(IMAGE_MENU, OEM_BACKGROUND_COLOR, dwData, SET_TREE_DATA);

				SetOEMColors(cHilight, cShaddow);
				ApplyProperty(IMAGE_GROUP_BOX, OEM_HILIGHT_COLOR, cHilight, SET_TREE_DATA);
				ApplyProperty(IMAGE_GROUP_BOX, OEM_SHADOW_COLOR, cShaddow, SET_TREE_DATA);

				afxData.clrBtnFace = dwData;
				afxData.clrBtnHilite = cHilight;
				afxData.clrBtnShadow = cShaddow;
				afxData.clrWindowFrame = dwData;

				HTREEITEM hItem = FindTreeItem(IMAGE_SYS_COLORS);
				if (hItem)
				{
					int nImage, nSelected;
					HTREEITEM hChild = m_Tree.GetNextItem(hItem, TVGN_CHILD);
					while (hChild)
					{
						m_Tree.GetItemImage(hChild, nImage, nSelected);
						switch (nImage)
						{
							case COLOR_BTNSHADOW:
							case COLOR_3DDKSHADOW:
								m_Tree.SetItemData(hChild, cShaddow);
								break;
							case COLOR_BTNHIGHLIGHT:
							case COLOR_3DLIGHT:
								m_Tree.SetItemData(hChild, cHilight);
								break;
							case COLOR_HIGHLIGHT:
							case COLOR_MENUHILIGHT:
								m_Tree.SetItemData(hChild, cSelection);
								break;
							case COLOR_MENU:
							case COLOR_WINDOW:
							case COLOR_WINDOWFRAME:
							case COLOR_MENUBAR:
							case COLOR_BTNFACE:
								m_Tree.SetItemData(hChild, dwData);
								break;
						}

						hChild = m_Tree.GetNextItem(hChild, TVGN_NEXT);
					}
				}
			}
			Update();
		}
		else if (sProperty == OEM_TEXT_COLOR)
		{
			SetTextColor(dwData);
			if (   (dwFlags & APPLY_TO_ALL) 
				&& AfxMessageBox(_T("Apply the Text color to all other Dialog items?"), MB_YESNO|MB_ICONQUESTION) == IDYES)
			{
				COLORREF cSelection = CSkinButton::ChangeBrightness(dwData,   20, SET_TREE_DATA);
				COLORREF cDisabled  = CSkinButton::ChangeBrightness(dwData,  -20, SET_TREE_DATA);

				ApplyProperty(IMAGE_BUTTON, OEM_TEXT_COLOR, dwData, SET_TREE_DATA);
				ApplyProperty(IMAGE_BUTTON, OEM_TEXT_COLOR_HILIGHTED, cSelection, SET_TREE_DATA);
				ApplyProperty(IMAGE_BUTTON, OEM_TEXT_COLOR_DISABLED, cDisabled, SET_TREE_DATA);

				ApplyProperty(IMAGE_LIST_BOX, OEM_TEXT_COLOR, dwData, SET_TREE_DATA);
				ApplyProperty(IMAGE_LIST_BOX, OEM_SELECTED_TEXT, cSelection, SET_TREE_DATA);

				ApplyProperty(IMAGE_STATIC, OEM_TEXT_COLOR, dwData, SET_TREE_DATA);
				ApplyProperty(IMAGE_SLIDER, OEM_TEXT_COLOR, dwData, SET_TREE_DATA);

				ApplyProperty(IMAGE_GROUP_BOX, OEM_TEXT_COLOR, dwData, SET_TREE_DATA);
				ApplyProperty(IMAGE_GROUP_BOX, OEM_TEXT_COLOR_DISABLED, cDisabled, SET_TREE_DATA);

				ApplyProperty(IMAGE_EDIT, OEM_TEXT_COLOR, dwData, SET_TREE_DATA);
				ApplyProperty(IMAGE_TREE, OEM_TEXT_COLOR, dwData, SET_TREE_DATA);
				ApplyProperty(IMAGE_TREE, OEM_LINE_COLOR, dwData, SET_TREE_DATA);
				ApplyProperty(IMAGE_LIST_CTRL, OEM_HEADER_TEXT_COLOR, dwData, SET_TREE_DATA);
				ApplyProperty(IMAGE_LIST_CTRL, OEM_TEXT_COLOR, dwData, SET_TREE_DATA);

				ApplyProperty(IMAGE_COMBO, OEM_TEXT_COLOR, dwData, SET_TREE_DATA);
				ApplyProperty(IMAGE_MENU, OEM_TEXT_COLOR, dwData, SET_TREE_DATA);

				afxData.clrBtnText = dwData;

				HTREEITEM hItem = FindTreeItem(IMAGE_SYS_COLORS);
				if (hItem)
				{
					int nImage, nSelected;
					HTREEITEM hChild = m_Tree.GetNextItem(hItem, TVGN_CHILD);
					while (hChild)
					{
						m_Tree.GetItemImage(hChild, nImage, nSelected);
						switch (nImage)
						{
							case COLOR_GRAYTEXT:
								m_Tree.SetItemData(hChild, cDisabled);
								break;
							case COLOR_HIGHLIGHTTEXT:
								m_Tree.SetItemData(hChild, cSelection);
								break;
							case COLOR_WINDOWTEXT:
							case COLOR_MENUTEXT:
							case COLOR_BTNTEXT:
								m_Tree.SetItemData(hChild, dwData);
								break;
						}
						hChild = m_Tree.GetNextItem(hChild, TVGN_NEXT);
					}
				}
			}
			Update();
		}
		else if (sProperty == OEM_HILIGHT_COLOR)
		{
			SetOEMColors(dwData, GetShadowColor());
			Update();
		}
		else if (sProperty == OEM_SHADOW_COLOR)
		{
			SetOEMColors(GetHilightColor(), dwData);
			Update();
		}
	}
	if (nImage == IMAGE_BUTTON)
	{
		bool bFirst = true;
		CSkinButton*pBtn =(CSkinButton*) GetDlgItem(RUNTIME_CLASS(CSkinButton));
		while (pBtn)
		{
			if (sProperty == OEM_SHAPE)
			{
				pBtn->SetShape((enumSkinButtonShape)dwData);
				if (bFirst) CSkinButton::SetOEMShape((enumSkinButtonShape)dwData);
			}
			else if (sProperty == OEM_SURFACE)
			{
				pBtn->SetSurface((enumSkinButtonSurface)dwData);
				if (bFirst) CSkinButton::SetOEMSurface((enumSkinButtonSurface)dwData);
			}
			else if (sProperty == OEM_BASE_COLOR)
			{
				pBtn->SetBaseColor(dwData);
				if (bFirst)	CSkinButton::SetOEMColors(dwData, CSkinButton::GetTextColorNormalG(),
					CSkinButton::GetTextColorHighlightedG(), CSkinButton::GetTextColorDisabledG(), CSkinButton::GetKeyColorG());
			}
			else if (sProperty == OEM_TEXT_COLOR)
			{
				pBtn->SetTextColorNormal(dwData);
				if (bFirst)	CSkinButton::SetOEMColors(CSkinButton::GetBaseColorG(), dwData,
					CSkinButton::GetTextColorHighlightedG(), CSkinButton::GetTextColorDisabledG(), CSkinButton::GetKeyColorG());
			}
			else if (sProperty == OEM_TEXT_COLOR_HILIGHTED)
			{
				pBtn->SetTextColorHighlighted(dwData);
				if (bFirst)	CSkinButton::SetOEMColors(CSkinButton::GetBaseColorG(), CSkinButton::GetTextColorNormalG(),
					dwData, CSkinButton::GetTextColorDisabledG(), CSkinButton::GetKeyColorG());
			}
			else if (sProperty == OEM_TEXT_COLOR_DISABLED)
			{
				pBtn->SetTextColorDisabled(dwData);
				if (bFirst)	CSkinButton::SetOEMColors(CSkinButton::GetBaseColorG(), CSkinButton::GetTextColorNormalG(),
					CSkinButton::GetTextColorHighlightedG(), dwData, CSkinButton::GetKeyColorG());
			}
			bFirst = false;
			pBtn =(CSkinButton*) GetDlgItem(RUNTIME_CLASS(CSkinButton), pBtn);
		}
		Update();
	}
	if (nImage == IMAGE_STATIC)
	{
		CSkinStatic*pStatic =(CSkinStatic*) GetDlgItem(RUNTIME_CLASS(CSkinStatic));
		while (pStatic)
		{
			if (sProperty == OEM_BACKGROUND_COLOR)
			{
				pStatic->SetBackgroundColor(dwData);
			}
			else if (sProperty == OEM_TEXT_COLOR)
			{
				pStatic->SetTextColor(dwData);
			}
			else if (sProperty == OEM_HILIGHT_COLOR)
			{
				pStatic->SetLineColorLight(dwData);
			}
			else if (sProperty == OEM_SHADOW_COLOR)
			{
				pStatic->SetLineColorDark(dwData);
			}
			pStatic =(CSkinStatic*) GetDlgItem(RUNTIME_CLASS(CSkinStatic), pStatic);
		}
	}

	if (nImage == IMAGE_LIST_BOX)
	{
		CSkinListBox*pListBox =(CSkinListBox*) GetDlgItem(RUNTIME_CLASS(CSkinListBox));
		while (pListBox)
		{
			if (sProperty == OEM_BACKGROUND_COLOR)
			{
				pListBox->SetBackgroundColorNormal(dwData);
			}
			else if (sProperty == OEM_TEXT_COLOR)
			{
				pListBox->SetTextColorNormal(dwData);
			}
			else if (sProperty == OEM_SELECTED_BACKGROUND)
			{
				pListBox->SetBackgroundColorHighlighted(dwData);
			}
			else if (sProperty == OEM_SELECTED_TEXT)
			{
				pListBox->SetTextColorHighlighted(dwData);
			}
			pListBox =(CSkinListBox*) GetDlgItem(RUNTIME_CLASS(CSkinListBox), pListBox);
		}
	}

	if (nImage == IMAGE_SLIDER)
	{
		CSkinSlider*pSlider =(CSkinSlider*) GetDlgItem(RUNTIME_CLASS(CSkinSlider));
		while (pSlider)
		{
			if (sProperty == OEM_SHAPE)
			{
				pSlider->SetShape((enumSkinButtonShape)dwData);
			}
			else if (sProperty == OEM_SURFACE)
			{
				pSlider->SetSurface((enumSkinButtonSurface)dwData);
			}
			else if (sProperty == OEM_BACKGROUND_COLOR)
			{
				pSlider->SetBaseColor(dwData);
			}
			else if (sProperty == OEM_TEXT_COLOR)
			{
				pSlider->SetTextColorNormal(dwData);
			}
			else if (sProperty == OEM_HILIGHT_COLOR)
			{
				pSlider->SetColorLineLight(dwData);
			}
			else if (sProperty == OEM_SHADOW_COLOR)
			{
				pSlider->SetColorLineDark(dwData);
			}
			pSlider =(CSkinSlider*) GetDlgItem(RUNTIME_CLASS(CSkinSlider), pSlider);
		}
	}
	if (nImage == IMAGE_GROUP_BOX)
	{
		CSkinGroupBox*pGroupBox =(CSkinGroupBox*) GetDlgItem(RUNTIME_CLASS(CSkinGroupBox));
		while (pGroupBox)
		{
			if (sProperty == OEM_TEXT_COLOR)
			{
				pGroupBox->SetTextColorNormal(dwData);
			}
			else if (sProperty == OEM_TEXT_COLOR_DISABLED)
			{
				pGroupBox->SetTextColorDisabled(dwData);
			}
			else if (sProperty == OEM_HILIGHT_COLOR)
			{
				pGroupBox->SetLineColorLight(dwData);
			}
			else if (sProperty == OEM_SHADOW_COLOR)
			{
				pGroupBox->SetLineColorDark(dwData);
			}
			pGroupBox =(CSkinGroupBox*) GetDlgItem(RUNTIME_CLASS(CSkinGroupBox), pGroupBox);
		}
	}
	if (nImage == IMAGE_EDIT)
	{
		CSkinEdit*pEdit =(CSkinEdit*) GetDlgItem(RUNTIME_CLASS(CSkinEdit));
		while (pEdit)
		{
			if (sProperty == OEM_BACKGROUND_COLOR)
			{
				pEdit->SetBackgroundColor(dwData);
			}
			else if (sProperty == OEM_TEXT_COLOR)
			{
				pEdit->SetTextColor(dwData);
			}
			pEdit =(CSkinEdit*) GetDlgItem(RUNTIME_CLASS(CSkinEdit), pEdit);
		}
	}
	if (nImage == IMAGE_PROGRESS)
	{
		CSkinProgress*pProgress =(CSkinProgress*) GetDlgItem(RUNTIME_CLASS(CSkinProgress));
		while (pProgress)
		{
			if (sProperty == OEM_BACKGROUND_COLOR)
			{
				pProgress->SetBackGroundColor(dwData);
			}
			else if (sProperty == OEM_PROGRESS_COLOR)
			{
				pProgress->SetProgressColor(dwData);
			}
			pProgress =(CSkinProgress*) GetDlgItem(RUNTIME_CLASS(CSkinProgress), pProgress);
		}
	}
	if (nImage == IMAGE_TREE)
	{
		CSkinTree*pTree =(CSkinTree*) GetDlgItem(RUNTIME_CLASS(CSkinTree));
		while (pTree)
		{
			if (sProperty == OEM_BACKGROUND_COLOR)
			{
				pTree->SetBkColor(dwData);
			}
			else if (sProperty == OEM_TEXT_COLOR)
			{
				pTree->SetTextColor(dwData);
			}
			else if (sProperty == OEM_LINE_COLOR)
			{
				pTree->SetLineColor(dwData);
			}
			pTree =(CSkinTree*) GetDlgItem(RUNTIME_CLASS(CSkinTree), pTree);
		}
	}
	if (nImage == IMAGE_LIST_CTRL)
	{
		CSkinListCtrlX*pListCtrl =(CSkinListCtrlX*) GetDlgItem(RUNTIME_CLASS(CSkinListCtrlX));
		while (pListCtrl)
		{
			if (sProperty == OEM_HEADER_BUTTON_SHAPE)
			{
				pListCtrl->SetShape((enumSkinButtonShape)dwData);
			}
			else if (sProperty == OEM_HEADER_BUTTON_SURFACE)
			{
				pListCtrl->SetSurface((enumSkinButtonSurface)dwData);
			}
			else if (sProperty == OEM_HEADER_BACKGROUND_COLOR)
			{
				pListCtrl->SetHeaderBkColor(dwData);
			}
			else if (sProperty == OEM_HEADER_TEXT_COLOR)
			{
				pListCtrl->SetHeaderTextColor(dwData);
			}
			else if (sProperty == OEM_BASE_COLOR)
			{
				pListCtrl->SetBkColor(dwData);
			}
			else if (sProperty == OEM_TEXT_BACKGROUND_COLOR)
			{
				pListCtrl->SetTextBkColor(dwData);
			}
			else if (sProperty == OEM_TEXT_COLOR)
			{
				pListCtrl->SetTextColor(dwData);
			}
			pListCtrl =(CSkinListCtrlX*) GetDlgItem(RUNTIME_CLASS(CSkinListCtrlX), pListCtrl);
		}
	}
	if (nImage == IMAGE_COMBO)
	{
		CSkinComboBox*pComboBox =(CSkinComboBox*) GetDlgItem(RUNTIME_CLASS(CSkinComboBox));
		while (pComboBox)
		{
			if (sProperty == OEM_SHAPE)
			{
				pComboBox->SetShape((enumSkinButtonShape)dwData);
			}
			else if (sProperty == OEM_SURFACE)
			{
				pComboBox->SetSurface((enumSkinButtonSurface)dwData);
			}
			else if (sProperty == OEM_BACKGROUND_COLOR)
			{
				pComboBox->SetBackgroundColor(dwData);
			}
			else if (sProperty == OEM_TEXT_COLOR)
			{
				pComboBox->SetTextColor(dwData);
			}
			pComboBox =(CSkinComboBox*) GetDlgItem(RUNTIME_CLASS(CSkinComboBox), pComboBox);
		}
	}
	if (nImage == IMAGE_MENU)
	{
		if (sProperty == OEM_SHAPE)
		{
			GetSkinMenu()->SetShape((enumSkinButtonShape)dwData);
		}
		else if (sProperty == OEM_SURFACE)
		{
			GetSkinMenu()->SetSurface((enumSkinButtonSurface)dwData);
		}
		else if (sProperty == OEM_BACKGROUND_COLOR)
		{
			GetSkinMenu()->SetBaseColor(dwData);
		}
		else if (sProperty == OEM_TEXT_COLOR)
		{
			GetSkinMenu()->SetTextColorNormal(dwData);
		}
	}
	if (nImage == IMAGE_SYS_COLORS)
	{
		if (sProperty == OEM_BUTTON_FACE)
		{
			afxData.clrBtnFace = dwData;
		}
		else if (sProperty == OEM_HILIGHT)
		{
			afxData.clrBtnHilite = dwData;
		}
		else if (sProperty == OEM_SHADOW)
		{
			afxData.clrBtnShadow = dwData;
		}
		else if (sProperty == OEM_BUTTON_TEXT)
		{
			afxData.clrBtnText = dwData;
		}
		else if (sProperty == OEM_WINDOW_FRAME)
		{
			afxData.clrWindowFrame = dwData;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COEMSkinOptionsDlg::OnFileLoad()
{
	CFileDialog dialog(TRUE, _T("ini"), m_sInifile, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Ini Datei (*.ini)|*.ini||"), this);
	if (dialog.DoModal()	== IDOK)
	{
		m_sInifile = dialog.GetPathName();
		LoadOEM(TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK SendUpdateUI(HWND hwnd, LPARAM /*lParam*/)
{
	::SendMessage(hwnd, WM_UPDATEUISTATE, 0, 0);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void COEMSkinOptionsDlg::LoadOEM(BOOL bApply/*=FALSE*/)
{
	CString sGroup, sProperty;
	LPARAM lParam;
	HTREEITEM hItem = m_Tree.GetRootItem();
	int nImage, nSelected;
	int nImageC, nSelectedC;
	while (hItem)
	{
		sGroup = m_Tree.GetItemText(hItem);
//		TRACE(_T("%s\n"), sGroup);
		m_Tree.GetItemImage(hItem, nImage, nSelected);
		HTREEITEM hChild = m_Tree.GetNextItem(hItem, TVGN_CHILD);
		while (hChild)
		{
			m_Tree.GetItemImage(hChild, nImageC, nSelectedC);
			sProperty = m_Tree.GetItemText(hChild);
			lParam = m_Tree.GetItemData(hChild);
// Hex Colors would be nice
//			if (nSelectedC == IMAGE_PROP_COLOR)
//			{
//				lParam = GetPrivateProfileHex(sGroup, sProperty, (int)lParam, m_sInifile);
//			}
//			else
			{
				lParam = GetPrivateProfileInt(sGroup, sProperty, (int)lParam, m_sInifile);
			}
			m_Tree.SetItemData(hChild, lParam);
			if (bApply)
			{
				ApplyProperty((eControlType)nSelected, sProperty, (DWORD)lParam);
			}
//			TRACE(_T("   %s\n"), sChild);
			hChild = m_Tree.GetNextItem(hChild, TVGN_NEXT);
		}
		hItem = m_Tree.GetNextItem(hItem, TVGN_NEXT);
	}
	if (bApply)
	{
		EnumChildWindows(m_hWnd, SendUpdateUI, 0);
		FlashWindow(TRUE);
		FlashWindow(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COEMSkinOptionsDlg::OnUpdateFileLoad(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}
/////////////////////////////////////////////////////////////////////////////
void COEMSkinOptionsDlg::OnFileSaveas()
{
	CFileDialog dialog(FALSE, _T("ini"), m_sInifile, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Ini Datei (*.ini)|*.ini||"), this);
	if (dialog.DoModal()	== IDOK)
	{
		m_sInifile = dialog.GetPathName();
		OnFileSave();
	}
}

void COEMSkinOptionsDlg::OnFileSave()
{
	CString sGroup, sProperty, str;
	LPARAM lParam;
	int nImage, nSelected;
	HTREEITEM hItem = m_Tree.GetRootItem();
	while (hItem)
	{
		sGroup = m_Tree.GetItemText(hItem);
//		TRACE(_T("%s\n"), sGroup);
		HTREEITEM hChild = m_Tree.GetNextItem(hItem, TVGN_CHILD);
		while (hChild)
		{
			m_Tree.GetItemImage(hChild, nImage, nSelected);
			if (nSelected != IMAGE_PROP_INVALID)
			{
				sProperty = m_Tree.GetItemText(hChild);
				lParam = m_Tree.GetItemData(hChild);
// Hex Colors would be nice
//				if (nSelected == IMAGE_PROP_COLOR)
//				{
//					str.Format(_T("%08x"), lParam);
//				}
//				else
				{
					str.Format(_T("%d"), lParam);
				}
				WritePrivateProfileString(sGroup, sProperty, str, m_sInifile);
			}
//			TRACE(_T("   %s\n"), sChild);
			hChild = m_Tree.GetNextItem(hChild, TVGN_NEXT);
		}
		hItem = m_Tree.GetNextItem(hItem, TVGN_NEXT);
	}
	str.Format(_T("%d"), m_BtnCancel.GetChangeOnlyBrightness());
	WritePrivateProfileString(OEM_DIALOG, OEM_CHANGE_ONLY_BRIGHTNESS, str, m_sInifile);
	str.Format(_T("%d"), (int)m_BtnCancel.GetMaxColorChangeRange());
	WritePrivateProfileString(OEM_DIALOG, OEM_MAX_COLOR_RANGE, str, m_sInifile);
	str.Format(_T("%d"), m_BtnCancel.GetBrushEffectFactor());
	WritePrivateProfileString(OEM_DIALOG, OEM_BRUSH_EFFECT_FACTOR, str, m_sInifile);
	str = _T("1");
	WritePrivateProfileString(OEM_DIALOG, OEM_USE_OEM_SKIN, str, m_sInifile);
	str = _T("0");
	WritePrivateProfileString(OEM_DIALOG, OEM_CHANGE_SYS_COLORS, str, m_sInifile);
}

void COEMSkinOptionsDlg::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}
/////////////////////////////////////////////////////////////////////////////
void COEMSkinOptionsDlg::OnUpdateFileSaveas(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}
/////////////////////////////////////////////////////////////////////////////
COLORREF COEMSkinOptionsDlg::ChooseColor(COLORREF colInit)
{
	CColorDialog dlg(colInit, CC_FULLOPEN|CC_ANYCOLOR, this);
	COLORREF customcolors[16] = 
	{
		SKIN_COLOR_VIDETE_CI,			// 1
		SKIN_COLOR_GOLD_METALLIC,		// 2
		SKIN_COLOR_GOLD_METALLIC_LIGHT,	// 3
		SKIN_COLOR_SILVER_METALLIC,		// 4
		SKIN_COLOR_DARK_RED,			// 5
		SKIN_COLOR_RED,					// 6
		SKIN_COLOR_GREEN,				// 7
		SKIN_COLOR_GREEN_SHADOW,		// 8
		SKIN_COLOR_GREEN_SHADOW_DARK,	// 9
		SKIN_COLOR_BLUE,				// 10
		SKIN_COLOR_BLUE_SHADOW,			// 11
		SKIN_COLOR_BLUE_SHADOW_DARK,	// 12
		SKIN_COLOR_GREY_DARK,			// 13
		SKIN_COLOR_GREY_MEDIUM_DARK,	// 14
		SKIN_COLOR_GREY,				// 15
		SKIN_COLOR_GREY_MEDIUM_LIGHT	// 16
	};
	dlg.m_cc.lpCustColors = customcolors;
	if (dlg.DoModal() == IDOK)
	{
		colInit = dlg.GetColor();
	}
	else
	{
		colInit = (COLORREF)-1;
	}
	return colInit;
}
/////////////////////////////////////////////////////////////////////////////
DWORD COEMSkinOptionsDlg::ChooseShape(DWORD dwInit)
{
	CPoint pt;
	GetCursorPos(&pt);
	CMenu *pSubMenu = m_Context.GetSubMenu(0);
	int nCmd = (int)dwInit;
	if (pSubMenu)
	{
		pSubMenu->CheckMenuRadioItem(0, 3, dwInit-1, MF_BYPOSITION);
		pSubMenu->EnableMenuItem(2, MF_BYPOSITION|MF_DISABLED|MF_GRAYED);
		nCmd = pSubMenu->TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, this);
		if (nCmd == 0) return (DWORD)-1;
		nCmd = nCmd - ID_SHAPE_RECTANGLE + 1;
	}
	if (nCmd == (int)dwInit)
	{
		dwInit = (DWORD)-1;
	}
	else
	{
		dwInit = (DWORD)nCmd;
	}
	return dwInit;
}
/////////////////////////////////////////////////////////////////////////////
DWORD COEMSkinOptionsDlg::ChooseSurface(DWORD dwInit)
{
	CPoint pt;
	GetCursorPos(&pt);
	CMenu *pSubMenu = m_Context.GetSubMenu(1);
	int nCmd = (int)dwInit;
	if (pSubMenu)
	{
		pSubMenu->CheckMenuRadioItem(0, 5, dwInit, MF_BYPOSITION);
		nCmd = pSubMenu->TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, this);
		if (nCmd == 0) return (DWORD)-1;
		nCmd = nCmd - ID_SURFACE_PLANECOLOR;
	}
	if (nCmd == (int)dwInit)
	{
		dwInit = (DWORD)-1;
	}
	else
	{
		dwInit = (DWORD)nCmd;
	}
	return dwInit;
}
/////////////////////////////////////////////////////////////////////////////
DWORD COEMSkinOptionsDlg::ChooseBkGndStyle(DWORD dwInit)
{
	CPoint pt;
	GetCursorPos(&pt);
	CMenu *pSubMenu = m_Context.GetSubMenu(2);
	int nCmd = (int)dwInit;
	if (pSubMenu)
	{
		pSubMenu->CheckMenuRadioItem(0, 2, dwInit-2, MF_BYPOSITION);
		nCmd = pSubMenu->TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, this);
		if (nCmd == 0) return (DWORD)-1;
		nCmd = nCmd - ID_BKSTYLE_PLANECOLOR + 2;
	}
	if (nCmd == (int)dwInit)
	{
		dwInit = (DWORD)-1;
	}
	else
	{
		dwInit = (DWORD)nCmd;
	}

	return dwInit;
}
/////////////////////////////////////////////////////////////////////////////
CWnd * COEMSkinOptionsDlg::GetDlgItem(CRuntimeClass*pRC, CWnd*pWndPrevious)
{
	CWnd *pWnd = (pWndPrevious == NULL) ? GetWindow(GW_CHILD) : pWndPrevious->GetNextWindow();
	while (pWnd && IsChild(pWnd))
	{
		if (pWnd->IsKindOf(pRC))
		{
			return pWnd;
		}
		pWnd = pWnd->GetNextWindow();
	};
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM COEMSkinOptionsDlg::FindTreeItem(int nItemImg)
{
	HTREEITEM hItem = m_Tree.GetRootItem();
	int nImage, nSelected;
	while (hItem)
	{
		m_Tree.GetItemImage(hItem, nImage, nSelected);
		ASSERT(nImage == IMAGE_PARENT);
		if (nSelected == nItemImg)
		{
			return hItem;
		}
		hItem = m_Tree.GetNextItem(hItem, TVGN_NEXT);
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM COEMSkinOptionsDlg::FindTreeItem(HTREEITEM hItem, CString sText)
{
	if (hItem)
	{
		HTREEITEM hChild = m_Tree.GetNextItem(hItem, TVGN_CHILD);
		while (hChild)
		{
			if (m_Tree.GetItemText(hChild) == sText)
			{
				return hChild;
			}
			hChild = m_Tree.GetNextItem(hChild, TVGN_NEXT);
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void COEMSkinOptionsDlg::OnTestEnableok()
{
	CWnd *pWnd = CDialog::GetDlgItem(IDOK);
	pWnd->EnableWindow(!pWnd->IsWindowEnabled());
	m_btnIcon.EnableWindow(pWnd->IsWindowEnabled());
	// TODO: Add your command handler code here
}

void COEMSkinOptionsDlg::OnUpdateTestEnableok(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(CDialog::GetDlgItem(IDOK)->IsWindowEnabled());
}

void COEMSkinOptionsDlg::OnDestroy()
{
	CSkinDialog::OnDestroy();
}

void COEMSkinOptionsDlg::OnBnClickedBtnIcon()
{
	TRACE(_T("OnBnClickedBtnIcon\n"));

	CSkinPropertyPage page1(IDD_ABOUTBOX, IDS_PAGE1);
	CSkinPropertyPage page2(IDD_OEMSKINOPTIONS_DIALOG, IDS_PAGE2);
	CSkinPropertyPage page3(IDD_ABOUTBOX, IDS_PAGE3);
	CSkinPropertySheet sheet;
//	sheet.SetWizardMode(); // RKE: does not work properly
	sheet.AddPage(&page1);
	sheet.AddPage(&page2);
	sheet.AddPage(&page3);

	sheet.DoModal();
//	m_Slider.SetPos(50);
}

void COEMSkinOptionsDlg::OnParentNotify(UINT message, LPARAM lParam)
{
	CSkinDialog::OnParentNotify(message, lParam);

	// TODO: Add your message handler code here
}
BOOL COEMSkinOptionsDlg::GetToolTip(UINT nID, CString&sText)
{
	return FALSE;
}
