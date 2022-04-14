// CommentHeaderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "replall.h"
#include "CommentHeaderDlg.h"
#include <locale.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define WM_EDITCTRL_KEYUP (WM_USER+1)

#define PARAMTER_INPUT     1
#define PARAMTER_OUTPUT    2
#define PARAMTER_TYPE_LINK 4
#define PARAMTER_OPTIONAL  8

#define LIST_DESCRIPTION   0
#define LIST_PARAM_NAME    1
#define LIST_TYPE_NAME     2
/////////////////////////////////////////////////////////////////////////////
// CCommentHeaderDlg dialog

CCommentHeaderDlg::CCommentHeaderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCommentHeaderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCommentHeaderDlg)
	m_strClass = _T("");
	m_strFunction = _T("");
	m_strResultType = _T("");
	m_strFunctionDscr = _T("");
	m_strResultTypeDscr = _T("");
	m_strAuthor = _T("");
	m_bTitle = FALSE;
	m_bAutoLinks = FALSE;
	m_bSourceCode = FALSE;
	m_strKeyWords = _T("");
	m_strTopicOrder = _T("0");
	m_strGroup = _T("");
	//}}AFX_DATA_INIT

   m_nSelection = -1;
   m_bAuthorEdited = false;
   m_nCurrAuthor  = -1;

   m_bGroupEdited = false;
   m_nCurrGroup   = -1;

   m_nParamLength = 0;
}

/****************************************************************************
 Klasse:    CCommentHeaderDlg
 Funktion: DoDataExchange
 Zweck:     
 Parameter: 1
  pDX: (E):    (CDataExchange*)
 Rückgabewert:   (void)
 erstellt am : 17. Mai 2002
****************************************************************************/
void CCommentHeaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCommentHeaderDlg)
	DDX_Control(pDX, IDC_COMBO_GROUP, m_cGroup);
	DDX_Control(pDX, IDC_COMBO_AUTHORS, m_cAuthors);
	DDX_Text(pDX, IDC_EDT_CLASS, m_strClass);
	DDX_Text(pDX, IDC_EDT_FUNCTION, m_strFunction);
	DDX_Text(pDX, IDC_EDT_RESULT_TYPE, m_strResultType);
	DDX_Control(pDX, IDC_PARAM_LIST, m_cParams);
	DDX_Text(pDX, IDC_EDT_FUNCTION_DESCRIPTION, m_strFunctionDscr);
	DDX_Text(pDX, IDC_EDT_RESULT_TYPE_DESCRIPTION, m_strResultTypeDscr);
	DDX_Check(pDX, IDC_CK_GERMAN, m_bGerman);
	DDX_CBString(pDX, IDC_COMBO_AUTHORS, m_strAuthor);
	DDX_Check(pDX, IDC_CK_INSERT_DATE, m_bInsertDate);
	DDX_Check(pDX, IDC_CK_INSERT_TITLE, m_bTitle);
	DDX_Check(pDX, IDC_CK_INSERT_AUTO_LINKS, m_bAutoLinks);
	DDX_Check(pDX, IDC_CK_INSERT_SOURCE_CODE, m_bSourceCode);
	DDX_Text(pDX, IDC_EDT_KEY_WORDS, m_strKeyWords);
	DDX_Text(pDX, IDC_EDT_TOPICORDER, m_strTopicOrder);
	DDX_CBString(pDX, IDC_COMBO_GROUP, m_strGroup);
	//}}AFX_DATA_MAP
/*
   DDX_Check(pDX, IDC_CK_INPUT_PARAM, m_bInput);
	DDX_Check(pDX, IDC_CK_OUTPUT_PARAM, m_bOutput);
	DDX_Check(pDX, IDC_CK_TYPE_LINK, m_bTypeLink);
	DDX_Check(pDX, IDC_CK_OPTIONAL_PARAM, m_bOptional);
*/
}


