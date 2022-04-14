// SDIConfigurationDialogNCR.h: interface for the CSDIConfigurationDialogNCR class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDICONFIGURATIONDIALOGNCR_H__E7764217_C7CA_11D2_B07A_004005A1D890__INCLUDED_)
#define AFX_SDICONFIGURATIONDIALOGNCR_H__E7764217_C7CA_11D2_B07A_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
#include "wkclasses\SDIControl.h"
#include "SDIConfigurationDialog.h"

/////////////////////////////////////////////////////////////////////////////
class CSDIProtocolArray;
class CSDIAlarmTypeArray;

class CSDIConfigurationDialogNCR : public CSDIConfigurationDialog  
{
public:
	CSDIConfigurationDialogNCR(SDIControlType eType,
								int iCom,
								CSDIProtocolArray* pProtocolArray,
								CSDIAlarmTypeArray* pAlarmTypeArray,
								CComParameters* pComParameters,
								CSVView* pView);
	virtual ~CSDIConfigurationDialogNCR();

	virtual void	InitListControlSampleData();
	virtual	void	EnableControls();
	virtual CSDIConfigurationRS232*	CreateRS232();
	virtual int		InsertSampleData(CString sOneSample);
	virtual void	UpdateSampleDataString();
	virtual void	SetDefaultValues();
	virtual	void	SetProtocolDefaultValues();
};

#endif // !defined(AFX_SDICONFIGURATIONDIALOGNCR_H__E7764217_C7CA_11D2_B07A_004005A1D890__INCLUDED_)
