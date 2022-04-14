// CommPort.h: Schnittstelle für die Klasse CCommPort.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMPORT_H__B0A5DFC2_E877_11D4_87C5_0000B48B0FC3__INCLUDED_)
#define AFX_COMMPORT_H__B0A5DFC2_E877_11D4_87C5_0000B48B0FC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define  ADDRESS_SIZE 32
class CCommPort  
{
public:
	void ConfigDlg(HWND, LPCTSTR);
	CCommPort();
	virtual ~CCommPort();

   bool OpenPort(CString &);
	bool ReadPort(void*, DWORD);
	bool WritePort(void *, DWORD);
	void ClosePort();
   void SetTimeOutRead(int nTime) {m_nTimeOutRead = nTime;};

   bool GetLastErrorText(char *, int nlen);
private:
   HANDLE      m_hPort;
   OVERLAPPED  m_OverLapped;
   COMMPROP    m_CommProp;
   COMMCONFIG  m_CommConfig;
   char        m_strPort[ADDRESS_SIZE];
   int         m_nTimeOutRead;
   int         m_nTimeOutWrite;
};

#endif // !defined(AFX_COMMPORT_H__B0A5DFC2_E877_11D4_87C5_0000B48B0FC3__INCLUDED_)
