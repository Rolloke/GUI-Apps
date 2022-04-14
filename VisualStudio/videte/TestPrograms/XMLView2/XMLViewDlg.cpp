// XMLViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "XMLView.h"
#include "XMLViewDlg.h"
#include ".\xmlviewdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GETICONRESOURCE16(i) (HICON)LoadImage(AfxGetResourceHandle( ),MAKEINTRESOURCE(i),IMAGE_ICON,16,16,0)
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXMLViewDlg dialog
CXMLViewDlg::sWinRect::sWinRect()
{
   rcWnd.SetRectEmpty();
}

CXMLViewDlg::CXMLViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXMLViewDlg::IDD, pParent),
   m_hIcon(NULL), m_hCurrent(NULL), m_hClicked(NULL), m_hAccelerator(NULL),
   m_nCurrentAttrib(0),
   m_szWnd(0, 0), m_szOld(0, 0),
   m_bEditMode(true), m_bChanged(false), m_bNameChanged(), m_bDocChanged(false),
   m_bCanDoChanges(true), m_bInitPredefinedTypes(false), m_bCaseSensitive(false),
   m_sXML_File(_T("")),
   m_sItemName(_T("")),
   m_sAttribute(_T("")),
   m_sItem(_T(""))
{
	//{{AFX_DATA_INIT(CXMLViewDlg)
	//}}AFX_DATA_INIT

   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
   m_bEditDlg             = CXMLViewDlg::IDD == IDD_EDIT_DLG ? true : false;
   m_pItemType            = NULL;

   if (m_bEditDlg)
   {
      m_Types.SetAt(CString("values"), (void*)eValues);
      m_Types.SetAt(CString("type")  , (void*)eType);
      m_Types.SetAt(CString("bool")  , (void*)eBool);
      m_Types.SetAt(CString("true")  , (void*)eBool);
      m_Types.SetAt(CString("false") , (void*)eBool);
      m_Types.SetAt(CString("int")   , (void*)eInt);
      m_Types.SetAt(CString("float") , (void*)eFloat);
      m_Types.SetAt(CString("string"), (void*)eString);
      m_Types.SetAt(CString("max")   , (void*)eMax);
      m_Types.SetAt(CString("min")   , (void*)eMin);
      m_Types.SetAt(CString("help")  , (void*)eHelp);
   }
}

CXMLViewDlg::~CXMLViewDlg()	// standard destructor
{
   POSITION pos = m_PredefinedTypes.GetStartPosition();
   CString Key;
   void *pValue;
   while (pos)
   {
      m_PredefinedTypes.GetNextAssoc(pos, Key, pValue);
      ((sItemValue*)pValue)->Release();
   }
   m_NodeType.RemoveAll();

   pos = m_WndRects.GetStartPosition();
   sWinRect*pWR;
   WORD wID;
   while (pos)
   {
      m_WndRects.GetNextAssoc(pos, wID, (void*&)pWR);
      delete pWR;
   }
}

void CXMLViewDlg::SetDlgID(UINT nID)
{
   m_lpszTemplateName = MAKEINTRESOURCE(nID);
   m_bEditDlg = nID == IDD_EDIT_DLG ? true : false;
}

void CXMLViewDlg::DDX_Check(CDataExchange *pDX, UINT uID, bool &b)
{
   int bI = b;
   ::DDX_Check(pDX, uID, bI);
   b = bI ? true : false;
}

void CXMLViewDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CXMLViewDlg)
   DDX_Control(pDX, IDC_TREE1, m_tree);
   //}}AFX_DATA_MAP
   if (m_bEditDlg)
   {
      DDX_Check(pDX, IDC_CK_EDIT_MODE, m_bEditMode);
      DDX_Control(pDX, IDC_COMBO_ITEM, m_sCombo);
      DDX_Text(pDX, IDC_EDIT_NAME, m_sItemName);
      DDX_Check(pDX, IDC_CK_CASE_SENS, m_bCaseSensitive);
      if (m_pItemType && pDX->m_bSaveAndValidate && m_bChanged)
      {
         int nValue;
         double dValue;
         enumKeys eType = eString;
         if (m_pItemType) eType = m_pItemType->m_Type;
         switch (eType)
         {
            case eInt:
            DDX_Text(pDX, IDC_EDT_ITEM, nValue);
            if (m_pItemType->m_bMin || m_pItemType->m_bMax)
            {
               DDV_MinMaxInt(pDX, nValue, m_pItemType->m_Min.n, m_pItemType->m_Max.n);
            }
            break;
            case eFloat:
            DDX_Text(pDX, IDC_EDT_ITEM, dValue);
            if (m_pItemType->m_bMin || m_pItemType->m_bMax)
            {
               DDV_MinMaxDouble(pDX, dValue, m_pItemType->m_Min.d, m_pItemType->m_Max.d);
            }
            break;
            case eString:
            DDX_Text(pDX, IDC_EDT_ITEM, m_sItem);
            break;
            default:
            break;
         }
      }
   }
}

