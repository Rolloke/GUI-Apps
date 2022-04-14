/////////////////////////////////////////////////////////////////////////////
// PROJECT:		ImageCompare
// FILE:		$Workfile: ImageCompareDoc.h $
// ARCHIVE:		$Archive: /Project/Tools/ImageCompare/ImageCompareDoc.h $
// CHECKIN:		$Date: 3.05.99 12:13 $
// VERSION:		$Revision: 15 $
// LAST CHANGE:	$Modtime: 3.05.99 12:05 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGECOMPAREDOC_H__C221097B_F324_11D2_8D05_004005A11E32__INCLUDED_)
#define AFX_IMAGECOMPAREDOC_H__C221097B_F324_11D2_8D05_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum IStatus
{
	Unknown	= 1,
	Valid	= 2,
	Unvalid	= 3
};

class CImageRef;
class CImageRefList;
class CImageCompareDoc : public CDocument
{
protected: // create from serialization only
	CImageCompareDoc();
	DECLARE_DYNCREATE(CImageCompareDoc)

// Attributes
public:
	
// Operations
public:
	BOOL InitImagin(HWND hWnd);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageCompareDoc)
	public:
	virtual BOOL OnNewDocument();
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CImageCompareDoc();
	long GetBufferID1(){return m_BufferID1;}
	long GetBufferID2(){return m_BufferID2;}

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CImageCompareDoc)
	afx_msg void OnButtonLeft();
	afx_msg void OnButtonRight();
	afx_msg void OnButtonImageOk();
	afx_msg void OnButtonImageWrong();
	afx_msg void OnUpdateButtonLeft(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonRight(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonImageOk(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonImageWrong(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonEnd(CCmdUI* pCmdUI);
	afx_msg void OnButtonEnd();
	afx_msg void OnButtonFirst();
	afx_msg void OnUpdateButtonFirst(CCmdUI* pCmdUI);
	afx_msg void OnButtonLast();
	afx_msg void OnUpdateButtonLast(CCmdUI* pCmdUI);
	afx_msg void OnButtonSave();
	afx_msg void OnUpdateButtonSave(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL	LoadImages(int nIndex);
	void	ScanWWWRoot();
	CImageRef* GetImageRef(int nIndex);

private:
	long			m_BufferID1;
	long			m_BufferID2;
	long			m_ViewID;
	int				m_nImageCnt;
	CImageRefList	*m_pImageList;
	BOOL			m_bModify;
	CString			m_sHtmPath;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGECOMPAREDOC_H__C221097B_F324_11D2_8D05_004005A11E32__INCLUDED_)
