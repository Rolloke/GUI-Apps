/*******************************************************************************
                                 CEtsList
  Zweck:
   Doppeltverkettete-Liste von Zeigerobjekte zur Verkettung wird das 
   Knotenobjekt CNode verwendet
  Copyright:
   (C) 1999 ELAC Technische Software GmbH
  VERSION 
   1.03 Stand 10.09.1999
                                                   programmed by Oliver Wesnigk
                                                   edited by Rolf Ehlers
*******************************************************************************/

#include "StdAfx.h"
#include "CEtsList.h"                            // Headerdatei für CEtsList einbinden

/*******************************************************************************
                               Listenknotenklasse CNode
*******************************************************************************/

class CNode
{

friend CEtsList;                                 // friend zu CEtsList, damit dieser direkt auf
                                                 // die Members von CNode zugreifen kann
public:
   CNode() {ZeroMemory(this, sizeof(CNode));};
   ~CNode(){};

private:
   void* __cdecl operator new(size_t nSize)
   {
      return malloc(nSize);
   }
   void  __cdecl operator delete(void* p)
   {
      free(p);
   }
   void   * pData;
   CNode  * pPrevious;
   CNode  * pNext; 
};
                                                 // Implementationsmakros nötig für alle von 
                                                 // CRtti abgeleiteten Klassen (erzeugt Typeinformationsdaten
                                                 // und CreateInstance Funktionen

/*******************************************************************************
                        Methoden der CEtsList Klasse
                                  Constructor
*******************************************************************************/
                                                 // Der CEtsList enthält keine eigenen
CEtsList::CEtsList(bool mode)
{
   m_pFirstNode   = NULL;                        // Liste enthält noch keine Objekte
   m_pActualNode  = NULL;
   m_pLastNode    = NULL;
   m_nCounter     = 0;                           // Objektzähler auf 0
   m_bMode        = mode;                        // Flag, ob der Destructor die enthaltenen
   m_nActualIndex = -1;                          // Objekte löscht (default = true)
   m_pfnDelete    = DeleteObject;
};

/*******************************************************************************
                                 Destruktor
public Function: zerstören einer Instance
*******************************************************************************/

CEtsList::~CEtsList()
{
   Destroy();                                    // rufe einfach die Funktion Destroy auf
}

void CEtsList::Destroy()
{
   CNode *pNext;                                // Zeiger auf nächsten Listenknoten
   CNode *pAct          = m_pFirstNode;         // Zeiger auf aktuellen Listenknoten
   int    nEqualCounter = 0;                    // Zähler für gelöschte Knoten

   while(pAct)                                  // solange aktueller Listenknoten vorhanden
   {
      pNext = pAct->pNext;                      // Zeiger auf nächsten Knoten speichern
      if(m_bMode && (pAct->pData != NULL))
      {
         m_pfnDelete(pAct->pData);              // soll die enthaltenen Objekte gelöscht werden,dann löschen
      }
      delete pAct;                              // aktuellen Listenknoten löschen
      pAct = pNext;                             // neuen aktuellen Listenknoten setzen
      IsValid(++nEqualCounter);                 // Zähler für gelöschte Listenknoten erhöhen
   }

   m_pFirstNode   = NULL;                       // alle Listenknotenzeiger zurücksetzen
   m_pLastNode    = NULL;
   m_pActualNode  = NULL;
   m_nActualIndex = -1;
   if(nEqualCounter!=m_nCounter)                // Anzahl gelöschter Listenknoten ungleich Sollzähler, dann Fehler
      ErrorMessage("CEtsList: nodecounters not equal !");

   m_nCounter      = 0;                         // die Liste ist leer also Sollzähler zurücksetzen
}
/*******************************************************************************
Name       : IsValid
Definition : void IsValid(int);
Zweck      : überprüft ob übergebene Zahl (Counter) negativ wird und
             erzeugt dann einen Laufzeitfehler
Parameter  : 
 counter (E): Anzahl der Elemente
Rückgabewert : keiner
*******************************************************************************/
void CEtsList::IsValid(int counter)
{
   if(counter < 0) 
      ErrorMessage("CEtsList : nodecounter overflow !");
}

