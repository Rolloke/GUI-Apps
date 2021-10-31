// AudioControlData.cpp: Implementierung der Klasse CAudioControlData.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AC_COM.h"
#include "resource.h"
#include "AudioControlData.h"
#include "AC_COMDlg.h"
#include "Curve.h" 
#include "MathCurve.h" 
#include "Plot.h"
#include "FileHeader.h" 
#include "Measurement.h"
#include <float.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
/*
#define TEST_VALUES_LEFT44          78
#define TEST_VALUES_LEFT60          78
#define TEST_VALUES_LEFT90          78
#define TEST_VALUES_DUMMYLEFT16    122
#define TEST_VALUES_CENTER44       198
#define TEST_VALUES_DUMMYCENTER16  242
#define TEST_VALUES_CENTER60       198
#define TEST_VALUES_RIGHT44        138
#define TEST_VALUES_DUMMYRIGHT16   182
#define TEST_VALUES_RIGHT60        138
#define TEST_VALUES_RIGHT90        168
#define DATA_END                   258


#define ABS_PROPAGATION_TIME_LEFT   85
#define ABS_PROPAGATION_TIME_CENTER 86
#define ABS_PROPAGATION_TIME_RIGHT  87

#define ACTUAL_SOUND_VALUES64       78
#define BEST_SOUND_VALUES64        142
#define SCHLECHPUNKTE_VALUES52     206
*/

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

const int CAudioControlData::gm_pnMilliSeconds[] =
{ 
     320,   640,  1280,   2560,  10240,                     // Minuten
   20480, 40960, 61440, 102400, 204800, 307200              // Stunden
};

CMathCurve *CAudioControlData::gm_pCorrValues            = NULL;
CMathCurve *CAudioControlData::gm_pFrqResponseCorrValues = NULL;
CAC_COMDlg *CAudioControlData::gm_pParent                = NULL;
double      CAudioControlData::gm_dNAN                   = sqrt(-1.0);

CAudioControlData::CAudioControlData()
{
   InitData();
}

CAudioControlData::~CAudioControlData()
{

}

void CAudioControlData::InitMeasurementParams()
{
   switch (cMeaningOfTestValues)
   {
      case SELF_TEST_AUTO_EQ:
         bFree1 = gm_pParent->m_BasicParam.m_bTrack9_ACD2;     // Frequenzgangkorrektur
         bFree2 = false;                                       // Calc Average mit true initialisieren
         bFree3 = true;                                        // Show Curves mit true initialisieren
         bFree4 = gm_pParent->m_BasicParam.m_bTrack10_ACD3;    // Track 10 der neuen ACD 20 - 200 Hz
         break;
      case VALUES_LEFT_SPEAKER:
      case VALUES_RIGHT_SPEAKER:
      case VALUES_CENTER_SPEAKER:
         bFree1 = false;                                       // hat hier keine Bedeutung
         bFree2 = true;                                        // Calc Average mit true initialisieren
         bFree3 = false;                                       // Show Curves mit false initialisieren
         bFree4 = false;                                       // hat hier keine Bedeutung
         break;
      case DB_VALUES_FROM_DATA_LOGGER:
         bFree1 = false;                                       // Als Frequenzgang anzeigen
         bFree2 = false;                                       // hat hier keine Bedeutung
         bFree3 = true;                                        // Show Curves mit false initialisieren
         bFree4 = false;                                       // Frequenzgang linear, logarithmisch (true, false)
         break;
      default:
         bFree1 = bFree2 = bFree3 = bFree4 = false;            // hat hier keine Bedeutung
         break;
   }
}

void CAudioControlData::CalculateChecksum(BYTE &cCS1, BYTE &cCS2)
{
   int i;
   BYTE *pszData = (BYTE*)this;
   cCS1 = pszData[2];
   cCS2 = 0;
   for (i=3; i<DATA_SIZE-2; i++) cCS1 ^= pszData[i];
   for (i=2; i<DATA_SIZE-2; i++) cCS2 += pszData[i];
}

void CAudioControlData::SaveParam(BYTE *pszSave, int nSize)
{
   char *pszData = (char*)this;
   memcpy(pszSave, &pszData[2], nSize);
}

