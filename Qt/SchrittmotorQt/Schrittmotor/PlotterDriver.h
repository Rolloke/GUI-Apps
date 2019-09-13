#pragma once

#include "LPTPort.h"
#include "PlotterDC.h"
#include "defines.h"

// PlotterDriver
#include <QThread>
#include <QWidget>
#include <QActionEvent>
#include <QSemaphore>
#include <QMutex>
#include <memory>


struct MotorBits
{
    unsigned char motor1 : 2; /// pin 2 u 3
    unsigned char motor2 : 2; /// pin 4 u 5
    unsigned char motor3 : 2; /// pin 6 u 7
    unsigned char motor4 : 2; /// pin 8 u 9
};

struct HalfStepMotorBits
{
    unsigned char motor1 : 4; /// pin 2, 3, 4, 5
    unsigned char motor2 : 4; /// pin 6, 7, 8, 9
};


union MotorControl
{
    Bits b;
    MotorBits m;
    HalfStepMotorBits hsm;
};

struct PlotterControlBits
{
    unsigned char motor_coil1       : 1; /// pin 1  (inv)
    unsigned char motor_coil2       : 1; /// pin 14 (inv)
    unsigned char init_printer      : 1; /// pin 16
    unsigned char enable_motor      : 1; /// pin 17 (inv)
    unsigned char enable_interrupt  : 1;
    unsigned char notused1          : 1;
    unsigned char notused2          : 1;
    unsigned char notused3          : 1;
};

union PlotterControl
{
    Bits b;
    PlotterControlBits p;
    MotorBits m;
};

struct PlotterStatusBits
{
    unsigned char notused1          : 1; 
    unsigned char notused2          : 1;
    unsigned char notused3          : 1;
    unsigned char error             : 1; /// pin 15
    unsigned char right_end         : 1; /// pin 13
    unsigned char top_end           : 1; /// pin 12
    unsigned char left_end          : 1; /// pin 10
    unsigned char pen_down          : 1; /// pin 11 (inv)
};

union PlotterStatus
{
    Bits b;
    PlotterStatusBits s;
};

class PlotterDriverConnection;

class PlotterDriver : public QThread
{
public:
	PlotterDriver();           // protected constructor used by dynamic creation
	virtual ~PlotterDriver();

public:
    enum consts { FullStepMask = 3, HalfStepMask = 7 };
    enum motor { unknown_motor =-1, fsbipolar_motor1=0, fsbipolar_motor2, fsbipolar_motor3, fsbipolar_motor4,
                                    hsbipolar_motor1 , hsbipolar_motor3 ,
                                    hsunipolar_motor1, hsunipolar_motor3,
                                    hsbipolarctrl_motor1, hsbipolarctrl_motor2, hsbipolarctrl_motor3, hsbipolarctrl_motor4  };
    enum direction { xdir, ydir, zdir, directions, left=0x10, right=0x20, forward=0x40, backward=0x80, up=0x100, down=0x200 };
    enum offset { to_paper, to_plate, to_moving_pos, offsets };
    enum delay { horizonal_delay, pen_delay, drill_delay, manual_delay_horizonal, manual_delay_vertical, horizonal_start_delay, horizonal_ramp_steps, horizontal_ramp_function, pen_start_delay, pen_ramp_steps, pen_ramp_function, delays };

    struct Command
    {
        enum eCmd
        {
            UnKnown,
            Plot,
            TestTurn,
            Drill,
            CalibratePen,
            CalibratePlotter,
            TestPen,
            Quit
        };

        Command(eCmd aCmd, WPARAM aw, LPARAM al): mCmd(aCmd), wParam(aw), lParam(al)
        {

        }
        eCmd mCmd;
        WPARAM wParam;
        LPARAM lParam;
    };

    class Ramp
    {
    public:
        enum eRamp { time, frequency, function };
        Ramp(float aStart, float aStop, long aSteps, long aLoop, eRamp aUseFrequency=time);
        Ramp(const std::vector<float>& aFunction, long aLoop);
        float getDelay(int aIndex);
    private:
        long mUpIndex;
        long mDownIndex;
        float mValue;
        float mStep;
        eRamp mRamp;
        std::vector<float> mFunction;
    };

