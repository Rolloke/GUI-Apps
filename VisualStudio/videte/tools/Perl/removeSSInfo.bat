

REM pattern is begin of line, comment with space, one $ in text one $ at EOL

v:\perl5\bin\superReplace.bat "^\/\/ .*\$.*\$$" GARNIX
v:\perl5\bin\removeGlobalReplace.bat
del remove.log
del get.bat

