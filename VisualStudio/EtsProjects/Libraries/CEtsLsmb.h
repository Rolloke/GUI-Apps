#ifndef _CETSLSMB_
#define _CETSLSMB_

#define ETSLSMB_SAVE     0
#define ETSLSMB_LOAD     1
#define ETSLSMB_CALC     2
#define ETSLSMB_NOTHING -1
#define ETSLSMB_NOTPROC -2
#define ETSLSMB_STRINGLENGTH 64
#define ETSLSMB_MAX_STRING   16

struct ETSLSMB_CLSMB                             // Übergabeparameter für die LoadSaveMessageBox
{
   int  nLines;                                  // Anzahl der Textzeilen 1 bis 16
   int  nLoadSave;                               // Calc = 2, Load =1, Save = 0 (für den ANI-Cursor)
   TCHAR szText[ETSLSMB_MAX_STRING][ETSLSMB_STRINGLENGTH];// Speicher für die 16 Textzeilen mit Maximal 63 Zeichen
};

class CEtsLsmb
{
public:
  CEtsLsmb();
  CEtsLsmb(ETSLSMB_CLSMB*);
  ~CEtsLsmb();

  void Step(int icon = ETSLSMB_NOTHING);
  void Destructor();

private:

   unsigned int m_dwThreadID;
   HANDLE       m_hThread;
};
#endif