// SecAnalyzerView.cpp : implementation of the CSecAnalyzerView class
//

#include "stdafx.h"
#include "SecAnalyzer.h"

#include "MainFrm.h"
#include "SecAnalyzerDoc.h"
#include "SecAnalyzerView.h"
#include "Analyze.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSecAnalyzerView

IMPLEMENT_DYNCREATE(CSecAnalyzerView, CScrollView)

BEGIN_MESSAGE_MAP(CSecAnalyzerView, CScrollView)
	//{{AFX_MSG_MAP(CSecAnalyzerView)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_START_POINT, OnStartPoint)
	ON_COMMAND(ID_END_POINT, OnEndPoint)
	ON_COMMAND(ID_NOP, OnNop)
	ON_COMMAND(ID_POPUP_EXCLUDE, OnPopupExclude)
	ON_COMMAND(ID_POPUP_INCLUDE, OnPopupInclude)
	ON_COMMAND(ID_COLORED_BACKGROUND, OnColoredBackground)
	ON_UPDATE_COMMAND_UI(ID_COLORED_BACKGROUND, OnUpdateColoredBackground)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSecAnalyzerView construction/destruction


static COLORREF buttonColors[]= {
		RGB(0,64,0),
		RGB(0,0,255),
		RGB(64,64,64),
		RGB(128,128,128),
};

static COLORREF colors[]= {
	RGB(0,0,0),
	RGB(128,150,128),
	RGB(255,0,0),
	RGB(0,64,0),
	RGB(0,0,255),
};
static int numColors = 5; //sizeof(colors[]);


CSecAnalyzerView::CSecAnalyzerView()
{
	// TODO: add construction code here
	m_bDoColoredBackground=TRUE;	// OOPS handcrafted member
	m_startTime = CTime::GetCurrentTime();	// init dummies
	m_endTime = CTime::GetCurrentTime();
}

CSecAnalyzerView::~CSecAnalyzerView()
{
}

BOOL CSecAnalyzerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSecAnalyzerView drawing
static void SwapPoints(POINT pPoints[], int num)
{
	for (int i=0;i<num;i++) {
		POINT p=pPoints[i];
		pPoints[i].x=p.y;
		pPoints[i].y=p.x;
	}
}

void CSecAnalyzerView::DrawButtons(CDC* pDC)
{
#if 0
	CRect clientRect;
	GetClientRect(clientRect);
	// double f = (clientRect.Height() / 310.0);
	// double f = (clientRect.Width() / 700.0);

	// DOES NOT WORK pDC->Draw3dRect( 10,10, 30,30,  colors[0],colors[1] );
	POINT triPoints[4];
	triPoints[0].x=clientRect.TopLeft().x+10;
	triPoints[0].y=10;
	triPoints[1]=triPoints[0];
	triPoints[1].x += 5;
	triPoints[1].y += 5;
	triPoints[2]=triPoints[0];
	triPoints[2].x -= 5;
	triPoints[2].y += 5;
	triPoints[3]=triPoints[0];

	BOOL bDraw = pDC->Polyline( triPoints, 4);
#endif
}

void CSecAnalyzerView::MapToScreen(int x,int y, int &xOut, int &yOut) const
{
}