BEGIN_MESSAGE_MAP(CCommentHeaderDlg, CDialog)
	//{{AFX_MSG_MAP(CCommentHeaderDlg)
	ON_NOTIFY(NM_CLICK, IDC_PARAM_LIST, OnClickParamList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_PARAM_LIST, OnEndlabeleditParamList)
	ON_CBN_KILLFOCUS(IDC_COMBO_AUTHORS, OnKillfocusComboAuthors)
	ON_BN_CLICKED(IDC_BTN_SAVE_SETTINGS, OnBtnSaveSettings)
	ON_CBN_EDITCHANGE(IDC_COMBO_AUTHORS, OnEditchangeComboAuthors)
	ON_CBN_SELCHANGE(IDC_COMBO_AUTHORS, OnSelchangeComboAuthors)
	ON_BN_CLICKED(IDC_BTN_DEL_AUTHOR, OnBtnDelAuthor)
	ON_BN_CLICKED(IDC_CK_GERMAN, OnCkGerman)
	ON_NOTIFY(NM_DBLCLK, IDC_PARAM_LIST, OnDblclkParamList)
	ON_BN_CLICKED(IDC_CK_INSERT_DATE, OnCkInsertDate)
	ON_BN_CLICKED(IDC_CK_INSERT_TITLE, OnCkInsertTitle)
	ON_CBN_KILLFOCUS(IDC_COMBO_GROUP, OnKillfocusComboGroup)
	ON_CBN_SELCHANGE(IDC_COMBO_GROUP, OnSelchangeComboGroup)
	ON_CBN_EDITCHANGE(IDC_COMBO_GROUP, OnEditchangeComboGroup)
	ON_BN_CLICKED(IDC_BTN_DEL_GROUP, OnBtnDelGroup)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_PARAM_LIST, OnBeginlabeleditParamList)
	ON_MESSAGE(WM_EDITCTRL_KEYUP, OnEditCtrlKeyUp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/*
	ON_EN_KILLFOCUS(IDC_EDT_CLASS, OnKillfocusEdtClass)
	ON_EN_KILLFOCUS(IDC_EDT_FUNCTION, OnKillfocusEdtFunction)
	ON_EN_KILLFOCUS(IDC_EDT_FUNCTION_DESCRIPTION, OnKillfocusEdtFunctionDescription)
	ON_EN_KILLFOCUS(IDC_EDT_RESULT_TYPE, OnKillfocusEdtResultType)
	ON_EN_KILLFOCUS(IDC_EDT_RESULT_TYPE_DESCRIPTION, OnKillfocusEdtResultTypeDescription)
*/

/////////////////////////////////////////////////////////////////////////////
// CCommentHeaderDlg message handlers

/*********************************************************************************************
 Klasse  : CCommentHeaderDlg
 Funktion: OnInitDialog
 Zweck   : Initialisiert die Dialogfelder und parsed den Funktionsstring, um die Parameter,
           den Klassennamen und den Rückgabewert zu trennen.

 Parameter: Keine

 Rückgabewert: Dialogfelder initialisiert (true, false) (BOOL)
 Author: Rolf Kary-Ehlers
 erstellt am: 28. Mai 2002
 <TITLE OnInitDialog>
*********************************************************************************************/
BOOL CCommentHeaderDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if (!m_strText.IsEmpty())
   {
      int j, nItem    = m_strText.Find(_T("****/"));
		if (nItem != -1)
		{
			nItem += 7;
			m_strOldComment = m_strText.Left(nItem);
			m_strText       = m_strText.Mid(nItem);
		}
      int nBraceOpen  = m_strText.Find(_T("("));
      int nBraceClose = m_strText.Find(_T(")"));
      int nFirstSpace = nBraceOpen;
      int nClassSep   = m_strText.Find(_T("::"));
      int nOperator   = m_strText.Find(_T("operator"));
      int nComma      = -1;
      LPCTSTR str     = m_strText;
		nItem = 0;
      if ((nBraceClose != -1) && (nBraceClose != -1))
      {
         CWinApp*pApp = AfxGetApp();
         CString string;
         CString strSpaceOrTAB = _T(" \t");
         CString strParam;
         CString strType;
         CString strComment;
         if (nOperator == -1) nFirstSpace = nBraceOpen-1;
         else                 nFirstSpace = nOperator;
         for (; nFirstSpace>=0; nFirstSpace--)
         {
            if ((str[nFirstSpace] == ' ') || (str[nFirstSpace] == '\t')) break;
         }
         if (nFirstSpace != 0)
         {
            if (nFirstSpace < nBraceOpen)
            {
               m_strResultType = m_strText.Left(nFirstSpace);
            }
            else
            {
               nFirstSpace = -1;
            }
            if (nClassSep != -1)
            {
               if (nClassSep>nFirstSpace)
               {
                  m_strClass = m_strText.Mid(nFirstSpace+1, nClassSep-(nFirstSpace+1));
               }
            }
            else
            {
               nClassSep = m_strText.Find(_T(" "), nFirstSpace+1);
               if (nClassSep==-1)                              // Kein Space gefunden
               {                                               // Tabulator suchen
                  nClassSep = m_strText.Find(_T("\t"), nFirstSpace+1);
               }
               if (nClassSep > nBraceOpen) nClassSep = nFirstSpace;
               nClassSep--;
            }
            m_strFunction = m_strText.Mid(nClassSep+2, nBraceOpen-(nClassSep+2));
         }
         else 
         {
            AfxMessageBox(IDS_WRONG_FORMAT);
            OnCancel();
            return false;
         }

         m_cParams.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT, LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
         string.LoadString(IDS_DESCRIPTION);
         m_cParams.InsertColumn(0, string, LVCFMT_LEFT, 300);
         string.LoadString(IDS_PARAM);
         m_cParams.InsertColumn(1, string, LVCFMT_LEFT, 100);
         string.LoadString(IDS_TYPE);
         m_cParams.InsertColumn(2, string, LVCFMT_LEFT, 100);
         int i, nOrder[3] = {2,1,0};
         m_cParams.SetColumnOrderArray(3, nOrder);

         CString strFormat, strEntry;
         strFormat.LoadString(IDS_AUTHOR);
         for (i=1; ; i++)
         {
            strEntry.Format(strFormat, i);
            string = pApp->GetProfileString(COMMENT_AUTHORS, strEntry);	
            if (string.IsEmpty()) break;
            if (string == _T("--")) break;
            m_cAuthors.AddString(string);
         }
         m_nCurrAuthor = m_cAuthors.SetCurSel(0);

         strFormat.LoadString(IDS_GROUPX);
         for (i=1; ; i++)
         {
            strEntry.Format(strFormat, i);
            string = pApp->GetProfileString(COMMENT_GROUPS, strEntry);	
            if (string.IsEmpty()) break;
            if (string == _T("--")) break;
            m_cGroup.AddString(string);
         }
         m_nCurrGroup = m_cGroup.SetCurSel(0);

         m_bGerman     = pApp->GetProfileInt(DEFAULT_SECTION, GERMAN_COMMENT, TRUE);
	      m_bInsertDate = pApp->GetProfileInt(DEFAULT_SECTION, INSERT_DATE   , FALSE);
	      m_bTitle      = pApp->GetProfileInt(DEFAULT_SECTION, TITLE_FNC     , FALSE);
         InitToolTips();
			
			if (m_strOldComment.Find(_T("Description")) != -1)
			{
				m_bGerman = FALSE;
			}
			else if (m_strOldComment.Find(_T("Zweck")) != -1)
			{
				m_bGerman = TRUE;
			}

			{ // Description Find Block
				int nStart, nEnd;
				if (m_bGerman)
				{
					nStart = m_strOldComment.Find(_T("Zweck"));
				}
				else
				{
					nStart = m_strOldComment.Find(_T("Description"));
				}
				if (nStart != -1)
				{
					nStart = m_strOldComment.Find(_T(": "), nStart);
					if (nStart != -1)
					{
						nEnd = m_strOldComment.Find(_T("\n\r\n"), nStart);
						if (nEnd != -1)
						{
							nStart += 2;
							m_strFunctionDscr = m_strOldComment.Mid(nStart, nEnd-nStart-1);
						}
					}
				}
				if (m_bGerman)
				{
					nStart = m_strOldComment.Find(_T("Rückgabewert"));
				}
				else
				{
					nStart = m_strOldComment.Find(_T("Result type"));
				}
				if (nStart != -1)
				{
					nStart = m_strOldComment.Find(_T(": "), nStart);
					if (nStart != -1)
					{
						nEnd = m_strOldComment.Find(m_strResultType, nStart);
						if (nEnd != -1)
						{
							nStart += 2;
							nEnd--;
							m_strResultTypeDscr = m_strOldComment.Mid(nStart, nEnd-nStart-1);
						}
					}
				}
			}

         char *ptr, *ptrcb, *ptrce, cOld;
         bool bParam = false;
         int  nComment = 0;
         for (i=nBraceOpen+1; i<nBraceClose; i++)
         {
            if (nComment)                                      // Kommentar
            {
               if (((nComment == 1) && (str[i] == '*') && (str[i+1] == '/')) ||
                   ((nComment == 2) && (str[i] == '\n')))
               {
                  nComment = 0;
                  if ((nComma != -1) && strParam.IsEmpty())
                  {
                     strParam = strComment;
                     InsertToList(strParam, strType, strComment, nItem);
                     nComma = -1;
                  }
               }
               else
               {
                  CString strAdd(str[i]);
                  strComment += strAdd;
               }
            }
            else if (isalpha(str[i]))                          // Parameter und Typen sind gültige Zeichen
            {
               if (nComma != -1)                                  
               {
                  ptr = (char*)&str[nComma];                   // Parameter geht bis zum Komma
                  int nOff = (ptr - &str[i]);                  // Länge ermitteln
                  cOld = ptr[0];
                  ptr[0] = 0;                                  // Kommentar vor dem Komma suchen
                  ptrcb = strstr(&str[i], "/*");
                  ptrce = strstr(&str[i], "*/");
                  ptr[0] = cOld;
                  if (ptrcb && ptrce)                          // Kommentar einfügen
                  {
                     cOld   = ptrce[0];
                     ptrce[0] = 0;
                     strComment = _T(&ptrcb[2]);
                     ptrce[0] = cOld;
                     ptr =ptrcb;
                  }
                  cOld   = ptr[0];                             // Parameter einfügen
                  ptr[0] = 0;
                  strParam = _T(&str[i]);
                  ptr[0] = cOld;
                  i += nOff;                                   // weiterspringen
               }
               else
               {
                  InsertToList(strParam, strType, strComment, nItem);
                  nComma = m_strText.Find(_T(","), i);         // Komma trennt die Parameter
                  if      (nComma == -1)                       // kein Komma: letzter Parameter
                  {
                     nComma = m_strText.Find(_T("//"), i);
                     if (nComma != -1)                         // Kommentar dahinter ?
                     {
                        j = m_strText.Find(_T("\n"), nComma);  // Zeilenende suchen
                        if (j!=-1)
                        {
                           j--;
                           cOld = str[j];
                           ((char*)str)[j] = 0;
                           strComment = _T(&str[nComma+2]);    // Kommentar geht nur bis Zeilenende
                           ((char*)str)[j] = cOld;
                           for (j=nComma; j>i; j--)            // dahinter:
                           {
                              if (isalpha(str[j]))             // nächstes gültiges Zeichen suchen
                              {
                                 nComma = j+1;
                                 break;
                              }
                           }
                        }
                        nBraceClose = nComma;
                     }
                     else                                      // kein Kommentar
                     {
                        nComma = nBraceClose;                  // dann folgt nur nach die Klammer
                     }
                  }
                  else if (nComma > nBraceClose)               // die Klammer ist immer am Ende
                  {
                     nComma = nBraceClose;
                  }

                  ptr = NULL;                                  // Parameter:
                  for (j=nComma-2; j>i; j--)                   // Rückwärts suchen, da Parameter
                  {
                     if ((str[j] == ' ')||(str[j] == '\t')||   // keine Leerzeichen oder Tabulatoren enthalten
								((str[j] == '*') && isalpha(str[j-1]) && isalpha(str[j+1])))
                     {
                        if (nComment)
                           continue;
								if (str[j] == '*') j++;
                        ptr = (char*)&str[j];
                        break;
                     }
                     else if ((str[j] == '*') && (str[j+1] == '/'))
                     {
                        nComment = 1;// außer in Kommentaren
                     }
                     else if ((str[j] == '/') && (str[j+1] == '*'))
                     {
                        nComment = -1;
                     }
                     else if ((nComment == -1) && isalpha(str[i]))
                     {
                        nComment = 0;
                     }
                  }
                  if (!ptr) ptr = (char*)&str[nBraceClose];    // nix gefunden, dann Schlußklammer nehmen

                  int nC = ptr-str;                            // Breite des Parameters
                  if (nC > nComma)                             // zu klein
                  {
                     cOld = str[nComma];                       // Kein Parameter deklariert
                     ((char*)str)[nComma] = 0;                 // Typdeklaration geht bis zum Komma
                  }
                  else
                  {
                     cOld   = ptr[0];                          // Typdeklaration geht bis zu Parameter
                     ptr[0] = 0;
                  }
                  strType = _T(&str[i]);                       // Typ speichern 

                  if (nC > nComma) ((char*)str)[nComma] = cOld;
                  else             ptr[0]               = cOld;

                  i += (ptr - &str[i]);                        // weiterspringen
               }
               if (i>=nComma)                                  // Komma zurücksetzen
               {
                  nComma = -1;
               }
            }
            else if ((str[i] == '/') && (str[i+1] == '/'))     // Kommentar anfang
            {
               nComment = 2;                                   // Typ 1 wird durch \n beendet
               i++;
            }
            else if ((str[i] == '/') && (str[i+1] == '*'))     // Kommentar anfang
            {
               nComment = 1;                                   // Typ 2 wird durch */ beendet
               i++;
            }
            else if ((nComma != -1) && ((str[i] == '*')||(str[i] == '&')))// Parameter Referenzoperatoren
            {
               CString strAdd(str[i]);
               strType += strAdd;                                 // einfügen
            }
         }
         InsertToList(strParam, strType, strComment, nItem);
         UpdateData(false);
         GetDlgItem(IDC_EDT_FUNCTION_DESCRIPTION)->SetFocus();
      }
      else 
      {
         AfxMessageBox(IDS_WRONG_FORMAT);
         OnCancel();
         return false;
      }
   }	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCommentHeaderDlg::InitToolTips()
{
	// Alle wollen sie ToolTips haben
	if (m_ToolTip.Create(this) )
   {
		m_ToolTip.Activate(true);
		// Lasse den ToolTip die Zeilenumbrueche beachten
		m_ToolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, SHRT_MAX);

		// Wenn man will, kann man auch noch die Zeiten veraendern
		// Verweildauer des ToolTips (Maximale Zeit=SHRT_MAX)
		m_ToolTip.SendMessage(TTM_SETDELAYTIME, TTDT_AUTOPOP, SHRT_MAX);
		// Wie schnell soll er erscheinen, wenn der Cursor ueber einem Tool steht
		m_ToolTip.SendMessage(TTM_SETDELAYTIME, TTDT_INITIAL, 200);
		// Mindest-Verzoegerungszeit bis zum naechsten Erscheinen
		m_ToolTip.SendMessage(TTM_SETDELAYTIME, TTDT_RESHOW, 200);

		// Alle Child-Windows durchklappern, um deren ToolTips einzulesen
		UINT uID = 0;
		CString sTip;
		int iIndex = 0;
		CWnd* pItem = GetWindow(GW_CHILD);
		// Wir wollen keine Endlosschleife
		while ( pItem && IsChild(pItem) )
      {
			// die ToolTip-Texte aus der Resource laden,
			uID = pItem->GetDlgCtrlID();
			// falls Text vorhanden ins Array schreiben und das Tool anhaengen
			if (sTip.LoadString(uID) && !sTip.IsEmpty())
         {
				iIndex = m_sTTArray.Add(sTip);
				m_ToolTip.AddTool( pItem, m_sTTArray.GetAt(iIndex));
			}
			// Haben wir noch ein Child?
			pItem = pItem->GetNextWindow();
		}
		m_ToolTip.Activate(true);
	}
}

