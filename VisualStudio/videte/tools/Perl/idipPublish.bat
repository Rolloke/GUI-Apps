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
$DEST_DIR="p:\\programme\\intern\\idip";
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
print "idip Publish started at $mday.$month.$year, $hour:$min\n";

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

&DoTransmitter("idip");
&DoReceiver("idip","idip","inspicio","W:\\Dokumentation\\Handbücher_videte");
&DoReceiver("idip","idip","inspicio_demo","W:\\Dokumentation\\Handbücher_videte");
# &DoReceiver("idip","idip","siemens_receiver","W:\\Dokumentation\\Handbücher_videte");

print "missing $COPY_ERRORS files\n";
foreach $i (@MISSING_FILES) { print "$i\n" }  

$DoneText = "idip Publish done for Version $VERSION";
$NewVersionText = "Neue idip Version auf p!";
print "$DoneText\n";

sub ReadVersion
{
	open (LABEL_IN_FILE,"<$INSTALL_DIR\\idipu\\version.txt");
	while (<LABEL_IN_FILE>)
	{
		$VERSION = "$VERSION$_";
	}
	close(LABEL_IN_FILE);
	$VERSION =~  s/\n//;
	$VERSION =~  s/\r//;

	$a = split(/\./,$VERSION);     # Anzahl der Elemente
	@b = split(/\./,$VERSION);     # Array der einzelnen Teilstrings

	$VERSION_SHORT = "$b[0]\.$b[1]\.$b[2]";

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

sub DoTransmitter
{
	my($OEM_LONG)=@_;
#---------------------------------------------------------------------------
# publish the binaries to p:
#---------------------------------------------------------------------------
	&MyCreateDir($DEST_DIR);
	if (! -d $DEST_DIR)
	{
		`md $DEST_DIR`;
	}
	$sourcedir = "$INSTALL_DIR\\$OEM_LONG";
	$sourcedir .= "U";
#	&MyXCopy("xcopy $sourcedir $DEST_DIR /s /r /q");
	&MyCopy("$sourcedir\\german\\dongle.exe","$DEST_DIR");
	&MyCopy("$sourcedir\\english\\setup.*","$DEST_DIR");

	# create the compresses transmitter software
	$zip_file = "$DEST_DIR\\idip";
	$zip_file .= "_";
	$zip_file .= "$VERSION.zip";
	print `$ZIP_CMD -add -silent -store $zip_file $sourcedir\\english\\setup.*`;
}

sub DoReceiver
{
	my($OEM_LONG,$OEM_SHORT,$RECEIVER_NAME,$MANUAL_SOURCE_DIR)=@_;
	print "do $OEM_LONG, $OEM_SHORT, $RECEIVER_NAME, $MANUAL_SOURCE_DIR\n";

	$DEST_DIR_OEM = "$DEST_DIR";


#---------------------------------------------------------------------------
# create the idip receiver cd's
#---------------------------------------------------------------------------
	$RECEIVER_DIR = "$DEST_DIR_OEM\\$RECEIVER_NAME";
	&MyCreateDir($RECEIVER_DIR);

	# copy the real receiver software
	$sourcedir = "$INSTALL_DIR\\$OEM_LONG";
	$sourcedir .= "U";
	$temp = "$RECEIVER_NAME";
	$temp .= ".exe";
	$source = $sourcedir;
	$source .= "\\german\\";
	$source .= $temp;
	&MyCopy($source,"$RECEIVER_DIR");
	
	# create the compresses receiver software
	$zip_file = "$DEST_DIR_OEM\\$RECEIVER_NAME";
	$zip_file .= "_";
	$zip_file .= "$VERSION.zip";
	print `$ZIP_CMD -add -silent -store $zip_file $source`;


	# copy the autorun.inf file
	$temp = "$RECEIVER_NAME";
	&MyCopy("P:\\Programme\\Intern\\DTS_Receiver\\$temp\\autorun.inf","$RECEIVER_DIR");

	# copy the german manual
	$temp = "$OEM_SHORT";
	$temp .= "\\version_";
	$temp .= "$VERSION_SHORT\\PDF\\de\\$OEM_SHORT";
	$temp .= "_Handbuch_";
	$temp .= "$VERSION_SHORT";
	$temp .= "_de.pdf";
	&MyCopy("$MANUAL_SOURCE_DIR\\$temp","$RECEIVER_DIR\\manual_deu.pdf");

	# copy the english manual
	$temp = "$OEM_SHORT";
	$temp .= "\\version_";
	$temp .= "$VERSION_SHORT\\PDF\\eng\\$OEM_SHORT";
	$temp .= "_Manual_";
	$temp .= "$VERSION_SHORT";
	$temp .= "_eng.pdf";
	&MyCopy("$MANUAL_SOURCE_DIR\\$temp","$RECEIVER_DIR\\manual_enu.pdf");
	
	# copy the common files like cdstart and acrobat
	&CopyCommonFiles($RECEIVER_DIR);
	&MyCopy("$sourcedir\\version.txt","$RECEIVER_DIR\\version.txt");
	
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

	$_= $source;
	if (! /\*/)	
	{
		if (! -f $source)
		{
			die "cannot find file $source\n";
		}
	}
	
	$copycmd = "copy $source $dest /Y";
	print "$copycmd\n";
	$errorLog = `$copycmd`;
	print "$errorLog\n";
	$_= $errorLog;
	if (!(/Datei(en) kopiert/))
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
