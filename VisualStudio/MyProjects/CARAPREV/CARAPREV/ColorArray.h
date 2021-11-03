// ColorArray.h: Schnittstelle für die Klasse CColorArray.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORARRAY_H__372F0180_F990_11D2_9E4E_0000B458D891__INCLUDED_)
#define AFX_COLORARRAY_H__372F0180_F990_11D2_9E4E_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define  INVALID_COLOR 0x80000000
#define  CV_PROTECTED      1
#define  CV_INTERPOLATE    2

#define MIN_COLORS               2
#define MAX_COLORS               4096

class AFX_EXT_CLASS CColorArray  
{
public:
	void FillSolidRect(CDC*pDC, int, int, int, int, COLORREF);
	CColorArray();
	virtual ~CColorArray();
	void     Draw(CDC*, CRect *);
   COLORREF&SetColor(int);
	bool     SetColors(COLORREF*, int, int nFirst=0);
	COLORREF GetColor(int);
   bool     GetColors(COLORREF*, int, int nFirst=0);
   void     SetPaletteColors();
   HPALETTE GetPaletteHandle() {return m_hPalette;}
   int      GetNumColor() {return m_nColors;};
   const    COLORREF* GetColorBuffer();
   bool     CheckRange(int& ); 
	void     InterpolateColors(int, int, COLORREF, COLORREF);
	void     SetIndexColors(int , int , COLORREF, COLORREF);
	bool     SetArraySize(int);
   void     SetColorMode(int nM) {m_nColorMode = nM;};
   int      GetColorMode()       {return m_nColorMode;};
	bool     IsChanged();

   void     operator =(CColorArray &);

   static void* alloc(size_t nSize, void *pOld=NULL);
   static void  free(void* p);
	static int Round(double);

protected:
	int        m_nColors;
   int        m_nColorMode;
	COLORREF  *m_pColors;
   HPALETTE   m_hPalette;
   bool       m_bChanged;
   static  int          gm_nNeedPalette;
private:
   static  HANDLE       gm_hHeap;
   static  unsigned     gm_nInstances;
   static  COLORREF     gm_cDummy;
};

#endif // !defined(AFX_COLORARRAY_H__372F0180_F990_11D2_9E4E_0000B458D891__INCLUDED_)
