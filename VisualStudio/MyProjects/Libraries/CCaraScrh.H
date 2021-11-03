#ifndef _CARASCRH_
#define _CARASCRH_

/********************************************************************
*                                                                   *
*             Kommunikationsklasse für CARASCRH.DLL                 *
*                                                                   *
*           (c) 1998 ELAC Technische Software GmbH 1998             *
*                                                                   *
*                  Version 1.00 Stand 30.04.1998                    *
*                                                                   *
*                                       programmed by Oliver Wesnigk*
********************************************************************/



class CCaraScrh
{
public:
   ~CCaraScrh();                                 // entfernt CARASCRH aus dem Speicher und läßt wieder den
                                                 // registrierten Bildschirmschoner zu

   static void HookScreenSaverMessage(HWND,int); // legt das Fenster und die benutzerdefinierte Nachricht fest,
                                                 // welche an dieses Fenster zur Aktivierung des Bildschirmschoners
                                                 // gesendet werden soll und deaktiviert den registrierten Bildschirmschoner.
                                                 // Ist der Fensterhandle == NULL wird der Bildschirmschoner deaktiviert
                                                 // und wieder der registrierte Bildschirmschoner zugelassen
   static void ActivateScreenSaver(const char *);// schaltet den Bildschirmschoner mit übergebenen Ausgabetext ein.
                                                 // Dies ist die Antwort auf die vorher festgelegte benutzerdefinierte Nachricht.
   static void SetScreenSaverText(const char *); // ändert den Ausgabetext, wenn der Bildschirmschonen aktiv ist,
                                                 // ansonsten passiert nichts !
};

#endif