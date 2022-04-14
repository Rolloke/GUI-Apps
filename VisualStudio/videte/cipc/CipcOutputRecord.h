/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcOutputRecord.h $
// ARCHIVE:		$Archive: /Project/CIPC/CipcOutputRecord.h $
// CHECKIN:		$Date: 26.06.06 13:25 $
// VERSION:		$Revision: 18 $
// LAST CHANGE:	$Modtime: 26.06.06 13:23 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef __CIPC_OUTPUT_RECORD_H
#define __CIPC_OUTPUT_RECORD_H

#include "SecID.h"

////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////
//  CIPCOutputRecord | The class encapsulates all necessary information
// about one output. An output can be a camera or an relay. Some flags define
// wether the camera is active or not, black/white or color, JPEG or MPEG/H.263
// or if it is a pan tilt zoom camera.
// The state of a relais is defined by flags too, every object has an id and
// a name
////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CIPCOutputRecord : public CObject
{
	DECLARE_DYNAMIC(CIPCOutputRecord)	// runtime class identification
	//  construction/destruction
public:
	//!ic! constructor
	inline CIPCOutputRecord();
	//!ic! copy constructor
	inline CIPCOutputRecord(const CIPCOutputRecord& rec);
	//!ic! assignment operator
	inline const CIPCOutputRecord & operator=(const CIPCOutputRecord& rec);

	//  attributes
public:
	//!ic! name of camera/relais
	inline const CString &GetName() const;
	//!ic! id of camera/relais
	inline CSecID GetID() const;
	//!ic! state of camera / relais
	inline WORD GetFlags() const;
	//!ic! is hardware state OK
	inline BOOL	IsOkay() const;
	//!ic! is enabled by sysadmin
	inline BOOL	IsEnabled() const;
	//!ic! is it a relais
	inline BOOL	IsRelay() const;
	//!ic! is relais closed
	inline BOOL	RelayIsClosed() const;
	//!ic! is relais open
	inline BOOL	RelayIsOpen() const;
	//!ic! is relais edge conrolled
	inline BOOL	RelayIsEdgeControlled() const;
	//!ic! is it a camera
	inline BOOL	IsCamera() const;
	//!ic! is the camera/compression card color
	inline BOOL	CameraHasColor() const;
	//!ic! is it not a pan tilt zoom
	inline BOOL	CameraIsFixed() const;
	//!ic! is it a jpeg camera i.e. MiCo or JaCob
	inline BOOL CameraDoesJpeg() const;
	//!ic! is it a YUV camera
	inline BOOL CameraIsYUV() const;
	//!ic! is it a camera connected to a PT device
	inline BOOL CameraIsPresence() const;
	//!ic! is it a camera with reference function
	inline BOOL IsReference() const;
	//!ic! is it a camera with reference function
	inline BOOL CameraDoesMpeg() const;

	//  operations
public:
	//!ic! sets name id and flags
	inline void Set(const CString &s, CSecID id, WORD wFlags);
	//!ic! sets flags
	inline void SetFlags(WORD wNewFlags);
	//
	inline BOOL operator != (const CIPCOutputRecord& or);
	
	//
private:
	// data:
	CString m_sName;
	CSecID	m_id;
	WORD	m_wFlags;
};
///////////////////////////////////////////////////////////////////
//  CIPCOutputRecord is hardware ok
#define OREC_IS_OKAY				((WORD)1)
//  CIPCOutputRecord is enabled by sysadmin
#define OREC_IS_ENABLED				((WORD)1<<1)
//  CIPCOutputRecord is a relais
#define OREC_IS_RELAY				((WORD)1<<2)
//  CIPCOutputRecord is relais closed
#define OREC_RELAY_CLOSED			((WORD)1<<3)
//  CIPCOutputRecord is a camera
#define OREC_IS_CAMERA				((WORD)1<<4)
//  CIPCOutputRecord is a color compression algorithm
#define OREC_CAMERA_COLOR			((WORD)1<<5)
//  CIPCOutputRecord is not a pan tilt zoom camera
#define OREC_CAMERA_IS_FIXED		((WORD)1<<6)
//  CIPCOutputRecord is a JPEG (color or bw) compression
#define OREC_CAMERA_DOES_JPEG		((WORD)1<<7)
//  CIPCOutputRecord is a YUV camera
#define OREC_CAMERA_YUV				((WORD)1<<8)
//  CIPCOutputRecord is PT200 box camera
#define OREC_CAMERA_PRESENCE		((WORD)1<<9)
//  CIPCOutputRecord camera has reference image function enabled (only idip)
#define OREC_CAMERA_REFERENCE		((WORD)1<<10)
// CIPCOutputRecord camera is shown in Vision
#define OREC_CAMERA_VISIBLE		((WORD)1<<11)
// CIPCOutputRecord camera is Tasha/MPEG4
#define OREC_CAMERA_DOES_MPEG	((WORD)1<<12)
// CIPCOutputRecord relais is flank controlled
#define OREC_RELAY_IS_EDGE       ((WORD)1<<13)
///////////////////////////////////////////////////////////////////
typedef CIPCOutputRecord  *CIPCOutputRecordPtr;
WK_PTR_ARRAY(CIPCOutputRecordArray,CIPCOutputRecordPtr)
///////////////// inlined functions ///////////////////////