CEtsList::CEtsList(const CEtsList* pQuelle)
{
   m_pFirstNode   = NULL;                        // Liste enthält noch keine Objekte
   m_pActualNode  = NULL;
   m_pLastNode    = NULL;
   m_nCounter     = 0;                           // Objektzähler auf 0
   m_bMode        = false;                       // Flag, ob der Destructor die enthaltenen Objekte löscht hier false !
   m_nActualIndex = -1;

   CNode * pNext = pQuelle->m_pFirstNode;

   while(pNext)
   {
      ADDHead(pNext->pData);
      pNext = pNext->pNext;
   }
}


/*******************************************************************************
Name       : ADDFirst
Definition : void ADDFirst(void *);
Zweck      : erstes Objekt in die Liste einfügen
Aufruf     : ADDFirst(pObject); (private)
Parameter  : 
 pObject (E): Zeiger auf eingefügtes Objekt                             (void*)
*******************************************************************************/
void CEtsList::ADDFirst(void * pObject)
{
   if(pObject)                                   // Objekt vorhanden ?
   {
      m_pFirstNode = new CNode;                  // Listenknoten Objekt erzeugen

      m_pFirstNode->pData     = pObject;

      m_pLastNode  = m_pFirstNode;               // letztes Objekt in der Liste = erstes
      if (m_nActualIndex != -1) m_nActualIndex++;
      IsValid(++m_nCounter);                     // Sollzähler erhöhen
   }
}

void CEtsList::ADDHead(void * pObject)
{
   if(m_pLastNode==NULL)                         // noch kein Objekt in der Liste ?
      ADDFirst(pObject);                         // dann erstes Objekt in die Liste einfügen
   else                                          // es gibt schon Objekte in der Liste
   {
      if(pObject)                                // Objekt vorhanden
      {         
         CNode * pNewObject = new CNode;         // Listenknoten Objekt erzeugen

         m_pLastNode->pNext = pNewObject;        // Listenknoten verketten
      
         pNewObject->pPrevious = m_pLastNode;    // Daten füllen
         pNewObject->pData     = pObject;
      
         m_pLastNode = pNewObject;               // neuer letzter Listenknoten
         m_nActualIndex = -1;
         IsValid(++m_nCounter);                  // Sollzähler erhöhen
      }
   }   
}

void CEtsList::ADDHead_Splitted(CEtsList * pContainer)
{
   void * pObject = pContainer->GetFirst();      // Zeiger auf das erste Objekt der übergebenen Liste

   while(pObject)                                // solange ein gültiges Objekt
   {
      ADDHead(pObject);                          // dieses an das Listenende einfügen
      pObject = pContainer->GetNext();           // nachfolgendes Objekt holen
   }
}

void CEtsList::ADDTail(void * pObject)
{
   if(m_pFirstNode==NULL)                        // noch kein Objekt in der Liste ?
      ADDFirst(pObject);                         // dann erstes Objekt in die Liste einfügen
   else                                          // es gibt schon Objekte in der Liste
   {
      if(pObject)                                // Objekt vorhanden
      {                                          // Listenknoten Objekt erzeugen
         CNode * pNewObject = new CNode;

         m_pFirstNode->pPrevious = pNewObject;   // Listenknoten verketten
      
         pNewObject->pNext     = m_pFirstNode;
         pNewObject->pData     = pObject;
         m_nActualIndex = -1;
         m_pFirstNode = pNewObject;              // neuen ersten Listenknoten
         IsValid(++m_nCounter);                  // Sollzähler erhöhen
      }
   }
} 

/*******************************************************************************
Name       : GetObject
Definition : inline void * GetObject(CNode *, int);
Zweck      : Objekt aus Listenknoten holen mit Typeprüfung und Listenknoten
             als den aktuellen Listenknoten merken
Aufruf     : GetObject(pFrom, nActual); (private)
Parameter  : 
 pFrom   (E) : Zeiger auf den Listenknoten                           (CNode*)
 nActual (E) : Aktueller Index in der Liste                          (int)
Rückgabewert : Zeiger auf das Objekt                                 (void*)
*******************************************************************************/
inline void * CEtsList::GetObject(CNode * pFrom, int nActual)
{
   if(pFrom)                                     // Listenknoten vorhanden
   {
      m_pActualNode  = pFrom;                    // Listenknoten als aktuell merken
      m_nActualIndex = nActual;
      return pFrom->pData;                       // enthaltenes Objekt zurückgeben
   }
   else return NULL;                             // kein Objekt zurückgeben
}

