@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

#
# PERL PART FOLLOWS HERE
#

use Cwd;

######################################################################################################
# start main

# force "flush output"
$|=1;

select(STDERR); $|=1;
select(STDOUT); $|=1;

#**********************************************************
# Trace options
#**********************************************************
$bDebuggingGlobal = 0;

#**********************************************************
# Globals
#**********************************************************
$sec,$min,$hour,$mday,$mon,$year;

$bWorkAsSuperMake = 1;		
$bWorkAsSuperCopy = 0;
$bWorkAsSuperHistory = 0;

$bInFinalCopy = 0;
$theCopyErrorCount = 0;		# modified in MyDoCopy
$errorCount = 0;
$warningCount = 0;
$UnknownCount = 0;
$bRebuildAll = 0;
$bRebuildProj=0;		# rebuild automatic if dsp changed, see TrySourceSafe()

$VersionLabel = "";			# leer fuer aktuelle Version
							# oder mit VersionsLabel passend zum SourceSafe $/Project label
							# $VersionLabel = "4.3.0.487";

$VersionMajor = 5;
$VersionMinor = 0;
$VersionSubIndex = 0;
$versionInfo = "$VersionMajor.$VersionMinor.$VersionSubIndex.";

$buildNumber = 0;		# if 0 read from vinfo.pvi, if >0 used as predefined build number
$currentBuild = 0;		# updated in ReadBuildNumber ($buildNumber +1)

# hardcoded D:/Project
$ProjectDrive	= "D:";
$theProjectDir	= "$ProjectDrive/Project";

$theDestinationDrive	= "v:";
$theDestinationDir		= "$theDestinationDrive\\bin\\idip";		# has to contain vinfo.pvi for the buildNumber info

$totalLines=0;
$statText = "Statistic:\n";

$DoneText = "DONE";
$EndText = "END";

$OutputFileName = "idipMakeOutput.txt";

#**********************************************************
# open output file
open (OUTPUT_FILE, ">$ProjectDrive\\$OutputFileName");

#**********************************************************
&SetTimeVariables();
&ReadBuildNumber();

&PrintOutput("idip Make started at $mday.$month.$year, $hour:$min:$sec, version was '$VersionLabel', compiling for build $currentBuild\n");

#**********************************************************
# handle command line parameters
for (@ARGV)
{
	if (/-rebuild/i)
	{
		$bRebuildAll = 1;
		&PrintOutput("Rebuild all\n");
	}
	elsif (/-superCopy/i)
	{
		$bWorkAsSuperMake = 0;
		$bWorkAsSuperCopy = 1;
	}
	elsif (/-superHistory/i)
	{
		$bWorkAsSuperMake	 = 0;
		$bWorkAsSuperHistory = 1;
	}
	else
	{
		die "Unknown command line parameter: $_\n";
	}
}

#**********************************************************
# Dependence projects are not compiled
$bGetLibs=1;								# disable for faster testing
@DepProjects=(
				"Cipc",						# wk.h is already used in WKClasses
				"include",
				"Lib/C4Lib",				# CodeBase 6.4
				"Lib/h263",					# Header werden von TOBSUnit eingebunden, Prüfung durch UF erfolgt
				"Lib/luca",
				"Lib/netcomm",				# OOPS only for winsocket.h
				"Lib/vdbs",
				"Units/MiCo/MiCoUnitPCI",	# OOPS only for micodefs.h
				"Units/JaCob/JaCobDA",		# OOPS
				"Units/JaCob/JaCobDll",		# OOPS only for jacobdefs.h
				"Units/SaVic/SaVicDA",		# OOPS only for SavicDirectAccess.h
				);

#**********************************************************
# Liste aller relevanten Projekte
# CAVEAT muss auch weiter unten angepasst werden
# siehe CAVEAT2
# NEW PROJECT das WorkingDirectory muss auf Senso fuer D:\Project\NewProject gesetzt werden
# NEW PROJECT add here
@Projects=( 
	# LibProjects are compiled first, that is before the normal Projects
	# Order is important for dependencies
			"Lib/WKClasses",			# is already used in CIPC
			"Tools/LogZipLib",			# is already used in CIPC
			"Cipc",
			"Capi/Capi4",
			"Lib/Crypto",
			"Lib/DeviceDetect",
			"Lib/h263dec",
			"Lib/h263enc",
			"Lib/jpeg",
			"Lib/JpegLib",
			"Lib/oemgui",
			"Lib/PTDecoder",
			"Lib/Skins",
			"Lib/vdbs",
#		); #@Projects2=(
	# normal projects start here

			"Capi/ISDNUnit",		# also contains PTUnit			
			"Clients/Recherche",
			"Clients/Vision",
			"Dbs",
			"DV/acdc",
			"HHLA/MiniClient",
			"HHLA/RemoteControl",
			"MegraV2/Convert",
			"MegraV2/MegraV2DLL",
			"SecurityLauncher",
			"SecurityServer", 
			"SocketUnit",
			"SystemVerwaltung",
			"Tools/CDStart",
			"Tools/convntfs",
			"Tools/DelRegDb",
			"Tools/DvrtRun",
			"Tools/EULA",
			"Tools/LogView",
			"Tools/LogZip",
			"Tools/ProductVersion",
			"Tools/ProductView",
			"Tools/RegMove",
			"Tools/RegPriv",
			"Tools/RSA",
			"Tools/SecAnalyzer",
			"Tools/UpdateHandler",
			"Tools/Win95Reboot",
			"Units/Aku/AkuSys",			# no real compile, just copy
			"Units/Aku/AkuUnit", 
			"Units/CoCoUnit",
			"Units/CommUnit", 
			"Units/EmailUnit",
			"Units/GemosUnit",
			"Units/MiCo/MiCoDA",		# compile first, is used at MiCoUnitPCI
			"Units/MiCo/MiCoSys",		# no real compile, just copy
			"Units/MiCo/MiCoUnitPCI",
			"Units/MiCo/MiCoVxd",		# no real compile, just copy
			"Units/JaCob/JaCobDA",		# compile first, is used at JaCobDll
			"Units/JaCob/JaCobDll",		# compile first, is used at JaCobUnit
			"Units/JaCob/JaCobSys",		# no real compile, just copy
			"Units/JaCob/JaCobTest",
			"Units/JaCob/JaCobUnit",
			"Units/JaCob/JaCobVxd",		# no real compile, just copy
			"Units/SDIUnit",
			"Units/SimUnit",
			"Units/SMSUnit",
			"Units/TOBSUnit",
		);

