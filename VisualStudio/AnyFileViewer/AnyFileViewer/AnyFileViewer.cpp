// AnyFileViewer.cpp : Definiert das Klassenverhalten für die Anwendung.
//

#include "stdafx.h"
#include "AnyFileViewer.h"
#include "MainFrm.h"

#include "AnyFileViewerDoc.h"
#include "FormatView.h"
#include "DisplayType.h"
#include "RepresentationDlg.h"

#include "afxregpath.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAnyFileViewerApp

BEGIN_MESSAGE_MAP(CAnyFileViewerApp, CWinApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    // Dateibasierte Standarddokumentbefehle
    ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
    ON_UPDATE_COMMAND_UI(ID_SETTINGS_REGISTRY, OnUpdateSettingsRegistry)
    ON_COMMAND(ID_SETTINGS_REGISTRY, OnSettingsRegistry)
    ON_COMMAND(ID_SETTINGS_REGISTER_FILE_TYPE, OnSettingsRegisterFileType)
    ON_UPDATE_COMMAND_UI(ID_SETTINGS_REGISTER_FILE_TYPE, OnUpdateSettingsRegisterFileType)
    ON_COMMAND(ID_SEPARATOR_COMMA, &CAnyFileViewerApp::OnSeparatorComma)
    ON_COMMAND(ID_SEPARATOR_SEMICOLON, &CAnyFileViewerApp::OnIdSeparatorSemicolon)
    ON_COMMAND(ID_SEPARATOR_LF, &CAnyFileViewerApp::OnSeparatorLf)
    ON_COMMAND(ID_SEPARATOR_CR, &CAnyFileViewerApp::OnSeparatorCr)
    ON_UPDATE_COMMAND_UI(ID_SEPARATOR_COMMA, &CAnyFileViewerApp::OnUpdateSeparatorComma)
    ON_UPDATE_COMMAND_UI(ID_SEPARATOR_SEMICOLON, &CAnyFileViewerApp::OnUpdateIdSeparatorSemicolon)
    ON_UPDATE_COMMAND_UI(ID_SEPARATOR_LF, &CAnyFileViewerApp::OnUpdateSeparatorLf)
    ON_UPDATE_COMMAND_UI(ID_SEPARATOR_CR, &CAnyFileViewerApp::OnUpdateSeparatorCr)
    ON_COMMAND(ID_SEPARATOR_TAB, &CAnyFileViewerApp::OnSeparatorTab)
    ON_UPDATE_COMMAND_UI(ID_SEPARATOR_TAB, &CAnyFileViewerApp::OnUpdateSeparatorTab)
    ON_COMMAND(ID_EDIT_COPY_TYPE, &CAnyFileViewerApp::OnEditCopyType)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY_TYPE, &CAnyFileViewerApp::OnUpdateEditCopyType)
    ON_COMMAND(ID_EDIT_COPY_NAME, &CAnyFileViewerApp::OnEditCopyName)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY_NAME, &CAnyFileViewerApp::OnUpdateEditCopyName)
    ON_COMMAND(ID_OPTIONS_REPRESENTATION, &CAnyFileViewerApp::OnOptionsRepresentation)
    ON_UPDATE_COMMAND_UI(ID_OPTIONS_REPRESENTATION, &CAnyFileViewerApp::OnUpdateOptionsRepresentation)
END_MESSAGE_MAP()


// CAnyFileViewerApp-Erstellung

