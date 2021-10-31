#ifndef AFX_CARADOC_H__30FA9ED2_14B6_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_CARADOC_H__30FA9ED2_14B6_11D2_9DB9_0000B458D891__INCLUDED_


#define UPDATE_DELETE_LABEL      0x0001
#define UPDATE_DELETE_LABEL_FROM 0x0002
#define UPDATE_DELETE_CONTENTS   0x0004
#define UPDATE_INSERT_LABEL      0x0008
#define UPDATE_INSERT_LABEL_INTO 0x0010
#define UPDATE_INSERT_NEWDOC     0x0020
#define UPDATE_INSERT_DOC        0x0040
#define UPDATE_RESET_PICKSTATES  0x0080
#define UPDATE_LABEL_REGION      0x0100
#define UPDATE_PREVIEW_LABEL     0x0200
#define UPDATE_PRINT_SETTINGS    0x0400
#define UPDATE_CURVE_UNDO        0x0800

#define CRC_EXCEPTION            1000
#define VERSION_EXCEPTION        1001

#define LABEL_FILE    _T("CVL")
#define PRINTER_DATA  _T("PRT")

// CARADoc.h : Header-Datei
//
#include "stdafx.h"

#include "LabelContainer.h"
#include "PictureLabel.h"
#include "CaraWinApp.h"
#include "PreviewFileHeader.h"
#include "CEtsLsmb.h"
/////////////////////////////////////////////////////////////////////////////
// CCaraSingleDocTemplate:

/////////////////////////////////////////////////////////////////////////////
// Dokument CCARADoc 

class CTreeDlg;
class CCaraView;
 
#ifdef ETS_OLE_SERVER
   class COleServerSrvrItem;
   class AFX_EXT_CLASS CCARADoc : public COleServerDoc
#else
   class AFX_EXT_CLASS CCARADoc : public CDocument
#endif
{
protected:
	CCARADoc();           // Dynamische Erstellung verwendet geschützten Konstruktor
	DECLARE_DYNCREATE(CCARADoc)

// Attribute
public:
#ifdef ETS_OLE_SERVER
	COleServerSrvrItem* GetEmbeddedItem()
		{ return (COleServerSrvrItem*)COleServerDoc::GetEmbeddedItem(); }
protected:
	virtual COleServerItem* OnGetEmbeddedItem();

public:
bool CCARADoc::IsEmbeddedItemValid()
{
   return (m_pEmbeddedItem != NULL) ? true : false;
}

#endif

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CCARADoc)
	public:
	virtual void SetTitle(LPCTSTR lpszTitle);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void ReportSaveLoadException(LPCTSTR lpszPathName, CException* e, BOOL bSaving, UINT nIDPDefault);
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	virtual BOOL OnNewDocument();
	virtual void DeleteContents();
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CCARADoc();
   virtual bool      Undo(CUndoParams *, CLabel*, LPARAM) {return false;};

   CLabelContainer*  GetLabelContainer() {return &m_Container;};
// Mehrere Seiten durch Auswahl von Untercontainern Controlle in CDocument
   bool              LabelsAreInPrintRect(CDC *, CRect);
	LPCTSTR           GetDocName();
	bool              ReadFileHeader(CArchive &);
	void              WriteFileHeader(CArchive&, bool bCurrent = true);
	void              CreateCaraLogo();
   static void       DosToWindowText(TCHAR*);
	static void       StrReplace(TCHAR*, TCHAR*, TCHAR*);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void  CheckVersion();
	bool  ReadPrinterData(CArchive&);
	void  WritePrinterData(CArchive&);

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CCARADoc)
	afx_msg void OnShowLabelstructure();
	afx_msg void OnFileSendMail();
	afx_msg void OnUpdateFileSendMail(CCmdUI* pCmdUI);
	afx_msg void OnFileSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetReadOnly(bool);
	void DestroyLSMB();
	void ShowLabelStructure();
	void SetDocPageParams();
	void DeleteDocPrtParam();
	CCaraView* GetCaraView();

   CPictureLabel  *m_pCaraLogo;
   CFileHeader     m_FileHeader;
   bool            m_bIsTempFile;
   bool            m_bDeleteTempFile;
   CEtsLsmb       *m_pLsmb;
protected:
   CLabelContainer m_Container;
   long            m_nCurrentFHPos;
   CEtsLsmb	       m_Lsmb;
   BYTE           *m_pDocPrtParam;
   DWORD           m_nDocPrtParam;
private:
	static int ShowLabelStructure(CLabel*, void*);
	static int FillLabelStructure(CTreeDlg*, int);
};


#define TYPE_UNKNOWN  -1
#define TYPE_DOS  0
#define TYPE_UNIX 1
#define TYPE_MAC  2

class AFX_EXT_CLASS CArchiveEx : public CArchive
{
public:
    CArchiveEx(CFile* pFile, UINT nMode, int nBufSize = 4096, void* lpBuf = NULL):
      CArchive(pFile, nMode, nBufSize, lpBuf)
    {};
    CArchiveEx(const CArchive&ar):
      CArchive(ar)
    {};
   int ReadString(DWORD &nType, CString &sRead);
   int ReadString(DWORD &nType, TCHAR*psRead, long nLen);
   DWORD GetType();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_CARADOC_H__30FA9ED2_14B6_11D2_9DB9_0000B458D891__INCLUDED_
