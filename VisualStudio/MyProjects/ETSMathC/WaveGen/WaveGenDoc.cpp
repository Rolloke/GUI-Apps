// WaveGenDoc.cpp : Implementierung der Klasse CWaveGenDoc
//


#include "stdafx.h"
#include "WaveGen.h"

#include "WaveGenDoc.h"
#include "CEtsMathClass.h"
#include "WaveProperties.h"
#include "FilterProperties.h"
#include "StandardProperties.h"
#include "CreateTimeFunction.h"
// Preview
#include "Curve.h"
#include "CurveLabel.h"
#include "PreviewFileHeader.h"
#include "caratoolbar.h"
// Dll's
#include "CEtsDiv.h"

#include <process.h>
#include <Mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaveGenDoc
#define LOAD_NORMAL        0
#define LOAD_LEFT_CURVE    1
#define LOAD_RIGHT_CURVE   2


IMPLEMENT_DYNCREATE(CWaveGenDoc, CCARADoc)

BEGIN_MESSAGE_MAP(CWaveGenDoc, CCARADoc)
	//{{AFX_MSG_MAP(CWaveGenDoc)
	ON_COMMAND(ID_CALC_RE_FOURIER, OnCalcFourier)
	ON_COMMAND(ID_CALC_RANDOM, OnCalcRandom)
	ON_UPDATE_COMMAND_UI(ID_CALC_RE_FOURIER, OnUpdateCalcReFourier)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_FILTER, OnUpdateOptionsFilter)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_WAVEFILE, OnUpdateOptionsWavefile)
	ON_COMMAND(ID_OPTIONS_FILTER, OnOptionsFilter)
	ON_COMMAND(ID_OPTIONS_WAVEFILE, OnOptionsWavefile)
	ON_COMMAND(ID_CALC_RANDOM_FRQ, OnCalcRandomFrq)
	ON_COMMAND(ID_OPTIONS_STANDARD, OnOptionsStandard)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_STANDARD, OnUpdateOptionsStandard)
	ON_COMMAND(ID_FILE_LOAD_LEFTCURVE, OnFileLoadLeftcurve)
	ON_UPDATE_COMMAND_UI(ID_FILE_LOAD_LEFTCURVE, OnUpdateFileLoadLeftcurve)
	ON_COMMAND(ID_FILE_LOAD_RIGHTCURVE, OnFileLoadRightcurve)
	ON_UPDATE_COMMAND_UI(ID_FILE_LOAD_RIGHTCURVE, OnUpdateFileLoadRightcurve)
	ON_COMMAND(ID_CALC_NORM_FRQ, OnCalcNormFrq)
	ON_COMMAND(ID_OPTIONS_FILTER_EDIT, OnOptionsFilterEdit)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_FILTER_EDIT, OnUpdateOptionsFilterEdit)
	ON_COMMAND(ID_CALC_CANCEL, OnCalcCancel)
	ON_UPDATE_COMMAND_UI(ID_CALC_CANCEL, OnUpdateCalcCancel)
	ON_COMMAND(ID_PLAY_WAVE, OnPlayWave)
	ON_UPDATE_COMMAND_UI(ID_PLAY_WAVE, OnUpdatePlayWave)
	ON_COMMAND(ID_PLAY_STOP, OnPlayStop)
	ON_UPDATE_COMMAND_UI(ID_PLAY_STOP, OnUpdatePlayStop)
	ON_UPDATE_COMMAND_UI(ID_CALC_FOURIER, OnUpdateCalcFourier)
	ON_COMMAND(ID_CALC_TIME_FUNCTION, OnCalcTimeFunction)
	ON_COMMAND(ID_CALC_FOURIER, OnCalcFourier)
	ON_UPDATE_COMMAND_UI(ID_CALC_NORM_FRQ, OnUpdateCalcNew)
	ON_UPDATE_COMMAND_UI(ID_CALC_RANDOM, OnUpdateCalcNew)
	ON_UPDATE_COMMAND_UI(ID_CALC_TIME_FUNCTION, OnUpdateCalcNew)
	ON_UPDATE_COMMAND_UI(ID_CALC_RANDOM_FRQ, OnUpdateCalcNew)
	ON_COMMAND(ID_CALC_MULTIPLY_FILTER+ID_TB_RIGHT_EXT, OnOptionsFilter)
	ON_UPDATE_COMMAND_UI(ID_CALC_MULTIPLY_FILTER, OnUpdateCalcMathFilter)
	ON_UPDATE_COMMAND_UI(ID_CALC_DIVIDE_FILTER, OnUpdateCalcMathFilter)
	ON_UPDATE_COMMAND_UI(ID_CALC_ADD_FILTER, OnUpdateCalcMathFilter)
	ON_UPDATE_COMMAND_UI(ID_CALC_SUBTRACT_FILTER, OnUpdateCalcMathFilter)
	ON_COMMAND_RANGE(ID_CALC_MULTIPLY_FILTER, ID_CALC_SUBTRACT_FILTER, OnCalcMathFilter)
	ON_COMMAND(ID_REPEAT_WAVE, OnRepeatWave)
	ON_UPDATE_COMMAND_UI(ID_REPEAT_WAVE, OnUpdateRepeatWave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaveGenDoc Konstruktion/Destruktion

CWaveGenDoc::CWaveGenDoc()
{
   Init();
   m_nOrder        = 14;
   m_nLoadOption   = 0;
   m_bTimeFunction = false;
   m_nInput        = 0;
   ZeroMemory(&m_FilterL, sizeof(m_FilterL));
   m_FilterL.nTPOrder        = 4;
   m_FilterL.nTPType         = 0;
   m_FilterL.dTPQFaktor      = 1.0;
   m_FilterL.dTPFreqSelected = 10;

   m_FilterL.nHPOrder        = 4;
   m_FilterL.nHPType         = 0;
   m_FilterL.dHPQFaktor      = 1.0;
   m_FilterL.dHPFreqSelected = 20; 
   m_dBandWidth              =   2;
   m_dFilterM                = 100;

   m_hThread                 = INVALID_HANDLE_VALUE;
   m_nThreadID               = 0;
   m_bNewDocContent          = true;
   m_bShowPlot               = false;
}

void CWaveGenDoc::Init()
{
   ZeroMemory(&m_wfEX, sizeof(__WAVEFORMATEX));
   m_wfEX.wFormatTag  = WAVE_FORMAT_PCM;
   m_wfEX.nBlockAlign    = 4;
   m_wfEX.nChannels      = 2;
   m_wfEX.wBitsPerSample = 16;
   m_wfEX.nSamplesPerSec = 44100;
   m_wfEX.nAvgBytesPerSec = m_wfEX.nSamplesPerSec * m_wfEX.nBlockAlign;
   strncpy(m_wfEX.sData, "data", 4);

   m_nValues       = 0;
   m_nInput        = -1;
   m_lWaveFileSize = 0;

   m_dMaxValueL    = 1;
   m_dRMS_ValueL   = 0.707;
   m_dMaxMagnitudeL= 0.13;
   m_dDCOffsetL    = 0;
   m_bInvertL      = false;
   m_bFourierArrayAttachedL = false;

   m_dMaxValueR    = 1;
   m_dRMS_ValueR   = 0.707;
   m_dMaxMagnitudeR= 0.13;
   m_dDCOffsetR    = 0;
   m_bInvertR      = false;
   m_bFourierArrayAttachedR = false;
   m_pFilterCurve  = NULL;
   m_bNewDocContent = true;
   m_bTimeFktChanged = true;
   m_bRepeatWave = false;
   
   m_nFFTValues = 4096;
}

CWaveGenDoc::~CWaveGenDoc()
{
}

BOOL CWaveGenDoc::OnNewDocument()
{
	if (!CCARADoc::OnNewDocument())
		return FALSE;

	return TRUE;
}

void CWaveGenDoc::DeleteContents() 
{
   OnCalcCancel();
	if (m_nLoadOption == LOAD_NORMAL)
   {
      UpdateAllViews(NULL, UPDATE_DELETE_CONTENTS, NULL);
      m_Container.DeleteAll();

      m_mmfL.Close();
      m_mmfR.Close();
      m_mmfWave.Close();
      m_mmfFFT.Close();
      if (m_pFilterCurve) delete m_pFilterCurve;
      Init();
   }

	CCARADoc::DeleteContents();
}


/////////////////////////////////////////////////////////////////////////////
// CWaveGenDoc Serialisierung
void CWaveGenDoc::Serialize(CArchive& ar)
{
   SWaveFile    wf;
	if (ar.IsStoring())
	{
      if (!IsArrayOk())
      {
         AfxThrowArchiveException(CArchiveException::generic, "No Array");
      }
      int nValues;
      ZeroMemory(&wf, sizeof(SWaveFile));
      strncpy((char*)&wf.sRIFF    , "RIFF"    , 4);
      strncpy((char*)&wf.sWAVEfmt_, "WAVEfmt ", 8);
      if (m_lWaveFileSize) nValues = m_wfEX.nSampleLength / m_wfEX.nBlockAlign;
      else
      {
         nValues = m_nValues-1;
         m_wfEX.nSampleLength = nValues * m_wfEX.nBlockAlign;
      }

      wf.nHeaderLength = sizeof(SWaveFile) - sizeof(long);
      if (m_bTimeFunction)
      {
         if (m_dMaxMagnitudeL > 1 || m_dMaxMagnitudeR > 1)
         {
            OnOptionsWavefile();
         }

         int    i;
         short  nValueL, nValueR;
         double dNormFactorL = m_dMaxMagnitudeL * (double)(1 << (m_wfEX.wBitsPerSample-1)) / m_dMaxValueL;
         double dNormFactorR = m_dMaxMagnitudeR * (double)(1 << (m_wfEX.wBitsPerSample-1)) / m_dMaxValueR;
         int   nPointer = 0;
         float *pfFourierL = (float*)m_mmfL.MapViewOfFile(0,0,0);
         float *pfFourierR = (float*)m_mmfR.MapViewOfFile(0,0,0);
         CString str;
         m_FileHeader.Init("WAV", "2001", 100);
         m_FileHeader.SetComment(NULL);
         str.Format(IDS_WAVE_FILE_COMMENT, AfxGetApp()->m_pszAppName, m_wfEX.nSamplesPerSec, m_wfEX.nChannels, 8 * m_wfEX.nBlockAlign / m_wfEX.nChannels);
         m_FileHeader.SetComment((char*)LPCTSTR(str)),
         m_FileHeader.CalcChecksum();

         m_wfEX.wFormatTag = WAVE_FORMAT_PCM;
         wf.nFileLength    = m_wfEX.nSampleLength + sizeof(SWaveFile) + sizeof(__WAVEFORMATEX) + sizeof(CFileHeader) - 8;
         CFile *pf = ar.GetFile();
         if (pf->IsKindOf(RUNTIME_CLASS(CMemFile)))
         {
            int nSize = m_lWaveFileSize;
            nSize = wf.nFileLength + 1024;
            if (!m_mmfWave.IsValid())
            {
               m_mmfWave.Alloc(0, nSize, true, true, true, true);
            }
            if (m_mmfWave.IsValid())
            {
               ((CMemFile*)pf)->Attach((BYTE*)m_mmfWave.MapViewOfFile(0,0,0), nSize, 0);
            }
         }

         ar.Write(&wf, sizeof(SWaveFile));
         ar.Write(&m_wfEX, sizeof(__WAVEFORMATEX));

         if (m_wfEX.nChannels == 1)
         {
            if (m_bInvertL)
            {
               for (i=0; i<nValues; i++)
               {
                  nValueL = -(short)CEtsDiv::Round(dNormFactorL * pfFourierL[i]);
                  ar.Write(&nValueL, sizeof(short));
               }
            }
            else
            {
               for (i=0; i<nValues; i++)
               {
                  nValueL = -(short)CEtsDiv::Round(dNormFactorL * pfFourierL[i]);
                  ar.Write(&nValueL, sizeof(short));
               }
            }
         }
         else
         {
            for (i=0; i<nValues; i++)
            {
               switch (m_nInput)
               {
                  case WAVE_CHANNEL_MONO_L:
                  nValueR = nValueL = (short)CEtsDiv::Round(dNormFactorL * pfFourierL[i]);
                  break;
                  case WAVE_CHANNEL_MONO_R:
                  nValueR = nValueL = (short)CEtsDiv::Round(dNormFactorR * pfFourierR[i]);
                  break;
                  case WAVE_CHANNEL_STEREO_LR_INV:
                  nValueL = (short)CEtsDiv::Round(dNormFactorR * pfFourierR[i]);
                  nValueR = (short)CEtsDiv::Round(dNormFactorL * pfFourierL[i]);
                  break;
                  case WAVE_CHANNEL_STEREO_LR:
                  nValueL = (short)CEtsDiv::Round(dNormFactorL * pfFourierL[i]);
                  nValueR = (short)CEtsDiv::Round(dNormFactorR * pfFourierR[i]);
                  break;
               }
               if (m_bInvertL) nValueL = -nValueL;
               ar.Write(&nValueL, sizeof(short));
               if (m_bInvertR) nValueR = -nValueR;
               ar.Write(&nValueR, sizeof(short));
            }
         }
      }
      else
      {
         m_wfEX.wFormatTag = WAVE_FORMAT_FOURIER;
         if (m_mmfR.IsValid()) m_wfEX.wFormatTag++;
         float *pfFourierL = (float*)m_mmfL.MapViewOfFile(0,0,0);
         float *pfFourierR = (float*)m_mmfR.MapViewOfFile(0,0,0);

         wf.nFileLength    = m_wfEX.nSampleLength + sizeof(SWaveFile) + sizeof(__WAVEFORMATEX);
         ar.Write(&wf, sizeof(SWaveFile));
         ar.Write(&m_wfEX, sizeof(__WAVEFORMATEX));
         ar.Write(&m_nOrder, sizeof(int));
         ar.Write(&m_nValues, sizeof(int));

         ar.Write(&m_dMaxMagnitudeL, sizeof(double));
         ar.Write(pfFourierL, m_nValues * sizeof(double));
         ar.Write(&m_FilterL, sizeof(FilterBox));
         if (pfFourierR)
         {
            ar.Write(&m_dMaxMagnitudeR, sizeof(double));
            ar.Write(pfFourierR, m_nValues * sizeof(double));
         }
      }
	}
	else if (m_nLoadOption == LOAD_NORMAL)       // Normal laden
	{
      ASSERT(!m_mmfL.IsValid());
      bool bFourier = true, bStereo = false, bExt = false;
      ar.Read(&wf, sizeof(SWaveFile));
      if (strncmp(wf.sRIFF, "RIFF", 4)         != 0) AfxThrowArchiveException(CArchiveException::badIndex, NULL);
      if (strncmp(wf.sWAVEfmt_, "WAVEfmt ", 8) != 0) AfxThrowArchiveException(CArchiveException::badIndex, NULL);

      ar.Read(&m_wfEX, sizeof(__WAVEFORMATEX));
      switch (m_wfEX.wFormatTag)
      {
         case WAVE_FORMAT_FOURIER: break;
         case WAVE_FORMAT_FOURIER+1:    bStereo = true; break;
         case WAVE_FORMAT_FOURIER_EX:   bExt    = true; break;
         case WAVE_FORMAT_FOURIER_EX+1: bStereo = true, bExt = true; break;
            break;
         default: bFourier = false; break;
      }
      if ((m_wfEX.nChannels < 1)||(m_wfEX.nChannels > 2))    AfxThrowArchiveException(CArchiveException::badIndex, NULL);

      if      (m_wfEX.wFormatTag == WAVE_FORMAT_PCM) // Wave Format
      {
         int   i, nValues;
         short nValue;
         nValues         = m_wfEX.nSampleLength / m_wfEX.nBlockAlign;
         m_lWaveFileSize = wf.nFileLength;
         bool bIncrease = false;
         for (m_nOrder=nValues, i=0; m_nOrder>1; i++)
         {
            if ((i > 0) && (m_nOrder & 0x01)) bIncrease = true;
            m_nOrder >>= 1;
         }
         if (bIncrease) i++;
         m_nOrder        = i;
         m_nValues       = (1 << m_nOrder)+1;

         m_bTimeFunction = true;
         if (m_wfEX.nChannels == 2) m_nInput = 2;
         else                       m_nInput = 0;
         if (m_nOrder > 31)
         {
            AfxThrowArchiveException(CArchiveException::generic, "No Array");
         }
         SetArraySizes(true, true);
         float *pfFourierL = (float*)m_mmfL.MapViewOfFile(0,0,0);
         float *pfFourierR = (float*)m_mmfR.MapViewOfFile(0,0,0);

         if (pfFourierL == NULL)
         {
            AfxThrowArchiveException(CArchiveException::generic, "No Array"); 
         }
         if (m_wfEX.nChannels == 2)                         // Stereo
         {
            if (pfFourierR == NULL)
            {
               AfxThrowArchiveException(CArchiveException::generic, "No Array");
            }
         }
         if (m_wfEX.nChannels == 2)                            // Stereo
         {
            for (i=0; i<nValues; i++)
            {
               ar.Read(&nValue, sizeof(short));
               pfFourierL[i] = (float) nValue;
               ar.Read(&nValue, sizeof(short));
               pfFourierR[i] = (float) nValue;
            }
            for (; i<m_nValues; i++)
            {
               pfFourierL[i]  = 0;
               pfFourierR[i]  = 0;
            }
         }
         else                                                  // Mono
         {
            for (i=0; i<nValues; i++)
            {
               ar.Read(&nValue, sizeof(short));
               pfFourierL[i] = (float) nValue;
            }
            for (; i<m_nValues; i++)
            {
               pfFourierL[i]  = 0;
            }
         }
      }
      if (bFourier)                                            // Fourierformat
      {
         int nMirrored = 0;
         m_bTimeFunction = false;
         ar.Read(&m_nOrder, sizeof(int));
         ar.Read(&m_nValues, sizeof(int));
         ar.Read(&m_dMaxMagnitudeL, sizeof(double));
         SetArraySizes(true, false);
         float *pfFourierL = (float*)m_mmfL.MapViewOfFile(0,0,0);
         float *pfFourierR = (float*)m_mmfR.MapViewOfFile(0,0,0);
         if (pfFourierL == NULL)
         {
            AfxThrowArchiveException(CArchiveException::generic, "No Array");
         }
         if (bExt)
         {
            int nValues = m_nValues;
            ar.Read(pfFourierL, nValues * sizeof(float));
         }
         else
         {
            double dVal;
            int i;
            for (i=0; i<m_nValues;i++)
            {
               ar.Read(&dVal, sizeof(double));
               pfFourierL[i] = (float)dVal;
            }
         }
         ar.Read(&m_FilterL, sizeof(FilterBox));
         if (bStereo)
         {
            SetArraySizes(false, true);
            if (pfFourierR == NULL)
            {
               AfxThrowArchiveException(CArchiveException::generic, "No Array");
            }
            if (bExt)
            {
               int nValues = m_nValues;
               ar.Read(pfFourierR, nValues* sizeof(float));
            }
            else
            {
               double dVal;
               int i;
               for (i=0; i<m_nValues;i++)
               {
                  ar.Read(&dVal, sizeof(double));
                  pfFourierR[i] = (float)dVal;
               }
            }
         }
      }
      CheckFunktionValues();
//      UpdateAllViews(NULL, UPDATE_INSERT_CONTENTS);
	}
	else if (m_nLoadOption == LOAD_LEFT_CURVE)
	{
      ASSERT(!m_mmfL.IsValid());
      bool bFourier = true, bStereo = false, bExt = false;
      ar.Read(&wf, sizeof(SWaveFile));
      if (strncmp(wf.sRIFF, "RIFF", 4)         != 0) AfxThrowArchiveException(CArchiveException::badIndex, NULL);
      if (strncmp(wf.sWAVEfmt_, "WAVEfmt ", 8) != 0) AfxThrowArchiveException(CArchiveException::badIndex, NULL);

      ar.Read(&m_wfEX, sizeof(__WAVEFORMATEX));
      switch (m_wfEX.wFormatTag)
      {
         case WAVE_FORMAT_FOURIER: break;
         case WAVE_FORMAT_FOURIER+1:    bStereo = true; break;
         case WAVE_FORMAT_FOURIER_EX:   bExt   = true; break;
         case WAVE_FORMAT_FOURIER_EX+1: bStereo = true, bExt = true; break;
            break;
         default: bFourier = false; break;
      }
      if ((m_wfEX.nChannels < 1)||(m_wfEX.nChannels > 2))    AfxThrowArchiveException(CArchiveException::badIndex, NULL);

      if (m_wfEX.wFormatTag == WAVE_FORMAT_PCM)            // Wave Format
      {
         int   i, nValues, nOrder;
         short nValue;
         nValues        = m_wfEX.nSampleLength / m_wfEX.nBlockAlign;
         for (nOrder=nValues, i=0; nOrder>0; nOrder >>=1, i++);
         m_lWaveFileSize = wf.nFileLength;

         if (m_nValues == 0) m_nOrder = 0;
         if ((m_nOrder != 0) && (nOrder != m_nOrder))
         {
            AfxThrowArchiveException(CArchiveException::generic, "No Array");
         }
         m_nOrder        = i;
         m_nValues       = (1 << m_nOrder) + 1;
         m_bTimeFunction = true;
         if (m_nOrder > 31)
            AfxThrowArchiveException(CArchiveException::generic, "No Array");

         SetArraySizes(true, false);
         float *pfFourierL = (float*)m_mmfL.MapViewOfFile(0,0,0);
         if (pfFourierL == NULL)
            AfxThrowArchiveException(CArchiveException::generic, "No Array");

         for (i=0; i<nValues;i++)
         {
            ar.Read(&nValue, sizeof(short));
            pfFourierL[i] = (float) nValue;
            if (m_wfEX.nChannels == 2)                         // Stereo
            {
               ar.Read(&nValue, sizeof(short));
            }
         }
         for (; i<m_nValues;i++)
            pfFourierL[i] = 0;
      }
      if (bFourier)                                            // Fourierformat
      {
         m_bTimeFunction = false;
         int nOrder, nValues, nMirrored = 0;
         ar.Read(&nOrder, sizeof(int));
         ar.Read(&nValues, sizeof(int));
         if (bExt) ar.Read(&nMirrored, sizeof(int));
         if ((m_nValues != 0) && (nValues != m_nValues))
         {
            AfxThrowArchiveException(CArchiveException::generic, "No Array");
         }
         m_nOrder    = nOrder;
         m_nValues   = nValues;
         ar.Read(&m_dMaxMagnitudeL, sizeof(double));
         SetArraySizes(true, false);
         float *pfFourierL = (float*)m_mmfL.MapViewOfFile(0,0,0);
         if (pfFourierL == NULL)
            AfxThrowArchiveException(CArchiveException::generic, "No Array");
         if (bExt)
         {
            int nValues = m_nValues;
            ar.Read(pfFourierL, nValues * sizeof(float));
         }
         else
         {
            double dVal;
            int i;
            for (i=0; i<nValues;i++)
            {
               ar.Read(&dVal, sizeof(double));
               pfFourierL[i] = (float)dVal;
            }
         }
         ar.Read(&m_FilterL, sizeof(FilterBox));
      }
      CheckFunktionValues();
//      UpdateAllViews(NULL, UPDATE_INSERT_CONTENTS);
	}
	else if (m_nLoadOption == LOAD_RIGHT_CURVE)
	{
      ASSERT(!m_mmfR.IsValid());
      bool bFourier = true, bStereo = false, bExt = false;
      ar.Read(&wf, sizeof(SWaveFile));
      if (strncmp(wf.sRIFF, "RIFF", 4)         != 0) AfxThrowArchiveException(CArchiveException::badIndex, NULL);
      if (strncmp(wf.sWAVEfmt_, "WAVEfmt ", 8) != 0) AfxThrowArchiveException(CArchiveException::badIndex, NULL);

      ar.Read(&m_wfEX, sizeof(__WAVEFORMATEX));
      switch (m_wfEX.wFormatTag)
      {
         case WAVE_FORMAT_FOURIER: break;
         case WAVE_FORMAT_FOURIER+1:    bStereo = true; break;
         case WAVE_FORMAT_FOURIER_EX:   bExt   = true; break;
         case WAVE_FORMAT_FOURIER_EX+1: bStereo = true, bExt = true; break;
            break;
         default: bFourier = false; break;
      }
      if ((m_wfEX.nChannels < 1)||(m_wfEX.nChannels > 2))    AfxThrowArchiveException(CArchiveException::badIndex, NULL);

      if      (m_wfEX.wFormatTag == WAVE_FORMAT_PCM)            // Wave Format
      {
         int   i, nValues, nOrder;
         short nValue;
         nValues        = m_wfEX.nSampleLength / m_wfEX.nBlockAlign;
         m_lWaveFileSize = wf.nFileLength;
         for (nOrder=nValues, i=0; nOrder>0; nOrder >>=1, i++);
         if (m_nValues == 0) m_nOrder = 0;
         if ((m_nOrder != 0) && (nOrder != m_nOrder))
         {
            AfxThrowArchiveException(CArchiveException::generic, "No Array");
         }
         m_nOrder        = i;
         m_nValues       = (1 << m_nOrder) + 1;
         m_bTimeFunction = true;
         if (m_nOrder > 31)
            AfxThrowArchiveException(CArchiveException::generic, "No Array");

         SetArraySizes(false, true);
         float *pfFourierR = (float*)m_mmfR.MapViewOfFile(0,0,0);
         if (pfFourierR == NULL)
            AfxThrowArchiveException(CArchiveException::generic, "No Array");

         for (i=0; i<nValues;i++)
         {
            ar.Read(&nValue, sizeof(short));
            pfFourierR[i] = (float) nValue;
            if (m_wfEX.nChannels == 2)                         // Stereo
            {
               ar.Read(&nValue, sizeof(short));
            }
         }
         for (; i<m_nValues;i++)
            pfFourierR[i] = 0;
      }
      if (bFourier)                                            // Fourierformat
      {
         m_bTimeFunction = false;
         int nOrder, nValues, nMirrored = 0;
         ar.Read(&nOrder, sizeof(int));
         ar.Read(&nValues, sizeof(int));
         if (bExt) ar.Read(&nMirrored, sizeof(int));
         if ((m_nValues != 0) && (nValues != m_nValues))
         {
            AfxThrowArchiveException(CArchiveException::generic, "No Array");
         }
         m_nOrder  = nOrder;
         m_nValues = nValues;
         ar.Read(&m_dMaxMagnitudeR, sizeof(double));

         SetArraySizes(false, true);
         float *pfFourierR = (float*)m_mmfR.MapViewOfFile(0,0,0);
         if (pfFourierR == NULL)
         {
            AfxThrowArchiveException(CArchiveException::generic, "No Array");
         }
         if (bExt) ar.Read(pfFourierR, m_nValues * sizeof(float));
         else
         {
            double dVal;
            int i;
            for (i=0; i<nValues;i++)
            {
               ar.Read(&dVal, sizeof(double));
               pfFourierR[i] = (float)dVal;
            }
         }
         ar.Read(&m_FilterL, sizeof(FilterBox));
      }
      CheckFunktionValues();
//      UpdateAllViews(NULL, UPDATE_INSERT_CONTENTS);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWaveGenDoc Diagnose

#ifdef _DEBUG
void CWaveGenDoc::AssertValid() const
{
	CCARADoc::AssertValid();
}

void CWaveGenDoc::Dump(CDumpContext& dc) const
{
	CCARADoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWaveGenDoc Befehle
void CWaveGenDoc::OnCalcRandom()
{
   if (m_mmfL.IsValid())
   {
      ASSERT(FALSE);
      return;
   }
   if (m_hThread == INVALID_HANDLE_VALUE)
   {
      CWaveGenApp *pApp = (CWaveGenApp*)AfxGetApp();
      pApp->CreateProgressDlg("Berechne Zufallsfrequenzen");
      m_hThread = (HANDLE) _beginthreadex(NULL, 0, CWaveGenDoc::OnCalcRandomThread, (void*)this, 0, &m_nThreadID);
   }
}

unsigned int CWaveGenDoc::OnCalcRandomThread(void *pParam)
{
   CWaveGenDoc* pDoc = (CWaveGenDoc*) pParam;
   int i;
   double dSum = 0;

   pDoc->m_nValues = (1 << pDoc->m_nOrder) + 1;                     // Anzahl der Werte berechnen
   pDoc->SetArraySizes(true, false);
   float *pfFourierL = (float*)pDoc->m_mmfL.MapViewOfFile(0,0,0);
   if (pfFourierL != NULL)
   {
      CWaveGenApp *pApp = (CWaveGenApp*)AfxGetApp();
      pApp->ProgressDlgSetRange(pDoc->m_nValues*2);

      for (i=0; i<pDoc->m_nValues-1; i++)                            // Zufallszahlen erzeugen
      {
         pfFourierL[i] = (float)((int)CETSMathClass::GetRandomValue());
         dSum   += pfFourierL[i];
         pApp->ProgressDlgStep();
      }
      pfFourierL[i] = 0;                                             // letzten Wert definieren

      dSum /= (double)(pDoc->m_nValues-1);                           // Offset bestimmen
      for (i=0; i<pDoc->m_nValues-1; i++)                            // Offset abziehen
      {
         pfFourierL[i] -= (float)dSum;
         pApp->ProgressDlgStep();
      }
      pDoc->m_strHistory = _T("Zufalls Zeitwerte");
      pDoc->m_bTimeFunction = true;
   }
   theApp.m_pMainWnd->PostMessage(WM_COMMAND, MAKELONG(ID_CALC_CANCEL, 1), NULL);
   return 0;
}

void CWaveGenDoc::OnCalcRandomFrq()
{
   if (m_mmfL.IsValid())
   {
      ASSERT(FALSE);
      return;
   }

   if (m_hThread == INVALID_HANDLE_VALUE)
   {
      CWaveGenApp *pApp = (CWaveGenApp*)AfxGetApp();
      pApp->CreateProgressDlg("Berechne Zufallsfrequenzen");
      m_hThread = (HANDLE) _beginthreadex(NULL, 0, CWaveGenDoc::OnCalcRandomFrqThread, (void*)this, 0, &m_nThreadID);
   }
}

unsigned int CWaveGenDoc::OnCalcRandomFrqThread(void *pParam)
{
   CWaveGenDoc* pDoc = (CWaveGenDoc*) pParam;
   int i;

   pDoc->m_nValues = (1 << pDoc->m_nOrder) + 1;                // Anzahl der Werte berechnen
   pDoc->SetArraySizes(true, false);
   float *pfFourierL = (float*)pDoc->m_mmfL.MapViewOfFile(0,0,0);
   if (pfFourierL != NULL)
   {
      CWaveGenApp *pApp = (CWaveGenApp*)AfxGetApp();
      pApp->ProgressDlgSetRange(pDoc->m_nValues);

      pfFourierL[0]  = 0;                              // erster Wert = Offset = 0
      for (i=1; i<pDoc->m_nValues; i++)                        // Zufallszahlen erzeugen
      {
         pfFourierL[i] = (float)((int)CETSMathClass::GetRandomValue());
         pApp->ProgressDlgStep();
      }

      pDoc->m_strHistory = _T("Zufalls Frequenzwerte");
      pDoc->m_bTimeFunction = false;
   }	
   theApp.m_pMainWnd->PostMessage(WM_COMMAND, MAKELONG(ID_CALC_CANCEL, 1), NULL);
   return 0;
}

void CWaveGenDoc::OnCalcNormFrq()
{
   if (m_mmfL.IsValid())
   {
      ASSERT(FALSE);
      return;
   }
   if (m_hThread == INVALID_HANDLE_VALUE)
   {
      CWaveGenApp *pApp = (CWaveGenApp*)AfxGetApp();
      pApp->CreateProgressDlg("Berechne Zufallsfrequenzen");
      m_hThread = (HANDLE) _beginthreadex(NULL, 0, CWaveGenDoc::OnCalcNormFrqThread, (void*)this, 0, &m_nThreadID);
   }
}

unsigned int CWaveGenDoc::OnCalcNormFrqThread(void *pParam)
{
   CWaveGenDoc* pDoc = (CWaveGenDoc*) pParam;
   int i;

   pDoc->m_nValues   = (1 << pDoc->m_nOrder) + 1;                // Anzahl der Werte berechnen
   pDoc->SetArraySizes(true, false);
   float *pfFourierL = (float*)pDoc->m_mmfL.MapViewOfFile(0,0,0);
   if (pfFourierL != NULL)
   {
      CWaveGenApp *pApp = (CWaveGenApp*)AfxGetApp();
      pApp->ProgressDlgSetRange(pDoc->m_nValues);

      double dFrequ = 0;
      double dStep  = pDoc->GetMinFrqStep();
      double dMin   = 400,
             dMax   = 405;
      pfFourierL[0]  = 0;                                      // erster Wert = Offset = 0
      for (i=1; i<pDoc->m_nValues; i++)
      {
         if (i&1) dFrequ += dStep;
         if ((dFrequ > dMin) && (dFrequ < dMax))
         {
            if (i&1) pfFourierL[i] = 1.0;
            else     pfFourierL[i] = 0.0;
         }
         else pfFourierL[i] = 0.0;
         pApp->ProgressDlgStep();
      }

      pDoc->m_bTimeFunction = false;
      pDoc->m_strHistory = _T("Normierte Frequenzwerte");
   }	
   theApp.m_pMainWnd->PostMessage(WM_COMMAND, MAKELONG(ID_CALC_CANCEL, 1), NULL);
   return 0;
}

void CWaveGenDoc::OnCalcFourier()
{
   if (!IsArrayOk()) return;

   if (m_hThread == INVALID_HANDLE_VALUE)
   {
      CWaveGenApp *pApp = (CWaveGenApp*)AfxGetApp();
      if (m_bTimeFunction)
         pApp->CreateProgressDlg("Berechne Fouriertransformation");
      else
         pApp->CreateProgressDlg("Berechne inverse Fouriertransformation");
      m_hThread = (HANDLE) _beginthreadex(NULL, 0, CWaveGenDoc::OnCalcFourierThread, (void*)this, 0, &m_nThreadID);
   }
}

void CWaveGenDoc::OnCalcFourierThread()
{

   ComplexFFT(0, m_nValues-1, 0);
   ComplexFFT(0, m_nValues-1, 1);
/*
   int i, n;
   n = (m_nValues-1) / m_nFFTValues;
   for (i=0; i<n; i++)
   {
      ComplexFFT(i, m_nFFTValues, 0);
      ComplexFFT(i, m_nFFTValues, 1);
   }
*/
   if (m_bTimeFunction) m_strHistory = _T("Transformation");
   else                 m_strHistory = _T("Rücktransformation");

   m_bTimeFunction = !m_bTimeFunction;
   if (m_bTimeFunction) m_bTimeFktChanged = true;
}

unsigned int CWaveGenDoc::OnCalcFourierThread(void *pParam)
{
   CWaveGenDoc* pDoc = (CWaveGenDoc*) pParam;
   pDoc->OnCalcFourierThread();
   theApp.m_pMainWnd->PostMessage(WM_COMMAND, MAKELONG(ID_CALC_CANCEL, 1), NULL);
   return 0;
}

void CWaveGenDoc::OnCalcMathFilter(UINT nWhat) 
{
   if (!IsArrayOk()) return;

   m_FilterL.bCalcArray = false;
   CalculateFilter(&m_FilterL, nWhat);
   switch (nWhat)
   {
      case ID_CALC_MULTIPLY_FILTER: m_strHistory = _T("Filter multipliziert");break;
      case ID_CALC_DIVIDE_FILTER  : m_strHistory = _T("Filter dividiert");    break;
      case ID_CALC_ADD_FILTER     : m_strHistory = _T("Filter addiert");      break;
      case ID_CALC_SUBTRACT_FILTER: m_strHistory = _T("Filter subtrahiert");  break;
      default: return;
   }
  
   UpdateAllViews(NULL, UPDATE_INSERT_HISTORY|UPDATE_HISTORY_FILTER, (CObject*)&m_strHistory);
   UpdateAllViews(NULL, UPDATE_CHANGE_PLOTS);
}

void CWaveGenDoc::OnOptionsStandard()
{
   StandardProperties sp;
   sp.m_nOrder = m_nOrder;
   if (sp.DoModal() == IDOK)
   {
      m_nOrder = sp.m_nOrder;
      UpdateAllViews(NULL, UPDATE_CHANGE_CONTENTS);
   }
}


void CWaveGenDoc::OnOptionsFilter() 
{
   CFilterProperties fp;

   fp.m_dFrequencyTP       = m_FilterL.dTPFreqSelected;
   fp.m_dFrequencyHP       = m_FilterL.dHPFreqSelected;
   fp.m_dQFactorTP         = m_FilterL.dTPQFaktor;
   fp.m_dQFactorHP         = m_FilterL.dHPQFaktor;
   fp.m_nFilterCharacterTP = m_FilterL.nTPType;
   fp.m_nFilterCharacterHP = m_FilterL.nHPType;
   fp.m_nOrderTP           = m_FilterL.nTPOrder;
   fp.m_nOrderHP           = m_FilterL.nHPOrder;
   fp.m_dBandwidth         = m_dBandWidth;
   fp.m_dFilterM           = m_dFilterM;
   if (fp.DoModal() == IDOK)
   {
      m_FilterL.dTPFreqSelected = fp.m_dFrequencyTP;
      m_FilterL.dHPFreqSelected = fp.m_dFrequencyHP;
      m_FilterL.dTPQFaktor      = fp.m_dQFactorTP;
      m_FilterL.dHPQFaktor      = fp.m_dQFactorHP;
      m_FilterL.nTPType         = fp.m_nFilterCharacterTP;
      m_FilterL.nHPType         = fp.m_nFilterCharacterHP;
      m_FilterL.nTPOrder        = fp.m_nOrderTP;
      m_FilterL.nHPOrder        = fp.m_nOrderHP;
      m_dBandWidth              = fp.m_dBandwidth;
      m_dFilterM                = fp.m_dFilterM ;
      UpdateAllViews(NULL, UPDATE_INSERT_FILTERFUNCTION);
   }
}

void CWaveGenDoc::OnOptionsFilterEdit() 
{
	
}

void CWaveGenDoc::OnOptionsWavefile() 
{
	CWaveProperties wp;
   if (m_mmfL.IsValid()) wp.m_dMaxMagnitude_L = m_dMaxMagnitudeL * 100.0;
   else                  wp.m_dMaxMagnitude_L = -2;
   if (m_mmfR.IsValid()) wp.m_dMaxMagnitude_R = m_dMaxMagnitudeR * 100.0;
   else                  wp.m_dMaxMagnitude_R = -2;

   wp.m_bStereo  = (m_wfEX.nChannels == 2) ? true : false;
   wp.m_bLInvert = m_bInvertL;
   wp.m_bRInvert = m_bInvertR;
   wp.m_nBits    = ((m_wfEX.nBlockAlign / m_wfEX.nChannels)==1) ? WAVE_BITS_8 : WAVE_BITS_16;
   wp.m_nInput   = m_nInput;
   switch (m_wfEX.nSamplesPerSec)
   {
      case 11025: wp.m_nFrequency = WAVE_SAMPLE_FRQ_11025; break;
      case 22050: wp.m_nFrequency = WAVE_SAMPLE_FRQ_22050; break;
      case 44100: wp.m_nFrequency = WAVE_SAMPLE_FRQ_44100; break;
      default:    wp.m_nFrequency = -1; break;
   }

   if (wp.DoModal() == IDOK)
   {
      if (m_mmfL.IsValid())
      {
         m_dMaxMagnitudeL = wp.m_dMaxMagnitude_L * 0.01;
         if (wp.m_bRMSPegel) m_dMaxMagnitudeL = m_dMaxMagnitudeL * m_dMaxValueL / m_dRMS_ValueL;
      }

      if (m_mmfR.IsValid())
      {
         m_dMaxMagnitudeR = wp.m_dMaxMagnitude_R * 0.01;
         if (wp.m_bRMSPegel) m_dMaxMagnitudeR = m_dMaxMagnitudeR * m_dMaxValueR / m_dRMS_ValueR;
      }

      m_wfEX.nChannels = (wp.m_bStereo) ? 2 : 1;
      m_nInput         = wp.m_nInput;
      m_bInvertL       = (wp.m_bLInvert != 0) ? true : false;
      m_bInvertR       = (wp.m_bRInvert != 0) ? true : false;
      if (wp.m_nBits == WAVE_BITS_8)
      {
         m_wfEX.nBlockAlign    = m_wfEX.nChannels;             // 1 Byte
         m_wfEX.wBitsPerSample = 8;
      }
      else
      {
         m_wfEX.nBlockAlign    = 2 * m_wfEX.nChannels;         // 2 Byte
         m_wfEX.wBitsPerSample = 16;
      }
      switch (wp.m_nFrequency)
      {
         case WAVE_SAMPLE_FRQ_11025: m_wfEX.nSamplesPerSec = 11025; break;
         case WAVE_SAMPLE_FRQ_22050: m_wfEX.nSamplesPerSec = 22050; break;
         case WAVE_SAMPLE_FRQ_44100: m_wfEX.nSamplesPerSec = 44100; break;
         default:                    m_wfEX.nSamplesPerSec = 44100; break;
      }
      m_wfEX.nAvgBytesPerSec = m_wfEX.nSamplesPerSec * m_wfEX.nBlockAlign;
      m_bTimeFktChanged = true;
      UpdateAllViews(NULL, UPDATE_CHANGE_CONTENTS);
   }
}

void CWaveGenDoc::OnUpdateCalcNew(CCmdUI* pCmdUI)         {pCmdUI->Enable((m_hThread == INVALID_HANDLE_VALUE)&&!m_mmfL.IsValid());}

void CWaveGenDoc::OnUpdateCalcFourier(CCmdUI* pCmdUI)     {pCmdUI->Enable((m_hThread == INVALID_HANDLE_VALUE)&& m_mmfL.IsValid() && (m_bTimeFunction));}
void CWaveGenDoc::OnUpdateCalcReFourier(CCmdUI* pCmdUI)   {pCmdUI->Enable((m_hThread == INVALID_HANDLE_VALUE)&& m_mmfL.IsValid() && (!m_bTimeFunction));}

void CWaveGenDoc::OnUpdateCalcFilter(CCmdUI* pCmdUI)      {pCmdUI->Enable((m_hThread == INVALID_HANDLE_VALUE));}
void CWaveGenDoc::OnUpdateCalcMathFilter(CCmdUI* pCmdUI)  {pCmdUI->Enable((m_hThread == INVALID_HANDLE_VALUE)&& m_mmfL.IsValid() && (m_FilterL.pcFilterBox != NULL) && (!m_bTimeFunction));}

void CWaveGenDoc::OnUpdateOptionsFilter(CCmdUI* pCmdUI)   {pCmdUI->Enable((m_hThread == INVALID_HANDLE_VALUE)&&!m_bTimeFunction);}
void CWaveGenDoc::OnUpdateOptionsWavefile(CCmdUI* pCmdUI) {pCmdUI->Enable((m_hThread == INVALID_HANDLE_VALUE)&&m_bTimeFunction);}

void CWaveGenDoc::OnUpdateOptionsStandard(CCmdUI* pCmdUI) {pCmdUI->Enable((m_hThread == INVALID_HANDLE_VALUE)&&(m_nValues==0));}

void CWaveGenDoc::OnUpdateFileLoadLeftcurve(CCmdUI* pCmdUI) {pCmdUI->Enable((m_hThread == INVALID_HANDLE_VALUE)&& !m_mmfL.IsValid());}
void CWaveGenDoc::OnUpdateFileLoadRightcurve(CCmdUI* pCmdUI){pCmdUI->Enable((m_hThread == INVALID_HANDLE_VALUE)&& !m_mmfL.IsValid());}

void CWaveGenDoc::OnUpdateOptionsFilterEdit(CCmdUI* pCmdUI) {pCmdUI->Enable((m_hThread == INVALID_HANDLE_VALUE)&&!m_bTimeFunction);}

// Helfer Funktionen
bool CWaveGenDoc::IsArrayOk()
{
   if (m_mmfL.IsValid() || m_mmfR.IsValid())
      return true;
   return false;
}

double CWaveGenDoc::GetMinFrqStep()
{
   double dStep = (double)m_wfEX.nSamplesPerSec / (m_nValues-1);
   return dStep;
}

int CWaveGenDoc::GetNoOfFrqSteps()
{
   return (m_nValues >> 1);
}

void CWaveGenDoc::CheckFunktionValues()
{
   if (!IsArrayOk()) return;
   int    i, n;
   double dVal, dQSum, dSum;
   float *pfFourierL = (float*)m_mmfL.MapViewOfFile(0,0,0);
   float *pfFourierR = (float*)m_mmfR.MapViewOfFile(0,0,0);
   if (m_bTimeFunction)
   {
      int nValues;
      if (m_lWaveFileSize) nValues = m_wfEX.nSampleLength / m_wfEX.nBlockAlign;
      else                 nValues = m_nValues-1;
      if (pfFourierL)
      {
         m_dMaxValueL = dQSum = dSum = 0;
         for (i=0; i<nValues; i++)
         {
            dVal   = pfFourierL[i];
            dSum  += dVal;
            dQSum += (dVal*dVal);
            if (dVal < 0.0) dVal = -dVal;
            if (m_dMaxValueL < dVal) m_dMaxValueL = dVal;
         }
         m_dDCOffsetL  = dSum / (double)nValues;
         m_dRMS_ValueL = sqrt(dQSum / (double)nValues);
      }
      if (pfFourierR)
      {
         m_dMaxValueR = dQSum = dSum = 0;
         for (i=0; i<nValues; i++)
         {
            dVal   = pfFourierR[i];
            dSum  += dVal;
            dQSum += (dVal*dVal);
            if (dVal < 0.0) dVal = -dVal;
            if (m_dMaxValueR < dVal) m_dMaxValueR = dVal;
         }
         m_dDCOffsetR  = dSum / (double)nValues;
         m_dRMS_ValueR = sqrt(dQSum / (double)nValues);
      }
      int nBits = (m_wfEX.nBlockAlign / m_wfEX.nChannels) * 8;
      nBits = 1 << (nBits-1);
      m_dMaxMagnitudeL = m_dMaxValueL / (double)nBits;
      m_dMaxMagnitudeR = m_dMaxValueR / (double)nBits;
   }
   else
   {
      n = GetNoOfFrqSteps();
      if (pfFourierL)
      {
         m_dMaxValueL = dQSum = 0;
         for (i=1; i<n; i+=2)
         {
            dVal   = pfFourierL[i] * pfFourierL[i] + pfFourierL[i+1] * pfFourierL[i+1];
            dQSum += dVal;
            dVal   = sqrt(dVal);
            if (m_dMaxValueL < dVal) m_dMaxValueL = dVal;
         }
         m_dMaxValueL  = sqrt(m_dMaxValueL);
         m_dRMS_ValueL = sqrt(dQSum / (double)(n));
      }
      if (pfFourierR)
      {
         m_dMaxValueR = dQSum = 0;
         for (i=1; i<n; i+=2)
         {
            dVal   = pfFourierR[i] * pfFourierR[i] + pfFourierR[i+1] * pfFourierR[i+1];
            dQSum += dVal;
            dVal   = sqrt(dVal);
            if (m_dMaxValueR < dVal) m_dMaxValueR = dVal;
         }
         m_dMaxValueR  = sqrt(m_dMaxValueR);
         m_dRMS_ValueR = sqrt(dQSum / (double)(n));
      }
   }
}

void CWaveGenDoc::CalcFilterArray() 
{
   m_FilterL.bCalcArray = true;
   CalculateFilter(&m_FilterL, ID_CALC_MULTIPLY_FILTER);
}

void CWaveGenDoc::CalculateFilter(FilterBox *pFS, UINT nWhat)
{
   int    i, j, nFrequ; 
   double dFrequ, dStep;
   float *pfFourierL = (float*)m_mmfL.MapViewOfFile(0,0,0);
   float *pfFourierR = (float*)m_mmfR.MapViewOfFile(0,0,0);

   if (pFS->bCalcArray)
   {
      nFrequ = NO_OF_FREQUENCIES;
      // evtl. Bereich einstellen !
   }
   else
   {
      nFrequ = GetNoOfFrqSteps();
      dFrequ = 0;
      dStep  = GetMinFrqStep();
   }

   if ((pFS->nTPOrder == 0) && (pFS->nHPOrder == 0))  // Pink Noise
   {
      double  dF0 = pFS->dTPFreqSelected;
      CWaveGenApp *pApp = (CWaveGenApp*)AfxGetApp();
      pApp->CreateProgressDlg("Berechne Filter");
      pApp->ProgressDlgSetRange(nFrequ);

      for(i=0; i<nFrequ; i++)
      {
         if (pFS->bCalcArray) dFrequ = 5.0 * pow(2.0, (double)i/9.0);
         else                 dFrequ += dStep;

         if (pFS->bCalcArray) // Filterfunktin im Array ablegen
         {
            pFS->pcFilterBox[i] = PinkNoiseFilter(dF0, dFrequ);
         }
         else                 // Filterfunktion heranmultiplizieren
         {
            j = i << 1;
            if (pfFourierL)
               MathFilter(PinkNoiseFilter(dF0, dFrequ), pfFourierL[j+1], pfFourierL[j+2], nWhat);
            if (pfFourierR)
               MathFilter(PinkNoiseFilter(dF0, dFrequ), pfFourierR[j+1], pfFourierR[j+2], nWhat);
         }
         pApp->ProgressDlgStep();
      }
      pApp->DestroyProgressDlg();
   }
   else                                                        // Others
   {
      Complex     cSn;                                         // = j*2Pi*freq/2Pi*Grenzfreq
      Complex     cFilterKoeff1, cFilterKoeff2;

      double      da1[5][5], da2[5][5], db1[5][5], db2[5][5];  // Koeffizienten[Order][Type] der Filterpolynome

      for(i=0; i<5; i++)                                       // alle Koeff. auf Null setzen, max. Ordnung=4; max. Typ=4
      {                                                        // i = Ordnung, j = Typ
         for(j=0; j<5; j++)
         {
            da1[i][j] = 0.0;
            da2[i][j] = 0.0;
            db1[i][j] = 0.0;
            db2[i][j] = 0.0;
         }
      }
      da1[1][0] = da1[1][2] = da1[1][3] = 1.0;      // 1. Ordnung nur für Butterworth, Bessel u. Tschebyscheff

      da1[2][0] = 1.4142;                           // 2. Ordnung, Butterworth
      db1[2][0] = 1.0;
      da1[2][1] = 1.0;                              // 2. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
      db1[2][1] = 1.0;
      da1[2][2] = 1.3617;                           // 2. Ordnung, Bessel
      db1[2][2] = 0.6180;
      da1[2][3] = 1.3614;                           // 2. Ordnung, Tschebyscheff mit 0.5 dB Welligkeit
      db1[2][3] = 1.3827;

      da1[3][0] = 1.0;                              // 3. Ordnung, Butterworth
      da2[3][0] = 1.0;
      db2[3][0] = 1.0;
      da1[3][1] = 1.0;                              // 3. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
      da2[3][1] = 1.0;
      db2[3][1] = 1.0;
      da1[3][2] = 0.7560;                           // 3. Ordnung, Bessel
      da2[3][2] = 0.9996;
      db2[3][2] = 0.4772;
      da1[3][3] = 1.8636;                           // 3. Ordnung, Tschebyscheff mit 0.5 dB Welligkeit
      da2[3][3] = 0.6402;
      db2[3][3] = 1.1931;

      da1[4][0] = 1.8478;                           // 4. Ordnung, Butterworth
      db1[4][0] = 1.0;
      da2[4][0] = 0.7654;
      db2[4][0] = 1.0;
      da1[4][2] = 1.3397;                           // 4. Ordnung, Bessel
      db1[4][2] = 0.4889;
      da2[4][2] = 0.7743;
      db2[4][2] = 0.3890;
      da1[4][3] = 2.6282;                           // 4. Ordnung, Tschebyscheff mit 0.5 dB Welligkeit
      db1[4][3] = 3.4341;
      da2[4][3] = 0.3648;
      db2[4][3] = 1.1509;
      da1[4][4] = 1.4142;                           // 4. Ordnung, Linkwitz-Riley
      db1[4][4] = 1.0;
      da2[4][4] = 1.4142;
      db2[4][4] = 1.0;

      CWaveGenApp *pApp = (CWaveGenApp*)AfxGetApp();
      pApp->CreateProgressDlg("Berechne Filter");
      pApp->ProgressDlgSetRange(nFrequ);

/*
      int i, n;
      n = (m_nValues-1) / m_nFFTValues;
      for (i=0; i<n; i++)
      {
         if (i>0)
         {
            pfFourierL = &pfFourierL[m_nFFTValues];
            pfFourierR = &pfFourierR[m_nFFTValues];
         }
*/
      for(i=0; i<nFrequ; i++)
      {
         if (pFS->bCalcArray)
         {
            dFrequ = 5.0 * pow(2.0, (double)i/9.0);
         }
         else
         {
            dFrequ += dStep;
         }
         cFilterKoeff1 = Complex( 1.0, 0.0 );
         cFilterKoeff2 = Complex( 1.0, 0.0 );

         if( pFS->dHPFreqSelected > 0.0 )
            cSn = Complex( 0.0, dFrequ/pFS->dHPFreqSelected );
         else
            cSn = Complex( 0.0, dFrequ);           // HPFreq = 1 HZ

         if( (pFS->nHPType == 1) && (pFS->dHPQFaktor > 0.0) )
         {
            da1[2][1] = 1.0/pFS->dHPQFaktor; // 2. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
            da2[3][1] = 1.0/pFS->dHPQFaktor; // 3. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
         }

         if( (pFS->nHPOrder > 0) && (pFS->nHPOrder < 5) &&
             (pFS->nHPType >= 0) && (pFS->nHPType  < 5) )
         {
            cFilterKoeff1  = 1.0 + da1[pFS->nHPOrder][pFS->nHPType]/cSn +
                                   db1[pFS->nHPOrder][pFS->nHPType]/(cSn*cSn);
            cFilterKoeff1 *= 1.0 + da2[pFS->nHPOrder][pFS->nHPType]/cSn +
                                   db2[pFS->nHPOrder][pFS->nHPType]/(cSn*cSn);
            cFilterKoeff1  = 1.0/cFilterKoeff1;
         }

         if( pFS->dTPFreqSelected > 0.0 )
            cSn = Complex( 0.0, dFrequ/pFS->dTPFreqSelected);
         else
            cSn = Complex( 0.0, dFrequ/1000000.0 ); // TPFreq = 1 MHZ

         if( (pFS->nTPType == 1) && (pFS->dTPQFaktor > 0.0) )
         {
            da1[2][1] = 1.0/pFS->dTPQFaktor; // 2. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
            da2[3][1] = 1.0/pFS->dTPQFaktor; // 3. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
         }

         if( (pFS->nTPOrder > 0) && (pFS->nTPOrder < 5) &&
             (pFS->nTPType >= 0) && (pFS->nTPType  < 5) )
         {
            cFilterKoeff2  = 1.0 + da1[pFS->nTPOrder][pFS->nTPType]*cSn +
                                   db1[pFS->nTPOrder][pFS->nTPType]*cSn*cSn;
            cFilterKoeff2 *= 1.0 + da2[pFS->nTPOrder][pFS->nTPType]*cSn +
                                   db2[pFS->nTPOrder][pFS->nTPType]*cSn*cSn;
            cFilterKoeff2  = 1.0/cFilterKoeff2;
         }

         if (pFS->bCalcArray) // Filterfunktin im Array ablegen
         {
            pFS->pcFilterBox[i] = cFilterKoeff1*cFilterKoeff2;
         }
         else                 // Filterfunktion heranmultiplizieren
         {
            j = i << 1;
            if (pfFourierL)
               MathFilter(cFilterKoeff1*cFilterKoeff2, pfFourierL[j+1], pfFourierL[j+2], nWhat);
            if (pfFourierR)
               MathFilter(cFilterKoeff1*cFilterKoeff2, pfFourierR[j+1], pfFourierR[j+2], nWhat);
         }
         pApp->ProgressDlgStep();
      }
//      }
      pApp->DestroyProgressDlg();
   }
}

void CWaveGenDoc::MathFilter(Complex cFK, float &a, float&b, int nWhat)
{
   double  dA    = hypot(a,b);
   double  dPhi  = atan2(a,b);
   Complex cTemp;
   switch (nWhat)
   {
      case ID_CALC_MULTIPLY_FILTER: cTemp = dA * Complex(cos(dPhi), sin(dPhi)) * cFK; break;
      case ID_CALC_DIVIDE_FILTER  : cTemp = dA * Complex(cos(dPhi), sin(dPhi)) / cFK; break;
      case ID_CALC_ADD_FILTER     : cTemp = dA * Complex(cos(dPhi), sin(dPhi)) + cFK; break;
      case ID_CALC_SUBTRACT_FILTER: cTemp = dA * Complex(cos(dPhi), sin(dPhi)) - cFK; break;
      default: return;
   }
   dA   = Betrag(cTemp);
   dPhi = atan2(Imag(cTemp), Real(cTemp));
   a    = (float)(dA * sin(dPhi));
   b    = (float)(dA * cos(dPhi));
}

Complex CWaveGenDoc::PinkNoiseFilter(double dF0, double dFrequ)
{
   double dArgument = atan2(-dFrequ, dF0) * 0.5;
   return Complex(cos(dArgument), sin(dArgument)) / 
                  pow(1.0 + (dFrequ*dFrequ)/(dF0*dF0), 0.25);
}

void CWaveGenDoc::OnFileLoadLeftcurve() 
{
   CWaveGenApp * pApp = (CWaveGenApp*) AfxGetApp();
   m_nLoadOption = LOAD_LEFT_CURVE;
   pApp->OnFileOpen();
   m_nLoadOption = 0;
}

void CWaveGenDoc::OnFileLoadRightcurve() 
{
   CWaveGenApp * pApp = (CWaveGenApp*) AfxGetApp();
   m_nLoadOption = LOAD_RIGHT_CURVE;
   pApp->OnFileOpen();
   m_nLoadOption = 0;
}


void CWaveGenDoc::OnCalcCancel() 
{
   if (!m_strHistory.IsEmpty())
      UpdateAllViews(NULL, UPDATE_INSERT_HISTORY, (CObject*)&m_strHistory);
   CalcCancel(false);
}

void CWaveGenDoc::CalcCancel(bool bBreak)
{
   CWaveGenApp * pApp = (CWaveGenApp*) AfxGetApp();
   if ((m_hThread != INVALID_HANDLE_VALUE) && (m_nThreadID != 0))
   {
      DWORD dwExitCode;                                           // Thread beenden
      BOOL  bGet = GetExitCodeThread(m_hThread, &dwExitCode);
      while (bGet && (dwExitCode == STILL_ACTIVE))
      {
         ::TerminateThread(m_hThread, 1);
         bGet = GetExitCodeThread(m_hThread, &dwExitCode);
         ::Sleep(2);
      };
   }

   pApp->DestroyProgressDlg();
   if (!bBreak)
   {
      CheckFunktionValues();
      UpdateAllViews(NULL, UPDATE_INSERT_CONTENTS);
   }
   ::CloseHandle(m_hThread);
   m_hThread   = INVALID_HANDLE_VALUE;
   m_nThreadID = 0;
}

void CWaveGenDoc::OnUpdateCalcCancel(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(m_hThread != INVALID_HANDLE_VALUE);
}

void CWaveGenDoc::ProgressCallback(void *pParam, int nValue)
{
   ASSERT(pParam);
   CWaveGenApp *pApp = (CWaveGenApp*)pParam;
   if (nValue == -1)
      pApp->ProgressDlgStep();
   else
      pApp->ProgressDlgSetRange(nValue);
}

void CWaveGenDoc::OnPlayWave() 
{
   if (m_bTimeFunction)
   {
      OnPlayStop();
      if (m_bTimeFktChanged)
      {
         void *pWave;
         CMemFile mf;
         CArchive ar(&mf, CArchive::store | CArchive::bNoFlushOnDelete, 1024);
         ar.m_pDocument = this;
         Serialize(ar);
         ar.Flush();
         pWave = mf.Detach();
         ASSERT(pWave==m_mmfWave.MapViewOfFile(0, 0, 0));
         m_bTimeFktChanged = false;
      }
     

      if (m_mmfWave.IsValid())
      {
         LPCSTR pWave = (LPCSTR) m_mmfWave.MapViewOfFile(0, 0, 0);
         DWORD dwFlags = SND_ASYNC|SND_MEMORY;
         if (m_bRepeatWave)
         {
            dwFlags |= SND_LOOP;
         }
         PlaySound(pWave, NULL, dwFlags);
      }
      else OnPlayStop();
   }

}

void CWaveGenDoc::OnPlayStop() 
{
   PlaySound((LPCSTR)m_mmfWave.MapViewOfFile(0, 0, 0), NULL, SND_PURGE);
}

void CWaveGenDoc::OnUpdatePlayWave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bTimeFunction);
}


void CWaveGenDoc::OnUpdatePlayStop(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bTimeFunction);
}

void CWaveGenDoc::OnRepeatWave() 
{
   m_bRepeatWave = !m_bRepeatWave;
}

void CWaveGenDoc::OnUpdateRepeatWave(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(true);
   pCmdUI->SetCheck(m_bRepeatWave);
}

void CWaveGenDoc::SetArraySizes(bool bLeft, bool bRight)
{
   DWORD lValues = m_nValues;
   ULONGLONG liVal = UInt32x32To64(lValues, sizeof(float));
   LARGE_INTEGER *pliVal = (LARGE_INTEGER*)&liVal;

   if (bLeft)
   {
      m_mmfL.Alloc(pliVal->HighPart, pliVal->LowPart, true, true, true, true);
   }
   if (bRight)
   {
      m_mmfR.Alloc(pliVal->HighPart, pliVal->LowPart, true, true, true, true);
   }
}

CString & CWaveGenDoc::GetTitle()
{
   return m_strTitle;
}

BOOL CWaveGenDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CCARADoc::OnOpenDocument(lpszPathName))
		return FALSE;
	
	return TRUE;
}

