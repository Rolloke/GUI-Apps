/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: cdebug.h $
// ARCHIVE:		$Archive: /Project/Units/SaVic/SavicDll/cdebug.h $
// CHECKIN:		$Date: 20.06.03 12:01 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 20.06.03 11:50 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CDEBUG_H__C69A3093_06C4_11D3_8D23_004005A11E32__INCLUDED_)
#define AFX_CDEBUG_H__C69A3093_06C4_11D3_8D23_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AFX_EXT_CLASS CDebug  
{
public:
	CDebug();
	virtual ~CDebug();

	virtual void OnReceivedMessage(const CString& sError) const;
	virtual void OnReceivedDiagnosticMessage(const CString& sError) const;
	virtual void OnReceivedWarning(const CString& sError) const;
	virtual void OnReceivedError(const CString& sError) const;
	virtual void OnReceivedStatLog(LPCTSTR szTopic, int iValue, LPCTSTR sName) const;
	virtual void OnReceivedStatPeak(LPCTSTR szTopic, int iValue, LPCTSTR sName) const;
};

#endif // !defined(AFX_CDEBUG_H__C69A3093_06C4_11D3_8D23_004005A11E32__INCLUDED_)
