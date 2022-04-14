// Measurement.cpp: Implementierung der Klasse CMeasurement.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Measurement.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CMeasurement, CObject,1);

CMeasurement::CMeasurement()
{
   m_Time = CTime::GetCurrentTime();
   m_dF1  = m_dF2 = 0;
   m_dT1  = m_dT2 = 0;
   m_ACData.SetMeasurement(this);
}

CMeasurement::CMeasurement(CAudioControlData& acd)
{
   m_Time = CTime::GetCurrentTime();
   m_ACData = acd;
   m_ACData.SetMeasurement(this);
   m_ACData.InitMeasurementParams();
   m_dF1 = m_dF2 = 0;
   m_dT1 = m_dT2 = 0;
}

CMeasurement::~CMeasurement()
{

}

void CMeasurement::Serialize(CArchive & ar)
{
   CObject::Serialize(ar);
   if (ar.IsStoring())
   {
      ar << m_Time;
      ar.Write(&m_ACData, CAudioControlData::GetDataSize());
      ar.Write(&m_Position, sizeof(CVector));
      if ((m_ACData.cMeaningOfTestValues == DB_VALUES_FROM_DATA_LOGGER) && m_ACData.bFree1)
      {
         ar.Write(&m_dT1, sizeof(double));
         ar.Write(&m_dT2, sizeof(double));
         ar.Write(&m_dF1, sizeof(double));
         ar.Write(&m_dF2, sizeof(double));
      }
   }
   else
   {
      ar >> m_Time;
      ar.Read(&m_ACData, CAudioControlData::GetDataSize());
      ar.Read(&m_Position, sizeof(CVector));
      if ((m_ACData.cMeaningOfTestValues == DB_VALUES_FROM_DATA_LOGGER) && m_ACData.bFree1)
      {
         ar.Read(&m_dT1, sizeof(double));
         ar.Read(&m_dT2, sizeof(double));
         ar.Read(&m_dF1, sizeof(double));
         ar.Read(&m_dF2, sizeof(double));
      }
   }
}
