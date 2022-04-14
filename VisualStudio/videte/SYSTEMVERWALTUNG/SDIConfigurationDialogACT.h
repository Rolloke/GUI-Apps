// SDIConfigurationDialogACT.h: interface for the CSDIConfigurationDialogACT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDICONFIGURATIONDIALOGACT_H__D020B802_EA48_49F6_AA07_E6CF5EA8BDDF__INCLUDED_)
#define AFX_SDICONFIGURATIONDIALOGACT_H__D020B802_EA48_49F6_AA07_E6CF5EA8BDDF__INCLUDED_

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
class CSDIConfigurationDialogACT : public CSDIConfigurationDialog  
{
public:
	CSDIConfigurationDialogACT(SDIControlType eType,
									int iCom,
									CSDIProtocolArray* pProtocolArray,
									CSDIAlarmTypeArray* pAlarmTypeArray,
									CComParameters* pComParameters,
									CSVView* pView);
	virtual ~CSDIConfigurationDialogACT();

// Overrides
public:
	virtual	BOOL	CanNewProtocol();
	virtual void	InitListControlSampleData();
	virtual	void	EnableControls();
	virtual CSDIConfigurationRS232*	CreateRS232();
	virtual int		InsertSampleData(CString sOneSample);
	virtual void	UpdateSampleDataString();
	virtual void	SetDefaultValues();
};

#endif // !defined(AFX_SDICONFIGURATIONDIALOGACT_H__D020B802_EA48_49F6_AA07_E6CF5EA8BDDF__INCLUDED_)
