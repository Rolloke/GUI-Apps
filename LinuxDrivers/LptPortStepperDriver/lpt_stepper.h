#ifndef _LPT_STEPPER_H_
#define _LPT_STEPPER_H_

#include <linux/ioctl.h>

#ifndef __KERNEL__
namespace stepper {
#endif

enum ePort      { data_port, status_port, control_port, ports } ;
enum ePenPos    { Pen_up, Pen_down, Pen_drill, Pen_positions } ;
enum eDirection { xdir, ydir, zdir, dira, dirb, dirc, directions, invalid_dir } ;
enum eStates    { left_end, right_end, top_end, bottom_end, pen_down, stop_all, states, no_state } ;
enum eMotors    { BipolarFull, BipolarHalf, UnipolarFull, UnipolarHalf, TestPattern, motors, Invalid } ;
enum eDelayType { delay, start_delay, ramp_steps, delays } ;
enum eStatusBits  { Error=4, Select=5, PaperEnd=6, Acknowledge=7, Busy=8 };
enum eControlBits { DataStrobe=9, LineFeed=10, InitPrinter=11, SelectInput=12, EnableInterrupt=13 };

#ifndef __KERNEL__
};
#endif

 /*
  * Ioctl definitions
  */
 
 /* Use 'k' as magic number */
 #define LPT_STEPPER_IOC_MAGIC  'L'
 /* Please use a different 8-bit number in your code */
 
 #define LPT_STEPPER_IOCRESET    _IO(LPT_STEPPER_IOC_MAGIC, 0)
 
 /*!
  * S means "Set" through a ptr,
  * T means "Tell" directly with the argument value
  * G means "Get": reply by setting through a pointer
  * Q means "Query": response is on the return value
  * X means "eXchange": switch G and S atomically
  * H means "sHift": switch T and Q atomically
  */
  
/*! @brief: modifies the base address of the port
 *  @param arg new address value (unsigned long)
 *  @return zero at success */
#define LPT_STEPPER_IOCT_BASE_ADDRESS _IO(LPT_STEPPER_IOC_MAGIC,  1)

/*! @brief: sets the value oft the LPT port
 *  @param arg value and port number coded in 32 bits (unsigned long))
 *  lower  8 bits: value       (unsigned char)
 *  upper 16 bits: port number (ePort)
 *  @return zero at success */
#define LPT_STEPPER_IOCT_VALUE _IO(LPT_STEPPER_IOC_MAGIC,  2)

/*! @brief: retrieves the value oft the LPT port
 *  @param arg port number (ePort) 
 *  @return value of LPT port */
#define LPT_STEPPER_IOCQ_VALUE _IO(LPT_STEPPER_IOC_MAGIC,  3)

/*! @brief: retrieves the base address of the port
 *  @param arg unsused (unsigned long)
 *  @return base address of LPT port */
#define LPT_STEPPER_IOCQ_BASE_ADDRESS _IO(LPT_STEPPER_IOC_MAGIC,  4)

/*! @brief: calls the function move to with x and y aruments
 *  @param arg arrray with 2 values: new x and y position (int *)
 *  @return zero at success */
#define LPT_STEPPER_IOCS_MOVE_TO _IOW(LPT_STEPPER_IOC_MAGIC,  5, long)

/*! @brief: calls the function line to with x and y aruments
 *  @param arg arrray with 2 values: new x and y position (int *)
 *  @return zero at success */
#define LPT_STEPPER_IOCS_LINE_TO _IOW(LPT_STEPPER_IOC_MAGIC,  6, long)

/*! @brief: calls the function line to 3D with x, y and z aruments
 *  @param arg arrray with 3 values: new x, y and z position (int *)
 *  @return zero at success */
#define LPT_STEPPER_IOCS_LINE_TO_3D _IOW(LPT_STEPPER_IOC_MAGIC,  7, long)

/*! @brief: move to the predefined pen position
 *  @param arg predefined pen position  (ePenPos)
 *  @return zero at success */
