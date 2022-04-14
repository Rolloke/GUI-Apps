#ifndef __demultiplex_h__
#define __demultiplex_h__

BOOL UniversalDemultiplex(BYTE* pDst, BYTE* pSrc, DWORD dwBufferSize, int nTimeSliceSize, int nChannels);
BOOL Demultiplex4Channels16Bits(WORD* pDstBase, WORD* pSrcBase, DWORD dwBufferSize);
BOOL Demultiplex8Channels16Bits(WORD* pDstBase, WORD* pSrcBase, DWORD dwBufferSize);
BOOL Demultiplex4Channels32Bits(DWORD* pDstBase, DWORD* pSrcBase, DWORD dwBufferSize);
BOOL Demultiplex8Channels32Bits(DWORD* pDstBase, DWORD* pSrcBase, DWORD dwBufferSize);

#endif // __demultiplex_h__
