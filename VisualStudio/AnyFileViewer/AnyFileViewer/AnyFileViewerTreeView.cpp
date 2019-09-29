// AnyFileViewerTreeView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "AnyFileViewer.h"
#include "MainFrm.h"
#include "AnyFileViewerDoc.h"
#include "AnyFileViewerView.h"
#include "AnyFileViewerTreeView.h"
#include "DisplayType.h"
#include "TreeCtrlEx.h"

#include "tree_item_iterator.h"

#include <algorithm>

// TODO! Ordner für Formate und für Datendateien getrennt merken.
// TODO! Neu Laden der Daten im XML-View als Befehl und nach dem Laden der Daten im Ansichtsmodus Dateiinhalt
// TODO! Rule: Regel zum extrahieren, selektieren, berechnen bzw. anzeigen von Daten in Strukturen
// TODO! Algorithmen auswerten für Daten in Strukturen.


// CAnyFileViewerTreeView
IMPLEMENT_DYNCREATE(CAnyFileViewerTreeView, CTreeView)

CAnyFileViewerTreeView::CAnyFileViewerTreeView() : mViewType(showFileContent)
, mDraggedItem(0)
, mRightDragged(FALSE)
, m_bInOnUpdate(FALSE)
, m_bShowInline(FALSE)
, mItemStructureHint(NULL)
, mTreeCtrl(NULL)
{

}

CAnyFileViewerTreeView::~CAnyFileViewerTreeView()
{
    delete mTreeCtrl;
}

BEGIN_MESSAGE_MAP(CAnyFileViewerTreeView, CTreeView)
    ON_COMMAND(ID_FORMAT_SAVE, OnFormatSave)
    ON_WM_CREATE()
    ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, &CAnyFileViewerTreeView::OnTvnBeginlabeledit)
    ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, &CAnyFileViewerTreeView::OnTvnEndlabeledit)
    ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, &CAnyFileViewerTreeView::OnUpdateEditClear)
    ON_COMMAND(ID_EDIT_CLEAR, &CAnyFileViewerTreeView::OnEditClear)
    ON_NOTIFY_REFLECT(TVN_BEGINRDRAG, &CAnyFileViewerTreeView::OnTvnBeginrdrag)
    ON_NOTIFY_REFLECT(TVN_BEGINDRAG, &CAnyFileViewerTreeView::OnTvnBegindrag)
    ON_COMMAND(ID_EDIT_INSERT_REPEAT, &CAnyFileViewerTreeView::OnEditInsertRepeat)
    ON_UPDATE_COMMAND_UI(ID_EDIT_INSERT_REPEAT, &CAnyFileViewerTreeView::OnUpdateEditInsertRepeat)
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_COMMAND_RANGE(ID_EDIT_COPY, ID_EDIT_COPY, &CAnyFileViewerTreeView::OnEditCopy)
    ON_COMMAND_RANGE(ID_EDIT_COPY_EXCEL, ID_EDIT_COPY_EXCEL, &CAnyFileViewerTreeView::OnEditCopy)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CAnyFileViewerTreeView::OnUpdateEditCopy)
    ON_COMMAND_EX(ID_EDIT_PASTE, &CAnyFileViewerTreeView::OnEditPaste)
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CAnyFileViewerTreeView::OnUpdateEditPaste)
    ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CAnyFileViewerTreeView::OnTvnSelchanged)
    ON_COMMAND(ID_EDIT_INSERT_INLINE_ATTR, &CAnyFileViewerTreeView::OnEditInsertInlineAttr)
    ON_UPDATE_COMMAND_UI(ID_EDIT_INSERT_INLINE_ATTR, &CAnyFileViewerTreeView::OnUpdateEditInsertInlineAttr)
    ON_COMMAND(ID_VIEW_UPDATE, &CAnyFileViewerTreeView::OnViewUpdate)
    ON_COMMAND(ID_TREEVIEW_INLINE_STRUCTURE, &CAnyFileViewerTreeView::OnTreeviewInlineStructure)
    ON_UPDATE_COMMAND_UI(ID_TREEVIEW_INLINE_STRUCTURE, &CAnyFileViewerTreeView::OnUpdateTreeviewInlineStructure)
    ON_WM_RBUTTONUP()
    ON_NOTIFY_REFLECT(NM_RCLICK, &CAnyFileViewerTreeView::OnNMRClick)
    ON_WM_MOUSEWHEEL()
    ON_WM_LBUTTONDBLCLK()
    ON_NOTIFY_REFLECT(TVN_KEYDOWN, OnKeyDown)
    ON_COMMAND(ID_EDIT_INSERT_FACTOR_ATTR, &CAnyFileViewerTreeView::OnEditInsertFactorAttr)
    ON_UPDATE_COMMAND_UI(ID_EDIT_INSERT_FACTOR_ATTR, &CAnyFileViewerTreeView::OnUpdateEditInsertFactorAttr)
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CAnyFileViewerTreeView-Diagnose

#ifdef _DEBUG
void CAnyFileViewerTreeView::AssertValid() const
{
    CTreeView::AssertValid();
}

#ifndef _WIN32_WCE
void CAnyFileViewerTreeView::Dump(CDumpContext& dc) const
{
    CTreeView::Dump(dc);
}
#endif

CAnyFileViewerDoc* CAnyFileViewerTreeView::GetDocument() // Nicht-Debugversion ist inline
{
   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAnyFileViewerDoc)));
   return (CAnyFileViewerDoc*)m_pDocument;
}
#endif //_DEBUG

const LPCTSTR CAnyFileViewerTreeView::Attr::getNameOfType(CAnyFileViewerTreeView::Attr::eAttribute aType)
{
    switch (aType) {
        NAME_OF_TYPE(type);
        NAME_OF_TYPE(repeat);
        NAME_OF_TYPE(name);
        NAME_OF_TYPE(count);
        NAME_OF_TYPE(bytes);
        NAME_OF_TYPE(factor);
        NAME_OF_TYPE(rule);
        case in_line: return _T("inline");
        default: NAME_OF_TYPE(Unknown);
    }
}

CAnyFileViewerTreeView::Attr::eAttribute CAnyFileViewerTreeView::Attr::getTypeOfName(const CString& aName)
{
    eAttribute fType;
    static CMap<CString, LPCTSTR, eAttribute, eAttribute&> fTypes;
    if (fTypes.GetCount() == 0) 
    {
        for (int i=0; i<Last; i++) 
        {
            fType = static_cast<eAttribute>(i);
            fTypes.SetAt( getNameOfType(fType), fType);
        }
    }
    if (!fTypes.Lookup(aName, fType))
    {
        fType = Unknown;
    }
    return fType;
}

const LPCTSTR CAnyFileViewerTreeView::Item::getNameOfType(CAnyFileViewerTreeView::Item::eItem aType)
{
    switch (aType) 
    {
        NAME_OF_TYPE(Structure);
        NAME_OF_TYPE(Data);
        default: return _T("");
    }
}

