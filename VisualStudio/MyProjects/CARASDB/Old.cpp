void CFindCustomer::FindItem()
{
   LV_FINDINFO FI = { LVFI_PARTIAL, m_strSearchFor, NULL };
   LV_ITEM lvItem = {LVIF_STATE, 0, 0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED, NULL, 0, 0, NULL};
   lvItem.iItem   = m_FindList.FindItem(&FI);
   if (lvItem.iItem != -1)
   {
      m_FindList.SetItemState(lvItem.iItem, &lvItem);
      m_FindList.EnsureVisible(lvItem.iItem, false);
   }
}

void CFindCustomer::OnChangeFindEdit()
{
   if (UpdateData(true) && !m_strSearchFor.IsEmpty())
   {
      if (m_nColumn == 0);// FindItem();
      else
      {
         LV_ITEM lvItem = { LVIF_PARAM|LVIF_IMAGE, 0, 0, 0, 0, NULL, 0, 0, NULL};
         int nCount     = m_FindList.GetItemCount();
         int nLength    = m_strSearchFor.GetLength();
         const char *ptrI, *ptrS = LPCTSTR(m_strSearchFor);
         if ((m_nItem==-1)||(nLength<m_nSearchStrLen)||(nLength==1)) m_nItem = 0;
         m_nSearchStrLen = nLength;
         lvItem.iItem    = m_nItem;
         for (;lvItem.iItem<nCount; lvItem.iItem++)
         {
            m_FindList.GetItem(&lvItem);
            if (lvItem.lParam)
            {
               Customer *pC = (Customer*) lvItem.lParam;
               switch (m_nColumn)
               {
                  case 1 : ptrI = pC->szName;       break;
                  case 2 : ptrI = pC->szSirName;    break;
                  case 3 : ptrI = pC->szPostalCode; break;
                  case 4 : ptrI = pC->szCity;       break;
                  case 5 : ptrI = pC->szStreet;     break;
                  case 6 : ptrI = pC->szCompany;    break;
                  default: ptrI = NULL;             break;
               }

               if (ptrI && (strncmp(ptrI, ptrS, nLength)==0))
               {
                  m_nItem   = lvItem.iItem;
                  LV_ITEM lvI = {LVIF_STATE, 0, 0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED, NULL, 0, 0, NULL};
                  lvI.iItem = lvItem.iItem;
                  m_FindList.SetItemState(lvI.iItem, &lvI);
                  m_FindList.EnsureVisible(lvI.iItem, false);
                  break;
               }
            }
         }
      }
   }
   else m_nItem = -1;
}

/*
   m_FindList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
                          LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_TRACKSELECT|LVS_EX_CHECKBOXES,
                          LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_TRACKSELECT|LVS_EX_CHECKBOXES);


   int nItem = 0;
   Customer *pCustomer;
   pos = plist->GetHeadPosition();
   while (pos)
   {
      pCustomer = (Customer*) plist->GetNext(pos);
      if (pCustomer)
      {
         char text[32];
         wsprintf(text, "%d", pCustomer->nNo);
         if (m_FindList.InsertItem(LVIF_PARAM|LVIF_IMAGE, nItem, text, 0, 0, nItem, (long) pCustomer) != -1)
         {
            m_FindList.SetItem(nItem, 1, LVIF_TEXT, pCustomer->szName      , 0, 0, 0, NULL);
            m_FindList.SetItem(nItem, 2, LVIF_TEXT, pCustomer->szSirName   , 0, 0, 0, NULL);
            m_FindList.SetItem(nItem, 3, LVIF_TEXT, pCustomer->szPostalCode, 0, 0, 0, NULL);
            m_FindList.SetItem(nItem, 4, LVIF_TEXT, pCustomer->szCity      , 0, 0, 0, NULL);
            m_FindList.SetItem(nItem, 5, LVIF_TEXT, pCustomer->szStreet    , 0, 0, 0, NULL);
            m_FindList.SetItem(nItem, 6, LVIF_TEXT, pCustomer->szCompany   , 0, 0, 0, NULL);
            nItem++;
         }
      }
   }
*/