void CSecAnalyzerView::OnDraw(CDC* pDC)
{
	BeginWaitCursor();

	CSecAnalyzerDoc* pDoc = GetDocument();
	if (pDoc==NULL) {
		WK_TRACE_ERROR(_T("No doc !?\n"));
		return;
	}
	ASSERT_VALID(pDoc);

	pDC->SetMapMode(MM_TEXT);

	if (m_bDoColoredBackground) {
		DrawBackground(pDC);
	}

	DWORD startTick = pDoc->m_startTick;
	const CStatRecords &records = *pDoc->m_pRecords;

	CPoint origin((DWORD)(m_dScaleX*150),(DWORD)(m_dScaleY*300));
	DWORD width = m_dwWidth;
	DWORD height=(DWORD)(m_dScaleY*280);
	int fontSizeHeader = 12;
	DWORD dwSmallStep=5;
	if (pDC->IsPrinting()) {
		int deviceWidth = pDC->GetDeviceCaps(HORZRES);
		int deviceHeight= pDC->GetDeviceCaps(VERTRES);

		double fx=3.0;
		double fy=10.0;

		fx = deviceWidth / 700.0;
		fy = deviceHeight / 310.0;
		
		width = (int)(width * fx);
		height = (int)(height *fy);
		origin.x = (int) (width*0.15);// (int)(origin.x*fx);
		origin.y = (int)(height*0.95);
		dwSmallStep =(DWORD)(dwSmallStep*fy/2.0);
		fontSizeHeader = dwSmallStep;	// OOPS
	} else {
		CRect clientRect;
		GetClientRect(clientRect);
		double f = 0;
		f= (clientRect.Height() / 310.0);
		height = (DWORD) (height * f);
		origin.y = (DWORD) (282*f);
		f = (clientRect.Width() / 700.0);
		width  = (DWORD) (width * f);
		DrawButtons(pDC);
	}

	DWORD dwValueStep = (DWORD)(0.9*((height / max(1,records.GetSize()))/2));
	DWORD deltaY = height / (max(1,records.GetSize()));

	if (dwValueStep<1) {
		dwValueStep=1;
	}
	if (deltaY<1) {
		deltaY=1;
		// OOPS or adjust sizes
	}

	// draw axis
	POINT axisPoints[2];
	axisPoints[0]=origin;
	axisPoints[1]=origin;
	axisPoints[1].y -= height;
	axisPoints[0].y += dwSmallStep;	//lower, to make first line visible
	BOOL bDraw = pDC->Polyline( axisPoints, 2);
	axisPoints[0]=origin;
	axisPoints[1]=origin;
	axisPoints[1].x += width;
	axisPoints[0].y += dwSmallStep;
	axisPoints[1].y += dwSmallStep;
	bDraw = pDC->Polyline( axisPoints, 2);

	TEXTMETRIC tm;      // get size of font
	CFont	MyFont;
	CFont* pOldFont = NULL;
	LOGFONT logfont;
	memset(&logfont, 0, sizeof(logfont));
	_tcscpy(logfont.lfFaceName, _T("Arial"));        // TrueType font
	pDC->GetTextMetrics(&tm);
	
	logfont.lfHeight = fontSizeHeader;
	MyFont.CreateFontIndirect(&logfont);
	pOldFont = pDC->SelectObject(&MyFont);

	// draw record labels
	CString str;
	for (int i=0;i<records.GetSize();i++) {
		const CStatRecord  *pRecord = records[i];
		DWORD dwMin = pRecord->m_dwMinValue;
		DWORD dwMax = pRecord->m_dwMaxValue;
		// show min/man if applicable (exclude process min/max logs)
		if (dwMax>5) {
			str.Format(_T("%s[%d,%d,%d]"),records[i]->GetName(),
				dwMin,dwMax,records[i]->GetAverage());
		} else {
			str = records[i]->GetName();
		}
		if (str.GetLength()<3) {
			TRACE(_T("Name '%s'\n"),str);
		}
		if (! pDC->IsPrinting()) {	// only modify colors if not printing
			pDC->SetTextColor(colors[i % numColors]);
		}
		pDC->TextOut(10,origin.y-deltaY*i-10,str);

		axisPoints[0]=origin;
		axisPoints[1]=origin;
		axisPoints[1].x += width;
		axisPoints[0].y -= i*deltaY;
		axisPoints[1].y -= i*deltaY;

		CPen colorPen(PS_DOT,1,colors[i %numColors]);
		CPen *oldPen = NULL;
		if (! pDC->IsPrinting()) {	// only modify colors if not printing
			oldPen = pDC->SelectObject(&colorPen);
		} else {
			// printing
		}
		bDraw = pDC->Polyline( axisPoints,2);
		if (! pDC->IsPrinting()) {	// only modify colors if not printing
			pDC->SelectObject(oldPen);
		} else {
			// printing
		}
	} // loop over all records
	pDC->SetTextColor(RGB(0,0,0));
	
	if (records.GetSize()==0) {
		EndWaitCursor();
		return;	// <<< EXIT >>> give up here
	}

	records.GetMinMax(m_startTime,m_endTime);
	// NEW 240497 check for caculated time
	// not coming from the record themselfs
	if (pDoc) {
		if (pDoc->m_bUseStartTime) {
			m_startTime = pDoc->m_startTime;
		}
		if (pDoc->m_bUseEndTime) {
			m_endTime = pDoc->m_endTime;
		}
	}
	
	int labelOffset = dwSmallStep*2;
	int labelOffset2 = dwSmallStep*4;
	CString sTime;
	sTime = m_startTime.Format("%H:%M:%S");	// OOPS CTime.Format
	pDC->TextOut(origin.x,origin.y+labelOffset,sTime);
	sTime = m_startTime.Format("%d.%m.%y");
	pDC->TextOut(origin.x,origin.y+labelOffset2,sTime);

	sTime = m_endTime.Format("%H:%M:%S"); // OOPS CTime.Format
	pDC->TextOut(origin.x+width,origin.y+labelOffset,sTime);
	sTime = m_endTime.Format("%d.%m.%y");
	pDC->TextOut(origin.x+width,origin.y+labelOffset2,sTime);

	const int numXSteps=10;
	DWORD dwTotalSeconds = (m_endTime-m_startTime).GetTotalSeconds();
	DWORD deltaSeconds = dwTotalSeconds/numXSteps;
	double msTotal = dwTotalSeconds*1000.0;

	BOOL bMoreThanOneDay = 
		(m_startTime.GetDay()!=m_endTime.GetDay())
		||
		(m_startTime.GetDayOfWeek()!=m_endTime.GetDayOfWeek());

	BOOL bMoreThanOneYear = (m_startTime.GetYear()!=m_endTime.GetYear());

	// deltaSeconds -= (deltaSeconds % 60);
	CPen dotPen(PS_DOT,1,RGB(0,0,0));
	CPen *oldPen = pDC->SelectObject(&dotPen);
	CTime xTime=m_startTime;

	// draw x-axis labels
	for (i=1;i<numXSteps;i++) {
		xTime = xTime + CTimeSpan(0,0,0,deltaSeconds);
		sTime = xTime.Format("%H:%M:%S");
		pDC->TextOut(origin.x+i*width/numXSteps,origin.y+labelOffset,sTime);

		if (bMoreThanOneYear) {
			sTime = xTime.Format("%d.%m.%y");
			pDC->TextOut(origin.x+i*width/numXSteps,origin.y+labelOffset2,sTime);
		} else if (bMoreThanOneDay) {	// print date too
			sTime = xTime.Format("%d.%m.");
			pDC->TextOut(origin.x+i*width/numXSteps,origin.y+labelOffset2,sTime);
		}

		pDC->MoveTo(origin.x+i*width/numXSteps,origin.y);
		pDC->LineTo(origin.x+i*width/numXSteps,origin.y-height);
	}

	CTimeSpan totalTime = m_endTime-m_startTime;
	if (deltaSeconds < 60 ) {
		sTime.Format(_T("%d sec per intervall"),deltaSeconds);
	} else if (deltaSeconds < 60*60) {
		sTime.Format(_T("%d min %d sec per intervall"),
						deltaSeconds/60, deltaSeconds % 60);
	} else {
		if ( totalTime.GetDays()>10) {
			sTime.Format(_T("%d days per intervall"),totalTime.GetDays()/10);
		} else {
			sTime.Format(_T("%d hours %d min per intervall"),
						deltaSeconds/(60*60), (deltaSeconds % (60*60))/60);
		}
	}
	pDC->TextOut(20,origin.y+20,sTime);
	pDC->SelectObject(oldPen);
		
	pDC->SelectObject(pOldFont);
	MyFont.DeleteObject();

	// draw all records
	const iMaxPoints = 100;
	for (i=0;i<records.GetSize();i++) {
		const CStatRecord &s=*records[i];
		CPoint *pPoints = new CPoint[iMaxPoints];
		int numPoints=0;
		pPoints[0].x=origin.x;
		pPoints[0].y=origin.y-i*deltaY;
		numPoints++;
		DWORD dwLastValue=0;
		DWORD dwMaxValue = s.m_dwMaxValue;
		//
		CPoint yPoint;
		yPoint.x=origin.x;
		yPoint.y=origin.y-i*deltaY;
		
		// TRACE(_T("Record %s\n"),s.GetName());
		for (int p=0;p<s.m_values.GetSize();p++) {
			DWORD xpos=0;
			double ms;
			if (s.m_values[p]->m_dwTick!=-1 && startTick!=-1) {
				// use the mili seconds from the data
				ms = s.m_values[p]->m_dwTick - startTick;
			} else {
				DWORD seconds = (s.m_values[p]->m_time-m_startTime).GetTotalSeconds();
				ms = seconds*1000.0;	// full second value
			}

			ms = min(ms,msTotal);	// do not exceed msTotal
			if (msTotal) {
				xpos = (DWORD) ( (width * ms)/msTotal);
			} else {
				xpos = 0;
			}
			// TRACE(_T("xpos %d at ms %.0f\n"),xpos,ms);

			CPoint newPoint(yPoint);
			newPoint.x += xpos;

			// dwValue controls the Y value of the chart
			DWORD dwValue = s.m_values[p]->m_dwValue;
			if (dwValue==0 || dwMaxValue==0) {
				// already set in yPoint ;
			} else if (dwMaxValue<3) {
				newPoint.y -= (dwValue*dwValueStep);
			} else { // scaled
				newPoint.y -= (DWORD)(2*dwValueStep*((double)dwValue / (double)dwMaxValue));
			}
			
			// stretch old value up to new position
			if (dwLastValue!=s.m_values[p]->m_dwValue
				&& newPoint.x!=pPoints[numPoints-1].x ) {
				pPoints[numPoints]=pPoints[numPoints-1];
				pPoints[numPoints].x=origin.x+xpos;
				numPoints++;
			}
			if (numPoints && newPoint!=pPoints[numPoints-1]) {
				// add unique point
				pPoints[numPoints] = newPoint;
				numPoints++;
			} else {
				// duplicate point, no need to add
			}
			// record last value
			dwLastValue=s.m_values[p]->m_dwValue;
			if (numPoints>iMaxPoints-5) {
				CPen colorPen(PS_SOLID,2,colors[i %numColors]);
				CPen *oldPen = NULL;
				

				if (! pDC->IsPrinting()) {	// only modify colors if not printing
					oldPen = pDC->SelectObject(&colorPen);
					pDC->SetTextColor(colors[i % numColors]);
				} else {
					// printing
				}
				pDC->Polyline(pPoints,numPoints);
				if (! pDC->IsPrinting()) {	// only modify colors if not printing
					pDC->SelectObject(oldPen);
				} else {
					// printing
				}
				// save last point and reset line
				pPoints[0]=pPoints[numPoints-1];
				numPoints=1;
			}
		}	// end of p loop over values

		// extend last point to the end
		pPoints[numPoints]=pPoints[numPoints-1];
		pPoints[numPoints].x=origin.x+width;
		numPoints++;

		CPen colorPen(PS_SOLID,2,colors[i %numColors]);
		CPen *oldPen = NULL;
		
		if (! pDC->IsPrinting()) {	// only modify colors if not printing
			oldPen = pDC->SelectObject(&colorPen);
			pDC->SetTextColor(colors[i % numColors]);
		} else {
			// printing
		}

		pDC->Polyline(pPoints,numPoints);

		if (! pDC->IsPrinting()) {	// only modify colors if not printing
			pDC->SelectObject(oldPen);
		} else {
			// printing
		}
	
		WK_DELETE_ARRAY(pPoints);
	} // end of main record loop


	EndWaitCursor();
}	// end of OnDraw

