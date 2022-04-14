// SDIConfigurationDialogIBM.h: interface for the CSDIConfigurationDialogIBM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDICONFIGURATIONDIALOGIBM_H__E7764216_C7CA_11D2_B07A_004005A1D890__INCLUDED_)
#define AFX_SDICONFIGURATIONDIALOGIBM_H__E7764216_C7CA_11D2_B07A_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
#include "wkclasses\SDIControl.h"
#include "SDIConfigurationDialog.h"

/////////////////////////////////////////////////////////////////////////////
class CSDIProtocolArray;
class CSDIAlarmTypeArray;

class CSDIConfigurationDialogIBM : public CSDIConfigurationDialog  
{
public:
	CSDIConfigurationDialogIBM(SDIControlType eType,
								int iCom,
								CSDIProtocolArray* pProtocolArray,
								CSDIAlarmTypeArray* pAlarmTypeArray,
								CComParameters* pComParameters,
								CSVView* pView);
	virtual ~CSDIConfigurationDialogIBM();

// Overrides
public:
	virtual void	InitListControlSampleData();
	virtual	void	EnableControls();
	virtual CSDIConfigurationRS232*	CreateRS232();
	virtual int		InsertSampleData(CString sOneSample);
	virtual void	UpdateSampleDataString();
	virtual	void	SetProtocolDefaultValues();
	virtual	void	SetPortraitAlarm();
	virtual	void	SetMoneyAlarm();
};

#endif // !defined(AFX_SDICONFIGURATIONDIALOGIBM_H__E7764216_C7CA_11D2_B07A_004005A1D890__INCLUDED_)
