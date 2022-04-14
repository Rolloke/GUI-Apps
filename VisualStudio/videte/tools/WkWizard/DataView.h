// DataView.h : header file
//

/* Test
Kommentar */

class CTokenArray;
class CToken;
class CTSArray;
class CTranslateString;
/////////////////////////////////////////////////////////////////////////////
// CDataView view

class CDataView : public CListView
{
protected:
	CDataView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDataView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDataView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDataView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void OnTokenArray(CTokenArray* pTokenArray);
	void InsertToken(CToken* pToken);
	void OnTSArray(CTSArray* pTSArray);
	void InsertTranslateString(CTranslateString* pTranslateString);
	void DeleteItemsAndColumns();
};

/////////////////////////////////////////////////////////////////////////////
