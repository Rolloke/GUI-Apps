/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: cioport.cpp $
// ARCHIVE:		$Archive: /Project/Tools/HealthControl/HealthControl/cioport.cpp $
// CHECKIN:		$Date: 23.01.06 9:51 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 23.01.06 8:52 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CIOPort.h"
#if WKCLASSES == 1
 #include "D:/Privat/PROJECT/Units/SaVic2/SaVicDA/SavicDirectAccess.h"
#endif
int CIoPort::m_nInstanceCounter			= 0;
HINSTANCE		CIoPort::m_hDADll		= NULL;
DAREADPORT8		CIoPort::m_DAReadPort8	= (DAREADPORT8)NULL;
DAREADPORT16	CIoPort::m_DAReadPort16	= (DAREADPORT16)NULL;
DAREADPORT32	CIoPort::m_DAReadPort32	= (DAREADPORT32)NULL;

DAWRITEPORT8	CIoPort::m_DAWritePort8	= (DAWRITEPORT8)NULL;
DAWRITEPORT16	CIoPort::m_DAWritePort16= (DAWRITEPORT16)NULL;
DAWRITEPORT32	CIoPort::m_DAWritePort32= (DAWRITEPORT32)NULL;
DAISVALID		CIoPort::m_DAIsValid	= (DAISVALID)NULL;
BOOL			CIoPort::m_bInitOK		= FALSE;

#ifdef _DEBUG
 #ifdef _UNICODE
  static TCHAR BASED_CODE szJaCobDADll[]	= _T("JaCobDADU.Dll");
  static TCHAR BASED_CODE szSaVicDADll[]	= _T("SaVicDADU.Dll");
  static TCHAR BASED_CODE szTashaDADll[]	= _T("TashaDADU.Dll");
#else // _UNICODE
  static TCHAR BASED_CODE szJaCobDADll[]	= _T("JaCobDAD.Dll");
  static TCHAR BASED_CODE szSaVicDADll[]	= _T("SaVicDAD.Dll");
  static TCHAR BASED_CODE szTashaDADll[]	= _T("TashaDAD.Dll");
 #endif // _UNICODE
#else // _DEBUG 
 #ifdef _UNICODE
  static TCHAR BASED_CODE szJaCobDADll[]	= _T("JaCobDAU.Dll");
  static TCHAR BASED_CODE szSaVicDADll[]	= _T("SaVicDAU.Dll");
  static TCHAR BASED_CODE szTashaDADll[]	= _T("TashaDAU.Dll");
 #else // _UNICODE
  static TCHAR BASED_CODE szJaCobDADll[]	= _T("JaCobDA.Dll");
  static TCHAR BASED_CODE szSaVicDADll[]	= _T("SaVicDA.Dll");
  static TCHAR BASED_CODE szTashaDADll[]	= _T("TashaDA.Dll");
 #endif // _UNICODE
#endif // _DEBUG

