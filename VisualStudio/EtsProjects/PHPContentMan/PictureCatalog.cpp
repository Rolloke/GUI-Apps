// PictureCatalog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "PHPContentMan.h"
#include "PictureCatalog.h"
#include "PHPContentMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPictureCatalog 
#define SORT_CHANGED 0x00010000

#define SELECT_PICTURELIST 0
#define SELECT_SIZELIST    1
#define SELECT_ARTISTLIST  2
#define SELECT_STYLELIST   3

long CPictureCatalog::gm_lOldEditCtrlWndProc = 0;
bool CPictureCatalog::gm_bSortChanged        = false;
int  CPictureCatalog::gm_nSortDir            = 1;

CPictureCatalog::CPictureCatalog(CWnd* pParent /*=NULL*/)
	: CDialog(CPictureCatalog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPictureCatalog)
	m_strPictNo = _T("");
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
   m_ppPictureList = NULL;
   m_nPictureList  = 0;
   m_ppSizeList    = NULL;
   m_nSizeList     = 0;
   m_ppArtistList  = NULL;
   m_nArtistList   = 0;
   m_ppStyleList   = NULL;
   m_nStyleList    = 0;
   m_bItemChangedPictureList = false;
   m_bKeydownSizeList        = false;
   m_bChanged        = false;
}

CPictureCatalog::~CPictureCatalog()
{
   DeleteLists();
}

void CPictureCatalog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPictureCatalog)
	DDX_Control(pDX, IDC_PC_LIST_PICTURE, m_cPictureList);
	DDX_Control(pDX, IDC_PC_LIST_SIZE, m_cSizeList);
	DDX_Control(pDX, IDC_PC_LIST_ARTIST, m_cArtistList);
	DDX_Control(pDX, IDC_PC_LIST_STYLE, m_cStyleList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPictureCatalog, CDialog)
	//{{AFX_MSG_MAP(CPictureCatalog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PC_BTN_LOAD, OnPcBtnLoad)
	ON_BN_CLICKED(IDC_PC_BTN_SAVE, OnPcBtnSave)
	ON_BN_CLICKED(IDC_PC_BTN_DELETE_ARTIST_ITEM, OnPcBtnDeleteArtistItem)
	ON_BN_CLICKED(IDC_PC_BTN_DELETE_LIST_ITEM, OnPcBtnDeleteListItem)
	ON_BN_CLICKED(IDC_PC_BTN_DELETE_SIZE_ITEM, OnPcBtnDeleteSizeItem)
	ON_BN_CLICKED(IDC_PC_BTN_DELETE_STYLE_ITEM, OnPcBtnDeleteStyleItem)
	ON_BN_CLICKED(IDC_PC_BTN_NEW_ARTIST_ITEM, OnPcBtnNewArtistItem)
	ON_BN_CLICKED(IDC_PC_BTN_NEW_LIST_ITEM, OnPcBtnNewListItem)
	ON_BN_CLICKED(IDC_PC_BTN_NEW_SIZE_ITEM, OnPcBtnNewSizeItem)
	ON_BN_CLICKED(IDC_PC_BTN_NEW_STYLE_ITEM, OnPcBtnNewStyleItem)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_PC_LIST_STYLE, OnGetdispinfoPcListStyle)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_PC_LIST_ARTIST, OnGetdispinfoPcListArtist)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_PC_LIST_SIZE, OnGetdispinfoPcListSize)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_PC_LIST_PICTURE, OnGetdispinfoPcListPicture)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_PC_LIST_ARTIST, OnEndlabeleditPcListArtist)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_PC_LIST_PICTURE, OnEndlabeleditPcListPicture)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_PC_LIST_SIZE, OnEndlabeleditPcListSize)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_PC_LIST_STYLE, OnEndlabeleditPcListStyle)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_PC_LIST_STYLE, OnBeginlabeleditPcListStyle)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_PC_LIST_ARTIST, OnColumnclickPcListArtist)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_PC_LIST_STYLE, OnColumnclickPcListStyle)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_PC_LIST_PICTURE, OnColumnclickPcListPicture)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_PC_LIST_PICTURE, OnBeginlabeleditPcListPicture)
	ON_NOTIFY(LVN_KEYDOWN, IDC_PC_LIST_SIZE, OnKeydownPcListSize)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PC_LIST_ARTIST, OnItemchangedPcListArtist)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PC_LIST_PICTURE, OnItemchangedPcListPicture)
	ON_NOTIFY(NM_KILLFOCUS, IDC_PC_LIST_SIZE, OnKillfocusPcListSize)
   ON_NOTIFY(LVN_ITEMCHANGED, IDC_PC_LIST_SIZE, OnItemchangedPcListSize)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PC_LIST_STYLE, OnItemchangedPcListStyle)
	ON_BN_CLICKED(IDC_PC_BTN_SHOW_ALL_PICTURES, OnPcBtnShowAllPictures)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CPictureCatalog 