void CAudioControlData::RestoreParam(BYTE *pszRestore, int nSize)
{
   char *pszData = (char*)this;
   memcpy(&pszData[2], pszRestore, nSize);
}
/*
CAudioControlData& CAudioControlData::operator=(CAudioControlData& ac)
{
   memcpy(this, &ac, DATA_SIZE);
   return *this;
}
*/
int CAudioControlData::GetNoOfValues(int n)
{
   switch(cMeaningOfTestValues)
   {
      case DB_VALUES_FROM_DATA_LOGGER: return 180;
      case VALUES_LEFT_SPEAKER: case VALUES_CENTER_SPEAKER: case VALUES_RIGHT_SPEAKER:
         return 22;
      case SELF_TEST_AUTO_EQ:
         if (bLoudspeakermodeSurround) return 60;
         else                          return 90;
      case AUTO_SOUND:
         if (n == 2) return 52;
         else        return 64;
      default:return 0;
   }
}

double CAudioControlData::GetValue(int nWhat, int nIndex, int nMod, double dFrequ)
{
   switch(cMeaningOfTestValues)
   {
      case VALUES_CENTER_SPEAKER: nIndex += 120; break;
      case VALUES_RIGHT_SPEAKER:  nIndex +=  60; break;
      case SELF_TEST_AUTO_EQ:
         if (bLoudspeakermodeSurround) nIndex += 60*nWhat;
         else                          nIndex += 90*nWhat;
         break;
      case AUTO_SOUND:
         if      (nWhat == 1) nIndex += 64;
         else if (nWhat == 2) nIndex += 128;
         break;
      default:break;
   }
   if ((nIndex >=0) && (nIndex < 180))
   {
      double dVal = (double) cDataArray[nIndex];
      if (nMod & CALC_DB_VALUES     ) dVal  = dVal / 3.0 + 35.0;
      if (nMod & CALC_TIME_VALUES   )
      {
         if (cDataArray[nIndex] == 255) dVal  = gm_dNAN;
         else                           dVal *= 10.0;
      }
      if (nMod & CALC_FRQ_CORRECTION)
      {
         dVal += FrqResponseCorrValue(dFrequ);
      }
      return dVal;
   }
   return 0;
}

void CAudioControlData::InitData()
{
   ZeroMemory(this, sizeof(*this));

   cCD_CodeParameter[0] = 0;
   cCD_CodeCommands[0]  = 0;
   cCD_CodeTrack[0]     = 0;

   cCdPlayerRoomA       =  4;
   cCdPlayerRoomB       = 10;
   cCdPlayerRoomC       = 14;
   nMemo                = 0x2;
   nSound               = 0x2;
   nInvQuality          = 0xF;
   nWeigting            = 0x5;
   bFree1               = false;
   bFree2               = false;
   bFree3               = false;
   bFree4               = false;
   nPeakHold            = 0x6;

   cNetwork_C1          = 0x7;
   cNetwork_C3          = 0x5;
   cNetwork_R2          = 0x2;
   cNetwork_L2          = 0x4;
   cNetwork_R3          = 0x1;
   cNetwork_L3          = 0x3;
   cNetwork_R1          = 0x5;
   cNetwork_C2          = 0x2;
   bNetworkSwST         = 1;
   bNetworkSwSP         = 0;
   bNetworkSwSM         = 0;
   bNetworkSwSX         = 0;
   nNetworkL1           = 0;
   bDummy1              = 0;
   bDummy2              = 0;
   bDummy3              = 0;
   bDummy4              = 0;
   cMeaningOfTestValues = NO_VALUES;
}

bool operator==(CAudioControlData& ac1, CAudioControlData& ac2)
{
   return (memcmp(&ac1, &ac2, 258) == 0) ? true : false;
}

bool operator!=(CAudioControlData& ac1, CAudioControlData& ac2)
{
   return (memcmp(&ac1, &ac2, 258) != 0) ? true : false;
}

int CAudioControlData::GetPeakHoldTime()
{
  if ((nPeakHold >= 5) && (nPeakHold <= 15)) return gm_pnMilliSeconds[nPeakHold-5];
  return 0;
}

