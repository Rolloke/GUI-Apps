/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: media.h $
// ARCHIVE:		$Archive: /DV/DVProcess/media.h $
// CHECKIN:		$Date: 25.02.04 9:48 $
// VERSION:		$Revision: 9 $
// LAST CHANGE:	$Modtime: 25.02.04 9:36 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef __MEDIA_H_
#define __MEDIA_H_

#include "SecID.h"

////////////////////////////////////////////////////////////////////////////
#define AUDIO_RECORDING_LEVEL		0x01
#define AUDIO_RECORDING_ACTIVITY	0x02
#define AUDIO_RECORDING_EXTERN		0x04
////////////////////////////////////////////////////////////////////////////
// Definitionen für Media Struktur           
//
// Status-word(bitmask) der Struktur

class CIPCAudioServer;
class CCamera;
//////////////////////////////////////////////////////////////////////
class CMedia
{
	// construction / destruction
public:
	CMedia(CIPCAudioServer* pGroup, WORD wGroupID, WORD wSubID);
	~CMedia();
	const CMedia& operator=(const CMedia& inp);
	
	// access:
public:
	inline const CSecID		GetID() const;
	inline const CString&	GetName() const;
	inline const CString&	GetType() const;
				 CString    Format();

	inline CIPCAudioServer* GetGroup();
	inline CCamera* GetCamera();



	// settings:
public:
	inline void SetName(const CString &sName);
	inline void SetType(const CString &sType);
		   void SetCamera(CCamera* pCamera);

	// serialization
public:

private:
	CSecID	m_id;			// MediaID
	CString	m_sName;
	CString	m_sType;
	CIPCAudioServer* m_pGroup;

	CCamera* m_pCamera;
};
//////////////////////////////////////////////////////////////////////
const CSecID CMedia::GetID() const
{
	return (m_id);
}
//////////////////////////////////////////////////////////////////////
const CString &CMedia::GetName() const
{
	return (m_sName);
}
//////////////////////////////////////////////////////////////////////
const CString &CMedia::GetType() const
{
	return (m_sType);
}
//////////////////////////////////////////////////////////////////////
inline void CMedia::SetName(const CString &sName)
{
	m_sName = sName;
}
//////////////////////////////////////////////////////////////////////
inline void CMedia::SetType(const CString &sType)
{
	m_sType= sType;
}
//////////////////////////////////////////////////////////////////////
inline CCamera* CMedia::GetCamera()
{
	return m_pCamera;
}
//////////////////////////////////////////////////////////////////////
inline CIPCAudioServer* CMedia::GetGroup()
{
	return m_pGroup;
}
//////////////////////////////////////////////////////////////////////
typedef CMedia* CMediaPtr;
WK_PTR_ARRAY_CS(CMediaArray,CMediaPtr,CMediaArrayCS);
#endif
