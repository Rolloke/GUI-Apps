================================================================================
    MICROSOFT FOUNDATION CLASS LIBRARY : MenuToolTip Projektübersicht
================================================================================

Der Anwendungs-Assistent hat diese MenuToolTip-Anwendung erstellt. 
Diese Anwendung zeigt die prinzipielle Anwendung der Microsoft Foundation Classes 
und dient als Ausgangspunkt für die Erstellung Ihrer eigenen Anwendungen.

Diese Datei enthält die Zusammenfassung der Bestandteile aller Dateien, die 
Ihre MenuToolTip-Anwendung bilden.

MenuToolTip.vcproj
    Dies ist die Hauptprojektdatei für VC++-Projekte, die vom Anwendungs-Assistenten 
    erstellt wird. Sie enthält Informationen über die Version von Visual C++, mit der
    die Datei generiert wurde, über die Plattformen, Konfigurationen und Projektfeatures,
    die mit dem Anwendungs-Assistenten ausgewählt wurden.

MenuToolTip.h
    Hierbei handelt es sich um die Haupt-Headerdatei der Anwendung. Diese enthält 
    andere projektspezifische Header (einschließlich Resource.h) und deklariert die
    CMenuToolTipApp-Anwendungsklasse.

MenuToolTip.cpp
    Hierbei handelt es sich um die Haupt-Quellcodedatei der Anwendung. Diese enthält die
    Anwendungsklasse CMenuToolTipApp.

MenuToolTip.rc
    Hierbei handelt es sich um eine Auflistung aller Ressourcen von Microsoft Windows, die 
    vom Programm verwendet werden. Sie enthält die Symbole, Bitmaps und Cursors, die im 
    Unterverzeichnis RES gespeichert sind. Diese Datei lässt sich direkt in Microsoft
    Visual C++ bearbeiten. Ihre Projektressourcen befinden sich in 1031.

res\MenuToolTip.ico
    Dies ist eine Symboldatei, die als Symbol für die Anwendung verwendet wird. Dieses 
    Symbol wird durch die Haupt-Ressourcendatei MenuToolTip.rc eingebunden.

res\MenuToolTip.rc2
    Diese Datei enthält Ressourcen, die nicht von Microsoft Visual C++ bearbeitet wurden.
    In dieser Datei werden alle Ressourcen gespeichert, die vom Ressourcen-Editor nicht bearbeitet 
    werden können.
/////////////////////////////////////////////////////////////////////////////

Für das Hauptfenster:
    Das Projekt enthält eine MFC-Standardschnittstelle.
MainFrm.h, MainFrm.cpp
    Diese Dateien enthalten die Frame-Klasse CMainFrame, die von
    CFrameWnd abgeleitet wird und steuert alle SDI-Framefeatures.
res\Toolbar.bmp
    Diese Bitmap-Datei wird zum Erstellen unterteilter Bilder für die Symbolleiste verwendet.
    Die erste Symbol- und Statusleiste wird in der Klasse CMainFrame
    erstellt. Bearbeiten Sie diese Bitmap der Symbolleiste mit dem Ressourcen-Editor, und
    aktualisieren Sie das Array IDR_MAINFRAME TOOLBAR in MenuToolTip.rc, um 
    Schaltflächen für die Symbolleiste hinzuzufügen.
/////////////////////////////////////////////////////////////////////////////

Der Anwendungs-Assistent erstellt einen Dokumenttyp und eine Ansicht:

MenuToolTipDoc.h, MenuToolTipDoc.cpp - das Dokument
    Diese Dateien enthalten die Klasse CMenuToolTipDoc.  Bearbeiten Sie diese Dateien,
    um Ihre speziellen Dokumentdaten hinzuzufügen und das Speichern und Laden von 
    Dateien zu implementieren (mit CMenuToolTipDoc::Serialize).
MenuToolTipView.h, MenuToolTipView.cpp - die Ansicht des Dokuments
    Diese Dateien enthalten die Klasse CMenuToolTipView.
    CMenuToolTipView-Objekte werden verwendet, um CMenuToolTipDoc-Objekte anzuzeigen.
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
    MenuToolTip.pch und eine vorkompilierte Typdatei mit der Bezeichnung 
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
