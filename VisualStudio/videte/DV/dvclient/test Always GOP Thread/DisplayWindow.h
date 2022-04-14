#if !defined(AFX_DISPLAYWINDOW_H__6B5BBEDB_9757_11D3_A870_004005A19028__INCLUDED_)
#define AFX_DISPLAYWINDOW_H__6B5BBEDB_9757_11D3_A870_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DisplayWindow.h : header file
//
class CServer;
#include "QueryRectTracker.h"

#define ID_CHANGE_VIDEO	0x7000
#define ID_REDRAW_VIDEO	0x7001
#define ID_CHANGE_AUDIO	0x7002
#define ID_REQUEST		0x7003

#define REQUEST_TIMEOUT_SLOW (3000)
#define REQUEST_TIMEOUT_PLAY (20)
#define REQUEST_TIMEOUT_FF   (500)
#define REQUEST_TIMEOUT_FAST (20)

class CMainFrame;
/////////////////////////////////////////////////////////////////////////////
// CDisplayWindow window

#define SYMBOL_TIMER		0
#define SYMBOL_ZOOM			1
#define SYMBOL_UVV			2
#define SYMBOL_SUSPECT		3
#define SYMBOL_BACKUP1		4
#define SYMBOL_BACKUP2		5
#define SYMBOL_BACKUP3		6
#define SYMBOL_BACKUP_LAST	7
#define SYMBOL_ALARM		8
#define SYMBOL_PREALARM		9

#define DISPLAYWINDOW_BASE		0
#define DISPLAYWINDOW_LIVE		1
#define DISPLAYWINDOW_PLAY		2
#define DISPLAYWINDOW_BACKUP	3
#define DISPLAYWINDOW_DEBUG		4

class CMPEG4Decoder;
class CDisplayWindow : public CWnd
{
	DECLARE_DYNAMIC(CDisplayWindow)
// Construction
public:
	CDisplayWindow(CMainFrame* pParent, CServer* pServer);
	virtual ~CDisplayWindow();

// Attributes
public:
	static	BOOL	GetShowTargetCross();

	inline  BOOL		IsActive() const;
	inline BOOL			IsMotionDetected() const;
	virtual CString		GetTimeString();
	virtual CString		GetDateString();
	virtual CString		GetShortDateString();
	virtual CString		GetPrintTitle();
	virtual CSecID		GetID() const;
	virtual CString		GetName() const;
	virtual CString		GetTitle(CDC* pDC);
	virtual void	    GetFooter(CByteArray& Array);
	virtual CString		GetDefaultText();
	virtual COLORREF	GetOSDColor();
	virtual BOOL		IsShowable();
	virtual CSystemTime GetTimeStamp();
	inline  CServer*    GetServer();
	virtual int			GetType() const { return  DISPLAYWINDOW_BASE;};

	CMPEG4Decoder* GetMpeg4Decoder(){return m_pMpeg;};
	CJpeg* GetJpegDecoder(){return m_pJpeg;};

	// operators
public:
	BOOL operator < (const CDisplayWindow& s);

// Operations
public:

	virtual void InitialRequests();
	virtual void SetActive(BOOL bActive);
	virtual BOOL Request(int iMax=-1);
	virtual BOOL PictureData(const CIPCPictureRecord *pPict,DWORD dwX, DWORD dwY);
	virtual BOOL SetMDValues(WORD wX, WORD wY);
	virtual BOOL Create(const RECT& rect, CWnd* pParentWnd);

	virtual BOOL ExportImage(const CString& sExportPath);
	virtual BOOL PrintImage();
	virtual BOOL OnDeviceChange(UINT nEventType, DWORD dwData);
			void DoRedraw();
	virtual BOOL SetActiveWindowToAnalogOut();
	static	void SetShowTargetCross(BOOL bShow);
	virtual CJpeg* GetJpegObject(){return m_pJpeg;};
	
