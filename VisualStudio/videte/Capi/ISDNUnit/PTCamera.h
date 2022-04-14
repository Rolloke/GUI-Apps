
#ifndef _CPTCamera_H
#define _CPTCamera_H

#include "WK_Template.h"


class CPTCamera 
{
	// construction
public:
	inline CPTCamera(WORD wx);
	
	// attributes
public:	
	inline const CString &GetName() const;
	inline CSecID GetID() const;
	//
	inline WORD GetSizeX() const;
	inline WORD GetSizeY() const;
	//
	inline BOOL HasVideoSignal() const;
	inline Resolution GetResolution() const;

private:
	CString m_sName;
	CSecID m_id;
	//
	WORD m_wSizeX;
	WORD m_wSizeY;
	//
	BYTE m_byState;	// FF==VideoSignal, 00 nix

	friend class CPTBox;	// OOPS should be Set....
};

typedef CPTCamera * CPTCameraPtr;
WK_PTR_ARRAY_CS(CPTCamerasPlain,CPTCameraPtr,CPTCameraArray);

class CPTCameras : public CPTCameraArray
{
public:
	inline CPTCamera *GetCameraByID(CSecID id) const;
};

///////////////// inlined fns CPTCameras ////////////////////
inline CPTCamera *CPTCameras::GetCameraByID(CSecID id) const
{
	// NOT YET any cache
	// NOT YET or direct GetSubID test first
	for (int i=0;i<GetSize();i++) {
		if (GetAt(i)->GetID()==id) {
			return GetAt(i);	// <<< EXIT >>>
		}
	}
	return NULL;
}

///////////////// inlined fns CPTCamera ////////////////////
inline CPTCamera::CPTCamera(WORD wx)
{
	m_id.Set(SECID_GROUP_OUTPUT,wx);
	m_wSizeX =0;
	m_wSizeY =0;
	m_byState = 0;	// no video signal
	m_sName = "Unbekannt";
}
inline const CString &CPTCamera::GetName() const
{
	return m_sName;
}
inline CSecID CPTCamera::GetID() const
{
	return m_id;
}
//
inline WORD CPTCamera::GetSizeX() const
{
	return m_wSizeY;
}
inline WORD CPTCamera::GetSizeY() const
{
	return m_wSizeX;
}
//
inline BOOL CPTCamera::HasVideoSignal() const
{
	return (m_byState==(BYTE)0xFF);
}

inline Resolution CPTCamera::GetResolution() const
{

	// pt-200 erlaubt folgende Einstellungen
	// a) 88x72 176x144 352x288
	// b) 96x72 192x144 384x288
	// Dabei scheint b) guenstiger zu sein

	if (m_wSizeX==0) {
		// no trace in inlined fn.
		return RESOLUTION_NONE;
	} else if (m_wSizeX<100) {
		return RESOLUTION_QCIF;
	} else if (m_wSizeX<200) {
		return RESOLUTION_CIF;
	} else if (m_wSizeX<400) {
		return RESOLUTION_2CIF;
	} else {
		// no trace in inlined fn.
		return RESOLUTION_NONE;
	}
}

#endif