// ObjectTransformation.cpp: Implementierung der Klasse CObjectTransformation.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ObjectTransformation.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CObjectTransformation::CObjectTransformation()
{
   m_vTranslation  = CVector(0.0, 0.0, -5.0);
   m_vScale        = CVector(1.0, 1.0,  1.0);
   m_vAnchor       = CVector(0.0, 0.0,  0.0);
   m_dPhi          = 20.0;
   m_dTheta        = 55.0;
}

void CObjectTransformation::SetTransformation()
{
   CVector vTran = m_vTranslation + m_vAnchor;
   glTranslated( Vx(vTran)    ,  Vy(vTran)    ,  Vz(vTran));
   glRotated(m_dTheta, 1.0, 0.0, 0.0);
   glRotated(m_dPhi  , 0.0, 1.0, 0.0);
   glScaled(     Vx(m_vScale) ,  Vy(m_vScale) ,  Vz(m_vScale));
   glTranslated(-Vx(m_vAnchor), -Vy(m_vAnchor), -Vz(m_vAnchor));
}


void CObjectTransformation::SetPolarGridTransformation()
{
   CVector vTran = m_vTranslation + m_vAnchor;
   glTranslated( Vx(vTran)    ,  Vy(vTran)    ,  Vz(vTran));
   glScaled(     Vx(m_vScale) ,  Vy(m_vScale) ,  Vz(m_vScale));
   glTranslated(-Vx(m_vAnchor), -Vy(m_vAnchor), -Vz(m_vAnchor));
}