/*
Function: 
default constructor, no name, SECID_NO_ID, empty flags
*/
inline CIPCOutputRecord::CIPCOutputRecord()
{
	// m_sName;
	// m_id;
	m_wFlags=0;	// not enabled
}
/*Function: 
copy constructor
*/
inline CIPCOutputRecord::CIPCOutputRecord(const CIPCOutputRecord& rec)
{
	m_sName  = rec.m_sName;
	m_id     = rec.m_id;
	m_wFlags = rec.m_wFlags;	
}
/*Function: 
assigment operator
*/
inline const CIPCOutputRecord &CIPCOutputRecord::operator=(const CIPCOutputRecord& rec)
{
	m_sName  = rec.m_sName;
	m_id     = rec.m_id;
	m_wFlags = rec.m_wFlags;	
	return *this;
}
/*Function: 
returns the name of camera or relais
@rdesc a const CString reference, name of camera or relais
*/
inline const CString &CIPCOutputRecord::GetName() const
{
	return m_sName;
}
/*Function: 
returns the id of camera or relais
@rdesc a CSecID, id of camera or relais
 <c CSecID>
*/
inline CSecID CIPCOutputRecord::GetID() const
{
	return m_id;
}
/*Function: 
returns the flags of camera or relais
@rdesc a WORD, flags of camera or relais
*/
inline WORD CIPCOutputRecord::GetFlags() const
{
	return m_wFlags;
}
/*Function: 
returns hardware state of relais or camera
@rdesc TRUE if the hardware state is OK, otherwise FALSE
*/
inline BOOL	CIPCOutputRecord::IsOkay() const
{
	return (m_wFlags & OREC_IS_OKAY)!=0;
}
/*Function: 
returns configuration state of relais or camera
@rdesc TRUE if the hardware configuration is enabled, otherwise FALSE
*/
inline BOOL	CIPCOutputRecord::IsEnabled() const
{
	return (m_wFlags & OREC_IS_ENABLED)!=0;
}
/*Function: 
returns wether it's a relais
@rdesc TRUE if the object is a relais, otherwise FALSE
*/
inline BOOL	CIPCOutputRecord::IsRelay() const
{
	return (m_wFlags & OREC_IS_RELAY)!=0;
}
/*Function: 
returns wether the relais is closed or not
@rdesc TRUE if the relais is closed, otherwise FALSE
*/
inline BOOL	CIPCOutputRecord::RelayIsClosed() const
{
	return (m_wFlags & OREC_RELAY_CLOSED)!=0;
}
/*Function: 
returns wether the relais is open or not
@rdesc TRUE if the relais is open, otherwise FALSE
*/
inline BOOL	CIPCOutputRecord::RelayIsOpen() const
{
	return (m_wFlags & OREC_RELAY_CLOSED)==0;
}

