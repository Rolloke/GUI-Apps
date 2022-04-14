#pragma once
#include "wndsmall.h"

class CServer;

class CWndImage : public CWndSmall
{
	DECLARE_DYNAMIC(CWndImage)

public:
	CWndImage(CServer* pServer);
protected:
	CWndImage() {TRACE(_T("Error Do not use default constructor"));};
	virtual ~CWndImage(void);

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndImage)
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Overrides
public:
	virtual	CSize GetPictureSize();
	virtual UINT	GetToolBarID();
	virtual void	OnPrint(CDC* pDC, CPrintInfo* pInfo);
protected:

	// buffers for software decoding
	CCriticalSection		m_csPictureRecord;
	CIPCMediaSampleRecord*  m_pAudioRecord;
	CIPCPictureRecord*      m_pPictureRecord;
	CMPEG4Decoder*			m_pMpeg4Decoder;
	CJpeg*					m_pJpeg;
	CH263*					m_pH263;
	CPTDecoder*				m_pPTDecoder;
	CRect					m_rcZoom;

// Implementation
public:
	void	GetVideoClientRect(LPRECT lpRect);
protected:
	void	GetVideoClientRect(LPRECT lpRect, CSize picturesize, BOOL b1to1);
	void	DrawCross(CDC* pDC, WORD wX, WORD wY, CRect*prcVC=NULL);
	void	DrawCinema(CDC* pDC, LPRECT lpRectKeyColor = NULL);
};
