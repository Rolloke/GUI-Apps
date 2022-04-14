================================================================================
    MICROSOFT FOUNDATION CLASS LIBRARY : HardwareInfo Projektübersicht
================================================================================

Der Anwendungs-Assistent hat diese HardwareInfo-Anwendung erstellt. 
Diese Anwendung zeigt die prinzipielle Anwendung der Microsoft Foundation Classes 
und dient als Ausgangspunkt für die Erstellung Ihrer eigenen Anwendungen.

Diese Datei enthält die Zusammenfassung der Bestandteile aller Dateien, die 
Ihre HardwareInfo-Anwendung bilden.

HardwareInfo.vcproj
    Dies ist die Hauptprojektdatei für VC++-Projekte, die vom Anwendungs-Assistenten 
    erstellt wird. Sie enthält Informationen über die Version von Visual C++, mit der
    die Datei generiert wurde, über die Plattformen, Konfigurationen und Projektfeatures,
    die mit dem Anwendungs-Assistenten ausgewählt wurden.

HardwareInfo.h
    Hierbei handelt es sich um die Haupt-Headerdatei der Anwendung. Diese enthält 
    andere projektspezifische Header (einschließlich Resource.h) und deklariert die
    CHardwareInfoApp-Anwendungsklasse.

HardwareInfo.cpp
    Hierbei handelt es sich um die Haupt-Quellcodedatei der Anwendung. Diese enthält die
    Anwendungsklasse CHardwareInfoApp.

HardwareInfo.rc
    Hierbei handelt es sich um eine Auflistung aller Ressourcen von Microsoft Windows, die 
    vom Programm verwendet werden. Sie enthält die Symbole, Bitmaps und Cursors, die im 
    Unterverzeichnis RES gespeichert sind. Diese Datei lässt sich direkt in Microsoft
    Visual C++ bearbeiten. Ihre Projektressourcen befinden sich in 1031.

res\HardwareInfo.ico
    Dies ist eine Symboldatei, die als Symbol für die Anwendung verwendet wird. Dieses 
    Symbol wird durch die Haupt-Ressourcendatei HardwareInfo.rc eingebunden.

res\HardwareInfo.rc2
    Diese Datei enthält Ressourcen, die nicht von Microsoft Visual C++ bearbeitet wurden.
    In dieser Datei werden alle Ressourcen gespeichert, die vom Ressourcen-Editor nicht bearbeitet 
    werden können.

/////////////////////////////////////////////////////////////////////////////

Der Anwendungs-Assistent erstellt eine Dialogklasse:
HardwareInfoDlg.h, HardwareInfoDlg.cpp - das Dialogfeld
    Diese Dateien enthalten die Klasse CHardwareInfoDlg. Diese Klasse legt das
    Verhalten des Haupt-Dialogfelds der Anwendung fest. Die Vorlage des Dialog-
    felds befindet sich in HardwareInfo.rc, die mit Microsoft Visual C++
    bearbeitet werden kann.
/////////////////////////////////////////////////////////////////////////////

Weitere Features:

ActiveX-Steuerelemente
    Die Anwendung unterstützt die Verwendung von ActiveX-Steuerelementen.
/////////////////////////////////////////////////////////////////////////////

Weitere Standarddateien:

StdAfx.h, StdAfx.cpp
    Mit diesen Dateien werden vorkompilierte Headerdateien (PCH) mit der Bezeichnung
    HardwareInfo.pch und eine vorkompilierte Typdatei mit der Bezeichnung 
    StdAfx.obj erstellt.

Resource.h
    Dies ist die Standard-Headerdatei, die neue Ressourcen-IDs definiert.
    Microsoft Visual C++ liest und aktualisiert diese Datei.

/////////////////////////////////////////////////////////////////////////////

Weitere Hinweise:

Der Anwendungs-Assistent verwendet "TODO:", um die Stellen anzuzeigen, 
an denen Sie Erweiterungen oder Anpassungen vornehmen können.

Wenn Ihre Anwendung die MFC in einer gemeinsam genutzten DLL verwendet und 
eine andere als die aktuell auf dem Betriebssystem eingestellte Sprache verwendet, muss 
die entsprechend lokalisierte Ressource MFC70XXX.DLL von der Microsoft Visual C++ CD-ROM 
in das Verzeichnis system oder system32 kopiert und in MFCLOC.DLL umbenannt werden  
("XXX" steht für die Abkürzung der Sprache. So enthält beispielsweise MFC70DEU.DLL die ins Deutsche 
übersetzten Ressourcen.)  Anderenfalls werden einige Oberflächenelemente Ihrer Anwendung 
in der Sprache des Betriebssystems angezeigt.

/////////////////////////////////////////////////////////////////////////////
