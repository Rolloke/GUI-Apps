<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.0" language="de_DE">
<context>
    <name>AboutDlg</name>
    <message>
        <location filename="aboutdlg.ui" line="14"/>
        <source>Dialog</source>
        <translation>Dialog</translation>
    </message>
    <message>
        <location filename="aboutdlg.ui" line="26"/>
        <source>Author:</source>
        <translation>Author:</translation>
    </message>
    <message>
        <location filename="aboutdlg.ui" line="39"/>
        <source>Rolf Kary-Ehlers</source>
        <translation>Rolf Kary-Ehlers</translation>
    </message>
    <message>
        <location filename="aboutdlg.ui" line="52"/>
        <source>Version:</source>
        <translation>Version:</translation>
    </message>
    <message>
        <location filename="aboutdlg.ui" line="65"/>
        <source>1.1</source>
        <translation>1.1</translation>
    </message>
    <message>
        <location filename="aboutdlg.ui" line="78"/>
        <source>License:</source>
        <translation>Lizenz:</translation>
    </message>
    <message>
        <location filename="aboutdlg.ui" line="91"/>
        <source>GNU GPL Version 2</source>
        <translation>GNU GPL Version 2</translation>
    </message>
    <message>
        <location filename="aboutdlg.ui" line="170"/>
        <source>The program is provided AS IS with NO WARRANTY OF ANY KIND</source>
        <translation>Das Programm wird ohne jegliche Gewährleistung bereitgestellt</translation>
    </message>
    <message>
        <location filename="aboutdlg.ui" line="186"/>
        <source>Email:</source>
        <translation>Email:</translation>
    </message>
    <message>
        <location filename="aboutdlg.ui" line="199"/>
        <source>rolf-kary-ehlers@t-online.de</source>
        <translation>rolf-kary-ehlers@t-online.de</translation>
    </message>
    <message>
        <location filename="aboutdlg.ui" line="218"/>
        <source>Viewer for git repositories. Based on Qt 5.6 with c++ 14</source>
        <translation>Betrachter für git Repositories. Programmiert mit Qt5.6 und c++17 </translation>
    </message>
    <message>
        <location filename="aboutdlg.ui" line="237"/>
        <source>Built</source>
        <translation>Erstellt</translation>
    </message>
    <message>
        <location filename="aboutdlg.ui" line="250"/>
        <source>Build on:</source>
        <translation>Erstellt am:</translation>
    </message>
    <message>
        <location filename="aboutdlg.cpp" line="11"/>
        <source>About </source>
        <translation>Über </translation>
    </message>
    <message>
        <location filename="aboutdlg.cpp" line="14"/>
        <source>, </source>
        <translation>,</translation>
    </message>
</context>
<context>
    <name>CommitMessage</name>
    <message>
        <location filename="commitmessage.ui" line="14"/>
        <source>Enter a commit Message</source>
        <translation>Commit Beschreibung eingeben</translation>
    </message>
    <message>
        <location filename="commitmessage.ui" line="42"/>
        <source>Description for the commitment of the changes</source>
        <translation>Beschreibung der angewandten Änderungen</translation>
    </message>
    <message>
        <location filename="commitmessage.ui" line="55"/>
        <source>Check to automatically skip the staging area</source>
        <translation>Automatisch die &apos;staging&apos; Stufe überspringen</translation>
    </message>
    <message>
        <location filename="commitmessage.ui" line="58"/>
        <source>stage automatically</source>
        <translation>Automatisch &apos;stagen&apos;</translation>
    </message>
