// WkWizardDoc.cpp : implementation of the CWkWizardDoc class
//

#include "stdafx.h"
#include "WkWizard.h"

#include "WkWizardDoc.h"
#include "Parser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWkWizardDoc

IMPLEMENT_DYNCREATE(CWkWizardDoc, CDocument)

BEGIN_MESSAGE_MAP(CWkWizardDoc, CDocument)
	//{{AFX_MSG_MAP(CWkWizardDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWkWizardDoc construction/destruction

CWkWizardDoc::CWkWizardDoc()
{
	// TODO: add one-time construction code here

}

CWkWizardDoc::~CWkWizardDoc()
{
}

BOOL CWkWizardDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	m_saCPPFiles.RemoveAll();
	m_saRCFiles.RemoveAll();
	m_saDependenciesFiles.RemoveAll();
	m_TSArray.DeleteAll();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CWkWizardDoc serialization

void CWkWizardDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWkWizardDoc diagnostics

#ifdef _DEBUG
void CWkWizardDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWkWizardDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWkWizardDoc commands

BOOL CWkWizardDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	m_saCPPFiles.RemoveAll();
	m_saRCFiles.RemoveAll();
	m_saDependenciesFiles.RemoveAll();

	m_TSArray.DeleteAll();

	// read the make file to extract all
	// cpp
	// rc
	// dep
	CStdioFile file;
	CFileException cfe;
	CString line;
	CString name;
	int p,i;
	BOOL bFound = FALSE;

	m_sPath = lpszPathName;
	p = m_sPath.ReverseFind('\\');
	if (p!=-1)
	{
		m_sPath = m_sPath.Left(p+1);
	}
	if (!file.Open(lpszPathName,CFile::modeRead,&cfe))
	{
		return FALSE;
	}

	while (file.ReadString(line))
	{
		if (0==line.Find("SOURCE="))
		{
			// found a source file
			//p = line.ReverseFind('\\');
			//name = line.Mid(p+1);
			name = line.Mid(9);
			if (-1!=name.Find(".c"))
			{
				m_saCPPFiles.Add(name);
			}
			if (-1!=name.Find(".rc"))
			{
				m_saRCFiles.Add(name);
			}
			// now browse the dependencies
			while (file.ReadString(line))
			{
				if (line.GetLength()>0)
				{
					p = line.Find('\"');
					if (-1!=p)
					{
						name = line.Mid(p+1);
						p = name.Find('\"');
						if (-1!=p)
						{
							name = name.Left(p);
							bFound = FALSE;
							for (i=0;i<m_saDependenciesFiles.GetSize();i++)
							{
								if (m_saDependenciesFiles[i]==name)
								{
									bFound = TRUE;
								}
							}
							if (!bFound)
							{
								m_saDependenciesFiles.Add(name);
							}
						}
					}
				}
				else
				{
					break;
				}
			}
		}
	}

	file.Close();

	UpdateAllViews(NULL,1,NULL);
	
	return TRUE;
}
////////////////////////////////////////////////////////////////////
void CWkWizardDoc::ExtractResource(CString& name)
{
	CString fname = m_sPath + name;
	CScanner scanner(fname);
	CString lng,mes;
	CFileStatus cfs;
	int ret;

	lng = m_sPath + name + ".lng";
	if (CFile::GetStatus(lng,cfs))
	{
		ret = AfxMessageBox(IDS_LANGUAGE_ALREADY_EXIST,MB_YESNOCANCEL);
		switch (ret)
		{
		case IDYES:
			m_TSArray.Load(lng);
			break;
		case IDNO:
			m_TSArray.DeleteAll();
			break;
		case IDCANCEL:
			return;
		}
	}
	if (scanner.Scan())
	{
		CParser parser(&scanner,&m_TSArray,FALSE);

		if (parser.Parse())
		{
			m_TSArray.Save(lng);
			mes.Format("%d zu übersetzende Texte gefunden.\nTexte in Datei %s geschrieben",m_TSArray.GetSize(),lng);
			AfxMessageBox(mes);
			UpdateAllViews(NULL,3,(CObject*)&m_TSArray);
		}
		else
		{
			m_TSArray.DeleteAll();
		}
	}
}
////////////////////////////////////////////////////////////////////
void CWkWizardDoc::InsertResource(CString& orig,  CString& lng, CString& lang)
{
	TRACE("%s,%s,%s\n",orig,lng,lang);
	CString fname = m_sPath + orig;
	CScanner scanner(fname,TRUE);
	CString mes,ls,dest;
	CFileStatus cfs;
	int ret,p;


	p = fname.Find(".rc");
	if (p==-1)
	{
		return;
	}
	dest = fname.Left(p) + lang + ".rc";
	if (CFile::GetStatus(dest,cfs))
	{
		ls.LoadString(IDS_RES_EXISTS);
		mes.Format(ls,dest);
		ret = AfxMessageBox(mes,MB_YESNO);
		switch (ret)
		{
		case IDYES:
			break;
		case IDNO:
			return;
		}
	}
	if (!m_TSArray.Load(lng))
	{
		return;
	}

	if (scanner.Scan())
	{
		CParser parser(&scanner,&m_TSArray,dest,lang,FALSE);

		if (parser.Parse())
		{
			mes.Format("%d zu übersetzende Texte ersetzt.\nTexte in Datei %s geschrieben",m_TSArray.GetSize(),dest);
			AfxMessageBox(mes);
		}
	}
	m_TSArray.DeleteAll();
}
////////////////////////////////////////////////////////////////////
void CWkWizardDoc::ScanFile(CString& name)
{
	CString fname = m_sPath + name;
	CString cname = m_sPath + name;
	CScanner scanner(fname,TRUE);
	CToken* pToken;
	CStdioFile file;
	CFile cfile;
	CFileException cfe;
	CString line;
	int i=0;
	CString mes;

	fname += ".scn";
	cname += ".copy";
	if (scanner.Scan())
	{
		if (file.Open(fname,CFile::modeCreate|CFile::modeWrite,&cfe))
		{
			while (NULL!=(pToken = scanner.NextToken()))
			{
				line.Format("<%s,%d,%d>\n",(const char*)pToken->GetOriginal(),
					(int)pToken->GetSymbol(),pToken->GetLine());
				file.WriteString(line);
				i++;
			}
			file.Close();
		}
		if (cfile.Open(cname,CFile::modeCreate|CFile::modeWrite,&cfe))
		{
			scanner.BackToFirst();
			while (NULL!=(pToken = scanner.NextToken()))
			{
				cfile.Write((void*)(const char*)pToken->GetWhiteAndComment(),pToken->GetWhiteAndComment().GetLength());
				cfile.Write((void*)(const char*)pToken->GetOriginal(),pToken->GetOriginal().GetLength());
			}
			cfile.Close();
		}
		mes.Format("%d Token gescannt in Datei\n %s",i,(const char*)fname);
		AfxMessageBox(mes);
	}
	UpdateAllViews(NULL,2,(CObject*)scanner.GetTokens());
}
