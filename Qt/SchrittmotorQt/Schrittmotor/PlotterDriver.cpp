// PlotterDriver.cpp : implementation file
//

#include "PlotterDriver.h"

#include "plotterdriverconnection.h"

// helpers
// signum function 
int sgn(int x)
{
  return (x > 0) ? 1 : (x < 0) ? -1 : 0;
}


// PlotterDriver
const BYTE PlotterDriver::mBipolarFullStepControl[4] = { 0x0, 0x1, 0x3, 0x2 };
const BYTE PlotterDriver::mBipolarHalfStepControl[8] = { 0xc, 0x8, 0xe, 0x6, 0xf, 0xb, 0xd, 0x5 }; 
// Halfstep matrix for bipolar drive
// step    1 2 3 4 5 6 7 8
// enable0 1 1 1 0 1 1 1 0
// enable1 1 0 1 1 1 0 1 1
// bit0    0 0 1 1 1 1 0 0
// bit1    0 0 0 0 1 1 1 1
// hex     c 8 e 6 f b d 5
const BYTE PlotterDriver::mBipolarHalfStepControlBit[8]   =  { 0x0, 0x0, 0x1, 0x1, 0x3, 0x3, 0x2, 0x2 }; 
//const BYTE PlotterDriver::mBipolarHalfStepControlEnable[8] = { 0x0, 0x2, 0x0, 0x1, 0x0, 0x2, 0x0, 0x1 };
const BYTE PlotterDriver::mBipolarHalfStepControlEnable[8] = { 0x3, 0x1, 0x3, 0x2, 0x3, 0x1, 0x3, 0x2 };

//const BYTE PlotterDriver::mBipolarHalfStepControlEnable[8] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }; 

const BYTE PlotterDriver::mUnipolarHalfStepControl[8] = { 0x1, 0x9, 0x8, 0xa, 0x2, 0x6, 0x4, 0x5 }; 
// Halfstep matrix for unipolar drive
// step    1 2 3 4 5 6 7 8
// bit1    1 1 0 0 0 0 0 1
// bit2    0 0 0 1 1 1 0 0
// bit3    0 0 0 0 0 1 1 1
// bit4    0 1 1 1 0 0 0 0
// hex     1 9 8 a 2 6 4 5


PlotterDriver::Ramp::Ramp(float aStart, float aStop, long aSteps, long aLoop, eRamp aRamp)
: mUpIndex(0)
, mDownIndex(0)
, mValue(0)
, mStep(0)
, mRamp(aRamp)
{
    if (aStart > aStop && aSteps > 0)
    {
        switch (mRamp)
        {
        case frequency:
        {
            // TODO: test frequency
            mValue     = 1000.0f / aStart;
            mStep      = (1000.0f/aStop - 1000.0f/aStart) / static_cast<float>(aSteps);
        }   break;

        case time:
        {
            mValue     = aStart;
            mStep      = (aStop - aStart) / static_cast<float>(aSteps);
        } break;

        case function:
            assert(false);
            break;
        default: break;
        }

        if (aSteps * 2 < aLoop)
        {
            mUpIndex   = aSteps-1;
            mDownIndex = aLoop-aSteps;
        }
        else
        {
            mUpIndex   = aLoop/2-1;
            mDownIndex = aLoop/2;
        }

    }
    else if (aStart > 0)
    {
        mValue = aStart;
    }
    else
    {
        mValue = aStop;
    }
}

PlotterDriver::Ramp::Ramp(const std::vector<float>& aFunction, long aLoop)
: mUpIndex(0)
, mDownIndex(0)
, mValue(0)
, mStep(0)
, mRamp(function)
, mFunction(aFunction)
{
    int fSteps = mFunction.size();
    if (fSteps * 2 < aLoop)
    {
        mUpIndex   = fSteps;
        mDownIndex = aLoop-fSteps;
    }
    else
    {
        mUpIndex   = aLoop/2-1;
        mDownIndex = aLoop/2;
    }

}

float PlotterDriver::Ramp::getDelay(int aIndex)
{
    switch (mRamp)
    {
    case frequency:
    {
        if (mStep > 0)      // increase frequency for rising ramp
        {
            mValue += mStep;
            if (aIndex == mUpIndex) mStep = -mStep;
        }
        else if (mStep < 0  // decrease frequency for falling ramp
                 && aIndex >= mDownIndex)
        {
            mValue += mStep;
        }
        return 1000.0f / mValue;
    }   break;
    case time:
    {
        if (mStep < 0)      // decrease time for rising ramp
        {
            mValue += mStep;
            if (aIndex == mUpIndex) mStep = -mStep;
        }
        else if (mStep > 0  // increase time for falling ramp
                 && aIndex >= mDownIndex)
        {
            mValue += mStep;
        }
        return mValue;
    }
    case function:
    {
        if (aIndex < mUpIndex)
        {
        }
        else if (aIndex>=mDownIndex)
        {
            aIndex -= mDownIndex;
        }
        else
        {
            aIndex = mUpIndex-1;
        }
        return mFunction[aIndex];
    }

    }
    return 10;
}

const char* PlotterDriver::getNameOfEnum(PlotterDriver::delay eDelay)
{
    switch (eDelay)
    {
    case horizonal_delay: return "horizonal";
    case pen_delay: return "pen";
    case drill_delay: return "drill";
    case manual_delay_horizonal: return "manual_horizonal";
    case manual_delay_vertical: return "manual_vertical";
    case horizonal_start_delay: return "horizonal_start";
    case horizonal_ramp_steps: return "horizonal_ramp_steps";
    case horizontal_ramp_function: return "horizontal_ramp_function";
    case pen_start_delay: return "pen_start";
    case pen_ramp_steps: return "pen_ramp_steps";
    case pen_ramp_function: return "pen_ramp_function";
    default: return "unknown";
    }
}

