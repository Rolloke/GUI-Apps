
   switch (nMsg)
   {
      case WM_COMMAND:
      {
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam);
         switch (wmId)
         {
            case ID_EDIT_COPY_VIEW:
            {
               RECT rc;
               GetClientRect(hWnd, &rc);
               CTestCara3D *pPrintGl = new CTestCara3D(pTC3D);
               SIZE szPicture = {rc.right, rc.bottom};
               pPrintGl->SetSize(szPicture);
               pPrintGl->SetModes(ETS3D_CLIPBOARD_INDEPENDENT, ETS3D_DC_CLEAR);
               pPrintGl->Create("Copy", 0, &rc, hWnd);
            } break;
            case ID_EDIT_SAVE_VIEW:
            {
               RECT rc;
               GetClientRect(hWnd, &rc);
               CTestCara3D *pPrintGl = new CTestCara3D(pTC3D);
               SIZE szPicture = {rc.right, rc.bottom};
               pPrintGl->SetSize(szPicture);
               pPrintGl->SetModes(ETS3D_BMP_FILE_INDEPENDENT, ETS3D_DC_CLEAR);
               pPrintGl->Create("Test.bmp", 0, &rc, hWnd);
            } break;
            case ID_FILE_PRINT:
            {
               HDC hDC = ::CreateDC("WINSPOOL", "HP DeskJet 660 (Color)", NULL, NULL);
               DOCINFO di;
               ZeroMemory(&di, sizeof(DOCINFO));
               di.cbSize      = sizeof(DOCINFO);
               di.lpszDocName = "Document";
               StartDoc(hDC, &di);
               StartPage(hDC);
               TextOut(hDC, 100,  100, "Obere Ecke", 10);
               TextOut(hDC,2100, 2100, "Untere Ecke", 11);
               EndPage(hDC);
               EndDoc(hDC);
               if (hDC) DeleteDC(hDC);
               if (pPrintGl) delete pPrintGl;
            } break;
            default: pTC3D->OnCommand(wmId, wmEvent, hWnd);
         }
      }
   }
/*
               RECT rc;
               GetClientRect(hWnd, &rc);
               CTestCara3D *pPrintGl = new CTestCara3D(pTC3D);
               SIZE szPicture = {rc.right, rc.bottom};
               pPrintGl->SetSize(szPicture);
               pPrintGl->SetModes(ETS3D_DC_COPY, ETS3D_DC_CLEAR);
               pPrintGl->Create("Copy", 0, &rc, hWnd);
               HGLOBAL hGL = (HGLOBAL) pPrintGl->SendMessage(WM_GET_3DIMAGE_DATA, 0, 0);
               if (hGL) GlobalFree(hGL);
               delete pPrintGl;
*/
