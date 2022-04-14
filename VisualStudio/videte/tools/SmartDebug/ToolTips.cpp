#include "stdafx.h"

#include "ToolTips.h"


#define lengthof(T) (sizeof(T)/sizeof(T[0]))

CToolTips::CToolTips() : m_hWnd(NULL)
, m_hOwner(NULL)
, m_hInstance(NULL)
, m_HoverTimer(0)
, m_HoverCounter(0)
, m_LastToolTipID(0)
, m_dwError(0)
, m_DisplayTooltip(5) 
, m_HideTooltip(25)
, m_Functor(NULL)
{
    m_HoverPosition.x = 0;
    m_HoverPosition.y = 0;

    INITCOMMONCONTROLSEX icex;
    // Load the ToolTip class from the DLL.
    icex.dwSize = sizeof(icex);
    icex.dwICC = ICC_BAR_CLASSES;

    if (!InitCommonControlsEx(&icex))
    {
        m_dwError = GetLastError();
    }

}

CToolTips::~CToolTips()
{
    Close();
}

/*!
\brief sets a functor callback class to retrieve tooltip text
\param aFunctor pointer to CToolTipsFunctor derived class with callback function implementation
*/
void CToolTips::SetToolTipFunctor(CToolTipsFunctor*aFunctor)
{
    m_Functor = aFunctor;
}

/*!
\brief sets tooltip display delays for display and hide the tooltip
\param aDisplay delay to display the tooltip
\param aDisplay delay to hide the tooltip
*/
void  CToolTips::SetToolTipDelays(int aDisplay, int aHide)
{
    if (aDisplay <= HideOnMouseMove) aDisplay = MinDelay;
    if (aHide < MinDelay) aHide = MinDelay;
    m_DisplayTooltip = aDisplay;
    m_HideTooltip    = aDisplay + aHide;
}