void CSecAnalyzerView::DrawBackground(CDC* pDC)
{
	if (pDC->IsPrinting()) {
		// only draw on screen, NOT YET printing
		return;	// < EXIT>
	}

	CSecAnalyzerDoc* pDoc = GetDocument();
//	DWORD startTick = pDoc->m_startTick;
	const CStatRecords &records = *pDoc->m_pRecords;

	CPoint origin((DWORD)(m_dScaleX*150),(DWORD)(m_dScaleY*300));
	int width = m_dwWidth;
	int height=(int)(m_dScaleY*280);
	int fontSizeHeader = 12;
	DWORD dwSmallStep=5;
	{
		CRect clientRect;
		GetClientRect(clientRect);
		double f = 0;
		f= (clientRect.Height() / 310.0);
		height = (DWORD) (height * f);
		origin.y = (DWORD) (282*f);
		f = (clientRect.Width() / 700.0);
		width = (DWORD) (width * f);
		DrawButtons(pDC);
	}

	DWORD dwValueStep = (DWORD)(0.9*((height / max(1,records.GetSize()))/2));
	DWORD deltaY = height / (max(1,records.GetSize()));

	if (dwValueStep<1) {
		dwValueStep=1;
	}
	if (deltaY<1) {
		deltaY=1;
		// OOPS or adjust sizes
	}

	records.GetMinMax(m_startTime,m_endTime);
	// NEW 240497 check for caculated time
	// not coming from the record themselfs
	if (pDoc) {
		if (pDoc->m_bUseStartTime) {
			m_startTime = pDoc->m_startTime;
		}
		if (pDoc->m_bUseEndTime) {
			m_endTime = pDoc->m_endTime;
		}
	}
	
//	int labelOffset = dwSmallStep*2;
//	int labelOffset2 = dwSmallStep*4;
	
//	const int numXSteps=10;
	CTimeSpan totalTime = m_endTime-m_startTime;
	DWORD dwTotalSeconds = totalTime.GetTotalSeconds();
//	DWORD deltaSeconds = dwTotalSeconds/numXSteps;
	double msTotal = dwTotalSeconds*1000.0;

//	BOOL bMoreThanOneDay = 
		(m_startTime.GetDay()!=m_endTime.GetDay())
		||
		(m_startTime.GetDayOfWeek()!=m_endTime.GetDayOfWeek());

	// deltaSeconds -= (deltaSeconds % 60);
	CPen dotPen(PS_DOT,1,RGB(0,0,0));
	CPen *oldPen = pDC->SelectObject(&dotPen);

	CTime xTime(
		m_startTime.GetYear(), 
		m_startTime.GetMonth(), 
		m_startTime.GetDay(),
		0,0,0);

	// loop over time axis
	int iCounter=1;	// for color switches

	CTimeSpan deltaT(1,0,0,0);
	if (totalTime.GetDays()>1) {
		deltaT = CTimeSpan(1,0,0,0);
		xTime = CTime (
			m_startTime.GetYear(), 
			m_startTime.GetMonth(), 
			m_startTime.GetDay(),
			0,0,0	// 00:00:00
			);
	} else if (totalTime.GetTotalHours()>1){
		deltaT = CTimeSpan(0,1,0,0);
		xTime = CTime (
			m_startTime.GetYear(), 
			m_startTime.GetMonth(), 
			m_startTime.GetDay(),
			m_startTime.GetHour(),
			0,0
			);

	} else if (totalTime.GetTotalMinutes()>1) {
		deltaT = CTimeSpan(0,0,1,0);
		xTime = CTime (
			m_startTime.GetYear(), 
			m_startTime.GetMonth(), 
			m_startTime.GetDay(),
			m_startTime.GetHour(),
			m_startTime.GetMinute(),
			0
			);
	} else {
		deltaT = CTimeSpan(0,0,0,1);
		xTime = m_startTime;
	}

	while (xTime < m_endTime) {
		double ms = (xTime-m_startTime).GetTotalSeconds()*1000.0;
		int xpos = (int) (( width * ms)/msTotal);

		if (xpos<0) {
			xpos=0;
		}
		if (xpos>width) {
			xpos = width;
		}
		xpos += origin.x;

		// WK_TRACE(_T("Loop now at %s xpos %d\n"),xTime.Format(_T("%d.%m.,%H:%M")),xpos);

		// calc the next point
		CTime xTime1(xTime);	// remember start time
		xTime += deltaT;

		double ms2 = (xTime-m_startTime).GetTotalSeconds()*1000.0;
		int xpos2 = (int) (( width * ms2)/msTotal);
		if (xpos2<0) {
			xpos2=0;
		}
		if (xpos2>width) {
			xpos2=width;
		}
		xpos2 += origin.x;

		// draw box
		CRect rect(xpos,origin.y+dwSmallStep,xpos2,origin.y - height); // NOT YET top
		CBrush brush;
		COLORREF fillColor;
		if (iCounter&1) {
			fillColor=RGB(255,255,128);
		} else {
			fillColor=RGB(255,255,255);
		}

		if (brush.CreateSolidBrush( fillColor )) {
			pDC->FillRect( rect, &brush );
		} else {
			TRACE(_T("Could not create brush\n"));
		}

		pDC->SelectObject(oldPen);


		TEXTMETRIC tm;      // get size of font
		CFont	MyFont;
		CFont* pOldFont = NULL;
		LOGFONT logfont;
		memset(&logfont, 0, sizeof(logfont));
		_tcscpy(logfont.lfFaceName, _T("Arial"));        // TrueType font
		pDC->GetTextMetrics(&tm);
		
		logfont.lfHeight = fontSizeHeader;
		MyFont.CreateFontIndirect(&logfont);
		pOldFont = pDC->SelectObject(&MyFont);

		CString sTitle;

		// english variant sTitle = xTime.Format(_T("%a"));
		if (totalTime.GetDays()>1) {
			// try full name first
			switch (xTime1.GetDayOfWeek()) {
				case 1:	 sTitle=_T("Sonntag"); break;
				case 2:	 sTitle=_T("Montag"); break;
				case 3:	 sTitle=_T("Dienstag"); break;
				case 4:	 sTitle=_T("Mittwoch"); break;
				case 5:	 sTitle=_T("Donnerstag"); break;
				case 6:	 sTitle=_T("Freitag"); break;
				case 7:	 sTitle=_T("Samstag"); break;
				default:
					sTitle.Format(_T("?%d?"),xTime1.GetDayOfWeek());
			}
			CSize tmpSize = pDC->GetOutputTextExtent(sTitle);
			if (tmpSize.cx>=(xpos2-xpos)) {	// does it fit ?
				// if not retreat to shorter names
				switch (xTime1.GetDayOfWeek()) {
					case 1:	 sTitle=_T("Sonn"); break;
					case 2:	 sTitle=_T("Mon"); break;
					case 3:	 sTitle=_T("Die"); break;
					case 4:	 sTitle=_T("Mitt"); break;
					case 5:	 sTitle=_T("Donn"); break;
					case 6:	 sTitle=_T("Frei"); break;
					case 7:	 sTitle=_T("Sams"); break;
					default:
						sTitle.Format(_T("?%d?"),xTime1.GetDayOfWeek());
				}

				tmpSize = pDC->GetOutputTextExtent(sTitle);
				if (tmpSize.cx>=(xpos2-xpos)) {	// does it fit ?
					// if not retreat to shorter names
					switch (xTime1.GetDayOfWeek()) {
						case 1:	 sTitle=_T("So"); break;
						case 2:	 sTitle=_T("Mo"); break;
						case 3:	 sTitle=_T("Di"); break;
						case 4:	 sTitle=_T("Mi"); break;
						case 5:	 sTitle=_T("Do"); break;
						case 6:	 sTitle=_T("Fr"); break;
						case 7:	 sTitle=_T("Sa"); break;
						default:
							sTitle.Format(_T("?%d?"),xTime1.GetDayOfWeek());
					}
					// last resort, single letters
					tmpSize = pDC->GetOutputTextExtent(sTitle);
					if (tmpSize.cx>=xpos2-xpos) {	// does it fit ?
						// if not retreat to shorter names
						switch (xTime1.GetDayOfWeek()) {
							case 1:	 sTitle=_T("S"); break;
							case 2:	 sTitle=_T("M"); break;
							case 3:	 sTitle=_T("D"); break;
							case 4:	 sTitle=_T("M"); break;
							case 5:	 sTitle=_T("D"); break;
							case 6:	 sTitle=_T("F"); break;
							case 7:	 sTitle=_T("S"); break;
							default:
								sTitle.Format(_T("?%d?"),xTime1.GetDayOfWeek());
						}
					} else {
						// already fits double letters
					}
				} else {
					// already fits triple letter
				}
			} else {
					// already fits, full name
			}

		} else if (totalTime.GetTotalHours()>1){
				sTitle.Format(_T("%02d:"),
					xTime1.GetHour()
					);
		} else if (totalTime.GetTotalMinutes()>1) {
				sTitle.Format(_T(":%02d"),
					xTime1.GetMinute()
				);
		} else {
			sTitle=_T("");
			// no label
		}

		CSize textSize = pDC->GetOutputTextExtent(sTitle);
		int iTextX = xpos+(xpos2-xpos)/2-textSize.cx/2;

		int iOldMode = pDC->SetBkMode(TRANSPARENT);
		if (sTitle.GetLength() 
			&& (textSize.cx < xpos2-xpos) // does it fit within the box
			&& (iTextX-origin.x)<width) {
			pDC->TextOut(iTextX,origin.y-height+2,sTitle);
		}
		pDC->SelectObject(pOldFont);
		pDC->SetBkMode(iOldMode);

		MyFont.DeleteObject();

		iCounter++;
	}	// end of loop over time axis
	pDC->SelectObject(oldPen);


}