/*********************************************************************************************
 Klasse  : CCommentHeaderDlg
 Funktion: InsertToList
 Zweck   : Fügt die Parameter in die Liste ein und löscht die Strings, damit sie wieder neu
           initialisiert werden können.

 Parameter:  
  strParam  : (E): Parametername  (CString&)
  strType   : (E): Datentyp  (CString&)
  strComment: (E): Beschreibungstext  (CString&)
  nItem     : (E): aktueller Listenindex  (int&)

 Rückgabewert:  (void)
 Author: Rolf Kary-Ehlers
 erstellt am: 28. Mai 2002
 <TITLE InsertToList>
 <AUTOLINK ON>
 <IMPLEMENTATION ON>
*********************************************************************************************/
void CCommentHeaderDlg::InsertToList(CString &strParam, CString &strType, CString &strComment, int &nItem)
{
   if (!strParam.IsEmpty() || !strType.IsEmpty())
   {
      DWORD dwData = 0;
      int   i, j;
      CString strParamWoS;
		i = m_strOldComment.Find(strParam);			// Parameter
		if (i!=-1)
		{
			for (;;)
			{
				j = m_strOldComment.Find(strType, i);	// und Typ
				if (j == -1) break;
				int nNL = m_strOldComment.Find(_T("\n"), i+strParam.GetLength());
				if (j > nNL)
				{
					i = m_strOldComment.Find(strParam, nNL+1);
					if (i == -1)
					{
						j = -1;
						break;
					}
				}
				else
				{
					break;
				}
			}
			if (j!=-1)
			{
				if (m_strOldComment.GetAt(i-1) == '[') dwData |= PARAMTER_OPTIONAL; 

				int k = m_strOldComment.Find(_T("):"), i);
				if (k!=-1)
				{
					i = k+3;
					switch(m_strOldComment.GetAt(k-1))
					{
						case 'A': dwData |= PARAMTER_OUTPUT; break;
						case 'E': dwData |= PARAMTER_INPUT; break;
					}
					switch(m_strOldComment.GetAt(k-2))
					{
						case 'A': dwData |= PARAMTER_OUTPUT; break;
						case 'E': dwData |= PARAMTER_INPUT;  break;
					}
				}
				
				strComment = m_strOldComment.Mid(i, j-i);
				i = strComment.ReverseFind('(');
				if (i != -1)
				{
					j = strComment.Find(_T("LINK"), i);
					if (j!=-1)
					{
						dwData |= PARAMTER_TYPE_LINK; 
					}
					strComment = strComment.Left(i);
				}
			}
		}
      if (strComment.IsEmpty())
      {
			strComment = _T(" ");
      }
      else if (strComment.Find("=") != -1)
      {
         dwData    |= PARAMTER_OPTIONAL;
         strParamWoS.LoadString(m_bGerman ? IDS_DEFAULT_VALUE_GER : IDS_DEFAULT_VALUE_ENG);
         strComment = strParamWoS + strComment;
      }
      strParamWoS.GetBufferSetLength(strParam.GetLength());
      for (i=0, j=0; i<strParam.GetLength(); i++)
      {
         TCHAR c = strParam.GetAt(i);
         if (isalnum(c))
         {
            strParamWoS.SetAt(j++, c);
         }
      }
		if (dwData & (PARAMTER_OUTPUT|PARAMTER_INPUT) == 0)
		{
			dwData |= PARAMTER_INPUT;
		}
      strParamWoS.ReleaseBuffer(j);
      m_cParams.InsertItem(nItem, strComment);
      m_cParams.SetItemText(nItem, LIST_PARAM_NAME, strParamWoS);
      m_cParams.SetItemText(nItem, LIST_TYPE_NAME, strType);
      m_cParams.SetItemData(nItem++, dwData);
      int nLength = strParamWoS.GetLength();
      if (dwData & PARAMTER_OPTIONAL) nLength += 2;
      if (nLength > m_nParamLength) m_nParamLength = nLength;
      strParam.Empty();
      strType.Empty();
      strComment.Empty();
   }
} 

