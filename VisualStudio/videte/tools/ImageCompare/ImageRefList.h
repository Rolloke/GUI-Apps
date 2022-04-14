/////////////////////////////////////////////////////////////////////////////
// PROJECT:		ImageCompare
// FILE:		$Workfile: ImageRefList.h $
// ARCHIVE:		$Archive: /Project/Tools/ImageCompare/ImageRefList.h $
// CHECKIN:		$Date: 21.04.99 7:51 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 21.04.99 7:51 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGEREFLIST_H__EA2CDA09_F6EA_11D2_8D08_004005A11E32__INCLUDED_)
#define AFX_IMAGEREFLIST_H__EA2CDA09_F6EA_11D2_8D08_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CImageRef;
class CImageRefList : public CObList  
{
public:
	CImageRefList();
	virtual ~CImageRefList();
	BOOL AddImageRef(CImageRef* pImageRef);
	BOOL AddImageRef(CSR sHostName, CSR sCamName, CSR sImageName1, CSR sImageName2);
	CImageRef* GetImageRef(int nIndex);
};

#endif // !defined(AFX_IMAGEREFLIST_H__EA2CDA09_F6EA_11D2_8D08_004005A11E32__INCLUDED_)
