#if !defined(AFX_DIRDIALOG_H__9AF299E9_08CA_4231_A0E7_3C7DFDE721B9__INCLUDED_)
#define AFX_DIRDIALOG_H__9AF299E9_08CA_4231_A0E7_3C7DFDE721B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
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

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRDIALOG_H__9AF299E9_08CA_4231_A0E7_3C7DFDE721B9__INCLUDED_)
