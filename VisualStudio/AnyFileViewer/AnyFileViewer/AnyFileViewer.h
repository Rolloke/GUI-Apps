// AnyFileViewer.h : Hauptheaderdatei für die AnyFileViewer-Anwendung
//
#pragma once

#ifndef __AFXWIN_H__
    #error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include <vector>
#include "resource.h"       // Hauptsymbole

#define SECTION_SETTINGS    _T("Settings")
#define BAR_STATE               _T("BarState")
#define MAIN_VIEW_DIVISION      _T("MainViewDivision")
#define FORMAT_VIEW_DIVISION1   _T("FormatViewDivision1")
#define FORMAT_VIEW_DIVISION2   _T("FormatViewDivision2")

// CAnyFileViewerApp:
// Siehe AnyFileViewer.cpp für die Implementierung dieser Klasse
//
class CMainFrame;
class CDisplayType;
class CDisplayAscii;

class CAnyFileViewerApp : public CWinApp
{
public:
    enum eFormat
    {
        Clipboard, InlineItems, Excel,  MaxItem
    };
    CAnyFileViewerApp();
    ~CAnyFileViewerApp();


   CMainFrame *GetMainFrame();
// Überschreibungen
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();


// Implementierung
    //const CDisplayAscii &GetDisplayAscii() { return m_Asscii; }
    CDisplayType &GetDisplayType( int aType) const;
    int     GetNoOfDataTypes() { return static_cast<int>(m_DisplayTypes.size()); };
    BOOL    isDifferentEndian() const { return m_bDifferentEndianFormat; };
    void    invertEndianess() { m_bDifferentEndianFormat = !m_bDifferentEndianFormat; };
    void    setDifferentEndianess(BOOL aVaL) { m_bDifferentEndianFormat = aVaL; };

    void    setCurrentFormat(eFormat aFormat) { mCurrentFormat = aFormat; };
    eFormat getCurrentFormat() { return mCurrentFormat; };

    CString getSeparator() const;
    BOOL    CopyType() const;
    BOOL    CopyName() const;
    const CString& getFloatFormat() const;
    const CString& getDoubleFormat() const;

    // Nachrichten
    afx_msg void OnUpdateSettingsRegistry(CCmdUI *pCmdUI);
    afx_msg void OnSettingsRegistry();
    afx_msg void OnSettingsRegisterFileType();
    afx_msg void OnUpdateSettingsRegisterFileType(CCmdUI *pCmdUI);
    afx_msg void OnAppAbout();
    afx_msg void OnSeparatorComma();
    afx_msg void OnIdSeparatorSemicolon();
    afx_msg void OnSeparatorLf();
    afx_msg void OnSeparatorCr();
    afx_msg void OnUpdateSeparatorComma(CCmdUI *pCmdUI);
    afx_msg void OnUpdateIdSeparatorSemicolon(CCmdUI *pCmdUI);
    afx_msg void OnUpdateSeparatorLf(CCmdUI *pCmdUI);
    afx_msg void OnUpdateSeparatorCr(CCmdUI *pCmdUI);
    afx_msg void OnSeparatorTab();
    afx_msg void OnUpdateSeparatorTab(CCmdUI *pCmdUI);
    afx_msg void OnEditCopyType();
    afx_msg void OnUpdateEditCopyType(CCmdUI *pCmdUI);
    afx_msg void OnEditCopyName();
    afx_msg void OnUpdateEditCopyName(CCmdUI *pCmdUI);
    afx_msg void OnOptionsRepresentation();
    afx_msg void OnUpdateOptionsRepresentation(CCmdUI *pCmdUI);
    DECLARE_MESSAGE_MAP()

private:
    BOOL    m_bDifferentEndianFormat;
    eFormat mCurrentFormat;
    BOOL    mSeparatorComma[MaxItem];
    BOOL    mSeparatorSemicolon[MaxItem];
    BOOL    mSeparatorCR[MaxItem];
    BOOL    mSeparatorLF[MaxItem];
    BOOL    mSeparatorTAB[MaxItem];
    CString mSeparatorExtra[MaxItem];
    BOOL    mCopyType[MaxItem];
    BOOL    mCopyName[MaxItem];
    CString mFloatFormat[MaxItem];
    CString mDoubleFormat[MaxItem];
    CString mLocale[MaxItem];
public:
    CString mFormatFilesDir; 
    CString m_sPath; 
    DWORD   mSaveRegistry;
    std::vector<CDisplayType*> m_DisplayTypes;
    int     mcxViewPane0;
    int     mcxViewPane1;
    int     mcxViewPane1_0;
    int     mcxViewPane1_1;
};

extern CAnyFileViewerApp theApp;