PlotterDriver::PlotterDriver() :
  mPositionX(0)
, mPositionY(0)
, mPositionZ(0)
, mStepX(&PlotterDriver::stepBipolarFullMotor1)
, mStepY(&PlotterDriver::stepBipolarFullMotor2)
, mStepZ(&PlotterDriver::stepBipolarFullMotor3)
, mRun(true)
, mButtonID(0)
, mUpEndPosition(0)
, mDownEndPosition(0)
, mDrillEndPosition(0)
, mMilliSecondCounter(-1)
, mUseLptDriver(false)
{
    mMotors.b.bits = 0;
    mControl.b.bits = 0;
    enableMotors(false);
    mTransformation.identity(directions);
    mCalibrationDistance.resize(directions, -1);
    mSteps.resize(directions, 10);
    mDelay_ms.resize(delays, 10);
    mPenOffsets.resize(offsets, static_cast<FLOAT>(3.0));
    mConnection.reset(new PlotterDriverConnection(this));
    mConnection->moveToThread(this);
}

PlotterDriver::~PlotterDriver()
{
    PostCommand(Command::Quit);
    wait();
}


void PlotterDriver::PostCommand(const PlotterDriver::Command::eCmd& aCmd, WPARAM wParam, LPARAM lParam)
{
    mMutex.lock();
    mCommands.push_back(Command(aCmd, wParam, lParam));
    mMutex.unlock();
    mSemaphore.release();
}

void PlotterDriver::run()
{
    Command fCmd(Command::UnKnown, 0, 0);
    do
    {
        mSemaphore.acquire();
        mMutex.lock();
        if (not mCommands.empty())
        {
            fCmd = mCommands.front();
            mCommands.pop_front();
        }
        mMutex.unlock();
        switch (fCmd.mCmd)
        {
        case Command::Plot:             OnPlotCommands(); break;
        case Command::Drill:            OnDrillCommands(); break;
        case Command::CalibratePen:     OnCalibratePen(); break;
        case Command::CalibratePlotter: OnCalibratePlotter(fCmd.wParam, fCmd.lParam);break;
        case Command::TestTurn:         OnTestTurn(fCmd.wParam, fCmd.lParam);break;
        case Command::TestPen:          OnTestPen(fCmd.wParam, fCmd.lParam);break;
        default:break;
        }

    }
    while (fCmd.mCmd != Command::Quit);
}

bool PlotterDriver::setPortAddress(unsigned short aAdr)
{
    bool bReturn = mPort.setPortBaseAddress(aAdr);
    if (bReturn)
    {
        enableMotors(false);
        mPort.setBits(LPTPort::data, mMotors.b);
        mPort.setBits(LPTPort::control, mControl.b);
    }
    else
    {
        if (mPort.getPorts())
        {
            mPort.setPortPath(mPort.getPortPath(0));
        }
    }
    return bReturn;
}

bool PlotterDriver::setPortPath(const char* aPath)
{
    bool bReturn = mPort.setPortPath(aPath);
    if (bReturn)
    {
        enableMotors(false);
        mPort.setBits(LPTPort::data, mMotors.b);
        mPort.setBits(LPTPort::control, mControl.b);
    }
    return bReturn;
}

const LPTPort& PlotterDriver::getLPT()
{
    return mPort;
}

unsigned short PlotterDriver::getPortAddress()
{
    return mPort.getPortBaseAddress();
}