BOOL CPictureCatalog::OnInitDialog() 
{
	CDialog::OnInitDialog();
   CString str;
	
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{	
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

   str.LoadString(IDS_PICTURE_LIST_H1);
   m_cPictureList.InsertColumn(0, str, LVCFMT_LEFT, 220);
   str.LoadString(IDS_PICTURE_LIST_H2);
   m_cPictureList.InsertColumn(1, str, LVCFMT_LEFT, 120);
   m_cPictureList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

   str.LoadString(IDS_STYLE_LIST_H1);
   m_cStyleList.InsertColumn(0, str, LVCFMT_LEFT, 170);
   str.LoadString(IDS_STYLE_LIST_H2);
   m_cStyleList.InsertColumn(1, str, LVCFMT_LEFT, 170);
   m_cStyleList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

   str.LoadString(IDS_ARTIST_LIST_H1);
   m_cArtistList.InsertColumn(0, str, LVCFMT_LEFT, 340);
   m_cArtistList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

   str.LoadString(IDS_SIZE_LIST_H1);
   m_cSizeList.InsertColumn(0, str, LVCFMT_LEFT, 340);
   m_cSizeList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

   DisableDeleteButtons();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CPictureCatalog::OnCancel() 
{
   if (m_bChanged)
   {
      if (AfxMessageBox(IDS_SAVE_CHANGES, MB_YESNO|MB_ICONQUESTION) == IDYES)
      {
         OnPcBtnSave();
      }
   }
	
	CDialog::OnCancel();
}

void CPictureCatalog::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
      ((CPHPContentManApp*)AfxGetApp())->OnAppAbout();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// Wollen Sie Ihrem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch für Sie erledigt.

void CPictureCatalog::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext für Zeichnen

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Symbol in Client-Rechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// Die Systemaufrufe fragen den Cursorform ab, die angezeigt werden soll, während der Benutzer
//  das zum Symbol verkleinerte Fenster mit der Maus zieht.
HCURSOR CPictureCatalog::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPictureCatalog::OnPcBtnLoad() 
{
   CFile file;
   LPCTSTR pszRead = NULL;
   char *pszSearch = NULL, *pszOld = NULL;;
   int i, nLen, nCount;
   CString strPath, strFile;

//   DeleteLists();

   strFile.Empty();
   strPath.Format("%sContent\\liste.php", ((CPHPContentManApp*)AfxGetApp())->m_strAppPath);
   if (file.Open(strPath, CFile::modeRead))
   {
      CArchive ar(&file, CArchive::load);
      CPHPContentManApp::ReadHTML(ar, strFile);
   }
   if (file.m_hFile != 0xffffffff) file.Close();

   nCount = 0;
   nLen = strFile.GetLength();
   pszRead = strFile;
   if ((pszRead[0] == '<') && (pszRead[1] == '?'))
   {
      for (i=4; i<nLen; i++)
      {
         if (pszRead[i] == '\n') nCount++;
      }

      m_ppPictureList = SetSize(nCount, m_ppPictureList, m_nPictureList);
      pszOld = (char*)pszRead;
      pszSearch    = strstr(&pszOld[1], "#");
      pszOld       = &pszSearch[1];
      for (i=0; i<m_nPictureList; i++)
      {
         pszSearch    = strstr(&pszOld[1], "#");   // Nummer
         pszSearch[0] = 0;
         m_ppPictureList[i]->strText1 = _T(pszOld);
         pszOld       = &pszSearch[1];
         pszSearch    = strstr(&pszOld[1], "#");   // Stil
         pszSearch[0] = 0;
         m_ppPictureList[i]->nStyleIndex  = atoi(pszOld);
         pszOld       = &pszSearch[1];
         pszSearch    = strstr(&pszOld[1], "#");   // Künstler
         pszSearch[0] = 0;
         m_ppPictureList[i]->nArtistIndex = atoi(pszOld);
         pszOld       = &pszSearch[1];
         pszSearch    = strstr(&pszOld[1], "#");   // Größe
         pszSearch[0] = 0;
         m_ppPictureList[i]->nSizeIndex   = atoi(pszOld);
         pszOld       = &pszSearch[1];
         pszSearch    = strstr(&pszOld[1], "#");   // Titel
         pszSearch[0] = 0;
         m_ppPictureList[i]->strText2     = _T(pszOld);
         pszOld       = &pszSearch[1];
         pszSearch    = strstr(&pszOld[1], "#");
         pszOld       = &pszSearch[1];
      }
      m_cPictureList.SetItemCount(m_nPictureList);
   }

   strFile.Empty();
   strPath.Format("%sContent\\tha.php", ((CPHPContentManApp*)AfxGetApp())->m_strAppPath);
   if (file.Open(strPath, CFile::modeRead))
   {
      CArchive ar(&file, CArchive::load);
      CPHPContentManApp::ReadHTML(ar, strFile);
   }
   if (file.m_hFile != 0xffffffff) file.Close();

   nCount = 0;
   nLen = strFile.GetLength();
   pszRead = strFile;
   if ((pszRead[0] == '<') && (pszRead[1] == '?'))
   {
      for (i=4; i<nLen; i++)
      {
         if (pszRead[i] == '\n') nCount++;
      }

      m_ppSizeList = SetSize(nCount, m_ppSizeList, m_nSizeList);
      pszOld = (char*)pszRead;
      pszSearch    = strstr(&pszOld[1], "#");
      pszOld       = &pszSearch[1];
      for (i=0; i<m_nSizeList; i++)
      {
         pszSearch    = strstr(&pszOld[1], "#");   // Größe
         pszSearch[0] = 0;
         m_ppSizeList[i]->strText1 = _T(pszOld);
         m_ppSizeList[i]->nSizeIndex = i+1;
         pszOld       = &pszSearch[1];
         pszSearch    = strstr(&pszOld[1], "#");
         pszOld       = &pszSearch[1];
      }
      m_cSizeList.SetItemCount(m_nSizeList);
   }

   strFile.Empty();
   strPath.Format("%sContent\\thk.php", ((CPHPContentManApp*)AfxGetApp())->m_strAppPath);
   if (file.Open(strPath, CFile::modeRead))
   {
      CArchive ar(&file, CArchive::load);
      CPHPContentManApp::ReadHTML(ar, strFile);
   }
   if (file.m_hFile != 0xffffffff) file.Close();

   nCount = 0;
   nLen = strFile.GetLength();
   pszRead = strFile;
   if ((pszRead[0] == '<') && (pszRead[1] == '?'))
   {
      for (i=4; i<nLen; i++)
      {
         if (pszRead[i] == '\n') nCount++;
      }

      m_ppArtistList = SetSize(nCount, m_ppArtistList, m_nArtistList);
      pszOld = (char*)pszRead;
      pszSearch    = strstr(&pszOld[1], "#");
      pszOld       = &pszSearch[1];
      for (i=0; i<m_nArtistList; i++)
      {
         pszSearch    = strstr(&pszOld[1], "#");   // Künstler
         pszSearch[0] = 0;
         m_ppArtistList[i]->strText1 = _T(pszOld);
         m_ppArtistList[i]->nArtistIndex = i+1;
         pszOld       = &pszSearch[1];
         pszSearch    = strstr(&pszOld[1], "#");
         pszOld       = &pszSearch[1];
      }
      m_cArtistList.SetItemCount(m_nArtistList);
   }

   strFile.Empty();
   strPath.Format("%sContent\\ths.php", ((CPHPContentManApp*)AfxGetApp())->m_strAppPath);
   if (file.Open(strPath, CFile::modeRead))
   {
      CArchive ar(&file, CArchive::load);
      CPHPContentManApp::ReadHTML(ar, strFile);
   }
   if (file.m_hFile != 0xffffffff) file.Close();

   nCount = 0;
   nLen = strFile.GetLength();
   pszRead = strFile;
   if ((pszRead[0] == '<') && (pszRead[1] == '?'))
   {
      for (i=4; i<nLen; i++)
      {
         if (pszRead[i] == '\n') nCount++;
      }

      m_ppStyleList = SetSize(nCount, m_ppStyleList, m_nStyleList);
      pszOld = (char*)pszRead;
      pszSearch    = strstr(&pszOld[1], "#");
      pszOld       = &pszSearch[1];
      for (i=0; i<m_nStyleList; i++)
      {
         pszSearch    = strstr(&pszOld[1], "#");   // Stil deutsch
         pszSearch[0] = 0;
         m_ppStyleList[i]->strText1 = _T(pszOld);
         pszOld       = &pszSearch[1];
         pszSearch    = strstr(&pszOld[1], "#");   // Stil englisch
         pszSearch[0] = 0;
         m_ppStyleList[i]->strText2 = _T(pszOld);
         m_ppStyleList[i]->nStyleIndex = i+1;
         pszOld       = &pszSearch[1];
         pszSearch    = strstr(&pszOld[1], "#");
         pszOld       = &pszSearch[1];
      }
      m_cStyleList.SetItemCount(m_nStyleList);
   }

   m_nPictureSort    = 0;
   m_nSizeListSort   = 0;
   m_nArtistListSort = 0;
   m_nStyleListSort  = 0;
   m_bChanged        = false;
}

void CPictureCatalog::OnPcBtnSave() 
{
   CFile file;
   CString  strBegin = _T("<?\r\n");
   CString  strEnd   = _T("?>");
   CString  strWrite;
   CString  strPath, strFile;
   int i;

   UpdateData(true);

   strPath.Format("%sContent\\liste.php", ((CPHPContentManApp*)AfxGetApp())->m_strAppPath);
   if (file.Open(strPath, CFile::modeCreate|CFile::modeWrite))
   {
      CArchive ar(&file, CArchive::store);
      ar.Write(strBegin, strBegin.GetLength());
      for (i=0; i<m_nPictureList; i++)
      {
         strWrite.Format("#%s#%d#%d#%d#%s#\r\n", m_ppPictureList[i]->strText1,
                                                 m_ppPictureList[i]->nStyleIndex,
                                                 m_ppPictureList[i]->nArtistIndex,
                                                 m_ppPictureList[i]->nSizeIndex,
                                                 m_ppPictureList[i]->strText2);
         CPHPContentManApp::WriteHTML(ar, strWrite);
      }
      ar.Write(strEnd, strEnd.GetLength());
   }
   if (file.m_hFile != 0xffffffff) file.Close();

   strPath.Format("%sContent\\tha.php", ((CPHPContentManApp*)AfxGetApp())->m_strAppPath);
   if (file.Open(strPath, CFile::modeCreate|CFile::modeWrite))
   {
      CArchive ar(&file, CArchive::store);
      ar.Write(strBegin, strBegin.GetLength());
      for (i=0; i<m_nSizeList; i++)
      {
         strWrite.Format("#%s#\r\n", m_ppSizeList[i]->strText1);
         CPHPContentManApp::WriteHTML(ar, strWrite);
      }
      ar.Write(strEnd, strEnd.GetLength());
   }
   if (file.m_hFile != 0xffffffff) file.Close();

   strPath.Format("%sContent\\thk.php", ((CPHPContentManApp*)AfxGetApp())->m_strAppPath);
   if (file.Open(strPath, CFile::modeCreate|CFile::modeWrite))
   {
      CArchive ar(&file, CArchive::store);
      ar.Write(strBegin, strBegin.GetLength());
      for (i=0; i<m_nArtistList; i++)
      {
         strWrite.Format("#%s#\r\n", m_ppArtistList[i]->strText1);
         CPHPContentManApp::WriteHTML(ar, strWrite);
      }
      ar.Write(strEnd, strEnd.GetLength());
   }
   if (file.m_hFile != 0xffffffff) file.Close();

   strPath.Format("%sContent\\ths.php", ((CPHPContentManApp*)AfxGetApp())->m_strAppPath);
   if (file.Open(strPath, CFile::modeCreate|CFile::modeWrite))
   {
      CArchive ar(&file, CArchive::store);
      ar.Write(strBegin, strBegin.GetLength());
      for (i=0; i<m_nStyleList; i++)
      {
         strWrite.Format("#%s#%s#\r\n", m_ppStyleList[i]->strText1, m_ppStyleList[i]->strText2);
         CPHPContentManApp::WriteHTML(ar, strWrite);
      }
      ar.Write(strEnd, strEnd.GetLength());
   }
   if (file.m_hFile != 0xffffffff) file.Close();
   m_bChanged = false;
}

void CPictureCatalog::DeleteLists()
{
   m_ppPictureList = SetSize(0, m_ppPictureList, m_nPictureList);
   m_ppSizeList    = SetSize(0, m_ppSizeList   , m_nSizeList);
   m_ppArtistList  = SetSize(0, m_ppArtistList , m_nArtistList);
   m_ppStyleList   = SetSize(0, m_ppStyleList  , m_nStyleList);
}

PictureList** CPictureCatalog::SetSize(int nNewLen, PictureList **ppList, int &nLen)
{
   int i;
   if (nNewLen < 0) return ppList;
   for (i=nNewLen; i<nLen; i++)        // Liste wird verkürzt
   {
      ASSERT(ppList[i]!=NULL);
      delete ppList[i];
   }

   if (nNewLen > 0) ppList = (PictureList**) realloc(ppList, nNewLen*sizeof(PictureList*));
   else
   {
      if (ppList) free(ppList);
      ppList = NULL;
   }

   for (i=nLen; i<nNewLen; i++)        // Liste wird vergrößert
   {
      ppList[i] = new PictureList;
   }

   nLen = nNewLen;
   return ppList;
}

void CPictureCatalog::OnPcBtnNewArtistItem() 
{
   int nArtists = m_nArtistList + 1;
   m_ppArtistList = SetSize(nArtists, m_ppArtistList, m_nArtistList);
   m_cArtistList.SetItemCount(m_nArtistList);
   nArtists = m_nArtistList - 1;
   m_ppArtistList[nArtists]->nArtistIndex = m_nArtistList;
   m_cArtistList.SetFocus();
   m_cArtistList.EnsureVisible(nArtists, false);
   m_cArtistList.EditLabel(nArtists);
   m_nSubItem = 0;
   m_bChanged = true;
}

void CPictureCatalog::OnPcBtnDeleteArtistItem() 
{
   if (m_cPictureList.GetItemCount())
   {
      AfxMessageBox(IDS_CANT_DELETE_ARTIST, MB_OK|MB_ICONINFORMATION);
      return;
   }
   int nArtists = m_nArtistList - 1;
   int nSel     = m_cArtistList.GetSelectionMark();
   if (nSel != -1)
   {
      PictureList *pTemp              = m_ppArtistList[nSel];
      int          nOldIndex          = m_ppArtistList[m_nArtistList-1]->nArtistIndex;
      m_ppArtistList[nSel]            = m_ppArtistList[m_nArtistList-1];
      m_ppArtistList[m_nArtistList-1] = pTemp;
      m_ppArtistList[nSel]->nArtistIndex = nSel+1;
      for (int i=0; i<m_nPictureList; i++)
      {
         if (m_ppPictureList[i]->nArtistIndex == nOldIndex)
            m_ppPictureList[i]->nArtistIndex = m_ppArtistList[nSel]->nArtistIndex;
      }
      ResetSelections();
      m_ppArtistList = SetSize(nArtists, m_ppArtistList, m_nArtistList);
      m_cArtistList.SetItemCount(m_nArtistList);
      m_cArtistList.InvalidateRect(NULL);
      m_bChanged = true;
   }
}

void CPictureCatalog::OnPcBtnNewListItem() 
{
   int nPictures = m_nPictureList+ 1;
   m_ppPictureList = SetSize(nPictures, m_ppPictureList, m_nPictureList);
   m_cPictureList.SetItemCount(m_nPictureList);
   nPictures = m_nPictureList - 1;
   m_cPictureList.SetFocus();
   m_cPictureList.EnsureVisible(nPictures, false);
   m_cPictureList.EditLabel(nPictures);
   m_nSubItem = 0;
   m_bChanged = true;
}

void CPictureCatalog::OnPcBtnDeleteListItem() 
{
   int nPictures = m_nPictureList - 1;
   int nSel = m_cPictureList.GetSelectionMark();
   if (nSel != -1)
   {
      int nNewCount = -1;
      if (m_cPictureList.GetItemCount() != m_nPictureList)
      {
         nNewCount = m_cPictureList.GetItemCount()-1;
         PictureList *pTemp                = m_ppPictureList[nSel];
         m_ppPictureList[nSel]             = m_ppPictureList[nNewCount];
         m_ppPictureList[nNewCount]        = m_ppPictureList[m_nPictureList-1];
         m_ppPictureList[m_nPictureList-1] = pTemp;
      }
      else
      {
         PictureList *pTemp                = m_ppPictureList[nSel];
         m_ppPictureList[nSel]             = m_ppPictureList[m_nPictureList-1];
         m_ppPictureList[m_nPictureList-1] = pTemp;
      }
      m_ppPictureList = SetSize(nPictures, m_ppPictureList, m_nPictureList);
      ResetSelections();
      if (nNewCount == -1) m_cPictureList.SetItemCount(m_nPictureList);
      else                 m_cPictureList.SetItemCount(nNewCount);

      m_cPictureList.InvalidateRect(NULL);
      m_bChanged = true;
   }
}

void CPictureCatalog::OnPcBtnNewSizeItem() 
{
   int nSize = m_nSizeList+ 1;
   m_ppSizeList = SetSize(nSize, m_ppSizeList, m_nSizeList);
   m_cSizeList.SetItemCount(m_nSizeList);
   nSize = m_nSizeList - 1;
   m_ppSizeList[nSize]->nSizeIndex = m_nSizeList;
   m_cSizeList.SetFocus();
   m_cSizeList.EnsureVisible(nSize, false);
   m_cSizeList.EditLabel(nSize);
   m_nSubItem = 0;
   m_bChanged = true;
}

void CPictureCatalog::OnPcBtnDeleteSizeItem() 
{
   if (m_cPictureList.GetItemCount())
   {
      AfxMessageBox(IDS_CANT_DELETE_SIZE, MB_OK|MB_ICONINFORMATION);
      return;
   }
   int nSize = m_nSizeList - 1;
   int nSel = m_cSizeList.GetSelectionMark();
   if (nSel != -1)
   {
      PictureList *pTemp             = m_ppSizeList[nSel];
      int          nOldIndex         = m_ppSizeList[m_nSizeList-1]->nSizeIndex;
      m_ppSizeList[nSel]             = m_ppSizeList[m_nSizeList-1];
      m_ppSizeList[m_nSizeList-1]    = pTemp;
      m_ppSizeList[nSel]->nSizeIndex = nSel+1;
      for (int i=0; i<m_nPictureList; i++)
      {
         if (m_ppPictureList[i]->nSizeIndex == nOldIndex)
            m_ppPictureList[i]->nSizeIndex = m_ppSizeList[nSel]->nSizeIndex;
      }
      ResetSelections();
      m_ppSizeList = SetSize(nSize, m_ppSizeList, m_nSizeList);
      m_cSizeList.SetItemCount(m_nSizeList);
      m_cSizeList.InvalidateRect(NULL);
      m_bChanged = true;
   }
}

void CPictureCatalog::OnPcBtnNewStyleItem() 
{
   int nStyle = m_nStyleList+ 1;
   m_ppStyleList = SetSize(nStyle, m_ppStyleList, m_nStyleList);
   m_cStyleList.SetItemCount(m_nStyleList);
   nStyle = m_nStyleList - 1;
   m_ppStyleList[nStyle]->nStyleIndex = m_nStyleList;
   m_cStyleList.SetFocus();
   m_cStyleList.EnsureVisible(nStyle, false);
   m_cStyleList.EditLabel(nStyle);
   m_nSubItem = 0;
   m_bChanged = true;
}

void CPictureCatalog::OnPcBtnDeleteStyleItem() 
{
   if (m_cPictureList.GetItemCount())
   {
      AfxMessageBox(IDS_CANT_DELETE_STYLE, MB_OK|MB_ICONINFORMATION);
      return;
   }
   int nStyle = m_nStyleList - 1;
   int nSel  = m_cStyleList.GetSelectionMark();
   if (nSel != -1)
   {
      PictureList *pTemp            = m_ppStyleList[nSel];
      int          nOldIndex        = m_ppStyleList[m_nStyleList-1]->nStyleIndex;
      m_ppStyleList[nSel]           = m_ppStyleList[m_nStyleList-1];
      m_ppStyleList[m_nStyleList-1] = pTemp;
      m_ppStyleList[nSel]->nStyleIndex = nSel+1;
      for (int i=0; i<m_nPictureList; i++)
      {
         if (m_ppPictureList[i]->nStyleIndex == nOldIndex)
            m_ppPictureList[i]->nStyleIndex = m_ppStyleList[nSel]->nStyleIndex;
      }
      ResetSelections();
      m_ppStyleList = SetSize(nStyle, m_ppStyleList, m_nStyleList);
      m_cStyleList.SetItemCount(m_nStyleList);
      m_cStyleList.InvalidateRect(NULL);
      m_bChanged = true;
   }
}


void CPictureCatalog::OnGetdispinfoPcListStyle(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   if (pDispInfo->item.mask & LVIF_TEXT)
   {
      if (pDispInfo->item.iSubItem == 0)
         strncpy(pDispInfo->item.pszText, m_ppStyleList[pDispInfo->item.iItem]->strText1, pDispInfo->item.cchTextMax);
      if (pDispInfo->item.iSubItem == 1)
         strncpy(pDispInfo->item.pszText, m_ppStyleList[pDispInfo->item.iItem]->strText2, pDispInfo->item.cchTextMax);
   }
	*pResult = 0;
}

void CPictureCatalog::OnGetdispinfoPcListArtist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   if (pDispInfo->item.mask & LVIF_TEXT)
   {
      if (pDispInfo->item.iSubItem == 0)
         strncpy(pDispInfo->item.pszText, m_ppArtistList[pDispInfo->item.iItem]->strText1, pDispInfo->item.cchTextMax);
   }
	
	*pResult = 0;
}

void CPictureCatalog::OnGetdispinfoPcListSize(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   if (pDispInfo->item.mask & LVIF_TEXT)
   {
      if (pDispInfo->item.iSubItem == 0)
         strncpy(pDispInfo->item.pszText, m_ppSizeList[pDispInfo->item.iItem]->strText1, pDispInfo->item.cchTextMax);
   }
	
	*pResult = 0;
}

void CPictureCatalog::OnBeginlabeleditPcListPicture(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 1;
   LVHITTESTINFO lvHTI;
   ::GetCursorPos(&lvHTI.pt);
   m_cPictureList.ScreenToClient(&lvHTI.pt);
   m_cPictureList.SendMessage(LVM_SUBITEMHITTEST, 0, (LPARAM)&lvHTI);
   if ((lvHTI.iItem != -1) && (lvHTI.flags & LVHT_ONITEM) && ((lvHTI.iSubItem == 0) || (lvHTI.iSubItem == 1)))
   {
      CEdit* pEdit = m_cPictureList.GetEditControl();
      if (pEdit)
      {
         if (::GetWindowLong(m_cPictureList.m_hWnd, GWL_USERDATA)==0)
         {
            m_nItem    = lvHTI.iItem;
            m_nSubItem = lvHTI.iSubItem;
            gm_lOldEditCtrlWndProc = ::SetWindowLong(pEdit->m_hWnd, GWL_WNDPROC, (long)EditCtrlWndProc);
            ::SetWindowLong(m_cPictureList.m_hWnd, GWL_USERDATA, (long)this);
            CString str = m_cPictureList.GetItemText(m_nItem, m_nSubItem);
            pEdit->SetWindowText(str);
            *pResult = 0;
         }
      }
   }
	
	*pResult = 0;
}
void CPictureCatalog::OnEndlabeleditPcListPicture(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;
   CEdit* pEdit = m_cPictureList.GetEditControl();
   if (pEdit)
   {
      ::SetWindowLong(pEdit->m_hWnd, GWL_WNDPROC, gm_lOldEditCtrlWndProc);
      gm_lOldEditCtrlWndProc = 0;
      if ((pDispInfo->item.mask & LVIF_TEXT) &&
          (pDispInfo->item.pszText != NULL))
      {
         if (m_nSubItem == 0) m_ppPictureList[pDispInfo->item.iItem]->strText2 = _T(pDispInfo->item.pszText);
         else                 m_ppPictureList[pDispInfo->item.iItem]->strText1 = _T(pDispInfo->item.pszText);
         m_cPictureList.Update(pDispInfo->item.iItem);
      }
   }
   m_nSubItem = 0;
   ::SetWindowLong(m_cPictureList.m_hWnd, GWL_USERDATA, (long)0);
}

void CPictureCatalog::OnGetdispinfoPcListPicture(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   if (pDispInfo->item.mask & LVIF_TEXT)
   {
      if (pDispInfo->item.iSubItem == 0)
         strncpy(pDispInfo->item.pszText, m_ppPictureList[pDispInfo->item.iItem]->strText2, pDispInfo->item.cchTextMax);
      if (pDispInfo->item.iSubItem == 1)
         strncpy(pDispInfo->item.pszText, m_ppPictureList[pDispInfo->item.iItem]->strText1, pDispInfo->item.cchTextMax);
   }
	
	*pResult = 0;
}

void CPictureCatalog::OnEndlabeleditPcListArtist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   if ((pDispInfo->item.mask & LVIF_TEXT) &&
       (pDispInfo->item.pszText != NULL))
   {
      m_ppArtistList[pDispInfo->item.iItem]->strText1 = _T(pDispInfo->item.pszText);
   }
	
	*pResult = 0;
}

