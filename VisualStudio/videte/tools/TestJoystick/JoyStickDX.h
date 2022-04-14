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
#define JOY_CAPS_SLIDER1	0x00000040
#define JOY_CAPS_SLIDER2	0x00000080
#define JOY_CAPS_POV1		0x00000100
#define JOY_CAPS_POV2		0x00000200
#define JOY_CAPS_POV3		0x00000400
#define JOY_CAPS_POV4		0x00000800
#define JOY_CAPS_BUTTON1	0x00001000
#define JOY_CAPS_BUTTON2	0x00002000
#define JOY_CAPS_BUTTON3	0x00004000
#define JOY_CAPS_BUTTON4	0x00008000
#define JOY_CAPS_BUTTON5	0x00010000
#define JOY_CAPS_BUTTON6	0x00020000
#define JOY_CAPS_BUTTON7	0x00040000
#define JOY_CAPS_BUTTON8	0x00080000
#define JOY_CAPS_KEY1		0x00100000
#define JOY_CAPS_KEY2		0x00200000
#define JOY_CAPS_KEY3		0x00400000
#define JOY_CAPS_KEY4		0x00800000
#define JOY_CAPS_KEY5		0x01000000
#define JOY_CAPS_KEY6		0x02000000
#define JOY_CAPS_KEY7		0x04000000
#define JOY_CAPS_KEY8		0x08000000

class CJoyStickDX
{
public:
	CJoyStickDX();
	~CJoyStickDX();
private:

	LPDIRECTINPUT8			m_pDI;
	LPDIRECTINPUTDEVICE8	m_pJoystick;
	int						m_nSliderCount;
	int						m_nPOVCount;
	int						m_nButtonCount;
	int						m_nKeyCount;
	DWORD					m_dwCapabilities;

public:
	HRESULT		Init(HWND hWnd);
	DWORD		GetCapabilities() { return m_dwCapabilities;};

private:
	static BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);
	static BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext);
};
