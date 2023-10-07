<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="de_DE">
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
        <source>1.1</source>
        <translation type="vanished">1.1</translation>
    </message>
    <message>
        <location filename="aboutdlg.ui" line="65"/>
        <source>0.0.0.0</source>
        <translation type="unfinished">0.0.0.0</translation>
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
        <source>Viewer for git repositories. Compiled with c++ 17</source>
        <oldsource>Viewer for git repositories. Based on Qt 5.6 with c++ 14</oldsource>
        <translation type="unfinished">Betrachter für git Repositories. Programmiert mit Qt5.6 und c++17</translation>
    </message>
    <message>
        <location filename="aboutdlg.ui" line="237"/>
        <source>Built</source>
        <translation>Erstellt</translation>
    </message>
    <message>
        <location filename="aboutdlg.ui" line="250"/>
        <source>Built on:</source>
        <oldsource>Build on:</oldsource>
        <translation type="unfinished">Erstellt am:</translation>
    </message>
    <message>
        <location filename="aboutdlg.ui" line="263"/>
        <source>Based on:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="aboutdlg.ui" line="276"/>
        <source>Qt-Version </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="aboutdlg.cpp" line="11"/>
        <source>About </source>
        <translation>Über </translation>
    </message>
    <message>
        <location filename="aboutdlg.cpp" line="17"/>
        <source>, </source>
        <translation>, </translation>
    </message>
</context>
<context>
    <name>BinaryTableModel</name>
    <message>
        <location filename="qbinarytableview.cpp" line="961"/>
        <source>[%1]: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="qbinarytableview.cpp" line="992"/>
        <source>[%1]::</source>
        <translation type="unfinished"></translation>
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
    <message>
        <location filename="commitmessage.ui" line="71"/>
        <source>commit and push</source>
        <translation type="unfinished">Commit und Push</translation>
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
        <translation type="vanished">Zeigt git Kommandos</translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="35"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;List of custom commands&lt;br/&gt;Click &amp;quot;Whats This help&amp;quot; &lt;img src=&quot;://resource/24X24/dialog-question.png&quot; style=&quot;vertical-align:middle&quot;/&gt; &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="38"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;h4&gt;Displays application commands that can be edited here.&lt;/h4&gt;