void CPictureCatalog::OnEndlabeleditPcListSize(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   if ((pDispInfo->item.mask & LVIF_TEXT) &&
       (pDispInfo->item.pszText != NULL))
   {
      m_ppSizeList[pDispInfo->item.iItem]->strText1 = _T(pDispInfo->item.pszText);
   }
	
	*pResult = 0;
}

void CPictureCatalog::OnBeginlabeleditPcListStyle(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 1;
   LVHITTESTINFO lvHTI;
   ::GetCursorPos(&lvHTI.pt);
   m_cStyleList.ScreenToClient(&lvHTI.pt);
   m_cStyleList.SendMessage(LVM_SUBITEMHITTEST, 0, (LPARAM)&lvHTI);
   if ((lvHTI.iItem != -1) && (lvHTI.flags & LVHT_ONITEM) && ((lvHTI.iSubItem == 0) || (lvHTI.iSubItem == 1)))
   {
      CEdit* pEdit = m_cStyleList.GetEditControl();
      if (pEdit)
      {
         if (::GetWindowLong(m_cStyleList.m_hWnd, GWL_USERDATA)==0)
         {
            m_nItem    = lvHTI.iItem;
            m_nSubItem = lvHTI.iSubItem;
            gm_lOldEditCtrlWndProc = ::SetWindowLong(pEdit->m_hWnd, GWL_WNDPROC, (long)EditCtrlWndProc);
            ::SetWindowLong(m_cStyleList.m_hWnd, GWL_USERDATA, (long)this);
            CString str = m_cStyleList.GetItemText(m_nItem, m_nSubItem);
            pEdit->SetWindowText(str);
            *pResult = 0;
         }
      }
   }
}