CAnyFileViewerTreeView::Item::eItem CAnyFileViewerTreeView::Item::getTypeOfName(const CString& aName)
{
    eItem fType;
    static CMap<CString, LPCTSTR, eItem, eItem&> fTypes;
    if (fTypes.GetCount() == 0) 
    {
        for (int i=0; i<Last; i++) 
        {
            fType = static_cast<eItem>(i);
            fTypes.SetAt( getNameOfType(fType), fType);
        }
    }
    if (!fTypes.Lookup(aName, fType))
    {
        fType = Unknown;
    }
    return fType;
}


void CAnyFileViewerTreeView::OnFormatSave()
{
    CString str;
    str.LoadString(IDS_STRUCTURE_FILE_DLG);
    CFileDialog dlg(FALSE, _T("xml"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, str, this);
    dlg.m_ofn.lpstrInitialDir = theApp.mFormatFilesDir;
    if (dlg.DoModal() == IDOK)
    {
        theApp.mFormatFilesDir = dlg.GetPathName();
        int n = theApp.mFormatFilesDir.ReverseFind(_T('\\'));
        if (n != -1)
        {
            theApp.mFormatFilesDir = theApp.mFormatFilesDir.Left(n+1);
        }
        m_xmlDoc.Save(dlg.GetPathName());
    }
}

void CAnyFileViewerTreeView::OnUpdateFormatSave(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(1);
}

void CAnyFileViewerTreeView::LoadFormatFile(LPCTSTR sFile)
{
   if (m_xmlDoc.Load(sFile))
   {
      theApp.mFormatFilesDir = sFile;
      int n = theApp.mFormatFilesDir.ReverseFind(_T('\\'));
      if (n != -1)
      {
         theApp.mFormatFilesDir = theApp.mFormatFilesDir.Left(n+1);
      }
      if (mViewType == showFileStructure)
      {
          OnTreeviewFileStructure();
      }
      else
      {
          OnTreeviewFileContent();
      }
   }
}
/// \brief recurses through the xml elements and inserts them into the tree view
/// \param aNode XML Node to be inserted
/// \param aParent parent tree view item for new tree view item
/// \param hInsertAfter position of the new item
HTREEITEM CAnyFileViewerTreeView::RecurseXMLStructure(IDispatch *aNode, HTREEITEM aParent, HTREEITEM hInsertAfter)
{
    static const CString Element = _T("element");
    static const CString Text    = _T("text");
    CXmlNodeWrapper fXmlNode(aNode);
    CString fNodeType = fXmlNode.NodeType();
    CTreeCtrl&theTree = GetTreeCtrl();
    if (fNodeType == Text)
    {
        return NULL;
    }
    int fImage    = CDisplayType::Structure;
    int fSelected = CDisplayType::Structure;
    CString fName = fXmlNode.Name();
    CString fText;
    if (Item::getTypeOfName(fName) == Item::Data)
    {
         fText = fXmlNode.GetText();
    }

    for (int i = 0; i < fXmlNode.NumAttributes(); ++i)
    {
        switch(Attr::getTypeOfName(fXmlNode.GetAttribName(i)))
        {
        case Attr::type:
            fImage = fSelected = CDisplayType::getTypeOfName(fXmlNode.GetAttribVal(i));
            break;
        case Attr::name:
            fText = fXmlNode.GetAttribVal(i);
            break;
        }
    }

    if (aNode != NULL)
    {
        HTREEITEM fItem = theTree.InsertItem(fName + ": " + fText, fImage, fSelected, aParent, hInsertAfter);
        theTree.SetItemData(fItem, (DWORD_PTR)aNode);
        UpdateXmlAttributes(fItem, fXmlNode);
        for (int i = 0; i < fXmlNode.NumNodes(); ++i)
        {
            RecurseXMLStructure(fXmlNode.GetNode(i), fItem);
        }
        return fItem;
    }
    return NULL;
}
/// \brief update the attributes of an xml item shown in the tree view
/// \param aItem tree view item
/// \param aNode XML node with attribute information
void CAnyFileViewerTreeView::UpdateXmlAttributes(const HTREEITEM aItem, CXmlNodeWrapper&aNode)
{
    CTreeCtrl& theTree = GetTreeCtrl();
    HTREEITEM hDelete, hChild = theTree.GetNextItem(aItem, TVGN_CHILD);
    int fImage, fSelected;
    while (hChild)
    {
        hDelete = 0;
        theTree.GetItemImage(hChild, fImage, fSelected);
        switch (fImage)
        {
        case CDisplayType::RepeatAttribute:
        case CDisplayType::BytesAttribute:
        case CDisplayType::InLineAttribute:
        case CDisplayType::FactorAttribute:
            hDelete = hChild;
            break;
        }
        hChild = theTree.GetNextItem(hChild, TVGN_NEXT);
        if (hDelete)
        {
            theTree.DeleteItem(hDelete);
        }
    }
    for (int i = 0; i < aNode.NumAttributes(); ++i)
    {
        CString sAttrName = aNode.GetAttribName(i);
        switch (Attr::getTypeOfName(sAttrName))
        {
        case Attr::count:
        case Attr::repeat:
            theTree.InsertItem(sAttrName + _T(": ") + aNode.GetAttribVal(i), CDisplayType::RepeatAttribute, CDisplayType::RepeatAttribute, aItem);
            break;
        case Attr::bytes:
            theTree.InsertItem(sAttrName + _T(": ") + aNode.GetAttribVal(i), CDisplayType::BytesAttribute, CDisplayType::BytesAttribute, aItem);
            break;
        case Attr::in_line:
            theTree.InsertItem(sAttrName + _T(": ") + aNode.GetAttribVal(i), CDisplayType::InLineAttribute, CDisplayType::InLineAttribute, aItem);
            break;
        case Attr::factor:
            theTree.InsertItem(sAttrName + _T(": ") + aNode.GetAttribVal(i), CDisplayType::FactorAttribute, CDisplayType::FactorAttribute, aItem);
            break;
        }
    }
}

/// \brief recurses through the xml elements and inserts the data values described by the xml structure
/// \param aNode XML Node containing data value decription
/// \param aParent parent tree view item for new tree view item
///     \hint if aParent is 0 only the current position is modified
/// \param aPos current data position
///     \hint the current data position is updated after insertion for data values
/// \param [aTxt] optional text variable to be filled for clipboard
void CAnyFileViewerTreeView::RecurseContent(IDispatch *aNode, HTREEITEM aParent, size_t& aPos, CString*aTxt)
{
    BYTE*  fData = GetDocument()->GetData();
    size_t fSize = GetDocument()->GetSize(); 
    if (fData != 0 && aPos < fSize)
    {
        CTreeCtrl&theTree = GetTreeCtrl();
        CXmlNodeWrapper fXmlNode(aNode);
        int img = CDisplayType::Structure, imgSel = CDisplayType::Structure; 
        CString str;
        Item::eItem         fItem    = Item::getTypeOfName(fXmlNode.Name());
        CString             fName    = fXmlNode.GetText();
        CDisplayType::eType fType    = CDisplayType::Unknown;
        size_t              fRepeat  = 1;
        int                 fBytes   = 0;
        BOOL                bCounter = FALSE;
        BOOL                bInline  = FALSE;
        double              fFactor = 0.0;
        for (int iAttr = 0; iAttr < fXmlNode.NumAttributes(); ++iAttr)
        {
            Attr::eAttribute fAttribute = Attr::getTypeOfName(fXmlNode.GetAttribName(iAttr));
            switch(fAttribute)
            {
            case Attr::type:
                img = imgSel = fType = CDisplayType::getTypeOfName(fXmlNode.GetAttribVal(iAttr));
                break;
            case Attr::count:
                bCounter = TRUE;
                break;
            case Attr::in_line:
                bInline = m_bShowInline;
                break;
            case Attr::factor:
                fFactor = _ttof(fXmlNode.GetAttribVal(iAttr));
                break;
            case Attr::repeat:
            case Attr::bytes:
            {
                CString fAttrValue = fXmlNode.GetAttribVal(iAttr);
                size_t fValue = (fAttribute == Attr::repeat) ? fRepeat : fBytes;
                if (fAttrValue.GetLength())
                {
                    if (isdigit(fAttrValue[0]))
                    {
                        fValue = _ttoi(fAttrValue);
                    }
                    else
                    {
                        if (!mCounters.Lookup(fAttrValue, fValue))
                        {
                            CMessageHint fMessage;
                            fMessage.mMessage.LoadString(IDS_ERROR_NO_COUNTER_AVAILABLE);
                            fMessage.mName = fName;
                            fMessage.mPosition = aPos;
                            GetDocument()->UpdateAllViews(this, UPDATE_INSERT_MESSAGE_VIEW, &fMessage);
                        }
                    }
                }
                if (fAttribute == Attr::repeat) fRepeat = fValue;
                else                       fBytes  = static_cast<int>(fValue);
            }  break;
            case Attr::name:
                fName = fXmlNode.GetAttribVal(iAttr);
                break;
            }
        }
        if (fItem == Item::Structure)
        {
            CString fNumber;
            CAnyFileViewerApp::eFormat fOldFmt = theApp.getCurrentFormat();
            bool bExcel = fOldFmt == CAnyFileViewerApp::Excel;
            for (size_t i=0; i < fRepeat && aPos < fSize; ++i)
            {
                if (fRepeat > 1)
                {
                    fNumber.Format(_T("[%d]"), i);
                }
                CString fInlineText;
                if (bInline || bExcel)
                {
                    size_t fOldPos = aPos;
                    if (!bExcel)
                    {
                        theApp.setCurrentFormat(CAnyFileViewerApp::InlineItems);
                        fInlineText += _T(" { ");
                    }
                    for (int fNode = 0; fNode < fXmlNode.NumNodes(); ++fNode)
                    {
                        RecurseContent(fXmlNode.GetNode(fNode), NULL, aPos, &fInlineText);
                    }
                    if (bExcel)
                    {
                        if (fRepeat > 1)
                        {
                            fInlineText.Replace(theApp.getSeparator(), _T("\t"));
                            fInlineText += _T("\r\n");
                        }
                    }
                    else
                    {
                        fInlineText += _T(" }");
                    }
                    theApp.setCurrentFormat(fOldFmt);
                    if (fOldPos == aPos)
                    {
                        break; // error no position progress leads to infinite recursion
                    }
                }
                HTREEITEM hItem = NULL;
                if (mItemStructureHint)
                {
                    mItemStructureHint->mItems.push_back(CStructureItem(static_cast<unsigned int>(aPos), static_cast<char>(img), fName + fNumber + fInlineText));
                }
                else if (aParent)
                {
                    hItem = theTree.InsertItem(fName + fNumber + fInlineText, img, imgSel, aParent);
                    mContentData.push_back(ContentData(aPos, aNode));
                    theTree.SetItemData(hItem, mContentData.size() - 1);
                }
                else if (aTxt)
                {
                    *aTxt += fInlineText;
                }
                if (bInline || bExcel)
                {
                    continue;
                }
                size_t fOldPos = aPos;
                for (int fNode = 0; fNode < fXmlNode.NumNodes(); ++fNode)
                {
                    RecurseContent(fXmlNode.GetNode(fNode), hItem, aPos, aTxt);
                }
                if (fOldPos == aPos)
                {
                    break; // error no position progress leads to infinite recursion
                }
            }
        }
        else
        {
            CDisplayType& fDisplay = theApp.GetDisplayType(fType);
            if (fBytes == 0)
            {
                fBytes = fDisplay.GetByteLength();
            }
            else
            {
                fDisplay.SetBytes(fBytes);
            }
            if (bCounter && aTxt == 0)
            {
                size_t fValue = 1;
                if (mCounters.Lookup(fName, fValue))
                {
                    CMessageHint fMessage;
                    fMessage.mMessage.LoadString(IDS_ERROR_DOUBLE_COUNTER_ENTRY);
                    fMessage.mName = fName;
                    fMessage.mPosition = aPos;
                    GetDocument()->UpdateAllViews(this, UPDATE_INSERT_MESSAGE_VIEW, &fMessage);
                }
                mCounters.SetAt(fName, _ttoi(fDisplay.Display(&fData[aPos])));
            }
            CString fSeparator = theApp.getSeparator();
            CString fNumber;
            for (size_t i=0; (i < fRepeat && (aPos+fBytes) < fSize); ++i, aPos += fBytes)
            {
                CString fValue;
                if (fFactor != 0)
                {
                    double fDoubleValue = _ttof(fDisplay.Display(&fData[aPos])) * fFactor;
                    fValue.Format(_T("%f"), fDoubleValue);
                }
                else
                {
                    fValue = fDisplay.Display(&fData[aPos]);
                }
                if (fRepeat > 1)
                {
                    fNumber.Format(_T("[%d]"), i);
                }
                if (aTxt)   /// an optional text variable
                {           /// is filled with text for the clipboard
                    if (theApp.CopyName())
                    {
                        (*aTxt) += fName + fNumber;
                        (*aTxt) += _T(": ");
                    }
                    if (theApp.CopyType())
                    {
                        (*aTxt) += fDisplay.Type();
                        (*aTxt) += _T(": ");
                    }
                    (*aTxt) += fValue;
                    (*aTxt) += fSeparator;
                }
                if (aParent)
                {
                    str = fName + fNumber + _T(": ") + fValue;
                    HTREEITEM fInserted = theTree.InsertItem(str, img, imgSel, aParent);
                    mContentData.push_back(ContentData(aPos, aNode));
                    theTree.SetItemData(fInserted, mContentData.size() - 1);
                }
                else if (mItemStructureHint)
                {
                    mItemStructureHint->mItems.push_back(CStructureItem(static_cast<unsigned int>(aPos), static_cast<char>(img), fName + fNumber));
                }
            }
        }
    }
}

void CAnyFileViewerTreeView::FindDataPositionOfItem(HTREEITEM aItemToFind, size_t &aPos, HTREEITEM aParent)
{
    CTreeCtrl&theTree= GetTreeCtrl();
    if (aParent != TVI_ROOT)
    {
        int nImage=0, nSelected=0;
        theTree.GetItemImage(aParent, nImage, nSelected);
        size_t fByteLen = theApp.GetDisplayType(nImage).GetByteLength();
        TRACE(_T("%s: %d\n"), LPCTSTR(theTree.GetItemText(aParent)), aPos);
        IDispatch*fPtr = (IDispatch*)theTree.GetItemData(aParent);
        if (fPtr)
        {
            CXmlNodeWrapper fXmlNode(fPtr);
            CString sAttr = fXmlNode.GetValue(Attr::getNameOfType(Attr::bytes));
            if (sAttr.GetLength())
            {
                size_t fBytes = 1;
                if (isdigit(sAttr[0]))
                {
                    fBytes = _ttoi(LPCTSTR(sAttr));
                }
                else
                {
                    mCounters.Lookup(sAttr, fBytes);
                }
                fByteLen = fBytes;
            }
            sAttr = fXmlNode.GetValue(Attr::getNameOfType(Attr::repeat));
            if (sAttr.GetLength())
            {
                size_t fRepeat = 1;
                if (isdigit(sAttr[0]))
                {
                    fRepeat = _ttoi(LPCTSTR(sAttr));
                }
                else
                {
                    mCounters.Lookup(sAttr, fRepeat);
                }
                fByteLen *= fRepeat;
            }
            sAttr = fXmlNode.GetValue(Attr::getNameOfType(Attr::count));
            if (sAttr.GetLength())
            {
                BYTE*  fData = GetDocument()->GetData();
                size_t fSize = GetDocument()->GetSize(); 
                if (fData != 0 && aPos < fSize)
                {
                    mCounters.SetAt(fXmlNode.GetText(), _ttoi(theApp.GetDisplayType(nImage).Display(&fData[aPos])));
                }
            }
        }
        aPos += fByteLen;
        if (aItemToFind == aParent) return;
    }
    HTREEITEM hChild = theTree.GetNextItem(aParent, TVGN_CHILD);
    while (hChild)
    {
        if (aItemToFind == hChild) return ;
        FindDataPositionOfItem(aItemToFind, aPos, hChild);
        hChild = theTree.GetNextItem(hChild, TVGN_NEXT);
    }
    return ;
}

void CAnyFileViewerTreeView::OnTreeviewFileStructure()
{
    mViewType = showFileStructure;
    GetTreeCtrl().DeleteAllItems();
    RecurseXMLStructure(m_xmlDoc.AsNode(), TVI_ROOT);
}


void CAnyFileViewerTreeView::OnTreeviewFileContent()
{
   mViewType = showFileContent;
   GetTreeCtrl().DeleteAllItems();
   mContentData.clear();
   mCounters.RemoveAll();
   GetDocument()->UpdateAllViews(this, UPDATE_CLEAR_MESSAGE_VIEW);
   size_t fPos = 0;
   RecurseContent(m_xmlDoc.AsNode(), TVI_ROOT, fPos);
}


BOOL CAnyFileViewerTreeView::PreCreateWindow(CREATESTRUCT& cs)
{
   cs.style |= (TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_EDITLABELS );
   cs.style &= ~TVS_DISABLEDRAGDROP;
   return CTreeView::PreCreateWindow(cs);
}

int CAnyFileViewerTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    lpCreateStruct->style |= TVS_SHOWSELALWAYS;
    if (CTreeView::OnCreate(lpCreateStruct) == -1)
        return -1;

    mTreeImages.Create(IDR_TREE_IMAGES, 16, 1, RGB(192,192,192));
    GetTreeCtrl().SetImageList(&mTreeImages, TVSIL_NORMAL);
    GetTreeCtrl().DeleteAllItems();

#ifdef _DEBUG
 //   LoadFormatFile(_T(".\\testformats\\CurveFormat.xml"));
    //RecurseXMLStructure(m_xmlDoc.AsNode(), TVI_ROOT);
 //   mViewType = showFileStructure;
#endif

    return 0;
}

