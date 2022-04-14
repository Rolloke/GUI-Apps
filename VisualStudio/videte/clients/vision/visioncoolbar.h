// VisionCoolBar.h
class CMainFrame;

#ifndef _VISION_COOL_BAR_H
#define _VISION_COOL_BAR_H

/////////////////
// Vision Cool bar: specialized CCoolBar creates bands.
//
class CVisionCoolBar : public CCoolBar 
{
public:
	int HitTest();
	void SetStore(BOOL bOn);

protected:
	DECLARE_DYNAMIC(CVisionCoolBar)
	virtual BOOL   OnCreateBands();

private:
	int				m_iSaveIndex;
	CCoolToolBar	m_wndToolBar;			 // toolbar
	CBitmapButton	m_wndButton;

	CImageList		m_ilCold;
	CImageList		m_ilHot;

	friend class CMainFrame;
};

#endif
