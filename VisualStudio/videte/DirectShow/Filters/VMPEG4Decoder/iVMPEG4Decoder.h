// iVMPEG4Decoder.h: interface for the CVMPEG4Decoder class
//
//////////////////////////////////////////////////////////////////////
// Classes defined:
//
//   GUIDs of Filter CVMPEG4Decoder, Interface IVMPEG4Decoder and 
//       its property page CVMPEG4DecoderProp
//   VMPEG4DecoderParams:
//       The parameters that control the CVMPEG4Decoder
//   IVMPEG4Decoder:
//       The interface that controls the CVMPEG4Decoder.
//////////////////////////////////////////////////////////////////////
#pragma once

// The parameter control the CVMPEG4Decoder.
struct VMPEG4DecoderParams {
    int    param1;
    double param2;
};

// {29ca76d4-09db-46b2-9b0c-feb767535127}
DEFINE_GUID(CLSID_VMPEG4Decoder, 
0x29ca76d4, 0x9db, 0x46b2, 0x9b, 0xc, 0xfe, 0xb7, 0x67, 0x53, 0x51, 0x27);

// {70332344-3dd6-4ed8-8d9c-ef01990a3152}
DEFINE_GUID(CLSID_VMPEG4DecoderProp, 
0x70332344, 0x3dd6, 0x4ed8, 0x8d, 0x9c, 0xef, 0x1, 0x99, 0xa, 0x31, 0x52);

// {48fe8edd-998d-4ba7-88cc-da7a0f8b77e6}
DEFINE_GUID(IID_IVMPEG4Decoder, 
0x48fe8edd, 0x998d, 0x4ba7, 0x88, 0xcc, 0xda, 0x7a, 0xf, 0x8b, 0x77, 0xe6);

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_INTERFACE_(IVMPEG4Decoder, IUnknown)
{
    // Get the parameters of the filter
    STDMETHOD(GetParams) (THIS_
                VMPEG4DecoderParams *irp
             ) PURE;

    // Set the parameters of the filter
    STDMETHOD(SetParams) (THIS_
                VMPEG4DecoderParams *irp
             ) PURE;
};

#ifdef __cplusplus
}
#endif

