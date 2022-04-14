@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

#
# PERL PART FOLLOWS HERE
#

use Cwd;
#
# trace options
#
$bTraceOneConfig=0;
$bTraceCopy=1;
$bTraceUnlink=0;
$bTraceSuperHistory=1;
$bTraceSourceSafe=0;	# NOT YET implemented
$bGetLibs=1;	# disable for faster testing
$bPrintDSPConversion=0; # default 0; print local dsp conversion

#############
# <GLOBALS> #
#############
$bWorkAsSuperCopy = 0;
$bInFinalCopy = 0;
$theCopyErrorCount=0;	# modified in MyDoCopy
$errorCount=0;
$warningCount=0;
$UnknownCount=0;
$bRebuildAll=0;		# rebuild all if dsp changed, see TrySourceSafe()

$theDestinationDir="v:\\bin\\dvx";	# has to contain vinfo.pvi for the buildNumber info
$theDestinationDirEnu="v:\\bin\\dvx\\enu";

$LabelPrefix="DTS_Unicode";

# >>> VersionsLabel leer fuer aktuelle Version <<<
# oder mit VersionsLabel passend zum SourceSafe $/Project label
$VersionLabel="";
#$VersionLabel="DTS_2.0.0.570";

$buildNumber=0;		# read from vinfo.pvi, but used in multiple functions
$currentBuild=1;	# updated in ReadBuild
# >>> do not remove or modify this line, copy and uncomment <<<

#$VersionMinor=0;	# last Build 299, 13.11.00
#$VersionMinor=1;	# last Build 369, 02.04.01
#$VersionMinor=2;	# last Build 404, 06.06.01
#$VersionMinor=3;	# last Build 418, 26.07.01 and Bugfix Build 418_435, 04.10.01

# change Version number here
$VersionMajor=2;
$VersionMinor=5;
$VersionSubIndex=0;

#$versionInfo="3.2.0.";	# since build 593...
$versionInfo="$VersionMajor.$VersionMinor.$VersionSubIndex.";

$totalLines=0;
$statText = "Statistic:\n";

#@ErrorRecipients=("GEORGXP");
@ErrorRecipients=("UWEXP","MARTINXP","Tomas","GEORGXP","RolfXP","ANDREASHAGENXP");


##############
# </GLOBALS> #
##########+###

# set some global path variables
$ZIP_CMD="v:\\bin\\tools\\zip\\pkzip25.exe";
if (! -f "$ZIP_CMD")
{
	&NetSendError("pkzip.exe not found $ZIP_CMD");
	die "pkzip.exe not found $ZIP_CMD";
}

