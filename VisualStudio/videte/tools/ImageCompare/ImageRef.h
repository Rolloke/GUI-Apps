/////////////////////////////////////////////////////////////////////////////
// PROJECT:		ImageCompare
// FILE:		$Workfile: ImageRef.h $
// ARCHIVE:		$Archive: /Project/Tools/ImageCompare/ImageRef.h $
// CHECKIN:		$Date: 12.07.99 15:08 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 12.07.99 14:31 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGEREF_H__EA2CDA08_F6EA_11D2_8D08_004005A11E32__INCLUDED_)
#define AFX_IMAGEREF_H__EA2CDA08_F6EA_11D2_8D08_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ImageCompareDoc.h"
typedef const CString& CSR;

class CImageRef : public CObject  
{
public:
	CImageRef();
	CImageRef(CSR sHostName, CSR sCamName, CSR sImageName1, CSR sImageName2);
	virtual ~CImageRef();

	CString m_sHostName;
	CString m_sCamName;
	CString m_sImageName1;
	CString m_sImageName2;
	CString m_sImageComment;
	IStatus m_ImageStatus;
};

#endif // !defined(AFX_IMAGEREF_H__EA2CDA08_F6EA_11D2_8D08_004005A11E32__INCLUDED_)