</context>
<context>
    <name>CustomGitActions</name>
    <message>
        <location filename="customgitactions.ui" line="17"/>
        <source>Settings for custom git actions</source>
        <translation>Einstellungen für Benutzer git Kommandos</translation>
    </message>
    <message>
        <source>Shows git commands</source>
        <translation>Zeigt git Kommandos</translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="124"/>
        <source>Add command to menue or toolbar</source>
        <translation>Kommando zur Menü Werkzeugleiste hinzufügen</translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="138"/>
        <location filename="customgitactions.ui" line="152"/>
        <source>Move selected menue or toolbar item</source>
        <translation>Gewählten Menüeintrag bewegen</translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="166"/>
        <source>Add curstom git command entry</source>
        <translation>Benutzer git Kommando hinzufügen</translation>
    </message>
    <message>
        <source>Remove custom git command</source>
        <translation>Benuzer git Kommando enfernen</translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="33"/>
        <source>&lt;html&gt;
  &lt;head/&gt;
  &lt;body&gt;&lt;h4&gt;Displays application commands that can be edited here.&lt;/h4&gt;&lt;ul&gt;
      &lt;li&gt;Add a new custom command with the &apos;+&apos; button
          &lt;ul&gt;
                &lt;li&gt;git -C %1 &amp;lt;command&amp;gt; %2
                &lt;ul&gt;
                   &lt;li&gt;%1 represents the git repository&lt;/li&gt;
                   &lt;li&gt;%2 [optional] may represent the the selected file or folder&lt;/li&gt;
               &lt;/ul&gt;
               &lt;/li&gt;
                &lt;li&gt;git &amp;lt;command&amp;gt; %1  %2
                &lt;ul&gt;
                   &lt;li&gt;%1 is used for command options depending on case
                  &lt;ul&gt;
                     &lt;li&gt;--staged for diff of staged file&lt;/li&gt;
                     &lt;li&gt;--tool=&amp;lt;diff-tool&amp;gt;&lt;/li&gt;
                   &lt;/ul&gt;
                  &lt;/li&gt;
                   &lt;li&gt;%2 [optional] may represents the the selected file or folder&lt;/li&gt;
               &lt;/ul&gt;
               &lt;/li&gt;
            &lt;/ul&gt;&lt;/li&gt;
      &lt;li&gt;Remove selected custom command with &apos;-&apos; button&lt;/li&gt;
      &lt;li&gt;Assign a selected command to toolbar or context menu in right view with &apos;--&amp;gt;&apos; button&lt;/li&gt;
      &lt;li&gt;Edit commands (custom or built in) by double click on table entry
          &lt;ul&gt;
              &lt;li&gt;all &apos;git&apos; commands may be edited due to parameters or options&lt;/li&gt;
              &lt;li&gt;all names may be edited (the name should represent the commands functionality)&lt;/li&gt;
              &lt;li&gt;all shortcuts may be edited&lt;/li&gt;
              &lt;li&gt;A message box text may be edited or added
                &lt;ul&gt;
                   &lt;li&gt;&apos;None&apos; or empty defines that no message box is invoked&lt;/li&gt;
                   &lt;li&gt;The message box is invoked before execution of the git command&lt;/li&gt;
                   &lt;li&gt;&lt;b&gt;Format: &lt;/b&gt;Heading [%1];Question [%1]&lt;/li&gt;
                   &lt;li&gt;The parameter %1 is optional an displays the selected file or folder&lt;/li&gt;
               &lt;/ul&gt;
              &lt;/li&gt;
       &lt;/ul&gt;
     &lt;/li&gt;
     &lt;li&gt;&lt;b&gt;Note:&lt;/b&gt;The correct functionality may be lost by editing&lt;br&gt;
             however this can be repaired by &apos;reset modifications&apos; in context menu&lt;/li&gt;
     &lt;/ul&gt;
  &lt;/body&gt;
&lt;/html&gt;</source>
        <translation>&lt;html&gt;
  &lt;head/&gt;
  &lt;body&gt;&lt;h4&gt;Liste für editierbare Anwendungskommandos&lt;/h4&gt;&lt;ul&gt;
      &lt;li&gt;Mit der &apos;+&apos; Taste wird ein neues Benutzerkommando erzeugt.
          &lt;ul&gt;
                &lt;li&gt;git -C %1 &amp;lt;Kommando&amp;gt; %2
                &lt;ul&gt;
                   &lt;li&gt;%1 Platzhalter für das git repository&lt;/li&gt;
                   &lt;li&gt;%2 optionaler Platzhalter selektierte Datei bzw. Ordner&lt;/li&gt;
               &lt;/ul&gt;
               &lt;/li&gt;
                &lt;li&gt;git &amp;lt;Kommando&amp;gt; %1  %2
                &lt;ul&gt;
                   &lt;li&gt;%1 Platzhalter für optionale Fallbedingte Kommandooption
                  &lt;ul&gt;
                     &lt;li&gt;--staged für Vergleich von gestageten Dateien&lt;/li&gt;
                     &lt;li&gt;--tool=&amp;lt;Vergleichsprogramm&amp;gt;&lt;/li&gt;
                   &lt;/ul&gt;
                  &lt;/li&gt;
                   &lt;li&gt;%2 optionaler Platzhalter für selektierte Datei bzw. Ordner&lt;/li&gt;
               &lt;/ul&gt;
               &lt;/li&gt;
            &lt;/ul&gt;&lt;/li&gt;
      &lt;li&gt;Mit der &apos;-&apos; Taste wird das selektierte Benutzerkommando entfernt&lt;/li&gt;
      &lt;li&gt;Mit der &apos;--&amp;gt;&apos; Taste wird das selektierte Kommando einer Werkzeugleiste bzw. einem Menu zugewiesen&lt;/li&gt;
      &lt;li&gt;Mit einem Doppelklick auf den Tabelleneintrag können sowohl Benutzer- als auch eingebaute Kommandos geändert werden.
          &lt;ul&gt;
              &lt;li&gt;alle &apos;git&apos; Kommandos dürfen bzgl. der Optionen bzw. Parameter geändert werden.&lt;/li&gt;
              &lt;li&gt;alle Namen dürfen geändert werden (der Name sollte die Funktion beschreiben)&lt;/li&gt;
              &lt;li&gt;alle Tastenkürzel dürfen geändert werden&lt;/li&gt;
              &lt;li&gt;Ein Dialogtext darf geändert bzw. hinzugefügt werden.
                &lt;ul&gt;
                   &lt;li&gt;&apos;None&apos; oder leer bedeutet das kein Nachfragedialog aufgerufen wird&lt;/li&gt;
                   &lt;li&gt;Der Dialog wird vor der Ausführung des Kommandos aufgerufen.&lt;/li&gt;
                   &lt;li&gt;&lt;b&gt;Format: &lt;/b&gt;Überschrift [%1];Rückfrage [%1]&lt;/li&gt;
                   &lt;li&gt;Der optionale Platzhalter %1 zeigt die selektierte Datei bzw. Ordner an&lt;/li&gt;
               &lt;/ul&gt;
              &lt;/li&gt;
       &lt;/ul&gt;
     &lt;/li&gt;
     &lt;li&gt;&lt;b&gt;Hinweis:&lt;/b&gt;Die gewollte Funktionalität kann durch die Änderung verloren gehen&lt;br&gt;
             Jdoch kann sie wieder hergestellt werden durch &apos;reset modifications&apos; im Kontextmenu&lt;/li&gt;
     &lt;/ul&gt;
  &lt;/body&gt;
