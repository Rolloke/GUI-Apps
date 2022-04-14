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
$bTraceCopy=0;
$bTraceUnlink=0;
$bTraceSuperHistory=0;
$bTraceSourceSafe=0;	# NOT YET implemented
$bGetLibs=1;	# disable for faster testing
$bCompileLibs=1;	# disable for faster testing
$bPrintDSPConversion=0; # default 0; print local dsp conversion

#############
# <GLOBALS> #
#############
$bWorkAsSuperMake = 1;
$bWorkAsSuperCopy = 0;
$bWorkAsSuperHistory = 0;
$bInFinalCopy = 0;
$theCopyErrorCount=0;	# modified in MyDoCopy
$errorCount=0;
$warningCount=0;
$UnknownCount=0;
$bStopOnError=0;
@g_CheckTranslations=();
$bRebuildAll=0;		# rebuild all projects
$bRebuildProj=0;	# rebuild all if dsp changed, see TrySourceSafe()

$theDestinationDir="v:\\bin\\dvrt";	# has to contain vinfo.pvi for the buildNumber info
$theDestinationDirEnu="v:\\bin\\dvrtenu";	# has to contain vinfo.pvi for the buildNumber info
$theDestinationDirIta="v:\\bin\\dvrtita";	# has to contain vinfo.pvi for the buildNumber info

# $VersionLabel="4.4.1.527"; # >>> leer fuer aktuelle Version <<<
# oder mit VersionsLabel passend zum SourceSafe $/Project label
#$VersionLabel="4.6.3.633";

$buildNumber=0;	# read from vinfo.pvi, but used in multiple functions
$currentBuild=1;	# updated in ReadBuild
# >>> do not remove or modify this line, copy and uncomment <<<
#$versionInfo="4.2.";	# for build 197...
$VersionMajor=4;
$VersionMinor=7;
$VersionSubIndex=1;
$versionInfo="$VersionMajor.$VersionMinor.$VersionSubIndex.";	# since build 197...

$totalLines=0;
$statText = "Statistic:\n";

@ErrorRecipients=("UWEXP","TOMAS","MARTINXP","GEORGXP","ROLFXP","ANDREASHAGENXP");


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

