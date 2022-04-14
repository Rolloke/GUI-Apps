// QueryParameter.cpp: implementation of the CQueryParameter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IdipClient.h"
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
	m_bShowPictures = wkp.GetInt(sSection,_T("ShowPictures"),TRUE);
	m_bCopyPictures = wkp.GetInt(sSection,_T("CopyPictures"),TRUE);
	m_bExactDate = wkp.GetInt(sSection,_T("ExactDate"),FALSE);
	m_bExactTime = wkp.GetInt(sSection,_T("ExactTime"),FALSE);
	m_bTimeFromTo = wkp.GetInt(sSection,_T("TimeFromTo"),FALSE);
	m_bAll = wkp.GetInt(sSection,_T("All"),FALSE);

	m_Fields.Load(sSection,wkp);

	if (m_bExactDate)
	{
		m_stDateExact.FromString(wkp.GetString(sSection,_T("DateExact"),_T("")));
	}
	if (m_bExactTime)
	{
		m_stTimeExact.FromString(wkp.GetString(sSection,_T("TimeExact"),_T("")));
	}
	if (m_bTimeFromTo)
	{
		if(m_bExactDate)
		{
			m_stPCDateEnd.FromString(wkp.GetString(sSection,_T("PCDateEnd"), _T("")));
		}
		if(m_bExactTime)
		{
			m_stTimeEnd.FromString(wkp.GetString(sSection,_T("TimeEnd"), _T("")));
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////
void CQueryParameter::Save(const CString& sSection, CWK_Profile& wkp)
{
	wkp.WriteInt(sSection,_T("ShowPictures"),m_bShowPictures);
	wkp.WriteInt(sSection,_T("CopyPictures"),m_bCopyPictures);
	wkp.WriteInt(sSection,_T("ExactDate"),m_bExactDate);
	wkp.WriteInt(sSection,_T("ExactTime"),m_bExactTime);
	wkp.WriteInt(sSection,_T("TimeFromTo"),m_bTimeFromTo);
	wkp.WriteInt(sSection,_T("All"),m_bAll);

	m_Fields.Save(sSection,wkp);

	if (m_bExactDate)
	{
		wkp.WriteString(sSection,_T("DateExact"), (LPCTSTR)m_stDateExact.ToString());
		if (m_bTimeFromTo)
		{
			wkp.WriteString(sSection,_T("PCDateEnd"), (LPCTSTR)m_stPCDateEnd.ToString());
		}
	}
	else
	{
		wkp.DeleteEntry(sSection, _T("DateExact"));
		wkp.DeleteEntry(sSection, _T("PCDateEnd"));
	}

	if (m_bExactTime)
	{
		wkp.WriteString(sSection,_T("TimeExact"), (LPCTSTR)m_stTimeExact.ToString());
		if (m_bTimeFromTo)
		{
			wkp.WriteString(sSection,_T("TimeEnd"), (LPCTSTR)m_stTimeEnd.ToString());
		}
	}
	else
	{
		wkp.DeleteEntry(sSection, _T("TimeExact"));
		wkp.DeleteEntry(sSection, _T("TimeEnd"));
	}

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
			sArchives += _T(" .OR. ");
		}
		else
		{
			sArchives = _T("(");
		}
		s.Format(_T("(ARC_NR = %d)"),m_Archives.GetAt(i));
		sArchives += s;
	}
	if (!sArchives.IsEmpty())
	{
		sArchives += _T(")");
	}
	if (!sArchives.IsEmpty())
	{
		sArchives += _T(" .AND. ");
	}
	return sArchives;
}