void CAnyFileViewerTreeView::OnInitialUpdate()
{
    CTreeView::OnInitialUpdate();
    mTreeCtrl = new CTreeCtrlEx;
    mTreeCtrl->m_hWnd = CTreeView::GetTreeCtrl().m_hWnd;
    CComboBox& fCombo = mTreeCtrl->InsertComboBox(TVHT_ONITEMICON);
    CWnd *pWnd = AfxGetMainWnd();
    pWnd = &((CMainFrame *)pWnd)->GeFormatViewDlgBar();
    int n = static_cast<int>(pWnd->SendDlgItemMessage(IDC_COMBO_DATA_TYPE_INSERT, CB_GETCOUNT, 0));
    TCHAR szText[64];
    for (int i = 0; i < n; ++i)
    {
        pWnd->SendDlgItemMessage(IDC_COMBO_DATA_TYPE_INSERT, CB_GETLBTEXT, i, (LPARAM)&szText);
        int nItem = static_cast<int>(pWnd->SendDlgItemMessage(IDC_COMBO_DATA_TYPE_INSERT, CB_GETITEMDATA, i, 0));
        fCombo.AddString(szText);
        fCombo.SetItemData(i, nItem);
        TRACE("Init(%d): %s: %d\n", i, szText, nItem);
    }
}