&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="180"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Remove selected custom git command&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="222"/>
        <source>Select icons
Contextmenu entries or Toolbar entries</source>
        <translation>Wähle Symbole
Kontextmenüeinräge oder Werkzeugleisteneinträge</translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="227"/>
        <location filename="customgitactions.cpp" line="170"/>
        <source>Icons</source>
        <translation>Symbole</translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="232"/>
        <source>Menu Source Tree</source>
        <translation>Menu für Quellcode Baumansicht</translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="237"/>
        <source>Menu Empty Source Tree</source>
        <translation>Menu für leere Quellcode Baumansicht</translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="242"/>
        <source>Menu History Tree</source>
        <translation>Menu für Historien Baumansicht</translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="247"/>
        <source>Menu Branch Tree</source>
        <translation>Menu für Codezweig Baumansicht</translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="252"/>
        <location filename="customgitactions.cpp" line="175"/>
        <source>Toolbar 1</source>
        <translation>Werkzeugleiste 1</translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="257"/>
        <location filename="customgitactions.cpp" line="176"/>
        <source>Toolbar 2</source>
        <translation>Werkzeugleiste 2</translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="262"/>
        <source>Diff or Merge Tool</source>
        <translation>Differenz bzw. Zusammenführungswerkzeug</translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="288"/>
        <source>&lt;html&gt;
  &lt;head/&gt;
  &lt;body&gt;&lt;h4&gt;Displays application elements and controls.&lt;/h4&gt;
    &lt;ul&gt;
      &lt;li&gt;Select items in combo box above
          &lt;ul&gt;
                &lt;li&gt;Icons&lt;/li&gt;
                &lt;li&gt;Context menues&lt;/li&gt;
                &lt;li&gt;Toolbars&lt;/li&gt;
                &lt;li&gt;Merge tools&lt;/li&gt;
          &lt;ul&gt;
      &lt;/li&gt;
      &lt;li&gt;Application Icons
          &lt;ul&gt;
                &lt;li&gt;Assign a selected icon to a command selected  in left view with &apos;&amp;lt;--&apos; button&lt;/li&gt;
          &lt;ul&gt;
      &lt;/li&gt;
      &lt;li&gt;Context menus and toolbars
          &lt;ul&gt;
                &lt;li&gt;Remove a selected action with &apos;&amp;lt;--&apos; button&lt;/li&gt;
                &lt;li&gt;Move a selected action with up and down buttons&lt;/li&gt;
          &lt;ul&gt;
      &lt;/li&gt;
      &lt;li&gt;Merge tools
          &lt;ul&gt;
                &lt;li&gt;Enable or disable tool by left click on icon&lt;/li&gt;
          &lt;ul&gt;
      &lt;/li&gt;
    &lt;/ul&gt;
  &lt;/body&gt;
