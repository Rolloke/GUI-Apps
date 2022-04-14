/////////////////////////////////////////////////////////////////////////////
// PROJECT:        LogView
// FILE:        $Workfile: LogViewView.cpp $
// ARCHIVE:        $Archive: /Project/Tools/LogView/LogViewView.cpp $
// CHECKIN:        $Date: 14.03.06 21:22 $
// VERSION:        $Revision: 28 $
// LAST CHANGE:    $Modtime: 14.03.06 21:20 $
// BY AUTHOR:    $Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogView.h"

#include "LogViewDoc.h"
#include "CntrItem.h"
#include "LogViewView.h"
#include "MainFrm.h"
#include <algorithm>

#include <afximpl.h>
#include ".\logviewview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define EVENT_UPDATE_SELECTION  10
#define EVENT_SELECTION            11
#define EVENT_SELECT_BOOKMARK    12
#define EVENT_UNSELECT_BOOKMARK    13
/////////////////////////////////////////////////////////////////////////////
// CLogViewView

IMPLEMENT_DYNCREATE(CLogViewView, CRichEditView)

#ifdef _DEBUG
__declspec(dllimport) PROCESS_LOCAL(_AFX_RICHEDIT_STATE, _afxRichEditState)
_AFX_RICHEDIT_STATE* GetRichEditState()
{
    return _afxRichEditState.GetData();
}
#endif

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CLogViewView, CRichEditView)
    //{{AFX_MSG_MAP(CLogViewView)
    ON_WM_DESTROY()
    ON_COMMAND(ID_REFRESH, OnRefresh)
    ON_COMMAND(ID_FILE_PRINT, CRichEditView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, CRichEditView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, CRichEditView::OnFilePrintPreview)
    ON_COMMAND(ID_EDIT_FIND, OnEditFind)
    ON_COMMAND(ID_EDIT_SEARCH_IN_COMPRESSED, OnEditSearchInCompressed)
    ON_WM_TIMER()
    ON_UPDATE_COMMAND_UI(SC_CLOSE, OnUpdateSC_CLOSE)
    ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_SYNCPOINT, OnUpdateViewShowSyncpoint)
    ON_COMMAND(ID_VIEW_SHOW_SYNCPOINT, OnViewShowSyncpoint)
    ON_UPDATE_COMMAND_UI(ID_EDIT_SEARCH_TIME_VALUE, OnUpdateEditSearchTimeValue)
    ON_COMMAND(ID_EDIT_SEARCH_TIME_VALUE, OnEditSearchTimeValue)
    ON_COMMAND(ID_VIEW_SET_BOOKMARK, OnViewSetBookMark)
    ON_UPDATE_COMMAND_UI(ID_VIEW_SET_BOOKMARK, OnUpdateViewSetBookMark)
    ON_COMMAND(ID_VIEW_NEXT_BOOKMARK, OnViewNextBookmark)
    ON_UPDATE_COMMAND_UI(ID_VIEW_NEXT_BOOKMARK, OnUpdateViewNextBookmark)
    ON_COMMAND(ID_VIEW_PREV_BOOKMARK, OnViewPrevBookmark)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PREV_BOOKMARK, OnUpdateViewPrevBookmark)
    ON_COMMAND(ID_VIEW_DELETE_BOOKMARKS, OnViewDeleteBookmarks)
    ON_UPDATE_COMMAND_UI(ID_VIEW_DELETE_BOOKMARKS, OnUpdateViewDeleteBookmarks)
    ON_MESSAGE(WM_USER, OnUser)
    ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, OnUpdateEditFind)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogViewView construction/destruction

/////////////////////////////////////////////////////////////////////////////
CLogViewView::CLogViewView():
  m_nCurrentItem(-1)
, m_bNext(FALSE)
, m_bSearchInCompressed(FALSE)
, m_bFoundInCompressed(FALSE)
, m_posBookmark(NULL)
, mTimeCharacters(0)
{
    m_crPostSelection.cpMin = 0;
    m_crPostSelection.cpMax = 0;
}