/////////////////////////////////////////////////////////////////////////////
// CSecAnalyzerView printing

BOOL CSecAnalyzerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CSecAnalyzerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSecAnalyzerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CSecAnalyzerView diagnostics

#ifdef _DEBUG
void CSecAnalyzerView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CSecAnalyzerView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CSecAnalyzerDoc* CSecAnalyzerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSecAnalyzerDoc)));
	return (CSecAnalyzerDoc*)m_pDocument;
}
#endif //_DEBUG


void CSecAnalyzerView::UpdateWidthHeightMembers() 
{
	CSize currentSize = GetTotalSize();
	m_dWidth = currentSize.cx;
	m_dHeight = currentSize.cy;
	
	m_dScaleX = (m_dWidth/800.0);
	m_dScaleY = (m_dHeight/800.0);

	m_dwWidth = (DWORD)(m_dScaleX*500); 

}

void CSecAnalyzerView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
	
	m_dViewScale =1.0;
	SetScrollSizes(MM_LOMETRIC,CSize(800,800));

	UpdateWidthHeightMembers();
}

void CSecAnalyzerView::OnLButtonDown(UINT nFlags, CPoint point) 
{

#if 1
	TRACE(_T("Point %d %d\n"),point.x,point.y);

	if (point.x<15 && point.y<15) {
		m_dViewScale += 0.25;
		int iSize = (int)(800 *m_dViewScale);
		SetScrollSizes(MM_LOMETRIC,CSize(iSize,iSize));

		UpdateWidthHeightMembers();
		// NOT YET redraw/reposition?

	} else {
		// somewhere else
	}
#endif
	CScrollView::OnLButtonDown(nFlags, point);
}

