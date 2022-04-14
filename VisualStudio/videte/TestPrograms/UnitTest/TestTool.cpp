///////////////////////////////////////////////////////////////
// FILE   : $Workfile: TestTool.cpp $ 
// ARCHIVE: $Archive: /Project/Tools/UnitTest/TestTool.cpp $
// DATE   :	$Date: 25.04.97 11:57 $ 
// VERSION: $Revision: 3 $
// LAST   : $Modtime: 25.04.97 10:31 $ 
// AUTHOR : $Author: Hajo $
//        : $Nokeywords:$

/**modulref ***************************************************
 * name    : TestTool.cpp
 * class   : CTestTool
 * derived : CWinApp
 * members :
 * public  : implemented in "TestTool.h"
 * access  : inline BOOL	GetDoVerify() const;
 *         : inline CString	GetTestFileName() const;
 *         : inline int		GetDestination() const;
 *         : inline int		GetSizeofpData() const;
 *         : inline int		GetLoop() const;
 *         : inline int		GetDoDelete() const; 
 *         : 
 *         :		int		DoTest(const CString&);
 * private :		
 *         : void		SplitArgs(const CString&);
 *         : CString	ParseOptions(CStringArray&);
 *         : CString	FindHostNumberByName(const CString&) const;
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
 *         : 
 *         : 
 * purpose : test of the coc unit 
 *         :
 * see     : ParseOptions for valid comand line parameters 
 *         :
 * author  : Hajo Fierke 1997 w+k
 * history : 09.04.1997
 *         : 
 **************************************************************/
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

/**funcref ****************************************************
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
 **************************************************************/
