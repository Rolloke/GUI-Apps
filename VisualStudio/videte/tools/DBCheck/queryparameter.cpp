// QueryParameter.cpp: implementation of the CQueryParameter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DBCheck.h"
#include "QueryParameter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQueryParameter::CQueryParameter()
{
	m_bShowPictures = TRUE;
	m_bCopyPictures = TRUE;
	m_bExactDate = FALSE;
	m_bExactTime = FALSE;
	m_bTimeFromTo = FALSE;
	m_bAll = FALSE;

	//init rectangle to search for
	m_RectToQuery.top		= 0;
	m_RectToQuery.left		= 0;
	m_RectToQuery.bottom	= 0;
	m_RectToQuery.right		= 0;
}

CQueryParameter::~CQueryParameter()
{
}
///////////////////////////////////////////////////////////////////////////////////////
void CQueryParameter::Load(const CString& sSection, CWK_Profile& wkp)
{
	m_bShowPictures = wkp.GetInt(sSection,"ShowPictures",TRUE);
	m_bCopyPictures = wkp.GetInt(sSection,"CopyPictures",TRUE);
	m_bExactDate = wkp.GetInt(sSection,"ExactDate",FALSE);
	m_bExactTime = wkp.GetInt(sSection,"ExactTime",FALSE);
	m_bTimeFromTo = wkp.GetInt(sSection,"TimeFromTo",FALSE);
	m_bAll = wkp.GetInt(sSection,"All",FALSE);

	m_Fields.Load(sSection,wkp);

	if (m_bExactDate)
	{
		m_stDateExact.FromString(wkp.GetString(sSection,"DateExact",""));
	}
	if (m_bExactTime)
	{
		m_stTimeExact.FromString(wkp.GetString(sSection,"TimeExact",""));
	}
	if (m_bTimeFromTo)
	{
		if(m_bExactDate)
		{
			m_stPCDateEnd.FromString(wkp.GetString(sSection,"PCDateEnd", ""));
		}
		if(m_bExactTime)
		{
			m_stTimeEnd.FromString(wkp.GetString(sSection,"TimeEnd", ""));
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////
void CQueryParameter::Save(const CString& sSection, CWK_Profile& wkp)
{
	wkp.WriteInt(sSection,"ShowPictures",m_bShowPictures);
	wkp.WriteInt(sSection,"CopyPictures",m_bCopyPictures);
	wkp.WriteInt(sSection,"ExactDate",m_bExactDate);
	wkp.WriteInt(sSection,"ExactTime",m_bExactTime);
	wkp.WriteInt(sSection,"TimeFromTo",m_bTimeFromTo);
	wkp.WriteInt(sSection,"All",m_bAll);

	m_Fields.Save(sSection,wkp);

	wkp.WriteString(sSection,"DateExact",
		m_bExactDate ? (const char*)m_stDateExact.ToString() : NULL);
	wkp.WriteString(sSection,"TimeExact",
		m_bExactTime ? (const char*)m_stTimeExact.ToString() : NULL);

	wkp.WriteString(sSection,"PCDateEnd",
		m_bExactDate && m_bTimeFromTo ? (const char*)m_stPCDateEnd.ToString() : NULL);
	wkp.WriteString(sSection,"TimeEnd",
		m_bExactTime && m_bTimeFromTo ? (const char*)m_stTimeEnd.ToString() : NULL);
}
//////////////////////////////////////////////////////////////////////
void CQueryParameter::SetAddField(const CIPCField& field)
{
	CIPCField *pField = m_Fields.GetCIPCField(field.GetName());
	
	if (pField)
	{
		CString sValue = field.GetValue();
		sValue.TrimLeft();
		sValue.TrimRight();
		sValue = CIPCField::FillLeadingZeros(sValue,field.GetMaxLength());
		pField->SetValue(sValue);
	}
	else
	{
		CIPCField *pField = new CIPCField(field);
		pField->Trim();
		pField->LeadingZeros();

		m_Fields.SafeAdd(pField);
	}
}
//////////////////////////////////////////////////////////////////////
void CQueryParameter::DeleteFields()
{
	m_Fields.SafeDeleteAll();
}
//////////////////////////////////////////////////////////////////////
void CQueryParameter::SetExactDate(BOOL bExactDate)
{
	m_bExactDate = bExactDate;
}
//////////////////////////////////////////////////////////////////////
void CQueryParameter::SetExactTime(BOOL bExactTime)
{
	m_bExactTime = bExactTime;
}
//////////////////////////////////////////////////////////////////////
void CQueryParameter::SetTimeFromTo(BOOL bTimeFromTo)
{
	m_bTimeFromTo = bTimeFromTo;
}
//////////////////////////////////////////////////////////////////////
void CQueryParameter::SetShowPictures(BOOL bShowPictures)
{
	m_bShowPictures = bShowPictures;
}
//////////////////////////////////////////////////////////////////////
void CQueryParameter::SetCopyPictures(BOOL bCopyPictures)
{
	m_bCopyPictures = bCopyPictures;
}
//////////////////////////////////////////////////////////////////////
void CQueryParameter::SetAll(BOOL bAll)
{
	m_bAll = bAll;
}
//////////////////////////////////////////////////////////////////////
void CQueryParameter::SetExactDate(const CSystemTime& t)
{
	m_stDateExact = t;
}
//////////////////////////////////////////////////////////////////////
void CQueryParameter::SetPCDateEnd(const CSystemTime& t)
{
	m_stPCDateEnd = t;
}
//////////////////////////////////////////////////////////////////////
void CQueryParameter::SetTimeEnd(const CSystemTime& t)
{
	m_stTimeEnd = t;
}
//////////////////////////////////////////////////////////////////////
void CQueryParameter::SetExactTime(const CSystemTime& t)
{
	m_stTimeExact = t;
}
//////////////////////////////////////////////////////////////////////
void CQueryParameter::SetRectToQuery(CRect rect)
{
	m_RectToQuery = rect;
}
//////////////////////////////////////////////////////////////////////
CQueryArchives::CQueryArchives()
{
}
//////////////////////////////////////////////////////////////////////
CQueryArchives::~CQueryArchives()
{
}
//////////////////////////////////////////////////////////////////////
BOOL CQueryArchives::IsArchive(WORD wArchive)
{
	int i,c;
	c = m_Archives.GetSize();
	for (i=0;i<c;i++)
	{
		if (m_Archives[i]==wArchive)
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void CQueryArchives::AddArchive(WORD wArchive)
{
	if (!IsArchive(wArchive))
	{
		m_Archives.Add(wArchive);
	}
}
//////////////////////////////////////////////////////////////////////
void CQueryArchives::DeleteArchives()
{
	m_Archives.RemoveAll();
}
//////////////////////////////////////////////////////////////////////
CString CQueryArchives::GetOldArchives()
{
	CString sArchives,s;

	for (int i=0;i<m_Archives.GetSize();i++)
	{
		if (!sArchives.IsEmpty())
		{
			sArchives += " .OR. ";
		}
		else
		{
			sArchives = "(";
		}
		s.Format("(ARC_NR = %d)",m_Archives.GetAt(i));
		sArchives += s;
	}
	if (!sArchives.IsEmpty())
	{
		sArchives += ")";
	}
	if (!sArchives.IsEmpty())
	{
		sArchives += " .AND. ";
	}
	return sArchives;
}
