/* GlobalReplace: m_iNumStandardlMacros --> m_iNumStandardMacros */
/* GlobalReplace: m_iNumNormalMacros --> m_iNumStandardMacros */
/* GlobalReplace: m_sPrototProcessMacroCount --> m_sProtorProcessMacroCount */
/* GlobalReplace: m_sProcessMacroCount --> m_sProtorProcessMacroCount */
// ProtorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Protor.h"
#include "ProtorDlg.h"

#include "SecID.h"
#include "WK_String.h"
#include "WK_Profile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProtorDlg dialog

CProtorDlg::CProtorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProtorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProtorDlg)
	m_sProtorProcessMacroCount = _T("");
	m_sNormalProcessMacroCount = _T("");
	m_sProcessMacroCount = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_iNumProtorMacros = 0;
	m_iNumStandardMacros = 0;
	m_processMacros.SetAutoDelete(TRUE);

	ScanProcessMacros();
}

void CProtorDlg::DoDataExchange(CDataExchange* pDX)
{
	m_sProtorProcessMacroCount.Format("%d",m_iNumProtorMacros);
	m_sNormalProcessMacroCount.Format("%d",m_iNumStandardMacros);
	m_sProcessMacroCount.Format("%d",m_processMacros.GetSize());

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProtorDlg)
	DDX_Text(pDX, IDC_TXT_PROTOR_MACRO_COUNT, m_sProtorProcessMacroCount);
	DDX_Text(pDX, IDC_TXT_STANDARD_MACRO_COUNT, m_sNormalProcessMacroCount);
	DDX_Text(pDX, IDC_TXT_MACRO_COUNT, m_sProcessMacroCount);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProtorDlg, CDialog)
	//{{AFX_MSG_MAP(CProtorDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ADD_PROTOR_MACRO, OnAddProtorMacro)
	ON_BN_CLICKED(IDC_ADD_STANDARD_MACROS, OnAddStandardMacros)
	ON_BN_CLICKED(IDC_REMOVE_PROTOR_MACRO, OnRemoveProtorMacro)
	ON_BN_CLICKED(IDC_REMOVE_STANDARD_MACROS, OnRemoveStandardMacros)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProtorDlg message handlers

BOOL CProtorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProtorDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProtorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CProtorDlg::ScanProcessMacros()
{
	// reset existing data
	m_processMacros.DeleteAll();
	m_iNumProtorMacros = 0;
	m_iNumStandardMacros = 0;

	CWK_Profile profile;
	CString sOops = profile.GetString("ProcessMacros", "MaxProzess", "0");
	int iMaxProz = atoi(sOops);	// still no Dword

	// read all macro and fill into StringArray m_processMacros
	for (int i=1;i<=iMaxProz;i++) {
		CWK_String sEntry = profile.GetString("ProcessMacros", i, "");
		if (sEntry.GetLength()) {
			m_processMacros.Add(new CWK_String(sEntry));
			if (sEntry.Contains(PSD_PROTOR_MARK)) {
				m_iNumProtorMacros++;
			} else if (sEntry.Contains(PSD_STANDARD_MARK)) {
				m_iNumStandardMacros++;
			} else {
				// some user defined/named macro
			}
		} else {
			// empty process macro ?
		}
	}

}


void CProtorDlg::WriteProcessMacros()
{
	CWK_Profile profile;
	profile.DeleteSection(SEC_NAME_PROZESS);

	CString sOOPS;
	sOOPS.Format("%d",m_processMacros.GetSize());
	profile.WriteString(SEC_NAME_PROZESS, "MaxProzess", sOOPS);

	for (int i=0;i<m_processMacros.GetSize();i++) {
		CString sKey;
		sKey.Format("%d",i+1);
		profile.WriteString(SEC_NAME_PROZESS, sKey, *(m_processMacros[i]));
	}
}

