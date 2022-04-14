/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcMediaRecord.h $
// ARCHIVE:		$Archive: /Project/CIPC/CipcMediaRecord.h $
// CHECKIN:		$Date: 7.07.04 17:01 $
// VERSION:		$Revision: 15 $
// LAST CHANGE:	$Modtime: 7.07.04 16:46 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __CIPC_MEDIA_RECORD_H
#define __CIPC_MEDIA_RECORD_H

#include "SecID.h"

////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////
//  CIPCMediaRecord | The class encapsulates all necessary information
// about one MediaChannnel.
////////////////////////////////////////////////////////////////////////////
	
class AFX_EXT_CLASS CIPCMediaRecord : public CObject
{
	DECLARE_DYNAMIC(CIPCMediaRecord)		
public:
	struct MediaRecordFlags
	{
		void Reset()
		{
			ASSERT(sizeof(*this) == sizeof(DWORD));
			ZeroMemory(this, sizeof(*this));
		}
		void Set(DWORD dw)
		{
			ASSERT(sizeof(*this) == sizeof(DWORD));
			memcpy(this, &dw, sizeof(*this));
		}
		operator DWORD()
		{
			DWORD dw;
			ASSERT(sizeof(*this) == sizeof(DWORD));
			memcpy(&dw, this, sizeof(*this));
			return dw;
		}
	public:
		DWORD IsOkay               : 1; // 1
		DWORD IsEnabled            : 1; // 2
		DWORD IsInput              : 1; // 3
		DWORD IsOutput             : 1; // 4
		DWORD IsAudio              : 1; // 5
		DWORD IsVideo              : 1; // 6
		DWORD IsDefault            : 1; // 7
		DWORD IsCapturing          : 1; // 8
		DWORD IsRendering          : 1; // 9
		DWORD IsCaptureInitialized : 1; // 10
		DWORD IsRenderInitialized  : 1; // 11
		DWORD Dummy : 21;
	};

	inline CIPCMediaRecord();
	inline CIPCMediaRecord(const CIPCMediaRecord& rec);
	inline const CIPCMediaRecord & operator=(const CIPCMediaRecord& rec);

	// access attributes
public:
	inline const CString &GetName() const;
	inline CSecID GetID() const;
	inline DWORD GetFlags();
	inline BOOL	IsOkay() const;					// Returns the Hardware state
	inline BOOL	IsEnabled() const;				// Returns whether the MediaChannel is enabled
	inline BOOL	IsInput() const;				// Returns whether the MediaChannel is an input channel
	inline BOOL	IsOutput() const;				// Returns whether the MediaChannel is the default device
	inline BOOL	IsAudio() const;				// Returns whether the MediaChannel is Audio
	inline BOOL	IsVideo() const;				// Returns whether the MediaChannel is Video
	inline BOOL	IsDefault() const;				// Returns whether the MediaChannel is the default device
	inline BOOL	IsCapturing() const;			// Returns whether the device is capturing now
	inline BOOL	IsRendering() const;			// Returns whether the device is rendering now
	inline BOOL	IsCaptureInitialized() const;	// Returns whether the capture device is initialized
	inline BOOL	IsRenderInitialized() const;	// Returns whether the render device is initialized

	// access operations
public:
	inline void Set(const CString &s, CSecID id, DWORD Flags);
	inline void SetFlags(DWORD dwNewFlags);
	
	inline void SetDefault(bool bDefault);
	inline void SetAudio(bool bAuio);
	inline void SetVideo(bool bVideo);
	inline void SetCapturing(bool bCapturing);
	inline void SetRendering(bool bRendering);
	inline void SetCaptureInitialized(bool bInitialized);
	inline void SetRenderInitialized(bool bInitialized);
	inline void SetEnabled(bool bEnabled);

	inline BOOL operator != (const CIPCMediaRecord& or);
	
private:
	// data:
	CString  m_sName;
	CSecID	 m_id;
	MediaRecordFlags m_Flags;
};

///////////////////////////////////////////////////////////////////
typedef CIPCMediaRecord  *CIPCMediaRecordPtr;
WK_PTR_ARRAY(CIPCMediaRecordArray,CIPCMediaRecordPtr)
///////////////// inlined functions ///////////////////////

inline CIPCMediaRecord::CIPCMediaRecord()
{
	// m_sName;
	// m_id;
	m_Flags.Set(0);	// not enabled
}
/*********************************************************************************************
 Class      : CIPCMediaRecord
 Function   : CIPCMediaRecord
 Description: Copy Contructor

 Parameters:   
  rec: (E): MediaRecord to Copy  (const CIPCMediaRecord&)

 Result type:  (inline)
 created: September, 23 2003
 <TITLE CIPCMediaRecord>
*********************************************************************************************/
inline CIPCMediaRecord::CIPCMediaRecord(const CIPCMediaRecord& rec)
{
	m_sName  = rec.m_sName;
	m_id     = rec.m_id;
	m_Flags  = rec.m_Flags;	
}
/*********************************************************************************************
 Class      : &CIPCMediaRecord
 Function   : operator=
 Description: Assignement Operator

 Parameters:   
  rec: (E): MediaRecord to assign with  (const CIPCMediaRecord&)

 Result type:  (inline const CIPCMediaRecord)
 created: September, 23 2003
 <TITLE operator=>
*********************************************************************************************/
inline const CIPCMediaRecord &CIPCMediaRecord::operator=(const CIPCMediaRecord& rec)
{
	m_sName   = rec.m_sName;
	m_id      = rec.m_id;
	m_Flags   = rec.m_Flags;
	return *this;
}

