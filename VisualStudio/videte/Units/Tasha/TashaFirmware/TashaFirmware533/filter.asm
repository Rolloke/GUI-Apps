#include <asm_sprt.h>
#include <defBF533.h>


/*******************************************************************************
Copyright(c) 2000 - 2002 Analog Devices. All Rights Reserved.
Developed by Joint Development Software Application Team, IPDC, Bangalore, India
for Blackfin DSPs  ( Micro Signal Architecture 1.0 specification).

By using this module you agree to the terms of the Analog Devices License
Agreement for DSP Software. 
********************************************************************************
Module name     : median.asm
Label name      : _median
Version         :   1.3
Change History  :

                Version     Date          Author        Comments
                1.3         11/18/2002    Swarnalatha   Tested with VDSP++ 3.0
                                                        compiler 6.2.2 on 
                                                        ADSP-21535 Rev.0.2
                1.2         11/13/2002    Swarnalatha   Tested with VDSP++ 3.0
                                                        on ADSP-21535 Rev. 0.2
                1.1         01/28/2002    Raghavendra   Modified to match 
                                                        silicon cycle count
                1.0         06/29/2001    Raghavendra   Original 

Description     : In this function each pixel is replaced by median of gray 
                  levels in a neighborhood of that pixel. Border of the  output
                  image is same as input image. The median M of a set of values 
                  is such that half the values in the set are less than M and
                  half are greater than M. This filter removes the effect of 
                  extreme values  from data. This routine is helpful in removing
                  the effect of impulsive noise in images.
                 
Algorithm       : The alogorithm is implemented as follows.
                  The 3x3 median filtering is used here. The 3 elements of each 
                  row are arranged in MAX,MED and

                MIN values. i.e
                | MAX0  MED0  MIN0 |
                | MAX1  MED1  MIN1 |
                | MAX2  MED2  MIN2 |  

                 Later  MIN of first column, median of second column and MAX of
                 third column is taken.
                 The   median of these three pixel is written as output.
                 In output image border pixels are same as input pixel.

Prototype       : void _median(unsigned char* in, int row, int col, 
                               unsigned char *out );
      
                   in   ->  It is pointer to the input image.
                   row  ->  It is number of rows of input image.
                   col  ->  It is number of columns of input image.
                   out  ->  It is pointer the output buffer.

Registers used  : R0-R7, I0, M0, P0-P5, LC0, LC1.

Performance     : 
                  Code Size     :  290 Bytes
                  Cycle count   : 1926 Cycles for 8x8 image
                  Inner loop    : 50*(Column -2)
                  Outer loop    : 9*(Row-2)
ML: Funktioniert!
*******************************************************************************/

/*------------------------------------------------------------------*/
/* Programmsektion in L1				     						*/		
/*------------------------------------------------------------------*/
.section program;
.global     _median;
.align       8;

_median:

    [--SP]=(R7:4,P5:3);
    P4=[SP+40];             // Address of output array
    P5=R0;                  // Address of input array
    M0=R0;
    P1=R2;                  // Number of columns
    P0=R2;
    P2=R1;                  // Number of rows
    P3=R2;                   
//////////////////// First Row  ///////////////////////////////////////////////
    LSETUP(FIRST_ROW_ST,FIRST_ROW_END)LC0=P1;
FIRST_ROW_ST:
        R0=B[P5++](Z);          
FIRST_ROW_END:
        B[P4++]=R0;
                            // output = input 
    P5=M0;                  // Address of input array
    R3=R2<<1 ||R0=B[P5++](Z);
                            // fetch first element 
    P1+=-2;                 // Col-2
    P2+=-2;                 // ROW-2;
    R3+=1;
    P3+=-1;                 // col -1
//////////////// Middle loop ///////////////////////////////////////
    LSETUP(ROW_ST,ROW_END)LC0=P2;
                            // loop counter == ROW-2 (M-2) 
    P2=R3;                  // 2*col
    R1=B[P5++](Z);          // fetch x01
    P5=P5+P1;               // increment to fetch x10
    R2=B[P5](Z);            // fetch x10
    P5-=P1;                 // modify to first row
ROW_ST:
        B[P4++]=R2;
                            // first element in each row output = input 
        LSETUP(COL_ST,COL_END)LC1=P1;
                            // loop counter=col-2 
COL_ST:     R3=R0+R1(NS)|| R2=B[P5](Z);
                            // add first two elements and fetch next data 
            P5=P5+P1;       // modify pointer to next row
            R3=R3+R2(NS);   // get total sum of 3 element
            R4=MAX(R0,R1);          
            R4=MAX(R4,R2);  // maximum of first row
            R1=MIN(R0,R1)||R0=B[P5++](Z);
            R6=MIN(R1,R2)||R1=B[P5++](Z);
                            // minimum of first row,fetch next row elements 
            R5=R4+R6;       // sum of max and min of first row
            R3=R3-R5(NS) ||R2=B[P5](Z);
                            // median value of first row
            P5=P5+P1;       // Increment pointer to 3rd row
            R5=R0+R1;         
            R5=R5+R2;       // sum of 3 elements in second row
            R7=MAX(R0,R1);
            R7=MAX(R7,R2);  // Maximum value of second row
            R0=MIN(R0,R1);
            R0=MIN(R0,R2);  // minimum value of second row
            R1=R0+R7;
            R5=R5-R1(NS);   // median value of second row
            R6=MAX(R6,R0)||R0=B[P5++](Z);
                            // Maximum value of first row MIN and second row MIN 
            I0=R6;
            R4=MIN(R4,R7)||R1=B[P5++](Z);
                            // Minimum of first row MAX and second row MAX 
            R7=R0+R1(NS) ||R2=B[P5](Z); 
            R7=R7+R2(NS);
            R6=MAX(R0,R1);
            R6=MAX(R6,R2);  // Maximum value of 3rd row
            R0=MIN(R0,R1);
            R0=MIN(R0,R2);  // Minimum of 3rd row
            R2=R0+R6;
            R7=R7-R2;       // Median value of 3rd row
            R2=I0;
            R4=MIN(R4,R6);  //X= MIN(MAX0,MAX1,MAX2)
            R6=MAX(R0,R2);  //Y= MAX(MIN0,MIN1, MIN2)
            R0=R3+R7;
            R1=R0+R5;       // SUM OF MED0,MED1 and MED2
            R0=MAX(R3,R7);
            R0=MAX(R0,R5);  // MAX(MED0.MED1,MED2)
            R2=MIN(R3,R7);
            R2=MIN(R2,R5);  // MIN(MED0,MED1,MED2)
            R2=R2+R0;
            R1=R1-R2(NS);   //Z= Median of (MED0,MED1,MED2)
            R2=R4+R6;
            R2=R2+R1;       // Sum of X,Y,Z
            P5-=P2;
            R3=MAX(R4,R6);
            R3=MAX(R3,R1);  // MAX(X,Y,Z)
            R4=MIN(R4,R6);
            R4=MIN(R4,R1);  // MIN(X,Y,Z)
            R4=R4+R3(NS)||R0=B[P5++](Z);
            R2=R2-R4(NS)||R1=B[P5++](Z);
                            // R2==median value of 9 elements 
