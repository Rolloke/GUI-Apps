/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ptwindow.h $
// ARCHIVE:		$Archive: /Project/Clients/Vision/ptwindow.h $
// CHECKIN:		$Date: 2.11.99 10:10 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 2.11.99 10:02 $
// BY AUTHOR:	$Author: Uwe $
// $Nokeywords:$

#ifndef __PTWND_H_
#define __PTWND_H_

#include "DisplayWindow.h"

class CPTDecoder;

/////////////////////////////////////////////////////////////////////////////
// CPTWindow window
class CPTWindow : public CDisplayWindow
{
// Construction
public:
	CPTWindow(CServer* pServer, const CIPCOutputRecord &or);
	virtual ~CPTWindow();

// Attributes
public:
	virtual BOOL IsPresence();
	
// Operations
public:
	virtual void Settings();
	virtual	CSize GetPictureSize();
	virtual void Set1to1(BOOL b1to1);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPTWindow)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnDraw(CDC* pDC);

	// Generated message map functions
protected:
	//{{AFX_MSG(CPTWindow)
	afx_msg void OnChangeVideo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// private helper methods
private:

	// data members
private:

	CPTDecoder*	m_pDecoder;
};
/////////////////////////////////////////////////////////////////////////////
#endif