void CAnyFileViewerTreeView::OnInsertDataType(CString &aName, int aDataType, int aLen, int aInsertPos)
{
    if (mViewType == showFileStructure)
    {
        MSXML2::IXMLDOMNode *fNode = m_xmlDoc.AsNode();
        CTreeCtrl&theTree          = GetTreeCtrl();
        HTREEITEM fSelectedItem    = theTree.GetSelectedItem();
        HTREEITEM fParentItem      = theTree.GetParentItem(fSelectedItem);
        if (fSelectedItem != 0 )
        {
            fNode = reinterpret_cast<MSXML2::IXMLDOMNode*>(theTree.GetItemData(fSelectedItem));
            if (fNode == 0) return;
        }
        else
        {
            CString fStartNode = _T("<?xml version=\"1.0\" encoding=\"utf-8\"?><Structure name=\"new file structure\"></Structure>");
            m_xmlDoc.LoadXML(fStartNode);
            fNode = m_xmlDoc.AsNode();
            RecurseXMLStructure(fNode, TVI_ROOT);
            aInsertPos = ip::child;
            fSelectedItem = theTree.GetNextItem(TVI_ROOT, TVGN_CHILD);
        }

        CString sNodeName = Item::getNameOfType(aDataType == CDisplayType::Structure ? Item::Structure : Item::Data);
        IDispatch *fNewNode = 0;
        CXmlNodeWrapper fXmlNode(fNode);
        CXmlNodeWrapper fXmlParent(fXmlNode.Parent());
        int fInsertIndex = 0; 
        if (fParentItem == 0)
        {
            if (aInsertPos == ip::after)
            {
                fInsertIndex = -1;
            }
            aInsertPos = ip::child;
        }
        switch (aInsertPos)
        {
        case ip::before:
            fNewNode = fXmlParent.InsertBefore(fNode, sNodeName); 
            break;
        case ip::after:
            fNewNode = fXmlParent.InsertAfter(fNode, sNodeName); 
            break;
        case ip::child:
            fNewNode = fXmlNode.InsertNode(fInsertIndex, sNodeName);
            fParentItem   = fSelectedItem;
            fSelectedItem = TVI_LAST;
            break;
        }        

        CXmlNodeWrapper fNewXmlNode(fNewNode);
        fNewXmlNode.SetValue(Attr::getNameOfType(Attr::type), CDisplayType::getNameOfType(static_cast<CDisplayType::eType>(aDataType)));
        if (aDataType == CDisplayType::Structure)
        {
            fNewXmlNode.SetValue(Attr::getNameOfType(Attr::name), aName);
        }
        else
        {
            fNewXmlNode.SetText(aName);
        }
        if (aLen > 0)
        {
            fNewXmlNode.SetValue(Attr::getNameOfType(Attr::bytes), aLen);
        }
        HTREEITEM fItem = RecurseXMLStructure(fNewNode, fParentItem, fSelectedItem);
        if (fItem)
        {
            theTree.SelectItem(fItem);
            theTree.EnsureVisible(fItem);
            theTree.Expand(fItem, TVE_EXPAND);
            GetParentFrame()->SetActiveView(this);
        }
    }
}