/*********************************************************************************************
 Klasse  : CCommentHeaderDlg
 Funktion: OnClickParamList
 Zweck   : Wertet die Nachricht LVN_CLICKLIST aus, um die Parameter der Liste zu setzen
           bzw. geänderte Parameter zu speichern. Zu diesen Parametern gehören die Input- 
           und Outputwerte.

 Parameter:  
  pNMHDR : (E): Notifyheader  (NMHDR*)
  pResult: (E): Rückgabewert  (LRESULT*)

 Rückgabewert:  (void)
 Author: Rolf Kary-Ehlers
 erstellt am: 28. Mai 2002
 <TITLE OnClickParamList>
*********************************************************************************************/
void CCommentHeaderDlg::OnClickParamList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   LVHITTESTINFO lvHTI;

   GetCursorPos(&lvHTI.pt);
   m_cParams.ScreenToClient(&lvHTI.pt);

   m_cParams.SendMessage(LVM_SUBITEMHITTEST, 0, (LPARAM)&lvHTI);
   if (m_nSelection != lvHTI.iItem)
   {
      CDataExchange dx(this, true);
      DWORD dwData = 0;
      BOOL bInput, bOutput, bTypeLink, bOptional;
      if (m_nSelection != -1)
      {
	      DDX_Check(&dx, IDC_CK_INPUT_PARAM   , bInput);
	      DDX_Check(&dx, IDC_CK_OUTPUT_PARAM  , bOutput);
         DDX_Check(&dx, IDC_CK_TYPE_LINK     , bTypeLink);
      	DDX_Check(&dx, IDC_CK_OPTIONAL_PARAM, bOptional);
         if (bInput)    dwData |= PARAMTER_INPUT;
         if (bOutput)   dwData |= PARAMTER_OUTPUT;
         if (bTypeLink) dwData |= PARAMTER_TYPE_LINK;
         if (bOptional) dwData |= PARAMTER_OPTIONAL;
         m_cParams.SetItemData(m_nSelection, dwData);
      }
      if (lvHTI.iItem != -1)
      {
         dwData = m_cParams.GetItemData(lvHTI.iItem);
         if (dwData&PARAMTER_INPUT)     bInput    = true;
         else                           bInput    = false;
         if (dwData&PARAMTER_OUTPUT)    bOutput   = true;
         else                           bOutput   = false;
         if (dwData&PARAMTER_TYPE_LINK) bTypeLink = true;
         else                           bTypeLink = false;
         if (dwData&PARAMTER_OPTIONAL)  bOptional = true;
         else                           bOptional = false;
         dx.m_bSaveAndValidate   = false;
	      DDX_Check(&dx, IDC_CK_INPUT_PARAM   , bInput);
	      DDX_Check(&dx, IDC_CK_OUTPUT_PARAM  , bOutput);
         DDX_Check(&dx, IDC_CK_TYPE_LINK     , bTypeLink);
      	DDX_Check(&dx, IDC_CK_OPTIONAL_PARAM, bOptional);
         GetDlgItem(IDC_CK_INPUT_PARAM   )->EnableWindow(true);
         GetDlgItem(IDC_CK_OUTPUT_PARAM  )->EnableWindow(true);
         GetDlgItem(IDC_CK_TYPE_LINK     )->EnableWindow(true);
         GetDlgItem(IDC_CK_OPTIONAL_PARAM)->EnableWindow(true);
      }
      else
      {
         GetDlgItem(IDC_CK_INPUT_PARAM   )->EnableWindow(false);
         GetDlgItem(IDC_CK_OUTPUT_PARAM  )->EnableWindow(false);
         GetDlgItem(IDC_CK_TYPE_LINK     )->EnableWindow(false);
         GetDlgItem(IDC_CK_OPTIONAL_PARAM)->EnableWindow(false);
      }
   }
   m_nSelection = lvHTI.iItem;
	*pResult = 0;
}

