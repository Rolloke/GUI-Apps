#pragma once

//idip transmitter dongle
#define DONGLE_FLAG_BACKUP			0x00000001
#define DONGLE_FLAG_Q				0x00000002
#define DONGLE_FLAG_IP				0x00000004
#define DONGLE_FLAG_Q_CAM_4			0x00000008
#define DONGLE_FLAG_Q_CAM_8			0x00000010
#define DONGLE_FLAG_Q_CAM_16		0x00000020
#define DONGLE_FLAG_Q_CAM_24		0x00000040
#define DONGLE_FLAG_Q_CAM_32		0x00000080
#define DONGLE_FLAG_ISDN			0x00000100
#define DONGLE_FLAG_PTZ				0x00000200
#define DONGLE_FLAG_SERIAL_ALARM	0x00000400
#define DONGLE_FLAG_GAA				0x00000800
#define DONGLE_FLAG_ACCESS			0x00001000
#define DONGLE_FLAG_SCANNER			0x00002000
#define DONGLE_FLAG_PARK			0x00004000
#define DONGLE_FLAG_ICPCON			0x00008000
#define DONGLE_FLAG_HTTP			0x00010000
#define DONGLE_FLAG_IP_CAM_4		0x00020000
#define DONGLE_FLAG_IP_CAM_8		0x00040000
#define DONGLE_FLAG_IP_CAM_16		0x00080000
#define DONGLE_FLAG_IP_CAM_24		0x00100000
#define DONGLE_FLAG_IP_CAM_32		0x00200000

#define NR_OF_CAMERAS_4				4
#define NR_OF_CAMERAS_8				8
#define NR_OF_CAMERAS_16			16
#define NR_OF_CAMERAS_24			24
#define NR_OF_CAMERAS_32			32

// CExtensionCodeDlg dialog

class CExtensionCodeDlg : public CSkinDialog, public CWK_Dongle
{
	DECLARE_DYNAMIC(CExtensionCodeDlg)

public:
	CExtensionCodeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CExtensionCodeDlg();

// Dialog Data
	enum { IDD = IDD_EXTENSION_CODE};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL AddNewDongleFeatures();
	BOOL WriteTempDCF(const CString &sFilename) const;
	void SetNewDongleFeatures(DWORD dwDongle);
	DWORD CreateDongleChecksum(DWORD dwDongle);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnChangeEdtExtensionCode();
	virtual BOOL OnInitDialog();
};