CAnyFileViewerApp::CAnyFileViewerApp(): mSaveRegistry(0)
, m_bDifferentEndianFormat(FALSE)
, mCurrentFormat(Clipboard)
, mcxViewPane0(333)
, mcxViewPane1(333)
, mcxViewPane1_0(800)
{
    EnableHtmlHelp();

    m_DisplayTypes.push_back(new CDisplayChar);
    m_DisplayTypes.push_back(new CDisplayUChar);
    m_DisplayTypes.push_back(new CDisplayShort);
    m_DisplayTypes.push_back(new CDisplayUShort);
    m_DisplayTypes.push_back(new CDisplayLong);
    m_DisplayTypes.push_back(new CDisplayULong);
    m_DisplayTypes.push_back(new CDisplayLongLong);
    m_DisplayTypes.push_back(new CDisplayULongLong);
    m_DisplayTypes.push_back(new CDisplayFloat);
    m_DisplayTypes.push_back(new CDisplayDouble);
    m_DisplayTypes.push_back(new CDisplayHEX2);
    m_DisplayTypes.push_back(new CDisplayHEX4);
    m_DisplayTypes.push_back(new CDisplayHEX8);
    m_DisplayTypes.push_back(new CDisplayHEX16);
    m_DisplayTypes.push_back(new CDisplayAscii);
    m_DisplayTypes.push_back(new CDisplayUnicode);
    m_DisplayTypes.push_back(new CDisplayBinary);

    TCHAR szLocale[LOCALE_NAME_MAX_LENGTH];
    GetUserDefaultLocaleName(szLocale, LOCALE_NAME_MAX_LENGTH);

    mSeparatorComma[Clipboard]     = FALSE;
    mSeparatorSemicolon[Clipboard] = FALSE;
    mSeparatorCR[Clipboard]        = TRUE;
    mSeparatorLF[Clipboard]        = TRUE;
    mSeparatorTAB[Clipboard]       = FALSE;
    mCopyType[Clipboard]           = TRUE;
    mCopyName[Clipboard]           = TRUE;
    mFloatFormat[Clipboard]        = _T("%f");
    mDoubleFormat[Clipboard]       = _T("%f");
    mLocale[Clipboard]             = szLocale;

    mSeparatorComma[InlineItems]     = FALSE;
    mSeparatorSemicolon[InlineItems] = FALSE;
    mSeparatorCR[InlineItems]        = FALSE;
    mSeparatorLF[InlineItems]        = FALSE;
    mSeparatorTAB[InlineItems]       = FALSE;
    mCopyType[InlineItems]           = FALSE;
    mCopyName[InlineItems]           = FALSE;
    mFloatFormat[InlineItems]        = _T("%f");
    mDoubleFormat[InlineItems]       = _T("%f");
    mLocale[InlineItems]             = szLocale;
    TCHAR fGrouping[4];
    GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SLIST, (LPWSTR)fGrouping,  sizeof(fGrouping) / sizeof(TCHAR));
    mSeparatorExtra[InlineItems]     = fGrouping;

    mSeparatorComma[Excel]     = FALSE;
    mSeparatorSemicolon[Excel] = FALSE;
    mSeparatorCR[Excel]        = TRUE;  // CR/LF um Items zu kopieren.
    mSeparatorLF[Excel]        = TRUE;
    mSeparatorTAB[Excel]       = FALSE; // TAB um Inline attribute zu separieren
    mCopyType[Excel]           = FALSE;
    mCopyName[Excel]           = FALSE;
    mFloatFormat[Excel]        = _T("%.13e");
    mDoubleFormat[Excel]       = _T("%.7e");
    mLocale[Excel]             = szLocale;
    // TODO! Rule benutzen, um Excel Formate zu kopieren.
}

CAnyFileViewerApp::~CAnyFileViewerApp()
{
   std::vector<CDisplayType*>::iterator iter = m_DisplayTypes.begin();
   for (; iter != m_DisplayTypes.end(); iter++)
   {
      delete *iter;
   }
}

CDisplayType& CAnyFileViewerApp::GetDisplayType( int aType) const
{
    static CDisplayType dummy;
    if (aType < m_DisplayTypes.size())
    {
        ASSERT(m_DisplayTypes[aType]->getType() == aType);
        return *m_DisplayTypes[aType];
    }
    return dummy;
}


// Das einzige CAnyFileViewerApp-Objekt

CAnyFileViewerApp theApp;

// CAnyFileViewerApp Initialisierung

