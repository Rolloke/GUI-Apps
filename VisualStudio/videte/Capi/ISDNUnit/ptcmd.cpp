
#include "stdafx.h"

#include "pt.h"


CPTCmdCheckBoxPwd::CPTCmdCheckBoxPwd(
		const CString &sAccessPwd,
		const CString &sConfigPwd,
		WORD wHostVersion,
		WORD wHostCapabilities
		) : CPTCmd(9+9+2+2)
{
	BYTE *pData = m_pData;
	int i=0;
	// OOPS silent truncate
	*pData++ = (BYTE)min(8,sAccessPwd.GetLength());
	for (i=0;i<8 && i<sAccessPwd.GetLength();i++) 
	{
		*pData++ = (BYTE)sAccessPwd[i];
	}
	*pData++ = (BYTE)min(8,sConfigPwd.GetLength());
	for (i=0;i<8 && i<sConfigPwd.GetLength();i++) 
	{
		*pData++= (BYTE)sConfigPwd[i];
	}
	*pData++= BYTE (wHostVersion & 0xff);
	*pData++= BYTE ((wHostVersion>>8) & 0xff);
	*pData++= BYTE (wHostCapabilities& 0xff);
	*pData++= BYTE ((wHostCapabilities>>8) & 0xff);
}