/*!
\brief Creates the tooltip window
\param aOwner owner of the tooltip window
\param aInstance handle to the resource instance
*/
bool CToolTips::Create(HWND aOwner, HINSTANCE aInstance)
{
    if (m_hWnd == NULL)
    {
        m_hInstance = aInstance;
        m_hWnd = CreateWindow(TOOLTIPS_CLASS, TEXT(""), WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                CW_USEDEFAULT, NULL, (HMENU) NULL, aInstance, NULL);
        if (m_hWnd)
        {
            m_hOwner = aOwner;
            SetWindowLongPtr(m_hWnd, GWL_USERDATA, (LONG_PTR)this);
            m_WndProc = GetWindowLongPtr(m_hWnd, GWL_WNDPROC);
            SetWindowLongPtr(m_hWnd, GWL_WNDPROC, (LONG_PTR)WindowProc);
            SendMessage(m_hWnd, TTM_SETMAXTIPWIDTH, 0, 200);
            SetWindowPos(m_hOwner, m_hWnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            m_HoverTimer = SetTimer(m_hWnd, TimerID, TimeOutMS, NULL);
        }
    }
    return false;
}

/*!
\brief destroys the tooltip window
*/
void CToolTips::Close()
{
    if (m_HoverTimer)
    {
        KillTimer(m_hWnd, m_HoverTimer);
        m_HoverTimer = 0;
    }
    if (m_hWnd)
    {
        DestroyWindow(m_hWnd);
        m_hWnd = 0;
    }
}

/*!
\brief sets the maximum width of the tooltip window
\param aWidth maximum width in pixel
*/
void CToolTips::SetMaxWidth(int aWidth)
{
    if (m_hWnd)
    {
        SendMessage(m_hWnd, TTM_SETMAXTIPWIDTH, 0, aWidth);
    }
}

/*!
\brief subclass window pocedure 
\param aWnd window handle
\param aMsg current message
\param aWparam W param of the message
\param aLparam L param of the message
*/
LRESULT CALLBACK CToolTips::WindowProc(HWND aWnd, UINT aMsg, WPARAM aWparam, LPARAM aLparam)
{
    CToolTips*pThis = (CToolTips*)GetWindowLong(aWnd, GWL_USERDATA);
    if (pThis)
    {
        switch(aMsg)
        {
        case WM_TIMER: pThis->OnTimer(aWparam);
        }
        return CallWindowProc((WNDPROC)pThis->m_WndProc, aWnd, aMsg, aWparam, aLparam);
    }
    return 0;
}

/*!
\brief timer to control the state machine of the tooltip display process
\param aEvent timer event ID
*/
LRESULT CToolTips::OnTimer(UINT aEvent)
{
    if (aEvent == m_HoverTimer)
    {
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(m_hOwner, &pt);
        if (m_HoverPosition.x != pt.x || m_HoverPosition.y != pt.y)
        {
            m_HoverPosition = pt;
            m_HoverCounter = StartCount;
        }
        if (m_HoverCounter != NoCounting)
        {
            m_HoverCounter++;
        }
        if (m_HoverCounter == m_DisplayTooltip)
        {
            POINT ptScreen = m_HoverPosition;
            ClientToScreen(m_hOwner, &ptScreen);
            HWND hwnd = ChildWindowFromPointEx(m_hOwner, m_HoverPosition,
                CWP_SKIPINVISIBLE | CWP_SKIPDISABLED | CWP_SKIPTRANSPARENT);
            UINT nID = GetDlgCtrlID(hwnd);
            if (nID != 0 && nID != 0xffffffff)
            {
                TCHAR sFormat[1024];
                std::string sText;
                LPTSTR pStr = NULL;
                if (m_Functor) sText = m_Functor->GetToolTipText(nID, hwnd, ptScreen);
                if (sText.size()) pStr  = (LPTSTR)sText.c_str();
                else
                {
                    if (LoadString(m_hInstance, nID, sFormat, lengthof(sFormat)))
                    {
                        pStr = sFormat;
                    }
                }
                if (pStr)
                {
                    ptScreen.x += 10;
                    ShowToolTip(pStr, ptScreen, (UINT)hwnd);
                }
            }
        }
        else if (m_LastToolTipID 
            && (m_HoverCounter == m_HideTooltip || m_HoverCounter == HideOnMouseMove))
        {
            HideToolTip();
        }
    }
    return 0;
}

/*!
\brief shows a tooltip window with given text at position in screen coordinates
\param aText text to be displayed
\param aScreenPoint left top position of the tooltipwindow in screen coordinates
\param aLastId ID of the last displayed tooltip
\note if this ID is 0 the tooltip is not hidden after the hide delay time
*/
void CToolTips::ShowToolTip(LPTSTR aText, const POINT&aScreenPoint, UINT aLastId)
{
    TOOLINFO ti =
    { sizeof(TOOLINFO), TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE, m_hOwner, (UINT)aLastId,
    { 0, 0, 0, 0 }, m_hInstance, aText };
    m_LastToolTipID = aLastId;
    SendMessage(m_hWnd, TTM_ADDTOOL, 0, (LPARAM) &ti);
    SendMessage(m_hWnd, TTM_TRACKACTIVATE, (WPARAM) TRUE, (LPARAM) & ti);
    SendMessage(m_hWnd, TTM_TRACKPOSITION, (WPARAM) 0, (LPARAM) MAKELONG(aScreenPoint.x, aScreenPoint.y));
}

/*!
\brief hides the tooltip window
*/
void CToolTips::HideToolTip()
{
    TOOLINFO ti =
    { sizeof(TOOLINFO), TTF_IDISHWND, m_hOwner, m_LastToolTipID,
    { 0, 0, 0, 0 }, 0, 0 };
    m_HoverCounter = NoCounting;
    m_LastToolTipID = 0;
    SendMessage(m_hWnd, TTM_TRACKACTIVATE, (WPARAM) 0, (LPARAM) & ti);
    SendMessage(m_hWnd, TTM_DELTOOL, (WPARAM) 0, (LPARAM) & ti);
}