inline void * CEtsList::GetObject(CNode * pFrom) const
{
   if(pFrom)                                     // Listenknoten vorhanden
   {
      return pFrom->pData;                       // enthaltenes Objekt zurückgeben
   }
   else return NULL;                             // kein Objekt zurückgeben
}

void * CEtsList::GetFirst()
{
   return GetObject(m_pFirstNode, 0);
}

void * CEtsList::GetLast()
{
   return GetObject(m_pLastNode, m_nCounter-1);
}

void * CEtsList::GetNext()
{
   if(m_pActualNode)                             // Listenknoten vorhanden
   {
      return GetObject(m_pActualNode->pNext, m_nActualIndex+1);// nachfolgendes Objekt holen und Listenknoten merken
   }
   return NULL;                                  // kein Objekt !
}

void * CEtsList::GetPrevious()
{
   if(m_pActualNode)                             // Listenknoten vorhanden
   {
      return GetObject(m_pActualNode->pPrevious, m_nActualIndex-1);
   }
   return NULL;                                  // kein Objekt !
}

void * CEtsList::GetActual() const
{
   return GetObject(m_pActualNode);
}

void * CEtsList::GetAt(int nPos)                // Objekt mit der Position nPos holen
{
   int i;
   if ((nPos < 0) || (nPos >= m_nCounter)) return NULL;
   if ((m_pActualNode != NULL) && (m_nActualIndex != -1))
   {
      if (nPos > m_nActualIndex)
      {
         i = m_nActualIndex+1;
         CNode * pNext = m_pActualNode->pNext;        // Zeiger mit nächsten Listenknoten initialisieren
         while(pNext)                                 // solange Listenknoten vorhanden
         {
            if (i == nPos) return GetObject(pNext, i);
            pNext = pNext->pNext, i++;                // weitersuchen aufwärts
         }
      }
      else if (nPos < m_nActualIndex)
      {
         i = m_nActualIndex-1;
         CNode * pPrevious = m_pActualNode->pPrevious; // Zeiger mit vorhergehenden Listenknoten initialisieren
         while(pPrevious)                              // solange Listenknoten vorhanden
         {
            if (i == nPos) return GetObject(pPrevious, i);
            pPrevious = pPrevious->pPrevious, i--;     // weitersuchen abwärts
         }
      }
      else return GetObject(m_pActualNode);
   }
   else
   {
      return GetObject(FindIndex(nPos), nPos);
   }
   m_nActualIndex = -1;
   return NULL;
}

void * CEtsList::GetActualPrevious() const
{
   if(m_pActualNode)                             // Listenknoten vorhanden 
   {
      return GetObject(m_pActualNode->pPrevious);
   }
   return NULL;                                  // kein Objekt
}

void * CEtsList::GetActualNext() const
{
   if(m_pActualNode)                             // Listenknoten vorhanden
   {
      return GetObject(m_pActualNode->pNext);
   }
   return NULL;                                  // kein Objekt
}


bool CEtsList::InList(void * pObject) const
{
   if(FindObjectList(pObject)) return true;
   else                        return false;
}