bool PlotterDriver::setMotorDirection(motor aMotor, direction aDir)
{
    void (PlotterDriver::*fMotor)(int, bool) = 0;
    switch (aMotor)
    {
    case fsbipolar_motor1:
        fMotor = &PlotterDriver::stepBipolarFullMotor1;
        mPort.setMotor((stepper::eDirection)aDir, stepper::BipolarFull);
        mPort.setMotorBits((stepper::eDirection)aDir, 1, 2, 0, 0);
        break;
    case fsbipolar_motor2:
        fMotor = &PlotterDriver::stepBipolarFullMotor2;
        mPort.setMotor((stepper::eDirection)aDir, stepper::BipolarFull);
        mPort.setMotorBits((stepper::eDirection)aDir, 3, 4, -9, -10);
        break;
    case fsbipolar_motor3:
        fMotor = &PlotterDriver::stepBipolarFullMotor3;
        mPort.setMotor((stepper::eDirection)aDir, stepper::BipolarFull);
        mPort.setMotorBits((stepper::eDirection)aDir, 5, 6, 0, 0);
        break;
    case fsbipolar_motor4:
        fMotor = &PlotterDriver::stepBipolarFullMotor4;
        mPort.setMotor((stepper::eDirection)aDir, stepper::BipolarFull);
        mPort.setMotorBits((stepper::eDirection)aDir, 7, 8, 0, 0);
        break;
    case hsbipolar_motor1:
        fMotor = &PlotterDriver::stepBipolarHalfMotor1;
        mPort.setMotor((stepper::eDirection)aDir, stepper::BipolarHalf);
        mPort.setMotorBits((stepper::eDirection)aDir, 1, 2, 3, 4);
        break;
    case hsbipolar_motor3:
        fMotor = &PlotterDriver::stepBipolarHalfMotor3;
        mPort.setMotor((stepper::eDirection)aDir, stepper::BipolarHalf);
        mPort.setMotorBits((stepper::eDirection)aDir, 5, 6, 7, 8);
        break;
    case hsunipolar_motor1:
        fMotor = &PlotterDriver::stepUnipolarHalfMotor1;
        mPort.setMotor((stepper::eDirection)aDir, stepper::UnipolarHalf);
        mPort.setMotorBits((stepper::eDirection)aDir, 1, 2, 3, 4);
        break;
    case hsunipolar_motor3:
        fMotor = &PlotterDriver::stepUnipolarHalfMotor3;
        mPort.setMotor((stepper::eDirection)aDir, stepper::UnipolarHalf);
        mPort.setMotorBits((stepper::eDirection)aDir, 5, 6, 7, 8);
        break;
    case hsbipolarctrl_motor1:
        fMotor = &PlotterDriver::stepBipolarHalfCtrlMotor1;
        mPort.setMotor((stepper::eDirection)aDir, stepper::BipolarHalf);
        mPort.setMotorBits((stepper::eDirection)aDir, 1, 2, -9, -10);
        break;
    case hsbipolarctrl_motor2:
        fMotor = &PlotterDriver::stepBipolarHalfCtrlMotor2;
        mPort.setMotor((stepper::eDirection)aDir, stepper::BipolarHalf);
        mPort.setMotorBits((stepper::eDirection)aDir, 3, 4, -9, -10);
        break;
    case hsbipolarctrl_motor3:
        fMotor = &PlotterDriver::stepBipolarHalfCtrlMotor3;
        mPort.setMotor((stepper::eDirection)aDir, stepper::BipolarHalf);
        mPort.setMotorBits((stepper::eDirection)aDir, 5, 6, -9, -10);
        break;
    case hsbipolarctrl_motor4:
        fMotor = &PlotterDriver::stepBipolarHalfCtrlMotor4;
        mPort.setMotor((stepper::eDirection)aDir, stepper::BipolarHalf);
        mPort.setMotorBits((stepper::eDirection)aDir, 7, 8, -9, -10);
        break;
    default: return false;
    }

    switch (aDir)
    {
    case xdir: mStepX = fMotor; break;
    case ydir: mStepY = fMotor; break;
    case zdir: mStepZ = fMotor; break;
    default: return false;
    }

    if (mUseLptDriver)
    {
        mPort.setStatusBit(stepper::left_end, 7);
        mPort.setStatusBit(stepper::right_end, 5);
        mPort.setStatusBit(stepper::top_end, 6);
        mPort.setStatusBit(stepper::bottom_end, 4);
        mPort.setStatusBit(stepper::pen_down, 8);
        mPort.setEnableMotorsBit(-12);
        mPort.setAsynchron(2);
    }
    return (fMotor!=0);
}

void PlotterDriver::setDelayIn_ms(const std::vector<float>& aDelay)
{
    mPort.setDelay(stepper::xdir, static_cast<long>(aDelay[horizonal_delay]*1000));
    mPort.setDelay(stepper::ydir, static_cast<long>(aDelay[horizonal_delay]*1000));

    mPort.setStartDelay(stepper::xdir, static_cast<long>(aDelay[horizonal_start_delay]*1000));
    mPort.setStartDelay(stepper::ydir, static_cast<long>(aDelay[horizonal_start_delay]*1000));

    mPort.setRampSteps(stepper::xdir, static_cast<long>(aDelay[horizonal_ramp_steps]*1000));
    mPort.setRampSteps(stepper::ydir, static_cast<long>(aDelay[horizonal_ramp_steps]*1000));

    mPort.setDelay(stepper::zdir, static_cast<long>(aDelay[pen_delay]*1000));
    mPort.setStartDelay(stepper::zdir, static_cast<long>(aDelay[pen_start_delay]*1000));
    mPort.setRampSteps(stepper::zdir, static_cast<long>(aDelay[pen_ramp_steps]*1000));

    mDelay_ms = aDelay;
}

PlotterDriver::motor PlotterDriver::getMotor(direction aDir)
{
    void (PlotterDriver::*fMotor)(int, bool) = 0;
    switch (aDir)
    {
    case xdir: fMotor = mStepX; break;
    case ydir: fMotor = mStepY; break;
    case zdir: fMotor = mStepZ; break;
    default: return unknown_motor;
    }
    if (fMotor == &PlotterDriver::stepBipolarFullMotor1) return fsbipolar_motor1;
    if (fMotor == &PlotterDriver::stepBipolarFullMotor2) return fsbipolar_motor2;
    if (fMotor == &PlotterDriver::stepBipolarFullMotor3) return fsbipolar_motor3;
    if (fMotor == &PlotterDriver::stepBipolarFullMotor4) return fsbipolar_motor4;
    if (fMotor == &PlotterDriver::stepBipolarHalfMotor1) return hsbipolar_motor1;
    if (fMotor == &PlotterDriver::stepBipolarHalfMotor3) return hsbipolar_motor3;
    if (fMotor == &PlotterDriver::stepUnipolarHalfMotor1) return hsunipolar_motor1;
    if (fMotor == &PlotterDriver::stepUnipolarHalfMotor3) return hsunipolar_motor3;
    if (fMotor == &PlotterDriver::stepBipolarHalfCtrlMotor1) return hsbipolarctrl_motor1;
    if (fMotor == &PlotterDriver::stepBipolarHalfCtrlMotor2) return hsbipolarctrl_motor2;
    if (fMotor == &PlotterDriver::stepBipolarHalfCtrlMotor3) return hsbipolarctrl_motor3;
    if (fMotor == &PlotterDriver::stepBipolarHalfCtrlMotor4) return hsbipolarctrl_motor4;
    return unknown_motor;
}

void PlotterDriver::stopRun()
{
    if (mUseLptDriver)
    {
        mPort.enableMotors(false);
    }
    mRun = false;
}

