// CValue.cpp: implementation of the CValue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Simulator.h"
#include "CValues.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CValues::CValues()
{

}

CValues::~CValues()
{

}

void CValues::Init(const CString sName, int nNr)
{
	m_sName = sName;
	m_nNr	= nNr;
}

CString CValues::GetName()
{
	return m_sName;
}

int CValues::GetNr()
{
	return m_nNr;
}
