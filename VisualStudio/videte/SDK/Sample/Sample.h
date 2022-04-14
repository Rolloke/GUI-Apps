/******************************************************************************
|* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
|* ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
|* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
|* PARTICULAR PURPOSE.
|* 
|* Copyright 2005 videte IT ® AG. All Rights Reserved.
|*-----------------------------------------------------------------------------
|* videteSDK / Sample
|*
|* PROGRAM: Sample.h
|*
|* PURPOSE: Main file of the application.
******************************************************************************/

// Sample.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "IPCOutputSample.h"

class CSampleApp : public CWinApp
{
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
public:
	CSampleApp();

//////////////////////////////////////////////////////////////////////
// overrides
//////////////////////////////////////////////////////////////////////
public:

	virtual BOOL		InitInstance();

//////////////////////////////////////////////////////////////////////
// message map
//////////////////////////////////////////////////////////////////////
public:

	DECLARE_MESSAGE_MAP()
	virtual int			ExitInstance();
};
//////////////////////////////////////////////////////////////////////
extern CSampleApp theApp;
