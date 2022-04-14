// MSDNEditDoc.cpp : implementation of the CMSDNEditDoc class
//

#include "stdafx.h"
#include "MSDNEdit.h"

#include "MSDNEditDoc.h"
#include "MainFrm.h"
#include "LeftView.h"
#include "MSDNIntegration.h"
#include "MSDNCollection.h"
#include "MSDNLocations.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMSDNEditDoc


IMPLEMENT_DYNCREATE(CMSDNEditDoc, CDocument)

BEGIN_MESSAGE_MAP(CMSDNEditDoc, CDocument)
	//{{AFX_MSG_MAP(CMSDNEditDoc)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_FILE_IMPORT, OnFileImport)
	ON_UPDATE_COMMAND_UI(ID_FILE_IMPORT, OnUpdateFileImport)
	ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_EDIT_NEW_ITEM, OnEditNewItem)
	ON_UPDATE_COMMAND_UI(ID_EDIT_NEW_ITEM, OnUpdateEditNewItem)
	ON_COMMAND(ID_FILE_RELOAD, OnFileReload)
	ON_UPDATE_COMMAND_UI(ID_FILE_RELOAD, OnUpdateFileReload)
	ON_COMMAND(ID_VIEW_COL_FILE, OnViewColFile)
	ON_COMMAND(ID_VIEW_REG_FILE, OnViewRegFile)
	ON_COMMAND(ID_VIEW_COL_INFO, OnViewColInfo)
	ON_COMMAND(ID_VIEW_COLECTIONS, OnViewColections)
	ON_COMMAND(ID_VIEW_LOCATIONS, OnViewLocations)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LOCATIONS, OnUpdateViewLocations)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COLECTIONS, OnUpdateViewColections)
	ON_COMMAND(ID_FILE_CHANGE_DOC_INFO, OnFileChangeDocInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMSDNEditDoc construction/destruction

CMSDNEditDoc::CMSDNEditDoc()
{
   m_strXML                 =_T("<XML>");
   m_strNXML                =_T("</XML>");
   m_strFolder              =_T("<Folder>");
   m_strNFolder             =_T("</Folder>");
   m_strFolders             =_T("<Folders>");
   m_strNFolders            =_T("</Folders>");
   m_strTitle               =_T("<TitleString value=");
   m_strFolderOrder         =_T("<FolderOrder value=");
   m_strLangID              =_T("<LangId value=");
   m_strDocCompId           =_T("<DocCompId value=");
   m_strTitleLocation       =_T("<TitleLocation value=");
   m_strNewLine             =_T("\r\n");
   m_strNextCollectionId    =_T("<NextCollectionId value=");
   m_strCollections         =_T("<Collections>");
   m_strCollection          =_T("<Collection>");
   m_strColNum              =_T("<ColNum value=");
   m_strColName             =_T("<ColName value=");
   m_strNCollection         =_T("</Collection>");
   m_strNCollections        =_T("</Collections>");
   m_strLocations           =_T("<Locations>");
   m_strLocation            =_T("<Location>");
   m_strLocColNum           =_T("<LocColNum value=");
   m_strLocName             =_T("<LocName value=");
   m_strTitleString         =_T("<TitleString value=");
   m_strLocPath             =_T("<LocPath value=");
   m_strVolume              =_T("<Volume value=");
   m_strNLocations          =_T("</Locations>");
   m_strNLocation           =_T("</Location>");
   m_strDocCompilations     =_T("<DocCompilations>");
   m_strNDocCompilations    =_T("</DocCompilations>");
   m_strDocCompilation      =_T("<DocCompilation>");
   m_strNDocCompilation     =_T("</DocCompilation>");
   m_strDocCompLanguage     =_T("<DocCompLanguage value=");
   m_strIndexLocation       =_T("<IndexLocation value=");
   m_strQueryLocation       =_T("<QueryLocation value=");
   m_strLocationRef         =_T("<LocationRef value=");
   m_strDCVersion           =_T("<Version value=");
   m_strLastPromptedVersion =_T("<LastPromptedVersion value=");
   m_strTitleSampleLocation =_T("<TitleSampleLocation value=");
   m_strTitleQueryLocation  =_T("<TitleQueryLocation value=");
   m_strSupportsMerge       =_T("<SupportsMerge value=");
   m_strLocationHistory     =_T("<LocationHistory>");
   m_strNLocationHistory    =_T("</LocationHistory>");
   m_strHTMLHelpDocInfo     =_T("<HTMLHelpDocInfo>");
   m_strNHTMLHelpDocInfo    =_T("</HTMLHelpDocInfo>");
   m_strHTMLHelpCollection  =_T("<HTMLHelpCollection>");
   m_strNHTMLHelpCollection =_T("</HTMLHelpCollection>");
   m_strmasterchm           =_T("<masterchm value=");
   m_strmasterlangid        =_T("<masterlangid value=");
   m_strsamplelocation      =_T("<samplelocation value=");
   m_strcollectionnum       =_T("<collectionnum value=");
   m_strrefcount            =_T("<refcount value=");
   m_strversion             =_T("<version value=");
   m_strfindmergedchms      =_T("<findmergedchms value=");

   m_nDoctype          = 0;
   m_bImportExport     = false;
   m_nNextCollectionID = 0;
   m_nFilesImported    = 0;
   m_bCreateBackUpFile = false;
   m_strMasterChmValue = _T("");
	m_strSampLocValue   = _T("");
   m_nMasterLangId     = 0;
  	m_nCollectionNum    = 0;
  	m_nRefCount         = 0;
  	m_nVersion          = 0;
  	m_nFindMergeDchms   = 0;

}

CMSDNEditDoc::~CMSDNEditDoc()
{
   DeleteLocations();
   DeleteCollections();
}

BOOL CMSDNEditDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;


	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMSDNEditDoc serialization

void CMSDNEditDoc::Serialize(CArchive& ar)
{
   CString str;
   if (m_nDoctype == 0)
   {
      CString strFileName(ar.GetFile()->GetFileName());
      strFileName.MakeUpper();
      if      (strFileName.Find(_T(".COL")) != -1) m_nDoctype = DOC_TYPE_HTMLHELPCOLLECTION;
      else if (strFileName.Find(_T(".DAT")) != -1) m_nDoctype = DOC_TYPE_HTMLHELPDOCINFO;
   }
   if (ar.IsStoring())
	{
      if (m_nDoctype == DOC_TYPE_HTMLHELPCOLLECTION)
      {
         ar.WriteString(m_strXML+m_strNewLine);
         ar.WriteString(m_strHTMLHelpCollection+m_strNewLine);
         if (!m_bImportExport)
         {
            str.Format("%s\"%s\"/>%s", m_strmasterchm, m_strMasterChmValue, m_strNewLine);
		      ar.WriteString(str);
            str.Format("%s%d/>%s", m_strmasterlangid, m_nMasterLangId, m_strNewLine);
		      ar.WriteString(str);
            str.Format("%s\"%s\"/>%s", m_strsamplelocation, m_strSampLocValue, m_strNewLine);
		      ar.WriteString(str);
            str.Format("%s%d/>%s", m_strcollectionnum, m_nCollectionNum, m_strNewLine);
		      ar.WriteString(str);
            str.Format("%s%d/>%s", m_strrefcount, m_nRefCount, m_strNewLine);
		      ar.WriteString(str);
            str.Format("%s%d/>%s", m_strversion, m_nVersion, m_strNewLine);
		      ar.WriteString(str);
            str.Format("%s%d/>%s", m_strfindmergedchms, m_nFindMergeDchms, m_strNewLine);
		      ar.WriteString(str);
         }
         ar.WriteString(m_strFolders+m_strNewLine);
         
         if (m_bImportExport)
         {
            UpdateAllViews(NULL, HTMLHELPCOLLECTION_EXPORT, (CObject*) &ar);
         }
         else
         {
            UpdateAllViews(NULL, HTMLHELPCOLLECTION_SAVE, (CObject*) &ar);
         }
      
         ar.WriteString(m_strNFolders+m_strNewLine);
         ar.WriteString(m_strNHTMLHelpCollection+m_strNewLine);
         ar.WriteString(m_strNXML);
      }
      else if (m_nDoctype == DOC_TYPE_HTMLHELPDOCINFO)
      {
         MSDNDocCol  *pDocCol = NULL;
         MSDNDocLoc  *pDocLoc = NULL;
         int i, nCount;
         ar.WriteString(m_strXML+m_strNewLine);
         ar.WriteString(m_strHTMLHelpDocInfo+m_strNewLine);
         if (m_bImportExport)
         {
            ar.WriteString(m_strDocCompilations+m_strNewLine);
            UpdateAllViews(NULL, HTMLHELPDOCINFO_EXPORT, (CObject*) &ar);
            ar.WriteString(m_strNDocCompilations+m_strNewLine);
         }
         else
         {
            str.Format("%s%d/>%s", m_strNextCollectionId, m_nNextCollectionID, m_strNewLine);
		      ar.WriteString(str);
            ar.WriteString(m_strCollections+m_strNewLine);        // Collections
            nCount = m_DocInfoCollections.GetSize();      
            for (i=0; i<nCount; i++)
            {
               pDocCol = (MSDNDocCol*)m_DocInfoCollections.GetAt(i);
               ar.WriteString(m_strCollection+m_strNewLine);
               str.Format("\t%s%d/>%s", m_strColNum, pDocCol->nNum, m_strNewLine);
		         ar.WriteString(str);
               str.Format("\t%s\"%s\"/>%s", m_strColNum, pDocCol->strName, m_strNewLine);
		         ar.WriteString(str);
               ar.WriteString(m_strNCollection+m_strNewLine);
            }
            ar.WriteString(m_strNCollections+m_strNewLine);
            ar.WriteString(m_strLocations+m_strNewLine);        // Locations
            nCount = m_DocInfoLocations.GetSize();      
            for (i=0; i<nCount; i++)
            {
               pDocLoc = (MSDNDocLoc*)m_DocInfoLocations.GetAt(i);
               ar.WriteString(m_strLocation+m_strNewLine);
               str.Format("\t%s%d/>%s", m_strLocColNum, pDocLoc->nLocColNum, m_strNewLine);
		         ar.WriteString(str);
               str.Format("\t%s\"%s\"/>%s", m_strLocName, pDocLoc->strLocName, m_strNewLine);
		         ar.WriteString(str);
               str.Format("\t%s\"%s\"/>%s", m_strTitleString, pDocLoc->strTitleString, m_strNewLine);
		         ar.WriteString(str);
               str.Format("\t%s\"%s\"/>%s", m_strLocPath, pDocLoc->strLocPath, m_strNewLine);
		         ar.WriteString(str);
               str.Format("\t%s\"%s\"/>%s", m_strVolume, pDocLoc->strVolume, m_strNewLine);
		         ar.WriteString(str);
               ar.WriteString(m_strNLocation+m_strNewLine);
            }
            ar.WriteString(m_strNLocations+m_strNewLine);
            ar.WriteString(m_strDocCompilations+m_strNewLine);
            UpdateAllViews(NULL, HTMLHELPDOCINFO_SAVE, (CObject*) &ar);
            ar.WriteString(m_strNDocCompilations+m_strNewLine);
         }
         ar.WriteString(m_strNHTMLHelpDocInfo+m_strNewLine);
         ar.WriteString(m_strNXML);
      }
	}
	else
	{
      BOOL bRead;
      bool bHTMLhelpCollection = false;
      bool bXML             = false;
      bool bHTMLHelpDocInfo = false;
      bool bDocCompilations = false;
      bool bDocCompilation  = false;
      bool bLocationHistory = false;
      bool bFolders         = false;
      bool bLastFolder      = false;
      bool bCollections     = false;
      bool bLocations       = false;
      bool bCollection      = false;
      bool bLocation        = false;
      int  nFolderLevel = -1;
      int  nFolderOrder = 0;
      int  nFind = 0;
      MSDNFolders  msdnf;
      MSDNDocComp  msdndc;
      MSDNDocCol  *pDocCol = NULL;
      MSDNDocLoc  *pDocLoc = NULL;
      HTREEITEM    hOldParent[32];
      CString      strOldTitle;
      msdnf.hParent   = TVI_ROOT;
      msdnf.hReturned = NULL;
      msdnf.nLangID   = 0;
      msdnf.nOrder    = 0;
      CStringArray  strArr;
      do    
      {
   		bRead = ar.ReadString(str);                           // Textzeilen lesen
         if (bRead)
         {
            strArr.Add(str);
            if (bXML)                                          // XML Datei
            {
               if (bHTMLhelpCollection)                        // HelpCollection
               {
                  if (bFolders)                                // Folders
                  {
                     if (str.Find(m_strFolder) != -1)
                     {
                        if (bLastFolder)
                        {
                           strOldTitle = msdnf.strTitle;
                           UpdateAllViews(NULL, HTMLHELPCOLLECTION_INSERT, (CObject*) &msdnf);
                           nFolderLevel++;
                           ASSERT(nFolderLevel >= 0 && nFolderLevel < 32);
                           hOldParent[nFolderLevel] = msdnf.hParent;
                           msdnf.hParent = msdnf.hReturned;
                           nFolderOrder = 1;
                        }
                        else
                        {
                           nFolderOrder++;
                        }
                        bLastFolder = true;
                     }
                     else if (str.Find(m_strNFolder) != -1)
                     {
                        if (!msdnf.strTitle.IsEmpty()) 
                        {
                           UpdateAllViews(NULL, HTMLHELPCOLLECTION_INSERT, (CObject*) &msdnf);
                           if (!msdndc.strDocCompID.IsEmpty())
                           {
                              IntegrateHelpFile(msdndc);
                           }
                        }
                        if (!bLastFolder)
                        {
                           ASSERT(nFolderLevel >= 0 && nFolderLevel < 32);
                           msdnf.hParent = hOldParent[nFolderLevel];
                           nFolderLevel--;
                        }
                        bLastFolder = false;
                     }
                     else if ((nFind = str.Find(m_strTitle)) != -1)
                     {
                        nFind = str.GetLength()-(nFind+m_strTitle.GetLength())-1;
                        msdnf.strTitle = str.Right(nFind);
                        msdnf.strTitle = msdnf.strTitle.Left(msdnf.strTitle.GetLength()-3);
                     }
                     else if ((nFind = str.Find(m_strTitleLocation)) != -1)
                     {
                        msdndc.strDocCompID   = msdnf.strTitle.Right(msdnf.strTitle.GetLength()-1);
                        msdndc.strLocationRef = strOldTitle;
                        nFind = str.GetLength()-(nFind+m_strTitleLocation.GetLength())-1;
                        msdndc.strTitleLocation = str.Right(nFind);
                        msdndc.strTitleLocation = msdndc.strTitleLocation.Left(msdndc.strTitleLocation.GetLength()-3);
                     }
                     else if ((nFind = str.Find(m_strIndexLocation)) != -1)
                     {
                        nFind = str.GetLength()-(nFind+m_strIndexLocation.GetLength())-1;
                        msdndc.strIndexLocation = str.Right(nFind);
                        msdndc.strIndexLocation = msdndc.strIndexLocation.Left(msdndc.strIndexLocation.GetLength()-3);
                     }
                     else if ((nFind = str.Find(m_strFolderOrder)) != -1)
                     {
                        LPCTSTR pstr = str;
                        msdnf.nOrder = atoi(&pstr[nFind+m_strFolderOrder.GetLength()]);
                     }
                     else if ((nFind = str.Find(m_strLangID)) != -1)
                     {
                        LPCTSTR pstr = str;
                        msdnf.nLangID = atoi(&pstr[nFind+m_strLangID.GetLength()]);
                     }
                     else if (str.Find(m_strNFolders) != -1)
                     {
                        strArr.RemoveAll();
                        strArr.Add(str);
                        bFolders = false;
                     }
                  }
                  else if (str.Find(m_strFolders) != -1)
                  {
                     bFolders = true;
                     continue;
                  }
                  else if ((nFind = str.Find(m_strmasterchm)) != -1)
                  {
                     if (m_bImportExport) continue;
                     nFind = str.GetLength()-(nFind+m_strmasterchm.GetLength())-1;
                     m_strMasterChmValue = str.Right(nFind);
                     m_strMasterChmValue = m_strMasterChmValue.Left(m_strMasterChmValue.GetLength()-3);
                  }
                  else if ((nFind = str.Find(m_strsamplelocation)) != -1)
                  {
                     if (m_bImportExport) continue;
                     nFind = str.GetLength()-(nFind+m_strsamplelocation.GetLength())-1;
                     m_strSampLocValue = str.Right(nFind);
                     m_strSampLocValue = m_strSampLocValue.Left(m_strSampLocValue.GetLength()-3);
                  }
                  else if ((nFind = str.Find(m_strmasterlangid)) != -1)
                  {
                     if (m_bImportExport) continue;
                     LPCTSTR pstr = str;
                     m_nMasterLangId = atoi(&pstr[nFind+m_strmasterlangid.GetLength()]);
                  }
                  else if ((nFind = str.Find(m_strcollectionnum)) != -1)
                  {
                     if (m_bImportExport) continue;
                     LPCTSTR pstr = str;
                     m_nCollectionNum = atoi(&pstr[nFind+m_strcollectionnum.GetLength()]);
                  }
                  else if ((nFind = str.Find(m_strrefcount)) != -1)
                  {
                     if (m_bImportExport) continue;
                     LPCTSTR pstr = str;
                     m_nRefCount = atoi(&pstr[nFind+m_strrefcount.GetLength()]);
                  }
                  else if ((nFind = str.Find(m_strversion)) != -1)
                  {
                     if (m_bImportExport) continue;
                     LPCTSTR pstr = str;
                     m_nVersion = atoi(&pstr[nFind+m_strversion.GetLength()]);
                  }
                  else if ((nFind = str.Find(m_strfindmergedchms )) != -1)
                  {
                     if (m_bImportExport) continue;
                     LPCTSTR pstr = str;
                     m_nFindMergeDchms = atoi(&pstr[nFind+m_strfindmergedchms.GetLength()]);
                  }
                  else if (str.Find(m_strNHTMLHelpCollection) != -1)
                  {
                     bHTMLhelpCollection = false;
                     continue;
                  }
               }
               else if (bHTMLHelpDocInfo)
               {
                  if (bDocCompilations)
                  {
                     if (bDocCompilation)
                     {
                        if (bLocationHistory)
                        {
                           if ((nFind = str.Find(m_strColNum)) != -1)
                           {
                              LPCTSTR pstr = str;
                              msdndc.nColNum = atoi(&pstr[nFind+m_strColNum.GetLength()]);
                           }
                           else if ((nFind = str.Find(m_strTitleLocation)) != -1)
                           {
                              nFind = str.GetLength()-(nFind+m_strTitleLocation.GetLength())-1;
                              msdndc.strTitleLocation = str.Right(nFind);
                              msdndc.strTitleLocation = msdndc.strTitleLocation.Left(msdndc.strTitleLocation.GetLength()-3);
                           }
                           else if ((nFind = str.Find(m_strIndexLocation)) != -1)
                           {
                              nFind = str.GetLength()-(nFind+m_strIndexLocation.GetLength())-1;
                              msdndc.strIndexLocation = str.Right(nFind);
                              msdndc.strIndexLocation = msdndc.strIndexLocation.Left(msdndc.strIndexLocation.GetLength()-3);
                           }
                           else if ((nFind = str.Find(m_strQueryLocation)) != -1)
                           {
                              nFind = str.GetLength()-(nFind+m_strQueryLocation.GetLength())-1;
                              msdndc.strQueryLocation = str.Right(nFind);
                              msdndc.strQueryLocation = msdndc.strQueryLocation.Left(msdndc.strQueryLocation.GetLength()-3);
                           }
                           else if ((nFind = str.Find(m_strLocationRef)) != -1)
                           {
                              nFind = str.GetLength()-(nFind+m_strLocationRef.GetLength())-1;
                              msdndc.strLocationRef = str.Right(nFind);
                              msdndc.strLocationRef = msdndc.strLocationRef.Left(msdndc.strLocationRef.GetLength()-3);
                           }
                           else if ((nFind = str.Find(m_strDCVersion)) != -1)
                           {
                              LPCTSTR pstr = str;
                              msdndc.nVersion = atoi(&pstr[nFind+m_strDCVersion.GetLength()]);
                           }
                           else if ((nFind = str.Find(m_strLastPromptedVersion)) != -1)
                           {
                              LPCTSTR pstr = str;
                              msdndc.nLastPromptedVersion = atoi(&pstr[nFind+m_strLastPromptedVersion.GetLength()]);
                           }
                           else if ((nFind = str.Find(m_strTitleSampleLocation)) != -1)
                           {
                              nFind = str.GetLength()-(nFind+m_strTitleSampleLocation.GetLength())-1;
                              msdndc.strTitleSampleLocation = str.Right(nFind);
                              msdndc.strTitleSampleLocation = msdndc.strTitleSampleLocation.Left(msdndc.strTitleSampleLocation.GetLength()-3);
                           }
                           else if ((nFind = str.Find(m_strTitleQueryLocation)) != -1)
                           {
                              nFind = str.GetLength()-(nFind+m_strTitleQueryLocation.GetLength())-1;
                              msdndc.strTitleQueryLocation = str.Right(nFind);
                              msdndc.strTitleQueryLocation = msdndc.strTitleQueryLocation.Left(msdndc.strTitleQueryLocation.GetLength()-3);
                           }
                           else if ((nFind = str.Find(m_strSupportsMerge)) != -1)
                           {
                              LPCTSTR pstr = str;
                              msdndc.nSupportsMerge = atoi(&pstr[nFind+m_strSupportsMerge.GetLength()]);
                           }
                           else if (str.Find(m_strNLocationHistory) != -1)
                           {
                              bLocationHistory = false;
                           }
                        }
                        else if ((nFind = str.Find(m_strDocCompId)) != -1)
                        {
                           nFind = str.GetLength()-(nFind+m_strDocCompId.GetLength())-1;
                           msdndc.strDocCompID = str.Right(nFind);
                           msdndc.strDocCompID = msdndc.strDocCompID.Left(msdndc.strDocCompID.GetLength()-3);
                        }
                        else if ((nFind = str.Find(m_strDocCompLanguage)) != -1)
                        {
                           LPCTSTR pstr = str;
                           msdndc.nDocCompLanguage = atoi(&pstr[nFind+m_strDocCompLanguage.GetLength()]);
                        }
                        else if (str.Find(m_strLocationHistory) != -1)
                        {
                           bLocationHistory = true;
                        }
                        else if (str.Find(m_strNDocCompilation) != -1)
                        {
                           UpdateAllViews(NULL, HTMLHELPDOCINFO_INSERT, (CObject*) &msdndc);
                           bDocCompilation = false;
                        }
                     }
                     else if (str.Find(m_strDocCompilation) != -1)
                     {
                        bDocCompilation = true;
                     }
                     else if (str.Find(m_strNDocCompilations) != -1)
                     {
                        bDocCompilations = false;
                     }
                  }
                  else if (bCollections)
                  {
                     if (bCollection)
                     {
                        if ((nFind = str.Find(m_strColNum)) != -1)
                        {
                           LPCTSTR pstr = str;
                           pDocCol->nNum = atoi(&pstr[nFind+m_strColNum.GetLength()]);
                        }
                        else if ((nFind = str.Find(m_strColName)) != -1)
                        {
                           nFind = str.GetLength()-(nFind+m_strColName.GetLength())-1;
                           pDocCol->strName = str.Right(nFind);
                           pDocCol->strName = pDocCol->strName.Left(pDocCol->strName.GetLength()-3);
                        }
                        else if (str.Find(m_strNCollection) != -1)
                        {
                           if (pDocCol)
                           {
                              m_DocInfoCollections.Add(pDocCol);
                              pDocCol = NULL;
                           }
                           bCollection = false;
                        }
                     }
                     else if (str.Find(m_strCollection) != -1)
                     {
                        pDocCol = new MSDNDocCol;
                        bCollection = true;
                     }
                     else if (str.Find(m_strNCollections) != -1)
                     {
                        bCollections = false;
                     }
                  }
                  else if (bLocations)
                  {
                     if (bLocation)
                     {
                        if ((nFind = str.Find(m_strLocColNum)) != -1)
                        {
                           LPCTSTR pstr = str;
                           pDocLoc->nLocColNum = atoi(&pstr[nFind+m_strLocColNum.GetLength()]);
                        }
                        else if ((nFind = str.Find(m_strLocName)) != -1)
                        {
                           nFind = str.GetLength()-(nFind+m_strLocName.GetLength())-1;
                           pDocLoc->strLocName = str.Right(nFind);
                           pDocLoc->strLocName = pDocLoc->strLocName.Left(pDocLoc->strLocName.GetLength()-3);
                        }
                        else if ((nFind = str.Find(m_strTitleString)) != -1)
                        {
                           nFind = str.GetLength()-(nFind+m_strTitleString.GetLength())-1;
                           pDocLoc->strTitleString = str.Right(nFind);
                           pDocLoc->strTitleString = pDocLoc->strTitleString.Left(pDocLoc->strTitleString.GetLength()-3);
                        }
                        else if ((nFind = str.Find(m_strLocPath)) != -1)
                        {
                           nFind = str.GetLength()-(nFind+m_strLocPath.GetLength())-1;
                           pDocLoc->strLocPath = str.Right(nFind);
                           pDocLoc->strLocPath = pDocLoc->strLocPath.Left(pDocLoc->strLocPath.GetLength()-3);
                        }
                        else if ((nFind = str.Find(m_strVolume)) != -1)
                        {
                           nFind = str.GetLength()-(nFind+m_strVolume.GetLength())-1;
                           pDocLoc->strVolume = str.Right(nFind);
                           pDocLoc->strVolume = pDocLoc->strVolume.Left(pDocLoc->strVolume.GetLength()-3);
                        }
                        else if (str.Find(m_strNLocation) != -1)
                        {
                           bLocation = false;
                           if (pDocLoc)
                           {
                              m_DocInfoLocations.Add(pDocLoc);
                              pDocLoc = NULL;
                           }
                        }
                     }
                     else if (str.Find(m_strLocation) != -1)
                     {
                        ASSERT(pDocLoc==NULL);
                        bLocation = true;
                        pDocLoc   = new MSDNDocLoc;
                     }
                     else if (str.Find(m_strNLocations) != -1)
                     {
                        bLocations = false;
                     }
                  }
                  else if ((nFind = str.Find(m_strNextCollectionId)) != -1)
                  {
                     LPCTSTR pstr = str;
                     m_nNextCollectionID = atoi(&pstr[nFind+m_strNextCollectionId.GetLength()]);
                  }
                  else if (str.Find(m_strDocCompilations) != -1)
                  {
                     bDocCompilations = true;
                  }
                  else if (str.Find(m_strCollections) != -1)
                  {
                     bCollections = true;
                  }
                  else if (str.Find(m_strLocations) != -1)
                  {
                     bLocations = true;
                  }
                  else if (str.Find(m_strNHTMLHelpDocInfo) != -1)
                  {
                     bHTMLHelpDocInfo = false;
                  }
               }
               else if (str.Find(m_strHTMLHelpDocInfo) != -1)
               {
                  bHTMLHelpDocInfo = true;
                  if (!m_bImportExport)
                  {
                     UpdateAllViews(NULL, HTMLHELPDOCINFO_DELETE);
                     DeleteCollections();
                     DeleteLocations();
                  }
               }
               else if (str.Find(m_strHTMLHelpCollection) != -1)
               {
                  bHTMLhelpCollection = true;
                  if (!m_bImportExport)
                  {
                     UpdateAllViews(NULL, HTMLHELPCOLLECTION_DELETE);
                  }
                  continue;
               }
               else if (str.Find(m_strNXML) != -1)
               {
                  bXML = false;
               }
            }
            else if (str.Find(m_strXML) != -1)
            {
               bXML = true;
            }
         }
      	
      } while(bRead);
      if (m_nDoctype == DOC_TYPE_HTMLHELPCOLLECTION)
      {
         GetFileTime((HANDLE)ar.GetFile()->m_hFile, NULL, NULL, &m_ftHTMLHELPCOLLECTION);
      }
      else if (m_nDoctype == DOC_TYPE_HTMLHELPDOCINFO)
      {
         GetFileTime((HANDLE)ar.GetFile()->m_hFile, NULL, NULL, &m_ftHTMLHELPDOCINFO);
      }
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMSDNEditDoc diagnostics

#ifdef _DEBUG
void CMSDNEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMSDNEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMSDNEditDoc commands

BOOL CMSDNEditDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
   CString strFileName(lpszPathName);
   strFileName.MakeUpper();
   BOOL bModify = IsModified();
   if      (strFileName.Find(_T(".COL")) != -1) m_nDoctype = DOC_TYPE_HTMLHELPCOLLECTION;
   else if (strFileName.Find(_T(".DAT")) != -1) m_nDoctype = DOC_TYPE_HTMLHELPDOCINFO;
	BOOL bReturn = CDocument::OnSaveDocument(lpszPathName);
   m_nDoctype = 0;
   
   return bReturn;
}

BOOL CMSDNEditDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
   CString strFileName(lpszPathName);
   strFileName.MakeUpper();
   int nFind;
   if      ((nFind = strFileName.Find(_T(".COL"))) != -1)
   {
      m_nDoctype = DOC_TYPE_HTMLHELPCOLLECTION;
      if (!m_bImportExport)
      {
         m_strHTMLHELPCOLLECTION = _T(lpszPathName);
      }
   }
   else if ((nFind = strFileName.Find(_T(".DAT"))) != -1)
   {
      m_nDoctype = DOC_TYPE_HTMLHELPDOCINFO;
      if (!m_bImportExport)
      {
         m_strHTMLHELPDOCINFO = _T(lpszPathName);
      }
   }
   if (m_bCreateBackUpFile)
   {
      strFileName.SetAt(nFind+1, 'B');
      strFileName.SetAt(nFind+2, 'A');
      strFileName.SetAt(nFind+3, 'K');
      if (::GetFileAttributes(strFileName) != -1)
      {
         CString strNewFileName, strFormat = strFileName.Left(nFind+1) + _T("%03d");
         int i = 0;
         do 
         {
            strNewFileName.Format(strFormat, i++);
         } while(::GetFileAttributes(strNewFileName) != -1);
         CFile::Rename(strFileName, strNewFileName);
      }
      CopyFile(lpszPathName, strFileName, false);
   }
	BOOL bReturn = CDocument::OnOpenDocument(lpszPathName);
   m_nDoctype = 0;
	return bReturn;
}

void CMSDNEditDoc::DeleteContents() 
{
	CDocument::DeleteContents();
}

void CMSDNEditDoc::OnUpdateFileImport(CCmdUI* pCmdUI) {pCmdUI->Enable();}
void CMSDNEditDoc::OnUpdateFileSave(CCmdUI* pCmdUI)   
{
   pCmdUI->Enable(IsModified());
   CView *pView = ((CMainFrame*)AfxGetMainWnd())->GetActiveView();
   m_nActiveType = (pView->IsKindOf(RUNTIME_CLASS(CLeftView))) ? DOC_TYPE_HTMLHELPCOLLECTION : DOC_TYPE_HTMLHELPDOCINFO;
}

void CMSDNEditDoc::OnFileImport() 
{
   CMSDNEditApp *pApp = (CMSDNEditApp*) AfxGetApp();
   CString strFileName;
   CDocTemplate *pTemp = NULL;
//   if (m_nActiveType == DOC_TYPE_HTMLHELPDOCINFO) pTemp = pApp->GetDT2();
   BOOL bReturn = pApp->DoPromptFileName(strFileName, IDS_IMPORT_FILE, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, TRUE, pTemp);
   if (bReturn)
   {
      m_nFilesImported = 0;
      m_bImportExport  = true;
      OnOpenDocument(strFileName);
      m_bImportExport = false;
      if (m_nFilesImported > 0)
      {
         OnSaveDocument(m_strHTMLHELPCOLLECTION);
         OnOpenDocument(m_strHTMLHELPDOCINFO);
         strFileName.Format(IDS_FILES_IMPORTED, m_nFilesImported);
         AfxMessageBox(strFileName);
      }
   }
}


void CMSDNEditDoc::OnFileExport() 
{
   CMSDNEditApp *pApp = (CMSDNEditApp*) AfxGetApp();
   CDocTemplate *pTemp = NULL;
//   if (m_nActiveType == DOC_TYPE_HTMLHELPDOCINFO) pTemp = pApp->GetDT2();
   CString strFileName;
   BOOL bReturn = pApp->DoPromptFileName(strFileName, IDS_EXPORT_FILE, OFN_HIDEREADONLY, FALSE, pTemp);
   if (bReturn)
   {
      m_bImportExport = true;
      OnSaveDocument(strFileName);
      m_bImportExport = false;
   }
}

void CMSDNEditDoc::DeleteCollections()
{
   MSDNDocCol *pDC;
   while (m_DocInfoCollections.GetSize())
   {
      pDC = (MSDNDocCol*) m_DocInfoCollections.GetAt(0);
      m_DocInfoCollections.RemoveAt(0);
      delete pDC;
   }
}

void CMSDNEditDoc::DeleteLocations()
{
   MSDNDocLoc *pDL;
   while (m_DocInfoLocations.GetSize())
   {
      pDL = (MSDNDocLoc*) m_DocInfoLocations.GetAt(0);
      m_DocInfoLocations.RemoveAt(0);
      delete pDL;
   }
}

void CMSDNEditDoc::OnFileSave() 
{
   CMSDNEditApp *pApp = (CMSDNEditApp*) AfxGetApp();
   CString strOld(m_strPathName);
   CDocTemplate *pOld = NULL;
   BOOL bModified = IsModified();
   if (bModified & DOC_TYPE_HTMLHELPDOCINFO)
   {
      pOld = m_pDocTemplate;
      m_pDocTemplate = pApp->GetDT2();
      m_strPathName = m_strHTMLHELPDOCINFO;
      CDocument::OnFileSave();
      m_pDocTemplate = pOld;
   }
   if (bModified & DOC_TYPE_HTMLHELPCOLLECTION)
   {
      m_strPathName = m_strHTMLHELPCOLLECTION;
      CDocument::OnFileSave();
   }
   m_strPathName  = strOld;
}

void CMSDNEditDoc::OnFileSaveAs() 
{
   CMSDNEditApp *pApp = (CMSDNEditApp*) AfxGetApp();
   CDocTemplate *pOld = NULL;
   CString strOld(m_strPathName);
   if (m_nActiveType == DOC_TYPE_HTMLHELPDOCINFO)
   {
      pOld = m_pDocTemplate;
      m_pDocTemplate = pApp->GetDT2();
      m_strPathName = m_strHTMLHELPDOCINFO;
   }
   else
   {
      m_strPathName = m_strHTMLHELPCOLLECTION;
   }
   CDocument::OnFileSaveAs();	
	if (pOld)
   {
      m_pDocTemplate = pOld;
   }
   m_strPathName  = strOld;
}

void CMSDNEditDoc::SetModifiedFlagEx(BOOL bMod)
{
   m_bModified |= bMod;
}

void CMSDNEditDoc::OnUpdateEditNewItem(CCmdUI* pCmdUI) 
{
   CMSDNEditApp *pApp = (CMSDNEditApp*) AfxGetApp();
   if (pApp->TestMSDNIntProcess())
   {
      pCmdUI->Enable(false);
   }
   else
   {
      pCmdUI->Enable(true);
   }
}
void CMSDNEditDoc::OnUpdateFileReload(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(!m_strHTMLHELPCOLLECTION.IsEmpty() && !m_strHTMLHELPDOCINFO.IsEmpty());
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
   DWORD dwProcessId;
   PROCESS_INFORMATION *pI = (PROCESS_INFORMATION*)lParam;
   GetWindowThreadProcessId(hwnd, &dwProcessId);
   if (pI->dwProcessId == dwProcessId)
   {
      pI->dwThreadId = (DWORD) hwnd;
      return 0;
   }
   return 1;
}

void CMSDNEditDoc::OnEditNewItem() 
{
   OnFileSave();
   if (IsModified()) return ;

   CMSDNEditApp *pApp = (CMSDNEditApp*) AfxGetApp();
   STARTUPINFO startupinfo;
   ZeroMemory(&startupinfo, sizeof(STARTUPINFO));              // StartupInfo Struktur füllen
   startupinfo.cb          = sizeof(STARTUPINFO);
   startupinfo.wShowWindow = SW_SHOW;

   if (CreateProcess(NULL, "MSDNIntegrator.exe", NULL, NULL, false, 0, NULL, NULL, &startupinfo, &pApp->m_piMSNDIntegratior))
   {
      pApp->m_piMSNDIntegratior.dwThreadId = 0;
      Sleep(500);
      EnumWindows(EnumWindowsProc, (LPARAM)&pApp->m_piMSNDIntegratior);
      if (pApp->m_piMSNDIntegratior.dwThreadId)
      {
         ::SetWindowPos((HWND)pApp->m_piMSNDIntegratior.dwThreadId, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
         AfxGetMainWnd()->SetTimer(MSDN_WATCH_TIMER, 500, NULL);
         AfxGetMainWnd()->EnableWindow(false);
      }
   }
}

void CMSDNEditDoc::OnFileReload() 
{
   CFile File;
   FILETIME  LastWriteTime;
   int nTDI=0, nTHC=0;
   CMSDNEditApp *pApp = (CMSDNEditApp*) AfxGetApp();
   if (m_strHTMLHELPDOCINFO.IsEmpty())
   {
      m_strHTMLHELPDOCINFO = pApp->m_pIntegration->GetRegistryFile();
      nTDI = 1;
#ifndef _DEBUG
      m_bCreateBackUpFile = true;
#endif
   }
   else if (File.Open(m_strHTMLHELPDOCINFO, CFile::modeRead))
   {
      GetFileTime((HANDLE)File.m_hFile, NULL, NULL, &LastWriteTime);
      nTDI = CompareFileTime(&LastWriteTime, &m_ftHTMLHELPDOCINFO);
      File.Close();
   }
   if (m_strHTMLHELPCOLLECTION.IsEmpty())
   {
      m_strHTMLHELPCOLLECTION = pApp->m_pIntegration->GetCollectionFile();
      nTHC = 1;
#ifndef _DEBUG
      m_bCreateBackUpFile = true;
#endif
   }
   else if (File.Open(m_strHTMLHELPCOLLECTION, CFile::modeRead))
   {
      GetFileTime((HANDLE)File.m_hFile, NULL, NULL, &LastWriteTime);
      nTHC = CompareFileTime(&LastWriteTime, &m_ftHTMLHELPCOLLECTION);
      File.Close();
   }

   if ((nTDI > 0) || (nTHC > 0))
   {
      OnNewDocument();
      OnOpenDocument(m_strHTMLHELPDOCINFO);
      OnOpenDocument(m_strHTMLHELPCOLLECTION);
   }
   m_bCreateBackUpFile = false;
}

void CMSDNEditDoc::IntegrateHelpFile(MSDNDocComp &msdndc)
{
   CMSDNEditApp *pApp = (CMSDNEditApp*) AfxGetApp();
   CString strPathName;
   strPathName.Format("%s\\%s", m_strHelpFilePath, msdndc.strTitleLocation);
   if (GetFileAttributes(strPathName) == 0xffffffff)
   {
      CString strFilter, strPath;
      strFilter.LoadString(IDS_CHM_FILTER);
      CFileDialog dialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter);
      do 
      {
         if (dialog.DoModal() == IDOK)
         {
            strPath = dialog.GetPathName();
            int nFind = strPath.ReverseFind('\\');
            if (nFind != -1) strPath = strPath.Left(nFind);
            strPathName.Format("%s\\%s", strPath, msdndc.strTitleLocation);
            if (GetFileAttributes(strPathName) != 0xffffffff)
            {
               m_strHelpFilePath = strPath;
               strPathName.Format("%s\\%s", m_strHelpFilePath, msdndc.strIndexLocation);
            }
         }
         else
         {
            m_strHelpFilePath.Empty();
            break;
         }
      } while(GetFileAttributes(strPathName) == 0xffffffff);
   }

   strPathName.Format("%s\\%s", m_strHelpFilePath, msdndc.strIndexLocation);
   pApp->m_pIntegration->SetChiFile(strPathName);
   strPathName.Format("%s\\%s", m_strHelpFilePath, msdndc.strTitleLocation);
   pApp->m_pIntegration->SetChmFile(strPathName);
   pApp->m_pIntegration->SetUniqueID(msdndc.strDocCompID);
   pApp->m_pIntegration->SetTitleString(msdndc.strLocationRef);

   if (pApp->m_pIntegration->Integrate())
   {
      m_nFilesImported++;
   }

   msdndc.strDocCompID.Empty();
   msdndc.strTitleLocation.Empty();
   msdndc.strIndexLocation.Empty();
   msdndc.strLocationRef.Empty();
}

void CMSDNEditDoc::OnViewColFile() 
{
   if (!m_strHTMLHELPCOLLECTION.IsEmpty())
   {
      CString str = m_strHTMLHELPCOLLECTION;
      int nFind = str.ReverseFind('\\');
      if (nFind != -1) str = str.Left(nFind);
      ShellExecute(AfxGetMainWnd()->m_hWnd, "explore", str, NULL, NULL, SW_SHOW);
   }
}

void CMSDNEditDoc::OnViewRegFile() 
{
   if (!m_strHTMLHELPDOCINFO.IsEmpty())
   {
      CString str = m_strHTMLHELPDOCINFO;
      int nFind = str.ReverseFind('\\');
      if (nFind != -1) str = str.Left(nFind);
      ShellExecute(AfxGetMainWnd()->m_hWnd, "explore", str, NULL, NULL, SW_SHOW);
   }
}

void CMSDNEditDoc::OnViewColInfo() 
{
   CString str;
   str.Format(IDS_COL_INFO, m_strMasterChmValue, m_nMasterLangId, m_strSampLocValue, m_nCollectionNum, m_nRefCount, m_nVersion, m_nFindMergeDchms);
   AfxMessageBox(str);
}

void CMSDNEditDoc::OnViewColections() 
{
	CMSDNCollection dlg;
   dlg.m_pPtrList = &m_DocInfoCollections;
   dlg.DoModal();
}

void CMSDNEditDoc::OnViewLocations() 
{
   CMSDNLocations dlg;
   dlg.m_pPtrList = &m_DocInfoLocations;
   dlg.DoModal();
}

void CMSDNEditDoc::OnUpdateViewLocations(CCmdUI* pCmdUI) 
{
}

void CMSDNEditDoc::OnUpdateViewColections(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(m_DocInfoCollections.GetSize()>0);	
}

void CMSDNEditDoc::OnFileChangeDocInfo() 
{
   SetModifiedFlagEx(DOC_TYPE_HTMLHELPDOCINFO);
}