BEGIN_MESSAGE_MAP(CXMLViewDlg, CDialog)
	//{{AFX_MSG_MAP(CXMLViewDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_OPEN_FILE, OnBnClickedOpenFile)
	ON_BN_CLICKED(IDC_SET_XML_2_EDT, OnBnClickedSetXml2Edt)
	ON_BN_CLICKED(IDC_PARSE_EDT_XML, OnBnClickedParseEdtXml)
   ON_WM_DROPFILES()
   ON_BN_CLICKED(IDC_CK_EDIT_MODE, OnBnClickedCkUseKeywords)
   ON_WM_DESTROY()
   ON_NOTIFY(NM_CLICK, IDC_TREE1, OnNMClickTree1)
   ON_CBN_SELCHANGE(IDC_COMBO_ITEM, OnCbnSelchangeComboItem)
   ON_BN_CLICKED(IDC_SAVE_FILE, OnBnClickedSaveFile)
   ON_EN_CHANGE(IDC_EDT_ITEM, OnEnChangeEdtItem)
   ON_BN_CLICKED(ID_EDIT_COPY, OnEditCopy)
   ON_BN_CLICKED(ID_EDIT_PASTE, OnEditPaste)
   ON_BN_CLICKED(ID_EDIT_CLEAR, OnEditClear)
   ON_EN_CHANGE(IDC_EDIT_NAME, OnEnChangeEditName)
   ON_BN_CLICKED(IDC_BTN_FIND, OnBnClickedBtnFind)
   ON_BN_CLICKED(IDC_CK_CASE_SENS, OnBnClickedCkCaseSens)
   ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
   ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
   ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
	//}}AFX_MSG_MAP
   ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXMLViewDlg message handlers

BOOL CXMLViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

   DragAcceptFiles();
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	CImageList iml;
	iml.Create(16,16,ILC_COLOR32|ILC_MASK,5,1);
	iml.Add(GETICONRESOURCE16(IDI_NODECLOSED));
	iml.Add(GETICONRESOURCE16(IDI_NODEOPEN));
	iml.Add(GETICONRESOURCE16(IDI_TEXT));
	iml.Add(GETICONRESOURCE16(IDI_ATTRIB));
   iml.Add(GETICONRESOURCE16(IDI_COMMENT));
	m_tree.SetImageList(&iml,TVSIL_NORMAL);
	iml.Detach();

	// create XML tree sample

   if (m_bEditDlg)
   {
      m_hAccelerator = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR));
		if(m_xmlDoc.Load(_T("PreDefinedItems.xml")))
      {
         m_bInitPredefinedTypes = true;
		   ParseNode(m_xmlDoc.AsNode(),TVI_ROOT);
         ClearTree();
         m_bInitPredefinedTypes = false;
      }
      CWnd *pWnd;
      pWnd = GetDlgItem(ID_EDIT_CLEAR);
      if (pWnd)
      {
          ((CButton*)pWnd)->SetIcon((HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_CLEAR), IMAGE_ICON, 15,15,LR_DEFAULTCOLOR));
      }
      pWnd = GetDlgItem(ID_EDIT_COPY);
      if (pWnd)
      {
          ((CButton*)pWnd)->SetIcon((HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_COPY), IMAGE_ICON, 15,15,LR_DEFAULTCOLOR));
      }
      pWnd = GetDlgItem(ID_EDIT_PASTE);
      if (pWnd)
      {
          ((CButton*)pWnd)->SetIcon((HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_PASTE), IMAGE_ICON, 15,15,LR_DEFAULTCOLOR));
      }
   }
   else
   {
	   m_xmlDoc.LoadXML(_T("<?xml  version=\"1.0\" ?><root/>"));
	   CXmlNodeWrapper root(m_xmlDoc.AsNode());
	   CXmlNodeWrapper child, granchild;
	   child = root.InsertNode(-1, _T("Child1"));
	   granchild = child.InsertNode(0, _T("Granchild1"));
	   granchild.SetText(_T("Text1"));
	   granchild.SetValue(_T("V1"), _T("Value1"));
	   granchild.SetValue(_T("V2"), _T("Value2"));
	   granchild.SetValue(_T("V3"), _T("Value3"));
	   child = root.InsertNode(-1, _T("Child2"));
	   granchild = child.InsertNode(0, _T("Granchild1"));
	   granchild.SetText(_T("Text1"));
	   granchild.SetValue(_T("V1"), _T("Value1"));
	   granchild.SetValue(_T("V2"), _T("Value2"));
	   granchild.SetValue(_T("V3"), _T("Value3"));

	   ParseNode(m_xmlDoc.AsNode(), TVI_ROOT);
   }

   CWnd *pWnd = GetWindow(GW_CHILD);
	while (pWnd && IsChild(pWnd))
	{
      sWinRect*pWR = new sWinRect;
      pWnd->GetWindowRect(&pWR->rcWnd);
      ScreenToClient(&pWR->rcWnd);
      m_WndRects.SetAt(pWnd->GetDlgCtrlID(), pWR);
		pWnd = pWnd->GetNextWindow();
	};

   SetBind(IDC_TREE1, eTop);
   SetBind(IDC_TREE1, eLeft);
   SetBind(IDC_TREE1, eBottom);
   SetBind(IDC_OPEN_FILE, eBottom|eNoStretchY);
   SetBind(IDC_CK_EDIT_MODE, eBottom|eNoStretchY);
   SetBind(IDC_SAVE_FILE, eBottom|eNoStretchY);
   SetBind(IDC_SET_XML_2_EDT, eBottom|eNoStretchY);
   SetBind(IDC_PARSE_EDT_XML, eBottom|eNoStretchY);
   SetBind(ID_EDIT_COPY, eBottom|eNoStretchY);
   SetBind(ID_EDIT_PASTE, eBottom|eNoStretchY);
   SetBind(ID_EDIT_CLEAR, eBottom|eNoStretchY);
   SetBind(ID_EDIT_COPY, eRight|eNoStretchX);
   SetBind(ID_EDIT_PASTE, eRight|eNoStretchX);
   SetBind(ID_EDIT_CLEAR, eRight|eNoStretchX);
   SetBind(IDC_TXT_NAME, eTop|eNoStretchY);
   SetBind(IDC_TXT_NAME, eRight);
   SetBind(IDC_EDIT_NAME, eTop|eNoStretchY);
   SetBind(IDC_EDIT_NAME, eRight);
   SetBind(IDC_TXT_VALUE, eTop|eNoStretchY);
   SetBind(IDC_TXT_VALUE, eRight);
   SetBind(IDC_EDT_ITEM, eTop|eNoStretchY);
   SetBind(IDC_EDT_ITEM, eRight);
   SetBind(IDC_COMBO_ITEM, eTop|eNoStretchY);
   SetBind(IDC_COMBO_ITEM, eRight);
   SetBind(IDC_TXT_HELP, eTop|eNoStretchY);
   SetBind(IDC_TXT_HELP, eRight);
   SetBind(IDC_BTN_FIND, eTop|eNoStretchY);
   SetBind(IDC_EDIT_FIND, eTop|eNoStretchY);
   SetBind(IDC_EDIT_FIND, eRight);
   SetBind(IDC_CK_CASE_SENS, eTop|eNoStretchY);
   SetBind(IDC_CK_CASE_SENS, eTop|eNoStretchY);
   SetBind(IDC_EDT_XML, eTop);
   SetBind(IDC_EDT_XML, eRight);
   SetBind(IDC_EDT_XML, eBottom);

   EnableToolTips(true);
   return TRUE;  // return TRUE  unless you set the focus to a control
}