// PlotterDriver message handlers
void PlotterDriver::OnTestTurn(WPARAM aTestSteps, LPARAM aDir)
{
    mRun = true;
    enableMotors(mRun);
    int nDelay = mDelay_ms[manual_delay_horizonal];
    PlotterStatus fStatus;
    fStatus.b.bits = 0;

    for (UINT i=0; i<aTestSteps && mRun; ++i)
    {
        getPlotterStatus(fStatus);
        if (aDir == PlotterDriver::down  && fStatus.s.pen_down ) { break; }
        if (aDir == PlotterDriver::right && fStatus.s.right_end) { break; }
        if (aDir == PlotterDriver::left  && fStatus.s.left_end ) { break; }
        switch (aDir)
        {
        case PlotterDriver::left:     ++mPositionX; break;
        case PlotterDriver::right:    --mPositionX; break;
        case PlotterDriver::forward:  ++mPositionY; break;
        case PlotterDriver::backward: --mPositionY; break;
        case PlotterDriver::down:   { ++mPositionZ; nDelay = mDelay_ms[manual_delay_vertical]; } break;
        case PlotterDriver::up:     { --mPositionZ; nDelay = mDelay_ms[manual_delay_vertical]; } break;
        }
        //TRACE(Logger::info,"Pos: %d, %d, %d\n", mPositionX, mPositionY, mPositionZ);

        UpdatePlotterPosition(mPositionX, mPositionY, mPositionZ);
        sendStep();
        Sleep(nDelay);
    }
    if (mUseLptDriver)
    {
        mPort.setPosition(stepper::xdir, mPositionX);
        mPort.setPosition(stepper::ydir, mPositionY);
        mPort.setPosition(stepper::zdir, mPositionZ);
    }
    UpdatePlotterPosition(mPositionX, mPositionY, mPositionZ);
    mRun = false;
    enableMotors(mRun);
}

void PlotterDriver::OnTestPen(WPARAM aPos, LPARAM )
{
    mRun = true;
    enableMotors(mRun);
    if (aPos == 2)
    {
        movePenDown();
    }
    else if (aPos == 1 )
    {
        drillDown();
    }
    else if (aPos == 0)
    {
        movePenUp();
    }
    UpdatePlotterPosition(mPositionX, mPositionY, mPositionZ);
    mRun = false;
    enableMotors(mRun);
}

void PlotterDriver::enableMotors(bool aEnable)
{
    if (isCountingMS())
    {
        aEnable = false;
    }
    if (mUseLptDriver)
    {
        mPort.enableMotors(aEnable);
    }
    else
    {
        mControl.p.enable_motor = !aEnable;
        mPort.setBits(LPTPort::control, mControl.b);
        if (!aEnable)
        {
            if (mStepX) (*this.*mStepX)(mPositionX, aEnable);
            if (mStepY) (*this.*mStepY)(mPositionY, aEnable);
            if (mStepZ) (*this.*mStepZ)(mPositionZ, aEnable);
        }
    }
}

void PlotterDriver::setCalibrationDistance(const std::vector<float>& aCD)
{
    mCalibrationDistance = aCD;
}

void PlotterDriver::setCalibrationSteps(const std::vector<int>& aTS)
{
    mSteps = aTS;
    std::vector<float> fScale;
    fScale.resize(directions, 1);
    mTransformation.identity(directions);
    if (aTS[xdir]) fScale[xdir] = static_cast<float>(aTS[xdir]) / (mCalibrationDistance[xdir]*SCALE_FACTOR);
    if (aTS[ydir]) fScale[ydir] = static_cast<float>(aTS[ydir]) / (mCalibrationDistance[ydir]*SCALE_FACTOR);
    if (aTS[zdir]) fScale[zdir] = static_cast<float>(aTS[zdir]) / (mCalibrationDistance[zdir]*SCALE_FACTOR);
    mTransformation.scale(fScale);
   
}

void PlotterDriver::setPenPosition(FLOAT aXpos, FLOAT aYpos)
{
    transform_point(aXpos, aYpos, mPositionX, mPositionY);
    UpdatePlotterPosition(mPositionX, mPositionY, mPositionZ);
}

void PlotterDriver::setUpEndPosition()
{
    mPort.setPenPos(stepper::Pen_up, mPort.getPosition(stepper::zdir));
    mUpEndPosition = mPositionZ;
}

void PlotterDriver::setDownEndPosition()
{
    mPort.setPenPos(stepper::Pen_down, mPort.getPosition(stepper::zdir));
    mDownEndPosition = mPositionZ;
}

void PlotterDriver::setDrillEndPosition()
{
    mPort.setPenPos(stepper::Pen_drill, mPort.getPosition(stepper::zdir));
    mDrillEndPosition = mPositionZ;
}

void PlotterDriver::getPlotterStatus(PlotterStatus &aStatus)
{
    if (not isCountingMS())
    {
        aStatus.b = mPort.getBits(LPTPort::status);
    }
}

bool PlotterDriver::isXdirectionEnd(const PlotterStatus& fStatus)
{
    return fStatus.s.left_end || fStatus.s.right_end;
}

bool PlotterDriver::isYdirectionEnd(const PlotterStatus& fStatus)
{
    return fStatus.s.top_end;
}

