// CJpeg.h: interface for the CJpeg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CJPEG_H__B3389344_4A42_11D3_8D9F_004005A11E32__INCLUDED_)
#define AFX_CJPEG_H__B3389344_4A42_11D3_8D9F_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#ifndef __IJL_H__
#pragma pack (push) 
#include "vfw.h"
#include "IJL.h"
#pragma pack (pop)
#endif

// byte offsets								   
#define BO_BLUE     0
#define BO_GREEN    1
#define BO_RED      2

class CJpeg  
{
public:
	CJpeg();
	virtual ~CJpeg();

	void SetOSDText1(int nX, int nY, const char* sText);
	void SetBackgroundColor(COLORREF colBgrnd);
	void SetTextColor(COLORREF colText);
	void Copy(HWND hWnd);

	// Decode
	BOOL	DecodeJpegFromMemory(BYTE* pData , DWORD dwLen, BOOL bAllowDoubleFieldDecoding=TRUE, BOOL bInterpolate=TRUE);
    
	// Drawing							  
	void	OnDraw(HDC hDC, const RECT& rcDest, const RECT& rcSrc, BOOL bPrint);
	
	// Information
	SIZE    GetImageDims() const;
	BOOL    IsEmpty()		const { return (m_pDibImageData == NULL); }
	BOOL	IsValid()		const { return m_bValidImage;}

private:
	JPEG_CORE_PROPERTIES	m_JpegCoreProp;
	BITMAPINFO				m_bmi;
    BYTE*					m_pDibImageData;
    SIZE					m_JpegCorePropDims;
	int						m_nChannels;
	BOOL					m_bScaleToFit;
	BOOL					m_bCenterOnPage;
	BOOL					m_bValidImage;

	int						m_nOSDText1X;
	int						m_nOSDText1Y;
	char					m_szOSDText1[_MAX_PATH];

	COLORREF				m_colText;
	COLORREF				m_colBgrnd;

	HDRAWDIB				m_hDrawDib;

	BYTE*					m_blDibLine;					
};

#endif // !defined(AFX_CJPEG_H__B3389344_4A42_11D3_8D9F_004005A11E32__INCLUDED_)