void CXMLViewDlg::SetBind(WORD wID, int nBind, UINT nIDRef/*=0*/)
{
   sWinRect*pWR;
   if (m_WndRects.Lookup(wID, (void*&)pWR))
   {
      pWR->eBind.Add(nBind);
      pWR->nIDRef.Add(nIDRef);
   }
}

void CXMLViewDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CXMLViewDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CXMLViewDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

bool CXMLViewDlg::FindNode(HTREEITEM hParent, CString&sFind)
{
   if (hParent != (HTREEITEM)0xffff0000)
   {
      if (m_hClicked)
      {
         if (m_hClicked == hParent)
         {
            m_hClicked = NULL;
         }
      }
      else
      {
         CString s = m_tree.GetItemText(hParent);
         if (!m_bCaseSensitive) s.MakeLower();
         if (s.Find(sFind) != -1)
         {
            m_tree.SelectItem(hParent);
            m_hClicked = hParent;
            LRESULT lR = 1;
            OnNMClickTree1(NULL, &lR);
            return false; // no further search
         }
      }
   }
   HTREEITEM hChild = m_tree.GetNextItem(hParent, TVGN_CHILD);
   while (hChild)
   {
      if (!FindNode(hChild, sFind)) return false;
      hChild = m_tree.GetNextItem(hChild, TVGN_NEXT);
   }
   return true;
}

int  SplitString(const CString &str, _TCHAR c, CStringArray &arr)
{
  int n1=0, n2;
  CString s;
  n2 = str.Find(c);
  while (n2 != -1)
  {
    s = str.Mid(n1, n2-n1);
    s.TrimLeft();
    s.TrimRight();
    if (s.GetLength())
    {
       arr.Add(s);
    }
    n1 = n2+1;
    n2 = str.Find(c, n1);
  }
  if (n1 < str.GetLength())
  {
    s = str.Mid(n1);
    s.TrimLeft();
    s.TrimRight();
    arr.Add(s);
  }
  return arr.GetCount();
}