/*Function: 
returns wether the relais is edge controlled
*/
inline BOOL	CIPCOutputRecord::RelayIsEdgeControlled() const
{
	return (m_wFlags & OREC_RELAY_IS_EDGE)!=0;
}

/*Function: 
returns wether it's a camera
@rdesc TRUE if the object is a camera, otherwise FALSE
*/
inline BOOL	CIPCOutputRecord::IsCamera() const
{
	return (m_wFlags & OREC_IS_CAMERA)!=0;
}
/*Function: 
returns wether it's camera with color compression algorithm
@rdesc TRUE if the camera can deliver color images, otherwise FALSE
*/
inline BOOL	CIPCOutputRecord::CameraHasColor() const
{
	return (m_wFlags & OREC_CAMERA_COLOR)!=0;
}
/*Function: 
returns wether it's camera without pan tilt zoom function
@rdesc TRUE if the camera is fixed, without any pan tilt or zoom function, otherwise FALSE
*/
inline BOOL	CIPCOutputRecord::CameraIsFixed() const
{
	return (m_wFlags & OREC_CAMERA_IS_FIXED)!=0;
}
/*Function: 
returns wether it's camera with color compression algorithm JPEG
@rdesc TRUE if the camera can deliver JPEG images, otherwise FALSE
*/
inline BOOL	CIPCOutputRecord::CameraDoesJpeg() const
{
	return (m_wFlags & OREC_CAMERA_DOES_JPEG)!=0;
}
/*Function: 
returns wether it's camera with color compression algorithm MPEG
@rdesc TRUE if the camera can deliver MPEG images, otherwise FALSE
*/
inline BOOL	CIPCOutputRecord::CameraDoesMpeg() const
{
	return (m_wFlags & OREC_CAMERA_DOES_MPEG)!=0;
}
/*Function: 
returns wether it's camera with color YUV uncompressed
@rdesc TRUE if the camera can deliver YUV images, otherwise FALSE
*/
inline BOOL CIPCOutputRecord::CameraIsYUV() const
{
	return (m_wFlags & OREC_CAMERA_YUV)!=0;
}
/*Function: 
returns wether it's camera with color compression algorithm from company Presence Technology
@rdesc TRUE if the camera can deliver PT-200 images, otherwise FALSE
*/
inline BOOL CIPCOutputRecord::CameraIsPresence() const
{
	return (m_wFlags & OREC_CAMERA_PRESENCE)!=0;
}
/*Function: 
returns wether the reference functionality is activated for the camera
@rdesc TRUE if the reference functionality is activated for the camera, otherwise FALSE
*/
inline BOOL CIPCOutputRecord::IsReference() const
{
	return (m_wFlags & OREC_CAMERA_REFERENCE)!=0;
}
/*Function: 
Set's the name id and flags of the camera or relais.
param: const CString& | s | the new name
param: CSecID | id | the new id
param: WORD | flags | the new flags
 <c CSecID>
*/
inline void CIPCOutputRecord::Set(const CString &s, CSecID id, WORD flags)
{
	m_sName = s;
	m_id = id;
	m_wFlags = flags;
}
/*Function: 
Set's the flags of the camera or relais.
param: WORD | flags | the new flags
*/
inline void CIPCOutputRecord::SetFlags(WORD flags)
{
	m_wFlags = flags;
}

inline BOOL CIPCOutputRecord::operator != (const CIPCOutputRecord& or)
{
	return    m_sName != or.m_sName
		   || m_id != or.m_id
		   || m_wFlags != or.m_wFlags;

}

#endif
