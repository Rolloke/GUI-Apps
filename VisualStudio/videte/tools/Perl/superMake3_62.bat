@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

#
# PERL PART FOLLOWS HERE
#

#
# trace options
#
$bTraceOneConfig=1;
$bTraceCopy=1;
$bTraceUnlink=0;
$bTraceSuperHistory=0;
$bTraceSourceSafe=0;
$bGetLibs=1;	# disable for faster testing

#############
# <GLOBALS> #
#############
$bWorkAsSuperCopy = 0;
$bInFinalCopy = 0;
$theCopyErrorCount=0;	# modified in MyDoCopy
$errorCount=0;
$warningCount=0;

$theProjectDir="Project Dir not found";
$theDestinationDir="V:\\bin\\DVRT";

# used in superCopy mode only
$theSourceSafeDestination="\$/Bin/DVRT";

# $VersionLabel=""; # >>> leer fuer aktuelle Version <<<
# oder mit VersionsLabel passend zum SourceSafe $/Project label
$VersionLabel="Build_150_19.8.98,11:44";

$buildNumber=8;	# read from vinfo.pvi, but used in multiple functions

$totalLines=0;
$statText = "Statistic:\n";
##############
# </GLOBALS> #
##########+###


# argument check:
# -ftpUpdate, only ftUpdate buildNotes.htm
# - superCopy work as copy only, assumes successful superMake
# no args, plain superMake
if ($#ARGV==0 && $ARGV[0] eq "-ftpUpdate") {	# last index, not count
	&FtpUpdate();
	die "FTP done.";
}

if ($#ARGV==0 && $ARGV[0] eq "-superCopy") {	# last index, not count
	$bWorkAsSuperCopy = 1;
} else {
	$bWorkAsSuperCopy = 0;
}


# dependencie projects are not compiled
@DepProjects=(  "include",
				"Capi/Capi4",
				"CodeBase/LIB32/CodeBase51ForApp",
				"Lib/jpeg",
				"Lib/WKClasses",
				"Tools/LogzipLib",
				"Units/MiCo/MiCoSys",
				"Units/Aku/AkuSys",
				"Units/MiCo/MiCoDA",
				"Units/MiCo/MiCoUnitPCI" # OOPS only for micodefs.h
				);

# LibProjects are compiled first, that is before the normal Projects
@LibProjects=("Cipc",
			  "MegraV2/Convert",
			  "MegraV2/MegraV2DLL",
			  "Units/MiCo/MiCoDA",
			  "Lib/DVRCore",
			  "Lib/oemgui",
			  "Lib/PTDecoder",
			  "Lib/jpeg",
			  "Lib/h263dec",
			  "Lib/client40",
			  "Lib/WKClasses",
			  "Units/MiCo/MiCoSys",	# no real compile, just copy
			  "Units/Aku/AkuSys",	# no real compile, just copy
			  "Units/MiCo/MiCoVxd"	# no real compile, just copy
			  );

# project which have AutoDuck comments
@DocProjects = (
		"Cipc",
		"Lib/WKClasses",
		"Lib/DVRCore",
		"Lib/h263"
);


# Liste aller relevanten Projekte
# CAVEAT muss auch weiter unten angepasst werden
# siehe CAVEAT2
# NEW PROJECT das WorkingDirectory muss auf Senso fuer D:\Project\NewProject gesetzt werden
# NEW PROJECT add here
@Projects=( 
			"Units/Aku/AkuUnit", 
			"Units/CoCoUnit",
			"Units/CommUnit", 
			"Units/GAUnit",
			"Units/MiCo/MiCoUnitPCI",
			"Units/SDIUnit",
			"Units/SimUnit",

			"Clients/SDIConfig",
			# DROPPED "Clients/Vision",
			# NOT YET PTConfig

			"DataBaseClient",
			"DataBaseServer", 
			"Clients/CDRWriter",	# alias DBBackup

			"Tools/convntfs",
			"Tools/DelRegDb",
			"Tools/RegMove",
			"Tools/RegPriv",
			"Tools/LogZip",
			"Tools/ProductView",
			"Tools/ProductVersion",
			"Tools/SecAnalyzer",
			"Tools/UpdateHandler",

			"SecurityExplorer",	# alias vision			
			"SecurityLauncher",
			"SecurityServer", 
			"SystemVerwaltung",
				
			"SocketUnit",

			"Capi/ISDNUnit"		# also contains PTUnit			
			);