COL_END:    B[P4++]=R2;
                            // store the result 
        P5=P5+P3;           // modify to starting of next row
        R0=B[P5++](Z);      // store least element of the row
        R2=B[P5--](Z);    
        B[P4++]=R0;         // store first element of next row
        P5-=P3;                      
        R0=B[P5++](Z); 
ROW_END:R1=B[P5++](Z);
                            // fetch first two elements 
    P5=P5+P1;
    
    LSETUP(LAST_ROW_ST,LAST_ROW_END)LC0=P0;
LAST_ROW_ST:
        R0=B[P5++](Z);
LAST_ROW_END:
        B[P4++]=R0;
                            // copy last row output == input 
    (R7:4,P5:3)=[SP++];
    RTS;
    NOP;                    // To avoid one stall if LINK or UNLINK happens to 
                            //be the  next instruction in the memory.
_median.end: 

/*******************************************************************************
Copyright(c) 2000 - 2002 Analog Devices. All Rights Reserved.
Developed by Joint Development Software Application Team, IPDC, Bangalore, India
for Blackfin DSPs  ( Micro Signal Architecture 1.0 specification).

By using this module you agree to the terms of the Analog Devices License
Agreement for DSP Software. 
********************************************************************************
Module Name     : corr_3x3.asm
Label name      : __corr_3x3
Version         :   1.3
Change History  :

                Version     Date          Author        Comments
                1.3         11/18/2002    Swarnalatha   Tested with VDSP++ 3.0
                                                        compiler 6.2.2 on 
                                                        ADSP-21535 Rev.0.2
                1.2         11/13/2002    Swarnalatha   Tested with VDSP++ 3.0
                                                        on ADSP-21535 Rev. 0.2
                1.1         01/28/2002    Raghavendra   Modified to match 
                                                        silicon cycle count
                1.0         07/10/2001    Raghavendra   Original 

Description     : This function performs correlation between input image and 3x3 
                  mask. The mask elements are multiplied with corresponding input
                  elements and then summed together.

                 if input == | x00 x01 x02 |
                             | x10 x11 x12 |
                             | x20 x21 x22 |

                 and mask == | h00 h01 h02 |
                             | h10 h11 h12 |
                             | h20 h21 h22 |
 
                then output y11=( x00*h00 + x01*h01 + x02*h02 + x10*h10 + 
                                  x11*h11 + x12*h12 + x20*h20 + x21*h21 + 
                                  x22*h22 )>>12;
                 
Prototype       : void _corr_3x3(unsigned char* in, int row, int col, 
                                 short * mask, unsigned char *out );
                 
                   in   ->  It is pointer to the input image.
                   row  ->  It is number of rows of input image.
                   col  ->  It is number of columns of input image.
                   mask ->  It is pointer 3x3 mask.
                   out  ->  It is pointer the output buffer.

Assumption      : 1. Number of columns in input image is even  AND  aligned to 
                     word or half word boundary 
                  2. Input image and mask elements are in different Banks

Registers used  : A0, A1, R0-R3, I1, B1, M0-M3, L1, P0-P5, LC0, LC1.

Performance     :  
                  Code Size      : 690 bytes.
                  Cycle count    : 657 cycles for 8x8 image
       first and last row        : 10*(col-2)
       Middle rows    
           Inner loop            : 9*(col-2)
           Outer loop            :23*(row-2) 
      four corner elements       : 8*4 

*******************************************************************************/
.section  program;
.global   _corr_3x3;
.align    8;
    
_corr_3x3:
    [--SP]=(P5:3);
	[--SP]=L1;
    R3=[SP+28];             // Address of structured element
    I1=R3;                  // Initialize I register to structure element 
                            // address  
    B1=R3;                  // Initialize base register
    L1=18;                  // Initialize  L register
    M0=8;                   // Initialize to fetch h11
    M1=6;                   // initialize to skip 3 element in structured 
                            // element array
    P5=R0;                  // Address of input array
    M3=R0;                  // duplicate the address
    P0=R2;                  // Number of columns  (N)
    P1=R2;                  // Duplicate the number of columns
    I1+=M0;                 // Increment to fetch h11 element
    P4=[SP+32];             // Address of output array
    P2=R1;                  // Number of rows    (M)
    P3=R2;                  // Number of columns
    P0+=-4;                 // Columns -4
    R3=R2<<1 ||R0=B[P5++](Z)||R1.L=W[I1++];
                            // r3= 2*Col,fetch first input and h11 
    M0=R3;                  //
    R3+=2;
    M2=R3;                  // 2*col +2
    P3+=2;                  // Col+2
    P1+=-2;                 // Col-2
    P2+=-2;                 // ROW-2;