double CAudioControlData::FrqCorrValue(double dX)
{
   if (gm_pCorrValues == NULL)
   {
      CFile file;
      CString strOpen = CAC_COMApp::gm_strStartDirectory + _T("Correct.CVE");
      if (!file.Open(strOpen, CFile::modeRead))        throw (int) NO_CORRECT_CURVE;
      gm_pCorrValues = new CMathCurve;
      if (!gm_pCorrValues)                             throw (int) NO_CORRECT_CURVE;
      if (!gm_pCorrValues->Read((HANDLE)file.m_hFile)) throw (int) NO_CORRECT_CURVE;
   }
   if (gm_pCorrValues)
   {
      return gm_pCorrValues->Get_Y_Value(dX);
   }
   return -1;
}

double CAudioControlData::FrqResponseCorrValue(double dX)
{
   if (gm_pFrqResponseCorrValues == NULL)
   {
      CFile file;
      CString strOpen = CAC_COMApp::gm_strStartDirectory + _T("FrqRspCo.CVE");
      if (!file.Open(strOpen, CFile::modeRead))        throw (int) NO_CORRECT_CURVE;
      gm_pFrqResponseCorrValues = new CMathCurve;
      if (!gm_pFrqResponseCorrValues)                             throw (int) NO_CORRECT_CURVE;
      if (!gm_pFrqResponseCorrValues->Read((HANDLE)file.m_hFile)) throw (int) NO_CORRECT_CURVE;
   }
   if (gm_pFrqResponseCorrValues)
   {
      return gm_pFrqResponseCorrValues->Get_Y_Value(dX);
   }
   return -1;
}

void CAudioControlData::DeleteCorrectionValues()
{
   if (gm_pCorrValues)
   {
      delete gm_pCorrValues;
      gm_pCorrValues = NULL;
   }
   if (gm_pFrqResponseCorrValues)
   {
      delete gm_pFrqResponseCorrValues;
      gm_pFrqResponseCorrValues = NULL;
   }
}

