extern "C" BOOL median(BYTE* PtrInput, int nHeight, int nWeidth, BYTE* PtrOutput);
extern "C" BOOL skeleton(BYTE* PtrInput, int nHeight, int nWeidth, BYTE* pTemp, BYTE* PtrOutput);
extern "C" BOOL corr_3x3(BYTE* PtrInput, int nHeight, int nWeidth, short int* pMask3x3, BYTE* PtrOutput);
extern "C" BOOL temporal(BYTE* FirstPtrInput, int nHeight, int nWeidth, BYTE* SecondPtrInput);

