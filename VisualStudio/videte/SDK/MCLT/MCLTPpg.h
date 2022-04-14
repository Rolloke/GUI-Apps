#if !defined(AFX_MCLTPPG_H__08C1825A_3889_4D91_B9D5_3C2DF293881B__INCLUDED_)
#define AFX_MCLTPPG_H__08C1825A_3889_4D91_B9D5_3C2DF293881B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// MCLTPpg.h : Declaration of the CMCLTPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CMCLTPropPage : See MCLTPpg.cpp.cpp for implementation.

class CMCLTPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CMCLTPropPage)
	DECLARE_OLECREATE_EX(CMCLTPropPage)

// Constructor
public:
	CMCLTPropPage();

// Dialog Data
	//{{AFX_DATA(CMCLTPropPage)
	enum { IDD = IDD_PROPPAGE_MCLT };
	int		m_nShape;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CMCLTPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MCLTPPG_H__08C1825A_3889_4D91_B9D5_3C2DF293881B__INCLUDED)
