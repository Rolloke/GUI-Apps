// H263DirectDraw.h: interface for the CH263DirectDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_H263DIRECTDRAW_H__3491C362_7069_11D1_93E4_00C095EC9EFA__INCLUDED_)
#define AFX_H263DIRECTDRAW_H__3491C362_7069_11D1_93E4_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CH263DirectDraw  
{
	// construction/destruction
public:
	CH263DirectDraw();
	virtual ~CH263DirectDraw();

	// attributes
public:
	BOOL	GetFit();
	void	SetFit(BOOL bFit);

	int		GetZoom();
	void	SetZoom(int iZoom);

	// operations
public:
	BOOL Init(HWND hWnd);
	BOOL Exit();

	BOOL DrawFrameYUV(unsigned char *src0,
					  unsigned char *src1,
					  unsigned char *src2,
					  int width,
					  int height
					  );

	// implementation
private:
	HRESULT CreatePrimarySurface();
	BOOL	CreateOverlay();
	void	DestroyOverlay();
	BOOL	DisplayOverlay(int width, int height);
	BOOL	DisplayBlitter(int width, int height);

	BOOL	CreateBlitter();
	void	DestroyBlitter();

	void	CopyYUY2ToSurface(BYTE* srcY, BYTE *srcU, BYTE *srcV,
							 DWORD* destS,
							 int width,
							 int height,
							 DWORD dwPitch);

	void	CopyUYVYToSurface(BYTE* srcY, BYTE *srcU, BYTE *srcV,
							 DWORD* destS,
							 int width,
							 int height,
							 DWORD dwPitch);

	// data member
private:
	static	int m_iInstances;

	BOOL	m_bFit;
	int		m_iZoom;
	BOOL	m_bOverlay;

	HWND	m_hWnd;

	LPDIRECTDRAW        m_lpdd;           // DirectDraw object
	LPDIRECTDRAWSURFACE	m_lpddsPrimary;
	LPDIRECTDRAWSURFACE	m_lpddsOverlay;
	LPDIRECTDRAWCLIPPER m_lpClipper;
	LPDIRECTDRAWSURFACE	m_lpddsBlitter;

};

#endif // !defined(AFX_H263DIRECTDRAW_H__3491C362_7069_11D1_93E4_00C095EC9EFA__INCLUDED_)