BOOL CAnyFileViewerTreeView::isStructureView()
{
    return mViewType == showFileStructure;
}

void CAnyFileViewerTreeView::OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
    *pResult = 1;
    if (pTVDispInfo->item.mask & TVIF_TEXT)
    {
        *pResult = 0;
         int fImage(0), fSelected(0);
        CString fText(pTVDispInfo->item.pszText);
        CTreeCtrl&theTree = GetTreeCtrl();
        theTree.GetItemImage(pTVDispInfo->item.hItem, fImage, fSelected);
        if (isStructureView())  // edit the names of the file structure elements
        {
            int fPos = fText.Find(_T(": "));
            if (fPos != -1) fText = fText.Mid(fPos+2);

            if ((   fImage == CDisplayType::RepeatAttribute
                 || fImage == CDisplayType::BytesAttribute
                 || fImage == CDisplayType::InLineAttribute
                 || fImage == CDisplayType::FactorAttribute)
                 && fText.GetLength())
            {
                // repeat or byte counting numbers can be changed
                if (!isdigit(fText[0])) // variable names for repeat, factor, count or bytes cannot be edited
                {
                    *pResult = 1; 
                }
            }
            IDispatch*fPtr = (IDispatch*)theTree.GetItemData(pTVDispInfo->item.hItem);
            if (fPtr)
            {
                CXmlNodeWrapper fXmlNode(fPtr);
                CString sAttr = fXmlNode.GetValue(Attr::getNameOfType(Attr::count));
                if (sAttr.GetLength() != 0)
                {
                    *pResult = 1; 
                }
            }
        }
        else    // edit the values of the file content
        {
            if (fImage == CDisplayType::Structure)
            {
                *pResult = 1;   // structure is not edited in file content view
            }
            else                // edit only values
            {
                int fPos = fText.Find(_T(": "));
                if (fPos != -1) fText = fText.Mid(fPos+2);
            }
        }
        theTree.GetEditControl()->SetWindowText(fText);
    }
}

void CAnyFileViewerTreeView::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
    if (pTVDispInfo->item.mask & TVIF_TEXT && pTVDispInfo->item.pszText)
    {
        CString fText(pTVDispInfo->item.pszText);
        int fImage(0), fSelected(0);
        CTreeCtrl&theTree = GetTreeCtrl();
        theTree.GetItemImage(pTVDispInfo->item.hItem, fImage, fSelected);
        CString fItem = theTree.GetItemText(pTVDispInfo->item.hItem);
        int pos = fItem.Find(_T(": "));
        if (pos != -1)  // combine the item name with the value
        {
            fItem = fItem.Left(pos+2) + fText;
        }

        if (isStructureView())  // in structure view
        {
            BOOL fOk = FALSE;
            switch (fImage) // set attributes
            {
                case CDisplayType::RepeatAttribute:
                case CDisplayType::BytesAttribute:
                case CDisplayType::FactorAttribute:
                case CDisplayType::InLineAttribute:
                if (fText.GetLength() && isdigit(fText[0]))
                {
                    IDispatch*fPtr = (IDispatch*)theTree.GetItemData(theTree.GetParentItem(pTVDispInfo->item.hItem));
                    if (fPtr)
                    {
                        CXmlNodeWrapper fXmlNode(fPtr);
                        Attr::eAttribute fAttr = Attr::Unknown;
                        switch (fImage)
                        {
                        case CDisplayType::RepeatAttribute: fAttr = Attr::repeat; break;
                        case CDisplayType::BytesAttribute: fAttr = Attr::bytes; break;
                        case CDisplayType::FactorAttribute: fAttr = Attr::factor; break;
                        case CDisplayType::InLineAttribute: fAttr = Attr::in_line; break;
                        }
                        fXmlNode.SetValue(Attr::getNameOfType(fAttr), fText);
                        fOk = TRUE;
                    } 
                } break;
                default:
                {
                    IDispatch*fPtr = (IDispatch*)theTree.GetItemData(pTVDispInfo->item.hItem);
                    if (fPtr)   // or names
                    {
                        CXmlNodeWrapper fXmlNode(fPtr);
                        if (fImage == CDisplayType::Structure)
                        {
                            fXmlNode.SetValue(Attr::getNameOfType(Attr::name), fText);
                        }
                        else
                        {
                            fXmlNode.SetText(fText);
                        }
                        fOk = TRUE;
                    }
                } break;
            }
            if (fOk)
            {
                theTree.SetItemText(pTVDispInfo->item.hItem, fItem);
            }
        }
        else    // in data view
        {
            if (fImage == CDisplayType::Structure)
            {
                ASSERT(FALSE);
            }
            else
            {
                BYTE*fData  = GetDocument()->GetData();
                UINT fIndex = static_cast<UINT>(theTree.GetItemData(pTVDispInfo->item.hItem));
                if (fIndex < mContentData.size())
                {
                    size_t fPos = mContentData[fIndex].mPos;
                    if (fData && fPos < GetDocument()->GetSize()) //change document content
                    {
                        CDisplayType& fElement = theApp.GetDisplayType(fImage);
                        // determine whether the data type is variable:
                        if (fElement.isSizeFixed() == false)
                        {
                            //   determine the length of the old data
                            int fOldSize = fElement.GetByteLength();
                            //   determine the length of the new data: use Write function returning copied bytes
                            int fNewSize = fElement.Write(NULL, fText);
                            if (fOldSize != fNewSize)
                            {
                                // TODO: handle variable types
                                // change the documents data size, if necessary
                                // move the data behind the insertion position, if necessary
                                // change the according variable containing the length of the variable type
                            }
                        }
                        if (fElement.Write(&fData[fPos], fText))
                        {   
                            CHint fHint(fPos);
                            GetDocument()->UpdateAllViews(this, UPDATE_DATA_AT_POS, &fHint);
                            GetDocument()->SetModifiedFlag(TRUE);
                            theTree.SetItemText(pTVDispInfo->item.hItem, fItem);
                        }
                    }
                }
            }
        }
        GetTreeCtrl().GetEditControl()->SetWindowText(fText);
    }
    *pResult = 0;
}

