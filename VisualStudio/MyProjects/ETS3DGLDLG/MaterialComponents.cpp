// MaterialComponents.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ETS3DGLDLG.h"
#include "MaterialComponents.h"
#include "CCaraMat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CMaterialComponents 


CMaterialComponents::CMaterialComponents(CWnd* pParent /*=NULL*/)
	: CDialog(CMaterialComponents::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMaterialComponents)
	m_bBorder = FALSE;
	m_bGenerateCoordinates = true;
	m_fShinines = 0.0f;
	m_nMatNo = 0;
	m_fsScaleFactor = 2.0f;
	m_ftScaleFactor = 2.0f;
	//}}AFX_DATA_INIT
	m_pfAmbient[0] = 0.7f;
	m_pfAmbient[1] = 0.7f;
	m_pfAmbient[2] = 0.7f;
	m_pfAmbient[3] = 1.0f;
	m_pfDiffuse[0] = 0.7f;
	m_pfDiffuse[1] = 0.7f;
	m_pfDiffuse[2] = 0.7f;
	m_pfDiffuse[3] = 1.0f;
	m_pfEmission[0] = 0.0f;
	m_pfEmission[1] = 0.0f;
	m_pfEmission[2] = 0.0f;
	m_pfEmission[3] = 1.0f;
	m_pfSpecular[0] = 0.0f;
	m_pfSpecular[1] = 0.0f;
	m_pfSpecular[2] = 0.0f;
	m_pfSpecular[3] = 1.0f;
}


void CMaterialComponents::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMaterialComponents)
	DDX_Check(pDX, IDC_BORDER, m_bBorder);
	DDX_Check(pDX, IDC_GENERATE_TEX_COORD, m_bGenerateCoordinates);
	//}}AFX_DATA_MAP

	DDX_TextVar(pDX, IDC_SHININESS, FLT_DIG, m_fShinines);
	DDV_MinMaxFloat(pDX, m_fShinines, 0.f, 128.f);
	DDX_TextVar(pDX, IDC_SCALEFACTOR_S, FLT_DIG, m_fsScaleFactor);
	DDV_MinMaxFloat(pDX, m_fsScaleFactor, 1.e-003f, 1000.f);
	DDX_TextVar(pDX, IDC_SCALEFACTOR_T, FLT_DIG, m_ftScaleFactor);
	DDV_MinMaxFloat(pDX, m_ftScaleFactor, 1.e-003f, 1000.f);

   DDX_TextVar(pDX, IDC_RED_AMBIENT, FLT_DIG, m_pfAmbient[0]);     // Red
	DDV_MinMaxFloat(pDX, m_pfAmbient[0], 0.f, 1.f);
	DDX_TextVar(pDX, IDC_RED_DIFFUSE, FLT_DIG, m_pfDiffuse[0]);
	DDV_MinMaxFloat(pDX, m_pfDiffuse[0], 0.f, 1.f);
	DDX_TextVar(pDX, IDC_RED_EMISSION, FLT_DIG, m_pfEmission[0]);
	DDV_MinMaxFloat(pDX, m_pfEmission[0], 0.f, 1.f);
	DDX_TextVar(pDX, IDC_RED_SPECULAR, FLT_DIG, m_pfSpecular[0]);
	DDV_MinMaxFloat(pDX, m_pfSpecular[0], 0.f, 1.f);

   DDX_TextVar(pDX, IDC_GREEN_AMBIENT, FLT_DIG, m_pfAmbient[1]);     // Green
	DDV_MinMaxFloat(pDX, m_pfAmbient[1], 0.f, 1.f);
	DDX_TextVar(pDX, IDC_GREEN_DIFFUSE, FLT_DIG, m_pfDiffuse[1]);
	DDV_MinMaxFloat(pDX, m_pfDiffuse[1], 0.f, 1.f);
	DDX_TextVar(pDX, IDC_GREEN_EMISSION, FLT_DIG, m_pfEmission[1]);
	DDV_MinMaxFloat(pDX, m_pfEmission[1], 0.f, 1.f);
	DDX_TextVar(pDX, IDC_GREEN_SPECULAR, FLT_DIG, m_pfSpecular[1]);
	DDV_MinMaxFloat(pDX, m_pfSpecular[1], 0.f, 1.f);

	DDX_TextVar(pDX, IDC_BLUE_AMBIENT, FLT_DIG, m_pfAmbient[2]);     // Blue
	DDV_MinMaxFloat(pDX, m_pfAmbient[2], 0.f, 1.f);
	DDX_TextVar(pDX, IDC_BLUE_DIFFUSE, FLT_DIG, m_pfDiffuse[2]);
	DDV_MinMaxFloat(pDX, m_pfDiffuse[2], 0.f, 1.f);
	DDX_TextVar(pDX, IDC_BLUE_EMISSION, FLT_DIG, m_pfEmission[2]);
	DDV_MinMaxFloat(pDX, m_pfEmission[2], 0.f, 1.f);
	DDX_TextVar(pDX, IDC_BLUE_SPECULAR, FLT_DIG, m_pfSpecular[2]);
	DDV_MinMaxFloat(pDX, m_pfSpecular[2], 0.f, 1.f);

 	DDX_TextVar(pDX, IDC_ALPHA_AMBIENT, FLT_DIG, m_pfAmbient[3]);     // Alpha
	DDV_MinMaxFloat(pDX, m_pfAmbient[3], 0.f, 1.f);
	DDX_TextVar(pDX, IDC_ALPHA_DIFFUSE, FLT_DIG, m_pfDiffuse[3]);
	DDV_MinMaxFloat(pDX, m_pfDiffuse[3], 0.f, 1.f);
	DDX_TextVar(pDX, IDC_ALPHA_EMISSION, FLT_DIG, m_pfEmission[3]);
	DDV_MinMaxFloat(pDX, m_pfEmission[3], 0.f, 1.f);
	DDX_TextVar(pDX, IDC_ALPHA_SPECULAR, FLT_DIG, m_pfSpecular[3]);
	DDV_MinMaxFloat(pDX, m_pfSpecular[3], 0.f, 1.f);
}


