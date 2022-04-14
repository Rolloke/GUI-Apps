#include "StdAfx.h"
#include "joystickdx.h"

CJoyStickDX::CJoyStickDX()
{
	m_pDI = NULL;
	m_pJoystick = NULL;
	m_nSliderCount = 0;
	m_nPOVCount = 0;
	m_nButtonCount = 0;
	m_nKeyCount = 0;
	m_dwCapabilities = 0;
}

CJoyStickDX::~CJoyStickDX()
{
    if( m_pJoystick )
	{
       m_pJoystick->Unacquire();
	   m_pJoystick->Release();
	}
	if (m_pDI)
	{
		m_pDI->Release();
	}
}

HRESULT CJoyStickDX::Init(HWND hWnd)
{

	HRESULT hr;
    // Register with the DirectInput subsystem and get a pointer
    // to a IDirectInput interface we can use.
    // Create a DInput object
	hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&m_pDI, NULL);
    if (FAILED(hr)) return hr;

    // Look for a simple joystick we can use for this sample program.
	hr = m_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);
    if (FAILED(hr)) return hr;

    // Make sure we got a joystick
    if (NULL == m_pJoystick)
    {
        return S_FALSE;
    }

    // Set the data format to "simple joystick" - a predefined data format 
    //
    // A data format specifies which controls on a device we are interested in,
    // and how they should be reported. This tells DInput that we will be
    // passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
	hr = m_pJoystick->SetDataFormat(&c_dfDIJoystick2);
    if (FAILED(hr)) return hr;

    // Set the cooperative level to let DInput know how this device should
    // interact with the system and with other DInput applications.
	hr = m_pJoystick->SetCooperativeLevel( hWnd, DISCL_EXCLUSIVE|DISCL_FOREGROUND);
    if (FAILED(hr)) return hr;

    // Enumerate the joystick objects. The callback function enabled user
    // interface elements for objects that are found, and sets the min/max
    // values property for discovered axes.
	m_nSliderCount = 0;
	m_nPOVCount = 0;
	m_nButtonCount = 0;
	m_nKeyCount = 0;
	hr = m_pJoystick->EnumObjects(EnumObjectsCallback, (VOID*)this, DIDFT_ALL);
    if (FAILED(hr)) return hr;

    return S_OK;

}

HRESULT CJoyStickDX::UpdateInputState(HWND hDlg)
{
    HRESULT     hr;
    TCHAR       strText[512]; // Device state text
    DIJOYSTATE2 js;           // DInput joystick state 
    TCHAR*      str;

    if( NULL == g_pJoystick ) 
        return S_OK;

    // Poll the device to read the current state
    hr = g_pJoystick->Poll(); 
    if( FAILED(hr) )  
    {
        // DInput is telling us that the input stream has been
        // interrupted. We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done. We
        // just re-acquire and try again.
        hr = g_pJoystick->Acquire();
        while( hr == DIERR_INPUTLOST ) 
            hr = g_pJoystick->Acquire();

        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
        // may occur when the app is minimized or in the process of 
        // switching, so just try again later 
        return S_OK; 
    }

    // Get the input's device state
    if( FAILED( hr = g_pJoystick->GetDeviceState( sizeof(DIJOYSTATE2), &js ) ) )
        return hr; // The device should have been acquired during the Poll()

    // Display joystick state to dialog

    // Axes
    wsprintf( strText, TEXT("%ld"), js.lX ); 
    SetWindowText( GetDlgItem( hDlg, IDC_X_AXIS ), strText );
    wsprintf( strText, TEXT("%ld"), js.lY ); 
    SetWindowText( GetDlgItem( hDlg, IDC_Y_AXIS ), strText );
    wsprintf( strText, TEXT("%ld"), js.lZ ); 
    SetWindowText( GetDlgItem( hDlg, IDC_Z_AXIS ), strText );
    wsprintf( strText, TEXT("%ld"), js.lRx ); 
    SetWindowText( GetDlgItem( hDlg, IDC_X_ROT ), strText );
    wsprintf( strText, TEXT("%ld"), js.lRy ); 
    SetWindowText( GetDlgItem( hDlg, IDC_Y_ROT ), strText );
    wsprintf( strText, TEXT("%ld"), js.lRz ); 
    SetWindowText( GetDlgItem( hDlg, IDC_Z_ROT ), strText );

    // Slider controls
    wsprintf( strText, TEXT("%ld"), js.rglSlider[0] ); 
    SetWindowText( GetDlgItem( hDlg, IDC_SLIDER0 ), strText );
    wsprintf( strText, TEXT("%ld"), js.rglSlider[1] ); 
    SetWindowText( GetDlgItem( hDlg, IDC_SLIDER1 ), strText );

    // Points of view
    wsprintf( strText, TEXT("%ld"), js.rgdwPOV[0] );
    SetWindowText( GetDlgItem( hDlg, IDC_POV0 ), strText );
    wsprintf( strText, TEXT("%ld"), js.rgdwPOV[1] );
    SetWindowText( GetDlgItem( hDlg, IDC_POV1 ), strText );
    wsprintf( strText, TEXT("%ld"), js.rgdwPOV[2] );
    SetWindowText( GetDlgItem( hDlg, IDC_POV2 ), strText );
    wsprintf( strText, TEXT("%ld"), js.rgdwPOV[3] );
    SetWindowText( GetDlgItem( hDlg, IDC_POV3 ), strText );
 
   
    // Fill up text with which buttons are pressed
    str = strText;
    for( int i = 0; i < 128; i++ )
    {
        if ( js.rgbButtons[i] & 0x80 )
            str += wsprintf( str, TEXT("%02d "), i );
    }
    *str = 0;   // Terminate the string 

    SetWindowText( GetDlgItem( hDlg, IDC_BUTTONS ), strText );

    return S_OK;
}

