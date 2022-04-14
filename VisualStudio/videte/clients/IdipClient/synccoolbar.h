// SyncCoolBar.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYNCCOOLBAR_H)
#define AFX_SYNCCOOLBAR_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "IdipClient.h"
#include "QueryResult.h"
class CMainFrame;

typedef CMap<DWORD,DWORD,DWORD,DWORD> CMapDWORDToDWORD;

#define SCB_QUERY_RESULT	1
#define SCB_SET_PLAY_STATUS	2
#define SCB_CONFIRM_PICTURE	3


class CSyncCoolBar : public CCoolBar 
{
	// construction
protected:
	DECLARE_DYNAMIC(CSyncCoolBar)
	virtual	~CSyncCoolBar();
	virtual BOOL OnCreateBands();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	// attributes
public:
	int  GetNrOfArchives();
	PlayStatus GetPlayStatus();
	CSystemTime GetDateTime();
	BOOL IsInQuery(DWORD dwQueryID);	
	int HitTest();

	// operations:
public:
	void NotifyPlayStatusToPlayWindows();
	void OnIdle();
	void AddArchive(WORD wServer, WORD wArchive);
	void DeleteArchive(WORD wServer, WORD wArchive, BOOL bCloseWindow);
	void SetPlayStatus(PlayStatus ps);
	void ConfirmQuery(DWORD dwUserID);
	void ConfirmQuery(DWORD dwUserID,
					  WORD wServerNr,
					  WORD wArchivNr,
					  WORD wSequenceNr, 
					  DWORD dwRecordNr,
					  int iNumFields,
					  const CIPCField fields[]);
	void ConfirmPicture();

	//{{AFX_MSG(CSyncCoolBar)
	afx_msg void OnSyncBack();
	afx_msg void OnSyncSingleBack();
	afx_msg void OnSyncStop();
	afx_msg void OnSyncForward();
	afx_msg void OnSyncSingleForward();
	afx_msg void OnUpdateSyncBack(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSyncSingleBack(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSyncStop(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSyncForward(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSyncSingleForward(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// implementation
protected:
	void SetDateTime(const CSystemTime& t);
	void GotoTime(const CSystemTime& t);
	void GotoTime(const CSystemTime& t, WORD wServer, WORD wArchive);

	void OnPlayBack();
	void OnSkipBack();
	void OnPlayForward();
	void OnSkipForward();
	void OnStop();

	void Skip(int i=1);

	// data member
private:
	CCoolToolBar		m_wndToolBar;			 // toolbar
	CSkinDateTimeCtrl	m_wndDate;
	CSkinDateTimeCtrl	m_wndTime;
	CSkinSlider			m_wndSlider;
	CMainFrame*			m_pMainFrame;

	CImageList		m_ilHot;

	DWORD			 m_dwQueryID;
	CDWordArray		 m_dwIDs;
	CMapDWORDToDWORD m_mapQueryIDs;
	PlayStatus		 m_PlayStatus;
	CQueryResultArrayCS m_QueryResults;
	BOOL			 m_bTimeValid;
	int				 m_iRequests;

	DWORD			 m_dwTick;
	DWORD			 m_dwTickDiff;
	DWORD			 m_dwLastConfirmed;

	friend class CMainFrame;
protected:
};


#endif
