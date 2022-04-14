// SDIConfigurationDialogRondo.h: interface for the CSDIConfigurationDialogRondo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDICONFIGURATIONDIALOGRONDO_H__AB98AC66_5EA2_11D3_BA71_00400531137E__INCLUDED_)
#define AFX_SDICONFIGURATIONDIALOGRONDO_H__AB98AC66_5EA2_11D3_BA71_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SDIConfigurationDialog.h"

class CSDIConfigurationDialogRondo : public CSDIConfigurationDialog  
{
public:
	CSDIConfigurationDialogRondo(SDIControlType eType,
									int iCom,
									CSDIProtocolArray* pProtocolArray,
									CSDIAlarmTypeArray* pAlarmTypeArray,
									CComParameters* pComParameters,
									CSVPage* pParent);
	virtual ~CSDIConfigurationDialogRondo();

	virtual void	InitListControlSampleData();
	virtual	void	EnableControls();
	virtual CSDIConfigurationRS232*	CreateRS232();
	virtual int		InsertSampleData(CString sOneSample);
	virtual void	UpdateSampleDataString();
	virtual void	SetDefaultValues();
	virtual	void	SetSpecialProtocolDefaultValues(CSDIProtocol* pProtocol);
	virtual	void	SetPortraitAlarm();
	virtual	void	SetMoneyAlarm();
};

#endif // !defined(AFX_SDICONFIGURATIONDIALOGRONDO_H__AB98AC66_5EA2_11D3_BA71_00400531137E__INCLUDED_)