/////////////////////// FIRST ROW ///////////////////////////////////
// First row  and last row is computed separately. 
//In each row first and last elements are computed separately.
	A0=R0.L*R1.L(IS)||R0=B[P5++](z)||R1.L=W[I1++];
                            // fetch x(0,1) and h12 
    R2=W[P5++P1](Z)||R3.L=W[I1++];
                            // Dummy fetches to increment P5 and I1 
    A0+=R0.L *R1.L(IS)||R0=B[P5++](Z) || R1.L=W[I1++];
                            // Fetch x(1,0) and h21 
    A0+=R0.L*R1.L(IS)||R0=B[P5](Z) || R1.L=W[I1++];
                            // Fetch x(1,1) and h22 
    R2=(A0+=R0.L * R1.L)(IS)|| I1+=M1;
                            // Increment I1 to fetch h10 
    P5=M3;                  // Initialize P5 to  input array
    R2=R2>>12||R0=B[P5++](z) || R1.L=W[I1++];
                            // Fetch X(0,0) and H10 
//    B[P4++]=R2;             // store the result
    B[P4++]=R0;             // Original speichern, da sonst Farbfehler am Rand enstehen

    LSETUP(CORR_FIRST_ROW_ST,CORR_FIRST_ROW_END)LC0=P1;
                            // Loop counter ==COL-2 
   
CORR_FIRST_ROW_ST:
        A0=R0.L *R1.L(IS)||R0=B[P5++](z) ||R1.L=W[I1++];
                            // Fetch X(0,1) and H11 
        A0+=R0.L*R1.L(IS)||R0=B[P5] (z)||R1.L=W[I1++];
                            // Fetch X(0,2) and H12 
        P5=P5+P1;               // increment P5 to fetch x(1,0)
        A0+=R0.L *R1.L (IS)||R0=B[P5++](Z) || R1.L=W[I1++];
                            // Fetch X(1,0) and H20 
        A0+=R0.L *R1.L (IS)||R0=B[P5++](z) || R1.L=W[I1++];
                            // Fetch X(1,1) and H21 
        A0+=R0.L *R1.L(IS) ||R0=B[P5++](z) || R1.L=W[I1++];
                            // Fetch X(1,2) and H22 
        R2=(A0+=R0.L *R1.L)(IS) || I1+=M1;
                            // Increment I1 to fetch h10 
        P5-=P3; 
        R2=R2>>12||R0=B[p5++](z);
CORR_FIRST_ROW_END:
        //B[P4++]=R2|| R1.L=W[I1++]; // Store the result 
        B[P4++]=R0|| R1.L=W[I1++]; // Original speichern, da sonst Farbfehler am Rand enstehen
        
       
    A0=R0.L *R1.L (IS) ||R0=B[P5++](z)  || R1.L=W[I1++];
                            // fetch x(0,n-1) and H11 
    NOP;
    R2=W[P5++P1](Z) || R3.L=W[I1++];
                            // Dummy fetch to increment P5 and I1 pointers 
    A0+=R0.L * R1.L (IS)|| R0=B[P5++](z)|| R1.L=W[I1++];
                            // Fetch x(1,N-2) and H20 
    A0+=R0.L * R1.L (IS)|| R0=B[P5++](z)|| R1.L=W[I1++];
                            // Fetch x(1,N-1) and H21 
    R2=(A0+=R0.L *R1.L) (IS)|| I1+=4;
                            // Increment to fetch h01 
    P5=M3;                  // Initialize P5 to starting of  input array
    R2=R2>>12||R0=B[P5++](z)||R1.L =W[I1++] ;
                            // Fetch x(0,0) and H01 
    // B[P4++]=R2;             // store output of last element in first row
    B[P4++]=R0;             // Original speichern, da sonst Farbfehler am Rand enstehen
    
    P3=M0;                  // Initialized to 2*col
/////////////////////  MIDDLE   LOOP   ///////////////////////////////////////
    LSETUP(CORR_ROW_ST,CORR_ROW_END)LC0=P2;
                            // loop counter == ROW-2 (M-2) 
    P2=M2;                  // Initialized to 2*COL+2
   
CORR_ROW_ST:
		A0=R0.L *R1.L(IS) || R0=B[P5++](z) || R1.L=W[I1++];
                            // Fetch x(0,1) and H02 
        R2=W[P5++P1](Z) ||R3.L=W[I1++];
                            // Dummy fetch to adjust P5 and I1 
        A0+=R0.L*R1.L(IS) || R0=B[P5++](z) || R1.L=W[I1++];
                            // fetch x(1,0) and H11 
        A0+=R0.L* R1.L(IS) || R0=B[P5++](z)|| R1.L=W[I1++];
                            // Fetch x(1,1) and H12 
        R2=W[P5++P1](Z)|| R3.L=W[I1++];
                            // Dummy fetch to adjust P5 and I1 
        A0+=R0.L*R1.L(IS) || R0=B[P5++](z) || R1.L=W[I1++];
                            // Fetch x(2,0) and H21 
        A0+=R0.L* R1.L(IS) || R0=B[P5++](z)|| R1.L=W[I1++];
                            // Fetch x(2,1) and H22 
        R2=(A0+=R0.L*R1.L )(IS)|| R1.L=W[I1++];
                            //fetch first element of the row and h00 
        P5-=P2;             // P5 modified to starting of the row
        R2=R2>>12||R0=B[P5++](z);
        // B[P4++]=R2;         // store the output
    	B[P4++]=R0;             // Original speichern, da sonst Farbfehler am Rand enstehen
    
