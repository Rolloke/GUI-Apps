// AudioSOM.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


struct ErrorStruct
{
	ErrorStruct(HRESULT hr, int nLine, const char*sFile) 
	{
		m_hr = hr;
		m_nLine = nLine;
		m_sFile = sFile;
	}
	int         m_nLine;
	HRESULT     m_hr;
	const char* m_sFile;
};

#define HRESULT_ERROR_AT_POS(hr) new ErrorStruct(hr, __LINE__, __FILE__)
#define HR_EXCEPTION(HR) if (FAILED(hr=HR))	throw HRESULT_ERROR_AT_POS(hr);
#define HR_REPORT(HR) if (FAILED(hr=HR))	ReportError(HRESULT_ERROR_AT_POS(hr), false);
#define HR_MSG_BOX(HR) if (FAILED(hr=HR))	ReportError(HRESULT_ERROR_AT_POS(hr));

#define SECTION_SETTINGS    _T("Settings")
#define AUDIO_SOURCE        _T("AudioInput")
#define AUDIO_SOURCE_PIN    _T("AudioInputPin")
#define AUDIO_SOURCE_LEVEL  _T("AudioInputLevel")
#define AUDIO_SOURCE_CAP    _T("AudioInputCap")
#define AUDIO_SAMPLE_LEN    _T("AudioSampleLength")
#define AUDIO_RENDER        _T("AudioOutput")
#define AUDIO_OUT_VOLUME    _T("OutputVolume")
#define AUDIO_OUT_FRQ       _T("OutputFrq")
#define AUDIO_OUT_BALANCE   _T("OutputBalance")

#define SECTION_SYNTHESIZER _T("Synthesizer")
#define SYNTH_WAVEFORM      _T("WaveForm")
#define SYNTH_CORRELATED    _T("Correlated")
#define SYNTH_FREQUENCY1    _T("Frequency1")
#define SYNTH_FREQUENCY2    _T("Frequency2")
#define SYNTH_TIME          _T("Time")
#define SYNTH_PHASE         _T("Phase")
#define SYNTH_MAGNITUDE1    _T("Magnitude1")
#define SYNTH_MAGNITUDE2    _T("Magnitude2")
#define SYNTH_OFFSET1       _T("Offset1")
#define SYNTH_OFFSET2       _T("Offset2")
#define SYNTH_CHANNEL       _T("Channel")
#define SYNTH_BITS_PER_SAMP _T("BitsPerSample")

#define SECTION_FILE       _T("File")
#define FILE_INPUT_NAME    _T("Input")
#define FILE_OUTPUT_NAME   _T("Output")
// CAudioSOMApp:
// See AudioSOM.cpp for the implementation of this class
//

class CAudioSOMApp : public CWinApp
{
public:
	CAudioSOMApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation
private:
   void InstallFilter(const CString& sID, const CString &sPath);
	DECLARE_MESSAGE_MAP()
};

extern CAudioSOMApp theApp;