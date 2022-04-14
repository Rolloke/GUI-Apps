/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: media.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/media.h $
// CHECKIN:		$Date: 16.01.04 16:05 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 16.01.04 11:44 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __MEDIA_H_
#define __MEDIA_H_

#include "SecID.h"

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// Definitionen für Media Struktur           
//
// Status-word(bitmask) der Struktur

class CMediaGroup;
//////////////////////////////////////////////////////////////////////
class CMedia
{
	// construction / destruction
public:
	CMedia(WORD wGroupID, WORD wSubID);
	~CMedia();
	const CMedia& operator=(const CMedia& inp);
	
	// access:
public:
	inline const CSecID		GetID() const;
	inline const CString&	GetName() const;
	inline const CString&	GetType() const;

	const bool  IsDefault()  const { return m_bIsDefault;  };
	const bool  IsAudio()    const { return m_bIsAudio;    };
	const bool  IsVideo()    const { return m_bIsVideo;    };
	const bool  IsDisabled() const { return m_bIsDisabled; };

	// settings:
public:
	inline void SetName(const CString &sName);
	inline void SetType(const CString &sType);

	void			SetDefault( bool b) { m_bIsDefault  = b; };
	void			SetAudio(   bool b) { m_bIsAudio    = b; };
	void			SetVideo(   bool b) { m_bIsVideo    = b; };
	void			SetDisabled(bool b) { m_bIsDisabled = b; };

	// serialization
public:
	BOOL Load(CWK_Profile& wkp);
	BOOL Save(CWK_Profile& wkp);
	void GenerateHTMLFile(CMediaGroup* pMediaGroup);

public:

private:
	CSecID	m_id;			// MediaID
	CString	m_sName;
	CString	m_sType;
	bool     m_bIsDefault;
	bool     m_bIsAudio;
	bool     m_bIsVideo;
	bool     m_bIsDisabled;
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
typedef CMedia* CMediaPtr;
WK_PTR_ARRAY(CMediaArray,CMediaPtr);
#endif