/*******************************************************************************
Name       : FindObjectList
Definition : CNode * FindObjectList(void *) const;
Zweck      : suche den Listenknotenzeiger welchen den übergebenen Objektzeiger
             enthält
Aufruf     : FindObjectList(pObject) (private)
Parameter  : 
 pObject   : Zeiger auf das Objekt                                   (void*)
Rückgabewert : Zeiger auf den Listenknoten                           (CNode*)
*******************************************************************************/
CNode * CEtsList::FindObjectList(void * pObject) const
{
   if(m_pActualNode)                             // gibt es ein aktuellen Listenknoten ?
   {                                             // Ja, dann wird von diesem aufwärts gesucht
      CNode * pNext = m_pActualNode;             // Zeiger mit aktuellen Listenknoten initialisieren
      while(pNext)                               // solange Listenknoten vorhanden
      {
         if(pNext->pData==pObject)               // vergleiche Inhalt mit Objektzeiger
         {
            return pNext;                        // gefunden !
         }
         pNext = pNext->pNext;                   // weitersuchen aufwärts
      }
                                                 // und nun vom aktuellen Listenknoten abwärts suchen
      CNode * pPrevious = m_pActualNode->pPrevious;
      while(pPrevious)                           // solange Listenknoten vorhanden
      {
         if(pPrevious->pData==pObject)           // vergleiche Inhalt mit Objektzeiger
         {
            return pPrevious;                    // gefunden !
         }
         pPrevious = pPrevious->pPrevious;       // weitersuchen abwärts
      }
   }
   else                                          // es gab kein aktuellen Listenknoten, dann
   {                                             // die Liste von Anfang an durchsuchen
      CNode * pNext = m_pFirstNode;              // Zeiger mit ersten Listenknoten initialisieren
      while(pNext)                               // solange Listenknoten vorhanden
      {
         if(pNext->pData==pObject)               // vergleiche Inhalt mit Objektzeiger
         {
            return pNext;                        // gefunden !
         }
         pNext = pNext->pNext;                   // weitersuchen aufwärts
      }
   }

   return NULL;                                  // Listenknotenzeiger nicht gefunden !
}

/*******************************************************************************
Name       : DeleteObjectList
Definition : void DeleteObjectList(CNode *);
Zweck      : entkettet den übergebenen Listenknoten (als Zeiger) aus der Liste 
             und löscht anschließen das Listenknotenobjekt
Aufruf     : DeleteObjectList(pFind) (private)
Parameter  : 
 pFind (E) : Zeiger auf den Listenknoten                           (CNode*)
*******************************************************************************/
void CEtsList::DeleteObjectList(CNode * pFind)
{
   if(pFind)                                     // Listenknoten vorhanden
   {
      CNode * pNext     = pFind->pNext;          // Zeiger auf vorhergehenden und
      CNode * pPrevious = pFind->pPrevious;      // nachfolgenden Listenknoten initialisieren

      if(pPrevious)                              // vorhergehender Listenknoten vorhanden ?
      {
         if (pNext)                              // nachfolgender Listenknoten vorhanden ?
         {                                       // dann Listenknoten zwischen 2 Listenknoten entfernen
            pPrevious->pNext = pNext;            // Verkettung auflösen
            pNext->pPrevious = pPrevious;
         }
         else                                    // kein nachfolgenen Listenknoten !
         {                                       // dann Listenknoten als letzten Listenknoten entfernen
            m_pLastNode      = pPrevious;        // neuen letzten Listenknoten merken
            pPrevious->pNext = NULL;             // Verkettung auflösen
         }
      }
      else                                       // kein vorhergehenden Listenknoten !
      {
         if(pNext)                               // nachfolgender Listenknoten vorhanden ?
         {                                       // dann Listenknoten als ersten Listenknoten entfernen
            m_pFirstNode   = pNext;              // neuen erste Listenknoten merken
            pNext->pPrevious = NULL;             // Verkettung auflösen
         }
         else                                    // kein nachfolgender Listenknoten vorhanden !
         {                                       // dann enthält die Liste keine Listenknoten mehr
            m_pLastNode  = NULL;
            m_pFirstNode = NULL;
         }
      }
      delete pFind;                              // Listenknotenobjekt löschen
      m_pActualNode  = NULL;
      m_nActualIndex = -1;
   }
}

