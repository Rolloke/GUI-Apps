// LPTPortTest.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "SchrittmotorLPT.h"
#include "LPTPortTest.h"
#include "PlotterDriver.h"


// CLPTPortTest-Dialogfeld

IMPLEMENT_DYNAMIC(CLPTPortTest, CDialog)

CLPTPortTest::CLPTPortTest(CWnd* pParent /*=NULL*/)
	: CDialog(CLPTPortTest::IDD, pParent)
    , mPollTimer(0)
    , mTestSteps(500)
    , mMotorDriver(0)
{

}

CLPTPortTest::~CLPTPortTest()
{
}

void CLPTPortTest::DoDataExchange(CDataExchange* pDX)
{
	 CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDT_TEST_STEPS, mTestSteps);
    DDV_MinMaxInt(pDX, mTestSteps, 1, 10000);
}


BEGIN_MESSAGE_MAP(CLPTPortTest, CDialog)
    ON_BN_CLICKED(IDC_BTN_RIGHT, &CLPTPortTest::OnBnClickedBtnRight)
    ON_BN_CLICKED(IDC_BTN_LEFT, &CLPTPortTest::OnBnClickedBtnLeft)
    ON_BN_CLICKED(IDC_GET_STATUS_BITS, &CLPTPortTest::OnBnClickedGetStatusBits)
    ON_BN_CLICKED(IDC_SET_DATA_BITS, &CLPTPortTest::OnBnClickedSetDataBits)
    ON_BN_CLICKED(IDC_SET_CONTROL_BITS, &CLPTPortTest::OnBnClickedSetControlBits)
    ON_BN_CLICKED(IDC_CK_POLL_STATUS_BITS, &CLPTPortTest::OnBnClickedCkPollStatusBits)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BTN_UP, &CLPTPortTest::OnBnClickedBtnUp)
    ON_BN_CLICKED(IDC_BTN_DOWN, &CLPTPortTest::OnBnClickedBtnDown)
    ON_BN_CLICKED(IDC_BTN_BACKWARD, &CLPTPortTest::OnBnClickedBtnBackward)
    ON_BN_CLICKED(IDC_BTN_FORWARD, &CLPTPortTest::OnBnClickedBtnForward)
END_MESSAGE_MAP()


// CLPTPortTest-Meldungshandler

void CLPTPortTest::OnBnClickedBtnLeft()
{
    if (UpdateData())
    {
        mMotorDriver->PostThreadMessage(WM_TEST_RUN, mTestSteps, PlotterDriver::left);
    }
}

void CLPTPortTest::OnBnClickedBtnRight()
{
    if (UpdateData())
    {
        mMotorDriver->PostThreadMessage(WM_TEST_RUN, mTestSteps, PlotterDriver::right);
    }
}

void CLPTPortTest::OnBnClickedBtnForward()
{
    if (UpdateData())
    {
       mMotorDriver->PostThreadMessage(WM_TEST_RUN, mTestSteps, PlotterDriver::forward);
    }
}

void CLPTPortTest::OnBnClickedBtnBackward()
{
    if (UpdateData())
    {
        mMotorDriver->PostThreadMessage(WM_TEST_RUN, mTestSteps, PlotterDriver::backward);
    }
}

void CLPTPortTest::OnBnClickedBtnUp()
{
    if (UpdateData())
    {
        mMotorDriver->PostThreadMessage(WM_TEST_RUN, mTestSteps, PlotterDriver::up);
    }
}

void CLPTPortTest::OnBnClickedBtnDown()
{
    if (UpdateData())
    {
        mMotorDriver->PostThreadMessage(WM_TEST_RUN, mTestSteps, PlotterDriver::down);
    }
}

void CLPTPortTest::OnBnClickedGetStatusBits()
{
   int i;
   UCHAR j; 
   Bits bits = mPort.getBits(LPTPort::status);
   for (i=IDC_STATUS1, j=1; i<=IDC_STATUS8; ++i, j<<=1)
   {
       CheckDlgButton(i, (bits.bits & j) ? BST_CHECKED : BST_UNCHECKED);
   }
}

void CLPTPortTest::OnBnClickedSetDataBits()
{
   int i;
   UCHAR j; 
   Bits bits;
   bits.bits = 0;
   for (i=IDC_CK_DATA1, j=1; i<=IDC_CK_DATA8; ++i, j<<=1)
   {
      if(IsDlgButtonChecked(i))
      {
          bits.bits |= j;
      }

   }
   mPort.setBits(LPTPort::data, bits);
}

void CLPTPortTest::OnBnClickedSetControlBits()
{
   int i;
   UCHAR j; 
   Bits bits;
   bits.bits = 0;
   for (i=IDC_CONTROL1, j=1; i<=IDC_CONTROL8; ++i, j<<=1)
   {
      if(IsDlgButtonChecked(i))
      {
          bits.bits |= j;
      }
   }
   mPort.setBits(LPTPort::control, bits);
}

BOOL CLPTPortTest::OnInitDialog()
{
    CDialog::OnInitDialog();
    int i;
    UCHAR j; 
    Bits bits;
    bits = mPort.getBits(LPTPort::data);
    for (i=IDC_CK_DATA1, j=1; i<=IDC_CK_DATA8; ++i, j<<=1)
    {
        CheckDlgButton(i, (bits.bits & j) ? BST_CHECKED : BST_UNCHECKED);
    }

    bits = mPort.getBits(LPTPort::control);
    for (i=IDC_CONTROL1, j=1; i<=IDC_CONTROL8; ++i, j<<=1)
    {
        CheckDlgButton(i, (bits.bits & j) ? BST_CHECKED : BST_UNCHECKED);
    }
    OnBnClickedGetStatusBits();

    return TRUE;  // return TRUE unless you set the focus to a control
    // AUSNAHME: OCX-Eigenschaftenseite muss FALSE zurückgeben.
}

void CLPTPortTest::OnBnClickedCkPollStatusBits()
{
    if (IsDlgButtonChecked(IDC_CK_POLL_STATUS_BITS))
    {
        mPollTimer = SetTimer(100, 200, NULL);
    }
    else if (mPollTimer)
    {
        KillTimer(mPollTimer);
        mPollTimer = 0;
    }
}

void CLPTPortTest::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == mPollTimer)
    {
        OnBnClickedGetStatusBits();
    }

    CDialog::OnTimer(nIDEvent);
}

