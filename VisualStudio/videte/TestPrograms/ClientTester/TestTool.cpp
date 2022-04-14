/////////////////////////////////////////////////////////////
// FILE   : $Workfile: TestTool.cpp $ 
// ARCHIVE: $Archive: /Project/Tools/ClientTester/TestTool.cpp $
// DATE   :	$Date: 13.06.97 12:28 $ 
// VERSION: $Revision: 6 $
// LAST   : $Modtime: 13.06.97 11:57 $ 
// AUTHOR : $Author: Hedu $
//        : $Nokeywords:$

/**modulref **************************************************
 * name    : TestTool.cpp
 * class   : CTestTool
 * derived : CWinApp
 * members :
 * public  :		
 *         : void		SplitArgs(const CString&);
 *         : CString	ParseOptions(CStringArray&);
 *         : CString	FindHostNumberByName(const CString&);
 *         :
 * public  : implemented in "TestTool.h":
 * access  : inline BOOL	GetDoVerify() const;
 *         : inline CString	GetTestFileName() const;
 *         : inline int		GetDestination() const;
 *         : inline int		GetSizeofpData() const;
 *         : inline int		GetLoop() const;
 *         : inline int		GetDoDelete() const; 
 *         : 	//new ones for coconut
 *         : inline	CString		GetShmName() const;
 *         : inline	Compression	GetCompression() const;
 *         : inline	Resolution	GetResolution() const;
 *         : inline	WORD		GetNumPictures() const;
 *         : inline	DWORD		GetBitrateScale() const;
 *         : inline	DWORD		GetUserData() const;
 *         : inline	BOOL		GetIsServer() const;
 * private :
 * data    : CString		  m_szHost;			
 *         : CString		  m_szUserDummy;
 *         : CString		  m_sTestFileName;	
 *         : int			  m_iDestination;	
 *         : int			  m_iSizeofpData;	
 *         : int			  m_iLoop;			
 *         : CIPCInputTester* m_pInput;
 *         : CStringArray	  m_lpCmdLineArray;
 *         : BOOL			  m_bDoVerify;
 *         : BOOL			  m_bDoDelete;
 *         : CString		  m_shmName;
 *         : 	//new ones for coconut
 *         : Compression	  m_Compression;
 *         : Resolution		  m_Resolution;
 *         : WORD			  m_wNumPictures;
 *         : DWORD			  m_dwBitrateScale;
 *         : BOOL			  m_bIsServer;
 *         : 
 * purpose : requests numpic (200) coconut pictures
 *         : throughout the ISDN unit
 * see     : ParseOptions for valid comand line parameters 
 *         :
 * output  : c:\log\ClientPicTester.log  
 *         : may be followed by SecAnalyser
 *		   :
 * author  : Hajo Fierke 1997 w+k
 * history : 09.04.1997
 *         : 23.05.1997	self tcp: check added in FindHost...
 *************************************************************/
#include "stdafx.h"
#include "TestTool.h"

#include "wk.h"
#include "CIPCServerControlClientSide.h"
#include "User.h"
#include "host.h"
#include "WK_Names.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**funcref ***************************************************
 * class   : CTestTool 
 * name    : CTestTool();
 * input   : nil
 * output  : nil
 *         :
 * purpose : constructor
 * uses    : nil
 *         :
 * author  : Hajo Fierke  1997 w+k
 * history : 09.04.1997
 *         : 09.04.1997	self
 *************************************************************/
/*
@MD
*/
CTestTool::CTestTool()
{
	m_bDoDelete		= FALSE;
	m_bDoVerify		= FALSE;
	m_iDestination	= 4;	
	m_iSizeofpData  = (1024*2);
	m_iLoop			= 1;
	m_szHost		= CString( "OhneHost" ); 
	m_szUserDummy	= CString( "Tester" );
	m_sTestFileName	= CString( "C:\\Log\\foo.dat" );
	// these are brand new keys
	m_shmName		= SM_COCO_OUTPUT_CAM0;
	m_Resolution	= RESOLUTION_HIGH;
	// 	COMPRESSION_13 =12, 256KBit/s	12fps
	// 	COMPRESSION_18 =17,	128KBit/s	12fps
	//  COMPRESSION_23 =22,	  64Bit/s	12fps
	m_Compression	= COMPRESSION_23;
	m_wNumPictures	= 100;
	m_dwBitrateScale= 1000;	// 1000 Faktor 1
	m_bIsServer		= FALSE;

	m_bDoStatLogs = FALSE;
}

/**funcref ****************************************************
 * class   : CTestTool 
 * name    : ~CTestTool();
 * input   : nil
 * output  : nil
 *         :
 * purpose : destructor
 * uses    : nil
 *         :
 * author  : Hajo Fierke  1997 w+k
 * history : 09.04.1997
 *         : 09.04.1997	self
 *************************************************************/
