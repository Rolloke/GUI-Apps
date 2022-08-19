
#ifndef _STEPPER_CONTROL_H_
#define _STEPPER_CONTROL_H_

#include "lpt_stepper.h"

#include <linux/cdev.h>
#include <linux/interrupt.h>

typedef unsigned char BYTE;
enum eBits       { bit0, bit1, bit2, bit3, bits };

struct delegated_data;

// LPT Stepper Driver structure
typedef struct lpt_stepper_dev
{
  unsigned long base_address;       // base address for LPT port access
  int  device_number;               // number of he device
  long position[directions];        // positions of stepper motors
  enum eMotors motor[directions];   // motor numbers
  long motor_bits[directions][bits];// motor bits (1 - 12)
  long enable_motors;               // enable_motors (1 - 12)
  long status_bits[states];         // status bits  (3 - 7)
  long pen_position[Pen_positions]; // predefined positions of the pen
  BYTE data_control[2];             // storage for data and control bits
  volatile int  run;                // run loop
  unsigned long delay[directions];  // delays for stepper
  unsigned long start_delay[directions];// delays for stepper
  unsigned long ramp_steps[directions];// ramp_steps
  unsigned int irq_nr;              // interrupt request number
  unsigned long use_work_queue;     // flag for delegating work over workqueue
  struct cdev cdev;                 // character device structure
} lpt_stepper_dev_t;


const char* getHelpText( void );
const char* getNameOfMotor(enum eMotors);
const char* getNameOfStatus(enum eStates aS);

void stepper_move_to(lpt_stepper_dev_t*pdev, long x, long y);
void stepper_line_to(lpt_stepper_dev_t*pdev, long x, long y);
void stepper_move_relative(lpt_stepper_dev_t*pdev, long x, long y);
void stepper_line_relative(lpt_stepper_dev_t*pdev, long x, long y);
void enableMotors(lpt_stepper_dev_t*pdev, long aEnable, long dummy);
void movePen(lpt_stepper_dev_t*pdev, long pen_pos, long dummy);
void setOutputBit(lpt_stepper_dev_t*pdev, long number, long set);
void moveMotor(lpt_stepper_dev_t*pdev, long direction, long position);

void stepper_line_to_3d(lpt_stepper_dev_t*pdev, long x1, long y1, long z1);
unsigned char getStatus(lpt_stepper_dev_t*pdev);
void setOutput(lpt_stepper_dev_t*pdev);
int  hasRechedBoundary(lpt_stepper_dev_t*pdev, unsigned char status);
void sendStep(lpt_stepper_dev_t*pdev);
int  testBit(long bitnumber, unsigned char testbits);

irqreturn_t handleLptInterrupt(int irq, void *data);

typedef void (*stepper_fnct_ptr)(lpt_stepper_dev_t*, long, long);
typedef BYTE (*motor_fnct_ptr)(int);

struct delegated_data
{
    struct list_head list;
    stepper_fnct_ptr pfnct;
    lpt_stepper_dev_t*pdev;
    long p1;
    long p2;
};
#endif // _STEPPER_CONTROL_H_
