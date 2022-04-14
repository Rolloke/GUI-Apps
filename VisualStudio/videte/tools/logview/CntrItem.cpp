/////////////////////////////////////////////////////////////////////////////
// PROJECT:		LogView
// FILE:		$Workfile: CntrItem.cpp $
// ARCHIVE:		$Archive: /Project/Tools/LogView/CntrItem.cpp $
// CHECKIN:		$Date: 14.01.05 14:07 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 14.01.05 14:06 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogView.h"

#include "LogViewDoc.h"
#include "LogViewView.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogViewCntrItem implementation

IMPLEMENT_SERIAL(CLogViewCntrItem, CRichEditCntrItem, 0)

CLogViewCntrItem::CLogViewCntrItem(REOBJECT* preo, CLogViewDoc* pContainer)
	: CRichEditCntrItem(preo, pContainer)
{
	
}

CLogViewCntrItem::~CLogViewCntrItem()
{

}

/////////////////////////////////////////////////////////////////////////////
// CLogViewCntrItem diagnostics

#ifdef _DEBUG
/////////////////////////////////////////////////////////////////////////////
void CLogViewCntrItem::AssertValid() const
{
	CRichEditCntrItem::AssertValid();
}

/////////////////////////////////////////////////////////////////////////////
void CLogViewCntrItem::Dump(CDumpContext& dc) const
{
	CRichEditCntrItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
