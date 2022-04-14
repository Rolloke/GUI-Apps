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

# set some global path variables
$ZIP_CMD="v:\\bin\\tools\\zip\\pkzip25.exe";
$MAKE_UPDATE_CMD="v:\\bin\\tools\\MakeUpdateDisk.exe";

# set some global path variables
#$SAFE_CMD="C:\\msdev98\\common\\vss\\win32\\ss.exe";
$WISE_CMD="d:\\apps\\wise\\wise32.exe";
$INSTALL_DIR="d:\\install\\DTS\\Unicode";
$DEST_DIR="d:\\dvx";
$WISE_DIR="d:\\project\\dv\\install";

# CAVEAT: At any changes: The BuildZipDir in FindLatestBuild() must match $BuildZipDir 
$BuildZipDir="g:\\BuildDVU";
$BuildZipPrefix="Unicode";
$BUILD = "AUTO";

if (! -f $WISE_CMD)
{
	$WISE_CMD = "c:\\Programme\\wise\\wise32.exe"
}

if (! -f "$ZIP_CMD") {
	die "pkzip.exe not found $ZIP_CMD";
}

# no source safe no DVWise
#if (! -f "$SAFE_CMD") {
#	die "ss.exe not found quitting";
#}

# no Wise no DVWise
if (! -f "$WISE_CMD") {
	die "wise.exe not found quitting";
}

&SetTimeVariables();
print "DVUnicodeWise started at $mday.$month.$year, $hour:$min:$sec\n";

#print "SourceSafe is $SAFE_CMD\n";
print "Wise is $WISE_CMD\n";
print "Using $DEST_DIR\n";

&FindLatestBuild();

print "latest build is $BUILD\n";

if (-d "$DEST_DIR")
{
	print "remove dir $DEST_DIR\n";
	print `rmdir /S /Q $DEST_DIR`;
}

# read from zip
chdir("D:");
chdir("/");

if (!(-f "$BuildZipDir\\$BuildZipPrefix$BUILD\.zip"))
{
	print "$BuildZipDir\\$BuildZipPrefix$BUILD\.zip not found\n";
	die;
}

`mkdir $DEST_DIR`;
if (!(-d "$DEST_DIR"))
{
	print "$DEST_DIR doesn't exist\n";
	die;
}
chdir($DEST_DIR);
# creates bin
#printf ("current dir %s\n", getcwd);
print "executing $ZIP_CMD -extract -recurse -directories $BuildZipDir\\$BuildZipPrefix$BUILD\.zip\n";
print `$ZIP_CMD -extract -recurse -directories $BuildZipDir\\$BuildZipPrefix$BUILD\.zip`;
chdir("D:");
chdir("/");

if (!(-d "$DEST_DIR"))
{
	print "error $DEST_DIR not found\n";
	die;
}

#//////////////////////////////////////////////////////////
print "deleting old update disks and zips\n";
`del $INSTALL_DIR\\DTS\\Update.zip`;
#`del $INSTALL_DIR\\alarmcom\\Update.zip`;
#`del $INSTALL_DIR\\dresearch\\Update.zip`;
#`del $INSTALL_DIR\\santec\\Update.zip`;

chdir("$INSTALL_DIR\\DTS\\Update");
printf ("current dir should be $INSTALL_DIR\\DTS\\Update ; is %s\n", getcwd);
printf ("deleting all files underneath $INSTALL_DIR\\DTS\\Update\\ \n");
`del /F /S /Q $INSTALL_DIR\\DTS\\Update\\*.*`;

#chdir("$INSTALL_DIR\\Alarmcom\\Update");
#printf ("deleting all files underneath $INSTALL_DIR\\Alarmcom\\Update\\ \n");
#`del /F /S /Q $INSTALL_DIR\\Alarmcom\\Update\\*.*`;
#chdir("$INSTALL_DIR\\Santec\\Update");
#printf ("deleting all files underneath $INSTALL_DIR\\Santec\\Update\\ \n");
#`del /F /S /Q $INSTALL_DIR\\Santec\\Update\\*.*`;
#chdir("$INSTALL_DIR\\DResearch\\Update");
#printf ("deleting all files underneath $INSTALL_DIR\\DResearch\\Update\\ \n");
#`del /F /S /Q $INSTALL_DIR\\DResearch\\Update\\*.*`;

