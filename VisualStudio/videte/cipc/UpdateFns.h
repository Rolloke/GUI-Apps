#ifndef CLASSX
error
#else
// file update:
// Destination: 0=AppDir, 1=Windows, 2=Windows/System, 3=Root
//				4=use sFileName
void CLASSX::DoRequestFileUpdate(int iDestination,
								 const CString &sFileName,
								 const void *pData,
								 DWORD dwDataLen,
								 BOOL bNeedsReboot)
{
	DWORD dwStringDataLength = 0;
#ifdef _UNICODE
	CWK_String swk(sFileName);
	BOOL bUnicode = (m_wCodePage == CODEPAGE_UNICODE) ? TRUE : FALSE;
	dwStringDataLength = swk.CopyToMemory(bUnicode, NULL, -1, m_wCodePage);
	DWORD dwSum = dwStringDataLength+dwDataLen;
	BYTE *pTmp = new BYTE[ dwSum ];
	BYTE *pTmpX = pTmp;
	pTmpX += swk.CopyToMemory(bUnicode, pTmpX, -1, m_wCodePage);
	CopyMemory(pTmpX, pData, dwDataLen);
#else
	dwStringDataLength = sFileName.GetLength();
	DWORD dwSum = dwStringDataLength+dwDataLen;
	BYTE *pTmp = new BYTE[ dwSum ];
	CopyMemory(pTmp, sFileName, dwStringDataLength);
	CopyMemory(pTmp+sFileName.GetLength(), pData, dwDataLen);
#endif	

	CIPCExtraMemory *pBubble = new CIPCExtraMemory();
	if (pBubble->Create(this, dwSum, pTmp))
	{
		WriteCmdWithExtraMemory(pBubble,
								INP_REQUEST_FILE_UPDATE,
								iDestination,	// dw1
								dwStringDataLength,	// dw2
								dwDataLen,	// dw3
								bNeedsReboot	// dw4
								);
	}
	else
	{
		WK_DELETE(pBubble);
		WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in DoRequestFileUpdate\n"));
	}

	WK_DELETE_ARRAY(pTmp);
}
void CLASSX::OnRequestFileUpdate(
				int iDestination,
				const CString &sFileName,
				const void *pData,
				DWORD dwDataLen,
				BOOL bNeedsReboot
				)
{
	OverrideError();
}
void CLASSX::DoConfirmFileUpdate(const CString &sFileName)
{
	CIPCExtraMemory *pBubble = new CIPCExtraMemory();
	
	if (pBubble->Create(this,sFileName))
	{
		WriteCmdWithExtraMemory(pBubble,INP_CONFIRM_FILE_UPDATE);
	}
	else
	{
		WK_DELETE(pBubble);
		WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in DoConfirmFileUpdate\n"));
	}
}

void CLASSX::OnConfirmFileUpdate(const CString &sFileName)
{
	OverrideError();
}
#endif
