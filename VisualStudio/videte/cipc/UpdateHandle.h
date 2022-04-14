case INP_REQUEST_FILE_UPDATE:
	if (pExtraMem) 
	{
		int iDestination = (int)dwParam1;
		DWORD dwStringLen = dwParam2;
		DWORD dwDataLen = dwParam3;
		BOOL bNeedsReboot	= (BOOL)dwParam4;
		CWK_String sFileName;
#ifdef _UNICODE
		WORD wCodePage = pExtraMem->GetCIPC()->GetCodePage();
		BOOL bUnicode = wCodePage == CODEPAGE_UNICODE;
		sFileName.InitFromMemory(bUnicode, pExtraMem->GetAddress(), dwStringLen, wCodePage);
		TRACE(_T("INP_REQUEST_FILE_UPDATE:%s\n"), sFileName);
#else
		CopyMemory(sFileName.GetBufferSetLength(dwStringLen), pExtraMem->GetAddress(), dwStringLen);
		sFileName.ReleaseBuffer();
#endif
		sFileName.ReleaseBuffer();
//		WK_TRACE(_T("INP_REQUEST_FILE_UPDATE (%s)\n"), sFileName);
/*
		char *pTmp = new char[dwStringLen+1];
		CopyMemory(pTmp,pExtraMem->GetAddress(),dwStringLen);
		pTmp[dwStringLen]=0;
		sFileName = pTmp;
*/
		OnRequestFileUpdate(
			iDestination, 
			sFileName,
			(const void*)((BYTE*)pExtraMem->GetAddress()+dwStringLen),
			dwDataLen,
			bNeedsReboot
			);
//		WK_DELETE_ARRAY(pTmp);
	}
	else 
	{
		// NOT YET
	}
	break;
case INP_CONFIRM_FILE_UPDATE:
	if (pExtraMem) 
	{
		CString sFileName = pExtraMem->GetString();
		OnConfirmFileUpdate(sFileName);
	} else {
		// NOT YET
	}
	break;