/////////////////////////// CORE LOOP      /////////////////////////////////////
// Here data is processed to get two pixels output at a time
        LSETUP(CORR_COL_ST,CORR_COL_END)LC1=P1>>1;
                            // Loop counter ==(COL-2)/2 
CORR_COL_ST:     A0=R0.L*R1.L(IS) ||R0=B[P5++](z)   || R1.H=W[I1++];
                            // Fetch x(0,1) and h01 
            A1=R0.L * R1.L ,A0+=R0.L*R1.H(IS)  || R0=B[P5++](z) || R1.L=W[I1++];
                            // Fetch x(1,2) and h02 
            A1+=R0.L *R1.H ,A0+=R0.L *R1.L(IS) || R0=B[P5++](z) ||R1.H=W[I1++];
                            // Fetch x(1,3) and h10 
            R2=W[P5++P0](Z) ||NOP;     // move  P5 to starting of next row
            A1+= R0.L *R1.L (IS) ||R0=B[P5++](z)||R1.L=W[I1++];
                            // Fetch x(1,0) and h11 
            A0+=R0.L *R1.H(IS)|| R0=B[P5++](z) ;
                            // Fetch x(1,1) 
            A1+=R0.L * R1.H,A0+=R0.L *R1.L(IS) || R0=B[P5++](z)|| R1.H=W[I1++];
                            // Fetch x(1,2) and h12 
            A1+= R0.L *R1.L ,A0+=R0.L *R1.H(IS)|| R0=B[P5++](z)||R1.L=W[I1++];
                            // Fetch x(1,3) and h20 
            P5=P5+P0;               // move P5 to starting of next row
            A1+= R0.L *R1.H(IS)||R0=B[P5++](z) || R1.H=W[I1++];
                            // Fetch x(2,0) and h21 
            A0+=R0.L*R1.L(IS)  ||R0=B[P5++](z);
                            // Fetch x(2,1) 
            A1+=R0.L * R1.L ,A0+=R0.L *R1.H(IS)|| R0=B[P5++] (z)||R1.L=W[I1++];
                            // Fetch x(2,2) and h22 
            A1+=R0.L *R1.H,R2=(A0+=R0.L *R1.L)(IS)||R0=B[P5++](z);
                            // Fetch x(2,3) 
            P5-=P2;                 // modify P5 to process next set of data
            R3=(A1+=R0.L *R1.L)(IS)||R0=B[P5++](z) || R1.L=W[I1++];
                            // Fetch x(0,2) and h00 
            R2>>=12;
            R3=R3>>12||B[P4++]=R2;
                            // Store first output
CORR_COL_END:    B[P4++]=R3;
                            // Store next output 
    
        A0=R0.L *R1.L (IS)||R0=B[P5++](z) || R1.L=W[I1++];
                            // Fetch x(0,N-2) and h01 
        R2=W[P5++P1](Z) ||R3.L=W[I1++];
                            // Dummy fetch to increment P5 and I1 
        A0+=R0.L *R1.L (IS)||R0=B[P5++](z) || R1.L=W[I1++];
                            // Fetch  X(1,N-2) and H10; 
        A0+=R0.L*R1.L(IS) ||R0=B[P5++](z) || R1.L=W[I1++];
                            // Fetch  X(1,N-1) and H11; 
        R2=W[P5++P1](Z) || R3.L=W[I1++];
                            // Dummy fetch to increment P5 and I1 
        A0+=R0.L *R1.L (IS)||R0=B[P5++] (z)|| R1.L=W[I1++];
                            // Fetch x(2,N-2) and h20 
        A0+=R0.L*R1.L(IS) ||R0=B[P5++] (z)|| R1.L=W[I1++];
                            // Fetch x(2,N-1) and h21; 
        R2=(A0+=R0.L *R1.L)(IS)|| I1+=4;
                            // modify I1 to fetch h01 
        P5-=P3;                 // modify P5 to starting of next row
        R2=R2>>12||R0=B[P5++](z)||R1.L =W[I1++];
                            // fetch x(0,0) and H01; 

CORR_ROW_END:B[P4++]=R2;   
    
//////////////////////  LAST ROW   ////////////////////////////////////////////

	P2=P1;
    P2+=4;                  // P2==Col +2
    A0=R0.L *R1.L(IS) || R0=B[P5++](z)||R1.L=W[I1++];
                            // fetch x(M-2,1) and h02 
    R2=W[P5++P1](Z)|| R3.L=W[I1++];
                            // dummy fetch to increment P5 and I1 
    A0+=R0.L*R1.L(IS)||R0=B[P5++](z)||R1.L=W[I1++];
                            // fetch x(M-1,0) and h11 
    A0+=R0.L *R1.L (IS)||R0=B[P5++](z)||R1.L=W[I1++];
                            // fetch x(M-1,2) and h12 
    P5-=P2;      
    I1+=M1;                 // I1 is modified to fetch h00
    R2=(A0+=R0.L *R1.L) (IS)||R0=B[P5++](z) || R1.L=W[I1++];
                            // fetch x(M-2,0) and  h00 
    NOP;
    R2>>=12;
    //B[P4++]=R2;           // store the result
   	B[P4++]=R0;             // Original speichern, da sonst Farbfehler am Rand enstehen
   
    LSETUP(CORR_LAST_ROW_ST,CORR_LAST_ROW_END)LC0=P1;
