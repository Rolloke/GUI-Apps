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

#   PKZIP Commands:
#
# Add                   Default          Header          Sfx
# Comment               Delete           Help            Test
# Configuration         Extract          License         Version
# Console               Fix              Print           View
#
#   PKZIP Options:
#
# 204                   Header          NoExtended       Shortnam
# After                 Include         NoFix            Silent
# Attributes            Level           Normal           Sort
# Authenticity          ListChar        NoZipExtension   Span
# Before                ListSfxTypes    OptionChar       Speed
# Comment               Locale          Overwrite        Store
# Decode                Mask            Password         Temp
# Directories           Maximum         Path             Times
# Encode                More            Preview          Volume
# Exclude               Move            Recurse          Warning
# Fast                  NameSfx         Sfx              Zipdate
#

#********************************************************
# <GLOBALS> 
#********************************************************
@Recipients=("UweXP",
			 "Martin01",
			 "Tomas",
			 "GeorgXP",
			 "RolfXP",
			 "Make"
			 );

$g_Second=0;
$g_Minute=0;
$g_Hour=0;
$g_Day=0;
$g_Month=0;
$g_Year=0;

# set some global path variables
$ZIP_CMD="v:\\bin\\tools\\zip\\pkzip25.exe";
$g_sSSDir="v:\\vss";
$g_sSSDestDir="g:\\vss";
$g_sDestDir="g:\\vsssafe";

$g_sSSZipFileOld = "";
$g_sSSZipFileNew = "";

$g_SizeSSAndZips = 0;
$g_Size2SSAndZips = 0;

#********************************************************
&SetTimeVariables();
print "SourceSafeZip started at $g_Day.$g_Month.$g_Year, $g_Hour:$g_Minute:$g_Second\n";

if (! -f "$ZIP_CMD") {
	die "pkzip.exe not found $ZIP_CMD\n";
}

print "Pkzip is $ZIP_CMD\n";

if (! -d "$g_sSSDir" )
{
	print "no source safe dir $g_sSSDir\n";
	die;
}

if (! -d "$g_sDestDir" )
{
	`mkdir "$g_sDestDir"`
}

if (! -d "$g_sSSDestDir" )
{
	`mkdir "$g_sSSDestDir"`
}

#********************************************************
# Warn Software that backup starts now
for (@Recipients)
{
	`Net Send $_ "ACHTUNG: Software-Sicherung wird gestartet"`;
}

#********************************************************
print "xcopying $g_sSSDir to $g_sSSDestDir\n";
`xcopy /E /S /Q /C /H /R /Y $g_sSSDir $g_sSSDestDir`;
print "xcopying $g_sSSDir to $g_sSSDestDir complete\n";

#********************************************************
$g_sSSZipFileNew = "$g_Year$g_Month$g_Day.zip";
$EXE_CMD = "$ZIP_CMD -add -recurse -directories $g_sDestDir\\$g_sSSZipFileNew $g_sSSDestDir\\*.*";
print "Compressing $g_sSSDestDir to $g_sDestDir\n";
print "$EXE_CMD\n";
`$EXE_CMD`;
print "Compressing $g_sSSDestDir to $g_sDestDir complete\n";

#********************************************************
print "deleting copied files\n";
if (-d "$g_sSSDestDir" )
{
	`cd $g_sDestDir`;						# dir to delete must not be actual dir
#	print `deltree /Y $g_sSSDestDir`;	# Win95 command
	print `rd /s /q $g_sSSDestDir`;		# WinNT command
}
print "deleting copied files complete\n";

#********************************************************
# check for success of zipping (may be out of harddisk space)
$DoneText = "SourceSafeZip FAILED";
$SizeText = "";
if (-f "$g_sDestDir\\$g_sSSZipFileNew" )
{
	print "counting total files size for burning\n";
	&CountTotalSize();
	$DoneText = "SourceSafeZip done";
	$SizeText = "Size VSS: $g_SizeSSAndZips MB ; Two VSS: $g_Size2SSAndZips MB";
}

#********************************************************
&SetTimeVariables();
print "$DoneText at $g_Day.$g_Month.$g_Year, $g_Hour:$g_Minute:$g_Second  $SizeText\n";
$DoneText = "$DoneText  $SizeText\n";
for (@Recipients)
{
	print "Net Send $_\n";
	`Net Send $_ "$DoneText"`;
}