BOOL CALLBACK CJoyStickDX::EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
{
    HRESULT hr;
	CJoyStickDX *pThis = (CJoyStickDX*)pContext;
    // Obtain an interface to the enumerated joystick.
    hr = pThis->m_pDI->CreateDevice( pdidInstance->guidInstance, &pThis->m_pJoystick, NULL );

    // If it failed, then we can't use this joystick. (Maybe the user unplugged
    // it while we were in the middle of enumerating it.)
    if( FAILED(hr) ) 
        return DIENUM_CONTINUE;

    // Stop enumeration. Note: we're just taking the first joystick we get. You
    // could store all the enumerated joysticks and let the user pick.
    return DIENUM_STOP;
}

BOOL CALLBACK CJoyStickDX::EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext )
{
	CJoyStickDX *pThis = (CJoyStickDX*)pContext;

    // For axes that are returned, set the DIPROP_RANGE property for the
    // enumerated axis in order to scale min/max values.
    if( pdidoi->dwType & DIDFT_AXIS )
    {
        DIPROPRANGE diprg; 
        diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
        diprg.diph.dwHow        = DIPH_BYID; 
        diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
        diprg.lMin              = -1000; 
        diprg.lMax              = +1000; 
    
        // Set the range for the axis
        if( FAILED( pThis->m_pJoystick->SetProperty( DIPROP_RANGE, &diprg.diph ) ) ) 
            return DIENUM_STOP;
         
    }


    // Set the UI to reflect what objects the joystick supports
    if (pdidoi->guidType == GUID_XAxis)
    {
		pThis->m_dwCapabilities |= JOY_CAPS_XAXIS;
    }
    if (pdidoi->guidType == GUID_YAxis)
    {
		pThis->m_dwCapabilities |= JOY_CAPS_YAXIS;
    }
    if (pdidoi->guidType == GUID_ZAxis)
    {
		pThis->m_dwCapabilities |= JOY_CAPS_ZAXIS;
    }
    if (pdidoi->guidType == GUID_RxAxis)
    {
		pThis->m_dwCapabilities |= JOY_CAPS_RXAXIS;
    }
    if (pdidoi->guidType == GUID_RyAxis)
    {
		pThis->m_dwCapabilities |= JOY_CAPS_RYAXIS;
    }
    if (pdidoi->guidType == GUID_RzAxis)
    {
 		pThis->m_dwCapabilities |= JOY_CAPS_RZAXIS;
   }
    if (pdidoi->guidType == GUID_Slider)
    {
		switch( pThis->m_nSliderCount++)
        {
            case 0 :
 				pThis->m_dwCapabilities |= JOY_CAPS_SLIDER1;
                break;

            case 1 :
 				pThis->m_dwCapabilities |= JOY_CAPS_SLIDER2;
                break;
        }
    }
    if (pdidoi->guidType == GUID_POV)
    {
		switch(pThis->m_nPOVCount++)
        {
            case 0 :
 				pThis->m_dwCapabilities |= JOY_CAPS_POV1;
                break;

            case 1 :
 				pThis->m_dwCapabilities |= JOY_CAPS_POV2;
                break;

            case 2 :
 				pThis->m_dwCapabilities |= JOY_CAPS_POV3;
                break;

            case 3 :
 				pThis->m_dwCapabilities |= JOY_CAPS_POV4;
                break;
        }
    }
    if (pdidoi->guidType == GUID_Button)
    {
		switch(pThis->m_nButtonCount++)
        {
            case 0 :
				pThis->m_dwCapabilities |= JOY_CAPS_BUTTON1;
                break;

            case 1 :
 				pThis->m_dwCapabilities |= JOY_CAPS_BUTTON2;
                break;

            case 2 :
 				pThis->m_dwCapabilities |= JOY_CAPS_BUTTON3;
                break;

            case 3 :
 				pThis->m_dwCapabilities |= JOY_CAPS_BUTTON4;
                break;

			case 4 :
				pThis->m_dwCapabilities |= JOY_CAPS_BUTTON5;
                break;

            case 5 :
 				pThis->m_dwCapabilities |= JOY_CAPS_BUTTON6;
                break;

            case 6 :
 				pThis->m_dwCapabilities |= JOY_CAPS_BUTTON7;
                break;

            case 7 :
 				pThis->m_dwCapabilities |= JOY_CAPS_BUTTON8;
                break;
		}
    }
	if (pdidoi->guidType == GUID_Key)
    {
		switch(pThis->m_nKeyCount++)
        {
            case 0 :
				pThis->m_dwCapabilities |= JOY_CAPS_KEY1;
                break;

            case 1 :
 				pThis->m_dwCapabilities |= JOY_CAPS_KEY2;
                break;

            case 2 :
 				pThis->m_dwCapabilities |= JOY_CAPS_KEY3;
                break;

            case 3 :
 				pThis->m_dwCapabilities |= JOY_CAPS_KEY4;
                break;

             case 4 :
				pThis->m_dwCapabilities |= JOY_CAPS_KEY5;
                break;

            case 5 :
 				pThis->m_dwCapabilities |= JOY_CAPS_KEY6;
                break;

            case 6 :
 				pThis->m_dwCapabilities |= JOY_CAPS_KEY7;
                break;

            case 7 :
 				pThis->m_dwCapabilities |= JOY_CAPS_KEY8;
                break;
        }
	}

    return DIENUM_CONTINUE;
}
