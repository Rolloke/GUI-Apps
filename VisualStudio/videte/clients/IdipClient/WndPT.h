/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: WndPT.h $
// ARCHIVE:		$Archive: /Project/Clients/IdipClient/WndPT.h $
// CHECKIN:		$Date: 4.05.06 11:34 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 4.05.06 11:12 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __PTWND_H_
#define __PTWND_H_

#include "WndLive.h"

class CPTDecoder;

/////////////////////////////////////////////////////////////////////////////
// CWndPT window
class CWndPT : public CWndLive
{
	DECLARE_DYNAMIC(CWndPT)
// Construction
public:
	CWndPT(CServer* pServer, const CIPCOutputRecord &or);
	virtual ~CWndPT();

// Attributes
public:
	
// Operations
public:
	virtual void Settings();
	virtual	CSize GetPictureSize();
	virtual void Set1to1(BOOL b1to1);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndPT)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnDraw(CDC* pDC);

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndPT)
	afx_msg LRESULT OnChangeVideo(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// private helper methods
private:

	// data members
private:
};
/////////////////////////////////////////////////////////////////////////////
#endif
