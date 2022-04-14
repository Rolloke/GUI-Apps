// YUVToRGB.h: interface for the CYUVToRGB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_YUVTORGB_H__7F99C845_72CA_11D1_93E4_00C095EC9EFA__INCLUDED_)
#define AFX_YUVTORGB_H__7F99C845_72CA_11D1_93E4_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CYUVToRGB  
{
	// construction
public:
	CYUVToRGB();
	virtual ~CYUVToRGB();

	// operation
public:
	void ConvertYUVtoRGB(BYTE *srcY, BYTE *srcU, BYTE *srcV, 
						 BYTE *dstRGB,  int width,  int height);

private:
	long int m_crv_tab[256];
	long int m_cbu_tab[256];
	long int m_cgu_tab[256];
	long int m_cgv_tab[256];
	long int m_tab_76309[256];

	BYTE*	m_clp;
};

#endif // !defined(AFX_YUVTORGB_H__7F99C845_72CA_11D1_93E4_00C095EC9EFA__INCLUDED_)
