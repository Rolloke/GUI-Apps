// DBCheckView.cpp : implementation of the CDBCheckView class
//

#include "stdafx.h"
#include "DBCheck.h"

#include "DBCheckDoc.h"
#include "DBCheckView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDBCheckView

IMPLEMENT_DYNCREATE(CDBCheckView, CListView)

BEGIN_MESSAGE_MAP(CDBCheckView, CListView)
	//{{AFX_MSG_MAP(CDBCheckView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteitem)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBCheckView construction/destruction

CDBCheckView::CDBCheckView()
{
   m_nMaxArchiveTimePos  = 0;
   m_nMaxSequenceTimePos = 0;
   m_MaxArchiveTime = CSystemTime();
   m_dwFirstRequestedNr = 1;
}

CDBCheckView::~CDBCheckView()
{
}

BOOL CDBCheckView::PreCreateWindow(CREATESTRUCT& cs)
{
   cs.style &= ~(LVS_LIST|LVS_SORTASCENDING|LVS_SORTDESCENDING);
   cs.style |=  (LVS_REPORT|LVS_SHOWSELALWAYS);
	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDBCheckView drawing

void CDBCheckView::OnDraw(CDC* pDC)
{
	CDBCheckDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CListCtrl& refCtrl = GetListCtrl();
	refCtrl.InsertItem(0, "Item!");
	// TODO: add draw code for native data here
}

void CDBCheckView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();


	// TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to GetListCtrl().
}

/////////////////////////////////////////////////////////////////////////////
// CDBCheckView printing

BOOL CDBCheckView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDBCheckView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDBCheckView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CDBCheckView diagnostics

#ifdef _DEBUG
void CDBCheckView::AssertValid() const
{
	CListView::AssertValid();
}

void CDBCheckView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CDBCheckDoc* CDBCheckView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDBCheckDoc)));
	return (CDBCheckDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDBCheckView message handlers
void CDBCheckView::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	//TODO: add code to react to the user changing the view style of your window
}

void CDBCheckView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	
	
}

bool CDBCheckView::ContainsSequence(WORD wArchivNr, WORD wSequenceNr, DWORD dwRecordNr)
{
	CListCtrl   &theCtrl = GetListCtrl();
   CString      str;
   str.Format("A:%d, S:%d, R:%d", wArchivNr, wSequenceNr, dwRecordNr);
   LVFINDINFO info;
   info.flags = LVFI_PARTIAL|LVFI_STRING;
   info.psz = str;
   return (theCtrl.FindItem(&info) != -1) ? true : false;
}

void CDBCheckView::InsertRecordNr(WORD wArchivNr, WORD wSequenceNr, 
                                  DWORD dwRecordNr, DWORD dwNrOfRecords, const CIPCPictureRecord &pict,
                                  int iNumFields, const CIPCField fields[])
{
	CListCtrl   &theCtrl = GetListCtrl();
   int          nPos    = theCtrl.GetItemCount();
   CString      str;
   const CSystemTime &time    = pict.GetTimeStamp();
   if (m_dwFirstRequestedNr == 0xffffffff)
   {
      m_dwFirstRequestedNr = dwRecordNr;
   }
   if (dwRecordNr == m_dwFirstRequestedNr)
   {
      DWORD dwData = 0;
      str.Format("A:%d, S:%d, R:%d", wArchivNr, wSequenceNr, dwRecordNr);
      theCtrl.InsertItem(nPos, str);
      if (nPos > 1) dwData = theCtrl.GetItemData(nPos-1);
      if (dwData)
      {
         CSystemTime *pST = (CSystemTime*)dwData;
         if (((CSystemTime&)time) > (*pST))
         {
            CSystemTime tDiff = time - *pST;
            str.Format("%d Tage, %02d:%02d:%02d, %03d", tDiff.GetDay()-1, tDiff.GetHour(), tDiff.GetMinute(), tDiff.GetSecond(), tDiff.GetMilliseconds());
            theCtrl.SetItemText(nPos, 1, str);
         }
      }
      theCtrl.SetItemCount(nPos+dwNrOfRecords);
      nPos++;
      m_MaxSequenceTime = CSystemTime();
   }
   
   str = time.GetDateTime();
   theCtrl.InsertItem(nPos, str);
   theCtrl.SetItemData(nPos, (DWORD)new CSystemTime(time));
   if (dwRecordNr > m_dwFirstRequestedNr)
   {
      DWORD dwData =0;
      if (nPos > 0) dwData = theCtrl.GetItemData(nPos-1);
      if (dwData)
      {
         CSystemTime *pST = (CSystemTime*)dwData;
         CSystemTime tDiff = time - *pST;
         if (tDiff > m_MaxSequenceTime)
         {
            m_MaxSequenceTime     = tDiff;
            m_nMaxSequenceTimePos = nPos;
         }
         str.Format("%02d:%02d:%02d, %03d", tDiff.GetHour(), tDiff.GetMinute(), tDiff.GetSecond(), tDiff.GetMilliseconds());
         theCtrl.SetItemText(nPos, 1, str);
      }
   }
   if (dwRecordNr == dwNrOfRecords)
   {
      if (m_MaxSequenceTime > m_MaxArchiveTime)
      {
         m_MaxArchiveTime     = m_MaxSequenceTime;
         m_nMaxArchiveTimePos = m_nMaxSequenceTimePos;
      }
      CSystemTime tDiff = m_MaxSequenceTime;
      str.Format("%02d:%02d:%02d,%03d", tDiff.GetHour(), tDiff.GetMinute(), tDiff.GetSecond(), tDiff.GetMilliseconds());
      theCtrl.SetItemText(m_nMaxSequenceTimePos, 2, str);
   }
}

