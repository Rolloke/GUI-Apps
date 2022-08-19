
#include "stepper_control.h"

#include <linux/ioport.h>
#include <asm/io.h>             
#include <linux/delay.h>

struct s_ramp_delay
{
    long up_index;
    long down_index;
    long value;
    long step;
};

void init_ramp_delay(struct s_ramp_delay* pramp, unsigned long start, unsigned long stop, unsigned long steps, unsigned long loop);
unsigned long get_ramp_delay(struct s_ramp_delay* pramp, int i);

const char* getHelpText( void )
{
    static const char *szHelpText = 
    "\n Use \"set parameter: x\" to assign a parameter\n"
    " e.g.: set pen positions up: 123\n"
    "\n Help for bits to pin assignement:\n"
    "------------------------------------\n"
    " data bits and according pins\n"
    " bit   1   2   3   4   5   6   7   8\n"
    " pin   2   3   4   5   6   7   8   9\n"
    " dir out out out out out out out out\n\n"
    "------------------------------------\n"
    " control bits and according pins\n"
    " bit   9  10  11  12  13  14  15  16\n"
    " pin   1  14  16  17  nc  nc  nc  nc\n"
    " dir out out out out  ie\n"
    " inv   X   X       X  \n\n"
    "------------------------------------\n"
    " status bits and according pins\n"
    " bit   1   2   3   4   5   6   7   8\n"
    " pin  nc  nc  nc  15  13  12  10  11\n"
    " dir              in  in  in  in  in\n"
    " inv                               X\n";
    return szHelpText;
}


BYTE getMotorBipolarFullStepControl(int index)
{
    // Fullstep matrix for bipolar drive
    // step 1 2 3 4
    // bit0 0 0 1 1
    // bit1 0 1 1 0
    // hex  0 1 3 2
    static const BYTE fBipolarFullStepControl[4] = { 0x0, 0x1, 0x3, 0x2 };
    
    return fBipolarFullStepControl[index & 3];
}

BYTE getMotorBipolarHalfStepControlEnableAndBits(int index)
{
    // Halfstep matrix for bipolar drive
    // step    1 2 3 4 5 6 7 8
    // enable0 0 0 0 1 0 0 0 1
    // enable1 0 1 0 0 0 1 0 0
    // bit0    0 0 1 1 1 1 0 0
    // bit1    0 0 0 0 1 1 1 1
    // hex     c 8 e 6 f b d 5

    // enable and bits
    static const BYTE fBipolarHalfStepControlEnableAndBits[8] = { 0x0, 0x4, 0x2, 0xa, 0x3, 0x7, 0x1, 0x9 }; 
    
    // enable
    // enable0 1 1 1 0 1 1 1 0
    // enable1 1 0 1 1 1 0 1 1
    // static const BYTE fBipolarHalfStepControlEnableAndBits[8] = { 0xc, 0x8, 0xe, 0x6, 0xf, 0xb, 0xd, 0x5 }; 
    // static const BYTE gBipolarHalfStepControlEnable[8]        = { 0x3, 0x1, 0x3, 0x2, 0x3, 0x1, 0x3, 0x2 }; 
    // !enable
    // static const BYTE gBipolarHalfStepControlNotEnable[8]     = { 0x0, 0x2, 0x0, 0x1, 0x0, 0x2, 0x0, 0x1 }; 
    // and bits
    // static const BYTE gBipolarHalfStepControlBit[8]           = { 0x0, 0x0, 0x1, 0x1, 0x3, 0x3, 0x2, 0x2 }; 
    
    return fBipolarHalfStepControlEnableAndBits[index & 7];
}

BYTE getMotorUnipolarFullStepControl(int index)
{
    // Fullstep matrix for unipolar drive
    // step    2 4 6 8
    // bit1    1 0 0 1
    // bit2    0 1 1 0
    // bit3    0 0 1 1
    // bit4    1 1 0 0
    // hex     9 a 6 5
    static const BYTE fUnipolarFullStepControl[4] = { 0x9, 0xa, 0x6, 0x5 }; 
    
    return fUnipolarFullStepControl[index & 3];
}

BYTE getMotorUnipolarHalfStepControl(int index)
{
    // Halfstep matrix for unipolar drive
    // step    1 2 3 4 5 6 7 8
    // bit1    1 1 0 0 0 0 0 1
    // bit2    0 0 0 1 1 1 0 0
    // bit3    0 0 0 0 0 1 1 1
    // bit4    0 1 1 1 0 0 0 0
    // hex     1 9 8 a 2 6 4 5
    static const BYTE gUnipolarHalfStepControl[8] = { 0x1, 0x9, 0x8, 0xa, 0x2, 0x6, 0x4, 0x5 }; 
    
    return gUnipolarHalfStepControl[index & 7];
}