void CPictureCatalog::OnEndlabeleditPcListStyle(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	*pResult = 0;
   CEdit* pEdit = m_cStyleList.GetEditControl();
   if (pEdit)
   {
      ::SetWindowLong(pEdit->m_hWnd, GWL_WNDPROC, gm_lOldEditCtrlWndProc);
      gm_lOldEditCtrlWndProc = 0;
      if ((pDispInfo->item.mask & LVIF_TEXT) &&
          (pDispInfo->item.pszText != NULL))
      {
         if (m_nSubItem == 0) m_ppStyleList[pDispInfo->item.iItem]->strText1 = _T(pDispInfo->item.pszText);
         else                 m_ppStyleList[pDispInfo->item.iItem]->strText2 = _T(pDispInfo->item.pszText);
         m_cStyleList.Update(pDispInfo->item.iItem);
      }
   }
   m_nSubItem = 0;
   ::SetWindowLong(m_cStyleList.m_hWnd, GWL_USERDATA, (long)0);
}

LRESULT CALLBACK CPictureCatalog::EditCtrlWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   if (gm_lOldEditCtrlWndProc == 0) return 0;

   if (uMsg == WM_WINDOWPOSCHANGING)
   {
      WINDOWPOS *pwp = (WINDOWPOS*)lParam;
      HWND hwndP = ::GetParent(hwnd);
      CRect rc;
      CPictureCatalog *pDlg = (CPictureCatalog*) ::GetWindowLong(hwndP, GWL_USERDATA);
      if (pDlg)
      {
         ListView_GetSubItemRect(hwndP, pDlg->m_nItem, pDlg->m_nSubItem, LVIR_BOUNDS, (RECT*)&rc);
         pwp->x = rc.left;
         pwp->y = rc.top;
         pwp->cx= rc.Width();
         pwp->cy= rc.Height();
      }
   }  
   return CallWindowProc((WNDPROC)gm_lOldEditCtrlWndProc, hwnd, uMsg, wParam, lParam);
}