int CAudioControlData::GetPlots(CPlot **ppIn, int &nPlots, bool &bAutoRange, int *pnStartCurve, int nMeasurement)
{
   CCurve  *pCurve, *pCurveB;
   double   dTemp;
   int      nStartCurve[3] = {0, 1, 2};
   if (!pnStartCurve) pnStartCurve = (int*)nStartCurve;
   if (ppIn == NULL) return 1;

   CString  strSpace(" "), str;
   int      i, nDiv = 0;
   int      nValues, nTimeStep = 0;

   switch (gm_pParent->m_BasicParam.m_nDivision)
   {
      case 0: nDiv = CCURVE_LINEAR;      break;
      case 1: nDiv = CCURVE_LOGARITHMIC; break;
      case 2: nDiv = CCURVE_FREQUENCY;   break;
   }

   CPlot *pIn = *ppIn;

   switch ((int)cMeaningOfTestValues)
   {
      case DB_VALUES_FROM_DATA_LOGGER:
      {
         bool bFrequencyResponse = bFree1 && (m_pM != NULL) ? true : false;
         bool bLinear            = bFree4;
         int nStartValue = 0, nStopValue = 0;
         if (!pIn)
         {
            nPlots = 1;
            *ppIn = pIn = new CPlot[nPlots];
            pIn[0].SetSize(1);
            if (bFrequencyResponse)
            {
               if (bLinear) pIn[0].SetXDivision(CCURVE_LINEAR);
               else         pIn[0].SetXDivision(nDiv);
            }
            else            pIn[0].SetXDivision(CCURVE_LINEAR);
         }
         nTimeStep = GetPeakHoldTime();
         pCurve = pIn[0].GetCurve(pnStartCurve[0]);
         if (bFrequencyResponse)
         {
            nStartValue = (int)(0.5 +(m_pM->m_dT1 / (0.001*nTimeStep)));
            nStopValue  = (int)(0.5 +(m_pM->m_dT2 / (0.001*nTimeStep)));
            nValues     = nStopValue - nStartValue;

            if (nValues > 180)
            {
               nStartValue += 180-nValues;
               nValues = 180;
               ASSERT(FALSE);
            }

            if (!SetCurveParams(pCurve, gm_pParent->m_strNewName, "Hz", "dB", nValues, true, m_pM->m_dF1, m_pM->m_dF2))
               return ERROR_SETCURVE;
            for (i=0; i<nValues; i++)
            {
               if (!pCurve->SetY_Value(i, GetValue(0, i+nStartValue, CALC_DB_VALUES|CALC_FRQ_CORRECTION, pCurve->GetX_Value(i)))) return ERROR_SETVALUE;
            }
            RANGE r;
            r.y1 = gm_pParent->m_BasicParam.m_Range1.y1;
            r.y2 = gm_pParent->m_BasicParam.m_Range1.y2;
            r.x1 = m_pM->m_dF1;
            r.x2 = m_pM->m_dF2;
            pIn[0].SetDefaultRange(r);
         }
         else
         {
            nValues = GetNoOfValues(DB_VALUES_FROM_DATA_LOGGER);
            if (!SetCurveParams(pCurve, gm_pParent->m_strNewName, "s", "dB", nValues, false, 0.001*nTimeStep, (nTimeStep != 0) ? 0.001*nTimeStep : 1.0))
               return ERROR_SETCURVE;
            for (i=0; i<nValues; i++)
            {
               if (!pCurve->SetY_Value(i, GetValue(0, i, CALC_DB_VALUES, 0))) return ERROR_SETVALUE;
            }
            pIn[0].DeterminRange();
            RANGE r = pIn[0].GetDefaultRange();
            r.y1 = gm_pParent->m_BasicParam.m_Range1.y1, r.y2 = gm_pParent->m_BasicParam.m_Range1.y2;
            pIn[0].SetDefaultRange(r);
         }
      }break;

      case VALUES_LEFT_SPEAKER:
      case VALUES_CENTER_SPEAKER:
      case VALUES_RIGHT_SPEAKER:
      {
         int nValue;
         if (!pIn)
         {
            nPlots = 2;
            *ppIn = pIn = new CPlot[nPlots];
            pIn[0].SetSize(1);
            pIn[1].SetSize(1);
            pIn[0].SetXDivision(nDiv);
            pIn[1].SetXDivision(nDiv);
         }
         if (nPlots > 1)
         {
            str.LoadString(IDS_REVERBERATION_TIME);
            pIn[1].SetHeading(str);
         }
         pCurve  = pIn[0].GetCurve(pnStartCurve[0]);
         pCurveB = pIn[1].GetCurve(pnStartCurve[0]);
         if      (cMeaningOfTestValues == VALUES_LEFT_SPEAKER)   str.Format(IDS_LEFT  , nMeasurement);
         else if (cMeaningOfTestValues == VALUES_CENTER_SPEAKER) str.Format(IDS_CENTER, nMeasurement);
         else if (cMeaningOfTestValues == VALUES_RIGHT_SPEAKER ) str.Format(IDS_RIGHT , nMeasurement);

         nValues = GetNoOfValues();
         str = gm_pParent->m_strNewName + strSpace + str;

         if (!SetCurveParams(pCurve, str, "Hz", "dB", nValues, true, 100, 12500, true, CCURVE_LOGARITHMIC))
            return ERROR_SETCURVE;
         if (!SetCurveParams(pCurveB, str, "Hz", "ms", nValues, true, 100, 12500, true, CCURVE_LOGARITHMIC))
            return ERROR_SETCURVE;

         for (i=0,nValue=0; nValue<nValues; nValue++)
         {
            if (!pCurve->SetY_Value(nValue, GetValue(-1, i++, CALC_DB_VALUES|CALC_FRQ_CORRECTION, pCurve->GetX_Value(nValue)))) return ERROR_SETVALUE;
            dTemp = GetValue(-1, i++, CALC_TIME_VALUES, 0);
            if (_isnan(dTemp)) pCurveB->SetInterupted(true);
            if (!pCurveB->SetY_Value(nValue, dTemp)) return ERROR_SETVALUE;
         }
         if (bAutoRange)
         {
            RANGE r = gm_pParent->m_BasicParam.m_Range2;
            pIn[1].SetDefaultRange(r);
            r.y1 = gm_pParent->m_BasicParam.m_Range1.y1, r.y2 = gm_pParent->m_BasicParam.m_Range1.y2;
            pIn[0].SetDefaultRange(r);
            bAutoRange = false;
         }
      } break;

      case SELF_TEST_AUTO_EQ:                               // Lautsprecherfrequenzgänge
      {
         double dF2 = 20000;
         if (bFree4) dF2 = 200;
         if (bLoudspeakermodeSurround)                      // Surround
         {
            double dT = 10.0 / 59.0;
            if (!pIn)
            {
               int i, nSize = 0;
               if (pnStartCurve != (int*)nStartCurve)
               {
                  for (i=0; i<3; i++) if (pnStartCurve[0]!=-1) nSize++;
               }
               else nSize = 3;
               nPlots = 1;
               *ppIn = pIn = new CPlot[nPlots];
               pIn[0].SetSize(nSize);
               pIn[0].SetXDivision(nDiv);
            }
            nValues = GetNoOfValues();
            pCurve = pIn[0].GetCurve(pnStartCurve[0]);
            if (pCurve)
            {
               str.Format(IDS_LEFT, nMeasurement);
               str = gm_pParent->m_strNewName + strSpace + str;
               if (!SetCurveParams(pCurve, str, "Hz", "dB", nValues, true, 20, dF2, !bFree1, CCURVE_LOGARITHMIC))
                  return ERROR_SETCURVE;
               for (i=0; i<nValues; i++)
               {
                  if (bFree1) pCurve->SetX_Value(i, FrqCorrValue(i*dT));
                  if (!pCurve->SetY_Value(i, GetValue(0, i, CALC_DB_VALUES|CALC_FRQ_CORRECTION, pCurve->GetX_Value(i)))) return ERROR_SETVALUE;
               }
            }
            pCurve = pIn[0].GetCurve(pnStartCurve[1]);;
            if (pCurve)
            {
               str.Format(IDS_RIGHT, nMeasurement);
               str = gm_pParent->m_strNewName + strSpace + str;
               if (!SetCurveParams(pCurve, str, "Hz", "dB", nValues, true, 20, dF2, !bFree1, CCURVE_LOGARITHMIC))
                  return ERROR_SETCURVE;
               for (i=0; i<nValues; i++)
               {
                  if (bFree1) pCurve->SetX_Value(i, FrqCorrValue(i*dT));
                  if (!pCurve->SetY_Value(i, GetValue(1, i, CALC_DB_VALUES|CALC_FRQ_CORRECTION, pCurve->GetX_Value(i)))) return ERROR_SETVALUE;
               }
            }
            pCurve = pIn[0].GetCurve(pnStartCurve[2]);;
            if (pCurve)
            {
               str.Format(IDS_CENTER, nMeasurement);
               str = gm_pParent->m_strNewName + strSpace + str;
               if (!SetCurveParams(pCurve, str, "Hz", "dB", nValues, true, 20, dF2, !bFree1, CCURVE_LOGARITHMIC))
                  return ERROR_SETCURVE;
               for (i=0; i<nValues; i++)
               {
                  if (bFree1) pCurve->SetX_Value(i, FrqCorrValue(i*dT));
                  if (!pCurve->SetY_Value(i, GetValue(2, i, CALC_DB_VALUES|CALC_FRQ_CORRECTION, pCurve->GetX_Value(i)))) return ERROR_SETVALUE;
               }
            }
         }
         else                                               // Stereo
         {
            double dT = 10.0 / 89.0;
            if (!pIn)
            {
               int i, nSize = 0;
               if (pnStartCurve != (int*)nStartCurve)
               {
                  for (i=0; i<3; i++) if (pnStartCurve[0]!=-1) nSize++;
               }
               else nSize = 2;

               nPlots = 1;
               *ppIn = pIn = new CPlot[nPlots];
               pIn[0].SetSize(nSize);
               pIn[0].SetXDivision(nDiv);
            }
            nValues = GetNoOfValues();
            pCurve = pIn[0].GetCurve(pnStartCurve[0]);
            if (pCurve)
            {
               str.Format(IDS_LEFT, nMeasurement);
               str = gm_pParent->m_strNewName + strSpace + str;
               if (!SetCurveParams(pCurve, str, "Hz", "dB", nValues, true, 20, dF2, !bFree1, CCURVE_LOGARITHMIC))
                  return ERROR_SETCURVE;
               for (i=0; i<nValues; i++)
               {
                  if (bFree1) pCurve->SetX_Value(i, FrqCorrValue(i*dT));
                  if (!pCurve->SetY_Value(i, GetValue(0, i, CALC_DB_VALUES|CALC_FRQ_CORRECTION, pCurve->GetX_Value(i)))) return ERROR_SETVALUE;
               }
            }
            pCurve = pIn[0].GetCurve(pnStartCurve[1]);
            if (pCurve)
            {
               str.Format(IDS_RIGHT, nMeasurement);
               str = gm_pParent->m_strNewName + strSpace + str;
               if (!SetCurveParams(pCurve, str, "Hz", "dB", nValues, true, 20, dF2, !bFree1, CCURVE_LOGARITHMIC))
                  return ERROR_SETCURVE;
               for (i=0; i<nValues; i++)
               {
                  if (bFree1) pCurve->SetX_Value(i, FrqCorrValue(i*dT));
                  if (!pCurve->SetY_Value(i, GetValue(1, i, CALC_DB_VALUES|CALC_FRQ_CORRECTION, pCurve->GetX_Value(i)))) return ERROR_SETVALUE;
               }
            }
         }
         if (bAutoRange)
         {
            pIn[0].SetDefaultRange(gm_pParent->m_BasicParam.m_Range1);
            bAutoRange = false;
         }
      } break;

      case AUTO_BALANCE_FUNCTION:                           // Keine Messkurven !
      {
         CString strCaption;
         strCaption.LoadString(IDS_MEASUREMENT_VALUES);
         gm_pParent->m_cMeaningOfTestVal.GetLBText(cMeaningOfTestValues, str); 
         strCaption += strSpace + str;
         str.Format(IDS_AUTOBALANCE_RESULTS,
            GetValue(0, TEST_VALUE_LEFT,0,0),               // Lautstärken
            GetValue(0, TEST_VALUE_RIGHT,0,0),
            GetValue(0, TEST_VALUE_CENTER,0,0),
            GetValue(0, TEST_VALUE_AVG_LRC,0,0),            // Lautstärken (Mittelwerte)
            (char)GetValue(0, TEST_VALUE_AVG_LEFT,0,0),
            (char)GetValue(0, TEST_VALUE_AVG_RIGHT,0,0),
            (char)GetValue(0, TEST_VALUE_AVG_CENTER,0,0),
            cPropagationTimeLeft,                           // Laufzeiten
            cPropagationTimeCenter,
            cPropagationTimeRight);
// Testen, zwischen 1,..,63 gültige Werte ? Sonderfall 1,1,1 : Fehler
//               m_AudioCtrlData[ABS_PROPAGATION_TIME_LEFT],   // Absolute Laufzeiten
//               m_AudioCtrlData[ABS_PROPAGATION_TIME_CENTER],
//               m_AudioCtrlData[ABS_PROPAGATION_TIME_RIGHT]);
         ::MessageBox(gm_pParent->m_hWnd, str, strCaption, MB_OK|MB_ICONINFORMATION);
      }   break;

      case AUTO_SOUND:                                      // Messkurven ? (testen !!!)
      {
         if (!pIn)
         {
            nPlots = 2;
            *ppIn = pIn = new CPlot[nPlots];
            pIn[0].SetSize(2);
            pIn[0].SetXDivision(nDiv);
         }
         if (pnStartCurve == (int*)nStartCurve)
         {
            pnStartCurve[2] = 0;
         }
         pCurve = pIn[0].GetCurve(pnStartCurve[0]);
         if (pCurve)
         {
            nValues = GetNoOfValues(0);
            str.Format(IDS_AC_AUTOSOUND_NO, nMeasurement);
            if (!SetCurveParams(pCurve, str, "Hz", "dB", nValues, true, 100, 12500, true, CCURVE_LOGARITHMIC))
                  return ERROR_SETCURVE;
            for (i=0; i<nValues; i++)
            {
               if (!pCurve->SetY_Value(i, GetValue(0, i, CALC_DB_VALUES|CALC_FRQ_CORRECTION, pCurve->GetX_Value(i)))) return ERROR_SETVALUE;
            }
         }
         pCurve = pIn[0].GetCurve(pnStartCurve[1]);
         if (pCurve)
         {
            str.Format(IDS_AC_AUTOSOUND_BEST);
            nValues = GetNoOfValues(1);
            if (!SetCurveParams(pCurve, str, "Hz", "dB", nValues, true, 100, 12500, true, CCURVE_LOGARITHMIC))
                  return ERROR_SETCURVE;
            for (i=0; i<nValues; i++)
            {
               if (!pCurve->SetY_Value(i, GetValue(1, i, CALC_DB_VALUES|CALC_FRQ_CORRECTION, pCurve->GetX_Value(i)))) return ERROR_SETVALUE;
            }
         }

         pIn[1].SetSize(1);
         pCurve = pIn[1].GetCurve(pnStartCurve[2]);
         if (pCurve)
         {
            str.LoadString(IDS_AC_QUALITY_REFERENCE_NO);
            nValues = GetNoOfValues(2);
            if (!SetCurveParams(pCurve, str, "n", "Q", nValues, false, 1, 1))
                  return ERROR_SETCURVE;
            for (i=0; i<nValues; i++)
            {
               if (!pCurve->SetY_Value(i, 0.0625*GetValue(2, i, 0, 0))) return ERROR_SETVALUE;
            }
         }
         if (bAutoRange)
         {
            RANGE r = gm_pParent->m_BasicParam.m_Range2;
            r.y1 = gm_pParent->m_BasicParam.m_Range1.y1, r.y2 = gm_pParent->m_BasicParam.m_Range1.y2;
            pIn[0].SetDefaultRange(r);
            r.y1 = -0.5, r.y2 = 16.5;
            r.x1 = 0   , r.x2 = nValues+1;
            pIn[1].SetDefaultRange(r);
            bAutoRange = false;
         }
      } break;

      case NO_VALUES: default: return NO_VALUES;
   }
   return 0;
}

