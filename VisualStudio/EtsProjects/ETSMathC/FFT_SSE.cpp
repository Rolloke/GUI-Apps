#include "Altivec_FFT.h"


#undef vector

#ifdef _DEBUG_HEAP
 #include   "DebugHeapPP.h"  // Memory leak finder
 #define new DLL_DEBUG_NEW
#endif

//////////////////////////////  cfft methods //////////////////////////////
altivec_cfft::altivec_cfft( int size )
{
    int k;

    //set variables
    for( k = 0; ; ++k )
    {
        if( (1<<k) == size) break;
        if( k==14 || (1<<k) >size ) throw ("fft size not a power of 2\n");
    }
    N = 1<<k;
    log2N = k;
    
    numOfBufs = 0; //no buffers at the beginning

    //get the w buffer
    w_orig = new float [N/2 + 0x10];
    w      = (float *)(((unsigned int)w_orig & 0xfffffff0) + 0x10);

    //get a temp buffer
    temp_orig = new float [N*2 + 0x10];
    temp      = (std::complex<float> *)(((unsigned int)temp_orig & 0xfffffff0) + 0x10);
    
    moto_fft_wbuf_init();
}


/*
 * destructor frees the memory
 */
altivec_cfft::~altivec_cfft()
{
    delete [] w_orig;

    delete [] temp_orig;
    
    for (unsigned int i=0;i<numOfBufs;i++)
        delete [] buf_orig[i];
}


std::complex<float> *
altivec_cfft::GetFFTWorkBuffer(void)
{
    if (numOfBufs >= 16) return NULL;
    
    buf_orig[numOfBufs] = new float [N*2 + 0x10];

    numOfBufs++;

    return (std::complex<float> *)(((unsigned int)buf_orig[numOfBufs-1] & 0xfffffff0) + 0x10);
}


void 
altivec_cfft::moto_fft_wbuf_init(void)
//    this computes the contents of the wbuf
{
    float angle;
    float curSin;
    float curCos;
    unsigned int i;
    
    for(i=0; i < N/4; i++) 
    {
        angle = -2.0 * PI * i/N;

        curCos = cos(angle);
        curSin = sin(angle);
        
        w[2*i] = curCos;
        w[2*i+1] = curSin;
    }
}

void
altivec_cfft::moto_fft_descramble(void)
{
    unsigned int i;
    unsigned int ir=0;
    unsigned int n2=N/2;
    unsigned int mask;
    double * dd=( double *)temp;
    for (i=0;i<N;i++) {
        if (i>0) {
            /* Increment ir */
            mask=n2;
            while (ir >= mask) {
                ir -= mask;
                mask>>=1;
            }
            ir+=mask;
        }
        if (i<ir) {
            /* Swap... Trick - move both floats as a single double */
            double tmp=dd[i];
            dd[i]=dd[ir];
            dd[ir]=tmp;
        }
    }
}

/*
 * cfft::fft_func(buf) performs a forward fft on the data in the buffer specified exp(-j*phi).
 * cfft::ifft_func(buf) performs an inverse fft on the data in the buffer specified exp(+j*phi).
 */