CORR_LAST_ROW_ST:
        A0=R0.L*R1.L (IS) ||R0=B[P5++](z) || R1.L=W[I1++];
                            // fetch x(M-2,1) and h01 
        A0+=R0.L *R1.L(IS) ||R0=B[P5](z) || R1.L=W[I1++];
                            // fetch x(M-2,2) and h02 
        P5=P5+P1;               // modify P5 to  next row
        A0+=R0.L *R1.L(IS) ||R0=B[P5++](z) || R1.L=W[I1++];
                            // fetch x(M-1,0) and h10 
        A0+=R0.L *R1.L(IS) ||R0=B[P5++](Z) || R1.L=W[I1++];
                            // fetch x(M-1,1) and h11 
        A0+=R0.L *R1.L(IS) ||R0=B[P5++](z) || R1.L=W[I1++];
                            // fetch x(M-1,2) and h12 
        R2=(A0+=R0.L *R1.L)(IS)||I1+=M1;
                            // modify I1 to fetch h00 
        P5-=P2;
        R2=R2>>12||R0=B[P5++] (z)|| R1.L=W[I1++];
                            // fetch h00; 
CORR_LAST_ROW_END:
        //B[P4++]=R2;         // store the result
        B[P4++]=R0;         // Original speichern, da sonst Farbfehler am Rand enstehen
    
    A0=R0.L *R1.L(IS) || R0=B[P5++](z)||R1.L=W[I1++];
                            // fetch x(m-2,N-1) and h01 
    R2=W[P5++P1](Z) || R3.L=W[I1++];
                            // dummy fetch to increment P5 to next row and I1 
                            // to fetch h10 
    A0+=R0.L*R1.L(IS)||R0=B[P5++](z)||R1.L=W[I1++];
                            // fetch h10 and x(m-1,N-2) 
    A0+=R0.L *R1.L (IS)||R0=B[P5++](z)||R1.L=W[I1++];
                            // fetch h11 and x(m-1,N-1) 
    R2=(A0+=R0.L *R1.L )(IS);
    R2>>=12;
    B[P4++]=R2 ;            // store the final output pixel

    L1=[SP++];
    (P5:3)=[SP++];          // POP  P5-3
    RTS;
    NOP;                    // To avoid one stall if LINK or UNLINK happens to 
                            // be the  next instruction in the memory.
                            
_corr_3x3.end: 


/*------------------------------------------------------------------*/
/* Programmsektion in L1				     						*/		
/*------------------------------------------------------------------*/
.section program;
.global     _temporal;
.align       8;
#if (0)
_temporal:

    [--SP]=(R7:4,P5:3);
    P4=R0;                  // Address of first input array
    P0=R2;                  // Number of columns
    P1=R1;                  // Number of rows
    P5=[SP+40];             // Address of second input array

   	R5 = 8;					// Schwellwert
   	
//////////////// Middle loop ///////////////////////////////////////
    LSETUP(TEMPO_ROW_ST,TEMPO_ROW_END)LC0=P1;
                            
TEMPO_ROW_ST:
                           
        LSETUP(TEMPO_COL_ST,TEMPO_COL_END)LC1=P0;
        
		              
TEMPO_COL_ST:
        R0 = B[P4](z);
        R1 = B[P5](z);
               
        R2 = R0+R1;
        R2 >>= 1; 			// Mittelwert         
    
        B[P5++] = R0;		// Original Frame kopieren
            
        R3 = R2-R0;			// Absolute Abweichung vom Mittelwert
        R3 = ABS R3;		//     "

        CC = R3 <= R5;		// Kleiner Schwellwert?     
	    if CC JUMP TEMPO_FILTER;
			R0 = 0;
			
			P2  = P4;
	        P2 -= P0;
	        R1 = B[P2-1](z);	    
			R0 = R0 + R1; 	
			
			R1 = B[P2+0](z);    
			R0 = R0 + R1; 
			R0 = R0 + R1; 
			
			R1 = B[P2+1](z);    
			R0 = R0 + R1;     	
			
			R1 = B[P4-1](z);    
			R0 = R0 + R1; 
			R0 = R0 + R1; 

			R1 = B[P4+0](z);    
			R1 <<= 2;
			R0 = R0 + R1; 
			
			R1 = B[P4+1](z);    
			R0 = R0 + R1; 
			R0 = R0 + R1; 
			
			P2  = P4;
	        P2  = P2 + P0;

	        R1 = B[P2-1](z);    
			R0 = R0 + R1; 
			
			R1 = B[P2+0](z);    
			R0 = R0 + R1; 
			R0 = R0 + R1; 

			R1 = B[P2+1](z);    
			R0 = R0 + R1; 
			
			R0 = R0 >> 4;

			JUMP TEMPO_CONT;				
TEMPO_FILTER: 	
	    	R0 = R2;	
TEMPO_CONT:
	    	B[P4++] = R0;		       
        
TEMPO_COL_END: NOP;

TEMPO_ROW_END: NOP;
                            
    (R7:4,P5:3)=[SP++];
    RTS;
    NOP;                    // To avoid one stall if LINK or UNLINK happens to 
                            //be the  next instruction in the memory.
_temporal.end: 
#else
_temporal:

    [--SP]=(R7:4,P5:3);
    I2=R0;                  // Address of first input array
    P1=R2;                  // Number of columns
    P2=R1;                  // Number of rows
    P0=[SP+40];             // Address of second input array
   	I3 = P0;

    P1=P1>>2;				// /4
   	I0 = 0;
   	I1 = 0;
   	L0 = 0;
   	L1 = 0; 
//////////////// Middle loop ///////////////////////////////////////
    LSETUP(TEMPO_ROW_ST,TEMPO_ROW_END)LC0=P2;
                            
TEMPO_ROW_ST:
                           
        LSETUP(TEMPO_COL_ST,TEMPO_COL_END)LC1=P1;
        
TEMPO_COL_ST:
		R0 = [I2] || R2 = [I3];
        R4 = BYTEOP1P(R1:0, R3:2) || [I3++] = R0;
TEMPO_COL_END: [I2++] = R4;


TEMPO_ROW_END: NOP;
                          
    (R7:4,P5:3)=[SP++];
    RTS;
    NOP;                    // To avoid one stall if LINK or UNLINK happens to 
                            //be the  next instruction in the memory.
_temporal.end: 
#endif