BOOL CAnyFileViewerApp::InitInstance()
{
    // InitCommonControls() ist für Windows XP erforderlich, wenn ein Anwendungsmanifest
    // die Verwendung von ComCtl32.dll Version 6 oder höher zum Aktivieren
    // von visuellen Stilen angibt. Ansonsten treten beim Erstellen von Fenstern Fehler auf.
    InitCommonControls();

    CWinApp::InitInstance();
    CoInitialize(NULL);

    m_sPath = m_pszHelpFilePath;
    int n = m_sPath.ReverseFind(_T('\\'));
    if (n != -1)
    {
        m_sPath = m_sPath.Left(n+1);
    }

    SetRegistryKey(_T("KESoft"));
    CString sKey = AFXGetRegPath(_T(""), 0);
    HKEY hKey = NULL;
    sKey = sKey.Left(sKey.GetLength()-1);
    sKey += SECTION_SETTINGS;
    RegOpenKeyEx(HKEY_CURRENT_USER, sKey, 0, KEY_READ, &hKey);
    if (hKey)
    {
        DWORD dwType, dwSize = sizeof(DWORD);
        RegQueryValueEx(hKey, NAME_OF(mSaveRegistry), NULL, &dwType, (LPBYTE)&mSaveRegistry, &dwSize);
        RegCloseKey(hKey);
    }

    mSaveRegistry = !mSaveRegistry;   // invert state because
    OnSettingsRegistry();             // this function inverts the state, too

    LoadStdProfileSettings(10);  // Standard INI-Dateioptionen laden (einschließlich MRU)

    mFormatFilesDir = GetProfileString(SECTION_SETTINGS, OF_MEMBER(mFormatFilesDir));
    if (mFormatFilesDir.IsEmpty())
    {
        mFormatFilesDir = m_sPath + _T("Formats\\");
    }

    mSeparatorComma[Clipboard]     = GetProfileInt(SECTION_SETTINGS, OF_MEMBER(mSeparatorComma[Clipboard]));
    mSeparatorCR[Clipboard]        = GetProfileInt(SECTION_SETTINGS, OF_MEMBER(mSeparatorCR[Clipboard]));
    mSeparatorLF[Clipboard]        = GetProfileInt(SECTION_SETTINGS, OF_MEMBER(mSeparatorLF[Clipboard]));
    mSeparatorSemicolon[Clipboard] = GetProfileInt(SECTION_SETTINGS, OF_MEMBER(mSeparatorSemicolon[Clipboard]));
    mSeparatorTAB[Clipboard]       = GetProfileInt(SECTION_SETTINGS, OF_MEMBER(mSeparatorTAB[Clipboard]));
    mCopyType[Clipboard]           = GetProfileInt(SECTION_SETTINGS, OF_MEMBER(mCopyType[Clipboard]));
    mCopyName[Clipboard]           = GetProfileInt(SECTION_SETTINGS, OF_MEMBER(mCopyName[Clipboard]));
    mLocale[Clipboard]             = GetProfileString(SECTION_SETTINGS, OF_MEMBER(mLocale[Clipboard]));
    mSeparatorExtra[Clipboard]     = GetProfileString(SECTION_SETTINGS, OF_MEMBER(mSeparatorExtra[Clipboard]));
    mcxViewPane0                   = GetProfileInt(SECTION_SETTINGS, OF_MEMBER(mcxViewPane0));
    mcxViewPane1                   = GetProfileInt(SECTION_SETTINGS, OF_MEMBER(mcxViewPane1));
    mcxViewPane1_0                 = GetProfileInt(SECTION_SETTINGS, OF_MEMBER(mcxViewPane1_0));

    CSingleDocTemplate* pDocTemplate;
    pDocTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(CAnyFileViewerDoc),
        RUNTIME_CLASS(CMainFrame),       // Haupt-SDI-Rahmenfenster
        RUNTIME_CLASS(CFormatView));
    if (!pDocTemplate)
        return FALSE;
    AddDocTemplate(pDocTemplate);
    // DDE-Execute-Open aktivieren
    EnableShellOpen();
    RegisterShellFileTypes(TRUE);
    // Befehlszeile parsen, um zu prüfen auf Standardumgebungsbefehle DDE, Datei offen
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);
    // Verteilung der in der Befehlszeile angegebenen Befehle. Es wird FALSE zurückgegeben, wenn
    // die Anwendung mit /RegServer, /Register, /Unregserver oder /Unregister gestartet wurde.
    if (!ProcessShellCommand(cmdInfo))
        return FALSE;
    // Das einzige Fenster ist initialisiert und kann jetzt angezeigt und aktualisiert werden.
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();
    m_pMainWnd->DragAcceptFiles();
    return TRUE;
}

CMainFrame *CAnyFileViewerApp::GetMainFrame()
{
   if (m_pMainWnd)
   {
      ASSERT_KINDOF(CMainFrame, m_pMainWnd);
      return (CMainFrame *)m_pMainWnd;
   }
   return NULL;
}


// CAboutDlg-Dialogfeld für Anwendungsbefehl 'Info'

class CAboutDlg : public CDialog
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
    afx_msg void OnBnClickedOk();
    virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// Anwendungsbefehl zum Ausführen des Dialogfelds
void CAnyFileViewerApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}


// CAnyFileViewerApp Meldungshandler