void CXMLViewDlg::AddChildren(HTREEITEM hParent, CMapStringToPtr&aNodes)
{
   CStringArray aEqual;
   HTREEITEM hChild = m_tree.GetNextItem(hParent, TVGN_CHILD);
   CPtrArray*pHandles;
   while (hChild)
   {
      SplitString(m_tree.GetItemText(hChild), '=', aEqual);
      if (!aNodes.Lookup(aEqual[0], (void*&)pHandles))
      {
         pHandles = new CPtrArray;
         aNodes.SetAt(aEqual[0], (void*)pHandles);
      }
      pHandles->Add((void*)hChild);
      AddChildren(hChild, aNodes);
      hChild = m_tree.GetNextItem(hChild, TVGN_NEXT);
      aEqual.RemoveAll();     
   }
}

HTREEITEM CXMLViewDlg::ParseNode(IDispatch *pNode,HTREEITEM hParent, HTREEITEM hInsertAfter/*=TVI_LAST*/)
{
   CXmlNodeWrapper node(pNode);
   int img,imgSel;
   CString str, type;
   type = node.NodeType();
   TRACE(_T("%s\n"), type);

	if (type == _T("element"))
	{
      img = eNodeClosed;      // element is alway inserted
      imgSel = eNodeOpen;
      str = node.Name();
	}
	else if (type == _T("comment"))
   {
      img = imgSel = eNodeComment;
  	   str = node.GetText();
      if (m_bEditMode         // comment is parsed in edit mode
         && HIWORD((DWORD)hInsertAfter) != 0xffff
         && CXmlNodeWrapper((IDispatch*)m_tree.GetItemData(hInsertAfter)).NodeType() == _T("element"))
      {
         int iAdd, nAdd, iTyp, nTyp, nVal;
         CStringArray aAdd, aTyp, aVal;
         CMapStringToPtr aNodes;
         CString sName;
         CStringArray aEqual; // extract name
         SplitString(m_tree.GetItemText(hInsertAfter), '=', aEqual);
         CPtrArray*pHandles = new CPtrArray;
         pHandles->Add((void*)hInsertAfter);
         aNodes.SetAt(aEqual[0], (void*)pHandles); // relate names to handle
         AddChildren(hInsertAfter, aNodes);// add also child node recursively
         str.Replace(_T("\r"), _T(""));
         str.Replace(_T("\n"), _T(""));
         nAdd = SplitString(str, _T('@'), aAdd);  // split names
         for (iAdd=0; iAdd<nAdd; iAdd++)          // iterate all names
         {
            aTyp.RemoveAll();
            nTyp = SplitString(aAdd[iAdd], _T(';'), aTyp);// split types
            enumKeys Current;
            sItemValue sCurrent;
            void *p = 0;
            sCurrent.m_Type = eString;
            sCurrent.m_sHelp.Empty();
            sCurrent.m_sValues.RemoveAll();
            sName.Empty();
            for (iTyp=0; iTyp<nTyp; iTyp++)        // iterate types
            {
               aVal.RemoveAll();
               nVal = SplitString(aTyp[iTyp], _T(':'), aVal);// split value and type
               if (nVal == 1) // @name
               {
                  sName = aVal[0];
                  if (sName == _T("*"))
                  {
                     CString sKey;
                     POSITION pos = aNodes.GetStartPosition();
                     if (pos)
                     {
                        aNodes.GetNextAssoc(pos, sKey, p);
                        pHandles = (CPtrArray*)p;
                     }
                     while (pos)
                     {
                        aNodes.GetNextAssoc(pos, sKey, p);
                        pHandles->Append(*(CPtrArray*)p);
                     }
                  }
                  else
                  {
                     if (aNodes.Lookup(sName, p))
                     {
                        pHandles = (CPtrArray*)p;
                     }
                  }
               }
               else if (nVal == 2)
               {
                  if (m_Types.Lookup(aVal[0], p))
                  {
                     Current = (enumKeys)(int)p;
                     switch (Current)
                     {
                        case eType:
                        m_Types.Lookup(aVal[1], p);
                        sCurrent.m_Type = (enumKeys)(int)p;
                        break;
                        case eMin:
                        if (sCurrent.m_Type == eInt)
                        {
                            sCurrent.m_Min.n = _ttoi(LPCTSTR(aVal[1]));
                            if (!sCurrent.m_bMax)
                            {
                                sCurrent.m_Max.n = (sCurrent.m_Min.n > 0) ? MAXLONG : 0;
                            }
                        }
                        else
                        {
                            sCurrent.m_Min.d = _tstof(LPCTSTR(aVal[1]));
                        }
                        sCurrent.m_bMin = true;
                        break;
                        case eHelp:
                        sCurrent.m_sHelp = aVal[1];
                        break;
                        case eMax:
                        if (sCurrent.m_Type == eInt)
                        {
                            sCurrent.m_Max.n = _ttoi(LPCTSTR(aVal[1]));
                            if (!sCurrent.m_bMin)
                            {
                                sCurrent.m_Min.n = (sCurrent.m_Min.n < 0) ? MINLONG : 0;
                            }
                        }
                        else
                        {
                            sCurrent.m_Max.d = _tstof(LPCTSTR(aVal[1]));
                        }
                        sCurrent.m_bMax = true;
                        break;
                        case eValues:
                        sCurrent.m_Type = Current;
                        SplitString(aVal[1], _T(','), sCurrent.m_sValues);
                        break;
                     }
                  }
               }
            }
            if (m_bInitPredefinedTypes)
            {
               m_PredefinedTypes.SetAt(sName, new sItemValue(sCurrent));
            }
            else
            {
               int i, n = pHandles->GetCount();
               sItemValue *ps = new sItemValue(sCurrent);
               for (i=0; i<n; i++)
               {
                  void*p;
                  if (!m_NodeType.Lookup((void*)pHandles->ElementAt(i), p))
                  {
                     m_NodeType.SetAt((void*)pHandles->ElementAt(i), ps);
                     ps->AddReference();
                  }
               }
               ps->Release();
            }
         }
         POSITION pos = aNodes.GetStartPosition();
         while (pos)
         {
            aNodes.GetNextAssoc(pos, str, (void*&)pHandles);
            delete pHandles;
         }
         return TVI_LAST;
      }
   }
	else
	{
      CString sParent;
  	   str = node.GetText();
      img = imgSel = eNodeText;
      if (m_bEditMode)
      {
         if (HIWORD((DWORD)hParent) != 0xffff)
         {
            sParent = m_tree.GetItemText(hParent);
            m_tree.SetItemText(hParent, sParent + " = " + str);
         }
         return TVI_LAST;
      }
	}

   sItemValue *ps = NULL;
   if (m_bEditMode && HIWORD((DWORD)hInsertAfter) != 0xffff)
   {
      if (!m_NodeType.Lookup((void*)hInsertAfter, (void*&)ps))
      {
         CStringArray aEqual;
         ps = NULL;
         SplitString(m_tree.GetItemText(hInsertAfter), '=', aEqual);
         if (m_PredefinedTypes.Lookup(aEqual[0], (void*&)ps))
         {
            ps->AddReference();
         }
         else if (m_PredefinedTypes.Lookup(aEqual[0], (void*&)ps))
         {
            ps->AddReference();
         }
         if (ps)
         {
            m_NodeType.SetAt((void*)hInsertAfter, ps);
         }
         ps = NULL;
      }
   }

	HTREEITEM hLast = TVI_LAST, hItem = m_tree.InsertItem(str,img,imgSel,hParent,hInsertAfter);
	m_tree.SetItemData(hItem,(DWORD)pNode);

   for (int i = 0; i < node.NumAttributes(); i++)
   {
      HTREEITEM hAttribItem = m_tree.InsertItem(node.GetAttribName(i) + _T(" = ") + node.GetAttribVal(i),eNodeAttrib,eNodeAttrib,hItem);
      m_tree.SetItemData(hAttribItem,(DWORD)pNode);
   }

	for (i = 0; i < node.NumNodes(); i++)
	{
		hLast = ParseNode(node.GetNode(i),hItem, hLast);
	}
   return hItem;
}

