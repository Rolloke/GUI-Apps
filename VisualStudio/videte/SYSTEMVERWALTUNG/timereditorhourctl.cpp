// TimerEditorHourCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "TimerEditorHourCtrl.h"

#include "WKClasses/WK_String.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimerEditorHourCtrl

void CTimerEditorHourCtrl::SetMinute(int hour,int minute,BOOL bValue,BOOL bRedraw)
{
	BOOL bModified = FALSE;

	if (hour >=0 && hour <24) {
		if (minute>=0 && minute <60) {
			int iBit;
			int ix;	// which DWORD to use
			if (minute<30) {
				iBit = minute;
				ix =0;
			} else {
				iBit=minute-30;
				ix =1;
			}

			WK_ASSERT(iBit<32);
			BOOL bOldValue;

			// no drag the normal data
			bOldValue = (m_minutes[hour][ix] & (1<<iBit))!=0;
			if (bValue) {
				m_minutes[hour][ix] |= (1<<iBit);
			} else {
				m_minutes[hour][ix] &= ~(1<<iBit);
			}

			if (bOldValue!=bValue) {
					bModified=TRUE;
			}

		} else {
			TRACE(_T("Invalid minute %02d\n"),minute);
		}
	} else {
		TRACE(_T("Invalid hour %02d\n"),hour);
	}

	if (bRedraw && bModified) {
		RedrawBox(hour,minute);
	}
}

BOOL CTimerEditorHourCtrl::GetMinute(int hour,int minute) const
{
	BOOL bResult=FALSE;

	if (hour >=0 && hour <24) {
		if (minute>=0 && minute <60) {
			int iBit;
			int ix;	// which DWORD to use
			if (minute<30) {
				iBit = minute;
				ix =0;
			} else {
				iBit=minute-30;
				ix =1;
			}

			WK_ASSERT(iBit<32);


			bResult = ((m_minutes[hour][ix] & (1<<iBit))!=0);
		}
	}

	return bResult;
}


CTimerEditorHourCtrl::CTimerEditorHourCtrl()
	: m_top(60,20)
{
	m_pCtlHoursText = NULL;

	m_iBlockWidth = 8;
	m_iBlockHeight= 15;
	m_ileftLabelOffset = 20;
	m_bDragMode = TRUE;
	m_bButtonDown = FALSE;
	m_bIsDragging = FALSE;

	m_mouseLabelPos = CPoint(m_top.x+m_iBlockWidth*10,m_top.y+(24+1)*m_iBlockHeight);

	m_emptyBrush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	m_blackBrush.CreateSolidBrush(RGB(0,0,0));
	m_hourBrush.CreateSolidBrush(RGB(0,0,252));


	for (int h=0;h<24;h++) {
		for (int m=0;m<60;m++) {
			SetMinute(h,m,FALSE);
		}
	}
}

CTimerEditorHourCtrl::~CTimerEditorHourCtrl()
{
}


BEGIN_MESSAGE_MAP(CTimerEditorHourCtrl, CWnd)
	//{{AFX_MSG_MAP(CTimerEditorHourCtrl)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimerEditorHourCtrl message handlers