int CAnyFileViewerApp::ExitInstance()
{
   WriteProfileString(SECTION_SETTINGS, OF_MEMBER(mFormatFilesDir));

   WriteProfileInt(SECTION_SETTINGS, OF_MEMBER(mSaveRegistry));
   WriteProfileInt(SECTION_SETTINGS, OF_MEMBER(mSeparatorComma[Clipboard]));
   WriteProfileInt(SECTION_SETTINGS, OF_MEMBER(mSeparatorSemicolon[Clipboard]));
   WriteProfileInt(SECTION_SETTINGS, OF_MEMBER(mSeparatorCR[Clipboard]));
   WriteProfileInt(SECTION_SETTINGS, OF_MEMBER(mSeparatorLF[Clipboard]));
   WriteProfileInt(SECTION_SETTINGS, OF_MEMBER(mSeparatorTAB[Clipboard]));
   WriteProfileInt(SECTION_SETTINGS, OF_MEMBER(mCopyType[Clipboard]));
   WriteProfileInt(SECTION_SETTINGS, OF_MEMBER(mCopyName[Clipboard]));
   WriteProfileString(SECTION_SETTINGS, OF_MEMBER(mLocale[Clipboard]));
   WriteProfileString(SECTION_SETTINGS, OF_MEMBER(mSeparatorExtra[Clipboard]));
   WriteProfileInt(SECTION_SETTINGS, OF_MEMBER(mcxViewPane0));
   WriteProfileInt(SECTION_SETTINGS, OF_MEMBER(mcxViewPane1));
   WriteProfileInt(SECTION_SETTINGS, OF_MEMBER(mcxViewPane1_0));
   CoUninitialize();

   return CWinApp::ExitInstance();
}

void CAnyFileViewerApp::OnSettingsRegistry()
{
    if (mSaveRegistry == 0)
    {
        free((void*)m_pszRegistryKey);
        m_pszRegistryKey = 0;
        SetRegistryKey(_T("KESoft"));
        mSaveRegistry = 1;
    }
    else
    {
        CString sProfile(m_sPath);
        sProfile += m_pszAppName;
        sProfile += _T(".ini");
        free((void*)m_pszRegistryKey);
        m_pszRegistryKey = 0;
        free((void*)m_pszProfileName);
        m_pszProfileName = _tcsdup(sProfile);
        mSaveRegistry = 0;
    }
}

void CAnyFileViewerApp::OnSettingsRegisterFileType()
{
    CStringA sCmd;
    HKEY hKey;
    sCmd = "REG ADD \"HKCR\\*\\shell\\openanyfile\" /ve /t REG_SZ /d \"Open with AnyFileViewer\" /f";
    system(sCmd);
    RegOpenKeyEx(HKEY_CLASSES_ROOT, _T("*\\shell\\openanyfile"), 0, KEY_WRITE, &hKey);
    if (hKey)
    {
        HKEY hSubKey = NULL;
        RegCreateKeyEx(hKey, _T("command"), 0, NULL, 0, KEY_WRITE, NULL, &hSubKey, NULL);
        if (hSubKey)
        {
            CString sPath;
            sPath.Format(_T("\"%s%s.exe\" \"%%1\""), LPCTSTR(m_sPath), m_pszExeName);
            RegSetValueEx(hSubKey, _T(""), 0, REG_SZ, (BYTE*)LPCTSTR(sPath), sPath.GetLength()*sizeof(_TCHAR));
            RegCloseKey(hSubKey);
        }
        RegCloseKey(hKey);
    }
}

void CAnyFileViewerApp::OnUpdateSettingsRegistry(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
    pCmdUI->SetCheck(mSaveRegistry);
}

void CAnyFileViewerApp::OnUpdateSettingsRegisterFileType(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(TRUE);
}

void CAnyFileViewerApp::OnSeparatorComma()
{
    mSeparatorComma[Clipboard] = !mSeparatorComma[Clipboard];
}

void CAnyFileViewerApp::OnIdSeparatorSemicolon()
{
    mSeparatorSemicolon[Clipboard] = !mSeparatorSemicolon[Clipboard];
}

void CAnyFileViewerApp::OnSeparatorLf()
{
    mSeparatorLF[Clipboard] = !mSeparatorLF[Clipboard];
}

void CAnyFileViewerApp::OnSeparatorCr()
{
    mSeparatorCR[Clipboard] = !mSeparatorCR[Clipboard];
}

void CAnyFileViewerApp::OnSeparatorTab()
{
    mSeparatorTAB[Clipboard] = !mSeparatorTAB[Clipboard];
}

void CAnyFileViewerApp::OnEditCopyType()
{
    mCopyType[Clipboard] = !mCopyType[Clipboard];
}

void CAnyFileViewerApp::OnEditCopyName()
{
    mCopyName[Clipboard] = !mCopyName[Clipboard];
}

void CAnyFileViewerApp::OnUpdateSeparatorComma(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
    pCmdUI->SetCheck(mSeparatorComma[Clipboard]);
}

