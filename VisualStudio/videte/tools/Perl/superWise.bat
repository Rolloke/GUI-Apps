@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';
 
#
# PERL PART FOLLOWS HERE
#

# force "flush output"
$|=1;

# set some global path variables
$ZIP_CMD="v:\\bin\\tools\\zip\\pkzip25.exe";

# set some global path variables
$WISE_CMD="c:\\programme\\wise\\wise32.exe";
$DEST_DIR="d:\\dvrt";
$DEST_DIR_ENU="d:\\dvrtenu";
$WISE_DIR="d:\\installation\\wise\\dvrt";
$INSTALL_DIR="d:\\install\\idip";

$BUILD="AUTO"; # $BUILD="201"; or set to "AUTO"

if (! -f "$ZIP_CMD") {
	die "pkzip.exe not found $ZIP_CMD";
}

# no Wise no SuperWise
if (! -f "$WISE_CMD") {
	die "wise.exe not found quitting";
}

&SetTimeVariables();
print "SuperWise started at $mday.$month.$year, $hour:$min\n";

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

# read from zip
chdir("D:");
chdir("/");
`mkdir $DEST_DIR`;
chdir($DEST_DIR);
# creates bin
print "executing $ZIP_CMD -extract -recurse -directories g:\\BuildZips\\$BUILD\.zip\n";
print `$ZIP_CMD -extract -recurse -directories g:\\BuildZips\\$BUILD\.zip`;

# print "creating Convert\n";
# `$WISE_CMD /c \"$WISE_DIR\\Convert\\FAT2NTFS.wse\" `;
# print "created Convert\n";

# print "creating Export2NT\n";
# `$WISE_CMD /c \"$WISE_DIR\\Export\\Export2NT.wse\" `;
# print "created Export2NT\n";

# print "creating Import2NT\n";
# `$WISE_CMD /c \"$WISE_DIR\\Import\\Import2NT.wse\" `;
# print "created Import2NT\n";

print "creating german\n";
`$WISE_CMD /c \"$WISE_DIR\\german.wse\" `;
print "created german\n";

print "creating ProductView\n";
`$WISE_CMD /c \"$WISE_DIR\\dongle.wse\" `;
print "created ProductView\n";

print "creating MNO\n";
`$WISE_CMD /c \"$WISE_DIR\\mno.wse\" `;
print "created MNO\n";

print "creating HHLA\n";
`$WISE_CMD /c \"$WISE_DIR\\hhla.wse\" `;
print "created HHLA\n";

# read from zip
chdir("D:");
chdir("/");
`mkdir $DEST_DIR_ENU`;
chdir($DEST_DIR_ENU);
# creates bin
print "executing $ZIP_CMD -extract -recurse -directories g:\\BuildZips\\Enu$BUILD\.zip\n";
print `$ZIP_CMD -extract -recurse -directories g:\\BuildZips\\Enu$BUILD\.zip`;
chdir("D:");
chdir("/");

print "creating english\n";
`$WISE_CMD /c \"$WISE_DIR\\english.wse\" `;
print "created english\n";

print "creating idip Receiver\n";
`$WISE_CMD /c \"$WISE_DIR\\idip_receiver.wse\" `;
print "created idip Receiver\n";

print "creating Alarmcom Receiver\n";
`$WISE_CMD /c \"$WISE_DIR\\alarmcom_receiver.wse\" `;
print "created Alarmcom Receiver\n";

# print "creating Universal Receiver\n";
# `$WISE_CMD /c \"$WISE_DIR\\universal_receiver.wse\" `;
# print "created Universal Receiver\n";

# print "creating TOBS Receiver\n";
# `$WISE_CMD /c \"$WISE_DIR\\tobs_receiver.wse\" `;
# print "created TOBS Receiver\n";

#print "creating TO 3000\n";
#`$WISE_CMD /c \"$WISE_DIR\\to_3000.wse\" `;
#print "created TO 3000\n";

`copy /Y $DEST_DIR\\version.txt $INSTALL_DIR\\version.txt`;

print "adding setup to build zips\n";
chdir("$INSTALL_DIR");
`$ZIP_CMD -add -recurse -directories -silent g:\\BuildZips\\$BUILD\.zip $INSTALL_DIR\\german\\*.*`;
`$ZIP_CMD -add -recurse -directories -silent g:\\BuildZips\\Enu$BUILD\.zip $INSTALL_DIR\\english\\*.*`;

$WiseDoneText = "SuperWise done for build $BUILD";

&SetTimeVariables();
print "$WiseDoneText at $mday.$month.$year, $hour:$min:$sec\n";

$NewVersionText = "Neue idip! Version!";

@Recipients=(
			 "Uwe.Freiwald\@videte.com",
			 "Martin.Lueck\@videte.com",
			 "Tomas.Krogh\@videte.com",
			 "Georg.Feddern\@videte.com",
			 "Stefan.Behl\@videte.com",
			 "Matthias.Riedel\@videte.com",
			 "Rolf.Kary-Ehlers\@videte.com",
			 "Lars.Krackow\@videte.com",
			 "Wilfredo.Toscano\@videte.com",
			 "Tester\@videte.com",
			 "Ardeshir.Hosseini\@videte.com",
			 "Sascha.Luge\@videte.com",
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

$NewVersionText = "Neue idip Version $Version";
for (@Recipients)
{
	print "v:\\bin\\tools\\email.exe mail01 uf\@videte.com $_ \"$NewVersionText\" $DEST_DIR\\BuildNotes$BUILD.htm\n";
	print `v:\\bin\\tools\\email.exe mail01 uf\@videte.com $_ \"$NewVersionText\" $DEST_DIR\\BuildNotes$BUILD.htm`;
}

&SetTimeVariables();
print "SuperWise end $NewVersionText at $mday.$month.$year, $hour:$min:$sec\n";
###########################################################
# end of main

###########################################################
sub FindLatestBuild
{
	if ($BUILD eq "AUTO") {
		chdir("g:");
		chdir("/BuildZips");

		local(@someFiles) = `dir /b /on *.zip`;
		local($maxBuild)=100;
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
	local ($sec,$isdst,$wday,$yday);
	
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
###########################################################
__END__
:endofperl
