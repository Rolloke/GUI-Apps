/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: WndCoco.h $
// ARCHIVE:		$Archive: /Project/Clients/IdipClient/WndCoco.h $
// CHECKIN:		$Date: 5.05.06 13:28 $
// VERSION:		$Revision: 13 $
// LAST CHANGE:	$Modtime: 5.05.06 11:34 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __COCOWND_H_
#define __COCOWND_H_

#include "WndLive.h"

#define CW_KEYCOLOR_OFFSET 3
/////////////////////////////////////////////////////////////////////////////
// CWndCoco window
class CWndCoco : public CWndLive
{
	DECLARE_DYNAMIC(CWndCoco)
// Construction
public:
	CWndCoco(CServer* pServer, const CIPCOutputRecord &or, BOOL bJPEG2h263);
	virtual ~CWndCoco();

// Attributes
public:
	inline  BOOL IsJPEG2H263() const;
	
// Operations
public:
			void GetVideoRect(LPRECT lpRect);
	virtual BOOL PictureData(const CIPCPictureRecord &pict, 
							CSecID idArchive,
							DWORD dwX,
							DWORD dwY);
	virtual	void ClearWasSomeTimesActive();
	virtual	CSize GetPictureSize();
	virtual void Set1to1(BOOL b1to1);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndCoco)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnDraw(CDC* pDC);
	virtual void DrawBitmap(CDC* pDC, const CRect &rcDest);
			void DrawKeyColor(CDC* pDC);
			BOOL CanAdjustFPS(int fps);

protected:
	virtual CString GetTitleText(CDC* pDC);
	virtual CH263* GetH263();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndCoco)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnDecodeVideo(WPARAM, LPARAM);
	afx_msg void OnFps_X_X(UINT);
	afx_msg void OnUpdateFps_X_X(CCmdUI* pCmdUI);
	afx_msg void OnVideoSaveReference();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// private helper methods
private:

	// data members
public:
private:
	CCriticalSection m_csH263;
	DWORD   m_dwTick;
	int		m_iFrames;
	BOOL	m_bJPEG2h263;
};
/////////////////////////////////////////////////////////////////////////////
inline BOOL CWndCoco::IsJPEG2H263() const
{
	return m_bJPEG2h263;
}
/////////////////////////////////////////////////////////////////////////////
#endif
