#pragma once


// CustomizedFileDialog

class CustomizedFileDialog : public CFileDialog
{
    DECLARE_DYNAMIC(CustomizedFileDialog)

public:
    CustomizedFileDialog(BOOL bOpenFileDialog, // TRUE für FileOpen, FALSE für FileSaveAs
        LPCTSTR lpszDefExt = NULL,
        LPCTSTR lpszFileName = NULL,
        DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        LPCTSTR lpszFilter = NULL,
        CWnd* pParentWnd = NULL);
    virtual ~CustomizedFileDialog();

    virtual void OnFinalRelease();

protected:
    DECLARE_MESSAGE_MAP()
    DECLARE_DISPATCH_MAP()
    DECLARE_INTERFACE_MAP()
public:
    bool setInitialDir(const CString& aFolder);
};


