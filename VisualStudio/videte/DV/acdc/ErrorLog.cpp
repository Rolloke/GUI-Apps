// ErrorLog.cpp: implementation of the ErrorLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "acdc.h"
#include "ErrorLog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Constructor

CErrorLog::CErrorLog ()
{
	m_fh = NULL;
}


// Destructor

CErrorLog::~CErrorLog ()
{
	if (m_fh)
	{
		fclose (m_fh);
	}
}

// Open logfile

bool CErrorLog::Open (LPCSTR psFilename)
{
	if (NULL != psFilename)
	{
		m_fh = fopen (psFilename, "wt");
	}
	
	TRACE("m_fh: 0x%x \n", m_fh);
	return m_fh != NULL;
}

// Add one line to the logfile

int CErrorLog::printf (const char * format, ...)
{
	va_list args;
	
	va_start (args, format);

	if (NULL != m_fh)
	{
		vfprintf (m_fh, format, args);
	}

	int ret = vprintf (format, args);

	va_end (args);

	return ret;
}

