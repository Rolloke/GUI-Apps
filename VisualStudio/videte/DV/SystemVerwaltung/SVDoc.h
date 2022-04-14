/* GlobalReplace: CDVRDrive --> CIPCDrive */
// SVDoc.h : interface of the CSVDoc class
//
#ifndef _SVDOC_H
#define _SVDOC_H

/////////////////////////////////////////////////////////////////////////////

class CSVView;

class CSVDoc : public CDocument
{
protected: // create from serialization only
	CSVDoc();
	DECLARE_DYNCREATE(CSVDoc)

// Attributes
public:
	inline CHostArray*			GetHosts();
	inline CArchivInfoArray*	GetArchivs();
	inline CIPCDrives*			GetDrives();

	// attributes
public:
	CWK_Profile&	GetProfile();
	CWK_Dongle*		GetDongle();
	inline DWORD	GetFlags() const;
	inline CString	GetHostName() const;
	
	CSVView*		GetView();

// Operations
public:
	void AddInitApp(const CString& sAppname);
	void InitServer();
	void InitApps();
	void Load();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSVDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CSVDoc)
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

private:
	CWK_Profile*			  m_pProfile;
	CWK_Dongle*				  m_pDongle;
	DWORD					  m_dwFlags;
	CSecID					  m_idHost;
	CString					  m_sHost;

	CHostArray				  m_Hosts;
	CArchivInfoArray		  m_Archivs;
	CIPCDrives				  m_Drives;

	CStringArray	m_saInitApps;
};
/////////////////////////////////////////////////////////////////////////////
inline CHostArray* CSVDoc::GetHosts()		 
{
	return &m_Hosts;
}
/////////////////////////////////////////////////////////////////////////////
inline CArchivInfoArray* CSVDoc::GetArchivs()	 
{
	return &m_Archivs;
}
/////////////////////////////////////////////////////////////////////////////
inline CIPCDrives* CSVDoc::GetDrives()
{
	return &m_Drives;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CSVDoc::GetFlags() const
{
	return m_dwFlags;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CSVDoc::GetHostName() const
{
	return m_sHost;
}
/////////////////////////////////////////////////////////////////////////////

#endif
/////////////////////////////////////////////////////////////////////////////
