// TimerSetDialog.cpp : implementation file
//

#include "stdafx.h"
#include "timereditor.h"
#include "TimerSetDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimerSetDialog dialog


CTimerSetDialog::CTimerSetDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CTimerSetDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimerSetDialog)
	m_iType = -1;
	m_sWeekDay = _T("");
	m_sDay = _T("");
	m_sMonth = _T("");
	m_sYear = _T("");
	m_sTimerName = _T("");
	//}}AFX_DATA_INIT
}


void CTimerSetDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimerSetDialog)
	DDX_Control(pDX, IDC_TIMER_HOURS_TEXT, m_ctlHoursText);
	DDX_Control(pDX, IDC_TIMER_YEAR, m_ctlYear);
	DDX_Control(pDX, IDC_TIMER_MONTH, m_ctlMonth);
	DDX_Control(pDX, IDC_TIMER_DAY, m_ctlDay);
	DDX_Control(pDX, IDC_TIMER_WEEK_DAY_COMBO, m_ctlWeekdayCombo);
	DDX_Radio(pDX, IDC_TIMER_SET_TYPE_WEEK_DAY, m_iType);
	DDX_CBString(pDX, IDC_TIMER_WEEK_DAY_COMBO, m_sWeekDay);
	DDX_Text(pDX, IDC_TIMER_DAY, m_sDay);
	DDX_Text(pDX, IDC_TIMER_MONTH, m_sMonth);
	DDX_Text(pDX, IDC_TIMER_YEAR, m_sYear);
	DDX_Text(pDX, IDC_TXT_TIMER_SET_NAME_LABEL, m_sTimerName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTimerSetDialog, CDialog)
	//{{AFX_MSG_MAP(CTimerSetDialog)
	ON_BN_CLICKED(IDC_TIMER_SET_TYPE_WEEK_DAY, OnTimerSetTypeWeekDay)
	ON_BN_CLICKED(IDC_TIMER_SET_TYPE_DATE, OnTimerSetTypeDate)
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_TIMER_WEEK_DAY_COMBO, OnSelchangeTimerWeekDayCombo)
	ON_EN_UPDATE(IDC_TIMER_DAY, OnUpdateTimerDay)
	ON_EN_UPDATE(IDC_TIMER_MONTH, OnUpdateTimerMonth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimerSetDialog message handlers

void CTimerSetDialog::OnTimerSetTypeWeekDay() 
{
	DisableAllDateWindos();
}

void CTimerSetDialog::OnTimerSetTypeDate() 
{
	DisableAllWeekWindos();
}

void CTimerSetDialog::DisableAllWeekWindos() 
{
	m_ctlWeekdayCombo.EnableWindow(FALSE);
	// also activate the date windows
	m_ctlDay.EnableWindow(TRUE);
	m_ctlMonth.EnableWindow(TRUE);
	m_ctlYear.EnableWindow(TRUE);
	
	m_iType = 1;
	UpdateData(FALSE);
}

void CTimerSetDialog::DisableAllDateWindos() 
{
	m_ctlDay.EnableWindow(FALSE);
	m_ctlMonth.EnableWindow(FALSE);
	m_ctlYear.EnableWindow(FALSE);
	// also activate the week windows
	m_ctlWeekdayCombo.EnableWindow(TRUE);

	m_iType = 0;
	UpdateData(FALSE);
}


BOOL CTimerSetDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// attach CHourCtl class tow owner draw button
	BOOL bOkay = m_wndHours.SubclassDlgItem( IDC_HOURTABLE, this);
	m_wndHours.m_pCtlHoursText = &m_ctlHoursText;
	m_wndHours.UpdateHoursText();

	m_ctlWeekdayCombo.SetCurSel(m_iWeekDayIndex);
	if (m_iType==0) {
		DisableAllDateWindos();
	} else {
		DisableAllWeekWindos();
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CTimerSetDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CTimerSetDialog::FillHoursFromString(const CString &sHours)	// CAVEAT fixed format
{
	m_wndHours.FillHoursFromString(sHours);
}

void CTimerSetDialog::SetDayType(const CString &sDay)
{
	int ix=-1;

	if (sDay.CompareNoCase("Mo")==0) {
		ix=0;
	}
	if (sDay.CompareNoCase("Di")==0) {
		ix=1;
	}
	if (sDay.CompareNoCase("Mi")==0) {
		ix=2;
	}
	if (sDay.CompareNoCase("Do")==0) {
		ix=3;
	}
	if (sDay.CompareNoCase("Fr")==0) {
		ix=4;
	}
	if (sDay.CompareNoCase("Sa")==0) {
		ix=5;
	}
	if (sDay.CompareNoCase("So")==0) {
		ix=6;
	}

	if (ix != -1) {
		m_iType=0;
		// CAVEAT can't do that here, it's not a window m_ctlWeekdayCombo.SetCurSel(ix);
		m_iWeekDayIndex = ix;
	} else {
		// split the string in its date parts
		CString sTime(sDay);
		ix = sTime.Find('.');
		m_sDay =  sTime.Left(ix);
		sTime = sTime.Mid(ix+1);

		ix = sTime.Find('.');
		m_sMonth = sTime.Left(ix);
		sTime = sTime.Mid(ix+1);

		m_sYear = sTime;
		
		m_iType=1;
	}
}

CString CTimerSetDialog::GetHoursAsString() const
{
	return m_wndHours.GetHoursAsString();
}

CString CTimerSetDialog::GetDayAsString() const
{
	CString sResult;
	if (m_iType==0) {
		switch (m_iWeekDayIndex) {
			// NOT YET localized
			case 0: sResult ="Mo"; break;
			case 1: sResult ="Di"; break;
			case 2: sResult ="Mi"; break;
			case 3: sResult ="Do"; break;
			case 4: sResult ="Fr"; break;
			case 5: sResult ="Sa"; break;
			case 6: sResult ="So"; break;
			
			default: sResult = "Invalid WeekDay";
		}
	} else if (m_iType==1) {
		sResult.Format("%s.%s.",m_sDay,m_sMonth);
		if (m_sYear.Find('*') != -1) {
			sResult += "****";
		} else {
			int y=atoi(m_sYear);
			if (m_sYear.GetLength()==4) {
				// the simple case, use the given year
				sResult += m_sYear;
			} else {	// not exactly 4 digits
				int y=atoi(m_sYear);
				if (y>=90 && y<=99) {
					sResult += "19";
					sResult += m_sYear;
				} else {
					sResult += "20";
					sResult += m_sYear;
				}
			}
		}
	} else {
		WK_TRACE_ERROR("InternalError: weekDay type is %d\n",m_iType);
	}
	return sResult;
}

void CTimerSetDialog::OnSelchangeTimerWeekDayCombo() 
{
	m_iWeekDayIndex = m_ctlWeekdayCombo.GetCurSel();
	if (m_iWeekDayIndex<0 || m_iWeekDayIndex>6) {
		// OOPS
	}
}

void CTimerSetDialog::OnUpdateTimerDay() 
{
	// EN_UPDATE
	CString sDay;
	m_ctlDay.GetWindowText(sDay);

	// CAVEAT alreay restricted to numbers in the style
	if (sDay.GetLength()==2) {
		// 2 digits move to month
		m_ctlMonth.SetFocus();
	}
	// NOT YET restrict to certain numbers
}

void CTimerSetDialog::OnUpdateTimerMonth() 
{
	// EN_UPDATE
	CString sMonth;
	m_ctlMonth.GetWindowText(sMonth);

	// CAVEAT alreay restricted to numbers in the style
	if (sMonth.GetLength()==2) {
		// 2 digits move to year
		m_ctlYear.SetFocus();
	}

	// NOT YET restrict to certain numbers
	
}
