#if !defined(AFX_ENUMERATIONS_H__40CB1D13_82C2_11D3_8DDD_004035A11E32__INCLUDED_)
#define AFX_ENUMERATIONS_H__40CB1D13_82C2_11D3_8DDD_004035A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////////////////
typedef enum
{				
	OemDTS		= 0,
	OemAlarmCom	= 1,
	OemDResearch= 2,
	OemSantec	= 3,
	OemPOne		= 4
} OEM;
////////////////////////////////////////////////////////////////////////////
typedef enum 
{
	ViewFull   = 0, 
	ViewXxX    = 1, 
	View1pX    = 2, 
} ViewMode;
////////////////////////////////////////////////////////////////////////////
typedef enum 
{
	VOUT_UNKNOWN	= -1, 
	VOUT_OFF		= 0, 
	VOUT_ACTIVITY	= 1, 
	VOUT_CLIENT		= 2,
	VOUT_SEQUENCE	= 3
} enumVideoOutputMode;

#endif // !defined(AFX_ENUMERATIONS_H__40CB1D13_82C2_11D3_8DDD_004035A11E32__INCLUDED_)