/*************************************************************************************
* Definition : void Delete(void *);
* Zweck      : Löscht ein Element aus der Liste.
* Parameter (E): pObject: Objektzeiger                                        (void*)
**************************************************************************************/
void CEtsList::Delete(void * pObject)
{
   CNode * pFind = FindObjectList(pObject);     // Listenknoten suchen, welcher dieses Objekt enthält

   if(pFind)                                    // Listenknoten gefunden ?
   {
      if(pFind->pData != NULL)
      {
         m_pfnDelete(pFind->pData);             // Ja, enthaltenes Objekt löschen
      }
      DeleteObjectList(pFind);                  // Listenknoten Objekt entketten und löschen
      IsValid(--m_nCounter);                    // Sollzähler verringern
   }
}

/*************************************************************************************
* Definition : void Remove(void *)
* Zweck      : Entfern ein Element aus der Liste
* Parameter (E): pObject: Objektzeiger                                        (void*)
**************************************************************************************/
void CEtsList::Remove(void * pObject)
{
   CNode * pFind = FindObjectList(pObject);      // Listenknoten suchen, welcher dieses Objekt enthält

   if(pFind)                                     // Listenknoten gefunden ?
   {
      DeleteObjectList(pFind);                   // Listenknoten Objekt entketten und löschen
      IsValid(--m_nCounter);                     // Sollzähler verringer..n
   }
}

/*************************************************************************************
* Definition : void CEtsList::Remove_Container(CEtsList * pContainer)
* Zweck      : Entfernt die Objekte der übergeben Liste von dieser Liste, wenn sie
*              in dieser Liste enthalten sind.
* Parameter (E): pContainer : Liste mit zu entfernenden Objekten
**************************************************************************************/
void CEtsList::Remove_Container(CEtsList * pContainer)
{
   void * pObject = pContainer->GetFirst();    // erste Objekt aus übergebener Liste holen
   
   while(pObject)                                // solange Objekt vorhanden
   {
      Remove(pObject);                           // diese Objekt aus der Liste entfernen (wenn vorhanden)
      pObject = pContainer->GetNext();           // nachfolgendes Objekt aus übergebener Liste holen
   }   
}

/*************************************************************************************
* Definition : bool CEtsList::IsFirst(void * pIs) const
* Zweck      : prüft ob das übergebene Objekt das erste in der Liste ist
* Parameter (E): pObject: Objektzeiger                                        (void*)
* Funktionswert: (true, false)                                                (bool)
**************************************************************************************/
bool CEtsList::IsFirst(void * pIs) const
{
   if(m_pFirstNode)                              // Listenknoten vorhanden ?
   {
      return (m_pFirstNode->pData==pIs);
   }
   else
      return false;                              // nicht erstes Element
}

/*************************************************************************************
* Definition : bool CEtsList::IsLast(void * pIs) const
* Zweck      : prüft ob das übergebene Objekt das letzte in der Liste ist
* Parameter (E): pObject: Objektzeiger                                        (void*)
* Funktionswert: (true, false)                                                (bool)
**************************************************************************************/
bool CEtsList::IsLast(void * pIs) const
{
   if(m_pLastNode)                               // Listenknoten vorhanden ?
   {
      return (m_pLastNode->pData==pIs);
   }
   else
      return false;                              // nicht letztes Element
}

void CEtsList::ErrorMessage(char *pszError)
{
   MessageBox(NULL, pszError, "CEtsListError", MB_OK|MB_TASKMODAL|MB_ICONEXCLAMATION);
}

/*************************************************************************************
* Definition : int FindElement(SORTFUNCTION, void *) const;
* Zweck      : Sucht das Element in der Liste nach Parametern, die durch die
*              Suchfunktion vorgegeben werden.
* Parameter (E): [pfnSort]: Suchfunktion zum Vergleichen der Objekte    (SORTFUNCTION)
*                (pfnSort==NULL): suche nach dem Objektzeiger
*           (E): pObject: Zeiger auf ein beliebiges Suchobjekt          (void *)
* Funktionswert: 
**************************************************************************************/
int CEtsList::FindElement(SORTFUNCTION pfnSort, void *pObject) const
{
   CNode * pNext = m_pFirstNode;              // Zeiger mit ersten Listenknoten initialisieren
   int  nCount = 0;
   while(pNext)                               // solange Listenknoten vorhanden
   {
      if (pfnSort)                            // vergleiche Inhalt mit Objektzeiger
      {
         if (pfnSort(pNext->pData, pObject) == 0)
            return nCount;
      }
      else if (pNext->pData == pObject)       // vergleiche Objektzeiger
         return nCount;
      pNext = pNext->pNext;                   // weitersuchen aufwärts
      nCount++;
   }
   return -1;
}

