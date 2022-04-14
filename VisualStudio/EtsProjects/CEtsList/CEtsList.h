#ifndef _CEtsList_
#define _CEtsList_

/*******************************************************************************
                                 CEtsList
                                 ---------------
                 Doppeltverkettete-Liste von Zeigern
            zur Verkettung wird das Knotenobjekt CNode verwendet
                    (C) 1999 ELAC Technische Software GmbH

                         VERSION 1.03 Stand 25.08.99

                                                   programmed by Oliver Wesnigk
                                                   edited by Rolf Ehlers
*******************************************************************************/
typedef int  (*SORTFUNCTION)   (const void *, const void *);
typedef void (*DELETEFUNCTION) (void *);
typedef int  (*PROCESSFUNCTION)(void *, WPARAM, LPARAM);

class CNode;                                     // Listenknotenobjekt Vordeklaration
class CEtsList
{
public:
   CEtsList(bool mode=true);
   CEtsList(const CEtsList*);
   ~CEtsList();
	static void * Alloc(size_t);
	static void Free(void*);

// Methodes public
public:
   void   SetDestroyMode(bool bD){m_bMode=bD;};  // Destroymode einstellen (==true dann wegen enthaltene Objekte mit gelöscht)
   bool   GetDestroyMode(){return m_bMode;};     // Destroymode lesen
	void   SetDeleteFunction(DELETEFUNCTION);     // Löschfunktion für die Objekte
   int    GetCounter()const {return m_nCounter;};// die Anzahl der enthaltenen Objekte zurückgeben
   void   ADDHead(void *);                       // ein Objekt am Ende in dieser Liste einfügen
   void   ADDHead_Splitted(CEtsList *);          // Objekte einer Liste am Ende in dieser Liste einfügen
   void   ADDTail(void *);                       // Objekt am Anfang in dieser Liste einfügen
   void   Delete(void *);                        // ein Objekt aus der Liste entfernen und das Objekt löschen !
   void   Remove(void *);                        // ein Objekt aus der Liste entfernen ohne das Objekt zu löschen
   void   Remove_Container(CEtsList *);          // Objekte einer Liste aus dieser Liste entfernen
   bool   IsFirst(void * pIs )const;             // Ist Objekt erstes Objekt dieser Liste ?
   bool   IsLast(void * pIs ) const;             // Ist Objekt letztes Objekt dieser Liste ?
   bool   InList(void * ) const;                 // Teste ob Objekt schon in der Liste

   void * GetFirst();                            // erstes Objekt aus Liste holen und als Aktuelles merken
   void * GetLast();                             // letztes Objekt aus der Liste holen und als Aktuelles merken
   void * GetPrevious();                         // Objekt vor dem Aktuellem holen und als Aktuelles merken
   void * GetNext();                             // Objekt nach dem Aktuellem holen und als Aktuelles merken
   void * GetAt(int nPos);                       // Objekt mit der Position nPos holen
   void * GetActual()         const;             // aktuelles Objekt holen
   void * GetActualPrevious() const;             // Objekt vor dem Aktuellen holen
   void * GetActualNext()     const;             // Objekt nach dem Aktuellen holen
   int    GetActualPosition() {return m_nActualIndex;}
	int    FindElement(SORTFUNCTION, void *) const;// 
	void   Sort(SORTFUNCTION);                    // Sortierfunktion
	void   InsertAfter(SORTFUNCTION, void *p);    // Sortiertes Einfügen in eine sortierte Liste
	void*  ReplaceAt(int, void *, bool);          // Ersetzen an einer bestimmten Position
	void   ProcessWithObjects(PROCESSFUNCTION, WPARAM, LPARAM);
   void   Destroy();

#ifdef _DEBUG
	bool IsValid();
#endif
   
protected:
	void quicksort(SORTFUNCTION, int, CNode*, int, CNode*);
	CNode* FindIndex(int i);

// Methodes private (Kommentar siehe CEtsList.cpp)
protected:
   void    IsValid(int);
   void   *GetObject(CNode *, int);
   void   *GetObject(CNode *)  const;
   void    ADDFirst(void *);
   CNode  *FindObjectList(void *) const;
   void    DeleteObjectList(CNode *);
	void    ErrorMessage(char*);

   static void DeleteObject(void*);

// Members
private:
   CNode * m_pFirstNode;                        // Pointer auf ersten Listenknoten
   CNode * m_pLastNode;                         // Pointer auf letzten Listenknoten
   CNode * m_pActualNode;                       // Pointer auf aktuellen Listenknoten
   bool    m_bMode;                             // Inhalt verwerfen wenn Liste zerstört wird
   int     m_nCounter;                          // Zähler für die Anzahl von Knoten(Objekten)
   int     m_nActualIndex;                      // Aktueller Index
   DELETEFUNCTION m_pfnDelete;                  // Funktion zum Löschen eines Objektes
};

#endif