void PlotterDriver::OnCalibratePen()
{
    mRun = true;
    enableMotors(mRun);
    PlotterStatus fStatus;
    fStatus.b.bits = 0;
    int fStep = mCalibrationDistance[xdir] > 0 ? 1 : -1;
    mPositionZ = 0;

    // go to right end to calibrate the pen
    getPlotterStatus(fStatus);
    for (; !isXdirectionEnd(fStatus) && mRun; mPositionX += fStep)
    {
        (*this.*mStepX)(mPositionX, true);
        mPort.setBits(LPTPort::data, mMotors.b);
        Sleep(mDelay_ms[horizonal_delay]);
        getPlotterStatus(fStatus);
    }
    if (fStatus.s.left_end) // if wrong end position
    {
        int fXsteps = 0;    // take the other right position
        for (; !isXdirectionEnd(fStatus) && mRun; mPositionX -= fStep, ++fXsteps)
        {
            (*this.*mStepX)(mPositionX, true);
            mPort.setBits(LPTPort::data, mMotors.b);
            Sleep(mDelay_ms[horizonal_delay]);
            getPlotterStatus(fStatus);
        }
    }
    // drive pen to lower end position
    for (; !fStatus.s.pen_down && mRun; ++mPositionZ)
    {
        (*this.*mStepZ)(mPositionZ, true);
        mPort.setBits(LPTPort::data, mMotors.b);
        Sleep(mDelay_ms[pen_delay]);
        getPlotterStatus(fStatus);

    }
    // save end position
    const FLOAT fZscale = mTransformation[zdir][zdir];
    TRACE(Logger::info,"zscale: %f, %d\n", fZscale, mPositionZ);
    int nSteps = round_fast (mPenOffsets[to_paper] * SCALE_FACTOR * fZscale);
    TRACE(Logger::info,"%d steps for %f mm \n", nSteps, mPenOffsets[to_paper]);
    mDownEndPosition = mPositionZ + nSteps;
    mPort.setPenPos(stepper::Pen_down, mDownEndPosition);
    nSteps = round_fast (mPenOffsets[to_plate] * SCALE_FACTOR * fZscale);
    TRACE(Logger::info,"%d steps for %f mm \n", nSteps, mPenOffsets[to_plate]);
    mDrillEndPosition = mPositionZ + nSteps;
    mPort.setPenPos(stepper::Pen_drill, mDrillEndPosition);
    nSteps = round_fast (mPenOffsets[to_moving_pos] * SCALE_FACTOR * fZscale);
    TRACE(Logger::info,"%d steps for %f mm \n", nSteps, mPenOffsets[to_moving_pos]);
    mUpEndPosition    = mPositionZ + nSteps;
    mPort.setPenPos(stepper::Pen_up, mUpEndPosition);
    TRACE(Logger::info,"Positions: %d, %d, %d \n", mUpEndPosition, mDownEndPosition, mDrillEndPosition);
    movePenUp();
    mRun = false;
    enableMotors(mRun);
    UpdatePlotterPosition(mUpEndPosition, mDownEndPosition, mDrillEndPosition);

}

void PlotterDriver::OnCalibratePlotter(WPARAM wParam, LPARAM lParam)
{
    int aXSteps = static_cast<int>(wParam);
    int aYSteps = static_cast<int>(lParam);
    if (mCalibrationDistance[xdir] < 0) aXSteps = -aXSteps;
    if (mCalibrationDistance[ydir] < 0) aYSteps = -aYSteps;

    mRun = true;
    enableMotors(mRun);

    movePenDown();
    lineToPos(mPositionX + aXSteps, mPositionY          );
    UpdatePlotterPosition(mPositionX, mPositionY, mPositionZ);
    lineToPos(mPositionX          , mPositionY - aYSteps);
    UpdatePlotterPosition(mPositionX, mPositionY, mPositionZ);
    lineToPos(mPositionX - aXSteps, mPositionY );
    UpdatePlotterPosition(mPositionX, mPositionY, mPositionZ);
    lineToPos(mPositionX          , mPositionY + aYSteps);
    UpdatePlotterPosition(mPositionX, mPositionY, mPositionZ);
    movePenUp();

    mRun = false;
    enableMotors(mRun);
}

void PlotterDriver::OnDrillCommands( )
{
    CPlotterDC::DrawCommandList::const_iterator it;
    PointF centerpoint;
    int fX, fY;
    int fCount = 0;
    int fIndex = 0;
    int fProgress = 0;

    for (it = mPlotCommands.begin(); it != mPlotCommands.end(); ++it)
    {
        if (it->isVisible() && it->isDrillPoint()) ++fCount;
    }

    enableMotors(true);

    mRun = true;

    movePenUp();
    for (it = mPlotCommands.begin(); it != mPlotCommands.end() && mRun; ++it, ++fIndex)
    {
        if (it->isVisible() && it->isDrillPoint())
        {
            ++fProgress;
            UpdatePlotterParam(100.0 * fProgress / fCount, CPlotterDC::Progress);
            //UpdatePlotterParam(fIndex, CPlotterDC::CmdIndex);
            centerpoint = it->getCenterPoint();
            transform_point(centerpoint.x, centerpoint.y, fX, fY);
            moveToPos(fX, fY);
            UpdatePlotterPosition(mPositionX, mPositionY, mPositionZ);
            movePenDown();
            drillDown();
            movePenUp();
        }
    }
    UpdatePlotterParam(100.0, CPlotterDC::Progress);
}


