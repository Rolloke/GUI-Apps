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
$bTraceCopy         = 1;
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
$g_bIsSuperHist = 0;
$g_bUseVS71     = 1;
$g_bFinalCopy   = 0;
$g_iErrors      = 0;
$g_iWarnings    = 0;
$g_iUnknownProj = 0;
$g_iCopyErrors  = 0;
$g_bStopOnError = 0;
$g_bGetDepend   = 1;	# disable for faster testing
$g_bCompileLibs = 1;	# disable for faster testing
$g_bRebuildAll  = 0;	# rebuild all projects
$g_bRebuildProj = 0;	# rebuild all if dsp changed, see TrySourceSafe()
$g_bUpdateBuild = 1;	# Update build number and set label, disable for testing
$g_bOnlyTest    = 0;

@g_CheckTranslations=();

$MS_VC_DIR              = "";
$MSDEV_DIR          = "";
$g_theProjectDrive      = "D:";	# may be set by command line parameter
$g_theProjectDir        = "UNKNOWN";
$g_theCurrentProjectDir = "UNKNOWN";
$g_theDestinationDrive  = "G:";	# may be set by command line parameter
$g_theDestinationDir    = "UNKNOWN";	# has to contain vinfo.pvi for the buildNumber info
$g_theDestinationDirEnu = "UNKNOWN";
$g_theDestinationDirIta = "UNKNOWN";
$g_theBuildZipDrive     =  "g:";
$g_theBuildZipDir       =  "$g_theBuildZipDrive\\BuildIdip";

$LabelPrefix="Test_IDIP_";
$VersionLabel = "";
# >>> leer fuer aktuelle Version <<<
# oder mit VersionsLabel passend zum SourceSafe $/Project label
#$VersionLabel = "5.0.0.700";

$buildNumber  = 0;	# read from vinfo.pvi, but used in multiple functions
$currentBuild = 1;	# updated in ReadBuild

$VersionMajor    = 5;
$VersionMinor    = 1;
$VersionSubIndex = 1;
$versionInfo     = "$VersionMajor.$VersionMinor.$VersionSubIndex.";	# since build 197...

$g_iTotalLines = 0;
$g_sStatisticsText = "Statistic:\n";

@ErrorRecipients=("UWEXP","TOMAS","MARTINXP","ROLFXP");
#@ErrorRecipients=("127.0.0.1");

# set some global path variables
$ZIP_CMD="v:\\bin\\tools\\zip\\pkzip25.exe";

#**********************************************************
# dependencie projects are not compiled
@DepProjects=(
				"Lib/luca",
				"include",
				"DirectShow/Lib",
				);

#**********************************************************
# LibProjects are compiled first, that is before the normal Projects
# CAVEAT muss auch weiter unten angepasst werden
# siehe CAVEAT2 und CAVEAT3
@LibProjects=(
			  "Lib/WKClasses",		# is already used in CIPC
#			  "Tools/LogZipLib",	# is already used in CIPC
			  "Cipc",				# is used in all projects
			  "Lib/Skins",			# is already used in OemGui

			  "Capi/Capi4",

			  "DirectShow/Filters/baseclasses",			# a lib. alreday used in Common
			  "DirectShow/Filters/dump",				# ax
			  "DirectShow/Filters/inftee",				# ax
#			  "DirectShow/Filters/mediasource",			# ax
			  "DirectShow/Filters/PushSource",			# ax
#			  "DirectShow/Filters/Sample2MemRenderer",	# ax
			  "DirectShow/Filters/WaveSplitter",		# ax
			  "DirectShow/Common",						# dll

			  "Lib/codebase65",			# neue codebase lib unicode und msdev2003	
			  "Lib/Crypto",
			  "Lib/DeviceDetect",
			  "Lib/HardwareInfo",
			  "Lib/ICPCONDll",
#			  "Lib/JpegLib",
			  "Lib/oemgui",
			  "Lib/NetComm",
			  "Lib/NFC",
			  "Lib/VImage",

#			  "Units/Aku/AkuSys",		# no real compile, just copy
# JaCobUnit will compile all dependencies
			  "Units/JaCob/JaCobDA",	# no real compile, just copy
			  "Units/JaCob/JaCobDll",	# no real compile, just copy
			  "Units/JaCob/JaCobSys",	# no real compile, just copy
# obsolete			  "Units/JaCob/JaCobVxd",	# no real compile, just copy
# obsolete			  "Units/MiCo/MiCoDA",
# obsolete			  "Units/MiCo/MiCoSys",		# no real compile, just copy
# obsolete			  "Units/MiCo/MiCoVxd",		# no real compile, just copy
# SaVicUnit will compile all dependencies
			  "Units/SaVic2/SaVicDA",	# no real compile, just copy
			  "Units/SaVic2/SaVicDll",	# no real compile, just copy
			  "Units/SaVic2/SaVicSys",	# no real compile, just copy
# TashaUnit will compile all dependencies
			  "Units/Tasha/TashaDA",		# no real compile, just copy
			  "Units/Tasha/TashaDll",		# no real compile, just copy
			  "Units/Tasha/TashaSys",		# no real compile, just copy
			  "Units/Tasha/TashaFirmware",		# no real compile, just copy
			  );

#**********************************************************
@TestProjects=( 
			  "Lib/WKClasses",		# is already used in CIPC
		  );

#**********************************************************
# Liste aller relevanten Projekte
# CAVEAT muss auch weiter unten angepasst werden
# siehe CAVEAT2 und CAVEAT3
# NEW PROJECT das WorkingDirectory muss auf MAKE fuer D:\Project\NewProject gesetzt werden
# NEW PROJECT add here

@Projects=( 
			"Capi/ISDNUnit",		# also contains PTUnit			

			"Clients/IdipClient",
			"ControlService",
			"Dbs",

			"DV/acdc",
			"SDK/Sample",

			"SecurityLauncher",
			"SecurityServer", 
				
			"SocketUnit",

			"SystemVerwaltung",

			"Tools/HiveInjection",
			"Tools/VideteCheckDisk",
			"Tools/EwfControl",
			"Tools/CDStart",
			"Tools/DelRegDb",
			"Tools/DvrtRun",		# no need for unicode
			"Tools/EULA",
			"Tools/LogView",
			"Tools/ProductVersion",
			"Tools/ProductView",
			"Tools/RegMove",		# no need for unicode
			"Tools/RegPriv",		# no need for unicode
			"Tools/RSA",
			"Tools/SecAnalyzer",
			"Tools/UpdateHandler",
			"Tools/HealthControl",
			"Tools/Win95Reboot",	# no need for unicode
			"TestPrograms/SDITest",

			"Units/Audio/AudioUnit",
			"Units/CommUnit", 
			"Units/EmailUnit",
			"Units/ICPCONUnit",
			"Units/JaCob/JaCobUnit",
			"Units/NetUnit",
			"Units/SaVic2/SaVicUnit",
			"Units/SDIUnit",
			"Units/SimUnit",
			"Units/SMSUnit",
			"Units/QUnit",
			"Units/Tasha/TashaUnit",	# compiles DLLs, so first of all Tasha!
			"Units/Yuta/USBAlarmUnit", 
			);