void CXMLViewDlg::OnBnClickedOk()
{
	OnOK();
}

void CXMLViewDlg::OnBnClickedOpenFile()
{
	CFileDialog fDlg(TRUE,_T("xml"),NULL,OFN_HIDEREADONLY,_T("Xml Files (*.xml)|*.xml||"));
	if (fDlg.DoModal() == IDOK)
	{
        ClearTree();
        m_sXML_File = fDlg.GetPathName();
		m_xmlDoc.Load(m_sXML_File);
		ParseNode(m_xmlDoc.AsNode(),TVI_ROOT);
	}
/*
	CFileDialog fDlg(TRUE,_T("*"),NULL,OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT,_T("Xml Files (*.xml)|*.xml||"));
    fDlg.m_ofn.nMaxFile = 2048;
    fDlg.m_ofn.lpstrFile = (LPTSTR)GlobalAlloc(GMEM_FIXED, fDlg.m_ofn.nMaxFile*sizeof(_TCHAR));
    CString s;
	if (fDlg.DoModal() == IDOK)
	{
        POSITION pos = fDlg.GetStartPosition();
        while (pos)
        {
            s = fDlg.GetNextPathName(pos);
        }
    }
 */
}

void CXMLViewDlg::OnBnClickedSaveFile()
{
   m_xmlDoc.Save(m_sXML_File);
   m_bDocChanged = FALSE;
}

