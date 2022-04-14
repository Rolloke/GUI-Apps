// CDecodeThread.h: interface for the CDecodeThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CDECODETHREAD_H__F242F2D1_4AFF_11D3_8DA1_004005A11E32__INCLUDED_)
#define AFX_CDECODETHREAD_H__F242F2D1_4AFF_11D3_8DA1_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CChildView;
class CMyH26xEncoder;

class CDecodeThread : public CWK_Thread  
{
public:
	CDecodeThread(CChildView* pView);
	virtual ~CDecodeThread();

public:
    void	OnDraw( CDC* pDC, const CRect &rcDest, const CRect &rcSrc = CRect(0,0,0,0));
	void    SetSelectionRect(const CRect &rcSelection);
	inline  DWORD GetCurrentFrame() const;
	inline  CJpeg& GetJPEG();

public:
	virtual BOOL Run(LPVOID lpContext);
	virtual	BOOL StartThread();
	virtual	BOOL StopThread();
			CString GetNextFileName();
			void OnEncodedFrame();

private:
	DWORD	m_dwDecodedFrames;
	CJpeg	m_Jpeg;
	CString m_sJPEGPath;
	CString m_sFileName;
	CString m_sTitle;
	DWORD	m_dwTC1;
	DWORD	m_dwTC2;
	CWK_PerfMon* m_pPerfDecode;
	CMyH26xEncoder* m_pH26xEncoder;
	CChildView* m_pView;
	CString m_sOutputFilename;
};
/////////////////////////////////////////////////////////
inline DWORD CDecodeThread::GetCurrentFrame() const
{
	return m_dwDecodedFrames;
}
/////////////////////////////////////////////////////////
inline CJpeg& CDecodeThread::GetJPEG()
{
	return m_Jpeg;
}

#endif // !defined(AFX_CDECODETHREAD_H__F242F2D1_4AFF_11D3_8DA1_004005A11E32__INCLUDED_)
