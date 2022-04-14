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
$ZIP_CMD  = "v:\\bin\\tools\\zip\\pkzip25.exe";
$WISE_CMD = "c:\\programme\\wise\\wise32.exe";
$DOCOMATIC2 = "C:\\Programme\\Doc-O-Matic 2\\dmcc.exe";

# set some global path variables
$WISE_DIR = "d:\\installation\\wise\\dvrt";
$DEST_DIR ="p:\\programme\\intern\\sdk";
$SOURCE_DIR="d:\\project";
$TEMP_DIR="d:\\sdk";
$INSTALL_DIR="\\\\make\\install";
$DLL_DIR = "d:\\idip\\dlls";
$VERSION = "";
$VERSION_SHORT = "";
$CD_ROOT = "P:\\Programme\\Intern\\DTS_Receiver\\root15";
$ACROBAT = "P:\\Programme\\Intern\\DTS_Receiver\\Acrobat";
$COPY_ERRORS = 0;
@MISSING_FILES = {};

if (! -f "$ZIP_CMD") {
	die "pkzip.exe not found $ZIP_CMD";
}

&SetTimeVariables();
print "sdk Publish started at $mday.$month.$year, $hour:$min\n";

print "Source is $SOURCE_DIR\n";
print "Temp is $TEMP_DIR\n";
print "ZIP is $ZIP_CMD\n";

&ReadVersion();

$DEST_DIR = "$DEST_DIR\\$VERSION";
print "Using $DEST_DIR\n";
if (-d $DEST_DIR)
{
	print `rmdir /S /Q $DEST_DIR`;
}
&MyCreateDir($DEST_DIR);


$TEMP_DIR = "$TEMP_DIR\\$VERSION";
if (-d $TEMP_DIR)
{
	print `rmdir /S /Q $TEMP_DIR`;
}
&MyCreateDir($TEMP_DIR);

&MyCopy("$INSTALL_DIR\\idipu\\version.txt",$TEMP_DIR);
&DoInclude();
&DoLib();
&DoDLL();
&DoHelp();
&DoSamples();
&CreateZip();
&CreateWise();

print "missing $COPY_ERRORS files\n";
foreach $i (@MISSING_FILES) { print "$i\n" }  

$DoneText = "sdk Publish done for Version $VERSION";
$NewVersionText = "Neues SDK auf p!";
print "$DoneText\n";

sub DoInclude
{
	$INCLUDE_DIR = $TEMP_DIR."\\include";
	&MyCreateDir($INCLUDE_DIR);
	
	&MyCopy($SOURCE_DIR."\\include\\*.h",$INCLUDE_DIR);
	&MyCopy($SOURCE_DIR."\\cipc\\*.h;",$INCLUDE_DIR);
	&MyCopy($SOURCE_DIR."\\cipc\\*.inl;",$INCLUDE_DIR);

	&MyCreateDir($INCLUDE_DIR."\\wkclasses");
	&MyCopy($SOURCE_DIR."\\lib\\wkclasses\\*.h",$INCLUDE_DIR."\\wkclasses");
	&MyCopy($SOURCE_DIR."\\lib\\wkclasses\\*.inl",$INCLUDE_DIR."\\wkclasses");

	&MyCreateDir($INCLUDE_DIR."\\vimage");
	&MyCopy($SOURCE_DIR."\\lib\\vimage\\*.h",$INCLUDE_DIR."\\vimage");
	&MyCopy($SOURCE_DIR."\\lib\\vimage\\*.inl",$INCLUDE_DIR."\\vimage");
	
	&MyCreateDir($INCLUDE_DIR."\\nfc");
	&MyCopy($SOURCE_DIR."\\lib\\nfc\\*.h",$INCLUDE_DIR."\\nfc");
	&MyCopy($SOURCE_DIR."\\lib\\nfc\\*.inl",$INCLUDE_DIR."\\nfc");
	
	`del /Q /S "$INCLUDE_DIR\\std*.h"`;
}
sub DoLib
{
	$LIB_DIR = $TEMP_DIR."\\lib";
	&MyCreateDir($LIB_DIR);
	&MyCopy($SOURCE_DIR."\\lib\\vimage*.lib",$LIB_DIR);
	&MyCopy($SOURCE_DIR."\\lib\\nfc*.lib",$LIB_DIR);
	&MyCopy($SOURCE_DIR."\\lib\\wkclasses*.lib",$LIB_DIR);
	&MyCopy($SOURCE_DIR."\\cipc\\release\\cipc.lib",$LIB_DIR);
	&MyCopy($SOURCE_DIR."\\cipc\\releaseunicode\\cipcu.lib",$LIB_DIR);
}
sub DoDLL
{
	$BIN_DIR = $TEMP_DIR."\\bin";
	&MyCreateDir($BIN_DIR);
	&MyCopy($SOURCE_DIR."\\lib\\vimage*.dll",$BIN_DIR);
	&MyCopy($SOURCE_DIR."\\lib\\nfc*.dll",$BIN_DIR);
	&MyCopy($SOURCE_DIR."\\cipc\\release\\cipc.dll",$BIN_DIR);
	&MyCopy($SOURCE_DIR."\\cipc\\releaseunicode\\cipcu.dll",$BIN_DIR);

	&MyCopy($DLL_DIR."\\ijl15.dll",$BIN_DIR);
	&MyCopy($DLL_DIR."\\mp4spvd.dll",$BIN_DIR);
}