void CPictureCatalog::OnColumnclickPcListArtist(NMHDR* pNMHDR, LRESULT* pResult)
{
   if (m_ppArtistList)
   {
	   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	   if (m_nArtistListSort <= 0) m_nArtistListSort =  1;
      else                        m_nArtistListSort = -1;
      gm_nSortDir = m_nArtistListSort;
      gm_bSortChanged = false;
      qsort(m_ppArtistList, m_nArtistList, sizeof(PictureList*), SortText1);
      m_nArtistListSort |= SORT_CHANGED;
      m_cArtistList.InvalidateRect(NULL);
      UpdateDependencies();
   }
	*pResult = 0;
}

void CPictureCatalog::OnColumnclickPcListStyle(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (m_ppStyleList)
   {
   	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	   if (m_nStyleListSort <= 0) m_nStyleListSort =  1;
      else                       m_nStyleListSort = -1;
      gm_nSortDir = m_nStyleListSort;
      gm_bSortChanged = false;
      if (pNMListView->iSubItem == 0)
         qsort(m_ppStyleList, m_nStyleList, sizeof(PictureList*), SortText1);
      else
         qsort(m_ppStyleList, m_nStyleList, sizeof(PictureList*), SortText2);
      m_cStyleList.InvalidateRect(NULL);
      m_nStyleListSort |= SORT_CHANGED;
      UpdateDependencies();
   }
	*pResult = 0;
}