/*******************************************************************************
Copyright(c) 2000 - 2002 Analog Devices. All Rights Reserved.
Developed by Joint Development Software Application Team, IPDC, Bangalore, India
for Blackfin DSPs  ( Micro Signal Architecture 1.0 specification).

By using this module you agree to the terms of the Analog Devices License
Agreement for DSP Software. 
********************************************************************************
Module name     : skeleton.asm
Label name      : __skeleton
Version         :   1.3
Change History  :

                Version     Date          Author        Comments
                1.3         11/18/2002    Swarnalatha   Tested with VDSP++ 3.0
                                                        compiler 6.2.2 on 
                                                        ADSP-21535 Rev.0.2
                1.2         11/13/2002    Swarnalatha   Tested with VDSP++ 3.0
                                                        on ADSP-21535 Rev. 0.2
                1.1         01/28/2002    Raghavendra   Modified to match 
                                                        silicon cycle count
                1.0         07/04/2001    Raghavendra   Original 

Description     : This function performs skeleton operation on input binary 
                  image. Skeleton is the way to decompose an object into a set 
                  of unique characteristics or features that can be used to 
                  identify the object. Skeleton is achieved repetitive erosion 
                  and opening operation and finding the difference.
                  The nth  order difference between the eroded image and the 
                  opening of the eroded image is defined by
                
                    Kn(A)=En(A)-open(En(A),B). 

                  The skeleton of object A becomes the union of all N terms of 
                  Kn(A), implemented until the nth erosion of A by nB yields 
                  the NULL set.

                  The structured elements are used is  as below

                                       | 0  1  0 |
                                       | 1  1  1 |
                                       | 0  1  0 |

                 Only basic operation is implemented in this routine. i.e
                 In this implementation erosion and opening operation on object
                 A is done and difference between these two is calculated which 
                 is a partial result.
                 The Union of these partial result is the final result.
                 User has to call this routine  such that eroded result becomes 
                 the input for next iteration.
                 The user has to pass the previous partial output as output 
                 array because, OR operation is done on present result and 
                 previous  results. The result of opening operation is stored in
                 stack before calculating the difference. 
             
Assumption      : 1. Number of columns of input image is even  AND  aligned to 
                     word or half word  boundary 
                  2. Output  buffer is cleared for the first time calling and 
                     contains previous results also aligned to word or half word
                     boundary

Prototype       : void _skeleton(unsigned char* in, int row, int col, unsigned 
                                 char * erode_res, unsigned char *out );

                   in   ->  It is pointer to the input image.
                   row  ->  It is number of rows of input image.
                   col  ->  It is number of columns of input image.
              erode_res ->  It is pointer  to temp where output of erosion is 
                            stored.
                   out  ->  It is pointer the output buffer.

Registers used  : A0, A1, R0-R7, I0, I2, I3, M0-M2, P0-P5, LC0, LC1.

Performance     :  
             Code size   : 720 bytes
             Cycle count : 1287 for 8x8 image
            Erosion      : 
       first and last row: 1* (col)
          Middle rows    :   Inner loop  : 8.5*(Col -2) 
                             outer loop  :   5*(Row-2)

            Dilation    :
     four corner elements: 32 cycles
     first and last row : 7* (Col -2)
     Middle rows        : Inner Loop    :8.5* (Col-2) 
                          Outer Loop    :21 * (Row-2)     
 Difference calculation and
 Union with previous result : 5* row*col     
ML: Habe ich bisher nicht zur Funktion gebracht
*******************************************************************************/
.section program;
.global  _skeleton;
.align   8;
    
_skeleton:
    [--SP]=(R7:4,P5:3);
                            // Push R7-4 and P5-3 
    R5=R1-R1(NS) ||P4=[SP+40];
                            // Clear R5, Address of erode_res 
    P5=R0;                  // Address of input array
    I0=R0;                          
    P1=R2;                  // Number of columns
    M2=P4;                         
    I2=R2;                  // Number of columns
    P3=R2;                  // Number of columns
    P2=R1;                  // Number of rows
    R6=5;                   // Threshold to compare the result for erosion
    P0=R2;                  // Number of columns  (N)
    R0=R1.L*R2.L(IS)||R3=[SP+44];
                            // Address of output array and fetch h00; 
    M0=R0;             
    I3=R3;                  // Address of output buffer
    NOP;                    // To remove IAU empty STALL
/////////// EROSION First Row  ///////////////////////////////////////////////
    LSETUP(EROSION_FIRST_ROW_ST,EROSION_FIRST_ROW_ST)LC0=P1;
EROSION_FIRST_ROW_ST:
        B[P4++]=R5;
                            // Clear first row 
    P5+=1;
    R3=R2<<1 ||R0=B[P5++](Z);
                            // R3= 2*Col,fetch first input 
    P0+=-3;                 // Columns -3
    P1+=-2;                 // Col-2
    P2+=-2;                 // ROW-2;
    R7=1; 
    M1=P2;                  // Col-2
///////////// EROSION Middle Rows   ////////////////////////////////////////
    LSETUP(EROSION_ST,EROSION_END)LC0=P2;
                            // loop counter == ROW-2 (M-2) 
    P2=R3;                  // 2*col
EROSION_ST:
        B[P4++]=R5;

        LSETUP(E_COL_ST,E_COL_END)LC1=P1>>1;
                            // loop counter ==(COL-2)/2 
