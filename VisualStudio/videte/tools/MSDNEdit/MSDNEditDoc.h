// MSDNEditDoc.h : interface of the CMSDNEditDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSDNEDITDOC_H__DB191CE5_6DC4_4862_AB83_FB7A6335D0E4__INCLUDED_)
#define AFX_MSDNEDITDOC_H__DB191CE5_6DC4_4862_AB83_FB7A6335D0E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DOC_TYPE_HTMLHELPCOLLECTION  0x01
#define DOC_TYPE_HTMLHELPDOCINFO     0x02

#define HTMLHELPCOLLECTION_INSERT       1
#define HTMLHELPCOLLECTION_SELCHANGE    2
#define HTMLHELPCOLLECTION_SAVE         3
#define HTMLHELPCOLLECTION_DELETE       4
#define HTMLHELPCOLLECTION_EXPORT       5
#define HTMLHELPCOLLECTION_GETFILENAMES 6

#define HTMLHELPDOCINFO_INSERT         11
#define HTMLHELPDOCINFO_SELCHANGE      12
#define HTMLHELPDOCINFO_SAVE           13
#define HTMLHELPDOCINFO_DELETE         14
#define HTMLHELPDOCINFO_EXPORT         15

struct MSDNFolders
{
   MSDNFolders()
   {
      nOrder=0;
      nLangID=0;
      hReturned=0;
      hParent=0;
   };
   CString   strTitle;
   int       nOrder;
   int       nLangID;
   HTREEITEM hReturned;
   HTREEITEM hParent;
};

struct MSDNDocComp
{
   MSDNDocComp()
   {
      nDocCompLanguage=0;
      nColNum=0;
      nVersion=0;
      nLastPromptedVersion=0;
      nSupportsMerge=0;
   };
   CString strDocCompID;
   int     nDocCompLanguage;
   int     nColNum;
   CString strTitleLocation;
   CString strIndexLocation;
   CString strQueryLocation;
   CString strLocationRef;
   int     nVersion;
   int     nLastPromptedVersion;
   CString strTitleSampleLocation;
   CString strTitleQueryLocation;
   int     nSupportsMerge;
};

struct MSDNDocCol
{
   MSDNDocCol()
   {
      nNum = 0;
   };
   int     nNum;
   CString strName;
};

struct MSDNDocLoc
{
   MSDNDocLoc()
   {
      nLocColNum = 0;
   };
   int nLocColNum;
   CString strLocName;
   CString strTitleString;
   CString strLocPath;
   CString strVolume;
};

class CMSDNEditDoc : public CDocument
{
protected: // create from serialization only
	CMSDNEditDoc();
	DECLARE_DYNCREATE(CMSDNEditDoc)

// Operations
public:
	void SetModifiedFlagEx(BOOL);

// Attributes
public:
   CString m_strXML;
   CString m_strNXML;
   CString m_strNewLine;
   CString m_strFolder;
   CString m_strNFolder;
   CString m_strFolders;
   CString m_strNFolders;
   CString m_strTitle;
   CString m_strFolderOrder;
   CString m_strLangID;
   CString m_strDocCompId;
   CString m_strTitleLocation;
   CString m_strNextCollectionId;
   CString m_strCollections;
   CString m_strCollection;
   CString m_strColNum;
   CString m_strColName;
   CString m_strNCollection;
   CString m_strNCollections;
   CString m_strLocations;
   CString m_strLocation;
   CString m_strLocColNum;
   CString m_strLocName;
   CString m_strTitleString;
   CString m_strLocPath;
   CString m_strVolume;
   CString m_strNLocations;
   CString m_strNLocation;
   CString m_strLocationHistory;
   CString m_strDocCompilations;
   CString m_strNDocCompilations;
   CString m_strDocCompilation;
   CString m_strDocCompLanguage;
   CString m_strIndexLocation;
   CString m_strQueryLocation;
   CString m_strLocationRef;
   CString m_strDCVersion;
   CString m_strLastPromptedVersion;
   CString m_strTitleSampleLocation;
   CString m_strTitleQueryLocation;
   CString m_strSupportsMerge;
   CString m_strNDocCompilation;
   CString m_strNLocationHistory;
   CString m_strHTMLHelpDocInfo;
   CString m_strNHTMLHelpDocInfo;
   CString m_strHTMLHelpCollection;
   CString m_strNHTMLHelpCollection;
   CString m_strmasterchm;
   CString m_strmasterlangid;
   CString m_strsamplelocation;
   CString m_strcollectionnum;
   CString m_strrefcount;
   CString m_strversion;
   CString m_strfindmergedchms;

	CString m_strMasterChmValue;
	CString m_strSampLocValue;
	int     m_nMasterLangId;
  	int     m_nCollectionNum;
  	int     m_nRefCount;
  	int     m_nVersion;
  	int     m_nFindMergeDchms;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMSDNEditDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void DeleteContents();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMSDNEditDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMSDNEditDoc)
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnFileImport();
	afx_msg void OnUpdateFileImport(CCmdUI* pCmdUI);
	afx_msg void OnFileExport();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnEditNewItem();
	afx_msg void OnUpdateEditNewItem(CCmdUI* pCmdUI);
	afx_msg void OnFileReload();
	afx_msg void OnUpdateFileReload(CCmdUI* pCmdUI);
	afx_msg void OnViewColFile();
	afx_msg void OnViewRegFile();
	afx_msg void OnViewColInfo();
	afx_msg void OnViewColections();
	afx_msg void OnViewLocations();
	afx_msg void OnUpdateViewLocations(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewColections(CCmdUI* pCmdUI);
	afx_msg void OnFileChangeDocInfo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void IntegrateHelpFile(MSDNDocComp&);
	void DeleteLocations();
	void DeleteCollections();
   CString      m_strHTMLHELPCOLLECTION;
   CString      m_strHTMLHELPDOCINFO;
   CString      m_strHelpFilePath;
   FILETIME     m_ftHTMLHELPCOLLECTION;
   FILETIME     m_ftHTMLHELPDOCINFO;
   CPtrArray    m_DocInfoCollections;
   CPtrArray    m_DocInfoLocations;
   int          m_nNextCollectionID;
   int          m_nDoctype;
   int          m_nActiveType;
   int          m_nFilesImported;
   bool         m_bImportExport;
   bool         m_bCreateBackUpFile;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSDNEDITDOC_H__DB191CE5_6DC4_4862_AB83_FB7A6335D0E4__INCLUDED_)