void CTimerEditorHourCtrl::OnPaint() 
{
	CRect updateRect;
	GetUpdateRect(updateRect);
	CPaintDC dc(this); // device context for painting

	CRect rect;
	CBrush b;
	b.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	GetClientRect(rect);
	dc.SaveDC();
	dc.FillRect(rect,&b);
	
	// LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight );
	dc.Draw3dRect(rect,GetSysColor(COLOR_3DSHADOW),GetSysColor(COLOR_3DHILIGHT));

	CFont myFont;
	if (CSkinDialog::UseGlobalFonts())
	{
		dc.SelectObject(CSkinDialog::GetDialogItemGlobalFont());
	}
	else
	{
		myFont.CreatePointFont(80,_T("MS Sans Serif"),&dc);
		dc.SelectObject(&myFont);
	}

	dc.SetBkColor(GetSysColor(COLOR_BTNFACE));

	CString sTmp;
	sTmp.LoadString(IDS_HOUR);
	DWORD dwOldAlignment = dc.SetTextAlign(TA_RIGHT);
	dc.TextOut(m_top.x-m_ileftLabelOffset,m_top.y-15,sTmp);
	dc.SetTextAlign(dwOldAlignment);

	int h=0;
	int m=0;
	for (h=0;h<24;h++) 
	{	// loop over hours
		sTmp.Format(_T("%d"),h);
		dwOldAlignment = dc.SetTextAlign(TA_RIGHT);
		dc.TextOut(m_top.x-m_ileftLabelOffset,m_top.y+h*m_iBlockHeight+1,sTmp);
		// also a label on the right side, but without button
		sTmp +=_T(":59");
		dc.TextOut(m_top.x+60*m_iBlockWidth+m_ileftLabelOffset+10,m_top.y+h*m_iBlockHeight+1,sTmp);

		// draw small hour buttons, which activate the whole hour at once
		CRect hourButtonRect(
			CPoint(m_top.x-m_ileftLabelOffset-20,m_top.y+h*m_iBlockHeight),
			CSize(30,m_iBlockHeight)
			);
		dc.Draw3dRect(hourButtonRect,
			GetSysColor(COLOR_3DHILIGHT),
			GetSysColor(COLOR_3DSHADOW)
			);
		dc.SetBkColor(GetSysColor(COLOR_BTNFACE));


		// minute labels
		if (h==0)
		{
			for (m=0;m<60;m+=15)
			{
				sTmp.Format(_T("%d"),m);
				dwOldAlignment = dc.SetTextAlign(TA_CENTER);
				dc.TextOut(m_top.x+m*m_iBlockWidth+m_iBlockWidth/2,m_top.y-15,sTmp);
				dc.SetTextAlign(dwOldAlignment);
			}
		}
		dc.SetTextAlign(dwOldAlignment);

		for (m=0;m<60;m++)
		{	// loop over minutes
			DrawBox (dc, h,m,GetMinute(h,m));
		}	// end of loop over minutes
	}	// end of loop over hours

	RedrawAxis(dc);

	dc.RestoreDC(-1);
	myFont.DeleteObject();

	// Do not call CButton::OnPaint() for painting messages
}


void CTimerEditorHourCtrl::DrawMouseMessage(CDC &dc, const CString &sMsg) 
{
	CFont myFont;
	dc.SaveDC();
	if (CSkinDialog::UseGlobalFonts())
	{
		dc.SelectObject(CSkinDialog::GetDialogItemGlobalFont());
	}
	else
	{
		myFont.CreatePointFont(80,_T("MS Sans Serif"),&dc);
		dc.SelectObject(&myFont);
	}
	DWORD dwOldAlignment = dc.SetTextAlign(TA_LEFT);
	
	dc.SetBkColor(GetSysColor(COLOR_BTNFACE));


	dc.TextOut(m_mouseLabelPos.x,m_mouseLabelPos.y,sMsg);
	dc.SetTextAlign(dwOldAlignment);

	dc.RestoreDC(-1);
	myFont.DeleteObject();
}

void CTimerEditorHourCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	int hour=-1;
	int minute=-1;

	PosToTime(point.x,point.y,hour,minute);

	CClientDC dc(this); // device context for painting
	CString sTmp;
	CString sMice;

	sMice.LoadString(IDS_MICE);
	
	if (m_bIsDragging==FALSE) {
		if (hour!=-1 && minute !=-1) {
			sTmp.Format(_T("%s : %02d:%02d                            "),sMice,hour,minute);	// trailing blanks to erase old stuff
		} else {
			sTmp = sMice + _T(" :                              ");
		}

		DrawMouseMessage(dc,sTmp);
	} else {
		if (hour!=-1 && minute!=-1) {
			int hl,ml;	// last drag time
			int hd,md;	// current drag time
			int mc,hc;	// clicked time

			
			PosToTime(m_pointDrag.x,m_pointDrag.y,hl,ml);	// last pos
			PosToTime(point.x,point.y,hd,md);				// drag pos
			PosToTime(m_pointClicked.x,m_pointClicked.y,hc,mc);	// click/start pos

			if ( hl!=hd || ml!=md ) {
				TimeTrack(hl,ml,hd,md,
					TRUE,	// just redraw
					FALSE	// do not use the drag mode
					);	// erase last piece
			}

			TimeTrack(hc,mc,hd,md,
				TRUE,	// just redraw
				TRUE	// use the drag mode
				);	// redraw the full track

			m_pointDrag=point;


			int h1=hc;
			int m1=mc;
			int h2=hd;
			int m2=md;

			if (h2<h1 || (h1==h2 && m2<m1)) {
				int c;
				c=h1;
				h1=h2;
				h2=c;
				c=m1;
				m1=m2;
				m2=c;
			}

			sTmp.Format(_T("%s : %02d:%02d,00 - %02d:%02d,59                   "),sMice,h1,m1,h2,m2);	// trailing blanks to erase old stuff
			DrawMouseMessage(dc,sTmp);

		}

		// CAVEAT cant do UpdateHoursText(); here since the data is modified in ButtonRelease

	}	// end of dragMode
		
	CButton::OnMouseMove(nFlags, point);
}


void CTimerEditorHourCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bButtonDown = TRUE;
	m_pointClicked = point;	// stroe the click position, which is used for the drag track

	// calc time position
	int h,m;
	PosToTime(point.x,point.y,h,m);

	if (m==-1 && h==-1) {	// outside of grid, can be a special _T('button')
		m_bIsDragging=FALSE;

		// an hour button ?
		if (point.x<m_top.x
			&& point.y>=m_top.y && point.y<=m_top.y+24*m_iBlockHeight) {
			// calc h by hand, the normal PosToTime only works within the grid
			h = ((point.y-m_top.y)%(24*m_iBlockHeight))/m_iBlockHeight;
			if (h>=0 && h<24){
				for (m=0;m<60;m++) {
					SetMinute(h,m,!GetMinute(h,m),TRUE);	// invert
				}
			} else {
				TRACE(_T("Invalid hour %d (button)\n"),h);
			}
		}
	} else {
		// within grid
		m_bDragMode = ! GetMinute(h,m);
		m_bIsDragging=TRUE;
		m_pointDrag = m_pointClicked;

		TimeTrack(h,m,h,m,
			TRUE,	// just redraw
			TRUE	// use the dragMode
			);
	}

	UpdateHoursText();

	CButton::OnLButtonDown(nFlags, point);
}

void CTimerEditorHourCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bIsDragging) {
		m_bButtonDown = FALSE;
		m_bIsDragging = FALSE;

		// the buttons might be released outside of the grid
		int hl,hd,hc;
		int ml,md,mc;
		int finalHour,finalMinute;

		PosToTime(m_pointDrag.x,m_pointDrag.y,hl,ml);	// last pos
		PosToTime(point.x,point.y,hd,md);				// drag pos
		PosToTime(m_pointClicked.x,m_pointClicked.y,hc,mc);	// click/start pos

		if (hd!=-1 && md!=-1) {
			// inside of grid, fine
			finalHour=hd;
			finalMinute=md;
		} else {
			// released outside of grid
			finalHour = hl;
			finalMinute = ml;
		}

		if (finalHour!=-1 && finalMinute!=-1) {
			TimeTrack(hc,mc,finalHour,finalMinute,
				FALSE,	// no redraw, set the data
				TRUE	// use the dragmode
				);	// no redraw, set the data
		}

		UpdateHoursText();
	}
	
	// NOT YET some kind of redraw
	
	CButton::OnLButtonUp(nFlags, point);
}