&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Shows icons and menu entries</source>
        <translation>Zeigt Symbole und Menüeinträge</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="33"/>
        <source>ID</source>
        <translation>ID</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="33"/>
        <location filename="customgitactions.cpp" line="275"/>
        <source>Icon</source>
        <translation>Symbol</translation>
    </message>
    <message>
        <source>Command</source>
        <translation>Kommando</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="33"/>
        <location filename="customgitactions.cpp" line="230"/>
        <source>Name</source>
        <translation>Name</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="33"/>
        <source>Shortcut</source>
        <translation>Tastenkürzel</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="33"/>
        <source>Message box text</source>
        <translation>Meldungsdialogtext</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="33"/>
        <source>Command or status text</source>
        <translation>Kommando oder Statustext</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="62"/>
        <location filename="customgitactions.cpp" line="292"/>
        <source>-- Separator --</source>
        <translation>--Trenner--</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="135"/>
        <source>Apply selected icon in right view to selected command entry in left view</source>
        <translation>Ausgewähles Symbol dem gewählten git Kommando zuweisen</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="143"/>
        <source>Remove selected item from %1</source>
        <translation>Ausgewählten Eintrag von %1 entfernen</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="171"/>
        <source>Context Menu Source</source>
        <translation>Kontextmenü für Quellcodebaumansincht</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="172"/>
        <source>Context Menu Empty Source</source>
        <translation>Kontextmenü für leere Quellcodebaumansincht</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="173"/>
        <source>Context Menu History</source>
        <translation>Kontextmenü für Hitorienbaumansincht</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="174"/>
        <source>Context Menu Branch</source>
        <translation>Kontextmenü Quellcodezweigbaumansincht</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="177"/>
        <source>Merge or Diff Tool</source>
        <translation>Differenz bzw. Zusammenführungswerkzeug</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="258"/>
        <source>Active</source>
        <translation>Aktiv</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="259"/>
        <source>Tools</source>
        <translation>Werkzeuge</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="363"/>
        <source>command name</source>
        <translation>Kommando Name</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="467"/>
        <source>Do nothing after command execution</source>
        <translation>Führt keine Aktion aus nach dem Kommando </translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="472"/>
        <source>Update item status after command execution</source>
        <translation>Aktualisiere Zustand der gewähltenDate bzw. der gewählten Ordners </translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="477"/>
        <source>Parse command result for history view</source>
        <translation>Parse das Ergebnis für die Historienansicht</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="482"/>
        <source>Parse command result for branch view</source>
        <translation>Parse Ergebnis für die Zweigansicht</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="516"/>
        <source>Enable for git status set</source>
        <translation>Für git Status aktivieren</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="517"/>
        <source>Disable for git status set</source>
        <translation>Für git Status deaktivieren</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="518"/>
        <source>Enable for git status not set</source>
        <translation>Für inaktiven git Status aktivieren</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="528"/>
        <source>Enable command if this git status is set</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="534"/>
        <source>Disable command if this git status is set</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="540"/>
        <source>Enable command if this git status is not set</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="548"/>
        <source>Reset modifications</source>
        <translation>Änderungen verwerfen</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="549"/>
        <source>After program restart all modifications of this command are removed</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="553"/>
        <source>Branch command</source>
        <translation>Quellcodezweig Kommando</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="557"/>
        <source>The git command is called in branch view context</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="563"/>
        <source>The git command is called in history view context</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="568"/>
        <source>The git command is called without blocking the program</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="582"/>
        <source>Don&apos;t change command</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Called in branch view context</source>
        <translation>Wird in Quellcodezweigansicht angewendet</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="559"/>
        <source>History command</source>
        <translation>Historienkommando</translation>
    </message>
    <message>
        <source>Called in history view context</source>
        <translation>Wird in Historienansicht angewendet</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="565"/>
        <source>Invoke command unattached</source>
        <translation>Rufe Kommando indirekt auf</translation>
    </message>
    <message>
        <source>Called without waiting for command</source>
        <translation>Aufrufen ohne zu blockieren</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="582"/>
        <source>Cancel</source>
        <translation>Abbrechen</translation>
    </message>
</context>
<context>
    <name>Highlighter</name>
    <message>
        <location filename="highlighter.cpp" line="203"/>
        <source>%1,%2,%3</source>
        <translation>Meyers</translation>
    </message>
    <message>
        <location filename="highlighter.cpp" line="427"/>
        <source>\b(?:%1)\b</source>
        <translation>\b(?:%1)\b</translation>
    </message>