void CDBCheckView::InsertRecord(WORD wArchivNr, WORD wSequenceNr, DWORD dwRecordNr, CIPCFields fields, int *pnPositions, CIPCFields records)
{
	CListCtrl   &theCtrl = GetListCtrl();
   int          nPos    = theCtrl.GetItemCount();
   int          nDatePos=-1, nDateLen=0, nTimePos= -1, nTimeLen=0, nMSPos=-1, nMSLen=0;
   int          i, nRecords = records.GetSize();
   CString      str, strDate, strTime;
   CSystemTime  time;
   CIPCField*pF = fields.GetCIPCField("DBP_DATE");
   if (pF)
   {
      nDatePos = pnPositions[fields.Index(pF)];
      nDateLen = pF->GetMaxLength();
   }
   else
   {
      pF = fields.GetCIPCField("DVR_DATE");
      if (pF)
      {
         nDatePos = pnPositions[fields.Index(pF)];
         nDateLen = pF->GetMaxLength();
      }
   }
   pF = fields.GetCIPCField("DBP_TIME");
   if (pF)
   {
      nTimePos = pnPositions[fields.Index(pF)];
      nTimeLen = pF->GetMaxLength();
   }
   else
   {
      pF = fields.GetCIPCField("DVR_TIME");
      if (pF)
      {
         nTimePos = pnPositions[fields.Index(pF)];
         nTimeLen = pF->GetMaxLength();
      }
   }
   pF = fields.GetCIPCField("DBP_MS");
   if (pF)
   {
      nMSPos = pnPositions[fields.Index(pF)];
      nMSLen = pF->GetMaxLength();
   }
   else
   {
      pF = fields.GetCIPCField("DVR_MS");
      if (pF)
      {
         nMSPos = pnPositions[fields.Index(pF)];
         nMSLen = pF->GetMaxLength();
      }
   }
   
   if (!nTimeLen || !nDateLen)
   {
      return;
   }
   DWORD dwData = 0;
   str.Format("A:%d, S:%d, R:%d", wArchivNr, wSequenceNr, dwRecordNr);
   theCtrl.InsertItem(nPos, str);
   if (nPos > 1) dwData = theCtrl.GetItemData(nPos-1);
   if (dwData)
   {
      CSystemTime *pST = (CSystemTime*)dwData;
      str     = records[0]->GetValue();
      strTime = str.Mid(nTimePos, nTimeLen);
      strDate = str.Mid(nDatePos, nDateLen);
      time.SetDBTime(strTime);
      time.SetDBDate(strDate);
      if (nMSLen)
      {
         strTime = str.Mid(nMSPos, nMSLen);
         time.IncrementTime(0, 0, 0, 0, atoi(strTime));
      }
         
      if (((CSystemTime&)time) > (*pST))
      {
         CSystemTime tDiff = time - *pST;
         str.Format("%d Tage, %02d:%02d:%02d, %03d", tDiff.GetDay()-1, tDiff.GetHour(), tDiff.GetMinute(), tDiff.GetSecond(), tDiff.GetMilliseconds());
         theCtrl.SetItemText(nPos, 1, str);
      }
   }
   
   theCtrl.SetItemCount(nPos+nRecords);
   nPos++;
   m_MaxSequenceTime = CSystemTime();
   
   for (i=0; i<nRecords; i++, nPos++)
   {
      str     = records[i]->GetValue();
      strTime = str.Mid(nTimePos, nTimeLen);
      strDate = str.Mid(nDatePos, nDateLen);
      time.SetDBTime(strTime);
      time.SetDBDate(strDate);
      if (nMSLen)
      {
         strTime = str.Mid(nMSPos, nMSLen);
         time.IncrementTime(0, 0, 0, 0, atoi(strTime));
      }
      str = time.GetDateTime();
      theCtrl.InsertItem(nPos, str);
      theCtrl.SetItemData(nPos, (DWORD)new CSystemTime(time));
      if (i > 0)
      {
         DWORD dwData =0;
         if (nPos > 0) dwData = theCtrl.GetItemData(nPos-1);
         if (dwData)
         {
            CSystemTime *pST = (CSystemTime*)dwData;
            CSystemTime tDiff = time - *pST;
            if (tDiff > m_MaxSequenceTime)
            {
               m_MaxSequenceTime     = tDiff;
               m_nMaxSequenceTimePos = nPos;
            }
            str.Format("%02d:%02d:%02d, %03d", tDiff.GetHour(), tDiff.GetMinute(), tDiff.GetSecond(), tDiff.GetMilliseconds());
            theCtrl.SetItemText(nPos, 1, str);
         }
      }
      if (i == nRecords-1)
      {
         if (m_MaxSequenceTime > m_MaxArchiveTime)
         {
            m_MaxArchiveTime     = m_MaxSequenceTime;
            m_nMaxArchiveTimePos = m_nMaxSequenceTimePos;
         }
         CSystemTime tDiff = m_MaxSequenceTime;
         str.Format("%02d:%02d:%02d,%03d", tDiff.GetHour(), tDiff.GetMinute(), tDiff.GetSecond(), tDiff.GetMilliseconds());
         theCtrl.SetItemText(m_nMaxSequenceTimePos, 2, str);
      }
   }
}