BEGIN_MESSAGE_MAP(CMaterialComponents, CDialog)
	//{{AFX_MSG_MAP(CMaterialComponents)
	ON_BN_CLICKED(IDC_BORDER_COLOR, OnBorderColor)
	ON_BN_CLICKED(IDC_AMBIENT_LIGHT, OnAmbientLight)
	ON_BN_CLICKED(IDC_DIFFUSE_LIGHT, OnDiffuseLight)
	ON_BN_CLICKED(IDC_SPECULAR_LIGHT, OnSpecularLight)
	ON_BN_CLICKED(IDC_EMISSION_LIGHT, OnEmissionLight)
	ON_BN_CLICKED(IDC_BITMAP_FILE, OnBitmapFile)
	ON_BN_CLICKED(IDC_SELECT_MATERIAL, OnSelectMaterial)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CMaterialComponents 

void CMaterialComponents::OnOK() 
{
   if (SaveMaterial())
      CDialog::OnOK();
}

void CMaterialComponents::OnCancel() 
{
   char szCurrenDir[_MAX_PATH];
   CString str;
   GetCurrentDirectory(_MAX_PATH, szCurrenDir);
   str.Format("%s\\Bmp\\Material%d.CMP", szCurrenDir, m_nMatNo);
   ::DeleteFile(str);	
	CDialog::OnCancel();
}

void CMaterialComponents::OnBorderColor() 
{
	CColorDialog cd(m_cBorder, CC_RGBINIT, this);
   if (cd.DoModal()==IDOK)
   {
      m_cBorder = cd.m_cc.rgbResult;
   }
}

void CMaterialComponents::OnAmbientLight() 
{
   COLORREF col = RGB(m_pfAmbient[0]*255,	m_pfAmbient[1]*255,m_pfAmbient[2]*255);
	CColorDialog cd(col, CC_RGBINIT, this);
   if (cd.DoModal()==IDOK)
   {
      col = cd.m_cc.rgbResult;
      m_pfAmbient[0] = (float)GetRValue(col)/255.0f;
      m_pfAmbient[1] = (float)GetGValue(col)/255.0f;
      m_pfAmbient[2] = (float)GetBValue(col)/255.0f;
      UpdateData(false);
   }
}

void CMaterialComponents::OnDiffuseLight() 
{
   COLORREF col = RGB(m_pfDiffuse[0]*255,	m_pfDiffuse[1]*255,m_pfDiffuse[2]*255);
	CColorDialog cd(col, CC_RGBINIT, this);
   if (cd.DoModal()==IDOK)
   {
      col = cd.m_cc.rgbResult;
      m_pfDiffuse[0] = (float)GetRValue(col)/255.0f;
      m_pfDiffuse[1] = (float)GetGValue(col)/255.0f;
      m_pfDiffuse[2] = (float)GetBValue(col)/255.0f;
      UpdateData(false);
   }
}

void CMaterialComponents::OnSpecularLight() 
{
   COLORREF col = RGB(m_pfSpecular[0]*255,	m_pfSpecular[1]*255,m_pfSpecular[2]*255);
	CColorDialog cd(col, CC_RGBINIT, this);
   if (cd.DoModal()==IDOK)
   {
      col = cd.m_cc.rgbResult;
      m_pfSpecular[0] = (float)GetRValue(col)/255.0f;
      m_pfSpecular[1] = (float)GetGValue(col)/255.0f;
      m_pfSpecular[2] = (float)GetBValue(col)/255.0f;
      UpdateData(false);
   }
}

void CMaterialComponents::OnEmissionLight() 
{
   COLORREF col = RGB(m_pfEmission[0]*255,	m_pfEmission[1]*255,m_pfEmission[2]*255);
	CColorDialog cd(col, CC_RGBINIT, this);
   if (cd.DoModal()==IDOK)
   {
      col = cd.m_cc.rgbResult;
      m_pfEmission[0] = (float)GetRValue(col)/255.0f;
      m_pfEmission[1] = (float)GetGValue(col)/255.0f;
      m_pfEmission[2] = (float)GetBValue(col)/255.0f;
      UpdateData(false);
   }
}