void CXMLViewDlg::OnBnClickedCkCaseSens()
{
   CDataExchange dx(this, TRUE);
   DDX_Check(&dx, IDC_CK_CASE_SENS, m_bCaseSensitive);
}

void CXMLViewDlg::OnBnClickedBtnFind()
{
   CString str;
   GetDlgItemText(IDC_EDIT_FIND, str);
   if (!m_bCaseSensitive) str.MakeLower();
   FindNode(TVI_ROOT, str);
}

void CXMLViewDlg::ClearTree()
{
	m_tree.DeleteAllItems();
   POSITION pos = m_NodeType.GetStartPosition();
   void*pKey, *pValue;
   while (pos)
   {
      m_NodeType.GetNextAssoc(pos, pKey, pValue);
      ((sItemValue*)pValue)->Release();
   }
   m_NodeType.RemoveAll();
   m_hCurrent       = NULL;
   m_hClicked       = NULL;
   m_bNameChanged   = false;
   m_bChanged       = false;
   m_bDocChanged    = false;
   m_CurrentNode.Detach();
}

void CXMLViewDlg::OnBnClickedSetXml2Edt()
{
	SetDlgItemText(IDC_EDT_XML, m_xmlDoc.GetXML());
}

void CXMLViewDlg::OnBnClickedParseEdtXml()
{
   CString sXML;
   GetDlgItemText(IDC_EDT_XML,	sXML);
   ClearTree();
   m_xmlDoc.LoadXML(sXML);
   ParseNode(m_xmlDoc.AsNode(),TVI_ROOT);
}

void CXMLViewDlg::OnDropFiles(HDROP hDropInfo)
{
   CString sPath;
   UINT nChars = DragQueryFile(hDropInfo, 0, sPath.GetBufferSetLength(MAX_PATH), MAX_PATH-1);
   if (nChars)
   {
      sPath.ReleaseBufferSetLength(nChars);
      ClearTree();
   	m_xmlDoc.Load(sPath);
		ParseNode(m_xmlDoc.AsNode(),TVI_ROOT);
      m_sXML_File = sPath;
   }
   else
   {
      sPath.ReleaseBuffer();
   }
   CDialog::OnDropFiles(hDropInfo);
}

void CXMLViewDlg::OnBnClickedCkUseKeywords()
{
   m_bEditMode = IsDlgButtonChecked(IDC_CK_EDIT_MODE) ? true:false;
}

void CXMLViewDlg::OnDestroy()
{
   UpdateChanges();
   if (m_bDocChanged && AfxMessageBox(_T("Speichern?"), MB_YESNO, 0) == IDYES)
   {
      OnBnClickedSaveFile();
   }
   ClearTree();
   CDialog::OnDestroy();
}

void CXMLViewDlg::UpdateChanges()
{
   CString s;
   enumKeys eType = eString;
   if (m_pItemType) eType = m_pItemType->m_Type;

   if (m_bChanged && m_CurrentNode.IsValid() && UpdateData())
   {
      switch (eType)
      {
         case eBool:case eValues:
            m_sCombo.GetLBText(m_sCombo.GetCurSel(), s);
            break;
         case eString: case eInt: case eFloat:
            GetDlgItemText(IDC_EDT_ITEM, s);
            break;
      }

      if (m_nCurrentAttrib != -1)
      {
         m_CurrentNode.SetAttribVal(m_nCurrentAttrib, s);
         if (m_hCurrent)
         {
            m_tree.SetItemText(m_hCurrent, m_CurrentNode.GetAttribName(m_nCurrentAttrib) + " = " + s);
         }
      }
      else
      {
         m_CurrentNode.SetText(s);
         if (m_hCurrent)
         {
            CXmlNodeWrapper node(m_CurrentNode.Parent());
            m_tree.SetItemText(m_hCurrent, node.Name() + " = " + s);
         }
      }
      m_bChanged = false;
   }
}

