// SDIConfigCoolBar.h: interface for the CSDIConfigCoolBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDICONFIGCOOLBAR_H__CE47D5B3_B42D_11D2_B565_004005A19028__INCLUDED_)
#define AFX_SDICONFIGCOOLBAR_H__CE47D5B3_B42D_11D2_B565_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSDIConfigCoolBar : public CCoolBar  
{
protected:
	DECLARE_DYNAMIC(CSDIConfigCoolBar)
	virtual BOOL   OnCreateBands();

private:
	CCoolToolBar	m_wndToolBar;			 // toolbar
	CBitmapButton	m_wndButton;

	CImageList		m_ilCold;
	CImageList		m_ilHot;

	friend class CMainFrame;
};

#endif // !defined(AFX_SDICONFIGCOOLBAR_H__CE47D5B3_B42D_11D2_B565_004005A19028__INCLUDED_)