if ($#ARGV==0 && $ARGV[0] eq "-superCopy")	# last index, not count
{
	$bWorkAsSuperCopy = 1;
}
else
{
	$bWorkAsSuperCopy = 0;
}

&SetTimeVariables();
&ReadBuildNumber();

&SetTimeVariables();
print "Script started at $mday.$month.$year, $hour:$min:$sec\n";

# dependencie projects are not compiled
@DepProjects=(
				"include",

				"Capi/Capi4",
				"DV/DVClient",			# only for resource.h for LangDll
				"DV/LangDll",			# only for resource.h for DVClient
# c4lib.lib does not retain old version, it has to be compiled for every build!
#			    "Lib/C4Lib", # CodeBase 6.4
			    "Lib/jpeg",
				"Lib/WKClasses",
				"Tools/LogzipLib",
				"Units/JaCob/JaCobDA",	# OOPS
				"Units/JaCob/JaCobDll",	# OOPS only for jacobdefs.h
				"Units/JaCob/JaCobSys",
				"Units/SaVic2/SaVicDA",	
				"Units/SaVic2/SaVicDll",	
				"Units/SaVic2/SaVicSys",
				);

# LibProjects are compiled first, that is before the normal Projects
# CAVE: Order is importent, if mentioned
@LibProjects=(
			  "Lib/WKClasses",			# is already used in CIPC and other libs
			  "Cipc",					# is used in other libs

			  "DV/KeyHook",				# no real compile, just copy
			  "DV/LangDll",
			  "DV/Lib/Hooks",

			  "Lib/C4Lib",				# CodeBase, has to be compiled for elder versions with VersionLabel only
			  "Lib/DeviceDetect",
			  "Lib/HardwareInfo",
#			  "Lib/h263dec",
			  "Lib/h263enc",
			  "Lib/jpeg",
			  "Lib/JpegLib",
			  "Lib/NetComm",
			  "Lib/oemgui",
			  "Lib/Skins",

			  "Units/JaCob/JaCobDA",
			  "Units/JaCob/JaCobDll",
			  "Units/JaCob/JaCobSys",	# no real compile, just copy
			  "Units/JaCob/JaCobVxd",	# no real compile, just copy
			  "Units/SaVic2/SaVicDA",
			  "Units/SaVic2/SaVicDll",
			  "Units/SaVic2/SaVicSys",	# no real compile, just copy
			  );

# Liste aller relevanten Projekte
# CAVEAT muss auch weiter unten angepasst werden
# siehe CAVEAT2
# NEW PROJECT das WorkingDirectory muss auf Senso fuer D:\Project\NewProject gesetzt werden
# NEW PROJECT add here
@Projects=( 
			"Capi/ISDNUnit",

#			"Clients/Recherche",
#			"Clients/Vision",

#			"Dbs",

			"DV/acdc",
			"DV/DVClient",
			"DV/DVHook",
			"DV/DVProcess",
			"DV/DVStarter",
			"DV/DVStorage",
			"DV/RemoteCD",
#			"DV/SystemVerwaltung",
#			"DV/Tools/DVSoftAlarm",
#			"DV/Tools/DVSTest",
#			"DV/Tools/ExpansionCode",
			"DV/WatchCD",

#			"SecurityLauncher",
			"SocketUnit",

#			"Tools/CDStart",
			"Tools/CloneNT",
			"Tools/DelRegDb",
			"Tools/DvrtRun",
			"Tools/EULA",
			"Tools/JVIEW",
			"Tools/LogView",
#			"Tools/ProductVersion",
#			"Tools/ProductView",
			"Tools/RSA",
#			"Tools/SecAnalyzer",
#			"Tools/StressIt",
			"Tools/VipCleanNT",
			"Tools/Win95Reboot",

			"Units/CommUnit",
			"Units/JaCob/JaCobUnit",
#			"Units/JaCob/RegisterMonitor",
#			"Units/JaCob/JaCobClock",
#			"Units/JaCob/JaCobTest",
#			"Units/JaCob/JaCobEEPrommer",
			"Units/SaVic2/SaVicUnit",
			);

# force "flush output"
$|=1;

select(STDERR); $|=1;
select(STDOUT); $|=1;

$theCurrentProjectDir="UNKNOWN";
$theCurrentProjectDrive="UNKNOWN";

# hardcoded D:/Project
# create required directories
chdir ("d:\\");
&CreateNewDir("D:/dv/Dlls");
chdir ("d:\\");
&CreateNewDir("D:/Project/Units/JaCob/JaCobDA/Release");
chdir ("d:\\");
&CreateNewDir("D:/Project/Units/JaCob/JaCobDll/Release");
chdir ("d:\\");
&CreateNewDir("D:/Project/Units/JaCob/JaCobUnit/Release");
chdir ("d:\\");
&CreateNewDir("D:/Project/Units/JaCob/JaCobTest/Release");
chdir ("d:\\");
&CreateNewDir("D:/Project/Units/SaVic2/SaVicDA/Release");
chdir ("d:\\");
&CreateNewDir("D:/Project/Units/SaVic2/SaVicDll/Release");
chdir ("d:\\");
&CreateNewDir("D:/Project/Units/SaVic2/SaVicUnit/Release");
chdir ("d:\\");
&CreateNewDir("D:/Project/DV/LangDll/Libs");
chdir ("d:\\");

$theCurrentProjectDrive="D:";
$theProjectDir="D:\\Project";
print ("Using $theProjectDir\n");
#chdir("$theProjectDir") || die "chdir '$theProjectDir' failed\n";
if (!chdir("$theProjectDir"))
{
	&NetSendError("chdir '$theProjectDir' failed");
	die "chdir '$theProjectDir' failed\n";
}

&SearchSS();
&SearchMSDEV();
&SetEnv();

if (! -d "$theProjectDir\\Maps")
{
	`mkdir "$theProjectDir\\Maps`;
}

if (! -d "$theProjectDir\\Dlls")
{
	`mkdir "$theProjectDir\\Dlls`;
}

if ($bWorkAsSuperCopy==0)
{
	print "DVUnicodeMake started at $mday.$month.$year, $hour:$min, version is '$VersionLabel $currentBuild'\n";
	if ($bGetLibs)
	{
		#
		# get imagn32.lib
		#
		print ("*** Getting Libs and dependency projects... ***\n");
		
		if (! -d "$theProjectDir/Lib")
		{
			`mkdir "$theProjectDir/Lib"`;
		}

		chdir("$theProjectDir/Lib");
		print `$SAFE_CMD  Get \"\\$\\project\\lib\\imagn32.lib" -I- \"-VL$VersionLabel\"`;
# c4lib.lib does not retain old version, it has to be compiled for every build!
#		print `$SAFE_CMD  Get \"\\$\\project\\lib\\c4lib.lib" -I- \"-VL$VersionLabel\"`;
		print `$SAFE_CMD  Get \"\\$\\project\\lib\\ijl15.lib" -I- \"-VL$VersionLabel\"`;
		print `$SAFE_CMD  Get \"\\$\\dv\\acdc\\NeroApiGlue.lib" -I- \"-VL$VersionLabel\"`;

		#
		# get latest version from source safe
		#
		for (@DepProjects)
		{
			$project = $_;

			print ("*");

			chdir("/Project/");	# back to project root dir
			if (! -d $project)	# subdir already there ?
			{
				&CreateNewDir($project);
				chdir("/Project/");
			}

			chdir($project);

			# remove before the SourceSafe complains about a writablr copy
			# it´s recompiled anyway, thus it has no write-protection
			if (/JaCobDA/)
			{
				&MyUnlink("Release/JaCobDA.lib",0);
				&MyUnlink("Release/JaCobDA.dll",0);
				&MyUnlink("ReleaseUnicode/JaCobDAU.lib",0);
				&MyUnlink("ReleaseUnicode/JaCobDAU.dll",0);
			}
			if (/JaCobDll/)
			{
				&MyUnlink("Release/JaCob.lib",0);
				&MyUnlink("Release/JaCob.dll",0);
				&MyUnlink("ReleaseUnicode/JaCobU.lib",0);
				&MyUnlink("ReleaseUnicode/JaCobU.dll",0);
				&MyUnlink("EnuUnicode/JaCobU.lib",0);
				&MyUnlink("EnuUnicode/JaCobU.dll",0);
			}
			if (/SaVicDA/)
			{
				&MyUnlink("Release/SaVicDA.lib",0);
				&MyUnlink("Release/SaVicDA.dll",0);
				&MyUnlink("ReleaseUnicode/SaVicDAU.lib",0);
				&MyUnlink("ReleaseUnicode/SaVicDAU.dll",0);
			}
			if (/SaVicDll/)
			{
				&MyUnlink("Release/SaVic.lib",0);
				&MyUnlink("Release/SaVic.dll",0);
				&MyUnlink("ReleaseUnicode/SaVicU.lib",0);
				&MyUnlink("ReleaseUnicode/SaVicU.dll",0);
				&MyUnlink("EnuUnicode/SaVicU.lib",0);
				&MyUnlink("EnuUnicode/SaVicU.dll",0);
			}

			if (/DV/)
			{
				&TrySourceSafe("\$\\$project");
			}
			else
			{
				&TrySourceSafe("\$\\Project\\$project");
			}
		}
		print ("\n");
	}
	else
	{
		print ">>> Not getting DepProjects! <<<\n";
	}

	# loop over all LibProjects
	print ("*** Working on libs now... ***\n");
	for (@LibProjects)
	{
		local($project);
		$project=$_;

		chdir("/Project/");
		if (! -d $project)
		{
			&CreateNewDir($project);
			chdir("/Project/");
		}
		chdir($project);

		# remove all lib versions
		$_ = $project;
		if (/Cipc/)
		{
			&MyUnlink("Release/CIPC.lib",0);
			&MyUnlink("Release/CIPC.dll",0);
			&MyUnlink("ReleaseUnicode/CIPCU.lib",0);
			&MyUnlink("ReleaseUnicode/CIPCU.dll",0);
		}
		elsif (/JaCobDA/)
		{
			&MyUnlink("Release/JaCobDA.lib",0);
			&MyUnlink("Release/JaCobDA.dll",0);
			&MyUnlink("ReleaseUnicode/JaCobDAU.lib",0);
			&MyUnlink("ReleaseUnicode/JaCobDAU.dll",0);
		}
		elsif (/JaCobDll/)
		{
			&MyUnlink("Release/JaCob.lib",0);
			&MyUnlink("Release/JaCob.dll",0);
			&MyUnlink("ReleaseUnicode/JaCobU.lib",0);
			&MyUnlink("ReleaseUnicode/JaCobU.dll",0);
			&MyUnlink("EnuUnicode/JaCobU.lib",0);
			&MyUnlink("EnuUnicode/JaCobU.dll",0);
		}
		elsif (/SaVicDA/)
		{
			&MyUnlink("Release/SaVicDA.lib",0);
			&MyUnlink("Release/SaVicDA.dll",0);
			&MyUnlink("ReleaseUnicode/SaVicDAU.lib",0);
			&MyUnlink("ReleaseUnicode/SaVicDAU.dll",0);
		}
		elsif (/SaVicDll/)
		{
			&MyUnlink("Release/SaVic.lib",0);
			&MyUnlink("Release/SaVic.dll",0);
			&MyUnlink("ReleaseUnicode/SaVicU.lib",0);
			&MyUnlink("ReleaseUnicode/SaVicU.dll",0);
			&MyUnlink("EnuUnicode/SaVicU.lib",0);
			&MyUnlink("EnuUnicode/SaVicU.dll",0);
		}
		else
		{
			&MyUnlink("$theProjectDir/$project.lib",0);
			&MyUnlink("$theProjectDir/$project.dll",0);
#			print "else MyUnlink $project\n";
		}

		# get the latest/labeled version from source safe
		if (/DV/)
		{
			&TrySourceSafe("\$\\$project");
		}
		else
		{
			&TrySourceSafe("\$\\Project\\$project");
		}
		$theCurrentProjectDir="Project/$project";

		# remove not writable libs before compiling
		chdir($theCurrentProjectDir);
		$_ = $project;
		if (/Cipc/)
		{
			&MyUnlink("Release/CIPC.lib",1);
			&MyUnlink("Release/CIPC.dll",1);
			&MyUnlink("ReleaseUnicode/CIPCU.lib",1);
			&MyUnlink("ReleaseUnicode/CIPCU.dll",1);
		}
		elsif (/JaCobDA/)
		{
			&MyUnlink("Release/JaCobDA.lib",1);
			&MyUnlink("Release/JaCobDA.dll",1);
			&MyUnlink("ReleaseUnicode/JaCobDAU.lib",1);
			&MyUnlink("ReleaseUnicode/JaCobDAU.dll",1);
		}
		elsif (/JaCobDll/)
		{
			&MyUnlink("Release/JaCob.lib",1);
			&MyUnlink("Release/JaCob.dll",1);
			&MyUnlink("ReleaseUnicode/JaCobU.lib",1);
			&MyUnlink("ReleaseUnicode/JaCobU.dll",1);
			&MyUnlink("EnuUnicode/JaCobU.lib",1);
			&MyUnlink("EnuUnicode/JaCobU.dll",1);
		}
		elsif (/SaVicDA/)
		{
			&MyUnlink("Release/SaVicDA.lib",1);
			&MyUnlink("Release/SaVicDA.dll",1);
			&MyUnlink("ReleaseUnicode/SaVicDAU.lib",1);
			&MyUnlink("ReleaseUnicode/SaVicDAU.dll",1);
		}
		elsif (/SaVicDll/)
		{
			&MyUnlink("Release/SaVic.lib",1);
			&MyUnlink("Release/SaVic.dll",1);
			&MyUnlink("ReleaseUnicode/SaVicU.lib",1);
			&MyUnlink("ReleaseUnicode/SaVicU.dll",1);
			&MyUnlink("EnuUnicode/SaVicU.lib",1);
			&MyUnlink("EnuUnicode/SaVicU.dll",1);
		}
		
		&DoConfigurations($project);
	}	# end of loop over all LibProjects

	#
	# the main loop over all Projects
	print ("*** Working on projects now... ***\n");
	#
	for (@Projects)
	{
		local($project);
		$project=$_;

		chdir("/Project/");
		if (! -d $project)
		{
			&CreateNewDir($project);
			chdir("/Project/");
		}
		chdir($project);

		if (/DV/)
		{
			&TrySourceSafe("\$\\$project");
		}
		else
		{
			&TrySourceSafe("\$\\Project\\$project");
		}
		$theCurrentProjectDir="Project/$project";

		&DoConfigurations($project);
	}
	print ("\n$statText");
	print ("Lines in total: $totalLines\n");

	&SetTimeVariables();
	$MakeDoneText = "DVUnicodeMake DONE at $mday.$month.$year, $hour:$min:$sec,  version $versionInfo$currentBuild";
	print "\n$MakeDoneText\n";

	$MakeErrorText = "OK";
	if ($errorCount || $warningCount || $UnknownCount)
	{
		$MakeErrorText = "$errorCount error[s], $warningCount warning[s], $UnknownCount Unknown projects";
		print ("Es gab $MakeErrorText\n\n" );
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
	$MakeEndText = "DVUnicodeMake END at $mday.$month.$year, $hour:$min, version $versionInfo$currentBuild";
	print "\n$MakeEndText\n";
}
else	# NEW 171197 do super copy right here
{
#	bWorkAsSuperCopy == 1

	print "DVUnicodeCopy started at $mday.$month.$year, $hour:$min, version was '$VersionLabel', compiling for build $currentBuild\n";
	# switch to copy mode
	$bInFinalCopy = 1;	
	# and again loop over all projects, depending on $bInFinalCopy
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

	$MakeDoneText = "DVUnicodeCopy DONE at $mday.$month.$year, $hour:$min:$sec,  version $versionInfo$currentBuild";
	print "\n$MakeDoneText\n";

	$MakeErrorText = "OK";
	if ($theCopyErrorCount!=0)
	{
		$MakeErrorText = "$theCopyErrorCount Error[s]";
		print ("Es gab $theCopyErrorCount Fehler beim Kopieren\n");
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
	$MakeEndText = "DVUnicodeCopy END at $mday.$month.$year, $hour:$min, version $versionInfo$currentBuild";
	print "\n$MakeEndText\n";
}
   
#
# end of 'main'
#

#********************************************************************
sub DoConfigurations
{ 
	local($project)=@_;
	
	local(@cfgs);
	local($mak)="foo.mak";
	local($dest)="foo";

	# print("DO \"$project\"\n");
		
	# CAVEAT2 jedes Project muss hier eingetragen sein!
	# special settings per project

	# set default values
	$mak = "$project\.mak";
	$dest = "$project";

	#
	# CAVEAT order is important
	#
	$mak =~  s/^Units\/JaCob\///;	# drop leading Units/JaCob/JaCobUnit
	$dest =~ s/^Units\/JaCob\///;	# drop leading Units/JaCob/JaCobUnit

	$mak =~  s/^Units\/SaVic2\///;	# drop leading Units/SaVic2/SaVicUnit
	$dest =~ s/^Units\/SaVic2\///;	# drop leading Units/SaVic2/SaVicUnit

	$mak =~  s/^DV\/Tools\///;	# drop leading DV/Tools
	$dest =~ s/^DV\/Tools\///;

	$mak =~ s/^Units\///;	# drop leading Units/
	$dest =~ s/^Units\///;	# drop leading Units/

	$mak =~ s/^DV\///;	# drop leading 
	$dest =~ s/^DV\///;	# drop leading

	$mak =~ s/^Tools\///;	# drop leading Tools/
	$dest =~ s/^Tools\///;	# drop leading Tools/

	$mak =~ s/^Clients\///;	# drop leading Clients/
	$dest =~ s/^Clients\///;	# drop leading Clients/

	$mak =~ s/^Capi\///;	# drop leading Capi/
	$dest =~ s/^Capi\///;	# drop leading Capi/

	$mak =~ s/^Lib\///;		# drop leading Lib/
	$dest =~ s/^Lib\///;	# drop leading Lib/

	# NEW PROJECT add here, if on 'new' subdirectory

	local($bValidProject)=0;
	local ($bNoCompile)=0;

	if (   $project eq "DV/KeyHook"
		|| $project eq "Units/JaCob/JaCobSys"
		|| $project eq "Units/JaCob/JaCobVxd"
		|| $project eq "Units/SaVic2/SaVicSys"
		) 
	{
			$bNoCompile=1;
			$bValidProject=1;
			# still needs one cfg to get the copy done
			@cfgs=("Release");
	} 
	elsif ($project eq "Lib/h263dec")
	{
			@cfgs=("Release","H26x");
			$bValidProject=1;
	}
	elsif (	   $project eq "DV/RemoteCD"
			|| $project eq "DV/Tools/ExpansionCode"
			|| $project eq "DV/Tools/DVSoftAlarm"
			|| $project eq "DV/Tools/DVSTest"
			|| $project eq "DV/WatchCD"

			|| $project eq "Lib/C4Lib"
			|| $project eq "Lib/jpeg"
			|| $project eq "Lib/NetComm"
			|| $project eq "Lib/pil"

			|| $project eq "Tools/CDStart"
			|| $project eq "Tools/CloneNT"
			|| $project eq "Tools/DvrtRun"
			|| $project eq "Tools/JVIEW"
			|| $project eq "Tools/ProductVersion"
			|| $project eq "Tools/ProductView"
			|| $project eq "Tools/RSA"
			|| $project eq "Tools/SecAnalyzer"
			|| $project eq "Tools/StressIt"
			|| $project eq "Tools/Win95Reboot"

			|| $project eq "Units/JaCob/JaCobEEPrommer"
			|| $project eq "Units/JaCob/JaCobClock"
			|| $project eq "Units/JaCob/JaCobTest"
			|| $project eq "Units/JaCob/RegisterMonitor"
		) 
	{
		# nur release
		@cfgs=("Release");
		$bValidProject=1;
	}
	elsif (	   $project eq "Cipc"
			|| $project eq "Lib/WKClasses"
		  )
    {
		# nur release unicode
		@cfgs=("Release", "ReleaseUnicode");
		$bValidProject=1;
	}
	elsif (	   $project eq "DV/DVClient"
			|| $project eq "DV/DVHook"
			|| $project eq "DV/DVStarter"
			|| $project eq "DV/Lib/Hooks"
			|| $project eq "Lib/DeviceDetect"
			|| $project eq "Lib/h263enc"
			|| $project eq "Lib/HardwareInfo"
			|| $project eq "Lib/JpegLib"
			|| $project eq "Lib/oemgui"
			|| $project eq "Lib/Skins"
			|| $project eq "Units/JaCob/JaCobDA"
			|| $project eq "Units/SaVic2/SaVicDA"
		  )
    {
		# nur release unicode
		@cfgs=("ReleaseUnicode");
		$bValidProject=1;
	}
	elsif (    $project eq "DV/SystemVerwaltung"
			|| $project eq "SecurityLauncher"
			|| $project eq "Dbs"
			|| $project eq "Tools/EULA"
			)
	{
		# release und enu only
		@cfgs=("Release", "Enu");
		$bValidProject=1;
	} 
	elsif (    $project eq "DV/acdc"
			|| $project eq "DV/DVProcess"
			|| $project eq "DV/DVStorage"
			|| $project eq "Units/CommUnit"
			|| $project eq "Units/JaCob/JaCobDll"
			|| $project eq "Units/JaCob/JaCobUnit"
			|| $project eq "Units/SaVic2/SaVicDll"
			|| $project eq "Units/SaVic2/SaVicUnit"
		  )
    {
		# nur release unicode und enu unicode
		@cfgs=("ReleaseUnicode", "EnuUnicode");
		$bValidProject=1;
	}
	elsif (    $project eq "Capi/ISDNUnit"
			|| $project eq "SocketUnit"
			|| $project eq "Tools/LogView"
			|| $project eq "Tools/VipCleanNT"
			)
	{
		@cfgs=("DtsUnicode");
		$bValidProject=1;
	}
	elsif (    $project eq "Clients/Vision"
			|| $project eq "Clients/Recherche"
			|| $project eq "Tools/DelRegDb"
		  )
    {
		# release und enu ita
		@cfgs=("Dts", "DtsEnu");
		$bValidProject=1;
    }
	elsif (    $project eq "DV/LangDll")
	{
		# alle Sprachversionen, hier muss ggf erweitert werden
		@cfgs=("ChsU", "DeuU","EnuU", "EspU","ItaU", "PtgU");
		$bValidProject=1;
	} 
	else 
	{
		$bValidProject=0;
		$UnknownCount++;
		print "Unknown project \"$project\"\n";
	}

	# a valid project ? Then loop over the given configurations.
	if ($bValidProject) 
	{
		if ($bInFinalCopy==0) 
		{ # it´s not copy; do the nmake
			if ($bNoCompile==0) 
			{	# some projects do not need to be compiled (DDK stuff etc.)

				if (!($project eq "Lib/C4Lib"))
				{
					&DoStaticAnalysis();
				}

				for (@cfgs) 
				{
					if ($bTraceOneConfig) 
					{
						print ("DoOne $project,$mak,$dest\n");
					}
					&DoOneConfiguration($mak,$dest,$_);
				}
			}
		} 
		else 
		{ # do not do the make, but do the copy
			for (@cfgs) 
			{
				&DoCopy($project,$dest,$_);
			}
		}	# end of copy action
	} 
	else 
	{
		print ("Invalid project:$project\n");
	}

	if ($bWorkAsSuperCopy==0)
	{
		# NOT YET check for AutoDuck directory ?
		if (grep (/^$project$/,@DocProjects)) # is it a AutoDuck project
		{
			@makeDocsLog = `v:\\Dokumentation\\AutoDuck\\makedocs.bat $dest`;
			for ($mi=0;$mi <= $#makeDocsLog;$mi++) {
				$_=$makeDocsLog[$mi];
				if ( /makedocs DONE/) {
					# supress
				} else {
					print ($_);
				}
			}
		}
		else 
		{
			# print  ("No Autoduck for $project\n");
		}
	}
}

#********************************************************************
sub DoOneConfiguration
{
	local($mak,$dest,$cfg)=@_;
	local($oneLine)="";
	local($ei)=0;

	# print " .... $mak CFG=\"$dest - Win32 $cfg\"\n";

	# GF 22.10.2002
	# Static analysis moved sub routine DoStaticAnalysis()
	# called from DoConfigurations only once per project

	# new style call msdev, avoiding the .dep files in each project
	local($dspName)="";
	$dspName  = $mak;
	$dspName =~ s/\.mak/.dsp/;

	if ($cfg ne "PTUnitRelease") {
		&UpdateRC2();
	}

	chmod 0755, "$dspName";	# allow for local conversion
	if ($bRebuildAll)
	{
		print "compiling $dest - Win32 $cfg, rebuild all\n";
		@errorLog = `msdev.exe $dspName /MAKE \"$dest - Win32 $cfg\" /REBUILD /USEENV `;
	}
	else
	{
		print "compiling $dest - Win32 $cfg\n";
		@errorLog = `msdev.exe $dspName /MAKE \"$dest - Win32 $cfg\" /USEENV `;
	}
	chmod 0555, "$dspName";	# undo the above operation, make it readonly again

	if ($#errorLog>0) {
		for ($ei=0;$ei <= $#errorLog;$ei++) {
			$oneLine=$errorLog[$ei];

			# print ("MAK:$oneLine\n");
			# modify filenames, to allow easy F4 stepping
			$_ = $oneLine;	# OOPS
			if ( /kopiert/ || /copied/ 
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
				) {
				# do not print		
				$oneLine = "";
			} elsif ($bPrintDSPConversion==0
					&& (/by a previous/ || /your original project/ )) {
				# do not print		
				$oneLine = "";
			}
			$_ = $oneLine;

			
			# count make errors and warnings
			if (   / error /
				|| /Error: There are no valid targets to build/
				)
			{
				if (/LNK4089/ || /LNK4033/)
				{
					print "don not count $oneLine Linker Error\n";
				}
				else
				{
					$errorCount++;
				}
			}
			if (/ [wW]arning /) {
				$warningCount++;
			}

			print ("$oneLine");	# already has newline

		}
	} else {
		print "Done  $mak CFG=\"$dest - Win32 $cfg\"\n";
	}
}

#********************************************************************
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
			$statText .= $oneLine;
			$oneLine =~ s/\s+/ /gm;
			$oneLine =~ s/^\s+//gm;
			# looks like "#STAT# 8191 lines in d:\Project\SecurityLauncher"
			($tmpJunk,$lines,$restJunk) = split(/ /,$oneLine,3);
			$totalLines += $lines;
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
				$warningCount++;
			}
		}
	}
}

