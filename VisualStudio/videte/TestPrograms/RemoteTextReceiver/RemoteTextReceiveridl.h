

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Sun May 24 21:07:34 2009
 */
/* Compiler settings for .\RemoteTextReceiver.idl:
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


#ifndef __RemoteTextReceiveridl_h__
#define __RemoteTextReceiveridl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DRemoteTextReceiver_FWD_DEFINED__
#define ___DRemoteTextReceiver_FWD_DEFINED__
typedef interface _DRemoteTextReceiver _DRemoteTextReceiver;
#endif 	/* ___DRemoteTextReceiver_FWD_DEFINED__ */


#ifndef ___DRemoteTextReceiverEvents_FWD_DEFINED__
#define ___DRemoteTextReceiverEvents_FWD_DEFINED__
typedef interface _DRemoteTextReceiverEvents _DRemoteTextReceiverEvents;
#endif 	/* ___DRemoteTextReceiverEvents_FWD_DEFINED__ */


#ifndef __RemoteTextReceiver_FWD_DEFINED__
#define __RemoteTextReceiver_FWD_DEFINED__

#ifdef __cplusplus
typedef class RemoteTextReceiver RemoteTextReceiver;
#else
typedef struct RemoteTextReceiver RemoteTextReceiver;
#endif /* __cplusplus */

#endif 	/* __RemoteTextReceiver_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 


#ifndef __RemoteTextReceiverLib_LIBRARY_DEFINED__
#define __RemoteTextReceiverLib_LIBRARY_DEFINED__

/* library RemoteTextReceiverLib */
/* [control][helpstring][helpfile][version][uuid] */ 


EXTERN_C const IID LIBID_RemoteTextReceiverLib;

#ifndef ___DRemoteTextReceiver_DISPINTERFACE_DEFINED__
#define ___DRemoteTextReceiver_DISPINTERFACE_DEFINED__

/* dispinterface _DRemoteTextReceiver */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DRemoteTextReceiver;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("884AE4FA-5170-48F7-8B90-7A605E62294C")
    _DRemoteTextReceiver : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DRemoteTextReceiverVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DRemoteTextReceiver * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DRemoteTextReceiver * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DRemoteTextReceiver * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DRemoteTextReceiver * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DRemoteTextReceiver * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DRemoteTextReceiver * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DRemoteTextReceiver * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DRemoteTextReceiverVtbl;

    interface _DRemoteTextReceiver
    {
        CONST_VTBL struct _DRemoteTextReceiverVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DRemoteTextReceiver_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _DRemoteTextReceiver_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _DRemoteTextReceiver_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _DRemoteTextReceiver_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _DRemoteTextReceiver_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _DRemoteTextReceiver_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _DRemoteTextReceiver_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DRemoteTextReceiver_DISPINTERFACE_DEFINED__ */


#ifndef ___DRemoteTextReceiverEvents_DISPINTERFACE_DEFINED__
#define ___DRemoteTextReceiverEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DRemoteTextReceiverEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DRemoteTextReceiverEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("426F7FB8-BE1C-4414-AE3C-3C681AF9FC88")
    _DRemoteTextReceiverEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DRemoteTextReceiverEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DRemoteTextReceiverEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DRemoteTextReceiverEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DRemoteTextReceiverEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DRemoteTextReceiverEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DRemoteTextReceiverEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DRemoteTextReceiverEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DRemoteTextReceiverEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DRemoteTextReceiverEventsVtbl;

    interface _DRemoteTextReceiverEvents
    {
        CONST_VTBL struct _DRemoteTextReceiverEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DRemoteTextReceiverEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _DRemoteTextReceiverEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _DRemoteTextReceiverEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _DRemoteTextReceiverEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _DRemoteTextReceiverEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _DRemoteTextReceiverEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _DRemoteTextReceiverEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DRemoteTextReceiverEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_RemoteTextReceiver;

#ifdef __cplusplus

class DECLSPEC_UUID("1B0F0BFD-8A71-4564-AD20-BDEB1CC9A796")
RemoteTextReceiver;
#endif
#endif /* __RemoteTextReceiverLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


