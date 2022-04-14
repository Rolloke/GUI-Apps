/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: wk_debugoptions.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/wk_debugoptions.cpp $
// CHECKIN:		$Date: 20.01.06 9:28 $
// VERSION:		$Revision: 11 $
// LAST CHANGE:	$Modtime: 19.01.06 20:28 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdcipc.h"

#include "wk_debugoptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CWKDebugOptions::SetValue(const CString &sKey, int iValue,
									  LPCTSTR szComment)
{
	for (int i=0;i<m_keys.GetSize();i++) 
	{
		if (sKey==m_keys[i]) 
		{
			m_values.SetAt(i,(DWORD)iValue);
			if (szComment && _tcslen(szComment))
			{
				m_comments.SetAt(i,szComment);
			}
			return;
		}
	}
	m_keys.Add(sKey);
	m_values.Add((DWORD)iValue);
	m_comments.Add(szComment);
}
// unknown keys return the default
int CWKDebugOptions::GetValue(const CString &sKey,int iDefault)		// slow !
{
	for (int i=0;i<m_keys.GetSize();i++) 
	{
		if ( sKey.CompareNoCase(m_keys[i])==0 )
		{
			return (int)m_values[i];	// EXIT
		}
	}
	// add it
	SetValue(sKey,iDefault);
	return iDefault;
}

BOOL ReadString(CFile& file, CString& s)
{
	char c;

	s.Empty();

	while (file.Read(&c,sizeof(c)))
	{
		if (c == _T('\n'))
		{
			break;
		}
		else if (c == _T('\r'))
		{
		}
		else
		{
			s += c;
		}
	}

	return !s.IsEmpty();
}

BOOL CWKDebugOptions::ReadFromFile(CFile& file)
{
	BOOL bOkay = FALSE;
	CString sOneLine;

	TRY
	{
		file.SeekToBegin();

		while (ReadString(file,sOneLine))
		{
			int ic = sOneLine.Find(';');
			CString sComment;
			if (ic != -1)
			{	// strip ; comment
				sComment = sOneLine.Mid(ic+1);
				sOneLine = sOneLine.Left(ic);
			}
			int ix = sOneLine.Find('=');
			if (ix != -1) 
			{
				CString sKey = sOneLine.Left(ix);
				CString sValue = sOneLine.Right(sOneLine.GetLength()-ix-1);
				int iValue;
				sKey.TrimLeft();
				sKey.TrimRight();
				sValue.TrimLeft();
				sValue.TrimRight();
				if (sValue==_T("TRUE") || sValue==_T("ON")) iValue=1;
				else if (sValue==_T("FALSE") || sValue==_T("OFF")) iValue=0;
				else iValue =  _ttoi(sValue);
				if (sKey)
				{
					// use SetValue, which also checks for duplicates
					SetValue(sKey, iValue, sComment);
				}
				else 
				{
					// found = but empty key
				}
			}
			else 
			{
				// no = found, might be an empty comment line
				if (sComment.GetLength()) 
				{
					m_keys.Add(_T(""));
					m_values.Add(0);
					m_comments.Add(sComment);
				}
			}
		}
	}
	WK_CATCH(_T("cannot read debug options"))
	bOkay=TRUE;
	return bOkay;
}


BOOL CWKDebugOptions::ReadFromFile(const CString &sFilename)
{
	BOOL bOkay = FALSE;
#ifdef _UNICODE
	CStdioFileU f;
#else
	CStdioFile f;
#endif
	CString sOneLine;

	m_sFilename = sFilename;
	if (f.Open(sFilename,CFile::modeRead | CFile::shareDenyNone))
	{
		TRY
		{
		while (f.ReadString(sOneLine))
		{
			int ic = sOneLine.Find(';');
			CString sComment;
			if (ic != -1)
			{	// strip ; comment
				sComment = sOneLine.Mid(ic+1);
				sOneLine = sOneLine.Left(ic);
			}
			int ix = sOneLine.Find('=');
			if (ix != -1) 
			{
				CString sKey = sOneLine.Left(ix);
				CString sValue = sOneLine.Right(sOneLine.GetLength()-ix-1);
				int iValue;
				sKey.TrimLeft();
				sKey.TrimRight();
				sValue.TrimLeft();
				sValue.TrimRight();
				if (sValue==_T("TRUE") || sValue==_T("ON")) iValue=1;
				else if (sValue==_T("FALSE") || sValue==_T("OFF")) iValue=0;
				else iValue =  _ttoi(sValue);
				if (sKey)
				{
					// use SetValue, which also checks for duplicates
					SetValue(sKey, iValue, sComment);
				}
				else 
				{
					// found = but empty key
				}
			}
			else 
			{
				// no = found, might be an empty comment line
				if (sComment.GetLength()) 
				{
					m_keys.Add(_T(""));
					m_values.Add(0);
					m_comments.Add(sComment);
				}
			}
		}
		f.Close();
		}
		WK_CATCH(_T("cannot read debug options"))

		bOkay=TRUE;
	} 
	else
	{
		bOkay=FALSE;
	}
	return bOkay;
}

BOOL CWKDebugOptions::WriteToFile(const CString &sFilename)
{
#ifdef _UNICODE
	CStdioFileU f;
	f.SetFileMCBS(CP_ACP);
#else
	CStdioFile f;
#endif
	BOOL bOkay=TRUE;

	if (f.Open(sFilename,CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone)) 
	{
		TRY	
		{
			for (int i=0;i<m_keys.GetSize();i++)
			{
				CString sOneLine;
				if (m_keys[i].IsEmpty())
				{
					sOneLine=_T("\t;");
					sOneLine += m_comments[i];
				}
				else 
				{
					sOneLine.Format(_T("%s = %d"),m_keys[i],m_values[i]);
					if (m_comments[i].GetLength()) 
					{
						sOneLine += _T("\t;");
						sOneLine += m_comments[i];
					}
				}
				sOneLine += '\n';
				f.WriteString(sOneLine);
			}
			f.Close();
		}
		WK_CATCH(_T("cannot write debugoptions"))
	} else {
		bOkay=FALSE;
	}
	return bOkay;
}

void CWKDebugOptions::Clear()
{
	m_keys.RemoveAll();
	m_values.RemoveAll();
	m_comments.RemoveAll();
}