bool CAudioControlData::SetCurveParams(CCurve *pcurve, const char *szNewName, const char *szUnitX, const char *szUnitY, int nSize, bool bAuto, double dOrigin, double dStep, bool bSingle, int nXDiv)
{
   if (pcurve)
   {
      if (bSingle) pcurve->SetFormat(CF_SINGLE_DOUBLE);
      else         pcurve->SetFormat(CF_DOUBLE);
      if (nXDiv != -1) pcurve->SetXDivision(nXDiv);
      if (!pcurve->SetSize(nSize)) return false;
      pcurve->SetDescription((char*)szNewName);
      pcurve->SetXUnit((char*)szUnitX);
      pcurve->SetYUnit((char*)szUnitY);
      if (bAuto)
      {
         pcurve->SetX_Value(0, dOrigin);
         pcurve->SetX_Value(nSize-1, dStep);
      }
      else
      {
         pcurve->SetXOrigin(dOrigin);
         pcurve->SetXStep(dStep);
      }
      return true;
   }
   return false;
}

BOOL CALLBACK PostMsgThreadWindow(HWND hwnd, LPARAM lParam)
{
  DWORD *pnArr = (DWORD*) lParam;
  ::SendMessageTimeout(hwnd, WM_APP + 17, (WPARAM)pnArr[0], (LPARAM)pnArr[1], SMTO_ABORTIFHUNG, 1000, &pnArr[2]);
  return 1;
}

