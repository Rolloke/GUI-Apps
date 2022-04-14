#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <basetsd.h>
#include <dinput.h>

#define JOY_CAPS_XAXIS		0x00000001
#define JOY_CAPS_YAXIS		0x00000002
#define JOY_CAPS_ZAXIS		0x00000004
#define JOY_CAPS_RXAXIS		0x00000008
#define JOY_CAPS_RYAXIS		0x00000010
#define JOY_CAPS_RZAXIS		0x00000020

#define INITF_USE_DIJOYSTATE2 0x00000001

class CJoyStickDX
{
// Construction/Destruction
public:
	CJoyStickDX();
	~CJoyStickDX();

// Attributes
public:
	inline DWORD	GetCapabilities();
	inline int		GetNoOfSliders();
	inline int		GetNoOfButtons();
	inline int		GetNoOfKeys();
	inline int		GetNoOfPOVs();
	inline int		GetAxisRangeMin();
	inline int		GetAxisRangeMax();
	
// Implementation
public:
	HRESULT		Init(HWND hWnd, DWORD dwCooperativeLevel, DWORD dwFlags);
	HRESULT		UpdateInputState(DIJOYSTATE *pjs, DIJOYSTATE2 *pjs2=NULL);
	void		SetAxisRange(int nMin, int nMax);
// private Data
private:
	LPDIRECTINPUT8			m_pDI;
	LPDIRECTINPUTDEVICE8	m_pJoystick;
	int						m_nRangeMin;
	int						m_nRangeMax;
	int						m_nSliderCount;
	int						m_nPOVCount;
	int						m_nButtonCount;
	int						m_nKeyCount;
	DWORD					m_dwCapabilities;

private:
	static BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);
	static BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext);
};

inline DWORD CJoyStickDX::GetCapabilities()
{
	return m_dwCapabilities;
}
inline int CJoyStickDX::GetNoOfSliders()
{
	return m_nSliderCount;
}
inline int CJoyStickDX::GetNoOfButtons()
{
	return m_nButtonCount;
}
inline int CJoyStickDX::GetNoOfKeys()
{
	return m_nKeyCount;
}
inline int CJoyStickDX::GetNoOfPOVs()
{
	return m_nPOVCount;
}
inline int CJoyStickDX::GetAxisRangeMin()
{
	return m_nRangeMin;
}
inline int CJoyStickDX::GetAxisRangeMax()
{
	return m_nRangeMax;
}