@Languages=(
			"Enu",
			"Ita",
			);
$theDestinationDirEnu	= "$theDestinationDir/Enu";
$theDestinationDirIta	= "$theDestinationDir/Ita";

#@Recipients=("UWEXP","TOMAS","MARTIN01","GEORGXP","ROLFXP");
@Recipients=("GEORGXP");

#**********************************************************
# create required project directories
&PrintOutput("\n########################################################\n");		# seperator
if ($bDebuggingGlobal) {print ("Creating project directories\n");}

@ProjectDirs=(
				"$theProjectDir",
				"$theProjectDir/Lib",
				"$theProjectDir/Units/MiCo/MicoDA/Release",
				"$theProjectDir/Units/MiCo/MicoUnitPCI/Release",
				"$theProjectDir/Units/JaCob/JaCobDA/Release",
				"$theProjectDir/Units/JaCob/JaCobDll/Release",
				"$theProjectDir/Units/JaCob/JaCobUnit/Release",
				"$theProjectDir/Units/JaCob/JaCobTest/Release",
				"$theProjectDir/Maps",
				"$theProjectDir/Dlls",
				"$ProjectDrive/idip/Dlls"
			 );

for (@ProjectDirs)
{
	&CreateNewDir("$_");
	if (!chdir("$_"))
	{
		&NetSendError("chdir '$_' failed");
	}
}
&PrintOutput("Using $theProjectDir\n");

#**********************************************************
# create required destination directories
if ($bDebuggingGlobal) {print ("Creating destination directories\n");}

&CreateNewDir($theDestinationDir);
if (!chdir("$theDestinationDir"))
{
	&NetSendError("chdir '$theDestinationDir' failed");
}
for (@Languages)
{
	my($sDir) = "$theDestinationDir$_"; 
	&CreateNewDir("$sDir");
	if (!chdir("$sDir"))
	{
		&NetSendError("chdir '$sDir' failed");
	}
}


#**********************************************************
# check for necessary programs and environment
&SearchZIP();
&SearchSS();
&SearchMSDEV();
&SetEnv();


#**********************************************************
# start the real work
if ($bWorkAsSuperMake==1)
{
	&DoSuperMake();
}
elsif ($bWorkAsSuperCopy==1)
{
	&DoSuperCopy();
}
elsif ($bWorkAsSuperHistory==1)
{
	&PrintOutput("Work as SuperHistory\n");
	&CollectBuildNotes();
    &CopyBuildNotes();
#	&ReadBuildNumber();
	# As we do it for the actual, already done build,
	# substitute the currentBuild number with the real one
#	$currentBuild = buildNumber;
    $ErrorText = "";
    $DoneText = "SuperHistory DONE";
    $EndText = "SuperHistory END";
}
&InformRecipientsAndEnd();

#**********************************************************
# close output file
close(OUTPUT_FILE);

close(STDERR);
close(STDOUT);

######################################################################################################
# end of 'main'
#