/*
@MD
*/
CTestTool::~CTestTool()
{
 int i=0;

  m_lpCmdLineArray.RemoveAll();

#if 0
 // OOPS HEDU, das soll doch bestimmt kein if sondern ein while sein
 if ((i=m_lpCmdLineArray.GetSize())>0)
 {
	m_lpCmdLineArray.RemoveAt(0,i);
 } // end if
#endif
}

/**funcref ***************************************************
 * class   : CTestTool 
 * name    : FindHostNumberByName(
 * input   : const CString &sHostsname) const;	 
 *         : 
 * output  : CString: if   sHostsname matches TCP:
 *         :			   returns sHostsnamen unchanged 
 *         : CString: if   sHostsname matches with CHost Array: 
 *         :			   returns host number as CString: 
 *         :          else returns m_szHost	(number)
 *         :
 * purpose : I bother You and I fake You, by the living god
 *         :	that made You!		UFO 
 * uses    : CHost, CHostArray
 *         :
 * author  : Hajo Fierke  1997 w+k
 * history : 09.04.1997
 *         : 23.05.1997	self tcp: check added
 *************************************************************/
/*
@MD
*/
CString CTestTool::FindHostNumberByName(
	const CString &sHostsname) const
{
 int iWhatNum=sHostsname.FindOneOf( "0123456789" );
 CString isTcpIp=sHostsname.Left(4); // TCP:
 
 if(isTcpIp.CompareNoCase( "TCP:" ) == 0) // equal
 {
	return sHostsname;
 }

 if(iWhatNum > -1 && iWhatNum < 2)
 {
	return sHostsname;
 }
 // else:
 CHostArray l_hosts;
 
 l_hosts.Load();

 for (int i=0; i < l_hosts.GetSize(); i++)
 {
	CHost *pHost = l_hosts[i];
	if (pHost->GetName()== sHostsname) 
	{
		return (CString)pHost->GetNumber(); //(const char*)
	}
 }
 return m_szHost; 
}

/**funcref ***************************************************
 * class   : CTestTool
 * name    : SplitArgs(
 * input   : const CString &argsIn);	m_lpCmdLine 
 *         :							
 * purpose : splits the command line into a member 
 *         : string array
 * uses    : CStringArray* m_lpCmdLineArray 
 *         :
 * author  : Hajo Fierke 1997 w+k
 * history : 24.03.1997
 *         : 24.03.1997	self
 *         : 18.04.1997	hajo added blanc to collect string
 *************************************************************/
/*
@MD
*/
void CTestTool::SplitArgs(const CString &argsIn)
{
 CString collect; // (argsIn);
 int num = argsIn.GetLength();

 WK_TRACE("argsIn= %s\n",(const char*)argsIn);
 int i=0;
 for(i=0; i<num; i++)
  {
   collect.Empty();
   while(i < num && argsIn[i]==' ') 
	   i++; //remove leading spaces
   while(i < num && argsIn[i]!=' ') 
   {
    collect += argsIn[i];
    i++;
   } // end while

   WK_TRACE("%2d:Add arg '%s'\n",m_lpCmdLineArray.GetSize(),(const char *)collect);
   m_lpCmdLineArray.Add( collect );
  }	// end for
}

/**funcref ***************************************************
 * class   : CTestTool
 * name    : ParseOptions(
 * input   : nil	 
 *         :
 * purpose : parses Comand Line parameters into allowed values
 *         : 
 *-filename: -f : m_sTestFileName							
 *-size    : -s : m_iSizeofpData						
 *-host    : -h : m_szHost 		  =	name, num, or tcp:name			
 *-verify  :    : m_bDoVerify
 *-loop    : -l : m_iLoop
 *-delete  :    : m_bDoDelete	
 *-coco    :    : m_shmName       = SM_COCO_OUTPUT_CAM0;
 *-mico    :    : m_shmName       = SM_MICO_OUTPUT_CAMERAS;
 *-compress: -c : m_Compression   = 1-25
 *-resolut : -r : m_Resolution    = h,m,l
 *-bitrate : -b : m_dwBitrateScale=
 *-numpic  : -n : m_wNumPictures  = 1-1000
 *         :
 * uses    : m_lpCmdLineArray, depends on SplitArgs
 *         : WK_TRACE_ERROR(...);
 *         :
 * author  : Hajo Fierke 1997 w+k
 * history : 24.03.1997
 *         : 25.03.1997	hajo -loop implemented
 *         : 09.04.1997	hajo rebuild
 *         : 17.04.1997	hajo -server added
 *         : 24.04.1997	hajo -server thrown out again
 *************************************************************/
