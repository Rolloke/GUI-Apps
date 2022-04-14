#pragma once

// RemoteTextReceiverCtrl.h : Declaration of the CRemoteTextReceiverCtrl ActiveX Control class.

class CRemoteTextReceiverCtrl;
// CRemoteTextReceiverCtrl : See RemoteTextReceiverCtrl.cpp for implementation.
class CReceivingSocket : public CSocket
{
public:
   CReceivingSocket(CRemoteTextReceiverCtrl*pCtrl) { m_pCtrl = pCtrl; }
protected:
   virtual void OnReceive(int nErrorCode);
   virtual void OnAccept(int nErrorCode);

private:
   CRemoteTextReceiverCtrl *m_pCtrl;
};

class CRemoteTextReceiverCtrl : public COleControl
{
	DECLARE_DYNCREATE(CRemoteTextReceiverCtrl)

// Constructor
public:
	CRemoteTextReceiverCtrl();

public:
   void WriteTextToLogfile(LPCTSTR s);
// Overrides
protected:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();

// Implementation
protected:
	~CRemoteTextReceiverCtrl();

	BEGIN_OLEFACTORY(CRemoteTextReceiverCtrl)        // Class factory and guid
		virtual BOOL VerifyUserLicense();
		virtual BOOL GetLicenseKey(DWORD, BSTR FAR*);
	END_OLEFACTORY(CRemoteTextReceiverCtrl)

	DECLARE_OLETYPELIB(CRemoteTextReceiverCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CRemoteTextReceiverCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CRemoteTextReceiverCtrl)		// Type name and misc status

	// Subclassed control support
	BOOL IsSubclassedControl();
	LRESULT OnOcmCommand(WPARAM wParam, LPARAM lParam);

// Message maps
   afx_msg void OnTimer(UINT nIDEvent);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	DECLARE_DISPATCH_MAP()

// Event maps
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
private:
   CReceivingSocket m_Receiver;
   CStdioFile       m_LogFile;
   CString          m_sUrl, m_sLogFile, m_sText;
   ULONG            m_nPort;
};

