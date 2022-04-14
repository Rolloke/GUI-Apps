// SecAnalyzerDoc.h : interface of the CSecAnalyzerDoc class
//
/////////////////////////////////////////////////////////////////////////////

#include "wk.h"
#include "WKClasses/WK_String.h"

class CStatRecords;
class CIPCInputAlarmPlayer;

class CWK_HashStrings : public CWK_Strings
{
public:
	void UpdateMap();
	int MapCheck(const CWK_String &s);
private:
	CMapStringToPtr m_map;
};


class CSecAnalyzerDoc : public CDocument
{
protected: // create from serialization only
	CSecAnalyzerDoc();
	DECLARE_DYNCREATE(CSecAnalyzerDoc)

// Attributes
public:

// Operations
public:
	static CWK_HashStrings m_knownGroups;
	static CWK_HashStrings m_usedGroups;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSecAnalyzerDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSecAnalyzerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSecAnalyzerDoc)
	afx_msg void OnAlarmPlayback();
	afx_msg void OnStopPlayer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void GetValuesFromDialog();
	void ScanFile();
	void DeleteTmpFile();

	CString m_sFilename;
	CStatRecords *m_pRecords;
	CTime m_lastUpdateTime;
	//
	CStringArray m_includePatterns;
	CStringArray m_excludePatterns;
	//
	int m_iNumOldFiles;
	CTime	m_startTime;
	DWORD	m_startTick;
	BOOL	m_bUseStartTime;
	CTime	m_endTime;
	DWORD	m_endTick;
	CTimeSpan m_endSpan;	// for relative end
	BOOL	m_bUseEndTime;
	//
	BOOL	m_bAutoUpdate;
	BOOL	m_bLastReset;
	BOOL	m_bRelativeEnd;
	//
	BOOL m_bShowProcessInput;
	BOOL m_bShowProcessOutput;

	BOOL m_bIncludeLevel1;
	BOOL m_bIncludeLevel2;
	//
	CIPCInputAlarmPlayer *m_pPlayer;
};

/////////////////////////////////////////////////////////////////////////////
