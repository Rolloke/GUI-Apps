/////////////////////////////////////////////////////////////////////////////
// PROJECT:		MiCoTst
// FILE:		$Workfile: ZoranIo.h $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/MiCoTst/ZoranIo.h $
// CHECKIN:		$Date: 5.08.98 10:03 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 5.08.98 10:02 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "zr050_io.h"

class ZoranIo : public Z050_IO       
{
public:
	ZoranIo(WORD wIOBASE);

    virtual BYTE Read050( WORD Address); 

    virtual void Write050( WORD Address, BYTE Data); 

    virtual BOOL Check050End( ); 

	WORD m_wIOBase;
};