// hour and minute are -1, if outsude
void CTimerEditorHourCtrl::PosToTime(int x,int y,int &hour,int &minute)
{
	hour = -1;
	minute = -1;

	if (x>=m_top.x && x<m_top.x+60*m_iBlockWidth
		&& y>=m_top.y && y<m_top.y+24*m_iBlockHeight
	) {
		minute = ((x-m_top.x)%(m_iBlockWidth*60))/m_iBlockWidth;
		hour = ((y-m_top.y)%(24*m_iBlockHeight))/m_iBlockHeight;
	}
}


void CTimerEditorHourCtrl::TimeToPos(int hour,int minute,int &x, int &y)
{
	x=-1;
	y=-1;
}


void CTimerEditorHourCtrl::FillHoursFromString(const CString &sHours)	// CAVEAT fixed format
{
	CWK_Strings spans;
	spans.SplitArgs(sHours,_T(','),TRUE);

	for (int i=0;i<spans.GetSize();i++) {
		CWK_String sSpan = *spans[i];
		sSpan.TrimLeft();
		sSpan.TrimRight();

		int h1,m1,h2,m2;
		int iNumRead = _stscanf(sSpan,_T("%d:%d-%d:%d"),&h1,&m1,&h2,&m2);
		if (iNumRead==4) {
			if (h2==0 && m2==0) {
				h2=23;
				m2=59;
			} else {
				m2--;
				if (m2<0) {
					h2--;
					m2=59;
				}
			}
			TimeTrack(h1,m1,h2,m2,FALSE,FALSE);
		}
	}

	UpdateHoursText();
}



void CTimerEditorHourCtrl::DrawItem(LPDRAWITEMSTRUCT pDraw)
{
	// OOPS NOP, but is needed in an OwnerDraw button
}


void CTimerEditorHourCtrl::RedrawBox(int h, int m)
{
	CClientDC dc(this);

	DrawBox(dc,h,m,GetMinute(h,m));
}

void CTimerEditorHourCtrl::TimeTrack(int h1, int m1,int h2, int m2,BOOL bRedrawOnly, BOOL bUseDragMode)
{
	// swap if second point is in front of first point
	if (h2<h1 || (h1==h2 && m2<m1) ) {
		// TRACE(_T("SWAP %02d:%02d <-> %02d:%02d\n"),h1,m1,h2,m2);
		int c;
		c=h1;
		h1=h2;
		h2=c;

		c=m1;
		m1=m2;
		m2=c;
	}
	// TRACE(_T("Drag track from %02d:%02d to %02d:%02d (dragMode %d)\n"),h1,m1,h2,m2,bUseDragMode);

	// CAVEAT the special fn TrackAction takes care of the action
	// controlled by the parameters
	int m;
	for (int h=h1;h<=h2;h++) {	// loop over all hours
		if (h==h1) {	// first hour
			if (h1==h2) {
				for (m=m1;m<=m2;m++) {
					TrackAction(h,m,bRedrawOnly,bUseDragMode);
				}
			} else {
				for (m=m1;m<60;m++) {
					TrackAction(h,m,bRedrawOnly,bUseDragMode);
				}
			}
		} else if (h==h2) {
			for (m=0;m<=m2;m++) {
				TrackAction(h,m,bRedrawOnly,bUseDragMode);
			}
		} else {
			for (m=0;m<60;m++) {
				TrackAction(h,m,bRedrawOnly,bUseDragMode);
			}
		}
	}
}