void CMaterialComponents::OnBitmapFile() 
{
   if (m_nMatNo != 0)
   {
      char szCurrenDir[_MAX_PATH];
      GetCurrentDirectory(_MAX_PATH, szCurrenDir);
      CFileDialog fd(true, "BMP", m_strBmpPath, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, "(Bitmap, *.bmp)|*.bmp|", this);
      if (fd.DoModal() == IDOK)
      {
         CString str;
         m_strBmpPath = _T(fd.m_ofn.lpstrFile);
/*
         str.Format("%s\\Bmp\\Bitmap%d.BMP", szCurrenDir, m_nMatNo);
         ::CopyFile(m_strBmpPath, str, false);
*/
      }
      SetCurrentDirectory(szCurrenDir);
   }
}

void CMaterialComponents::OnSelectMaterial() 
{
   if (m_nMatNo != 0)
   {
      if (AfxMessageBox(IDS_SAVE_MATERIAL_NOW, MB_YESNO|MB_ICONQUESTION) == IDYES)
      {
         SaveMaterial();
      }
   }
	CCaraMat cm;
   CString str;
   str.LoadString(IDS_SELECT_MATERIAL_HEADING);
   if (cm.SelectMaterial(m_hWnd, &m_nMatNo, (char*)LPCTSTR(str)))
   {
      char szCurrenDir[_MAX_PATH];
      CString str;
      CARAMAT_MATDSCRS cmdescr;
      cm.GetMatDscrDaten(m_nMatNo, &cmdescr);
      str.Format("%s : %s",cmdescr.pszGroupName, cmdescr.pszMatName);
      GetDlgItem(IDC_MATERIA_DESCR)->SetWindowText(str);
      GetCurrentDirectory(_MAX_PATH, szCurrenDir);
      str.Format("%s\\Bmp\\Material%d.CMP", szCurrenDir, m_nMatNo);
      CFile file;
      if (file.Open(str, CFile::modeRead))
      {
         int nLen;
         CArchive ar(&file, CArchive::load);

         ar.Read(m_pfAmbient            , sizeof(float)*4);
         ar.Read(m_pfDiffuse            , sizeof(float)*4);
         ar.Read(m_pfSpecular           , sizeof(float)*4);
         ar.Read(m_pfEmission           , sizeof(float)*4);
         ar.Read(&m_fShinines           , sizeof(float)  );
         ar.Read(&m_bBorder             , sizeof(BOOL)   );
         ar.Read(&m_fsScaleFactor       , sizeof(float)  );
         ar.Read(&m_ftScaleFactor       , sizeof(float)  );
         ar.Read(&m_bGenerateCoordinates, sizeof(BOOL)   );
         ar.Read(&m_cBorder             , sizeof(COLORREF));
         ar.Read(&nLen                  , sizeof(int)    );
         if ((nLen>1) && (nLen <= _MAX_PATH))
         {
            char *szBuffer = (char*)m_strBmpPath.GetBufferSetLength(nLen+1);
            ar.Read(szBuffer, nLen);
            szBuffer[nLen] = 0;
            m_strBmpPath.ReleaseBuffer();
         }
         UpdateData(false);
         ar.Close();
         file.Close();
      }	   
   }
   cm.Destructor();
}

bool CMaterialComponents::SaveMaterial()
{
   if (UpdateData())
   {
      char szCurrenDir[_MAX_PATH];
      CString str;
      GetCurrentDirectory(_MAX_PATH, szCurrenDir);
      str.Format("%s\\Bmp\\Material%d.CMP", szCurrenDir, m_nMatNo);
      CFile file;
      if (file.Open(str, CFile::modeCreate|CFile::modeWrite))
      {
         int nLen = m_strBmpPath.GetLength();
         CArchive ar(&file, CArchive::store);

         ar.Write(m_pfAmbient            , sizeof(float)*4);
         ar.Write(m_pfDiffuse            , sizeof(float)*4);
         ar.Write(m_pfSpecular           , sizeof(float)*4);
         ar.Write(m_pfEmission           , sizeof(float)*4);
         ar.Write(&m_fShinines           , sizeof(float)  );
         ar.Write(&m_bBorder             , sizeof(BOOL)   );
         ar.Write(&m_fsScaleFactor       , sizeof(float)  );
         ar.Write(&m_ftScaleFactor       , sizeof(float)  );
         ar.Write(&m_bGenerateCoordinates, sizeof(BOOL)   );
         ar.Write(&m_cBorder             , sizeof(COLORREF));
         ar.Write(&nLen                  , sizeof(int)    );
         if (nLen) ar.Write(LPCTSTR(m_strBmpPath), nLen);
         ar.Close();
         file.Close();
         return true;
      }	   
   }
   return false;
}
