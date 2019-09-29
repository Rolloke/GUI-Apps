================================================================================
    MICROSOFT FOUNDATION CLASS LIBRARY : AnalyseMapFile Projektübersicht
================================================================================

Der Anwendungs-Assistent hat diese AnalyseMapFile-Anwendung erstellt. 
Diese Anwendung zeigt die prinzipielle Anwendung der Microsoft Foundation Classes 
und dient als Ausgangspunkt für die Erstellung Ihrer eigenen Anwendungen.

Diese Datei enthält die Zusammenfassung der Bestandteile aller Dateien, die 
Ihre AnalyseMapFile-Anwendung bilden.

AnalyseMapFile.vcproj
    Dies ist die Hauptprojektdatei für VC++-Projekte, die vom Anwendungs-Assistenten 
    erstellt wird. Sie enthält Informationen über die Version von Visual C++, mit der
    die Datei generiert wurde, über die Plattformen, Konfigurationen und Projektfeatures,
    die mit dem Anwendungs-Assistenten ausgewählt wurden.

AnalyseMapFile.h
    Hierbei handelt es sich um die Haupt-Headerdatei der Anwendung. Diese enthält 
    andere projektspezifische Header (einschließlich Resource.h) und deklariert die
    CAnalyseMapFileApp-Anwendungsklasse.

AnalyseMapFile.cpp
    Hierbei handelt es sich um die Haupt-Quellcodedatei der Anwendung. Diese enthält die
    Anwendungsklasse CAnalyseMapFileApp.

AnalyseMapFile.rc
    Hierbei handelt es sich um eine Auflistung aller Ressourcen von Microsoft Windows, die 
    vom Programm verwendet werden. Sie enthält die Symbole, Bitmaps und Cursors, die im 
    Unterverzeichnis RES gespeichert sind. Diese Datei lässt sich direkt in Microsoft
    Visual C++ bearbeiten. Ihre Projektressourcen befinden sich in 1031.

res\AnalyseMapFile.ico
    Dies ist eine Symboldatei, die als Symbol für die Anwendung verwendet wird. Dieses 
    Symbol wird durch die Haupt-Ressourcendatei AnalyseMapFile.rc eingebunden.

res\AnalyseMapFile.rc2
    Diese Datei enthält Ressourcen, die nicht von Microsoft Visual C++ bearbeitet wurden.
    In dieser Datei werden alle Ressourcen gespeichert, die vom Ressourcen-Editor nicht bearbeitet 
    werden können.
AnalyseMapFile.reg
    Dies ist eine REG-Beispieldatei, die anzeigt, welche Registrierungseinstellungen
    von Framework festgelegt werden. Diese können Sie als .REG-
    -Datei für Ihre Anwendung festlegen, oder diese löschen und die Standardregistrierung 
    RegisterShellFileTypes verwenden.
/////////////////////////////////////////////////////////////////////////////

Für das Hauptfenster:
    Windows Explorer-Stil: Das Projekt enthält eine Windows Explorer-Schnittstelle mit zwei Rahmen.
MainFrm.h, MainFrm.cpp
    Diese Dateien enthalten die Frame-Klasse CMainFrame, die von
    CFrameWnd abgeleitet wird und steuert alle SDI-Framefeatures.
LeftView.h, LeftView.cpp
    Diese Dateien enthalten die linke Frame-Klasse CLeftView, die von
    CTreeView abgeleitet wird.
res\Toolbar.bmp
    Diese Bitmap-Datei wird zum Erstellen unterteilter Bilder für die Symbolleiste verwendet.
    Die erste Symbol- und Statusleiste wird in der Klasse CMainFrame
    erstellt. Bearbeiten Sie diese Bitmap der Symbolleiste mit dem Ressourcen-Editor, und
    aktualisieren Sie das Array IDR_MAINFRAME TOOLBAR in AnalyseMapFile.rc, um 
    Schaltflächen für die Symbolleiste hinzuzufügen.
/////////////////////////////////////////////////////////////////////////////

Der Anwendungs-Assistent erstellt einen Dokumenttyp und eine Ansicht:

AnalyseMapFileDoc.h, AnalyseMapFileDoc.cpp - das Dokument
    Diese Dateien enthalten die Klasse CAnalyseMapFileDoc.  Bearbeiten Sie diese Dateien,
    um Ihre speziellen Dokumentdaten hinzuzufügen und das Speichern und Laden von 
    Dateien zu implementieren (mit CAnalyseMapFileDoc::Serialize).
    Das Dokument enthält folgende Zeichenfolgen:
        Datei erweiterung:      map
        Dateityp-ID:            AnalyseMapFile.Document
        Hauptrahmenbezeichnung: AnalyseMapFile
        Dokumenttypname:        AnalyseMapFile
        Filtername:             AnalyseMapFile Files (*.map)
        Neuer kurzer Dateiname: AnalyseMapFile
        Langer Dateitypname:    AnalyseMapFile.Document
AnalyseMapFileView.h, AnalyseMapFileView.cpp - die Ansicht des Dokuments
    Diese Dateien enthalten die Klasse CAnalyseMapFileView.
    CAnalyseMapFileView-Objekte werden verwendet, um CAnalyseMapFileDoc-Objekte anzuzeigen.
/////////////////////////////////////////////////////////////////////////////

Weitere Features:

ActiveX-Steuerelemente
    Die Anwendung unterstützt die Verwendung von ActiveX-Steuerelementen.

Unterstützung für das Drucken und die Seitenansicht
    Der Anwendungs-Assistent hat den Code zum Verarbeiten der Befehle "Drucken", "Seite einrichten" und 
    "Seitenansicht" durch Aufrufen der Memberfuntkionen in der CView-Klasse der MFC-Bibliothek generiert.
/////////////////////////////////////////////////////////////////////////////

Weitere Standarddateien:

StdAfx.h, StdAfx.cpp
    Mit diesen Dateien werden vorkompilierte Headerdateien (PCH) mit der Bezeichnung
    AnalyseMapFile.pch und eine vorkompilierte Typdatei mit der Bezeichnung 
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
