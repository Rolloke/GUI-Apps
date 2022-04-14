#ifndef LSBOXDB_CFG_H
   #define LSBOXDB_CFG_H

#define MAX_BOXES                   8           // Anzahl der Boxen in der Übergabestruktur
#define BDD_NAME_LENGTH            32
#define BOX_NAME_LENGTH           512           // Maximale Länge des Lautsprecher-Dateinamens
#define BOX_DESCRIPTION_LENGTH     32           // Maximale Länge der Beschreibung
#define MANUFACTURER_NAME_LENGTH   64           // Maximale Länge des Herstellernamens

#define CARAWALK_VERSION          110           // Versionnummer der Datenbank
#define BOX_DATA_BASE            "LsBoxDb.cfg"  // Dateiname der Datenbank
#define CARA_TYPE                0x41524143     // Identifikation der Datenbank

struct BoxData
{
   char     szName[BOX_NAME_LENGTH];
   char     szDescription[BOX_DESCRIPTION_LENGTH];
   WORD     nType;
   WORD     nFiles;
   WORD     nMaterial;
   bool     bFound;
   char    *pszFiles;
};

class CEtsList;
struct Manufacturer
{
   char      szName[MANUFACTURER_NAME_LENGTH];
   CEtsList *pBoxes;
};

#endif // LSBOXDB_CFG_H