// CMyJpeg.h: interface for the CMyJpeg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMYJPEG_H__B3389344_4A42_11D3_8D9F_004005A11E32__INCLUDED_)
#define AFX_CMYJPEG_H__B3389344_4A42_11D3_8D9F_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define USE_PERMON


#include <afxmt.h>

#ifndef __IJL_H__
#pragma pack (push) 
#include "vfw.h"
#include "IJL.h"
#pragma pack (pop)
#endif

#ifdef USE_PERMON
	#include <WKClasses\WK_PerfMon.h>
#endif

class AFX_EXT_CLASS CMyJpeg  
{
	// construction / destruction
public:
	CMyJpeg();
	virtual ~CMyJpeg();

	// Copy construcor
/*
	CMyJpeg(CMyJpeg* pJpeg);
*/
								  
	// attributes
public:
	// Image Data
	COLORREF GetPixel(int nX, int nY);
	inline const BYTE* GetDibBuffer() const;
	inline IJL_COLOR GetColorSpace() const;

	// Information
	SIZE    GetImageDims() const;
	BOOL    IsEmpty()		const { return (m_pDibImageData == NULL); }
	BOOL	IsValid()		const { return m_bValidImage;}

	// operations
public:
	void SetProperties(int iWidth=768,int iHeight=576,IJL_COLOR DIBColor=IJL_BGR);
	//void SetProperties(int iWidth,int iHeight,int DIBColor);
	
	void SetQuality(int iJpegQuality);

	// Encode
	BOOL EncodeJpegToMemory(int nSrcWidth, int nSrcHeight, int nDstWidth, int nDstHeight, int nSourceType, BYTE* pDIBData, BYTE* &pJpegData, DWORD &dwJpegLen);
	BOOL ScaleDown(const CSize& siSrc, const CSize& siDst, BGR24* pSrc);

	// Randbedingung: siSrc muﬂ ein geradzahlig vielfaches von siDst sein!
	BOOL ScaleDownYUV422(const CSize& siSrc, const CSize& siDst, YUV422* pSrc);

private:
	int m_iJpegQuality;

	JPEG_CORE_PROPERTIES	m_JpegCoreProp;
	BITMAPINFO				m_bmi;						   
    BYTE*					m_pDibImageData;

	int						m_nChannels;
	BOOL					m_bScaleToFit;
	BOOL					m_bCenterOnPage;
	BOOL					m_bValidImage;

	int						m_iWidth;
	int						m_iHeight;
	IJL_COLOR				m_DIBColor;
	CWK_PerfMon*			m_pPerf1;
	CWK_PerfMon*			m_pPerf2;
};

inline const BYTE* CMyJpeg::GetDibBuffer() const
{
	return m_pDibImageData;
}
inline IJL_COLOR CMyJpeg::GetColorSpace() const
{
	return m_DIBColor;
}


#endif // !defined(AFX_CMYJPEG_H__B3389344_4A42_11D3_8D9F_004005A11E32__INCLUDED_)