/*********************************************************************************************
 Klasse  : 
 Funktion: ReverseFind
 Zweck   : Sucht innerhalb eines Bereiches rückwärts in einem String und liefert den Index
           der Strings

 Parameter:  
  str      : (E): Stringobject  (CString&)
  strFind  : (E): Suchstring  (LPCTSTR)
  nStart=-1: (E): Startwert: (nStart > nStop)  (int)
  nStop=0  : (E): Stopwert  (int)

 Rückgabewert: -1 bei Fehler sonst den Index (int)
 Author: Rolf Kary-Ehlers
 erstellt am: 28. Mai 2002
 <TITLE ReverseFind>
*********************************************************************************************/
int ReverseFind(CString& str, LPCTSTR strFind, int nStart=-1, int nStop=0)
{
   if (nStart==-1) nStart = str.GetLength();
   if (str.GetLength() >= nStart)
   {
      LPCTSTR pstr    = str;
      int     i, nLen = strlen(strFind);
      if (nLen <= 0) return -1;
      for (i=nStart-1-nLen; i>=nStop; i--)
      {
         if (strncmp(&pstr[i], strFind, nLen) == 0)
            return i;
      }
   }
   return -1;
}

/*********************************************************************************************
 Klasse  : CCommentHeaderDlg
 Funktion: OnOK
 Zweck   : Formatiert den Kommentartext und fügt ihn ein
 Rückgabewert:  (void)
 erstellt am: 28. Mai 2002
 <TITLE OnOK>
*********************************************************************************************/
void CCommentHeaderDlg::OnOK() 
{
   if (::GetFocus() == m_cParams.m_hWnd)
   {
      int nSel = m_cParams.GetSelectionMark();
      if (nSel != -1) m_cParams.EditLabel(nSel);
      return;
   }

   CDialog::OnOK();
   CString strFormat, m_strAuthor, strText, strParam = _T(" ");
   int i, nSect, nLen, nStop = 0, nStart, nP = m_cParams.GetItemCount();
   OnClickParamList(NULL, (LRESULT*)&i);
   strFormat.LoadString(m_bGerman ? IDS_HEADER_FORMAT_TOP2 : IDS_HEADER_FORMAT_TOP);
   nLen    = strFormat.Find("\n");
   nStart  = strFormat.Find(": ", nLen+1);
   nStart -= nLen;
   if (m_strFunctionDscr.GetLength() > (nLen-nStart))
   {
      do 
      {
         nSect = m_strFunctionDscr.Find("\n", nStop);
         if (nSect > (nStop+nLen-nStart)) nSect = -1;
         if (nSect != -1)
         {
            for (i=0; i<=nStart; i++)
               m_strFunctionDscr.Insert(nSect+1, ' ');
            nStop = nSect + nStart;
         }
         else 
         {
            nSect = ReverseFind(m_strFunctionDscr, " ", nStop+nLen-nStart, nStop);
            if (nSect != -1)
            {
               m_strFunctionDscr.Insert(nSect, '\n');
               for (i=0; i<nStart; i++)
                  m_strFunctionDscr.Insert(nSect+1, ' ');
               nStop = nSect + nStart;
            }
         }
      } while(m_strFunctionDscr.GetLength() > (nStop+nLen));
   }
   if (nP == 0) strParam.LoadString(m_bGerman ? IDS_NONE2 : IDS_NONE);
   strText.Format(strFormat, m_strClass, m_strFunction, m_strFunctionDscr, strParam);
   m_strText = strText;

   strFormat.LoadString(m_bGerman ? IDS_HEADER_FORMAT_MIDDLE2 : IDS_HEADER_FORMAT_MIDDLE);
   int nPos = strFormat.Find("10");                            // Parameter einfügen
   if (nPos)
   {
      strText.Format("%02d", m_nParamLength);                  // Länge des Strings ausrichten
      if (strText[0] == '0') strFormat.SetAt(nPos  , ' ');
      else                   strFormat.SetAt(nPos  , strText[0]);
      strFormat.SetAt(nPos+1, strText[1]);
   }
   for (i=0; i<nP; i++)
   {
      DWORD dwData = m_cParams.GetItemData(i);
      CString strTypeLink, strParamName;
      if (dwData&PARAMTER_TYPE_LINK)
      {
         CString strType = m_cParams.GetItemText(i, LIST_TYPE_NAME);
         CString strReference;
         int nReference = strType.FindOneOf("&*");
         if (nReference != -1)
         {
            strReference = strType.Left(nReference);
            nReference   = strReference.Find("const");
            if (nReference != -1)
            {
               strReference = strReference.Right(strReference.GetLength()-(nReference+6));
            }
         }
         else                  strReference = strType;
         strTypeLink.Format("<LINK %s, %s>", strReference, strType);
      }
      else
      {
         strTypeLink = m_cParams.GetItemText(i, LIST_TYPE_NAME);
      }
      
      if (dwData&PARAMTER_OPTIONAL)
      {
         strParamName = "[" + m_cParams.GetItemText(i, LIST_PARAM_NAME) + "]";
      }
      else
      {
         strParamName = m_cParams.GetItemText(i, LIST_PARAM_NAME);
      }
      strText.Format(strFormat,
                     strParamName,                              // Parameter
                     (dwData&PARAMTER_INPUT ) ? "E" : "",       // Input
                     (dwData&PARAMTER_OUTPUT) ? "A" : "",       // Output
                     m_cParams.GetItemText(i, LIST_DESCRIPTION),// Description
                     strTypeLink);                              // Type(+Link)
      m_strText += strText;
   }
   
   strFormat.LoadString(m_bGerman ? IDS_HEADER_FORMAT_RESULT2 : IDS_HEADER_FORMAT_RESULT);
   strText.Format(strFormat, m_strResultTypeDscr, m_strResultType);
   m_strText += strText;
   
   if (m_nCurrAuthor>0)                                        // Author einfügen
   {
      m_cAuthors.GetWindowText(m_strAuthor);
      if (!m_strAuthor.IsEmpty() && (m_strAuthor != _T("--")))
      {
         strText.Format(IDS_HEADER_FORMAT_AUTHOR, m_strAuthor);
         m_strText += strText;
      }
   }

   if (m_bInsertDate)                                          // Datum einfügen
   {
      CTime time = CTime::GetCurrentTime();
      if (m_bGerman)
      {
         setlocale(LC_TIME, "German");
         strText = time.Format(IDS_DATE_GERMAN);
      }
      else
      {
         setlocale(LC_TIME, "English");
         strText = time.Format(IDS_DATE_ENGLISH);
      }
      m_strText += strText;
   }
                                                               // Doc-O-matic spezifisch
   if (m_bTitle)                                               // Funktionsname als Titel in der Hilfe
   {
      strText.Format(IDS_TITLE, m_strFunction);
      m_strText += strText;
   }
   if (m_bAutoLinks)                                           // Funktionsnamen als Auto Link überall in der Hilfe
   {
      strText.LoadString(IDS_AUTOLINK);
      m_strText += strText;
   }
   if (m_bSourceCode)                                          // Sourc Code als Link einfügen
   {
      strText.LoadString(IDS_SOURCE_CODE);
      m_strText += strText;
   }
   if (m_nCurrGroup>0)                                         // Gruppe einfügen
   {
      m_cGroup.GetWindowText(m_strGroup);
      if (!m_strGroup.IsEmpty() && (m_strGroup != _T("--")))
      {
         strText.Format(IDS_HEADER_FORMAT_GROUP, m_strGroup);
         m_strText += strText;
         strText.Format(IDS_TOPICORDER, m_strTopicOrder);
         m_strText += strText;
      }
   }
   if (!m_strKeyWords.IsEmpty())                               // Suchbegriffe
   {
      strText.Format(IDS_KEYWORDS, m_strKeyWords);
      m_strText += strText;
   }

   strText.LoadString(IDS_HEADER_FORMAT_BOTTOM);
   m_strText += strText;
}