##############
# </GLOBALS> #
##############

#********************************************************
# force "flush output"
$|=1;
select(STDERR); $|=1;
select(STDOUT); $|=1;

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
		elsif (/-superHistory/)
		{
			$g_bIsSuperMake = 0;
			$g_bIsSuperHist = 1;
		}
		elsif (/-rebuild/i)
		{
			$g_bRebuildAll = 1;
			if ($bTraceDebug) {print ("Rebuild all\n");}
		}
		elsif (/-vs60/i)
		{
			$g_bUseVS71 = 0;
			if ($bTraceDebug) {print ("Using VisualStudio 6.0 (.NET 2003) in $g_theProjectDir\n");}
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
$g_theProjectDir        = "$g_theProjectDrive\\Project";
$g_theDestinationDir    = "$g_theDestinationDrive\\bin\\idip";	# has to contain vinfo.pvi for the buildNumber info
$g_theDestinationDirEnu = "$g_theDestinationDrive\\bin\\idipEnu";
$g_theDestinationDirIta = "$g_theDestinationDrive\\bin\\idipIta";

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
if (!chdir("$g_theProjectDir"))
{
	&NetSendError("chdir '$g_theProjectDir' failed");
	die "chdir '$g_theProjectDir' failed\n";
}

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

chdir ("$g_theDestinationDrive\\");
if (! -d "$g_theDestinationDirIta")
{
	`mkdir "$g_theDestinationDirIta`;
}
if (! -d "$g_theDestinationDirIta\\Dlls")
{
	`mkdir "$g_theDestinationDirIta\\Dlls`;
}
if (! -d "$g_theDestinationDirIta\\Maps")
{
	`mkdir "$g_theDestinationDirIta\\Maps`;
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
# &SetPath();

if($g_bOnlyTest)
{
	print "Only TestFunction\n";
	&TestFunction();
	die "End of TestFunction\n";
}

#********************************************************
print "\n########################################################\n";		# seperator
if ($g_bIsSuperMake==1)
{
	&SetTimeVariables();
	print "idip Unicode Make started at $mday.$month.$year, $hour:$min:$sec ; Versionlabel '$VersionLabel' ; compiling for '$versionInfo$currentBuild'\n";

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

	#
	# the main loop over all Projects
	#
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
	$MakeDoneText = "idip Unicode Make DONE at $mday.$month.$year, $hour:$min:$sec, Version $versionInfo$currentBuild";
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
	$MakeEndText = "idip Unicode Make END at $mday.$month.$year, $hour:$min:$sec, Version $versionInfo$currentBuild";
	print "\n$MakeEndText\n";
}
elsif ($g_bIsSuperCopy==1)
{
	&SetTimeVariables();
	print "idip Unicode Copy started at $mday.$month.$year, $hour:$min:$sec ; Versionlabel '$VersionLabel' ; copying for '$versionInfo$currentBuild'\n";

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
	$MakeDoneText = "idip Unicode Copy DONE at $mday.$month.$year, $hour:$min:$sec, Version $versionInfo$currentBuild";
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
	$MakeEndText = "idip Unicode Copy END at $mday.$month.$year, $hour:$min:$sec, Version $versionInfo$currentBuild";
	print "\n$MakeEndText\n";
}
elsif ($g_bIsSuperHist==1)
{
	&SetTimeVariables();
	print "idip Unicode History started at $mday.$month.$year, $hour:$min:$sec ; Versionlabel '$VersionLabel' ; copying for '$versionInfo$currentBuild'\n";
	&CollectBuildNotes();
    &CopyBuildNotes();
	&SetTimeVariables();
	$MakeEndText = "idip Unicode History END at $mday.$month.$year, $hour:$min:$sec, Version $versionInfo$currentBuild";
	print "\n$MakeEndText\n";
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
	print `$SAFE_CMD  Get \"\\$\\project\\lib\\luca.lib" -I- \"-VL$VersionLabel\"`;
	print `$SAFE_CMD  Get \"\\$\\dv\\acdc\\NeroApiGlue.lib" -I- \"-VL$VersionLabel\"`;
	print `$SAFE_CMD  Get \"\\$\\project\\lib\\i7000.lib" -I- \"-VL$VersionLabel\"`;
	print `$SAFE_CMD  Get \"\\$\\project\\lib\\uart.lib" -I- \"-VL$VersionLabel\"`;
	print `$SAFE_CMD  Get \"\\$\\project\\lib\\mp4spvd_PIV.lib" -I- \"-VL$VersionLabel\"`;
	print `$SAFE_CMD  Get \"\\$\\project\\lib\\mpeg4_sp_enc.lib" -I- \"-VL$VersionLabel\"`;
	print `$SAFE_CMD  Get \"\\$\\project\\lib\\codebase65.lib" -I- \"-VL$VersionLabel\"`;
	print `$SAFE_CMD  Get \"\\$\\project\\lib\\capseries.lib" -I- \"-VL$VersionLabel\"`;
	

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
		$project = $_;

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
		&DeleteEvenIfRO("ReleaseUnicode/Capi4_32U.lib",$bDoMsg);
		&DeleteEvenIfRO("Release/Capi4_32.lib",$bDoMsg);
	}
	elsif (/Cipc/)
	{
		&DeleteEvenIfRO("ReleaseUnicode/CIPCU.lib",$bDoMsg);
		&DeleteEvenIfRO("ReleaseUnicode/CIPCU.dll",$bDoMsg);
		&DeleteEvenIfRO("Release/CIPC.lib",$bDoMsg);
		&DeleteEvenIfRO("Release/CIPC.dll",$bDoMsg);
		&DeleteEvenIfRO("DebugUnicode/CIPCDebugU.lib",$bDoMsg);
		&DeleteEvenIfRO("DebugUnicode/CIPCDebugU.dll",$bDoMsg);
		&DeleteEvenIfRO("Debug/CIPCDebug.lib",$bDoMsg);
		&DeleteEvenIfRO("Debug/CIPCDebug.dll",$bDoMsg);
	}
	elsif (/JaCobDA/)
	{
		&DeleteEvenIfRO("Release/JaCobDAU.lib",$bDoMsg);
		&DeleteEvenIfRO("Release/JaCobDAU.dll",$bDoMsg);
		&DeleteEvenIfRO("Release/JaCobDA.lib",$bDoMsg);
		&DeleteEvenIfRO("Release/JaCobDA.dll",$bDoMsg);
	}
	elsif (/JaCobDll/)
	{
		&DeleteEvenIfRO("Release/JaCobU.lib",$bDoMsg);
		&DeleteEvenIfRO("Release/JaCobU.dll",$bDoMsg);
		&DeleteEvenIfRO("Release/JaCob.lib",$bDoMsg);
		&DeleteEvenIfRO("Release/JaCob.dll",$bDoMsg);
	}
	elsif (/LogZipLib/)
	{
		&DeleteEvenIfRO("ReleaseUnicode/LogZipLibU.lib",$bDoMsg);
		&DeleteEvenIfRO("Release/LogZipLib.lib",$bDoMsg);
		&DeleteEvenIfRO("DebugUnicode/LogZipLibDebugU.lib",$bDoMsg);
		&DeleteEvenIfRO("Debug/LogZipLibDebug.lib",$bDoMsg);
	}
	elsif (/MiCoDA/)
	{
		&DeleteEvenIfRO("Release/MiCoDA.lib",$bDoMsg);
		&DeleteEvenIfRO("Release/MiCoDA.dll",$bDoMsg);
	}
	elsif (/SaVicDA/)
	{
		&DeleteEvenIfRO("Release/SaVicDAU.lib",$bDoMsg);
		&DeleteEvenIfRO("Release/SaVicDAU.dll",$bDoMsg);
		&DeleteEvenIfRO("Release/SaVicDA.lib",$bDoMsg);
		&DeleteEvenIfRO("Release/SaVicDA.dll",$bDoMsg);
	}
	elsif (/SaVicDll/)
	{
		&DeleteEvenIfRO("Release/SaVicU.lib",$bDoMsg);
		&DeleteEvenIfRO("Release/SaVicU.dll",$bDoMsg);
		&DeleteEvenIfRO("Release/SaVic.lib",$bDoMsg);
		&DeleteEvenIfRO("Release/SaVic.dll",$bDoMsg);
	}
	elsif (/Lib\//)
	{
		# Only projects in LIB

		$ProjectOnly = $project;
		$ProjectOnly =~ s/^Lib\///;	# drop leading Lib/
		$ProjectOnlyU = $ProjectOnly;
		$ProjectOnlyU .= "U";

		if ($bTraceDebug) {printf("Delete output file $ProjectOnlyU\.lib ...\n");}
		&DeleteEvenIfRO("..\\$ProjectOnlyU\.lib",$TraceDeleteEvenIfRO);
		if ($bTraceDebug) {printf("Delete output file $ProjectOnlyU\.dll ...\n");}
		&DeleteEvenIfRO("..\\$ProjectOnlyU\.dll",$TraceDeleteEvenIfRO);
		if ($bTraceDebug) {printf("Delete output file $ProjectOnly\.lib ...\n");}
		&DeleteEvenIfRO("$ProjectOnly.lib",$TraceDeleteEvenIfRO);
		if ($bTraceDebug) {printf("Delete output file $ProjectOnly\.dll ...\n");}
		&DeleteEvenIfRO("$ProjectOnly.dll",$TraceDeleteEvenIfRO);

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
	
	local(@cfgs60);
	local(@cfgs71);
	local($projfile);
	local($projfile60);
	local($projfile71);
	local($dest);

#	print("Build \"$project\"...\n");
		
	# CAVEAT2 jedes Project muss hier eingetragen sein!
	# special settings per project
	# set default values
	$projfile = "$project";
	$dest = "$project";

	if ($project eq "SecurityServer")
	{
		# makefile has different name than project
		$projfile = "sec3";
		$dest		= "sec3";
	}
	elsif ($project eq "Capi/Capi4")
	{
		$projfile = "Capi4_32";
		$dest = "Capi4_32";
	}
	else
	{
		# set default values
		$projfile60 = "$project";
		$dest = "$project";

		#
		# CAVEAT order is important
		#
		$projfile	=~  s/^Units\/Aku\///;	# drop leading Units/Aku
		$dest		=~ s/^Units\/Aku\///;	# drop leading Units/Aku

		$projfile	=~  s/^Units\/Audio\///;	# drop leading Units/Audio
		$dest		=~ s/^Units\/Audio\///;	# drop leading Units/Audio

		$projfile	=~  s/^Units\/JaCob\///;	# drop leading Units/JaCob/JaCobUnit
		$dest		=~ s/^Units\/JaCob\///;	# drop leading Units/JaCob/JaCobUnit

		$projfile	=~  s/^Units\/MiCo\///;	# drop leading Units/MiCo/MiCoUnitPCI
		$dest		=~ s/^Units\/MiCo\///;	# drop leading Units/MiCo/MiCoUnitPCI

		$projfile	=~  s/^Units\/SaVic2\///;	# drop leading Units/SaVic2/SaVicUnit
		$dest		=~ s/^Units\/SaVic2\///;	# drop leading Units/SaVic2/SaVicUnit

		$projfile	=~  s/^Units\/SaVic\///;	# drop leading Units/SaVic/SaVicUnit
		$dest		=~ s/^Units\/SaVic\///;	# drop leading Units/SaVic/SaVicUnit

		$projfile	=~  s/^Units\/Tasha\///;	# drop leading Units/SaVic/SaVicUnit
		$dest		=~ s/^Units\/Tasha\///;	# drop leading Units/SaVic/SaVicUnit

		$projfile	=~ s/^Units\///;	# drop leading Units/
		$dest		=~ s/^Units\///;	# drop leading Units/

		$projfile	=~ s/^DV\///;	# drop leading DV/
		$dest		=~ s/^DV\///;	# drop leading DV/

		$projfile	=~ s/^Tools\///;	# drop leading Tools/
		$dest		=~ s/^Tools\///;	# drop leading Tools/

		$projfile	=~ s/^TestPrograms\///;	# drop leading TestPrograms/
		$dest		=~ s/^TestPrograms\///;	# drop leading TestPrograms/

		$projfile	=~ s/^Capi\///;	# drop leading Capi/
		$dest		=~ s/^Capi\///;	# drop leading Capi/

		$projfile	=~ s/^Lib\///;		# drop leading Lib/
		$dest		=~ s/^Lib\///;	# drop leading Lib/

		$projfile	=~ s/^Clients\///;		# drop leading 
		$dest		=~ s/^Clients\///;	# drop leading

		$projfile	=~ s/^SDK\///;		# drop leading 
		$dest		=~ s/^SDK\///;	# drop leading

		$projfile	=~ s/^HHLA\///;	# drop leading 
		$dest		=~ s/^HHLA\///;	# drop leading

		$projfile	=~ s/^Audio\///;	# drop leading 
		$dest		=~ s/^Audio\///;	# drop leading

		$projfile	=~ s/^DirectShow\///;	# drop leading 
		$dest		=~ s/^DirectShow\///;	# drop leading

		$projfile	=~ s/^Filters\///;	# drop leading 
		$dest		=~ s/^Filters\///;	# drop leading

		$projfile	=~ s/^Yuta\///;	# drop leading 
		$dest		=~ s/^Yuta\///;	# drop leading

		$projfile	=~ s/^USBCAM\///;	# drop leading 
		$dest		=~ s/^USBCAM\///;	# drop leading

		# NEW PROJECT add here, if on 'new' subdirectory
	}

	$projfile71 = $projfile.".vcproj";
	$projfile60 = $projfile.".dsp";

	local($bValidProject)=0;
	local ($bNoCompile)=0;

	# libs may have Unicode version
	if (   $project eq "Lib/WKClasses"
		|| $project eq "Cipc"
		|| $project eq "Lib/NFC"
  	    || $project eq "Lib/VImage"
		)
	{
		@cfgs71=("ReleaseUnicode","DebugUnicode");
		@cfgs60=("Release","Debug");
		$bValidProject=1;
	}
	elsif (   $project eq "Capi/Capi4"
		|| $project eq "DirectShow/Common"
		|| $project eq "Lib/Crypto"
		|| $project eq "Lib/DeviceDetect"
		|| $project eq "Lib/HardwareInfo"
		|| $project eq "Lib/ICPCONDll"
		|| $project eq "Lib/NetComm"
		|| $project eq "Lib/Skins"
		)
	{
		@cfgs71=("ReleaseUnicode");
		$bValidProject=1;
	}
	# or Unicode and EnuUnicode versions
	elsif (	   $project eq "Lib/oemgui"
			)
	{
		@cfgs71=("ReleaseUnicode","EnuUnicode","RusUnicode");
		$bValidProject=1;
	}
	elsif (
			   $project eq "Lib/codebase65"
		  ) 
	{
		# nur release
		@cfgs71=("Release");
		$bValidProject=1;
	}
	elsif (	   $project eq "Capi/ISDNUnit"
			)
	{
		# special for ISDNUnit und PTUnit in einem Project
		@cfgs71=("Release", "Enu", "PTUnitRelease", "PTEnu");
		$bValidProject=1;
	}
	elsif (    $project eq "Units/Aku/AkuSys"
			|| $project eq "Units/JaCob/JaCobDA"
			|| $project eq "Units/JaCob/JaCobSys"
			|| $project eq "Units/JaCob/JaCobVxd"
			|| $project eq "Units/MiCo/MiCoSys"
			|| $project eq "Units/MiCo/MiCoVxd"
			|| $project eq "Units/SaVic2/SaVicDA"
			|| $project eq "Units/SaVic2/SaVicSys"
			|| $project eq "Units/Tasha/TashaDA"
			|| $project eq "Units/Tasha/TashaDll"
			|| $project eq "Units/Tasha/TashaSys"
			|| $project eq "Units/Tasha/TashaFirmware"
			)
	{
		$bNoCompile=1;
		$bValidProject=1;
		# still needs one cfg to get the copy done
		@cfgs71=("Release");
	}
	elsif (    $project eq "Units/JaCob/JaCobDll"
			|| $project eq "Units/SaVic2/SaVicDll"
			)
	{
		$bNoCompile=1;
		$bValidProject=1;
		# still needs one cfg to get the copy done
		@cfgs71=("Release", "Enu");
	}
	elsif (
			   $project eq "DirectShow/Filters/baseclasses"			# a lib
			|| $project eq "DirectShow/Filters/dump"				# ax
			|| $project eq "DirectShow/Filters/inftee"				# ax
			|| $project eq "DirectShow/Filters/mediasource"			# ax
			|| $project eq "DirectShow/Filters/PushSource"			# ax
			|| $project eq "DirectShow/Filters/Sample2MemRenderer"	# ax
			|| $project eq "DirectShow/Filters/WaveSplitter"		# ax
			|| $project eq "HHLA/RemoteControl"
			|| $project eq "ControlService"
			|| $project eq "Tools/VideteCheckDisk"
			|| $project eq "Tools/EwfControl"
			|| $project eq "Tools/HiveInjection"
			|| $project eq "Tools/CDStart"
#			|| $project eq "Tools/convntfs"
			|| $project eq "Tools/DvrtRun"
			|| $project eq "Tools/LogZip"
			|| $project eq "Tools/RegMove"
			|| $project eq "Tools/RegPriv"
			|| $project eq "Tools/RSA"
			|| $project eq "Tools/Win95Reboot"
			|| $project eq "TestPrograms/SDITest"
			|| $project eq "Tools/WkTranslator"
			|| $project eq "Units/JaCob/JaCobTest"
			|| $project eq "Units/MiCo/MiCoDA"
			|| $project eq "Units/Tasha/TashaUnit"
			|| $project eq "Units/QUnit"
			|| $project eq "Tools/UpdateHandler"
			|| $project eq "Tools/HealthControl"
			|| $project eq "SDK/Sample"
			)
	{
		# nur release
		@cfgs71=("Release");
		$bValidProject=1;
	}
	elsif (
			   $project eq "Clients/ICPCONClient"
			|| $project eq "DV/acdc"
			|| $project eq "Dbs"
			|| $project eq "SecurityServer"
			|| $project eq "SocketUnit"
			|| $project eq "Tools/DelRegDb"
			|| $project eq "Tools/EULA"
			|| $project eq "Tools/ProductVersion"
			|| $project eq "Tools/ProductView"
			|| $project eq "Tools/SecAnalyzer"
			|| $project eq "Units/Aku/AkuUnit"
			|| $project eq "Units/Audio/AudioUnit"
			|| $project eq "Units/CoCoUnit"
			|| $project eq "Units/CommUnit"
			|| $project eq "Units/EmailUnit"
			|| $project eq "Units/GemosUnit"
			|| $project eq "Units/ICPCONUnit"
			|| $project eq "Units/JaCob/JaCobUnit"
			|| $project eq "Units/MiCo/MiCoUnitPCI"
			|| $project eq "Units/NetUnit"
			|| $project eq "Units/SaVic2/SaVicUnit"
			|| $project eq "Units/SDIUnit"
			|| $project eq "Units/SimUnit"
			|| $project eq "Units/SMSUnit"
			|| $project eq "Units/TOBSUnit"
			|| $project eq "Units/USBCAM/USBCamUnit"
			|| $project eq "Units/Yuta/USBAlarmUnit"
			)
	{
		# release und enu
		@cfgs71=("Release","Enu");
		$bValidProject=1;
	}
	elsif (   $project eq "Clients/IdipClient"
		   || $project eq "SystemVerwaltung"
		   || $project eq "SecurityLauncher"
		   || $project eq "Tools/LogView"
		  )
	{
		# release und enu
		@cfgs71=("Release","Enu","Rus");
		$bValidProject=1;
	}
	else
	{
		$bValidProject = 0;
		$g_iUnknownProj++;
		print "Unknown project: \"$project\"\n";
		&StopOnError("Tool cancelled at unknown project");
	}

	# a valid project ? Then loop over the given configurations.
	if ($bValidProject) 
	{
		if ($g_bFinalCopy==0) 
		{
			# it´s not copy; do the nmake
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
					&CheckMultipleRC($dest, @cfgs71);
				}

				# &SetEnv(0);
				for (@cfgs60) 
				{
					if ($bTraceOneConfig) {print ("DoOne 60 $project,$projfile60,$dest\n");}
					&DoOneConfiguration60($projfile60,$dest,$_);
				}
				# &SetEnv(1);
				for (@cfgs71) 
				{
					if ($bTraceOneConfig) {print ("DoOne 71 $project,$projfile71,$dest\n");}
					&DoOneConfiguration71($projfile71,$dest,$_);
				}
			}
		} 
		else 
		{
			# do not do the make, but do the copy
			for (@cfgs71) 
			{
				&DoCopy($project,$dest,$_);
			}
		}
	} 
	else 
	{
		print ("Invalid project: \"$project\"\n");
	}
}
#######################################################################
sub DoStaticAnalysis
{
	# perform static analysis of sourceCode
	local($checkCmd)="v:\\perl5\\bin\\checkBreak.bat "; # getcwd is appended
	$checkCmd .= getcwd;
#	print ("DoStaticAnalysis $checkCmd\n");
	@checkLog = `$checkCmd`;

	# collect total line stats
	for ($ci=0;$ci <= $#checkLog;$ci++)
	{
		local($tmpJunk)="";
		local($restJunk)="";
		$oneLine=$checkLog[$ci];	# get first line
		$_ = $oneLine;
		if (/#STAT#/)
		{
			$g_sStatisticsText .= $oneLine;
			$oneLine =~ s/\s+/ /gm;
			$oneLine =~ s/^\s+//gm;
			# looks like "#STAT# 8191 lines in d:\Project\SecurityLauncher"
			($tmpJunk,$lines,$restJunk) = split(/ /,$oneLine,3);
			$g_iTotalLines += $lines;
		}
		elsif ( /nicht/ )
		{
			# ignore
		}
		else
		{
			print ("$oneLine");
		}
		if (/ [wW]arning /) 
		{
			# ignore some non-relevant warnings
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
sub DoOneConfiguration60
{
	local($projfileName,$dest,$cfg)=@_;
	local($oneLine)="";
	local($ei)=0;

#	print " .... $projfileName CFG=\"$dest - Win32 $cfg\"\n";

	if ($cfg ne "PTUnitRelease") {
		&UpdateRC2();
	}

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

	&AnalyseDevEnvOutput($project, $dest, $cfg, @errorLog);
}
#######################################################################
sub DoOneConfiguration71
{
	local($projfileName,$dest,$cfg)=@_;
	local($oneLine)="";
	local($ei)=0;

#	print " .... $projfileName CFG=\"$dest - Win32 $cfg\"\n";

	if ($cfg ne "PTUnitRelease") {
		&UpdateRC2();
	}

	local($builtType) = "build";
	if ($g_bRebuildAll || $g_bRebuildProj)
	{
		$builtType = "rebuild";
	}

	print "$builtType $dest - $cfg\n";
	# SYNTAX: devenv SolutionName /build ConfigName [/project ProjName] [/projectconfig ConfigName]
	# SYNTAX: devenv solutionfile.sln  /build solutionconfig
	local($cmdLine) = "devenv $projfileName /$builtType $cfg /project $dest";
#	print "$cmdLine\n";
	@errorLog = `$cmdLine `;

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
		my($oneLine) = "";
		my($ei) = 0;
		
		if ($bDebugging) {print ("LOG: Error lines: $#errorLog\n");}

		for ($ei=0 ; $ei < $#errorLog ; $ei++)
		{
			$oneLine = $errorLog[$ei];	# already has newline
			# DevEnv write explicit some ASCII_NUL to the outout,
			# which skips the printing of the following line!
			# So we have to replace the ASCII_NUL with e.g. SPACE 
			$oneLine =~ s/\0//g;
			$_ = $oneLine;

			if ($bDebugging) {print ("LOG $ei : $oneLine");}

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
				if ($bDebugging) {print ("LOG $ei : skipped\n");}
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
				# ignore some non-relevant warnings
				if (
					   / C4189:/	# local variable is initialized but not referenced
#					|| /LNK4098/	# defaultlib "library" conflicts with use of other libs; use /NODEFAULTLIB:library
#					|| /LNK4089/	# all references to 'OLEAUT32.dll' discarded by /OPT:REF
					)
				{
					# don't count
				}
				else
				{
					$cfgWarning++;
					$g_iWarnings++;
				}
			}
#			if (/ COPY FAILED /)
#			{
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
	`$g_theDestinationDir\\ProductVersion -updateDirectory $g_theDestinationDir`;

	chdir($g_theProjectDrive);	

	# NEW 16.10.98 no date/time in label
	$LabelText = "$LabelPrefix$versionInfo$currentBuild";
	if ($g_bUpdateBuild)
	{
		&CollectBuildNotes();
		&CopyBuildNotes();

	    # set a version label in the SourceSafe
		print "Setting SourceSafeLabel: $LabelText\n";
		`$SAFE_CMD  Label \$/Project -C- -L$LabelText`;
		# gf new 02.01.2003 label for acdc to get build notes
		`$SAFE_CMD  Label \$/DV -C- -L$LabelText`;
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
	print "executing $ZIP_CMD -add -recurse -directories $g_theBuildZipDir\\$currentBuild.zip $g_theDestinationDir\\*.*\n";
	`$ZIP_CMD -add -recurse -directories $g_theBuildZipDir\\$currentBuild.zip $g_theDestinationDir\\*.*`;

	chdir($g_theDestinationDirEnu);	
	print "Creating $g_theBuildZipDir\\Enu$currentBuild.zip\n";
	print "executing $ZIP_CMD -add -recurse -directories $g_theBuildZipDir\\Enu$currentBuild.zip $g_theDestinationDirEnu\\*.*\n";
	`$ZIP_CMD -add -recurse -directories $g_theBuildZipDir\\Enu$currentBuild.zip $g_theDestinationDirEnu\\*.*`;

#	chdir($g_theDestinationDirIta);	
#	print "Creating $g_theBuildZipDir\\Ita$currentBuild.zip\n";
#	print "executing $ZIP_CMD -add -recurse -directories $g_theBuildZipDir\\Ita$currentBuild.zip $g_theDestinationDirIta\\*.*\n";
#	`$ZIP_CMD -add -recurse -directories $g_theBuildZipDir\\Ita$currentBuild.zip $g_theDestinationDirIta\\*.*`;
}
#######################################################################
sub CopyBuildNotes
{
	$source = "$g_theDestinationDir\\BuildNotes$currentBuild.htm";
	$dest   = "\\\\comm01\\buildnotes\\idip\\BuildNotes$currentBuild.htm";
	print `copy $source $dest`;
	print ("CopyBuildNotes done.\n");
}
#######################################################################
sub TrySourceSafe
{
	local($bTraceDebug) = 0;
	local($proj)=@_;

	if (-d "res")
	{
		# delete modified rc2
	}

	# check if dsp file has changed
	$g_bRebuildProj = 0;
	local($dspPath) = $g_theCurrentProjectDir;
	local($dspFile) = "";
	
	if ($bTraceDebug) 
	{
		print ("$dspPath\n");
	}

	local(@array)  = split(/\\/,$dspPath);     # Array der einzelnen Teilstrings
	# last one is project itself
	foreach $x (@array)
	{
		$dspFile = $x;
	}
	local(@arrayfs)  = split(/\//,$dspFile);     # Array der einzelnen Teilstrings
	# last one is project itself
	foreach $x (@arrayfs)
	{
		$dspFile = $x;
	}
	
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

	if ($g_bUseVS71)
	{
		$dspFile .= "\.vcproj";
	}
	else
	{
		$dspFile .= "\.dsp";
	}

	$dspPathname = $dspPath;
	$dspPathname .= "/"; 
	$dspPathname .= $dspFile; 

	
	if (   !-f $dspPathname 
	    && !(/include/)
	    && !(/luca/)
		&& !(/AkuSys/)	    
	    )
	{
		$g_bRebuildProj = 1;
	}

	# save old file time
	local($time_secs_old) = (stat($dspPathname))[9]; 
	print ("time_secs_old $time_secs_old $dspPathname\n");

	print "getting $proj Version $VersionLabel\n";

	# -R recusive
	# -I no input
	# -VL version by label

	local($OutputFile) = "vsstmp.txt"; 
	unlink($OutputFile);

	# try to avoid complaint "A writable copy already exist
#	my ($file) = $proj;
#	$file =~ s/^\$\\//;	# drop leading 
#	if ($bTraceDebug) {print ("file is $file\n");}
#	if (-f $file)
#	{
#		if ($bTraceDebug) {print ("GET FILE $proj\n");}
#	}
	if ($VersionLabel ne "") {
		`$SAFE_CMD  Get \"$proj\" -R -I-N -O\@$OutputFile \"-VL$VersionLabel\"`;
	} else {
		`$SAFE_CMD  Get \"$proj\" -R -I-N -O\@$OutputFile`;
	}

	# get new file time	
	local($time_secs_new) = (stat($dspPathname))[9]; 
	print("time_secs_new $time_secs_new\n");

	# if file time changed -> file changed -> rebuild all
	if ($time_secs_new != $time_secs_old)
	{
		$g_bRebuildProj = 1;
	}
	else
	{
		$g_bRebuildProj = 0;
	}
	print ("Rebuild $g_bRebuildProj\n");

	# check if any error at SourceSafe operation
	open(HFILE, $OutputFile);
	while (<HFILE>)
	{
		if (/is not an existing filename or project/)
		{
			print ("ERROR: $_\n");
			&OnError();
		}
		if ($bTraceSourceSafe)
		{
			print ("$_");
		}
	}
	close(HFILE);
	unlink($OutputFile);
}
#######################################################################
sub CheckMultipleRC
{
	local($bTraceDebug) = 0;
	if ($bTraceDebug) {print ("CheckMultipleRC\n");}

	local($proj, @cfgs71) = @_;

	if ($#cfgs > 0)
	{
		# check for multiple RC files
		@RCFiles= `dir /b /on *.rc`;
		if ($#RCFiles > 0)
		{
			if ($bTraceDebug) {print ("TEST: Project $proj\n");}
			local($deuRCFile) = $proj;
			$deuRCFile .= "\.rc";
			if ($bTraceDebug) {print ("TEST: deuRCFile $deuRCFile\n");}
			# save german rc file time
			local($timeSecsDeuRC) = (stat($deuRCFile))[9]; 
			if ($bTraceDebug) {print ("TEST timeSecsDeuRC $timeSecsDeuRC\n");}
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
							if ($bTraceDebug) {print ("TEST $oneRCFile timeSecsOneRC $timeSecsOneRC\n");}
							# if file time OneRC < file time DeuRC -> check translation!
							if ($timeSecsOneRC < $timeSecsDeuRC)
							{
								$warningCount++;
								print ("Warning: Check translation $oneRCFile\n");
								push(@g_CheckTranslations, $oneRCFile);
							}
							else
							{
								if ($bTraceDebug) {print ("TEST translation OK $oneRCFile\n");}
							}
						}
						else
						{
							if ($bTraceDebug) {print ("TEST $cfg not found in $oneRCFile\n");}
						}
					}
				}
				else
				{
					if ($bTraceDebug) {print ("TEST OOPS NON-RC file? $oneRCFile\n");}
				}
			}
		}
		else
		{ # only one rc file
			if ($bTraceDebug) {print ("TEST: Project $proj has only one RC\n");}
		}
	}
	else
	{ # only one configuration
		if ($bTraceDebug) {print ("TEST: Project $proj has only one cfg $cfgs[0]\n");}
	}
}
#######################################################################
sub CreateNewDir
{
	local($bTraceDebug) = 0;
	local($sDir)=@_;
	if ($bTraceDebug) {print "CreateSubDirs $sDir\n";}
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
	my ($bTraceDebug) = 0;

	if ($bTraceCopy) {print ("DoCopy Project:$project, Dest:$dest, Config:$config\n");}

	if ($dest eq "ISDNUnit" && $config eq "PTUnitRelease")
	{
		$dest="PTUnit";
	}
	elsif ($dest eq "ISDNUnit" && $config eq "PTEnu")
	{
		$dest="PTUnit";
	}

	# make the destination dependend from the configuration
	if (   $config eq "Enu"
		|| $config eq "EnuUnicode"
		|| $config eq "PTEnu")
	{
	  $destinationDir = $g_theDestinationDirEnu;
	}
	elsif (   $config eq "Ita"
		   || $config eq "ItaUnicode"
		   || $config eq "PTIta")
	{
	  $destinationDir = $g_theDestinationDirIta;
	}
	else
	{
	  $destinationDir = $g_theDestinationDir;
	}

	# create non existing dir
	if (! -d "$destinationDir")
	{
		`mkdir $destinationDir`;
	}

	local ($bDoCopy) = 1;

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
		if (  $config eq "Debug"
			||$config eq "DebugUnicode")
		{
			$dest .= "Debug";
		}
		if ($g_bUseVS71)
		{
			$dest .= "U";
		}
		$source = "$g_theProjectDir\\$project\\$config\\$dest\.dll";
		$finalDest = "$destinationDir\\Dlls\\$dest\.dll";
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
	elsif (
			  $project eq "Lib/Crypto"
		   || $project eq "Lib/DeviceDetect"
		   || $project eq "Lib/ICPCONDll"
		   || $project eq "Lib/HardwareInfo"
#		   || $project eq "Lib/JpegLib"
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
	elsif (   $project eq "Lib/oemgui"
		  )
	{
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

			# die russische Resource
			$destDll = $dest;
			$destDll .= "Rus.dll";
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
	elsif (   (    $project eq "SystemVerwaltung"
				|| $project eq "Clients/IdipClient"
 			    || $project eq "SecurityLauncher"
				|| $project eq "Tools/LogView"
			  )
		   && $g_bUseVS71
		  )
	{
		# mal was neues mit Sprachen-DLL
		if ($config eq "Release")
		{
			# das Programm an sich
			$source = "$g_theProjectDir\\$project\\$config\\$dest.exe";
			$finalDest = "$destinationDir\\$dest.exe";

			my($destDll) = "";
			
			# die deutsche Resource
			$destDll = $dest;
			$destDll .= "Deu.dll";
			&MyDoCopy("$g_theProjectDir\\$project\\Release\\$destDll", "$destinationDir\\$destDll", 1);
			
			# die englische Resource
			$destDll = $dest;
			$destDll .= "Enu.dll";
			&MyDoCopy("$g_theProjectDir\\$project\\Release\\$destDll", "$destinationDir\\$destDll", 1);

			# die russische Resource
			$destDll = $dest;
			$destDll .= "Rus.dll";
			&MyDoCopy("$g_theProjectDir\\$project\\Release\\$destDll", "$destinationDir\\$destDll", 1);
		}
		else
		{
			$bDoCopy=0;
		}
	}
	elsif ($project eq "Units/Aku/AkuSys")
	{
		$source = "$g_theProjectDir\\$project\\bin\\Aku.sys";
		$finalDest = "$destinationDir\\Aku.sys";
	}
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
		&MyDoCopy("$g_theProjectDir\\$project\\Bin\\SaVic.inf","$destinationDir\\SaVic.inf",1);	# do count errors
		# Schreibschutz entfernen !
		chmod 0755, "$destinationDir\\SaVic.inf";
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
	elsif ($project eq "Units/Tasha/TashaDll")
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
	elsif ($project eq "Units/Yuta/USBAlarmUnit")
	{
		# der absolute special fall yuta.inf yuta.sys und yutaunit.exe
		# der Treiber Yuta.sys
		&MyDoCopy("$g_theProjectDir\\$project\\Yuta.sys","$destinationDir\\Yuta.sys",1);
		# die Installationsdatei Yuta.inf
		&MyDoCopy("$g_theProjectDir\\$project\\Yuta.inf","$destinationDir\\Yuta.inf",1);
		# die Yuta Firmware
		&MyDoCopy("$g_theProjectDir\\$project\\YutaFirmware.hex","$destinationDir\\YutaFirmware.hex",1);

		$source = "$g_theProjectDir\\$project\\$config\\YutaUnit.exe";
		$finalDest = "$destinationDir\\YutaUnit.exe";
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
		&MyDoCopy($source, $finalDest, 1);	# do count errors

		# followed by.map
		#

		# destination is Maps, but not CoCo/Maps
		$destinationDir .= "\\Maps";
		# remmove coco dir
		$destinationDir =~ s/\\CoCo\\/\\/;

		# create if not already there
		if (! -d "$destinationDir")
		{
			`mkdir $destinationDir`;
		}
		
		$source =~ s/\.exe/.map/;
		$source =~ s/\.dll/.map/;

		$_ = $project;
		if (/Clients\\Vision/)
		{
			$dest = "Vision40";
		}
		elsif (
				   /oemgui/
				|| /PTDecoder/
#				|| /JpegLib/
				|| /DeviceDetect/
				|| /h263enc/
				|| /VImage/
				|| /h263dec/
				|| /ICPCONDll/
			)
		{
			# .map are not in Lib/ but in their Release/ directories
			$source = "$g_theProjectDir\\$project\\$config\\$dest.map";
		}
		elsif ( /MiCoSys/ || /MiCoVxd/ || /JaCobSys/ || /JaCobVxD/ || /AkuSys/ || /SaVicSys/)
		{
			$source = "NO COPY"; # no map file
		}

#ML		$finalDest = "$destinationDir\\maps\\$dest.map";
		$finalDest = "$destinationDir\\$dest.map";

		$_ = $source;

		if (/\.map/)
		{
			if (!(-f $source))
			{
				$source =~ s/\.map/.pdb/;
				$finalDest =~ s/\.map/.pdb/;
				print "using pdb instead of map $source \n";
			}
		}

		if (! ($source eq "NO COPY") )
		{
			&MyDoCopy($source,$finalDest,0);	# do NOT count errors
		}
		else
		{
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
	local($source,$finalDest,$bDoCountError)=@_;
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

	$prevLabelText = "";
	open (LABEL_IN_FILE,"<$g_theDestinationDir\\label.txt");
	while (<LABEL_IN_FILE>)
	{
		$prevLabelText = "$prevLabelText$_";
	}
	close(LABEL_IN_FILE);
	$prevLabelText =~  s/\n//;
	$prevLabelText =~  s/\r//;

	my ($sSuperHistoryFile) = "SuperHistory.txt";

	for (@allProjects) 
	{
		local($project);
		$project=$_;
		chdir("/Project/$project");
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
			
			# -IECW ignore case/end of line/white space changes
			print "Diff $projectSS/history.txt -DS200 -IECW -VL$prevLabelText > $sSuperHistoryFile\n";
			print `$SAFE_CMD Diff $projectSS/history.txt -DS200 -IECW -VL$prevLabelText > $sSuperHistoryFile`;

			# now $sSuperHistoryFile is created
			if (-f "$sSuperHistoryFile")
			{
				# collect between the topmost Label:"Build
				# from this take only Chg: To : Ins:
				local($bAlreadyDidPrint)=0;
				local($bAlreadyDidPrintText)=0;
				open(HFILE,"$sSuperHistoryFile") || warn "can't open $sSuperHistoryFile\n";
				if ($bTraceSuperHistory)
				{
					printf ("Scanning $sSuperHistoryFile in %s...\n",getcwd());
				}
				while (<HFILE>)
				{
					if (   /^Against:/	# skip line
						|| /^Diffing:/	# skip line
						|| /^\$\//		# skip line with SourceSafe project name, if line break before (project name too long)
						)
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
							print (OFILE "<H2>$project/history.txt</H2>\n"); # no need for a break here, foolowing line have a <BR> at the beginning
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


						if (   /^\s*$/
							&& ($bAlreadyDidPrintText==0)
							)
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
					print (OFILE "<BR><BR>\n");
				}
				close(HFILE);
				unlink("$sSuperHistoryFile");
			}
			else
			{
				print "$sSuperHistoryFile does not exist\n"
			}
		}
		else
		{
			print ("$project has no history.txt\n");
		}
	}

	print (OFILE "</BODY>");
	print (OFILE "</HTML>");
	close (OFILE);

}

close(STDERR);
close(STDOUT);

#######################################################################
sub DeleteEvenIfRO
{
	local($oneFile,$bDoMsg)=@_;

	if ($bTraceUnlink) {
		printf ("DeleteEvenIfRO for $oneFile, in %s, msg $bDoMsg\n", getcwd());
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
		if ( /AudioUnit/ ) {
			$rcFile =~ s/AudioUnit/AudioUnit/;
		}
		if ( /SaVicUnit/ ) {
			$rcFile =~ s/SaVicUnit/SaVicUnit/;
		}
		if ( /JaCobTest/ ) {
			$rcFile =~ s/JaCobTest/JaCobTest/;
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
		open(FILE,"$g_theDestinationDir/vinfo.pvi") || warn "can't open vinfo.pvi\n";
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
} # end of ReadBuildNumber
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
		if ( -d "C:/MSDev2003" ) {
			$MS_VC_DIR="C:\\MSDev2003\\Vc7";
			$MSDEV_DIR="C:\\MSDev2003\\Common7\\IDE";
		} else {
			&NetSendError("MSDEV not found!");
			die "MsDev not found\n";
		}
	}
	else
	{
		if ( -d "C:/VS_60/VC98" ) {
			$MS_VC_DIR="C:\\VS_60\\VC98";
		} elsif ( -d "C:/devstudio/vc" ) {
			$MS_VC_DIR="C:\\devstudio\\vc";
		} elsif ( -d "c:/msdev/vc" ) {
			$MS_VC_DIR="c:\\msdev\\vc";
		} elsif ( -f "c:/msdev/common/msdev98/bin/msdev.exe" ) {
			$MS_VC_DIR="c:\\msdev\\vc98";
		} elsif (-d "E:/devstudio/vc" ) {
			$MS_VC_DIR="E:\\devstudio\\vc";
		} elsif ( -d "F:/devstudio/vc" ) {
			$MS_VC_DIR="F:\\devstudio\\vc";
		} elsif ( -d "d:/msdev98/vc" ) { # CAVEAT order is important. special 6.0 hack
			$MS_VC_DIR="d:\\msdev98\\vc";
		} elsif ( -d "c:/msdev98/vc98" ) { # CAVEAT order is important. special 6.0 hack
			$MS_VC_DIR="c:\\msdev98\\vc98";
		} else {
			&NetSendError("MSDEV not found!");
			die "MsDev not found\n";
		}
	}
	print "Found $MS_VC_DIR\n";
}
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
	} elsif ( -f "C:/Msdev60/Common/VSS/win32/ss.exe" ) {
		$SAFE_CMD="C:\\Msdev60\\Common\\VSS\\win32\\ss.exe";
	} elsif ( -f "c:/msdev98/common/vss/win32/ss.exe" ) {
		$SAFE_CMD="C:\\msdev98\\common\\vss\\win32\\ss.exe";
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
sub SetPath
{
	my ($TmpPath)      = $ENV{'PATH'};
	#*******************************************
	print "Original PATH is $TmpPath\n";
	if ($g_bUseVS71)
	{
		$_ = $TmpPath;
		if ( !/$MSDEV_DIR/)
		{
			print "Append $MSDEV_DIR to PATH\n";
#			$TmpPath .= "$MSDEV_DIR;"
			$TmpPath .= ";$MSDEV_DIR;$MS_VC_DIR\BIN;C:\\MSDev2003\\Common7\\Tools;C:\\MSDev2003\\Common7\\Tools\\bin\\prerelease;C:\\MSDev2003\\Common7\\Tools\\bin;C:\\MSDev2003\\SDK\\v1.1\\bin;C:\\WINDOWS\\Microsoft.NET\\Framework\\v1.1.4322;";
		}
	}
	else
	{
	}

#	if (-d "c:\\Msdev98") {
#		$ENV{'PATH'} = "$MS_VC_DIR\\bin;C:\\Winnt;C:\\Winnt\\system32;c:\\MsDev98\\Common\\Msdev98\\bin;c:\\Msdev98\\Common\\Tools;$g_theDestinationDir\\Dlls";
#	} elsif (-d "D:\\Msdev98") {
#		$ENV{'PATH'} = "$MS_VC_DIR\\bin;c:\\msdev\\sharedide\\bin;C:\\Windows;D:\\DevStudio\\Tools;D:\\MsDev98\\Common\\Msdev98\\bin;D:\\Msdev98\\Common\\Tools;$g_theDestinationDir\\Dlls";
#	} else {
#		$ENV{'PATH'} = "$MS_VC_DIR\\bin;c:\\msdev\\sharedide\\bin;C:\\Windows;D:\\DevStudio\\Tools;$g_theDestinationDir\\Dlls";
#	}
	
	$ENV{'PATH'} = $TmpPath;
	print "PATH is $ENV{'PATH'}\n";
}
#######################################################################
sub SetEnv
{
	my($bUseVS71)=@_;
	# set env variables, not using V:
	# CAVEAT requires $g_theProjectDir
	
	my ($TmpInclude)   = $ENV{'INCLUDE'};
	my ($TmpLib)       = $ENV{'LIB'};
	my ($TmpPath)      = $ENV{'PATH'};
	my ($FrameworkSDK) = "";

	#*******************************************
#	print "Original INCLUDE is $TmpInclude\n";
	if ($bUseVS71)
	{
		$TmpInclude = "$MS_VC_DIR\\atlmfc\\include;$MS_VC_DIR\\Include;$MS_VC_DIR\\atlmfc\\src\\mfc;$MS_VC_DIR\\PlatformSDK\\include\\prerelease;$MS_VC_DIR\\PlatformSDK\\include;$ENV{'INCLUDE'}";
		$TmpInclude .= "$g_theProjectDir\\Include;$g_theProjectDir\\Cipc;$g_theProjectDir\\lib;$g_theProjectDir\\Dv\\lib;";
		if (-d "C:/MSDev2003/SDK/v1.1/include")
		{
			$FrameworkSDK = "C:\\MSDev2003\\SDK\\v1.1\\include;";
#			print "Append $FrameworkSDK to INCLUDE\n";
			$TmpInclude .= $FrameworkSDK;
		}
	}
	else
	{
#		$TmpInclude = "$MS_VC_DIR\\Include;$MS_VC_DIR\\mfc\\include;$MS_VC_DIR\\atl\\include;$MS_VC_DIR\\mfc\\src;";
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
#	print "INCLUDE is: $TmpInclude\n";

	#*******************************************
#	print "Original LIB is $TmpLib\n";
	if ($bUseVS71)
	{
		$TmpLib = "$MS_VC_DIR\\lib;$MS_VC_DIR\\atlmfc\\lib;;$MS_VC_DIR\\PlatformSDK\\lib\\prerelease;$MS_VC_DIR\\PlatformSDK\\lib;";
		$TmpLib .= "$g_theProjectDir\\Lib;$g_theProjectDir\\Cipc\\ReleaseUnicode;$g_theProjectDir\\Cipc\\Release;$g_theProjectDir\\DV\\Lib;";
		if (-d "C:/MSDev2003/SDK/v1.1/lib")
		{
			$FrameworkSDK = "C:\\MSDev2003\\SDK\\v1.1\\lib;";
#			print "Append $FrameworkSDK to LIB\n";
			$TmpLib .= $FrameworkSDK;
		}
	}
	else
	{
		$TmpLib = "d:\\inetsdk\\lib;$MS_VC_DIR\\lib;$MS_VC_DIR\\mfc\\lib;$g_theProjectDir\\Lib;";
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
#	print "LIB is: $TmpLib\n";
	
}
#######################################################################
sub NetSendError
{
	my($sMessage)=@_;

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
sub TestFunction
{

	##############################
	# ALWAYS DIE!
	die "End of TestFunction\n";
}
#######################################################################
__END__
:endofperl