/*************************************************************************************
* Definition : void CEtsList::InsertAfter(SORTFUNCTION, void *)
* Zweck      : Fügt ein Element in die Liste ein. Die Position wird durch die 
*              Sortierfunktion bestimmt. Die Liste sollte vorher mit derselben
*              Sortierfunktion sortiert worden sein.
* Parameter (E): pfnSort: Sortierfunktion zum Vergleichen der Objekte    (SORTFUNCTION)
*           (E): pObject: Zeiger auf ein Objekt                          (void *)
**************************************************************************************/
void CEtsList::InsertAfter(SORTFUNCTION pfnSort, void *pObject)
{
   CNode * pNext = m_pFirstNode;                            // Zeiger mit ersten Listenknoten initialisieren
   while(pNext)                                             // solange Listenknoten vorhanden
   {
      if(pfnSort(pNext->pData, pObject) > 0)                // vergleiche Inhalt mit Objektzeiger
      {
         if      (m_pLastNode==NULL)     ADDFirst(pObject);
         else if (IsLast(pNext->pData))  ADDHead(pObject);
         else if (IsFirst(pNext->pData)) ADDTail(pObject);
         else
         {
            CNode * pNewObject      = new CNode;
            pNewObject->pData       = pObject;
            pNewObject->pNext       = pNext;
            pNewObject->pPrevious   = pNext->pPrevious;
            pNext->pPrevious->pNext = pNewObject;
            pNext->pPrevious        = pNewObject;
            IsValid(++m_nCounter);                             // Sollzähler erhöhen
         }
         m_nActualIndex = -1;
         m_pActualNode  = NULL;
         break;
      }
      pNext = pNext->pNext;                                    // weitersuchen aufwärts
   }
}

/*************************************************************************************
* Definition : void* ReplaceAt(int, void *, bool)
* Zweck      : Ersetzt ein Objekt an einer bestimmten Position
* Parameter (E): nPos: Position an der das Objekt ersetzt werden soll        (int)
*           (E): pNew: Einzufügendes Objekt                                  (void*)
*           (E): bDelete: Löschen des alten Objektes                         (bool)
* Funktionswert: altes Objekt, wenn es nicht gelöscht wurde                  (void*)
**************************************************************************************/
void* CEtsList::ReplaceAt(int nPos, void *pNew, bool bDelete)
{
   CNode * pObject = FindIndex(nPos);
   if (pObject)
   {
      void *pOld     = pObject->pData;
      pObject->pData = pNew;
      if (bDelete)
      {
         m_pfnDelete(pOld);
         pOld = NULL;
      }
      return pOld;
   }
   return NULL;
}

/*************************************************************************************
* Definition : void CEtsList::InsertAfter(SORTFUNCTION, void *)
* Zweck      : Sortiert die Liste mit Hilfe der Sortierfunktion.
* Parameter (E): pfnSort: Sortierfunktion zum Vergleichen der Objekte    (SORTFUNCTION)
**************************************************************************************/
void CEtsList::Sort(SORTFUNCTION pfn)
{
   quicksort(pfn, 0, m_pFirstNode, m_nCounter-1, m_pLastNode);
   m_nActualIndex = -1;
   m_pActualNode  = NULL;
}

