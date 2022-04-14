// SDIConfigurationDialogMicrolock.h: interface for the CSDIConfigurationDialogMicrolock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDICONFIGURATIONDIALOGMICROLOCK_H__E8DF9284_FA2D_4F9C_BA74_F60B952E0C53__INCLUDED_)
#define AFX_SDICONFIGURATIONDIALOGMICROLOCK_H__E8DF9284_FA2D_4F9C_BA74_F60B952E0C53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
#include "wkclasses\SDIControl.h"
#include "SDIConfigurationDialog.h"

/////////////////////////////////////////////////////////////////////////////
class CSDIProtocolArray;
class CSDIAlarmTypeArray;

/////////////////////////////////////////////////////////////////////////////
class CSDIConfigurationDialogMicrolock : public  CSDIConfigurationDialog  
{
public:
	CSDIConfigurationDialogMicrolock(SDIControlType eType,
									int iCom,
									CSDIProtocolArray* pProtocolArray,
									CSDIAlarmTypeArray* pAlarmTypeArray,
									CComParameters* pComParameters,
									CSVView* pView);
	virtual ~CSDIConfigurationDialogMicrolock();

// Overrides
public:
	virtual	BOOL	CanNewProtocol();
	virtual void	InitListControlSampleData();
	virtual	void	EnableControls();
	virtual CSDIConfigurationRS232*	CreateRS232();
	virtual int		InsertSampleData(CString sOneSample);
	virtual void	UpdateSampleDataString();
	virtual void	SetDefaultValues();

// Implementation
private:
	void	SetDefaultValuesSystem1x();
	void	SetDefaultValuesSystem1();

};

#endif // !defined(AFX_SDICONFIGURATIONDIALOGMICROLOCK_H__E8DF9284_FA2D_4F9C_BA74_F60B952E0C53__INCLUDED_)
