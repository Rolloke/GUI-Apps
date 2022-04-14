

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __Deinterlace_h__
#define __Deinterlace_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IDeinterlace_FWD_DEFINED__
#define __IDeinterlace_FWD_DEFINED__
typedef interface IDeinterlace IDeinterlace;
#endif 	/* __IDeinterlace_FWD_DEFINED__ */


#ifndef __Deinterlace_FWD_DEFINED__
#define __Deinterlace_FWD_DEFINED__

#ifdef __cplusplus
typedef class Deinterlace Deinterlace;
#else
typedef struct Deinterlace Deinterlace;
#endif /* __cplusplus */

#endif 	/* __Deinterlace_FWD_DEFINED__ */


#ifndef __DeinterlacePropertyPage_FWD_DEFINED__
#define __DeinterlacePropertyPage_FWD_DEFINED__

#ifdef __cplusplus
typedef class DeinterlacePropertyPage DeinterlacePropertyPage;
#else
typedef struct DeinterlacePropertyPage DeinterlacePropertyPage;
#endif /* __cplusplus */

#endif 	/* __DeinterlacePropertyPage_FWD_DEFINED__ */


#ifndef __DeinterlaceAboutPage_FWD_DEFINED__
#define __DeinterlaceAboutPage_FWD_DEFINED__

#ifdef __cplusplus
typedef class DeinterlaceAboutPage DeinterlaceAboutPage;
#else
typedef struct DeinterlaceAboutPage DeinterlaceAboutPage;
#endif /* __cplusplus */

#endif 	/* __DeinterlaceAboutPage_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IDeinterlace_INTERFACE_DEFINED__
#define __IDeinterlace_INTERFACE_DEFINED__

/* interface IDeinterlace */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDeinterlace;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("463D645C-48F7-11d4-8464-0008C782A257")
    IDeinterlace : public IUnknown
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DeinterlaceType( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DeinterlaceType( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsOddFieldFirst( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_IsOddFieldFirst( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DScalerPluginName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DScalerPluginName( 
            /* [in] */ BSTR newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDeinterlaceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDeinterlace * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDeinterlace * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDeinterlace * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeinterlaceType )( 
            IDeinterlace * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DeinterlaceType )( 
            IDeinterlace * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsOddFieldFirst )( 
            IDeinterlace * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_IsOddFieldFirst )( 
            IDeinterlace * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DScalerPluginName )( 
            IDeinterlace * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DScalerPluginName )( 
            IDeinterlace * This,
            /* [in] */ BSTR newVal);
        
        END_INTERFACE
    } IDeinterlaceVtbl;

    interface IDeinterlace
    {
        CONST_VTBL struct IDeinterlaceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDeinterlace_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDeinterlace_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDeinterlace_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDeinterlace_get_DeinterlaceType(This,pVal)	\
    (This)->lpVtbl -> get_DeinterlaceType(This,pVal)

#define IDeinterlace_put_DeinterlaceType(This,newVal)	\
    (This)->lpVtbl -> put_DeinterlaceType(This,newVal)

#define IDeinterlace_get_IsOddFieldFirst(This,pVal)	\
    (This)->lpVtbl -> get_IsOddFieldFirst(This,pVal)

#define IDeinterlace_put_IsOddFieldFirst(This,newVal)	\
    (This)->lpVtbl -> put_IsOddFieldFirst(This,newVal)

#define IDeinterlace_get_DScalerPluginName(This,pVal)	\
    (This)->lpVtbl -> get_DScalerPluginName(This,pVal)

#define IDeinterlace_put_DScalerPluginName(This,newVal)	\
    (This)->lpVtbl -> put_DScalerPluginName(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IDeinterlace_get_DeinterlaceType_Proxy( 
    IDeinterlace * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IDeinterlace_get_DeinterlaceType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IDeinterlace_put_DeinterlaceType_Proxy( 
    IDeinterlace * This,
    /* [in] */ long newVal);


void __RPC_STUB IDeinterlace_put_DeinterlaceType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IDeinterlace_get_IsOddFieldFirst_Proxy( 
    IDeinterlace * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB IDeinterlace_get_IsOddFieldFirst_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IDeinterlace_put_IsOddFieldFirst_Proxy( 
    IDeinterlace * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IDeinterlace_put_IsOddFieldFirst_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IDeinterlace_get_DScalerPluginName_Proxy( 
    IDeinterlace * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IDeinterlace_get_DScalerPluginName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IDeinterlace_put_DScalerPluginName_Proxy( 
    IDeinterlace * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IDeinterlace_put_DScalerPluginName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDeinterlace_INTERFACE_DEFINED__ */



#ifndef __DeinterlaceLib_LIBRARY_DEFINED__
#define __DeinterlaceLib_LIBRARY_DEFINED__

/* library DeinterlaceLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_DeinterlaceLib;

EXTERN_C const CLSID CLSID_Deinterlace;

#ifdef __cplusplus

class DECLSPEC_UUID("463D645D-48F7-11d4-8464-0008C782A257")
Deinterlace;
#endif

EXTERN_C const CLSID CLSID_DeinterlacePropertyPage;

#ifdef __cplusplus

class DECLSPEC_UUID("463D645E-48F7-11d4-8464-0008C782A257")
DeinterlacePropertyPage;
#endif

EXTERN_C const CLSID CLSID_DeinterlaceAboutPage;

#ifdef __cplusplus

class DECLSPEC_UUID("B26BFF59-55BB-4a9e-94F9-56652554E42F")
DeinterlaceAboutPage;
#endif
#endif /* __DeinterlaceLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