&lt;ul&gt;
&lt;li&gt;Add a new custom command with the &lt;img src=&quot;://resource/24X24/list-add.png&quot; style=&quot;vertical-align:middle&quot;/&gt; button &lt;/li&gt;
&lt;ul&gt;
&lt;li&gt;Optional parameters are marked with square brackets e.g. [%2]&lt;/li&gt;
&lt;li&gt;The parameters order is fixed for git commands&lt;/li&gt;
&lt;li&gt;git -C %1 &amp;lt;command&amp;gt; [%2] &lt;/li&gt;
&lt;ul&gt;
&lt;li&gt;%1 represents the git repository &lt;/li&gt;
&lt;li&gt;%2 may represent the the selected file or folder &lt;/li&gt;
&lt;/ul&gt;
&lt;li&gt;git &amp;lt;command&amp;gt; %1 [%2] &lt;/li&gt;
&lt;ul&gt;
&lt;li&gt;%1 is used for command options depending on case &lt;/li&gt;
&lt;ul&gt;
&lt;li&gt;--staged for diff of staged file &lt;/li&gt;
&lt;li&gt;--tool=&amp;lt;diff-tool&amp;gt; &lt;/li&gt;
&lt;/ul&gt;
&lt;li&gt;%2  may represent the the selected file or folder &lt;/li&gt;
&lt;/ul&gt;
&lt;li&gt;The parameters order is arbitrary for custom commands&lt;/li&gt;
&lt;li&gt;custom_command [%1] [%2] [#2] [%3] &lt;/li&gt;
&lt;ul&gt;
&lt;li&gt;%1 may represent the selected file or folder &lt;/li&gt;
&lt;li&gt;%2 may represent the folder above selected file&lt;/li&gt;
&lt;li&gt;#2 sets current working dir to folder above selected file&lt;/li&gt;
&lt;li&gt;#r executes command recursve for selected folder content&lt;br&gt;
    &lt;b&gt;may lead to time consuming process&lt;/b&gt;&lt;/li&gt;
&lt;li&gt;%3 may represent the root folder of the repository&lt;/li&gt;
&lt;/ul&gt;
&lt;/ul&gt;
&lt;li&gt;Remove selected custom command with &lt;img src=&quot;://resource/24X24/list-remove.png&quot; style=&quot;vertical-align:middle&quot;/&gt; button &lt;/li&gt;
&lt;li&gt;Assign a selected command to toolbar or context menu in right view with &lt;img src=&quot;://resource/24X24/go-next.png&quot; style=&quot;vertical-align:middle&quot;/&gt; button&lt;br&gt;You may insert:&lt;/li&gt;
&lt;ul&gt;
&lt;li&gt;Command&lt;/li&gt;
&lt;li&gt;Separator&lt;/li&gt;
&lt;li&gt;Sub menu entry&lt;/li&gt;
&lt;ul&gt;
&lt;li&gt;Edit name in &lt;b&gt;Name&lt;/b&gt; column&lt;/li&gt;
&lt;li&gt;Insert the sub menu into context menu and move it to desired position&lt;/li&gt;
&lt;li&gt;The folowing entries until next separator or sub menu are included here&lt;/li&gt;
&lt;/ul&gt;
&lt;/ul&gt;
&lt;li&gt;Edit commands (custom or built in) by double click on table entry &lt;/li&gt;
&lt;ul&gt;
&lt;li&gt;all &apos;git&apos; commands may be edited due to parameters or options &lt;/li&gt;
&lt;li&gt;all names may be edited (the name should represent the commands functionality) &lt;/li&gt;
&lt;li&gt;all shortcuts may be edited &lt;/li&gt;
&lt;li&gt;A message box text may be edited or added &lt;/li&gt;
&lt;/ul&gt;
&lt;ul&gt;
&lt;li&gt;&apos;None&apos; or empty defines that no message box is invoked &lt;/li&gt;
&lt;li&gt;The message box is invoked before execution of the git command &lt;/li&gt;
&lt;li&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;Format: &lt;/span&gt;Heading [%1];Question [%1] &lt;/li&gt;
&lt;li&gt;The parameter %1 is optional an displays the selected file or folder&lt;/li&gt;
&lt;li&gt;A trailing &apos;;&apos; changes the MessageBox to an EditBox &lt;/li&gt;
&lt;/ul&gt;
&lt;li&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;Note:&lt;/span&gt;The correct functionality may be lost by editing&lt;br/&gt;however this can be repaired by &apos;reset modifications&apos; in context menu &lt;/li&gt;
&lt;/ul&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="117"/>
        <source>Execute the command line for testing purpose</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="120"/>
        <source>Execute</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="127"/>
        <source>Enter command to execute</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="134"/>
        <source>Find in command table</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="137"/>
        <source>Find</source>
        <translation type="unfinished">Suche</translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="144"/>
        <source>Add command to command table</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="147"/>
        <source>Add</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="181"/>
        <source>Activates Whats This Help</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="198"/>
        <source>Open external icon folder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="226"/>
        <source>Add command to menue or toolbar</source>
        <translation>Kommando zur Menü Werkzeugleiste hinzufügen</translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="240"/>
        <location filename="customgitactions.ui" line="254"/>
        <source>Move selected menue or toolbar item</source>
        <translation>Gewählten Menüeintrag bewegen</translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="268"/>
        <source>Add custom git command entry</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="282"/>
        <source>Remove selected custom git command</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="350"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;List of application elements and controls&lt;br/&gt;Click &amp;quot;Whats This help&amp;quot; &lt;img src=&quot;://resource/24X24/dialog-question.png&quot; style=&quot;vertical-align:middle&quot;/&gt; &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="353"/>
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
                &lt;li&gt;Assign a selected icon to a command selected  in left view with &lt;img src=&quot;://resource/24X24/go-previous.png&quot; style=&quot;vertical-align:middle&quot;/&gt;  button&lt;/li&gt;
          &lt;ul&gt;
      &lt;/li&gt;
      &lt;li&gt;Context menus and toolbars
          &lt;ul&gt;
                &lt;li&gt;Remove a selected action with &lt;img src=&quot;://resource/24X24/go-previous.png&quot; style=&quot;vertical-align:middle&quot;/&gt;  button&lt;/li&gt;
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
        <source>Add curstom git command entry</source>
        <translation type="vanished">Benutzer git Kommando hinzufügen</translation>
    </message>
    <message>
        <source>Remove custom git command</source>
        <translation type="vanished">Benuzer git Kommando entfernen</translation>
    </message>
    <message>
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
        <translation type="vanished">&lt;html&gt;
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
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Remove selected custom git command&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Selektiertes Benutzerkommando löschen&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="customgitactions.ui" line="324"/>
        <source>Select icons
Contextmenu entries or Toolbar entries</source>
        <translation>Wähle Symbole
Kontextmenüeinräge oder Werkzeugleisteneinträge</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="271"/>
        <source>Icons</source>
        <translation>Symbole</translation>
    </message>
    <message>
        <source>Menu Source Tree</source>
        <translation type="vanished">Menu für Quellcode Baumansicht</translation>
    </message>
    <message>
        <source>Menu Empty Source Tree</source>
        <translation type="vanished">Menu für leere Quellcode Baumansicht</translation>
    </message>
    <message>
        <source>Menu History Tree</source>
        <translation type="vanished">Menu für Historien Baumansicht</translation>
    </message>
    <message>
        <source>Menu Branch Tree</source>
        <translation type="vanished">Menu für Codezweig Baumansicht</translation>
    </message>
    <message>
        <source>Toolbar 1</source>
        <translation type="vanished">Werkzeugleiste 1</translation>
    </message>
    <message>
        <source>Toolbar 2</source>
        <translation type="vanished">Werkzeugleiste 2</translation>
    </message>
    <message>
        <source>Diff or Merge Tool</source>
        <translation type="vanished">Differenz bzw. Zusammenführungswerkzeug</translation>
    </message>
    <message>
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
        <translation type="vanished">&lt;html&gt;
  &lt;head/&gt;
  &lt;body&gt;&lt;h4&gt;Listen für Icons, Menüs und Werkzeugleiste&lt;/h4&gt;
    &lt;ul&gt;
      &lt;li&gt;Auswahl der Liste durch die Combobox
          &lt;ul&gt;
                &lt;li&gt;Icons&lt;/li&gt;
                &lt;li&gt;Kontextmenüs&lt;/li&gt;
                &lt;li&gt;Werkzeugleisten&lt;/li&gt;
                &lt;li&gt;Zusammenführungswerkzeuge&lt;/li&gt;
          &lt;ul&gt;
      &lt;/li&gt;
      &lt;li&gt;Anwendungs Icons
          &lt;ul&gt;
                &lt;li&gt;Mit der Taste &apos;&amp;lt;--&apos; wird das gewählte Icon dieser Liste dem gewählten Kommando in der Kommandoliste (links) zugewiesen&lt;/li&gt;
          &lt;ul&gt;
      &lt;/li&gt;
      &lt;li&gt;Kontextmenüs und Werkzeugleisten
          &lt;ul&gt;
                &lt;li&gt;Mit der &apos;&amp;lt;--&apos; Taste wird das gewählte Kommando aus der Liste entfernt&lt;/li&gt;
                &lt;li&gt;Mit den auf/ab Tasten wird das gewähle Kommando in der Liste verschoben&lt;/li&gt;
          &lt;ul&gt;
      &lt;/li&gt;
      &lt;li&gt;Zusammenführungswerkzeuge
          &lt;ul&gt;
                &lt;li&gt;Mit Linksklick auf das Icon wird das Werkzeug aktiviert bzw. deaktiviert&lt;/li&gt;
          &lt;ul&gt;
      &lt;/li&gt;
    &lt;/ul&gt;
  &lt;/body&gt;
&lt;/html&gt;</translation>
    </message>
    <message>
        <source>Shows icons and menu entries</source>
        <translation type="vanished">Zeigt Symbole und Menüeinträge</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="67"/>
        <source>ID</source>
        <translation>ID</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="67"/>
        <location filename="customgitactions.cpp" line="400"/>
        <source>Icon</source>
        <translation>Symbol</translation>
    </message>
    <message>
        <source>Command</source>
        <translation type="vanished">Kommando</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="67"/>
        <location filename="customgitactions.cpp" line="335"/>
        <source>Name</source>
        <translation>Name</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="67"/>
        <source>Shortcut</source>
        <translation>Tastenkürzel</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="67"/>
        <source>Message box text</source>
        <translation>Meldungsdialogtext</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="67"/>
        <source>Command or status text</source>
        <translation>Kommando oder Statustext</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="97"/>
        <location filename="customgitactions.cpp" line="417"/>
        <source>-- Separator --</source>
        <translation>--Trenner--</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="102"/>
        <source>Insert submenu:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="103"/>
        <source>sub menu name</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="192"/>
        <source>%1</source>
        <translation type="unfinished">%1</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="208"/>
        <source>Apply selected icon in right view to selected command entry in left view</source>
        <translation>Ausgewähles Symbol dem gewählten git Kommando zuweisen</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="226"/>
        <source>Remove selected item from %1</source>
        <translation>Ausgewählten Eintrag von %1 entfernen</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="272"/>
        <source>External Icons</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="273"/>
        <source>Context Menu Source</source>
        <translation>Kontextmenü für Quellcodebaumansincht</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="274"/>
        <source>Context Menu Empty Source</source>
        <translation>Kontextmenü für leere Quellcodebaumansincht</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="275"/>
        <source>Context Menu Graphics View</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="276"/>
        <source>Context Menu Text View</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="277"/>
        <source>Context Menu History</source>
        <translation>Kontextmenü für Hitorienbaumansincht</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="278"/>
        <source>Context Menu Branch</source>
        <translation>Kontextmenü Quellcodezweigbaumansincht</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="279"/>
        <source>Context Menu Stash</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="280"/>
        <source>Context Menu Find Text</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="281"/>
        <source>Merge or Diff Tool</source>
        <translation>Differenz bzw. Zusammenführungswerkzeug</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="368"/>
        <source>&lt; empty &gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="383"/>
        <source>Active</source>
        <translation>Aktiv</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="384"/>
        <source>Tools</source>
        <translation>Werkzeuge</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="511"/>
        <source>command name</source>
        <translation>Kommando Name</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="667"/>
        <source>Do nothing after command execution</source>
        <translation>Führt keine Aktion aus nach dem Kommando</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="668"/>
        <source>Update item status after command execution</source>
        <translation>Aktualisiere Zustand der gewählten Datei bzw. des gewählten Ordners</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="669"/>
        <source>Parse command result for history view</source>
        <translation>Parse das Ergebnis für die Historienansicht</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="670"/>
        <source>Parse command result for branch view</source>
        <translation>Parse Ergebnis für die Zweigansicht</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="671"/>
        <source>Parse command result for stash view</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="672"/>
        <source>Parse command result for git blame</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="673"/>
        <source>Update root item status after command execution</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="674"/>
        <source>Update repository after command execution</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="715"/>
        <source>Enable for git status set</source>
        <translation>Für git Status aktivieren</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="716"/>
        <source>Disable for git status set</source>
        <translation>Für git Status deaktivieren</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="717"/>
        <source>Enable for git status not set</source>
        <translation>Für inaktiven git Status aktivieren</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="736"/>
        <source>Ignore git file type status</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="741"/>
        <source>Enable command if this git status is set</source>
        <translation>Aktiviere das Kommando für diesen git Status</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="747"/>
        <source>Disable command if this git status is set</source>
        <translation>Deaktiviere das Kommando für diesen git Status</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="753"/>
        <source>Enable command if this git status is not set</source>
        <translation>Aktiviere das Kommando, wenn dieser git Status nicht gesetzt ist</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="761"/>
        <source>Reset modifications</source>
        <translation>Änderungen verwerfen</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="762"/>
        <source>After program restart all modifications of this command are removed</source>
        <translation>Nach Neustart des Programms sind die Modifikationen dieses Kommandos entfernt</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="767"/>
        <source>Branch command</source>
        <translation>Quellcodezweig Kommando</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="767"/>
        <source>The git command is called in branch view context</source>
        <translation>Dieses git Kommando wird im Kontext der Zweigansicht aufgerufen</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="768"/>
        <source>The git command is called in history view context</source>
        <translation>Dieses git Kommando wird im Kontext der Historienansicht aufgerufen</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="769"/>
        <source>Stash command</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="769"/>
        <source>The git command is called in stash view context</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="770"/>
        <source>The git command is called without blocking the program</source>
        <translation>Dieses git Kommando aufgerufen, ohne das Program zu blockieren</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="771"/>
        <source>Unattached instant output</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="771"/>
        <source>The git command is called without blocking the program with instant output</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="773"/>
        <source>Dont hide command window</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="773"/>
        <source>The command window is not hidden to allow password input</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="814"/>
        <source>Edit toolbar button menu</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="815"/>
        <source>Edit or create toolbar button menu list for custom command</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="818"/>
        <source>Don&apos;t change command</source>
        <translation>Kommando nicht ändern</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="867"/>
        <source>Edit menu for toolbar button</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="868"/>
        <source>Enter comma separated list for command option menu&lt;br&gt;(--option, o1,o2[,o3,..])&lt;br&gt;Mark default option with * like this: *o1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="948"/>
        <source>Select Icon Files</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="965"/>
        <source>Create Toolbar...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="965"/>
        <source>Create a custom toolbar</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="970"/>
        <source>Delete %1</source>
        <translation type="unfinished">Lösche %1</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="971"/>
        <source>Delete selected custom toolbar</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="974"/>
        <source>No Action</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="981"/>
        <source>Enter name for a custom toolbar</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="981"/>
        <source>Toolbar Custom</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Called in branch view context</source>
        <translation type="vanished">Wird in Quellcodezweigansicht angewendet</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="768"/>
        <source>History command</source>
        <translation>Historienkommando</translation>
    </message>
    <message>
        <source>Called in history view context</source>
        <translation type="vanished">Wird in Historienansicht angewendet</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="770"/>
        <source>Invoke command unattached</source>
        <translation>Rufe Kommando indirekt auf</translation>
    </message>
    <message>
        <source>Called without waiting for command</source>
        <translation type="vanished">Aufrufen ohne zu blockieren</translation>
    </message>
    <message>
        <location filename="customgitactions.cpp" line="818"/>
        <location filename="customgitactions.cpp" line="974"/>
        <source>Cancel</source>
        <translation>Abbrechen</translation>
    </message>
</context>
<context>
    <name>Highlighter</name>
    <message>
        <location filename="highlighter.cpp" line="203"/>
        <source>%1,%2,%3</source>
        <translatorcomment>Meyers</translatorcomment>
        <translation>%1,%2,%3</translation>
    </message>
    <message>
        <source>\b(?:%1)\b</source>
        <translation type="vanished">\b(?:%1)\b</translation>
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
        <location filename="highlighterdialog.ui" line="32"/>
        <source>Color</source>
        <translation type="unfinished">Farbe</translation>
    </message>
    <message>
        <location filename="highlighterdialog.ui" line="52"/>
        <source>Style</source>
        <translation type="unfinished">Schrift</translation>
    </message>
    <message>
        <location filename="highlighterdialog.ui" line="72"/>
        <source>Weight</source>
        <translation type="unfinished">Schriftstärke</translation>
    </message>
    <message>
        <location filename="highlighterdialog.ui" line="89"/>
        <source>OK</source>
        <translation>OK</translation>
    </message>
    <message>
        <location filename="highlighterdialog.cpp" line="25"/>
        <location filename="highlighterdialog.cpp" line="91"/>
        <source>QPushButton {background-color: %1; color: %2;}</source>
        <translation>QPushButton {background-color: %1; color: %2;}</translation>
    </message>
    <message>
        <location filename="highlighterdialog.cpp" line="29"/>
        <source>italic</source>
        <translation>kursiv</translation>
    </message>
    <message>
        <source>QPushButton {background-color: #FFFFFF; color: %1;}</source>
        <translation type="vanished">QPushButton {background-color: #FFFFFF; color: %1;}</translation>
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
        <location filename="mainwindow.ui" line="107"/>
        <source>Show files depending on git status</source>
        <translation>Zeigt Dateien abhängig vom git Status an</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="111"/>
        <source>Show all files</source>
        <translation>Alle anzeigen</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="116"/>
        <source>Show all git actions</source>
        <translation>Alle git Stati anzeigen</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="121"/>
        <source>Show modified</source>
        <translation>Nur modifizierte</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="126"/>
        <source>Show added</source>
        <translation>Nur hinzugefügte</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="131"/>
        <source>Show deleted</source>
        <translation>Nur entfernte</translation>
    </message>
    <message>
        <source>Show unknown</source>
        <translation type="vanished">Nur unbekannte</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="141"/>
        <source>Show staged</source>
        <translation>Nur staged</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="146"/>
        <source>Show unmerged</source>
        <translation>Nur non merged</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="151"/>
        <source>Show selected</source>
        <translation>Nur markierte</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="177"/>
        <source>Hides parent tree item, if it does not contain entries</source>
        <translation>Versteckt leere Knoten der Baumansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="180"/>
        <source>Hide empty Parent  </source>
        <translation>Leerknoten verstecken  </translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="196"/>
        <source>Show short git status</source>
        <translation>Zeigt Kürzel des git Status an</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="199"/>
        <source>Short State  </source>
        <translation>Kürzel  </translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="206"/>
        <source>Diff tool:  </source>
        <translation>Werkzeug:  </translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="219"/>
        <source>Select diff or merge tool</source>
        <translation>Wähle Differenz bzw. Zusammenführungswerkzeug aus</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="222"/>
        <location filename="mainwindow.ui" line="226"/>
        <source>Default</source>
        <translation>Standard</translation>
    </message>
    <message>
        <source>Show file and folder tree of git repositories</source>
        <oldsource>Show file and folder tree of git folder</oldsource>
        <translation type="vanished">Zeigt Datei bzw. Ordner Baumansicht für git Repository</translation>
    </message>
    <message>
        <source>Select Diff or Merge Tool</source>
        <translation type="vanished">Wähle Differenz bzw. Zusammenführungswerkzeug aus</translation>
    </message>
    <message>
        <source>Git repository view</source>
        <translation type="vanished">Ansicht des git Repository</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="273"/>
        <source>Name</source>
        <translation>Name</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="278"/>
        <source>Date</source>
        <translation>Datum</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="283"/>
        <source>State</source>
        <translation>Status</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="288"/>
        <source>Size</source>
        <translation>Größe</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="323"/>
        <source>File/Text:  </source>
        <translation>Datei/Text:  </translation>
    </message>
    <message>
        <source>Shows filename of opened file</source>
        <translation type="vanished">Zeigt Dateiname der geöffneten Datei</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="349"/>
        <source>Close file or text view</source>
        <translation>Schliesst Datei oder Textansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1833"/>
        <source>Close</source>
        <translation>Schließen</translation>
    </message>
    <message>
        <source>Save changed text to file</source>
        <translation type="vanished">Änderungen der Textdatei speichern</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1830"/>
        <source>Save</source>
        <translation>Speichern</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="419"/>
        <source>Shows language for hilighting</source>
        <translation>Zeigt die Programmiersprache für Texthervorhebung an</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="422"/>
        <source> L:</source>
        <translation> Typ:</translation>
    </message>
    <message>
        <source>Git history view</source>
        <translation type="vanished">Ansicht der git Historie</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="500"/>
        <source>History</source>
        <translation>Historie</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="510"/>
        <source>Author</source>
        <translation>Author</translation>
    </message>
    <message>
        <source>Shows git branches</source>
        <translation type="vanished">Zeigt Codezweige an</translation>
    </message>
    <message>
        <source>Git branch view</source>
        <translation type="vanished">Asnicht für git Zweige</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="585"/>
        <source>Branch</source>
        <translation>Codezweig</translation>
    </message>
    <message>
        <source>Shows git command results or
File content opened with double click</source>
        <translation type="vanished">Zeigt git Kommandoergebnis oder
Inhalt einer mit doppelklick geöffneten Datei an</translation>
    </message>
    <message>
        <source>Text view and editor</source>
        <translation type="vanished">Textansicht bzw. Editor</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="750"/>
        <source>File Flags:</source>
        <translation>Eigenschaften von Dateien:</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="770"/>
        <source>Show system files in git folder</source>
        <translation>Zeige Systemdateien an</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="773"/>
        <source>System Files</source>
        <translation>Systemdateien</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="792"/>
        <source>Show hidden files in git folder</source>
        <translation>Zeige versteckte Dateien an</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="795"/>
        <source>Hidden Files</source>
        <translation>Versteckte Dateien</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="818"/>
        <source>Show symbolic links in git folder</source>
        <translation>Zeige Dateiveknüpfungen an</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="821"/>
        <source>Symbolic Links</source>
        <translation>Verknüpfungen</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="805"/>
        <source>Show folders in git folder</source>
        <translation>Zeige Ordner in git Ordner an</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="808"/>
        <source>Directories</source>
        <translation>Ordner</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="757"/>
        <source>Show files in git folder</source>
        <translation>Zeige Dateien ind git Ordner an</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="136"/>
        <source>Show untracked</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="156"/>
        <source>Show stashed</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="242"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;
Shows file and folder tree of git repositories
&lt;ul&gt;
&lt;li&gt;Insert root git folder using &lt;b&gt;Add git source folder...&lt;/b&gt; &lt;img src=&quot;://resource/24X24/folder-open.png&quot; style=&quot;vertical-align:middle&quot;/&gt;&lt;/li&gt;
&lt;li&gt;Clone root git folder using &lt;b&gt;Clone...&lt;/b&gt; &lt;img src=&quot;://resource/24X24/icons8-clone-30.png&quot; style=&quot;vertical-align:middle&quot;/&gt;&lt;/li&gt;
&lt;li&gt;Remove root git folder using &lt;b&gt;Remove git source folder...&lt;/b&gt; &lt;img src=&quot;://resource/24X24/folder.png&quot; style=&quot;vertical-align:middle&quot;/&gt;&lt;/li&gt;
&lt;li&gt;&lt;b&gt;Update git status&lt;/b&gt; renews all files and status&lt;img src=&quot;://resource/24X24/view-refresh.png&quot; style=&quot;vertical-align:middle&quot;/&gt;&lt;/li&gt;
&lt;li&gt;&lt;b&gt;Show short status&lt;/b&gt; renews only status (F5)&lt;/li&gt;
&lt;li&gt;Insert ignored files via context menue of the selected folder&lt;/li&gt;
&lt;li&gt;Move git files or folders via drag and drop&lt;/li&gt;
&lt;li&gt;Show only files with desired git status using combo box &lt;b&gt;Show all files&lt;/b&gt;&lt;/li&gt;
&lt;li&gt;Select first file (or folder) by right click, start &lt;b&gt;Compare to...&lt;/b&gt; and select second file to compare&lt;/li&gt;
&lt;li&gt;Select file or folders and perform git actions&lt;/li&gt;
&lt;li&gt;Right click shows context menu&lt;/li&gt;
&lt;/ul&gt;
&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="393"/>
        <source>Save changes to file</source>
        <translation type="unfinished">Änderungen speichern</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="477"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;
Shows git history of files or folders
&lt;ul&gt;
&lt;li&gt;Insert all changed file names whith context menu entry&lt;/li&gt;
&lt;ul &gt;
&lt;li&gt;If one sub folder is selected, files are inserted regarding the difference to previous commit&lt;/li&gt;
&lt;li&gt;If two sub folders are selected, files are inserted into a new subentry regarding the difference between the selected commits&lt;/li&gt;
&lt;li&gt;If root folder is selected, for all sub items files are inserted&lt;/li&gt;
&lt;/ul&gt;
&lt;li&gt;Click on &lt;b&gt;History&lt;/b&gt; column item to see commit informations&lt;/li&gt;
&lt;li&gt;Click on &lt;b&gt;Description&lt;/b&gt; column item to see difference for all files within this commit&lt;/li&gt;
&lt;li&gt;Right click shows context menu&lt;/li&gt;
&lt;/ul&gt;
&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="505"/>
        <location filename="mainwindow.ui" line="551"/>
        <source>Description</source>
        <translation type="unfinished">Beschreibung</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="530"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;
Shows git stashed of files or folders
&lt;ul&gt;
&lt;li&gt;Click on &lt;b&gt;List Stashes&lt;/b&gt; to show stashes of repository&lt;img src=&quot;://resource/24X24/text-x-log.png&quot; style=&quot;vertical-align:middle&quot;/&gt;&lt;/li&gt;
&lt;li&gt;Right click shows context menu&lt;/li&gt;
&lt;/ul&gt;
&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="565"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;
Shows git branches
&lt;ul&gt;
&lt;li&gt;Double click branch to checkout&lt;/li&gt;
&lt;li&gt;Click &lt;b&gt;List Branches&lt;/b&gt; to show local branches of repository&lt;/li&gt;
&lt;li&gt;Right click shows context menu&lt;/li&gt;
&lt;/ul&gt;
&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="599"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;
Shows found text items and according lines and files&lt;br&gt;
Bookmarks are shown in the same way
&lt;ul&gt;
&lt;li&gt;Select &lt;b&gt;Text in Files below Folder&lt;/b&gt; in &lt;i&gt;Find in&lt;/i&gt; combo box&lt;/li&gt;
&lt;li&gt;Enter search expression&lt;/li&gt;
&lt;li&gt;Select folder to search in repository tree view&lt;/li&gt;
&lt;li&gt;Click on All&lt;/li&gt;
&lt;li&gt;Right click shows context menu&lt;/li&gt;
&lt;/ul&gt;
&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="619"/>
        <source>File</source>
        <translation type="unfinished">Datei</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="624"/>
        <source>Line</source>
        <translation type="unfinished">Zeile</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="629"/>
        <source>Found Text Line</source>
        <translation type="unfinished">Gefundene Textzeile</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="659"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;
Shows text of: 
&lt;ul&gt;
&lt;li&gt;git command results&lt;/li&gt;
&lt;li&gt;file content opened with double click on file in repository tree&lt;/li&gt;
&lt;li&gt;rendering of html can be enabled or disabled with &lt;b&gt;Render File&lt;/b&gt;&lt;/li&gt;
&lt;/ul&gt;
The command results and files may be shown in 3 modes
&lt;ul&gt;
&lt;li&gt;Command results and files in one editor&lt;/li&gt;
&lt;li&gt;Command results in viewer and files in one additional editor&lt;/li&gt;
&lt;li&gt;Command results in viewer and one additional editor per file (default)&lt;/li&gt;
&lt;/ul&gt;
The context menue allows:
&lt;ul&gt;
&lt;li&gt;to create bookmarks&lt;/li&gt;
&lt;li&gt;to find and replace&lt;/li&gt;
&lt;li&gt;show and hide line numbers&lt;/li&gt;
&lt;li&gt;&lt;b&gt;Extra&lt;/b&gt; text operations&lt;/li&gt;
&lt;li&gt;selection, copy and pase&lt;/li&gt;
&lt;/ul&gt;


&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="704"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;
Shows graphics file content opened with double click
&lt;ul&gt;
&lt;li&gt;Right click shows context menu&lt;/li&gt;
&lt;/ul&gt;
&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="719"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;
Shows binary file content opened with double click
&lt;ul&gt;
&lt;li&gt;Move byte wise with left or right cursor&lt;/li&gt;
&lt;li&gt;Move row wise with up or down cursor&lt;/li&gt;
&lt;li&gt;Select position with mouse cursor&lt;/li&gt;
&lt;/ul&gt;
&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="760"/>
        <source>Files</source>
        <translation>Dateien</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="828"/>
        <source>View</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="835"/>
        <source>Select Visual Style</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="845"/>
        <source>Select User defines Styles for App</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="849"/>
        <source>No Style</source>
        <translation type="unfinished">Kein Stil</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="854"/>
        <source>User Style</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="859"/>
        <source>Dark Palette</source>
        <translation type="unfinished">Dunkle Palette</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="867"/>
        <source>Select Toolbar Style</source>
        <translation type="unfinished">Wähle Stil für Werkzeugleiste</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="871"/>
        <source>Icon</source>
        <translation type="unfinished">Symbol</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="876"/>
        <source>Text</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="881"/>
        <source>Text beside Icon</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="886"/>
        <source>Text under Icon</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="891"/>
        <source>Follow System</source>
        <translation type="unfinished">Systemeinstellung folgen</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="899"/>
        <source>Tabs</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="906"/>
        <source>Select position of view tabs</source>
        <translation type="unfinished">Wähle Tap-Position für die Baumansichten</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="910"/>
        <source>Top</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="915"/>
        <source>Bottom</source>
        <translation type="unfinished">Unten</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="920"/>
        <source>Left</source>
        <translation type="unfinished">Links</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="925"/>
        <source>Right</source>
        <translation type="unfinished">Rechts</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="933"/>
        <source>Editor / Viewer</source>
        <translation type="unfinished">Editor / Textansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="946"/>
        <source>Select code page for text file&lt;br&gt;
Enter text to search</source>
        <translation type="unfinished">Codepage für Textdatei auswählen&lt;br&gt;Geben Sie den zu suchenden Text ein</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="960"/>
        <source>Open Editors (click What&apos;s this?)</source>
        <translation type="unfinished">Offene Editoren (Klick Hilfe für)</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="963"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;
Select additional open Editors.&lt;br&gt;
Git or command results are always displayed in main editor.
&lt;ul&gt;
&lt;li&gt;&lt;b&gt;No additional Editor:&lt;/b&gt; File text is also displayed in main editor&lt;/li&gt;
&lt;li&gt;&lt;b&gt;One additional Editor:&lt;/b&gt;File text is displayed in a separate editor&lt;/li&gt;
&lt;li&gt;&lt;b&gt;One Editor per File:&lt;/b&gt;Every file is displayed in a separate editor&lt;/li&gt;
&lt;/ul&gt;
&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="975"/>
        <source>No additional Editor</source>
        <translation type="unfinished">Kein Extra Editor</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="980"/>
        <source>One additional Editor</source>
        <translation type="unfinished">Ein Extra Editor</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="985"/>
        <source>One Editor per File</source>
        <translation type="unfinished">Ein Editor pro Datei</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="993"/>
        <location filename="mainwindow.ui" line="1000"/>
        <source>Tabulator width</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1013"/>
        <source>Font parameter</source>
        <translation type="unfinished">Schriftparameter</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1032"/>
        <source>Choose an equidistant font for the editor</source>
        <translation type="unfinished">Wähle äquidistante Schriftart</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1052"/>
        <source>Render graphics or HTML file in Graphics View</source>
        <translation type="unfinished">Zeigt Bilder oder HTML graphisch an</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1055"/>
        <source>Render File</source>
        <translation type="unfinished">Datei graphisch anzeigen</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1065"/>
        <source>Shows line numbers in text view</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1068"/>
        <source>Show line numbers</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1075"/>
        <source>Miscelaneous:</source>
        <translation type="unfinished">Verschiedenes:</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1082"/>
        <source>Display Type Converter</source>
        <translation type="unfinished">Zahlenformatkonverter</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1085"/>
        <source>Type Converter</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1092"/>
        <source>Use fast file search function (fsrc)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1095"/>
        <source>Fast File Search</source>
        <translation type="unfinished">Schnelle Dateisuche</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1102"/>
        <source>Store search results as a tree, not as single items</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1105"/>
        <source>Search Result as Tree</source>
        <translation type="unfinished">Suchergebnis in Baumansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1115"/>
        <location filename="mainwindow.ui" line="1118"/>
        <source>Show history also in graphics view</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1121"/>
        <source>Show History graphically</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1128"/>
        <source>Experimental functions</source>
        <translation type="unfinished">Experimentelle Funktionen</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1131"/>
        <source>Experimental</source>
        <translation type="unfinished">Experimentell</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1138"/>
        <source>Append background command to batch list, if busy</source>
        <translation type="unfinished">Kommando im Hintergrund ausführen</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1141"/>
        <source>Append to Batch</source>
        <translation type="unfinished">Zu Stapel für Hintergrundausführung hinzufügen</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1154"/>
        <source>Find in </source>
        <translation>Suche in </translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1176"/>
        <source>Select View for search function</source>
        <translation>Ansicht für Suchfunktion auswählen</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1183"/>
        <source>Search backward</source>
        <translation>Rückwärts suchen</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1186"/>
        <source>Previous</source>
        <translation>Rückwärts</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1199"/>
        <source>Search forward</source>
        <translation>Vorwärts suchen</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1202"/>
        <source>Next</source>
        <translation>Vorwärts</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1209"/>
        <source>Enter search expression or words</source>
        <translation>Suchwort bzw. regulären Ausdruck eingeben</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1216"/>
        <source>Search all </source>
        <translation type="unfinished">Alle suchen</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1219"/>
        <location filename="mainwindow.cpp" line="2266"/>
        <source>All</source>
        <translation type="unfinished">Alle</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1226"/>
        <source>Enter replace text</source>
        <translation type="unfinished">Text zum Ersetzen eingeben</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1240"/>
        <source>Search case sensitive</source>
        <translation>Beachte Groß- und Kleinschreibung</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1243"/>
        <source>Case Sensitive </source>
        <translation>Groß/kleinbuchstaben </translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1250"/>
        <source>Search whole words</source>
        <translation>Suche ganzes Wort</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1253"/>
        <source>Whole Word </source>
        <translation>Ganzes Wort </translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1260"/>
        <source>Search with regular expression</source>
        <translation>Suche mit regulären Ausdrücken</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1263"/>
        <source>Regular Expression </source>
        <translation>Reguläre Ausdrücke </translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1274"/>
        <source>Select source folder...</source>
        <translation>Wähle git Repository aus...</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1279"/>
        <source>Select Destination...</source>
        <translation>Wähle Ziel...</translation>
    </message>
    <message>
        <source>Text Editor</source>
        <translation type="vanished">Texteditor</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="767"/>
        <source>History View</source>
        <translation>Historienansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="773"/>
        <source>Branch View</source>
        <translation type="unfinished">Codezweigansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="806"/>
        <source>Git File View States</source>
        <translation>Git Textansichtwerkzeuge</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="815"/>
        <source>Text Editor / Viewer Control</source>
        <translation>Test-editor / ansicht Werkzeuge</translation>
    </message>
    <message>
        <source>File Flags</source>
        <translation type="vanished">Dateieigenschaften</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="1173"/>
        <location filename="mainwindow.cpp" line="833"/>
        <source>Find</source>
        <translation>Suche</translation>
    </message>
    <message>
        <location filename="mainwindow_sourcetree.cpp" line="217"/>
        <source>Repository: </source>
        <translation>Repository: </translation>
    </message>
    <message>
        <location filename="mainwindow_sourcetree.cpp" line="281"/>
        <source>Select SourceFiles</source>
        <translation>Wähle git Repository</translation>
    </message>
    <message>
        <location filename="mainwindow_sourcetree.cpp" line="296"/>
        <source>Select file to open</source>
        <translation type="unfinished">Wähle zu öffnende Datei</translation>
    </message>
    <message>
        <location filename="mainwindow_sourcetree.cpp" line="305"/>
        <source>Enter file new name</source>
        <translation type="unfinished">Neuen Namen eingeben</translation>
    </message>
    <message>
        <location filename="mainwindow_sourcetree.cpp" line="395"/>
        <source>Open large file %1?</source>
        <translation type="unfinished">Große Datei %1 öffnen?</translation>
    </message>
    <message>
        <location filename="mainwindow_sourcetree.cpp" line="396"/>
        <source>File size %1 is larger than
warning level size %2.
This could be time consuming</source>
        <translation type="unfinished">Dateigröße %1 ist größer als\nWarnungsgröße %2.\n</translation>
    </message>
    <message>
        <location filename="mainwindow_sourcetree.cpp" line="434"/>
        <source>binary</source>
        <translation type="unfinished">Binär</translation>
    </message>
    <message>
        <location filename="mainwindow_sourcetree.cpp" line="542"/>
        <source>Total selected bytes: </source>
        <translation>Gesamt ausgewählte Bytes: </translation>
    </message>
    <message>
        <location filename="mainwindow_sourcetree.cpp" line="727"/>
        <source>Clone git Repository</source>
        <translation type="unfinished">Klone git Repository</translation>
    </message>
    <message>
        <location filename="mainwindow_sourcetree.cpp" line="728"/>
        <source>Enter git repository address:</source>
        <translation type="unfinished">Git Repositoryadresse eingeben</translation>
    </message>
    <message>
        <location filename="mainwindow_sourcetree.cpp" line="731"/>
        <source>Select destination for repository &quot;%1&quot;</source>
        <translation type="unfinished">Wähle Ziel für Repository \&quot;%1\&quot;</translation>
    </message>
    <message>
        <location filename="mainwindow_sourcetree.cpp" line="804"/>
        <source>Enter a new name or destination for &quot;%1&quot;.
To move the %3 insert the destination path before.
To rename just modify the name.</source>
        <translation type="unfinished">Neuen Namen bzw. Zielort für \&quot;%1\&quot; eingeben.\nUm %3 zu verschieben füge das Zielverzeichnis davor ein.\nFür Umbennenung muss nur der Name geändert werden.</translation>
    </message>
    <message>
        <location filename="mainwindow_sourcetree.cpp" line="1122"/>
        <source>Could not delete %1 %2
</source>
        <translation type="unfinished">Konnte %1 %2 nicht löschen</translation>
    </message>
    <message>
        <location filename="mainwindow_sourcetree.cpp" line="1239"/>
        <source>Information about %1
%2</source>
        <translation type="unfinished">Information über %1\n%2</translation>
    </message>
    <message>
        <location filename="mainwindow_textview.cpp" line="302"/>
        <source>Save content of text editor</source>
        <translation>Speichere Inhalt der Textansicht</translation>
    </message>
    <message>
        <source>The document has been modified.</source>
        <translation type="vanished">Die Textdatei wurde geändert.</translation>
    </message>
    <message>
        <location filename="mainwindow_textview.cpp" line="109"/>
        <source>Reload file%1?;File %1%2 has changed</source>
        <translation type="unfinished">Datei %1 neu laden?;Datei %1%2 wurde geändert</translation>
    </message>
    <message>
        <location filename="mainwindow_textview.cpp" line="224"/>
        <source>The document has been modified.

%1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow_textview.cpp" line="225"/>
        <source>Do you want to save your changes?</source>
        <translation>Möchten Sie die Änderungen speichern?</translation>
    </message>
    <message>
        <location filename="mainwindow_textview.cpp" line="339"/>
        <source> Type: </source>
        <translation> Typ: </translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1511"/>
        <source>Show difference</source>
        <translation>Zeige Unterschiede an</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1518"/>
        <source>Call diff tool...</source>
        <translation>Zeige Unterschiede extern an...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1524"/>
        <source>Call merge tool...</source>
        <translation>Rufe Zusammenführungswerkzeug auf...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1530"/>
        <source>Merge file...</source>
        <translation>Führe Datei zusammen...</translation>
    </message>
    <message>
        <source>Merge selected file</source>
        <translation type="vanished">Führe Änderungen der ausgewählten Datei zusammen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="142"/>
        <source>QTreeView::branch:has-siblings:!adjoins-item {    border-image: url(%1) 0; }QTreeView::branch:has-siblings:adjoins-item {    border-image: url(%2) 0; }QTreeView::branch:!has-children:!has-siblings:adjoins-item {    border-image: url(%3) 0; }QTreeView::branch:has-children:!has-siblings:closed,QTreeView::branch:closed:has-children:has-siblings {     border-image: none; image: url(%4); }QTreeView::branch:open:has-children:!has-siblings,QTreeView::branch:open:has-children:has-siblings  {     border-image: none; image: url(%5); }</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="748"/>
        <source>Text View/Editor</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="750"/>
        <source>Go to line</source>
        <translation type="unfinished">Gehe zu Zeile</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="756"/>
        <source>Graphics View</source>
        <translation type="unfinished">Grafikansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="762"/>
        <source>Binary View</source>
        <translation type="unfinished">Binär Ansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="779"/>
        <source>Stash View</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="785"/>
        <source>Found in Text Files</source>
        <translation type="unfinished">Gefunden in Textdateien</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="786"/>
        <source> View</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="787"/>
        <source>Text files below folder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="792"/>
        <source>Binary Values</source>
        <translation type="unfinished">Binärwert</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="799"/>
        <source>Html &amp;&amp; Markdown</source>
        <translation type="unfinished">HTML &amp;&amp; Markdown</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="824"/>
        <source>Settings</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1338"/>
        <source>pidof %1</source>
        <translation type="unfinished">Prozess ID von %1</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1352"/>
        <source>Do you really whant to kill all background processes &quot;%1&quot;?;%1
Yes kills process ID: %2
Yes To All empties also batch list</source>
        <translation type="unfinished">Soll der Hintergrundprozess \&quot;%1\&quot; abgebrochen werden?;%1\n</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1373"/>
        <source>no pids found for %1</source>
        <translation type="unfinished">Keine Prozess ID gefunden für %1</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1378"/>
        <source>no command found: %1</source>
        <translation type="unfinished">Kein Kommando gefunden: %1</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1503"/>
        <location filename="mainwindow_branchtree.cpp" line="45"/>
        <location filename="mainwindow_historytree.cpp" line="55"/>
        <location filename="mainwindow_historytree.cpp" line="89"/>
        <location filename="mainwindow_sourcetree.cpp" line="701"/>
        <location filename="mainwindow_sourcetree.cpp" line="711"/>
        <location filename="mainwindow_sourcetree.cpp" line="740"/>
        <location filename="mainwindow_sourcetree.cpp" line="847"/>
        <location filename="mainwindow_sourcetree.cpp" line="933"/>
        <source>
Error %1 occurred</source>
        <translation type="unfinished">Fehler %1 ist aufgetreten</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1530"/>
        <source>Merge selected file (experimental, not working)</source>
        <translation>Führe gewählte Datei zusammen (experimentell)</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1533"/>
        <source>Edit Highlighting...</source>
        <translation>Bearbeite Texthervorhebung...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1533"/>
        <source>Edit highlighting color and font</source>
        <translation>Bearbeitet Farbe und Schrift der Hervorhebung</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1537"/>
        <source>Show status</source>
        <translation>Aktualisiere git Status</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1540"/>
        <source>Show short status</source>
        <translation>Aktualisiere Kurzstatus</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1545"/>
        <source>Add to git (stage)</source>
        <translation>Zu git hinzufügen (stagen)</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1551"/>
        <source>Reset file (unstage)</source>
        <translation>Aus git entfernen (unstage)</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1556"/>
        <source>Show History</source>
        <translation>Historie anzeigen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1561"/>
        <source>Blame</source>
        <translation type="unfinished">Author zuordnen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1566"/>
        <source>Remove from git...</source>
        <translation>Aus git enfernen...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1572"/>
        <source>Restore changes...</source>
        <translation>Änderungen rückgangig machen...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1573"/>
        <source>Restore %1 changes;Do you want to restore changes in &quot;%1&quot;?</source>
        <translation type="unfinished">Wiederherstellen von %1 ;Sollen die Änderungen in \&quot;%1\&quot; rückgängig gemacht werden?</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1601"/>
        <source>Stash all entries;Do you whant to stash all entries of repository:
&quot;%1&quot;?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1607"/>
        <source>Show stash</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1610"/>
        <source>Stash push</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1619"/>
        <source>Stash apply</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1622"/>
        <source>Stash drop</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1625"/>
        <source>Stash clear</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1628"/>
        <source>List stashes</source>
        <translation type="unfinished">Stash auflisten</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1656"/>
        <source>Checkout %1;Do you want to set &quot;%1&quot; active?;</source>
        <translation type="unfinished">Aktiviere Zweig %1;Den Codezweig \&quot;%1\&quot; aktivieren?;</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1660"/>
        <source>Diff between Branches</source>
        <translation type="unfinished">Unteschied zwischen Codezweigen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1721"/>
        <source>Clear all tree entries</source>
        <translation type="unfinished">Alle Einträge löschen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1721"/>
        <source>Clears all tree entries in focused tree except repository tree</source>
        <translation type="unfinished">Löscht alle Einträge der aktiven Baumansicht außer im Repository Baum</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1725"/>
        <source>Delete selected tree items</source>
        <translation type="unfinished">Lösche Auswahl in Baumansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1725"/>
        <source>Deletes all selected tree items in focused tree except repository tree</source>
        <translation type="unfinished">Löscht alle gewählten Einträge der Baumansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1729"/>
        <source>Set open external flag</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1729"/>
        <source>Sets parameters to open files with this extension externally</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1733"/>
        <source>Remove open external flag</source>
        <translation type="unfinished">Extern öffen deaktivieren</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1733"/>
        <source>Removes open external parameters</source>
        <translation type="unfinished">Parameter für externes Öffen löschen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1737"/>
        <source>Open file externally</source>
        <translation type="unfinished">Datei mit externem Programm öffnen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1737"/>
        <source>Opens file externally</source>
        <translation type="unfinished">Öffnet Datei mit externem Programm</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1741"/>
        <source>Whats this?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1741"/>
        <source>Whats this help</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1744"/>
        <source>Compare to...</source>
        <translation type="unfinished">Vergleiche mit...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1744"/>
        <source>Start compare mode to select other file ore folder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1748"/>
        <source>Close all</source>
        <translation type="unfinished">Alle schließen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1748"/>
        <source>Closes all documents</source>
        <translation type="unfinished">Schließt alle Dokumente</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1749"/>
        <source>Save all</source>
        <translation type="unfinished">Alles speichern</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1749"/>
        <source>Saves all documents</source>
        <translation type="unfinished">Alle Dokumente speichern</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1769"/>
        <source>Running Background Activity...</source>
        <translation type="unfinished">Starte Hintergrund Aktivität...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1769"/>
        <source>You may try to kill git action running in background</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1774"/>
        <source>Copy file name</source>
        <translation type="unfinished">Kopiere Dateinamen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1774"/>
        <source>Copy file name to clipboard</source>
        <translation type="unfinished">Kopiere Dateinamen in die Zwischenablage</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1777"/>
        <source>Copy file and path</source>
        <translation type="unfinished">Kopiere Dateipfad</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1777"/>
        <source>Copy file or folder and path to clipboard</source>
        <translation type="unfinished">Kopiere Datei- bzw. Ordnerpfad in die Zwischenablage</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1782"/>
        <source>Zoom in</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1782"/>
        <source>Zoom in (make larger)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1785"/>
        <source>Zoom out</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1785"/>
        <source>Zoom out (make smaller)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1788"/>
        <source>Fit in View</source>
        <translation type="unfinished">Alles in der Ansicht anzeigen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1788"/>
        <source>Fits Item in View, when opened</source>
        <translation type="unfinished">Alles in der Ansicht anzeigen, wenn geöffnet wird</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1793"/>
        <source>Clone text view</source>
        <translation type="unfinished">Klone Textansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1793"/>
        <source>Opens this file in a new window</source>
        <translation type="unfinished">Öffnet Datei in neuem Fenster</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1796"/>
        <source>Create Bookmark</source>
        <translation type="unfinished">Lesezeichen erstellen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1796"/>
        <source>Creates a Bookmark</source>
        <translation type="unfinished">Erstellt ein Lesezeichen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1799"/>
        <source>Show Information</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1799"/>
        <source>Show information about selected item</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1804"/>
        <source>test command</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1987"/>
        <source>Program parameter</source>
        <translation type="unfinished">Programm Parameter</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1988"/>
        <source>Set open program for &apos;%1&apos; file extension
Leave empty for default program &apos;%2&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="2260"/>
        <source>Replace</source>
        <translation type="unfinished">Ersetzen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="2261"/>
        <source>Replace expression</source>
        <translation type="unfinished">Ersetzt Ausdruck</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="2267"/>
        <source>Search expression in all files of selected folder</source>
        <translation type="unfinished">Suche Ausdruck in allen Dateien des gewählten Ordners</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="2276"/>
        <source>Search in Text Editor</source>
        <translation type="unfinished">Suche im Text Editor</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="2277"/>
        <source>Go to line in Text Editor</source>
        <translation type="unfinished">Gehe zu Zeile im Texteditor</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="2278"/>
        <source>Search for text in files under selected folder in Repository View</source>
        <translation type="unfinished">Suche nach Text in Dateien unterhalb des gewählten Ordners in der Repository Ansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="2279"/>
        <source>Search for text in find results</source>
        <translation type="unfinished">Suche nach Text in Suchergebnis Ansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="2280"/>
        <source>Search files or folders in Repository View</source>
        <translation type="unfinished">Suche Dateien oder Orner in der Repository Ansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="2281"/>
        <source>Search item in History View</source>
        <translation type="unfinished">Suche Text in Historienansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="2282"/>
        <source>Search item in Branch View</source>
        <translation type="unfinished">Suche Text ind Zweigansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="2283"/>
        <source>Search in Stash View</source>
        <translation type="unfinished">Suche in Stash Ansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="2644"/>
        <source>%1 -d %2 %3 &apos;%4&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="2668"/>
        <source>%1 %2 &apos;%3&apos; &apos;%4&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="2674"/>
        <source>Search expression: %1</source>
        <translation type="unfinished">Such Asudruck: %1</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="2757"/>
        <source>
result error: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="2893"/>
        <source>Text Viewer</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="2893"/>
        <source>Text Viewer / Editor</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Restore changes;Do you want to restore changes in file &quot;%1&quot;?</source>
        <translation type="vanished">Änderungen rückgängig;Möchten Sie die Änderungen in der Datei &quot;%1&quot; verwerfen?</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1580"/>
        <source>Commit...</source>
        <translation>Committen...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1585"/>
        <source>Clone</source>
        <translation type="unfinished">Klonen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1588"/>
        <source>Push</source>
        <translation>Hochladen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1591"/>
        <source>Pull</source>
        <translation>Herunerladen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1595"/>
        <source>Fetch</source>
        <translation type="unfinished">Holen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1598"/>
        <source>Show</source>
        <translation>Anzeigen</translation>
    </message>
    <message>
        <location filename="mainwindow.ui" line="546"/>
        <location filename="mainwindow.cpp" line="1602"/>
        <source>Stash</source>
        <translation>In git Speicher verschieben</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1616"/>
        <source>Stash pop</source>
        <translation>Aus git Speicher holen</translation>
    </message>
    <message>
        <source>Stash show</source>
        <translation type="vanished">git Speicher anzeigen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1632"/>
        <source>List Branches</source>
        <translation>Quellcodezweige auflisten</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1636"/>
        <source>List remote Branches</source>
        <translation>Entfernte Quellcodezweige auflisten</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1640"/>
        <source>List merged Branches</source>
        <translation>Zusammengeführte Quellcodezweige auflisten</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1644"/>
        <source>List not merged Branches</source>
        <translation>Nicht zusammengeführte Quellcodezweige auflisten</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1649"/>
        <source>Delete Branch</source>
        <translation>Zweig löschen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1650"/>
        <source>Delete %1 from git;Do you want to delete &quot;%1&quot;?</source>
        <translation>Lösche %1 aus git;Möchten sie &quot;%1&quot; aus git entfernen?</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1655"/>
        <source>Checkout Branch</source>
        <translation>Zweig auschecken</translation>
    </message>
    <message>
        <source>Checkout %1;Do you want to set &quot;%1&quot; active?</source>
        <translation type="vanished">Auschecken von %1;Möchten Sie den Zweig &quot;%1&quot; aktivieren?</translation>
    </message>
    <message>
        <source>History Branch</source>
        <translation type="vanished">Historie des Zweiges</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1663"/>
        <source>Show Branch</source>
        <translation>Zweig anzeigen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1667"/>
        <source>Move / Rename...</source>
        <translation>Verschieben / Umbenennen...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1674"/>
        <source>Expand Tree Items</source>
        <translation>Baumansicht aufklappen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1674"/>
        <source>Expands all tree item of focused tree</source>
        <translation>Klappt alle Zweige der Baumansicht auf</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1678"/>
        <source>Collapse Tree Items</source>
        <translation>Zweige zuklappen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1678"/>
        <source>Collapses all tree item of focused tree</source>
        <translation>Klappt alle Zweige der Baumansicht zusammen</translation>
    </message>
    <message>
        <source>Do you really whant to kill the git processes &quot;%1&quot;?</source>
        <translation type="vanished">Möchten Sie die Prozesse &quot;%1&quot; wirklich abbbrechen?</translation>
    </message>
    <message>
        <source>Add git source folder</source>
        <translation type="vanished">Füge git Repository hinzu</translation>
    </message>
    <message>
        <source>Adds a git source folder to the source treeview</source>
        <translation type="vanished">Fügt ein git Repository hinzu</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1683"/>
        <source>Add git source folder...</source>
        <translation>git Repository hinzufügen...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1683"/>
        <source>Add a git source folder to repository view</source>
        <translation type="unfinished">Git Repository Ordner hinzufügen...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1686"/>
        <source>Remove git source folder</source>
        <translation type="unfinished">Git Repository Ordner schließen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1686"/>
        <source>Remove a git source folder from repository view</source>
        <translation type="unfinished">Git Repo schließen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1689"/>
        <source>Open File...</source>
        <translation type="unfinished">Datei öffnen...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1689"/>
        <source>Open arbitrary file</source>
        <translation type="unfinished">Beliebige Datei öffnen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1693"/>
        <source>Save File as...</source>
        <translation type="unfinished">Speichern unter...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1693"/>
        <source>Save file under alternative name</source>
        <translation type="unfinished">Speichern mit anderem Namen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1697"/>
        <source>Replace All</source>
        <translation type="unfinished">Alles ersetzen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1697"/>
        <source>Replace all found items</source>
        <translation type="unfinished">Ersetzt alles gefundenen Einträge</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1701"/>
        <source>To Upper.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1701"/>
        <source>Modify selected text to upper case</source>
        <translation type="unfinished">Seletierten Text in Großbuchstaben ändern</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1704"/>
        <source>To Lower</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1704"/>
        <source>Modify selected text to lower case</source>
        <translation type="unfinished">Seletierten Text in Kleinbuchstaben ändern</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1707"/>
        <source>Comment / uncomment</source>
        <translation type="unfinished">Kommentieren / Auskommentieren</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1707"/>
        <source>Comment or uncomment selected text lines</source>
        <translation type="unfinished">Wandelt Code in Komentar und umgekehrt</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1710"/>
        <source>To Snake Case</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1710"/>
        <source>Modify selected text to snake_case</source>
        <translation type="unfinished">Seletierten Text in snake_case ändern</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1713"/>
        <source>To Camel Case</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1713"/>
        <source>Modify selected text to CameCase</source>
        <translation type="unfinished">Seletierten Text in CameCase ändern</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1717"/>
        <source>Update git status</source>
        <translation>git Status aktualisieren</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1717"/>
        <source>Updates the git status of the selected source folder</source>
        <translation>Aktualisiert den git Status des ausgewählten Eintrags</translation>
    </message>
    <message>
        <source>Show/Hide tree</source>
        <translation type="vanished">Show/Hide tree</translation>
    </message>
    <message>
        <source>Shows or hides history or branches tree</source>
        <translation type="vanished">Shows or hides history or branches tree</translation>
    </message>
    <message>
        <source>Clear tree items</source>
        <translation type="vanished">Löscht alle Einträge</translation>
    </message>
    <message>
        <source>Clears all history or branch tree entries</source>
        <translation type="vanished">Löscht alle Einträge der Baumansicht</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1751"/>
        <source>Customize git actions...</source>
        <translation>Benutzer Kommandos bearbeiten...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1751"/>
        <source>Edit custom git actions, menues and toolbars</source>
        <translation>Bearbeitet git Kommandos, Menüeintrage und Werkzeugleisteneinträge</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1754"/>
        <source>Insert File Name List</source>
        <translation>Für Dateinameliste ein</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1754"/>
        <source>Inserts file names that differ from previous hash</source>
        <translation>Fügt Dateinamen ein, die von dem vorzustand abweichen</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1757"/>
        <source>About...</source>
        <translation>Über...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1757"/>
        <source>Information about GitView</source>
        <translation>Über GitView</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1761"/>
        <source>Delete...</source>
        <translation>Löschen...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1761"/>
        <source>Delete file or folder</source>
        <translation>Lösche Datei bzw. Ordner</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1765"/>
        <source>Select Language...</source>
        <translation>Wähle Sprache...</translation>
    </message>
    <message>
        <location filename="mainwindow.cpp" line="1765"/>
        <source>Select language for text highlighting</source>
        <translation>Wähle Sprache für Texthervorhebung</translation>
    </message>
    <message>
        <source>Kill Background Activity...</source>
        <translation type="vanished">Hintergrundaktivität abbrechen...</translation>
    </message>
    <message>
        <source>Kill git action running in background</source>
        <translation type="vanished">git Kommando abbrechen</translation>
    </message>
    <message>
        <location filename="mainwindow_sourcetree.cpp" line="772"/>
        <source>git -C %1 difftool --tool-help</source>
        <translation>git -C %1 difftool --tool-help</translation>
    </message>
    <message>
        <source>Enable all</source>
        <translation type="vanished">Alle anzeigen</translation>
    </message>
    <message>
        <source>Disable all</source>
        <translation type="vanished">Keinen anzeigen</translation>
    </message>
    <message>
        <source>Authors</source>
        <translation type="vanished">Autoren</translation>
    </message>
    <message>
        <location filename="mainwindow_sourcetree.cpp" line="803"/>
        <source>Move or rename %1</source>
        <translation>Verschieben bzw. umbenennen %1</translation>
    </message>
    <message>
        <source>Enter a new name or destination for &quot;./%1&quot;.
To move the %3 insert the destination path before.
To rename just modify the name.</source>
        <translation type="vanished">Neuer Name bzw. Zielort &quot;./%1&quot;.
Zum Verschieben von %3 muss der Zielort vor dem Namen eingegeben werden.
Zum Umbenennen muss nur der Name geändert werden.</translation>
    </message>
    <message>
        <location filename="mainwindow_historytree.cpp" line="29"/>
        <source>history</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="mainwindow_historytree.cpp" line="74"/>
        <source>git -C %1 </source>
        <translation>git -C %1 </translation>
    </message>
    <message>
        <location filename="mainwindow_historytree.cpp" line="144"/>
        <source>git show %1:%2</source>
        <translation type="unfinished">git show %1:%2</translation>
    </message>
    <message>
        <location filename="mainwindow_sourcetree.cpp" line="1091"/>
        <source>Delete %1</source>
        <translation>Lösche %1</translation>
    </message>
    <message>
        <location filename="mainwindow_textview.cpp" line="53"/>
        <location filename="mainwindow_textview.cpp" line="383"/>
        <source>%1</source>
        <translation>%1</translation>
    </message>
    <message>
        <location filename="mainwindow_branchtree.cpp" line="77"/>
        <location filename="mainwindow_stashtree.cpp" line="70"/>
        <source>Error %1 occurred</source>
        <translation type="unfinished">Fehler %1 ist aufgetreten</translation>
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
        <location filename="mergedialog.cpp" line="107"/>
        <source>git log -- %1</source>
        <translation>git log -- %1</translation>
    </message>
    <message>
        <location filename="mergedialog.cpp" line="122"/>
        <source>git %1</source>
        <translation>git %1</translation>
    </message>
</context>
<context>
    <name>QBranchTreeWidget</name>
    <message>
        <location filename="qbranchtreewidget.cpp" line="48"/>
        <source>git -C %1 log %2 --max-count=1 --pretty=format:&quot;%H&lt;td&gt;%T&lt;td&gt;%P&lt;td&gt;%B&lt;td&gt;%an&lt;td&gt;%ae&lt;td&gt;%ad&lt;td&gt;%cn&lt;td&gt;%ce&lt;td&gt;%cd&lt;tr&gt;&quot;</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>QHistoryTreeWidget</name>
    <message>
        <location filename="qhistorytreewidget.cpp" line="237"/>
        <location filename="qhistorytreewidget.cpp" line="260"/>
        <source>Enable all</source>
        <translation type="unfinished">Alle anzeigen</translation>
    </message>
    <message>
        <location filename="qhistorytreewidget.cpp" line="238"/>
        <location filename="qhistorytreewidget.cpp" line="261"/>
        <source>Disable all</source>
        <translation type="unfinished">Keinen anzeigen</translation>
    </message>
    <message>
        <location filename="qhistorytreewidget.cpp" line="273"/>
        <source>Authors</source>
        <translation type="unfinished">Autoren</translation>
    </message>
    <message>
        <location filename="qhistorytreewidget.cpp" line="304"/>
        <source>Files: %1</source>
        <translation>Dateien: %1</translation>
    </message>
    <message>
        <location filename="qhistorytreewidget.cpp" line="321"/>
        <source>Result failure no: %1</source>
        <translation>Ergebnisfehlernummer: %1</translation>
    </message>
    <message>
        <location filename="qhistorytreewidget.cpp" line="386"/>
        <source>git diff --name-only %1 %2</source>
        <translation>git diff --name-only %1 %2</translation>
    </message>
    <message>
        <location filename="qhistorytreewidget.cpp" line="392"/>
        <source>git diff --name-only %1</source>
        <translation>git diff --name-only %1</translation>
    </message>
    <message>
        <location filename="qhistorytreewidget.cpp" line="407"/>
        <source>Branch difference for repository %1</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <source>Git Commands</source>
        <translation type="vanished">Git Kommandos</translation>
    </message>
    <message>
        <source>Control Commands</source>
        <translation type="vanished">Kontroll Kommandos</translation>
    </message>
    <message>
        <location filename="git_type.cpp" line="79"/>
        <source>Toolbar Git Commands</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="git_type.cpp" line="80"/>
        <source>Toolbar Control Commands</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="git_type.cpp" line="88"/>
        <source>Do Nothing</source>
        <translation>Tue nichts</translation>
    </message>
    <message>
        <location filename="git_type.cpp" line="89"/>
        <source>Update Item Status</source>
        <translation>Staus aktualisieren</translation>
    </message>
    <message>
        <location filename="git_type.cpp" line="90"/>
        <source>Parse History List Text</source>
        <translation>Parse Historienausgabe</translation>
    </message>
    <message>
        <location filename="git_type.cpp" line="91"/>
        <source>Parse Branch List Text</source>
        <translation>Parse git branch Ausgabe</translation>
    </message>
    <message>
        <location filename="git_type.cpp" line="92"/>
        <source>Parse Stash List Text</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="git_type.cpp" line="93"/>
        <source>Parse Blame Text</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="git_type.cpp" line="94"/>
        <source>Update Root Item Status</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="git_type.cpp" line="95"/>
        <source>Update Repository Status</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>&quot;%1&quot; not inserted from %2</source>
        <translation type="vanished">&quot;%1&quot; not inserted from %2</translation>
    </message>
    <message>
        <source>&quot;%1&quot; inserted from %2</source>
        <translation type="vanished">&quot;%1&quot; inserted from %2</translation>
    </message>
    <message>
        <location filename="helper.cpp" line="156"/>
        <location filename="helper.cpp" line="176"/>
        <source>Delete %1 entry;%1</source>
        <translation>Lösche Eintrag %1; %1</translation>
    </message>
    <message>
        <location filename="helper.cpp" line="156"/>
        <location filename="helper.cpp" line="176"/>
        <source>Tree</source>
        <translation>Baum</translation>
    </message>
    <message>
        <location filename="helper.cpp" line="375"/>
        <source>
Error occurred executing command: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="helper.cpp" line="378"/>
        <source>
Error number : %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Error occurred executing command: </source>
        <translation type="vanished">Ein Fehler ist augetreten bei der Ausführung des Kommandos: </translation>
    </message>
    <message>
        <source>Error number : </source>
        <translation type="vanished">Fehlernummer: </translation>
    </message>
    <message>
        <location filename="main.cpp" line="26"/>
        <source>Alternative Config file name.</source>
        <translation>Alternativer Konfigurationsdateiname.</translation>
    </message>
    <message>
        <location filename="main.cpp" line="27"/>
        <source>Log file name for debugging.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="main.cpp" line="32"/>
        <source>Language.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>QSourceTreeWidget</name>
    <message>
        <location filename="qsourcetreewidget.cpp" line="461"/>
        <source>Insert Ignored Folder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="qsourcetreewidget.cpp" line="492"/>
        <source>Insert Ignored File</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>QStashTreeWidget</name>
    <message>
        <location filename="qstashtreewidget.cpp" line="48"/>
        <source>git -C %1 show %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="qstashtreewidget.cpp" line="66"/>
        <source>git -C %1 stash show %2</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>WorkerThreadConnector</name>
    <message>
        <location filename="workerthreadconnector.cpp" line="98"/>
        <source>Background commands:
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="workerthreadconnector.cpp" line="101"/>
        <source>none</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="workerthreadconnector.cpp" line="105"/>
        <source>currently running: %1
</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>binary_values_view</name>
    <message>
        <location filename="binary_values_view.ui" line="14"/>
        <source>Dialog</source>
        <translation type="unfinished">Dialog</translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="53"/>
        <source>Length</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="82"/>
        <source>Unsigned</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="114"/>
        <source>Signed</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="143"/>
        <source>Hexadecimal</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="173"/>
        <source>Character</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="187"/>
        <source>Check to edit this value</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="197"/>
        <source>Hit Enter to confirm input</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="264"/>
        <source>Short</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="339"/>
        <source>Long</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="414"/>
        <source>LongLong</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="489"/>
        <source>Float</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="533"/>
        <source>Double</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="577"/>
        <source>Binary</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="631"/>
        <source>Endian</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="643"/>
        <source>Select little endian</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="646"/>
        <source>Little</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="659"/>
        <source>Select big endian</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="662"/>
        <source>Big</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="677"/>
        <source>Check to edit local values</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="680"/>
        <source>Local Edit</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="711"/>
        <source>Binary View Settings</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="732"/>
        <source>Choose data type for table view</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="739"/>
        <source>Edit number of collumns shown in binary table view</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="752"/>
        <source>Edit offset for first byte</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="774"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Read values again from binary content&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="777"/>
        <source>Read Value</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="784"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Store edited value to binary content&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="787"/>
        <source>Write Value</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="796"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Opens Json file with binary format description of the file content.&lt;br/&gt;Click &amp;quot;Whats This help&amp;quot; &lt;img src=&quot;://resource/24X24/dialog-question.png&quot; style=&quot;vertical-align:middle&quot;/&gt; to see file format&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.ui" line="799"/>
        <source>Open Type Format File...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.cpp" line="391"/>
        <source>Wrong input value: %1 for type %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="binary_values_view.cpp" line="400"/>
        <source>Open type format file for binary content</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>code_browser</name>
    <message>
        <location filename="code_browser.cpp" line="669"/>
        <source>commit:	</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="code_browser.cpp" line="669"/>
        <source>author:	</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="code_browser.cpp" line="669"/>
        <source>date:	</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="code_browser.cpp" line="669"/>
        <source>time:	</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>graphics_view</name>
    <message>
        <location filename="graphics_view.cpp" line="96"/>
        <source>Cancel</source>
        <translation type="unfinished">Abbrechen</translation>
    </message>
</context>
<context>
    <name>qbinarytableview</name>
    <message>
        <location filename="qbinarytableview.cpp" line="53"/>
        <source>Typed</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="qbinarytableview.cpp" line="53"/>
        <source>Character</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
