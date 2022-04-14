

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Wed Jun 08 14:28:22 2011
 */
/* Compiler settings for .\RemoteCurveControl.idl:
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


#ifndef __RemoteCurveControlidl_h__
#define __RemoteCurveControlidl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DRemoteCurveControl_FWD_DEFINED__
#define ___DRemoteCurveControl_FWD_DEFINED__
typedef interface _DRemoteCurveControl _DRemoteCurveControl;
#endif 	/* ___DRemoteCurveControl_FWD_DEFINED__ */


#ifndef ___DRemoteCurveControlEvents_FWD_DEFINED__
#define ___DRemoteCurveControlEvents_FWD_DEFINED__
typedef interface _DRemoteCurveControlEvents _DRemoteCurveControlEvents;
#endif 	/* ___DRemoteCurveControlEvents_FWD_DEFINED__ */


#ifndef __RemoteCurveControl_FWD_DEFINED__
#define __RemoteCurveControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class RemoteCurveControl RemoteCurveControl;
#else
typedef struct RemoteCurveControl RemoteCurveControl;
#endif /* __cplusplus */

#endif 	/* __RemoteCurveControl_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 


#ifndef __RemoteCurveControlLib_LIBRARY_DEFINED__
#define __RemoteCurveControlLib_LIBRARY_DEFINED__

/* library RemoteCurveControlLib */
/* [control][helpstring][helpfile][version][uuid] */ 


EXTERN_C const IID LIBID_RemoteCurveControlLib;

#ifndef ___DRemoteCurveControl_DISPINTERFACE_DEFINED__
#define ___DRemoteCurveControl_DISPINTERFACE_DEFINED__

/* dispinterface _DRemoteCurveControl */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DRemoteCurveControl;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("558E0221-7485-41F5-B739-2095CAE64950")
    _DRemoteCurveControl : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DRemoteCurveControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DRemoteCurveControl * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DRemoteCurveControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DRemoteCurveControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DRemoteCurveControl * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DRemoteCurveControl * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DRemoteCurveControl * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DRemoteCurveControl * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DRemoteCurveControlVtbl;

    interface _DRemoteCurveControl
    {
        CONST_VTBL struct _DRemoteCurveControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DRemoteCurveControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _DRemoteCurveControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _DRemoteCurveControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _DRemoteCurveControl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _DRemoteCurveControl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _DRemoteCurveControl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _DRemoteCurveControl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DRemoteCurveControl_DISPINTERFACE_DEFINED__ */


#ifndef ___DRemoteCurveControlEvents_DISPINTERFACE_DEFINED__
#define ___DRemoteCurveControlEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DRemoteCurveControlEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DRemoteCurveControlEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("9377B6FC-4B40-4A3B-BAF1-DDB4A757B85C")
    _DRemoteCurveControlEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DRemoteCurveControlEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DRemoteCurveControlEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DRemoteCurveControlEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DRemoteCurveControlEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DRemoteCurveControlEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DRemoteCurveControlEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DRemoteCurveControlEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DRemoteCurveControlEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DRemoteCurveControlEventsVtbl;

    interface _DRemoteCurveControlEvents
    {
        CONST_VTBL struct _DRemoteCurveControlEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DRemoteCurveControlEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _DRemoteCurveControlEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _DRemoteCurveControlEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _DRemoteCurveControlEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _DRemoteCurveControlEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _DRemoteCurveControlEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _DRemoteCurveControlEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DRemoteCurveControlEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_RemoteCurveControl;

#ifdef __cplusplus

class DECLSPEC_UUID("3CC27841-69C4-477A-9439-E75EF77FCCF3")
RemoteCurveControl;
#endif
#endif /* __RemoteCurveControlLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