void CSecAnalyzerView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_lastClickedPoint=point;	// rember the point, for the OnXXX function of the popup

	CTime xTime;
	BOOL bOkay = CalcTimeFromPoint(m_lastClickedPoint,xTime);

	CMenu menu;
	CMenu *pPopUp;
	menu.LoadMenu(IDR_CONTEXT);

	if (bOkay) {
		pPopUp = menu.GetSubMenu(0);
	} else {
		// no time, left of origin (the names)
		pPopUp = menu.GetSubMenu(1);
	}

	
	if (pPopUp) {

		CString sTime;
		if (bOkay) {
			sTime = xTime.Format(_T("%d.%m.%y,%H:%M"));	// OOPS CTime.Format
			pPopUp->ModifyMenu( ID_POPUP_TIME , MF_BYCOMMAND| MF_STRING, 0, sTime);
		} else {
			// left of axis, the label area
			CSecAnalyzerDoc* pDoc = GetDocument();
			const CStatRecords &records = *pDoc->m_pRecords;

			int iResult =0;
			CalcRecordIndexFromPoint(point,iResult);

			CString sData=_T("...");
			m_sClickedName = _T("???");
			if (iResult >=0 && iResult < records.GetSize()) {
				m_sClickedName = records[iResult]->GetName();

				// NOT YET more detailed
				if ((DWORD)records[iResult]->GetAverage()!=records[iResult]->m_dwMinValue
					&& (DWORD)records[iResult]->GetAverage()!=records[iResult]->m_dwMaxValue) {
					sData.Format(_T("min %d max %d av.%d"),
					records[iResult]->m_dwMinValue,
					records[iResult]->m_dwMaxValue,
					records[iResult]->GetAverage()
					);
				} else {
					// average the same as min or max
					sData.Format(_T("min %d max %d"),
					records[iResult]->m_dwMinValue,
					records[iResult]->m_dwMaxValue
					);
				}
			}
			pPopUp->ModifyMenu( ID_POPOP2_NAME , MF_BYCOMMAND| MF_STRING, 0, m_sClickedName);
			pPopUp->ModifyMenu( ID_POPUP2_DATA, MF_BYCOMMAND| MF_STRING, 0, sData);
		}
		
		CPoint tmpPoint(point);
		ClientToScreen(&tmpPoint);
		pPopUp->TrackPopupMenu(TPM_LEFTALIGN,tmpPoint.x,tmpPoint.y,GetParent());
	} else {
	}
	CScrollView::OnRButtonDown(nFlags, point);
}