/*******************************************************************************
Name       : quicksort
Definition : void quicksort(SORTFUNCTION, int, CNode*, int, CNode*);
Zweck      : Rekursive Funktion zum Sortieren der Liste.
Aufruf     : quicksort(pfn, nLeft, pLeft, nRight, pRight); (private)
Parameter  : 
 pfn    (E) : Zeiger auf die Sortierfunktion                      (SORTFUNCTION)
 nLeft  (E) : Index des Elementes auf der linken Seite des Teilabschnitts  (int)
 pLeft  (E) : Knoten des Elementes auf der linken Seite des Teilabschnitts (CNode*)
 nRight (E) : Index des Elementes auf der rechten Seite des Teilabschnitts (int)
 pRight (E) : Knoten des Elementes auf der rechten Seite des Teilabschnitts (CNode*)
*******************************************************************************/
void CEtsList::quicksort(SORTFUNCTION pfn, int nLeft, CNode *pLeft, int nRight, CNode *pRight)
{
   int    nI, nJ;
   void  *pData, *pTemp;
   CNode *pI, *pJ;

   if (nRight > nLeft)
   {
      nI = nLeft-1;
      nJ = nRight;

      pI    = pLeft->pPrevious;
      pJ    = pRight;
      pData = pRight->pData;

      do
      {
         do                                                    // aufwärts suchen
         {
            nI++;
            if (nI >= m_nCounter) break;
            if (pI == NULL) pI = FindIndex(nI);
            else            pI = pI->pNext;
         }
	      while(pfn(pI->pData, pData) > 0);
         do                                                    // abwärts suchen
         {
            nJ--;
            if (nJ < 0) break;
            if (pJ == NULL) pJ = FindIndex(nJ);
            else            pJ = pJ->pPrevious;
         }
	      while(pfn(pJ->pData, pData) < 0);

         pTemp = pI->pData;                                    // tauschen
         pI->pData = pJ->pData;
         pJ->pData = pTemp;
      }
      while (nJ > nI);

      pJ->pData     = pI->pData;                               // zurücktauschen
      pI->pData     = pRight->pData;
      pRight->pData = pTemp;

      quicksort(pfn, nLeft, pLeft    , nI-1  , pI->pPrevious);
      quicksort(pfn, nI+1 , pI->pNext, nRight, pRight       );
   }
}

CNode *CEtsList::FindIndex(int n)
{
   int i = 0;
   CNode * pNext = m_pFirstNode;              // Zeiger mit ersten Listenknoten initialisieren
   while(pNext)                               // solange Listenknoten vorhanden
   {
      if (i==n) return pNext;
      pNext = pNext->pNext;
      i++;
   }
   return NULL;
}

/*******************************************************************************
* Name       : DeleteObject
* Definition : void DeleteObject(void *);
* Zweck      : Default Löschfunktion für die Elemente in der Liste
* Aufruf     : DeleteObject(p) (private)
* Parameter  : 
* p     (E) : Zeiger auf das Objekt                                   (void*)
*******************************************************************************/
void CEtsList::DeleteObject(void *p)
{
   delete p;
}

/*************************************************************************************
* Definition : void CEtsList::SetDeleteFunction(DELETEFUNCTION);
* Zweck      : Ersetzt die Standardlöschfunktion durch ein Objektspezifische.
* Parameter (E): pfn : Löschfunktionzeiger                        (DELETEFUNCTION)
**************************************************************************************/
void CEtsList::SetDeleteFunction(DELETEFUNCTION pfn)
{
   if (pfn) m_pfnDelete = pfn;
   else     m_pfnDelete = CEtsList::DeleteObject;
}

#ifdef _DEBUG
bool CEtsList::IsValid()
{
   CNode * pTest = m_pFirstNode;              // Zeiger mit ersten Listenknoten initialisieren
   int i = 1;
   pTest = pTest->pNext;
   while(pTest)                               // solange Listenknoten vorhanden
   {
      if (pTest->pPrevious == NULL) return false;
      if (pTest->pData     == NULL) return false;
      pTest = pTest->pNext;
      i++;
   }

   return (i==m_nCounter) ? true : false;
}
#endif

/*************************************************************************************
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter (E): 
* Funktionswert: 
**************************************************************************************/
void CEtsList::ProcessWithObjects(PROCESSFUNCTION pfn, WPARAM wParam, LPARAM lParam)
{
   CNode * pTest = m_pFirstNode;              // Zeiger mit ersten Listenknoten initialisieren
   while(pTest)                               // solange Listenknoten vorhanden
   {
      if (pfn(pTest->pData, wParam, lParam) == 0) break;
      pTest = pTest->pNext;
   }
}
