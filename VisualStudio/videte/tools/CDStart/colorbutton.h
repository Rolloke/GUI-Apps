/////////////////////////////////////////////////////////////////////////////
// DVButton.h : header file
#if !defined(AFX_COLORBUTTON_H__9389B14F_A72A_12D3_8A31_004505A19078__INCLUDED_)
#define AFX_COLORBUTTON_H__9389B14F_A72A_12D3_8A31_004505A19078__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
class CColorButton : public CSkinButton
{
// Construction
public:
	CColorButton::CColorButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorButton)
	public:
	protected:
	//}}AFX_VIRTUAL
protected:
	virtual	void DrawRectangleButton(const enumODBS& eODBS, CRect rect, CDC* pDC);
	virtual	void DrawRectangle(const enumODBS& eODBS, CRect rect, COLORREF colBase, CDC* pDC);
	virtual	void DrawRectanglePlaneColor(CRect rect, COLORREF colBase, CDC* pDC);
	virtual void DrawRectangle3DPlaneEdge(const enumODBS& eODBS, int iEdge, CRect rect, COLORREF colBase, CDC* pDC);
	virtual void DrawRectangleColorChangeBrushed(CRect rect, COLORREF colBase, CDC* pDC);
	virtual void DrawRectangleColorChangeSimple(CRect rect, COLORREF colBase, CDC* pDC);

	virtual void DrawRoundButton(const enumODBS& eODBS, CRect rect, CDC* pDC);

// Implementation
public:
	inline void	EnableChangeOnlyBrightness();
protected:
private:
	void		RGB2YCrCb(COLORREF col, double& dY, double& dCb, double& dCr);
	COLORREF	YCrCb2RGB(const double& dY, const double& dCb, const double& dCr);
	COLORREF	ChangeBrightness(COLORREF col, double dYDiff);
	// Generated message map functions
protected:
	//{{AFX_MSG(CColorButton)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL	m_bChangeOnlyBrightness;
	double	m_dMaxColorChangeRange;
};
/////////////////////////////////////////////////////////////////////////////
void CColorButton::EnableChangeOnlyBrightness()
{
	m_bChangeOnlyBrightness = TRUE;
}
*/
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORBUTTON_H__9389B14F_A72A_12D3_8A31_004505A19078__INCLUDED_)
