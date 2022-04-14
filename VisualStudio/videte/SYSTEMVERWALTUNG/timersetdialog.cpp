// TimerSetDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Systemverwaltung.h"
#include "TimerSetDialog.h"

#include "TimerEditorPage.h"	// for localized weekday conversion

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimerSetDialog dialog
CTimerSetDialog::CTimerSetDialog(CWnd* pParent /*=NULL*/)
	: CWK_Dialog(CTimerSetDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimerSetDialog)
	m_iType = -1;
	m_sWeekDay = _T("");
	m_sDay = _T("");
	m_sMonth = _T("");
	m_sYear = _T("");
	m_sTimerName = _T("");
	//}}AFX_DATA_INIT
	m_iWeekDayIndex = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CTimerSetDialog::SetTimerName(const CString &sName)
{
	m_sTimerName = sName;
}
/////////////////////////////////////////////////////////////////////////////
void CTimerSetDialog::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
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
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CTimerSetDialog, CWK_Dialog)
	//{{AFX_MSG_MAP(CTimerSetDialog)
	ON_BN_CLICKED(IDC_TIMER_SET_TYPE_WEEK_DAY, OnTimerSetTypeWeekDay)
	ON_BN_CLICKED(IDC_TIMER_SET_TYPE_DATE, OnTimerSetTypeDate)
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_TIMER_WEEK_DAY_COMBO, OnSelchangeTimerWeekDayCombo)
	ON_EN_UPDATE(IDC_TIMER_DAY, OnUpdateTimerDay)
	ON_EN_UPDATE(IDC_TIMER_MONTH, OnUpdateTimerMonth)
	ON_EN_UPDATE(IDC_TIMER_YEAR, OnUpdateTimerYear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CTimerSetDialog message handlers
void CTimerSetDialog::OnTimerSetTypeWeekDay() 
{
	if ((m_iWeekDayIndex<0 || m_iWeekDayIndex>6)
		&& IsWindow(m_ctlWeekdayCombo)
	) {
		m_iWeekDayIndex = 0;
		m_ctlWeekdayCombo.SetCurSel(m_iWeekDayIndex);
	}

	DisableAllDateWindows();
}
/////////////////////////////////////////////////////////////////////////////
void CTimerSetDialog::OnTimerSetTypeDate() 
{
	UpdateData();	// read values from dialog
	// fill the fields with default values, if they are not set
	if (m_sDay.GetLength()==0) {
		m_sDay=_T("1");
		m_sMonth=_T("1");
		m_sYear=_T("****");
		UpdateData(FALSE);
	}

	DisableAllWeekWindows();
}
/////////////////////////////////////////////////////////////////////////////
void CTimerSetDialog::DisableAllWeekWindows() 
{
	m_ctlWeekdayCombo.EnableWindow(FALSE);
	// also activate the date windows
	m_ctlDay.EnableWindow(TRUE);
	m_ctlMonth.EnableWindow(TRUE);
	m_ctlYear.EnableWindow(TRUE);
	
	m_iType = 1;
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CTimerSetDialog::DisableAllDateWindows() 
{
	m_ctlDay.EnableWindow(FALSE);
	m_ctlMonth.EnableWindow(FALSE);
	m_ctlYear.EnableWindow(FALSE);
	// also activate the week windows
	m_ctlWeekdayCombo.EnableWindow(TRUE);

	m_iType = 0;
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTimerSetDialog::OnInitDialog() 
{
	CWK_Dialog::OnInitDialog();

	// attach CHourCtl class to owner draw button
	m_wndHours.SubclassDlgItem( IDC_HOURTABLE, this);
	m_wndHours.m_pCtlHoursText = &m_ctlHoursText;
	m_wndHours.UpdateHoursText();

	if (CSkinDialog::SetChildWindowFont(m_hWnd))
	{
		if (m_ToolTip.m_hWnd)
		{
			m_ToolTip.SetFont(CSkinDialog::GetDialogItemGlobalFont(), 0);
		}
	}


	CString sWeekDay;
	sWeekDay.LoadString(IDS_MONDAY);
	m_ctlWeekdayCombo.AddString(sWeekDay);
	sWeekDay.LoadString(IDS_TUESDAY);
	m_ctlWeekdayCombo.AddString(sWeekDay);
	sWeekDay.LoadString(IDS_WEDNESDAY);
	m_ctlWeekdayCombo.AddString(sWeekDay);
	sWeekDay.LoadString(IDS_THURSDAY);
	m_ctlWeekdayCombo.AddString(sWeekDay);
	sWeekDay.LoadString(IDS_FRIDAY);
	m_ctlWeekdayCombo.AddString(sWeekDay);
	sWeekDay.LoadString(IDS_SATURDAY);
	m_ctlWeekdayCombo.AddString(sWeekDay);
	sWeekDay.LoadString(IDS_SUNDAY);
	m_ctlWeekdayCombo.AddString(sWeekDay);
	sWeekDay.LoadString(IDS_DAILY);
	m_ctlWeekdayCombo.AddString(sWeekDay);
	sWeekDay.LoadString(IDS_MO_FR);
	m_ctlWeekdayCombo.AddString(sWeekDay);
	sWeekDay.LoadString(IDS_SA_SU);
	m_ctlWeekdayCombo.AddString(sWeekDay);

	if (m_iWeekDayIndex>=0 && m_iWeekDayIndex<=9) 
	{
		m_ctlWeekdayCombo.SetCurSel(m_iWeekDayIndex);
	}
	if (m_iType==0) 
	{
		DisableAllDateWindows();
	}
	else
	{
		DisableAllWeekWindows();
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
int CTimerSetDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWK_Dialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CTimerSetDialog::FillHoursFromString(const CString &sHours)	// CAVEAT fixed format
{
	m_wndHours.FillHoursFromString(sHours);
}
/////////////////////////////////////////////////////////////////////////////
void CTimerSetDialog::SetDayType(const CString &sDay)
{
	int ix = GetIndexByLocalizedWeekDayName(sDay);

	
	if (ix != -1) 
	{
		m_iType=0;
		// CAVEAT can_T('t do that here, it')s not a window m_ctlWeekdayCombo.SetCurSel(ix);
		m_iWeekDayIndex = ix;
	}
	else
	{
		// split the string in its date parts
		CString sTime(sDay);
		ix = sTime.Find(_T('.'));
		m_sDay =  sTime.Left(ix);
		sTime = sTime.Mid(ix+1);

		ix = sTime.Find(_T('.'));
		m_sMonth = sTime.Left(ix);
		sTime = sTime.Mid(ix+1);

		m_sYear = sTime;
		
		m_iType=1;
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CTimerSetDialog::GetHoursAsString() const
{
	return m_wndHours.GetHoursAsString();
}
/////////////////////////////////////////////////////////////////////////////
CString CTimerSetDialog::GetDayAsString() const
{
	// wird nur für die Anzeige im Dialog benutzt
	CString sResult;
	if (m_iType==0)
	{
		switch (m_iWeekDayIndex)
		{
			case 0: sResult.LoadString(IDS_MONDAY); break;
			case 1: sResult.LoadString(IDS_TUESDAY); break;
			case 2: sResult.LoadString(IDS_WEDNESDAY); break;
			case 3: sResult.LoadString(IDS_THURSDAY); break;
			case 4: sResult.LoadString(IDS_FRIDAY); break;
			case 5: sResult.LoadString(IDS_SATURDAY); break;
			case 6: sResult.LoadString(IDS_SUNDAY); break;
			case 7: sResult.LoadString(IDS_DAILY); break;
			case 8: sResult.LoadString(IDS_MO_FR); break;
			case 9: sResult.LoadString(IDS_SA_SU); break;
			
			default: sResult = _T("Invalid WeekDay");
		}
	}
	else if (m_iType==1)
	{
		sResult.Format(_T("%s.%s."),m_sDay,m_sMonth);
		if (   m_sYear.Find(_T('*')) != -1
			|| m_sYear.IsEmpty()
			)
		{
			sResult += _T("****");
		}
		else
		{
			if (m_sYear.GetLength() == 4)
			{
				// the simple case, use the given year
				sResult += m_sYear;
			}
			else if (m_sYear.GetLength() == 3)
			{
				int y = _ttoi(m_sYear);
				if (y >= 990 && y <= 999)
				{
					sResult += _T("1");
					sResult += m_sYear;
				}
				else
				{
					sResult += _T("2");
					sResult += m_sYear;
				}
			}
			else
			{
				int y = _ttoi(m_sYear);
				if (y >= 90 && y <= 99)
				{
					sResult += _T("19");
					sResult += m_sYear;
				}
				else if (y >= 0 && y <= 9)
				{
					sResult += _T("200");
					sResult += m_sYear;
				}
				else
				{
					sResult += _T("20");
					sResult += m_sYear;
				}
			}
		}
	}
	else 
	{
		WK_TRACE_ERROR(_T("InternalError: weekDay type is %d\n"),m_iType);
	}
	return sResult;
}
/////////////////////////////////////////////////////////////////////////////
void CTimerSetDialog::OnSelchangeTimerWeekDayCombo() 
{
	m_iWeekDayIndex = m_ctlWeekdayCombo.GetCurSel();
	if (m_iWeekDayIndex<0 || m_iWeekDayIndex>9) {
		// OOPS
	}
}
/////////////////////////////////////////////////////////////////////////////
void CTimerSetDialog::OnUpdateTimerDay() 
{
	// EN_UPDATE
	CString sDay;
	m_ctlDay.GetWindowText(sDay);

	if (_ttoi(sDay) > 31)
	{
		UpdateData(FALSE);
		m_ctlDay.SetSel(0, -1);
	}
	else
	{
		// CAVEAT alreay restricted to numbers in the style
		if (sDay.GetLength()==2)
		{
			// 2 digits move to month
			m_ctlMonth.SetFocus();
			m_ctlMonth.SetSel(0, -1);
		}
	}
	// NOT YET restrict to certain numbers
}
/////////////////////////////////////////////////////////////////////////////
void CTimerSetDialog::OnUpdateTimerMonth() 
{
	// EN_UPDATE
	CString sMonth;
	m_ctlMonth.GetWindowText(sMonth);

	if (_ttoi(sMonth) > 12)
	{
		UpdateData(FALSE);
		m_ctlMonth.SetSel(0, -1);
	}
	else
	{
		// CAVEAT alreay restricted to numbers in the style
		if (sMonth.GetLength()==2)
		{
			// 2 digits move to year
			m_ctlYear.SetFocus();
			m_ctlYear.SetSel(0, -1);
		}
	}
	// NOT YET restrict to certain numbers
}
/////////////////////////////////////////////////////////////////////////////
void CTimerSetDialog::OnUpdateTimerYear() 
{
	// EN_UPDATE
	CString sYear;
	m_ctlYear.GetWindowText(sYear);

	// Alle Zeichen bis auf die Ziffern und * verbieten
	CString sInclude = sYear.SpanIncluding(_T("1234567890*"));
	if (sInclude.GetLength() != sYear.GetLength())
	{
		UpdateData(FALSE);
		m_ctlYear.SetSel(0, -1);
	}

	if (_ttoi(sYear) > 2200)
	{
		UpdateData(FALSE);
		m_ctlYear.SetSel(0, -1);
	}

	if (sYear.GetLength() > 4)
	{
		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CTimerSetDialog::OnOK() 
{
	UpdateData();
	// TODO: Add extra validation here
	if (IsDataValid())
	{
		CWK_Dialog::OnOK();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL  CTimerSetDialog::IsDataValid()
{
	BOOL bReturn = TRUE;
	CString sBuffer;
	m_ctlHoursText.GetWindowText(sBuffer);
	if (sBuffer.IsEmpty())
	{
		AfxMessageBox(IDP_TIMER_WITHOUT_TIME, MB_ICONSTOP|MB_OK);
		bReturn = FALSE;
	}
	else if (m_iType == 1)
	{
		BOOL bWrongDate = FALSE;
		int iDay = _ttoi(m_sDay);
		int iMonth = _ttoi(m_sMonth);
		int iYear = _ttoi(m_sYear);	// will be 0 for "****"

		// First check for correct year
		if (   m_sYear != _T("****")
			&& m_sYear.IsEmpty() == FALSE
			)
		{
			if (iYear == 0)	// first char is a '*', but not all
			{
				bWrongDate = TRUE;
			}
			else if (iYear < 1900)	// year to small
			{
				bWrongDate = TRUE;
			}
		}

		if (!bWrongDate)
		{
			if (iMonth == 4  ||
				iMonth == 6  ||
				iMonth == 9  ||
				iMonth == 11
				)
			{
				if (iDay > 30)
				{
					bWrongDate = TRUE;
				}
			}
			else if (iMonth == 2)
			{
				if (iDay > 29)
				{
					bWrongDate = TRUE;
				}
				else if (iDay == 29)
				{
					if (iYear != 0)
					{
						if (   iYear % 100 == 0
							&& iYear % 400 != 0
							)	// Only every 4. century there is a 29.2.
						{
							bWrongDate = TRUE;
						}
						else if (iYear % 4 != 0)	// Only every 4. years there is a 29.2.
						{
							bWrongDate = TRUE;
						}
					}
				}
			}
		}
		if (bWrongDate)
		{
			AfxMessageBox(IDP_TIMER_WRONG_DATE, MB_ICONSTOP|MB_OK);
			bReturn = FALSE;
		}
	}
	return bReturn;
}
