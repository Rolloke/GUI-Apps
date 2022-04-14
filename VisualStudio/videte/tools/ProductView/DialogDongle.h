/* GlobalReplace: IPC_STORAGE_PROFESSIONAL --> IPC_STORAGE_PLUS */


#ifndef _CDialogDongle_H
#define _CDialogDongle_H

#include "WK_Dongle.h"

class CProductViewDlg;

class CDialogDongle : public CWK_Dongle
{
public:
	CDialogDongle();
	~CDialogDongle();
	//
	void TransferSettings(const CProductViewDlg *dlg);
	BOOL WriteToFile(const CString &sFilename) const;
	BOOL WriteToRegistry() const;
	BOOL WriteDCF(const CString &sFilename) const;
private:
	BOOL WriteEEP(const CString &sFilename) const;
	BOOL WriteHCC(const CString &sDir,const CString &sBaseName) const;
};

#endif