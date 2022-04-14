/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: stdafx.h $
// ARCHIVE:		$Archive: /Project/SecurityServer/stdafx.h $
// CHECKIN:		$Date: 23.06.06 14:42 $
// VERSION:		$Revision: 32 $
// LAST CHANGE:	$Modtime: 23.06.06 11:41 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
// #include <afxole.h>         // MFC OLE classes
// #include <afxodlgs.h>       // MFC OLE dialog classes
// #include <afxdisp.h>        // MFC OLE automation classes

#include <afxcoll.h>
#include <afxcmn.h>
#include <afxdlgs.h>
#include <afxmt.h>
#include <afxcview.h>
#include <afxinet.h>

#include <wk.h>
#include <WK_Names.h>
#include <wkclasses\wk_trace.h>
#include <wkclasses\wk_file.h>
#include <wk_dongle.h>
#include <wkclasses\wk_profile.h>
#include <WK_Timer.h>
#include <wkclasses\WK_RunTimeError.h>
#include <WK_DebugOptions.h>
#include <WK_Version.h>

#include <PictureDef.h>
#include <PictureRecord.h>

#include <SecID.h>
#include <Cipc.h>

#include <CIPCExtraMemory.h>
#include <CIPCOutput.h>
#include <CIPCInput.h>
#include <IPCInputFileUpdate.h>
#include <CIPCDatabase.h>
#include <CipcStringDefs.h>
#include <CipcOutputRecord.h>
#include <CIPCFetchResult.h>
#include <ConnectionRecord.h>
#include <IPCFetch.h>
#include <CIPCOutputClient.h>
#include <CipcServerControl.h>
#include <MediaSampleRecord.h>
#include <CIPCMedia.h>

#include <Host.h>
#include <User.h>
#include <Permission.h>
#include <SecTimer.h>
#include <NotificationMessage.h>

#include <Skins/Skins.h> // must be included before oemgui!

#include <CIPCField.h>
#include <wkclasses\wk_util.h>
#include <WKClasses/WK_StopWatch.h>
#include <WKClasses/WK_Thread.h>
#include <wkclasses/WK_PerfMon.h>
#include <wkclasses/statistic.h>
#include <wkclasses/rsa.h>
#include <WKClasses/WK_String.h>
#include <WKClasses/WK_RS232.h>
#include <wkclasses\WK_Utilities.h>
#include <wkclasses\WK_ThreadPool.h>
#include <OemGui/OemGuiApi.h>
#include <oemgui\res\ApplicationDefines.h>

#include <vimage\vimage.h>
#include <vimage\cavcodec.h>
