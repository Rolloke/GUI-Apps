// CMySaVic.h: interface for the CMySaVic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMYSAVIC_H__1B614B95_FB9A_11D2_8D11_004005A11E32__INCLUDED_)
#define AFX_CMYSAVIC_H__1B614B95_FB9A_11D2_8D11_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSaVicUnitDlg;
class CPCIDevice;
class CMySaVic : public CCodec  
{
public:
	CMySaVic(CSaVicUnitDlg* pMainWnd, const CString &sAppIniName, CPCIDevice* pDevice);

	virtual void OnReceivedImageData(const IMAGE* pImage) const;
	virtual void OnReceivedEncoderError(int nErrorCode) const;	
	virtual void OnReceivedAlarm(WORD wCurrentAlarmState) const;
	virtual void OnReceivedCameraStatus(WORD wSource, BOOL bStatus) const;
	virtual void OnReceivedMotionAlarm(WORD wSource, CMDPoints& Points) const;
	virtual void OnReceivedRunTimeError(WK_RunTimeErrorLevel Level, WK_RunTimeError RTError, const CString &sError, DWORD dwParam1, DWORD dwParam2, WK_RunTimeErrorSubcode errorSubcode) const;
	virtual void OnPowerOffButtonPressed() const;
	virtual void OnResetButtonPressed() const;
	virtual void OnReceivedMotionDIB(WORD wSource, HANDLE hDIB);

private:
	CSaVicUnitDlg *m_pMainWnd;
};

#endif // !defined(AFX_CMYSAVIC_H__1B614B95_FB9A_11D2_8D11_004005A11E32__INCLUDED_)