void CAnyFileViewerApp::OnUpdateIdSeparatorSemicolon(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
    pCmdUI->SetCheck(mSeparatorSemicolon[Clipboard]);
}

void CAnyFileViewerApp::OnUpdateSeparatorLf(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
    pCmdUI->SetCheck(mSeparatorLF[Clipboard]);
}

void CAnyFileViewerApp::OnUpdateSeparatorCr(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
    pCmdUI->SetCheck(mSeparatorCR[Clipboard]);
}

void CAnyFileViewerApp::OnUpdateSeparatorTab(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
    pCmdUI->SetCheck(mSeparatorTAB[Clipboard]);
}

void CAnyFileViewerApp::OnUpdateEditCopyType(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
    pCmdUI->SetCheck(mCopyType[Clipboard]);
}

void CAnyFileViewerApp::OnUpdateEditCopyName(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
    pCmdUI->SetCheck(mCopyName[Clipboard]);
}

CString CAnyFileViewerApp::getSeparator() const
{
    CString fSeparator;
    if (mLocale[mCurrentFormat].GetLength()) _tsetlocale(LC_NUMERIC, mLocale[mCurrentFormat]);
    if (mSeparatorComma[mCurrentFormat])     fSeparator += _T(",");
    if (mSeparatorSemicolon[mCurrentFormat]) fSeparator += _T(";");
    if (mSeparatorExtra[mCurrentFormat].GetLength()) fSeparator += mSeparatorExtra[mCurrentFormat];
    if (mSeparatorCR[mCurrentFormat])        fSeparator += _T("\r");
    if (mSeparatorLF[mCurrentFormat])        fSeparator += _T("\n");
    if (mSeparatorTAB[mCurrentFormat])       fSeparator += _T("\t");
    if (fSeparator.GetLength() == 0)         fSeparator += _T(" ");
    return fSeparator;
}

BOOL CAnyFileViewerApp::CopyType() const 
{
    return mCopyType[mCurrentFormat];
}

BOOL CAnyFileViewerApp::CopyName() const
{
    return mCopyName[mCurrentFormat];
}

const CString& CAnyFileViewerApp::getFloatFormat() const
{
    return mFloatFormat[mCurrentFormat];
}
const CString& CAnyFileViewerApp::getDoubleFormat() const
{
    return mDoubleFormat[mCurrentFormat];
}

void CAboutDlg::OnBnClickedOk()
{
    CDialog::OnOK();
}


BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    return TRUE;  // return TRUE unless you set the focus to a control
                  // AUSNAHME: OCX-Eigenschaftenseite muss FALSE zurückgeben.
}


void CAnyFileViewerApp::OnOptionsRepresentation()
{
    CRepresentationDlg dlg;
    dlg.mDoubleFormat       = mDoubleFormat[Clipboard];
    dlg.mFloatFormat        = mFloatFormat[Clipboard];
    dlg.mLocale             = mLocale[Clipboard];
    dlg.mSeparatorComma     = mSeparatorComma[Clipboard];
    dlg.mSeparatorLF        = mSeparatorLF[Clipboard];
    dlg.mSeparatorCR        = mSeparatorCR[Clipboard];
    dlg.mSeparatorSemicolon = mSeparatorSemicolon[Clipboard];
    dlg.mSeparatorTAB = mSeparatorTAB[Clipboard];
    dlg.mDescriptionName    = mCopyName[Clipboard];
    dlg.mDescriptionType    = mCopyType[Clipboard];
    dlg.mSeparatorExtra     = mSeparatorExtra[Clipboard];

    if (dlg.DoModal() == IDOK)
    {
        mDoubleFormat[Clipboard]       = dlg.mDoubleFormat;
        mFloatFormat[Clipboard]        = dlg.mFloatFormat;
        mLocale[Clipboard]             = dlg.mLocale;
        mSeparatorComma[Clipboard]     = dlg.mSeparatorComma;
        mSeparatorLF[Clipboard]        = dlg.mSeparatorLF;
        mSeparatorCR[Clipboard]        = dlg.mSeparatorCR;
        mSeparatorSemicolon[Clipboard] = dlg.mSeparatorSemicolon;
        mSeparatorTAB[Clipboard]       = dlg.mSeparatorTAB;
        mCopyName[Clipboard]           = dlg.mDescriptionName;
        mCopyType[Clipboard]           = dlg.mDescriptionType;
        mSeparatorExtra[Clipboard]     = dlg.mSeparatorExtra;
    }
}


void CAnyFileViewerApp::OnUpdateOptionsRepresentation(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
}