E_COL_ST:
            A0=R0.L*R7.L(IS) ||R0=B[P5](z);
                            // fetch x(0,1) 
            R4=R1-R1(NS)||R3=W[P5++P1](Z);
                            // clear R4, move  P5 to starting of next row 
            A1=R0.L * R7.L(IS)||R0=B[P5++](z);
                            // fetch x(1,0) 
            A0+=R0.L *R7.L(IS)|| R0=B[P5++](z);
                            // fetch x(1,1) 
            A1+=R0.L * R7.L,A0+=R0.L *R7.L(IS) ||R0=B[P5++](z);
                            // fetch x(1,2) 
            A1+= R0.L *R7.L ,A0+=R0.L *R7.L(IS)||R0=B[P5++](z);
                            // fetch x(1,3) 
            P5=P5+P0;               // move P5 to  x(2,1)
            A1+= R0.L *R7.L(IS)||R0=B[P5++](z);
                            // fetch x(2,21) 
            R2=(A0+=R0.L*R7.L)(IS)  ||R0=B[P5++](z);
                            // fetch x(2,2) 
            P5-=P2;         // modify P5 to process next set of data
            R3=(A1+=R0.L *R7.L)(IS)||R0=B[P5++](Z);
            CC=R2==R6;      // compare r2  with threshold
            IF CC R4=R7;    // if true r4==1
            R2=R1-R1(NS)||B[P4++]=R4;
                            // store first output 
            CC=R3==R6;      // compare r3 with threshold
            IF CC R2=R7;    // if true r2==1
E_COL_END:  B[P4++]=R2;
                            // store next output 
        P5+=1;              // Increment P5 to starting of next row
        R0=B[P5++](Z) ;     // fetch first element
EROSION_END:
        B[P4++]=R5;
                            // store  last element as zero 
    
////////////////// EROSION  LAST ROW   ////////////////////////////////////////
    LSETUP(EROSION_LAST_ROW_ST,EROSION_LAST_ROW_ST)LC0=P3;
EROSION_LAST_ROW_ST:
        B[P4++]=R5 ;
                            // store last row as zero 
///////////// DILATION  /////////////////////////////////////////////////////
    P5=M0;                  // ROW*COL
    SP-=P5;                 // modify stack pointer to store temp result
    P4=SP;    
    P5=M2;                  // Address where eroded result is stored
    R2=I2;                  // Number of column
    R3=R2<<1||R0=B[P5++](z);// 2*col
// First row  and last row is computed separately. 
//In each row first and last elements are computed separately.
    A0=R0.L*R7.L(IS)||R0=B[P5++](Z);
                            // fetch x(0,1) 
    P5=P5+P1;               // dummy fetches to increment P5
    A0+=R0.L *R7.L(IS) ||R0=B[P5](Z);
                            // fetch x(1,0) 
    R2=(A0+=R0.L *R7.L)(IS);
    CC=R2==0;               // check if result is zero
    IF !CC R2=R7;           // if not R2==1
    P5-=P3;                 // Initialize P5 to  input array
    B[P4++]=R2;             // store the result
    MNOP||R0=B[P5++](z);    // fetch X(0,0)

    LSETUP(D_FIRST_ROW_ST,D_FIRST_ROW_END)LC0=P1>>1;
                            // loop counter COL-2 
D_FIRST_ROW_ST:
        A0=R0.L *R7.L(IS) ||R0=B[P5++](z);
                            // fetch X(0,1) 
        A1=R0.L * R7.L, A0+=R0.L*R7.L(IS)||R0=B[P5++](z);
                            // fetch X(0,2) 
        A1+=R0.L*R7.L,A0+=R0.L*R7.L(IS)||R0=B[P5](Z);
                            // fetch x(0,3) 
        P5=P5+P1;               // increment P5 to fetch x(1,1)
        A1+=R0.L*R7.L(IS) ||R0=B[P5++](Z);
                            // fetch X(1,1) 
        R2=(A0+=R0.L *R7.L)(IS)||R0=B[P5](Z);
        P5-=P3;             // Decrement P5 to process next data
        R1=(A1+=R0.L*R7.L)(IS)||R0=B[P5++](Z);
        CC=R2==0;           // check if result is 0
        IF !CC R2=R7;       // if not r2==1
        B[P4++]=R2;         // store the result
        CC=R1==0;
        IF !CC R1=R7;
D_FIRST_ROW_END:
        B[P4++]=R1;
                            // store second result 
    A0=R0.L*R7.L (IS) ||R0=B[P5](z);
                            // fetch x(0,n-1) 
    P5=P5+P3;                     
    A0+=R0.L * R7.L (IS)|| R0=B[P5++](z) ;
                            // fetch x(1,N-1) 
    R2=(A0+=R0.L *R7.L)(IS) ;            
    CC=R2==0;               // check if result is zero
    IF !CC R2=R7;                             
    P5=M2;                  // Initialize P5 to starting of erosion output array
    B[P4++]=R2;             // store output of last element in first row
    
//////////////// DILATION  MIDDLE  LOOP   /////////////////////////////////////
    P2=M1;                  // ROW-2

    LSETUP(D_ROW_ST,D_ROW_END)LC0=P2;
                            // loop counter == ROW-2 (M-2) 
    P2=R3;                  // Initialized to 2*COL
    MNOP||R0=B[P5](z);      // fetch x(0,0)
    MNOP;                   // MNOP to remove IAU stall
D_ROW_ST:
        P5=P5+P3; // adjust P5 to next row
        A0=R0.L*R7.L(IS)||R0=B[P5++](z) ;
                            // fetch x(1,0) 
        A0+=R0.L* R7.L(IS)|| R0=B[P5++](z);
                            // fetch x(1,1) 
        P5=P5+P1;               // adjust P5 to next row
        A0+=R0.L*R7.L (IS)||R0=B[P5++](z);
                            // fetch first element of the row 
        P5-=P2;             // P5 modified to starting of the row
        R2=(A0+=R0.L*R7.L)(IS)|| R0=B[P5++](Z);
        CC=R2==0;
        IF !CC R2=R7;
        B[P4++]=R2 ;        // store the output
    
//////////////////// DILATION  CORE LOOP  ////////////////////////////////////
// Here data is processed to get two pixels output at a time
        LSETUP(D_COL_ST,D_COL_END)LC1=P1>>1;
                            // loop counter ==(COL-2)/2 
