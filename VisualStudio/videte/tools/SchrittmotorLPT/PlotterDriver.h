#pragma once

// PlotterDriver
#include "LPTPort.h"
#include "PlotterDC.h"

struct MotorBits
{
    unsigned char motor1 : 2; /// pin 2 u 3
    unsigned char motor2 : 2; /// pin 4 u 5
    unsigned char motor3 : 2; /// pin 6 u 7
    unsigned char motor4 : 2; /// pin 8 u 9
};

typedef union 
{
    Bits b;
    MotorBits m;
}MotorControl;


class PlotterDriver : public CWinThread
{
	DECLARE_DYNCREATE(PlotterDriver)
    enum consts { Mask = 3 };
protected:
public:
	PlotterDriver();           // protected constructor used by dynamic creation
	virtual ~PlotterDriver();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

    BOOL setPortAddress(unsigned short aAdr);
    void setTestSteps(int aSteps) { mTestSteps = aSteps; }
    void setDelayIn_ms(int aDelay) { mDelay_ms = aDelay; }
    void setPlotCommands(const std::vector<CPlotterDC::DrawCommand>& aPlCmds) { mPlotCommands = aPlCmds; }


protected:
   LRESULT OnTestTurnLeft(WPARAM wParam, LPARAM lParam);
   LRESULT OnTestTurnRight(WPARAM wParam, LPARAM lParam);
   LRESULT OnPlotCommands(WPARAM wParam, LPARAM lParam);
   LRESULT OnCalibratePlotter(WPARAM wParam, LPARAM lParam);
   DECLARE_MESSAGE_MAP()

private:
    void MoveToPos(int aPosX, int aPosY);
    void LineToPos(int aPosX, int aPosY);
    void sendStep();
    void transform_point(FLOAT aXin, FLOAT aYin, int &aXout, int& aYout);

private:
    LPTPort         mPort;
    Bits            mControl;
    MotorControl    mMotors;
    int             mPositionX;
    int             mPositionY;
    int             mPositionZ;
    int             mTestSteps;
    int             mDelay_ms;
    multibeam::matrix<float> mTransformation;
    std::vector<CPlotterDC::DrawCommand> mPlotCommands;

    static const BYTE mControlValue[4];
};