&SetTimeVariables();
print "End SourceSafeZip at $g_Day.$g_Month.$g_Year, $g_Hour:$g_Minute:$g_Second\n";
###################################################################################################
# end of main

###################################################################################################
# usage:
# &CountTotalSize();
# sums up in 2 global variables: g_SizeSSAndZips and g_Size2SSAndZips
sub CountTotalSize
{
	local ($actualDir, $dir, $datei, $size, $sizeTotalBuilds);
	$sizeTotalBuilds=0;
	$actualDir = getcwd;
#	printf("CurrentDir %s\n", $actualDir);

	# SourceSafeZip Dateien
	# only oldest and newest will be counted
	$dir = $g_sDestDir;
	chdir($dir);      
#	print("ChangeDir $dir\n");
#	printf("CurrentDir %s \n", getcwd);
	opendir(DIR, $dir);
	$bStartet=0;
	while($datei = readdir(DIR))
	{
		$size = (-s $datei);            
#		print "$datei : $size\n";
		if (   !$bStartet
		    && $datei =~ /\.zip/)	# search for ".zip")
		{
#			print "first VSS file $datei : $size\n";
			$bStartet=1;
			if ($datei ne $g_sSSZipFileNew)
			{
				g_sSSZipFileOld = $datei;
				$g_Size2SSAndZips += $size;
			}
		}
		if ($datei eq $g_sSSZipFileNew)
		{
#			print "actual VSS file $datei : $size\n";
			$g_SizeSSAndZips = $size;
			$g_Size2SSAndZips += $size;
		}
	}
	closedir(DIR);

	# BuildZips DV
	$dir = "V:/BuildDV";
	chdir($dir);      
	opendir(DIR, $dir);
	while($datei = readdir(DIR))
	{
		$sizeTotalBuilds += (-s $datei);
	}
	closedir(DIR);

	# BuildZips idip
	$dir = "V:/BuildZips";
	chdir($dir);      
	opendir(DIR, $dir);
	while($datei = readdir(DIR))
	{
		$sizeTotalBuilds += (-s $datei);
	}
	closedir(DIR);

	# BuildZips Telekom
	$dir = "V:/BuildZipsTelekom";
	chdir($dir);      
	opendir(DIR, $dir);
	while($datei = readdir(DIR))
	{
		$sizeTotalBuilds += (-s $datei);
	}
	closedir(DIR);

	chdir($actualDir);      

	$g_SizeSSAndZips += $sizeTotalBuilds;
	$g_Size2SSAndZips += $sizeTotalBuilds;

	$g_SizeSSAndZips = $g_SizeSSAndZips/1024/1024;	# Size in MB, caveat: can be float
	if($g_SizeSSAndZips =~ /\./)	# search for point of float
	{
		$g_SizeSSAndZips = $`;	# string in front of point => integer
		$g_SizeSSAndZips++;		# round up
	}
	$g_Size2SSAndZips = $g_Size2SSAndZips/1024/1024;	# Size in MB, caveat: can be float
	if($g_Size2SSAndZips =~ /\./)	# search for point of float
	{
		$g_Size2SSAndZips = $`;	# string in front of point => integer
		$g_Size2SSAndZips++;	# round up
	}
#	print "g_SizeSSAndZips $g_SizeSSAndZips\n";
#	print "g_Size2SSAndZips $g_Size2SSAndZips\n";
}
###################################################################################################
sub SetTimeVariables
{
	local ($isdst,$wday,$yday);
	
	($g_Second,$g_Minute,$g_Hour,$g_Day,$g_Month,$g_Year,$wday,$yday,$isdst) = localtime(time);
	$g_Month=eval($g_Month+1);
	$g_Year=eval($g_Year+1900);

	if ( $g_Second < 10) {
		$g_Second = "0$g_Second";
	}
	if ( $g_Minute < 10) {
		$g_Minute = "0$g_Minute";
	}
	if ( $g_Hour < 10) {
		$g_Hour = "0$g_Hour";
	}
	if ( $g_Day < 10) {
		$g_Day = "0$g_Day";
	}
	if ( $g_Month < 10) {
		$g_Month = "0$g_Month";
	}
}
###################################################################################################
__END__
:endofperl