/*
void CCommentHeaderDlg::OnKillfocusEdtClass() 
{
   CDataExchange dx(this, true);
	DDX_Text(&dx, IDC_EDT_CLASS, m_strClass);
}

void CCommentHeaderDlg::OnKillfocusEdtFunction() 
{
   CDataExchange dx(this, true);
	DDX_Text(&dx, IDC_EDT_FUNCTION, m_strFunction);
}

void CCommentHeaderDlg::OnKillfocusEdtFunctionDescription() 
{
   CDataExchange dx(this, true);
	DDX_Text(&dx, IDC_EDT_FUNCTION_DESCRIPTION, m_strFunctionDscr);
}

void CCommentHeaderDlg::OnKillfocusEdtResultType() 
{
   CDataExchange dx(this, true);
	DDX_Text(&dx, IDC_EDT_RESULT_TYPE, m_strResultType);
}

void CCommentHeaderDlg::OnKillfocusEdtResultTypeDescription() 
{
   CDataExchange dx(this, true);
	DDX_Text(&dx, IDC_EDT_RESULT_TYPE_DESCRIPTION, m_strResultTypeDscr);
}
*/

void CCommentHeaderDlg::OnKillfocusComboAuthors() 
{
   if (m_bAuthorEdited)
   {
      CString string;
      m_cAuthors.GetWindowText(string);
      if (string != _T("--"))
      {
         if (m_nCurrAuthor <= 0)
         {
            m_cAuthors.AddString(string);
            m_nCurrAuthor = m_cAuthors.SetCurSel(m_cAuthors.GetCount()-1);
         }
         else
         {
            m_cAuthors.InsertString(m_nCurrAuthor, string);
            m_cAuthors.DeleteString(m_nCurrAuthor+1);
            m_nCurrAuthor = m_cAuthors.SetCurSel(m_nCurrAuthor);
         }
      }
      m_bAuthorEdited = false;
   }
}