D_COL_ST:   A0=R0.L*R7.L(IS) ||R0=B[P5](z);
                            // fetch x(0,2) 
            MNOP||R3=W[P5++P1](Z);
                            // move  P5 to starting of next row
            A1=R0.L * R7.L(IS)||R0=B[P5++](z);
                            // fetch x(1,0) 
            A0+=R0.L *R7.L(IS)|| R0=B[P5++](z) ;
                            // fetch x(1,1) 
            A1+=R0.L * R7.L,A0+=R0.L *R7.L(IS) ||R0=B[P5++](z);
                            // fetch x(1,2) 
            A1+= R0.L *R7.L ,A0+=R0.L *R7.L(IS)||R0=B[P5++](z);
                            // fetch x(1,3) 
            P5=P5+P0;               // move P5 to starting of next row
            A1+= R0.L *R7.L(IS)||R0=B[P5++](z);
                            // fetch x(2,0) 
            R2=(A0+=R0.L*R7.L)(IS)  ||R0=B[P5++](z);
                            // fetch x(2,1) 
            P5-=P2;         // modify P5 to process next set of data
            R3=(A1+=R0.L *R7.L)(IS)||R0=B[P5++](Z);
            CC=R2==0;       // compare r2  with threshold
            IF !CC R2=R7;   // if false r2==1
            B[P4++]=R2;     // store first output
            CC=R3==0;       // compare r3 with zero
            IF !CC R3=R7;   // if false  r3==1
D_COL_END:  B[P4++]=R3;
                            // store next output 
        P5=P5+P1;           // increment P5
        A0=R0.L *R7.L (IS)||R0=B[P5++](z);
                            // fetch x(0,N-2) 
        A0+=R0.L *R7.L (IS)||R0=B[P5](z);
                            // fetch  X(1,N-2) 
        P5=P5+P3;           // increment P5 to next row
        A0+=R0.L*R7.L(IS) ||R0=B[P5++](z) ;
                            // fetch  X(1,N-1) 
        P5-=P2;             // modify P5 to starting of previous  row
        R2=(A0+=R0.L *R7.L)(IS)||R0=B[P5](Z);
                            // fetch h01 
        CC=R2==0;
        IF !CC R2=R7;  
D_ROW_END:
        B[P4++]=R2;   
    
///////////////////  DILATION LAST ROW   //////////////////////////////////
    P5=P5+P3;               // modify P5 to next row
    A0=R0.L *R7.L(IS)||R0=B[P5++](Z);
                            // fetch x(M-2,1) 
    A0+=R0.L*R7.L(IS)||R0=B[P5](z);
                            // fetch x(M-1,0) 
    P5-=P3;      
    R2=(A0+=R0.L *R7.L) (IS)||R0=B[P5++](z);
                            // fetch x(M-2,0) 
    CC=R2==0;
    IF !CC R2=R7;
    B[P4++]=R2;             // store the result
    MNOP;

    LSETUP(D_LAST_ROW_ST,D_LAST_ROW_END)LC0=P1>>1;
D_LAST_ROW_ST:
        A0=R0.L *R7.L(IS) ||R0=B[P5](z) ;
                            // fetch x(M-1,0) 
        P5=P5+P1; 
        A1=R0.L *R7.L(IS) ||R0=B[P5++](Z);
                            // fetch x(M-1,1) 
        A0+=R0.L *R7.L(IS)||R0=B[P5++](z);
                            // fetch x(M-1,2) 
        A1+=R0.L *R7.L,A0+=R0.L*R7.L(IS)||R0=B[P5++](Z);
        A1+=R0.L*R7.L,R2=(A0+=R0.L*R7.L)(IS)|| R0=B[P5](Z);
        P5-=P3;
        R1=(A1+=R0.L *R7.L)(IS)||R0=B[P5++](Z);
                            // fetch h00 
        CC=R2==0;
        IF !CC R2=R7;
        B[P4++]=R2;         // store the result
        CC=R1==0;
        IF !CC R1=R7;
D_LAST_ROW_END:
        B[P4++]=R1;
    
    P5=P5+P1;               // last output processed here
    A0=R0.L*R7.L(IS)||R0=B[P5++](z);
                            // fetch  x(m-1,N-2) 
    A0+=R0.L *R7.L (IS)||R0=B[P5](z);
                            // fetch  x(m-1,N-1) 
    R2=(A0+=R0.L *R7.L )(IS);
    CC=R2==0;
    IF !CC R2=R7;
    B[P4++]=R2 ;            // store the final output pixel
    
// In this loop Difference between EROSION and OPEN 
//function calculated and ORed with
                            // Previous partial result
    P3=M0;                  // Number of elements to process i.e ROW * COL
    P0=I3;
    P4=SP;                  // Address where result of OPEN is stored
    P1=I0;                  // Address where result of EROSION is stored
    R0=B[P1++](Z);          // fetch first element of EROSION result
    R1=B[P4++](Z);          // fetch first element of OPEN  result
    P2=P0;     
    R3=B[P2++](Z); 
    
    LSETUP(DIFF_ST,DIFF_END)LC0=P3;
DIFF_ST:
        R2=R1-R0(NS)|| R0=B[P4++](Z);
                            // En(A)-OPEN(EN(A),B) 
        R2=ABS R2 ||R1=B[P1++](Z);
        R2=R3|R2;           // OR with previous result
        R3=B[P2++](Z);
DIFF_END:
        B[P0++]=R2;         // store the result
    
    SP=SP+P3;               // Restore stack pointer
    (R7:4,P5:3)=[SP++];     // POP R7-4 and P5-3
    RTS;
    NOP;                    // To avoid one stall if LINK or UNLINK happens to 
                            //be the  next instruction in the memory.
_skeleton.end:   
