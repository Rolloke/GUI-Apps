#include "PinController.h"
#include <Arduino.h>

PinController::PinController(uint8_t repeats) :
    mRepeats(repeats)
  , mCurrentRepeat(repeats)
  , mCurrentCommand(0)
  , mLast_ms(0)
  , mCurrent_ms(0)
  , mNextTime_ms(0)
{

}

void PinController::start()
{
    mCurrent_ms = 0;
    mNextTime_ms = 0;
    mCurrentRepeat = 0;
    mCurrentCommand = 0;
}

void PinController::stop()
{
    mCurrentRepeat = mRepeats;
}

bool PinController::isPlaying()
{
    return mCurrentRepeat != mRepeats;
}

bool PinController::do_repeat()
{
    return mCurrentRepeat < mRepeats || (mRepeats == for_ever && mCurrentRepeat != for_ever);
}


DigitalPinController::DigitalPinController(uint8_t* pOutputPins, Command* pCommands, uint16_t commands, uint8_t repeats):
    PinController(repeats)
  , mpOutputPin(pOutputPins)
  , mpCommands(pCommands)
  , mCommands(commands)
  , mFlags(none)
{
    for (int i=0; mpOutputPin[i] != 0; ++i)
    {
        pinMode(mpOutputPin[i], OUTPUT);
    }
}

void DigitalPinController::tick(unsigned long fNow_ms)
{
    const unsigned long fStep_ms = fNow_ms - mLast_ms;
    mLast_ms = fNow_ms;
    mCurrent_ms += fStep_ms;
    if (mCurrent_ms > mNextTime_ms)
    {
        if (do_repeat())
        {
            if (mpCommands != 0 && mCurrentCommand >= 0 && mCurrentCommand < mCommands)
            {
                unsigned long fLength_ms = mpCommands[mCurrentCommand].mDelay;
                int i;
                for (i=0; i < 16 && mpOutputPin[i] != 0; ++i)
                {
                    digitalWrite(mpOutputPin[i], bitRead(mpCommands[mCurrentCommand].mBits,i) ? HIGH : LOW);
                }
                if (is_set(common_delay))
                {
                    for ( ; i < 32 && mpOutputPin[i] != 0; ++i)
                    {
                        digitalWrite(mpOutputPin[i], bitRead(mpCommands[mCurrentCommand].mBits,i) ? HIGH : LOW);
                    }
                    mNextTime_ms = mCurrent_ms + mDelay;
                }
                else
                {
                    mNextTime_ms = mCurrent_ms + fLength_ms;
                }
                if (is_set(reverse))
                {
                    --mCurrentCommand;
                }
                else
                {
                    ++mCurrentCommand;
                }
            }
            else
            {
                ++mCurrentRepeat;
                if (is_set(reverse))
                {
                    mCurrentCommand = mCommands - 1;
                }
                else
                {
                    mCurrentCommand = 0;
                }
            }
        }
    }
}

void DigitalPinController::setCommands(Command* aCommands, uint16_t commands)
{
    mpCommands = aCommands;
    mCommands  = commands;
}

void DigitalPinController::setFlags(uint8_t flags, bool set)
{
    if (set)
    {
        mFlags |= flags;
    }
    else
    {
        mFlags &= ~flags;
    }
}

bool DigitalPinController::is_set(uint8_t flags)
{
    return mFlags & flags ? true : false;
}

void DigitalPinController::setCommonDelay(uint16_t delay)
{
    mDelay = delay;
    setFlags(common_delay, mDelay != 0);
}


AnalogPinController::AnalogPinController(uint8_t* OutputPins, Function *pFunctions, uint8_t repeats):
    PinController(repeats)
  , mOutputPins(OutputPins)
  , mpFunctions(pFunctions)
  , mPreviousValue(0)
  , mPreviousTime_ms(0)

{
    for (int i=0; mOutputPins[i] != 0; ++i)
    {
        pinMode(mOutputPins[i], OUTPUT);
    }
}

void AnalogPinController::tick(unsigned long now_ms)
{
    const unsigned long step_ms = now_ms - mLast_ms;
    const bool          repeat  = do_repeat();
    mLast_ms     = now_ms;
    mCurrent_ms += step_ms;
    if (mCurrent_ms > mNextTime_ms && repeat)
    {
        if (mpFunctions)
        {
            mPreviousValue   = mpFunctions[mCurrentCommand].mValue;

            switch (mpFunctions[mCurrentCommand].mFunction)
            {
            case Function::set_start_value:
            case Function::constant:
            case Function::linear_ramp:
                write_to_pins(mpFunctions[mCurrentCommand].mValue);
                break;
             case Function::sine_half: case Function::none: break;
            }

            ++mCurrentCommand;
            mPreviousTime_ms = mCurrent_ms;
            mNextTime_ms     = mCurrent_ms + mpFunctions[mCurrentCommand].mPeriod;

            if (mpFunctions[mCurrentCommand].mFunction == Function::none)
            {
                if (repeat)
                {
                    ++mCurrentRepeat;
                }
                mCurrentCommand = 0;
            }
        }
    }
    else if (repeat && mCurrent_ms >= mPreviousTime_ms && mCurrent_ms < mNextTime_ms)
    {
        const uint16_t current_time = mCurrent_ms  - mPreviousTime_ms;
        const uint16_t length_ms    = mNextTime_ms - mPreviousTime_ms;
        int32_t value = 0;
        switch (mpFunctions[mCurrentCommand].mFunction)
        {
        case Function::linear_ramp:
            value = calculate_linear_ramp(current_time, length_ms);
            break;
        case Function::constant:
        case Function::set_start_value:
            return;
        case Function::sine_half:
            value = sin(M_PI * current_time / length_ms) * mpFunctions[mCurrentCommand].mValue;
            break;
        case Function::none: break;
        }
        write_to_pins(value);
    }
}

void AnalogPinController::write_to_pins(uint16_t value)
{
    for (int i=0; mOutputPins[i] != 0; ++i)
    {
        analogWrite(mOutputPins[i], value);
    }
}

int32_t AnalogPinController::calculate_linear_ramp(uint16_t current_time, uint16_t length_ms)
{
    int32_t value = static_cast<int32_t>(mpFunctions[mCurrentCommand].mValue - mPreviousValue);
    if (value > 0)
    {
        value = value * current_time / length_ms;
    }
    else
    {
        value = -value * (length_ms - current_time) / length_ms;
    }
    return value;
}

void AnalogPinController::setFunctions(Function* aFunctions)
{
    mpFunctions = aFunctions;
}

