// CommPort.cpp: Implementierung der Klasse CCommPort.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "CommPort.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCommPort::CCommPort()
{
   m_hPort = INVALID_HANDLE_VALUE;
   ZeroMemory(&m_OverLapped, sizeof(OVERLAPPED));
   ZeroMemory(&m_CommProp, sizeof(COMMPROP));
   ZeroMemory(&m_CommConfig, sizeof(COMMCONFIG));
   m_CommConfig.dwSize  = sizeof(COMMCONFIG);
   m_strPort[0] = 0;
   m_nTimeOutRead = 500;
   m_nTimeOutWrite = 500;
}

CCommPort::~CCommPort()
{
   ClosePort();
}

bool CCommPort::OpenPort(CString &strPortAdress)
{
   try
   {
      DWORD dwFileAttributes = FILE_FLAG_OVERLAPPED;
      DWORD dwSize = sizeof(COMMCONFIG);
      DWORD dwMask = 0;
      bool bNew = (strncmp(m_strPort, strPortAdress, ADDRESS_SIZE) != 0) ? true : false;
      if (bNew) ClosePort();

      if (m_hPort != INVALID_HANDLE_VALUE)
      {
         if (!m_OverLapped.hEvent) throw (int)4;
         ::ResetEvent(m_OverLapped.hEvent);
		   ::PurgeComm(m_hPort, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
         return true;
      }

      m_hPort = CreateFile(strPortAdress, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
                           NULL, OPEN_EXISTING, dwFileAttributes, NULL);

      if (m_hPort == INVALID_HANDLE_VALUE) throw (int)1;
      if (bNew)
      {
         if (!GetCommProperties(m_hPort, &m_CommProp)) throw (int)2;
         if (!(m_CommProp.dwSettableBaud & BAUD_9600) || 
             !(m_CommProp.wSettableStopParity & (PARITY_NONE|STOPBITS_20)))
             throw (int)6;
      }
      if (!GetCommConfig(m_hPort, &m_CommConfig, &dwSize)) throw (int)3;
      if ((m_CommConfig.dcb.BaudRate    != CBR_9600          ) ||
          (m_CommConfig.dcb.ByteSize    != 8                 ) ||
          (m_CommConfig.dcb.Parity      != 0                 ) ||
          (m_CommConfig.dcb.StopBits    != 2                 ) ||
          (m_CommConfig.dcb.fRtsControl != RTS_CONTROL_ENABLE))
      {
         m_CommConfig.dcb.BaudRate    = CBR_9600;
         m_CommConfig.dcb.ByteSize    = 8;
         m_CommConfig.dcb.Parity      = 0;
         m_CommConfig.dcb.StopBits    = 2;
         m_CommConfig.dcb.fRtsControl = RTS_CONTROL_ENABLE;
         if (!SetCommState(m_hPort, &m_CommConfig.dcb)) throw (int)8;
      }
      m_OverLapped.hEvent = CreateEvent(NULL, true, false, NULL);
      if (!m_OverLapped.hEvent) throw (int)4;
      strncpy(m_strPort, strPortAdress, ADDRESS_SIZE);
      return true;
   }
   catch (int nError)
   {
      CString str;
      char text[MAX_PATH] = "";
      GetLastErrorText(text, MAX_PATH);
      str.Format(IDS_OPEN_PORT_ERROR, strPortAdress, nError, text);
      AfxMessageBox(str, MB_OK|MB_ICONEXCLAMATION);
   }
   return false;
}

void CCommPort::ClosePort()
{
   if (m_OverLapped.hEvent)
   {
      CloseHandle(m_OverLapped.hEvent);
      m_OverLapped.hEvent = NULL;
   }
   if (m_hPort != INVALID_HANDLE_VALUE)
   {
      CloseHandle(m_hPort);
      m_hPort = INVALID_HANDLE_VALUE;
   }
}

bool CCommPort::WritePort(void *pData, DWORD dwSize)
{
   if (m_hPort != INVALID_HANDLE_VALUE)
   {
      DWORD dwResult;
      ::ResetEvent(m_OverLapped.hEvent);
      m_OverLapped.Offset     = 0;
      m_OverLapped.OffsetHigh = 0;
      WriteFile(m_hPort, pData, dwSize, &dwResult, &m_OverLapped);
      dwResult = ::WaitForSingleObject(m_OverLapped.hEvent, m_nTimeOutWrite);
      switch(dwResult)
      {
         case WAIT_TIMEOUT:
         case WAIT_FAILED:   return false;
         case WAIT_ABANDONED:
         case WAIT_OBJECT_0:
            return (dwSize==m_OverLapped.InternalHigh) ? true :  false;
      }
   }
   return false;
}

bool CCommPort::ReadPort(void *pData, DWORD dwSize)
{
   if (m_hPort != INVALID_HANDLE_VALUE)
   {
      DWORD dwResult;
      ::ResetEvent(m_OverLapped.hEvent);
      m_OverLapped.Offset     = 0;
      m_OverLapped.OffsetHigh = 0;
      ::ReadFile(m_hPort, pData, dwSize, &dwResult, &m_OverLapped);
      dwResult = ::WaitForSingleObject(m_OverLapped.hEvent, m_nTimeOutRead);
      switch(dwResult)
      {
         case WAIT_TIMEOUT:
         case WAIT_FAILED:   return false;
         case WAIT_ABANDONED:
         case WAIT_OBJECT_0:
            return (dwSize==m_OverLapped.InternalHigh) ? true :  false;
      }
   }
   return false;
}

bool CCommPort::GetLastErrorText(char *pszText, int nlen)
{
   DWORD dwError = ::GetLastError();
   if (dwError)
   {
#ifdef AFX_TARG_DEU
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwError, MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), pszText, nlen,  NULL);
#else
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwError, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), pszText, nlen,  NULL);
#endif
      return true;
   }
   return false;
}

void CCommPort::ConfigDlg(HWND hWnd, LPCTSTR strPortAdress)
{
   CommConfigDialog(strPortAdress, hWnd, &m_CommConfig);
}
