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
$DEST_DIR="p:\\programme\\intern\\dts";
$INSTALL_DIR="\\\\make\\install";
$VERSION = "";
$VERSION_SHORT = "";
$CD_ROOT = "P:\\Programme\\Intern\\DTS_Receiver\\root15";
$ACROBAT = "P:\\Programme\\Intern\\DTS_Receiver\\Acrobat";
$COPY_ERRORS = 0;
@MISSING_FILES = {};

if (! -f "$ZIP_CMD") {
	die "pkzip.exe not found $ZIP_CMD";
}

# no Wise no SuperPublish

&SetTimeVariables();
print "DTS Publish started at $mday.$month.$year, $hour:$min\n";

print "Source is $INSTALL_DIR\n";
print "Using $DEST_DIR\n";

&ReadVersion();

if (!(-d "$DEST_DIR"))
{
	`md $DEST_DIR`;
}
$DEST_DIR = "$DEST_DIR\\$VERSION";
if (-d $DEST_DIR)
{
	print `rmdir /S /Q $DEST_DIR`;
}
&MyCreateDir($DEST_DIR);

# &DoOneOem("Santec","SDR","SDR_SOFT_LAN","W:\\Dokumentation\\Handbücher_SANYO");
# &DoOneOem("Alarmcom","VDR","VDR_NetView","W:\\Dokumentation\\Handbücher_Alarmcom");
&DoOneOem("dts","DTS","DTS_NET_RECEIVER","W:\\Dokumentation\\Handbücher_videte");
#&MyXCopy("$INSTALL_DIR\\chs",$DEST_DIR);

print "missing $COPY_ERRORS files\n";
foreach $i (@MISSING_FILES) { print "$i\n" }  

$DoneText = "SuperPublish done for Version $VERSION";
$NewVersionText = "Neue DTS/VDR/SDR Version auf p!";
print "$DoneText\n";

sub ReadVersion
{
	open (LABEL_IN_FILE,"<$INSTALL_DIR\\dtsu\\version.txt");
	while (<LABEL_IN_FILE>)
	{
		$VERSION = "$VERSION$_";
	}
	close(LABEL_IN_FILE);
	$VERSION =~  s/\n//;
	$VERSION =~  s/\r//;

	$a = split(/\./,$VERSION);     # Anzahl der Elemente
	@b = split(/\./,$VERSION);     # Array der einzelnen Teilstrings

	$VERSION_SHORT = "$b[0]\.$b[1]";

	print "current version is $VERSION\n";
	print "short version is $VERSION_SHORT\n";
}

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