    bool setPortAddress(unsigned short aAdr);
    bool setPortPath(const char* aPath);
    const LPTPort& getLPT();
    bool setMotorDirection(motor aMotor, direction aDir);
    void setDelayIn_ms(const std::vector<float>& aDelay);
    void setCalibrationDistance(const std::vector<float>& aCD);
    void setCalibrationSteps(const std::vector<int>& aCD);
    void setPlotCommands(const CPlotterDC::DrawCommandList& aPlCmds) { mPlotCommands = aPlCmds; }
    void setPenPosition(FLOAT aXpos, FLOAT aYpos);
    void setPenOffsets(const std::vector<FLOAT>& aOffsets) { mPenOffsets = aOffsets; }
    void setUpEndPosition();
    void setDownEndPosition();
    void setDrillEndPosition();
    void setCountMS(bool aCount);
    float getDurationMS();

    unsigned short            getPortAddress();
    PlotterDriver::motor      getMotor(direction aDir);
    const std::vector<float>& getDelays() const { return mDelay_ms; }
    const std::vector<float>& getCalibrationDistance() const { return mCalibrationDistance; }
    const std::vector<int>&   getCalibrationSteps() const { return mSteps; }
    const std::vector<FLOAT>& getPenOffsets() const { return mPenOffsets; }

    UINT getButtonID() { return mButtonID; }
    void stopRun();
    void PostCommand(const Command::eCmd& aCmd, WPARAM wParam=0, LPARAM lParam=0);
    static const char* getNameOfEnum(delay eDelay);

protected:
   void run();
   void OnPlotCommands( );
   void OnTestTurn(WPARAM wParam, LPARAM lParam);
   void OnDrillCommands();
   void OnCalibratePen();
   void OnCalibratePlotter(WPARAM , LPARAM );
   void OnTestPen(WPARAM wParam, LPARAM );

private:
    void moveToPos(int aPosX, int aPosY);
    void lineToPos(int aPosX, int aPosY);
    void Circle(int x0, int y0, int radius, int aStartAngle=0, int aStopAngle=360);
    void enableMotors(bool aEnable);
    bool isXdirectionEnd(const PlotterStatus &fStatus);
    bool isYdirectionEnd(const PlotterStatus &fStatus);
    void getPlotterStatus(PlotterStatus &aStatus);
    void movePenUp();
    void movePenDown();
    void drillDown();

    void sendStep();
    void transform_point(FLOAT aXin, FLOAT aYin, int &aXout, int& aYout);

    void stepBipolarFullMotor1(int aPos, bool);
    void stepBipolarFullMotor2(int aPos, bool);
    void stepBipolarFullMotor3(int aPos, bool);
    void stepBipolarFullMotor4(int aPos, bool);
    void stepBipolarHalfMotor1(int aPos, bool bEnable);
    void stepBipolarHalfMotor3(int aPos, bool bEnable);
    void stepUnipolarHalfMotor1(int aPos, bool bEnable);
    void stepUnipolarHalfMotor3(int aPos, bool bEnable);
    void stepBipolarHalfCtrlMotor1(int aPos, bool bEnable);
    void stepBipolarHalfCtrlMotor2(int aPos, bool bEnable);
    void stepBipolarHalfCtrlMotor3(int aPos, bool bEnable);
    void stepBipolarHalfCtrlMotor4(int aPos, bool bEnable);
    void Sleep(float aMS);

    void UpdatePlotterParam(float, int);
    void UpdatePlotterPosition(int, int, int);

   bool  isCountingMS();

private:
    LPTPort         mPort;
    PlotterControl  mControl;
    MotorControl    mMotors;
    int             mPositionX;
    int             mPositionY;
    int             mPositionZ;
    QRect           mClipRect;

    std::vector<float> mDelay_ms;
    std::vector<float> mCalibrationDistance;
    std::vector<int>   mSteps;
    std::vector<FLOAT> mPenOffsets;

    void (PlotterDriver::*mStepX)(int, bool);
    void (PlotterDriver::*mStepY)(int, bool);
    void (PlotterDriver::*mStepZ)(int, bool);
    rke::matrix<float> mTransformation;

    CPlotterDC::DrawCommandList mPlotCommands;
    volatile bool   mRun;
    UINT            mButtonID;
    int             mUpEndPosition;
    int             mDownEndPosition;
    int             mDrillEndPosition;
    float           mMilliSecondCounter;

    QSemaphore     mSemaphore;
    QMutex         mMutex;
    QList<Command> mCommands;

    static const BYTE mBipolarFullStepControl[4];
    static const BYTE mBipolarHalfStepControl[8];
    static const BYTE mBipolarHalfStepControlBit[8];
    static const BYTE mBipolarHalfStepControlEnable[8];
    static const BYTE mUnipolarHalfStepControl[8];
public:
    bool           mUseLptDriver;
    std::shared_ptr<PlotterDriverConnection> mConnection;
};


