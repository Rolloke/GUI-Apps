@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

#
# PERL PART FOLLOWS HERE
#

use Cwd;

#############
# <trace options> #
#############
$bTraceOneConfig    = 0;
$bTraceCopy         = 0;
$bTraceUnlink       = 0;
$bTraceSuperHistory = 0;
$bTraceSourceSafe   = 0;

#############
# <GLOBALS> #
#############
$sec=0;
$min=0;
$hour=0;
$mday=0;
$mon=0;
$year=0;
$g_bIsSuperMake = 1;
$g_bIsSuperCopy = 0;

$g_bUseVS71     = 1;
$g_bFinalCopy   = 0;
$g_iErrors      = 0;
$g_iWarnings    = 0;
$g_iUnknownProj = 0;
$g_iCopyErrors  = 0;
$g_bStopOnError = 0;
$g_bGetDepend   = 1; # disable for faster testing
$g_bCompileLibs = 1; # disable for faster testing
$g_bRebuildAll  = 0; # rebuild all projects
$g_bRebuildProj = 1; # rebuild all if dsp changed, see TrySourceSafe()
$g_bUpdateBuild = 1;	# Update build number and set label, disable for testing


@g_CheckTranslations=();

$MSDEV_DIR              = "";
$MSDEV_IDE_DIR          = "";
$g_theProjectDrive      = "D:";	# may be set by command line parameter
$g_theProjectDir        = "UNKNOWN";
$g_theCurrentProjectDir = "UNKNOWN";
$g_theDestinationDrive  = "G:";	# may be set by command line parameter
$g_theDestinationDir    = "UNKNOWN";	# has to contain vinfo.pvi for the buildNumber info
$g_theDestinationDirEnu = "UNKNOWN";
$g_theBuildZipDrive     =  "g:";
$g_theBuildZipDir       =  "$g_theBuildZipDrive\\BuildDts";

$LabelPrefix="Test_DTS_";
$VersionLabel="";
# >>> VersionsLabel leer fuer aktuelle Version <<<
# oder mit VersionsLabel passend zum SourceSafe $/Project label
#$VersionLabel="DTS_2.5.0.612";

$buildNumber=0;		# read from vinfo.pvi, but used in multiple functions
$currentBuild=1;	# updated in ReadBuild

$VersionMajor    = 3;
$VersionMinor    = 1;
$VersionSubIndex = 0;
$versionInfo     = "$VersionMajor.$VersionMinor.$VersionSubIndex.";

$g_iTotalLines=0;
$g_sStatisticsText = "Statistic:\n";

@ErrorRecipients=("UWEXP","MARTINXP","Tomas","RolfXP");
#@ErrorRecipients=("127.0.0.1");

# set some global path variables
$ZIP_CMD="v:\\bin\\tools\\zip\\pkzip25.exe";

#**********************************************************
# dependencie projects are not compiled
@DepProjects=(
#				"Cipc",					# WKClasses uses some header...	
#				"DV/DVClient",			# only for resource.h for LangDll
#				"DV/LangDll",			# only for resource.h for DVClient
#				"DirectShow",
				"DirectShow/Lib",
				"include",
#				"Lib/WKClasses",
#				"Tools/LogzipLib",
#				"Units/JaCob",			# must get the Jacob.sln
#				"Units/JaCob/JaCobSys",
#				"Units/JaCob/JaCobDA",	# OOPS
#				"Units/JaCob/JaCobDll",	# OOPS only for jacobdefs.h
#				"Units/SaVic2",			# must get the SaVic2.sln
#				"Units/SaVic2/SaVicDA",	
#				"Units/SaVic2/SaVicDll",	
#				"Units/SaVic2/SaVicSys",
#				"Units/Tasha",			# must get the Tasha.sln
				);

#**********************************************************
# LibProjects are compiled first, that is before the normal Projects
@LibProjects=(
				"Lib/WKClasses",		# is already used in CIPC
#				"Tools/LogZipLib",		# is already used in CIPC
				"Cipc",					# is used in all projects
				"Lib/Skins",			# is already used in OemGui

				"Capi/Capi4",

				"DirectShow/Filters/baseclasses",			# a lib. alreday used in Common
				"DirectShow/Filters/dump",					# ax
				"DirectShow/Filters/inftee",				# ax
#				"DirectShow/Filters/mediasource",			# ax
				"DirectShow/Filters/PushSource",			# ax
#				"DirectShow/Filters/Sample2MemRenderer",	# ax
				"DirectShow/Filters/WaveSplitter",			# ax
				"DirectShow/Common",						# dll

				"DV/KeyHook",				# no real compile, just copy
				"DV/LangDll",
				"DV/Lib/Hooks",

				"Lib/codebase65",			# neue codebase lib unicode und msdev2003	
				"Lib/DeviceDetect",
				"Lib/HardwareInfo",
				"Lib/NetComm",
				"Lib/NFC",
				"Lib/oemgui",
				"Lib/mpeg4spenc",
				"Lib/VImage",

# JaCobUnit will compile all dependencies
				"Units/JaCob/JaCobDA",		# no real compile, just copy
				"Units/JaCob/JaCobDll",		# no real compile, just copy
				"Units/JaCob/JaCobSys",		# no real compile, just copy
# SaVicUnit will compile all dependencies
				"Units/SaVic2/SaVicDA",		# no real compile, just copy
				"Units/SaVic2/SaVicDll",	# no real compile, just copy
				"Units/SaVic2/SaVicSys",	# no real compile, just copy
# TashaUnit will compile all dependencies
				"Units/Tasha/TashaDA",		# no real compile, just copy
				"Units/Tasha/TashaDll",		# no real compile, just copy
				"Units/Tasha/TashaSys",		# no real compile, just copy
				"Units/Tasha/TashaFirmware",		# no real compile, just copy
			);

#**********************************************************
# Liste aller relevanten Projekte
# CAVEAT muss auch weiter unten angepasst werden
# siehe CAVEAT2 und CAVEAT3
# NEW PROJECT das WorkingDirectory muss auf Senso fuer D:\Project\NewProject gesetzt werden
# NEW PROJECT add here
@Projects=( 
			"Capi/ISDNUnit",
			"DV/acdc",
			"DV/DVClient",
			"DV/DVHook",
			"DV/DVProcess",
			"DV/DVStarter",
#			"DV/DVStorage",
			"DV/RemoteCD",
			"DV/Tools/DVSoftAlarm",
			"DV/Tools/DVSTest",
			"DV/Tools/ExpansionCode",
			"DV/WatchCD",

			"ControlService",
			"SocketUnit",
			"Dbs",

			"ControlService",
			"Tools/HiveInjection",
			"Tools/VideteCheckDisk",
			"Tools/HealthControl",
			"Tools/EwfControl",
			"Tools/CDStart",
			"Tools/DelRegDb",
			"Tools/DvrtRun",
			"Tools/EULA",
			"Tools/JVIEW",
			"Tools/LogView",
			"Tools/ProductVersion",
			"Tools/ProductView",
			"Tools/RSA",
			"Tools/SecAnalyzer",
			"Tools/StressIt",
			"Tools/VipCleanNT",
			"Tools/Win95Reboot",

			"Units/Audio/AudioUnit",
			"Units/CommUnit",
			"Units/NetUnit",
			"Units/JaCob/JaCobUnit",	# compiles DLLs, so first of all JaCob!
			"Units/JaCob/JaCobClock",
			"Units/JaCob/JaCobTest",
			"Units/JaCob/JaCobEEPrommer",
			"Units/JaCob/RegisterMonitor",
			"Units/SaVic2/SaVicUnit",	# compiles DLLs, so first of all Savic!
			"Units/Tasha/TashaUnit",	# compiles DLLs, so first of all Tasha!
			"Units/QUnit",	# compiles DLLs, so first of all Q!
			);

##############
# </GLOBALS> #
##############

#**********************************************************
# check some requirements
if (! -f "$ZIP_CMD")
{
	&NetSendError("pkzip.exe not found $ZIP_CMD");
	die "pkzip.exe not found $ZIP_CMD";
}

