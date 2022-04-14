///////////////////////////////////////////////////////////////
// FILE   : $Workfile: CIPCInputTester.h $ 
// ARCHIV : $Archive: /Project/Tools/Tester/CIPCInputTester.h $
// DATE   : $Date: 8.04.97 17:59 $ $Revision: 11 $
// VERSION: $Revision: 11 $
// HISTORY:	19.03.1997
// LAST   : $Modtime: 8.04.97 17:35 $
// AUTHOR : $Author: Hajo $
//        : $Nokeywords:$

/**headerRef***************************************************
 * header  : CIPCInputTester.h
 *         :
 * author  : Hajo Fierke / HeDu 1997 w+k
 * history : 19.03.1997
 *         : 01.04.1997	hajo
 **************************************************************/
#ifndef CIPCINPUTTESTER_H
	#define CIPCINPUTTESTER_H

#include "TickCount.h"
#include "CIPCInput.h"

class CIPCInputTester : public CIPCInput
{
public:
	CIPCInputTester(const char *szShmName);
	virtual ~CIPCInputTester();
	//                                                                                                        
	virtual void  OnReadChannelFound();
	virtual void  OnConfirmConnection();
	virtual void  OnRequestDisconnect();
	virtual void  DoMyRequestGetFile();
	virtual void  OnConfirmGetFile(
				  int   iDestination,
				  const CString &sFileName,
				  const void *pData,
				  DWORD dwDataLen	);
			void  DoMyRequestFileUpdate();
	virtual void  OnConfirmFileUpdate(const CString &sFileName);
			void  DoMyDeleteFile();
			void  Init(void);
protected:
	int			 m_iDestination;	
	CString		 m_sTestFileName;
	CTickCounter m_TickCounter[21]; // der schnelle hack
	BYTE*		 m_pData;
	int			 m_iSizeofpData;
	int			 m_iLoops;
};

#endif