#define LPT_STEPPER_IOCT_MOVE_TO_PEN_POS _IO(LPT_STEPPER_IOC_MAGIC,  8)

/*! @brief: enables or disables all motors
 *  @param arg 0: disable, 1: enable motors  (unsigned long)
 *  @return zero at success */
#define LPT_STEPPER_IOCT_ENABLE_MOTORS _IO(LPT_STEPPER_IOC_MAGIC,  9)

/*! @brief: sets the predefined pen position
 *  @param arg arrray with 2 values: 
 *  desired pen position        (ePenPos)
 *  z position of pen position  (int)
 *  @return zero at success */
#define LPT_STEPPER_IOCS_PEN_POS _IOW(LPT_STEPPER_IOC_MAGIC,  10, long)

/*! @brief: sets the stepper motor type
 *  @param arg arrray with 2 values:
 *  desired direction of motor  (eDirection)
 *  motor type                  (eMotors)
 *  @return zero at success */
#define LPT_STEPPER_IOCS_MOTORS _IOW(LPT_STEPPER_IOC_MAGIC,  11, unsigned long)

/*! @brief: sets the stepper motor bits for the desired output pin
 *  @param arg arrray with 5 values:
 *  desired direction of motor  (eDirection)
 *  bit0 of output pin (1-12)   (int)
 *  bit1 of output pin (1-12)   (int)
 *  bit2 of output pin (1-12)   (int)
 *  bit3 of output pin (1-12)   (int)
 *  @hint a negative pin value inverts the polarity
 *  @return zero at success */
#define LPT_STEPPER_IOCS_MOTOR_BITS _IOW(LPT_STEPPER_IOC_MAGIC,  12, long)

/*! @brief: sets the status bit for the desired input pin
 *  @param arg arrray with 2 values:
 *  desired state identifier    (eStates)
 *  bit of status pin (3-7)     (int)
 *  @hint a negative pin value inverts the polarity
 *  @return zero at success */
#define LPT_STEPPER_IOCS_STATUS_BIT _IOW(LPT_STEPPER_IOC_MAGIC,  13, long)

/*! @brief: sets the stepper motor position value without stepping
 *  @param arg arrray with 2 values:
 *  desired direction of position       (eDirection)
 *  position value for this direction   (int)
 *  @return zero at success */
#define LPT_STEPPER_IOCS_POSITION _IOW(LPT_STEPPER_IOC_MAGIC,  14, long)

/*! @brief: retrieves the bit value desired status identifier
 *  @param arg desired state identifier  (eStates)
 *  @return bit value of state identifier */
#define LPT_STEPPER_IOCQ_STATUS_BIT _IO(LPT_STEPPER_IOC_MAGIC,  15)

/*! @brief: retrieves the stepper position of the motor direction
 *  @param arg desired Direction (eDirection) (unsigned long)
 *  @return Position of the motor direction */
#define LPT_STEPPER_IOCQ_POSITION _IO(LPT_STEPPER_IOC_MAGIC,  16)

/*! @brief: sets the bit to enable or disable all motors
 *  @param arg number of bit (1 - 12)  (unsigned long)
 *  @hint a negative pin value inverts the polarity
 *  @return zero at success */
#define LPT_STEPPER_IOCT_ENABLE_MOTORS_BIT _IO(LPT_STEPPER_IOC_MAGIC,  17)

/*! @brief: sets the stepper motor delay for according direction
 *  @param arg arrray with 3 values:
 *  desired direction of motor           (eDirection)
 *  delay for this motor in microseconds (long)
 *  @return zero at success */
#define LPT_STEPPER_IOCS_DELAY _IOW(LPT_STEPPER_IOC_MAGIC,  18, long)

/*! @brief: registers an interrupt to pin 10 of the LPT port
 *  @param arg desired state identifier    (eStates)
 *  @return zero at success */
