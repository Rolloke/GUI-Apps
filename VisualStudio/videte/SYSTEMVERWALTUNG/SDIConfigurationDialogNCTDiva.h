// SDIConfigurationDialogNCTDiva.h: interface for the CSDIConfigurationDialogNCTDiva class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDICONFIGURATIONDIALOGNCTDIVA_H__96DF52A2_4CFE_483E_BDB6_BFE2B7855974__INCLUDED_)
#define AFX_SDICONFIGURATIONDIALOGNCTDIVA_H__96DF52A2_4CFE_483E_BDB6_BFE2B7855974__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SDIConfigurationDialog.h"

class CSDIConfigurationDialogNCTDiva : public CSDIConfigurationDialog  
{
public:
	CSDIConfigurationDialogNCTDiva(SDIControlType eType,
									int iCom,
									CSDIProtocolArray* pProtocolArray,
									CSDIAlarmTypeArray* pAlarmTypeArray,
									CComParameters* pComParameters,
									CSVView* pView);
	virtual ~CSDIConfigurationDialogNCTDiva();

// Overrides
public:
	virtual void	InitListControlSampleData();
	virtual	void	EnableControls();
	virtual CSDIConfigurationRS232*	CreateRS232();
	virtual int		InsertSampleData(CString sOneSample);
	virtual void	UpdateSampleDataString();
	virtual void	SetDefaultValues();
};

#endif // !defined(AFX_SDICONFIGURATIONDIALOGNCTDIVA_H__96DF52A2_4CFE_483E_BDB6_BFE2B7855974__INCLUDED_)