sub DoHelp
{
	$HELP_DIR = $TEMP_DIR."\\help";
	&MyCreateDir($HELP_DIR);
	&DoOneHelp("cipc");
	&DoOneHelp("lib/nfc");
	&DoOneHelp("lib/wkclasses");
	&DoOneHelp("lib/vimage");
}

sub DoOneHelp
{
	my($project) = @_;
	$ProjectOnly = $project;
	$ProjectOnly =~ s/^lib\///;	# drop leading Lib/
	
	$ProjectPath = $SOURCE_DIR."\\".$project;
	chdir($ProjectPath);
	
	$cmd = "\"$DOCOMATIC2\" -all -noshow $ProjectOnly.dox";
	print "$cmd\n";
	print `$cmd`;
	&MyCopy("help\\$ProjectOnly.chm",$HELP_DIR);
	&MyCopy("help\\$ProjectOnly.chi",$HELP_DIR);
}

sub DoSamples
{
	$SAMPLES_DIR = $TEMP_DIR."\\samples";
	&MyCreateDir($SAMPLES_DIR);
	
	$source = $SOURCE_DIR."\\sdk\\sample";
	$dest = $SAMPLES_DIR."\\sample";
	&CopyProject($source,$dest);
}

sub CopyProject
{
	my($source,$dest) = @_;
	&MyCreateDir($dest);
	&MyXCopy("xcopy $source\\*.h $dest /s");
	&MyXCopy("xcopy $source\\*.cpp $dest /s");
	&MyXCopy("xcopy $source\\*.rc? $dest /s");
	&MyXCopy("xcopy $source\\*.ico $dest /s");
	&MyXCopy("xcopy $source\\*.bmp $dest /s");
	&MyXCopy("xcopy $source\\*.rc? $dest /s");
	&MyXCopy("xcopy $source\\*.dsp $dest /s");
	&MyXCopy("xcopy $source\\*.vcproj $dest /s");
	&MyXCopy("xcopy $source\\*.exe $dest /s");
}

sub CreateZip
{
	$zip_file = "D:\\sdk\\idip_sdk.zip";
	chdir($TEMP_DIR);
	print `$ZIP_CMD -add -recurse -silent -directories $zip_file *.*`;
}

sub CreateWise
{
	`$WISE_CMD /c \"$WISE_DIR\\idip_sdk.wse\"`;
	&MyCopy("d:\\sdk\\idip_sdk.exe",$DEST_DIR);
}

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

	if (! ($source =~ /\*/))	
	{
		if (! -f $source)
		{
			die "cannot find file $source\n";
		}
	}
	
	$copycmd = "copy $source $dest /Y";
#	print "$copycmd\n";
	$errorLog = `$copycmd`;
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
