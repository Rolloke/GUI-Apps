
#ifndef _header_20capi_h
#define _header_20capi_h

// Capi functions as given in the spec:
// using LoadLibrary(), these functions are found dynamically and
// a call via a function ptr is made
// GET and PUT are inlined!

typedef DWORD (APIENTRY *CAPI_REGISTER_PTR) (DWORD MessageBufferSize,DWORD maxLogicalConnection, 
							  DWORD maxBDataBlocks,DWORD maxBDataLen,DWORD * pApplID);

typedef DWORD (APIENTRY *CAPI_RELEASE_PTR) (DWORD ApplID);
typedef DWORD (APIENTRY *CAPI_PUT_MESSAGE_PTR) (DWORD ApplID,PVOID pCAPIMessage);

typedef DWORD (APIENTRY *CAPI_GET_MESSAGE_PTR) (DWORD ApplID,PVOID * ppCAPIMessage);

typedef DWORD (APIENTRY *CAPI_WAIT_FOR_SIGNAL_PTR) (DWORD ApplID);
typedef VOID (APIENTRY *CAPI_GET_MANUFACTURER_PTR) (char * SzBuffer);
typedef DWORD (APIENTRY *CAPI_GET_VERSION_PTR) (DWORD * pCAPIMajor,DWORD * pCAPIMinor,
								 DWORD * pManufacturerMajor,DWORD * pManufacturerMinor);
typedef DWORD (APIENTRY *CAPI_GET_SERIAL_NUMBER_PTR) (char * SzBuffer);
typedef DWORD (APIENTRY *CAPI_GET_PROFILE_PTR) (PVOID SzBuffer,DWORD CtrlNr );
typedef DWORD (APIENTRY *CAPI_INSTALLED_PTR) (VOID);

// pointers to functions
extern CAPI_REGISTER_PTR pCAPI_REGISTER;
extern CAPI_RELEASE_PTR pCAPI_RELEASE;
extern CAPI_PUT_MESSAGE_PTR pCAPI_PUT_MESSAGE;
extern CAPI_GET_MESSAGE_PTR pCAPI_GET_MESSAGE;
extern CAPI_WAIT_FOR_SIGNAL_PTR pCAPI_WAIT_FOR_SIGNAL;
extern CAPI_GET_MANUFACTURER_PTR pCAPI_GET_MANUFACTURER;
extern CAPI_GET_VERSION_PTR pCAPI_GET_VERSION;
extern CAPI_GET_SERIAL_NUMBER_PTR pCAPI_GET_SERIAL_NUMBER;
extern CAPI_GET_PROFILE_PTR pCAPI_GET_PROFILE;
extern CAPI_INSTALLED_PTR pCAPI_INSTALLED;

// the functions itself
extern DWORD	APIENTRY CAPI_REGISTER			(DWORD MessageBufferSize,DWORD maxLogicalConnection, 
												DWORD maxBDataBlocks,DWORD maxBDataLen,DWORD * pApplID);

extern DWORD	APIENTRY CAPI_RELEASE			(DWORD ApplID);
inline DWORD	APIENTRY CAPI_PUT_MESSAGE		(DWORD ApplID,PVOID pCAPIMessage);

inline DWORD	APIENTRY CAPI_GET_MESSAGE		(DWORD ApplID,PVOID * ppCAPIMessage);

extern DWORD	APIENTRY CAPI_WAIT_FOR_SIGNAL	(DWORD ApplID);
extern VOID		APIENTRY CAPI_GET_MANUFACTURER	(char * SzBuffer);
extern DWORD	APIENTRY CAPI_GET_VERSION		(DWORD * pCAPIMajor,DWORD * pCAPIMinor,
												DWORD * pManufacturerMajor,DWORD * pManufacturerMinor);
extern DWORD	APIENTRY CAPI_GET_SERIAL_NUMBER	(char * SzBuffer);
extern DWORD	APIENTRY CAPI_GET_PROFILE		(PVOID SzBuffer,DWORD CtrlNr );
extern DWORD	APIENTRY CAPI_INSTALLED			(VOID);

extern void DllError(LPCTSTR sError);

inline DWORD APIENTRY CAPI_PUT_MESSAGE (DWORD ApplID,PVOID pCAPIMessage)
{
	if (pCAPI_PUT_MESSAGE ) {
		return pCAPI_PUT_MESSAGE(ApplID,pCAPIMessage);
	} else {
		DllError(_T("CAPI_PUT_MESSAGE "));
		return 0xffff;
	}
}

inline DWORD APIENTRY CAPI_GET_MESSAGE (DWORD ApplID,PVOID * ppCAPIMessage)
{
	if (pCAPI_GET_MESSAGE ) {
		return pCAPI_GET_MESSAGE(ApplID,ppCAPIMessage);
	} else {
		DllError(_T("CAPI_GET_MESSAGE "));
		return 0xffff;
	}
}


#endif