if ($#ARGV==0)
{  
	if ($ARGV[0] eq "-superCopy")	# last index, not count
	{
		$bWorkAsSuperCopy = 1;
		$bWorkAsSuperMake = 0;
	}
	elsif ($ARGV[0] eq "-superHistory")	# last index, not count
	{
		$bWorkAsSuperHistory = 1;
		$bWorkAsSuperMake = 0;
	}
}

&SetTimeVariables();
&ReadBuildNumber();

print "Script started at $mday.$month.$year, $hour:$min:$sec ; Versionlabel '$VersionLabel' ; working for '$versionInfo$currentBuild'\n";

# dependencie projects are not compiled
@DepProjects=(
				"include",
				"DirectShow",
				"Capi/Capi4",
#				"Lib/jpeg",
				"Lib/luca",
				"Lib/WKClasses",
				"Cipc",					# WKClasses uses some header...	
			    "Lib/C4Lib",			# CodeBase 6.5
			    "Lib/NetComm",
				"Tools/LogzipLib",
				"Units/MiCo/MiCoSys",
				"Units/MiCo/MiCoDA",	  
				"Units/MiCo/MiCoUnitPCI",
				"Units/JaCob/JaCobSys",
				"Units/JaCob/JaCobDA",
				"Units/JaCob/JaCobDll",
				"Units/Aku/AkuSys",
				"Units/SaVic2/SaVicSys",
				"Units/SaVic2/SaVicDA",
				"Units/SaVic2/SaVicDll",
				);

# LibProjects are compiled first, that is before the normal Projects
# CAVEAT muss auch weiter unten angepasst werden
# siehe CAVEAT2 und CAVEAT3
@LibProjects=(
# VS 71 need  "Capi/Capi4",
			  "Lib/WKClasses",		# is already used in CIPC
# VS 71 need  "Tools/LogZipLib",
			  "Cipc",				# is used in all projects
			  "Lib/Skins",			# is already used in OemGui

			  "Lib/Crypto",
			  "Lib/DeviceDetect",
#			  "Lib/h263dec",
#			  "Lib/h263enc",
			  "Lib/ICPCONDll",
#			  "Lib/jpeg",
			  "Lib/JpegLib",
			  "Lib/oemgui",
#			  "Lib/PTDecoder",
#			  "Lib/ReleaseAlloc",
			  "Lib/VImage",

			  "MegraV2/Convert",
			  "MegraV2/MegraV2DLL",

			  "Units/JaCob/JaCobDA",
			  "Units/JaCob/JaCobDll",
			  "Units/MiCo/MiCoDA",
			  "Units/SaVic2/SaVicDA",
			  "Units/SaVic2/SaVicDll",

			  "Units/Aku/AkuSys",		# no real compile, just copy
			  "Units/JaCob/JaCobSys",	# no real compile, just copy
			  "Units/JaCob/JaCobVxd",	# no real compile, just copy
			  "Units/MiCo/MiCoSys",		# no real compile, just copy
			  "Units/MiCo/MiCoVxd",		# no real compile, just copy
			  "Units/SaVic2/SaVicSys",	# no real compile, just copy

			   "DirectShow/Filters/baseclasses",		# a lib
			   "DirectShow/Filters/dump",				# ax
			   "DirectShow/Filters/inftee",				# ax
			   "DirectShow/Filters/mediasource",		# ax
			   "DirectShow/Filters/PushSource",			# ax
# obsolete			   "DirectShow/Filters/Sample2MemRenderer",	# ax
			   "DirectShow/Filters/WaveSplitter",		# ax
			   "DirectShow/Common",						# dll
			  );

# project which have AutoDuck comments
@DocProjects = (
#				"Cipc",
#				"Lib/WKClasses",
#				"Lib/h263"
				);


# Liste aller relevanten Projekte
# CAVEAT muss auch weiter unten angepasst werden
# siehe CAVEAT2 und CAVEAT3
# NEW PROJECT das WorkingDirectory muss auf Senso fuer D:\Project\NewProject gesetzt werden
# NEW PROJECT add here
@Projects=( 
			"Capi/ISDNUnit",		# also contains PTUnit			

			"DV/acdc",

			"Units/Aku/AkuUnit", 
			"Units/Yuta/USBAlarmUnit", 
			"Units/CoCoUnit",
			"Units/CommUnit", 
			"Units/GemosUnit",
			"Units/EmailUnit",
			"Units/MiCo/MiCoUnitPCI",
			"Units/ICPCONUnit",
			"Units/JaCob/JaCobUnit",
			"Units/JaCob/JaCobTest",
			"Units/SaVic2/SaVicUnit",
			"Units/SDIUnit",
			"Units/SimUnit",
			"Units/SMSUnit",
#			"Units/TOBSUnit",
#			"Units/USBCam/USBCamUnit",
			"Units/Audio/AudioUnit",

			"Clients/ICPCONClient",
			"Clients/Vision",
			"Clients/Recherche",
			"HHLA/MiniClient",
			"HHLA/RemoteControl",

			"Dbs",

			"Tools/convntfs",
			"Tools/RSA",
			"Tools/DelRegDb",
			"Tools/RegMove",
			"Tools/RegPriv",
			"Tools/LogZip",
			"Tools/ProductView",
			"Tools/ProductVersion",
			"Tools/SecAnalyzer",
			"Tools/LogView",
			"Tools/Win95Reboot",
			"Tools/DvrtRun",
			"Tools/UpdateHandler",
			"Tools/EULA",
			"Tools/CDStart",

			# DROPPED for 4.0 "SecurityExplorer",	# alias vision			
			"SecurityLauncher",
			"SecurityServer", 
			"SystemVerwaltung",
				
			"SocketUnit",

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
&CreateNewDir("D:/dvrt/Dlls");
chdir ("d:\\");
&CreateNewDir("D:/Project/Units/MiCo/MicoDA/Release");
chdir ("d:\\");
&CreateNewDir("D:/Project/Units/MiCo/MicoUnitPCI/Release");
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


if ($bWorkAsSuperMake==1)
{
	&SetTimeVariables();
	print "idipMake started at $mday.$month.$year, $hour:$min:$sec ; Versionlabel '$VersionLabel' ; compiling for '$versionInfo$currentBuild'\n";

	if ($bGetLibs)
	{
		#
		# get imagn32.lib
		#
		# print ("Getting imagn32.lib....\n");
		
		if (! -d "$theProjectDir/Lib")
		{
			`mkdir "$theProjectDir/Lib"`;
		}

		chdir("$theProjectDir/Lib");
		print `$SAFE_CMD  Get \"\\$\\project\\lib\\imagn32.lib" -I- \"-VL$VersionLabel\"`;
		print `$SAFE_CMD  Get \"\\$\\project\\lib\\ijl15.lib" -I- \"-VL$VersionLabel\"`;
		print `$SAFE_CMD  Get \"\\$\\project\\lib\\luca.lib" -I- \"-VL$VersionLabel\"`;
		print `$SAFE_CMD  Get \"\\$\\project\\lib\\c4lib.lib" -I- \"-VL$VersionLabel\"`;
		print `$SAFE_CMD  Get \"\\$\\dv\\acdc\\NeroApiGlue.lib" -I- \"-VL$VersionLabel\"`;
		print `$SAFE_CMD  Get \"\\$\\project\\lib\\i7000.lib" -I- \"-VL$VersionLabel\"`;
		print `$SAFE_CMD  Get \"\\$\\project\\lib\\uart.lib" -I- \"-VL$VersionLabel\"`;
		#
		# get latest version from source safe
		#
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

			# remove before the SourceSafe complains about a writable copy
			# it´s recompiled anyway, thus it has no write-protection
			if (/MiCoDA/)
			{
				&DeleteEvenIfRO("Release/MiCoDA.lib",0);
				&DeleteEvenIfRO("Release/MiCoDA.dll",0);
			}
			if (/JaCobDA/)
			{
				&DeleteEvenIfRO("Release/JaCobDA.lib",0);
				&DeleteEvenIfRO("Release/JaCobDA.dll",0);
			}
			if (/JaCobDll/)
			{
				&DeleteEvenIfRO("Release/JaCob.lib",0);
				&DeleteEvenIfRO("Release/JaCob.dll",0);
			}
			if (/SaVicDA/)
			{
				&DeleteEvenIfRO("Release/SaVicDA.lib",0);
				&DeleteEvenIfRO("Release/SaVicDA.dll",0);
			}
			if (/SaVicDll/)
			{
				&DeleteEvenIfRO("Release/SaVic.lib",0);
				&DeleteEvenIfRO("Release/SaVic.dll",0);
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
		}
	}
	else
	{
		print ">>> Not getting DepProjects! <<<\n";
	}

	# loop over all LibProjects
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
			&DeleteEvenIfRO("Release/CIPC.lib",0);
			&DeleteEvenIfRO("Release/CIPC.dll",0);
		}
		elsif (/MegraV2DLL/)
		{
			&DeleteEvenIfRO("Release/MegraV2.lib",0);
			&DeleteEvenIfRO("Release/MegraV2.dll",0);
		}
		elsif (/Convert/)
		{
			&DeleteEvenIfRO("Release/Convrt32.lib",0);
			&DeleteEvenIfRO("Release/Convrt32.dll",0);
		}
		elsif (/MiCoDA/)
		{
			&DeleteEvenIfRO("Release/MiCoDA.lib",0);
			&DeleteEvenIfRO("Release/MiCoDA.dll",0);
		}
		elsif (/JaCobDA/)
		{
			&DeleteEvenIfRO("Release/JaCobDA.lib",0);
			&DeleteEvenIfRO("Release/JaCobDA.dll",0);
		}
		elsif (/JaCobDll/)
		{
			&DeleteEvenIfRO("Release/JaCob.lib",0);
			&DeleteEvenIfRO("Release/JaCob.dll",0);
		}
		elsif (/SaVicDA/)
		{
			&DeleteEvenIfRO("Release/SaVicDA.lib",0);
			&DeleteEvenIfRO("Release/SaVicDA.dll",0);
		}
		elsif (/SaVicDll/)
		{
			&DeleteEvenIfRO("Release/SaVic.lib",0);
			&DeleteEvenIfRO("Release/SaVic.dll",0);
		}
		else
		{
			&DeleteEvenIfRO("$theProjectDir/$project.lib",0);
			&DeleteEvenIfRO("$theProjectDir/$project.dll",0);
#			print "else DeleteEvenIfRO $project\n";
#			die;
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
			&DeleteEvenIfRO("Release/CIPC.lib",1);
			&DeleteEvenIfRO("Release/CIPC.dll",1);
		}
		elsif (/MegraV2DLL/)
		{
			# no message, the libs are not in the project
			&DeleteEvenIfRO("Release/MegraV2.lib",0);
			&DeleteEvenIfRO("Release/MegraV2.dll",0);
		}
		elsif (/Convert/)
		{
			&DeleteEvenIfRO("Release/Convrt32.lib",1);
			&DeleteEvenIfRO("Release/Convrt32.dll",0);
		}
		elsif (/MiCoDA/)
		{
			&DeleteEvenIfRO("Release/MiCoDA.lib",1);
			&DeleteEvenIfRO("Release/MiCoDA.dll",1);
		}
		elsif (/JaCobDA/)
		{
			&DeleteEvenIfRO("Release/JaCobDA.lib",1);
			&DeleteEvenIfRO("Release/JaCobDA.dll",1);
		}
		elsif (/JaCobDll/)
		{
			&DeleteEvenIfRO("Release/JaCob.lib",1);
			&DeleteEvenIfRO("Release/JaCob.dll",1);
		}
		elsif (/SaVicDA/)
		{
			&DeleteEvenIfRO("Release/SaVicDA.lib",1);
			&DeleteEvenIfRO("Release/SaVicDA.dll",1);
		}
		elsif (/SaVicDll/)
		{
			&DeleteEvenIfRO("Release/SaVic.lib",1);
			&DeleteEvenIfRO("Release/SaVic.dll",1);
		}
		
		&DoConfigurations($project);
	}	# end of loop over all LibProjects

	#
	# the main loop over all Projects
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
	$MakeDoneText = "idip Make DONE at $mday.$month.$year, $hour:$min:$sec, Version $versionInfo$currentBuild";
	print "\n$MakeDoneText\n";

	$MakeErrorText = "OK";
	if ($errorCount || $warningCount || $UnknownCount || ($#g_CheckTranslations>=0))
	{
		$MakeErrorText = "$errorCount error[s], $warningCount warning[s], $UnknownCount Unknown projects";
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
	$MakeEndText = "idip Make END at $mday.$month.$year, $hour:$min:$sec, Version $versionInfo$currentBuild";
	print "\n$MakeEndText\n";
}
elsif ($bWorkAsSuperCopy==1)
{
	&SetTimeVariables();
	print "idipCopy started at $mday.$month.$year, $hour:$min:$sec ; Versionlabel '$VersionLabel' ; copying for '$versionInfo$currentBuild'\n";

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
	$MakeDoneText = "idipCopy DONE at $mday.$month.$year, $hour:$min:$sec, Version $versionInfo$currentBuild";
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
	$MakeEndText = "idipCopy END at $mday.$month.$year, $hour:$min:$sec, Version $versionInfo$currentBuild";
	print "\n$MakeEndText\n";
}
elsif ($bWorkAsSuperHistory==1)
{
	print "work as SuperHistory\n";
	&CollectBuildNotes();
    &CopyBuildNotes();
	print "SuperHistory DONE\n";
}
#
# end of 'main'
#
#######################################################################
sub UpdateBuild
{
	# update the BuildNumber
	print ("Updating $theDestinationDir/vinfo.pvi\n");
	`$theDestinationDir\\ProductVersion -updateDirectory $theDestinationDir`;

	chdir($theCurrentProjectDrive);	

	&CollectBuildNotes();
    &CopyBuildNotes();

	# NEW 16.10.98 no date/time in label
	$LabelText = "$versionInfo$currentBuild";
    # set a version label in the SourceSafe
    print "Setting SourceSafeLabel: $LabelText\n";
	`$SAFE_CMD  Label \$/Project -C- -L$LabelText`;
# gf new 02.01.2003 label for acdc to get build notes
	`$SAFE_CMD  Label \$/DV -C- -L$LabelText`;
	# safe label text in file.
	open (LABEL_OUT_FILE,">$theDestinationDir\\label.txt");
	print(LABEL_OUT_FILE "$LabelText\n");
	close(LABEL_OUT_FILE);

	# safe label text in file.
	open (VERSION_OUT_FILE,">$theDestinationDir\\version.txt");
	print(VERSION_OUT_FILE "$versionInfo$currentBuild\n");
	close(VERSION_OUT_FILE);

	chdir($theDestinationDir);	
	print "Creating g:\\BuildZips\\Build$currentBuild.zip\n";
	print "executing $ZIP_CMD -add -recurse -directories g:\\BuildZips\\$currentBuild.zip $theDestinationDir\\*.*\n";
	`$ZIP_CMD -add -recurse -directories g:\\BuildZips\\$currentBuild.zip $theDestinationDir\\*.*`;

	chdir($theDestinationDirEnu);	
	print "Creating g:\\BuildZips\\Enu$currentBuild.zip\n";
	print "executing $ZIP_CMD -add -recurse -directories g:\\BuildZips\\Enu$currentBuild.zip $theDestinationDirEnu\\*.*\n";
	`$ZIP_CMD -add -recurse -directories g:\\BuildZips\\Enu$currentBuild.zip $theDestinationDirEnu\\*.*`;

#	chdir($theDestinationDirIta);	
#	print "Creating g:\\BuildZips\\Ita$currentBuild.zip\n";
#	print "executing $ZIP_CMD -add -recurse -directories g:\\BuildZips\\Ita$currentBuild.zip $theDestinationDirIta\\*.*\n";
#	`$ZIP_CMD -add -recurse -directories g:\\BuildZips\\Ita$currentBuild.zip $theDestinationDirIta\\*.*`;
}
#######################################################################
sub CopyBuildNotes
{
	$source = "$theDestinationDir\\BuildNotes$currentBuild.htm";
	$dest   = "\\\\mail01\\buildnotes\\idip\\BuildNotes$currentBuild.htm";
	print `copy $source $dest`;
	print ("CopyBuildNotes done.\n");
}
#######################################################################
sub DoOneConfiguration
{
	local($mak,$dest,$cfg)=@_;
	local($oneLine)="";
	local($ei)=0;

	# print " .... $mak CFG=\"$dest - Win32 $cfg\"\n";

	# GF 22.10.2002
	# Static analysis moved sub routine DoStaticAnalysis()
	# called from DoConfigurations only once per project

	# /Q check only
	# /S suppress executed command
	# always kill the destination
	local($destDir);
	$destDir="Release";
	if ($cfg eq "PTUnitRelease") {
		$destDir="PTUnitRelease";
	}
	if ($cfg eq "PTEnu") {
		$destDir="PTEnu";
	}
	
	# new UF with absolute path
	# and for dll's with lib path
	# do not remove to avoid timestamp differences &DeleteEvenIfRO("$theProjectDir/lib/$dest.dll",0);
	# do not remove to avoid timestamp differences &DeleteEvenIfRO("$theProjectDir/$project/$destDir/$dest.exe",0);
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
	elsif ($bRebuildProj)
	{
		print "compiling $dest - Win32 $cfg, rebuild project\n";
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
				# || /LNK4089/ || /LNK4033/ 
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
			if (/ error /) {
				$errorCount++;
				if ($bStopOnError)
				{
					my($sMsg) = "Tool cancelled at error";
					&NetSendError($sMsg);
					die "$sMsg";
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
#######################################################################
sub DoConfigurations
{ 
	local($project)=@_;
	
	local(@cfgs);
	local($mak)="foo.mak";
	local($dest)="foo";

	# print("DO \"$project\"\n");
		
	# CAVEAT2 jedes Project muss hier eingetragen sein!
	# special settings per project
	if ($project eq "SecurityServer")
	{
		# makefile has diffenrent name than project
		$mak  = "sec3.mak";
		$dest = "sec3";
	}
	else
	{
		# set default values
		$mak  = "$project\.mak";
		$dest = "$project";

		#
		# CAVEAT order is important
		#
		$mak  =~  s/^Units\/MiCo\///;	# drop leading Units/MiCo/MiCoUnitPCI
		$dest =~ s/^Units\/MiCo\///;	# drop leading Units/MiCo/MiCoUnitPCI

		$mak  =~  s/^Units\/JaCob\///;	# drop leading Units/JaCob/JaCobUnit
		$dest =~ s/^Units\/JaCob\///;	# drop leading Units/JaCob/JaCobUnit

		$mak  =~  s/^Units\/SaVic2\///;	# drop leading Units/SaVic2/SaVicUnit
		$dest =~ s/^Units\/SaVic2\///;	# drop leading Units/SaVic2/SaVicUnit

		$mak  =~  s/^Units\/SaVic\///;	# drop leading Units/SaVic/SaVicUnit
		$dest =~ s/^Units\/SaVic\///;	# drop leading Units/SaVic/SaVicUnit

		$mak  =~  s/^Units\/Aku\///;	# drop leading Units/Aku
		$dest =~ s/^Units\/Aku\///;	# drop leading Units/Aku

		$mak  =~  s/^Units\/Audio\///;	# drop leading Units/Audio
		$dest =~ s/^Units\/Audio\///;	# drop leading Units/Audio

		$mak  =~  s/^MegraV2\///;	# drop leading 
		$dest =~ s/^MegraV2\///;	# drop leading 

		# convert MegraV2DLL.mak to MegraV2.mak
		$mak =~  s/DLL\.mak/.mak/;	# 
		$mak =~  s/Convert\.mak/Convrt32.mak/;	# 
		# ditto for the destination MegraV2DLL to MegraV2
		$dest =~  s/V2DLL/V2/;	# 
		$dest =~  s/Convert/Convrt32/;	# 

		$mak  =~ s/^Units\///;	# drop leading Units/
		$dest =~ s/^Units\///;	# drop leading Units/

		$mak  =~ s/^DV\///;	# drop leading DV/
		$dest =~ s/^DV\///;	# drop leading DV/

		$mak  =~ s/^Tools\///;	# drop leading Tools/
		$dest =~ s/^Tools\///;	# drop leading Tools/

		$mak  =~ s/^Capi\///;	# drop leading Capi/
		$dest =~ s/^Capi\///;	# drop leading Capi/

		$mak  =~ s/^Lib\///;		# drop leading Lib/
		$dest =~ s/^Lib\///;	# drop leading Lib/

		$mak  =~ s/^Clients\///;		# drop leading 
		$dest =~ s/^Clients\///;	# drop leading

		$mak  =~ s/^HHLA\///;	# drop leading 
		$dest =~ s/^HHLA\///;	# drop leading

		$mak  =~ s/^Audio\///;	# drop leading 
		$dest =~ s/^Audio\///;	# drop leading

		$mak  =~ s/^DirectShow\///;	# drop leading 
		$dest =~ s/^DirectShow\///;	# drop leading

		$mak  =~ s/^Filters\///;	# drop leading 
		$dest =~ s/^Filters\///;	# drop leading

		$mak  =~ s/^Yuta\///;	# drop leading 
		$dest =~ s/^Yuta\///;	# drop leading

		$mak  =~ s/^USBCAM\///;	# drop leading 
		$dest =~ s/^USBCAM\///;	# drop leading

		# NEW PROJECT add here, if on 'new' subdirectory
	}

	local($bValidProject)=0;
	local ($bNoCompile)=0;

	# 
	if ($project eq "Capi/ISDNUnit")
	{
		# special for ISDNUnit und PTUnit in einem Project
		@cfgs=("Release", "PTUnitRelease","Enu","PTEnu");
		$bValidProject=1;
	}
	elsif (    $project eq "Units/MiCo/MiCoSys"
			|| $project eq "Units/MiCo/MiCoVxd"
			|| $project eq "Units/JaCob/JaCobVxd"
			|| $project eq "Units/JaCob/JaCobSys"
			|| $project eq "Units/SaVic2/SaVicSys"
			|| $project eq "Units/Aku/AkuSys"
			)
	{
		$bNoCompile=1;
		$bValidProject=1;
		# still needs one cfg to get the copy done
		@cfgs=("Release");
	}
	elsif (    $project eq "Cipc"
			|| $project eq "Clients/ICPCONClient"
			|| $project eq "HHLA/MiniClient"
			|| $project eq "HHLA/RemoteControl"
			|| $project eq "Lib/C4Lib"
			|| $project eq "Lib/Crypto"
			|| $project eq "Lib/DeviceDetect"
			|| $project eq "Lib/h263enc"
			|| $project eq "Lib/VImage"
			|| $project eq "Lib/ICPCONDll"
			|| $project eq "Lib/jpeg"
			|| $project eq "Lib/JpegLib"
			|| $project eq "Lib/PTDecoder"
			|| $project eq "Lib/Skins"
			|| $project eq "Lib/WKClasses"
			|| $project eq "Lib/ReleaseAlloc"
			|| $project eq "MegraV2/Convert"
			|| $project eq "MegraV2/MegraV2DLL"
			|| $project eq "Tools/CDStart"
			|| $project eq "Tools/convntfs"
			|| $project eq "Tools/DvrtRun"
			|| $project eq "Tools/LogZip"
			|| $project eq "Tools/RegMove"
			|| $project eq "Tools/RegPriv"
			|| $project eq "Tools/RSA"
			|| $project eq "Tools/Win95Reboot"
			|| $project eq "Tools/WkTranslator"
			|| $project eq "Units/JaCob/JaCobDA"
			|| $project eq "Units/JaCob/JaCobTest"
			|| $project eq "Units/MiCo/MiCoDA"
			|| $project eq "Units/SaVic2/SaVicDA"
			|| $project eq "Tools/UpdateHandler"
			|| $project eq "DirectShow/Common"
			|| $project eq "DirectShow/Filters/baseclasses"			# a lib
			|| $project eq "DirectShow/Filters/dump"				# ax
			|| $project eq "DirectShow/Filters/inftee"				# ax
			|| $project eq "DirectShow/Filters/mediasource"			# ax
			|| $project eq "DirectShow/Filters/PushSource"			# ax
			|| $project eq "DirectShow/Filters/Sample2MemRenderer"	# ax
			|| $project eq "DirectShow/Filters/WaveSplitter"		# ax
			)
	{
		# nur release
		@cfgs=("Release");
		$bValidProject=1;
	}
	elsif (  $project eq "Lib/h263dec")
	{
		@cfgs=("Release","H26x");
		$bValidProject=1;
	}
	elsif (
			   $project eq "DV/acdc"
			 
			|| $project eq "SocketUnit"

			|| $project eq "Tools/ProductView"
			|| $project eq "Tools/LogView"
			|| $project eq "Tools/ProductVersion"
			|| $project eq "Tools/SecAnalyzer"

			|| $project eq "Units/Aku/AkuUnit"
			|| $project eq "Units/Audio/AudioUnit"
			|| $project eq "Units/CoCoUnit"
			|| $project eq "Units/CommUnit"
			|| $project eq "Units/ICPCONUnit"
			|| $project eq "Units/MiCo/MiCoUnitPCI"
			|| $project eq "Units/SimUnit"
			|| $project eq "Units/SMSUnit"
			|| $project eq "Units/GemosUnit"
			|| $project eq "Units/SDIUnit"
			|| $project eq "Units/EmailUnit"
			|| $project eq "Units/TOBSUnit"
			|| $project eq "Units/USBCAM/USBCamUnit"
			|| $project eq "Units/Yuta/USBAlarmUnit"
			)
	{
		# release und enu
		@cfgs=("Release","Enu");
		$bValidProject=1;
	}
	elsif (
			   $project eq "SecurityServer"
			
			|| $project eq "Lib/oemgui"
 			|| $project eq "SecurityLauncher"
			|| $project eq "SystemVerwaltung"

			|| $project eq "Dbs"

			|| $project eq "Units/JaCob/JaCobUnit"
			|| $project eq "Units/JaCob/JaCobDll"
			 
			|| $project eq "Units/SaVic2/SaVicUnit"
			|| $project eq "Units/SaVic2/SaVicDll"

			|| $project eq "Tools/DelRegDb"
			|| $project eq "Tools/EULA"
			
			|| $project eq "Clients/Recherche"
			|| $project eq "Clients/Vision"
			)
	{
		# release und enu
#		@cfgs=("Release","Enu","Ita");
		@cfgs=("Release","Enu");
		$bValidProject=1;
	}
	else
	{
		$bValidProject=0;
		print "Unknown project: \"$project\"\n";
		$UnknownCount++;
	}

	# a valid project ? Then loop over the given configurations.
	if ($bValidProject) 
	{
		if ($bInFinalCopy==0) 
		{ # it´s not copy; do the nmake
			if ($bNoCompile==0) 
			{	# some projects do not need to be compiled (DDK stuff etc.)

				# Do some static code analysis (break, lock a.o.)
				# some projects are old or foreign stuff with an "individual" source code style
				# do not check anymore, because they are assumed valid and only pop useless warnings
				if (   $project eq "Lib/C4Lib"
					|| $project eq "Tools/LogZipLib"
					)
				{
					# skip these
				}
				else
				{
					&DoStaticAnalysis();
					&CheckMultipleRC($dest, @cfgs);
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
		print ("Invalid project: \"$project\"\n");
	}

	if ($bWorkAsSuperCopy==0)
	{
		# NOT YET check for AutoDuck directory ?
		if (grep (/^$project$/,@DocProjects)) # is it a AutoDuck project
		{
			@makeDocsLog = `v:\\Dokumentation\\AutoDuck\\docs.bat $dest`;
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
#######################################################################
sub TrySourceSafe
{
#	$proj=getcwd;
#	$proj =~ s/[A-Za-z]:/\$/;

	local($proj)=@_;

	if (-d "res")
	{
		# delete modified rc2
	}

	# check if dsp file has changed
	$bRebuildProj = 0;
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

	$Label = $VersionLabel;
	$_ = $proj;

#	if (/Recherche/)
#	{
#		$Label = "4.4.0.510";
#	}
	print "getting $proj Version $Label\n";

	local($OutputFile) = "vsstmp.txt"; 
	if ($Label ne "") {
		`$SAFE_CMD  Get \"$proj\" -R -I-N -O\@$OutputFile \"-VL$Label\"`;
	} else {
		`$SAFE_CMD  Get \"$proj\" -R -I-N -O\@$OutputFile`;
	}

	# get new file time	
	local($time_secs_new) = (stat($dspFile))[9]; 
#	print ("$time_secs_new $time_secs_new\n");

	# if file time changed -> file changed -> rebuild all
	if ($time_secs_new != $time_secs_old)
	{
		$bRebuildProj = 1;
	}
#	print ("Rebuild $bRebuildProj\n");

# check if any error at SourceSafe operation
	open(HFILE, $OutputFile);
	while (<HFILE>)
	{
		if (/is not an existing filename or project/)
		{
			print ("ERROR: $_\n");
			$errorCount++;
		}
		if ($bTraceSourceSafe)
		{
			# NOT YET print ss
		}
	}
	close(HFILE);
	unlink($OutputFile);
}
#######################################################################
sub CheckMultipleRC
{
	local($proj, @cfgs) = @_;
	if ($#cfgs > 0)
	{
		# check for multiple RC files
		@RCFiles= `dir /b /on *.rc`;
		if ($#RCFiles > 0)
		{
#			print ("TEST: Project $proj\n");
			local($deuRCFile) = $proj;
			$deuRCFile .= "\.rc";
#			print ("TEST: deuRCFile $deuRCFile\n");
			# save german rc file time
			local($timeSecsDeuRC) = (stat($deuRCFile))[9]; 
#			print ("TEST timeSecsDeuRC $timeSecsDeuRC\n");
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
#							print ("TEST $oneRCFile timeSecsOneRC $timeSecsOneRC\n");
							# if file time OneRC < file time DeuRC -> check translation!
							if ($timeSecsOneRC < $timeSecsDeuRC)
							{
								$warningCount++;
								print ("Warning: Check translation $oneRCFile\n");
								push(@g_CheckTranslations, $oneRCFile);
							}
							else
							{
#								print ("TEST translation OK $oneRCFile\n");
							}
						}
						else
						{
#							print ("TEST $cfg not found in $oneRCFile\n");
						}
					}
				}
				else
				{
#					print ("TEST OOPS NON-RC file? $oneRCFile\n");
				}
			}
		}
		else
		{ # only one rc file
#			print ("TEST: Project $proj has only one RC\n");
		}
	}
	else
	{ # only one configuration
#		print ("TEST: Project $proj has only one cfg $cfgs[0]\n");
	}
}
#######################################################################
sub CreateNewDir
{
	local($TraceDebug) = 0;
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
				if ($TraceDebug) {print "Already existing $_\n";}
			}
			else
			{
				if ($TraceDebug) {printf ("Create $_ in %s\n",getcwd);}
				system("mkdir $_");
				if ($TraceDebug) {print ("Created $_\n");}
			}
		}
		if (-d $_ || /:/ )
		{
			if ($TraceDebug) {print "Change drive to $_\n";}
			chdir ("$_\\");	# absolute path, back to root dir
		}
		elsif (/:/)
		{
			if ($TraceDebug) {print "Change to root $_\\\n";}
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
	local($project,$dest,$config)=@_;

	if ($dest eq "ISDNUnit" && $config eq "PTUnitRelease") {
		$dest="PTUnit";
	} elsif ($dest eq "ISDNUnit" && $config eq "PTEnu") {
		$dest="PTUnit";
	}


	# make the destination dependend from the configuration
	local($destinationDir)=$theDestinationDir;
	if ($config eq "Enu" || $config eq "PTEnu")
	{
	  $destinationDir = $theDestinationDirEnu;
	}
	elsif ($config eq "Ita" || $config eq "PTIta")
	{
	  $destinationDir = $theDestinationDirIta;
	}
	else
	{
	  $destinationDir = $theDestinationDir;
	}

	# create non existing dir
	if (! -d "$destinationDir") {
		`mkdir $destinationDir`;
	}

	local ($bDoCopy)=1;

	$_ = $project;

	print "COPYING $_\n";

	# CAVEAT3 jedes Project das keine EXE ergibt oder sonstwie speziell ist
	# muss hier entsprechend eingetragen sein!

	if (
		   /Crypto/
		|| /DeviceDetect/
		|| /h263dec/
		|| /h263enc/
		|| /VImage/
		|| /ICPCONDll/
		|| /JpegLib/
		|| /oemgui/
		|| /PTDecoder/
		) {
		if (   $config eq "Enu"
			|| $config eq "Ita"
			)
		{
			$source = "$theProjectDir\\$project\\$config\\$dest.dll";
			$finalDest = "$destinationDir\\Dlls\\$dest.dll";
		} elsif ($config eq "H26x") {
			$source = "$project\\H26x\\$dest.dll";
			$finalDest = "$destinationDir\\Dlls\\h263decx.dll";
		} else {
			$source = "$theProjectDir\\Lib\\$dest.dll";
			$finalDest = "$destinationDir\\Dlls\\$dest.dll";
		}
	} elsif (/MiCoDA/ || /JaCobDA/ || /SaVicDA/ || /MegraV2/ ) {
		$source = "$project\\Release\\$dest.dll";
		$finalDest = "$destinationDir\\Dlls\\$dest.dll";
	} elsif (/MiCoVxd/) {
		$source = "$project\\bin\\micopci.vxd";
		$finalDest = "$destinationDir\\micopci.vxd";
	} elsif (/MiCoSys/) {
		$source = "$project\\bin\\MiCoPCI.sys";
		$finalDest = "$destinationDir\\MiCoPCI.sys";
	} elsif (/JaCobVxd/) {
		$source = "$project\\bin\\jacob.vxd";
		$finalDest = "$destinationDir\\JaCob.vxd";
	} elsif (/JaCobSys/) {
		$source = "$project\\bin\\JaCob.sys";
		$finalDest = "$destinationDir\\JaCob.sys";
		&MyDoCopy("$theProjectDir\\Units\\JaCob\\JaCobUnit\\JaCob.inf","$destinationDir\\JaCob.inf",1);	# do count errors
		# Schreibschutz entfernen !
		chmod 0755, "$destinationDir\\JaCob.inf";
	} elsif (/SaVicSys/) {
		$source = "$project\\bin\\SaVic.sys";
		$finalDest = "$destinationDir\\SaVic.sys";
		&MyDoCopy("$theProjectDir\\Units\\SaVic2\\SaVicSys\\Bin\\SaVic.inf","$destinationDir\\SaVic.inf",1);	# do count errors
		# Schreibschutz entfernen !
		chmod 0755, "$destinationDir\\SaVic.inf";
	} elsif (/JaCobDll/) {
		$source = "$project\\$config\\JaCob.dll";
		$finalDest = "$destinationDir\\Dlls\\JaCob.dll";
	} elsif (/SaVicDll/) {
		$source = "$project\\$config\\SaVic.dll";
		$finalDest = "$destinationDir\\Dlls\\SaVic.dll";
	} elsif (/AkuSys/) {
		$source = "$project\\bin\\Aku.sys";
		$finalDest = "$destinationDir\\Aku.sys";
	} elsif (/Convert/) {
		$source = "$project\\Release\\Convrt32.dll";
		$finalDest = "$destinationDir\\Dlls\\Convrt32.dll";
	} elsif (/Clients\/Vision/) {
		$source = "$project\\$config\\Vision.exe";
		$finalDest = "$destinationDir\\Vision.exe";
	} elsif (/Skins/) {
		$source = "$theProjectDir\\Lib\\$dest.dll";
		$finalDest = "$destinationDir\\Dlls\\$dest.dll";
	} elsif (/Cipc/) {
		$source = "$project\\release\\cipc.dll";
		$finalDest = "$destinationDir\\Dlls\\cipc.dll";
	} elsif (/WKClasses/ || /jpeg/ || /C4Lib/ || /baseclasses/ || /ReleaseAlloc/) {
		$bDoCopy=0;
	} elsif (/UpdateHandler/ ) {
		$source = "$project\\$config\\$dest.exe";
		$finalDest = "$destinationDir\\$dest.exe";
	} elsif (/Common/ ) {
		$source = "$theProjectDir\\DirectShow\\Lib\\CommonDirectShow.dll";
		$finalDest = "$destinationDir\\Dlls\\CommonDirectShow.dll";
	} elsif (/DirectShow/ ) {
		$source = "$project\\$config\\$dest.ax";
		$finalDest = "$destinationDir\\Dlls\\$dest.ax";
	} elsif (/USBAlarmUnit/ ) {
		# der absolute special fall yuta.inf yuta.sys und yutaunit.exe
		# der Treiber Yuta.sys
		&MyDoCopy("$project\\Yuta.sys","$destinationDir\\Yuta.sys",1);
		# die Installationsdatei Yuta.inf
		&MyDoCopy("$project\\Yuta.inf","$destinationDir\\Yuta.inf",1);
		# die Yuta Firmware
		&MyDoCopy("$project\\YutaFirmware.hex","$destinationDir\\YutaFirmware.hex",1);

		$source = "$project\\$config\\YutaUnit.exe";
		$finalDest = "$destinationDir\\YutaUnit.exe";
	} else {
		#
		# .exe 
		#
		$source = "$project\\$config\\$dest.exe";
		$finalDest = "$destinationDir\\$dest.exe";
	}
	
	if ($bDoCopy) {

		# .exe or .dll first
		&MyDoCopy($source,$finalDest,1);	# do count errors

		# followed by.map
		#

		# destination is Maps, but not CoCo/Maps
		$destinationDir .= "\\Maps";
		# remmove coco dir
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
		elsif (
				   /oemgui/
				|| /PTDecoder/
				|| /JpegLib/
				|| /DeviceDetect/
				|| /h263enc/
				|| /VImage/
				|| /h263dec/
				|| /ICPCONDll/
			)
		{
		# .map are not in Lib/ but in their Release/ directories
			$source = "$project\\$config\\$dest.map";
		} elsif ( /MiCoSys/ || /MiCoVxd/ || /JaCobSys/ || /JaCobVxD/ || /AkuSys/ || /SaVicSys/) {
			$source = "NO COPY"; # no map file
		}

#ML		$finalDest = "$destinationDir\\maps\\$dest.map";
		$finalDest = "$destinationDir\\$dest.map";

		$_ = $source;

		if (/\.map/) {
			if (!(-f $source)) {
				$source =~ s/\.map/.pdb/;
				$finalDest =~ s/\.map/.pdb/;
				print "using pdb instead of map $source \n";
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
sub CollectBuildNotes
{
	$filename = "$theDestinationDir\\BuildNotes$currentBuild.htm";
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
	open (LABEL_IN_FILE,"<$theDestinationDir\\label.txt");
	while (<LABEL_IN_FILE>)
	{
		$prevLabelText = "$prevLabelText$_";
	}
	close(LABEL_IN_FILE);
	$prevLabelText =~  s/\n//;
	$prevLabelText =~  s/\r//;

	for (@allProjects) 
	{
		local($project);
		$project=$_;
		chdir("/Project/$project");
		if ($bTraceSuperHistory) 
		{
			printf ("Searching history in %s...\n",getcwd());
		}

		if (-f "history.txt") 
		{
			local($projectSS) = "\$/Project/$project";
			if (/DV/) {
				$projectSS = "\$/$project";
			}
			
			# -IECW ignore case/end of line/white space changes
#			print "Diff $projectSS/history.txt -DS200 -IECW -VL$prevLabelText > SuperHistory.txt\n";
			print `$SAFE_CMD Diff $projectSS/history.txt -DS200 -IECW -VL$prevLabelText > SuperHistory.txt`;

			# now superHistory.txt is created
			# collect between the topmost Label:"Build
			# from this take only Chg: To : Ins:
			local($bAlreadyDidPrint)=0;
			local($bAlreadyDidPrintText)=0;
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
				print (OFILE "<BR><BR>\n");
			}
			close(HFILE);
			unlink("SuperHistory.txt");
		} else {
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
		if ($bDoCountError) {
			$theCopyErrorCount++;
			print "File not found, $source\n";
			print ("ERROR DURING COPY \n");
			&NetSendError("ERROR DURING COPY $source not found");
			die;
		} else {
			print "File not found, $source, but not counted\n";
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
#######################################################################
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
#######################################################################
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
	$TmpLib .= "$theProjectDir\\Cipc\\Release;$theProjectDir\\DV\\Lib;";
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
#	if (-d "c:\\Msdev98") {
#		$ENV{'PATH'} = "$MSDEV_DIR\\bin;C:\\Winnt;C:\\Winnt\\system32;c:\\MsDev98\\Common\\Msdev98\\bin;c:\\Msdev98\\Common\\Tools;$theDestinationDir\\Dlls";
#	} elsif (-d "D:\\Msdev98") {
#		$ENV{'PATH'} = "$MSDEV_DIR\\bin;c:\\msdev\\sharedide\\bin;C:\\Windows;D:\\DevStudio\\Tools;D:\\MsDev98\\Common\\Msdev98\\bin;D:\\Msdev98\\Common\\Tools;$theDestinationDir\\Dlls";
#	} else {
#		$ENV{'PATH'} = "$MSDEV_DIR\\bin;c:\\msdev\\sharedide\\bin;C:\\Windows;D:\\DevStudio\\Tools;$theDestinationDir\\Dlls";
#	}
	
	print "path is $ENV{'PATH'}\n";
}
#######################################################################
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
#######################################################################
__END__
:endofperl
