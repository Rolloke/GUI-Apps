// DirDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDirDialog dialog

class CDirDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CDirDialog)

public:
	CDirDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

protected:
	//{{AFX_MSG(CDirDialog)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	virtual void OnInitDone();
	virtual void OnFolderChange();
	DECLARE_MESSAGE_MAP()
public:
	CString m_sDir;
};