# force "flush output"
$|=1;

use Cwd;

open(STDERR,">&STDOUT");
select(STDERR); $|=1;
select(STDOUT); $|=1;

$theCurrentProjectDir="UNKNOWN";
$theCurrentProjectDrive="UNKNOWN";

if ( -f "C:/vss/win32/ss.exe" ) {
	$SAFE_CMD="c:\\vss\\win32\\ss.exe";
} elsif ( -f "c:/msdev/vss/win32/ss.exe" ) {
	$SAFE_CMD="C:\\msdev\\vss\\win32\\ss.exe";
} elsif ( -f "D:/vss/win32/ss.exe" ) {
	$SAFE_CMD="D:\\vss\\win32\\ss.exe";
} elsif (-f "E:/vss/win32/ss.exe" ) {
	$SAFE_CMD="E:\\vss\\win32\\ss.exe";
} elsif ( -f "F:/vss/win32/ss.exe" ) {
	$SAFE_CMD="F:\\vss\\win32\\ss.exe";
} else {
	die "SourceSafe not found\n";
}
print "SourceSafe is $SAFE_CMD\n";

if ( -d "C:/devstudio/vc" ) {
	$MSDEV_DIR="C:\\devstudio\\vc";
} elsif ( -d "c:/msdev/vc" ) {
	$MSDEV_DIR="c:\\msdev\\vc";
} elsif (-d "E:/devstudio/vc" ) {
	$MSDEV_DIR="E:\\devstudio\\vc";
} elsif ( -d "F:/devstudio/vc" ) {
	$MSDEV_DIR="F:\\devstudio\\vc";
} elsif ( -d "d:/msdev/vc" ) { # CAVEAT order is important. special 6.0 hack
	$MSDEV_DIR="d:\\msdev\\vc";
} elsif ( -d "D:/Microsoft Visual Studio/vc98" ) {
	$MSDEV_DIR="D:/Microsoft Visual Studio/vc98";
} else {
	die "MsDev not found\n";
}

print "Found $MSDEV_DIR\n";

chdir ("d:");
&CreateNewDir("d:\\Project");
@Drives=("d:");

for (@Drives) {
	$pdrive="$_";
	if ( -d "$_/Project" ) {

		$theCurrentProjectDrive="$_";
		$theProjectDir="$_\\Project";
		print ("Using $theProjectDir\n");
		chdir("$_/Project") || die "chdir $_/Project failed\n";

		last;
	}
}

if (! -d "$theProjectDir\\Maps") {
	`mkdir "$theProjectDir\\Maps`;
}

if (! -d "$theProjectDir\\Dlls") {
	`mkdir "$theProjectDir\\Dlls`;
}

#
# set env variables, not using V:
#
$ENV{'INCLUDE'}="$MSDEV_DIR\\include;d:\\inetsdk\\include;$MSDEV_DIR\\mfc\\include;$MSDEV_DIR\\atl\\include;$MSDEV_DIR\\mfc\\src;$theProjectDir\\Include;$theProjectDir\\Cipc;$theProjectDir\\lib;$theProjectDir\\lib\\WKClasses";
$ENV{'LIB'} = "d:\\inetsdk\\lib;$MSDEV_DIR\\lib;$MSDEV_DIR\\mfc\\lib;$theProjectDir\\Lib;$theProjectDir\\Cipc\\Release";
$ENV{'PATH'} = "$MSDEV_DIR\\bin;c:\\msdev\\sharedide\\bin;C:\\Windows;D:\\DevStudio\\Tools";

