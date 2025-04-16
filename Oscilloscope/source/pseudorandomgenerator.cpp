#include "pseudorandomgenerator.h"
#include <time.h>

PseudoRandomGenerator::PseudoRandomGenerator():
    mPSBMG(0)
  , mGeneratorIndex(0)
{
    init_PSBMG();
    fillGenerator();
}



void PseudoRandomGenerator::init_PSBMG()
{
    do
    {
        time_t fTime = time(NULL);
        mPSBMG = static_cast<unsigned long>(fTime);
    }  while(mPSBMG == 0);
}

unsigned char PseudoRandomGenerator::getNext_PSBMG()
{
   const bool UseBits[bitfieldsize]=
   {
       false,false,false,false,false,false,false,true ,
       true ,false,false,false,false,false,true ,false,
       false,false,false,true ,false,false,false,false,
       false,true ,false,false,false,true ,false,false
   };

   int       i;
   unsigned long     g = mPSBMG;
   unsigned long     e = mPSBMG;

   for (i=0;i<32;i++)
   {
      if(UseBits[i]) e^= g;
      g>>=1;
   }

   mPSBMG<<=1;
   mPSBMG |=e&1;

   return (unsigned char) mPSBMG;
}



void PseudoRandomGenerator::fillGenerator()
{
   unsigned long value;
   for (int i=0;i<generatorfieldsize;i++)
   {
      value  = getNext_PSBMG();
      value<<= 8;
      value |= getNext_PSBMG();
      value<<= 8;
      value |= getNext_PSBMG();
      value<<= 8;
      value |= getNext_PSBMG();

      mGeneratorField[i] = value;
   }

   mGeneratorIndex = getNext_PSBMG();
}


long PseudoRandomGenerator::getRandomValue()
{
   int ia1 = (getNext_PSBMG() + mGeneratorIndex) & 255;
   int ia2 = (getNext_PSBMG() + mGeneratorIndex) & 255;

   mGeneratorIndex++;
   mGeneratorIndex&=generatorfieldsize;

   return (mGeneratorField[mGeneratorIndex] = mGeneratorField[ia1] + mGeneratorField[ia2]);
}

/*

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
   unsigned long SectorsPerCluster; // Informationen über die Festplatte
   unsigned long unsigned charsPerSector;
   unsigned long FreeClusters;
   unsigned long Clusters;
   unsigned long Time;              // Windowszeit
   unsigned long disk;
   unsigned long memory;

   do
   {
      QueryPerformanceCounter(&qpc1);     // Hochaufläsenden Zähler besorgen

      Sleep(1);                           // Thread kurz beenden (es ist nicht genau bekannt wie lange das dauert ! "Systemabhängig")

      ZeroMemory(&ms,sizeof(ms));         // Information über aktuelle Hauptspeicherbelegung besorgen
      ms.dwLength = sizeof(ms);
      GlobalMemoryStatus(&ms);

      Sleep(1);                           // Thread kurz beenden (es ist nicht genau bekannt wie lange das dauert ! "Systemabhängig")

      QueryPerformanceCounter(&qpc2);     // Hochauflösenden Zähler besorgen

      GetDiskFreeSpace("C:\\",            // Information über aktuelle Festplatte besorgen
                       &SectorsPerCluster,
                       &unsigned charsPerSector,
                       &FreeClusters,
                       &Clusters);

      disk = SectorsPerCluster;
      disk^= unsigned charsPerSector;
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
*/
