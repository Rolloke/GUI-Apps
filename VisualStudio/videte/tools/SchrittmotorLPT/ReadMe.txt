================================================================================
    MICROSOFT FOUNDATION CLASS-BIBLIOTHEK: 
    SchrittmotorLPT-Projektübersicht
===============================================================================

Der Anwendungs-Assistent hat diese SchrittmotorLPT-Anwendung für Sie 
erstellt. Diese Anwendung zeigt nicht nur die Grundlagen der Verwendung von 
Microsoft Foundation Classes, sondern dient auch als Ausgangspunkt für das 
Schreiben Ihrer Anwendung.

Diese Datei bietet eine Übersicht über den Inhalt der einzelnen Dateien, aus
denen Ihre SchrittmotorLPT-Anwendung besteht.

SchrittmotorLPT.vcproj
    Dies ist die Hauptprojektdatei für VC++-Projekte, die mithilfe eines 
    Anwendungs-Assistenten erstellt werden.
    Sie enthält Informationen zur Visual C++-Version, mit der die Datei 
    generiert wurde, sowie Informationen zu Plattformen, Konfigurationen und 
    Projektfeatures, die mit dem Anwendungs-Assistenten ausgewählt wurden.

SchrittmotorLPT.h
    Dies ist die Hauptheaderdatei für die Anwendung. Sie enthält weitere
    projektspezifische Header (einschließlich "Resource.h") und deklariert
    die CSchrittmotorLPTApp-Anwendungsklasse.

SchrittmotorLPT.cpp
    Dies ist die Quelldatei der Hauptanwendung, die die CSchrittmotorLPTApp-
    Anwendungsklasse enthält.

SchrittmotorLPT.rc
    Dies ist eine Auflistung aller vom Programm verwendeten 
    Microsoft Windows-Ressourcen. Sie enthält die Symbole, Bitmaps und Cursor, 
    die im Unterverzeichnis "RES" gespeichert werden. Diese Datei kann direkt 
    in Microsoft Visual C++ bearbeitet werden. Ihre Projektressourcen befinden 
    sich in 1031.

res\SchrittmotorLPT.ico
    Dies ist eine Symboldatei, die als Symbol der Anwendung verwendet wird. 
    Dieses Symbol ist in der Hauptressourcendatei "SchrittmotorLPT.rc" 
    enthalten.

res\SchrittmotorLPT.rc2
    Diese Datei enthält Ressourcen, die nicht von Microsoft Visual C++ 
    bearbeitet werden. Sie sollten alle Ressourcen, die nicht mit dem
    Ressourcen-Editor bearbeitet werden können, in dieser Datei platzieren.


/////////////////////////////////////////////////////////////////////////////

Der Anwendungs-Assistent erstellt eine Dialogfeldklasse:

SchrittmotorLPTDlg.h, SchrittmotorLPTDlg.cpp – das Dialogfeld
    Diese Dateien enthalten Ihre CSchrittmotorLPTDlg-Klasse. Diese Klasse 
    bestimmt das Verhalten des Hauptdialogfelds Ihrer Anwendung. Die Vorlage 
    des Dialogfelds befindet sich in der Datei "SchrittmotorLPT.rc", die 
    in Microsoft Visual C++ bearbeitet werden kann.


/////////////////////////////////////////////////////////////////////////////

Weitere Funktionen:

Druck- und Seitenansichtunterstützung
    Der Anwendungs-Assistent hat Code generiert, um die Befehle für Drucken, 
    Druckeinrichtung und Seitenansicht zu verarbeiten, indem Memberfunktionen 
    in der CView-Klasse aus der MFC-Bibliothek aufgerufen werden.

/////////////////////////////////////////////////////////////////////////////

Andere Standarddateien:

StdAfx.h, StdAfx.cpp
    Mit diesen Dateien werden eine vorkompilierte Headerdatei (PCH)
    mit dem Namen "SchrittmotorLPT.pch" und eine 
    vorkompilierte Typendatei mit dem Namen "StdAfx.obj" erstellt.

Resource.h
    Dies ist die Standardheaderdatei, die neue Ressourcen-IDs definiert.
    Die Datei wird mit Microsoft Visual C++ gelesen und aktualisiert.

SchrittmotorLPT.manifest
	Anwendungsmanifestdateien werden in Windows XP zur Beschreibung einer
	Anwendungsabhängigkeit von verschiedenen Versionen paralleler 
        Assemblys verwendet. 
	Das Ladeprogramm verwendet diese Informationen, um die entsprechende 
	Assembly aus dem Assemblycache oder privat aus der Anwendung zu laden.
	Das Anwendungsmanifest kann zur Verteilung als externe Manifestdatei 
        enthalten sein, die im gleichen Ordner installiert ist wie die 
        ausführbare Datei der Anwendung, oder sie kann in Form einer Ressource 
        in der ausführbaren Datei enthalten sein.
/////////////////////////////////////////////////////////////////////////////

Weitere Hinweise:

Der Anwendungs-Assistent verwendet "TODO:", um auf Teile des Quellcodes
hinzuweisen, die Sie ergänzen oder anpassen sollten.

Wenn Ihre Anwendung MFC in einer gemeinsam genutzten DLL verwendet, 
müssen Sie die MFC-DLLs verteilen. Wenn die Anwendung eine andere Sprache 
als die des Gebietsschemas des Betriebssystems verwendet, müssen Sie außerdem 
die entsprechenden lokalisierten Ressourcen in "MFC90XXX.DLL" verteilen.
Weitere Informationen zu diesen beiden Themen finden Sie im Abschnitt 
zum Verteilen von Visual C++-Anwendungen in der Dokumentation zu MSDN.

/////////////////////////////////////////////////////////////////////////////
