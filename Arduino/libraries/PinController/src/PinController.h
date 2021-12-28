#ifndef PinController_h
#define PinController_h

/*!
 * PinController.h - library for switching pins
 * The library switches pins on and off asynchronously controlled by
 * repeatedly calling the tick(..) function within the function loop().
 */
#include <inttypes.h>

#define PINS02(                                                               p2, p1)                                                                                                  p2<<1|p1
#define PINS03(                                                           p3, p2, p1)                                                                                            p3<<2|p2<<1|p1
#define PINS04(                                                       p4, p3, p2, p1)                                                                                      p4<<3|p3<<2|p2<<1|p1
#define PINS05(                                                   p5, p4, p3, p2, p1)                                                                                p5<<4|p4<<3|p3<<2|p2<<1|p1
#define PINS06(                                               p6, p5, p4, p3, p2, p1)                                                                          p6<<5|p5<<4|p4<<3|p3<<2|p2<<1|p1
#define PINS07(                                           p7, p6, p5, p4, p3, p2, p1)                                                                    p7<<6|p6<<5|p5<<4|p4<<3|p3<<2|p2<<1|p1
#define PINS08(                                       p8, p7, p6, p5, p4, p3, p2, p1)                                                              p8<<7|p7<<6|p6<<5|p5<<4|p4<<3|p3<<2|p2<<1|p1
#define PINS09(                                   p9, p8, p7, p6, p5, p4, p3, p2, p1)                                                        p9<<8|p8<<7|p7<<6|p6<<5|p5<<4|p4<<3|p3<<2|p2<<1|p1
#define PINS10(                              p10, p9, p8, p7, p6, p5, p4, p3, p2, p1)                                                 p10<<9|p9<<8|p8<<7|p7<<6|p6<<5|p5<<4|p4<<3|p3<<2|p2<<1|p1
#define PINS11(                         p11, p10, p9, p8, p7, p6, p5, p4, p3, p2, p1)                                         p11<<10|p10<<9|p9<<8|p8<<7|p7<<6|p6<<5|p5<<4|p4<<3|p3<<2|p2<<1|p1
#define PINS12(                    p12, p11, p10, p9, p8, p7, p6, p5, p4, p3, p2, p1)                                 p12<<11|p11<<10|p10<<9|p9<<8|p8<<7|p7<<6|p6<<5|p5<<4|p4<<3|p3<<2|p2<<1|p1
#define PINS13(               p13, p12, p11, p10, p9, p8, p7, p6, p5, p4, p3, p2, p1)                         p13<<12|p12<<11|p11<<10|p10<<9|p9<<8|p8<<7|p7<<6|p6<<5|p5<<4|p4<<3|p3<<2|p2<<1|p1
#define PINS14(          p14, p13, p12, p11, p10, p9, p8, p7, p6, p5, p4, p3, p2, p1)                 p14<<13|p13<<12|p12<<11|p11<<10|p10<<9|p9<<8|p8<<7|p7<<6|p6<<5|p5<<4|p4<<3|p3<<2|p2<<1|p1
#define PINS15(     p15, p14, p13, p12, p11, p10, p9, p8, p7, p6, p5, p4, p3, p2, p1)         p15<<14|p14<<13|p13<<12|p12<<11|p11<<10|p10<<9|p9<<8|p8<<7|p7<<6|p6<<5|p5<<4|p4<<3|p3<<2|p2<<1|p1
#define PINS16(p16, p15, p14, p13, p12, p11, p10, p9, p8, p7, p6, p5, p4, p3, p2, p1) p16<<15|p15<<14|p14<<13|p13<<12|p12<<11|p11<<10|p10<<9|p9<<8|p8<<7|p7<<6|p6<<5|p5<<4|p4<<3|p3<<2|p2<<1|p1

typedef struct 
{
    uint16_t mBits;
    uint16_t mDelay;
} Command;

class PinController
{
public:
    enum e { for_ever = 255 };
    //! @brief starts playing the melody
    virtual void start();

    //! @brief stops playing the melody
    void stop();

    //! @brief determines, if melody playing is active
    bool isPlaying();

protected:
    PinController(uint8_t repeats);
    bool do_repeat();

    uint8_t  mRepeats;
    uint8_t  mCurrentRepeat;
    uint16_t mCurrentCommand;
    unsigned long mLast_ms;
    unsigned long mCurrent_ms;
    unsigned long mNextTime_ms;
};

class DigitalPinController : public PinController
{
public:
    //! @brief constructor with parameters
    //! @param outputPin No
    //! @param pCommands array with command bits zero terminated, see structure tones
    //! @note the last entry must contain zeros
    //! @param repeats of control commands, zero means forever
    DigitalPinController( uint8_t* pOutputPin, Command* pCommands, uint8_t repeats=for_ever);

    //! @brief controlles events to switch pins on and off
    //! @param fNow_ms current time determined by millis()
    //! @note must be called repeatedly within loop()
    void tick(unsigned long fNow_ms);

    //! @brief sets a new array with commands
    //! @param pCommands array with commands zero terminated, see structure Commands
    //! @note the last entry must contain zeros
    void setCommands(Command* aCommands);

private:
    DigitalPinController();

    uint8_t* mpOutputPin;
    Command* mpCommands;
};


struct Function
{
    enum eFunction
    {
        none,
        linear_ramp,
        constant,
        sine_half
    };
    eFunction mFunction;
    uint16_t  mValue;
    uint16_t  mPeriod;
};

class AnalogPinController: public PinController
{
public:
    enum e {start_value = 0x8000};
    //! @brief constructor with parameters
    //! @param outputPin No
    //! @param pCommands array with functions zero terminated, see structure Functions
    //! @note the last entry must contain zeros
    //! @param repeats of control commands, zero means forever
    AnalogPinController( uint8_t* OutputPins, Function* pFunctions, uint8_t repeats=for_ever);

    //! @brief controles events to play the melody
    //! @param fNow_ms current time determined by millis()
    //! @note must be called repeatedly within loop()
    void tick(unsigned long fNow_ms);

    //! @brief sets a new array with tones
    //! @param pCommands array with tones zero terminated, see structure tones
    //! @note the last entry must contain zeros
    void setFunctions(Function* aFunctions);

    void start() override;

private:
    AnalogPinController();
    int32_t calculate_linear_ramp(uint16_t current_time, uint16_t length_ms);
    void    write_to_pins(uint16_t value);

    uint8_t*  mOutputPins;
    Function* mpFunctions;
    uint16_t  mPreviousValue;
    unsigned long mPreviousTime_ms;
};

#endif
