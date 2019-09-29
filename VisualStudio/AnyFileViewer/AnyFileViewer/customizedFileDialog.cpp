// ustomizedFileDialog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "AnyFileViewer.h"
#include "customizedFileDialog.h"


// CustomizedFileDialog

IMPLEMENT_DYNAMIC(CustomizedFileDialog, CFileDialog)

CustomizedFileDialog::CustomizedFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
        DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
        CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{

    EnableAutomation();
}

CustomizedFileDialog::~CustomizedFileDialog()
{
}

void CustomizedFileDialog::OnFinalRelease()
{
    // Wenn der letzte Verweis auf ein Automatisierungsobjekt freigegeben wird,
    // wird OnFinalRelease aufgerufen. Die Basisklasse löscht das Objekt
    // automatisch. Fügen Sie zusätzlichen Bereinigungscode für Ihr Objekt
    // hinzu, bevor Sie die Basisklasse aufrufen.

    CFileDialog::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CustomizedFileDialog, CFileDialog)
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CustomizedFileDialog, CFileDialog)
END_DISPATCH_MAP()

// Hinweis: Wir stellen Unterstützung für IID_IustomizedFileDialog zur Verfügung, um typsicheres Binden 
//  von VBA zu unterstützen. Diese IID muss mit der GUID übereinstimmen, die bei der 
//  Disp-Schnittstelle in der .IDL-Datei angegeben ist.

// {4CDAD528-66EA-414C-A075-CA44FBB733AB}
static const IID IID_IustomizedFileDialog =
{ 0x4CDAD528, 0x66EA, 0x414C, { 0xA0, 0x75, 0xCA, 0x44, 0xFB, 0xB7, 0x33, 0xAB } };

BEGIN_INTERFACE_MAP(CustomizedFileDialog, CFileDialog)
    INTERFACE_PART(CustomizedFileDialog, IID_IustomizedFileDialog, Dispatch)
END_INTERFACE_MAP()


// CustomizedFileDialog-Meldungshandler



bool CustomizedFileDialog::setInitialDir(const CString& aFolder)
{
    if (m_pIFileDialog)
    {

//        IShellItem*psi = 0;
//        m_pIFileDialog->GetFolder(&psi);
        //psi->
//        m_pIFileDialog->SetFolder(psi);
//        psi->Release();
    }
    return false;
}
