// DlgSelectBackupDrive.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "DlgSelectBackupDrive.h"
#include "DlgBackup.h"
#include "Server.h"
#include ".\dlgselectbackupdrive.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
CDlgSelectBackupDrive::CDlgSelectBackupDrive(CDlgBackup* pParent /*=NULL*/)
	: CSkinDialog(CDlgSelectBackupDrive::IDD, pParent)
{
	m_nInitToolTips = FALSE;
	if (UseOEMSkin())
	{
		CreateTransparent(StyleBackGroundColorChangeSimple, m_BaseColor);
		// MoveOnClientAreaClick(false);
	}
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CDlgSelectBackupDrive)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
/////////////////////////////////////////////////////////////////////////////
void CDlgSelectBackupDrive::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelectBackupDrive)
	DDX_Control(pDX, IDC_LISTE, m_ctrlDrives);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgSelectBackupDrive, CSkinDialog)
	//{{AFX_MSG_MAP(CDlgSelectBackupDrive)
	ON_NOTIFY(NM_CLICK, IDC_LISTE, OnNMClickListe)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgSelectBackupDrive::OnInitDialog() 
{
	CSkinDialog::OnInitDialog();
	AutoCreateSkinStatic();
	AutoCreateSkinButtons();
	AutoCreateSkinListCtrl();
	
	InitDrives();
	InsertDrives();
	CWnd* pWnd = GetDlgItem(IDOK);
	if (pWnd)
	{
		pWnd->EnableWindow(FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CDlgSelectBackupDrive::InitDrives()
{
	m_Images.Create(IDB_DRIVES,16,1,RGB(255,0,255));
	m_ctrlDrives.SetImageList(&m_Images,LVSIL_SMALL);

	CRect			rect;
	LV_COLUMN		lvcolumn;
	int w[] = {10,30,30,30};	// CAVET add sum has to be 100
	int i,c;
#ifdef _DEBUG
	int nCount=0;
#endif
	c = 4;
	CString s[4];

	m_ctrlDrives.GetWindowRect(&rect);
	s[0].LoadString(IDS_DRIVE);
	s[1].LoadString(IDS_TYP);
	s[2].LoadString(IDS_USED);
	s[3].LoadString(IDS_SIZE);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	for (i = 0; i < c; i++)  // add the columns to the list control
	{
#ifdef _DEBUG
		nCount += w[i];
#endif
		lvcolumn.cx = (rect.Width() * w[i])/100;
		lvcolumn.pszText = s[i].GetBuffer(0);
		lvcolumn.iSubItem = i;
		m_ctrlDrives.InsertColumn(i, &lvcolumn);  // assumes return value is OK.
		s[i].ReleaseBuffer();
	}

#ifdef _DEBUG
	ASSERT(nCount == 100);
#endif
	
	LONG dw = m_ctrlDrives.GetExtendedStyle();
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_ctrlDrives.SetExtendedStyle(dw);

}
/////////////////////////////////////////////////////////////////////////////
void CDlgSelectBackupDrive::InsertDrives()
{
	// Insert drives dependent on the software/product version
	CWK_Dongle dongle;
	CServer* pServer = m_pParent->GetServer();

	CString sPath;
	GetModuleFileName(AfxGetApp()->m_hInstance,sPath.GetBuffer(_MAX_PATH),_MAX_PATH);
	sPath.ReleaseBuffer();
	m_ctrlDrives.DeleteAllItems();

	if(pServer)
	{
		BOOL bIsLocal = pServer->IsLocal();
			
		CIPCDrives& drives = pServer->GetDrives();
		drives.Lock(_T(__FUNCTION__));
		CIPCDrive* pDrive = NULL;
		for (int i=0;i<drives.GetSize();i++)
		{
			pDrive = drives.GetAtFast(i);
			if (  !pDrive->IsDatabase() 
				&& pDrive->IsEnabled())
			{
				if(bIsLocal && !pDrive->IsSystem())
				{
					// backup on local server
					// nicht das Laufwerk von dem wir gestartet wurden.
					if (0!=sPath.Find(pDrive->GetRootString()))
					{
						InsertDrive(pDrive);
					}
				}
				else
				{
					//backup on remote server
					if (   !pDrive->IsSystem() 
						&& !pDrive->IsCDROM()
						&& !pDrive->IsCDR()
						&& !pDrive->IsCDRW()
						&& !pDrive->IsDVDR()
						&& !pDrive->IsDVDROM()
						&& !pDrive->IsDVDRW()
						)
					{
						InsertDrive(pDrive);
					}
				}
			}
		}
		drives.Unlock();
	}

}
/////////////////////////////////////////////////////////////////////////////
CString CDlgSelectBackupDrive::GetTypString(UINT dt)
{
	CString ret;

	//GetDriveType
	switch (dt)
	{
	case DRIVE_UNKNOWN:
		// The drive type cannot be determined. 
		break;
	case DRIVE_NO_ROOT_DIR:
		// The root directory does not exist. 
		break;
	case DRIVE_REMOVABLE:
		// The disk can be removed from the drive. 
		ret.LoadString(IDS_DRIVE_FD);
		break;
	case DRIVE_FIXED:
		// The disk cannot be removed from the drive. 
		ret.LoadString(IDS_DRIVE_HD);
		break;
	case DRIVE_REMOTE:
		// The drive is a remote (network) drive. 
		ret.LoadString(IDS_DRIVE_NET);
		break;
	case DRIVE_CDROM:
		// The drive is a CD-ROM drive. 
		ret.LoadString(IDS_DRIVE_CD);
		break;
	case DRIVE_RAMDISK:
		// The drive is a RAM disk
		break;
	}
	return ret;
}
/////////////////////////////////////////////////////////////////////////////
int	CDlgSelectBackupDrive::GetTypImage(UINT dt)
{
	//GetDriveType
	switch (dt)
	{
	case DRIVE_UNKNOWN:
		// The drive type cannot be determined. 
		return 0;
	case DRIVE_NO_ROOT_DIR:
		// The root directory does not exist. 
		return 0;
	case DRIVE_REMOVABLE:
		// The disk can be removed from the drive. 
		return 0;
	case DRIVE_FIXED:
		// The disk cannot be removed from the drive. 
		return 1;
	case DRIVE_REMOTE:
		// The drive is a remote (network) drive. 
		return 2;
	case DRIVE_CDROM:
		// The drive is a CD-ROM drive. 
		return 3;
	case DRIVE_RAMDISK:
		// The drive is a RAM disk
		return 0;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
int CDlgSelectBackupDrive::InsertDrive(CIPCDrive* pDrive)
{
	LV_ITEM lvis;
	CString sRoot,sTyp,sUsedAs,sUsed,sMB;
	int i,c,iImage = 0;

	sRoot = pDrive->GetRootString();
	iImage = GetTypImage(pDrive->GetType());
	sTyp = GetTypString(pDrive->GetType());
	sUsed.Format(_T("%d"),pDrive->GetUsedMB());
	sMB.Format(_T("%d"),pDrive->GetMB());

	c = m_ctrlDrives.GetItemCount();

	lvis.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lvis.pszText = sRoot.GetBuffer(0);
	lvis.cchTextMax = sRoot.GetLength();
	lvis.lParam = (LPARAM)pDrive;
	lvis.iItem = c;
	lvis.iSubItem = 0;
	lvis.iImage = iImage;

	i = m_ctrlDrives.InsertItem(&lvis);
	sRoot.ReleaseBuffer();

	m_ctrlDrives.SetItemText(i,1,sTyp);
	m_ctrlDrives.SetItemText(i,2,sUsed);
	m_ctrlDrives.SetItemText(i,3,sMB);

	return i;
}
/////////////////////////////////////////////////////////////////////////////
CString CDlgSelectBackupDrive::GetRootString()
{
	return m_sRoot;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgSelectBackupDrive::OnOK() 
{
	CIPCDrive* pDrive = NULL;
	for (int i=0; i<m_ctrlDrives.GetItemCount(); i++)
	{
		if (m_ctrlDrives.GetItemState(i, LVIS_SELECTED)==LVIS_SELECTED)
		{
			pDrive = (CIPCDrive*)m_ctrlDrives.GetItemData(i);
			if (pDrive)
			{
				m_sRoot = pDrive->GetRootString();
				break;
			}
		}
	}
	CSkinDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
void CDlgSelectBackupDrive::OnNMClickListe(NMHDR *pNMHDR, LRESULT *pResult)
{
	CWnd* pWnd = GetDlgItem(IDOK);
	if (pWnd && m_ctrlDrives.GetSelectedCount() > 0)
	{
		pWnd->EnableWindow(TRUE);
	}
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
