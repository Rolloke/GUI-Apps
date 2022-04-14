#pragma once

class CTestCIPCPerformanceDlg;

class CIPCPictureRecord;
class CIPCFields;
class CIPCExtraMemory;

CIPCExtraMemory* BubbleFromFieldsAndBubble(CIPC *pCipc, const CIPCFields& f, const CIPCExtraMemory* pMem);

class CIPCTestPerformance :	public CIPC
{
public:
	CIPCTestPerformance(LPCTSTR lpSMName, BOOL bMaster, CTestCIPCPerformanceDlg*pDlg);
	virtual ~CIPCTestPerformance();

#ifdef TEST_DV_STORAGE
	void DoRequestNewSavePicture(WORD wArchivNr, const CIPCPictureRecord &pict, const CIPCFields& fields);
	void OnConfirmNewSavePicture(WORD wArchivNr, CSecID camID);
#endif
#ifdef TEST_PERFORMANCE
	void DoRequestTransmitData(DWORD dwParam1, DWORD dwParam2, DWORD dwParam3, UINT nSize, const void*pData);
	void OnRequestTransmitData(DWORD dwParam1, DWORD dwParam2, DWORD dwParam3, UINT nSize, const void*pData);
	void DoConfirmTransmitData(DWORD dwErrorCode);
	void OnConfirmTransmitData(DWORD dwErrorCode);
#endif
	DWORD CalcCheckSum(UINT nSize, const void *pData);


protected:
	virtual void OnReadChannelFound();
	virtual void OnRequestConnection();
	virtual void OnConfirmConnection();

	virtual BOOL HandleCmd(DWORD dwCmd,
						   DWORD dwParam1, 
						   DWORD dwParam2,
						   DWORD dwParam3, 
						   DWORD dwParam4,
						   const CIPCExtraMemory *pExtraMem);

private:
	DWORD	m_dwTick;
	CTestCIPCPerformanceDlg *m_pDlg;
//	CIPCTestPerformance() {};
//	CIPCTestPerformance(CIPCTestPerformance &src) {};
	static unsigned long gm_CRC_Table[256];
};

#define CIPC_TEST_CMD_REQUEST_TRANSMIT_DATA 10
#define CIPC_TEST_CMD_CONFIRM_TRANSMIT_DATA 11
