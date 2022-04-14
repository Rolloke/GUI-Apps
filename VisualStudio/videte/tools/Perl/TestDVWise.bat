@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

#
# PERL PART FOLLOWS HERE
#

use Cwd;

# force "flush output"
$|=1;

# set some debug variables
$bTraceDebug = 0;

# set some global command variables
$ZIP_CMD         = "v:\\bin\\tools\\zip\\pkzip25.exe";
$WISE_CMD        = "d:\\apps\\wise\\wise32.exe";
$MAKE_UPDATE_CMD = "v:\\bin\\tools\\MakeUpdateDisk.exe";
#$SAFE_CMD       = "C:\\msdev98\\common\\vss\\win32\\ss.exe";

# set some global path variables
$WISE_DIR        = "d:\\project\\dv\\install";
$BUILD_ZIP_DRIVE = "g:";
$BUILD_ZIP_DIR   = "$BUILD_ZIP_DRIVE\\BuildDts";
$DEST_DRIVE	     = "d:";
$DEST_DIR        = "$DEST_DRIVE\\Dts";
$INSTALL_DIR     = "d:\\install";

$BUILD = "AUTO";

#**********************************************************
if (! -f $WISE_CMD)
{
	$WISE_CMD = "c:\\Programme\\wise\\wise32.exe";
	if (! -f $WISE_CMD)
	{
		die "wise32.exe not found $WISE_CMD";
	}
}

if (! -f "$ZIP_CMD") {
	die "pkzip.exe not found $ZIP_CMD";
}

# no source safe no DTS Unicode Wise
#if (! -f "$SAFE_CMD") {
#	die "ss.exe not found quitting";
#}

# no Wise no DTS Unicode Wise
if (! -f "$WISE_CMD") {
	die "wise.exe not found quitting";
}

#**********************************************************
&SetTimeVariables();
print "DTS Unicode Wise started at $mday.$month.$year, $hour:$min:$sec\n";

#print "SourceSafe is $SAFE_CMD\n";
print "Wise is $WISE_CMD\n";
print "Using $DEST_DIR\n";

&FindLatestBuild();

print "latest build is $BUILD\n";

if (-d "$DEST_DIR")
{
	print `rmdir /S /Q $DEST_DIR`;
}

#**********************************************************
# change to destination dir and read from zip
if (!(-f "$BUILD_ZIP_DIR\\$BUILD\.zip"))
{
	print "$BUILD_ZIP_DIR\\$BUILD\.zip not found\n";
	die;
}
chdir($DEST_DRIVE);
chdir("/");
`mkdir $DEST_DIR`;
if (!(-d "$DEST_DIR"))
{
	print "$DEST_DIR doesn't exist\n";
	die;
}
chdir($DEST_DIR);
if ($bTraceDebug) {printf ("Changed to destination dir? %s\n",getcwd);}
print "executing $ZIP_CMD -extract -recurse -directories $BUILD_ZIP_DIR\\$BUILD\.zip\n";
print `$ZIP_CMD -extract -recurse -directories $BUILD_ZIP_DIR\\$BUILD\.zip`;

# change to destination drive, to access the SOURCE and SOURCE_BASE for Wise
chdir("/");
if ($bTraceDebug) {printf ("Changed to source base dir? %s\n",getcwd);}

if (!(-d "$DEST_DIR"))
{
	print "error $DEST_DIR not found\n";
	die;
}

print "deleting old update disks and zips\n";
`del $INSTALL_DIR\\DtsU\\Update.zip`;
`del $INSTALL_DIR\\alarmcomU\\Update.zip`;
`del $INSTALL_DIR\\santecU\\Update.zip`;

chdir("$INSTALL_DIR\\DtsU\\Update");
`del /F /S /Q $INSTALL_DIR\\DtsU\\Update\\*.*`;
chdir("$INSTALL_DIR\\AlarmcomU\\Update");
`del /F /S /Q $INSTALL_DIR\\AlarmcomU\\Update\\*.*`;
chdir("$INSTALL_DIR\\SantecU\\Update");
`del /F /S /Q $INSTALL_DIR\\SantecU\\Update\\*.*`;

chdir("$WISE_DIR");

print "creating Videte IT DTS\n";
`$WISE_CMD /c \"$WISE_DIR\\dts\\dts_transmitterU.wse\" `;
`$WISE_CMD /c \"$WISE_DIR\\dts\\dts_ip_receiverU.wse\" `;
`$WISE_CMD /c \"$WISE_DIR\\chs.wse\" `;
`$WISE_CMD /c \"$WISE_DIR\\dts\\dts_toolsU.wse\" `;
print "created Videte IT DTS\n";

print "creating AlarmCom VDR\n";
`$WISE_CMD /c \"$WISE_DIR\\vdr\\vdr_transmitterU.wse\" `;
# `$WISE_CMD /c \"$WISE_DIR\\vdr\\vdr_receiverU.wse\" `;
`$WISE_CMD /c \"$WISE_DIR\\vdr\\vdr_ip_receiverU.wse\" `;
print "created AlarmCom VDR\n";

# TODO gf
#	print "creating Protection One VDR\n";
#	`$WISE_CMD /c \"$WISE_DIR\\pone\\pone_transmitterU.wse\" `;
#	`$WISE_CMD /c \"$WISE_DIR\\pone\\pone_receiverU.wse\" `;
#	`$WISE_CMD /c \"$WISE_DIR\\pone\\pone_ip_receiverU.wse\" `;
#	print "created Protection One VDR\n";

