

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Fri Mar 24 15:48:01 2006
 */
/* Compiler settings for .\Deinterlace.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IDeinterlace,0x463D645C,0x48F7,0x11d4,0x84,0x64,0x00,0x08,0xC7,0x82,0xA2,0x57);


MIDL_DEFINE_GUID(IID, LIBID_DeinterlaceLib,0x7B08AC97,0x2997,0x4D9D,0xBE,0x7B,0x8D,0xB8,0x76,0x94,0x4E,0x54);


MIDL_DEFINE_GUID(CLSID, CLSID_Deinterlace,0x463D645D,0x48F7,0x11d4,0x84,0x64,0x00,0x08,0xC7,0x82,0xA2,0x57);


MIDL_DEFINE_GUID(CLSID, CLSID_DeinterlacePropertyPage,0x463D645E,0x48F7,0x11d4,0x84,0x64,0x00,0x08,0xC7,0x82,0xA2,0x57);


MIDL_DEFINE_GUID(CLSID, CLSID_DeinterlaceAboutPage,0xB26BFF59,0x55BB,0x4a9e,0x94,0xF9,0x56,0x65,0x25,0x54,0xE4,0x2F);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