/*********************************************************************************************
 Class      : &CIPCMediaRecord
 Function   : GetName
 Description: Returns the Name of the MediaRecord

 Parameters: None 

 Result type: The Name (inline const CString)
 created: September, 23 2003
 <TITLE GetName>
*********************************************************************************************/
inline const CString &CIPCMediaRecord::GetName() const
{
	return m_sName;
}

/*********************************************************************************************
 Class      : CIPCMediaRecord
 Function   : GetID
 Description: Returns the ID of the Record

 Parameters: None 

 Result type: Security ID (inline CSecID)
 created: September, 23 2003
 <TITLE GetID>
*********************************************************************************************/
inline CSecID CIPCMediaRecord::GetID() const
{
	return m_id;
}
//////////////////////////////////////////////////////////////////////////
inline BOOL	CIPCMediaRecord::IsOkay() const
{
	return m_Flags.IsOkay;
}
//////////////////////////////////////////////////////////////////////////
inline BOOL	CIPCMediaRecord::IsEnabled() const
{
	return m_Flags.IsEnabled;
}
//////////////////////////////////////////////////////////////////////////
inline BOOL	CIPCMediaRecord::IsInput() const
{
	return m_Flags.IsInput;
}
//////////////////////////////////////////////////////////////////////////
inline BOOL	CIPCMediaRecord::IsOutput() const
{
	return m_Flags.IsOutput;
}
//////////////////////////////////////////////////////////////////////////
inline BOOL	CIPCMediaRecord::IsAudio() const
{
	return m_Flags.IsAudio;
}
//////////////////////////////////////////////////////////////////////////
inline BOOL	CIPCMediaRecord::IsVideo() const
{
	return m_Flags.IsVideo;
}
//////////////////////////////////////////////////////////////////////////
inline BOOL	CIPCMediaRecord::IsDefault() const
{
	return m_Flags.IsDefault;
}
//////////////////////////////////////////////////////////////////////////
inline BOOL	CIPCMediaRecord::IsCapturing() const
{
	return m_Flags.IsCapturing;
}
//////////////////////////////////////////////////////////////////////////
inline BOOL	CIPCMediaRecord::IsRendering() const
{
	return m_Flags.IsRendering;
}
//////////////////////////////////////////////////////////////////////////
inline BOOL	CIPCMediaRecord::IsCaptureInitialized() const
{
	return m_Flags.IsCaptureInitialized;
}
//////////////////////////////////////////////////////////////////////////
inline BOOL	CIPCMediaRecord::IsRenderInitialized() const
{
	return m_Flags.IsRenderInitialized;
}
//////////////////////////////////////////////////////////////////////////
inline void CIPCMediaRecord::SetDefault(bool bDefault)
{
	m_Flags.IsDefault = bDefault;
}
//////////////////////////////////////////////////////////////////////////
inline void CIPCMediaRecord::SetAudio(bool bAudio)
{
	m_Flags.IsAudio = bAudio;
}
//////////////////////////////////////////////////////////////////////////
inline void CIPCMediaRecord::SetVideo(bool bVideo)
{
	m_Flags.IsVideo = bVideo;
}
//////////////////////////////////////////////////////////////////////////
inline void CIPCMediaRecord::SetCapturing(bool bCapturing)
{
	m_Flags.IsCapturing = bCapturing;
}
//////////////////////////////////////////////////////////////////////////
inline void CIPCMediaRecord::SetRendering(bool bRendering)
{
	m_Flags.IsRendering = bRendering;
}
//////////////////////////////////////////////////////////////////////////
inline void CIPCMediaRecord::SetCaptureInitialized(bool bInitialized)
{
	m_Flags.IsCaptureInitialized = bInitialized;
}
//////////////////////////////////////////////////////////////////////////
inline void CIPCMediaRecord::SetRenderInitialized(bool bInitialized)
{
	m_Flags.IsRenderInitialized = bInitialized;
}
inline void CIPCMediaRecord::SetEnabled(bool bEnabled)
{
	m_Flags.IsEnabled = bEnabled;
}

/*********************************************************************************************
 Class      : CIPCMediaRecord
 Function   : Set
 Description: Sets the Parameters of the MediaRecord

 Parameters:   
  s    : (E): Name  (const CString&)
  id   : (E): Security ID  (CSecID)
  flags: (E): Flags  (DWORD)

 Result type:  (inline void)
 created: September, 23 2003
 <TITLE Set>
*********************************************************************************************/
inline void CIPCMediaRecord::Set(const CString &s, CSecID id, DWORD Flags)
{
	m_sName = s;
	m_id = id;
	m_Flags.Set(Flags);
}
/*********************************************************************************************
 Class      : CIPCMediaRecord
 Function   : SetFlags
 Description: Sets the Flags 

 Parameters:   
  flags: (E): Flags  (DWORD)

 Result type:  (inline void)
 created: September, 23 2003
 <TITLE SetFlags>
*********************************************************************************************/
inline void CIPCMediaRecord::SetFlags(DWORD Flags)
{
	m_Flags.Set(Flags);
}

inline DWORD CIPCMediaRecord::GetFlags()
{
	return m_Flags;
}

/*********************************************************************************************
 Class      : CIPCMediaRecord
 Function   : operator != 
 Description: Comparison Operator for MediaRecords

 Parameters:   
  mr: (E): MediaRecord to compare with  (const CIPCMediaRecord&)

 Result type: (TRUE, FALSE) (inline BOOL)
 created: September, 23 2003
 <TITLE operator != >
*********************************************************************************************/
inline BOOL CIPCMediaRecord::operator != (const CIPCMediaRecord& mr)
{
	return   m_sName != mr.m_sName
		   || m_id   != mr.m_id
		   || memcmp(&m_Flags, &mr.m_Flags, sizeof(MediaRecordFlags)) == 0;

}

#endif