void CTimerEditorHourCtrl::RedrawAxis(CDC &dc)
{
	dc.SetBkColor(GetSysColor(COLOR_BTNFACE));
	CPen axisPen(PS_DOT,1,RGB(0,0,252));
	CPen *pOldPen = dc.SelectObject(&axisPen);
	
	// draw quarter axis
	for (int m=15;m<60;m+=15) {
		dc.MoveTo(m_top.x+m*m_iBlockWidth+m_iBlockWidth/2,m_top.y-3);
		dc.LineTo(m_top.x+m*m_iBlockWidth+m_iBlockWidth/2,m_top.y+24*m_iBlockHeight+3);
	}
	dc.SelectObject(pOldPen);

	// midday

	CPen haxisPen(PS_DOT,1,RGB(0,0,252));
	pOldPen = dc.SelectObject(&haxisPen);
	dc.MoveTo(m_top.x-3, m_top.y+12*m_iBlockHeight+m_iBlockHeight/2);
	dc.LineTo(m_top.x+60*m_iBlockWidth+3,m_top.y+12*m_iBlockHeight+m_iBlockHeight/2);

	dc.SelectObject(pOldPen);

}


void CTimerEditorHourCtrl::DrawBox(CDC &dc, int h, int m, BOOL bValue)
{
	if (h<0 || h>24 || m<0 || m>60) {
		WK_TRACE_ERROR(_T("Invalid time %02d:%02d\n"),h,m);
		return;	// <<< EXIT >>>
	}

	// time is valid here
	CRect rect(
		m_top.x+m*m_iBlockWidth,m_top.y+h*m_iBlockHeight,
		m_top.x+m*m_iBlockWidth+m_iBlockWidth+1,m_top.y+h*m_iBlockHeight+m_iBlockHeight+1
		);	// left top right bottom
	// rect.InflateRect(1,1);	// avoid double lines
	if (bValue) {
		dc.FillRect(rect,&m_hourBrush);
	} else {
		dc.FillRect(rect,&m_emptyBrush);
	}
	
	dc. FrameRect( rect, &m_blackBrush );

	if (h==12 || (m && (m%15)==0)) {
		RedrawAxis(dc);
	}


}

void CTimerEditorHourCtrl::TrackAction(int h, int m,BOOL bDrawOnly,BOOL bUseDragMode)
{
	if (bDrawOnly) {
		CClientDC dc(this);
		BOOL bValue;
		if (bUseDragMode) { 
			bValue = m_bDragMode; 
		} else { 
			bValue = GetMinute(h,m); // redraw the real value (undo)
		}
		DrawBox(dc,h,m,bValue);	// redraw if changed
	} else {
		if (bUseDragMode) {
			SetMinute(h,m,m_bDragMode);
		} else {
			SetMinute(h,m,TRUE);// NOT YET value in args
		}
	}


}

CString CTimerEditorHourCtrl::GetHoursAsString(const CString & sFiller) const
{
	CString sResult;
	CString sTmp;
	BOOL bCurrentMode=FALSE;

	for (int h=0;h<24;h++) {
		for (int m=0;m<60;m++) {
			BOOL bValue = GetMinute(h,m);
			if (bValue!=bCurrentMode) {
				// mode change
				if (bCurrentMode==FALSE) {
					// a new span begins
					if (sResult.GetLength()) {
						sResult += sFiller;
					}
					sTmp.Format(_T("%02d:%02d"),h,m);
					sResult += sTmp;

					bCurrentMode = TRUE;
				} else {
					// a span ends
					sResult +=_T("-");
					// we have to add one for the 08:00-12:00 display
					// we have to subtract one since we are behind the end
					// thus we don't have to do anything

					sTmp.Format(_T("%02d:%02d"),h,m);
					sResult += sTmp;
					bCurrentMode = FALSE;
				}
			} else {
				// move on
			}
		}	// end of loop over minutes
	}	// end of loop over  hours

	if (bCurrentMode) {
		// finish the day
			sResult +=_T("-00:00");
	}

	return sResult;
}

void CTimerEditorHourCtrl::UpdateHoursText()
{
	if (m_pCtlHoursText==NULL) {
		return;	// <<< EXIT >>>
	}

	CString sHours;
	sHours = GetHoursAsString(_T("\r\n"));
	m_pCtlHoursText->SetWindowText(sHours);
}

