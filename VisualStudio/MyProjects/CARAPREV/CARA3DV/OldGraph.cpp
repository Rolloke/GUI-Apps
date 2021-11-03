/*
bool CCARA3DVDoc::TestRead(CArchive &ar, CFileHeader* pfh)
{
//   EnterCriticalSection(&m_csDocData);                     // Dokumentdaten vor Zugriff eines anderen Threads schützen
   {
      if (ReadNetData(ar, pfh))
      {
         m_nFilmPictures = 200;
         m_hFileMapping = ::CreateFileMapping((HANDLE) 0xFFFFFFFF,
                          NULL, PAGE_READWRITE, 0,
                          sizeof(double)*m_nFilmPictures*m_nPoints, NULL);

         if(m_hFileMapping != NULL)
         {
            // Erzeugen der Z-Werte als Funktion f(x,y)
            if (m_pptXY_Koordinates)
            {  // File-Mapping mit Schreiberlaubnis
               float  * pfZ_Values = (float*) ::MapViewOfFile(m_hFileMapping, FILE_MAP_WRITE, 0, 0, 0);
               if (pfZ_Values)
               {
                  float  * pfZ_Value;
                  int k = 120;
                  for (int i=0; i<m_nFilmPictures; i++)
                  {
                     pfZ_Value = &pfZ_Values[i*m_nPoints];
                     for (int j=0; j<m_nPoints; j++)
                     {
                        if (m_pnZeroPoints[j] == 0) 
                           pfZ_Value[j] = -2.0;
                        else 
                           pfZ_Value[j] = (float) (sin(0.005*k*m_pptXY_Koordinates[j].x) + sin(0.0032*k*m_pptXY_Koordinates[j].x)*cos(0.006*k*m_pptXY_Koordinates[j].y));
                     }
                     k++;
                  }
               }
               ::UnmapViewOfFile(pfZ_Values);
            }

            m_pFileMappedPointer = ::MapViewOfFile(m_hFileMapping, FILE_MAP_READ, 0, 0, 0);
            m_pfZ_Value = (float*) m_pFileMappedPointer;
         }
         m_pDibLabel = new CPictureLabel;         // Druckrechteck
         m_pDibLabel->SetDrawPickPoints(true);
         ((CLabel*)m_pDibLabel)->SetPoint(CPoint(0,0),0);    // wird noch initialisiert
         ((CLabel*)m_pDibLabel)->SetPoint(CPoint(0,0),1);

         m_pHeadLine  = new CTextLabel;           // Überschrift
         if (m_pHeadLine)
         {
            m_pHeadLine->SetDrawPickPoints(false);
            m_pHeadLine->SetText("Dödel");
            CCARA3DVApp *pApp = (CCARA3DVApp*) AfxGetApp();
         }

         m_pLabel = new CLabelContainer;           // In den CLabelContainer einfügen für die Bearbeitung
         if (m_pLabel)
         {
            m_pLabel->SetDeleteContent(true);
            m_pLabel->InsertLabel(m_pDibLabel);
            m_pLabel->InsertLabel(m_pHeadLine);
         }
      }
      else return false;
   }
//   LeaveCriticalSection(&m_csDocData);                      // Dokumentdaten freigeben
   return true;
}

  
bool CCARA3DVDoc::ReadText(CArchive &ar)
{
#define STRING_LENGTH 256
   char line[STRING_LENGTH];
   int  index, i, ndummy;
//   int  xmin,xmax, ymin,ymax;

   ar.ReadString(line, STRING_LENGTH);
   sscanf(line, "%d %d %d", &m_nPoints, &m_nWebTriangles, &m_nFilmPictures);

   m_nFilmPictures   = 1;
   m_nContourPoints  = 0;

   m_pptXY_Koordinates = new Vector2Df[m_nPoints];
   if (!m_pptXY_Koordinates)  return false;

   for (i=0; i<m_nPoints; i++)
   {
      ar.ReadString(line, STRING_LENGTH);
      sscanf(line, "%d %f %f %d", &index, &m_pptXY_Koordinates[i].x, &m_pptXY_Koordinates[i].y, &ndummy);
      if (i==0)
      {
         xmin = xmax = m_pptXY_Koordinates[i].x;
         ymin = ymax = m_pptXY_Koordinates[i].y;
      }
      else
      {
         if (xmin > m_pptXY_Koordinates[i].x) xmin = m_pptXY_Koordinates[i].x;
         if (xmax < m_pptXY_Koordinates[i].x) xmax = m_pptXY_Koordinates[i].x;
         if (ymin > m_pptXY_Koordinates[i].y) ymin = m_pptXY_Koordinates[i].y;
         if (ymax > m_pptXY_Koordinates[i].y) ymax = m_pptXY_Koordinates[i].y;
      }
   }
   for (i=0; i<m_nPoints; i++)
   {
      
   }
   m_pnvWebTriangles   = new Vectorl3[m_nWebTriangles];
   if (!m_pnvWebTriangles) return false;

   for (i=0; i<m_nWebTriangles; i++)
   {
      ar.ReadString(line, STRING_LENGTH);
      sscanf(line, "%d %d %d %d", &index, &m_pnvWebTriangles[i].p[2],
                                          &m_pnvWebTriangles[i].p[1],
                                          &m_pnvWebTriangles[i].p[0]);
      m_pnvWebTriangles[i].p[0]--;
      m_pnvWebTriangles[i].p[1]--;
      m_pnvWebTriangles[i].p[2]--;
   }
   m_nFloorTriangles   = m_nWebTriangles;
   m_pnvFloorTriangles = m_pnvWebTriangles;


   int n, nr, imax;
   imax = m_nFilmPictures*m_nPoints;
   m_pfZ_Value = new float[imax];
   if (!m_pfZ_Value) return false;

   for (i=0; (i<imax) && (ar.ReadString(line, STRING_LENGTH)!=NULL);)
   {
      for (n=0; line[n] != 0; n++)
         if (line[n] == '.')
            for (nr = n-1; ; nr--)
               if (line[nr] == ' ')
               {
                  if (i<imax) 
                     m_pfZ_Value[i++] = (float) atof(&line[nr]);
                  break;
               }
   }
   UpdateAllViews(NULL, UPDATE_INSERT_NEWDOC);
   return true;
#undef STRING_LENGTH
}
#endif //_DEBUG
*/
