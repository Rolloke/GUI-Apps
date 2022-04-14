/////////////////////////////////////////////////////////////////////////////
// PROJECT:		ImageCompare
// FILE:		$Workfile: ImageRef.cpp $
// ARCHIVE:		$Archive: /Project/Tools/ImageCompare/ImageRef.cpp $
// CHECKIN:		$Date: 12.07.99 15:08 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 12.07.99 14:31 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageCompare.h"
#include "ImageRef.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CImageRef::CImageRef()
{
	m_sHostName		= "";
	m_sCamName		= "";
	m_sImageName1	= "";
	m_sImageName2	= "";
	m_ImageStatus	= Unknown;
	m_sImageComment = "";
}

//////////////////////////////////////////////////////////////////////
CImageRef::CImageRef(CSR sHostName, CSR sCamName, CSR sImageName1, CSR sImageName2)
{
	m_sHostName		= sHostName;
	m_sCamName		= sCamName;
	m_sImageName1	= sImageName1;
	m_sImageName2	= sImageName2;
	m_ImageStatus	= Unknown;
}

//////////////////////////////////////////////////////////////////////
CImageRef::~CImageRef()
{

}