BOOL CSecAnalyzerView::CalcTimeFromPoint(const CPoint point,CTime &result)
{
	CPoint origin((DWORD)(m_dScaleX*150),(DWORD)(m_dScaleY*300));
	double msTotalSeconds = (m_endTime-m_startTime).GetTotalSeconds();
	CRect clientRect;
	GetClientRect(clientRect);
	double f = 0;
	f= (clientRect.Height() / 310.0);
	origin.y = (DWORD) (282*f);
	f = (clientRect.Width() / 700.0);

	if (point.y < origin.y) {
		TRACE(_T("Above origin "));
	} else {
		TRACE(_T("Below origin "));
	}
	int seconds = (int) (((point.x-origin.x) * msTotalSeconds)/(m_dwWidth*f));

	if (seconds >0) {
		result= m_startTime + CTimeSpan(0,0,0,seconds);
		return TRUE;
	} else {
		return FALSE;
	}

}

BOOL CSecAnalyzerView::CalcRecordIndexFromPoint(const CPoint point, int &result)
{
	CSecAnalyzerDoc* pDoc = GetDocument();
	if (pDoc==NULL) {
		WK_TRACE_ERROR(_T("No doc !?\n"));
		result = 0;
		return FALSE;
	}
	ASSERT_VALID(pDoc);


	const CStatRecords &records = *pDoc->m_pRecords;
	CPoint origin((DWORD)(m_dScaleX*150),(DWORD)(m_dScaleY*300));
	CRect clientRect;
	GetClientRect(clientRect);
	double f = 0;
	f= (clientRect.Height() / 310.0);
	DWORD height=(DWORD)(m_dScaleY*280);
	height = (DWORD) (height * f);
	origin.y = (DWORD) (282*f);
	DWORD deltaY = height / (max(1,records.GetSize()));


	if (point.y < origin.y) {
		// above
		result = (origin.y - point.y)/deltaY;
		if (result > records.GetSize()) {
			result = records.GetSize();
		}
		if (result <0) {
			result = 0;
		}
		return TRUE;
	} else {
		result =0;
		return FALSE;
	}
}

