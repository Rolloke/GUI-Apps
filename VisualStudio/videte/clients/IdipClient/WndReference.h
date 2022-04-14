// WndReference.h: interface for the CWndReference class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WndReference_H__8C01148C_32D0_11D3_9963_004005A19028__INCLUDED_)
#define AFX_WndReference_H__8C01148C_32D0_11D3_9963_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndImage.h"

class CWndReference : public CWndImage  
{
	DECLARE_DYNAMIC(CWndReference)
	// construction / destruction
public:
	CWndReference(CServer* pServer, CSecID id, const CString& sCameraName);
	virtual ~CWndReference();

public:
				 CSecID	  GetID();
	inline const CString& GetCameraName() const;

// Operations
public:
	// window creation
	BOOL Create(const RECT& rect, UINT nID, CViewIdipClient* pParentWnd);
	void Reload();

	// Overrides
public:
	virtual UINT	GetToolBarID();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndReference)
	//}}AFX_VIRTUAL
protected:
	virtual void OnDraw(CDC* pDC);
	virtual CString GetTitleText(CDC* pDC);

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndReference)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString m_sCameraName;
	CSecID  m_idCamera;
};
///////////////////////////////////////////////////////////////
inline const CString& CWndReference::GetCameraName() const
{
	return m_sCameraName;
}

#endif // !defined(AFX_WndReference_H__8C01148C_32D0_11D3_9963_004005A19028__INCLUDED_)
