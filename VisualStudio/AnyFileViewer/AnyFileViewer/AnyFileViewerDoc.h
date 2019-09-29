// AnyFileViewerDoc.h : Schnittstelle der Klasse CAnyFileViewerDoc
//


#pragma once
#define UPDATE_INITIAL   0
#define UPDATE_LINE_SIZE 1
#define UPDATE_DATA_POS  2
#define UPDATE_DATA_FORMAT_POS  3
#define UPDATE_DATA_AT_POS  4
#define UPDATE_STRUCTURE_VIEW 5
#define UPDATE_INSERT_MESSAGE_VIEW 6
#define UPDATE_CLEAR_MESSAGE_VIEW 7

class CStructureItem
{
public:
    CStructureItem(size_t aPos, char aType, const CString& aName)
        : mStartPos(aPos)
        , mDisplayType(aType)
        , mDisplayName(aName)
    {}
    CStructureItem(size_t aPos)
        : mStartPos(aPos)
        , mDisplayType(0)
    {}

    bool operator==(const CStructureItem& _Right) const
    {    // test for iterator equality
        return (mStartPos == _Right.mStartPos);
    }

    bool operator<(const CStructureItem& _Right) const
    {    // test for iterator equality
        return (mStartPos < _Right.mStartPos);
    }

    size_t mStartPos;
    char         mDisplayType;
    CString      mDisplayName;
};


class CHint : public CObject
{
   DECLARE_DYNAMIC(CHint)   
public:
   CHint(int n);
   CHint(size_t n);
   CHint(const CString &s);
   VARENUM GetType();
   CString GetString();
   int     GetINT();
   size_t  GetSizeT();

private:
   CHint();
   VARENUM m_vtype;
   union u_data
   {
      size_t  ln;
      int     n;
      CString* s;
   }m_data;
};

class CMessageHint : public CObject
{
    DECLARE_DYNAMIC(CMessageHint)
public:
    CMessageHint();
    CString mName;
    CString mMessage;
    size_t  mPosition;
};

class CItemStructureHint : public CObject
{
    DECLARE_DYNAMIC(CItemStructureHint)
public:
    CItemStructureHint(std::vector<CStructureItem>& aItems)
        : mItems(aItems)
    {

    }
    std::vector<CStructureItem>& mItems;
};

class CXFile: public CFile
{
   const enum {BufferSize=256};
   const enum Modes {None, Ansi, Unicode, UnicodeBE};
public:
   CXFile();
   int ReadString(CString& sRead);
   int EndOfLine(WORD w);
private:
   Modes m_nMode;
};

class CAnyFileViewerDoc : public CDocument
{
protected: // Nur aus Serialisierung erstellen
   CAnyFileViewerDoc();
   DECLARE_DYNCREATE(CAnyFileViewerDoc)

// Attribute
public:
   size_t    GetSize() { return m_nSize; }
   BYTE*     GetData() { return m_pData; }

// Operationen
public:

// Überschreibungen
   public:
   virtual BOOL OnNewDocument();
   virtual void Serialize(CArchive& ar);
    virtual void DeleteContents();

// Implementierung
public:
   virtual ~CAnyFileViewerDoc();
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Funktionen für die Meldungstabellen
protected:
    afx_msg void OnFileSaveSelection();
   DECLARE_MESSAGE_MAP()

private:
   size_t m_nSize;
   BYTE* m_pData;
   BOOL  m_bFirstNewDocumentCall;
   size_t m_nStartPosition;
   size_t m_nStopPosition;
public:
    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
};