///////////////////////////////////////////////////////////////////////////////////////////////////////////
CIoPort::CIoPort(DWORD dwIOBase)
{	
	CStringArray arError;
	if (m_nInstanceCounter == 0)
	{
		m_bInitOK = FALSE;
		
		// Versuche die JaCobDA.dll für die Hardwarezugriffe zu verwenden.
		m_hDADll = LoadLibrary(_T("hwdll32.dll"));
		if (m_hDADll)
		{
			m_DAReadPort8	= (DAREADPORT8)GetProcAddress(m_hDADll, "_hw_IoInByte");
			m_DAReadPort16	= (DAREADPORT16)GetProcAddress(m_hDADll, "_hw_IoInWord");
			m_DAReadPort32	= (DAREADPORT32)GetProcAddress(m_hDADll, "_hw_IoInDWord");
			m_DAWritePort8	= (DAWRITEPORT8)GetProcAddress(m_hDADll,"_hw_IoOutByte");
			m_DAWritePort16	= (DAWRITEPORT16)GetProcAddress(m_hDADll,"_hw_IoOutWord");
			m_DAWritePort32	= (DAWRITEPORT32)GetProcAddress(m_hDADll,"_hw_IoOutDWord");
			m_DAIsValid		= (DAISVALID)GetProcAddress(m_hDADll,"hw_PciFindDevice");
			if (m_DAReadPort8 && m_DAReadPort16 && m_DAReadPort32 && m_DAWritePort8 && m_DAWritePort16 && m_DAWritePort32 && m_DAIsValid)
			{
				m_bInitOK = m_DAIsValid();
				if (m_bInitOK)
					WK_TRACE(_T("Loading hwdll32.dll successful!\n"));
				else
					arError.Add(_T("Can't load hwdll32.dll!\n"));
         }
      }			

		m_hDADll = LoadLibrary(szJaCobDADll);
		if (m_hDADll)
		{
			m_DAReadPort8	= (DAREADPORT8)GetProcAddress(m_hDADll, "DAReadPort8");
			m_DAReadPort16	= (DAREADPORT16)GetProcAddress(m_hDADll, "DAReadPort16");
			m_DAReadPort32	= (DAREADPORT32)GetProcAddress(m_hDADll, "DAReadPort32");
			m_DAWritePort8	= (DAWRITEPORT8)GetProcAddress(m_hDADll,"DAWritePort8");
			m_DAWritePort16	= (DAWRITEPORT16)GetProcAddress(m_hDADll,"DAWritePort16");
			m_DAWritePort32	= (DAWRITEPORT32)GetProcAddress(m_hDADll,"DAWritePort32");
			m_DAIsValid		= (DAISVALID)GetProcAddress(m_hDADll,"DAIsValid");
			
			if (m_DAReadPort8 && m_DAReadPort16 && m_DAReadPort32 && m_DAWritePort8 && m_DAWritePort16 && m_DAWritePort32 && m_DAIsValid)
			{
				m_bInitOK = m_DAIsValid();
				if (m_bInitOK)
					WK_TRACE(_T("Loading JaCobDA successful!\n"));
				else
					arError.Add(_T("Can't open JaCob.sys!\n"));
			}
#if WKCLASSES == 1
			else
			{
				CString sErr;
				sErr.Format(_T("Port access function in JaCobDA.dll not found. Error <%s>\n"), (LPCTSTR)GetLastErrorString());
				arError.Add(sErr);
			}
#endif
		}
		else
		{
			arError.Add(_T("Loading JaCobDA.dll failed...try to load SaVicDA.dll now.\n"));
		}

		// Versuche die SaVicDA.dll für die Hardwarezugriffe zu verwenden.
		if (!m_bInitOK)
		{
			m_hDADll = LoadLibrary(szSaVicDADll);
			if (m_hDADll)
			{
				m_DAReadPort8	= (DAREADPORT8)GetProcAddress(m_hDADll, "DAReadPort8");
				m_DAReadPort16	= (DAREADPORT16)GetProcAddress(m_hDADll, "DAReadPort16");
				m_DAReadPort32	= (DAREADPORT32)GetProcAddress(m_hDADll, "DAReadPort32");
				m_DAWritePort8	= (DAWRITEPORT8)GetProcAddress(m_hDADll,"DAWritePort8");
				m_DAWritePort16	= (DAWRITEPORT16)GetProcAddress(m_hDADll,"DAWritePort16");
				m_DAWritePort32	= (DAWRITEPORT32)GetProcAddress(m_hDADll,"DAWritePort32");
				m_DAIsValid		= (DAISVALID)GetProcAddress(m_hDADll,"DAIsValid");
				
				if (m_DAReadPort8 && m_DAReadPort16 && m_DAReadPort32 && m_DAWritePort8 && m_DAWritePort16 && m_DAWritePort32 && m_DAIsValid)
				{
					m_bInitOK = m_DAIsValid();

					if (m_bInitOK)
						WK_TRACE(_T("Loading SaVicDA successful!\n"));
					else
						arError.Add(_T("Can't open SaVic.sys!\n"));
				}
#if WKCLASSES == 1
				else
				{
					CString sErr;
					sErr.Format(_T("Port access function in SaVicDA.dll not found. Error <%s>\n"), (LPCTSTR)GetLastErrorString());
					arError.Add(sErr);
				}
#endif
			}
			else
				arError.Add(_T("Loading SaVicDA.dll failed...try to load TashaDA.dll now.\n"));
		}

		// Versuche die TashaDA.dll für die Hardwarezugriffe zu verwenden.
		if (!m_bInitOK)
		{
			m_hDADll = LoadLibrary(szTashaDADll);
			if (m_hDADll)
			{
				m_DAReadPort8	= (DAREADPORT8)GetProcAddress(m_hDADll, "DAReadPort8");
				m_DAReadPort16	= (DAREADPORT16)GetProcAddress(m_hDADll, "DAReadPort16");
				m_DAReadPort32	= (DAREADPORT32)GetProcAddress(m_hDADll, "DAReadPort32");
				m_DAWritePort8	= (DAWRITEPORT8)GetProcAddress(m_hDADll,"DAWritePort8");
				m_DAWritePort16	= (DAWRITEPORT16)GetProcAddress(m_hDADll,"DAWritePort16");
				m_DAWritePort32	= (DAWRITEPORT32)GetProcAddress(m_hDADll,"DAWritePort32");
				m_DAIsValid		= (DAISVALID)GetProcAddress(m_hDADll,"DAIsValid");
				
				if (m_DAReadPort8 && m_DAReadPort16 && m_DAReadPort32 && m_DAWritePort8 && m_DAWritePort16 && m_DAWritePort32 && m_DAIsValid)
				{
					m_bInitOK = m_DAIsValid();
					if (m_bInitOK)
						WK_TRACE(_T("Loading TashaDA successful!\n"));
					else
						arError.Add(_T("Can't open Tasha.sys!\n"));
				}
#if WKCLASSES == 1
				else
				{
					CString sErr;
					sErr.Format(_T("Port access function in TashaDA.dll not found. Error <%s>\n"), (LPCTSTR)GetLastErrorString());
					arError.Add(sErr);
				}
#endif
			}
			else
				arError.Add(_T("Can't access hardware!\n"));
		}
	}
	if (!m_bInitOK)
	{
		int i, n = arError.GetSize();
		for (i=0; i<n; i++)
		{
			WK_TRACE(arError.GetAt(i));
		}
	}

	m_nInstanceCounter++;
	m_dwIOBase = dwIOBase;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CIoPort::IsValid()
{
	return (m_bInitOK);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
CIoPort::~CIoPort()
{
	m_nInstanceCounter--;
	if (m_nInstanceCounter == 0)
	{
		if (m_hDADll)
		{
			if (AfxFreeLibrary(m_hDADll))
				m_hDADll = NULL;
			else
				WK_TRACE_ERROR(_T("xxDADll.dll NOT unloaded\n"));
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
BYTE CIoPort::Input8(DWORD dwPort) const
{
	return m_DAReadPort8(m_dwIOBase + dwPort);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
WORD CIoPort::Input16(DWORD dwPort) const
{
	return m_DAReadPort16(m_dwIOBase + dwPort);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD CIoPort::Input32(DWORD dwPort) const
{
	return m_DAReadPort32(m_dwIOBase + dwPort);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CIoPort::Output8(DWORD dwPort, BYTE byData) const 
{
	m_DAWritePort8(m_dwIOBase + dwPort, byData);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CIoPort::Output16(DWORD dwPort, WORD wData) const 
{
	m_DAWritePort32(m_dwIOBase + dwPort, wData);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CIoPort::Output32(DWORD dwPort, DWORD dwData) const 
{
	m_DAWritePort32(m_dwIOBase + dwPort, dwData);
}