void PlotterDriver::OnPlotCommands()
{
    CPlotterDC::DrawCommandList::const_iterator it;
    int fX, fY;
    enableMotors(true);
    mRun = true;

    movePenUp();
    int fCount = mPlotCommands.size();
    int fIndex = 0;

    for (it = mPlotCommands.begin(); it != mPlotCommands.end() && mRun; ++it, ++fIndex)
    {
        UpdatePlotterParam(100.0*fIndex / fCount, CPlotterDC::Progress);
        //UpdatePlotterParam(fIndex, CPlotterDC::CmdIndex);
        if (!it->isVisible()) continue;
        switch (it->mType)
        {
        case  CPlotterDC::DrawCommand::MoveTo:
            transform_point(it->mPosition[0].x, it->mPosition[0].y, fX, fY);
            moveToPos(fX, fY);
            break;
        case  CPlotterDC::DrawCommand::LineTo: 
            transform_point(it->mPosition[0].x, it->mPosition[0].y, fX, fY);
            movePenDown();
            lineToPos(fX, fY);
            break;
        case  CPlotterDC::DrawCommand::PolyLine:
        {
            transform_point(it->mPosition[0].x, it->mPosition[0].y, fX, fY);
            moveToPos(fX, fY);
            movePenDown();
            size_t size = it->mPosition.size();
            std::vector<int>::const_iterator itmtp = it->mMoveToPoint.begin();
            for (size_t i=1; i<size; ++i)
            {
                transform_point(it->mPosition[i].x, it->mPosition[i].y, fX, fY);
                if (itmtp != it->mMoveToPoint.end() && *itmtp == static_cast<int>(i))
                {
                    moveToPos(fX, fY);
                    movePenDown();
                    ++itmtp;
                    continue;
                }
                lineToPos(fX, fY);
            }
        } break;
        case  CPlotterDC::DrawCommand::Circle:
        {
            QRect rcBound;
            transform_point(it->mPosition[0].x, it->mPosition[0].y, fX, fY);
            rcBound.setTopLeft(QPoint(fX, fY));
            transform_point(it->mPosition[1].x, it->mPosition[1].y, fX, fY);
            rcBound.setBottomRight(QPoint(fX, fY));
            QPoint ptCenter = rcBound.center();
            Circle(ptCenter.x(), ptCenter.y(), fX - ptCenter.x());
        } break;
        default:
            TRACE(Logger::info,"Unknown cmd");
            break;
        }
        UpdatePlotterPosition(mPositionX, mPositionY, mPositionZ);
    }

    movePenUp();
    it = mPlotCommands.begin();
    if (it != mPlotCommands.end())
    {
        transform_point(it->mPosition[0].x, it->mPosition[0].y, fX, fY);
        moveToPos(fX, fY);
    }
    enableMotors(false);

    UpdatePlotterParam(100.0, CPlotterDC::Progress);
}

void  PlotterDriver::transform_point(FLOAT aXin, FLOAT aYin, int &aXout, int& aYout)
{
    aXout = round_fast(aXin * mTransformation[0][0] + aYin * mTransformation[0][1] + mTransformation[0][2]);
    aYout = round_fast(aXin * mTransformation[1][0] + aYin * mTransformation[1][1] + mTransformation[1][2]);
}

void PlotterDriver::sendStep()
{
    if (not isCountingMS())
    {
        if (mStepX) (*this.*mStepX)(mPositionX, true);
        if (mStepY) (*this.*mStepY)(mPositionY, true);
        if (mStepZ) (*this.*mStepZ)(mPositionZ, true);
        mPort.setBits(LPTPort::data, mMotors.b);
    }
}

void PlotterDriver::stepBipolarFullMotor1(int aPos, bool)
{
    mMotors.m.motor1 = mBipolarFullStepControl[aPos&FullStepMask];
}

void PlotterDriver::stepBipolarFullMotor2(int aPos, bool bEnable)
{
    mMotors.m.motor2 = mBipolarFullStepControl[aPos&FullStepMask];
    mControl.m.motor1 = bEnable ? 3 : 0;
}

void PlotterDriver::stepBipolarFullMotor3(int aPos, bool)
{
    mMotors.m.motor3 = mBipolarFullStepControl[aPos&FullStepMask];
}

void PlotterDriver::stepBipolarFullMotor4(int aPos, bool)
{
    mMotors.m.motor4 = mBipolarFullStepControl[aPos&FullStepMask];
}

void PlotterDriver::stepBipolarHalfMotor1(int aPos, bool bEnable)
{
    mMotors.hsm.motor1 = mBipolarHalfStepControl[aPos&HalfStepMask];
    if (!bEnable)
    {
        mMotors.m.motor2 = 0;
        mPort.setBits(LPTPort::data, mMotors.b);
    }
}

void PlotterDriver::stepBipolarHalfMotor3(int aPos, bool bEnable)
{
    mMotors.hsm.motor2 = mBipolarHalfStepControl[aPos&HalfStepMask];
    if (!bEnable)
    {
        mMotors.m.motor4 = 0;
        mPort.setBits(LPTPort::data, mMotors.b);
    }
}

void PlotterDriver::stepUnipolarHalfMotor1(int aPos, bool bEnable)
{
    mMotors.hsm.motor1 = mUnipolarHalfStepControl[aPos&HalfStepMask];
    if (!bEnable)
    {
        mMotors.hsm.motor1 = 0;
        mPort.setBits(LPTPort::data, mMotors.b);
    }
}

void PlotterDriver::stepUnipolarHalfMotor3(int aPos, bool bEnable)
{
    mMotors.hsm.motor2 = mUnipolarHalfStepControl[aPos&HalfStepMask];
    if (!bEnable)
    {
        mMotors.hsm.motor2 = 0;
        mPort.setBits(LPTPort::data, mMotors.b);
    }
}

void PlotterDriver::stepBipolarHalfCtrlMotor1(int aPos, bool bEnable)
{
    mMotors.m.motor1  = mBipolarHalfStepControlBit[aPos&HalfStepMask];
    mControl.m.motor1 = mBipolarHalfStepControlEnable[aPos&HalfStepMask];
    if (!bEnable)
    {
        mControl.m.motor1 = 3;
    }
    mPort.setBits(LPTPort::control, mControl.b);
}

void PlotterDriver::stepBipolarHalfCtrlMotor2(int aPos, bool bEnable)
{
    mMotors.m.motor2 = mBipolarHalfStepControlBit[aPos&HalfStepMask];
    mControl.m.motor1 = mBipolarHalfStepControlEnable[aPos&HalfStepMask];
    if (!bEnable)
    {
        mControl.m.motor1 = 3;
    }
    mPort.setBits(LPTPort::control, mControl.b);
}