BYTE  getTestPattern(int index)
{
    if (index == 0) return 0xf;
    else            return 0;
}

motor_fnct_ptr gMotors[motors] =
{
    getMotorBipolarFullStepControl,
    getMotorBipolarHalfStepControlEnableAndBits,
    getMotorUnipolarFullStepControl,
    getMotorUnipolarHalfStepControl,
    getTestPattern
};

const char* getNameOfMotor(enum eMotors aM)
{
    static const char* names[motors] = 
    {
        "BipolarFull", "BipolarHalf", "UnipolarFull", "UnipolarHalf", "TestPattern"
    };
    if (aM >= BipolarFull && aM < motors)
    {
        return names[aM];
    }
    return "Unknown";
}

const char* getNameOfStatus(enum eStates aS)
{
    static const char* names[states] = 
    {
        "left end", "right end", "top end", "bottom end", "pen down"
    };
    if (aS >= left_end && aS < states)
    {
        return names[aS];
    }
    return "Unknown";
}

int sgn(int x)
{
  return (x > 0) ? 1 : (x < 0) ? -1 : 0;
}

unsigned char getStatus(lpt_stepper_dev_t*pdev)
{
    return inb(pdev->base_address+status_port);
}

void setOutput(lpt_stepper_dev_t*pdev)
{
	outb(pdev->data_control[0], pdev->base_address+data_port);
	outb(pdev->data_control[1], pdev->base_address+control_port);
}

int testBit(long bitnumber, unsigned char testbits)
{
    if (bitnumber > 0)
    {
        return (testbits & (1 << (bitnumber-1))) != 0;
    }
    else if (bitnumber < 0) // inverted
    {
        return (testbits & 1 << (-bitnumber - 1)) == 0;
    }
    return 0;
}

void setBit(long bitnumber, long set, unsigned short* outBits)
{
    if (bitnumber > 0)
    {
        if (set) (*outBits) |=  (1 << (bitnumber-1));
        else     (*outBits) &= ~(1 << (bitnumber-1));
    }
    else if (bitnumber < 0) // inverted
    {
        if (set) (*outBits) &= ~(1 << (-bitnumber-1));
        else     (*outBits) |=  (1 << (-bitnumber-1));
    }
}

int hasReachedBoundary(lpt_stepper_dev_t*pdev, unsigned char status)
{
    return testBit(pdev->status_bits[  left_end], status)
        && testBit(pdev->status_bits[ right_end], status)
        && testBit(pdev->status_bits[   top_end], status)
        && testBit(pdev->status_bits[bottom_end], status)
        && testBit(pdev->status_bits[pen_down  ], status);
}

void enableMotor(enum eMotors motor, long* motor_bits, int enable, unsigned short *outBits)
{
    if (motor == BipolarFull)
    {
        setBit(motor_bits[bit2], enable, outBits);
        setBit(motor_bits[bit3], enable, outBits);
    }
    else if (enable == 0)
    {
        if (motor == BipolarHalf)
        {
            setBit(motor_bits[bit2], 0, outBits);
            setBit(motor_bits[bit3], 0, outBits);
        }
        else if (motor == UnipolarHalf)
        {
            setBit(motor_bits[bit0], 0, outBits);
            setBit(motor_bits[bit1], 0, outBits);
            setBit(motor_bits[bit2], 0, outBits);
            setBit(motor_bits[bit3], 0, outBits);
        }
    }
}

void setOutputBit(lpt_stepper_dev_t*pdev, long number, long set)
{
	setBit(number, set, (unsigned short*)&pdev->data_control[0]);
    setOutput(pdev);
}

irqreturn_t handleLptInterrupt(int irq, void *data)
{
    lpt_stepper_dev_t*pdev = (lpt_stepper_dev_t*)data;
    int i;
    for (i=0; i<states; ++i)
    {
        if (pdev->status_bits[i] == 10)
        {
            if (i == stop_all)
            {
                enableMotors(pdev, 0, 0);
            }
            break;
        }
    }
    return IRQ_HANDLED;
}