void CWaveGenDoc::UpdateFrameCounts()
{
   if (m_bNewDocContent)
      UpdateAllViews(NULL, UPDATE_INSERT_CONTENTS);
   m_bNewDocContent = false;
}

void CWaveGenDoc::OnCalcTimeFunction() 
{
   CCreateTimeFunction dlg;
   if (dlg.DoModal()==IDOK)
   {
      if (dlg.m_bSweep)
      {
      }
      else
      {

      }
   }
}

void CWaveGenDoc::ComplexFFT(int nPart, int nValues, int nLeft)
{
   double   *data;                               // Eingabedaten (Frequenzspektrum), Ausgabedaten (Zeitwerte); oder umgekehrt
   float    *pOrgData = NULL;
   int      i, j, k, N_2, N, NN;
   if (!m_mmfFFT.IsValid())
   {
      ULONGLONG liVal = UInt32x32To64(nValues, sizeof(Complex));
      LARGE_INTEGER *pliVal = (LARGE_INTEGER*)&liVal;
      m_mmfFFT.Alloc(pliVal->HighPart, pliVal->LowPart, true, true, true, true);
   }

   Complex *pData = (Complex*) m_mmfFFT.MapViewOfFile(0,0,0);
   if (!pData) return;

   if (nLeft == 0)
      pOrgData = (float*)m_mmfL.MapViewOfFile(0,0,0);
   else
      pOrgData = (float*)m_mmfR.MapViewOfFile(0,0,0);
   if (!pOrgData) return;

   if (nPart>0)
   {
      if (((nPart+1)*nValues)>m_nValues) return;
      int nOff = nPart*nValues;
      pOrgData = &pOrgData[nOff];
   }
   N   = nValues;                             // Anzahl der komplexen Werte im Zeit- und im (gespiegelten) Freq-Array
   NN  = N*2;                                 // echte Array-Länge, da Real- und ImagTeil getrennt als double-Zahlen
   N_2 = N/2;                                 // Es werden nur halb soviel komplexe FreqWerte (2er Potenz !!) übergeben 
                                              // wie (komplexe) Zeitwerte sich ergeben sollen. Daher FreqSpektrum spiegeln.
   int flag = m_bTimeFunction ? -1 : +1;
   if( flag == +1)                            // inverse FFT, FreqSpektrum -> Zeitspektrum
   {
      // Eingabedaten in erweiterten Arbeits/Zwischen-Array data[] schreiben

//      double dFakt = 1.0/sqrt(2.0);         // Faktor zur Leistungsnormierung im FreqSpektrum, da dieses gespiegelt
                                              // wird und sich dadurch eine Leistungsverdopplung ergäbe.
      pData[0].realteil = pData[0].imagteil = 0.0; // Werte für 0 Hz
      j= 1;
      k = N-1;
      for(i=1; i<=N_2; i++)
      {
         pData[i].realteil = pOrgData[j++];
         pData[i].imagteil = pOrgData[j++];
         pData[k--] = Konj(pData[i]);
      }
      pData[N_2].imagteil = 0;
   }
   else                                          // FFT, Zeitspektrum -> Frequenzspektrum
   {
      // Eingabedaten in erweiterte Arbeits/Zwischen-Arrays schreiben
      for(i=0; i<N; i++)
      {
         pData[i] = Complex((double) pOrgData[i], 0.0);  // reelle Orgwerte -> Realteil
      }
   }


   data = (double*) m_mmfFFT.MapViewOfFile(0,0,0);
   //do the bit reversal and sort the input data
   int    nI, nIreverse = 0;
   int    nStage;

   CWaveGenApp *pApp = (CWaveGenApp*)AfxGetApp();
   for( nStage = 2; nStage < NN; nStage *= 2 )
   {
      int      nCombs, k2;                       //Schleifenzähler für die Kombinationen und über die k
      int      nStep = nStage*2;
      for( nCombs = 0; nCombs < (nStage-1); nCombs+=2 )
         for( k2 = nCombs; k2 < NN; k2 += nStep )
            nIreverse++;
   }
   pApp->ProgressDlgSetRange(nIreverse);
   
   for( nI = 0; nI < N; nI++ )
   {
      int     nMask = N_2;
      int     nReverseMask = 1;                  //Startwerte initialisieren
      nIreverse = 0;

      while( nMask >= 1 )
      {
         if( nI&nMask )   nIreverse += nReverseMask;
         nMask        /= 2;
         nReverseMask *= 2;
      }
      if ( (nI != nIreverse) && (nI < nIreverse) )//wenn i und i_reverse verschieden sind
      {                                           //müssen die Daten ausgetauscht werden
         int      i  = nI*2;
         int      ir = nIreverse*2;
         double   tempr = data[i];
         double   tempi = data[i+1];
         data[i]    = data[ir];
         data[i+1]  = data[ir+1];
         data[ir]   = tempr;
         data[ir+1] = tempi;
      }
   }
   //bit reversal abgeschlossen

   //mit der eigentlichen FFT beginnen
   for( nStage = 2; nStage < NN; nStage *= 2 )
   {
      int      nStep = nStage*2;
      double   dphi  = 6.28318530717959/(flag*nStage);

      double   wtemp = sin(0.5*dphi);
      double   wpr   = -2.0*wtemp*wtemp;
      double   wpi   = sin(dphi);

      double   wr    = 1.0;
      double   wi    = 0.0;

      //alle Werte initialisiert und dphi berechnet für dieses nStage
      int      nCombs, k2;                       //Schleifenzähler für die Kombinationen und über die k

      for( nCombs = 0; nCombs < (nStage-1); nCombs+=2 )
      {
         for( k2 = nCombs; k2 < NN; k2 += nStep )
         {
            int      nkPartner = k2 + nStage;
            //Ausführen der Kombination zweier halber FFTs
            double   tempr = wr*data[nkPartner]   - wi*data[nkPartner+1]; 
            double   tempi = wr*data[nkPartner+1] + wi*data[nkPartner];

            data[nkPartner]   = data[k2]   - tempr;
            data[nkPartner+1] = data[k2+1] - tempi;

            data[k2]     += tempr;
            data[k2+1]   += tempi;
            pApp->ProgressDlgStep();
         }
         //neue Werte für W bestimmen (siehe Dokumentation)
         wtemp = wr;
         wr    = wr*wpr - wi*wpi+wr;
         wi    = wi*wpr + wtemp*wpi+wi;
      }
   }
   if( flag == +1)                               // iFFT mit 1/N normieren, Zeitspektrum zurückschreiben
   {
      double d1dn = 1.0 / N;                     // Normieren
      for(i=0; i<N; i++)
      {
         pOrgData[i] = (float)(pData[i].realteil * d1dn);
      }
   }
   else                                          // FreqSpektrum zurückschreiben; wenn nur die ersten N/2 Frequenzen
   {                                             // geschrieben werden, dann mit sqrt(2.0) multiplizieren
      pOrgData[0] = (float)pData[0].realteil;
      j= 1;
      for(i=1; i<=N_2; i++)
      {
         pOrgData[j++] = (float)pData[i].realteil;
         pOrgData[j++] = (float)pData[i].imagteil;
      }
   }
}
