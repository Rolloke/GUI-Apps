// RemoteCurveControlCtrl.h : Declaration of the CRemoteCurveControlCtrl ActiveX Control class.
// CRemoteCurveControlCtrl : See RemoteCurveControlCtrl.cpp for implementation.
#pragma once
#include "PlotLabel.h"

class CRemoteCurveControlCtrl;
//class CCriticalSection;
// CRemoteTextReceiverCtrl : See RemoteTextReceiverCtrl.cpp for implementation.
class CReceivingSocket : public CSocket
{
public:
	CReceivingSocket(): 
        mConnected(false), 
        mListening(false)
    {
    }
    void SetControl(CRemoteCurveControlCtrl*pCtrl)
    { m_pCtrl = pCtrl; };

    bool IsConnected() {return mConnected;};
    bool IsListening() {return mListening;};
    void SetListening() {mListening = true;};

protected:
    virtual void OnReceive(int nErrorCode);
    virtual void OnAccept(int nErrorCode);
    virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);

private:
    CRemoteCurveControlCtrl *m_pCtrl;
    bool mConnected;
    bool mListening;
};


class CRemoteCurveControlCtrl : public COleControl
{
	DECLARE_DYNCREATE(CRemoteCurveControlCtrl)

// Constructor
public:
	CRemoteCurveControlCtrl();

// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual DWORD GetControlFlags();
	virtual BOOL OnRenderFileData(LPFORMATETC lpFormatEtc, CFile* pFile);

// Implementation
protected:
	~CRemoteCurveControlCtrl();

	BEGIN_OLEFACTORY(CRemoteCurveControlCtrl)        // Class factory and guid
		virtual BOOL VerifyUserLicense();
		virtual BOOL GetLicenseKey(DWORD, BSTR FAR*);
	END_OLEFACTORY(CRemoteCurveControlCtrl)


	DECLARE_OLETYPELIB(CRemoteCurveControlCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CRemoteCurveControlCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CRemoteCurveControlCtrl)		// Type name and misc status
#ifdef USE_IOBJECTSAFETY
    // IObjectSafety
	BEGIN_INTERFACE_PART(ObjectSafety, IObjectSafety)
		INIT_INTERFACE_PART(CRemoteCurveControlCtrl, ObjectSafety)
        STDMETHOD(GetInterfaceSafetyOptions)(REFIID,DWORD *,DWORD *);
        STDMETHOD(SetInterfaceSafetyOptions)(REFIID,DWORD,DWORD);
	END_INTERFACE_PART_STATIC(ObjectSafety)
#endif
    DECLARE_INTERFACE_MAP()

	// Subclassed control support
	BOOL IsSubclassedControl();

    void        SetMouseCapture();
    void        ReleaseMouseCapture();
// Message maps
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnUpdateContextProperties(CCmdUI *pCmdUI);
	afx_msg void OnContextProperties();
	afx_msg void OnUpdateHelpAbout(CCmdUI *pCmdUI);
	afx_msg void OnHelpAbout();
	afx_msg void OnUpdateContextSaveplotasfile(CCmdUI *pCmdUI);
	afx_msg void OnContextSaveplotasfile();
	afx_msg void OnCurves1st(UINT nID);
	afx_msg void OnUpdateCurves1st(CCmdUI *pCmdUI);
	afx_msg void OnRangesNext();
	afx_msg void OnUpdateRangesNext(CCmdUI *pCmdUI);
	afx_msg void OnRangesPrevious();
	afx_msg void OnUpdateRangesPrevious(CCmdUI *pCmdUI);
	afx_msg void OnRangesDeleteThis();
	afx_msg void OnUpdateRangesDeleteThis(CCmdUI *pCmdUI);
	afx_msg void OnRangesDeleteall();
	afx_msg void OnUpdateRangesDeleteall(CCmdUI *pCmdUI);
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	DECLARE_DISPATCH_MAP()

// Event maps
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:

    // Implementations 
public:
    void OnReceive(CReceivingSocket*pSock, int nErrorcode);
protected:
    BOOL ReadColor(CPropExchange*pPX, const CString& txt, COLORREF &color);
    BOOL ReadGridStep(CPropExchange*pPX, const CString& txt, WORD&wStep);
    void OnPrepareDC(CDC*pDC);
    void CreateReceivingSocket();

private:
    CPlotLabel *mpPlot;
    CLabelContainer*mpPickList;
    CRITICAL_SECTION mcsPlot;
    CReceivingSocket mReceiver;
    CString          msUrl;
    ULONG            mnPort;
    UINT_PTR         muFirstUpdate;
    UINT_PTR         muCheckUpdate;
    BOOL             mbUpdateControl;
    BOOL             mbLbuttonDown;
    CPoint           m_OldMousePoint;
public:
protected:



public:
};

