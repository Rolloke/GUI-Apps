/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CDataBaseData.h $
// ARCHIVE:		$Archive: /Project/Include/CDataBaseData.h $
// CHECKIN:		$Date: 11.07.96 12:49 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 11.07.96 11:11 $
// BY AUTHOR:	$Author: Carsten $
// $Nokeywords:$

#ifndef __CDataBaseData_H__
#define __CDataBaseData_H__

class CDataBaseData : public CObject
{
public:
	DECLARE_SERIAL( CDataBaseData )

	CDataBaseData();
	~CDataBaseData();

	void	Serialize(CArchive& ar);
protected:

//globals

};

#endif
