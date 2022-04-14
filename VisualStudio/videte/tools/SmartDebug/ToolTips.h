#pragma once

#include <string>

/// functor class providing tooltip text callback function
class CToolTipsFunctor
{
    friend class CToolTips;
public:
    CToolTipsFunctor() {};
protected:
    virtual std::string GetToolTipText(UINT aID, HWND aWnd, POINT aScreenPoint) = 0;
};

/// class providing tooltip control with display and hide delay
/// \note the tooltip texts are defined in a string resource with the same ID as the dialog control
class CToolTips
{
    enum eHoverCount
    {
        NoCounting = -1, StartCount = 0, HideOnMouseMove = 1, TimerID = 100, TimeOutMS = 200, MinDelay=5
    };

public:
    CToolTips();
    ~CToolTips();

    bool Create(HWND hOwner, HINSTANCE aInstance);
    void Close();

    void SetMaxWidth(int aWidth);
    void SetToolTipDelays(int aDisplay, int aHide);
    void SetToolTipFunctor(CToolTipsFunctor*aFunctor);
    void ShowToolTip(LPTSTR aText, const POINT&aScreenPoint, UINT aLastId=0);
    void HideToolTip();

private:
    LRESULT OnTimer(UINT aEvent);
    static LRESULT CALLBACK WindowProc(HWND aWnd, UINT aMsg, WPARAM aWparam, LPARAM aLparam);

private:
    HWND     m_hWnd;
    HWND     m_hOwner;
    HINSTANCE m_hInstance;
    UINT_PTR m_WndProc;
    UINT_PTR m_HoverTimer;
    int      m_HoverCounter;
    UINT     m_LastToolTipID;
    DWORD    m_dwError;
    int      m_DisplayTooltip;
    int      m_HideTooltip;
    CToolTipsFunctor*m_Functor;
    POINT    m_HoverPosition;
};
