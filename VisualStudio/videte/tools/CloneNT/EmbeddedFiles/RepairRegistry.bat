@echo off
cd c:\WinNT\system32\config

expand c:\WinNT\repair\system._   system
expand c:\WinNT\repair\sam._      sam
expand c:\WinNT\repair\software._ software
expand c:\WinNT\repair\default._  default
expand c:\WinNT\repair\security._ security

shutdown.exe /L /T:0 /Y /C /R