print "creating Santec SDR\n";
`$WISE_CMD /c \"$WISE_DIR\\sdr\\sdr_transmitterU.wse\" `;
# `$WISE_CMD /c \"$WISE_DIR\\sdr\\sdr_receiverU.wse\" `;
`$WISE_CMD /c \"$WISE_DIR\\sdr\\sdr_ip_receiverU.wse\" `;
# `$WISE_CMD /c \"$WISE_DIR\\sdr\\sdr_receiver_demoU.wse\" `;
print "created Santec SDR\n";

print "adding setup to build zip\n";
chdir("$INSTALL_DIR");
`$ZIP_CMD -add -recurse -directories -silent $BUILD_ZIP_DIR\\$BUILD\.zip $INSTALL_DIR\\DtsU\\*.*`;
`$ZIP_CMD -add -recurse -directories -silent $BUILD_ZIP_DIR\\$BUILD\.zip $INSTALL_DIR\\AlarmcomU\\*.*`;
`$ZIP_CMD -add -recurse -directories -silent $BUILD_ZIP_DIR\\$BUILD\.zip $INSTALL_DIR\\SantecU\\*.*`;

print "creating update disks\n";
`$MAKE_UPDATE_CMD $INSTALL_DIR\\chs 0 $BUILD`;
`$MAKE_UPDATE_CMD $INSTALL_DIR\\dtsU 0 $BUILD`;
`$MAKE_UPDATE_CMD $INSTALL_DIR\\alarmcomU 1 $BUILD`;
`$MAKE_UPDATE_CMD $INSTALL_DIR\\santecU 3 $BUILD`;

print "creating update zips\n";
chdir("$INSTALL_DIR\\chs");
`$ZIP_CMD -add -recurse -directories -silent $INSTALL_DIR\\chs\\chs.zip $INSTALL_DIR\\chs\\Update\\*.*`;
chdir("$INSTALL_DIR\\DtsU");
`$ZIP_CMD -add -recurse -directories -silent $INSTALL_DIR\\DtsU\\Update.zip $INSTALL_DIR\\DtsU\\Update\\*.*`;
chdir("$INSTALL_DIR\\alarmcomU");
`$ZIP_CMD -add -recurse -directories -silent $INSTALL_DIR\\alarmcomU\\Update.zip $INSTALL_DIR\\alarmcomU\\Update\\*.*`;
chdir("$INSTALL_DIR\\santecU");
`$ZIP_CMD -add -recurse -directories -silent $INSTALL_DIR\\santecU\\Update.zip $INSTALL_DIR\\santecU\\Update\\*.*`;

# copying version.txt
`copy /Y $DEST_DIR\\version.txt $INSTALL_DIR\\chs\\version.txt`;
`copy /Y $DEST_DIR\\version.txt $INSTALL_DIR\\DtsU\\version.txt`;
`copy /Y $DEST_DIR\\version.txt $INSTALL_DIR\\alarmcomU\\version.txt`;
`copy /Y $DEST_DIR\\version.txt $INSTALL_DIR\\santecU\\version.txt`;

$WiseDoneText = "DTS Unicode Wise done for build $BUILD";
&SetTimeVariables();
print "$WiseDoneText at $mday.$month.$year, $hour:$min:$sec\n";

$NewVersionText = "Neue DTS Version! in $INSTALL_DIR";

@Recipients=(
			 "Uwe.Freiwald\@videte.com",
			 "Martin.Lueck\@videte.com",
			 "Tomas.Krogh\@videte.com",
			 "Stefan.Behl\@videte.com",
			 "Sascha.Mittendorff\@videte.com",
			 "Wolfgang.Schumacher\@videte.com",
			 "Rolf.Kary-Ehlers\@videte.com",
			 "Tester\@videte.com",
			 "Timo.Koch\@videte.com",
			 "Lars.Krackow\@videte.com",
			 "Goetz.Petersen\@videte.com"
			 );

$Version = "";
open (VERSION_IN_FILE,"<$DEST_DIR\\version.txt");
while (<VERSION_IN_FILE>)
{
	$Version = "$Version$_";
}
close(VERSION_IN_FILE);
$Version =~  s/\n//;
$Version =~  s/\r//;

$NewVersionText = "Neue DTS Version $Version";
for (@Recipients)
{
	print "v:\\bin\\tools\\email.exe mail01 uf\@videte.com $_ \"$NewVersionText\" $DEST_DIR\\BuildNotes$BUILD.htm\n";
	print `v:\\bin\\tools\\email.exe mail01 uf\@videte.com $_ \"$NewVersionText\" $DEST_DIR\\BuildNotes$BUILD.htm`;
}

&SetTimeVariables();
print "DTS Unicode Wise end $NewVersionText at $mday.$month.$year, $hour:$min:$sec\n";
###########################################################
# end of main

###########################################################
sub FindLatestBuild
{
	if ($BUILD eq "AUTO") {
		chdir($BUILD_ZIP_DRIVE);
		chdir($BUILD_ZIP_DIR);

		local(@someFiles) = `dir /b /on *.zip`;
		local($maxBuild)=1;
		for ($i=0;$i <= $#someFiles;$i++) {
			$oneLine=$someFiles[$i];

			$oneLine =~ s/[\n\r]//gm;
			$oneLine =~ s/\.zip//i;
			$oneLine =~ s/Build//i;

			if ($oneLine > $maxBuild) {
				$maxBuild = $oneLine;
			}
		}

		$BUILD=$maxBuild;
	}
}
###########################################################
sub SetTimeVariables
{
	local ($isdst,$wday,$yday);
	
	($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
	$month=eval($mon+1);
	$year=eval($year+1900);

	if ( $sec < 10) {
		$sec = "0$sec";
	}
	if ( $min < 10) {
		$min = "0$min";
	}
	if ( $hour < 10) {
		$hour = "0$hour";
	}
}


__END__
:endofperl