void CPictureCatalog::OnColumnclickPcListPicture(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (m_ppPictureList)
   {
      DisableDeleteButtons();
   	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	   if (m_nPictureSort <= 0) m_nPictureSort =  1;
      else                     m_nPictureSort = -1;
      gm_nSortDir = m_nPictureSort;
      gm_bSortChanged = false;
      int nPictureList = m_cPictureList.GetItemCount();
      if (pNMListView->iSubItem == 0)
         qsort(m_ppPictureList, nPictureList, sizeof(PictureList*), SortText2);
      else
         qsort(m_ppPictureList, nPictureList, sizeof(PictureList*), SortText1);
      m_cPictureList.InvalidateRect(NULL);
   }
	*pResult = 0;
}

int _cdecl CPictureCatalog::SortText1(const void *p1, const void *p2)
{
   PictureList *pL1 = ((PictureList**)p1)[0];
   PictureList *pL2 = ((PictureList**)p2)[0];
   int nRet = strcmpi(pL1->strText1, pL2->strText1) * gm_nSortDir;
   if (nRet < 0) gm_bSortChanged = true;
   return nRet;
}

int _cdecl CPictureCatalog::SortText2(const void *p1, const void *p2)
{
   PictureList *pL1 = ((PictureList**)p1)[0];
   PictureList *pL2 = ((PictureList**)p2)[0];
   int nRet = strcmpi(pL1->strText2, pL2->strText2) * gm_nSortDir;
   if (nRet < 0) gm_bSortChanged = true;
   return nRet;
}

int _cdecl CPictureCatalog::SortArtist(const void *p1, const void *p2)
{
   PictureList *pL1 = ((PictureList**)p1)[0];
   PictureList *pL2 = ((PictureList**)p2)[0];
   if (pL1->nArtistIndex == gm_nSortDir) return -1;
   if (pL2->nArtistIndex == gm_nSortDir) return 1;
   return -1;
}

int _cdecl CPictureCatalog::SortStyle(const void *p1, const void *p2)
{
   PictureList *pL1 = ((PictureList**)p1)[0];
   PictureList *pL2 = ((PictureList**)p2)[0];
   if (pL1->nStyleIndex == gm_nSortDir) return -1;
   if (pL2->nStyleIndex == gm_nSortDir) return 1;
   return -1;
}

int _cdecl CPictureCatalog::SortSize(const void *p1, const void *p2)
{
   PictureList *pL1 = ((PictureList**)p1)[0];
   PictureList *pL2 = ((PictureList**)p2)[0];
   if (pL1->nSizeIndex == gm_nSortDir) return -1;
   if (pL2->nSizeIndex == gm_nSortDir) return 1;
   return -1;
}

