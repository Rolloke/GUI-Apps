#include "StdAfx.h"
#include "joystickdx.h"
/////////////////////////////////////////////////////////////////////////////////////////
CJoyStickDX::CJoyStickDX()
{
	m_pDI = NULL;
	m_pJoystick = NULL;
	m_nRangeMin = -1000;
	m_nRangeMax =  1000;
	m_nSliderCount = 0;
	m_nPOVCount = 0;
	m_nButtonCount = 0;
	m_nKeyCount = 0;
	m_dwCapabilities = 0;
}
/////////////////////////////////////////////////////////////////////////////////////////
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
void CJoyStickDX::SetAxisRange(int nMin, int nMax)
{
	m_nRangeMin = nMin;
	m_nRangeMax = nMax;
}
/////////////////////////////////////////////////////////////////////////////////////////
HRESULT CJoyStickDX::Init(HWND hWnd, DWORD dwCooperativeLevel, DWORD dwFlags)
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
	if (INITF_USE_DIJOYSTATE2 & dwFlags)
	{
		hr = m_pJoystick->SetDataFormat(&c_dfDIJoystick2);
	}
	else
	{
		hr = m_pJoystick->SetDataFormat(&c_dfDIJoystick);
	}
    if (FAILED(hr)) return hr;

    // Set the cooperative level to let DInput know how this device should
    // interact with the system and with other DInput applications.
	hr = m_pJoystick->SetCooperativeLevel( hWnd, dwCooperativeLevel);
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
/////////////////////////////////////////////////////////////////////////////////////////
HRESULT CJoyStickDX::UpdateInputState(DIJOYSTATE *pjs, DIJOYSTATE2 *pjs2)
{
    HRESULT     hr;

    if( NULL == m_pJoystick ) 
		return E_NOINTERFACE;
    // Poll the device to read the current state
    hr = m_pJoystick->Poll(); 
    if( FAILED(hr) )  
    {
        // DInput is telling us that the input stream has been
        // interrupted. We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done. We
        // just re-acquire and try again.
        hr = m_pJoystick->Acquire();
        while( hr == DIERR_INPUTLOST )
		{
            hr = m_pJoystick->Acquire();
		}
		return DIERR_INPUTLOST; 
    }

    // Get the input's device state
	if (pjs)
	{
		if(FAILED(hr = m_pJoystick->GetDeviceState( sizeof(DIJOYSTATE), pjs)))
		{
			return hr;
		}
	}
	else if (pjs2)
	{
		if(FAILED( hr = m_pJoystick->GetDeviceState(sizeof(DIJOYSTATE2), pjs2)))
		{
			return hr;
		}
	}
    return S_OK;
}
/////////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////////
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
		diprg.lMin              = pThis->m_nRangeMin; 
        diprg.lMax              = pThis->m_nRangeMax; 
    
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
		pThis->m_nSliderCount++;
    }
    if (pdidoi->guidType == GUID_POV)
    {
		pThis->m_nPOVCount++;
    }
    if (pdidoi->guidType == GUID_Button)
    {
		pThis->m_nButtonCount++;
    }
	if (pdidoi->guidType == GUID_Key)
    {
		pThis->m_nKeyCount++;
	}

    return DIENUM_CONTINUE;
}
