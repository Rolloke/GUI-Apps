
//new dongle feature
#define FLAG_TIMER		0x00000010
#define FLAG_PTZ		0x00000020
#define FLAG_MONITOR	0x00000040
#define FLAG_MAPS		0x00000080
#define FLAG_HOST0		0x00000400
#define FLAG_HOST5		0x00000100
#define FLAG_HOST10		0x00000200
#define FLAG_HOST20		0x00000800
#define FLAG_HOST30		0x00001000
#define FLAG_HOST40		0x00002000
#define FLAG_HOST50		0x00004000
#define FLAG_HOST60		0x00008000
#define FLAG_RESET		0x00010000

#include "WK_Dongle.h"

class CEulaDongle :	public CWK_Dongle
{
public:
	CEulaDongle(const CString &sFileName);
	~CEulaDongle(void);

	BOOL CEulaDongle::WriteDCF(const CString &sFilename, DWORD dwDongleDecode);
	void CheckDongleExpansions(DWORD dwDongleDecode);
};
