
/*************************************************************************
 *
 *   ZR050_IO.H
 *
 *   050 coder IO scheme CLASS definitions
 *
 *
 *   Copyright (c) 1993 Zoran Corporation.  All Rights Reserved.
 *
 *
 *************************************************************************/

#ifndef ZR050_IO
#define ZR050_IO
#if 0



/*
 *------------------------------------------------------------------------------
 *
 * Class          : Z050_IO
 *
 * Purpose        : This is an ABSTRACT class which provides virtual READ and
 *                  WRITE functions to an 050 coder object (an instance of
 *                  the Z050_Coder class).
 *                  No object of this class type can be created. Instead, 
 *                  a derived class must be created in which an appropriate
 *                  implementation must be given for Read050 and Write050 
 *                  according to the IO scheme used to access the 050 on the
 *                  application board. 
 *                  A pointer to an object of the derived class then can be
 *                  passed as parameter to Z050_Coder constructor.  
 *
 * Data Structure : None
 *
 * Methods        : 1. Read050 - defines the IO scheme for reading a BYTE from
 *                     an 050 coder.
 *                  2. Write050 - defines the IO scheme for writing a BYTE to
 *                     an 050 coder.
 *                  3. Check050End - defines the IO scheme for checking if
 *                     the 050 coder has ASSERTED the END signal.
 *                     NOTE: the 050's END signal is supposed to be reflected
 *                           by the END bit in STATUS1 register.
 *                           this function has been added for cases when the
 *                           reading of the status is problematic.
 *
 *------------------------------------------------------------------------------
 */

                                        
class Z050_IO       
{
public:

    virtual BYTE Read050( WORD Address) = 0; 

    virtual void Write050( WORD Address, BYTE Data) = 0; 

    virtual BOOL Check050End( ) = 0; 
};
#endif
#endif
