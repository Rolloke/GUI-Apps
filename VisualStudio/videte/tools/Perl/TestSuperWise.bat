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
$ZIP_CMD  ="v:\\bin\\tools\\zip\\pkzip25.exe";
$WISE_CMD        = "c:\\programme\\wise\\wise32.exe";

# set some global path variables
$WISE_DIR        = "d:\\installation\\wise\\dvrt";
$BUILD_ZIP_DRIVE = "g:";
$BUILD_ZIP_DIR   = "$BUILD_ZIP_DRIVE\\Buildidip";
$DEST_DRIVE	     = "d:";
$DEST_DIR        = "$DEST_DRIVE\\idip";
$DEST_DIR_ENU    = "$DEST_DRIVE\\idipEnu";
$INSTALL_DIR     = "d:\\install\\idipU";

$BUILD="AUTO"; # $BUILD="201"; or set to "AUTO"

#**********************************************************
if (! -f "$ZIP_CMD") {
	die "pkzip.exe not found $ZIP_CMD";
}

# no Wise no idip Unicode Wise
if (! -f "$WISE_CMD") {
	die "wise.exe not found quitting";
}

#**********************************************************
&SetTimeVariables();
print "idip Unicode Wise started at $mday.$month.$year, $hour:$min\n";

print "Wise is $WISE_CMD\n";
print "Using $DEST_DIR\n";

&FindLatestBuild();

print "lastest build is $BUILD\n";

if (-d "$DEST_DIR")
{
	print `rmdir /S /Q $DEST_DIR`;
}

if (-d "$DEST_DIR_ENU")
{
	print `rmdir /S /Q $DEST_DIR_ENU`;
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

print "creating german\n";
`$WISE_CMD /c \"$WISE_DIR\\germanU.wse\" `;
print "created german\n";

print "creating Dongle\n";
`$WISE_CMD /c \"$WISE_DIR\\dongleU.wse\" `;
print "created Dongle\n";


# TODO gf
#	print "creating MNO\n";
#	`$WISE_CMD /c \"$WISE_DIR\\mno.wse\" `;
#	print "created MNO\n";

# TODO gf
#	print "creating HHLA\n";
#	`$WISE_CMD /c \"$WISE_DIR\\hhla.wse\" `;
#	print "created HHLA\n";

# read from zip
chdir($DEST_DRIVE);
chdir("/");
`mkdir $DEST_DIR_ENU`;
chdir($DEST_DIR_ENU);
# creates bin
print "executing $ZIP_CMD -extract -recurse -directories $BUILD_ZIP_DIR\\Enu$BUILD\.zip\n";
print `$ZIP_CMD -extract -recurse -directories $BUILD_ZIP_DIR\\Enu$BUILD\.zip`;
chdir($DEST_DRIVE);
chdir("/");

print "creating english\n";
`$WISE_CMD /c \"$WISE_DIR\\englishU.wse\" `;
print "created english\n";

print "creating idip receiver\n";
`$WISE_CMD /c \"$WISE_DIR\\idip_receiveru.wse\" `;
print "created idip receiver\n";

print "creating alarmcom receiver\n";
`$WISE_CMD /c \"$WISE_DIR\\alarmcom_receiveru.wse\" `;
print "created alarmcom receiver\n";

print "creating demo Receiver\n";
`$WISE_CMD /c \"$WISE_DIR\\idip_receiver_demou.wse\" `;
print "created demo Receiver\n";

# print "creating TOBS Receiver\n";
# `$WISE_CMD /c \"$WISE_DIR\\tobs_receiver.wse\" `;
# print "created TOBS Receiver\n";

#print "creating TO 3000\n";
#`$WISE_CMD /c \"$WISE_DIR\\to_3000.wse\" `;
#print "created TO 3000\n";

`copy /Y $DEST_DIR\\version.txt $INSTALL_DIR\\version.txt`;

print "adding setup to build zips\n";
chdir("$INSTALL_DIR");
`$ZIP_CMD -add -recurse -directories -silent $BUILD_ZIP_DIR\\$BUILD\.zip $INSTALL_DIR\\german\\*.*`;
`$ZIP_CMD -add -recurse -directories -silent $BUILD_ZIP_DIR\\Enu$BUILD\.zip $INSTALL_DIR\\english\\*.*`;

$WiseDoneText = "idip Unicode Wise done for build $BUILD";

&SetTimeVariables();
print "$WiseDoneText at $mday.$month.$year, $hour:$min:$sec\n";

$NewVersionText = "Neue idip! Version! in $INSTALL_DIR";

# TODO gf
@Recipients=(
			 "Uwe.Freiwald\@videte.com",
			 "Martin.Lueck\@videte.com",
			 "Tomas.Krogh\@videte.com",
			 "Wolfgang.Schumacher\@videte.com",
			 "Stefan.Behl\@videte.com",
			 "Sascha.Mittendorff\@videte.com",
			 "Rolf.Kary-Ehlers\@videte.com",
			 "Timo.Koch\@videte.com",
			 "Lars.Krackow\@videte.com",
			 "Tester\@videte.com",
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

$NewVersionText = "Neue idip Version $Version";
for (@Recipients)
{
	print "v:\\bin\\tools\\email.exe mail01 uf\@videte.com $_ \"$NewVersionText\" $DEST_DIR\\BuildNotes$BUILD.htm\n";
	print `v:\\bin\\tools\\email.exe mail01 uf\@videte.com $_ \"$NewVersionText\" $DEST_DIR\\BuildNotes$BUILD.htm`;
}

&SetTimeVariables();
print "idip Unicode Wise end $NewVersionText at $mday.$month.$year, $hour:$min:$sec\n";
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
###########################################################
__END__
:endofperl
