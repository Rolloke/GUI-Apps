// FilterProperties.cpp : implementation file
//

#include "stdafx.h"
#include "GraphCreator.h"
#include "FilterProperties.h"
#include ".\filterproperties.h"


// CFilterProperties dialog

IMPLEMENT_DYNAMIC(CFilterProperties, CPropertyPage)
CFilterProperties::CFilterProperties()
	: CPropertyPage(CFilterProperties::IDD)
{
}

CFilterProperties::~CFilterProperties()
{
}

void CFilterProperties::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFilterProperties, CPropertyPage)
END_MESSAGE_MAP()


// CFilterProperties message handlers

BOOL CFilterProperties::OnInitDialog()
{
   CPropertyPage::OnInitDialog();


   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}