int testfunc(tree_item_iterator& value)
{
    return value.getLevel() > 1 ? 1:0;
}
int testfunc2(tree_item_iterator& value1, tree_item_iterator& value2)
{
    return value1.getLevel() < value2.getLevel() ? 1:0;
}

void CAnyFileViewerTreeView::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
    //std::for_each(iter, tree_item_iterator(GetTreeCtrl(), 0), testfunc);
    //tree_item_iterator found = std::find_if(iter, tree_item_iterator(GetTreeCtrl(), 0), testfunc);
    //found = std::max_element(iter, tree_item_iterator(GetTreeCtrl(), 0), testfunc2);
    //found = std::min_element(iter, tree_item_iterator(GetTreeCtrl(), 0), testfunc2);
    //tree_item_iterator iter(GetTreeCtrl(), TVI_ROOT);
    //int n = std::count_if(iter, tree_item_iterator(GetTreeCtrl(), 0), testfunc);
    //CString s;
    //for (++iter; *iter != 0; ++iter)
    //{
    //    s = iter.getCtrl().GetItemText(*iter);
    //    TRACE(_T("%d: %s\n"), iter.getLevel(), LPCTSTR(s));
    //}
    //TRACE(_T("\n"));
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    if (pNMTreeView->itemNew.hItem)
    {
        if (isStructureView())
        {
            size_t fPos = 0;
            FindDataPositionOfItem(pNMTreeView->itemNew.hItem, fPos);
            CHint fHint(fPos);
            GetDocument()->UpdateAllViews(this, UPDATE_DATA_POS, &fHint);
        }
        else
        {
            if (pNMTreeView->itemNew.lParam < static_cast<LPARAM>(mContentData.size()))
            {
                CHint fHint(mContentData[pNMTreeView->itemNew.lParam].mPos);
                GetDocument()->UpdateAllViews(this, UPDATE_DATA_POS, &fHint);
            }
        }
    }
    *pResult = 0;
}

void CAnyFileViewerTreeView::OnUpdateEditClear(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(GetTreeCtrl().GetSelectedItem() != NULL && mViewType == showFileStructure);
}

void CAnyFileViewerTreeView::OnEditClear()
{
    CTreeCtrl&theTree = GetTreeCtrl();
    HTREEITEM hItem   = theTree.GetSelectedItem();
    if (hItem)
    {
        IDispatch*fNode = (IDispatch*)theTree.GetItemData(hItem);
        if (fNode)
        {
            CXmlNodeWrapper fXmlNode(fNode);
            CXmlNodeWrapper fXmlParent(fXmlNode.Parent());
            fXmlParent.RemoveNode(fXmlNode.Detach());
            theTree.DeleteItem(hItem);
        }
        else
        {
            IDispatch*fParentNode = (IDispatch*)theTree.GetItemData(theTree.GetParentItem(hItem));
            if (fParentNode)
            {
                CXmlNodeWrapper fXmlNode(fParentNode);
                int fImage, fSelected;
                if (theTree.GetItemImage(hItem, fImage, fSelected))
                {
                    switch (fImage)
                    {
                    case CDisplayType::RepeatAttribute:
                        fXmlNode.DeleteValue(Attr::getNameOfType(Attr::repeat));
                        break;
                    case CDisplayType::FactorAttribute:
                        fXmlNode.DeleteValue(Attr::getNameOfType(Attr::factor));
                        break;
                    case CDisplayType::InLineAttribute:
                        fXmlNode.DeleteValue(Attr::getNameOfType(Attr::in_line));
                        break;
                    case CDisplayType::BytesAttribute:
                        fXmlNode.DeleteValue(Attr::getNameOfType(Attr::bytes));
                        break;
                        // TODO: perform a consistency check and remove according entries
                    }
                }
                theTree.DeleteItem(hItem);
            }
        }
    }
}

void CAnyFileViewerTreeView::OnTvnBeginrdrag(NMHDR *pNMHDR, LRESULT *pResult)
{
    mRightDragged = TRUE;
    OnTvnBegindrag(pNMHDR, pResult);
}

void CAnyFileViewerTreeView::OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    CTreeCtrl&theTree = GetTreeCtrl();
    int fImage=0, fSelected = 0;
    theTree.GetItemImage(pNMTreeView->itemNew.hItem, fImage, fSelected);
    switch(fImage)
    {
    case CDisplayType::Char:     case CDisplayType::UChar:
    case CDisplayType::Short:    case CDisplayType::UShort:
    case CDisplayType::Long:     case CDisplayType::ULong:
    case CDisplayType::LongLong: case CDisplayType::ULongLong:
        mDraggedItem = pNMTreeView->itemNew.hItem;
        break;
    }
    if (mDraggedItem)
    {
        mTreeImages.BeginDrag(fImage, CPoint(16, 16));
        CPoint pt = pNMTreeView->ptDrag;
        ScreenToClient(&pt);
        mTreeImages.DragEnter(this,  pt);
        theTree.SelectItem(mDraggedItem);
    }
    *pResult = 0;
}

void CAnyFileViewerTreeView::OnMouseMove(UINT nFlags, CPoint point)
{
    if (mDraggedItem)
    {
        mTreeImages.DragMove(point);
    }
    CTreeView::OnMouseMove(nFlags, point);
}

void CAnyFileViewerTreeView::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (mDraggedItem)
    {
        mTreeImages.DragLeave(this);
        mTreeImages.EndDrag();
        CTreeCtrl&theTree      = GetTreeCtrl();
        HTREEITEM hDropItem    = theTree.HitTest(point);
        IDispatch*fDropNode    = (IDispatch*)theTree.GetItemData(hDropItem);
        IDispatch*fDraggedNode = (IDispatch*)theTree.GetItemData(mDraggedItem);
        if (fDropNode && fDraggedNode)
        {
            if (mRightDragged)
            {
                // TODO: find a reason for right dragged items
                // e.g. move items in tree view
            }
            else
            {
                CXmlNodeWrapper fXmlDropNode(fDropNode);
                CXmlNodeWrapper fXmlDraggedNode(fDraggedNode);
                fXmlDraggedNode.SetValue(Attr::getNameOfType(Attr::count), "True");
                if (fXmlDropNode.GetValue(Attr::getNameOfType(Attr::bytes)).GetLength())
                {
                    fXmlDropNode.SetValue(Attr::getNameOfType(Attr::bytes), fXmlDraggedNode.GetText());
                }
                else
                {
                    fXmlDropNode.SetValue(Attr::getNameOfType(Attr::repeat), fXmlDraggedNode.GetText());
                }
                theTree.SelectItem(hDropItem);
                // TODO: perform a consistency check
                UpdateXmlAttributes(hDropItem, fXmlDropNode);
                UpdateXmlAttributes(mDraggedItem, fXmlDraggedNode);
            }
        }
        mDraggedItem  = 0;
        mRightDragged = FALSE;
    }
    CTreeView::OnLButtonUp(nFlags, point);
}

