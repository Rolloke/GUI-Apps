#if !defined _ETSREGISTRY_H__INCLUDED_
#define _ETSREGISTRY_H__INCLUDED_

#define WIN32_LEAN_AND_MEAN		// Selten benutzte Teile der Windows-Header nicht einbinden

#include <windows.h>

/******************************************************************************
* Name       : SetGlobalRegParam                                              *
* Definition : bool SetGlobalRegParam(char *, char *, char *, bool);          *
* Zweck      : Registryparameter setzen                                       *
* Aufruf     : SetGlobalRegParam(pszModule, pszEtsKiel, pszSoftware, bIni);   *
* Parameter  :                                                                *
* pszModule   (E): Name des Moduls                         [optional](char*)  *
* pszEtsKiel  (E): Firmen Name                             [optional](char*)  *
* pszSoftware (E): Key "Software" in LOCAL_USER            [optional](char*)  *
* bIni        (E): Inidatei (true, false) Name = pszModule [optional](bool*)  *
* Funktionswert : keiner                                                      *
******************************************************************************/
void  SetGlobalRegParam(char *, char *, char *, bool);

/******************************************************************************
* Name       : OpenRegistry                                                   *
* Definition : bool OpenRegistry(HINSTANCE, REGSAM, PHKEY, char *);           *
* Zweck      : Öffnen der Registrierdatei                                     *
* Aufruf     : OpenRegistry(hInstance, regsam, phKey, pszSubKey);             *
* Parameter  :                                                                *
* hInstance(E): Instance Handle des Moduls                         (HINSTANCE)*
* regsam   (E): Security Acces Mask (KEY_WRITE, KEY_READ)           (REGSAM)  *
* phKey    (E): Registrierschlüssel                                 (HKEY*)   * 
* pszSubKey(E): Name des Unterschlüssels                            (char*)   *
* Funktionswert : Registrierdatei geöffnet (true, false)            (bool)    *
******************************************************************************/
bool  OpenRegistry(HINSTANCE, REGSAM, PHKEY, char*, bool *pbDebug=NULL);

/******************************************************************************
* Name       : EtsRegCloseKey                                                 *
* Definition : bool EtsRegCloseKey(HKEY&);                                    *
* Zweck      : Schließen des Registryschlüssels                               *
* Aufruf     : EtsRegCloseKey(hKey);                                          *
* Parameter  :                                                                *
* hKey   (EA): Registrierschlüssel                                 (HKEY&)    * 
* Funktionswert: Schlüssel wurde freigegeben (true, false)         (bool)     *
******************************************************************************/
bool  EtsRegCloseKey(HKEY &);

/******************************************************************************
* Name       : GetRegDWord, SetRegDWord                                       *
* Definition : DWORD GetRegDWord(HKEY, char *, DWORD);                        *
* Zweck      : Liefert / setzt den Wert des Registrierdateieintrags           *
* Aufruf     : GetRegDWord(hKey, pszKey, dwDef);                              *
*              SetRegDWord(hKey, pszKey, dwDef);                              *
* Parameter  :                                                                *
* hKey   (E) : geöffneter Registrierschlüssel                        (HKEY)   *
* pszKey (E) : Name des Schlüssels                                   (char*)  *
* dwDef  (E) : Defaultwert der zurückgegeben wird                    (DWORD)  *
* Funktionswert : Wert des Registrierdateieintrags                   (DWORD)  *
******************************************************************************/
DWORD GetRegDWord(HKEY, char*, DWORD);
bool  SetRegDWord(HKEY, char*, DWORD);

/******************************************************************************
* Name       : GetRegBinary (1), SetRegBinary (2)                             *
* Definition : bool GetRegBinary(HKEY,char *,void*,DWORD&,bool bText=false);  *
*              bool SetRegBinary(HKEY,char *,void*,DWORD, bool bText=false);  *
* Zweck      : Liefert / setzt den Inhalt des Registrierdateieintrags         *
* Aufruf     : GetRegBinary(hKey, pszKey, pParam, dwSize, [true]);            *
*              SetRegBinary(hKey, pszKey, pParam, dwSize, [true]);            *
* Parameter  :                                                                *
* hKey   (E) : geöffneter Registrierschlüssel                        (HKEY)   *
* pszKey (E) : Name des Schlüssels                                   (char*)  *
* pParam (EA): Zeiger auf Parameterfeld (1)                          (void*)  *
* pParam (E) : Zeiger auf Parameterfeld (2)                          (BYTE*)  *
* dwSize (EA): Größe Parameterfeldes    (1)                          (DWORD&) *
* dwSize (E) : Größe Parameterfeldes    (2)                          (DWORD)  *
* bText  (E) : Textstring (true, false)                              (bool)   *
* Wenn bText = true : es wird auf REG_SZ geprüft                              *
*  (1) : Eine 0 wird automatisch an den gelesenen String angehängt            *
*  (2) : Wenn (dwSize == 0) wird die Länge ermittelt                          *
* Funktionswert : Inhalt wurde gelesen (true, false)                 (bool)   *
******************************************************************************/
bool  GetRegBinary(HKEY, char*, void*, DWORD&, bool bText=false);
bool  SetRegBinary(HKEY, char*, BYTE*, DWORD,  bool bText=false);

#endif //_ETSREGISTRY_H__INCLUDED_