	CRect GetZoom();
	inline CQueryRectTracker* GetTracker();
	void   SetTrackerPromille(CRect rcNew);
	void   ResetTracker();	//destroys the old tracker object and creates a new 
	BOOL   SetTracker();	//Sets PlayWindow variables for rectangle search
							//is called only when rectangle was build in LiveWindow
	void   InitializeTracker();

	CSize GetPictureSize();
	CIPCPictureRecord* GetPictureCopy();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisplayWindow)
	protected:
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CDisplayWindow)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation
protected:
	virtual void OnDraw(CDC* pDC);
			void DrawJpeg(CDC* pDC);
			void DrawMpeg4(CDC* pDC);
			void DrawDefaultText(CDC* pDC);
			void OnRedrawVideo();
	virtual void OnChangeVideo();

			void ZoomDisplayWindow(const CPoint &point, BOOL bIn);
			BOOL ScrollDisplayWindow(CPoint point);
			void DrawCross(CDC* pDC, WORD wX, WORD wY);

			BOOL SaveHtmlTag(const CString& sFileName);
			void DeletePicture(BOOL bRedraw);
			void CreateJpegDecoder();
			void DestroyJpegDecoder();
			void CreateMpegDecoder();
			void DestroyMpegDecoder();

	//functions for rectangle search
			void ResetRectMembersInPlayWnd(); //resetet Variablen für die Rechtecksuche im PlayWindow

			//nur bei m_bCanRectSearch == TRUE, zeichnet alle Fadenkreuze
			void DoDrawAllCrosses(CDC* pDC);
			void DrawAllCrosses(CDC* pDC, WORD wX, WORD wY, COLORREF col, BOOL bLines = FALSE); 
			BOOL AllMDsOutsideRect(int iAllMDs, CRect rcRectangle);


	// data member
protected:
	CServer*	m_pServer;
	CMainFrame* m_pMainFrame;
	int			m_iRequests;
	DWORD		m_dwLastRequest;
	DWORD		m_dwWaitTimeout;
	BOOL		m_bIsActive;

	CRect				m_rcZoom;
	CIPCPictureRecord*	m_pPictureRecord;
	CCriticalSection	m_csPictureRecord;

	CImageList			m_ilSymbols;
	CQueryRectTracker*	m_pTracker;
	BOOL				m_bCanRectSearch;	//In Abhängigkeit der jeweilige installierten Server-Version 
											//wird die Rechtecksuche freigeschaltet, ab Version 1.4.
	CMPEG4Decoder*		m_pMpeg;
	DWORD				m_dwLastPictureReceivedTC;
	BOOL				m_bWasSomeTimesActive;
private:
	CJpeg*				m_pJpeg;

	WORD				m_wMD_X;
	WORD				m_wMD_Y;
	DWORD				m_dwLastMD_TC;
	BOOL				m_bMD;
	static BOOL			m_bShowTargetCross;
	
protected:
	//members for rectangle search
	CWordArray m_waAllMD_X; //nur bei m_bCanRectSearch == TRUE, für Anzeigen aller Fadenkreuze
	CWordArray m_waAllMD_Y; //nur bei m_bCanRectSearch == TRUE, für Anzeigen aller Fadenkreuze
public:
};
/////////////////////////////////////////////////////////////////////////////
inline BOOL CDisplayWindow::IsActive() const
{
	return m_bIsActive;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CDisplayWindow::IsMotionDetected() const
{
	return m_bMD;
}
/////////////////////////////////////////////////////////////////////////////
inline CQueryRectTracker* CDisplayWindow::GetTracker()
{
	return m_pTracker;
}
/////////////////////////////////////////////////////////////////////////////
inline CServer* CDisplayWindow::GetServer()
{
	return m_pServer;
}
/////////////////////////////////////////////////////////////////////////////
typedef CDisplayWindow* CDisplayWindowPtr;
WK_PTR_ARRAY_CS(CDisplayWindowArray,CDisplayWindowPtr,CDisplayWindows);

//WK_PTR_ARRAY_CS(CDisplayWindowArray,CDisplayWindow*,CDisplayWindows);
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISPLAYWINDOW_H__6B5BBEDB_9757_11D3_A870_004005A19028__INCLUDED_)