void CPictureCatalog::UpdateDependencies()
{
   int i, j;
   if (m_nArtistListSort & SORT_CHANGED)
   {
      for (i=0; i<m_nPictureList; i++)
      {
         for (j=1; j<=m_nArtistList; j++)
         {
            if (m_ppPictureList[i]->nArtistIndex == m_ppArtistList[j-1]->nArtistIndex)
               break;
         }
         if ((j>0) && (j <= m_nArtistList))
         {
            m_ppPictureList[i]->nArtistIndex = j;
         }
      }         
      for (j=1; j<=m_nArtistList; j++)
      {
         m_ppArtistList[j-1]->nArtistIndex = j;
      }
      m_nArtistListSort &= ~SORT_CHANGED;
      m_bChanged = true;
   }
   if (m_nSizeListSort & SORT_CHANGED)
   {
      for (i=0; i<m_nPictureList; i++)
      {
         for (j=1; j<=m_nSizeList; j++)
         {
            if (m_ppPictureList[i]->nSizeIndex == m_ppSizeList[j-1]->nSizeIndex)
               break;
         }
         if ((j>0) && (j <= m_nSizeList))
         {
            m_ppPictureList[i]->nSizeIndex = j;
         }
      }
      for (j=1; j<=m_nSizeList; j++)
      {
         m_ppSizeList[j-1]->nSizeIndex = j;
      }
      m_nSizeListSort &= ~SORT_CHANGED;
      m_bChanged = true;
   }
   if (m_nStyleListSort & SORT_CHANGED)
   {
      for (i=0; i<m_nPictureList; i++)
      {
         for (j=1; j<=m_nStyleList; j++)
         {
            if (m_ppPictureList[i]->nStyleIndex== m_ppStyleList[j-1]->nStyleIndex)
               break;
         }
         if ((j>0) && (j <= m_nStyleList))
         {
            m_ppPictureList[i]->nStyleIndex = j;
         }
      }
      for (j=1; j<=m_nStyleList; j++)
      {
         m_ppStyleList[j-1]->nStyleIndex = j;
      }
      m_nStyleListSort &= ~SORT_CHANGED;
      m_bChanged = true;
   }
}

void CPictureCatalog::OnKeydownPcListSize(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	*pResult = 0;
   int nSel = m_cSizeList.GetSelectionMark();
   if (nSel != -1)
   {
      int nNewSel = -1;
      if (pLVKeyDow->wVKey == VK_LEFT)
      {
         if (nSel > 0) nNewSel = nSel-1;
      }
      else if (pLVKeyDow->wVKey == VK_RIGHT)
      {
         if (nSel < m_nSizeList-1) nNewSel = nSel+1;
      }
      if (nNewSel != -1)
      {
         m_bKeydownSizeList = true;
         PictureList *pTemp    = m_ppSizeList[nSel];
         m_ppSizeList[nSel]    = m_ppSizeList[nNewSel];
         m_ppSizeList[nNewSel] = pTemp;
         m_cSizeList.SetItemState(nNewSel, LVIS_SELECTED, LVIS_SELECTED);
         m_cSizeList.SetHotItem(nNewSel);
         m_cSizeList.SetSelectionMark(nNewSel);
         m_cSizeList.EnsureVisible(nNewSel, false);
         m_cSizeList.InvalidateRect(NULL);
         m_nSizeListSort |= SORT_CHANGED;
         m_bKeydownSizeList = false;
      	*pResult = 1;
      }
   }
}

void CPictureCatalog::OnKillfocusPcListSize(NMHDR* pNMHDR, LRESULT* pResult) 
{
   UpdateDependencies();	
	*pResult = 0;
}

void CPictureCatalog::DisableDeleteButtons()
{
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_PC_BTN_DELETE_STYLE_ITEM) , false);
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_PC_BTN_DELETE_ARTIST_ITEM), false);
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_PC_BTN_DELETE_SIZE_ITEM)  , false);
}

void CPictureCatalog::OnItemchangedPcListPicture(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
   int nSel = pNMListView->iItem;
   m_bItemChangedPictureList = true;
   DisableDeleteButtons();
   if ((nSel >= 0) && (nSel < m_nPictureList))
   {
      if (pNMListView->uNewState != 0)
      {
         int nSetSel = m_ppPictureList[nSel]->nArtistIndex-1;
         m_cArtistList.SetSelectionMark(nSetSel);
         m_cArtistList.SetItemState(nSetSel, LVIS_SELECTED, LVIS_SELECTED);
         m_cArtistList.SetHotItem(nSetSel);
         m_cArtistList.EnsureVisible(nSetSel, false);

         nSetSel = m_ppPictureList[nSel]->nSizeIndex-1;
         m_cSizeList.SetSelectionMark(nSetSel);
         m_cSizeList.SetItemState(nSetSel, LVIS_SELECTED, LVIS_SELECTED);
         m_cSizeList.SetHotItem(nSetSel);
         m_cSizeList.EnsureVisible(nSetSel, false);

         nSetSel = m_ppPictureList[nSel]->nStyleIndex-1;
         m_cStyleList.SetSelectionMark(nSetSel);
         m_cStyleList.SetItemState(nSetSel, LVIS_SELECTED, LVIS_SELECTED);
         m_cStyleList.SetHotItem(nSetSel);
         m_cStyleList.EnsureVisible(nSetSel, false);
      }
   }
   m_bItemChangedPictureList = false;
	*pResult = 0;
}

void CPictureCatalog::OnItemchangedPcListStyle(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
   if (!m_bItemChangedPictureList && (pNMListView->iItem != -1) && (pNMListView->uNewState != 0))
   {
      int nSetSel  = pNMListView->iItem;
      int nSel     = m_cPictureList.GetSelectionMark();
      int nStyle   = m_ppStyleList[nSetSel]->nStyleIndex;
      bool bSelect = true;
      if ((nSel >= 0) && (nSel< m_nPictureList))
      {
         if (m_ppPictureList[nSel]->nStyleIndex != nStyle)
         {
            CString str;
            str.Format(IDS_SELECTSTYLE, m_ppPictureList[nSel]->strText2);
            if (AfxMessageBox(str, MB_YESNO) == IDYES)
            {
               m_ppPictureList[nSel]->nStyleIndex = nStyle;
               m_cPictureList.SetHotItem(nSel);
               bSelect = false;
            }
         }
         m_cStyleList.SetFocus();
      }	

      m_cStyleList.SetHotItem(nSetSel);
      if (bSelect && (nSetSel != -1))
      {
         CString str;
         str.Format(IDS_PICTURES_OF_STYLE, m_ppStyleList[nSetSel]->strText1);
         SetHeadingOfPictList(str);
         gm_nSortDir = nStyle;
         qsort(m_ppPictureList, m_nPictureList, sizeof(PictureList*), SortStyle);
         for (int i=0; i<m_nPictureList; i++)
         {
            if (m_ppPictureList[i]->nStyleIndex != nStyle) break;
         }
         DisableDeleteButtons();
         ::EnableWindow(::GetDlgItem(m_hWnd, IDC_PC_BTN_DELETE_STYLE_ITEM), true);

         m_cPictureList.SetItemCount(i);
         if (nSel != -1)
         {
            m_cPictureList.SetItemState(nSel, 0, LVIS_SELECTED);
            m_cPictureList.SetSelectionMark(-1);
         }
      }
   }
	*pResult = 0;
}