/*
@MD
*/
CTestTool::CTestTool()
{
	m_bDoDelete		= FALSE;
	m_bDoVerify		= FALSE;
	m_iDestination	= 4;	
	m_iSizeofpData  = (1024*2);
	m_iLoop			= 20;
	m_szHost		= CString( "85" ); 
	m_szUserDummy	= CString( "Tester" );
	m_sTestFileName	= CString( "C:\\Log\\foo.dat" );
	// these are brand new keys
	m_shmName		= SM_COCO_OUTPUT_CAM0;
	m_Resolution	= RESOLUTION_HIGH;
	// 	COMPRESSION_13,	256KBit/s	12fps
	// 	COMPRESSION_18	128KBit/s	12fps
	m_Compression	= COMPRESSION_18;
	m_wNumPictures	= 200;
	m_dwBitrateScale= 1000;	// 1000 Faktor 1
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
 **************************************************************/
/*
@MD
*/
CTestTool::~CTestTool()
{
 int i=0;
 if ((i=m_lpCmdLineArray.GetSize())>0)
 {
	m_lpCmdLineArray.RemoveAt(0,i);
 } // end if
}

/**funcref ****************************************************
 * class   : CTestTool 
 * name    : FindHostNumberByName(
 * input   : const CString &sHostsname) const;	 
 *         : 
 * output  : CString: if   sHostsname matches with CHost Array: 
 *         :			   returns host number as CString: 
 *         :          else returns m_szHost	(number)
 *         :
 * purpose : I bother You and I fake You, by the living god
 *         :	that made You!		UFO 
 * uses    : CHost, CHostArray
 *         :
 * author  : Hajo Fierke  1997 w+k
 * history : 09.04.1997
 *         : 09.04.1997	self
 **************************************************************/
/*
@MD
*/
CString 
CTestTool::FindHostNumberByName(const CString &sHostsname) const
{
 int iWhatNum=sHostsname.FindOneOf( "0123456789" );
 
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

/**funcref ****************************************************
 * class   : CTestTool
 * name    : SplitArgs(
 * input   : const CString &argsIn);	m_lpCmdLine 
 *         :							
 * purpose : splits the command line into a member string array
 *         :
 * uses    : CStringArray* m_lpCmdLineArray 
 *         :
 * author  : Hajo Fierke 1997 w+k
 * history : 24.03.1997
 *         : 24.03.1997	self
 **************************************************************/
/*
@MD
*/
void CTestTool::SplitArgs(const CString &argsIn)
{
 CString collect(argsIn);
 int num = argsIn.GetLength();

 WK_TRACE("argsIn= %s\n",(const char*)argsIn);
 int i=0;
 int j=0;
 for(i=0; i<num; i++)
  {
   j=0;
   while(i < num && argsIn[i]==' ') 
	   i++; //remove leading spaces
   while(i < num && argsIn[i]!=' ') 
   {
    collect.SetAt(j, argsIn[i]);
    i++;
    j++;
   }
   collect.SetAt(j, 0);

   m_lpCmdLineArray.Add(  (const char *)collect );
  }
}

/**funcref ****************************************************
 * class   : CTestTool
 * name    : ParseOptions(
 * input   : nil	 
 *         :
 * purpose : parses the CStringArray m_lpCmdLineArray 
 *         : into allowed values:
 *-filename: m_sTestFileName							
 *-size    : m_iSizeofpData						
 *-host    : m_szHost 						
 *-verify  : m_bDoVerify
 *-loop    : m_iLoop
 *-delete  : m_bDoDelete	
 *-coco    : m_shmName       = SM_COCO_OUTPUT_CAM0;
 *-mico    : m_shmName       = SM_MICO_OUTPUT_CAMERAS;
 *-compress: m_Compression   = 1-25
 *-c       : 
 *-resolut : m_Resolution    = h,m,l
 *-r	   :
 *-bitrate : m_dwBitrateScale=
 *-numpic  : m_wNumPictures  = 1-1000
 *         :
 *         :
 * uses    : m_lpCmdLineArray, depends on SplitArgs
 *         : WK_TRACE_ERROR(...);
 *         :
 * author  : Hajo Fierke 1997 w+k
 * history : 24.03.1997
 *         : 25.03.1997	hajo -loop implemented
 *         : 09.04.1997	hajo rebuild
 **************************************************************/
/*
@MD
*/
CString CTestTool::ParseOptions()
{
 CString l_sHost;

// WK_TRACE("lpCmdLineArray[%d]= %s\n",0,(CString)lpCmdLineArray->GetAt(0));
// WK_TRACE("lpCmdLineArray[%d]= %s\n",1,(CString)lpCmdLineArray->GetAt(1));
 int i=0;

 while ((i=m_lpCmdLineArray.GetSize()))
 {

	if ((m_lpCmdLineArray.GetAt(0)=="-host") && (i>=2) )
	{
		l_sHost = (CString)m_lpCmdLineArray.GetAt(1);
		m_lpCmdLineArray.RemoveAt(0,2);
	}
	else if ((m_lpCmdLineArray.GetAt(0)=="-filename") && (i>=2) )
	{
		m_sTestFileName = (CString)m_lpCmdLineArray.GetAt(1);
		m_lpCmdLineArray.RemoveAt(0,2);
	}
	else if ((m_lpCmdLineArray.GetAt(0)=="-loop") && (i>=2) ) 
	{
		m_iLoop = atoi(m_lpCmdLineArray.GetAt(1));
		if(m_iLoop > 20)
		{	
			m_iLoop=20;
			WK_TRACE("m_iLoop is set to %d\n",m_iLoop);
		}
		m_lpCmdLineArray.RemoveAt(0,2);
	}
	else if ((m_lpCmdLineArray.GetAt(0)=="-size")&& (i>=2) ) 
	{
		m_iSizeofpData = atoi(m_lpCmdLineArray.GetAt(1));
		m_lpCmdLineArray.RemoveAt(0,2);
//			WK_TRACE("m_iSizeofpData= %d\n",m_iSizeofpData);
	}
	else if (m_lpCmdLineArray[0]=="-verify") 
	{
		m_lpCmdLineArray.RemoveAt(0);
		m_bDoVerify=TRUE;
//			WK_TRACE("-verify enabled\n");
	}
	else if (m_lpCmdLineArray[0]=="-delete") 
	{
		m_lpCmdLineArray.RemoveAt(0);
		m_bDoDelete=TRUE;
//			WK_TRACE("-delete enabled\n");
	}
	// these are brand new ones:
	else if (m_lpCmdLineArray[0]=="-coco") 
	{
		m_lpCmdLineArray.RemoveAt(0);
		m_shmName = SM_COCO_OUTPUT_CAM0;
//			WK_TRACE("-coco enabled\n");
	}
	else if (m_lpCmdLineArray[0]=="-mico") 
	{
		m_lpCmdLineArray.RemoveAt(0);
		m_shmName = SM_MICO_OUTPUT_CAMERAS;
//			WK_TRACE("-mico enabled\n");
	}
	else if ((m_lpCmdLineArray[0]=="-compress")
		   ||(m_lpCmdLineArray[0]=="-c")) 
	{
		int el_Comprende = atoi(m_lpCmdLineArray.GetAt(1));

		if( (el_Comprende <  1)||
			(el_Comprende > 25)  )
		{	
		 m_Compression=(Compression)20;
		 WK_TRACE("m_Compression is set to %d\n", m_Compression);
		}
		else
		{
		 m_Compression=(Compression)el_Comprende;
		}
		m_lpCmdLineArray.RemoveAt(0,2);
//		WK_TRACE("-compression enabled\n");
	}
	else if ((m_lpCmdLineArray[0]=="-resolut") ||
			 (m_lpCmdLineArray[0]=="-r")) 
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
//		WK_TRACE("-m_Resolution enabled\n");
	}
	else if ((m_lpCmdLineArray[0]=="-numpic") ||
			 (m_lpCmdLineArray[0]=="-n") )
	{
		m_wNumPictures = atoi(m_lpCmdLineArray.GetAt(1));
		m_lpCmdLineArray.RemoveAt(0,2);

//			WK_TRACE("-numpic enabled\n");
	}
	else if ((m_lpCmdLineArray[0]=="-bitrate") || 
			 (m_lpCmdLineArray[0]=="-b")  )
	{
		m_dwBitrateScale = atoi(m_lpCmdLineArray.GetAt(1));
		m_lpCmdLineArray.RemoveAt(0,2);

//			WK_TRACE("-bitrate enabled\n");
	}
	else // if there are still some left
	{
//		WK_TRACE_ERROR("Unused arg %s\n",
//				(const char*)m_lpCmdLineArray[0]);
		m_lpCmdLineArray.RemoveAt(0); // push it out!
	}


 }	// end while 
 return l_sHost;
}