void enableMotors(lpt_stepper_dev_t*pdev, long enable, long dummy)
{
    setBit(pdev->enable_motors, enable, (unsigned short*)&pdev->data_control[0]);
    if (enable)
    {
        sendStep(pdev);
    }
    enableMotor(pdev->motor[xdir], pdev->motor_bits[xdir], enable, (unsigned short*)&pdev->data_control[0]);
    enableMotor(pdev->motor[ydir], pdev->motor_bits[ydir], enable, (unsigned short*)&pdev->data_control[0]);
    enableMotor(pdev->motor[zdir], pdev->motor_bits[zdir], enable, (unsigned short*)&pdev->data_control[0]);

    pdev->run = enable;
    setOutput(pdev);
}

void movePen(lpt_stepper_dev_t*pdev, long pos, long dummy)
{
    if (pos >= Pen_up && pos <= Pen_drill)
    {
        int i, fSteps = pdev->pen_position[pos] - pdev->position[zdir];
        int fInc   = sgn(fSteps);
		struct s_ramp_delay ramp = { 0 };
		
        fSteps     = abs(fSteps);
        init_ramp_delay(&ramp, pdev->start_delay[zdir], pdev->delay[zdir], pdev->ramp_steps[zdir], fSteps);
        
        for (i=0; i<fSteps && pdev->run; ++i)
        {
            if (pdev->position[zdir] != pdev->pen_position[pos]) pdev->position[zdir] += fInc;
            sendStep(pdev);
            udelay(get_ramp_delay(&ramp, i));
        }
    }
}

void moveMotor(lpt_stepper_dev_t*pdev, long direction, long position)
{
    int i, fSteps = position - pdev->position[direction];
    int fInc   = sgn(fSteps);
    struct s_ramp_delay ramp = { 0 };
    
    fSteps     = abs(fSteps);
    init_ramp_delay(&ramp, pdev->start_delay[direction], pdev->delay[direction], pdev->ramp_steps[direction], fSteps);
    
    for (i=0; i<fSteps && pdev->run; ++i)
    {
        if (pdev->position[direction] != position) pdev->position[direction] += fInc;
        sendStep(pdev);
        udelay(get_ramp_delay(&ramp, i));
    }
}

void sendStep(lpt_stepper_dev_t*pdev)
{
    int dir, bit;
    unsigned short testbit;
    BYTE motorbits;
    for (dir=0; dir<directions; ++dir)
    {
        if (pdev->motor[dir] != Invalid)
        {
            motorbits = gMotors[pdev->motor[dir]](pdev->position[dir]);
            for (bit=0, testbit=1; bit<bits; ++bit, testbit<<=1)
            {
                setBit(pdev->motor_bits[dir][bit], (motorbits & testbit) != 0, (unsigned short*)&pdev->data_control[0]);
            }
        }
    }
    setOutput(pdev);
}

void init_ramp_delay(struct s_ramp_delay* pramp, unsigned long start, unsigned long stop, unsigned long steps, unsigned long loop)
{
	if (start > stop && steps *2 < loop)
	{
		pramp->value      = start;
		pramp->step       = (start - stop) / steps;
		pramp->up_index   = steps-1;
		pramp->down_index = loop-steps;
	}
	else if (start > 0)
	{
		pramp->value = start;
	}
	else 
	{
		pramp->value = stop;
	}
}

unsigned long get_ramp_delay(struct s_ramp_delay* pramp, int i)
{
    if (pramp->step > 0)
    {
        pramp->value -= pramp->step;
        if (i == pramp->up_index) pramp->step = -pramp->step;
    }
    else if (pramp->step < 0 && i >= pramp->down_index)
    {
        pramp->value -= pramp->step;
    }
    return pramp->value;
}

void stepper_move_relative(lpt_stepper_dev_t*pdev, long x, long y)
{
    stepper_move_to(pdev, pdev->position[xdir] + x, pdev->position[ydir]+y);
}

void stepper_move_to(lpt_stepper_dev_t*pdev, long x, long y)
{
    int fSteps, i;
    int fStepsX = x - pdev->position[xdir];
    int fStepsY = y - pdev->position[ydir];
    int fIncX   = sgn(fStepsX);
    int fIncY   = sgn(fStepsY);
    struct s_ramp_delay ramp = { 0 };
    fStepsX     = abs(fStepsX);
    fStepsY     = abs(fStepsY);
    fSteps      = max(fStepsX, fStepsY);
  
    // todo! implement y_delay, use the higher value of both directions
    init_ramp_delay(&ramp, pdev->start_delay[xdir], pdev->delay[xdir], pdev->ramp_steps[xdir], fSteps);
    
    for (i=0; i<fSteps && pdev->run; ++i)
    {
        if (hasReachedBoundary(pdev, getStatus(pdev))) { pdev->run = 0; break; }
        if (pdev->position[xdir] != x) pdev->position[xdir] += fIncX;
        if (pdev->position[ydir] != y) pdev->position[ydir] += fIncY;
        sendStep(pdev);
        udelay(get_ramp_delay(&ramp, i));
    }
}