void PlotterDriver::stepBipolarHalfCtrlMotor3(int aPos, bool bEnable)
{
    mMotors.m.motor3 = mBipolarHalfStepControlBit[aPos&HalfStepMask];
    mControl.m.motor1 = mBipolarHalfStepControlEnable[aPos&HalfStepMask];
    if (!bEnable)
    {
        mControl.m.motor1 = 3;
    }
    mPort.setBits(LPTPort::control, mControl.b);
}

void PlotterDriver::stepBipolarHalfCtrlMotor4(int aPos, bool bEnable)
{
    mMotors.m.motor4 = mBipolarHalfStepControlBit[aPos&HalfStepMask];
    mControl.m.motor1 = mBipolarHalfStepControlEnable[aPos&HalfStepMask];
    if (!bEnable)
    {
        mControl.m.motor1 = 3;
    }
    mPort.setBits(LPTPort::control, mControl.b);
}

void PlotterDriver::movePenUp()
{
    if (mUseLptDriver)
    {
        mPort.moveToPenPos(stepper::Pen_up);
    }
    else
    {
        int fSteps = mUpEndPosition - mPositionZ;
        int fInc   = sgn(fSteps);
        fSteps     = abs(fSteps);
        Ramp ramp(mDelay_ms[pen_start_delay], mDelay_ms[pen_delay], static_cast<long>(mDelay_ms[pen_ramp_steps]), fSteps);
        for (int i=0; i<fSteps && mRun; ++i)
        {
            if (mPositionZ != mUpEndPosition) mPositionZ += fInc;
            sendStep();
            Sleep(ramp.getDelay(i));
        }
    }
}

void PlotterDriver::movePenDown()
{
    if (mUseLptDriver)
    {
        mPort.moveToPenPos(stepper::Pen_down);
    }
    else
    {
        int fSteps = mDownEndPosition - mPositionZ;
        int fInc   = sgn(fSteps);
        fSteps     = abs(fSteps);
        Ramp ramp(mDelay_ms[pen_start_delay], mDelay_ms[pen_delay], static_cast<long>(mDelay_ms[pen_ramp_steps]), fSteps);
        for (int i=0; i<fSteps && mRun; ++i)
        {
            if (mPositionZ != mDownEndPosition) mPositionZ += fInc;
            sendStep();
            Sleep(ramp.getDelay(i));
        }
    }
}

void PlotterDriver::drillDown()
{
    if (mUseLptDriver)
    {
        mPort.moveToPenPos(stepper::Pen_drill);
    }
    else
    {
        int fSteps = mDrillEndPosition - mPositionZ;
        int fInc   = sgn(fSteps);
        fSteps     = abs(fSteps);
        for (int i=0; i<fSteps && mRun; ++i)
        {
            if (mPositionZ != mDrillEndPosition) mPositionZ += fInc;
            sendStep();
            Sleep(mDelay_ms[drill_delay]);
        }
    }
}

void PlotterDriver::moveToPos(int aPosX, int aPosY)
{
    if (mUseLptDriver)
    {
        mPort.moveTo(aPosX, aPosY);
        mPositionX = mPort.getPosition(stepper::xdir);
        mPositionY = mPort.getPosition(stepper::ydir);
    }
    else
    {
        TRACE(Logger::info,"moveToPos(%d, %d)\n", aPosX, aPosY);
        int fStepsX = aPosX - mPositionX;
        int fStepsY = aPosY - mPositionY;
        int fIncX   = sgn(fStepsX);
        int fIncY   = sgn(fStepsY);
        fStepsX     = abs(fStepsX);
        fStepsY     = abs(fStepsY);
        int fSteps  = std::max(fStepsX, fStepsY);
        if (fSteps)
        {
            movePenUp();
        }
        else
        {
            TRACE(Logger::info,"moveToPos with zero offset");
        }

        Ramp ramp(mDelay_ms[horizonal_start_delay], mDelay_ms[horizonal_delay], static_cast<long>(mDelay_ms[horizonal_ramp_steps]), fSteps, Ramp::time);

        PlotterStatus fStatus;
        fStatus.b.bits = 0;
        for (int i=0; i<fSteps && mRun; ++i)
        {
            getPlotterStatus(fStatus);
            if (isXdirectionEnd(fStatus)) { mRun = false; break; }
            if (mPositionX != aPosX) mPositionX += fIncX;
            if (mPositionY != aPosY) mPositionY += fIncY;
            sendStep();
            Sleep(ramp.getDelay(i));
        }
    }
}

