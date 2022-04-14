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
|* PROGRAM: PictureControl.h
|*
|* PURPOSE: Base class to draw pictures on dialog interface 
|*			in it's own window.
******************************************************************************/

#pragma once


class CSampleDlg;
class CPictureControl : public CStatic
{
	DECLARE_DYNAMIC(CPictureControl)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
public:
	CPictureControl();
	virtual ~CPictureControl();

//////////////////////////////////////////////////////////////////////
// members
//////////////////////////////////////////////////////////////////////
public:

	CSampleDlg *m_pSampleDlg;	//pointer to the main dialog

//////////////////////////////////////////////////////////////////////
// message map
//////////////////////////////////////////////////////////////////////
public:

	DECLARE_MESSAGE_MAP()
	afx_msg BOOL			OnEraseBkgnd(CDC* pDC);
};
//////////////////////////////////////////////////////////////////////


