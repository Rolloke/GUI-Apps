// CIPCInputSV.cpp: implementation of the CIPCInputSV class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "systemverwaltung.h"
#include "CIPCInputSV.h"
#include "oemgui\oemgui.h"
#include "oemgui\DlgLoginNew.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCInputSV::CIPCInputSV(const CString& sCommand
	, BOOL bReceiving
	, LPCTSTR shmName
#ifdef _UNICODE
	, WORD wCodePage
#endif
)
	: CIPCInput(shmName,FALSE)
{
	//m_sRegistry;
	//m_sCoCoISAini;
#ifdef _UNICODE
	m_wCodePage = wCodePage;
#endif
	m_sCommand = sCommand;
	m_bReceiving = bReceiving;

	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCInputSV::~CIPCInputSV()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCInputSV::OnReadChannelFound()
{
	TRACE(_T("OnReadChannelFound requesting connection\n"));
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////
void CIPCInputSV::OnConfirmConnection()
{
	if (m_wCodePage != CODEPAGE_UNICODE)			// request only, if unicode is not used
	{
		WORD wCodePage = m_wCodePage;
		m_wCodePage = CODEPAGE_UNICODE;				// request is made in unicode
		DoRequestGetValue(CSecID(), CRF_CODEPAGE);	// Do you speak Unicode?
		m_wCodePage = wCodePage;
	}
	if (m_bReceiving)
	{
		WK_TRACE(_T("OnConfirmConnection requesting cocoisa.ini\n"));
		DoRequestGetFile(RFU_WINDOWS,_T("cocoisa.ini"));
		WK_TRACE(_T("OnConfirmConnection requesting registry export\n"));
		DoRequestGetFile(RFU_STRING_INTERFACE,_T("ExportRegistry nop"));
	}
	else
	{
		WK_TRACE(_T("OnConfirmConnection sending new registry\n"));

		CFile file;
		CFileException cfe;
		BYTE* pBuffer = NULL;

		if (!m_sCoCoISAini.IsEmpty())
		{
			WritePrivateProfileString(NULL,NULL,NULL,m_sCoCoISAini);
			Sleep(50);

			if (file.Open(m_sCoCoISAini,CFile::modeRead|CFile::shareDenyNone,&cfe))
			{
				DWORD dwLen = (DWORD)file.GetLength();
				pBuffer = new BYTE[dwLen];
				if (dwLen == file.Read(pBuffer,dwLen))
				{
					DoRequestFileUpdate(RFU_WINDOWS,
						_T("cocoisa.ini"),pBuffer,dwLen,FALSE);
					WK_TRACE(_T("new cocoisa send %s,%d\n"),m_sCoCoISAini,dwLen);
				}
				else
				{
					WK_TRACE_ERROR(_T("cannot read cocoisa\n"));
				}
				WK_DELETE_ARRAY(pBuffer);
				file.Close();
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot open cocoisa\n"));
			}
		}

		if (file.Open(m_sRegistry,CFile::modeRead|CFile::shareDenyNone,&cfe))
		{
			DWORD dwLen = (DWORD)file.GetLength();
			pBuffer = new BYTE[dwLen];
			if (dwLen == file.Read(pBuffer,dwLen))
			{
				DoRequestFileUpdate(RFU_STRING_INTERFACE,
					m_sCommand,pBuffer,dwLen,FALSE);
				WK_TRACE(_T("new registry send %s %d Bytes\n"),m_sRegistry,dwLen);
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot read send registry\n"));
			}
			WK_DELETE_ARRAY(pBuffer);
			file.Close();
		}
		else
		{
			WK_TRACE_ERROR(_T("cannot open send registry\n"));
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCInputSV::OnRequestDisconnect()
{
}
//////////////////////////////////////////////////////////////////////
void CIPCInputSV::OnConfirmGetFile(int iDestination,
								 const CString &sFileName,
								 const void *pData,
								 DWORD dwDataLen, DWORD dwCodePage)
{
	WK_TRACE(_T("OnConfirmGetFile <%s>\n"),sFileName);
	if (iDestination == CFU_FILE)
	{
		CString sName(sFileName);
		sName.MakeLower();
		if (-1!=sName.Find(_T("cocoisa.ini")))
		{
			WK_TRACE(_T("OnConfirmGetFile cocoisa.ini\n"));

			m_sCoCoISAini = WK_GetTempFile(_T("ini"));

			CFile file;

			if (file.Open(m_sCoCoISAini,CFile::modeCreate|CFile::modeWrite))
			{
				TRY
				{
					file.Write(pData,dwDataLen);
					file.Flush();
					WK_TRACE(_T("temporary cocoisa is %s, %d Bytes\n"),
									m_sCoCoISAini,file.GetLength());
					file.Close();
				}
				CATCH(CFileException, e)
				{
					WK_TRACE_ERROR(_T("Cannot save cocoisa.ini cause %i\n"), e->m_cause);
				}
				END_CATCH
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot open temp file for cocoisa.ini %s\n"), m_sCoCoISAini);
			}
		}
	}
	else if (iDestination == CFU_EXPORT_REGISTRY)
	{
		WK_TRACE(_T("OnConfirmGetFile ExportRegistry()\n"));

		m_sRegistry = WK_GetTempFile(_T("svs"));
		if (dwCodePage)
		{
			if (HIWORD(dwCodePage) & 0x0001)
			{
				m_nRemoteCodePage = MAKELONG(CODEPAGE_UNICODE, 0);
			}
			else
			{
				m_nRemoteCodePage = MAKELONG(LOWORD(dwCodePage), 0);
			}

			WK_TRACE(_T("Remote CodePage:%d, using unicode:%d\n"), LOWORD(dwCodePage), m_nRemoteCodePage == CODEPAGE_UNICODE ? 1 : 0);

			CStringArray ar;
			CoInitialize(NULL);
			if (GetCodePageInfo(LOWORD(dwCodePage), ar))
			{
				CString str = ar.GetAt(OEM_CP_GDICHARSET);
				long lGdiCharSet = DEFAULT_CHARSET;
				if (_stscanf(str, _T("%x"), &lGdiCharSet) == 1 && lGdiCharSet)
				{
					BOOL bUseGF = CSkinDialog::UseGlobalFonts();
					if (bUseGF)
					{
						m_nRemoteCodePage |= INPUT_CPF_USES_GLOBAL_FONTS;
					}
					else
					{
						if (COemGuiApi::GetKeyboardInput(0).IsValid() == NULL)
						{
							m_nRemoteCodePage |= INPUT_CPF_KB_LAYOUT_CREATED;
						}
						int nFind = sFileName.Find(_T(":"));
						if (nFind != -1)
						{
							CString sLangAbb = sFileName.Mid(nFind+1);
							CString sLang1 = COemGuiApi::GetLocaleID();
							CString sLang2 = CLoadResourceDll::LangIDfromLangAbb(sLangAbb);
							if (sLang1 == sLang2)
							{
								sLang1 = KBLANGID_ENU;
							}
							theApp.CreateKeyboardLayouts(sLang1, sLang2);
							m_nRemoteCodePage |= INPUT_CPF_KB_LAYOUT_CHANGED;
						}
					}

					CSkinDialog::DoUseGlobalFonts(TRUE);
					if (SetFontFaceNames(ar.GetAt(OEM_CP_FIXEDWIDTHFONT), ar.GetAt(OEM_CP_PROPORTIONALFONT)))
					{
						WK_TRACE(_T("Using GDI CharSet(%d): FixedFont(%s), VariableFont(%s)\n"), lGdiCharSet, ar.GetAt(OEM_CP_FIXEDWIDTHFONT), ar.GetAt(OEM_CP_PROPORTIONALFONT));
						m_nRemoteCodePage |= INPUT_CPF_GLOBAL_FONTS_CHANGED;
					}
					else
					{
						CSkinDialog::DoUseGlobalFonts(bUseGF);
					}
				}
			}
			CoUninitialize();
		}
		else
		{
#ifdef _UNICODE
			if (m_wCodePage != 0 && m_wCodePage != CODEPAGE_UNICODE)
			{
				m_nRemoteCodePage = MAKELONG(m_wCodePage, 0);
				WK_TRACE(_T("CodePage:%d\n"), m_wCodePage);
			}
			else
			{
				m_nRemoteCodePage = GetACP();	// use this codepage
				WK_TRACE(_T("Own CodePage:%d\n"), m_nRemoteCodePage);
			}
			m_nRemoteCodePage |= INPUT_CPF_USE_STRING_FORMAT;
#endif
		}
		
		CFile file;

		if (file.Open(m_sRegistry,CFile::modeCreate|CFile::modeWrite))
		{
			TRY
			{
				file.Write(pData,dwDataLen);
				file.Flush();
				WK_TRACE(_T("temporary registry is %s, %d Bytes\n"), m_sRegistry,file.GetLength());
				file.Close();
			}
			CATCH(CFileException, e)
			{
				WK_TRACE_ERROR(_T("Cannot save registry %s cause %i\n"), m_sRegistry, e->m_cause);
			}
			END_CATCH
		}
		else
		{
			WK_TRACE_ERROR(_T("cannot open temp file for save registry %s\n"), m_sRegistry);
		}

		m_eventRegistry.SetEvent();
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCInputSV::OnConfirmFileUpdate(const CString &sFileName)
{
	if (0==sFileName.Find(_T("ImportRegistry")))
	{
		TRACE(_T("OnConfirmFileUpdate %s\n"),sFileName);
		m_eventRegistry.SetEvent();
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCInputSV::OnConfirmHardware(WORD wGroupID,int iErrorCode)
{
	// dummy only
//	TRACE(_T("OnConfirmHardware\n"));
}
//////////////////////////////////////////////////////////////////////
void CIPCInputSV::OnConfirmInfoInputs(WORD wGroupID, int iNumGroups, 
								int iNumRecords, const CIPCInputRecord records[])
{
	// dummy only
//	TRACE(_T("OnConfirmInfoInputs\n"));
}
//////////////////////////////////////////////////////////////////////
void CIPCInputSV::OnConfirmAlarmState(WORD wGroupID,DWORD inputMask)
{
	// dummy only
//	TRACE(_T("OnConfirmAlarmState\n"));
}
//////////////////////////////////////////////////////////////////////
void CIPCInputSV::OnIndicateAlarmNr(CSecID id,
									BOOL bAlarm,
									DWORD dwData1,
									DWORD dwData2)
{
	// dummy only
//	TRACE(_T("OnIndicateAlarmNr\n"));
}