#define LPT_STEPPER_IOCT_REGISTER_INTERRUPT_TO_STATE _IO(LPT_STEPPER_IOC_MAGIC,  19)

/*! @brief: sets the stepper motor start delay for according direction
 *  @param arg arrray with 3 values:
 *  desired direction of motor                 (eDirection)
 *  start delay for this motor in microseconds (long)
 *  @return zero at success */
#define LPT_STEPPER_IOCS_START_DELAY _IOW(LPT_STEPPER_IOC_MAGIC, 20, long)

/*! @brief: sets the stepper motor ramps steps for according direction
 *  @param arg arrray with 3 values:
 *  desired direction of motor                       (eDirection)
 *  number of ramp steps for this motor acceleration (long)
 *  @return zero at success */
#define LPT_STEPPER_IOCS_RAMP_STEPS _IOW(LPT_STEPPER_IOC_MAGIC,  21, long)

/*! @brief: move the stepper motor to the position value
 *  @param arg arrray with 2 values:
 *  desired direction of position       (eDirection)
 *  position value for this direction   (int)
 *  @return zero at success */
#define LPT_STEPPER_IOCS_MOVE_TO_POSITION _IOW(LPT_STEPPER_IOC_MAGIC,  22, long)

/*! @brief: set output bit of LPT port
 *  @param arg arrray with 2 values:
 *  desired direction of position       (eControlBits)
 *  value true, false (0, 1)            (int)
 *  @return zero at success */
#define LPT_STEPPER_IOCS_LPT_CONTROL_BIT _IOW(LPT_STEPPER_IOC_MAGIC,  23, long)

/*! @brief: retrieves the Bit value oft the LPT Status port
 *  @param arg bit number (eStatusBits) 
 *  @return value of LPT Status port bit */
#define LPT_STEPPER_IOCQ_LPT_STATUS_BIT _IO(LPT_STEPPER_IOC_MAGIC,  24)

/*! @brief: calls the function move to relative with x and y aruments
 *  @param arg arrray with 2 values: new x and y position (int *)
 *  @return zero at success */
#define LPT_STEPPER_IOCS_MOVE_RELATIVE _IOW(LPT_STEPPER_IOC_MAGIC,  25, long)

/*! @brief: calls the function line to relative with x and y aruments
 *  @param arg arrray with 2 values: new x and y position (int *)
 *  @return zero at success */
#define LPT_STEPPER_IOCS_LINE_RELATIVE _IOW(LPT_STEPPER_IOC_MAGIC,  26, long)

/*! @brief: set asynchron delegation of motor control functions to a work queue
 *  @param arg 0: disable, 1: enable asynchron delegation (unsigned long)
 *             2: enables asynchron delegation but waits in a semaphore
 *  @return zero at success */
#define LPT_STEPPER_IOCT_SET_ASYNC _IO(LPT_STEPPER_IOC_MAGIC,  27)

/*! @brief: Wait useconds
 *  @param arg 0: microseconds to wait
 *  @return zero at success */
#define LPT_STEPPER_IOCT_MICRO_DELAY _IOW(LPT_STEPPER_IOC_MAGIC, 28, long)


 /*
 #define LPT_STEPPER_IOCSQSET    _IOW(LPT_STEPPER_IOC_MAGIC,  2, int)
 #define LPT_STEPPER_IOCTQUANTUM _IO(LPT_STEPPER_IOC_MAGIC,   3)
 #define LPT_STEPPER_IOCGQUANTUM _IOR(LPT_STEPPER_IOC_MAGIC,  5, int)
 #define LPT_STEPPER_IOCHQSET    _IO(LPT_STEPPER_IOC_MAGIC,  12)
 */
 
 /*
  * The other entities only have "Tell" and "Query", because they're
  * not printed in the book, and there's no need to have all six.
  * (The previous stuff was only there to show different ways to do it.
  */
 /* ... more to come */
 
 #define LPT_STEPPER_IOC_MAXNR 24

#endif // _LPT_STEPPER_H_