/////////////////////////////////////////////////////////////////////////////
CLogViewView::~CLogViewView()
{
    OnViewDeleteBookmarks();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CLogViewView::PreCreateWindow(CREATESTRUCT& cs)
{

    return CRichEditView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
void CLogViewView::OnInitialUpdate()
{
    CRichEditView::OnInitialUpdate();

    CFont font;

    font.CreatePointFont(100, GetFixFontFaceName());
    CStringArray ar;
    CoInitialize(NULL);
    if (GetCodePageInfo(CWK_String::GetCodePage(), ar))
    {
        CString s = ar.GetAt(OEM_CP_GDICHARSET);
        LPTSTR sz = NULL;
        long lCharset = wcstol(s, &sz, 16);
        if (lCharset)
        {
            LOGFONT lf;
            font.GetLogFont(&lf);
            lf.lfCharSet = (BYTE)lCharset;
            font.DeleteObject();
            font.CreateFontIndirect(&lf);
        }
    }
    CoUninitialize();
    SetFont(&font, TRUE);

    // Set the printing margins (720 twips = 1/2 inch).
    SetMargins(CRect(720, 720, 720, 720));

    GetRichEditCtrl().SetReadOnly();

    // Maximiere das MDIChildWnd, damit es des Hauptfenster voll ausfuellt
    // und es keine unnoetigen Child-Rahmen gibt
    CWnd* pFrame = GetParentFrame();
    ASSERT(pFrame->IsKindOf( RUNTIME_CLASS(CMDIChildWnd) ) );
    ((CMDIChildWnd*)pFrame)->MDIMaximize();
    DetermineTimeType();
}

/////////////////////////////////////////////////////////////////////////////
// CLogViewView printing

/////////////////////////////////////////////////////////////////////////////
BOOL CLogViewView::OnPreparePrinting(CPrintInfo* pInfo)
{
    // default preparation
    return DoPreparePrinting(pInfo);
}

/////////////////////////////////////////////////////////////////////////////
void CLogViewView::OnDestroy()
{
    // Deactivate the item on destruction; this is important
    // when a splitter view is being used.
   COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
   if (pActiveItem != NULL && pActiveItem->GetActiveView() == this)
   {
      pActiveItem->Deactivate();
      ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
   }
   CRichEditView::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// CLogViewView diagnostics

#ifdef _DEBUG
/////////////////////////////////////////////////////////////////////////////
void CLogViewView::AssertValid() const
{
    CRichEditView::AssertValid();
}

/////////////////////////////////////////////////////////////////////////////
void CLogViewView::Dump(CDumpContext& dc) const
{
    CRichEditView::Dump(dc);
}

/////////////////////////////////////////////////////////////////////////////
CLogViewDoc* CLogViewView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLogViewDoc)));
    return (CLogViewDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLogViewView message handlers
void CLogViewView::OnRefresh() 
{
    CRichEditCtrl& editControl = GetRichEditCtrl();

    editControl.SetRedraw(FALSE);
    GetDocument()->Refresh();
    editControl.SetRedraw(TRUE);
    if (((CMainFrame*)AfxGetMainWnd())->m_bScrollToEndAtRefresh)
    {
        editControl.SendMessage(EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
    }
    else
    {
        editControl.InvalidateRect(NULL);
    }
}
/////////////////////////////////////////////////////////////////////////////
int    CLogViewView::BSearchTime(long nStart, long nEnd, long nSyncDay, long nSyncSec)
{
    if (nStart < nEnd)
    {
        long i = (nStart + nEnd) >> 1, nDay, nSec, nTemp;
        nTemp = i;
        BOOL bReverse = FALSE;
        while (GetTimeOfLine(i, nDay, nSec) == FALSE)
        {    // keine Zahl gefunden
            if (bReverse)
            {
                if (i>nStart)
                {
                    i--;
                }
                else
                {
                    return nTemp;
                }
            }
            else
            {
                if (i<nEnd)
                {
                    i++;
                }
                else
                {
                    i = nTemp -1;
                    bReverse = TRUE;
                }
            }
        }
        if (i == m_nCurrentItem)
        {
            return i;
        }
        TRACE(_T("Line:%d\n"), i);
        m_nCurrentItem = i;
        if (nSyncDay == nDay || nDay == 0)
        {
            if (nSec > nSyncSec)
            {
                return BSearchTime(nStart, i, nSyncDay, nSyncSec);
            }
            else
            {
                return BSearchTime(i, nEnd, nSyncDay, nSyncSec);
            }
        }
        else if (nDay > nSyncDay)
        {
            return BSearchTime(nStart, i, nSyncDay, nSyncSec);
        }
        else
        {
            return BSearchTime(i, nEnd, nSyncDay, nSyncSec);
        }
    }
    else
    {
        return nStart;
    }
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLogViewView::IsTimeAvailable(long nSyncDay, long nSyncSec)
{
    CRichEditCtrl& editControl = GetRichEditCtrl();
    long nDay = 0, nSec = 0;
    int nLine = 0, nCount = editControl.GetLineCount();
    while (GetTimeOfLine(nLine, nDay, nSec) == FALSE)
    {
        if (nLine < nCount)
        {
            nLine++;
        }
        else
        {
            break;
        }
    }
    if (   nSyncDay > nDay
        || (nSyncDay == nDay && nSyncSec >= nSec))
    {
        nLine = nCount-1;
        while (GetTimeOfLine(nLine, nDay, nSec) == FALSE)
        {
            if (nLine >= 0)
            {
                nLine--;
            }
            else
            {
                break;
            }
        }
        if (   nSyncDay < nDay+1
            || (nSyncDay == nDay && nSyncSec <= nSec))
        {
            return TRUE;
        }
    }
    return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
int CLogViewView::OnViewSyncTime(long &nSyncDay, long &nSyncSec)
{
    CRichEditCtrl& editControl = GetRichEditCtrl();
    if (nSyncDay == 0)
    {
        long nLine = -1;
        BOOL bLineChanged = FALSE;
        m_crPostSelection = GetTimePositionOfLine(&nLine);
        int nCount = editControl.GetLineCount();
        while (GetTimeOfLine(nLine, nSyncDay, nSyncSec) == FALSE)
        {
            bLineChanged = TRUE;
            if (nLine < nCount)
            {
                nLine++;
            }
            else
            {
                break;
            }
        }
        if (bLineChanged)
        {
            m_crPostSelection = GetTimePositionOfLine(&nLine);
        }
        SendMessage(WM_TIMER, EVENT_UPDATE_SELECTION);
        return nLine;
    }
    else
    {
        long nDay, nSec, nDiff = 0, nMinDiff = MAXLONG, nCurrLine = -1;
        int i, nCount = editControl.GetLineCount();
        m_crPostSelection.cpMin = 0;
        m_crPostSelection.cpMax = 0;

        TRACE(_T("No Of Values:%d\n"), nCount);
        i = BSearchTime(0, nCount, nSyncDay, nSyncSec);
        if (m_nCurrentItem == i)        // Abbruch der Rekursion
        {
            nCurrLine = i;                // ist wohl in der Nähe
            i+=10;                        // genauer suchen
            if (i<nCount) nCount = i;    // im Bereich +- 10
            i-=20;                        // 
            if (i<0) i=0;
            for (; i<nCount; i++)    
            {
                if (GetTimeOfLine(i, nDay, nSec) == TRUE)
                {
                    if (nSyncDay == nDay || nDay == 0)
                    {
                        nDiff = abs(nSec - nSyncSec);
                        if (nDiff <= nMinDiff)
                        {
                            nMinDiff = nDiff;
                            nCurrLine = i;
                        }
                        else if (nDiff > nMinDiff)
                        {
                            break;
                        }
                        if (nDiff == 0)
                        {
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            nCurrLine = i;
        }
        m_nCurrentItem = -1;

        if (nCurrLine != -1)
        {
            m_crPostSelection.cpMin = editControl.LineIndex(nCurrLine);
            m_crPostSelection.cpMax = m_crPostSelection.cpMin + mTimeCharacters;
            PostMessage(WM_TIMER, EVENT_UPDATE_SELECTION);
            return nCurrLine;
        }
    }
    return -1;
}

void CLogViewView::ParseFormat(const CString &aTimeFmt, CString &aFmt, std::vector<int> &aParameter, int&aTimeStringLength)
{
    int n, i;
    _TCHAR fC;
    n = aTimeFmt.GetLength();
    for (i=0; i<n; ++i)
    {
        fC = aTimeFmt[i];
        if (fC == _T('%'))
        {
            ++i;
            _TCHAR fId = aTimeFmt[i];
            _TCHAR fSign = _T('d');
            switch (fId)
            {
            // %c: Date and time representation appropriate for locale
            // %x: Date representation for current locale
            // %X: Time representation for current locale
            // %z, %Z: Either the time-zone name or time zone abbreviation, depending on registry settings; no characters if time zone is unknown
            // %w: Weekday as decimal number (0 – 6; Sunday is 0)
            // %a: Abbreviated weekday name
            // %A: Full weekday name
            // %U: Week of year as decimal number, with Sunday as first day of week (00 – 53)
            // %W: Week of year as decimal number, with Monday as first day of week (00 – 53)
            case _T('d'): // %d: Day of month as decimal number (01 – 31)
                aParameter.push_back(TP::DayOfMonth);
                aTimeStringLength += 2;
                break;
            case _T('j'): // %: Day of year as decimal number (001 – 366)
                aParameter.push_back(TP::DayOfYear);
                aTimeStringLength += 2;
                break;
            case _T('w'): // %w: Day of week as decimal number (0 - 6)
                aParameter.push_back(TP::DayOfWeek);
                aTimeStringLength += 2;
                break;
            case _T('U'): // %j: Week of year as decimal number (00 – 53)
                aParameter.push_back(TP::WeekOfYear);
                aTimeStringLength += 2;
                break;
            case _T('H'): // %H: Hour in 24-hour format (00 – 23)
                aParameter.push_back(TP::Hour);
                aTimeStringLength += 2;
                break;
            case _T('I'): // %I: Hour in 12-hour format (01 – 12) TODO! 
                aParameter.push_back(TP::hour);
                aTimeStringLength += 2;
                break;
            case _T('m'): // %m: Month as decimal number (01 – 12)
                aParameter.push_back(TP::Month);
                aTimeStringLength += 2;
                break;
            case _T('b'): // %b: Abbreviated month name TODO!
                fSign = _T('s');
                aParameter.push_back(TP::AbrMonthName);
                aTimeStringLength += 3;
                break;
            case _T('B'): // %B: Full month name TODO!
                fSign = _T('s');
                aParameter.push_back(TP::MonthName);
                aTimeStringLength += 5;
                break;
            case _T('M'): // %M: Minute as decimal number (00 – 59)
                aParameter.push_back(TP::Minute);
                aTimeStringLength += 2;
                break;
            case _T('S'): // %S: Second as decimal number (00 – 59)
                aParameter.push_back(TP::Second);
                aTimeStringLength += 2;
                break;
            case _T('s'): // %s: mili seconds 
                aParameter.push_back(TP::MiliSecond);
                aTimeStringLength += 3;
                break;
            case _T('u'): // %u: micro seconds 
                aParameter.push_back(TP::MicroSecond);
                aTimeStringLength += 3;
                break;
            case _T('y'): // %y: Year without century, as decimal number (00 – 99)
                aParameter.push_back(TP::year);
                aTimeStringLength += 2;
                break;
            case _T('Y'): // %Y: Year with century, as decimal number
                aParameter.push_back(TP::Year);
                aTimeStringLength += 4;
                break;
            case _T('p'): // %p: Current locale's A.M./P.M. indicator for 12-hour clock TODO!
                fSign = _T('s');
                aParameter.push_back(TP::PM);
                aTimeStringLength += 2;
                break;
            case _T('%'): // %%: Percent sign
                fSign = _T('%');
                ++aTimeStringLength;
                break;
            }
            aFmt += _T("%");
            aFmt += fSign;
        }
        else
        {
           aFmt += fC;
           ++aTimeStringLength;
        }
    }
}

void CLogViewView::DetermineTimeType()
{
    CStringArray fFormats;
    CString      fFmt;
    int          fTSL;
    std::vector<int> fTimeStringLength;
    std::vector<int> fCounts;
    std::vector<int> fP;
    std::vector< std::vector<int> > fParameter;
    fFormats.Append(theApp.mFormats);

    for (int i=0; i<fFormats.GetCount(); ++i)
    {
        fTSL = 0;
        fFmt.Empty();
        fP.clear();
        ParseFormat(fFormats[i], fFmt, fP, fTSL);
        fFormats[i] = fFmt;
        fCounts.push_back(fP.size());
        fTimeStringLength.push_back(fTSL);
        fParameter.push_back(fP);
    }

    CRichEditCtrl& editControl = GetRichEditCtrl();
    int nLines = min(editControl.GetLineCount(), 20);
    for (int nLine=0; nLine<nLines; nLine++)
    {
        int nLineIndex = editControl.LineIndex(nLine);
        int nLineLength = editControl.LineLength(nLineIndex);
        CString sLine;
        editControl.GetLine(nLine, sLine.GetBufferSetLength(nLineLength), nLineLength);
        sLine.ReleaseBuffer(nLineLength);
        int i;

        for (i=0; i<fFormats.GetCount(); i++)
        {
            int fParams = min(TP::Count,fParameter[i].size());
            for (int j=0; j<fParams; j++)
            {
                mPtr[j] = &mP[j];
                switch (fParameter[i][j])
                {
                case TP::PM: case TP::AbrMonthName: case TP::MonthName:
                        mPtr[j] = mStringParam[j].GetBufferSetLength(TP::StringBuffer);
                        mStringParam[j].ReleaseBufferSetLength(TP::StringBuffer);
                        break;
                }
            }
            int nCount = _stscanf(sLine, fFormats[i], mPtr[0], mPtr[1], mPtr[2], mPtr[3], mPtr[4], mPtr[5], mPtr[6], mPtr[7], mPtr[8], mPtr[9]);
            if (nCount > 0 && nCount == fCounts[i])
            {
                break;
            }
        }
        if (i < fFormats.GetCount())
        {
            mTimeParamter   = fParameter[i];
            mTimeFormat     = fFormats[i];
            mTimeCharacters = fTimeStringLength[i];

            long days = 0;
            long seconds = 0;
            if (GetTimeOfLine(nLine, days, seconds))
            {
                if (days != 0)
                {
                    CTime date;
                    CMainFrame::fromDays(days, date);
                    CWnd*pWnd = theApp.GetMainWnd();
                    ASSERT_KINDOF(CMainFrame, pWnd);
                    ((CMainFrame*)pWnd)->GetDateCtrl()->SetTime(&date);
                }
                if (seconds != 0)
                {
                    CTime time;
                    CMainFrame::fromSeconds(seconds, time);
                    CWnd*pWnd = theApp.GetMainWnd();
                    ASSERT_KINDOF(CMainFrame, pWnd);
                    ((CMainFrame*)pWnd)->GetTimeCtrl()->SetTime(&time);
                }
            }
            break;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
BOOL CLogViewView::GetTimeOfLine(int nLine, long& nDay, long &nSec)
{
    CRichEditCtrl& editControl = GetRichEditCtrl();
    int nLength = editControl.LineLength(nLine);
    if (mTimeCharacters > 0 && nLength > mTimeCharacters)
    {
        CString sLine;
        nLength = mTimeCharacters;
        nLength = editControl.GetLine(nLine, sLine.GetBufferSetLength(nLength), nLength);
        sLine.ReleaseBuffer(nLength);
        int nCount = 0;
        nCount = _stscanf(sLine, mTimeFormat, mPtr[0], mPtr[1], mPtr[2], mPtr[3], mPtr[4], mPtr[5], mPtr[6], mPtr[7], mPtr[8], mPtr[9]);
        nDay = 0;
        nSec = 0;
        if (nCount == static_cast<int>(mTimeParamter.size()))
        {
            for (int i=0; i<nCount; ++i)
            {
                switch(mTimeParamter[i])
                {
                case TP::Year:  nDay += ((mP[i] - 2000) * 365); break;
                case TP::year:  // without century
                    if (mP[i] < 70)  nDay +=   mP[i] * 365;          // after  year 2000
                    else             nDay += ((mP[i] - 100) * 365);  // before year 2000
                     break;
                case TP::Month:      nDay += ((mP[i]-1) * 31); break;
                case TP::WeekOfYear: nDay += ((mP[i]-1) *  7); break;
                case TP::DayOfMonth: nDay += mP[i]; break;
                case TP::DayOfYear:  nDay += mP[i]; break;
                case TP::DayOfWeek:  nDay += mP[i]; break;

                case TP::Hour:       nSec += (mP[i] * 3600000); break;
                case TP::hour:  // correct 12 hour system by AM and PM
                {
                    //int val = TP::PM;
                    //std::vector<int>::iterator fPM = std::find(mTimeParamter.begin(), mTimeParamter.end(), val);
                    //if (fPM != mTimeParamter.end())
                    //{
                    //    int n = fPM - mTimeParamter.begin();
                    //    if (mP[n]
                    //}
                    nSec += (mP[i] * 3600000);
                } break;
                case TP::Minute:      nSec += (mP[i] *   60000); break;
                case TP::Second:      nSec += (mP[i] *    1000); break;
                case TP::MiliSecond:  nSec += mP[i]; break;
                case TP::MicroSecond: nSec += mP[i]/1000; break;
                }
            }
            return TRUE;
        }
    }
    return nLength == 0 ? -1 : FALSE;
}

/////////////////////////////////////////////////////////////////////////////
void CLogViewView::OnEditFind()
{
#ifdef _DEBUG
    OnEditFindReplace(TRUE);
#else
    CRichEditView::OnEditFindReplace(TRUE);
#endif
}
/////////////////////////////////////////////////////////////////////////////
CString CLogViewView::GetSelText(HWND hWnd) const
{
    ASSERT(::IsWindow(hWnd));
    CHARRANGE cr;
    cr.cpMin = cr.cpMax = 0;
    ::SendMessage(hWnd, EM_EXGETSEL, 0, (LPARAM)&cr);
    LPTSTR lpsz = (LPTSTR)_alloca((cr.cpMax - cr.cpMin + 1)*sizeof(TCHAR));
    lpsz[0] = NULL;
    ::SendMessage(hWnd, EM_GETSELTEXT, 0, (LPARAM)lpsz);
    return CString(lpsz);
}
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CLogViewView::OnEditFindReplace(BOOL bFindOnly)
{
    ASSERT_VALID(this);
    m_bFirstSearch = TRUE;

    _AFX_RICHEDIT_STATE* pEditState = GetRichEditState();

    if (pEditState->pFindReplaceDlg != NULL)
    {
        if (pEditState->bFindOnly == bFindOnly)
        {
            pEditState->pFindReplaceDlg->SetActiveWindow();
            pEditState->pFindReplaceDlg->ShowWindow(SW_SHOW);
            return;
        }
        else
        {
            ASSERT(pEditState->bFindOnly != bFindOnly);
            pEditState->pFindReplaceDlg->SendMessage(WM_CLOSE);
            ASSERT(pEditState->pFindReplaceDlg == NULL);
            ASSERT_VALID(this);
        }
    }
    
    CString strFind = GetSelText(GetRichEditCtrl().m_hWnd);
    // if selection is empty or spans multiple lines use old find text
    if (strFind.IsEmpty() || (strFind.FindOneOf(_T("\n\r")) != -1))
        strFind = pEditState->strFind;
    CString strReplace = pEditState->strReplace;
    pEditState->pFindReplaceDlg = new CFindReplaceDialog;
    ASSERT(pEditState->pFindReplaceDlg != NULL);
    DWORD dwFlags = NULL;
    if (pEditState->bNext)
        dwFlags |= FR_DOWN;
    if (pEditState->bCase)
        dwFlags |= FR_MATCHCASE;
    if (pEditState->bWord)
        dwFlags |= FR_WHOLEWORD;

    if (!pEditState->pFindReplaceDlg->Create(bFindOnly, strFind,
        strReplace, dwFlags, this))
    {
        pEditState->pFindReplaceDlg = NULL;
        ASSERT_VALID(this);
        return;
    }
    ASSERT(pEditState->pFindReplaceDlg != NULL);
    pEditState->bFindOnly = bFindOnly;
    pEditState->pFindReplaceDlg->SetActiveWindow();
    pEditState->pFindReplaceDlg->ShowWindow(SW_SHOW);
    ASSERT_VALID(this);
}
#endif
/////////////////////////////////////////////////////////////////////////////
void CLogViewView::OnEditSearchInCompressed()
{
    m_bSearchInCompressed = TRUE;
    GetDocument()->StartSearchInCompressedThread(this);
#ifndef _THREAD_SEARCH
    if (m_bFoundInCompressed == FALSE)
    {
        CRichEditView::OnTextNotFound(m_sFindText);
    }
    m_bSearchInCompressed = FALSE;
#endif
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLogViewView::OnEditRepeat()
{
    if (m_bSearchInCompressed)
    {
        m_bFoundInCompressed = TRUE;
    }
    CRichEditView::OnEditRepeat();
    return m_bFoundInCompressed;
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewView::OnTextNotFound(LPCTSTR lpszFind)
{
    if (m_bSearchInCompressed)
    {
        m_bFoundInCompressed = FALSE;
    }
    else
    {
        CRichEditView::OnTextNotFound(lpszFind);
        m_nCurrentItem = -1;
    }
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewView::OnFindNext(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord)
{
    m_bNext = bNext;
    m_sFindText = lpszFind;
    CRichEditView::OnFindNext(lpszFind, bNext, bCase, bWord);
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewView::OnTimer(UINT nIDEvent)
{
    BOOL bSelect = FALSE;
    BOOL bSetFormat = FALSE;
    BOOL bMakeBold = FALSE;
    BOOL bUpdateBar = FALSE;
    BOOL bUnSelect = FALSE;
    switch (nIDEvent)
    {
        case EVENT_UPDATE_SELECTION:
            bSelect = bUpdateBar = TRUE;
            break;
        case EVENT_SELECTION:
            bSelect = TRUE;
            break;
        case EVENT_SELECT_BOOKMARK:
            bSelect = bSetFormat = bMakeBold = TRUE;
            break;
        case EVENT_UNSELECT_BOOKMARK:
            bSelect = bSetFormat = bUnSelect = TRUE;
            break;
    }
    if (bSelect)
    {
        CRichEditCtrl& editControl = GetRichEditCtrl();
        editControl.SetSel(m_crPostSelection);
        if (bSetFormat)
        {
            CHARFORMAT chfmt;
            chfmt.cbSize        = sizeof(chfmt);
            if (bMakeBold)
            {
                chfmt.dwMask    = CFM_BOLD|CFM_COLOR;
                chfmt.dwEffects = CFM_BOLD;
                chfmt.crTextColor = RGB(255, 0,0);
            }
            else
            {
                chfmt.dwMask    = CFM_BOLD|CFM_COLOR;
                chfmt.dwEffects = CFE_AUTOCOLOR;
            }
            editControl.SetSelectionCharFormat(chfmt);
        }
        if (bUnSelect)
        {
            m_crPostSelection.cpMax = m_crPostSelection.cpMin;
            editControl.SetSel(m_crPostSelection);
        }
        CStatusBar *pBar = NULL;
        if (bUpdateBar)
        {
            pBar = ((CMainFrame*)AfxGetMainWnd())->GetStatusBar();
        }
        if (pBar)
        {
            CString sName, sSel, sText;
            GetParent()->GetWindowText(sName);
            CHARRANGE cr;
            cr.cpMin = cr.cpMax = 0;
            ::SendMessage(editControl.m_hWnd, EM_EXGETSEL, 0, (LPARAM)&cr);
            LPTSTR lpsz = sSel.GetBufferSetLength(cr.cpMax - cr.cpMin + 1);
            lpsz[0] = NULL;
            ::SendMessage(editControl.m_hWnd, EM_GETSELTEXT, 0, (LPARAM)lpsz);
            sSel.ReleaseBuffer();
            int nLine = editControl.LineFromChar(m_crPostSelection.cpMin);
            sText.Format(_T("%s(%d): %s\n"), sName, nLine, sSel);
            pBar->SetPaneText(0, sText);
        }
    }

    CRichEditView::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewView::OnUpdateSC_CLOSE(CCmdUI *pCmdUI)
{
    BOOL bEnable = TRUE;
    CMainFrame*pMF = (CMainFrame*)AfxGetMainWnd();
    if (pMF->IsSyncWindowThreadRunning())
    {
        pMF->StopSyncWindowThread();
        bEnable = FALSE;
    }
    if (GetDocument()->IsSearchThreadRunning())
    {
        GetDocument()->StopSearchThread();
        bEnable = FALSE;
    }
    pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewView::OnUpdateViewShowSyncpoint(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(m_crPostSelection.cpMax != 0);
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewView::OnViewShowSyncpoint()
{
    OnTimer(EVENT_UPDATE_SELECTION);
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewView::OnUpdateEditSearchTimeValue(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewView::OnEditSearchTimeValue()
{
    CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
    if (pMF)
    {
        pMF->OnEditSearchTimeValue(this);
    }
}
/////////////////////////////////////////////////////////////////////////////
int __cdecl CLogViewView::SortCharrangePtr(const void *p1, const void *p2)
{
    CHARRANGE*pc1 = (CHARRANGE*)((INT_PTR*)p1)[0];
    CHARRANGE*pc2 = (CHARRANGE*)((INT_PTR*)p2)[0];
    if   (pc1->cpMin < pc2->cpMin) return -1;
    else    return 1;
}
/////////////////////////////////////////////////////////////////////////////
CHARRANGE CLogViewView::GetTimePositionOfLine(long *pnLine/*=NULL*/)
{
    CRichEditCtrl& editControl = GetRichEditCtrl();
    long nStartChar, nEndChar, nLine = 0;
    if (pnLine && *pnLine != -1)
    {
        nLine = *pnLine;
        pnLine = NULL;
    }
    else
    {
        editControl.GetSel(nStartChar, nEndChar);
        nLine = editControl.LineFromChar(nStartChar);
    }
    CHARRANGE cr;
    cr.cpMin = editControl.LineIndex(nLine);
    cr.cpMax = cr.cpMin + mTimeCharacters;
    if (pnLine)
    {
        *pnLine = nLine;
    }
    return cr;
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewView::OnViewSetBookMark()
{
    CHARRANGE cr = GetTimePositionOfLine(),
            *pcr;
    BOOL bInsert = TRUE;

    int i, nCount = m_BookMarks.GetCount();
    for (i=0; i<nCount; i++)
    {
        pcr = (CHARRANGE*) m_BookMarks.GetAt(i);
        if (pcr->cpMin == cr.cpMin)    // löschen
        {
            swap(*pcr, m_crPostSelection);
            OnTimer(EVENT_UNSELECT_BOOKMARK);
            swap(*pcr, m_crPostSelection);
            m_BookMarks.RemoveAt(i);
            delete pcr;
            bInsert = FALSE;
            break;
        }
    }
    if (bInsert)
    {
        pcr = new CHARRANGE;
        *pcr = cr;
        m_BookMarks.Add(pcr);
        qsort(m_BookMarks.GetData(), m_BookMarks.GetCount(), sizeof(CHARRANGE*), SortCharrangePtr);
        swap(cr, m_crPostSelection);
        OnTimer(EVENT_SELECT_BOOKMARK);
        swap(cr, m_crPostSelection);
    }

}
/////////////////////////////////////////////////////////////////////////////
void CLogViewView::OnUpdateViewSetBookMark(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewView::OnViewNextBookmark()
{
    CHARRANGE cr = GetTimePositionOfLine(),
            *pcr1, *pcr2 = NULL;

    int i, nCount = m_BookMarks.GetCount();
    for (i=0; i<nCount-1; i++)
    {
        pcr1 = (CHARRANGE*) m_BookMarks.GetAt(i);
        pcr2 = (CHARRANGE*) m_BookMarks.GetAt(i+1);
        if (IsBetween(cr.cpMin, pcr1->cpMin, pcr2->cpMin-1))
        {
            break;
        }
        pcr2 = NULL;
    }
    if (pcr2 == NULL)
    {
        pcr2 = (CHARRANGE*)m_BookMarks.GetAt(0);
    }
    if (pcr2)
    {
        swap(*pcr2, m_crPostSelection);
        OnTimer(EVENT_UPDATE_SELECTION);
        swap(*pcr2, m_crPostSelection);
    }
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewView::OnUpdateViewNextBookmark(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(m_BookMarks.GetCount() > 0);
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewView::OnViewPrevBookmark()
{
    CHARRANGE cr = GetTimePositionOfLine(),
            *pcr1 = NULL, *pcr2;

    int i, nCount = m_BookMarks.GetCount();
    for (i=0; i<nCount-1; i++)
    {
        pcr1 = (CHARRANGE*) m_BookMarks.GetAt(i);
        pcr2 = (CHARRANGE*) m_BookMarks.GetAt(i+1);
        if (IsBetween(cr.cpMin, pcr1->cpMin+1, pcr2->cpMin))
        {
            break;
        }
        pcr1 = NULL;
    }
    if (pcr1 == NULL)
    {
        pcr1 = (CHARRANGE*)m_BookMarks.GetAt(nCount-1);
    }
    if (pcr1)
    {
        swap(*pcr1, m_crPostSelection);
        OnTimer(EVENT_UPDATE_SELECTION);
        swap(*pcr1, m_crPostSelection);
    }
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewView::OnUpdateViewPrevBookmark(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(m_BookMarks.GetCount() > 0);
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewView::OnViewDeleteBookmarks()
{
    int i, nCount = m_BookMarks.GetCount();
    for (i=0; i<nCount; i++)
    {
        CHARRANGE* pcr = (CHARRANGE*) m_BookMarks.GetAt(i);
        if (m_hWnd)
        {
            swap(*pcr, m_crPostSelection);
            OnTimer(EVENT_UNSELECT_BOOKMARK);
            swap(*pcr, m_crPostSelection);
        }
        delete pcr;
    }
    m_BookMarks.RemoveAll();
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewView::OnUpdateViewDeleteBookmarks(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(m_BookMarks.GetCount());
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CLogViewView::OnUser(WPARAM wParam, LPARAM lParam)
{
    if (LOWORD(wParam) == WM_COMMAND && HIWORD(wParam) == ID_EDIT_REPEAT)
    {
        BOOL *pbResult = (BOOL*)lParam;
        BOOL bresult = OnEditRepeat();
        if (bresult)
        {
            *pbResult = TRUE;
        }
    }
    return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewView::DeleteContentThreadSave()
{
    SetWindowText(_T(""));
    GetRichEditCtrl().EmptyUndoBuffer();
    m_bSyncCharFormat = m_bSyncParaFormat = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewView::EndSearchInCompressedThread()
{
    if (m_bFoundInCompressed == FALSE)
    {
        CRichEditView::OnTextNotFound(m_sFindText);
    }
    m_bSearchInCompressed = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewView::OnUpdateEditFind(CCmdUI *pCmdUI)
{
    if (GetDocument()->IsSearchThreadRunning())
    {
        pCmdUI->Enable(FALSE);
    }
    else
    {
        pCmdUI->Enable(GetWindowTextLength() != 0);
    }

}
/////////////////////////////////////////////////////////////////////////////
class ReadUnicodeCookie
{
public:
    CArchive&    m_ar;
    DWORD        m_dwError;
    BOOL        m_bDecode;
    DWORD        m_dwBytesRead;
    UINT        m_nCodePage;

    ReadUnicodeCookie(CArchive& ar) : m_ar(ar)
    {
        m_dwError=0;
        m_bDecode = TRUE;
        m_dwBytesRead = 0;
        m_nCodePage = CP_ACP;
    }
    void ExchangeBytes(BYTE*pBuff, int n);
};
void ReadUnicodeCookie::ExchangeBytes(BYTE*pBuff, int n)
{
    int i;
    for (i=0; i<n-1; i+=2)
    {
        swap(pBuff[i], pBuff[i+1]);
    }
}
/////////////////////////////////////////////////////////////////////////////
DWORD CALLBACK CLogViewView::ReadUnicodeStreamCallBack(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
    ReadUnicodeCookie* pCookie = (ReadUnicodeCookie*)dwCookie;
    CArchive& ar = pCookie->m_ar;
    ar.Flush();
    DWORD dw = 0;
    *pcb = cb;

    TRY
    {
        if (ar.IsStoring())
        {
            ASSERT(FALSE);
            ar.GetFile()->Write(pbBuff, cb);
        }
        else if (pCookie->m_nCodePage == (UINT)-1)
        {
            *pcb = ar.GetFile()->Read(pbBuff, cb);
        }
        else if (pCookie->m_nCodePage == (UINT)-2)
        {
            *pcb = ar.GetFile()->Read(pbBuff, cb);
            pCookie->ExchangeBytes(pbBuff, cb);
        }
        else
        {
            int nRead = cb / 2;
            BYTE*pRead = (BYTE*)_alloca(nRead);
            nRead = ar.GetFile()->Read(pRead, nRead);
            if (nRead)
            {
                int nStart = 0;
                if (pCookie->m_dwBytesRead == 0)
                {
                    if (   *((WORD*)pRead) == TEXTIO_UNICODE)
                    {
                        pCookie->m_nCodePage = (UINT)-1;
                    }
                    else if(*((WORD*)pRead) == TEXTIO_UNICODE_BIGENDIAN)
                    {
                        pCookie->m_nCodePage = (UINT)-2;
                    }
                    else if (pRead[0] == 0xff && pRead[1] == 0xff)
                    {
                        pCookie->m_nCodePage = MAKEWORD(pRead[2], pRead[3]);
                        nStart = 4;
                    }
                    else if (pRead[0] == 0xef && pRead[1] == 0xbb && pRead[2] == 0xbf)
                    {
                        pCookie->m_nCodePage = CP_UTF8;
                        nStart = 3;
                    }
                }
                if (pCookie->m_bDecode)
                {
                    Decode(pRead, nRead, TRUE);
                }
                if (pCookie->m_nCodePage != (UINT)-1 && pCookie->m_nCodePage != (UINT)-2)
                {
                    *pcb = MultiByteToWideChar(pCookie->m_nCodePage, 0, (LPSTR)&pRead[nStart], nRead-nStart, (LPWSTR)pbBuff, cb)*2;
                }
                else
                {
                    memcpy(pbBuff, &pRead[2], nRead-2);
                    if (pCookie->m_nCodePage == (UINT)-2)
                    {
                        pCookie->ExchangeBytes(pbBuff, nRead-2);
                    }
                    *pcb = nRead-2;
                }
            }
            else
            {
                *pcb = 0;
            }
        }
    }
    CATCH(CFileException, e)
    {
        *pcb = 0;
        pCookie->m_dwError = (DWORD)e->m_cause;
        dw = 1;
        delete (e);
    }
    AND_CATCH_ALL(e)
    {
        *pcb = 0;
        pCookie->m_dwError = (DWORD)CFileException::generic;
        dw = 1;
        delete (e);
    }
    END_CATCH_ALL
    pCookie->m_dwBytesRead += *pcb;
    return dw;
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewView::Stream(CArchive& ar, BOOL bSelection)
{
    EDITSTREAM es = {0, 0, ReadUnicodeStreamCallBack};
    ReadUnicodeCookie cookie(ar);
    cookie.m_nCodePage = CWK_String::GetCodePage();
    CString sPath = ar.GetFile()->GetFilePath();
    sPath.MakeLower();
    if (sPath.Find(TEMPORARY_LOGFILE) != -1)
    {
        cookie.m_bDecode = FALSE;
    }
    else if (sPath.Find(PING_LOG) != -1)
    {
        cookie.m_bDecode = FALSE;
    }
    else if (sPath.Find(_T(".txt")) != -1)
    {
        cookie.m_bDecode = FALSE;
    }
    es.dwCookie = (DWORD_PTR)&cookie;
    int nFormat = GetDocument()->GetStreamFormat()|SF_UNICODE;
    if (bSelection)
        nFormat |= SFF_SELECTION;
    if (ar.IsStoring())
    {
        ASSERT(FALSE);
        GetRichEditCtrl().StreamOut(nFormat, es);
    }
    else
    {
        GetRichEditCtrl().StreamIn(nFormat, es);
        Invalidate();
    }
    if (cookie.m_dwError != 0)
        AfxThrowFileException(cookie.m_dwError);
}

void CLogViewView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    CView::OnUpdate(pSender, lHint, pHint);
}
