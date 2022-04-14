// SVCoolBar.h
class CMainFrame;

#ifndef _SV_COOL_BAR_H
#define _SV_COOL_BAR_H

/////////////////
// SV Cool bar: specialized CCoolBar creates bands.
//
class CSVCoolBar : public CCoolBar 
{
public:
	int HitTest();
protected:
	DECLARE_DYNAMIC(CSVCoolBar)
	virtual BOOL   OnCreateBands();

private:
	CCoolToolBar	m_wndToolBar;			 // toolbar
	CBitmapButton	m_wndButton;

	CImageList		m_ilCold;
	CImageList		m_ilHot;

	friend class CMainFrame;
};

#endif