#//////////////////////////////////////////////////////////
chdir("$WISE_DIR");
print "creating Videte IT DTS\n";
`$WISE_CMD /c \"$WISE_DIR\\dts\\dts_transmitter_unicode.wse\" `;
#`$WISE_CMD /c \"$WISE_DIR\\dts\\dts_receiver_unicode.wse\" `;
`$WISE_CMD /c \"$WISE_DIR\\dts\\dts_ip_receiver_unicode.wse\" `;
#`$WISE_CMD /c \"$WISE_DIR\\dts\\dts_tools_unicode.wse\" `;
print "created Videte IT DTS\n";

#print "creating AlarmCom VDR\n";
#`$WISE_CMD /c \"$WISE_DIR\\vdr\\vdr_transmitter.wse\" `;
#`$WISE_CMD /c \"$WISE_DIR\\vdr\\vdr_receiver.wse\" `;
#`$WISE_CMD /c \"$WISE_DIR\\vdr\\vdr_ip_receiver.wse\" `;
#print "created AlarmCom VDR\n";

#print "creating Protection One VDR\n";
#`$WISE_CMD /c \"$WISE_DIR\\pone\\pone_transmitter.wse\" `;
#`$WISE_CMD /c \"$WISE_DIR\\pone\\pone_receiver.wse\" `;
#`$WISE_CMD /c \"$WISE_DIR\\pone\\pone_ip_receiver.wse\" `;
#print "created Protection One VDR\n";

#print "creating DResearch Vidia 500\n";
#`$WISE_CMD /c \"$WISE_DIR\\vidia\\vidia_transmitter.wse\" `;
#`$WISE_CMD /c \"$WISE_DIR\\vidia\\vidia_receiver.wse\" `;
#`$WISE_CMD /c \"$WISE_DIR\\vidia\\vidia_ip_receiver.wse\" `;
#print "created DResearch Vidia 500\n";

#print "creating Santec SDR\n";
#`$WISE_CMD /c \"$WISE_DIR\\sdr\\sdr_transmitter.wse\" `;
#`$WISE_CMD /c \"$WISE_DIR\\sdr\\sdr_receiver.wse\" `;
#`$WISE_CMD /c \"$WISE_DIR\\sdr\\sdr_ip_receiver.wse\" `;
#`$WISE_CMD /c \"$WISE_DIR\\sdr\\sdr_receiver_demo.wse\" `;
#print "created Santec SDR\n";

print "adding setup to build zip\n";
chdir("$INSTALL_DIR");
`$ZIP_CMD -add -recurse -directories -silent $BuildZipDir\\$BuildZipPrefix$BUILD\.zip $INSTALL_DIR\\dts\\*.*`;
#`$ZIP_CMD -add -recurse -directories -silent $BuildZipDir\\$BuildZipPrefix$BUILD\.zip $INSTALL_DIR\\Alarmcom\\*.*`;
#`$ZIP_CMD -add -recurse -directories -silent $BuildZipDir\\$BuildZipPrefix$BUILD\.zip $INSTALL_DIR\\Dresearch\\*.*`;
#`$ZIP_CMD -add -recurse -directories -silent $BuildZipDir\\$BuildZipPrefix$BUILD\.zip $INSTALL_DIR\\Santec\\*.*`;
#`$ZIP_CMD -add -recurse -directories -silent $BuildZipDir\\$BuildZipPrefix$BUILD\.zip $INSTALL_DIR\\Pone\\*.*`;

print "creating update disks\n";
`$MAKE_UPDATE_CMD $INSTALL_DIR\\dts 0 $BUILD`;
#`$MAKE_UPDATE_CMD $INSTALL_DIR\\alarmcom 1 $BUILD`;
#`$MAKE_UPDATE_CMD $INSTALL_DIR\\pone 4 $BUILD`;
#`$MAKE_UPDATE_CMD $INSTALL_DIR\\santec 3 $BUILD`;
#`$MAKE_UPDATE_CMD $INSTALL_DIR\\dresearch 2 $BUILD`;