void CXMLViewDlg::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
   if (m_bEditDlg && m_bEditMode)
   {
      CPoint pt;
      UINT uFlag;
      HTREEITEM hTree;
      int nShowCombo = SW_HIDE;
      int nShowEdit  = SW_HIDE;
      m_bCanDoChanges = false;
      CStringArray aEqual;

      GetCursorPos(&pt);
      m_tree.ScreenToClient(&pt);
      UpdateChanges();

      m_pItemType = NULL;
      m_hCurrent = NULL;
      m_nCurrentAttrib = -1;
      m_CurrentNode.Detach();

      if (pNMHDR) hTree = m_tree.HitTest(pt, &uFlag);
      else        hTree = m_hClicked;
      
      if (hTree)
      {
         int nImage, nSelected;
         CString str = m_tree.GetItemText(hTree);
         IDispatch*pNode = (IDispatch*)m_tree.GetItemData(hTree);

         m_tree.GetItemImage(hTree, nImage, nSelected);
         m_hClicked = hTree;
         SplitString(m_tree.GetItemText(hTree), '=', aEqual);
         if (aEqual.GetCount() == 2)
         {
            SetDlgItemText(IDC_EDIT_NAME, aEqual[0]);
            if (!m_NodeType.Lookup((void*)hTree, (void*&)m_pItemType))
            {
                m_pItemType = NULL;
            }
            m_hCurrent = m_hClicked;
            if (nImage == eNodeAttrib)
            {
               m_CurrentNode = pNode;
               for (nImage=0; nImage<m_CurrentNode.NumAttributes(); nImage++)
               {
                  if (m_CurrentNode.GetAttribName(nImage) == aEqual[0])
                  {
                     m_nCurrentAttrib = nImage;
                     break;
                  }
               }
            }
            else
            {
               CXmlNodeWrapper node(pNode);
               m_CurrentNode = node.GetNode(0);
            }
         }
         else
         {
            SetDlgItemText(IDC_EDIT_NAME, str);
         }
      }

      if (aEqual.GetCount() == 2)
      {
         CString s;
         int i;
         enumKeys eType = eString;
         if (m_pItemType) eType = m_pItemType->m_Type;

         s = aEqual[1];
         m_sCombo.ResetContent();

         switch (eType)
         {
            case eBool:
               m_sCombo.AddString(_T("false"));
               m_sCombo.AddString(_T("true"));
               m_sCombo.SelectString(0, s);
               nShowCombo = SW_SHOW;
               break;
            case eValues:
               for (i=0; i<m_pItemType->m_sValues.GetCount(); i++)
               {
                 m_sCombo.AddString(m_pItemType->m_sValues[i]);
               }
               m_sCombo.SelectString(0, s);
               nShowCombo = SW_SHOW;
               break;
            case eString: case eInt: case eFloat:
               SetDlgItemText(IDC_EDT_ITEM, s);
               nShowEdit = SW_SHOW;
               break;
            default:
               break;
         }
         if (m_pItemType)
         {
            SetDlgItemText(IDC_TXT_HELP, m_pItemType->m_sHelp);
         }
         else
         {
            SetDlgItemText(IDC_TXT_HELP, _T(""));
         }
      }
      m_sCombo.ShowWindow(nShowCombo);
      ::ShowWindow(::GetDlgItem(m_hWnd, IDC_EDT_ITEM), nShowEdit);
      m_bCanDoChanges = true;
   }
   *pResult = 0;
}

void CXMLViewDlg::OnCbnSelchangeComboItem()
{
   if (m_bCanDoChanges)
   {
      m_bDocChanged = m_bChanged = true;
   }
}

void CXMLViewDlg::OnEnChangeEdtItem()
{
   if (m_bCanDoChanges)
   {
      m_bDocChanged = m_bChanged = true;
   }
}

void CXMLViewDlg::OnEnChangeEditName()
{
   if (m_bCanDoChanges)
   {
      m_bDocChanged = m_bNameChanged = m_bChanged = true;
   }
}

void CXMLViewDlg::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_hClicked != NULL);
}

void CXMLViewDlg::OnEditPaste()
{
   if (m_hClicked)
   {
      IDispatch*pNode   = (IDispatch*)m_tree.GetItemData(m_hClicked);
      HTREEITEM hParent = m_tree.GetNextItem(m_hClicked, TVGN_PARENT);
      if (hParent)
      {
         CXmlDocumentWrapper doc;
         IDispatch*pParent = (IDispatch*)m_tree.GetItemData(hParent);
         CString sXML;
         GetDlgItemText(IDC_EDT_XML, sXML);
         if (pParent && pNode && doc.LoadXML(sXML))
         {
            CXmlNodeWrapper refNode(pParent);
            pNode = refNode.InsertAfter((MSXML2::IXMLDOMNode*) pNode, doc.AsNode());
            ParseNode(pNode, hParent, m_hClicked);
            m_bDocChanged = true;
         }
      }
   }
}

void CXMLViewDlg::OnUpdateEditClear(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_hClicked != NULL);
}

void CXMLViewDlg::OnEditClear()
{
   if (m_hClicked)
   {
      IDispatch*pNode   = (IDispatch*)m_tree.GetItemData(m_hClicked);
      HTREEITEM hParent = m_tree.GetNextItem(m_hClicked, TVGN_PARENT);
      if (hParent)
      {
         IDispatch*pParent = (IDispatch*)m_tree.GetItemData(hParent);
         if (pParent && pNode)
         {
            CXmlNodeWrapper refNode(pParent);
            refNode.RemoveNode(pNode);
            m_tree.DeleteItem(m_hClicked);
            m_bDocChanged = true;
         }
      }
   }
}

void CXMLViewDlg::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_hClicked != NULL);
}

