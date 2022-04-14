// WkTranslatorDoc.h : interface of the CWkTranslatorDoc class
//
/////////////////////////////////////////////////////////////////////////////
#include "TranslateString.h"
#include "GlossaryInfoDlg.h"

#include "TSArrayArray.h"


// Defines for UpdateViews
#define UPDATE_NEW_EMPTY_DOC	1
#define UPDATE_OPENED_DOC		3
#define UPDATE_TRANSLATION		10

/////////////////////////////////////////////////////////////////////////////
class CWkTranslatorDoc : public CDocument
{
protected: // create from serialization only
	CWkTranslatorDoc();
	DECLARE_DYNCREATE(CWkTranslatorDoc)

// Attributes
public:
	BOOL HasData();
	inline const CTSArrayArray&  GetTSArrayArray() const;
	inline const CMapStringToString&  GetMapAutomatic() const;

// Operations
public:
	BOOL EditTranslateString(int i, int j, BOOL bCheckData);
	void EditTranslateString(int i, int j, const CString& sNew);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWkTranslatorDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWkTranslatorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL RequestCARMO(BOOL &bRequested);
	void CheckAndReplaceMultipleOccurences(CTranslateString* pTS, CString sTranslation, int i, int j);
	void FileSaveAsGlossary(BOOL bGerman);
	void ImportGlossary(BOOL bGerman);
	void AddGlossary(const CString& sPathname, BOOL bGerman);
	void AddGlossaryFromTxtFile(const CString& sPathname);
	void AddAutomaticString(const CString& sLine, BOOL bGerman, const CString& sSeperator);
	void ExportGlossary(BOOL bGerman);
	CString GetAppDir();

// Generated message map functions
protected:
	//{{AFX_MSG(CWkTranslatorDoc)
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnGlossaryImport();
	afx_msg void OnGlossaryImportDeu();
	afx_msg void OnGlossaryExport();
	afx_msg void OnGlossaryExportDeu();
	afx_msg void OnFileSaveAsGlossary();
	afx_msg void OnFileSaveTextOnly();
	afx_msg void OnEnableEdit();
	afx_msg void OnUpdateEnableEdit(CCmdUI* pCmdUI);
	afx_msg void OnEnableEditAcc();
	afx_msg void OnFileSaveAsGlossaryDeu();
	afx_msg void OnFileSaveOriginalLengthComment();
	afx_msg void OnGlossaryImportTxtFile();
	//}}AFX_MSG
public:
	afx_msg LRESULT OnUserMsg(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

private:
	CTSArrayArray		m_TSArrayArray;
	CMapStringToString	m_mapAutomatic;
	BOOL				m_bEnableEdit;
	
	HANDLE         m_hThread;
	UINT				m_nThreadID;
	int				m_nWhat;
	CGlossaryInfoDlg m_dlgInfo;
	CString        m_sTempText;
	static UINT __stdcall ThreadProc(void*);
};
inline	const CTSArrayArray&  CWkTranslatorDoc::GetTSArrayArray() const
{
	return m_TSArrayArray;
}
inline const CMapStringToString&  CWkTranslatorDoc::GetMapAutomatic() const
{
	return m_mapAutomatic;
}

/////////////////////////////////////////////////////////////////////////////