print "creating update zips\n";
chdir("$INSTALL_DIR\\DTS");
`$ZIP_CMD -add -recurse -directories -silent $INSTALL_DIR\\DTS\\Update.zip $INSTALL_DIR\\DTS\\Update\\*.*`;
#chdir("$INSTALL_DIR\\alarmcom");
#`$ZIP_CMD -add -recurse -directories -silent $INSTALL_DIR\\alarmcom\\Update.zip $INSTALL_DIR\\alarmcom\\Update\\*.*`;
#chdir("$INSTALL_DIR\\pone");
#`$ZIP_CMD -add -recurse -directories -silent $INSTALL_DIR\\pone\\Update.zip $INSTALL_DIR\\pone\\Update\\*.*`;
#chdir("$INSTALL_DIR\\dresearch");
#`$ZIP_CMD -add -recurse -directories -silent $INSTALL_DIR\\dresearch\\Update.zip $INSTALL_DIR\\dresearch\\Update\\*.*`;
#chdir("$INSTALL_DIR\\santec");
#`$ZIP_CMD -add -recurse -directories -silent $INSTALL_DIR\\santec\\Update.zip $INSTALL_DIR\\santec\\Update\\*.*`;

# copying version.txt
`copy /Y $DEST_DIR\\version.txt $INSTALL_DIR\\version.txt`;

$WiseDoneText = "DVUnicodeWise done for build $BUILD";
&SetTimeVariables();
print "$WiseDoneText at $mday.$month.$year, $hour:$min:$sec\n";

$NewVersionText = "Neue DVUnicode Version!";

@Recipients=(
			 "Uwe.Freiwald\@videte.com",
			 "Martin.Lueck\@videte.com",
			 "Tomas.Krogh\@videte.com",
			 "Georg.Feddern\@videte.com",
			 "Stefan.Behl\@videte.com",
#			 "Matthias.Riedel\@videte.com",
			 "Rolf.Kary-Ehlers\@videte.com",
			 "Tester\@videte.com",
			 "Timo.Koch\@videte.com",
			 "Wilfredo.Toscano\@videte.com",
			 "Lars.Krackow\@videte.com",
			 "Ardeshir.Hosseini\@videte.com",
			 "Andreas.Hagen\@videte.com"
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

$NewVersionText = "Neue DTS Unicode Version $Version";
for (@Recipients)
{
	print "v:\\bin\\tools\\email.exe mail01 uf\@videte.com $_ \"$NewVersionText\" $DEST_DIR\\BuildNotes$BUILD.htm\n";
	print `v:\\bin\\tools\\email.exe mail01 uf\@videte.com $_ \"$NewVersionText\" $DEST_DIR\\BuildNotes$BUILD.htm`;
}

&SetTimeVariables();
print "DVUnicodeWise end $NewVersionText at $mday.$month.$year, $hour:$min:$sec\n";

###########################################################
# end of main


###########################################################
sub FindLatestBuild
{
	if ($BUILD eq "AUTO")
	{
# CAVEAT: The BuildZipDir must match $BuildZipDir 
		chdir("g:");
		chdir("/BuildDVU");

		local(@someFiles) = `dir /b /on *.zip`;
		local($maxBuild)=1;
		for ($i=0;$i <= $#someFiles;$i++)
		{
			$oneLine=$someFiles[$i];

#			print "Unicode build zips: $oneLine\n";
			$oneLine =~ s/[\n\r]//gm;
			$oneLine =~ s/\.zip//i;
			$oneLine =~ s/Unicode//i;

#			print "Unicode build no.: $oneLine > $maxBuild?\n";
			if ($oneLine > $maxBuild)
			{
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

	if ( $min < 10) {
		$min = "0$min";
	}
	if ( $hour < 10) {
		$hour = "0$hour";
	}
}

__END__
:endofperl
