// AudioControlData.h: Schnittstelle für die Klasse CAudioControlData.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUDIOCONTROLDATA_H__B0A5DFC0_E877_11D4_87C5_0000B48B0FC3__INCLUDED_)
#define AFX_AUDIOCONTROLDATA_H__B0A5DFC0_E877_11D4_87C5_0000B48B0FC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DATA_SIZE                   260
#define TEST_VALUE_LEFT             0
#define TEST_VALUE_RIGHT            1
#define TEST_VALUE_CENTER           2

#define TEST_VALUE_AVG_LRC          3
#define TEST_VALUE_AVG_LEFT         4
#define TEST_VALUE_AVG_RIGHT        5
#define TEST_VALUE_AVG_CENTER       6

#define NO_VALUES                    0        // Bedeutung der Messwerte
#define DB_VALUES_FROM_DATA_LOGGER   1
#define VALUES_LEFT_SPEAKER          2
#define VALUES_CENTER_SPEAKER        3
#define VALUES_RIGHT_SPEAKER         4
#define SELF_TEST_AUTO_EQ            5
#define AUTO_BALANCE_FUNCTION        6
#define AUTO_SOUND                   7
// Sonderform !!
#define SELF_TEST_AUTO_EQ_TRACK9OLD  8
#define SELF_TEST_AUTO_EQ_TRACK10NEW 9

#define NO_DATA          1
#define ERROR_SETCURVE   2
#define ERROR_SETVALUE   3
#define NO_PLOTS         4
#define NO_CORRECT_CURVE 5
#define EDIT_CORRECT_CURVE_ALREADY 6

#define CALC_DB_VALUES        0x00000001
#define CALC_TIME_VALUES      0x00000002
#define CALC_FRQ_CORRECTION   0x00000004

class CAC_COMDlg;
class CPlot;
class CCurve;
class CMathCurve;
class CMeasurement;

class CAudioControlData  
{
public:
	CAudioControlData();
	~CAudioControlData();

   void InitData();
   void CalculateChecksum(BYTE &cCS1, BYTE &cCS2);
	void InitMeasurementParams();

	double GetValue(int, int, int, double);
	int    GetNoOfValues(int n=-1);
	int    GetPeakHoldTime();

	void RestoreParam(BYTE*, int);
	void SaveParam(BYTE*,  int);

	int  GetPlots(CPlot**, int &, bool &, int *, int);
   void SetMeasurement(CMeasurement*pM) {m_pM = pM;};

   static bool SetCurveParams(CCurve *, const char *szNewName, const char *szUnitX, const char *, int, bool, double, double, bool bSingle=true, int nXDiv=-1);
	static void DeleteCorrectionValues();

   friend bool operator==(CAudioControlData&, CAudioControlData&);
   friend bool operator!=(CAudioControlData&, CAudioControlData&);
//   CAudioControlData&    operator=(CAudioControlData&);

   static double FrqCorrValue(double);
   static double FrqResponseCorrValue(double);
	static int    GetDataSize(){return DATA_SIZE;};
	static void   EditFrequencyResponseCorr();
	static void   Return2DFrequencyResponseCorr(LPARAM);

   static CAC_COMDlg *gm_pParent;
   static double      gm_dNAN;

   char cAt;
   char cCommResult;
   BYTE cCD_Track;
   BYTE cCD_CodeParameter[27];
   BYTE cCD_CodeCommands[10];
   BYTE cCD_CodeTrack[18];
   BYTE cChipCardFree;
   BYTE cPropagationTimeLeft;
   BYTE cPropagationTimeCenter;
   BYTE cPropagationTimeRight;
   BYTE cSoundMemoData;
   BYTE cAccCommandCode;
   BYTE cCalibrationValue;
   BYTE cCdPlayerRoomA;
   BYTE cCdPlayerRoomB;
   BYTE cCdPlayerRoomC;
   BYTE bExpertLevel            :1;
   BYTE bLoudspeakersystem2W    :1;
   BYTE bLoudspeakermodeSurround:1;
   BYTE bAutobalanceDeltaT      :1;
   BYTE bAutobalance_dB_Level   :1;
   BYTE bLevelMeterSettingDig   :1;
   BYTE bRT60_Noise             :1;
   BYTE bFilterTypeC            :1;
   BYTE nSound                  :4;
   BYTE nMemo                   :4;
   BYTE nWeigting               :4;
   BYTE nInvQuality             :4;
   BYTE nPeakHold               :4;
                                    // Bedeutung FreeX abhängig von cMeaningOfTestValues
                                    // | SelfTest | DataLogger        | ReverberationTimes |
   BYTE bFree1                  :1; // | Track 9  | FrequencyResponse |         %          |
   BYTE bFree2                  :1; // |    regard for calculation of the Average Curve    |
   BYTE bFree3                  :1; // |                 Show This Curve                   |
   BYTE bFree4                  :1; // | Track 10 | log. / lin.       |         %          |
   BYTE cNetwork_C3             :4;
   BYTE cNetwork_C1             :4;
   BYTE cNetwork_L2             :3;
   BYTE bDummy1                 :1;
   BYTE cNetwork_R2             :4;
   BYTE cNetwork_L3             :3;
   BYTE bDummy2                 :1;
   BYTE cNetwork_R3             :4;
   BYTE cNetwork_C2             :3;
   BYTE bDummy3                 :1;
   BYTE cNetwork_R1             :4;
   BYTE bNetworkSwST            :1;
   BYTE bNetworkSwSP            :1;
   BYTE bNetworkSwSM            :1;
   BYTE bNetworkSwSX            :1;
   BYTE nNetworkL1              :3;
   BYTE bDummy4                 :1;
   BYTE cMeaningOfTestValues;
   BYTE cDataArray[180];
   BYTE cChecksum1;
   BYTE cChecksum2;
private:
   CMeasurement *m_pM;
   static const int   gm_pnMilliSeconds[];
   static CMathCurve *gm_pCorrValues;
   static CMathCurve *gm_pFrqResponseCorrValues;
};

#endif // !defined(AFX_AUDIOCONTROLDATA_H__B0A5DFC0_E877_11D4_87C5_0000B48B0FC3__INCLUDED_)
