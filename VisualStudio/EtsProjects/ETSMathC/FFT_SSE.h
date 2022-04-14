
/*
>> I have a C++ template class for doing FFTs using
>> a 'Complex' class. It was actually done for research
>> into rounding effects with an integer FFT. So, you
>> can define a 'frac24complex' for instance (24-bit
>> fractional complex #'s) and apply the fft to that class
>> instead of a 'standard' complex class.
>> 
>> So, it's fairly canonical and general. It uses a
>> technique I have frequently used, where the 'twiddle'
>> factors are stored in a precomputed bit-reversed table.
>> exp[0], exp[pi*i/2], exp[pi*i/4], exp[3*pi*i/4], exp[pi*i/8] ...
>> 

It contains reasonable
self-documentation and has been tested under VC++4 with
a 'complex' class meeting only the minimum requirements
as defined in cfft.h.

The first paragraph above could serve as a reasonable
'link descriptor'.

*/

#if !defined( _AFFT_H_INC__ )
#define _AFFT_H_INC__ 1

#include <math.h>  // for sin and cos
#include <complex>


	// the original AltiVec way
#define Altivec_Const4(type,e0,e1,e2,e3) \
((type){e0,e1,e2,e3})
#define Altivec_Const8(type,e0,e1,e2,e3,e4,e5,e6,e7) \
((type){e0,e1,e2,e3,e4,e5,e6,e7})
#define Altivec_Const16(type,e0,e1,e2,e3,e4,e5,e6,e7,e8,e9,ea,eb,ec,ed,ee,ef) \
((type){e0,e1,e2,e3,e4,e5,e6,e7,e8,e9,ea,eb,ec,ed,ee,ef})


/*
 * This is a general-purpose C++ complex FFT transform class.
 * it is defined as a template over a complex type. For instance,
 * if using gnu gcc, the complex type is
 *			complex<double>    // include <complext.h> first
 * And you declare the cfft class as
 *          cfft<complex<double>>
 *
 * The underlying CPLX type requires:
 *  CPLX()
 *  operator = , CPLX(CPLX const&)
 *	CPLX(double,double)	[used on cos/sin]
 *	CPLX operator *( CPLX , double )
 *  CPLX conj(CPLX const &);		[conjugate]
 *  ComPlex::operator @ (CPLX , CPLX )	[ where @ = * + - ]
 */

/*
 * This class is used as follows:
 */
 //   #include <complex.h>			// WATCOM
 //   #include <complext.h>			// Gnu CC
 //   typedef complex<double> Complex;  //Gnu CC
 //   #include <math.h>
 //   #include <cfft.h>
 //   ...
 //   cfft<Complex>  FFT256( 256 ); // build an operator object
 //               // The constructor builds tables and places them
 //               // in the object.
 //   Complex Array[256];
 //   ...
 //   FFT256.fft( Array );		// forward transform
 //   FFT256.ifft( Array );     // reverse transform.
 //
/*
 * because this is a template class, it can be used on any
 * type with complex semantics. I originally created this class
 * for use with a 'fractional 24-bit' complex type, to study
 * rounding errors in DSP operations. If you look, you will find
 * ways to control the scaling within passes as well as in the
 * final pass. These are default parameters in the constructor.
 * There is no point in doing this in double-prec math,
 * but it makes a big difference in integer math.
 *
 * One final note: On error, the class throws a 'char const*' which
 * is an error message. This can only happen during construction.
 * The errors are
 *   - out of memory
 *   - size not a power of two.
 */
#ifndef PI
#define PI (3.1415926535897932384626433832795)
#endif

class altivec_cfft {

private:
   unsigned  int N, log2N;		// these define size of FFT buffer
    
	float *w;			// array [N/2] of cos/sin values
	float *w_orig;		// original buffer for w

	std::complex<float> *temp;
	float *temp_orig;
	
	float *buf_orig[16]; //a maximum of for fft work buffers per object
	unsigned int numOfBufs; //current number of buffers

	void moto_fft_wbuf_init(void);
	void moto_fft_descramble(void);

public:
    altivec_cfft( int size );		// size is power of 2

    ~altivec_cfft();

    void fft(std::complex<float> *buf, unsigned int max_bin, std::complex<float> *targetbuf = 0);		
    
    inline int length() const { return N; }

		std::complex<float> *GetFFTWorkBuffer(void);

}; // class cfft

void AltivecComplexAbsolute(std::complex<float> *aVector, size_t size);

#endif