void CProtorDlg::OnAddProtorMacro() 
{
	int iOldNum = m_processMacros.GetSize();

	if (m_processMacros.AlreadyHas("#Protor#1")==FALSE) {
		AddProcessMacro(
			"#Protor#1",
			"camera",
				1000,	// DWORD dwMainTime, 
				1000,	// DWORD dwRecTime, 
				1000,	// DWORD dwPauseTime,
				1000,	// WORD wNrPict,
				TRUE,	// BOOL bInterval, 
				TRUE,	// BOOL bHold, 
				RESOLUTION_HIGH,	// Resolution resolution,
				COMPRESSION_2		// Compression compression
				);
	}

	if (iOldNum!=m_processMacros.GetSize()) {
		WriteProcessMacros();
		UpdateData(FALSE);
	}
}

void CProtorDlg::OnAddStandardMacros() 
{
	WriteProcessMacros();
	UpdateData(FALSE);
}

void CProtorDlg::OnRemoveProtorMacro() 
{

	int iOldNum = m_processMacros.GetSize();

	for (int i=0;i<m_processMacros.GetSize();i++) {
		if (m_processMacros[i]->Contains(PSD_PROTOR_MARK)) {
			//	NOT YET collect ID and call DeleteActivationByPattern
			DWORD dwID = (DWORD)CWK_Profile::GetHexFromString(*m_processMacros[i], "\\ID", SECID_NO_ID);
			CString sIDText;
			sIDText.Format("%x",dwID);
			DeleteActivationByPattern(sIDText);
		}
	}

	m_iNumProtorMacros -= m_processMacros.RemoveByPattern(PSD_PROTOR_MARK);
	

	if (iOldNum!=m_processMacros.GetSize()) {
		WriteProcessMacros();
		UpdateData(FALSE);
	}
}

void CProtorDlg::OnRemoveStandardMacros() 
{
	WriteProcessMacros();
	UpdateData(FALSE);
}


#if 0
			CString s;
			if (pP->byType==P_RELAY) {
				sType = "relay";
				pP->m_dwMainTime = pP->dwRecTime;
				pP->dwPauseTime = 0;
				pP->byInterval = TRUE;
			}
			else if (pP->byType==P_ISDN)	sType = "isdn";
			else if (pP->byType==P_REBOOT)	sType = "reboot";
			else if (pP->byType==P_AUDIO)	sType = "audio";
			else sType = "camera";						// default

			s.Format( "\\ID%x\\CO%s\\TY%s\\MT%lu\\RT%lu\\PT%lu\\NP%u\\IT%u\\H%u\\B%u\\RS%u\\CP%u", 
						pP->GetID(),pP->GetName(), 
						sType, 
						pP->m_dwMainTime, pP->dwRecTime, pP->dwPauseTime, 
						pP->wNrPict, pP->byInterval, pP->byHold, pP->byBreak,
						pP->GetResolution(), pP->GetCompression() );
			for (int ai=0;ai<P_MAX_ARC;ai++)
			{
				if (pP->wArc[ai] != SECID_NO_SUBID)
				{
					s1.Format("\\L%u=%u", ai+1, pP->wArc[ai] );	
					s += s1;
				}
			}
			if (pP->byType==P_ISDN)
			{
				s1.Format("\\PM%08lx",pP->GetPermission().GetID());
				s += s1;
			}
			WK_WriteProfileStringIndex(SEC_NAME_PROZESS, i+1, s); 
		}	// end of valid pP
#endif