void CSecAnalyzerView::OnStartPoint() 
{
	CTime xTime;
	CTime ct = CTime::GetCurrentTime();
	
	BOOL bOkay = CalcTimeFromPoint(m_lastClickedPoint,xTime);

	if (bOkay) {
		CString sTime,sDate;

		sTime = xTime.Format(_T("%H:%M"));	// OOPS CTime.Format
		CWnd *pWnd = GetMain()->m_wndSecAnalyzerBar.GetDlgItem(IDC_START_TIME);
		if (WK_IS_WINDOW(pWnd)) {
			pWnd->SetWindowText(sTime);
		}
	
		if (xTime.GetYear()!=ct.GetYear()) {
			sDate = xTime.Format(_T("%d.%m.%Y"));	// OOPS CTime.Format
		} else {
			sDate = xTime.Format(_T("%d.%m"));	// OOPS CTime.Format
		}
		pWnd = GetMain()->m_wndSecAnalyzerBar.GetDlgItem(IDC_START_DATE);
		if (WK_IS_WINDOW(pWnd)) {
			pWnd->SetWindowText(sDate);
		}
	}

}

void CSecAnalyzerView::OnEndPoint() 
{
	CTime xTime;
	CTime ct = CTime::GetCurrentTime();

	BOOL bOkay = CalcTimeFromPoint(m_lastClickedPoint,xTime);

	if (bOkay) {
		CString sTime,sDate;
		sTime = xTime.Format(_T("%H:%M"));	// OOPS CTime.Format
		CWnd *pWnd = GetMain()->m_wndSecAnalyzerBar.GetDlgItem(IDC_END_TIME);
		if (WK_IS_WINDOW(pWnd)) {
			pWnd->SetWindowText(sTime);
		}

		if (xTime.GetYear()!=ct.GetYear()) {
			sDate = xTime.Format(_T("%d.%m.%Y"));	// OOPS CTime.Format
		} else {
			sDate = xTime.Format(_T("%d.%m"));	// OOPS CTime.Format
		}

		pWnd = GetMain()->m_wndSecAnalyzerBar.GetDlgItem(IDC_END_DATE);
		if (WK_IS_WINDOW(pWnd)) {
			pWnd->SetWindowText(sDate);
		}
	}

}

