// XMLViewDlg.h : header file
//

#if !defined(AFX_XMLVIEWDLG_H__32A0217B_15FC_45E0_98A7_39787FA1F9C3__INCLUDED_)
#define AFX_XMLVIEWDLG_H__32A0217B_15FC_45E0_98A7_39787FA1F9C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "XmlNodeWrapper.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CXMLViewDlg dialog
class CXMLViewDlg : public CDialog
{
// Construction
public:
	CXMLViewDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CXMLViewDlg();	// standard destructor

// Dialog Data

	//{{AFX_DATA(CXMLViewDlg)
   enum { IDD = IDD_EDIT_DLG};
	CTreeCtrl	m_tree;
   CComboBox m_sCombo;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXMLViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
   virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
   void SetDlgID(UINT nID);

private:
	HTREEITEM ParseNode(IDispatch* pNode,HTREEITEM hParent, HTREEITEM hInsertAfter=TVI_LAST);
   bool FindNode(HTREEITEM hParent, CString&sFind);
   void ClearTree();
   void UpdateChanges();
   void AddChildren(HTREEITEM hParent, CMapStringToPtr&aNodes);
   void DDX_Check(CDataExchange*pDX, UINT, bool &);

   typedef enum { eSideMask = 0x0f, eLeft=1, eTop=2, eRight=4, eBottom=8, eNoStretchX=0x10, eNoStretchY=0x20 } enumRectBind;
   struct sWinRect
   {
      sWinRect();
      CRect rcWnd;
      CDWordArray eBind;
      CDWordArray nIDRef;
   };
   void SetBind(WORD wID, int nBind, UINT nIDRef=0);

   typedef enum { eNone=0, eValues, eType, eBool, eInt, eFloat, eString, eMax, eMin, eHelp } enumKeys;
   typedef enum { eNodeClosed=0, eNodeOpen, eNodeText, eNodeAttrib, eNodeComment } eImages;
   class sItemValue
   {
   public:
      sItemValue()
      {
         m_Type = eString;
         memset(&m_Min, 0, sizeof(m_Min));
         memset(&m_Max, 0, sizeof(m_Max));
         m_bMin = false;
         m_bMax = false;
         m_nReference = 1;
      };
      sItemValue(sItemValue&iv)
      {
         m_Type = iv.m_Type;
         m_sValues.Append(iv.m_sValues);
         m_sHelp = iv.m_sHelp;
         m_bMin = iv.m_bMin;
         m_bMax = iv.m_bMax;
         memcpy(&m_Min, &iv.m_Min, sizeof(m_Min));
         memcpy(&m_Max, &iv.m_Max, sizeof(m_Max));
         m_nReference = 1;
      };
      void AddReference()
      {
         m_nReference++;
      };
      void Release()
      {
         m_nReference--;
         if (m_nReference <= 0)
         {
            delete this;
         }
      };
      enumKeys m_Type;
      union
      {
         double d;
         int n;
      }m_Min;
      union
      {
         double d;
         int n;
      }m_Max;
      bool m_bMin, m_bMax;
      CStringArray m_sValues;
      CString m_sHelp;
      int m_nReference;
   }*m_pItemType;

	// Generated message map functions
	//{{AFX_MSG(CXMLViewDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedOpenFile();
	afx_msg void OnBnClickedSetXml2Edt();
	afx_msg void OnBnClickedParseEdtXml();
   afx_msg void OnDropFiles(HDROP hDropInfo);
   afx_msg void OnBnClickedCkUseKeywords();
   afx_msg void OnDestroy();
   afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
   afx_msg void OnCbnSelchangeComboItem();
   afx_msg void OnEnChangeEdtItem();
   afx_msg void OnBnClickedSaveFile();
   afx_msg void OnEditCopy();
   afx_msg void OnEditPaste();
   afx_msg void OnEditClear();
   afx_msg void OnEnChangeEditName();
   afx_msg void OnBnClickedBtnFind();
   afx_msg void OnBnClickedCkCaseSens();
   afx_msg void OnUpdateEditClear(CCmdUI *pCmdUI);
   afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
   afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg BOOL OnToolTipNotify(UINT, NMHDR *, LRESULT *);
   afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
   HICON m_hIcon;
   HTREEITEM m_hCurrent, m_hClicked;
   HACCEL m_hAccelerator;

   int m_nCurrentAttrib;
   CSize         m_szWnd, m_szOld;

   bool m_bEditDlg;
   bool m_bEditMode;
   bool m_bChanged;
   bool m_bNameChanged;
   bool m_bDocChanged;
   bool m_bCanDoChanges;
   bool m_bInitPredefinedTypes;
   bool m_bCaseSensitive;
   CString m_sXML_File;
   CString m_sItemName;
   CString m_sAttribute;
   CString m_sItem;

	CXmlDocumentWrapper m_xmlDoc;
   CXmlNodeWrapper m_CurrentNode;
   CMapStringToPtr m_Types;
   CMapStringToPtr m_PredefinedTypes;
   CMapPtrToPtr m_NodeType;
   CMapWordToPtr m_WndRects;

public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XMLVIEWDLG_H__32A0217B_15FC_45E0_98A7_39787FA1F9C3__INCLUDED_)
