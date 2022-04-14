xcopy mdsad301.sys %WINDIR%\System32\Drivers\ /Y
xcopy mdsdsp.dll %WINDIR%\System32\ /Y
xcopy mdsad301.inf %WINDIR%\Inf\ /Y
xcopy Tasha.sys %WINDIR%\System32\Drivers\ /Y

regedit /s TashaSys.reg
net stop  Tasha
net start Tasha