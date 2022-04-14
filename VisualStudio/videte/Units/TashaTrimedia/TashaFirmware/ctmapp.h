/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: ctmapp.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/ctmapp.h $
// CHECKIN:		$Date: 11.12.01 9:23 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 11.12.01 9:12 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CTMAPP_H__
#define __CTMAPP_H__

#if defined(__cplusplus)
extern "C" {
#endif

void _psos_exit(int);
void root(void);
void _main(void);

class CCodec;
class CTmApp
{
public:
	CTmApp();
	virtual ~CTmApp();
	
	virtual bool InitInstance();
	virtual bool ExitInstance();
	virtual int  Run();

private:
	CCodec*	m_pCodec;
};

#if defined(__cplusplus)
}
#endif

#endif // __CTMAPP_H__
