// Measurement.h: Schnittstelle für die Klasse CMeasurement.
//
//////////////////////////////////////////////////////////////////////
#include <afxtempl.h>

#include "AudioControlData.h"
#include "CVector.h"

#if !defined(AFX_MEASUREMENT_H__0E624FAB_EB97_11D4_87C5_0000B48B0FC3__INCLUDED_)
#define AFX_MEASUREMENT_H__0E624FAB_EB97_11D4_87C5_0000B48B0FC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMeasurement : public CObject
{
   DECLARE_SERIAL(CMeasurement)

public:
	CMeasurement();
	CMeasurement(CAudioControlData&);
	virtual ~CMeasurement();
   virtual void  Serialize(CArchive &);

   CTime             m_Time;
   CVector           m_Position;
   CAudioControlData m_ACData;
   double            m_dT1, m_dT2;
   double            m_dF1, m_dF2;
};

// Definition einer TypedPtrList
typedef CTypedPtrList < CObList, CMeasurement* > CMeasurementList;

#endif // !defined(AFX_MEASUREMENT_H__0E624FAB_EB97_11D4_87C5_0000B48B0FC3__INCLUDED_)