void
altivec_cfft::fft( std::complex<float> *buf , unsigned int max_bin, std::complex<float> *targetbuf)
{
    __vector float* pvi = (__vector float*) buf;
    __vector float* pvf = (__vector float*) temp;
    __vector float* pw  = (__vector float*) w;
    //local constants
    __vector float vcfzero = Altivec_Const4(__vector float, 0., 0., 0., 0.);
    __vector float vcfnegeven = Altivec_Const4(__vector float, -0., 0.,-0., 0.);
    __vector float vcfnegodd = Altivec_Const4(__vector float, 0.,-0., 0.,-0.);
    __vector float vcppnn = Altivec_Const4(__vector float, 1., 1.,-1.,-1.);
    __vector float vcpnnp = Altivec_Const4(__vector float, 1.,-1.,-1., 1.);
    //SINGLE __vector PERM FORMS (V=[0,1,2,3])
    __vector unsigned char vcprm1032 =
    Altivec_Const16(__vector unsigned char, 4,5,6,7, 0,1,2,3, 12,13,14,15, 8,9,10,11 );
    __vector unsigned char vcprm0022 =
    Altivec_Const16(__vector unsigned char, 0,1,2,3, 0,1,2,3, 8,9,10,11, 8,9,10,11 );
    __vector unsigned char vcprm1133 =
    Altivec_Const16(__vector unsigned char, 4,5,6,7, 4,5,6,7, 12,13,14,15, 12,13,14,15 );
    __vector unsigned char vcprm2301 =
    Altivec_Const16(__vector unsigned char, 8,9,10,11,12,13,14,15, 0,1,2,3,4,5,6,7 );
    __vector unsigned char vcprm0101 =
    Altivec_Const16(__vector unsigned char, 0,1,2,3,4,5,6,7, 0,1,2,3,4,5,6,7 );
    __vector unsigned char vcprm3232 =
    Altivec_Const16(__vector unsigned char, 12,13,14,15,8,9,10,11, 12,13,14,15,8,9,10,11 );
    //DOUBLE __vector PERM FORMS (V1,V2=[ar,ai,x,x],[br,bi,x,x] x=dontcare)
    __vector unsigned char vcaiarbibr =
    Altivec_Const16(__vector unsigned char, 4,5,6,7, 0,1,2,3, 20,21,22,23, 16,17,18,19 );
    __vector unsigned char vcaraibrbi =
    Altivec_Const16(__vector unsigned char, 0,1,2,3,4,5,6,7, 16,17,18,19,20,21,22,23 );
    //__vector LOCALS
    __vector float vtf10,vtf11,vtf12,vtf13,vtf14,vtf15,vtf20,vtf21,vtf22,vtf23,vtf24,vtf25,
    vtf31,vtf32,vtf33,vtf34,vtf35,vtf41,vtf42,vtf43,vtf44,vtf45,vtfw0,vtfw1,vtfw2,vtfw3;
    //ITERATORS&INDICES
    unsigned int pa,pb,qa,qb,stride,edirts,iw;//indices
    unsigned int block,j;//iterators
    //BEGIN DIF FFT
    //FIRST STAGE
    stride = N/2; /* in stage 0 stride = n/2 */
    //special case since twiddles are (w1r,w1i,w2r,w2i) in same vec
    //and only one block
    for( j=0; j<stride/4; j++ ) {
        //PREP THE TWIDDLES
        vtfw1 = vec_perm( pw[j], pw[j], vcprm1032 );
        vtfw2 = vec_xor( vtfw1, vcfnegeven );//(-i, r)
        vtfw3 = vec_xor( vtfw1, vcfnegodd );//( i,-r)
        //BUTTERFLIES
        //upper two
        vtf10 = vec_add( pvi[j], pvi[N/4+j] );//PA+QA
        vtf11 = vec_sub( pvi[j], pvi[N/4+j] );//PA-QA
        pvf[j] = vtf10;
        vtf20 = vec_add( pvi[N/8+j], pvi[N/8+N/4+j] );//PB+QB
        vtf21 = vec_sub( pvi[N/8+j], pvi[N/8+N/4+j] );//PB-QB
        pvf[N/8+j] = vtf20;
        //lower two (apply twiddle)
        vtf12 = vec_perm( vtf11, vtf11, vcprm0022 );
        vtf12 = vec_madd( vtf12, pw[j], vcfzero );
        vtf13 = vec_perm( vtf11, vtf11, vcprm1133 );
        pvf[N/4+j] = vec_madd( vtfw2, vtf13, vtf12 );
        vtf22 = vec_perm( vtf21, vtf21, vcprm1133 ); //imag
        vtf22 = vec_madd( vtf22, pw[j], vcfzero ); //w.imag
        vtf23 = vec_perm( vtf21, vtf21, vcprm0022 ); //real
        pvf[N/4+N/8+j] = vec_madd( vtfw3, vtf23, vtf22 );//w.real
    }
    //END FIRST STAGE
    stride = N/4; /* in stage 1 stride = n/4 */
    //STAGES iterate over remaining, less the last two
    edirts = 1;
    while (stride>=4) {
        //BLOCKS iterate for the number of blocks
        for( block=0; block<N/2; block+=stride ) {
            pa = block;
            pb = block + stride/4;
            qa = block + stride/2;
            qb = block + stride/4 + stride/2;
            iw = 0;
            //itr(stride) of the block, /4 since 2 bufflies/vec and 2vecs/loop
            for( j=0; j<stride/4; j++ ) {
                //PREP THE TWIDDLES
                vtfw0 = vec_perm( pw[j*2*edirts],pw[j*2*edirts+edirts],vcaraibrbi );
                vtfw1 = vec_perm( pw[j*2*edirts],pw[j*2*edirts+edirts],vcaiarbibr );//swap r,i
                vtfw2 = vec_xor( vtfw1, vcfnegeven );//(-i, r)
                vtfw3 = vec_xor( vtfw1, vcfnegodd );//( i,-r)
                //BUTTERFLIES
                //upper two
                vtf10 = vec_add( pvf[pa+j], pvf[qa+j] );//PA+QA
                vtf11 = vec_sub( pvf[pa+j], pvf[qa+j] );//PA-QA
                pvf[pa+j] = vtf10;
                vtf20 = vec_add( pvf[pb+j], pvf[qb+j] );//PB+QB
                vtf21 = vec_sub( pvf[pb+j], pvf[qb+j] );//PB-QB
                pvf[pb+j] = vtf20;
                //lower two (apply twiddle)
                vtf12 = vec_perm( vtf11, vtf11, vcprm0022 );
                vtf12 = vec_madd( vtf12, vtfw0, vcfzero );
                vtf13 = vec_perm( vtf11, vtf11, vcprm1133 );
                pvf[qa+j] = vec_madd( vtfw2, vtf13, vtf12 );
                vtf22 = vec_perm( vtf21, vtf21, vcprm1133 );//imag
                vtf22 = vec_madd( vtf22, vtfw0, vcfzero ); //w.imag
                vtf23 = vec_perm( vtf21, vtf21, vcprm0022 ); //real
                pvf[qb+j] = vec_madd( vtfw3, vtf23, vtf22 ); //w.real
            }
            //end BUTTERFLIES
        }
        //stride halves, block count (twiddle separation) doubles
        stride = stride >> 1;
        edirts = edirts << 1;
    }
    //end STAGES (third-to-last stage, actually)
    //Last two stages (quad unrolled; forces n>=16)
    for( block=0; block<N/2; block+=8 ) { //(n/2 since 1Vec=2pts)
        vtf13 = vec_sub( pvf[block ], pvf[block+1] );//PB1
        vtf11 = vec_add( pvf[block ], pvf[block+1] );//PA1
        vtf23 = vec_sub( pvf[block+2], pvf[block+3] );//QB1
        vtf21 = vec_add( pvf[block+2], pvf[block+3] );//QA1
        vtf33 = vec_sub( pvf[block+4], pvf[block+5] );//PB1
        vtf31 = vec_add( pvf[block+4], pvf[block+5] );//PA1
        vtf43 = vec_sub( pvf[block+6], pvf[block+7] );//QB1
        vtf41 = vec_add( pvf[block+6], pvf[block+7] );//QA1
        vtf15 = vec_perm( vtf13, vtf13, vcprm3232 );//PB2b
        vtf14 = vec_perm( vtf13, vtf13, vcprm0101 );//PB2a
        vtf12 = vec_perm( vtf11, vtf11, vcprm2301 );//PA2
        vtf25 = vec_perm( vtf23, vtf23, vcprm3232 );//QB2b
        vtf24 = vec_perm( vtf23, vtf23, vcprm0101 );//QB2a
        vtf22 = vec_perm( vtf21, vtf21, vcprm2301 );//QA2
        vtf35 = vec_perm( vtf33, vtf33, vcprm3232 );//PB2b
        vtf34 = vec_perm( vtf33, vtf33, vcprm0101 );//PB2a
        vtf32 = vec_perm( vtf31, vtf31, vcprm2301 );//PA2
        vtf45 = vec_perm( vtf43, vtf43, vcprm3232 );//QB2b
        vtf44 = vec_perm( vtf43, vtf43, vcprm0101 );//QB2a
        vtf42 = vec_perm( vtf41, vtf41, vcprm2301 );//QA2
        pvf[block] = vec_madd( vtf11, vcppnn, vtf12 );//PA3
        pvf[block+1] = vec_madd( vtf15, vcpnnp, vtf14 );//PB3
        pvf[block+2] = vec_madd( vtf21, vcppnn, vtf22 );//QA3
        pvf[block+3] = vec_madd( vtf25, vcpnnp, vtf24 );//QB3
        pvf[block+4] = vec_madd( vtf31, vcppnn, vtf32 );//PA3
        pvf[block+5] = vec_madd( vtf35, vcpnnp, vtf34 );//PB3
        pvf[block+6] = vec_madd( vtf41, vcppnn, vtf42 );//QA3
        pvf[block+7] = vec_madd( vtf45, vcpnnp, vtf44 );//QB3
    }
    
    moto_fft_descramble();

    //if no target buffer was passed -> overwrite the in buffer
    if (!targetbuf) targetbuf = buf;

    //rearrange and set the last one to zero
    //this uses the passed value max_bin as max valid fft k-value
    memcpy((char *)targetbuf,(char *)&temp[N-max_bin],(max_bin)*sizeof(std::complex<float>));
    memcpy((char *)&targetbuf[max_bin],(char *)temp,(max_bin+1)*sizeof(std::complex<float>));
    memset((char *)&targetbuf[2*max_bin+1],0, (N-(2*max_bin+1))*sizeof(std::complex<float>));
}