######################################################################################################
sub DoSuperMake
{
	&PrintOutput("\n########################################################\n");		# seperator
	if ($bGetLibs)
	{
		&PrintOutput("Getting dependency libs\n");
		chdir("$theProjectDir/Lib");
		print `$SAFE_CMD  Get \"\\$\\project\\lib\\imagn32.lib" -I- \"-VL$VersionLabel\"`;
		print `$SAFE_CMD  Get \"\\$\\project\\lib\\ijl15.lib" -I- \"-VL$VersionLabel\"`;
		print `$SAFE_CMD  Get \"\\$\\project\\lib\\luca.lib" -I- \"-VL$VersionLabel\"`;
		print `$SAFE_CMD  Get \"\\$\\project\\lib\\c4lib.lib" -I- \"-VL$VersionLabel\"`;
		print `$SAFE_CMD  Get \"\\$\\project\\lib\\libDRcmd.lib" -I- \"-VL$VersionLabel\"`;
		print `$SAFE_CMD  Get \"\\$\\project\\lib\\dNT.lib" -I- \"-VL$VersionLabel\"`;
		print `$SAFE_CMD  Get \"\\$\\project\\lib\\libcodec.lib" -I- \"-VL$VersionLabel\"`;
#		print `$SAFE_CMD  Get \"\\$\\project\\MegraV2\\MegraV2Dll\\Release\\MegraV2.lib" -I- \"-VL$VersionLabel\"`;
		print `$SAFE_CMD  Get \"\\$\\dv\\acdc\\NeroApiGlue.lib" -I- \"-VL$VersionLabel\"`;
		#
		# get latest version from source safe
		#
		&PrintOutput("\n########################################################\n");		# seperator
		&PrintOutput("Getting dependency projects\n");
		for (@DepProjects)
		{
			$project = $_;
			chdir("$theProjectDir");	# back to project root dir
			&CreateNewDir($project);
			chdir("$project");

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
		&PrintOutput(">>> Not getting dependency libs / projects! <<<\n");
	}

	# the main loop over all Projects
	for (@Projects)
	{
		my($project) = $_;

		chdir("$theProjectDir");
		&CreateNewDir($project);
		chdir("$project");

		&PrintOutput("\n########################################################\n");		# seperator between projects
#		printf ("Current dir %s\n", getcwd);

		&PrintOutput("Current project is $_\n");
		if (/DV/)
		{
#			print "Get from SS \$\\$project\n";
			&TrySourceSafe("\$\\$project");
		}
		else
		{
#			print "Get from SS \$\\Project\\$project\n";
			&TrySourceSafe("\$\\Project\\$project");
		}

		$theCurrentProjectDir="$theProjectDir\/$project";
		chdir("$theCurrentProjectDir");

		&DoConfigurations($project);
	}

	&PrintOutput("\n$statText\n");
	&PrintOutput("Lines in total: $totalLines\n");

	&SetTimeVariables();
	$DoneText = "idip Make DONE at $mday.$month.$year, $hour:$min:$sec, version $versionInfo$currentBuild";
	&PrintOutput("\n$DoneText\n");

	$ErrorText = "OK";
	if ($errorCount || $warningCount || $UnknownCount)
	{
		$ErrorText = "$errorCount error[s], $warningCount warning[s], $UnknownCount Unknown projects";
		&PrintOutput("Es gab $ErrorText\n\n" );
	}
	else
	{
		&PrintOutput("Es gab keine Fehler oder Warnings.\n\n");
	}

	$EndText = "idip Make END";
}

######################################################################################################
sub DoSuperCopy
{
	# switch to copy mode
	$bInFinalCopy = 1;	

	# loop over all projects, depending on $bInFinalCopy
	# now a copy is made, not the nmake call
	for (@Projects)
	{
		my($project);
		$project=$_;

		chdir("$theProjectDir");
		&DoConfigurations($project);

	}	# end of loop over projects
      
	&SetTimeVariables();

	$DoneText = "idipCopy DONE at $mday.$month.$year, $hour:$min:$sec,  version $versionInfo$currentBuild";
	&PrintOutput("\n$DoneText\n");

	$ErrorText = "OK";
	if ($theCopyErrorCount != 0)
	{
		$ErrorText = "$theCopyErrorCount Error[s]";
		&PrintOutput("Es gab $theCopyErrorCount Fehler beim Kopieren\n");
		&PrintOutput("BuildNummer wird nicht erhoeht\n\n");
	}
	else
	{
		&PrintOutput("Es gab keine Fehler beim Kopieren.\n\n");
		&UpdateBuild();
	}

	$EndText = "idipCopy END";
}

######################################################################################################
sub DoConfigurations
{ 
	my($project) = @_;
	my($bDebugging) = ($bDebuggingGlobal || 0);
	my(@cfgs);
	my($proj) = "foo.vcproj";
	my($dest) = "foo";

	# print("DO \"$project\"\n");
		
	# CAVEAT2 jedes Project muss hier eingetragen sein!
	# special settings per project
	if ($project eq "SecurityServer") {
		# makefile and destination have different name than project
		$proj = "sec3.vcproj";
		$dest = "sec3";
	} elsif ($project eq "Capi/Capi4") {
		# makefile and destination have different name than project
		$proj = "Capi4_32.vcproj";
		$dest = "Capi4_32";
	} elsif ($project eq "Units/JaCob/JaCobDll") {
		# destination has different name than project
		$proj = "JaCobDll.vcproj";
		$dest = "JaCob";
	} else {
		# set default values
		$proj = "$project\.vcproj";
		$dest = "$project";

		#
		# CAVEAT order is important
		#
		$proj =~  s/^Units\/MiCo\///;	# drop leading Units/MiCo/MiCoUnitPCI
		$dest =~ s/^Units\/MiCo\///;	# drop leading Units/MiCo/MiCoUnitPCI

		$proj =~  s/^Units\/JaCob\///;	# drop leading Units/JaCob/JaCobUnit
		$dest =~ s/^Units\/JaCob\///;	# drop leading Units/JaCob/JaCobUnit

		$proj =~  s/^Units\/Aku\///;	# drop leading Units/Aku
		$dest =~ s/^Units\/Aku\///;		# drop leading Units/Aku

		$proj =~  s/^MegraV2\///;	# drop leading 
		$dest =~ s/^MegraV2\///;	# drop leading 
		# convert MegraV2DLL. to MegraV2.vcproj
		$proj =~  s/DLL\.vcproj/.vcproj/;	# 
		$dest =~  s/V2DLL/V2/;	# 
		# ditto for Convert to Convrt32
		$proj =~  s/Convert\.vcproj/Convrt32.vcproj/;
		$dest =~  s/Convert/Convrt32/;	# 

		$proj =~ s/^Units\///;	# drop leading Units/
		$dest =~ s/^Units\///;	# drop leading Units/

		$proj =~ s/^DV\///;	# drop leading DV/
		$dest =~ s/^DV\///;	# drop leading DV/

		$proj =~ s/^Tools\///;	# drop leading Tools/
		$dest =~ s/^Tools\///;	# drop leading Tools/

		$proj =~ s/^Capi\///;	# drop leading Capi/
		$dest =~ s/^Capi\///;	# drop leading Capi/

		$proj =~ s/^Lib\///;	# drop leading Lib/
		$dest =~ s/^Lib\///;	# drop leading Lib/

		$proj =~ s/^Clients\///;	# drop leading 
		$dest =~ s/^Clients\///;	# drop leading

		$proj =~ s/^HHLA\///;	# drop leading 
		$dest =~ s/^HHLA\///;	# drop leading

		# NEW PROJECT add here, if on 'new' subdirectory
	}

	# remove not writable Libs / Dlls before compiling
	$theCurrentProjectDir="$theProjectDir/$project";
	$_ = $project;
	if (
		   /Capi4/
		|| /Cipc/
		|| /Convert/
		|| /JaCobDA/
		|| /JaCobDll/
		|| /LogZipLib/
		|| /MegraV2DLL/
		|| /MiCoDA/
		)
	{
		&DeleteEvenIfRO("$theCurrentProjectDir/Release/$dest.lib");
		&DeleteEvenIfRO("$theCurrentProjectDir/Release/$dest.dll");
	}
	elsif (/Lib/)
	{
		&DeleteEvenIfRO("$theProjectDir/Lib/$dest.lib");
		&DeleteEvenIfRO("$theProjectDir/Lib/$dest.dll");
	}

	my($bValidProject)=0;
	my ($bNoCompile)=0;

	# 
	if ($project eq "Capi/ISDNUnit")
	{
		# special for ISDNUnit und PTUnit in einem Project
#		@cfgs=("Release", "PTUnitRelease","Enu","PTEnu");
		@cfgs=("Release");
		$bValidProject=1;
	}
	elsif (
			   $project eq "Cipc"
			|| $project eq "Capi/Capi4"
			|| $project eq "HHLA/MiniClient"
			|| $project eq "HHLA/RemoteControl"
			|| $project eq "Lib/C4Lib"
			|| $project eq "Lib/Crypto"
			|| $project eq "Lib/DeviceDetect"
			|| $project eq "Lib/h263enc"
			|| $project eq "Lib/jpeg"
			|| $project eq "Lib/JpegLib"
			|| $project eq "Lib/PTDecoder"
			|| $project eq "Lib/Skins"
			|| $project eq "Lib/vdbs"
			|| $project eq "Lib/WKClasses"
			|| $project eq "MegraV2/Convert"
			|| $project eq "MegraV2/MegraV2DLL"
			|| $project eq "Tools/CDStart"
			|| $project eq "Tools/convntfs"
			|| $project eq "Tools/DvrtRun"
			|| $project eq "Tools/LogZip"
			|| $project eq "Tools/LogZipLib"
			|| $project eq "Tools/RegMove"
			|| $project eq "Tools/RegPriv"
			|| $project eq "Tools/RSA"
			|| $project eq "Tools/Win95Reboot"
			|| $project eq "Tools/WkTranslator"
			|| $project eq "Units/JaCob/JaCobDA"
			|| $project eq "Units/MiCo/MiCoDA"
			)
	{
		# nur release
		@cfgs=("Release");
		$bValidProject=1;
	}
	elsif (  $project eq "Lib/h263dec")
	{
#		@cfgs=("Release","H26x");
		@cfgs=("Release");
		$bValidProject=1;
	}
	elsif (
			   $project eq "DV/acdc"
			|| $project eq "SocketUnit"
			 
			|| $project eq "Tools/LogView"
			|| $project eq "Tools/ProductVersion"
			|| $project eq "Tools/ProductView"
			|| $project eq "Tools/SecAnalyzer"

			|| $project eq "Units/Aku/AkuUnit"
			|| $project eq "Units/CoCoUnit"
			|| $project eq "Units/CommUnit"
			|| $project eq "Units/EmailUnit"
			|| $project eq "Units/GemosUnit"
			|| $project eq "Units/JaCob/JaCobTest"
			|| $project eq "Units/MiCo/MiCoUnitPCI"
			|| $project eq "Units/SDIUnit"
			|| $project eq "Units/SimUnit"
			|| $project eq "Units/SMSUnit"
			|| $project eq "Units/TOBSUnit"
			)
	{
		# release und enu
#		@cfgs=("Release","Enu");
		@cfgs=("Release");
		$bValidProject=1;
	}
	elsif (    $project eq "SecurityServer"
			
			|| $project eq "Lib/oemgui"
 			|| $project eq "SecurityLauncher"
			|| $project eq "SystemVerwaltung"

			|| $project eq "Dbs"

			|| $project eq "Units/JaCob/JaCobUnit"
			|| $project eq "Units/JaCob/JaCobDll"
			 
			|| $project eq "Tools/DelRegDb"
			|| $project eq "Tools/EULA"
			
			|| $project eq "Clients/Recherche"
			|| $project eq "Clients/Vision"
			)
	{
		# release und enu
#		@cfgs=("Release","Enu", "Ita");
		@cfgs=("Release");
		$bValidProject=1;
	}
	elsif ( $project eq "Tools/UpdateHandler" )
	{
#		@cfgs=("Release","Light");
		@cfgs=("Release");
		$bValidProject=1;
	}
	elsif (
			   $project eq "Units/Aku/AkuSys"
			|| $project eq "Units/JaCob/JaCobSys"
			|| $project eq "Units/JaCob/JaCobVxd"
			|| $project eq "Units/MiCo/MiCoSys"
			|| $project eq "Units/MiCo/MiCoVxd"
			)
	{
		$bNoCompile=1;
		$bValidProject=1;
		# still needs one cfg to get the copy done
		@cfgs=("Release");
	}
	else
	{
		$bValidProject=0;
		&PrintOutput("\nUnknown project: $project\n");
		$UnknownCount++;
		my($sMsg) = "Tool cancelled at unknown project";
		&NetSendError($sMsg);
		die "$sMsg";
	}

	# a valid project ? Then loop over the given configurations.
	if ($bValidProject) 
	{
		if ($bInFinalCopy==0) 
		{
			# it´s not copy; do the nmake
			if ($bNoCompile==0) 
			{	# some projects do not need to be compiled (DDK stuff etc.)

				&DoStaticAnalysis();

				for (@cfgs) 
				{
					if ($bDebugging) {print ("DoOne $project,$proj,$dest\n");}
					&DoOneConfiguration($project,$proj,$dest,$_);
				}
#				print ("$project done\n\n");
			}
		} 
		else 
		{
			# do not do the make, but do the copy
			for (@cfgs) 
			{
				if ($bDebugging) {print ("DoCopy $project,$dest,$_\n");}
				&DoCopy($project,$dest,$_);
			}
		}	# end of copy action
	} 
	else 
	{
		&PrintOutput("Invalid project: $project\n");
	}
}
######################################################################################################
sub DoStaticAnalysis
{
	# perform static analysis of sourceCode
	my($checkCmd)="v:\\perl5\\bin\\checkBreak.bat "; # getcwd is appended
	$checkCmd .= getcwd;
	@checkLog = `$checkCmd`;

	# collect total line stats
	for ($ci=0;$ci <= $#checkLog;$ci++)
	{
		my($tmpJunk)="";
		my($restJunk)="";
		$oneLine=$checkLog[$ci];	# get first line
		$_ = $oneLine;
		if (/#STAT#/)
		{
			$statText .= $oneLine;
			$oneLine =~ s/\s+/ /gm;
			$oneLine =~ s/^\s+//gm;
			# looks like "#STAT# 8191 lines in d:\Project\SecurityLauncher"
			($tmpJunk,$lines,$restJunk) = split(/ /,$oneLine,3);
			$totalLines += $lines;
		}
		elsif ( /nicht/ )
		{
			# ignore
		}
		else
		{
			&PrintOutput("$oneLine");
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
######################################################################################################
sub DoOneConfiguration
{
	my($project,$vcproj,$dest,$cfg)=@_;

#	print " .... $vcproj CFG=\"$dest - Win32 $cfg\"\n";

	my($destDir) = "Release";
	if ($cfg eq "PTUnitRelease") {
		$destDir = "PTUnitRelease";
	}
	if ($cfg eq "PTEnu") {
		$destDir = "PTEnu";
	}
	
	if (! -f $vcproj)
	{
		&NetSendError("$dest has no VCPROJ file $vcproj");
	}

	if ($cfg ne "PTUnitRelease") {
		&UpdateRC2($dest);
	}

#	chmod 0755, "$vcproj";	# allow for local conversion

	my($builtType) = "build";
	if ($bRebuildAll || $bRebuildProj)
	{
		$builtType = "rebuild";
	}

	&PrintOutput("\n$builtType $dest - $cfg\n");

	# 
	# SYNTAX: devenv SolutionName /build ConfigName [/project ProjName] [/projectconfig ConfigName] [ /out FileName]
	# SYNTAX: devenv solutionfile.sln  /build solutionconfig
	my(@errorLog) = `devenv $vcproj /$builtType $cfg `;

#	chmod 0555, "$vcproj";	# undo the above operation, make it readonly again

	&AnalyseDevEnvOutput($project, $dest, $cfg, @errorLog);
}
######################################################################################################
sub AnalyseDevEnvOutput
{
	my($project, $dest, $cfg, @errorLog) = @_;
	my($bDebugging) = ($bDebuggingGlobal || 0);
	my($cfgError) = 0;
	my($cfgWarning) = 0;

	if ($#errorLog>0)
	{
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

			# modify filenames, to allow easy F4 stepping
			if (   /\) \: warning C/
				|| /\) \: error C/
				)
			{
				if (substr($oneLine, 1, 2) ne "\:\\")
				{
					$oneLine = "$theProjectDir/$project/$oneLine";
				}
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
				if ($bDebugging) {print ("LOG $ei : skipped\n");}
			}

			&PrintOutput("$oneLine");					# already has newline

			# count make errors and warnings
			if (   /Invalid Command Line/
				|| / fatal error /
				)
			{
				my($sMsg) = "Tool cancelled at fatal error";
				&NetSendError($sMsg);
				die "$sMsg";
			}
			if (/ error /)
			{
				$cfgError++;
				$errorCount++;
			}
			if (/ [wW]arning /)
			{
				$cfgWarning++;
				$warningCount++;
			}
		}
	}

	local($sMsg) = "$dest - $cfg done";
	if ($cfgError || $cfgWarning)
	{
		$sMsg .= ", $cfgError Error(s) $cfgWarning Warning(s)";
	}
	else
	{
		$sMsg .= ", OK";
	}
	&PrintOutput("$sMsg\n");
}
######################################################################################################
sub TrySourceSafe
{
	my($proj) = @_;
	my($bDebugging) = ($bDebuggingGlobal || 0);

	if ($bDebugging) {print ("TrySourceSafe $proj\n");}

	if (-d "res")
	{
		# delete modified rc2
	}

	# check if vcproj file has changed
	$bRebuildProj = 0;
	my($vcprojPath) = $proj;
	my($vcprojFile) = "";
	if ($bDebugging) {print ("$vcprojPath\n");}

	my(@array)  = split(/\\/,$vcprojPath);     # Array der einzelnen Teilstrings

	# last one is project itself
	foreach $x (@array)
	{
		$vcprojFile = $x;
	}

	if ($bDebugging) {print ("$vcprojFile\n");}

	if ($vcprojFile eq "SecurityServer")
	{	# vcproj file has different name than project
		$vcprojFile = "sec3";
	}
	if ($vcprojFile eq "Capi4")
	{	# vcproj file has different name than project
		$vcprojFile = "Capi4_32";
	}

	$vcprojFile .= "\.vcproj";
	if ($bDebugging) {print ("$vcprojFile\n");}

	$vcprojPath = "$vcprojPath\\$vcprojFile";
	if ($bDebugging) {print ("$vcprojPath\n");}

	# save old file time
	my($time_secs_old) = (stat($vcprojFile))[9]; 
#	print ("time_secs_old $time_secs_old\n");

	# -R recusive
	# -I no input
	# -VL version by label

	my($Label) = $VersionLabel;
	$_ = $proj;

	my(@outputSS) = ();
	if ($Label ne "") {
		&PrintOutput("Getting $proj Version $Label\n");
		@outputSS = `$SAFE_CMD  Get \"$proj\" -R -I-N -O\@vsstmp.txt \"-VL$Label\"`;
	} else {
		&PrintOutput("Getting $proj\n");
		@outputSS = `$SAFE_CMD  Get \"$proj\" -R -I-N -O\@vsstmp.txt`;
	}

	# get new file time	
	my($time_secs_new) = (stat($vcprojFile))[9]; 
#	print ("time_secs_new $time_secs_new\n");

	# if file time changed -> file changed -> rebuild all
	if ($time_secs_new != $time_secs_old)
	{
		$bRebuildProj = 1;
		&PrintOutput("Project has to be rebuild because $vcprojFile changed\n");
	}
#	print ("Rebuild project $bRebuildProj\n");

	if ($bDebugging) {
		# NOT YET print ss
	}
	@outputSS = ();
	unlink("vsstmp.txt");
}
######################################################################################################
sub CreateNewDir
{
	my($pname)=@_;
	my($bDebugging) = ($bDebuggingGlobal || 0);

	if (   $pname =~ /\//
		&& $pname =~ /\\/
		)
	{
		die "ERROR: CreateNewDir wrong call with $pname";
	}
	if ($bDebugging) {print "CreateSubDir $pname\n";}

	my(@subdirs);
	if ($pname =~ /\//) {
		@subdirs = split("/",$pname);
	}
	else
	{
		@subdirs = split(/\\/,$pname);
	}
	foreach (@subdirs) {
		# special check for drive first!
		if (/:/) {
			# absolute path, back to root dir
			chdir ("$_\\");
			if ($bDebugging) {print "Change to $_\n";}
		}
		# check if subdir exists already
		elsif (-d $_) {
			chdir($_);
			if ($bDebugging) {print "Change to $_\n";}
		}
		# else create subdir
		else {
			printf ("Create $_ in %s\n",getcwd);
			system("mkdir $_");
			if (-d $_) {
				chdir($_);
				if ($bDebugging) {print "Change to $_\n";}
			}
			else {
				die "Change to '$_' failed\n";
			}
		}
	}
}
######################################################################################################
sub DoCopy
{
	my($project,$dest,$config)=@_;

	if ($dest eq "ISDNUnit" && $config eq "PTUnitRelease") {
		$dest="PTUnit";
	} elsif ($dest eq "ISDNUnit" && $config eq "PTEnu") {
		$dest="PTUnit";
	}


	# make the destination dependend from the configuration
	my($destinationDir)=$theDestinationDir;
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

	my ($bDoCopy)=1;

	$_ = $project;

	&PrintOutput("COPYING $_\n");

	if (   /oemgui/
		|| /PTDecoder/
		|| /h263dec/
		|| /JpegLib/
		|| /DeviceDetect/
		|| /h263enc/
		|| /Crypto/
		) {
		if ($config eq "Enu") {
			$source = "$theProjectDir\\$project\\$config\\$dest.dll";
			$finalDest = "$destinationDir\\Dlls\\$dest.dll";
		} elsif ($config eq "Ita") {
			$source = "$theProjectDir\\$project\\$config\\$dest.dll";
			$finalDest = "$destinationDir\\Dlls\\$dest.dll";
		} elsif ($config eq "H26x") {
			$source = "$project\\H26x\\$dest.dll";
			$finalDest = "$destinationDir\\Dlls\\h263decx.dll";
		} else {
			$source = "$theProjectDir\\Lib\\$dest.dll";
			$finalDest = "$destinationDir\\Dlls\\$dest.dll";
		}
	} elsif (/MiCoDA/ || /JaCobDA/ || /MegraV2/ ) {
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
	} elsif (/JaCobDll/) {
		$source = "$project\\$config\\JaCob.dll";
		$finalDest = "$destinationDir\\Dlls\\JaCob.dll";
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
	} elsif (/WKClasses/ || /jpeg/ || /C4Lib/ || /vdbs/) {
		$bDoCopy=0;
	} elsif (/UpdateHandler/ ) {
		$source = "$project\\$config\\$dest.exe";
		$finalDest = "$destinationDir\\$dest.exe";
		if ($config eq "Light") {
			$finalDest = "$theDestinationDir\\$dest"."Light.exe";
		}
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
		if (/Clients\\Vision/) {
			$dest = "Vision40";
		} elsif (
			/oemgui/ || 
			/PTDecoder/ || 
			/JpegLib/ || 
			/DeviceDetect/ || 
			/h263enc/ || 
			/h263dec/ ) {
		# .map are not in Lib/ but in their Release/ directories
			$source = "$project\\$config\\$dest.map";
		} elsif ( /MiCoSys/ || /MiCoVxd/ || /JaCobSys/ || /JaCobVxD/ || /AkuSys/ ) {
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
			&PrintOutput("NO copy of $finalDest\n");
		}
	} 
	else 
	{
		# do not copy, static libs for example
	}
}
######################################################################################################
sub UpdateBuild
{
	# update the BuildNumber
	&PrintOutput("Updating $theDestinationDir/vinfo.pvi\n");
	`$theDestinationDir\\ProductVersion -updateDirectory $theDestinationDir`;

	chdir($ProjectDrive);	

	&CollectBuildNotes();
    &CopyBuildNotes();

    # set a version label in the SourceSafe
	$LabelText = "$versionInfo$currentBuild";
    &PrintOutput("Setting SourceSafeLabel: $LabelText\n");
	`$SAFE_CMD  Label \$/Project -C- -L$LabelText`;

	# safe label text in file.
	open (LABEL_OUT_FILE,">$theDestinationDir\\label.txt");
	print(LABEL_OUT_FILE "$LabelText\n");
	close(LABEL_OUT_FILE);

	# safe label text in file.
	open (VERSION_OUT_FILE,">$theDestinationDir\\version.txt");
	print(VERSION_OUT_FILE "$versionInfo$currentBuild\n");
	close(VERSION_OUT_FILE);

	chdir($theDestinationDir);	
	&PrintOutput("Creating g:\\BuildZips\\Build$currentBuild.zip\n");
    &PrintOutput("executing $ZIP_CMD -add -recurse -directories g:\\BuildZips\\Build$currentBuild.zip $theDestinationDir\\*.*\n");
	`$ZIP_CMD -add -recurse -directories g:\\BuildZips\\Build$currentBuild.zip $theDestinationDir\\*.*`;

	chdir($theDestinationDirEnu);	
	&PrintOutput("Creating g:\\BuildZips\\Enu$currentBuild.zip\n");
    &PrintOutput("executing $ZIP_CMD -add -recurse -directories g:\\BuildZips\\Enu$currentBuild.zip $theDestinationDirEnu\\*.*\n");
	`$ZIP_CMD -add -recurse -directories g:\\BuildZips\\Enu$currentBuild.zip $theDestinationDirEnu\\*.*`;

	chdir($theDestinationDirIta);	
	&PrintOutput("Creating g:\\BuildZips\\Ita$currentBuild.zip\n");
    &PrintOutput("executing $ZIP_CMD -add -recurse -directories g:\\BuildZips\\Ita$currentBuild.zip $theDestinationDirIta\\*.*\n");
	`$ZIP_CMD -add -recurse -directories g:\\BuildZips\\Ita$currentBuild.zip $theDestinationDirIta\\*.*`;
}
######################################################################################################
sub CollectBuildNotes
{
	my($bDebugging) = ($bDebuggingGlobal || 0);

	$filename = "$theDestinationDir\\BuildNotes$currentBuild.htm";
	&DeleteEvenIfRO($filename);
	open (OFILE,">>$filename");
	&PrintOutput("BuildNumber $currentBuild at $mday.$month.$year, $hour:$min:$sec\n");
	$title = "BuildNumber $currentBuild at $mday.$month.$year, $hour:$min:$sec";
	print (OFILE "<HTML>\n");
	print (OFILE "<TITLE>$title</TITLE>\n");
	print (OFILE "<BODY>\n");
	print (OFILE "<H1>$title</H1>\n");
	print ("Scanning for history.txt...\n");
	# add CIPC to history projects
	# NOT YET take all dep projects
	
	my(@allProjects);

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
		my($project);
		$project=$_;
		chdir("/Project/$project");

		if ($bDebugging) {printf ("Searching history in %s...\n",getcwd());}

		if (-f "history.txt") 
		{

			# -IECW ignore case/end of line/white space changes
			print `$SAFE_CMD Diff \$/Project/$project/history.txt -DS200 -IECW -VL$prevLabelText > SuperHistory.txt`;
			# now superHistory.txt is created
			# collect between the topmost Label:"Build
			# from this take only Chg: To : Ins:
			my($bAlreadyDidPrint)=0;
			my($bAlreadyDidPrintText)=0;
			open(HFILE,"SuperHistory.txt") || warn "can't open SuperHistory.txt\n";

			if ($bDebugging) {printf ("Scanning SuperHistory.txt in %s...\n",getcwd());}

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
				print (OFILE "<BR>\n");
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
######################################################################################################
sub CopyBuildNotes
{
	$source = "$theDestinationDir\\BuildNotes$currentBuild.htm";
	$dest   = "\\\\mail01\\buildnotes\\idip\\BuildNotes$currentBuild.htm";
	print `copy $source $dest`;
	&PrintOutput("CopyBuildNotes done.\n");
}
######################################################################################################
sub DeleteEvenIfRO
{
	my($oneFile)=@_;
	my($bDebugging) = ($bDebuggingGlobal || 0);

	if ($bDebugging) {printf ("DeleteEvenIfRO for $oneFile in %s\n",getcwd());}

	if (-f "$oneFile") {
		# remove write protectection
		chmod 0755, "$oneFile";
		unlink ("$oneFile");

		if (-f "$oneFile") {
			&PrintOutput("OOPS Still have $oneFile\n");
		} else {
			if ($bDebugging) {print ("Removed $oneFile\n");}
		}
	} else {
		if ($bDebugging) {print ("DeleteEvenIfRO: $oneFile not found\n");}
	}
}
######################################################################################################
# usage:
# &MyDoCopy("Source","Dest",bDoCountErrors);
sub MyDoCopy
{
	my($source,$finalDest,$bDoCountError)=@_;
	my($copyError) = "";

	if (-f $source) {
		&PrintOutput("Copy $source --> $finalDest\n");
		$cmd = "copy $source $finalDest";
		$cmd =~ s/\//\\/gm;
		$copyError = `$cmd`;

		$_ = $copyError;
		if (/0 Datei/) {
			$copyError =~ s/\s+0 Datei.*\n//;
			if ($bDoCountError) {
				$theCopyErrorCount++;
				&NetSendError("$copyError");
			} else {
				&PrintOutput("Error not counted \n");
			}
		}

	} else {
		if ($bDoCountError) {
			$theCopyErrorCount++;
			&NetSendError("$source not found");
		} else {
			&PrintOutput("File not found, $source, but not counted\n");
		}
	}
}
######################################################################################################
sub UpdateRC2
{
	if (-d "res")
	{
		my($dest)=@_;

		# WK_VERSION(5, 0, 0, 1, "5.0.0.1\0", // @AUTO_UPDATE
		chdir("res");
		
		$rcFile="$dest.rc2";
		$_ = $rcFile;
		if ( /MiCoUnitPCI/ ) {
			$rcFile =~ s/MiCoUnitPCI/MiCoUnit/;
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
			# WK_VERSION(5, 0, 0, 1, "5.0.0.1\0", // @AUTO_UPDATE
			`v:\\perl5\\bin\\superReplace.bat -updateBuild "WK_VERSION.*\@AUTO_UPDATE" "WK_VERSION($VersionMajor, $VersionMinor, $VersionSubIndex, $currentBuild, \\"$versionInfo$currentBuild\\0\\", // \@AUTO_UPDATE"`;
			chmod 0555, $rcFile;
		}
		else
		{
			&PrintOutput("no rc2 file found $rcFile\n");
		}
		chdir("..");
	}
	else
	{
		&PrintOutput("no res dir found\n");
	}
}
######################################################################################################
sub ReadBuildNumber
{
	if ($buildNumber==0) {
		my($tmpVersion)=0;
		my($tmpNumRecord)=0;

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
		&PrintOutput("Using preset buildNumber $buildNumber\n");
	}
	$currentBuild = $buildNumber+1;
} # end of ReadBuildNumber
######################################################################################################
sub SetTimeVariables
{
	my ($isdst,$wday,$yday);
	
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
######################################################################################################
sub SearchZIP
{
	$ZIP_CMD="v:\\bin\\tools\\zip\\pkzip25.exe";
	if (! -f "$ZIP_CMD")
	{
		&NetSendError("pkzip.exe not found $ZIP_CMD");
	}
}
######################################################################################################
sub SearchMSDEV
{
	if ( -f "C:/VS_70/Common7/IDE/devenv.com" ) {
		$MSDEV_DIR = "C:\\VS_70";
	} else {
		&NetSendError("VisualStudio not found!");
	}
	&PrintOutput("VisualStudio found at $MSDEV_DIR\n");
}	# end of SearchMSDEV
######################################################################################################
sub SearchSS
{
	if ( -f "c:/msdev/common/vss/win32/ss.exe" ) {
		$SAFE_CMD="C:\\msdev\\common\\vss\\win32\\ss.exe";
	} elsif ( -f "C:/VS_60/Common/VSS/win32/ss.exe" ) {
		$SAFE_CMD="C:\\VS_60\\Common\\VSS\\win32\\ss.exe";
	} else {
		&NetSendError("SourceSafe not found");
	}
	&PrintOutput("SourceSafe found at $SAFE_CMD\n");
}
######################################################################################################
sub SetEnv
{
	# set env variables, not using V:
	# CAVEAT requires $theProjectDir

	# INCLUDE *******************************************************
	# keep order as in VS .NET itself

	$TmpInclude  = "$MSDEV_DIR\\Vc7\\Include;";
	$TmpInclude .= "$MSDEV_DIR\\Vc7\\atlmfc\\include;";
	$TmpInclude .= "$MSDEV_DIR\\Vc7\\atlmfc\\src\\mfc;";
	$TmpInclude .= "$MSDEV_DIR\\Vc7\\PlatformSDK\\include\\prerelease;";
	$TmpInclude .= "$MSDEV_DIR\\Vc7\\PlatformSDK\\include;";

	$TmpInclude .= "$MSDEV_DIR\\FrameworkSDK\\include;";

	$TmpInclude .= "$theProjectDir\\Include;";
	$TmpInclude .= "$theProjectDir\\Cipc;";
	$TmpInclude .= "$theProjectDir\\lib;";
#	$TmpInclude .= "$theProjectDir\\Dv\\lib;";

	&PrintOutput("INCLUDE IS: $TmpInclude\n");
	$ENV{'INCLUDE'}=$TmpInclude;

	# LIB *******************************************************
	# keep order as in VS .NET itself

	$TmpLib  = "$MSDEV_DIR\\Vc7\\lib;";
	$TmpLib .= "$MSDEV_DIR\\Vc7\\atlmfc\\lib;";
	$TmpLib .= "$MSDEV_DIR\\Vc7\\PlatformSDK\\lib\\prerelease;";
	$TmpLib .= "$MSDEV_DIR\\Vc7\\PlatformSDK\\lib;";

	$TmpLib .= "$MSDEV_DIR\\FrameworkSDK\\Lib;";

	$TmpLib .= "$theProjectDir\\Lib;";
	$TmpLib .= "$theProjectDir\\Cipc\\Release;";
#	$TmpLib .= "$theProjectDir\\DV\\Lib;";

	&PrintOutput("LIB IS: $TmpLib\n");
	$ENV{'LIB'} = $TmpLib;
	
	# PATH *******************************************************
	# make sure devenv is in the path
	$TmpPath = $ENV{'PATH'};
	$TmpPath .= ";$MSDEV_DIR\\Common7\\IDE;";
	$ENV{'PATH'} = $TmpPath;

	&PrintOutput("PATH is $ENV{'PATH'}\n");
}
######################################################################################################
sub NetSendError
{
	my($sMsg1)=@_;
	my($sMsg) = "ERROR: $sMsg1";

	&PrintOutput("\n$sMsg\n\n");

	for (@Recipients)
	{
		if ($bWorkAsSuperMake)
		{
			`Net Send $_ "$sMsg from idipMake"`;
		}
		elsif ($bWorkAsSuperCopy)
		{
			`Net Send $_ "$sMsg from idipCopy"`;
		}
		else
		{
			`Net Send $_ "$sMsg"`;
		}
	}
	die;
}
######################################################################################################
sub Wait
{
	my($waitSecs)=@_;
	SetTimeVariables();
	my($startSecs) = $hour*3600 + $min*60 + $sec;
	
	print ("Wait started at $startSecs for $waitSecs\n");

	my($currentSecs) = $startSecs;
	my($currentSecsCount) = $currentSecs;
	while ($currentSecs < ($startSecs + $waitSecs))
	{
		SetTimeVariables();
		$currentSecs = $hour*3600 + $min*60 + $sec;
		if ($currentSecs > $currentSecsCount)
		{
			print ("$currentSecs\n");
			$currentSecsCount = $currentSecs;
		}
	}
	print ("Wait leaved at  $currentSecs\n");
}
######################################################################################################
sub InformRecipientsAndEnd
{
	for (@Recipients)
	{
#		print "Net Send $_\n";
		`Net Send $_ "$DoneText, $ErrorText"`;
	}
	&SetTimeVariables();
	&PrintOutput("$EndText at $mday.$month.$year, $hour:$min:$sec, version $versionInfo$currentBuild\n");
}
######################################################################################################
sub PrintOutput
{
	my($text)=@_;
	print $text;
	print(OUTPUT_FILE $text);
}
######################################################################################################
__END__
:endofperl
