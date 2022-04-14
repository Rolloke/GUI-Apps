#include "..\Include\SpiDefs.h"

BOOL SelectBF533(int nBFNr);

BOOL Boot533(int nBFNr, BYTE* pBuffer, DWORD dwLen);
BOOL CreateBootDescriptorList(BYTE* pBuffer, DWORD dwLen);

BOOL WaitUntilSPITransferCompleted(int nTimeOut);

