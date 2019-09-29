// AnalyseMapFileDoc.h : Schnittstelle der Klasse CAnalyseMapFileDoc
//


#pragma once

#include <afxtempl.h>

struct FunctionParam
{
	CString sAddress;
	CString sFunction;
	CString sRVApBase;
	CString sObjectFile;
};

#define UPDATE_DOCUMENT_CONTENT 1
class CUpdateDocumentContentHint : public CObject
{
public:
	CUpdateDocumentContentHint(int nItems, LPCTSTR sPath);
	DECLARE_DYNAMIC(CUpdateDocumentContentHint)

	int m_nItems;
	CString m_sPath;
};

typedef CTypedPtrList<CPtrList, FunctionParam*> CBaseFPList;


class CFunctionParamList : public CBaseFPList
{
public:
	CFunctionParamList();
	~CFunctionParamList();

	void DeleteContent();
	BOOL IsMapFile();
	BOOL IsDllFile();

	CString m_sMapFile;
	FindSymbols *m_pFS;
};

typedef CTypedPtrList<CObList, CFunctionParamList*> CFunctionParamMap;

class CAnalyseMapFileDoc : public CDocument
{
protected: // Nur aus Serialisierung erstellen
	CAnalyseMapFileDoc();
	DECLARE_DYNCREATE(CAnalyseMapFileDoc)

// Attribute
public:
	int					GetFiles();
	int					GetSelectedFile();
	CFunctionParamList& GetFunctionParam();
// Operationen
public:
	void	AddMapFile();
	BOOL	SelectFile(LPCTSTR sFile, BOOL bUpdate=FALSE);
	void	SetSelectedFile(int nSelected, BOOL bUpdate=FALSE);
// Überschreibungen
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();

// Implementierung
public:
	virtual ~CAnalyseMapFileDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);

private:
	CFunctionParamList m_Dummy;
	CFunctionParamMap  m_FunctionParamMap;
	int				   m_nSelected;
	int				   m_nOrdinal;

//	CFunctionParamList m_FunctionParam;

// Generierte Funktionen für die Meldungstabellen
protected:
	//{{AFX_MSG(CAnalyseMapFileDoc)
	afx_msg void OnUpdateEditFindAddress(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditFindFunction(CCmdUI *pCmdUI);
	afx_msg void OnFileClose();
	afx_msg void OnUpdateFileClose(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	static BOOL CALLBACK EnumSymbolsProc(PCSTR SymbolName, DWORD64 SymbolAddress, ULONG SymbolSize, PVOID UserContext);
};


