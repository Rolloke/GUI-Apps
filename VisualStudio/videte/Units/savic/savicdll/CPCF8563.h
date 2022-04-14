/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: Cpcf8563.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicDll/Cpcf8563.h $
// CHECKIN:		$Date: 27.06.02 15:03 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 27.06.02 8:01 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPCF8563_H__D292F173_38EC_11D3_8D76_004005A11E32__INCLUDED_)
#define AFX_CPCF8563_H__D292F173_38EC_11D3_8D76_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CI2C;
class CPCF8563  
{
public:
	CPCF8563(DWORD dwIOBase);
	virtual ~CPCF8563();

	virtual BOOL	SetTime(const CTime& Time);
	virtual BOOL	GetTime(CTime& Time) const;
	virtual BOOL	EnableCountdown();
	virtual BOOL	DisableCountdown();
	virtual BOOL	TriggerCountdown(WORD wTi);

protected:
	int		DecToBCD(int nDec) const;
	int		BCDToDec(int nBCD) const;

private:
	CCriticalSection m_csPCF8563;
	CI2C*		m_pCI2C;

};

#endif // !defined(AFX_CPCF8563_H__D292F173_38EC_11D3_8D76_004005A11E32__INCLUDED_)