void CAudioControlData::EditFrequencyResponseCorr()
{
   bool bEditCurves = CAC_COMApp::gm_bEditCurves;
   CAC_COMApp::gm_bEditCurves = true;
   try
   {
      CPlot plot;
      CCurve *pCurve = NULL;
                                                               // Temp-Pfad für das Editieren der Korrekturkurve
      if (!CAC_COMApp::gm_strEditFileName.IsEmpty())  throw (int) EDIT_CORRECT_CURVE_ALREADY;
/*
      {
         FrqResponseCorrValue(20);
         pCurve = gm_pFrqResponseCorrValues;

         char        szTempPath[MAX_PATH*2];
         char        szCaraViewFileName[MAX_PATH*2];
         HANDLE      hFile;

         CFileHeader fh("CRV", "2001", 100);
         GetTempPath( MAX_PATH*2, szTempPath );                      // Pfad der Übergabedatei basteln
         GetTempFileName(szTempPath, "C2D", 0, szCaraViewFileName);
         hFile = CreateFile(szCaraViewFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
         fh.CalcChecksum();
         if( hFile != INVALID_HANDLE_VALUE )                         // Datei geöffnet
         {
            DWORD dwDummy;
            srand((unsigned)time(NULL));
            for (int j=0; j<pCurve->GetNumValues(); j++)
               pCurve->SetY_Value(j, 0.001*rand());
            pCurve->SetTargetFuncNo(1);
            pCurve->GetChecksum(&fh);
            WriteFile(hFile, (LPCVOID) &fh, sizeof(CFileHeader), (LPDWORD) &dwDummy, NULL);
            pCurve->Write(hFile);
            CloseHandle(hFile);
            int i, nSize = strlen(szCaraViewFileName);
            for (i=nSize-1; i>0; i--)
            {
               if (szCaraViewFileName[i] == '.') szCaraViewFileName[i]=0;
               if (szCaraViewFileName[i] == '\\') break;
            }
            PROCESS_INFORMATION *pi;
            POSITION pos = gm_pParent->m_CalledProcess.GetHeadPosition();
            if (pos)
            {
               pi= (PROCESS_INFORMATION*) gm_pParent->m_CalledProcess.GetNext(pos);
               if ((pi != NULL) && (pi->hProcess))
               {
                  DWORD   exitcode;
                  if(GetExitCodeProcess(pi->hProcess, &exitcode))
                  {
                     if(exitcode == STILL_ACTIVE)
                     {
                        long pnArr[3] = {0, 0, 0};
                        char *pArr= (char*)pnArr;
                        memcpy(pArr, &szCaraViewFileName[i+1], 8);
                        ::EnumThreadWindows(pi->dwThreadId, PostMsgThreadWindow, (LPARAM)pnArr);
                     }
                  }
               }
            }
         }
         return;
      }
*/
      FrqResponseCorrValue(20);                                // Kurve lesen (automatisch)
      if (!plot.SetSize(1)) throw (int) ERROR_SETCURVE;        // neue Kurve allocieren
      plot.SetDefaultRange(20, 20000, -10, 10);                // Editier bereich +/- 10 dB
      pCurve = plot.GetCurve(0);
      if (!pCurve)  throw (int) ERROR_SETCURVE;
      gm_pFrqResponseCorrValues->SetTargetFuncNo(1);           // Funktion nummerieren
      pCurve->RobAllParameter(*gm_pFrqResponseCorrValues);     // Array und Parameter holen
      pCurve->SetColor(0xffffffff);
      plot.SetHeading(pCurve->GetDescription());
      delete gm_pFrqResponseCorrValues;                        // löschen ! 
      gm_pFrqResponseCorrValues = NULL;

      gm_pParent->ShowPlot(1, &plot, false);                   // ETS2DView aufrufen
   }
   catch (int nError)
   {
      if (nError == NO_CORRECT_CURVE)
         AfxMessageBox(IDS_NO_CORRECT_CURVE, MB_OK|MB_ICONSTOP);
   }
   CAC_COMApp::gm_bEditCurves = bEditCurves;
}