void CAnyFileViewerTreeView::OnRButtonUp(UINT nFlags, CPoint point)
{
    mTreeImages.DragLeave(this);
    mTreeImages.EndDrag();

    CTreeView::OnRButtonUp(nFlags, point);
}

void CAnyFileViewerTreeView::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
    if (pNMHDR->code == NM_RCLICK)
    {
        CPoint point, screen;
        GetCursorPos(&screen);
        CTreeCtrl&theTree = GetTreeCtrl();
        point = screen;
        theTree.ScreenToClient(&point);
        HTREEITEM hDropItem = theTree.HitTest(point);
        CString sItem = theTree.GetItemText(hDropItem);
        CMenu fContextMenu;
        fContextMenu.LoadMenu(IDR_CONTEXT_TREE_ITEM);
        if (isStructureView())
        {

            fContextMenu.GetSubMenu(StructureView)->TrackPopupMenu(TPM_LEFTALIGN, screen.x, screen.y, AfxGetMainWnd());
            // TODO: Format typ einfügen ?
        }
        else
        {
            fContextMenu.GetSubMenu(ContentView)->TrackPopupMenu(TPM_LEFTALIGN, screen.x, screen.y, AfxGetMainWnd());
        }
    }
    *pResult = 0;
}


void CAnyFileViewerTreeView::OnEditInsertRepeat()
{
    CTreeCtrl&theTree = GetTreeCtrl();
    HTREEITEM hItem   = theTree.GetSelectedItem();
    if (hItem) // Inserts a repeat attribute for this variable or structure
    {
        IDispatch*fNode = (IDispatch*)theTree.GetItemData(hItem);
        if (fNode)
        {
            CXmlNodeWrapper fXmlNode(fNode);
            fXmlNode.SetValue(Attr::getNameOfType(Attr::repeat), 2);
            UpdateXmlAttributes(hItem, fXmlNode);
        }
    }
}

void CAnyFileViewerTreeView::OnUpdateEditInsertRepeat(CCmdUI *pCmdUI)
{
    BOOL bEnable = FALSE;
    if (isStructureView())
    {
        CTreeCtrl&theTree = GetTreeCtrl();
        HTREEITEM hItem   = theTree.GetSelectedItem();
        if (hItem)
        {
            int fImage=0, fSelected = 0;
            theTree.GetItemImage(hItem, fImage, fSelected);
            if (fImage > CDisplayType::Unknown && fImage <= CDisplayType::LastType)
            {
                bEnable = TRUE;
            }
        }        
    }
    pCmdUI->Enable(bEnable);
}

void CAnyFileViewerTreeView::OnEditInsertFactorAttr()
{
    CTreeCtrl&theTree = GetTreeCtrl();
    HTREEITEM hItem = theTree.GetSelectedItem();
    if (hItem) // Inserts a factor attribute for this variable or structure
    {
        IDispatch*fNode = (IDispatch*)theTree.GetItemData(hItem);
        if (fNode)
        {
            CXmlNodeWrapper fXmlNode(fNode);
            fXmlNode.SetValue(Attr::getNameOfType(Attr::factor), 1);
            UpdateXmlAttributes(hItem, fXmlNode);
        }
    }
}


void CAnyFileViewerTreeView::OnUpdateEditInsertFactorAttr(CCmdUI *pCmdUI)
{
    BOOL bEnable = FALSE;
    if (isStructureView())
    {
        CTreeCtrl&theTree = GetTreeCtrl();
        HTREEITEM hItem = theTree.GetSelectedItem();
        if (hItem)
        {
            int fImage = 0, fSelected = 0;
            theTree.GetItemImage(hItem, fImage, fSelected);
            if (fImage >= CDisplayType::Char && fImage <= CDisplayType::Double)
            {
                bEnable = TRUE;
            }
        }
    }
    pCmdUI->Enable(bEnable);
}

void CAnyFileViewerTreeView::OnEditInsertInlineAttr()
{
    CTreeCtrl&theTree = GetTreeCtrl();
    HTREEITEM hItem   = theTree.GetSelectedItem();
    if (hItem) // Inserts an inline attribute for this variable or structure
    {
        IDispatch*fNode = (IDispatch*)theTree.GetItemData(hItem);
        if (fNode)
        {
            CXmlNodeWrapper fXmlNode(fNode);
            fXmlNode.SetValue(Attr::getNameOfType(Attr::in_line), "");
            UpdateXmlAttributes(hItem, fXmlNode);
        }
    }
}

void CAnyFileViewerTreeView::OnUpdateEditInsertInlineAttr(CCmdUI *pCmdUI)
{
    BOOL bEnable = FALSE;
    if (isStructureView())
    {
        CTreeCtrl&theTree = GetTreeCtrl();
        HTREEITEM hItem   = theTree.GetSelectedItem();
        if (hItem)
        {
            int fImage=0, fSelected = 0;
            theTree.GetItemImage(hItem, fImage, fSelected);
            if (fImage == CDisplayType::Structure)
            {
                bEnable = TRUE;
            }
        }        
    }
    pCmdUI->Enable(bEnable);
}


void CAnyFileViewerTreeView::OnEditCopy(UINT aID)
{
    CTreeCtrl&theTree = GetTreeCtrl();
    HTREEITEM hItem   = theTree.GetSelectedItem();
    if (hItem)
    {
        if (isStructureView())
        {
            IDispatch*fNode = (IDispatch*)theTree.GetItemData(hItem);
            if (fNode)
            {
                CXmlNodeWrapper fXmlNode(fNode);
                theApp.GetMainFrame()->SetClipboardData(fXmlNode.GetXML());
            }
        }
        else
        {
            CAnyFileViewerApp::eFormat fOldFormat = theApp.getCurrentFormat();
            if (aID == ID_EDIT_COPY_EXCEL)
            {
                theApp.setCurrentFormat(CAnyFileViewerApp::Excel);
            }
            size_t fIndex = theTree.GetItemData(hItem);
            if (fIndex < mContentData.size())
            {
                CString fClipboardText;
                size_t    fPos  = mContentData[fIndex].mPos;
                IDispatch*fNode = mContentData[fIndex].mNode;
                RecurseContent(fNode, 0, fPos, &fClipboardText);
                theApp.GetMainFrame()->SetClipboardData(fClipboardText);
            }
            theApp.setCurrentFormat(fOldFormat);
        }
    }
}

void CAnyFileViewerTreeView::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(GetTreeCtrl().GetSelectedItem() != 0);
}

BOOL CAnyFileViewerTreeView::OnEditPaste(UINT uID)
{
    CTreeCtrl&theTree = GetTreeCtrl();
    HTREEITEM hItem   = theTree.GetSelectedItem();
    if (hItem)
    {
        HTREEITEM hParent = theTree.GetParentItem(hItem);
        IDispatch*pParent = (IDispatch*)GetTreeCtrl().GetItemData(hParent);
        CXmlDocumentWrapper doc;
        if (pParent && doc.LoadXML(theApp.GetMainFrame()->GetClipboardData()))
        {
            CXmlNodeWrapper refNode(pParent);
            MSXML2::IXMLDOMNode*pNode = 0;
            switch (uID)
            {
            case ID_EDIT_PASTE:        pNode = refNode.InsertAfter((MSXML2::IXMLDOMNode*) pParent, doc.AsNode()); break;
            }
            if (pNode)
            {
                RecurseXMLStructure(pNode, hParent, hItem);    
            }
        }
    }
    return TRUE;
}