/*
@MD
*/
CString CTestTool::ParseOptions()
{
 CString l_sHost;

 int i=0;

 while ((i=m_lpCmdLineArray.GetSize()))
 {

	if (( (m_lpCmdLineArray[0]=="-host")  
	   || (m_lpCmdLineArray[0]=="-h")) && (i>=2)) 
	{
		l_sHost = (CString)m_lpCmdLineArray.GetAt(1);
		m_lpCmdLineArray.RemoveAt(0,2);
	}
	else if (( (m_lpCmdLineArray[0]=="-filename")  
	        || (m_lpCmdLineArray[0]=="-f")) && (i>=2)) 
	{
		m_sTestFileName = (CString)m_lpCmdLineArray.GetAt(1);
		m_lpCmdLineArray.RemoveAt(0,2);
	}
	else if (( (m_lpCmdLineArray[0]=="-size")  
	        || (m_lpCmdLineArray[0]=="-s")) && (i>=2)) 
	{
		m_iSizeofpData = atoi(m_lpCmdLineArray.GetAt(1));
		m_lpCmdLineArray.RemoveAt(0,2);
	}
	else if (m_lpCmdLineArray[0]=="-verify") 
	{
		m_lpCmdLineArray.RemoveAt(0);
		m_bDoVerify=TRUE;
	}
	else if (m_lpCmdLineArray[0]=="-delete") 
	{
		m_lpCmdLineArray.RemoveAt(0);
		m_bDoDelete=TRUE;
	}
	else if (( (m_lpCmdLineArray[0]=="-loop")  
	        || (m_lpCmdLineArray[0]=="-l")) && (i>=2)) 
	{
		m_iLoop = atoi(m_lpCmdLineArray.GetAt(1));
		if(m_iLoop > 20)
		{	
			m_iLoop=20;
			WK_TRACE("m_iLoop is set to %d\n",m_iLoop);
		}
		m_lpCmdLineArray.RemoveAt(0,2);
	}
	// these are brand new ones:
	else if (m_lpCmdLineArray[0]=="-coco") 
	{
		m_lpCmdLineArray.RemoveAt(0);
		m_shmName = SM_COCO_OUTPUT_CAM0;
	}
	else if (m_lpCmdLineArray[0]=="-mico") 
	{
		m_lpCmdLineArray.RemoveAt(0);
		m_shmName = SM_MICO_OUTPUT_CAMERAS;
//			WK_TRACE("-mico enabled\n");
	}
	else if(( 
			(m_lpCmdLineArray[0]=="-compress")
			|| (m_lpCmdLineArray[0]=="-compression")
		   || (m_lpCmdLineArray[0]=="-c")) && (i>=2)) 
	{
		int el_Comprende = atoi(m_lpCmdLineArray.GetAt(1));

		if( (el_Comprende <  1)||
			(el_Comprende > 25)  )
		{	
		 m_Compression=(Compression)22;
		 WK_TRACE("m_Compression is set to %d\n", m_Compression);
		}
		else
		{
		 m_Compression=(Compression)el_Comprende;
		}
		m_lpCmdLineArray.RemoveAt(0,2);
	}
	else if (  
			(m_lpCmdLineArray[0]=="-resolut") 
			|| (m_lpCmdLineArray[0]=="-resolution") 
			|| (m_lpCmdLineArray[0]=="-r") && (i>=2) ) 
	{
		int l_Resopal = atoi(m_lpCmdLineArray.GetAt(1));

		switch(l_Resopal)
		{
		case 'H':
		case 'h':
		case  0 : m_Resolution=RESOLUTION_HIGH;
				  break;
		case 'M':
		case 'm':
		case  1 : m_Resolution=RESOLUTION_MID;
				  break;
		case 'L':
		case 'l':
		case  2 : m_Resolution=RESOLUTION_LOW;
				  break;
		default : m_Resolution=RESOLUTION_MID;
		}
		m_lpCmdLineArray.RemoveAt(0,2);
	}
	else if (
		(m_lpCmdLineArray[0]=="-numpic")
		|| (m_lpCmdLineArray[0]=="-numPictures") ||
			 (m_lpCmdLineArray[0]=="-n") )
	{
		m_wNumPictures = atoi(m_lpCmdLineArray.GetAt(1));
		m_lpCmdLineArray.RemoveAt(0,2);
	}
	else if ((m_lpCmdLineArray[0]=="-bitrate") || 
			 (m_lpCmdLineArray[0]=="-b")  )
	{
		m_dwBitrateScale = atoi(m_lpCmdLineArray.GetAt(1));
		m_lpCmdLineArray.RemoveAt(0,2);
	} 
	else if (m_lpCmdLineArray[0]=="-statLogFormat") {
		m_bDoStatLogs = TRUE;
	} else if ((m_lpCmdLineArray[0]=="-server") || 
			 (m_lpCmdLineArray[0]=="-e")  )
	{
//		m_bIsServer = TRUE;
		m_lpCmdLineArray.RemoveAt(0);
	}
	else // if there are still some left
	{
		WK_TRACE_ERROR("Unused arg %s\n",
				(const char*)m_lpCmdLineArray[0]);
		m_lpCmdLineArray.RemoveAt(0); // push it out!
	}


 }	// end while 
 return l_sHost;
}