void CProtorDlg::AddProcessMacro(
				const CWK_String & sName,
				const CWK_String & sType,
				DWORD dwMainTime, 
				DWORD dwRecTime, 
				DWORD dwPauseTime,
				WORD wNrPict,
				BOOL bInterval, 
				BOOL bHold, 
				Resolution resolution,
				Compression compression
			)
{
	CSecID newID;
	// find max ID
	for (int i=0;i<m_processMacros.GetSize();i++) {
		DWORD dwID((DWORD)CWK_Profile::GetHexFromString(*m_processMacros[i], "\\ID", 0));
		if (dwID >= newID.GetID() || newID==SECID_NO_ID) {
			newID.Set(dwID+1);
		}
	}

#if 0
	if (pP->byType==P_RELAY) {
				sType = "relay";
				pP->m_dwMainTime = pP->dwRecTime;
				pP->dwPauseTime = 0;
				pP->byInterval = TRUE;
			}
	else if (pP->byType==P_ISDN)	sType = "isdn";
	else if (pP->byType==P_REBOOT)	sType = "reboot";
	else if (pP->byType==P_AUDIO)	sType = "audio";
	else sType = "camera";						// default
#endif	
	

	CWK_String s;
	s.Format( "\\ID%x\\CO%s\\TY%s\\MT%lu\\RT%lu\\PT%lu\\NP%u\\IT%u\\H%u\\B%u\\RS%u\\CP%u", 
				newID.GetID(),
				sName,
				sType, 
				dwMainTime, dwRecTime, dwPauseTime, 
				wNrPict, 
				bInterval, bHold, 
				0,	// break is not used
				resolution, compression
				);

	DWORD dwSmallestArchiveNr = 1;
	int ai=1;
	CString sArchive;
	sArchive.Format("\\L%u=%u", ai, dwSmallestArchiveNr );	
	s += sArchive;

	m_processMacros.Add(new CWK_String(s));
	if (sName.Contains(PSD_PROTOR_MARK)) {
		m_iNumProtorMacros++;
	} else if (sName.Contains(PSD_STANDARD_MARK)) {
		m_iNumStandardMacros++;
	} else {
	}
}


// InputGrpoups/MaxGroups
// "1" : "\ID....."
// InputGroups/Group1001/InputsToOutputs
// CAVEAT SIDUnit has no GroupXXX
void CProtorDlg::DeleteActivationByPattern(const CWK_String &sPattern)
{
	CWK_Profile profile;
	int iNumInputGroups = profile.GetInt("InputGroups","MaxGroups",0);
	for (int g=0;g<iNumInputGroups;g++) {
		CWK_String sData = profile.GetString("InputGroups", g+1, "");
		CString sShmName = CWK_Profile::GetStringFromString(sData, "\\SM", "");	
		WORD wGroupID  = (WORD)CWK_Profile::GetHexFromString(sData, "\\ID", SECID_NO_GROUPID);
		int iNumInputs=	CWK_Profile::GetNrFromString(sData, "\\NR", 0);
		CWK_String sSectionName;
		sSectionName.Format("InputGroups\\Group%x",wGroupID);

		for (int i=0;i<iNumInputs;i++) {
			// Activations
			BOOL bActivationFound=TRUE;
			CProtorStringArray activations;
			CWK_String sOneActivation;
			for (int a=0; bActivationFound==TRUE; a++) {
				bActivationFound = ReadIo(sSectionName, i, a,sOneActivation);
				if (bActivationFound) {
					activations.Add(new CWK_String(sOneActivation));
				}
			}
			if (activations.GetSize()) {
				TRACE("%s[%d],found %d activations\n",
					(const char *)sSectionName,i,
					activations.GetSize());
				int iOldNum = activations.GetSize();
				// remove by pattern
				activations.RemoveByPattern(sPattern);
				if (iOldNum!=activations.GetSize()) {
					// write changes
					TRACE("Write changes\n");
					CWK_Profile profile;
					profile.DeleteSubSection(sSectionName,"InputsToOutputs");
					for (int n=0;n<activations.GetSize();n++) {
						CString sActivationFolder;
						sActivationFolder.Format("%s\\InputsToOutputs",(const char*)sSectionName);

						CString sActivationEntry;
						sActivationEntry.Format("%d %d",i,n);

						CWK_Profile profile;
						profile.WriteString(sActivationFolder, sActivationEntry, *activations[n]);
					}	// end of loop over changed activations
				}
			}
			activations.DeleteAll();
		}	// end of loop over inputs
	}	// end of loop over groups
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProtorDlg::ReadIo(const CString &sSectionName, 
						int inputNr, int ioNr,
						CWK_String &sOneActivation)
{
	sOneActivation.Empty();
	// Activations
	CString sActivationFolder;
	sActivationFolder.Format("%s\\InputsToOutputs",(const char*)sSectionName);

	CString sActivationEntry, sActivationValue;
	sActivationEntry.Format("%d %d",inputNr,ioNr);

	CWK_Profile profile;
	CWK_String sData = profile.GetString(sActivationFolder, sActivationEntry, "");
	if (sData.GetLength()) {
		// keep on reading
		sOneActivation = sData;
		return TRUE;
	} else {
		// nothing found
		return FALSE;
	}
}


