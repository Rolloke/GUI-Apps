// AnyFileViewerDoc.cpp : Implementierung der Klasse CAnyFileViewerDoc
//

#include "stdafx.h"
#include "AnyFileViewer.h"
#include "NewDocDlg.h"

#include "AnyFileViewerDoc.h"
#include "AnyFileViewerView.h"
#include "AnyFileViewerTreeView.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNAMIC(CHint, CObject)

CHint::CHint(int n)
{
   m_vtype = VT_INT;
   m_data.n = n;
}
CHint::CHint(size_t n)
{
   m_vtype = VT_I8;
   m_data.ln = n;
}
CHint::CHint(const CString &s)
{
   m_vtype = VT_BSTR;
   *m_data.s = s;
}
CString CHint::GetString()
{
   if (m_vtype == VT_BSTR)
   {
      return *m_data.s; 
   }
   return _T("");
}

int CHint::GetINT()
{
   if (m_vtype == VT_INT)
   {
      return m_data.n; 
   }
   if (m_vtype == VT_I8)
   {
      return (int)m_data.ln; 
   }
   return 0; 
}
size_t CHint::GetSizeT()
{
   if (m_vtype == VT_I8)
   {
      return m_data.ln; 
   }
   if (m_vtype == VT_INT)
   {
      return m_data.n; 
   }
   return 0; 
}

IMPLEMENT_DYNAMIC(CMessageHint, CObject)

CMessageHint::CMessageHint()
{

}


IMPLEMENT_DYNAMIC(CItemStructureHint, CObject)



CXFile::CXFile()
{
   m_nMode = Modes::None;
}

int CXFile::EndOfLine(WORD w)
{
   return (w == 13 || w == 10) ? TRUE : FALSE;
}

int CXFile::ReadString(CString& sRead)
{
   sRead.Empty();
   if (m_nMode == Modes::None)
   {
      BYTE Buff[3] = "  ";
      Read(Buff, 2);
      if (Buff[0] == 0xfe && Buff[1] == 0xff)
      {
         m_nMode = Modes::UnicodeBE;
      }
      else if (Buff[1] == 0xfe && Buff[0] == 0xff)
      {
         m_nMode = Modes::Unicode;
      }
      else
      {
         m_nMode = Modes::Ansi;
         sRead += CString(Buff);
      }
   }
   if (m_nMode == Modes::Unicode)
   {
      _TCHAR Buff;
      while (Read(&Buff, sizeof(Buff)))
      {
          if (EndOfLine(Buff)) return 1;
          sRead += Buff;
      }
   }
   else if (m_nMode == Modes::UnicodeBE)
   {
      _TCHAR Buff;
      while (Read(&Buff, sizeof(Buff)))
      {
         Buff = MAKEWORD(HIBYTE(Buff), LOBYTE(Buff));
         if (EndOfLine(Buff)) return 1;
         sRead += Buff;
      }
   }
   else
   {
      BYTE Buff;
      while (Read(&Buff, sizeof(Buff)))
      {
          if (EndOfLine(Buff)) return 1;
          sRead += Buff;
      }
   }
   return 0;
}
// CAnyFileViewerDoc

IMPLEMENT_DYNCREATE(CAnyFileViewerDoc, CDocument)

BEGIN_MESSAGE_MAP(CAnyFileViewerDoc, CDocument)
    ON_COMMAND(ID_FILE_SAVE_SELECTION, OnFileSaveSelection)
END_MESSAGE_MAP()


// CAnyFileViewerDoc Erstellung/Zerstörung

CAnyFileViewerDoc::CAnyFileViewerDoc(): m_pData(NULL), m_nSize(0), m_bFirstNewDocumentCall(TRUE)
{
}

CAnyFileViewerDoc::~CAnyFileViewerDoc()
{
}

BOOL CAnyFileViewerDoc::OnNewDocument()
{
   if (!CDocument::OnNewDocument())
      return FALSE;

   if (!m_bFirstNewDocumentCall)
   {
       CNewDocDlg dlg;
       if (dlg.DoModal() == IDOK)
       {
           m_nSize = dlg.m_nNewDocSize;
           m_pData = new BYTE[m_nSize];
       }
   }
   m_bFirstNewDocumentCall = FALSE;
   return TRUE;
}

// CAnyFileViewerDoc Serialisierung
void CAnyFileViewerDoc::Serialize(CArchive& ar)
{
   if (ar.IsStoring())
   {
      if (m_pData && m_nSize)
      {
          if (m_nStartPosition && m_nStopPosition && m_nStopPosition > m_nStartPosition)
          {
              ar.Write(&m_pData[m_nStartPosition], static_cast<UINT>(m_nStopPosition-m_nStartPosition));
          }
          else
          {
              ar.Write(m_pData, (UINT)m_nSize);
          }
      }
   }
   else
   {
      m_nSize = (size_t)ar.GetFile()->GetLength();
      m_pData = new BYTE[m_nSize];
      ar.Read(m_pData, (UINT)m_nSize);
   }
}

void CAnyFileViewerDoc::OnFileSaveSelection()
{
    CView *pView = theApp.GetMainFrame()->GetActiveView();
    if (pView)
    {
        bool bSelected = false;
        if (pView->IsKindOf(RUNTIME_CLASS(CAnyFileViewerView)))
        {
            bSelected = ((CAnyFileViewerView*)pView)->GetSelection(m_nStartPosition, m_nStopPosition);
        }
        if (pView->IsKindOf(RUNTIME_CLASS(CAnyFileViewerTreeView)))
        {
            bSelected = ((CAnyFileViewerTreeView*)pView)->GetSelection(m_nStartPosition, m_nStopPosition);
        }
        if (bSelected)
        {
            DoSave(NULL, FALSE);
        }
    }
    m_nStartPosition = 0;
    m_nStopPosition = 0;
}


// CAnyFileViewerDoc Diagnose

#ifdef _DEBUG
void CAnyFileViewerDoc::AssertValid() const
{
   CDocument::AssertValid();
}

void CAnyFileViewerDoc::Dump(CDumpContext& dc) const
{
   CDocument::Dump(dc);
}
#endif //_DEBUG


// CAnyFileViewerDoc-Befehle
void CAnyFileViewerDoc::DeleteContents()
{
   if (m_pData)
   {
      delete m_pData;
      m_pData = NULL;
   }
   CDocument::DeleteContents();
}

BOOL CAnyFileViewerDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
    if (!CDocument::OnOpenDocument(lpszPathName))
        return FALSE;

    return TRUE;
}
