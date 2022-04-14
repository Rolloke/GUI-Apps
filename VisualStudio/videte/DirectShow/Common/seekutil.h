//------------------------------------------------------------------------------
// File: SeekUtil.h
//
// Desc: DirectShow sample code - prototypes for seeking utility functions
//
// Copyright (c) 2000-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

//
// Constants
//
const int TICKLEN=100, TIMERID=55;

//
// Function prototypes
//
AFX_EXT_CLASS HRESULT ConfigureSeekbar(IMediaSeeking *pMS, CSliderCtrl Seekbar, CStatic& strPosition);

AFX_EXT_CLASS void StartSeekTimer();
AFX_EXT_CLASS void StopSeekTimer();
AFX_EXT_CLASS void UpdatePosition(IMediaSeeking *pMS, REFERENCE_TIME rtNow, CStatic& strPosition);
AFX_EXT_CLASS void ReadMediaPosition(IMediaSeeking *pMS, CSliderCtrl& Seekbar, CStatic& strPosition);

AFX_EXT_CLASS void HandleTrackbar(IMediaControl *pMC, IMediaSeeking *pMS, CSliderCtrl& Seekbar, CStatic& strPosition, WPARAM wReq);