sub DoOneOem
{
	my($OEM_LONG,$OEM_SHORT,$IP_RECEIVER_NAME,$MANUAL_SOURCE_DIR)=@_;
	print "do $OEM_LONG, $OEM_SHORT, $IP_RECEIVER_NAME, $MANUAL_SOURCE_DIR\n";

	$DEST_DIR_OEM = "$DEST_DIR\\$OEM_LONG";

#---------------------------------------------------------------------------
# publish the binaries to p:
#---------------------------------------------------------------------------
	&MyCreateDir($DEST_DIR_OEM);
	if (! -d $DEST_DIR_OEM)
	{
		`md $DEST_DIR_OEM`;
	}
	$sourcedir = "$INSTALL_DIR\\$OEM_LONG";
	$sourcedir .= "U";
	&MyXCopy("xcopy $sourcedir $DEST_DIR_OEM /s /r /q");
	$dest = "$DEST_DIR_OEM\\$OEM_SHORT";
	$dest .= "_update_";
	$dest .= $VERSION;
	$dest .= ".zip";
	&MyCopy("$DEST_DIR_OEM\\update.zip",$dest);

#---------------------------------------------------------------------------
# create the ip receiver cd's
#---------------------------------------------------------------------------
	$IP_RECEIVER_DIR = "$DEST_DIR_OEM\\$IP_RECEIVER_NAME";
	&MyCreateDir($IP_RECEIVER_DIR);

	# copy the real receiver software
	$temp = "$OEM_SHORT";
	$temp .= "_ip_receiver.exe";
	&MyCopy("$DEST_DIR_OEM\\$temp","$IP_RECEIVER_DIR");


	# copy the autorun.inf file
	$temp = "$IP_RECEIVER_NAME";
	&MyCopy("P:\\Programme\\Intern\\DTS_Receiver\\$temp\\autorun.inf","$IP_RECEIVER_DIR");

	# copy the german receiver manual
	$temp = "$OEM_SHORT";
	$temp .= "_IP-Receiver\\$VERSION_SHORT\\PDF\\de\\$IP_RECEIVER_NAME";
	$temp .= "_";
	$temp .= "$VERSION_SHORT";
	$temp .= "_de.pdf";
	&MyCopy("$MANUAL_SOURCE_DIR\\$temp","$IP_RECEIVER_DIR\\receiver_deu.pdf");

	# copy the english receiver manual
	$temp = "$OEM_SHORT";
	$temp .= "_IP-Receiver\\$VERSION_SHORT\\PDF\\eng\\$IP_RECEIVER_NAME";
	$temp .= "_";
	$temp .= "$VERSION_SHORT";
	$temp .= "_eng.pdf";
	&MyCopy("$MANUAL_SOURCE_DIR\\$temp","$IP_RECEIVER_DIR\\receiver_enu.pdf");
	
	# copy the common files like cdstart and acrobat
	&CopyCommonFiles($IP_RECEIVER_DIR);
	&MyCopy("$sourcedir\\version.txt","$DEST_CD_ROOT\\version.txt");

	# copy the german transmitter manual
	$temp = "$OEM_SHORT";
	$temp .= "\\$VERSION_SHORT\\PDF\\de\\";
	$temp .= "$OEM_SHORT";
	$temp .= "_Handbuch_";
	$temp .= "$VERSION_SHORT";
	$temp .= "_de.pdf";
	&MyCopy("$MANUAL_SOURCE_DIR\\$temp","$IP_RECEIVER_DIR\\unit_deu.pdf");

	# copy the english transmitter manual
	$temp = "$OEM_SHORT";
	$temp .= "\\$VERSION_SHORT\\PDF\\eng\\";
	$temp .= "$OEM_SHORT";
	$temp .= "_Manual_";
	$temp .= "$VERSION_SHORT";
	$temp .= "_eng.pdf";
	&MyCopy("$MANUAL_SOURCE_DIR\\$temp","$IP_RECEIVER_DIR\\unit_enu.pdf");
}

sub MyXCopy
{
	my($xcopycmd) = @_;
	print "-$xcopycmd-\n";
	$errorLog = `$xcopycmd`;
	print "$errorLog\n";
	$_= $errorLog;
}

sub MyCopy
{
	my($source,$dest) = @_;
	
	if (! -f $source)
	{
		die "cannot find file $source\n";
	}
	
	$copycmd = "copy $source $dest /Y";
#	print "$copycmd\n";
	$errorLog = `$copycmd`;
	$_= $errorLog;
	if (!(/1 Datei/))
	{
		$COPY_ERRORS++;
		push(@MISSING_FILES,$source);
		print "$source $_";
	}
}

sub MyCreateDir
{
	my($mydir) = @_;
	if (!-d $mydir)
	{
		`md $mydir`;
	}

}

sub CopyCommonFiles
{
	my($DEST_CD_ROOT)=@_;

	# copy the cd root dir files
	&MyXCopy("xcopy $CD_ROOT $DEST_CD_ROOT /s /r /q");

	# copy the acrobat files
	&MyCreateDir("$DEST_CD_ROOT\\Acrobat");
	&MyXCopy("xcopy $ACROBAT $DEST_CD_ROOT\\Acrobat /s /r /q");
}

__END__
:endofperl
