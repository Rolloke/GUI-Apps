/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CocoWindow.h $
// ARCHIVE:		$Archive: /Project/Clients/Vision/CocoWindow.h $
// CHECKIN:		$Date: 10.02.03 15:57 $
// VERSION:		$Revision: 23 $
// LAST CHANGE:	$Modtime: 10.02.03 12:00 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef __COCOWND_H_
#define __COCOWND_H_

#include "DisplayWindow.h"

#define CW_KEYCOLOR_OFFSET 3
/////////////////////////////////////////////////////////////////////////////
// CCocoWindow window
class CCocoWindow : public CDisplayWindow
{
// Construction
public:
	CCocoWindow(CServer* pServer, const CIPCOutputRecord &or, BOOL bJPEG2h263);
	virtual ~CCocoWindow();

// Attributes
public:
	virtual BOOL IsCoco();
	inline	BOOL IsSoftwareDecoding() const;
	inline 	BOOL IsHardwareDecoding() const;
	inline  BOOL IsJPEG2H263() const;
	
// Operations
public:
	virtual void SetCIPCOutputWindow(LPRECT lpRect = NULL);
			void GetVideoRect(LPRECT lpRect);
	virtual void GetVideoClientRect(LPRECT lpRect); 
	virtual BOOL PictureData(const CIPCPictureRecord &pict);
	virtual	void ClearWasSomeTimesActive();
	virtual	CSize GetPictureSize();
	virtual void Set1to1(BOOL b1to1);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCocoWindow)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnDraw(CDC* pDC);
	virtual void DrawBitmap(CDC* pDC, const CRect &rcDest);
			void DrawKeyColor(CDC* pDC);
			BOOL CanAdjustFPS(int fps);

protected:
	virtual CString GetTitleText(CDC* pDC);


	// Generated message map functions
protected:
	//{{AFX_MSG(CCocoWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDecodeVideo();
	afx_msg void OnFps11();
	afx_msg void OnUpdateFps11(CCmdUI* pCmdUI);
	afx_msg void OnFps31();
	afx_msg void OnUpdateFps31(CCmdUI* pCmdUI);
	afx_msg void OnFps61();
	afx_msg void OnUpdateFps61(CCmdUI* pCmdUI);
	afx_msg void OnFps121();
	afx_msg void OnUpdateFps121(CCmdUI* pCmdUI);
	afx_msg void OnFps91();
	afx_msg void OnUpdateFps91(CCmdUI* pCmdUI);
	afx_msg void OnVideoSaveReference();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// private helper methods
private:

	// data members
private:
	BOOL	m_bSoftwareDecoding;
	CCriticalSection m_csH263;
	CH263*	m_pH263;
	DWORD   m_dwTick;
	int		m_iFrames;
	BOOL	m_bJPEG2h263;
public:
	static  int m_bShowFPS;
};
/////////////////////////////////////////////////////////////////////////////
inline	BOOL CCocoWindow::IsSoftwareDecoding() const
{
	return m_bSoftwareDecoding && m_pH263;
}
/////////////////////////////////////////////////////////////////////////////
inline 	BOOL CCocoWindow::IsHardwareDecoding() const
{
	return !m_bSoftwareDecoding;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CCocoWindow::IsJPEG2H263() const
{
	return m_bJPEG2h263;
}
/////////////////////////////////////////////////////////////////////////////
#endif