</context>
<context>
    <name>HighlighterDialog</name>
    <message>
        <location filename="highlighterdialog.ui" line="14"/>
        <source>Colors and Font Property for Text Highlighting</source>
        <translation>Eigenschaften für Schrift und Farbe für Hervorheben von Quellcodetext</translation>
    </message>
    <message>
        <location filename="highlighterdialog.ui" line="59"/>
        <source>OK</source>
        <translation>OK</translation>
    </message>
    <message>
        <location filename="highlighterdialog.cpp" line="24"/>
        <source>QPushButton {background-color: %1; color: %2;}</source>
        <translation>QPushButton {background-color: %1; color: %2;}</translation>
    </message>
    <message>
        <location filename="highlighterdialog.cpp" line="30"/>
        <source>italic</source>
        <translation>kursiv</translation>
    </message>
    <message>
        <location filename="highlighterdialog.cpp" line="82"/>
        <source>QPushButton {background-color: #FFFFFF; color: %1;}</source>
        <translation>QPushButton {background-color: #FFFFFF; color: %1;}</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="mainwindow.ui" line="14"/>
        <source>Git View</source>
        <translation>Git View</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="104"/>
        <source>Show files depending on git status</source>
        <translation>Zeigt Dateien abhängig vom git Status an</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="108"/>
        <source>Show all files</source>
        <translation>Alle anzeigen</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="113"/>
        <source>Show all git actions</source>
        <translation>Alle git Stati anzeigen</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="118"/>
        <source>Show modified</source>
        <translation>Nur modifizierte</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="123"/>
        <source>Show added</source>
        <translation>Nur hinzugefügte</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="128"/>
        <source>Show deleted</source>
        <translation>Nur entfernte</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="133"/>
        <source>Show unknown</source>
        <translation>Nur unbekannte</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="138"/>
        <source>Show staged</source>
        <translation>Nur staged</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="143"/>
        <source>Show unmerged</source>
        <translation>Nur non merged</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="148"/>
        <source>Show selected</source>
        <translation>Nur markierte</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="169"/>
        <source>Hides parent tree item, if it does not contain entries</source>
        <translation>Versteckt leere Knoten der Baumansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="172"/>
        <source>Hide empty Parent  </source>
        <translation>Leerknoten verstecken</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="185"/>
        <source>Show short git status</source>
        <translation>Zeigt Kürzel des git Status an</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="188"/>
        <source>Short State  </source>
        <translation>Kürzel</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="195"/>
        <source>Diff tool:  </source>
        <translation>Werkzeug</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="208"/>
        <source>Select diff or merge tool</source>
        <translation>Wähle Differenz bzw. Zusammenführungswerkzeug aus</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="211"/>
        <location filename="mainwindow.ui" line="215"/>
        <source>Default</source>
        <translation>Standard</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="225"/>
        <source>Show file and folder tree of git folder</source>
        <translation>Zeigt Datei bzw. Ordner Baumansicht für git Repository</translation>
    </message>
    <message>
        <source>Select Diff or Merge Tool</source>
        <translation>Wähle Differenz bzw. Zusammenführungswerkzeug aus</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="228"/>
        <source>Git repository view</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="238"/>
        <source>Name</source>
        <translation>Name</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="243"/>
        <source>Date</source>
        <translation>Datum</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="248"/>
        <source>State</source>
        <translation>Status</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="253"/>
        <source>Size</source>
        <translation>Größe</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="288"/>
        <source>File/Text:  </source>
        <translation>Datei/Text:</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="301"/>
        <source>Shows filename of opened file</source>
        <translation>Zeigt Dateiname der geöffneten Datei</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="326"/>
        <source>Close file or text view</source>
        <translation>Schliesst Datei oder Textansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="329"/>
        <source>Close</source>
        <translation>Schließen</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="351"/>
        <source>Save changed text to file</source>
        <translation>Änderungen der Textdatei speichern</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="354"/>
        <source>Save</source>
        <translation>Speichern</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="367"/>
        <source>Shows language for hilighting</source>
        <translation>Zeigt die Programmiersprache für Texthervorhebung an</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="370"/>
        <source> L:</source>
        <translation> Typ:</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="419"/>
        <source>Shows git history of files or folders</source>
        <translation>Zeigt Historie für Dateien bzw. Ordner an.</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="422"/>
        <source>Git history view</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="432"/>
        <source>History</source>
        <translation>Historie</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="437"/>
        <source>Author</source>
        <translation>Author</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="445"/>
        <source>Shows git branches</source>
        <translation>Zeigt Codezweige an</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="448"/>
        <source>Git branch view</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="461"/>
        <source>Branch</source>
        <translation>Codezweig</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="471"/>
        <source>Shows git command results or
File content opened with double click</source>
        <translation>Zeigt git Kommandoergebnis oder
Inhalt einer mit doppelklick geöffneten Datei an</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="475"/>
        <source>Text view and editor</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="502"/>
        <source>File Flags:</source>
        <translation>Eigenschaften von Dateien:</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="509"/>
        <source>Show system files in git folder</source>
        <translation>Zeige Systemdateien an</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="512"/>
        <source>System Files</source>
        <translation>Systemdateien</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="531"/>
        <source>Show hidden files in git folder</source>
        <translation>Zeige versteckte Dateien an</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="534"/>
        <source>Hidden Files</source>
        <translation>Versteckte Dateien</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="544"/>
        <source>Show symbolic links in git folder</source>
        <translation>Zeige Dateiveknüpfungen an</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="547"/>
        <source>Symbolic Links</source>
        <translation>Verknüpfungen</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="554"/>
        <source>Show folders in git folder</source>
        <translation>Zeige Ordner in git Ordner an</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="557"/>
        <source>Directories</source>
        <translation>Ordner</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="567"/>
        <source>Show files in git folder</source>
        <translation>Zeige Dateien ind git Ordner an</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="570"/>
        <source>Files</source>
        <translation>Dateien</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="584"/>
        <source>Find in </source>
        <translation>Suche in </translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="603"/>
        <source>Select View for search function</source>
        <translation>Ansicht für Suchfunktion auswählen</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="610"/>
        <source>Search backward</source>
        <translation>Rückwärts suchen</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="613"/>
        <source>Previous</source>
        <translation>Rückwärts</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="620"/>
        <source>Search forward</source>
        <translation>Vorwärts suchen</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="623"/>
        <source>Next</source>
        <translation>Vorwärts</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="630"/>
        <source>Enter search expression or words</source>
        <translation>Suchwort bzw. regulären Ausdruck eingeben</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="644"/>
        <source>Search case sensitive</source>
        <translation>Beachte Groß- und Kleinschreibung</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="647"/>
        <source>Case Sensitive </source>
        <translation>Groß/kleinbuchstaben</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="654"/>
        <source>Search whole words</source>
        <translation>Suche ganzes Wort</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="657"/>
        <source>Whole Word </source>
        <translation>Ganzes Wort</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="664"/>
        <source>Search with regular expression</source>
        <translation>Suche mit regulären Ausdrücken</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="667"/>
        <source>Regular Expression </source>
        <translation>Reguläre Ausdrücke</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="678"/>
        <source>Select source folder...</source>
        <translation>Wähle git Repository aus...</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="683"/>
        <source>Select Destination...</source>
        <translation>Wähle Ziel...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="377"/>
        <source>Text Editor</source>
        <translation>Texteditor</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="389"/>
        <source>History View</source>
        <translation>Historienansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="400"/>
        <source>Branch View</source>
        <translation>Zweigansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="412"/>
        <source>Git File View States</source>
        <translation>Git Textansichtwerkzeuge</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="421"/>
        <source>Text Editor / Viewer Control</source>
        <translation>Test-editor / ansicht Werkzeuge</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="430"/>
        <source>File Flags</source>
        <translation>Dateieigenschaften</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="439"/>
        <source>Find</source>
        <translation>Suche</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="827"/>
        <source>Repository: </source>
        <translation>Repository: </translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1095"/>
        <source>Select SourceFiles</source>
        <translation>Wähle git Repository</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1118"/>
        <source>Total selected bytes: </source>
        <translation>Gesamt ausgewählte Bytes:</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1129"/>
        <source>Save content of text editor</source>
        <translation>Speichere Inhalt der Textansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1150"/>
        <source>The document has been modified.</source>
        <translation>Die Textdatei wurde geändert.</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1151"/>
        <source>Do you want to save your changes?</source>
        <translation>Möchten Sie die Änderungen speichern?</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1194"/>
        <source> Type: </source>
        <translation> Typ:</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1413"/>
        <source>Show difference</source>
        <translation>Zeige Unterschiede an</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1419"/>
        <source>Call diff tool...</source>
        <translation>Zeige Unterschiede extern an...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1425"/>
        <source>Call merge tool...</source>
        <translation>Rufe Zusammenführungswerkzeug auf...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1430"/>
        <source>Merge file...</source>
        <translation>Führe Datei zusammen...</translation>
    </message>
    <message>
        <source>Merge selected file</source>
        <translation>Führe Änderungen der ausgewählten Datei zusammen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1430"/>
        <source>Merge selected file (experimental, not working)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1432"/>
        <source>Edit Highlighting...</source>
        <translation>Bearbeite Texthervorhebung...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1432"/>
        <source>Edit highlighting color and font</source>
        <translation>Bearbeitet Farbe und Schrift der Hervorhebung</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1435"/>
        <source>Show status</source>
        <translation>Aktualisiere git Status</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1437"/>
        <source>Show short status</source>
        <translation>Aktualisiere Kurzstatus</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1440"/>
        <source>Add to git (stage)</source>
        <translation>Zu git hinzufügen (stagen)</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1447"/>
        <source>Reset file (unstage)</source>
        <translation>Aus git entfernen (unstage)</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1452"/>
        <source>Show History</source>
        <translation>Historie anzeigen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1456"/>
        <source>Remove from git...</source>
        <translation>Aus git enfernen...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1460"/>
        <source>Restore changes...</source>
        <translation>Änderungen rückgangig machen...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1461"/>
        <source>Restore changes;Do you want to restore changes in file &quot;%1&quot;?</source>
        <translation>Änderungen rückgängig;Möchten Sie die Änderungen in der Datei &quot;%1&quot; verwerfen?</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1467"/>
        <source>Commit...</source>
        <translation>Committen...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1471"/>
        <source>Push</source>
        <translation>Hochladen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1473"/>
        <source>Pull</source>
        <translation>Herunerladen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1475"/>
        <source>Show</source>
        <translation>Anzeigen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1477"/>
        <source>Stash</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1479"/>
        <source>Stash pop</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1480"/>
        <source>Stash show</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1482"/>
        <source>List Branches</source>
        <translation>Quellcodezweige auflisten</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1485"/>
        <source>List remote Branches</source>
        <translation>Entfernte Quellcodezweige auflisten</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1488"/>
        <source>List merged Branches</source>
        <translation>Zusammengeführte Quellcodezweige auflisten</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1491"/>
        <source>List not merged Branches</source>
        <translation>Nicht zusammengeführte Quellcodezweige auflisten</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1495"/>
        <source>Delete Branch</source>
        <translation>Zweig löschen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1496"/>
        <source>Delete %1 from git;Do you want to delete &quot;%1&quot;?</source>
        <translation>Lösche %1 aus git;Möchten sie &quot;%1&quot; aus git entfernen?</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1500"/>
        <source>Checkout Branch</source>
        <translation>Zweig auschecken</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1501"/>
        <source>Checkout %1;Do you want to set &quot;%1&quot; active?</source>
        <translation>Auschecken von %1;Möchten Sie den Zweig &quot;%1&quot; aktivieren?</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1505"/>
        <source>History Branch</source>
        <translation>Historie des Zweiges</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1508"/>
        <source>Show Branch</source>
        <translation>Zweig anzeigen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1511"/>
        <source>Move / Rename...</source>
        <translation>Verschieben / Umbenennen...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1515"/>
        <source>Expand Tree Items</source>
        <translation>Baumansicht aufklappen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1515"/>
        <source>Expands all tree item of focused tree</source>
        <translation>Klappt alle Zweige der Baumansicht auf</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1518"/>
        <source>Collapse Tree Items</source>
        <translation>Zweige zuklappen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1518"/>
        <source>Collapses all tree item of focused tree</source>
        <translation>Klappt alle Zweige der Baumansicht zusammen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="2140"/>
        <source>Do you really whant to kill the processes &quot;%1&quot;?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Add git source folder</source>
        <translation>Füge git Repository hinzu</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1522"/>
        <source>Adds a git source folder to the source treeview</source>
        <translation>Fügt ein git Repository hinzu</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1522"/>
        <source>Add git source folder...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1524"/>
        <source>Update git status</source>
        <translation>git Status aktualisieren</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1524"/>
        <source>Updates the git status of the selected source folder</source>
        <translation>Aktualisiert den git Status des ausgewählten Eintrags</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1529"/>
        <source>Show/Hide tree</source>
        <translation>Show/Hide tree</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1529"/>
        <source>Shows or hides history or branches tree</source>
        <translation>Shows or hides history or branches tree</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1533"/>
        <source>Clear tree items</source>
        <translation>Löscht alle Einträge</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1533"/>
        <source>Clears all history or branch tree entries</source>
        <translation>Löscht alle Einträge der Baumansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1536"/>
        <source>Customize git actions...</source>
        <translation>Benutzer Kommandos bearbeiten...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1536"/>
        <source>Edit custom git actions, menues and toolbars</source>
        <translation>Bearbeitet git Kommandos, Menüeintrage und Werkzeugleisteneinträge</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1538"/>
        <source>Insert File Name List</source>
        <translation>Für Dateinameliste ein</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1538"/>
        <source>Inserts file names that differ from previous hash</source>
        <translation>Fügt Dateinamen ein, die von dem vorzustand abweichen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1540"/>
        <source>About...</source>
        <translation>Über...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1540"/>
        <source>Information about GitView</source>
        <translation>Über GitView</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1543"/>
        <source>Delete...</source>
        <translation>Löschen...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1543"/>
        <source>Delete file or folder</source>
        <translation>Lösche Datei bzw. Ordner</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1545"/>
        <source>Select Language...</source>
        <translation>Wähle Sprache...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1545"/>
        <source>Select language for text highlighting</source>
        <translation>Wähle Sprache für Texthervorhebung</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1548"/>
        <source>Kill Background Activity...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1548"/>
        <source>Kill git action running in background</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1585"/>
        <source>git -C %1 difftool --tool-help</source>
        <translation>git -C %1 difftool --tool-help</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1613"/>
        <source>Enable all</source>
        <translation>Alle anzeigen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1614"/>
        <source>Disable all</source>
        <translation>Keinen anzeigen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1625"/>
        <source>Authors</source>
        <translation>Autoren</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1690"/>
        <source>Move or rename %1</source>
        <translation>Verschieben bzw. umbenennen %1 </translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1691"/>
        <source>Enter a new name or destination for &quot;./%1&quot;.
To move the %3 insert the destination path before.
To rename just modify the name.</source>
        <translation>Neuer Name bzw. Zielort &quot;./%1&quot;.
Zum Verschieben von %3 muss der Zielort vor dem Namen eingegeben werden.
Zum Umbenennen muss nur der Name geändert werden.</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1882"/>
        <source>git -C %1 </source>
        <translation>git -C %1 </translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="2069"/>
        <source>Delete %1</source>
        <translation>Lösche %1</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="2108"/>
        <source>%1</source>
        <translation>%1</translation>
    </message>
</context>
<context>
    <name>MergeDialog</name>
    <message>
        <location filename="mergedialog.ui" line="14"/>
        <location filename="mergedialog.ui" line="26"/>
        <location filename="mergedialog.ui" line="97"/>
        <source>Merge</source>
        <translation>Zusammenführen</translation>
    </message>
    <message>
        <location filename="mergedialog.ui" line="38"/>
        <source>Abort</source>
        <translation>Abbrechen</translation>
    </message>
    <message>
        <location filename="mergedialog.ui" line="51"/>
        <source>Continue</source>
        <translation>Weitermachen</translation>
    </message>
    <message>
        <location filename="mergedialog.ui" line="64"/>
        <source>Preview</source>
        <translation>Vorschau</translation>
    </message>
    <message>
        <location filename="mergedialog.ui" line="78"/>
        <source>Automatic</source>
        <translation>Automatisch</translation>
    </message>
    <message>
        <location filename="mergedialog.ui" line="83"/>
        <source>Default Tool</source>
        <translation>Standard Werkzeug</translation>
    </message>
    <message>
        <location filename="mergedialog.ui" line="111"/>
        <source>Merge Options</source>
        <translation>Optionen für Zusammenführung</translation>
    </message>
    <message>
        <location filename="mergedialog.ui" line="123"/>
        <source>Ours</source>
        <translation>Eigene Änderungen</translation>
    </message>
    <message>
        <location filename="mergedialog.ui" line="136"/>
        <source>Theirs</source>
        <translation>Andere Änderungen</translation>
    </message>
    <message>
        <location filename="mergedialog.ui" line="149"/>
        <source>Patience</source>
        <translation>Geduldig</translation>
    </message>
    <message>
        <location filename="mergedialog.ui" line="162"/>
        <source>Minimal</source>
        <translation>Minimal</translation>
    </message>
    <message>
        <location filename="mergedialog.ui" line="175"/>
        <source>Histogram</source>
        <translation>Histogramm</translation>
    </message>
    <message>
        <location filename="mergedialog.ui" line="188"/>
        <source>Meyers</source>
        <translation>Meyeers</translation>
    </message>
    <message>
        <location filename="mergedialog.ui" line="202"/>
        <source>File:</source>
        <translation>Datei:</translation>
    </message>
    <message>
        <location filename="mergedialog.cpp" line="99"/>
        <source>git log -- %1</source>
        <translation>git log -- %1</translation>
    </message>
    <message>
        <location filename="mergedialog.cpp" line="114"/>
        <source>git %1</source>
        <translation>git %1</translation>
    </message>
</context>
<context>
    <name>QHistoryTreeWidget</name>
    <message>
        <location filename="qhistorytreewidget.cpp" line="199"/>
        <source>Files: %1</source>
        <translation>Dateien: %1</translation>
    </message>
    <message>
        <location filename="qhistorytreewidget.cpp" line="216"/>
        <source>Result failure no: %1</source>
        <translation>Ergebnisfehlernummer: %1</translation>
    </message>
    <message>
        <location filename="qhistorytreewidget.cpp" line="261"/>
        <source>git diff --name-only %1 %2</source>
        <translation>git diff --name-only %1 %2</translation>
    </message>
    <message>
        <location filename="qhistorytreewidget.cpp" line="267"/>
        <source>git diff --name-only %1</source>
        <translation>git diff --name-only %1</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="git_type.cpp" line="60"/>
        <source>Git Commands</source>
        <translation>Git Kommandos</translation>
    </message>
    <message>
        <location filename="git_type.cpp" line="61"/>
        <source>Control Commands</source>
        <translation>Kontroll Kommandos</translation>
    </message>
    <message>
        <location filename="git_type.cpp" line="70"/>
        <source>Do Nothing</source>
        <translation>Tue nichts</translation>
    </message>
    <message>
        <location filename="git_type.cpp" line="71"/>
        <source>Update Item Status</source>
        <translation>Staus aktualisieren</translation>
    </message>
    <message>
        <location filename="git_type.cpp" line="72"/>
        <source>Parse History List Text</source>
        <translation>Parse Historienausgabe</translation>
    </message>
    <message>
        <location filename="git_type.cpp" line="73"/>
        <source>Parse Branch List Text</source>
        <translation>Parse git branch Ausgabe</translation>
    </message>
    <message>
        <location filename="gitignore.cpp" line="64"/>
        <source>&quot;%1&quot; not inserted from %2</source>
        <translation>&quot;%1&quot; not inserted from %2</translation>
    </message>
    <message>
        <location filename="gitignore.cpp" line="69"/>
        <source>&quot;%1&quot; inserted from %2</source>
        <translation>&quot;%1&quot; inserted from %2</translation>
    </message>
    <message>
        <location filename="helper.cpp" line="81"/>
        <source>Delete %1 entry;%1</source>
        <translation>Lösche Eintrag %1; %1</translation>
    </message>
    <message>
        <location filename="helper.cpp" line="81"/>
        <source>Tree</source>
        <translation>Baum</translation>
    </message>
    <message>
        <location filename="helper.cpp" line="160"/>
        <source>Error occurred executing command: </source>
        <translation>Ein Fehler ist augetreten bei der Ausführung des Kommandos:</translation>
    </message>
    <message>
        <location filename="helper.cpp" line="163"/>
        <source>Error number : </source>
        <translation>Fehlernummer:</translation>
    </message>
    <message>
        <location filename="main.cpp" line="23"/>
        <location filename="main.cpp" line="27"/>
        <source>Alternative Config file name.</source>
        <translation>Alternativer Konfigurationsdateiname.</translation>
    </message>
</context>
</TS>
