// WkWizardDoc.h : interface of the CWkWizardDoc class
//
/////////////////////////////////////////////////////////////////////////////
#include "TranslateString.h"
#include "Scanner.h"

class CWkWizardDoc : public CDocument
{
protected: // create from serialization only
	CWkWizardDoc();
	DECLARE_DYNCREATE(CWkWizardDoc)

// Attributes
public:
	inline const CString GetDirectory();
	// Operations
public:
	void ExtractResource(CString& name);
	void InsertResource(CString& orig, CString& lng, CString& lang);
	void ScanFile(CString& name);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWkWizardDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWkWizardDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWkWizardDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CStringArray	m_saCPPFiles;
	CStringArray	m_saRCFiles;
	CStringArray	m_saDependenciesFiles;

	CString			m_sPath;

	// data members
private:
	CTSArray m_TSArray;

};
inline const CString CWkWizardDoc::GetDirectory()
{
	return m_sPath;
}

/////////////////////////////////////////////////////////////////////////////
