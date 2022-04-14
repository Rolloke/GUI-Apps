/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: outputpage.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/outputpage.h $
// CHECKIN:		$Date: 10.07.06 16:52 $
// VERSION:		$Revision: 26 $
// LAST CHANGE:	$Modtime: 10.07.06 16:47 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef OUTPUTPAGE_H
#define OUTPUTPAGE_H

#include "SecID.h"
#include "Output.h"
#include "SVPage.h"

#include "OutputDialog.h"


class COutputGroup;
class COutputList;
class CInputList;

//******************************************************************
// This is the Camera page now!!!
//******************************************************************

/////////////////////////////////////////////////////////////////////////////
// COutputPage dialog
class COutputPage : public CSVPage
{
	DECLARE_DYNAMIC(COutputPage)

// Construction
public:
	COutputPage(CSVView* pParent,COutputGroup* pOutputGroup);
	~COutputPage();

// Overrides
protected:
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COutputPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// attributes
public:
	inline CSecID GetSelectedCamera() const;

// Implementation
public:

// Overrides
protected:
	virtual BOOL StretchElements();
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual int  GetNoOfSubDlgs();
	virtual CWK_Dialog* GetSubDlg(int);
	virtual BOOL GetToolTip(UINT nID, CString&sText);

protected:
	void	Resize();
	void	Reset(BOOL bDirection);
	void	CreateOutputDialogs();
	void	ShowHideStuff();
	void	WriteMiCoIni(int nMiCoNr);
	void	WriteCoCoIsaIni();
	void	CheckComPorts();
	BOOL	IsSNOK();
	BOOL	IsCameraOK();

	// Generated message map functions
	//{{AFX_MSG(COutputPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDeltaposSpinTypeCamera(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog Data
protected:
	//{{AFX_DATA(COutputPage)
	enum { IDD = IDD_OUTPUTS };
	CStatic	m_staticAtDisconnect;
	CScrollBar	m_ScrollBar;
	CStatic	m_staticOutputType;
	CStatic	m_staticOutputNr;
	CStatic	m_staticOutputComment;
	CStatic	m_staticOutputSN;
	CStatic	m_staticManu;
	CStatic	m_staticCom;
	CStatic	m_staticID;
	int		m_iScroll;
	//}}AFX_DATA
//	CStatic m_staticTermination;

private:
	CString				m_sGroup;
	CSecID				m_idGroup;
	CSecID				m_idSelectedCamera;
	COutputList*		m_pOutputList;	// Outputliste mit den Daten
	COutputGroup*		m_pOutputGroup;	// Aktuell selektierte Outputgruppe
	CInputList*			m_pInputList;	// Inputliste mit den Daten

	COutputDialogArray	m_OutputDialogs;
	BOOL				m_bAnySN;
	BOOL				m_bShowTermination;
	BOOL				m_bShowVHS;

	friend class COutputDialog;
public:
};
/////////////////////////////////////////////////////////////////////////
inline CSecID COutputPage::GetSelectedCamera() const
{
	return m_idSelectedCamera;
}


#endif // OUTPUTPAGE_H
