#pragma once


// CAudioInputMixerProperties dialog

class CAudioInputMixerProperties : public CPropertyPage
{
	DECLARE_DYNAMIC(CAudioInputMixerProperties)

public:
	CAudioInputMixerProperties();
	virtual ~CAudioInputMixerProperties();

   void SetName(CString&sName);
   void SetPin(IPin*pPin, int i);
// Dialog Data
	enum { IDD = IDD_AUDIO_INPUT_MIXER };
   CSliderCtrl m_cLevel;
   CSliderCtrl m_cBalance;
   CSliderCtrl m_cBass;
   CSliderCtrl m_cTreble;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnApply();
   virtual BOOL OnInitDialog();

   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnBnClickedCkInput();
   afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
private:
   IPin *m_pPin;
   int   m_iPin;
   IAMAudioInputMixer *m_pAudioInputMixer;
};
