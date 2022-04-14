/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CameraCommandRecord.h $
// ARCHIVE:		$Archive: /Project/Units/CommUnit/CameraCommandRecord.h $
// CHECKIN:		$Date: 26.06.03 17:54 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 25.06.03 9:53 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$

#ifndef __CameraCommandRecord_H
#define __CameraCommandRecord_H

#include "SecID.h"
#include "CameraControl.h"

/////////////////////////////////////////////////////////////////////////////
class CCameraCommandRecord
{
	// construction
public:
	CCameraCommandRecord(CSecID commID, 
					     CSecID camID,
						 CameraControlCmd cmd,
						 DWORD dwValue);

	// access
public:
	CSecID	GetCommID() const;
	CSecID	GetCamID() const;
	CameraControlCmd GetCommand() const;
	DWORD	GetValue() const;

	// implementation
private:
private:
	CSecID			  m_commID;
	CSecID			  m_camID;		// HIWORD: opt. system id, LOWORD: Camera id
	CameraControlCmd  m_cmd;
	DWORD			  m_dwValue;
};
/////////////////////////////////////////////////////////////////////////////
inline CCameraCommandRecord::CCameraCommandRecord(CSecID commID, 
												  CSecID camID,
												  CameraControlCmd cmd,
												  DWORD dwValue)
{
	m_commID = commID;
	m_camID = camID;
	m_cmd = cmd;
	m_dwValue = dwValue;
}
/////////////////////////////////////////////////////////////////////////////
inline CSecID CCameraCommandRecord::GetCommID() const
{
	return m_commID;
}
/////////////////////////////////////////////////////////////////////////////
inline CSecID CCameraCommandRecord::GetCamID() const
{
	return m_camID;
}
/////////////////////////////////////////////////////////////////////////////
inline CameraControlCmd CCameraCommandRecord::GetCommand() const
{
	return m_cmd;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CCameraCommandRecord::GetValue() const
{
	return m_dwValue;
}
/////////////////////////////////////////////////////////////////////////////
#endif