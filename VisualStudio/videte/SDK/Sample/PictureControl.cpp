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
|* PROGRAM: PictureControl.cpp
|*
|* PURPOSE: Base class to draw pictures on dialog interface in it's own 
|*			window.; implementation file.
******************************************************************************/

#include "stdafx.h"
#include "Sample.h"
#include "PictureControl.h"

#include "SampleDlg.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CPictureControl, CStatic)
CPictureControl::CPictureControl()
{
	m_pSampleDlg	= NULL;
}

CPictureControl::~CPictureControl()
{
}
/////////////////////////////////////////////////////////////////////////////
// message map
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CPictureControl, CStatic)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/*********************************************************************************************
Description   : This function is called to avoid redrawing the window background with the
				default brush. This avoid flicker effects during representation of live and
				database pictures.

Parameters:   
pDC		: (I): Pointer to the device context.		(CDC*)

Return Value:  (void)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
BOOL CPictureControl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}