void CCommentHeaderDlg::OnSelchangeComboAuthors()
{
	m_nCurrAuthor = m_cAuthors.GetCurSel();
   GetDlgItem(IDC_BTN_DEL_AUTHOR)->EnableWindow((m_nCurrAuthor>0) ? true : false);
}

void CCommentHeaderDlg::OnBtnDelAuthor()
{
   if (m_nCurrAuthor>0)
   {
      m_cAuthors.DeleteString(m_nCurrAuthor);
      m_nCurrAuthor = m_cAuthors.SetCurSel(0);
      GetDlgItem(IDC_BTN_SAVE_SETTINGS)->EnableWindow(true);
      GetDlgItem(IDC_BTN_DEL_AUTHOR)->EnableWindow((m_nCurrAuthor>0) ? true : false);
   }
}

void CCommentHeaderDlg::OnEditchangeComboAuthors() 
{
   GetDlgItem(IDC_BTN_SAVE_SETTINGS)->EnableWindow(true);
   m_bAuthorEdited = true;
}

void CCommentHeaderDlg::OnKillfocusComboGroup() 
{
   if (m_bGroupEdited)
   {
      CString string;
      m_cGroup.GetWindowText(string);
      if (string != _T("--"))
      {
         if (m_nCurrGroup <= 0)
         {
            m_cGroup.AddString(string);
            m_nCurrGroup = m_cGroup.SetCurSel(m_cGroup.GetCount()-1);
         }
         else
         {
            m_cGroup.InsertString(m_nCurrGroup, string);
            m_cGroup.DeleteString(m_nCurrGroup+1);
            m_nCurrGroup = m_cGroup.SetCurSel(m_nCurrGroup);
         }
      }
      m_bGroupEdited = false;
   }
}

void CCommentHeaderDlg::OnEditchangeComboGroup() 
{
   GetDlgItem(IDC_BTN_SAVE_SETTINGS)->EnableWindow(true);
   m_bGroupEdited = true;
}

void CCommentHeaderDlg::OnBtnDelGroup() 
{
   if (m_nCurrGroup>0)
   {
      m_cGroup.DeleteString(m_nCurrGroup);
      m_nCurrGroup = m_cGroup.SetCurSel(0);
      GetDlgItem(IDC_BTN_SAVE_SETTINGS)->EnableWindow(true);
      GetDlgItem(IDC_BTN_DEL_GROUP)->EnableWindow((m_nCurrGroup>0) ? true : false);
   }
}

void CCommentHeaderDlg::OnSelchangeComboGroup() 
{
	m_nCurrGroup = m_cGroup.GetCurSel();
   GetDlgItem(IDC_BTN_DEL_GROUP)->EnableWindow((m_nCurrGroup>0) ? true : false);
}

void CCommentHeaderDlg::OnCkGerman() 
{
   CDataExchange dx(this, true);
	DDX_Check(&dx, IDC_CK_GERMAN, m_bGerman);
   GetDlgItem(IDC_BTN_SAVE_SETTINGS)->EnableWindow(true);
}

void CCommentHeaderDlg::OnCkInsertDate() 
{
   CDataExchange dx(this, true);
	DDX_Check(&dx, IDC_CK_INSERT_DATE, m_bInsertDate);
   GetDlgItem(IDC_BTN_SAVE_SETTINGS)->EnableWindow(true);
}

void CCommentHeaderDlg::OnCkInsertTitle() 
{
   CDataExchange dx(this, true);
	DDX_Check(&dx, IDC_CK_INSERT_TITLE, m_bTitle);
   GetDlgItem(IDC_BTN_SAVE_SETTINGS)->EnableWindow(true);
}

