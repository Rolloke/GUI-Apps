#if !defined(AFX_PICTURECATALOG_H__835AB932_78C5_41B6_892A_85BA61565632__INCLUDED_)
#define AFX_PICTURECATALOG_H__835AB932_78C5_41B6_892A_85BA61565632__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PictureCatalog.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPictureCatalog 

struct PictureList
{
   PictureList() 
   {
      nStyleIndex  = 0;
      nArtistIndex = 0;
      nSizeIndex   = 0;
   };
   CString  strText1;
   int      nStyleIndex;
   int      nArtistIndex;
   int      nSizeIndex;
   CString  strText2;
};

class CPictureCatalog : public CDialog
{
// Konstruktion
public:
	void ResetSelections();
	CPictureCatalog(CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~CPictureCatalog();   // Standarddestruktor

// Dialogfelddaten
	//{{AFX_DATA(CPictureCatalog)
	enum { IDD = IDD_PICTURE_CATALOG };
	CListCtrl	m_cPictureList;
	CListCtrl	m_cSizeList;
	CListCtrl	m_cArtistList;
	CListCtrl	m_cStyleList;
	CString	m_strPictNo;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPictureCatalog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CPictureCatalog)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPcBtnLoad();
	afx_msg void OnPcBtnSave();
	afx_msg void OnPcBtnDeleteArtistItem();
	afx_msg void OnPcBtnDeleteListItem();
	afx_msg void OnPcBtnDeleteSizeItem();
	afx_msg void OnPcBtnDeleteStyleItem();
	afx_msg void OnPcBtnNewArtistItem();
	afx_msg void OnPcBtnNewListItem();
	afx_msg void OnPcBtnNewSizeItem();
	afx_msg void OnPcBtnNewStyleItem();
	afx_msg void OnGetdispinfoPcListStyle(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfoPcListArtist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfoPcListSize(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfoPcListPicture(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditPcListArtist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditPcListPicture(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditPcListSize(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditPcListStyle(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeleditPcListStyle(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickPcListArtist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickPcListStyle(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickPcListPicture(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeleditPcListPicture(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownPcListSize(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedPcListArtist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedPcListPicture(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusPcListSize(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedPcListSize(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedPcListStyle(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPcBtnShowAllPictures();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	PictureList** SetSize(int, PictureList**, int&);
	void DeleteLists();
	void DisableDeleteButtons();
	void UpdateDependencies();
	void SetHeadingOfPictList(CString&);

   PictureList **m_ppPictureList;
   PictureList **m_ppSizeList;
   PictureList **m_ppArtistList;
   PictureList **m_ppStyleList;

   int           m_nPictureList;
   int           m_nSizeList;
   int           m_nArtistList;
   int           m_nStyleList;

   int           m_nPictureSort;
   int           m_nSizeListSort;
   int           m_nArtistListSort;
   int           m_nStyleListSort;

   int  m_nItem;
   int  m_nSubItem;
	bool m_bItemChangedPictureList;
   bool m_bKeydownSizeList;
   bool m_bChanged;


	static int _cdecl SortText1(const void*, const void*);
	static int _cdecl SortText2(const void*, const void*);
	static int _cdecl SortArtist(const void*, const void*);
	static int _cdecl SortSize(const void*, const void*);
	static int _cdecl SortStyle(const void*, const void*);
   static bool       gm_bSortChanged;
   static int        gm_nSortDir;
   static long             gm_lOldEditCtrlWndProc;
   static LRESULT CALLBACK EditCtrlWndProc(HWND, UINT, WPARAM, LPARAM);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PICTURECATALOG_H__835AB932_78C5_41B6_892A_85BA61565632__INCLUDED_