void CXMLViewDlg::OnEditCopy()
{
   if (m_hClicked)
   {
      IDispatch*pNode = (IDispatch*)m_tree.GetItemData(m_hClicked);
      if (pNode)
      {
         CXmlDocumentWrapper doc;
         doc.LoadXML(_T("<x></x>"));
         CXmlNodeWrapper node(doc.AsNode());
         node.InsertNode(0, pNode);
         CString sXML = doc.GetXML();
         sXML.Replace(_T("<x>"), _T(""));
         sXML.Replace(_T("</x>"), _T(""));
         SetDlgItemText(IDC_EDT_XML, sXML);
      }
   }
}

BOOL CXMLViewDlg::PreTranslateMessage(MSG* pMsg)
{
   if (m_hAccelerator)
   {
      int n = TranslateAccelerator(m_hWnd, m_hAccelerator, pMsg);
      if (n>0) return n;
   }

   return CDialog::PreTranslateMessage(pMsg);
}

BOOL CXMLViewDlg::OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
   // need to handle both ANSI and UNICODE versions of the message
   TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;
   CString strTipText;
   UINT nID = (UINT)pNMHDR->idFrom;
   if (pNMHDR->code == TTN_NEEDTEXT && (pTTT->uFlags & TTF_IDISHWND))
   {
      // idFrom is actually the HWND of the tool
      nID = (UINT)::GetDlgCtrlID((HWND)pNMHDR->idFrom);
   }

   if (nID != 0)
   {// will be zero on a separator

	  pTTT->lpszText = MAKEINTRESOURCE(nID);
      pTTT->hinst = AfxGetResourceHandle();
      return(TRUE);
   }

   _tcsncpy(pTTT->szText, strTipText, sizeof(pTTT->szText));

   *pResult = 0;

   return TRUE;    // message was handled
}

void CXMLViewDlg::OnSize(UINT nType, int cx, int cy)
{
   CDialog::OnSize(nType, cx, cy);
   if (cx && cy)
   {
      if (m_szWnd.cx == 0)
      {
         m_szWnd.cx = cx;
         m_szWnd.cy = cy;
      }
      if (m_szOld.cx != cx || m_szOld.cy != cy)
      {
         POSITION pos = m_WndRects.GetStartPosition();
         sWinRect*pWR, *pBind;
         WORD wID;
         CPoint pt(0,0);
         int i, n;
         while (pos)
         {
            m_WndRects.GetNextAssoc(pos, wID, (void*&)pWR);
            CRect rc(MulDiv(pWR->rcWnd.left  , cx, m_szWnd.cx),
                     MulDiv(pWR->rcWnd.top   , cy, m_szWnd.cy),
                     MulDiv(pWR->rcWnd.right , cx, m_szWnd.cx),
                     MulDiv(pWR->rcWnd.bottom, cy, m_szWnd.cy));
            n = pWR->eBind.GetCount();
            for (i=0; i<n; i++)
            {
               pBind = NULL;
               m_WndRects.Lookup((WORD)pWR->nIDRef[i], (void*&)pBind);
               if (pBind)
               {
                  switch ((pWR->eBind[i]&eSideMask))
                  {
                     case eLeft:   rc.left   = pWR->rcWnd.left;   break;
                     case eTop:    rc.top    = MulDiv(pBind->rcWnd.bottom, cy, m_szWnd.cy) + (pWR->rcWnd.top - pBind->rcWnd.bottom);    break;
                     case eRight:  rc.right  = cx - (m_szWnd.cx - pWR->rcWnd.right);  break;
                     case eBottom: rc.bottom = cy - (m_szWnd.cy - pWR->rcWnd.bottom); break;
                  }
               }
               else
               {
                  switch ((pWR->eBind[i]&eSideMask))
                  {
                     case eLeft:   rc.left   = pWR->rcWnd.left;   break;
                     case eTop:    rc.top    = pWR->rcWnd.top;    break;
                     case eRight:  rc.right  = cx - (m_szWnd.cx - pWR->rcWnd.right);  break;
                     case eBottom: rc.bottom = cy - (m_szWnd.cy - pWR->rcWnd.bottom); break;
                  }
               }
               if (pWR->eBind[i] & eNoStretchX)
               {
                  switch ((pWR->eBind[i]&eSideMask))
                  {
                     case eLeft:   rc.right = rc.left  + pWR->rcWnd.Width(); break;
                     case eRight:  rc.left  = rc.right - pWR->rcWnd.Width(); break;
                  }
               }
               if (pWR->eBind[i] & eNoStretchY)
               {
                  switch ((pWR->eBind[i]&eSideMask))
                  {
                     case eTop:    rc.bottom = rc.top    + pWR->rcWnd.Height(); break;
                     case eBottom: rc.top    = rc.bottom - pWR->rcWnd.Height(); break;
                  }
               }
            }
            GetDlgItem((UINT)wID)->MoveWindow(&rc);
         }
         m_szOld.cx = cx;
         m_szOld.cy = cy;
      }
   }
}
