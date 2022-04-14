// VDBCameraMap.cpp: implementation of the CVDBCameraMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dbs.h"
#include "VDBCameraMap.h"
#include ".\vdbcameramap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static char szCameraID[] = "CAM_ID";
static char szCameraName[] = "NAME";
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CVDBCameraMap::CVDBCameraMap()
	: m_Fields(m_CodeBase)
{
	m_CodeBase.safety = FALSE;
	m_CodeBase.errOff = TRUE;
	m_CodeBase.accessMode = OPEN4DENY_NONE;
	m_CodeBase.optimize = OPT4OFF;
	m_CodeBase.optimizeWrite = OPT4OFF;

	m_Fields.add(szCameraID,'C',8,0);
	m_Fields.add(szCameraName,'C',32,0);
}
//////////////////////////////////////////////////////////////////////
CVDBCameraMap::~CVDBCameraMap()
{
	m_CodeBase.initUndo();
}
//////////////////////////////////////////////////////////////////////
void CVDBCameraMap::Load(const CString& sPathname)
{
	Data4 data;

	if (DoesFileExist(sPathname))
	{
		WK_TRACE(_T("camera names is %s\n"),sPathname);
		if (r4success == data.open(m_CodeBase, CWK_String(sPathname)))
		{
			WK_TRACE(_T("camera names opened %s %d records\n"),sPathname,data.recCount());
			for (data.top();!data.eof();data.skip())
			{
				Field4 c_id(data,szCameraID);

				if (c_id.isValid())
				{
					CWK_String sCameraID(c_id.str());

					Field4 c_name(data,szCameraName);
					if (c_name.isValid())
					{
						CWK_String sCameraName(c_name.str());
						sCameraName = sCameraName.TrimRight();
						SetAt(sCameraID,sCameraName);
					}
				}
				if (m_CodeBase.errorCode != 0)
				{
					WK_TRACE(_T("codebase error %d reading camera names file\n"),m_CodeBase.errorCode);
					m_CodeBase.errorSet();
				}
			}
			WK_TRACE(_T("loaded camera names %d entries\n"),GetCount());
			data.close();
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CVDBCameraMap::Save(const CString& sPathname)
{
	Data4 data;
	int rc;

	WK_TRACE(_T("saving %d camera names to %s\n"),GetCount(),sPathname);

	if (DoesFileExist(sPathname))
	{
		DeleteFile(sPathname);
	}

	rc = data.create(m_CodeBase,CWK_String(sPathname),m_Fields);
	if (r4success == rc)
	{
		POSITION pos = GetStartPosition();
		
		CWK_String sKey,sEntry;
		while (pos != NULL)
		{
			sKey.CleanUp();
			sEntry.CleanUp();
			GetNextAssoc(pos, sKey, sEntry);
			
			if (   !sKey.IsEmpty() 
				&& !sEntry.IsEmpty()
				&& r4success == data.appendStart())
			{
				data.blank();
				
				Field4 c_id(data,szCameraID);
				if (c_id.isValid())
				{
					c_id.assign(sKey);

					Field4 c_name(data,szCameraName);
					if (c_name.isValid())
					{
						c_name.assign(sEntry);
					}
				}
				if (m_CodeBase.errorCode == 0)
				{
					data.append();
				}
				if (m_CodeBase.errorCode != 0)
				{
					WK_TRACE(_T("codebase error %d writing camera names file\n"),m_CodeBase.errorCode);
					m_CodeBase.errorSet();
				}
				
			}
			else
			{
				WK_TRACE(_T("cannot appendStart loading camera names %d\n"),m_CodeBase.errorCode);
				m_CodeBase.errorSet();
			}
		}
		data.close();
		WK_TRACE(_T("saved camera names %d entries\n"),GetCount());
	}
	else
	{
		WK_TRACE_ERROR(_T("cannot create camera names to %s %d\n"),sPathname,rc);
		m_CodeBase.errorSet();
		DWORD dwError=theApp.TranslateDBError(rc);
		if (dwError)
		{
			theApp.HandleDriveError(dwError, sPathname.Left(3));
		}
	}
}

void CVDBCameraMap::DeleteContent(void)
{
	// TODO! RKE: delete Database content?
	RemoveAll();	// Delete Map Content
}