void CDBCheckView::InsertMaxArchiveTime()
{
	CListCtrl   &theCtrl = GetListCtrl();
   CSystemTime  tDiff = m_MaxArchiveTime;
   CString      str;
   str.Format("%02d:%02d:%02d,%03d", tDiff.GetHour(), tDiff.GetMinute(), tDiff.GetSecond(), tDiff.GetMilliseconds());
   theCtrl.SetItemText(m_nMaxArchiveTimePos, 3, str);
}

int CDBCheckView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

   CListCtrl &theCtrl = GetListCtrl();

   theCtrl.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_INFOTIP);
   int i=0;
   theCtrl.InsertColumn(i++, "Datum, Zeit", LVCFMT_LEFT, 150);
   theCtrl.InsertColumn(i++, "Differenz (H:M:S:ms)", LVCFMT_LEFT, 150);
   theCtrl.InsertColumn(i++, "Maxima der Sequenzen", LVCFMT_LEFT, 150);
   theCtrl.InsertColumn(i++, "Maximum eines Archivs", LVCFMT_LEFT, 150);
	return 0;
}

void CDBCheckView::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
   CListCtrl &theCtrl = GetListCtrl();
   DWORD dwData = theCtrl.GetItemData(pNMListView->iItem);
   if (dwData)
   {
      CSystemTime *pST = (CSystemTime*)dwData;
      delete pST;
   }
	*pResult = 0;
}

void CDBCheckView::SetFirstRequestedNr(DWORD dwNo)
{
   m_dwFirstRequestedNr = dwNo;
}

DWORD CDBCheckView::GetFirstRequestedNr()
{
   return m_dwFirstRequestedNr;
}

