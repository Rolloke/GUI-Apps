// FindFiles.cpp: implementation of the FindFiles class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "acdc.h"
#include "FindFiles.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Constructor
CFindFiles::CFindFiles (LPCSTR psPath)
{
	m_lHandle = _findfirst (psPath, &m_fd);

	if (-1 != m_lHandle)
	{
		m_bValid = true;
	}
	else
	{
		m_bValid = false;
	}
}


// Destructor
CFindFiles::~CFindFiles ()
{
	if (m_lHandle != -1)
	{
		_findclose (m_lHandle);
	}
}