void stepper_line_relative(lpt_stepper_dev_t*pdev, long x, long y)
{
    stepper_line_to(pdev, pdev->position[xdir] + x, pdev->position[ydir]+y);
}

void stepper_line_to(lpt_stepper_dev_t*pdev, long x, long y)
{
    int t, dx, dy, incx, incy, pdx, pdy, ddx, ddy, es, el, err;
    struct s_ramp_delay ramp = { 0 };
    
    dx = x - pdev->position[xdir];
    dy = y - pdev->position[ydir];
 
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
    // todo! implement y_delay, calculate the value depending on the direction
    // and the gradient
	init_ramp_delay(&ramp, pdev->start_delay[xdir], pdev->delay[xdir],
       pdev->ramp_steps[xdir], el);
 
    err = el/2;

    for(t=0; t<el && pdev->run; ++t) 
    {
        if (hasReachedBoundary(pdev, getStatus(pdev))) { pdev->run = 0; break; }

        err -= es;
        if(err<0)
        {
            err += el;
            pdev->position[xdir] += ddx;
            pdev->position[ydir] += ddy;
        }
        else
        {
            pdev->position[xdir] += pdx;
            pdev->position[ydir] += pdy;
        }
        sendStep(pdev);
        udelay(get_ramp_delay(&ramp, t));
   }
}

void stepper_line_to_3d(lpt_stepper_dev_t*pdev, long x1, long y1, long z1)
{
    int i, dx, dy, dz, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
    
    dx = pdev->position[xdir] - x1;
    dy = pdev->position[ydir] - y1;
    dz = pdev->position[zdir] - z1;
    x_inc = (dx < 0) ? -1 : 1;
    l = abs(dx);
    y_inc = (dy < 0) ? -1 : 1;
    m = abs(dy);
    z_inc = (dz < 0) ? -1 : 1;
    n = abs(dz);
    dx2 = l << 1;
    dy2 = m << 1;
    dz2 = n << 1;

    // todo! implement delay with ramp, 
    // calculate the values depending on the direction and the gradient
    
    if ((l >= m) && (l >= n)) 
    {
        err_1 = dy2 - l;
        err_2 = dz2 - l;
        for (i = 0; i < l && pdev->run; i++) 
        {
            if (hasReachedBoundary(pdev, getStatus(pdev))) { pdev->run = 0; break; }
            sendStep(pdev);
            udelay(pdev->delay[xdir]);
            if (err_1 > 0) 
            {
                pdev->position[ydir] += y_inc;
                err_1 -= dx2;
            }
            if (err_2 > 0) 
            {
                pdev->position[zdir] += z_inc;
                err_2 -= dx2;
            }
            err_1 += dy2;
            err_2 += dz2;
            pdev->position[xdir] += x_inc;
        }
    } 
    else if ((m >= l) && (m >= n)) 
    {
        err_1 = dx2 - m;
        err_2 = dz2 - m;
        for (i = 0; i < m && pdev->run; i++) 
        {
            if (hasReachedBoundary(pdev, getStatus(pdev))) { pdev->run = 0; break; }
            sendStep(pdev);
            udelay(pdev->delay[ydir]);
            if (err_1 > 0)
            {
                pdev->position[xdir] += x_inc;
                err_1 -= dy2;
            }
            if (err_2 > 0) 
            {
                pdev->position[zdir] += z_inc;
                err_2 -= dy2;
            }
            err_1 += dx2;
            err_2 += dz2;
            pdev->position[ydir] += y_inc;
        }
     }
     else 
     {
        err_1 = dy2 - n;
        err_2 = dx2 - n;
        for (i = 0; i < n && pdev->run; i++) 
        {
            if (hasReachedBoundary(pdev, getStatus(pdev))) { pdev->run = 0; break; }
            sendStep(pdev);
            udelay(pdev->delay[zdir]);
            if (err_1 > 0) 
            {
                pdev->position[ydir] += y_inc;
                err_1 -= dz2;
            }
            if (err_2 > 0) 
            {
                pdev->position[xdir] += x_inc;
                err_2 -= dz2;
            }
            err_1 += dy2;
            err_2 += dx2;
            pdev->position[zdir] += z_inc;
        }
    }
    if (pdev->run)
    {
        sendStep(pdev);
    }
}
