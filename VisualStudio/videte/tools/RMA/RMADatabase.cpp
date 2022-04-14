// RMADatabase.cpp: implementation of the CRMADatabase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RMADatabase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRMADatabase::CRMADatabase()
	: m_fieldinfo(m_cb)
{
	m_cb.safety = FALSE;
	m_cb.errOff = TRUE;
	m_cb.accessMode = OPEN4DENY_RW;
	m_cb.optimize = OPT4OFF;
	m_cb.optimizeWrite = OPT4OFF;

	m_fieldinfo.add("Company",'C',50,0);
	m_fieldinfo.add("BillNr",'C',20,0);
	m_fieldinfo.add("City",'C',50,0);
	m_fieldinfo.add("Company",'C',50,0);
	m_fieldinfo.add("Country",'C',50,0);
	m_fieldinfo.add("Date",'C',8,0);
	m_fieldinfo.add("EMail",'C',50,0);
	m_fieldinfo.add("Error",'C',255,0);
	m_fieldinfo.add("Fax",'C',50,0);
	m_fieldinfo.add("Fon",'C',50,0);
	m_fieldinfo.add("Name",'C',50,0);
	m_fieldinfo.add("PostalCode",'C',5,0);
	m_fieldinfo.add("RMANr",'C',10,0);
	m_fieldinfo.add("Serial",'C',10,0);
	m_fieldinfo.add("Street",'C',50,0);
	m_fieldinfo.add("Type",'C',20,0);
}

CRMADatabase::~CRMADatabase()
{
	m_cb.initUndo();
}