void CAnyFileViewerTreeView::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
#ifdef _UNICODE
    UINT fClipboardFormat(CF_UNICODETEXT); 
#else
    UINT fClipboardFormat(CF_TEXT); 
#endif

   pCmdUI->Enable(GetTreeCtrl().GetSelectedItem() != 0 
       && isStructureView() 
       && ::IsClipboardFormatAvailable(fClipboardFormat));
}

CString CAnyFileViewerTreeView::GetXmlAttribute(CXmlNodeWrapper&aNode, Attr::eAttribute aAttr)
{
    for (int i = 0; i < aNode.NumAttributes(); ++i)
    {
        if (Attr::getTypeOfName(aNode.GetAttribName(i)) == aAttr)
        {
            return aNode.GetAttribVal(i);
        }
    }
    return _T("");
}

bool CAnyFileViewerTreeView::GetSelection(size_t& aStartPos, size_t& aStopPos)
{
    CTreeCtrl&theTree = GetTreeCtrl();
    if (isStructureView())
    {
        // TODO! determine selection of tree view item
    }
    else
    {
        HTREEITEM hItem = theTree.GetSelectedItem();
        size_t fIndex = theTree.GetItemData(hItem);
        if (fIndex < mContentData.size())
        {
            size_t    fPos  = mContentData[fIndex].mPos;
            IDispatch*fNode = mContentData[fIndex].mNode;
            aStartPos = fPos;
            aStopPos = aStartPos;
            RecurseContent(fNode, (HTREEITEM)0, aStopPos);
            return true;
        }
    }
    return false;
}

int CAnyFileViewerTreeView::ContentData::findpos(const ContentData* a1, const ContentData* a2)
{
    if (a1->mPos == a2->mPos) return  0;
    return (a1->mPos > a2->mPos) ? 1 : -1;
}

bool FindLParam(const CTreeCtrl&aTree, HTREEITEM aItem, const void*aValue)
{
    return aTree.GetItemData(aItem) == (DWORD_PTR)aValue ? true : false;
}

HTREEITEM CAnyFileViewerTreeView::FindTreeItem(const void*aValue, bool (*aFunc)(const CTreeCtrl&aTree, HTREEITEM aItem, const void*aValue), HTREEITEM aParent)
{
    CTreeCtrl&ctrl = GetTreeCtrl();
    HTREEITEM fNext, fItem = ctrl.GetNextItem(aParent, TVGN_CHILD);
    for (; fItem != 0; fItem = fNext)
    {
        if (aFunc(GetTreeCtrl(), fItem, aValue)) return fItem;
        fNext = ctrl.GetNextItem(fItem, TVGN_NEXT);
        HTREEITEM fFound = FindTreeItem(aValue, aFunc, fItem);
        if (fFound) return fFound;
    }
    return fItem;
}

void CAnyFileViewerTreeView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* pObject)
{
    m_bInOnUpdate = TRUE;
    CHint *pHint = (CHint*)pObject;

    CTreeCtrl&ctrl = GetTreeCtrl();
    if (lHint == UPDATE_DATA_AT_POS)
    {
        ASSERT(pHint != NULL);
        ASSERT_KINDOF(CHint, pObject);
        size_t nPos   = pHint->GetSizeT();
        if (nPos >= GetDocument()->GetSize()) return;
        if (isStructureView())
        {
            // Do nothing
        }
        else if (mContentData.size())
        {
            ContentData aVal(nPos, 0);
            ContentData*pFound = static_cast<ContentData*>(bsearch(&aVal, &mContentData[0], mContentData.size(), sizeof(ContentData), (int (*)(const void*, const void*))ContentData::findpos));
            if (pFound)
            {
                size_t fIndex = pFound - &mContentData[0];
                HTREEITEM fItem = FindTreeItem((const void*)fIndex, FindLParam, ctrl.GetRootItem());
                if (fItem)
                {
                    // TODO! UPDATE_DATA_AT_POS
                }
            }
        }
    }
    if (lHint == UPDATE_STRUCTURE_VIEW)
    {
        ASSERT(pHint != NULL);
        ASSERT_KINDOF(CItemStructureHint, pObject);
        mItemStructureHint = (CItemStructureHint*) pHint;
        size_t fPos = 0;
        mContentData.clear();
        mCounters.RemoveAll();
        GetDocument()->UpdateAllViews(this, UPDATE_CLEAR_MESSAGE_VIEW);
        RecurseContent(m_xmlDoc.AsNode(), TVI_ROOT, fPos);
        mItemStructureHint = NULL;
    }
    if (lHint == UPDATE_DATA_POS)
    {
        ASSERT(pHint != NULL);
        ASSERT_KINDOF(CHint, pObject);
        size_t nPos   = pHint->GetSizeT();
        if (nPos >= GetDocument()->GetSize()) return;
        if (isStructureView())
        {
            // TODO! select tree view item from data pos
        }
        else if (mContentData.size())
        {
            ContentData aVal(nPos, 0);
            ContentData*pFound = static_cast<ContentData*>(bsearch(&aVal, &mContentData[0], mContentData.size(), sizeof(ContentData), (int (*)(const void*, const void*))ContentData::findpos));
            if (pFound)
            {
                size_t fIndex = pFound - &mContentData[0];
                HTREEITEM fItem = FindTreeItem((const void*)fIndex, FindLParam, ctrl.GetRootItem());
                if (fItem) ctrl.SelectItem(fItem);
            }
        }
    }
    else
    {
        // do nothing
    }
    m_bInOnUpdate = FALSE;
}


void CAnyFileViewerTreeView::OnViewUpdate()
{
    if (mViewType == showFileContent)
    {
        OnTreeviewFileContent();
    }
}


void CAnyFileViewerTreeView::OnTreeviewInlineStructure()
{
    m_bShowInline = !m_bShowInline;
}


void CAnyFileViewerTreeView::OnUpdateTreeviewInlineStructure(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
    pCmdUI->SetCheck(m_bShowInline);
}

BOOL CAnyFileViewerTreeView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: Datentyp ändern, wenn hover über Datentyp Icon im Treeitem.

    return CTreeView::OnMouseWheel(nFlags, zDelta, pt);
}


void CAnyFileViewerTreeView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    if (mTreeCtrl && isStructureView())
    {
        mTreeCtrl->OnLButtonDblClk(nFlags, point);
    }

    CTreeView::OnLButtonDblClk(nFlags, point);
}


void CAnyFileViewerTreeView::OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult)
{
    if (mTreeCtrl && isStructureView())
    {
        mTreeCtrl->OnKeyDown(pNMHDR, pResult);
    }
}


afx_msg void CAnyFileViewerTreeView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    nChar = 0;
}