void CSecAnalyzerView::OnNop() 
{
	// NOP no operation
}

void CSecAnalyzerView::OnPopupExclude() 
{
	if (m_sClickedName!=_T("???")) {
		CWnd *pWnd = GetMain()->m_wndSecAnalyzerBar.GetDlgItem(IDC_EXCLUDE_PATTERN);
		if (WK_IS_WINDOW(pWnd)) {
			CString s;
			pWnd->GetWindowText(s);
			if (s.Find(m_sClickedName) == -1) {
				if (s.GetLength()) {
					s += _T(' ');
				}
				s += _T('"') + m_sClickedName +_T('"');
				pWnd->SetWindowText(s);
			} else {
				// already in there
			}
		}
	}
	// NOT YET make include/exclude exclusive
}

void CSecAnalyzerView::OnPopupInclude() 
{
	if (m_sClickedName!=_T("???")) {
		CWnd *pWnd = GetMain()->m_wndSecAnalyzerBar.GetDlgItem(IDC_INCLUDE_PATTERN);
		if (WK_IS_WINDOW(pWnd)) {
			CString s;
			pWnd->GetWindowText(s);
			if (s.Find(m_sClickedName) == -1) {
				if (s.GetLength()) {
					s += _T(' ');
				}
				s += _T('"') + m_sClickedName +_T('"');
				pWnd->SetWindowText(s);
			} else {
				// already in there
			}
		}
	}
	// NOT YET make include/exclude exclusive
}

void CSecAnalyzerView::OnColoredBackground() 
{

	m_bDoColoredBackground = ! m_bDoColoredBackground;
	
	// iniate a redraw
	CWnd* pWnd = AfxGetApp()->m_pMainWnd;
	if (pWnd && pWnd->m_hWnd && ::IsWindow(pWnd->m_hWnd))
	{
		pWnd->PostMessage(WM_COMMAND,ID_FILE_NEW);
	}
}

void CSecAnalyzerView::OnUpdateColoredBackground(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bDoColoredBackground);
	pCmdUI->ContinueRouting();	// OOPS what's that


}