#********************************************************************
sub DoCopy
{
	local($project,$dest,$cfg)=@_;

	if ($bTraceCopy) 
	{
		printf("DoCopy $project,$dest,$cfg ; current dir %s\n", getcwd);
	}

	# make the destination dependend from the configuration
	local($destinationDir)=$theDestinationDir;
	$destinationDir = $theDestinationDir;

#	print "COPYING $project\n";

	if (   $project eq "Clients/Vision"
		|| $project eq "Clients/Recherche"
		|| $project eq "DV/SystemVerwaltung"
		|| $project eq "Tools/DelRegDb"
		|| $project eq "Tools/EULA"
		|| $project eq "SecurityLauncher"
		|| $project eq "Dbs"
		|| $project eq "Lib/oemgui"
	   )
	{
		if (   $cfg eq "Enu"
		    || $cfg eq "DtsEnu"
			|| $cfg eq "EnuUnicode"
			)
		{
			print "ENGLISH DoCopy $project $dest $cfg\n";
			$destinationDir = $theDestinationDirEnu;
		}
	}

	# create non existing dir
	if (! -d "$destinationDir") {
		`mkdir $destinationDir`;
	}

	local ($bDoCopy)=1;

	$_ = $project;
	if (/Cipc/)
	{
		&MyDoCopy("$theProjectDir\\DV\\DVStarter\\TimeZonesNT.reg","$destinationDir\\TimeZonesNT.reg",1);	# do count errors
		$_ = $cfg;
		if (/Unicode/)
		{
			$dest .= "U";
			$source = "$project\\$cfg\\$dest.dll";
			$finalDest = "$destinationDir\\Dlls\\$dest.dll";
		}
		else
		{
			$source = "$project\\$cfg\\$dest.dll";
			$finalDest = "$destinationDir\\Dlls\\$dest.dll";
		}
	}
	elsif (/h263dec/)
	{
		$source = "$project\\H26x\\$dest.dll";
		$finalDest = "$destinationDir\\Dlls\\$dest.dll";
	}
	elsif (   /NetComm/
		  ) 
	{
		$source = "$theProjectDir\\Lib\\$dest.dll";
		$finalDest = "$destinationDir\\Dlls\\$dest.dll";
	}
	elsif (   /DeviceDetect/
		   || /h263enc/
		   || /HardwareInfo/
		   || /JpegLib/
		   || /oemgui/
		   || /Skins/
		  ) 
	{
		$dest .= "U";
		$source = "$theProjectDir\\Lib\\$dest.dll";
		$finalDest = "$destinationDir\\Dlls\\$dest.dll";
	}
	elsif (    /JaCobDA/
			|| /SaVicDA/
			)
	{
		$dest .= "U";
		$source = "$project\\$cfg\\$dest.dll";
		$finalDest = "$destinationDir\\Dlls\\$dest.dll";
	}
	elsif (/JaCobDll/)
	{
		$dest = "JaCobU";
		$source = "$project\\$cfg\\$dest.dll";
		$finalDest = "$destinationDir\\Dlls\\$dest.dll";
	}
	elsif (/SaVicDll/)
	{
		$dest = "SaVicU";
		$source = "$project\\$cfg\\$dest.dll";
		$finalDest = "$destinationDir\\Dlls\\$dest.dll";
	}
	elsif (/JaCobVxd/)
	{
		$source = "$project\\bin\\jacob.vxd";
		$finalDest = "$destinationDir\\JaCob.vxd";
	}
	elsif (/JaCobSys/)
	{
		$source = "$project\\bin\\JaCob.sys";
		$finalDest = "$destinationDir\\JaCob.sys";
		&MyDoCopy("$theProjectDir\\Units\\JaCob\\JaCobUnit\\JaCob.inf","$destinationDir\\JaCob.inf",1);	# do count errors
	}
	elsif (/SaVicSys/)
	{
		$source = "$project\\bin\\SaVic.sys";
		$finalDest = "$destinationDir\\SaVic.sys";
		&MyDoCopy("$theProjectDir\\Units\\SaVic2\\SaVicSys\\Bin\\SaVic.inf","$destinationDir\\SaVic.inf",1);	# do count errors
	}
	elsif (/KeyHook/)
	{
		$source = "$theProjectDir\\DV\\KeyHook\\bin\\KeyHook.sys";
		$finalDest = "$destinationDir\\KeyHook.sys";
	}
	elsif (/Hooks/)
	{
		$dest .= "U";
		$source = "$theProjectDir\\DV\\Lib\\$dest.dll";
		$finalDest = "$destinationDir\\Dlls\\$dest.dll";
	}
	elsif (/LangDll/)
	{
		local($config) = $cfg;
		chop($config);
		local($langdll) = "dvu$config.dll";
		$source = "$project\\$cfg\\$langdll";
		$finalDest = "$destinationDir\\Dlls\\$langdll";
	}
	elsif (/VipCleanNT/)
	{
		$source = "$project\\$cfg\\FactoryReset.exe";
		$finalDest = "$destinationDir\\FactoryReset.exe";
	}
	elsif (    /WKClasses/
			|| /jpeg/
			|| /C4Lib/
			|| /pil/
			)
	{
		$bDoCopy=0;
	} else {
		#
		# .exe 
		#
		$source = "$project\\$cfg\\$dest.exe";
		$finalDest = "$destinationDir\\$dest.exe";
	}
	
	if ($bDoCopy) {

		# .exe or .dll first
		&MyDoCopy($source,$finalDest,1);	# do count errors

		# followed by.map
		#

		# destination is Maps, but not CoCo/Maps
		$destinationDir .= "\\Maps";
		# remove coco dir
		$destinationDir =~ s/\\CoCo\\/\\/;

		# create if not already there
		if (! -d "$destinationDir") {
			`mkdir $destinationDir`;
		}
		
		$source =~ s/\.exe/.map/;
		$source =~ s/\.dll/.map/;

		$_ = $project;
		if (/Clients\\Vision/)
		{
			$dest = "Vision40";
		}
		elsif(
				 /DeviceDetect/
			  || /HardwareInfo/
			  || /Hooks/
			  || /oemgui/
			  || /JpegLib/
			  || /h263dec/
			  || /h263enc/
			  ) 
		{
			# .map are not in Lib/ but in their $cfg/ directories
			$source = "$project\\$cfg\\$dest.map";
		}
		elsif ( /MiCoSys/ || /MiCoVxd/ || /JaCobSys/ || /JaCobVxD/ || /SaVicSys/ || /AkuSys/ || /KeyHook/)
		{
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

#********************************************************************
sub UpdateBuild
{
	# update the BuildNumber
	print ("Updating $theDestinationDir/vinfo.pvi\n");
	`v:\\bin\\dv\\ProductVersion -updateDirectory $theDestinationDir`;

	# Compressing german executable
#	print "Compressing $theDestinationDir\\*.exe...\n";
#	`upx $theDestinationDir\\JView.exe`;
#	print "Ready\n";

	# Compressing german dlls
#	print "Compressing $theDestinationDir\\Ms_dll\\*.dll...\n";
#	`upx $theDestinationDir\\Ms_dll\\*.dll`;
#	print "Ready\n";

	# Compressing german dlls
#	print "Compressing $theDestinationDir\\Dlls\\*.dll...\n";
#	`upx $theDestinationDir\\Dlls\\*.dll`;
#	print "Ready\n";


	chdir($theCurrentProjectDrive);	
	&CollectBuildNotes();
    &CopyBuildNotes();

	# NEW 16.10.98 no date/time in label
	$LabelText = "$LabelPrefix$versionInfo$currentBuild";
    # set a version label in the SourceSafe
    print "Setting SourceSafeLabel: $LabelText\n";
	`$SAFE_CMD Label \$/Project -C- -L$LabelText`;
	`$SAFE_CMD Label \$/DV -C- -L$LabelText`;

	# safe label text in file.
	open (LABEL_OUT_FILE,">$theDestinationDir\\label.txt");
	print(LABEL_OUT_FILE "$LabelText\n");
	close(LABEL_OUT_FILE);

	# safe label text in file.
	open (VERSION_OUT_FILE,">$theDestinationDir\\version.txt");
	print(VERSION_OUT_FILE "$versionInfo$currentBuild\n");
	close(VERSION_OUT_FILE);

	chdir($theDestinationDir);
	local($BuildPrefix) = "Unicode";
	local($BuildZipDir) = "BuildDVU";
	print "Creating g:\\$BuildZipDir\\$BuildPrefix$currentBuild.zip\n";
    print "executing $ZIP_CMD -add -recurse -directories -silent g:\\$BuildZipDir\\$BuildPrefix$currentBuild.zip $theDestinationDir\\*.*\n";
	`$ZIP_CMD -add -recurse -directories -silent g:\\$BuildZipDir\\$BuildPrefix$currentBuild.zip $theDestinationDir\\*.*`;
}

#********************************************************************
sub CollectBuildNotes
{
	$filename = "$theDestinationDir\\BuildNotes$currentBuild.htm";
	&MyUnlink($filename);
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

	$prevLabelText = "";
	open (LABEL_IN_FILE,"<$theDestinationDir\\label.txt");
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
		if ($bTraceSuperHistory) {
			printf("Searching history for $project\n");
		}
		chdir("$theProjectDir/$project");

		if (-f "history.txt") 
		{
			# -IECW ignore case/end of line/white space changes
			if (/DV/)
			{
				if ($bTraceSuperHistory) {
					printf("Searching history in %s\n",getcwd());
					print "$SAFE_CMD Diff \$/$project/history.txt -DS200 -IECW -VL$prevLabelText > SuperHistory.txt\n";
				}
				print `$SAFE_CMD Diff \$/$project/history.txt -DS200 -IECW -VL$prevLabelText > SuperHistory.txt`;
			}
			else
			{
				if ($bTraceSuperHistory) {
					printf("Searching history in %s\n",getcwd());
					print "$SAFE_CMD Diff \$/Project/$project/history.txt -DS200 -IECW -VL$prevLabelText > SuperHistory.txt\n";
				}
				print `$SAFE_CMD Diff \$/Project/$project/history.txt -DS200 -IECW -VL$prevLabelText > SuperHistory.txt`;
			}
			# now superHistory.txt is created
			# collect between the topmost Label:"Build
			# from this take only Chg: To : Ins:
			local($bAlreadyDidPrint)=0;
			local($bAlreadyDidPrintText)=0;
			if (-f "SuperHistory.txt")
			{
				open(HFILE,"SuperHistory.txt") || warn "can't open SuperHistory.txt\n";
				if ($bTraceSuperHistory) {
					printf ("Scanning SuperHistory.txt in %s...\n",getcwd());
				}
				while (<HFILE>) {
					if (/^Against:/ 
					|| /^Diffing:/) {
						# ignore
					} elsif (/No differences/) {
						# ignore
					} elsif (/\w/) { # at least one word
						# inTop and important stuff
						if ($bAlreadyDidPrint==0) {
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


						if (/^\s*$/ && $bAlreadyDidPrintText==0) {
							# drop leading blank lines
						} else {
							print ($_);
							print (OFILE $_);
						}
					} else {
						# empty line, not a single word
					}
				}

				if ($bAlreadyDidPrint) {
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

#********************************************************************
sub CopyBuildNotes
{
	$source = "$theDestinationDir\\BuildNotes$currentBuild.htm";
	$dest   = "\\\\mail01\\buildnotes\\dts\\Unicode$currentBuild.htm";
	print `copy $source $dest`;
	print ("CopyBuildNotes done.\n");
}

#********************************************************************
close(STDERR);
close(STDOUT);

#********************************************************************
sub TrySourceSafe
{
	local($proj)=@_;
	if ($bTraceSourceSafe)
	{
		print ("Getting $proj from SourceSafe...\n");
	}

	if (-d "res")
	{
		# delete modified rc2
	}

	# check if dsp file has changed
	$bRebuildAll = 0;
	local($dspPath) = $proj;
	local($dspFile) = "";
#	print ("$dspPath\n");

	local(@array)  = split(/\\/,$dspPath);     # Array der einzelnen Teilstrings

	# last one is project itself
	foreach $x (@array)
	{
		$dspFile = $x;
	}
#	print ("$dspFile\n");
	if ($dspFile eq "SecurityServer")
	{	# dsp file has different name than project
		$dspFile = "sec3";
	}
	$dspFile .= "\.dsp";
#	print ("$dspFile\n");
	$dspPath = "$dspPath\\$dspFile";
#	print ("$dspPath\n");

	# save old file time
	local($time_secs_old) = (stat($dspFile))[9]; 
#	print ("time_secs_old $time_secs_old\n");

	# -R recusive
	# -I no input
	# -VL version by label
	if ($VersionLabel ne "") {
		`$SAFE_CMD  Get \"$proj\" -R -I- -O\@vsstmp.txt \"-VL$VersionLabel\"`;
	} else {
		`$SAFE_CMD  Get \"$proj\" -R -I- -O\@vsstmp.txt`;
	}

	# get new file time	
	local($time_secs_new) = (stat($dspFile))[9]; 
#	print ("time_secs_new $time_secs_new\n");

	# if file time changed -> file changed -> rebuild all
	if ($time_secs_new != $time_secs_old)
	{
		$bRebuildAll = 1;
	}
#	print ("Rebuild $bRebuildAll\n");

	if ($bTraceSourceSafe) {
		# NOT YET print ss
	}
	unlink("vsstmp.txt");
}

#********************************************************************
sub CreateNewDir
{
	local($pname)=@_;
	# print "CreateSubDirs $pname\n";
	local(@subdirs) = split("/",$pname);
	foreach (@subdirs) {
		# special check for D:/
		if (! -d $_ && !/:/) {	# subdirectories can exists like Units/SimUnits
			printf ("Create $_ in %s\n",getcwd);
			system("mkdir $_");
			print ("Created $_\n");
		}
		if (-d $_ || /:/ ) {
			chdir($_);
		} elsif (/:/) {
			chdir ("$_\\");	# absolute path, back to root dir
		} else {
			print ("Failed $_\n");
		}
	}
}

#********************************************************************
sub MyUnlink
{
	local($oneFile,$bDoMsg)=@_;

	if ($bTraceUnlink) {
		printf ("MyUnlink for $oneFile, in %s, msg $bDoMsg\n",getcwd());
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
			print ("MyUnlink: $oneFile not found\n");
		}
	}

}

#********************************************************************
# usage:
# &MyDoCopy("Source","Dest",bDoCountErrors);
sub MyDoCopy
{
	local($source,$finalDest,$bDoCountError)=@_;
	local($copyError) = "";

	if (-f $source) {
		print ("Copy $source --> $finalDest\n");
		$cmd = "copy $source $finalDest";
		$cmd =~ s/\//\\/gm;
#		printf("$cmd ; current dir %s\n", getcwd);
		$copyError = `$cmd`;

		$_ = $copyError;
		if (/0 Datei/) {
			$copyError =~ s/\s+0 Datei.*\n//;
			print ("$copyError");
			if ($bDoCountError) {
				$theCopyErrorCount++;
				print ("ERROR DURING COPY \n");
				&NetSendError("ERROR DURING COPY $copyError");
				die;
			} else {
				print ("Error not counted \n");
			}
		}

	} else {
		if ($bDoCountError) 
		{
			$theCopyErrorCount++;
			print "File not found, $source\n";
			print ("ERROR DURING COPY \n");
			&NetSendError("ERROR DURING COPY $source not found");
			die;
		} 
		else 
		{
			print "File not found, $source, but not counted\n";
		}
	}
}

#********************************************************************
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

#********************************************************************
sub ReadBuildNumber
{
	if ($buildNumber==0) {
		local($tmpVersion)=0;
		local($tmpNumRecord)=0;

		# scan the first line of vinfo.pvi
		open(FILE,"$theDestinationDir/vinfo.pvi") || warn "can't open vinfo.pvi\n";
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

#********************************************************************
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

#********************************************************************
sub SearchMSDEV
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
	print "Found $MSDEV_DIR\n";
}	# end of SearchMSDEV

#********************************************************************
sub SearchSS
{
	if ( -f "C:/vss/win32/ss.exe" ) {
		$SAFE_CMD="c:\\vss\\win32\\ss.exe";
	} elsif ( -f "c:/msdev/vss/win32/ss.exe" ) {
		$SAFE_CMD="C:\\msdev\\vss\\win32\\ss.exe";
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

#********************************************************************
sub SetEnv
{
	# set env variables, not using V:
	# CAVEAT requires $theProjectDir
	
	$TmpInclude = "$MSDEV_DIR\\Include;$MSDEV_DIR\\mfc\\include;$MSDEV_DIR\\atl\\include;$MSDEV_DIR\\mfc\\src;";
	$TmpInclude .= "$theProjectDir\\Include;$theProjectDir\\Cipc;$theProjectDir\\lib;$theProjectDir\\Dv\\lib;";
	if (-d "d:/Microsoft SDK/Include")
	{
		$TmpInclude .= "d:\\Microsoft SDK\\Include;";
	}
	elsif (-d "c:/Microsoft SDK/Include")
	{
		$TmpInclude .= "c:\\Microsoft SDK\\Include;";
	}
	print ("INCLUDE IS: $TmpInclude\n");

	$ENV{'INCLUDE'}=$TmpInclude;
	$TmpLib = "d:\\inetsdk\\lib;$MSDEV_DIR\\lib;$MSDEV_DIR\\mfc\\lib;$theProjectDir\\Lib;";
	$TmpLib .= "$theProjectDir\\Cipc\\Release;$theProjectDir\\Cipc\\ReleaseUnicode;$theProjectDir\\DV\\Lib;";
	if (-d "d:/Microsoft SDK/Lib")
	{
		$TmpLib .= "d:\\Microsoft SDK\\Lib;";
	}
	elsif (-d "c:/Microsoft SDK/Lib")
	{
		$TmpLib .= "c:\\Microsoft SDK\\Lib;";
	}
	$ENV{'LIB'} = $TmpLib;
	print ("LIB IS: $TmpLib\n");
	
	# make sure ftp.exe is in the path
	print "PATH IS $ENV{'PATH'}\n";
#	if (-d "c:\\Msdev98") {
#		$ENV{'PATH'} = "$MSDEV_DIR\\bin;C:\\Winnt;C:\\Winnt\\system32;c:\\MsDev98\\Common\\Msdev98\\bin;c:\\Msdev98\\Common\\Tools;$theDestinationDir\\Dlls";
#	} elsif (-d "c:\\Msdev") {
#		$ENV{'PATH'} = "c:\\msdev\\common\\vc98\\bin;c:\\winnt;c:\\winnt\\system32;$theDestinationDir\\Dlls";
#	} elsif (-d "D:\\Msdev98") {
#		$ENV{'PATH'} = "$MSDEV_DIR\\bin;c:\\msdev\\sharedide\\bin;C:\\Windows;D:\\DevStudio\\Tools;D:\\MsDev98\\Common\\Msdev98\\bin;D:\\Msdev98\\Common\\Tools;$theDestinationDir\\Dlls";
#	} else {
#		$ENV{'PATH'} = "$MSDEV_DIR\\bin;c:\\msdev\\sharedide\\bin;C:\\Windows;D:\\DevStudio\\Tools;$theDestinationDir\\Dlls";
#	}
#	print "PATH IS $ENV{'PATH'}\n";
#	die;
}

#********************************************************************
sub NetSendError
{
	local($sMessage)=@_;

	for (@ErrorRecipients)
	{
		if ($bWorkAsSuperMake)
		{
			`Net Send $_ "$sMessage from SuperMake"`;
		}
		elsif ($bWorkAsSuperCopy)
		{
			`Net Send $_ "$sMessage from SuperCopy"`;
		}
		else
		{
			`Net Send $_ "$sMessage"`;
		}
	}
}

#********************************************************************

__END__
:endofperl