#**********************************************************
# handle command line parameters
if ($#ARGV != -1)
{  
	my ($bTraceDebug) = 1;
	my ($DriveLetter) = "";

	for (@ARGV)
	{
		if ($bTraceDebug) {print ("Parameter $_\n");}

		if (/-superCopy/)
		{
			$g_bIsSuperMake = 0;
			$g_bIsSuperCopy = 1;
		}
		elsif (/-rebuild/i)
		{
			$g_bRebuildAll = 1;
			if ($bTraceDebug) {print ("Rebuild all\n");}
		}
		elsif (/-vs60/i)
		{
			$g_bUseVS71 = 0;
			if ($bTraceDebug) {print ("Using VisualStudio 6.0\n");}
		}
		elsif (/-vl:/i)
		{

			$VersionLabel = substr($_, 4);
			if ($bTraceDebug) {print ("Version is \"$VersionLabel\"\n");}
		}
		elsif (/-Stop/i)
		{
			$g_bStopOnError = 1;
			if ($bTraceDebug) {print ("Stop on error\n");}
		}
		elsif (/-ProjDrive:/i)
		{
			$DriveLetter = substr($_, 11, 1);
			$g_theProjectDrive = "$DriveLetter\:";
			if ($bTraceDebug) {print ("ProjectDrive is \"$g_theProjectDrive\"\n");}
			if (! -d "$g_theProjectDrive\\")
			{
				die "Project drive does not exist: $g_theProjectDrive\n";
			}
		}
		elsif (/-DestDrive:/i)
		{
			$DriveLetter = substr($_, 11, 1);
			$g_theDestinationDrive = "$DriveLetter\:";
			if ($bTraceDebug) {print ("DestDrive is \"$g_theDestinationDrive\"\n");}
			if (! -d "$g_theDestinationDrive\\")
			{
				die "Destination drive does not exist: $g_theDestinationDrive\n";
			}
		}
		else
		{
			die "Unknown command line parameter: $_\n";
		}
	}
}
else
{
	# ok, work as superMake without additional parameters
}

#********************************************************
# Project and/or destination drive may be set by command line
$g_theProjectDir          = "$g_theProjectDrive\\Project";
$g_theDestinationDir      = "$g_theDestinationDrive\\bin\\DTS";	# has to contain vinfo.pvi for the buildNumber info
$g_theDestinationDirEnu   = "$g_theDestinationDir\\Enu";

#********************************************************
&SetTimeVariables();
&ReadBuildNumber();

&SetTimeVariables();
print "Script started at $mday.$month.$year, $hour:$min:$sec ; Versionlabel '$VersionLabel' ; working for '$versionInfo$currentBuild'\n";

if ($g_bUseVS71) {print ("### Using VisualStudio 7.1 (.NET 2003) in $g_theProjectDir ###\n");}
else {print "### Using VisualStudio 6.0 in $g_theProjectDir ###\n";}

if ($g_bRebuildAll) {print "### Rebuild all projects ###\n";}

if ($g_bStopOnError) {print "### Stop on errors ###\n";}

print ("### Use Project dir $g_theProjectDir ###\n");
print ("### Use Destination $g_theDestinationDir ###\n");

#********************************************************
# force "flush output"
$|=1;
select(STDERR); $|=1;
select(STDOUT); $|=1;

#********************************************************
# create required directories
chdir ("$g_theProjectDrive\\");
if (! -d "$g_theProjectDir")
{
	`mkdir "$g_theProjectDir`;
}
if (! -d "$g_theProjectDir\\Maps")
{
	`mkdir "$g_theProjectDir\\Maps`;
}
if (! -d "$g_theProjectDir\\Dlls")
{
	`mkdir "$g_theProjectDir\\Dlls`;
}
if (! -d "$g_theProjectDir\\DV\\LangDll\\Libs")
{
	`mkdir "$g_theProjectDir\\DV\\LangDll\\Libs`;
}
if (!chdir("$g_theProjectDir"))
{
	&NetSendError("chdir '$g_theProjectDir' failed");
	die "chdir '$g_theProjectDir' failed\n";
}
chdir ("$g_theProjectDrive\\");
&CreateNewDir("D:/dv/Dlls");
chdir ("$g_theProjectDrive\\");

chdir ("$g_theDestinationDrive\\");
if (! -d "$g_theDestinationDir")
{
	`mkdir "$g_theDestinationDir`;
}
if (! -d "$g_theDestinationDir\\Dlls")
{
	`mkdir "$g_theDestinationDir\\Dlls`;
}
if (! -d "$g_theDestinationDir\\Maps")
{
	`mkdir "$g_theDestinationDir\\Maps`;
}

chdir ("$g_theDestinationDrive\\");
if (! -d "$g_theDestinationDirEnu")
{
	`mkdir "$g_theDestinationDirEnu`;
}
if (! -d "$g_theDestinationDirEnu\\Dlls")
{
	`mkdir "$g_theDestinationDirEnu\\Dlls`;
}
if (! -d "$g_theDestinationDirEnu\\Maps")
{
	`mkdir "$g_theDestinationDirEnu\\Maps`;
}

chdir ("$g_theBuildZipDrive\\");
if (! -d "$g_theBuildZipDir")
{
	`mkdir "$g_theBuildZipDir`;
}

chdir ("$g_theProjectDrive\\");
chdir ("$g_theProjectDir");

#********************************************************
print "\n########################################################\n";		# seperator
&SearchSS();
&SearchMSDEV();
&SetEnv();

#********************************************************
print "\n########################################################\n";		# seperator
if ($g_bIsSuperMake)
{
	&SetTimeVariables();
	print "DTS Unicode Make started at $mday.$month.$year, $hour:$min:$sec ; Versionlabel '$VersionLabel' ; compiling for '$versionInfo$currentBuild'\n";

	if ($g_bGetDepend)
	{
		&GetDependencies();
	}
	else
	{
		print ">>> Not getting DepProjects! <<<\n";
	}

	if ($g_bCompileLibs)
	{
		&CompileLibs();
	}
	else
	{
		print ">>> Not compiling Libs! <<<\n";
	}

	for (@Projects)
	{
		print "\n########################################################\n";		# seperator
		local($project);
		$project=$_;

		chdir("/Project/");
		if (! -d $project)
		{
			&CreateNewDir($project);
			chdir("/Project/");
		}
		chdir($project);
		$g_theCurrentProjectDir="$g_theProjectDir\\$project";

		if (/DV/)
		{
			&TrySourceSafe("\$\\$project");
		}
		else
		{
			&TrySourceSafe("\$\\Project\\$project");
		}

		&DoConfigurations($project);
	}
	print ("\n$g_sStatisticsText");
	print ("Lines in total: $g_iTotalLines\n");

	&SetTimeVariables();
	$MakeDoneText = "DTS Unicode Make DONE at $mday.$month.$year, $hour:$min:$sec, Version $versionInfo$currentBuild";
	print "\n$MakeDoneText\n";

	$MakeErrorText = "OK";
	if ($g_iErrors || $g_iWarnings || $g_iUnknownProj || ($#g_CheckTranslations>=0))
	{
		$MakeErrorText = "$g_iErrors error[s], $g_iWarnings warning[s], $g_iUnknownProj Unknown projects";
		print ("Es gab $MakeErrorText\n\n" );
		if ($#g_CheckTranslations >= 0)
		{
			$MakeErrorText .= ", Check translations:";
			print "CheckTranslations:\n";
			foreach (@g_CheckTranslations)
			{
				$MakeErrorText .= " $_";
				print "   $_\n";
			}
			print "\n";
		}
	}
	else
	{
		print "Es gab keine Fehler oder Warnings.\n\n";
	}

	for (@ErrorRecipients)
	{
		print "Net Send $_\n";
		`Net Send $_ "$MakeDoneText, $MakeErrorText"`;
	}
	&SetTimeVariables();
	$MakeEndText = "DTS Unicode Make END at $mday.$month.$year, $hour:$min:$sec, Version $versionInfo$currentBuild";
	print "\n$MakeEndText\n";
}
elsif ($g_bIsSuperCopy)
{
	&SetTimeVariables();
	print "DTS Unicode Copy started at $mday.$month.$year, $hour:$min:$sec ; Versionlabel '$VersionLabel' ; copying for '$versionInfo$currentBuild'\n";

	# switch to copy mode
	$g_bFinalCopy = 1;	
	# and again loop over all projects, depending on $g_bFinalCopy
	# now a copy is made, not the nmake call

	for (@LibProjects)
	{
		local($project);
		$project=$_;

		chdir("/Project/");
		&DoConfigurations($project);
	}	# end of loop over projects
	
	
	for (@Projects)
	{
		local($project);
		$project=$_;

		chdir("/Project/");
		&DoConfigurations($project);

	}	# end of loop over projects

	&SetTimeVariables();
	$MakeDoneText = "DTS Unicode Copy DONE at $mday.$month.$year, $hour:$min:$sec, Version $versionInfo$currentBuild";
	print "\n$MakeDoneText\n";

	$MakeErrorText = "OK";
	if ($g_iCopyErrors!=0)
	{
		$MakeErrorText = "$g_iCopyErrors Error[s]";
		print ("Es gab $g_iCopyErrors Fehler beim Kopieren\n");
		print ("BuildNummer wird nicht erhoeht\n\n");
	}
	else
	{
		print ("Es gab keine Fehler beim Kopieren.\n\n");
		&UpdateBuild();
	}

	for (@ErrorRecipients)
	{
		print "Net Send $_\n";
		`Net Send $_ "$MakeDoneText, $MakeErrorText"`;
	}
	&SetTimeVariables();
	$MakeEndText = "DTS Unicode Copy END at $mday.$month.$year, $hour:$min:$sec, Version $versionInfo$currentBuild";
	print "\n$MakeEndText\n";
}
else
{
	die ("Unknown script variant\n");
}
  
#
# end of 'main'
#
#######################################################################
#**********************************************************************
#######################################################################
sub GetDependencies
{
	print "\n########################################################\n";		# seperator
	print ("Getting Libs....\n");
	
	if (! -d "$g_theProjectDir/Lib")
	{
		`mkdir "$g_theProjectDir/Lib"`;
	}

	chdir("$g_theProjectDir/Lib");

	print `$SAFE_CMD  Get \"\\$\\project\\lib\\imagn32.lib" -I- \"-VL$VersionLabel\"`;
	print `$SAFE_CMD  Get \"\\$\\project\\lib\\ijl15.lib" -I- \"-VL$VersionLabel\"`;
	print `$SAFE_CMD  Get \"\\$\\dv\\acdc\\NeroApiGlue.lib" -I- \"-VL$VersionLabel\"`;
	print `$SAFE_CMD  Get \"\\$\\project\\lib\\mp4spvd_PIV.lib" -I- \"-VL$VersionLabel\"`;
	print `$SAFE_CMD  Get \"\\$\\project\\lib\\mpeg4_sp_enc.lib" -I- \"-VL$VersionLabel\"`;
	print `$SAFE_CMD  Get \"\\$\\project\\lib\\codebase65.lib" -I- \"-VL$VersionLabel\"`;
	print `$SAFE_CMD  Get \"\\$\\project\\lib\\CapSeries.lib" -I- \"-VL$VersionLabel\"`;

	for (@DepProjects)
	{
		$project = $_;
		chdir("/Project/");	# back to project root dir
		if (! -d $project)	# subdir already there ?
		{
			&CreateNewDir($project);
			chdir("/Project/");
		}
		chdir($project);
		$g_theCurrentProjectDir="$g_theProjectDir/$project";

		# remove before the SourceSafe complains about a writable copy
		# it´s recompiled anyway, thus it has no write-protection
		&DeleteOutputFile($project, 1);

		# get the latest/labeled version from source safe
		if (/DV/)
		{
			&TrySourceSafe("\$\\$project");
		}
		else
		{
			&TrySourceSafe("\$\\Project\\$project");
		}
	}
}
#######################################################################
sub CompileLibs
{
	# loop over all LibProjects
	for (@LibProjects)
	{
		print "\n########################################################\n";		# seperator
		local($project);
		$project=$_;

		if ($project eq "Lib/codebase65")
		{
			# skip
		}
		else
		{
			chdir("/Project/");
			if (! -d $project)
			{
				&CreateNewDir($project);
				chdir("/Project/");
			}
			chdir($project);
			$g_theCurrentProjectDir="$g_theProjectDir/$project";

#			# remove before the SourceSafe complains about a writable copy
#			# it´s recompiled anyway, thus it has no write-protection
			&DeleteOutputFile($project, 1);

			# get the latest/labeled version from source safe
			if (/DV/)
			{
				&TrySourceSafe("\$\\$project");
			}
			else
			{
				&TrySourceSafe("\$\\Project\\$project");
			}


			# remove not writable libs before compiling
			chdir($g_theCurrentProjectDir);
			&DeleteOutputFile($project, 1);
			
			&DoConfigurations($project);
		}
	}	# end of loop over all LibProjects
}
#######################################################################
sub DeleteOutputFile
{
	local($project,$bDoMsg) = @_;
	local ($bTraceDebug) = 1;
	
	if ($bTraceDebug) {printf("Delete output file of project $project ...\n");}
	$_ = $project;
	if (/Capi4/)
	{
		if ($g_bUseVS71)
		{
			&DeleteEvenIfRO("ReleaseUnicode/Capi4_32U.lib",$bDoMsg);
		}
		else
		{
			&DeleteEvenIfRO("Release/Capi4_32.lib",$bDoMsg);
		}
	}
	elsif (/Cipc/)
	{
		if ($g_bUseVS71)
		{
			&DeleteEvenIfRO("ReleaseUnicode/CIPCU.lib",$bDoMsg);
			&DeleteEvenIfRO("ReleaseUnicode/CIPCU.dll",$bDoMsg);
		}
		else
		{
			&DeleteEvenIfRO("Release/CIPC.lib",$bDoMsg);
			&DeleteEvenIfRO("Release/CIPC.dll",$bDoMsg);
		}
	}
	elsif (/DV\/Lib\//)
	{
		# Only projects in LIB

		# change to LIB dir
#		chdir($g_theProjectDir\\DV\\Lib);
#		if ($bTraceDebug) {printf ("Changed to Lib dir? %s\n",getcwd);}
		$ProjectOnly = $project;
		$ProjectOnly =~ s/^DV\/Lib\///;	# drop leading DV/Lib/
		$ProjectOnlyU = $ProjectOnly;
		$ProjectOnlyU .= "U";

		if ($g_bUseVS71)
		{
			if ($bTraceDebug) {printf("Delete output file $ProjectOnlyU\.lib ...\n");}
			&DeleteEvenIfRO("..\\$ProjectOnlyU\.lib",$TraceDeleteEvenIfRO);
			if ($bTraceDebug) {printf("Delete output file $ProjectOnlyU\.dll ...\n");}
			&DeleteEvenIfRO("..\\$ProjectOnlyU\.dll",$TraceDeleteEvenIfRO);
		}
		else
		{
			if ($bTraceDebug) {printf("Delete output file $ProjectOnly\.lib ...\n");}
			&DeleteEvenIfRO("$ProjectOnly.lib",$TraceDeleteEvenIfRO);
			if ($bTraceDebug) {printf("Delete output file $ProjectOnly\.dll ...\n");}
			&DeleteEvenIfRO("$ProjectOnly.dll",$TraceDeleteEvenIfRO);
		}

		$ProjectOnly = $project;
#		# change back to project dir
#		if ($bTraceDebug) {printf ("Change to current project dir   $g_theCurrentProjectDir\n");}
#		chdir($g_theCurrentProjectDir);
#		if ($bTraceDebug) {printf ("Changed to current project dir? %s\n",getcwd);}
	}
	elsif (/JaCobDA/)
	{
		if ($g_bUseVS71)
		{
			&DeleteEvenIfRO("Release/JaCobDAU.lib",$bDoMsg);
			&DeleteEvenIfRO("Release/JaCobDAU.dll",$bDoMsg);
		}
		else
		{
			&DeleteEvenIfRO("Release/JaCobDA.lib",$bDoMsg);
			&DeleteEvenIfRO("Release/JaCobDA.dll",$bDoMsg);
		}
	}
	elsif (/JaCobDll/)
	{
		if ($g_bUseVS71)
		{
			&DeleteEvenIfRO("Release/JaCobU.lib",$bDoMsg);
			&DeleteEvenIfRO("Release/JaCobU.dll",$bDoMsg);
		}
		else
		{
			&DeleteEvenIfRO("Release/JaCob.lib",$bDoMsg);
			&DeleteEvenIfRO("Release/JaCob.dll",$bDoMsg);
		}
	}
	elsif (/LogZipLib/)
	{
		if ($g_bUseVS71)
		{
			&DeleteEvenIfRO("ReleaseUnicode/LogZipLibU.lib",$bDoMsg);
		}
		else
		{
			&DeleteEvenIfRO("Release/LogZipLib.lib",$bDoMsg);
		}
	}
	elsif (/MiCoDA/)
	{
		&DeleteEvenIfRO("Release/MiCoDA.lib",$bDoMsg);
		&DeleteEvenIfRO("Release/MiCoDA.dll",$bDoMsg);
	}
	elsif (/SaVicDA/)
	{
		if ($g_bUseVS71)
		{
			&DeleteEvenIfRO("Release/SaVicDAU.lib",$bDoMsg);
			&DeleteEvenIfRO("Release/SaVicDAU.dll",$bDoMsg);
		}
		else
		{
			&DeleteEvenIfRO("Release/SaVicDA.lib",$bDoMsg);
			&DeleteEvenIfRO("Release/SaVicDA.dll",$bDoMsg);
		}
	}
	elsif (/SaVicDll/)
	{
		if ($g_bUseVS71)
		{
			&DeleteEvenIfRO("Release/SaVicU.lib",$bDoMsg);
			&DeleteEvenIfRO("Release/SaVicU.dll",$bDoMsg);
		}
		else
		{
			&DeleteEvenIfRO("Release/SaVic.lib",$bDoMsg);
			&DeleteEvenIfRO("Release/SaVic.dll",$bDoMsg);
		}
	}
	elsif (/Lib\//)
	{
		# Only projects in LIB

		# change to LIB dir
#		chdir($g_theLibDir);
#		if ($bTraceDebug) {printf ("Changed to Lib dir? %s\n",getcwd);}
		$ProjectOnly = $project;
		$ProjectOnly =~ s/^Lib\///;	# drop leading Lib/
		$ProjectOnlyU = $ProjectOnly;
		$ProjectOnlyU .= "U";

		if ($g_bUseVS71)
		{
			if ($bTraceDebug) {printf("Delete output file $ProjectOnlyU\.lib ...\n");}
			&DeleteEvenIfRO("..\\$ProjectOnlyU\.lib",$TraceDeleteEvenIfRO);
			if ($bTraceDebug) {printf("Delete output file $ProjectOnlyU\.dll ...\n");}
			&DeleteEvenIfRO("..\\$ProjectOnlyU\.dll",$TraceDeleteEvenIfRO);
		}
		else
		{
			if ($bTraceDebug) {printf("Delete output file $ProjectOnly\.lib ...\n");}
			&DeleteEvenIfRO("$ProjectOnly.lib",$TraceDeleteEvenIfRO);
			if ($bTraceDebug) {printf("Delete output file $ProjectOnly\.dll ...\n");}
			&DeleteEvenIfRO("$ProjectOnly.dll",$TraceDeleteEvenIfRO);
		}

		$ProjectOnly = $project;
#		# change back to project dir
#		if ($bTraceDebug) {printf ("Change to current project dir   $g_theCurrentProjectDir\n");}
#		chdir($g_theCurrentProjectDir);
#		if ($bTraceDebug) {printf ("Changed to current project dir? %s\n",getcwd);}
	}
	else
	{
		if ($bTraceDebug) {printf("Delete output file UNKNOWN $project\n");}
	}
}
#######################################################################
sub DoConfigurations
{ 
	local($project)=@_;
	
	local(@cfgs);
	local($projfile)="foo.dsp";
	local($dest)="foo";

	# print("DO \"$project\"\n");
		
	# CAVEAT2 jedes Project muss hier eingetragen sein!
	# special settings per project
	# set default values
	$projfile = "$project\.dsp";
	if ($g_bUseVS71)
	{
		$projfile = "$project\.vcproj";
	}
	$dest = "$project";

	if ($project eq "SecurityServer")
	 {
		# makefile has different name than project
		$projfile	= "sec3.dsp";
		if ($g_bUseVS71)
		{
			$projfile = "sec3.vcproj";
		}
		$dest = "sec3";
	}
	elsif ($project eq "Capi/Capi4")
	{
		$projfile = "Capi4_32.dsp";
		if ($g_bUseVS71)
		{
			$projfile = "Capi4_32.vcproj";
		}
		$dest = "Capi4_32";
	}
	else
	{
		# set default values
		$projfile	= "$project\.dsp";
		if ($g_bUseVS71)
		{
			$projfile = "$project\.vcproj";
		}
		$dest = "$project";

		#
		# CAVEAT order is important, multi-dir always first
		#
		$projfile =~  s/^Units\/JaCob\///;	# drop leading Units/JaCob/JaCobUnit
		$dest =~ s/^Units\/JaCob\///;	# drop leading Units/JaCob/JaCobUnit

		$projfile =~  s/^Units\/SaVic2\///;	# drop leading Units/SaVic/SaVicUnit
		$dest =~ s/^Units\/SaVic2\///;	# drop leading Units/SaVic/SaVicUnit

		$projfile =~  s/^Units\/SaVic\///;	# drop leading Units/SaVic/SaVicUnit
		$dest =~ s/^Units\/SaVic\///;	# drop leading Units/SaVic/SaVicUnit

		$projfile  =~  s/^Units\/Audio\///;	# drop leading Units/Audio
		$dest =~ s/^Units\/Audio\///;	# drop leading Units/Audio

		$projfile	=~  s/^Units\/Tasha\///;	# drop leading Units/SaVic/SaVicUnit
		$dest		=~ s/^Units\/Tasha\///;	# drop leading Units/SaVic/SaVicUnit

		$projfile =~  s/^DV\/Tools\///;	# drop leading DV/Tools
		$dest =~ s/^DV\/Tools\///;

		$projfile =~ s/^Units\///;	# drop leading Units/
		$dest =~ s/^Units\///;	# drop leading Units/

		$projfile =~ s/^DV\///;	# drop leading 
		$dest =~ s/^DV\///;	# drop leading

		$projfile =~ s/^Tools\///;	# drop leading Tools/
		$dest =~ s/^Tools\///;	# drop leading Tools/

		$projfile =~ s/^Clients\///;	# drop leading Clients/
		$dest =~ s/^Clients\///;	# drop leading Clients/

		$projfile  =~ s/^Audio\///;	# drop leading 
		$dest =~ s/^Audio\///;	# drop leading

		$projfile  =~ s/^DirectShow\///;	# drop leading 
		$dest =~ s/^DirectShow\///;	# drop leading

		$projfile  =~ s/^Filters\///;	# drop leading 
		$dest =~ s/^Filters\///;	# drop leading

		$projfile =~ s/^Capi\///;	# drop leading Capi/
		$dest =~ s/^Capi\///;	# drop leading Capi/

		$projfile =~ s/^Lib\///;		# drop leading Lib/
		$dest =~ s/^Lib\///;	# drop leading Lib/

		# NEW PROJECT add here, if on 'new' subdirectory
	}

	local($bValidProject)=0;
	local ($bNoCompile) = 0;	# some projects do not need to be compiled (DDK stuff etc.)

	print "$project\n";

	# libs may have Unicode version
	if (   
		   $project eq "Capi/Capi4"
		|| $project eq "Cipc"
		|| $project eq "DirectShow/Common"
		|| $project eq "DV/Lib/Hooks"
		|| $project eq "Lib/Crypto"
		|| $project eq "Lib/DeviceDetect"
		|| $project eq "Lib/HardwareInfo"
		|| $project eq "Lib/ICPCONDll"
		|| $project eq "Lib/JpegLib"
		|| $project eq "Lib/NetComm"
		|| $project eq "Lib/NFC"
		|| $project eq "Lib/Skins"
		|| $project eq "Lib/WKClasses"
		|| $project eq "Tools/LogZipLib"
 		|| $project eq "Lib/VImage"
		)
	{
		if ($g_bUseVS71)
		{
			@cfgs=("ReleaseUnicode");
		}
		else
		{
			@cfgs=("Release");
		}
		$bValidProject=1;
	}
	# or Unicode and EnuUnicode versions
	elsif (	   $project eq "Lib/oemgui"
			)
	{
		if ($g_bUseVS71)
		{
			@cfgs=("ReleaseUnicode","EnuUnicode");
		}
		else
		{
			@cfgs=("Release","Enu");
		}
		$bValidProject=1;
	}
	elsif (
			   $project eq "DV/KeyHook"
			|| $project eq "Units/JaCob/JaCobSys"
			|| $project eq "Units/JaCob/JaCobVxd"
			|| $project eq "Units/SaVic2/SaVicSys"
			|| $project eq "Units/Tasha/TashaSys"
			|| $project eq "Units/Tasha/TashaFirmware"
	   )
	{
			$bNoCompile=1;
			$bValidProject=1;
			# still needs cfg to get the copy done
			@cfgs=("Release");
	} 
	elsif (
			   $project eq "Lib/codebase65"
			|| $project eq "Lib/mpeg4spenc"
		  ) 
	{
		if ($g_bUseVS71 == 0)
		{
			$bNoCompile=1;
		}
		# nur release
		@cfgs=("Release");
		$bValidProject=1;
	}
	elsif (
			   $project eq "Units/JaCob/JaCobDA"
			|| $project eq "Units/SaVic2/SaVicDA"
			|| $project eq "Units/Tasha/TashaDA"
			|| $project eq "Units/Tasha/TashaDll"
		  ) 
	{
		if ($g_bUseVS71)
		{
			$bNoCompile=1;
		}
		# nur release
		@cfgs=("Release");
		$bValidProject=1;
	}
	elsif (
			   $project eq "Units/JaCob/JaCobDll"
			|| $project eq "Units/SaVic2/SaVicDll"
	   )
	{
		if ($g_bUseVS71)
		{
			$bNoCompile=1;
		}
		$bValidProject=1;
		# still needs cfg to get the copy done
		@cfgs=("Release", "Enu");
	} 
	elsif ($project eq "Lib/h263dec")
	{
		@cfgs=("Release","H26x");
		$bValidProject=1;
	}
	elsif (
			   $project eq "DirectShow/Filters/baseclasses"
			|| $project eq "DirectShow/Filters/dump"
			|| $project eq "DirectShow/Filters/inftee"
			|| $project eq "DirectShow/Filters/mediasource"
			|| $project eq "DirectShow/Filters/PushSource"
			|| $project eq "DirectShow/Filters/Sample2MemRenderer"
			|| $project eq "DirectShow/Filters/WaveSplitter"
			|| $project eq "DV/acdc"
			|| $project eq "DV/RemoteCD"
			|| $project eq "DV/Tools/DVSoftAlarm"
			|| $project eq "DV/Tools/ExpansionCode"
			|| $project eq "DV/WatchCD"
			|| $project eq "Lib/jpeg"
			|| $project eq "Lib/pil"
			|| $project eq "ControlService"
			|| $project eq "Tools/HiveInjection"
			|| $project eq "Tools/VideteCheckDisk"
			|| $project eq "Tools/HealthControl"
			|| $project eq "Tools/EwfControl"
			|| $project eq "Tools/CDStart"
			|| $project eq "Tools/CloneNT"
			|| $project eq "Tools/DvrtRun"
			|| $project eq "Tools/JVIEW"
			|| $project eq "Tools/RSA"
			|| $project eq "Tools/ProductView"
			|| $project eq "Tools/ProductVersion"
			|| $project eq "Tools/SecAnalyzer"
			|| $project eq "Tools/StressIt"
			|| $project eq "Tools/Win95Reboot"
			|| $project eq "Units/JaCob/JaCobClock"
			|| $project eq "Units/JaCob/JaCobTest"
			|| $project eq "Units/JaCob/JaCobEEPrommer"
			|| $project eq "Units/JaCob/JaCobTest"
			|| $project eq "Units/JaCob/RegisterMonitor"
			|| $project eq "Units/Tasha/TashaUnit"
			|| $project eq "Units/QUnit"
		) 
	{
		# nur release
		@cfgs=("Release");
		$bValidProject=1;
	}
	elsif (
			   $project eq "DV/DVClient"
			|| $project eq "DV/DVHook"
			|| $project eq "DV/DVStarter"
			|| $project eq "DV/Tools/DVSTest"
		) 
	{
		if ($g_bUseVS71)
		{
			@cfgs=("ReleaseUnicode");
		}
		else
		{
			@cfgs=("Release");
		}
		$bValidProject=1;
	}
	elsif (
			   $project eq "Tools/VipCleanNT"
		) 
	{
		if ($g_bUseVS71)
		{
			@cfgs=("DtsUnicode");
		}
		else
		{
			@cfgs=("Dts");
		}
		$bValidProject=1;
	}
	elsif (
			   $project eq "Dbs"
			|| $project eq "DV/SystemVerwaltung"
			|| $project eq "SecurityLauncher"
			|| $project eq "Units/Audio/AudioUnit"
			|| $project eq "Units/CommUnit"
			|| $project eq "Units/JaCob/JaCobUnit"
			|| $project eq "Units/SaVic2/SaVicUnit"
			|| $project eq "Tools/EULA"
		  )
	{
		# release und enu only
		@cfgs=("Release", "Enu");
		$bValidProject=1;
	} 
	elsif (
			   $project eq "DV/DVProcess"
			|| $project eq "DV/DVStorage"
		  )
	{
		if ($g_bUseVS71)
		{
			@cfgs=("ReleaseUnicode","EnuUnicode");
		}
		else
		{
			@cfgs=("Release","Enu");
		}
		$bValidProject=1;
	} 
	elsif (
			   $project eq "Clients/Recherche"
			|| $project eq "Clients/Vision"
			|| $project eq "Tools/DelRegDb"
		  )
    {
		# release und enu
		@cfgs=("Dts", "DtsEnu");
		$bValidProject=1;
    }
	elsif (
			   $project eq "Capi/ISDNUnit"
			|| $project eq "SocketUnit"
			|| $project eq "Tools/LogView"
			|| $project eq "Units/NetUnit"
		  )
	{
		@cfgs=("Dts");
		$bValidProject=1;
	}
	elsif ($project eq "DV/LangDll")
	{
		# alle Sprachversionen, hier muss ggf erweitert werden
#		@cfgs=("Deu","Enu", "Esp","Ita", "Nld", "Csy", "Fra", "Ptg");
		@cfgs=("ChsU","DeuU","EnuU", "EspU","ItaU", "FraU", "PtgU","RusU","CsyU");
		$bValidProject=1;
	} 
	else 
	{
		$bValidProject=0;
		$g_iUnknownProj++;
		print "Unknown project \"$project\"\n";
		&StopOnError("Tool cancelled at unknown project");
	}

	# a valid project ? Then loop over the given configurations.
	if ($bValidProject) 
	{
		# if it´s not copy; do the nmake
		if ($g_bFinalCopy == 0) 
		{
			# some projects do not need to be compiled (DDK stuff etc.)
			if ($bNoCompile == 0) 
			{
				# Do some static code analysis (break, lock a.o.)
				# some projects are old or foreign stuff with an "individual" source code style
				# do not check anymore, because they are assumed valid and only pop useless warnings
				if (   !($project eq "Lib/codebase65")
					&& !($project eq "Tools/LogZipLib")
				   )
				{
					&DoStaticAnalysis();
					&CheckMultipleRC($dest, @cfgs);
				}

				for (@cfgs) 
				{
					if ($bTraceOneConfig)
					{
						print ("DoOne $project,$projfile,$dest\n");
					}
					&DoOneConfiguration($projfile,$dest,$_);
				}
			}
		} 
		else 
		{ # do not do the make, but do the copy
			for (@cfgs) 
			{
				if ($bTraceCopy) 
				{
					print ("DoCopy $project,$dest,$_\n");
				}
				&DoCopy($project,$dest,$_);
			}
		}	# end of copy action
	} 
	else 
	{
		print ("Invalid project:$project\n");
	}
}
#######################################################################
sub DoStaticAnalysis
{
	# perform static analysis of sourceCode
	local($checkCmd)="v:\\perl5\\bin\\checkBreak.bat "; # getcwd is appended
	$checkCmd .= getcwd;
	@checkLog = `$checkCmd`;

	# collect total line stats
	for ($ci=0;$ci <= $#checkLog;$ci++) {
		local($tmpJunk)="";
		local($restJunk)="";
		$oneLine=$checkLog[$ci];	# get first line
		$_ = $oneLine;
		if (/#STAT#/) {
			$g_sStatisticsText .= $oneLine;
			$oneLine =~ s/\s+/ /gm;
			$oneLine =~ s/^\s+//gm;
			# looks like "#STAT# 8191 lines in d:\Project\SecurityLauncher"
			($tmpJunk,$lines,$restJunk) = split(/ /,$oneLine,3);
			$g_iTotalLines += $lines;
		} elsif ( /nicht/ ) {
			# ignore
		} else {
			print ("$oneLine");
		}
		if (/ [wW]arning /) 
		{
			if (   /LNK4098/ 
				|| /LNK4089/
				)
			{
				# don't count
			}
			else
			{
				$g_iWarnings++;
			}
		}
	}
}
#######################################################################
sub DoOneConfiguration
{
	local($projfileName,$dest,$cfg)=@_;
	local($oneLine)="";
	local($ei)=0;

#	print " .... $projfileName CFG=\"$dest - Win32 $cfg\"\n";

	if ($cfg ne "PTUnitRelease") {
		&UpdateRC2();
	}

	if ($g_bUseVS71)
	{
		local($builtType) = "build";
		if ($g_bRebuildAll || $g_bRebuildProj)
		{
			$builtType = "rebuild";
		}

		print "$builtType $dest - $cfg\n";
		# SYNTAX: devenv /build ConfigName [/project ProjName] [/projectconfig ConfigName] SolutionName
		# SYNTAX: devenv solutionfile.sln  /build solutionconfig
#		local($cmdLine) = "devenv $projfileName /$builtType $cfg /out D:\\Error.txt";
#		local($cmdLine) = "devenv $projfileName /$builtType $cfg";
		local($cmdLine) = "devenv $projfileName /$builtType $cfg /project $dest";
#		local($cmdLine) = "devenv $projfileName /$builtType $cfg /project $dest /useenv";
		print "$cmdLine\n";
		@errorLog = `$cmdLine `;
	}
	else
	{
		if ($g_bRebuildAll || $g_bRebuildProj)
		{
			print "msdev.exe $projfileName /MAKE \"$dest - Win32 $cfg\" /REBUILD \n";
			@errorLog = `msdev.exe $projfileName /MAKE \"$dest - Win32 $cfg\" /REBUILD `;
		}
		else
		{
			print "msdev.exe $projfileName /MAKE \"$dest - Win32 $cfg\" ";
			@errorLog = `msdev.exe $projfileName /MAKE \"$dest - Win32 $cfg\" `;
		}
	}

	&AnalyseDevEnvOutput($project, $dest, $cfg, @errorLog);
}
######################################################################################################
sub AnalyseDevEnvOutput
{
	my($project, $dest, $cfg, @errorLog) = @_;
	my($cfgError) = 0;
	my($cfgWarning) = 0;
	my($copyFailed) = 0;

	if ($#errorLog>0)
	{
		my($bDebugging) = 0;
		my($bModifyFileNames) = 0;
		my($oneLine) = "";
		my($ei) = 0;
		
		if ($bDebugging) {
			print ("LOG: Error lines: $#errorLog\n");
		}

		for ($ei=0 ; $ei < $#errorLog ; $ei++)
		{
			$oneLine = $errorLog[$ei];	# already has newline
			# DevEnv write explicit some ASCII_NUL to the outout,
			# which skips the printing of the following line!
			# So we have to replace the ASCII_NUL with e.g. SPACE 
			$oneLine =~ s/\0//g;
			$_ = $oneLine;

			if ($bDebugging) {
				print ("LOG $ei : $oneLine");
			}

			# modify filenames, to allow easy F4 stepping ???
			# but no longer, if Post-Build Event is reached
			if (/Performing Post-Build Event.../)
			{
				$bModifyFileNames = 0;
			}
			if ($bModifyFileNames)
			{
				$oneLine = "$g_theProjectDir/$project/$oneLine";
			}

			if (
				   $_ eq ""
				|| $_ eq "\n"
				|| $_ eq "\r\n"
				|| /kopiert/ || /copied/ 
				|| /Creating lib/ 
				|| /Linking/
				|| /Generating Code/ 
				|| /Compiling/
				|| /md \.\\Autoduck/
				|| /^[A-Za-z0-9_]+\.cpp[\n\r]/i	# just a plain foo.cpp
				|| /^[A-Za-z0-9_]+\.c[\n\r]/i	# just a plain foo.c
				|| /^$/	# empty line
				|| /-----/
				|| /converting object format/
#				|| /LNK4089/ || /LNK4033/ 
				|| /Microsoft /
				|| /Build log was saved at/
				|| /Build: /
				|| /$dest - /
				)
			{
				# do not print		
				$oneLine = "";
				if ($bDebugging) {
					print ("LOG $ei : skipped\n");
				}
			}

			print ("$oneLine");					# already has newline

			# count make errors and warnings
			# always stop on fatal errors
			if (   /Invalid Command Line/
				|| / fatal error /
				)
			{
if (0)
{
				my($sMsg) = "Tool cancelled at fatal error";
				&NetSendError($sMsg);
				die "$sMsg";
}
				&StopOnError("Tool cancelled at fatal error");
			}
			if (/ error /)
			{
				$cfgError++;
				$g_iErrors++;
				&StopOnError("Tool cancelled at compiler error");
			}
			if (/ [wW]arning /)
			{
				$cfgWarning++;
				$g_iWarnings++;
			}
#			if (/ COPY FAILED /) {
#				$copyFailed++;
#				$CopyFailedCount++;
#			}
		}
	}

	local($sMsg) = "$dest - $cfg done";
	if ($cfgError || $cfgWarning || $copyFailed)
	{
		$sMsg .= ", $cfgError Error(s) $cfgWarning Warning(s)";
	}
	elsif ($copyFailed)
	{
		$sMsg .= ", COPY FAILED";
	}
	else
	{
		$sMsg .= ", OK";
	}
	print "$sMsg\n";
}
#######################################################################
sub UpdateBuild
{
	# update the BuildNumber
	print ("Updating $g_theDestinationDir/vinfo.pvi\n");
	print `copy $g_theDestinationDir\\dlls\\oemgui*.dll $g_theDestinationDir /Y`;
	`v:\\bin\\dts\\ProductVersion -updateDirectory $g_theDestinationDir`;

	# Compressing german executable
#	print "Compressing $g_theDestinationDir\\*.exe...\n";
#	`upx $g_theDestinationDir\\JView.exe`;
#	print "Ready\n";

	# Compressing german dlls
#	print "Compressing $g_theDestinationDir\\Ms_dll\\*.dll...\n";
#	`upx $g_theDestinationDir\\Ms_dll\\*.dll`;
#	print "Ready\n";

	# Compressing german dlls
#	print "Compressing $g_theDestinationDir\\Dlls\\*.dll...\n";
#	`upx $g_theDestinationDir\\Dlls\\*.dll`;
#	print "Ready\n";


# gf	chdir($theCurrentProjectDrive);	
	# NEW 16.10.98 no date/time in label
	my ($LabelText) = "$LabelPrefix$versionInfo$currentBuild";
	if ($g_bUpdateBuild)
	{
		&CollectBuildNotes();
		&CopyBuildNotes();
		# set a version label in the SourceSafe
		print "Setting SourceSafeLabel: $LabelText\n";
		`$SAFE_CMD Label \$/Project -C- -L$LabelText`;
		`$SAFE_CMD Label \$/DV -C- -L$LabelText`;
	}
	else
	{
		print ("UpdateBuild übersprungen, Label nicht gesetzt\n\n");
	}


	# safe label text in file.
	open (LABEL_OUT_FILE,">$g_theDestinationDir\\label.txt");
	print(LABEL_OUT_FILE "$LabelText\n");
	close(LABEL_OUT_FILE);

	# safe version text in file.
	open (VERSION_OUT_FILE,">$g_theDestinationDir\\version.txt");
	print(VERSION_OUT_FILE "$versionInfo$currentBuild\n");
	close(VERSION_OUT_FILE);

	chdir($g_theDestinationDir);	
	print "Creating $g_theBuildZipDir\\$currentBuild.zip\n";
    print "executing $ZIP_CMD -add -recurse -directories -silent $g_theBuildZipDir\\$currentBuild.zip $g_theDestinationDir\\*.*\n";
	`$ZIP_CMD -add -recurse -directories -silent $g_theBuildZipDir\\$currentBuild.zip $g_theDestinationDir\\*.*`;
}
#######################################################################
sub TrySourceSafe
{
	local($proj)=@_;

	if (-d "res")
	{
		# delete modified rc2
	}

	# check if dsp file has changed
	$g_bRebuildProj = 0;
	local($dspPath) = $g_theCurrentProjectDir;
	print "### $dspPath\n";
	local($dspFile) = "";
	print ("$proj\n");

	local(@arraybs)  = split(/\\/,$proj);     # Array der einzelnen Teilstrings
	# last one is project itself
	foreach $x (@arraybs)
	{
		$dspFile = $x;
	}
	local(@arrayfs)  = split(/\//,$dspFile);     # Array der einzelnen Teilstrings
	# last one is project itself
	foreach $x (@arrayfs)
	{
		$dspFile = $x;
	}
	
	print ("$dspFile\n");
	if ($dspFile eq "SecurityServer")
	{	
		# dsp file has different name than project
		$dspFile = "sec3";
	}
	if ($dspFile eq "Capi4")
	{	
		# dsp file has different name than project
		$dspFile = "capi4_32";
	}

	$dspFile .= "\.vcproj";
#	print ("$dspFile\n");
	$dspPath = "$dspPath\\$dspFile";
	print ("$dspPath\n");
	
	if (   !-f $dspPath 
	    && !(/include/)
	    )
	{
		$g_bRebuildProj = 1;
	}
	
	# save old file time
	local($time_secs_old) = (stat($dspPath))[9]; 
	print ("time_secs_old $time_secs_old\n");

	print "getting $proj Version $VersionLabel\n";

	# -R recusive
	# -I no input
	# -VL version by label

	local($OutputFile) = "vsstmp.txt"; 
	unlink($OutputFile);

	if ($VersionLabel ne "") {
		`$SAFE_CMD  Get \"$proj\" -R -I-N -O\@$OutputFile \"-VL$VersionLabel\"`;
	} else {
		`$SAFE_CMD  Get \"$proj\" -R -I-N -O\@$OutputFile`;
	}

	# get new file time	
	local($time_secs_new) = (stat($dspPath))[9]; 
	print ("time_secs_new $time_secs_new\n");

	# if file time changed -> file changed -> rebuild all
	if ($time_secs_new != $time_secs_old)
	{
		$g_bRebuildProj = 1;
	}
	print ("Rebuild $g_bRebuildProj\n");

# check if any error at SourceSafe operation
	open(HFILE, $OutputFile);
	while (<HFILE>)
	{
		if (/is not an existing filename or project/)
		{
			print ("ERROR: $_\n");
			$g_errorCount++;
		}
		if ($bTraceSourceSafe)
		{
			print ("$_");
		}
	}
	close(HFILE);
#	unlink($OutputFile);
}
#######################################################################
sub CheckMultipleRC
{
	local($bTraceDebug) = 1;
	if ($bTraceDebug) {print ("CheckMultipleRC\n");}

	local($proj, @cfgs) = @_;

	if ($#cfgs > 0)
	{
		# check for multiple RC files
		@RCFiles= `dir /b /on *.rc`;
		if ($#RCFiles > 0)
		{
			if ($TraceDebug) {print ("TEST: Project $proj\n");}
			local($deuRCFile) = $proj;
			$deuRCFile .= "\.rc";
			if ($TraceDebug) {print ("TEST: deuRCFile $deuRCFile\n");}
			# save german rc file time
			local($timeSecsDeuRC) = (stat($deuRCFile))[9]; 
			if ($TraceDebug) {print ("TEST timeSecsDeuRC $timeSecsDeuRC\n");}
			# get translation rc file time	
			
			for ($i=0;$i <= $#RCFiles;$i++)
			{
				local($oneRCFile)=$RCFiles[$i];
				$oneRCFile =~ s/[\n\r]//gm;
				$_=$oneRCFile;
				if (/\.rc$/i)
				{
					# check for valid config
					for ($j=0;$j <= $#cfgs;$j++)
					{
						local($cfg)=$cfgs[$j];
						if (/$cfg\.rc$/i)
						{
							local($timeSecsOneRC) = (stat($oneRCFile))[9]; 
							if ($TraceDebug) {print ("TEST $oneRCFile timeSecsOneRC $timeSecsOneRC\n");}
							# if file time OneRC < file time DeuRC -> check translation!
							if ($timeSecsOneRC < $timeSecsDeuRC)
							{
								$g_iWarnings++;
								print ("Warning: Check translation $oneRCFile\n");
								push(@g_CheckTranslations, $oneRCFile);
							}
							else
							{
								if ($TraceDebug) {print ("TEST translation OK $oneRCFile\n");}
							}
						}
						else
						{
							if ($TraceDebug) {print ("TEST $cfg not found in $oneRCFile\n");}
						}
					}
				}
				else
				{
					if ($TraceDebug) {print ("TEST OOPS NON-RC file? $oneRCFile\n");}
				}
			}
		}
		else
		{ # only one rc file
			if ($TraceDebug) {print ("TEST: Project $proj has only one RC\n");}
		}
	}
	else
	{ # only one configuration
		if ($TraceDebug) {print ("TEST: Project $proj has only one cfg $cfgs[0]\n");}
	}
}
#######################################################################
sub CreateNewDir
{
	local($bTraceDebug) = 0;
	local($sDir)=@_;
	print "CreateSubDirs $sDir\n";
	local(@subdirs) = split("/",$sDir);
	foreach (@subdirs)
	{
		# special check for D:/
		if (! -d $_ && !/:/)
		{
			# subdirectories can exists like Units/SimUnits
			if (chdir($_))
			{
				if ($bTraceDebug) {print "Already existing $_\n";}
			}
			else
			{
				if ($bTraceDebug) {printf ("Create $_ in %s\n",getcwd);}
				system("mkdir $_");
				if ($bTraceDebug) {print ("Created $_\n");}
			}
		}
		if (-d $_ || /:/ )
		{
			if ($bTraceDebug) {print "Change drive to $_\n";}
			chdir ("$_\\");	# absolute path, back to root dir
		}
		elsif (/:/)
		{
			if ($bTraceDebug) {print "Change to root $_\\\n";}
			chdir ("$_\\");	# absolute path, back to root dir
		}
		else
		{
			print ("Failed $_\n");
		}
	}
}
#######################################################################
sub DoCopy
{
	local($project, $dest, $config) = @_;
	local($destinationDir) = $g_theDestinationDir;
	my ($bTraceDebug) = 1;
	my ($source) = "";
	my ($finalDest) = "";

	if ($bTraceCopy) {print ("DoCopy Project:$project, Dest:$dest, Config:$config\n");}

	# make the destination dependend from the configuration
	$destinationDir = $g_theDestinationDir;

	# print "COPYING $project\n";

	if (   $config eq "Enu"
		|| $config eq "EnuUnicode"
		|| $config eq "DtsEnu")
	{
		if (   $project ne "DV/LangDll")
		{
			print "ENGLISH DoCopy $project $dest $config\n";
			$destinationDir = $g_theDestinationDirEnu;
		}
	}

	# create non existing dir
	if (! -d "$destinationDir")
	{
		`mkdir $destinationDir`;
	}

	local ($bDoCopy)=1;

	$_ = $project;

	# CAVEAT3 jedes Project das keine EXE ergibt oder sonstwie speziell ist
	# muss hier entsprechend eingetragen sein!

	if (	  $project eq "Capi/Capi4"
		   || $project eq "DirectShow/Filters/baseclasses"
		   || $project eq "Lib/WKClasses"
		   || $project eq "Lib/codebase65"
		   || $project eq "Lib/WKClasses"
		   || $project eq "Tools/LogZipLib"
		  )
	{
		$bDoCopy=0;
	}
	elsif ($project eq "Cipc")
	{
		if ($g_bUseVS71)
		{
			$dest .= "U";
		}
		$source = "$g_theProjectDir\\$project\\$config\\$dest.dll";
		$finalDest = "$destinationDir\\Dlls\\$dest.dll";
	}
	elsif ($project eq "DirectShow/Common")
	{
		if ($g_bUseVS71)
		{
			$source = "$g_theProjectDir\\DirectShow\\Lib\\CommonDirectShowU.dll";
			$finalDest = "$destinationDir\\Dlls\\CommonDirectShowU.dll";
		}
		else
		{
			$source = "$g_theProjectDir\\DirectShow\\Lib\\CommonDirectShow.dll";
			$finalDest = "$destinationDir\\Dlls\\CommonDirectShow.dll";
		}
	}
	elsif (/DirectShow\/Filters/)
	{
		$source = "$g_theProjectDir\\$project\\$config\\$dest.ax";
		$finalDest = "$destinationDir\\Dlls\\$dest.ax";
	}
	elsif (	  $project eq "DV/Lib/Hooks"
		  )
	{
		if ($g_bUseVS71)
		{
			$dest .= "U";
			$source = "$g_theProjectDir\\DV\\Lib\\$dest\.dll";
			$finalDest = "$destinationDir\\Dlls\\$dest\.dll";
		}
		else
		{
			$source = "$g_theProjectDir\\DV\\Lib\\$dest.dll";
			$finalDest = "$destinationDir\\Dlls\\$dest.dll";
		}
	}
	elsif ($project eq "DV/LangDll")
	{
		local($langdll) = "dvdeu.dll";
		if ($g_bUseVS71)
		{
			$langdll  = "DVU";
			$langdll .= substr($config, 0, 3);
			$langdll .= ".dll";
			$source = "$g_theProjectDir\\$project\\$config\\$langdll";
			$finalDest = "$destinationDir\\Dlls\\$langdll";
		}
		else
		{
			$langdll = "dv$config.dll";
			$source = "$g_theProjectDir\\$project\\$config\\$langdll";
			$finalDest = "$destinationDir\\Dlls\\$langdll";
		}
	}
	elsif (	  $project eq "Lib/Crypto"
		   || $project eq "Lib/DeviceDetect"
		   || $project eq "Lib/HardwareInfo"
		   || $project eq "Lib/ICPCONDll"
		   || $project eq "Lib/JpegLib"
		   || $project eq "Lib/NetComm"
		   || $project eq "Lib/NFC"
		   || $project eq "Lib/Skins"
		   || $project eq "Lib/VImage"
		  )
	{
		if ($g_bUseVS71)
		{
			$dest .= "U";
			if (   $config eq "EnuUnicode"
				|| $config eq "ItaUnicode"
				)
			{
				$source = "$g_theProjectDir\\$project\\$config\\$dest\.dll";
				$finalDest = "$destinationDir\\Dlls\\$dest\.dll";
			}
			else
			{
				$source = "$g_theProjectDir\\Lib\\$dest\.dll";
				$finalDest = "$destinationDir\\Dlls\\$dest\.dll";
			}
		}
		else
		{
			if (   $config eq "Enu"
				|| $config eq "Ita"
				)
			{
				$source = "$g_theProjectDir\\$project\\$config\\$dest.dll";
				$finalDest = "$destinationDir\\Dlls\\$dest.dll";
			}
			else
			{
				$source = "$g_theProjectDir\\Lib\\$dest.dll";
				$finalDest = "$destinationDir\\Dlls\\$dest.dll";
			}
		}
	}
	elsif ( $project eq "Lib/mpeg4spenc")
	{
		$source = "$g_theProjectDir\\Lib\\$dest\.dll";
		$finalDest = "$destinationDir\\Dlls\\$dest\.dll";
	}
	elsif (   $project eq "Lib/oemgui"
		  )
	{
		# mal was neues mit Sprachen-DLL
		# mal was neues mit Sprachen-DLL
		if ($g_bUseVS71)
		{
			my($destDll) = "";
			# als erstes die DLL
			$destDll = $dest;
			$destDll .= "U.dll";
			$source = "$g_theProjectDir\\Lib\\$destDll";
			$finalDest = "$destinationDir\\Dlls\\$destDll";
			# die deutsche Resource
			$destDll = $dest;
			$destDll .= "Deu.dll";
			&MyDoCopy("$g_theProjectDir\\Lib\\$destDll", "$destinationDir\\Dlls\\$destDll", 1);
			# die englische Resource
			$destDll = $dest;
			$destDll .= "Enu.dll";
			&MyDoCopy("$g_theProjectDir\\Lib\\$destDll", "$destinationDir\\Dlls\\$destDll", 1);
		}
		else
		{
			if (   $config eq "Enu"
				|| $config eq "Ita"
				)
			{
				$source = "$g_theProjectDir\\$project\\$config\\$dest.dll";
				$finalDest = "$destinationDir\\Dlls\\$dest.dll";
			}
			else
			{
				$source = "$g_theProjectDir\\Lib\\$dest.dll";
				$finalDest = "$destinationDir\\Dlls\\$dest.dll";
			}
		}
	}
	elsif ($project eq "Clients/Vision")
	{
		$source = "$g_theProjectDir\\$project\\$config\\Vision.exe";
		$finalDest = "$destinationDir\\Vision.exe";
	}
	elsif ($project eq "DV/KeyHook")
	{
		$source = "$g_theProjectDir\\$project\\bin\\KeyHook.sys";
		$finalDest = "$destinationDir\\KeyHook.sys";
	}
#	elsif (   $project eq "SystemVerwaltung"
#		   && $g_bUseVS71
#		  )
#	{
#		# mal was neues mit Sprachen-DLL
#		if ($config eq "Release")
#		{
#			my($destDll) = "";
#			# die deutsche Resource
#			$destDll = $dest;
#			$destDll .= "Deu.dll";
#			&MyDoCopy("$g_theProjectDir\\$project\\Release\\$destDll", "$destinationDir\\$destDll", 1);
#			# die englische Resource
#			$destDll = $dest;
#			$destDll .= "Enu.dll";
#			&MyDoCopy("$g_theProjectDir\\$project\\Release\\$destDll", "$destinationDir\\$destDll", 1);
#
#			# das Programm an sich
#			$source = "$g_theProjectDir\\$project\\$config\\$dest.exe";
#			$finalDest = "$destinationDir\\$dest.exe";
#		}
#		else
#		{
#			$bDoCopy=0;
#		}
#	}
	elsif ($project eq "Units/JaCob/JaCobVxd")
	{
		$source = "$g_theProjectDir\\$project\\bin\\jacob.vxd";
		$finalDest = "$destinationDir\\JaCob.vxd";
	}
	elsif ($project eq "Units/JaCob/JaCobSys")
	{
		$source = "$g_theProjectDir\\$project\\bin\\JaCob.sys";
		$finalDest = "$destinationDir\\JaCob.sys";
		&MyDoCopy("$g_theProjectDir\\Units\\JaCob\\JaCobUnit\\JaCob.inf","$destinationDir\\JaCob.inf",1);	# do count errors
	}
	elsif ($project eq "Units/SaVic2/SaVicSys")
	{
		$source = "$g_theProjectDir\\$project\\bin\\SaVic.sys";
		$finalDest = "$destinationDir\\SaVic.sys";
#		&MyDoCopy("$g_theProjectDir\\Units\\SaVic2\\SaVicSys\\Bin\\SaVic.inf","$destinationDir\\SaVic.inf",1);	# do count errors
		&MyDoCopy("$g_theProjectDir\\$project\\Bin\\SaVic.inf","$destinationDir\\SaVic.inf",1);	# do count errors
	}
	elsif ($project eq "Units/Tasha/TashaSys")
	{
		if ($g_bUseVS71)
		{
			$source = "$g_theProjectDir\\$project\\bin\\Tasha.sys";
			$finalDest = "$destinationDir\\Tasha.sys";
			&MyDoCopy("$g_theProjectDir\\$project\\Bin\\mdsad301.inf","$destinationDir\\mdsad301.inf",1);	# do count errors
			&MyDoCopy("$g_theProjectDir\\$project\\Bin\\mdsad301.sys","$destinationDir\\mdsad301.sys",1);	# do count errors
			&MyDoCopy("$g_theProjectDir\\$project\\Bin\\mdsdsp.dll",  "$destinationDir\\mdsdsp.dll",1);	# do count errors
			&MyDoCopy("$g_theProjectDir\\Units\\Tasha\\EPLD\\Bin\\Tasha.rbf",  "$destinationDir\\Tasha.rbf",1);	# do count errors
		}
		else
		{
			$bDoCopy = 0;
		}
	}
	elsif ($project eq "Units/Tasha/TashaFirmware")
	{
		if ($g_bUseVS71)
		{
			&MyDoCopy("$g_theProjectDir\\Units\\Tasha\\TashaFirmware\\BootLoader533\\Bin\\TashaBootLoader533.ldr",  "$destinationDir\\TashaBootLoader533.ldr",1);	# do count errors
			&MyDoCopy("$g_theProjectDir\\Units\\Tasha\\TashaFirmware\\TashaFirmware533\\Bin\\TashaFirmware533_BF0.ldr",  "$destinationDir\\TashaFirmware533_BF0.ldr",1);	# do count errors
			&MyDoCopy("$g_theProjectDir\\Units\\Tasha\\TashaFirmware\\TashaFirmware533\\Bin\\TashaFirmware533_BF1.ldr",  "$destinationDir\\TashaFirmware533_BF1.ldr",1);	# do count errors
			&MyDoCopy("$g_theProjectDir\\Units\\Tasha\\TashaFirmware\\TashaFirmware533\\Bin\\TashaFirmware533_BF2.ldr",  "$destinationDir\\TashaFirmware533_BF2.ldr",1);	# do count errors
			&MyDoCopy("$g_theProjectDir\\Units\\Tasha\\TashaFirmware\\TashaFirmware533\\Bin\\TashaFirmware533_BF3.ldr",  "$destinationDir\\TashaFirmware533_BF3.ldr",1);	# do count errors
			&MyDoCopy("$g_theProjectDir\\Units\\Tasha\\TashaFirmware\\TashaFirmware533\\Bin\\TashaFirmware533_BF4.ldr",  "$destinationDir\\TashaFirmware533_BF4.ldr",1);	# do count errors
			&MyDoCopy("$g_theProjectDir\\Units\\Tasha\\TashaFirmware\\TashaFirmware533\\Bin\\TashaFirmware533_BF5.ldr",  "$destinationDir\\TashaFirmware533_BF5.ldr",1);	# do count errors
			&MyDoCopy("$g_theProjectDir\\Units\\Tasha\\TashaFirmware\\TashaFirmware533\\Bin\\TashaFirmware533_BF6.ldr",  "$destinationDir\\TashaFirmware533_BF6.ldr",1);	# do count errors
			&MyDoCopy("$g_theProjectDir\\Units\\Tasha\\TashaFirmware\\TashaFirmware533\\Bin\\TashaFirmware533_BF7.ldr",  "$destinationDir\\TashaFirmware533_BF7.ldr",1);	# do count errors
			&MyDoCopy("$g_theProjectDir\\Units\\Tasha\\TashaFirmware\\TashaFirmware535\\Bin\\TashaFirmware535.ldr",  "$destinationDir\\TashaFirmware535.ldr",1);	# do count errors
		}
		$bDoCopy = 0;
	}
	elsif (   $project eq "Units/JaCob/JaCobDA"
		   || $project eq "Units/SaVic2/SaVicDA"
		   || $project eq "Units/Tasha/TashaDA"
		   )
	{
		if ($g_bUseVS71)
		{
			$dest .= "U";
			$source = "$g_theProjectDir\\$project\\$config\\$dest\.dll";
			$finalDest = "$destinationDir\\Dlls\\$dest\.dll";
		}
		else
		{
			$source = "$g_theProjectDir\\$project\\$config\\$dest.dll";
			$finalDest = "$destinationDir\\Dlls\\$dest.dll";
		}
	}
	elsif ($project eq "Units/JaCob/JaCobDll")
	{
		if ($g_bUseVS71)
		{
			$source = "$g_theProjectDir\\$project\\$config\\JaCobU.dll";
			$finalDest = "$destinationDir\\Dlls\\JaCobU.dll";
		}
		else
		{
			$source = "$g_theProjectDir\\$project\\$config\\JaCob.dll";
			$finalDest = "$destinationDir\\Dlls\\JaCob.dll";
		}
	}
	elsif ($project eq "Units/SaVic2/SaVicDll")
	{
		if ($g_bUseVS71)
		{
			$source = "$g_theProjectDir\\$project\\$config\\SaVicU.dll";
			$finalDest = "$destinationDir\\Dlls\\SaVicU.dll";
		}
		else
		{
			$source = "$g_theProjectDir\\$project\\$config\\SaVic.dll";
			$finalDest = "$destinationDir\\Dlls\\SaVic.dll";
		}
	}
	elsif (   $project eq "Units/Tasha/TashaDll"
		   )
	{
		if ($g_bUseVS71)
		{
			$source = "$g_theProjectDir\\$project\\$config\\TashaU.dll";
			$finalDest = "$destinationDir\\Dlls\\TashaU.dll";
		}
		else
		{
			$bDoCopy = 0;
		}
	}
	elsif ($project eq "Units/MiCo/MiCoDA")
	{
		$source = "$g_theProjectDir\\$project\\Release\\$dest.dll";
		$finalDest = "$destinationDir\\Dlls\\$dest.dll";
	}
	elsif ($project eq "Units/MiCo/MiCoSys")
	{
		$source = "$g_theProjectDir\\$project\\bin\\MiCoPCI.sys";
		$finalDest = "$destinationDir\\MiCoPCI.sys";
	}
	elsif ($project eq "Units/MiCo/MiCoVxd")
	{
		$source = "$g_theProjectDir\\$project\\bin\\micopci.vxd";
		$finalDest = "$destinationDir\\micopci.vxd";
	}
#	elsif ($project eq "Units/NetUnit")
#	{
#		# ein halber special fall wegen OutputFileName
#		$source = "$g_theProjectDir\\$project\\$config\\SocketUnit.exe";
#		$finalDest = "$destinationDir\\NetUnit.exe";
#	}
	elsif ($project eq "Tools/VipCleanNT")
	{
		$source = "$g_theProjectDir\\$project\\$config\\FactoryReset.exe";
		$finalDest = "$destinationDir\\FactoryReset.exe";
	}
	else
	{
		#
		# .exe 
		#
		$source = "$g_theProjectDir\\$project\\$config\\$dest.exe";
		$finalDest = "$destinationDir\\$dest.exe";
	}
	
	if ($bDoCopy)
	{

		# .exe or .dll first
		print "MyDoCopy $project\n";
		&MyDoCopy($source, $finalDest, 1);	# do count errors

		# followed by.map
		#

		# destination is Maps, but not CoCo/Maps
		$destinationDir .= "\\Maps";
		# remove coco dir
		$destinationDir =~ s/\\CoCo\\/\\/;

		# create if not already there
		if (! -d "$destinationDir")
		{
			`mkdir $destinationDir`;
		}
		
		$source =~ s/\.exe/.map/;
		$source =~ s/\.dll/.map/;

		$_ = $project;
		if (/Clients\\Vision/) {
			$dest = "Vision40";
		} elsif (
			/oemgui/ || 
			/JpegLib/ ||
			/HardwareInfo/ || 
			/DeviceDetect/ || 
			/VImage/ ||
			/mpeg4spenc/ ||
			/h263enc/ || 
			/h263dec/ ) 
		{
		# .map are not in Lib/ but in their Release/ directories
			$source = "$project\\$config\\$dest.map";
		} elsif ( /MiCoSys/ || /MiCoVxd/ || /JaCobSys/ || /JaCobVxD/ || /SaVicSys/ || /AkuSys/ || /KeyHook/) {
			$source = "NO COPY"; # no map file
		}

#ML		$finalDest = "$destinationDir\\maps\\$dest.map";
		$finalDest = "$destinationDir\\$dest.map";

		$_ = $source;

		if (/\.map/) {
			if (!(-f $source)) {
				$source =~ s/\.map/.pdb/;
				$finalDest =~ s/\.map/.pdb/;
				# print "using pdb instead of map $source \n";
			}
		}

		if (! ($source eq "NO COPY") ) {
			&MyDoCopy($source,$finalDest,0);	# do NOT count errors
		} else {
			print ("NO copy of $finalDest\n");
		}
	} 
	else 
	{
		# do not copy, static libs for example
	}
}
#######################################################################
# usage:
# &MyDoCopy("Source","Dest",bDoCountErrors);
sub MyDoCopy
{
	local($source, $finalDest, $bDoCountError) = @_;
	local($copyError) = "";
	my ($bTraceDebug) = 1;

	print ("Copy $source --> $finalDest\n");

	if (-f $source)
	{
		$cmd = "copy $source $finalDest";
		if ($bTraceDebug) {print ("Copy cmd 1 $cmd\n");}
		$cmd =~ s/\//\\/gm;
		if ($bTraceDebug) {print ("Copy cmd 2 $cmd\n");}
		$copyError = `$cmd`;

		$_ = $copyError;
		if (/0 Datei/)
		{
			$copyError =~ s/\s+0 Datei.*\n//;
			print ("$copyError");
			if ($bDoCountError)
			{
				$g_iCopyErrors++;
				print ("ERROR DURING COPY\n");
				&StopOnError("ERROR DURING COPY $copyError");
			}
			else
			{
				print ("Error not counted \n");
			}
		}
		else
		{ # ok, may be remove write protection here for all copied files?
			chmod 0755, "$finalDest";
		}
	}
	else
	{
		if ($bDoCountError) 
		{
			$g_iCopyErrors++;
			print "File not found, $source\n";
			print ("ERROR DURING COPY  $copyError\n");
			&StopOnError("ERROR DURING COPY $source not found");
		} 
		else 
		{
			print "File not found, $source, but not counted\n";
		}
	}
}
#######################################################################
sub CopyBuildNotes
{
	$source = "$g_theDestinationDir\\BuildNotes$currentBuild.htm";
	$dest   = "\\\\comm01\\buildnotes\\dts\\BuildNotes$currentBuild.htm";
	print `copy $source $dest`;
	print ("CopyBuildNotes done.\n");
}
#######################################################################
sub CollectBuildNotes
{
	$filename = "$g_theDestinationDir\\BuildNotes$currentBuild.htm";
	&DeleteEvenIfRO($filename);
	open (OFILE,">>$filename");

	print ("BuildNumber $currentBuild at $mday.$month.$year, $hour:$min\n");
	$title = "BuildNumber $currentBuild at $mday.$month.$year, $hour:$min";
	print (OFILE "<HTML>\n");
	print (OFILE "<TITLE>$title</TITLE>\n");
	print (OFILE "<BODY>\n");
	print (OFILE "<H1>$title</H1>\n");
	print (BNFILE "$title\n\n");
	print ("Scanning for history.txt...\n");
	# add CIPC to history projects
	# NOT YET take all dep projects
	
	local(@allProjects);

	# collect all projects

	for (@LibProjects) {
		push(@allProjects,$_);
	}

	for (@Projects) {
		push(@allProjects,$_);
	}

	my ($prevLabelText) = "";
	open (LABEL_IN_FILE,"<$g_theDestinationDir\\label.txt");
	while (<LABEL_IN_FILE>)
	{
		$prevLabelText = "$prevLabelText$_";
	}
	close(LABEL_IN_FILE);
	$prevLabelText =~  s/\n//;
	$prevLabelText =~  s/\r//;

	print "Previous Label is <$prevLabelText>\n";

	for (@allProjects) 
	{
		local($project);
		$project=$_;
		chdir("$g_theProjectDir/$project");
		if ($bTraceSuperHistory)
		{
			printf("Searching history for $project in %s...\n",getcwd());
		}

		if (-f "history.txt") 
		{
			local($projectSS) = "\$/Project/$project";
			if (/DV/)
			{
				$projectSS = "\$/$project";
			}

			unlink("SuperHistory.txt");

			# -IECW ignore case/end of line/white space changes
			print "Diff $projectSS/history.txt -DS200 -IECW -VL$prevLabelText > SuperHistory.txt\n";
			print `$SAFE_CMD Diff $projectSS/history.txt -DS200 -IECW -VL$prevLabelText > SuperHistory.txt`;

			# now superHistory.txt is created
			# collect between the topmost Label:"Build
			# from this take only Chg: To : Ins:
			local($bAlreadyDidPrint)=0;
			local($bAlreadyDidPrintText)=0;
			if (-f "SuperHistory.txt")
			{
				open(HFILE,"SuperHistory.txt") || warn "can't open SuperHistory.txt\n";
				if ($bTraceSuperHistory)
				{
					printf ("Scanning SuperHistory.txt in %s...\n",getcwd());
				}
				while (<HFILE>)
				{
					if (   /^Against:/ 
						|| /^Diffing:/)
					{
						# ignore
					}
					elsif (/No differences/)
					{
						# ignore
					}
					elsif (/\w/)
					{
						# at least one word
						# inTop and important stuff
						if ($bAlreadyDidPrint==0)
						{
							# print 'file header'
							print ("----- $project/history.txt -----\n");
							print (OFILE "<B>$project/history.txt</B>\n"); # no need for a break here, foolowing line have a <BR> at the beginning
							$bAlreadyDidPrint=1;
							$bAlreadyDidPrintText=0;
						}
						$_ =~ s/</&lt\;/g;
						$_ =~ s/>/&gt\;/g;

						# drop leading line numbers
						$_ =~ s/^\s*\d+\s+//;
						# convert Ins: to whitespace
						$_ =~ s/\s*Ins:/<BR> /;
						$_ =~ s/\s*To:/ <BR>To:/;
						$_ =~ s/\s*Del:/ <BR>Del:/;
						$_ =~ s/\s*Change:\s+$/<BR>Chg: &lt;empty line&gt;/;
						$_ =~ s/\s*Change:/<BR>Chg:/;

						# escape HTML specials
						$_ =~ s/\x81/&uuml;/g;	# ue
						$_ =~ s/\x84/&auml;/g;	# ae
						$_ =~ s/\x94/&ouml;/g;	# oe
						$_ =~ s/\xE1/ss/g;		# sz

						# CAVEAT these are never returned from ss.exe
						$_ =~ s/ä/&auml;\;/g;
						$_ =~ s/ö/&ouml;\;/g;
						$_ =~ s/ü/&uuml;\;/g;

						$_ =~ s/Ä/&Auml;\;/g;
						$_ =~ s/Ö/&Ouml;\;/g;
						$_ =~ s/Ü/&Uuml;\;/g;


						if (/^\s*$/ && $bAlreadyDidPrintText==0)
						{
							# drop leading blank lines
						}
						else
						{
							print ($_);
							print (OFILE $_);
						}
					}
					else
					{
						# empty line, not a single word
					}
				}

				if ($bAlreadyDidPrint)
				{
					print (OFILE "<BR>\n");
				}
				close(HFILE);
				unlink("SuperHistory.txt");
			}
			else
			{
				print "SuperHistory.txt does not exist\n"
			}
		} 
		else 
		{
			print ("no history.txt found for $project\n");
		}
	}

	close (OFILE);

}
#######################################################################
sub DeleteEvenIfRO
{
	local($oneFile,$bDoMsg)=@_;

	if ($bTraceUnlink) {
		printf ("DeleteEvenIfRO for $oneFile, in %s, msg $bDoMsg\n",getcwd());
	}

	if (-f "$oneFile") {
		# remove write protectection
		chmod 0755, "$oneFile";
		unlink ("$oneFile");

		if (-f "$oneFile") {
			print ("OOPS Still have $oneFile\n");
		} else {
			# print ("Removed $oneFile\n");
		}
	} else {
		if ($bDoMsg) {
			print ("DeleteEvenIfRO: $oneFile not found\n");
		}
	}

}
#######################################################################
sub UpdateRC2
{
	if (-d "res") {
		# WK_VERSION(3, 12, 13, 43, "03/12/98,13:43\0", // @AUTO_UPDATE
		chdir("res");
		
		$rcFile="$dest.rc2";
		$_ = $rcFile;
		if ( /MiCoUnitPCI/ ) {
			$rcFile =~ s/MiCoUnitPCI/MiCoUnit/;
		}
		if ( /JaCobUnit/ ) {
			$rcFile =~ s/JaCobUnit/JaCobUnit/;
		}
		if ( /JaCobTest/ ) {
			$rcFile =~ s/JaCobTest/JaCobTest/;
		}
		if ( /AudioUnit/ ) {
			$rcFile =~ s/AudioUnit/AudioUnit/;
		}
		if ( /SaVicUnit/ ) {
			$rcFile =~ s/SaVicUnit/SaVicUnit/;
		}
		if ( /Convrt32/ ) {
			$rcFile =~ s/Convrt32/Convert/;
		}
		if ( /MegraV2/ ) {
			$rcFile =~ s/MegraV2/Megra/;
		}


		if (-f "$rcFile" ) {
			# print ("updating $rcFile\n");
			chmod 0755, $rcFile; 
			# alt WK_VERSION(3, 12, 13, 43, "03/12/98,13:43\0", // @AUTO_UPDATE
			# neu WK_VERSION(1, 0, 0, 194, "1.0.0.194\0", // @AUTO_UPDATE
			`v:\\perl5\\bin\\superReplace.bat -updateBuild "WK_VERSION.*\@AUTO_UPDATE" "WK_VERSION($VersionMajor, $VersionMinor, $VersionSubIndex, $currentBuild, \\"$versionInfo$currentBuild\\0\\", // \@AUTO_UPDATE"`;
#			`v:\\perl5\\bin\\superReplace.bat -updateBuild "\\".*\\".\\s*// \@AUTO_UPDATE" "\\"$versionInfo$currentBuild\\0\\", // \@AUTO_UPDATE"`;
			chmod 0555, $rcFile;
		}
		else
		{
			print "no rc2 file found $rcFile\n";
		}

		chdir("..");
	}
	else
	{
		print "no res dir found\n"; 
	}

}
#######################################################################
sub ReadBuildNumber
{
	if ($buildNumber==0) {
		local($tmpVersion)=0;
		local($tmpNumRecord)=0;

		# scan the first line of vinfo.pvi
		open(FILE,"$g_theDestinationDir/vinfo.pvi") || warn "can't open vinfo.pvi in $g_theDestinationDir\n";
		while (<FILE>) {
			if (/\d+\s+\d+\s+\d+/) {
				# first line in vinfo.pvi is "1 10 28" format build numRecords
				$_ =~ s/[\ \t]+/ /gm;	# kill multiple blanks to do the split
				($tmpVersion,$buildNumber,$tmpNumRecord) = split(/ /,$_,3);
			}
		} # EOF vinfo.pvi
		close(FILE);
	} else {
		print ("using preset buildNumber $buildNumber\n");
	}
	$currentBuild = $buildNumber+1;
}
#######################################################################
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
#######################################################################
sub SearchMSDEV
{
	###############################################################################
	#                                                                             #
	# CAVEAT: Perl does not support pathes with spaces, so do not add those here! #
	#                                                                             #
	###############################################################################

	if ($g_bUseVS71)
	{
		# the one and only allowed!
		if ( -d "C:/MSDev2003/Vc7" ) {
			$MSDEV_DIR="C:\\MSDev2003\\Vc7";
			$MSDEV_IDE_DIR="C:\\MSDev2003\\Common7\\IDE";
		} else {
			&NetSendError("MSDEV not found!");
			die "MsDev not found\n";
		}
	}
	else
	{
		if ( -d "C:/devstudio/vc" ) {
			$MSDEV_DIR="C:\\devstudio\\vc";
		} elsif ( -d "c:/msdev/vc" ) {
			$MSDEV_DIR="c:\\msdev\\vc";
		} elsif ( -f "c:/msdev/common/msdev98/bin/msdev.exe" ) {
			$MSDEV_DIR="c:\\msdev\\vc98";
		} elsif (-d "E:/devstudio/vc" ) {
			$MSDEV_DIR="E:\\devstudio\\vc";
		} elsif ( -d "F:/devstudio/vc" ) {
			$MSDEV_DIR="F:\\devstudio\\vc";
		} elsif ( -d "d:/msdev98/vc" ) { # CAVEAT order is important. special 6.0 hack
			$MSDEV_DIR="d:\\msdev98\\vc";
		} elsif ( -d "c:/msdev98/vc98" ) { # CAVEAT order is important. special 6.0 hack
			$MSDEV_DIR="c:\\msdev98\\vc98";
		} else {
			&NetSendError("MSDEV not found!");
			die "MsDev not found\n";
		}
	}
	print "Found $MSDEV_DIR\n";
}	# end of SearchMSDEV
#######################################################################
sub SearchSS
{
	###############################################################################
	#                                                                             #
	# CAVEAT: Perl does not support pathes with spaces, so do not add those here! #
	#                                                                             #
	###############################################################################

	if ( -f "C:/VS_60/Common/Vss/win32/ss.exe" ) {
		$SAFE_CMD="c:\\VS_60\\Common\\Vss\\win32\\ss.exe";
	} elsif ( -f "C:/vss/win32/ss.exe" ) {
		$SAFE_CMD="c:\\vss\\win32\\ss.exe";
	} elsif ( -f "c:/msdev/vss/win32/ss.exe" ) {
		$SAFE_CMD="C:\\msdev\\vss\\win32\\ss.exe";
	} elsif ( -f "c:/msdev98/common/vss/win32/ss.exe" ) {
		$SAFE_CMD="C:\\msdev98\\common\\vss\\win32\\ss.exe";
	} elsif ( -f "C:/Msdev60/Common/VSS/win32/ss.exe" ) {
		$SAFE_CMD="C:\\Msdev60\\Common\\VSS\\win32\\ss.exe";
	} elsif ( -f "c:/msdev/common/vss/win32/ss.exe" ) {
		$SAFE_CMD="C:\\msdev\\common\\vss\\win32\\ss.exe";
	} elsif ( -f "D:/vss/win32/ss.exe" ) {
		$SAFE_CMD="D:\\vss\\win32\\ss.exe";
	} elsif (-f "E:/vss/win32/ss.exe" ) {
		$SAFE_CMD="E:\\vss\\win32\\ss.exe";
	} elsif ( -f "F:/vss/win32/ss.exe" ) {
		$SAFE_CMD="F:\\vss\\win32\\ss.exe";
	} else {
		&NetSendError("SourceSafe not found");
		die "SourceSafe not found\n";
	}
	print "SourceSafe is $SAFE_CMD\n";
}
#######################################################################
sub SetEnv
{
	# set env variables, not using V:
	# CAVEAT requires $g_theProjectDir
	
	my ($TmpInclude) = "";
	my ($TmpLib) = "";
	my ($TmpPath) = "";

	#*******************************************
	if ($g_bUseVS71)
	{
		$TmpInclude = "$MSDEV_DIR\\Include;$MSDEV_DIR\\atlmfc\\include;$MSDEV_DIR\\atlmfc\\src\\mfc;$MSDEV_DIR\\PlatformSDK\\include\\prerelease;$MSDEV_DIR\\PlatformSDK\\include;";
		$TmpInclude .= "$g_theProjectDir\\Include;$g_theProjectDir\\Cipc;$g_theProjectDir\\lib;$g_theProjectDir\\Dv\\lib;";
		if (-d "C:/MSDev2003/SDK/v1.1/include")
		{
			$TmpInclude .= "C:\\MSDev2003\\SDK\\v1.1\\include;";
		}
	}
	else
	{
		$TmpInclude = "$MSDEV_DIR\\Include;$MSDEV_DIR\\mfc\\include;$MSDEV_DIR\\atl\\include;$MSDEV_DIR\\mfc\\src;";
		$TmpInclude .= "$g_theProjectDir\\Include;$g_theProjectDir\\Cipc;$g_theProjectDir\\lib;$g_theProjectDir\\Dv\\lib;";
		if (-d "d:/Microsoft SDK/Include")
		{
			$TmpInclude .= "d:\\Microsoft SDK\\Include;";
		}
		elsif (-d "c:/Microsoft SDK/Include")
		{
			$TmpInclude .= "c:\\Microsoft SDK\\Include;";
		}
	}
	$ENV{'INCLUDE'} = $TmpInclude;
	print "INCLUDE is: $TmpInclude\n";

	#*******************************************
	if ($g_bUseVS71)
	{
		$TmpLib = "$MSDEV_DIR\\lib;$MSDEV_DIR\\atlmfc\\lib;;$MSDEV_DIR\\PlatformSDK\\lib\\prerelease;$MSDEV_DIR\\PlatformSDK\\lib;";
		$TmpLib .= "$g_theProjectDir\\Lib;$g_theProjectDir\\Cipc\\ReleaseUnicode;$g_theProjectDir\\Cipc\\Release;$g_theProjectDir\\DV\\Lib;";
		if (-d "C:/MSDev2003/SDK/v1.1/lib")
		{
			$TmpInclude .= "C:\\MSDev2003\\SDK\\v1.1\\lib;";
		}
	}
	else
	{
		$TmpLib = "d:\\inetsdk\\lib;$MSDEV_DIR\\lib;$MSDEV_DIR\\mfc\\lib;$g_theProjectDir\\Lib;";
		$TmpLib .= "$g_theProjectDir\\Cipc\\Release;$g_theProjectDir\\DV\\Lib;";
		if (-d "d:/Microsoft SDK/Lib")
		{
			$TmpLib .= "d:\\Microsoft SDK\\Lib;";
		}
		elsif (-d "c:/Microsoft SDK/Lib")
		{
			$TmpLib .= "c:\\Microsoft SDK\\Lib;";
		}
	}
	$ENV{'LIB'} = $TmpLib;
	print "LIB is: $TmpLib\n";
	
	#*******************************************
	$TmpPath = $ENV{'PATH'};
	print "Original PATH is $TmpPath\n";
	if ($g_bUseVS71)
	{
		$_ = $TmpPath;
		if ( !/$MSDEV_IDE_DIR/)
		{
			print "Append $MSDEV_IDE_DIR to PATH\n";
			$TmpPath .= ";$MSDEV_IDE_DIR;"
		}
	}
	else
	{
#		if (-d "c:\\Msdev98") {
#			$ENV{'PATH'} = "$MSDEV_DIR\\bin;C:\\Winnt;C:\\Winnt\\system32;c:\\MsDev98\\Common\\Msdev98\\bin;c:\\Msdev98\\Common\\Tools;$g_theDestinationDir\\Dlls";
#		} elsif (-d "D:\\Msdev98") {
#			$ENV{'PATH'} = "$MSDEV_DIR\\bin;c:\\msdev\\sharedide\\bin;C:\\Windows;D:\\DevStudio\\Tools;D:\\MsDev98\\Common\\Msdev98\\bin;D:\\Msdev98\\Common\\Tools;$g_theDestinationDir\\Dlls";
#		} else {
#			$ENV{'PATH'} = "$MSDEV_DIR\\bin;c:\\msdev\\sharedide\\bin;C:\\Windows;D:\\DevStudio\\Tools;$g_theDestinationDir\\Dlls";
#		}
	}
	
	$ENV{'PATH'} = $TmpPath;
	print "PATH is $ENV{'PATH'}\n";
}
#######################################################################
sub NetSendError
{
	local($sMessage)=@_;

	for (@ErrorRecipients)
	{
		if ($g_bIsSuperMake)
		{
			`Net Send $_ "$sMessage from SuperMake"`;
		}
		elsif ($g_bIsSuperCopy)
		{
			`Net Send $_ "$sMessage from SuperCopy"`;
		}
		else
		{
			`Net Send $_ "$sMessage"`;
		}
	}
}
#######################################################################
sub StopOnError
{
	my($sMsg) = @_;
	if ($g_bStopOnError)
	{
		&NetSendError($sMsg);
		die "$sMsg";
	}
}
#######################################################################

close(STDERR);
close(STDOUT);

__END__
:endofperl