/*********************************************************************************************
 Klasse  : CCommentHeaderDlg
 Funktion: OnBtnSaveSettings
 Zweck   : Speichert die Einstellungen in der Registry
 Rückgabewert:  (void)
 erstellt am: 28. Mai 2002
 <TITLE OnBtnSaveSettings>
*********************************************************************************************/
void CCommentHeaderDlg::OnBtnSaveSettings() 
{
   CWinApp*pApp = AfxGetApp();
   int i, nCount = m_cAuthors.GetCount();
   CString strFormat, strEntry, strItem;
   strFormat.LoadString(IDS_AUTHOR);
   for (i=1; i<=nCount; i++)
   {
      if (i==nCount) strItem = _T("--");
      else           m_cAuthors.GetLBText(i, strItem);
      strEntry.Format(strFormat, i);
      pApp->WriteProfileString(COMMENT_AUTHORS, strEntry, strItem);
   }

   nCount = m_cGroup.GetCount();
   strFormat.LoadString(IDS_GROUPX);
   for (i=1; i<=nCount; i++)
   {
      if (i==nCount) strItem = _T("--");
      else           m_cGroup.GetLBText(i, strItem);
      strEntry.Format(strFormat, i);
      pApp->WriteProfileString(COMMENT_GROUPS, strEntry, strItem);
   }

   pApp->WriteProfileInt(DEFAULT_SECTION, GERMAN_COMMENT, m_bGerman);
   pApp->WriteProfileInt(DEFAULT_SECTION, INSERT_DATE   , m_bInsertDate);
   pApp->WriteProfileInt(DEFAULT_SECTION, TITLE_FNC     , m_bTitle);
   GetDlgItem(IDC_BTN_SAVE_SETTINGS)->EnableWindow(false);
}

void CCommentHeaderDlg::OnDblclkParamList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   int nSel = m_cParams.GetSelectionMark();
   if (nSel != -1) m_cParams.EditLabel(nSel);
   *pResult = 0;
}

void CCommentHeaderDlg::OnBeginlabeleditParamList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   long lResult = 1;
   HWND hwndEdit = (HWND) ::SendMessage(pNMHDR->hwndFrom, LVM_GETEDITCONTROL, 0, 0);
   if (hwndEdit)
   {
      if (::GetWindowLong(hwndEdit, GWL_USERDATA) == 0)
      {
         ::SetWindowLong(hwndEdit, GWL_USERDATA, ::SetWindowLong(hwndEdit, GWL_WNDPROC, (long)EditCtrlWndProc));
      }
   }
   lResult = 0;
   ::SetWindowLong(m_hWnd, DWL_MSGRESULT, lResult);
}

void CCommentHeaderDlg::OnEndlabeleditParamList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   if (pDispInfo->item.pszText)
   {
      m_cParams.SetItemText(pDispInfo->item.iItem, pDispInfo->item.iSubItem, pDispInfo->item.pszText);
   }

   HWND hwndEdit = (HWND) ::SendMessage(pNMHDR->hwndFrom, LVM_GETEDITCONTROL, 0, 0);
   if (hwndEdit)
   {
      ::SetWindowLong(hwndEdit, GWL_WNDPROC, ::GetWindowLong(hwndEdit, GWL_USERDATA));
      ::SetWindowLong(hwndEdit, GWL_USERDATA, 0);
   }
	
	*pResult = 0;
}

LRESULT CALLBACK CCommentHeaderDlg::EditCtrlWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   long lProc = ::GetWindowLong(hwnd, GWL_USERDATA);
   if (lProc == 0) return 0;

   if (uMsg == WM_KEYDOWN)
   {
      if ((wParam == VK_UP)||(wParam == VK_DOWN)||(wParam == VK_TAB))
      {
         HWND hwndP = ::GetParent(::GetParent(hwnd));
         ::PostMessage(hwndP, WM_EDITCTRL_KEYUP, wParam, lParam);
         return 0;
      }
   }  
   return CallWindowProc((WNDPROC)lProc, hwnd, uMsg, wParam, lParam);
}

/*********************************************************************************************
 Klasse  : CCommentHeaderDlg
 Funktion: OnEditCtrlKeyUp
 Zweck   : Bearbeitet die User-Message WM_EDITCTRL_KEYUP. Diese wertet die Tasten
           Pfeil hoch, runter und die TAB-Taste aus, um zwischen den Listenitems im
           Editmodus umzuschalten.

 Parameter:  
  wParam: (E):    (WPARAM)
  lParam: (E):    (LPARAM)

 Rückgabewert:  (LRESULT)
 erstellt am: 28. Mai 2002
 <TITLE OnEditCtrlKeyUp>
*********************************************************************************************/
LRESULT CCommentHeaderDlg::OnEditCtrlKeyUp(WPARAM wParam, LPARAM lParam)
{
   CWnd*pWnd  = GetDlgItem(IDC_PARAM_LIST);
   if (!pWnd) return 0;

   int nItems = pWnd->SendMessage(LVM_GETITEMCOUNT);
   int nItem  = pWnd->SendMessage(LVM_GETSELECTIONMARK);
   if (wParam == VK_UP)
   {
      nItem--;
   }
   if ((wParam == VK_DOWN) || (wParam == VK_TAB))
   {
      nItem++;
      if (nItem >= nItems) nItem = -1;
   }
   if (nItem >= 0)
   {
      pWnd->SendMessage(LVM_SETSELECTIONMARK, 0, nItem);
      pWnd->SetFocus();
      pWnd->SendMessage(LVM_EDITLABEL, nItem, 0);
   }
   return 0;
}

BOOL CCommentHeaderDlg::PreTranslateMessage(MSG* pMsg) 
{
   if (m_ToolTip.m_hWnd && IsWindow(m_ToolTip.m_hWnd))
   {
      m_ToolTip.RelayEvent(pMsg);
   }
	return CDialog::PreTranslateMessage(pMsg);
}
