// GraphicsLib.h: interface for the CGraphicsLib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHICSLIB_H__B020F106_B9FE_11D4_869B_004005A26A3B__INCLUDED_)
#define AFX_GRAPHICSLIB_H__B020F106_B9FE_11D4_869B_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGraphicsLib  
{
	// construction / destruction
public:
	CGraphicsLib();
	CGraphicsLib(HANDLE hDIB);
	virtual ~CGraphicsLib();

	// attributes
public:
	HANDLE GetDIB();
	HBITMAP GetHBitmap();

	// operations
public:
	void Convolve2D();
	void DisplayImage(CWnd* pWnd,CDC *dc, int id, HBITMAP hBmp);
	void ResetData(HANDLE hDIB);
	BOOL FixedFilter(IplFilter iFilter);
	BOOL ReduceImageSize(int iFactor, int iInterpolate=IPL_INTER_NN);
	BOOL ResizeImage(int iWidth, int iHeight, int iInterpolate=IPL_INTER_NN);
	BOOL RGB2YUV(BYTE* Y, BYTE* U, BYTE* V);

	// data
private:
	char* m_pImageDIBData;
	DWORD m_dwSizeOfHeader;
	DWORD m_dwSizeOfImage;
	WORD m_wBitsPerPixel;
	LONG m_lHeight;
	LONG m_lWidth;
	int  m_iPlanes;
	int  m_iCompression;	
  	int  m_ixPelsPerMeter;
	int  m_iyPelsPerMeter;    
  	int  m_ibiClrUsed;
  
	void InitData();
	HBITMAP CreateBitmap(int iwidth, int iheight, WORD wBitsPerPixel);
	IplImage* DIBToIPL(HBITMAP hBitmap);
	IplImage *m_pIPLBitmap;

	HBITMAP CreateHBITMAP(LPBITMAPINFO lpBMI);
	HBITMAP m_hBITMAP;
	HBITMAP DIBToHBITMAP(HANDLE hDIB);
	HANDLE m_hDIB;
};

#endif // !defined(AFX_GRAPHICSLIB_H__B020F106_B9FE_11D4_869B_004005A26A3B__INCLUDED_)
