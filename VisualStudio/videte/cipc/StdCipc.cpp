
#include "stdcipc.h"

// for runtime class identification
#include "CipcInputRecord.h"
IMPLEMENT_DYNAMIC(CIPCInputRecord, CObject)

#include "CipcOutputRecord.h"
IMPLEMENT_DYNAMIC(CIPCOutputRecord, CObject)

#include "CipcMediaRecord.h"
IMPLEMENT_DYNAMIC(CIPCMediaRecord, CObject)

#include "CipcArchivRecord.h"
IMPLEMENT_DYNAMIC(CIPCArchivRecord, CObject)

#include "CipcSequenceRecord.h"
IMPLEMENT_DYNAMIC(CIPCSequenceRecord, CObject)

#ifdef _WK_CLASSES_DLL
UINT  CWK_String::gm_uCodePage         = CP_ACP;
DWORD CWK_String::gm_dwConversionFlags = 0;
#endif
// #include "WK_New.inl"