void CPictureCatalog::OnItemchangedPcListArtist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
   if (!m_bItemChangedPictureList && (pNMListView->iItem != -1) && (pNMListView->uNewState != 0))
   {
      int nSetSel  = pNMListView->iItem;
      int nSel     = m_cPictureList.GetSelectionMark();
      int nArtist  = m_ppArtistList[nSetSel]->nArtistIndex;
      bool bSelect = true;
      if ((nSel >= 0) && (nSel< m_nPictureList))
      {
         if (m_ppPictureList[nSel]->nArtistIndex != nArtist)
         {
            CString str;
            str.Format(IDS_SELECTARTIST, m_ppPictureList[nSel]->strText2);
            if (AfxMessageBox(str, MB_YESNO) == IDYES)
            {
               m_ppPictureList[nSel]->nArtistIndex = nArtist;
               m_cPictureList.SetHotItem(nSel);
               m_cPictureList.SetItemState(nSetSel, LVIS_SELECTED, LVIS_SELECTED);
               bSelect = false;
            }
         }
         m_cArtistList.SetFocus();
      }

      m_cArtistList.SetHotItem(nSetSel);
      if (bSelect && (nSetSel != -1))
      {
         CString str;
         str.Format(IDS_PICTURES_OF_ARTIST, m_ppArtistList[nSetSel]->strText1);
         SetHeadingOfPictList(str);
         gm_nSortDir = nArtist;
         qsort(m_ppPictureList, m_nPictureList, sizeof(PictureList*), SortArtist);
         for (int i=0; i<m_nPictureList; i++)
         {
            if (m_ppPictureList[i]->nArtistIndex != nArtist) break;
         }
         DisableDeleteButtons();
         ::EnableWindow(::GetDlgItem(m_hWnd, IDC_PC_BTN_DELETE_ARTIST_ITEM), true);

         m_cPictureList.SetItemCount(i);
         if (nSel != -1)
         {
            m_cPictureList.SetItemState(nSel, 0, LVIS_SELECTED);
            m_cPictureList.SetSelectionMark(-1);
         }
      }
   }
	*pResult = 0;
}

void CPictureCatalog::OnItemchangedPcListSize(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
   if (!m_bItemChangedPictureList && !m_bKeydownSizeList && (pNMListView->iItem != -1) && (pNMListView->uNewState != 0))
   {
      int n = m_cSizeList.GetSelectionMark();
      int nSetSel = pNMListView->iItem;
      int nSel    = m_cPictureList.GetSelectionMark();
      int nSize   = m_ppSizeList[nSetSel]->nSizeIndex;
      bool bSelect = true;
      if ((nSel >= 0) && (nSel< m_nPictureList))
      {
         if (m_ppPictureList[nSel]->nSizeIndex != nSize)
         {
            CString str;
            str.Format(IDS_SELECTFORMAT, m_ppPictureList[nSel]->strText2);
            if (AfxMessageBox(str, MB_YESNO) == IDYES)
            {
               m_ppPictureList[nSel]->nSizeIndex = nSize;
               m_cPictureList.SetHotItem(nSel);
               bSelect = false;
            }
         }
         m_cSizeList.SetFocus();
      }	

      m_cSizeList.SetHotItem(nSetSel);
      if (bSelect && (nSetSel != -1))
      {
         CString str;
         str.Format(IDS_PICTURES_OF_SIZE, m_ppSizeList[nSetSel]->strText1);
         SetHeadingOfPictList(str);
         gm_nSortDir = nSize;
         qsort(m_ppPictureList, m_nPictureList, sizeof(PictureList*), SortSize);
         for (int i=0; i<m_nPictureList; i++)
         {
            if (m_ppPictureList[i]->nSizeIndex != nSize) break;
         }
         DisableDeleteButtons();
         ::EnableWindow(::GetDlgItem(m_hWnd, IDC_PC_BTN_DELETE_SIZE_ITEM), true);
         m_cPictureList.SetItemCount(i);
         if (nSel != -1)
         {
            m_cPictureList.SetItemState(nSel, 0, LVIS_SELECTED);
            m_cPictureList.SetSelectionMark(-1);
         }
      }
   }
	*pResult = 0;
}

void CPictureCatalog::SetHeadingOfPictList(CString& str)
{
   LVCOLUMNA lvc;
   ZeroMemory(&lvc, sizeof(LVCOLUMN));
   lvc.mask = LVCF_TEXT;
   lvc.pszText = (char*)LPCTSTR(str);
   lvc.cchTextMax = str.GetLength();
   m_cPictureList.SetColumn(0, &lvc);
}

void CPictureCatalog::OnPcBtnShowAllPictures() 
{
   CString str;
   str.LoadString(IDS_PICTURE_LIST_H1);
   m_cPictureList.SetItemCount(m_nPictureList);
   SetHeadingOfPictList(str);
}

void CPictureCatalog::ResetSelections()
{
   int nSel = m_cArtistList.GetSelectionMark();
   m_cArtistList.SetItemState(nSel, 0, LVIS_SELECTED);
   m_cArtistList.SetSelectionMark(-1);

   nSel = m_cSizeList.GetSelectionMark();
   m_cSizeList.SetItemState(nSel, 0, LVIS_SELECTED);
   m_cSizeList.SetSelectionMark(-1);

   nSel = m_cStyleList.GetSelectionMark();
   m_cStyleList.SetItemState(nSel, 0, LVIS_SELECTED);
   m_cStyleList.SetSelectionMark(-1);
}