void PlotterDriver::lineToPos(int aPosX, int aPosY)
/*--------------------------------------------------------------
 * Bresenham-Algorithmus: Linien auf Rasterger�ten zeichnen
 *
 * Eingabeparameter:
 *    int xstart, ystart        = Koordinaten des Startpunkts
 *    int xend, yend            = Koordinaten des Endpunkts
 *
 * Ausgabe:
 *    void SetPixel(int x, int y) setze ein Pixel in der Grafik
 *         (wird in dieser oder aehnlicher Form vorausgesetzt)
 *---------------------------------------------------------------
 */
{
    if (mUseLptDriver)
    {
        mPort.lineTo(aPosX, aPosY);
        mPositionX = mPort.getPosition(stepper::xdir);
        mPositionY = mPort.getPosition(stepper::ydir);
    }
    else
    {
    //    TRACE(Logger::info,"lineToPos(%d, %d)\n", aPosX, aPosY);
        int t, dx, dy, incx, incy, pdx, pdy, ddx, ddy, es, el, err;
        dx = aPosX - mPositionX;
        dy = aPosY - mPositionY;

        incx = sgn(dx);
        incy = sgn(dy);
        if(dx<0) dx = -dx;
        if(dy<0) dy = -dy;

        if (dx>dy) // x ist schnelle Richtung
        {
            pdx=incx; pdy=0;    /* pd. ist Parallelschritt */
            ddx=incx; ddy=incy; /* dd. ist Diagonalschritt */
            es =dy;   el =dx;   /* Fehlerschritte schnell, langsam */
        }
        else // y ist schnelle Richtung
        {
            pdx=0;    pdy=incy; /* pd. ist Parallelschritt */
            ddx=incx; ddy=incy; /* dd. ist Diagonalschritt */
            es =dx;   el =dy;   /* Fehlerschritte schnell, langsam */
       }

       err = el/2;

       Ramp ramp(mDelay_ms[horizonal_start_delay], mDelay_ms[horizonal_delay],  static_cast<long>(mDelay_ms[horizonal_ramp_steps]), el, Ramp::time);

       PlotterStatus fStatus;
       fStatus.b.bits = 0;
        for(t=0; t<el && mRun; ++t) /* t zaehlt die Pixel, el ist auch Anzahl */
        {
            getPlotterStatus(fStatus);
            if (isXdirectionEnd(fStatus)) { mRun = false ; break; }
            err -= es;
            if(err<0)
            {
                err += el;
                mPositionX += ddx;
                mPositionY += ddy;
            }
            else
            {
                mPositionX += pdx;
                mPositionY += pdy;
            }
            sendStep();
            Sleep(ramp.getDelay(t));
       }
    }
} //

void PlotterDriver::Circle(int x0, int y0, int radius, int aStartAngle/* =0 */, int aStopAngle /* =360 */)
{
    int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;
    std::vector< QPoint > v11, v21, v31, v41, v12, v22, v32, v42, vall;

    v11.push_back(QPoint(x0, y0 + radius));
    v41.push_back(QPoint(x0 + radius, y0));
    v32.push_back(QPoint(x0, y0 - radius));
    v22.push_back(QPoint(x0 - radius, y0));

    while(x < y) 
    {
        if(f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x + 1;
        v11.push_back(QPoint(x0+x, y0+y));
        v12.push_back(QPoint(x0-x, y0+y));
        v31.push_back(QPoint(x0+x, y0-y));
        v32.push_back(QPoint(x0-x, y0-y));
        v21.push_back(QPoint(x0+y, y0+x));
        v22.push_back(QPoint(x0-y, y0+x));
        v41.push_back(QPoint(x0+y, y0-x));
        v42.push_back(QPoint(x0-y, y0-x));
    }

    // right turn circle begin at top edge
    //vall = v11;
    //vall.insert(vall.end(), v21.rbegin()+1, v21.rend());
    //vall.insert(vall.end(), v41.begin() , v41.end());
    //vall.insert(vall.end(), v31.rbegin()+1, v31.rend());
    //vall.insert(vall.end(), v32.begin() , v32.end());
    //vall.insert(vall.end(), v42.rbegin()+1, v42.rend());
    //vall.insert(vall.end(), v22.begin() , v22.end());
    //vall.insert(vall.end(), v12.rbegin()+1, v12.rend());

    // left turn circle begin at left edge
    vall.insert(vall.end(), v21.begin()   , v21.end());
    vall.insert(vall.end(), v11.rbegin()+1, v11.rend());
    vall.insert(vall.end(), v12.begin()   , v12.end());
    vall.insert(vall.end(), v22.rbegin()+1, v22.rend());
    vall.insert(vall.end(), v42.begin()   , v42.end());
    vall.insert(vall.end(), v32.rbegin()+1, v32.rend());
    vall.insert(vall.end(), v31.begin()   , v31.end());
    vall.insert(vall.end(), v41.rbegin()+1, v41.rend());
    size_t fEnd   = vall.size();
    if (fEnd>0)
    {
        size_t fStart = aStartAngle * fEnd / 360;
        fEnd = aStopAngle * fEnd / 360;

        movePenUp();
        moveToPos(vall[fStart].x(), vall[fStart].y());
        movePenDown();
        for (size_t i=fStart+1; i<fEnd; ++i)
        {
            mPositionX = vall[i].x();
            mPositionY = vall[i].y();
            sendStep();
            Sleep(mDelay_ms[horizonal_delay]);
        }
    }
}

void PlotterDriver::setCountMS(bool aCount)
{
    mMilliSecondCounter = aCount ? 0 : -1;
}

bool  PlotterDriver::isCountingMS()
{
    return mMilliSecondCounter != -1.0f;
}

float PlotterDriver::getDurationMS()
{
    return mMilliSecondCounter;
}


void  PlotterDriver::Sleep(float aMS)
{
    if (isCountingMS())
    {
        mMilliSecondCounter += aMS;
    }
    else
    {
        // sleep works only in debug mode with QT5
//        std::this_thread::yield();
//        std::this_thread::sleep_for (std::chrono::microseconds(static_cast<long>(aMS*1000)));
//        std::this_thread::yield();
//        struct timespec deadline;
//        deadline.tv_sec = 0;
//        deadline.tv_nsec = aMS * 1000000;
//        clock_nanosleep(CLOCK_REALTIME, 0, &deadline, NULL);
//        if (mUseLptDriver)
//        {
//          mPort.usleep(static_cast<long>(aMS*1000));
//        }
//        else
        {
          QThread::usleep(static_cast<unsigned long>(aMS*1000));
        }
    }
}

void PlotterDriver::UpdatePlotterParam(float aValue, int aID)
{
    mConnection->UpdatePlotterParam(aValue, aID);
}

void PlotterDriver::UpdatePlotterPosition(int aX, int aY, int aZ)
{
    mConnection->UpdatePlotterPosition(aX, aY, aZ);
}
