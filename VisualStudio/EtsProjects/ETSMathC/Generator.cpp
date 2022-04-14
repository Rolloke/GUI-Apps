#include <windows.h>
#include <stdio.h>

DWORD g_GeneratorField[256];                     // Feld für den Additiven Generator (Wiederholänge ca. 2^256-1)
DWORD g_GeneratorIndex;                          // Arbeitsindex für den Additiven Generator
DWORD g_PSBMG;                                   // Arbeitsfeld für den PseudoStatistischer Bitmuster Generator (Wiederhollänge ca. 2^31-1)


void  Init_PSBMG();
BYTE  GetNext_PSBMG();
void  Fill_Generator();
DWORD GetNext_Generator();

void main()
{
   Init_PSBMG();                                 // bilde den Startwert für den Pseudo Statistischen Bitmuster Generator

   Fill_Generator();                             // fülle das Start Array für den Additiven Generator unter Verwendung des PSBMG

   GetNext_Generator();                          // eine Zahl besorgen
}

void Init_PSBMG()
{
   MEMORYSTATUS ms;         // Struktur für Hauptspeicherinformationen
   LARGE_INTEGER qpc1;      // Struktur für vergangene Zeit nach Systemstart
   LARGE_INTEGER qpc2;      // Struktur für vergangene Zeit nach Systemstart
   LARGE_INTEGER qpc3;      // Struktur für vergangene Zeit nach Systemstart
   DWORD SectorsPerCluster; // Informationen über die Festplatte
   DWORD BytesPerSector;
   DWORD FreeClusters;
   DWORD Clusters;
   DWORD Time;              // Windowszeit
   DWORD disk;
   DWORD memory;

   do
   {
      QueryPerformanceCounter(&qpc1);     // Hochauflösenden Zähler besorgen

      Sleep(1);                           // Thread kurz beenden (es ist nicht genau bekannt wie lange das dauert ! "Systemabhängig")

      ZeroMemory(&ms,sizeof(ms));         // Information über aktuelle Hauptspeicherbelegung besorgen
      ms.dwLength = sizeof(ms);
      GlobalMemoryStatus(&ms); 

      Sleep(1);                           // Thread kurz beenden (es ist nicht genau bekannt wie lange das dauert ! "Systemabhängig")

      QueryPerformanceCounter(&qpc2);     // Hochauflösenden Zähler besorgen

      GetDiskFreeSpace("C:\\",            // Information über aktuelle Festplatte besorgen
                       &SectorsPerCluster,
                       &BytesPerSector,
                       &FreeClusters,
                       &Clusters);

      disk = SectorsPerCluster;
      disk^= BytesPerSector;
      disk^= FreeClusters;
      disk^= Clusters;

      memory = ms.dwTotalPhys;
      memory^= ~ms.dwAvailPhys;
      memory^= ms.dwTotalPageFile;
      memory^= ~ms.dwAvailPageFile;
      memory^= ms.dwTotalVirtual;
      memory^= ~ms.dwAvailVirtual;

      Sleep(1);                           // Thread kurz beenden (es ist nicht genau bekannt wie lange das dauert ! "Systemabhängig")

      Time   = GetCurrentTime();          // vergangene Zeit nach Windowsstart

      Sleep(1);                           // Thread kurz beenden (es ist nicht genau bekannt wie lange das dauert ! "Systemabhängig")

      QueryPerformanceCounter(&qpc3);     // Hochauflösenden Zähler besorgen

      g_PSBMG  = memory;                  // Daten über den Hauptspeicher
      g_PSBMG<<=1;                        // Verschieben
      g_PSBMG ^= qpc1.LowPart;            // Xor mit Hochauflösenden Zähler Get(1)
      g_PSBMG<<=1;                        // Verschieben
      g_PSBMG ^= disk;                    // Xor mit Daten über die Festplatte
      g_PSBMG<<=1;                        // Verschieben
      g_PSBMG ^= ~qpc3.LowPart;           // Xor mit Hochauflösenden Zähler Get(3)
      g_PSBMG<<=1;                        // Verschieben
      g_PSBMG ^= Time;                    // Xor mit Windowszeit
      g_PSBMG<<=1;                        // Verschieben
      g_PSBMG ^= qpc2.LowPart;            // Xor mit Hochauflösenden Zähler Get(2)
   }while(g_PSBMG==0);                    // g_PSBMG darf nicht 0 sein 1
}



BYTE GetNext_PSBMG()
{
   const bool UseBits[32]={false,false,false,false,false,false,false,true ,
                           true ,false,false,false,false,false,true ,false,
                           false,false,false,true ,false,false,false,false,
                           false,true ,false,false,false,true ,false,false};
   int       i;
   DWORD     g = g_PSBMG;
   DWORD     e = g_PSBMG;

   for (i=0;i<32;i++)
   {
      if(UseBits[i]) e^= g;
      g>>=1;
   }
   
   g_PSBMG<<=1;
   g_PSBMG |=e&1;

   return (BYTE) g_PSBMG;
}



void Fill_Generator()
{
   DWORD value;
   for (int i=0;i<256;i++)
   {
      value  = GetNext_PSBMG();
      value<<= 8;
      value |= GetNext_PSBMG();
      value<<= 8;
      value |= GetNext_PSBMG();
      value<<= 8;
      value |= GetNext_PSBMG();

      g_GeneratorField[i] = value;
   }

   g_GeneratorIndex = GetNext_PSBMG();
}


DWORD GetNext_Generator()
{
   int ia1 = (GetNext_PSBMG() + g_GeneratorIndex) & 255;
   int ia2 = (GetNext_PSBMG() + g_GeneratorIndex) & 255;

   g_GeneratorIndex++;
   g_GeneratorIndex&=255;

   return (g_GeneratorField[g_GeneratorIndex] = g_GeneratorField[ia1] + g_GeneratorField[ia2]);
}