void CAudioControlData::Return2DFrequencyResponseCorr(LPARAM lParam)
{
   CPlot *pPlot = NULL;
   try
   {
      CFile file;
      int  nPlots = 0;
      bool bCRC_OK;
      CCurve *pCurve;
      if (lParam == 1)
      {
         if (CAC_COMApp::gm_strEditFileName.IsEmpty())      throw (int) 20;
         pPlot = ReadPlots(CAC_COMApp::gm_strEditFileName, nPlots, bCRC_OK);
         if ((pPlot == NULL) || (nPlots != 1) || !bCRC_OK)  throw (int) 21;

         pCurve = pPlot->GetCurve(0);
         if (pCurve == NULL)                                throw (int) 22;
         if (pCurve->GetTargetFuncNo() != 1)                throw (int) 23;

         CString strOpen = CAC_COMApp::gm_strStartDirectory + _T("FrqRspCo.CVE");
         if (!file.Open(strOpen, CFile::modeCreate | CFile::modeWrite)) throw (int) 24;
         pPlot->GetCurve(0)->Write((HANDLE)file.m_hFile);
         file.Close();
      }
      ::DeleteFile(CAC_COMApp::gm_strEditFileName);
      CAC_COMApp::gm_strEditFileName.Empty();
   }
   catch (int nError)
   {
      if ((nError == 24) || (nError == 21))
      {
         CString str;
         str.Format(IDS_ERROR_OCCURRED, ::GetLastError(), nError);
         AfxMessageBox(str, MB_OK|MB_ICONSTOP);
      }
   }
   if (pPlot) delete[] pPlot;
}
