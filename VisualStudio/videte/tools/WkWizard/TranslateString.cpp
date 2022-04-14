/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "TranslateString.h"

/////////////////////////////////////////////////////////////////////////////
CString Enum2Name(ResourceStringType rst)
{
	CString sRet;
	switch (rst)
	{
	case RST_DEFAULT:
		sRet = "DEFAULT";
		break;
	case RST_MENUITEM:
		sRet = "MENUITEM";
		break;
	case RST_POPUP:
		sRet = "POPUP";
		break;
	case RST_CAPTION:
		sRet = "CAPTION";
		break;
	case RST_LTEXT:
		sRet = "LTEXT";
		break;
	case RST_CTEXT:
		sRet = "CTEXT";
		break;
	case RST_RTEXT:
		sRet = "RTEXT";
		break;
	case RST_DEFPUSHBUTTON:
		sRet = "DEFPUSHBUTTON";
		break;
	case RST_PUSHBUTTON:
		sRet = "PUSHBUTTON";
		break;
	case RST_GROUPBOX:
		sRet = "GROUPBOX";
		break;
	case RST_CONTROL:
		sRet = "CONTROL";
		break;
	case RST_PUSHBOX:
		sRet = "PUSHBOX";
		break;
	case RST_SCROLLBAR:
		sRet = "SCROLLBAR";
		break;
	case RST_STATE3:
		sRet = "STATE3";
		break;
	case RST_STRING:
		sRet = "STRING";
		break;
	default:
		sRet = "ERROR";
		break;
	}
	return sRet;
}
/////////////////////////////////////////////////////////////////////////////
ResourceStringType Name2Enum(const CString& sName)
{
	if (sName=="DEFAULT")
		return RST_DEFAULT;
	if (sName=="MENUITEM")
		return RST_MENUITEM;
	if (sName=="POPUP")
		return RST_POPUP;
	if (sName=="CAPTION")
		return RST_CAPTION;
	if (sName=="LTEXT")
		return RST_LTEXT;
	if (sName=="CTEXT")
		return RST_CTEXT;
	if (sName=="RTEXT")
		return RST_RTEXT;
	if (sName=="DEFPUSHBUTTON")
		return RST_DEFPUSHBUTTON;
	if (sName=="PUSHBUTTON")
		return RST_PUSHBUTTON;
	if (sName=="GROUPBOX")
		return RST_GROUPBOX;
	if (sName=="CONTROL")
		return RST_CONTROL;
	if (sName=="PUSHBOX")
		return RST_PUSHBOX;
	if (sName=="SCROLLBAR")
		return RST_SCROLLBAR;
	if (sName=="STATE3")
		return RST_STATE3;
	if (sName=="STRING")
		return RST_STRING;

	return RST_DEFAULT;
}
char CTranslateString::m_cDelimiter = '@';
/////////////////////////////////////////////////////////////////////////////
CTranslateString::CTranslateString()
{
//	CString	m_sResourceFile;
//	CString m_sText;
//	CString m_sTranslate;
//	CString m_sID;
	m_rst = RST_DEFAULT;

//	CString m_sDialogID;
//	CString m_sMenuID;
//	CString m_sPopup;

//	CTime	m_tScan;
//	CTime	m_tTranslate;
//	CTime	m_tInsert;
}
/////////////////////////////////////////////////////////////////////////////
CTranslateString::CTranslateString(const CString& sResourceFile,
								   const CString& sID,
								   const CString& sText)
{
	m_sResourceFile = sResourceFile;
	m_rst = RST_STRING;
	m_sText = sText;
	m_sID = sID;
	m_sTranslate = "TODO! " + sText;
//	m_sTranslate = "TODO";

	m_tScan = CTime::GetCurrentTime();
	m_tTranslate = m_tScan;
	m_tInsert = m_tScan;

	m_sFullID = CalcID();
	CountWords();
}
/////////////////////////////////////////////////////////////////////////////
// constuctor for dialog control
CTranslateString::CTranslateString(ResourceStringType rst,
								   const CString& sResourceFile,
								   const CString& sDialogID,
								   const CString& sControlID,
								   const CString& sText)
{
	m_sResourceFile = sResourceFile;
	m_rst = rst;
	m_sText = sText;
	m_sDialogID = sDialogID;
	m_sID = sControlID;
	m_sTranslate = "TODO! " + sText;
//	m_sTranslate = "TODO";

	m_tScan = CTime::GetCurrentTime();
	m_tTranslate = m_tScan;
	m_tInsert = m_tScan;

	m_sFullID = CalcID();
	CountWords();
}
/////////////////////////////////////////////////////////////////////////////
// constuctor for menu
CTranslateString::CTranslateString(ResourceStringType	rst,
									 const CString& sResourceFile,
									 const CString& sMenuID,
									 int   iPopup,
									 const CString& sItemID,
									 const CString& sText)
{
	m_sResourceFile = sResourceFile;
	m_rst = rst;
	m_sText = sText;
	m_sMenuID = sMenuID;
	m_sPopup.Format("POPUP%d",iPopup);
	m_sID = sItemID;
	m_sTranslate = "TODO! " + sText;
//	m_sTranslate = "TODO";

	m_tScan = CTime::GetCurrentTime();
	m_tTranslate = CTime(1980,1,1,1,1,1);
	m_tInsert = m_tTranslate;

	m_sFullID = CalcID();
	CountWords();
}
/////////////////////////////////////////////////////////////////////////////
CTranslateString::~CTranslateString()
{
}
/////////////////////////////////////////////////////////////////////////////
void CTranslateString::Update(CTranslateString* pTS)
{
	if (m_sText != pTS->m_sText)
	{
		m_sText = pTS->m_sText;
		CountWords();
		m_sTranslate = "TODO! " + m_sTranslate;
		m_tScan = pTS->m_tScan;
	}
}
/////////////////////////////////////////////////////////////////////////////
const CString CTranslateString::CalcID()
{
	switch (m_rst)
	{
	case RST_STRING:
		return m_sID;
		break;
	case RST_CAPTION:
		return m_sDialogID + ".CAPTION";
		break;
	case RST_LTEXT:	
	case RST_CTEXT:
	case RST_RTEXT:
	case RST_DEFPUSHBUTTON:
	case RST_PUSHBUTTON:
	case RST_GROUPBOX:
	case RST_CONTROL:
	case RST_PUSHBOX:
	case RST_SCROLLBAR:
	case RST_STATE3:
		return m_sDialogID + '.' + m_sID;
		break;
	case RST_POPUP:
		return m_sMenuID + '.' + m_sPopup;
		break;
	case RST_MENUITEM:
		return m_sMenuID + '.' + m_sPopup + '.' + m_sID;
		break;
	default:
		break;
	}

	return "";
}
/////////////////////////////////////////////////////////////////////////////
const CString CTranslateString::ToString()
{
	CString sLine;
	CString sTemp;

	// resource file
	sLine = m_sResourceFile;
	sLine += m_cDelimiter;
	// resource type
	sLine += Enum2Name(m_rst);
	sLine += m_cDelimiter;
	// resource id
	sLine += GetID();
	sLine += m_cDelimiter;
	// scan time
	sLine += m_tScan.Format("%d,%m,%Y,%H,%M,%S");
	sLine += m_cDelimiter;
	// translate time
	sLine += m_tTranslate.Format("%d,%m,%Y,%H,%M,%S");
	sLine += m_cDelimiter;
	// reinsert time
	sLine += m_tInsert.Format("%d,%m,%Y,%H,%M,%S");
	sLine += m_cDelimiter;
	// test to translate
	sLine += m_sText;
	sLine += m_cDelimiter;
	// translated text
	sLine += m_sTranslate;
	return sLine;

}
/////////////////////////////////////////////////////////////////////////////
void CTranslateString::SetTranslate(CString& sTranslate)
{
	m_sTranslate = sTranslate;
	m_tTranslate = CTime::GetCurrentTime();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTranslateString::FromString(const CString& str)
{
	CString s = str;
	CString id,temp;
	int p;
	int day,month,year,hour,minute,second;

	// resource file
	p = s.Find(m_cDelimiter);
	if (p==-1)
	{
		return FALSE;
	}
	m_sResourceFile = s.Left(p);
	s = s.Mid(p+1);

	// resource type
	p = s.Find(m_cDelimiter);
	if (p==-1)
	{
		return FALSE;
	}
	m_rst = Name2Enum(s.Left(p));
	s = s.Mid(p+1);

	// resource id
	p = s.Find(m_cDelimiter);
	if (p==-1)
	{
		return FALSE;
	}
	id = s.Left(p);
	if (!IdFrom(id))
	{
		return FALSE;
	}
	s = s.Mid(p+1);
	// scan time
	p = s.Find(m_cDelimiter);
	if (p==-1)
	{
		return FALSE;
	}
	temp = s.Left(p);
	sscanf(temp,"%d,%d,%d,%d,%d,%d",&day,&month,&year,&hour,&minute,&second);
	m_tScan = CTime(year,month,day,hour,minute,second);
	s = s.Mid(p+1);
	// translate time
	p = s.Find(m_cDelimiter);
	if (p==-1)
	{
		return FALSE;
	}
	temp = s.Left(p);
	sscanf(temp,"%d,%d,%d,%d,%d,%d",&day,&month,&year,&hour,&minute,&second);
	m_tTranslate = CTime(year,month,day,hour,minute,second);
	s = s.Mid(p+1);
	// reinsert time
	p = s.Find(m_cDelimiter);
	if (p==-1)
	{
		return FALSE;
	}
	temp = s.Left(p);
	sscanf(temp,"%d,%d,%d,%d,%d,%d",&day,&month,&year,&hour,&minute,&second);
	m_tInsert = CTime(year,month,day,hour,minute,second);
	s = s.Mid(p+1);
	// test to translate
	p = s.Find(m_cDelimiter);
	if (p==-1)
	{
		return FALSE;
	}
	m_sText = s.Left(p);
	s = s.Mid(p+1);
	// translated text
	m_sTranslate = s;
	m_sFullID = CalcID();
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTranslateString::IdFrom(const CString& id)
{
	int p;
	CString sid=id;

	switch (m_rst)
	{
	case RST_STRING:
		m_sID = sid;
		break;
	case RST_CAPTION:
		p = sid.Find(".CAPTION");
		if (p==-1)
		{
			return FALSE;
		}
		m_sDialogID = sid.Left(p);
		break;
	case RST_LTEXT:	
	case RST_CTEXT:
	case RST_RTEXT:
	case RST_DEFPUSHBUTTON:
	case RST_PUSHBUTTON:
	case RST_GROUPBOX:
	case RST_CONTROL:
	case RST_PUSHBOX:
	case RST_SCROLLBAR:
	case RST_STATE3:
		p = sid.Find('.');
		if (p==-1)
		{
			return FALSE;
		}
		m_sDialogID = sid.Left(p);
		m_sID = sid.Mid(p+1);
		break;
	case RST_POPUP:
		p = sid.Find('.');
		if (p==-1)
		{
			return FALSE;
		}
		m_sMenuID = sid.Left(p);
		m_sPopup = sid.Mid(p+1);
		break;
	case RST_MENUITEM:
		p = sid.Find('.');
		if (p==-1)
		{
			return FALSE;
		}
		m_sMenuID = sid.Left(p);
		sid = sid.Mid(p+1);
		p = sid.Find('.');
		if (p==-1)
		{
			return FALSE;
		}
		m_sPopup = sid.Left(p);
		m_sID = sid.Mid(p+1);
		break;
	default:
		break;
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CTranslateString::AutomaticTranslate(const CMapStringToString& mapAutomatic)
{
	if ( (m_sText.GetLength()<2) ||
		 (m_sText.SpanIncluding("0123456789")==m_sText) ||
		 (m_sText.SpanIncluding("<>,.: +-*/\\!\"\'§$%&()=?[]{}#_;")==m_sText) ||
		 (m_sText.SpanIncluding(" ")==m_sText) ||
		 (m_sText == "List1") ||
		 (m_sText == "Progress1") ||
		 (m_sText == "Slider1")
	   )
	{
		m_sTranslate = m_sText;
		m_tTranslate = CTime::GetCurrentTime();
	}
	else
	{
		if (m_sTranslate.IsEmpty() || 0==m_sTranslate.Find("TODO"))
		{
			CString sValue;
			if (mapAutomatic.Lookup(m_sText,sValue))
			{
				m_sTranslate = sValue;
				TRACE("AUTO <%s> -> <%s>\n",m_sText,m_sTranslate);
				m_tTranslate = CTime::GetCurrentTime();
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CTranslateString::CountWords()
{
	m_iNumOfWords = 0;
	int i;
	BOOL bDeli;

	bDeli = FALSE;

	for (i=0;i<m_sText.GetLength();i++)
	{
		if (isalpha(m_sText[i])!=0)
		{
			if (!bDeli)
			{
				m_iNumOfWords++;
			}
			bDeli = TRUE;
		}
		else
		{
			bDeli = FALSE;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CTSArray::CTSArray()
{
	m_iNumOfWords = 0;
}
/////////////////////////////////////////////////////////////////////////////
CTSArray::~CTSArray()
{
	DeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
void CTSArray::DeleteAll()
{
	for (int i=0;i<GetSize();i++) 
	{ 
		if (GetAt(i)) 
		{ 
			delete (CTranslateString*) GetAt(i); 
		} 
	} 
	RemoveAll(); 
	m_iNumOfWords = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTSArray::ExportAsGlossary(const CString& sPathName)
{
	CFile file;
	CFileException cfe;
	CString line,sT;
	int i;

	TRACE("exporting glossary %s\n",(const char*)sPathName);
	if (file.Open(sPathName,CFile::modeCreate|CFile::modeWrite|CFile::modeNoTruncate ,&cfe))
	{
		file.SeekToEnd();
		for (i=0;i<GetSize();i++)
		{
			sT = GetAt(i)->GetTranslate();
			if (-1!=sT.Find(','))
			{
				sT = '\"' + sT + '\"';
			}
			line = sT + ",,";
			sT = GetAt(i)->GetText();
			if (-1!=sT.Find(','))
			{
				sT = '\"' + sT + '\"';
			}
			line += sT + ",,\n";
			file.Write((const char*)line,line.GetLength());
		}
		file.Flush();
		file.Close();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTSArray::Save(const CString& sFilename)
{
	CFile file;
	CFileException cfe;
	CString line;
	int i;

	TRACE("saving file %s\n",(const char*)sFilename);
	if (file.Open(sFilename,CFile::modeCreate|CFile::modeWrite,&cfe))
	{
		for (i=0;i<GetSize();i++)
		{
			line = GetAt(i)->ToString() + "\n";
			file.Write((const char*)line,line.GetLength());
		}
		file.Close();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTSArray::Load(const CString& sFilename)
{
	CStdioFile file;
	CFileException cfe;
	CString line;
	CTranslateString* pTS;
	int p;

	DeleteAll();

	p = sFilename.ReverseFind('\\');
	m_sName = sFilename.Mid(p+1);
	TRACE("loading file %s\n",(const char*)sFilename);

	if (!file.Open(sFilename,CFile::modeRead,&cfe))
	{
		return FALSE;
	}

	while (file.ReadString(line))
	{
		pTS = new CTranslateString();
		if (pTS->FromString(line))
		{
			Add(pTS);
		}
		else
		{
			TRACE("FromString Failed %s\n",line);
			delete pTS;
		}
	}
	file.Close();
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CTSArray::AddReplace(CTranslateString* pTS)
{
	int i;

	m_iNumOfWords += pTS->GetNumOfWords();

	for (i=0;i<GetSize();i++)
	{
		if (pTS->GetID() == GetAt(i)->GetID())
		{
			GetAt(i)->Update(pTS);
			delete pTS;
			return;
		}
	}
	Add(pTS);
}
/////////////////////////////////////////////////////////////////////////////
CTranslateString* CTSArray::GetTSFromID(const CString& id)
{
	int i;

	for (i=0;i<GetSize();i++)
	{
		if (id== GetAt(i)->GetID())
		{
			return GetAt(i);
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CTSArray::AutomaticTranslate(const CMapStringToString& mapAutomatic)
{
	int i;

	for (i=0;i<GetSize();i++)
	{
		GetAt(i)->AutomaticTranslate(mapAutomatic);
	}
}