/*
    rearrange and set the last to zero
    max_bin wird in C++ Konvention übergeben
    in C++ : 0                 -> max_bin          --->    max_bin     -> 2*max_bin
             (fftSize-max_bin) -> (fftSize-1)    --->    0           -> max_bin-1
                                                                                                    2*max_bin+1 -> fftSize-1 ---> auf 0

    beams(1,((0)+1):((maxbin-1)+1)) = beam(((sizeFFT-maxbin)+1):((sizeFFT-1)+1)); // k = max_bin -> 1
    beams(1,((maxbin)+1):((2*maxbin)+1)) = beam(((0)+1):((maxbin)+1));
    beams(1,((2*maxbin+1)+1):((sizeFFT-1)+1)) = 1;
*/

void AltivecComplexAbsolute(std::complex<float> *aVector, size_t size)
{ 
    float test1[4];
    __vector float* pvo1 = (__vector float*) test1;
    __vector float* pvi1;
    __vector float  realvalues;
    __vector float  imagvalues;
    __vector unsigned char realbytes =
    Altivec_Const16(__vector unsigned char, 0,1,2,3,  8,9,10,11, 16,17,18,19, 24,25,26,27 );
    __vector unsigned char imagbytes =
    Altivec_Const16(__vector unsigned char, 4,5,6,7,  12,13,14,15,  20,21,22,23, 28,29,30,31 );
 
    size_t i, j, k;
    for (i=0; i<size; i+=4)
    {
        pvi1 = (__vector float*) &aVector[i];
        realvalues = vec_perm( pvi1[0], pvi1[1], realbytes);
        imagvalues = vec_perm( pvi1[0], pvi1[1], imagbytes);
        {
            __vector float  sum = Altivec_Const4(__vector float, 0., 0., 0., 0.);
            sum = vec_madd(realvalues, realvalues, sum); 
            sum = vec_madd(imagvalues, imagvalues, sum);
            sum = vec_rsqrte(sum);
            pvo1[0] = vec_re(sum); 
        }
        for (j=i,k=0; k<4; j++,k++) 
        {
            aVector[j] = std::complex<float>(test1[k], 0);
        }
    }
}
