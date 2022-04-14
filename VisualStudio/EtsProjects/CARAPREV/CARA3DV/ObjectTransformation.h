// ObjectTransformation.h: Schnittstelle für die Klasse CObjectTransformation.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTTRANSFORMATION_H__D4B90FC2_F8B6_11D2_9E4E_0000B458D891__INCLUDED_)
#define AFX_OBJECTTRANSFORMATION_H__D4B90FC2_F8B6_11D2_9E4E_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CObjectTransformation  
{
public:
	void SetPolarGridTransformation();
	CObjectTransformation();
	void SetTransformation();

	CVector m_vAnchor;
	CVector m_vTranslation;
	CVector m_vScale;
	double  m_dTheta;
	double  m_dPhi;
};

#endif // !defined(AFX_OBJECTTRANSFORMATION_H__D4B90FC2_F8B6_11D2_9E4E_0000B458D891__INCLUDED_)