if ($bWorkAsSuperCopy==0) {

	if ($bGetLibs) {
		#
		# get imagn32.lib
		#
		print ("Getting imagn32.lib....\n");
		
		if (! -d "$theProjectDir/Lib") {
			`mkdir "$theProjectDir/Lib"`;
		}

		chdir("$theProjectDir/Lib");
		`$SAFE_CMD  Get \"\\$\\project\\lib\\imagn32.lib" -I- \"-VL$VersionLabel\"`;
		#
		# get latest version from source safe
		#
		for (@DepProjects) {
			$project = $_;
			chdir("/Project/");	# back to project root dir
			if (! -d $project) {	# subdir already there ?
				&CreateNewDir($project);
				chdir("/Project/");
			}
			chdir($project);
			&TrySourceSafe;
		}
	} else {
		print "not getting DepProjects!\n";
	}

	# loop over all LibProjects
	for (@LibProjects) {
		local($project);
		$project=$_;

		chdir("/Project/");
		if (! -d $project) {
			&CreateNewDir($project);
			chdir("/Project/");
		}
		chdir($project);

		# remove all lib versions
		$_ = $project;
		if (/Cipc/) {
			&MyUnlink("Release/CIPC.lib",0);
		} elsif (/MegraV2DLL/) {
			&MyUnlink("Release/MegraV2.lib",0);
			&MyUnlink("Release/MegraV2.dll",0);
		} elsif (/Convert/) {
			&MyUnlink("Release/Convrt32.lib",0);
			&MyUnlink("Release/Convrt32.dll",0);
		} elsif (/MiCoDA/) {
			&MyUnlink("Release/MiCoDA.lib",0);
			&MyUnlink("Release/MiCoDA.dll",0);
			if (! -d "$theProjectDir/Units/MiCo/MicoUnitPCI/Release") {
				# fuer komische copies
				&CreateNewDir("$theProjectDir/Units/MiCo/MicoUnitPCI/Release");
			}
		}

		# get the latest/labeled version from source safe
		&TrySourceSafe;

		$theCurrentProjectDir="Project/$project";

		# remove not writable libs before compiling

		chdir($theCurrentProjectDir);
		$_ = $project;
		if (/Cipc/) {
			&MyUnlink("Release/CIPC.lib",1);
		} elsif (/MegraV2DLL/) {
			# no message, the libs are not in the project
			&MyUnlink("Release/MegraV2.lib",0);
			&MyUnlink("Release/MegraV2.dll",0);
		} elsif (/Convert/) {
			&MyUnlink("Release/Convrt32.lib",1);
			&MyUnlink("Release/Convrt32.dll",1);
		} elsif (/MiCoDA/) {
			&MyUnlink("Release/MiCoDA.lib",1);
			&MyUnlink("Release/MiCoDA.dll",1);
		}

		&DoConfigurations($project);
	}	# end of loop over all LibProjects

	#
	# the main loop over all Projects
	#
	for (@Projects) {
		local($project);
		$project=$_;

		chdir("/Project/");
		if (! -d $project) {
			&CreateNewDir($project);
			chdir("/Project/");
		}
		chdir($project);
		&TrySourceSafe;
		$theCurrentProjectDir="Project/$project";
		&DoConfigurations($project);
	}

}

	# NEW 171197 do super copy right here
	if ($bWorkAsSuperCopy) {
		# switch to copy mode
		$bInFinalCopy = 1;	
		# and again loop over all projects, depending on $bInFinalCopy
		# now a copy is made, not the nmake call

		for (@LibProjects) {
			local($project);
			$project=$_;

			chdir("/Project/");
			&DoConfigurations($project);
		}	# end of loop over projects
		
		for (@Projects) {
			local($project);
			$project=$_;

			chdir("/Project/");
			&DoConfigurations($project);

		}	# end of loop over projects
		
	}


	($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
	$month=eval($mon+1);
	$filler=":";
	if ( $min < 10) {
		$min = "0$min";
	}
	if ( $hour < 10) {
		$hour = "0$hour";
	}

	if ($bWorkAsSuperCopy==0) {
		print ("$statText");
		print ("Lines in total: $totalLines\n");
		print "SuperMake DONE at $mday.$month.$year, $hour:$min, version was $VersionLabel\n";
		if ($errorCount || $warningCount) {
			print ("Es gab $errorCount error[s], $warningCount warning[s]\n" );
		} else {
			print "Es gab keine Fehler oder Warnings.\n";
		}
	} else {
		if ($theCopyErrorCount!=0) {
			print (">>> Es gab $theCopyErrorCount Fehler beim Kopieren<<<\n");
			print (">>> BuildNummer wird nicht erhoeht <<<\n");
		} else {
			print ("Es gab keine Fehler beim Kopieren.\n");
			&UpdateBuild();
		}
		print "SuperCopy DONE at $mday.$month.$year, $hour:$min, version was $VersionLabel\n";
	}


# end of 'main'

sub UpdateBuild
{
	# update the BuildNumber
	print ("Updating $theDestinationDir/vinfo.pvi\n");
	`$theDestinationDir\\ProductVersion -updateDirectory $theDestinationDir`;
	
	# scan the first line of vinfo.pvi
	open(FILE,"$theDestinationDir/vinfo.pvi") || warn "can't open vinfo.pvi\n";
	while (<FILE>) {
		if (/\d+\s+\d+\s+\d+/) {
			# first line in vinfo.pvi is "1 10 28" format build numRecords
			$_ =~ s/[\ \t]+/ /gm;	# kill multiple blanks to do the split
			($version,$buildNumber,$numRecord) = split(/ /,$_,3);
		}
	} # EOF vinfo.pvi
	close(FILE);
	
	$LabelText = "Build_$buildNumber\_$mday.$month.$year,$hour:$min";
	# set a version label in the SourceSafe
	`$SAFE_CMD  Label \$/Project -C- -L$LabelText`;

	# recursive checkin off all files known in the SourceSafe
	# this is not recursive of $theDestinationDir !!!
	# -K	Keep the file checked out after checking it in.
	chdir ($theDestinationDir);
	print ("CheckingIn $theDestinationDir\n");	
	`$SAFE_CMD  Checkin  $theSourceSafeDestination -K -C$LabelText -R -I-`;
	
	# now the same label for $/Bin
	`$SAFE_CMD  Label $theSourceSafeDestination -C- -L$LabelText`;

	chdir($theCurrentProjectDrive);	

	&CollectBuildNotes();

	&FtpUpdate();
}

sub FtpUpdate
{
	# automagically update BuildNotes.htm on the IntraNet server
	print ("Updating webdoc/BuildNotes.htm...");
	print `c:\\windows\\ftp.exe -s:V:\\bin\\doftp.inp 134.245.76.81`;
	print ("done.\n");
}

#########################


sub DoOneConfiguration
{
	local($mak,$dest,$cfg)=@_;
	local($oneLine)="";
	local($ei)=0;

	# print " .... $mak CFG=\"$dest - Win32 $cfg\"\n";

	# always remove precompiled header
	# NOT YET &MyUnlink("Release/$dest\.pch",0);

	# perform static analysis of sourceCode
	local($checkCmd)="v:\\perl5\\bin\\checkBreak.bat ";

	$checkCmd .= getcwd;
	@checkLog = `$checkCmd`;
	# collect total line stats
	for ($ci=0;$ci <= $#checkLog;$ci++) {
		$firstLine=$checkLog[$ci];	# get first line
		$_ = $firstLine;
		if (/#STAT#/) {
			$statText .= $firstLine;
			# looks like "#STAT# 8191 lines in d:\Project\SecurityLauncher"
			($junk,$lines) = split(/ /,$firstLine,2);
			$totalLines += $lines;
		} else {
			print ($firstLine);
		}
	}
	
	&ModifyMakeFile($mak);
	
	if (-f "$MSDEV_DIR\\bin\\nmake.exe") {
		# /Q check only
		# /S suppress executed command
		# always kill the destination
		local($destDir);
		$destDir="Release";
		if ($cfg eq "PTUnitRelease") {
			$destDir="PTUnitRelease";
		}
		
		# new UF with absolute path
		# and for dll's with lib path
		&MyUnlink("$theProjectDir/lib/$dest.dll",0);
		&MyUnlink("$theProjectDir/$project/$destDir/$dest.exe",0);

		# check for blanks in dir
		$_ = $MSDEV_DIR;
		if ( / / ) {
			# space in dir can use bak tics
			print ("damned blanks, using d:\\nmake\n");
			@errorLog = `D:\\nmake /nologo /S /F New$mak CFG=\"$dest - Win32 $cfg\"`;
		} else {
			@errorLog = `$MSDEV_DIR\\bin\\nmake /nologo /S /F New$mak CFG=\"$dest - Win32 $cfg\"`;
		}

		if ($#errorLog>0) {
			for ($ei=0;$ei <= $#errorLog;$ei++) {
				$oneLine=$errorLog[$ei];
	
				# modify filenames, to allow easy F4 stepping
				$_ = $oneLine;	# OOPS
				if ( /kopiert/ || /copied/ 
					|| /Creating lib/ 
					|| /Generating Code/ || /Compiling\.\.\./) {
					# do not print		
					$oneLine = "";
				} elsif ( /LNK4089/ || /LNK4033/ ) {
					# do not print / count
					# warning LNK4089: all references to "GDI32.dll" discarded by /OPT:REF
					$oneLine = "";
				} else {
					$oneLine=~ s/^/$theCurrentProjectDrive\\$theCurrentProjectDir\\/gm;
					$oneLine =~ s/^$theCurrentProjectDrive.*\.cpp$//gm;
				}
				$_ = $oneLine;	# OOPS

				
				if (/error/) {
					$errorCount++;
				}
				if (/[wW]arning/) {
					$warningCount++;
				}


				if ( /\w/ ){		
					print ("$oneLine");	# already has newline
				}
			}
		} else {
			print "Done  $mak CFG=\"$dest - Win32 $cfg\"\n";
		}
	} else {
		print ("$MSDEV_DIR\\bin\\nmake NOT FOUND !?\n");
	}
}

sub DoConfigurations
{ 
	local($project)=@_;
	
	local(@cfgs);
	local($mak)="foo.mak";
	local($dest)="foo";

	# print("DO \"$project\"\n");
		
	# CAVEAT2 jedes Project muss hier eingetragen sein!
	# special settings per project
	if ($project eq "SecurityServer") {
		# makefile has diffenrent name than project
		$mak = "sec3.mak";
		$dest = "sec3";
	} else {
		# set default values
		$mak = "$project\.mak";
		$dest = "$project";

		#
		# CAVEAT order is important
		#
		$mak =~  s/^Units\/MiCo\///;	# drop leading Units/MiCo/MiCoUnitPCI
		$dest =~ s/^Units\/MiCo\///;	# drop leading Units/MiCo/MiCoUnitPCI

		$mak =~  s/^Units\/Aku\///;	# drop leading Units/Aku
		$dest =~ s/^Units\/Aku\///;	# drop leading Units/Aku

		$mak =~  s/^MegraV2\///;	# drop leading 
		$dest =~ s/^MegraV2\///;	# drop leading 
		# convert MegraV2DLL.mak to MegraV2.mak
		$mak =~  s/DLL\.mak/.mak/;	# 
		$mak =~  s/Convert\.mak/Convrt32.mak/;	# 
		# ditto fo the destination MegraV2DLL to MegraV2
		$dest =~  s/V2DLL/V2/;	# 
		$dest =~  s/Convert/Convrt32/;	# 

		$mak =~ s/^Units\///;	# drop leading Units/
		$dest =~ s/^Units\///;	# drop leading Units/

		$mak =~ s/^Tools\///;	# drop leading Units/
		$dest =~ s/^Tools\///;	# drop leading Units/

		$mak =~ s/^Capi\///;	# drop leading Units/
		$dest =~ s/^Capi\///;	# drop leading Units/

		$mak =~ s/^Lib\///;		# drop leading Lib/
		$dest =~ s/^Lib\///;	# drop leading Lib/

		$mak =~ s/^Clients\///;	# drop leading 
		$dest =~ s/^Clients\///;	# drop leading

		# NEW PROJECT add here, if on 'new' subdirectory
	}

	local($bValidProject)=0;
	local ($bNoCompile)=0;

	# 
	if ($project eq "Capi/ISDNUnit") {
		# special for ISDNUnit und PTUnit in einem Project
		@cfgs=("Release", "PTUnitRelease");
		$bValidProject=1;
	} elsif ($project eq "Units/MiCo/MiCoSys"
			|| $project eq "Units/MiCo/MiCoVxd"
			|| $project eq "Units/Aku/AkuSys"
			) {
			$bNoCompile=1;
			$bValidProject=1;
			# still needs one cfg to get the copy done
			@cfgs=("Release");
	} elsif (
			   $project eq "Cipc"
			|| $project eq "Lib/DVRCore"
			|| $project eq "Lib/oemgui"
			|| $project eq "Lib/PTDecoder"
			|| $project eq "Lib/jpeg"
			|| $project eq "Lib/h263dec"
			|| $project eq "Lib/client40"
			|| $project eq "Lib/WKClasses"
			|| $project eq "MegraV2/Convert"
			|| $project eq "MegraV2/MegraV2DLL"
			|| $project eq "Units/MiCo/MiCoDA"

			|| $project eq "SecurityLauncher"
			|| $project eq "SecurityExplorer"
			|| $project eq "SecurityServer"
			|| $project eq "SystemVerwaltung"
			|| $project eq "SocketUnit"

			|| $project eq "Units/CoCoUnit"
			|| $project eq "Units/Aku/AkuUnit"
			|| $project eq "Units/CommUnit"
			|| $project eq "Units/MiCo/MiCoUnitPCI"
			|| $project eq "Units/SimUnit"
			|| $project eq "Units/GAUnit"
			|| $project eq "Units/SDIUnit"

			|| $project eq "DataBaseClient"
			|| $project eq "DataBaseServer"
			|| $project eq "Clients/CDRWriter"
			
			|| $project eq "Tools/convntfs"
			|| $project eq "Tools/DelRegDb"
			|| $project eq "Tools/RegMove"
			|| $project eq "Tools/RegPriv"
			|| $project eq "Tools/ProductView"
			|| $project eq "Tools/ProductVersion"
			|| $project eq "Tools/UpdateHandler"
			|| $project eq "Tools/WkTranslator"
			|| $project eq "Tools/SecAnalyzer"
			|| $project eq "Tools/LogZip"
			
			|| $project eq "Clients/SDIConfig"
			|| $project eq "Clients/Recherche"
			|| $project eq "Clients/Vision"
			) {
		# nur release
		@cfgs=("Release");
		$bValidProject=1;
	} else {
		$bValidProject=0;
		print "Unknown project \"$project\"\n";
	}

	# a valid project ? Then loop over the given configurations.
	if ($bValidProject) 
	{
		if ($bInFinalCopy==0) 
		{ # it´s not copy; do the nmake
			if ($bNoCompile==0) 
			{	# some projects do not need to be compiled (DDK stuff etc.)
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
		print ("Invalid project:$project");
	}

	if ($bWorkAsSuperCopy==0)
	{
		if (grep (/^$project$/,@DocProjects)) 
		{
			`v:\\Dokumentation\\AutoDuck\\makedocs.bat $dest`;
		}
		else 
		{
			# print  ("No Autoduck for $project\n");
		}
	}
}



sub TrySourceSafe
{
	# NOT YET if there is no .map kill the .exe, else the map is not build
	local ($cwd)=getcwd;

    $proj=getcwd;
    $proj =~ s/[A-Za-z]:/\$/;
	$proj =~ s/\\/\//g;

	local($tmpProj)=$proj;

	# try brach project first
	$proj =~ s/Project/BranchProject_3.6/;

	$_=`$SAFE_CMD Workfold $proj $cwd`;
	if ( !/has been established as the working/ ) {
		print $_;
		$proj=$tmpProj;	# back to old project name, the non-branch
		local($tmpVersionLabel)="Build_150_19.8.98,11:44";

		print ("Using version labled $tmpVersionLabel for $proj\n");
		print `$SAFE_CMD Workfold $proj $cwd`;
		# using -Y to deal with pemanent deleted files
		# the question was "Continue anyway?"
		print `$SAFE_CMD  Get \"$proj\" -R -I-Y \"-VL$tmpVersionLabel\"`;
	} else {
		print ("Found brach project for $proj\n");
		print `$SAFE_CMD  Get \"$proj\" -R -I-"`;
	}
	# -R recusive
	# -I no input
	# -VL version by label

}

sub CreateNewDir
{
	local($pname)=@_;
	# print "CreateSubDirs $pname\n";
	local(@subdirs) = split("/",$pname);
	foreach (@subdirs) {
		if (! -d $_) {	# subdirectories can exists like Units/SimUnits
			printf ("Create $_ in %s\n",getcwd);
			system("mkdir $_");
		}
		if (-d $_) {
			chdir($_);
		} else {
			print ("Failed $_\n");
		}
	}
}

sub DoCopy
{
	local($project,$dest,$cfg)=@_;

	if ($dest eq "CDRWriter") {
		$dest="DBBackUp";
	} elsif ($dest eq "ISDNUnit" && $cfg eq "PTUnitRelease") {
		$dest="PTUnit";
	}


	# make the destination dependend from the configuration
	$_ = $cfg;
	local($destinationDir)=$theDestinationDir;

	# create non existing dir
	if (! -d "$destinationDir") {
		`mkdir $destinationDir`;
	}

	local ($bDoCopy)=1;

	$_ = $project;
	if (/oemgui/ || 
		/PTDecoder/ || 
		/h263dec/|| 
		/client40/
		) {
		$source = "$theProjectDir\\Lib\\$dest.dll";
		$finalDest = "$theDestinationDir\\Dlls\\$dest.dll";
	} elsif (/MiCoDA/ || /MegraV2/ ) {
		$source = "$project\\Release\\$dest.dll";
		$finalDest = "$theDestinationDir\\Dlls\\$dest.dll";
	} elsif (/MiCoVxd/) {
		$source = "$project\\bin\\micopci.vxd";
		$finalDest = "$theDestinationDir\\micopci.vxd";
	} elsif (/MiCoSys/) {
		$source = "$project\\bin\\MiCoPCI.sys";
		$finalDest = "$theDestinationDir\\MiCoPCI.sys";
	} elsif (/AkuSys/) {
		$source = "$project\\bin\\Aku.sys";
		$finalDest = "$theDestinationDir\\Aku.sys";
	} elsif (/Convert/) {
		$source = "$project\\Release\\Convrt32.dll";
		$finalDest = "$theDestinationDir\\Dlls\\Convrt32.dll";
	} elsif (/Clients\/Vision/) {
		$source = "$project\\$cfg\\Vision.exe";
		$finalDest = "$theDestinationDir\\Vision40.exe";
	} elsif (/SecurityExplorer/) {
		$source = "$project\\$cfg\\Vision.exe";
		$finalDest = "$theDestinationDir\\Vision.exe";
	} elsif (/Cipc/ || /WKClasses/ || /jpeg/ || /DVRCore/) {
		$bDoCopy=0;
	} else {
		#
		# .exe 
		#
		$source = "$project\\$cfg\\$dest.exe";
		$finalDest = "$theDestinationDir\\$dest.exe";
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
			/h263dec/|| 
			/client40/ ) {
		# .map are not in Lib/ but in their Release/ directories
			$source = "$project\\$cfg\\$dest.map";
		} elsif ( /MiCoSys/ || /MiCoVxd/ || /AkuSys/ ) {
			$source = "NO COPY"; # no map file
		}

		$finalDest = "$theDestinationDir\\maps\\$dest.map";

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

sub CollectBuildNotes
{
	open (OFILE,">>V:\\bin\\BuildNotes.htm");
	print ("BuildNumber $buildNumber at $mday.$month.$year, $hour:$min\n");
	# <HR></PRE><H3>BuildNumber 28 at 9.12.97, 14:22</H3>
	print (OFILE "<HR><H3>BuildNumber $buildNumber at $mday.$month.$year, $hour:$min</H3>\n");
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

	for (@allProjects) {
		local($project);
		$project=$_;
		chdir("/Project/$project");
		if ($bTraceSuperHistory) {
			printf ("Searching history in %s...\n",getcwd());
		}

		if (-f "history.txt") {
			# fine
			`V:\\perl5\\bin\\superHistory.bat $project ^ history.txt`;
			# now superHistory.txt is created
			# collect between the topmost Label:"Build
			# from this take only Chg: To : Ins:
			local($bAlreadyDidPrint)=0;
			local($bInTop)=1;
			local($iLabelCount)=0;
			local($bAlreadyDidPrintText)=0;
			open(HFILE,"SuperHistory.txt") || warn "can't open SuperHistory.txt\n";
			if ($bTraceSuperHistory) {
				printf ("Scanning SuperHistory.txt in %s...\n",getcwd());
			}
			while (<HFILE>) {
				if ($bInTop) {
					if (/^Label:"Build/) {
						$iLabelCount++;
						# here is the number of BuildLabels to use for BuildNotes.htm
						if ($iLabelCount>1) {
							$bInTop=0;
						}
					} elsif ( /Ins:/ || /Chg:/ || /To :/ ){
						# inTop and important stuff
						if ($bAlreadyDidPrint==0) {
							# print 'file header'
							print ("----- $project/history.txt -----\n");
							print (OFILE "$project/history.txt\n<PRE>");
							$bAlreadyDidPrint=1;
							$bAlreadyDidPrintText=0;
						}
						# convert Ins: to whitespace
						$_ =~ s/ Ins:/    /;
						# escape HTML specials
						$_ =~ s/</&lt\;/g;
						$_ =~ s/>/&gt\;/g;

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
							$bAlreayDidPrintText=1;
						}
					} else {
						# in top but unimportant
						if ($bTraceSuperHistory) {
							print ("IgnoreInTop:$_");
						}
					}
				} else {
					# not in top forget about the rest
					if ($bTraceSuperHistory) {
						print ("Ignore:$_");
					}
				}
			}
			if ($bAlreadyDidPrint) {
				print (OFILE "</PRE>\n");
			}
			close(HFILE);
			unlink("SuperHistory.txt");
			unlink("TmpHistory.txt");
		} else {
			print ("$project has no history.txt\n");
		}
	}

	close (OFILE);

}

close(STDERR);
close(STDOUT);


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
		$foo = 0;
		$copyError = `$cmd`;

		$_ = $copyError;
		if (/0 Datei/) {
			$copyError =~ s/\s+0 Datei.*\n//;
			print ("$copyError");
			if ($bDoCountError) {
				$theCopyErrorCount++;
			} else {
				print ("Error not counted \n");
			}
		}

	} else {
		print "File not found, $source\n";
		if ($bDoCountError) {
			$theCopyErrorCount++;
		} else {
			print ("Error not counted \n");
		}
	}
}

sub ModifyMakeFile
{
	local($makeFile)=@_;
	local ($NewMakeFile);
	local ($oneLine);

	if (-f $makeFile) {
		$NewMakeFile = "New$makeFile\n";

		# V:\Project --> D:\Project
	
		&MyUnlink($NewMakeFile,0);

		# read $makeFile and write to $NewMakeFile
		open(INFILE,"$makeFile") || warn "can't open $makeFile\n";
		open(OUTFILE,">$NewMakeFile") || warn "can't open $NewMakeFILE\n";
		
		while (<INFILE>) {
			
			$oneLine  = $_;

			# is there inetsdk/file.h
			# that's a fine pattern using () to get the filename only in $2
			if (/(inetsdk\\include\\)(.*\.h)"/) {
				$hFile = $2;
				if (-f "$MSDEV_DIR/Include/$hFile") {
					print ("Builtin inetsdk for $hFile\n");
					$oneLine =~ s/inetsdk\\include/\{\$\(INCLUDE\)\}/;
				}
			}

			if (/[EeFfVv]:\\[pP]roject/) {
				$oneLine =~ s/[VvEeFf]:\\[pP]roject/D:\\Project/m;
			} elsif (/\.\.\\\.\.\\\.\.\\devstudio\\vc\\mfc/) {
				$oneLine =~ s/\.\.\\\.\.\\\.\.\\devstudio\\vc\\mfc/C:\..\..\..MsDev\vc\mfc/;
			} elsif (/[cCdDeEfF]:\\vc\\mfc\\src\\/) {					  
				# mfc/src is already in the include path
				$oneLine =~ s/[cCdDeEfF]:\\vc\\mfc\\src\\/$MSDEV_DIR\\mfc\\src\\/;
			}
		
			print (OUTFILE $oneLine);
		
		} # EOF makeFile

		close(OUTFILE);
		close(INFILE);
	
	} else {
		print "Makefile `$makeFile` not found\n";
	}
}
__END__
:endofperl
