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
$bTraceTesting=0;
$bTraceChangeDir=0;
$bTraceCreateNewDir=0;
$bTraceOneConfig=0;
$bTraceCopy=0;
$bTraceUnlink=0;
$bTraceSuperHistory=0;
$bTraceSourceSafe=0;
$bGetLibs=1;			# disable for faster testing
$bPrintDSPConversion=0; # default 0; print local dsp conversion

#############
# <GLOBALS> #
#############
$WorkAs = "SuperMake";
$bInFinalCopy = 0;
$theCopyErrorCount=0;	# modified in MyDoCopy
$errorCount=0;
$warningCount=0;

$theDestinationDir="v:/bin/Akubis";			# has to contain vinfo.pvi for the buildNumber info
$theDestinationDirEnu="v:/bin/AkubisEnu";	# has to contain vinfo.pvi for the buildNumber info

$VersionLabel="";		#	leer fuer aktuelle Version <<<
#							oder mit VersionsLabel passend zum SourceSafe $/Project label
#							$VersionLabel="Release3.5_1309_2100";

$buildNumber=0;			# read from vinfo.pvi, but used in multiple functions
$currentBuild=1;		# updated in ReadBuildNumber

# >>> do not remove or modify this line, copy and uncomment <<<
#$versionInfo="5.1.";	# for build 1...
$VersionMajor=5;
$VersionMinor=5;
$VersionSubIndex=0;
$versionInfo="$VersionMajor.$VersionMinor.$VersionSubIndex.";	# since build 16...

$totalLines=0;
$statText = "Statistic:\n";

$WISE_CMD="d:\\apps\\wise\\wise32.exe";

##############
# </GLOBALS> #
##########+###

# set some global path variables
$ZIP_CMD="v:\\bin\\tools\\zip\\pkzip25.exe";
if (! -f "$ZIP_CMD")
{
	die "pkzip.exe not found $ZIP_CMD";
}

# argument check:
# -superCopy	work as copy only, assumes successful superMake
# -superHistory	work as History only, collect build notes
# -ftpUpdate	only ftUpdate buildNotes.htm
# no args, plain superMake
if ($#ARGV==0 && $ARGV[0] eq "-ftpUpdate")	# last index, not count
{
#	&FtpUpdate();
#	die "FTP done.";
}

if ($#ARGV==0 && $ARGV[0] eq "-superCopy")	# last index, not count
{
	$WorkAs = "SuperCopy";
}

if ($#ARGV==0 && $ARGV[0] eq "-superHistory")	# last index, not count
{
	$WorkAs = "SuperHistory";
}

# dependencie projects are not compiled
@DepProjects=(
				"Project/include",
				"Project/Lib/luca",
				"Project/Tools/LogzipLib",
				);

# LibProjects are compiled first, that is before the normal Projects
@LibProjects=(
				"Project/Cipc",
				"Project/Lib/WKClasses",
				"Project/Lib/oemgui",
				"Project/Lib/Network",
				);

# project which have AutoDuck comments
@DocProjects = (
#				"Project/Cipc",
#				"Project/Lib/WKClasses",
				);

# Liste aller relevanten Projekte
# CAVEAT muss auch weiter unten angepasst werden
# siehe CAVEAT2
# NEW PROJECT das WorkingDirectory sollte auf dem Rechner auf
#             "theProjectDrive\NewProject" gesetzt werden
# NEW PROJECT add here
@Projects=(	 
			"Telekom/MasterControl",
#			"Telekom/Evaluation",
			"Project/Tools/LogView",
			);

&SetTimeVariables();
&ReadBuildNumber();

print "Script started at $mday.$month.$year, $hour:$min, version was '$VersionLabel', next build is $currentBuild\n";

# force "flush output"
$|=1;

select(STDERR); $|=1;
select(STDOUT); $|=1;

$theProjectDrive="D:";
$theCurrentProjectDir="UNKNOWN";


print ("Using $theProjectDrive\n");
chdir("$theProjectDrive\\") || die "chdir '$theProjectDrive' failed\n";

&SearchSS();
&SearchMSDEV();
&SetEnv();
&SetTimeVariables();

if ($WorkAs eq "SuperMake")
{
	print "AkubisMake started at $mday.$month.$year, $hour:$min, version was '$VersionLabel', compiling for build $currentBuild\n";
	if ($bGetLibs)
	{
		&CreateNewDir("$theProjectDrive/Project/Lib");
		print `$SAFE_CMD  Get \"\\$\\project\\lib\\luca.lib" -I- \"-VL$VersionLabel\"`;
		print `$SAFE_CMD  Get \"\\$\\project\\lib\\cppclasses.lib" -I- \"-VL$VersionLabel\"`;
		#
		# get latest version from source safe
		#
		for (@DepProjects)
		{
			local($project) = $_;
			&ChangeDir("$theProjectDrive");	# back to project root dir
			&CreateNewDir($project);
			&TrySourceSafe;
		}
	}
	else
	{
		print ">>> Not getting DepProjects! <<<\n";
	}

	# loop over all LibProjects
	for (@LibProjects)
	{
		local($project) = $_;
		&ChangeDir("$theProjectDrive");	# back to project root dir
		&CreateNewDir($project);

		# remove all lib versions
		$_ = $project;
		if (/Cipc/)
		{
			&MyUnlink("Release/CIPC.lib",0);
			&MyUnlink("Release/CIPC.dll",0);
		}
		else
		{
			&MyUnlink("$theProjectDrive/$project.lib",0);
			&MyUnlink("$theProjectDrive/$project.dll",0);
#			print "else MyUnlink $project\n";
#			die;
		}

		# get the latest/labeled version from source safe
		&TrySourceSafe;

		# remove not writable libs before compiling
		$theCurrentProjectDir="$theProjectDrive/$project";
		&ChangeDir($theCurrentProjectDir);
		$_ = $project;
		if (/Cipc/)
		{
			&MyUnlink("Release/CIPC.lib",1);
			&MyUnlink("Release/CIPC.dll",1);
		}
		
		&DoConfigurations($project);
	}	# end of loop over all LibProjects

	#
	# the main loop over all Projects
	#
	for (@Projects)
	{
		local($project) = $_;
		if ($bTraceTesting)
		{
			print("Actual Project is $project\n");
		}
		&ChangeDir("$theProjectDrive");	# back to project root dir
		&CreateNewDir($project);
		if ($bTraceTesting)
		{
			print("Actual Project is $project after CreateNewProject\n");
		}

		&TrySourceSafe;
		if ($bTraceTesting)
		{
			print("Actual Project is $project after SourceSafe\n");
		}

		$theCurrentProjectDir="$theProjectDrive/$project";
		&ChangeDir($theCurrentProjectDir);
		&DoConfigurations($project);
	}

	# print some statistics
	print ("$statText");
	print ("Lines in total: $totalLines\n");
	&SetTimeVariables();
	print "AkubisMake DONE at $mday.$month.$year, $hour:$min, version was '$versionInfo', compiling build $currentBuild\n";
	if ($errorCount || $warningCount)
	{
		print ("Es gab $errorCount error[s], $warningCount warning[s]\n" );
	}
	else
	{
		print "Es gab keine Fehler oder Warnings.\n";
	}
	print "Net Send Georg \n";
	`Net Send Georg "AkubisMake DONE at $mday.$month.$year,  $hour:$min,  version $versionInfo build $currentBuild  $errorCount error[s], $warningCount warning[s]"`;
}
elsif ($WorkAs eq "SuperCopy")
{
	print "AkubisCopy started at $mday.$month.$year, $hour:$min, version was '$VersionLabel', copying for build $currentBuild\n";
	# switch to copy mode
	$bInFinalCopy = 1;	

	# create required directories
	&CreateNewDir("$theDestinationDir/Dlls");
	&CreateNewDir("$theDestinationDir/Maps");
#	&CreateNewDir("$theDestinationDirEnu/Dlls");
#	&CreateNewDir("$theDestinationDirEnu/Maps");
	&CreateNewDir("v:/BuildZipsTelekom");

	#copy some dll's which are needed
	&MyDoCopy("V:/Project/Lib/luca.dll", "$theDestinationDir/Dlls/luca.dll",1);	# do count errors

	# and again loop over all projects, depending on $bInFinalCopy
	# now a copy is made, not the nmake call

	for (@LibProjects)
	{
		local($project);
		$project=$_;

		chdir("$theProjectDrive");
		&DoConfigurations($project);
	}	# end of loop over lib projects
	
	for (@Projects)
	{
		local($project);
		$project=$_;

		chdir("$theProjectDrive");
		&DoConfigurations($project);

	}	# end of loop over projects
  
	if ($theCopyErrorCount!=0)
	{
		print ("\n");
		print ("Es gab $theCopyErrorCount Fehler beim Kopieren\n");
		print ("BuildNummer wird nicht erhoeht\n");
		print ("\n");
	}
	else
	{
		print ("\n");
		print ("Es gab keine Fehler beim Kopieren.\n");
		print ("\n");
		&UpdateBuild();
	}
	&SetTimeVariables();
	print "AkubisCopy DONE at $mday.$month.$year, $hour:$min, version was '$versionInfo$currentBuild'\n";
	print "Net Send Georg \n";
	`Net Send Georg "AkubisCopy DONE at $mday.$month.$year,  $hour:$min,  version $versionInfo build $currentBuild  $theCopyErrorCount error[s], $warningCount warning[s]"`;
}
elsif ($WorkAs eq "SuperHistory")
{
	print "AkubisHistory started at $mday.$month.$year, $hour:$min, version was '$VersionLabel', copying for build $currentBuild\n";
	&CollectBuildNotes();
#   &FtpUpdate();
	&SetTimeVariables();
	print "SuperHistory DONE at $mday.$month.$year, $hour:$min, version was '$versionInfo$currentBuild'\n";
}

#
# end of 'main'
#

###################################################################################################
sub UpdateBuild
{
	# update the BuildNumber
	print ("Updating $theDestinationDir/vinfo.pvi\n");
	`v:\\bin\\dvrt\\ProductVersion -updateDirectory $theDestinationDir`;

	# delete previous pvi files
	chdir($theDestinationDir);	
	system("del vinfo*_*.pvi");

	# set a version label in the SourceSafe
	$LabelText = "$versionInfo$currentBuild";
	print "Setting SourceSafeLabel: $LabelText\n";
	`$SAFE_CMD  Label \$/Telekom -C- -L$LabelText`;

	# create the setup files
	# create required directories
	&CreateNewDir("$theDestinationDir/Installation");
	&CreateNewDir("$theProjectDrive/Telekom/Installation");
#	print `$SAFE_CMD  Get \"\\$\\Telekom\\Installation\\Evaluation.wse" -I- \"-VL$VersionLabel\"`;
#	`$WISE_CMD /c \"$theProjectDrive\\Telekom\\Installation\\Evaluation.wse\" `;
	print `$SAFE_CMD  Get \"\\$\\Telekom\\Installation\\MasterControl.wse" -I- \"-VL$VersionLabel\"`;
	`$WISE_CMD /c \"$theProjectDrive\\Telekom\\Installation\\MasterControl.wse\" `;

	# create the build zips
	# create required directories
	&CreateNewDir("v:/BuildZipsTelekom");

	$BuildZipName = "Build\_$VersionMajor\_$VersionMinor\_$VersionSubIndex\_$currentBuild.zip";
	print "Creating v:\\BuildZipsTelekom\\$BuildZipName\n";
	print "executing $ZIP_CMD -add -recurse -directories v:\\BuildZipsTelekom\\$BuildZipName $theDestinationDir\\*.*\n";
	`$ZIP_CMD -add -recurse -directories v:\\BuildZipsTelekom\\$BuildZipName $theDestinationDir\\*.*`;

#	$BuildZipName = "EnuBuild_$VersionMajor_$VersionMinor_$VersionSubIndex_$currentBuild.zip";
#	print "Creating v:\\BuildZipsTelekom\\$BuildZipName\n";
#	print "executing $ZIP_CMD -add -recurse -directories v:\\BuildZipsTelekom\\$BuildZipName $theDestinationDirEnu\\*.*\n";
#	`$ZIP_CMD -add -recurse -directories v:\\BuildZipsTelekom\\$BuildZipName $theDestinationDirEnu\\*.*`;

#	print "Creating v:\\BuildZipsTelekom\\Enu$currentBuild.zip\n";
#	print "executing $ZIP_CMD -add -recurse -directories v:\\BuildZipsTelekom\\Enu$currentBuild.zip $theDestinationDirEnu\\*.*\n";
#	`$ZIP_CMD -add -recurse -directories v:\\BuildZipsTelekom\\Enu$currentBuild.zip $theDestinationDirEnu\\*.*`;

	chdir($theProjectDrive);	

	&CollectBuildNotes();
#	&FtpUpdate();
}
###################################################################################################
sub FtpUpdate
{
	# automagically update BuildNotes.htm on the IntraNet server
	print ("Updating webdoc/BuildNotes.htm...");

	chdir ("V:\\bin");
	open (OFILE,">D:\\ftptmp.inp");
	print (OFILE "ftpsofti\n");
	print (OFILE "ftppush\n");
	print (OFILE "cd webdoc\n");
	print (OFILE "put BuildNotes.htm\n");
	print (OFILE "quit\n");
	close(OFILE);

	`ftp -s:D:\\ftptmp.inp 192.168.0.81`; #just execute, no print
	
	unlink("d:\\ftptmp.inp");
	print ("ftpPush done.\n");
}
###################################################################################################
sub DoConfigurations
{ 
	local($project)=@_;
	
	local(@cfgs);
	local($mak)="foo.mak";
	local($dest)="foo";

	print("DO \"$project\"\n");
		
	# CAVEAT2 jedes Project muss hier eingetragen sein!
	# special settings per project
#	if ($project eq "Telekom/MasterControl_5_2")
#	{
#		# e.g. makefile has different name than project
#		$mak = "MasterControl\.mak";
#		$dest = "MasterControl";
#	}
#	else
	{
		# set default values
		$mak = "$project\.mak";
		$dest = "$project";

		#
		# CAVEAT order is important
		#

		$mak =~ s/^Project\///;		# drop leading Project/
		$dest =~ s/^Project\///;	# drop leading Project/

		$mak =~ s/^Telekom\///;		# drop leading Telekom/
		$dest =~ s/^Telekom\///;	# drop leading Telekom/

		$mak =~ s/^Tools\///;		# drop leading Tools/
		$dest =~ s/^Tools\///;		# drop leading Tools/

		$mak =~ s/^Lib\///;			# drop leading Lib/
		$dest =~ s/^Lib\///;		# drop leading Lib/

		# NEW PROJECT add here, if on 'new' subdirectory
	}

	local($bValidProject)=0;
	local ($bNoCompile)=0;

	# 
	if ($project eq "Project/Tools/LogView"
			)
	{
		# Akubis Configuration
		@cfgs=("Akubis");
		$bValidProject=1;
	}
	elsif (    $project eq "Project/Cipc"
			|| $project eq "Project/Lib/WKClasses"
			|| $project eq "Project/Lib/oemgui"
			|| $project eq "Project/Lib/Network"
			|| $project eq "Telekom/MasterControl"
			|| $project eq "Telekom/Evaluation"
			)
	{
		# nur release
		@cfgs=("Release");
		$bValidProject=1;
	}
	else
	{
		$bValidProject=0;
		print " Error Unknown project \"$project\"\n";
	}

	# a valid project ? Then loop over the given configurations.
	if ($bValidProject) 
	{
		if ($bInFinalCopy==0) 
		{	# it´s not copy; do the nmake
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
		{	# do not do the make, but do the copy
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
		print (" Error Invalid project:$project\n");
	}

	if ($bWorkAsSuperCopy==0)
	{
		# NOT YET check for AutoDuck directory ?
		if (grep (/^$project$/,@DocProjects)) # is it a AutoDuck project
		{
			@makeDocsLog = `v:\\Dokumentation\\AutoDuck\\makedocs.bat $dest`;
			for ($mi=0;$mi <= $#makeDocsLog;$mi++) {
				$_=$makeDocsLog[$mi];
				if ( /makedocs DONE/)
				{
					# supress
				}
				else
				{
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
###################################################################################################
sub DoOneConfiguration
{
	local($mak,$dest,$cfg)=@_;
	local($oneLine)="";
	local($ei)=0;

	# print " .... $mak CFG=\"$dest - Win32 $cfg\"\n";

	# perform static analysis of sourceCode
	local($checkCmd)="v:\\perl5\\bin\\checkBreak.bat "; # getcwd is appended
	$checkCmd .= getcwd;
	@checkLog = `$checkCmd`;

	# collect total line stats
	for ($ci=0;$ci <= $#checkLog;$ci++)
	{
		local($tmpJunk)="";
		local($restJunk)="";
		$firstLine=$checkLog[$ci];	# get first line
		$_ = $firstLine;
		if (/#STAT#/)
		{
			$statText .= $firstLine;
			$firstLine =~ s/\s+/ /gm;
			$firstLine =~ s/^\s+//gm;
			# looks like "#STAT# 8191 lines in d:\Project\SecurityLauncher"
			($tmpJunk,$lines,$restJunk) = split(/ /,$firstLine,3);
			$totalLines += $lines;
		}
		elsif ( /nicht/ )
		{
			# ignore
		}
		else
		{
			print ("$firstLine");
		}
	}
	
	# /Q check only
	# /S suppress executed command
	# always kill the destination
	local($destDir);
	$destDir="Release";
	
	# new UF with absolute path
	# and for dll's with lib path
	# do not remove to avoid timestamp differences &MyUnlink("$theProjectDrive/lib/$dest.dll",0);
	# do not remove to avoid timestamp differences &MyUnlink("$theProjectDrive/$project/$destDir/$dest.exe",0);
	# new style call msdev, avoiding the .dep files in each project
	local($dspName)="";
	$dspName  = $mak;
	$dspName =~ s/\.mak/.dsp/;

	&UpdateVersion();

	chmod 0755, "$dspName";	# allow for local conversion
	print "compiling $dest - Win32 $cfg\n";
	@errorLog = `msdev.exe $dspName /MAKE \"$dest - Win32 $cfg\" /USEENV `;
	chmod 0555, "$dspName";	# undo the above operation, make it readonly again

	if ($#errorLog>0)
	{
		for ($ei=0;$ei <= $#errorLog;$ei++)
		{
			$oneLine=$errorLog[$ei];

			# print ("MAK:$oneLine\n");
			# modify filenames, to allow easy F4 stepping
			$_ = $oneLine;	# OOPS
			if (   /kopiert/
				|| /copied/ 
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
				)
			{
				# do not print		
				$oneLine = "";
			}
			elsif (    ($bPrintDSPConversion == 0)
					&& (/by a previous/ || /your original project/ )
					)
			{
				# do not print		
				$oneLine = "";
			}
			$_ = $oneLine;

			
			# count make errors and warnings
			if (/ [eE]rror /)
			{
				$errorCount++;
			}
			if (/ [wW]arning /)
			{
				$warningCount++;
			}

			print ("$oneLine");	# already has newline

		}
	}
	else
	{
		print "Done  $mak CFG=\"$dest - Win32 $cfg\"\n";
	}
}
###################################################################################################
sub TrySourceSafe
{
	local($project) = getcwd;
	$project =~ s/[A-Za-z]:/\$/;

	if ($bTraceSourceSafe)
	{
		print("Try SourceSafe for $project\n");
	}

	if (-d "res")
	{
		# delete modified rc2
	}

	# -R recusive
	# -I no input
	# -VL version by label
	if ($VersionLabel ne "")
	{
		`$SAFE_CMD  Get \"$project\" -R -I- -O\@vsstmp.txt \"-VL$VersionLabel\"`;
	}
	else
	{
		`$SAFE_CMD  Get \"$project\" -R -I- -O\@vsstmp.txt`;
	}
	if ($bTraceSourceSafe)
	{
		print("End SourceSafe for $project\n");
	}
	unlink("vsstmp.txt");
}
###################################################################################################
sub CreateNewDir
{
	local($pname)=@_;
	if ($bTraceCreateNewDir)
	{
		print "CreateSubDirs $pname\n";
	}

#	local($curDir) = getcwd;	# save previous dir 

	local(@subdirs) = split("/",$pname);
	foreach (@subdirs)
	{
		if ($bTraceCreateNewDir)
		{
			printf ("Next Dir is $_ ; %s\n", getcwd);
		}
		# if subdir does not exist and is not the root
		if (! -d $_ && !/:/)
		{
			printf ("Create $_ in %s\n", getcwd);
			system("mkdir $_");
		}
		
		if (-d $_ || !/:/ )	# if subdir exists and is not the root
		{
			chdir($_);
			if ($bTraceCreateNewDir)
			{
				printf ("ChangeDir to $_ ; %s\n", getcwd);
			}
		}
		elsif (/:/)	# if subdir is the root
		{
			chdir ("$_\\");	# absolute path, back to root dir
			if ($bTraceCreateNewDir)
			{
				printf ("ChangeDrive to $_ ; %s\n", getcwd);
			}
		}
		else
		{
			print ("Failed $_\n");
		}
	}
}
###################################################################################################
sub ChangeDir
{
	local($pname)=@_;
	if ($bTraceChangeDir)
	{
		print("ChangeDir $pname\n");
	}
	local(@subdirs) = split("/",$pname);
	foreach (@subdirs)
	{
		# special check for D:/
		if (-d $_ && !/:/ )	# subdirectories can exists like Units/SimUnits
		{
			chdir($_);
			if ($bTraceChangeDir)
			{
				print("ChangeDir $_\n");
			}
		}
		elsif (/:/)
		{
			chdir ("$_\\");	# absolute path, back to root dir
			if ($bTraceChangeDir)
			{
				print("ChangeDir $_\\\n");
			}
		}
		else
		{
			print ("Failed ChangeDir $_\n");
		}
	}
	printf("CurrentDir %s\n", getcwd);
}
###################################################################################################
sub DoCopy
{
	local($project,$dest,$config)=@_;

	# make the destination dependend from the configuration
	$_ = $config;
	local($destinationDir)=$theDestinationDir;
	if ($_ eq "Enu")
	{
	  $destinationDir = $theDestinationDirEnu;
	}
	else
	{
	  $destinationDir = $theDestinationDir;
	}

	# create non existing dir
	if (! -d "$destinationDir")
	{
		&CreateNewDir($destinationDir);
	}

	local ($bDoCopy)=1;
	local ($bDoCopyHelp)=1;

	$_ = $project;

	print "COPYING $_\n";
	local($source)="";
	local($finalDest)="";
	local($finalDestDir)="";

	if (   /oemgui/
		|| /Network/
		)
	{
		$bDoCopyHelp=0;
		if ($config eq "Enu")
		{
			$source = "$theProjectDrive\\Project\\Lib\\$config\\$dest.dll";
		}
		else
		{
			$source = "$theProjectDrive\\Project\\Lib\\$dest.dll";
		}
		$finalDestDir = "$destinationDir\\Dlls";
		$finalDest = "$finalDestDir\\$dest.dll";
	}
	elsif (/Cipc/)
	{
		$bDoCopyHelp=0;
		$source = "$project\\release\\cipc.dll";
		$finalDestDir = "$destinationDir\\Dlls";
		$finalDest = "$finalDestDir\\cipc.dll";
	}
	elsif (/WKClasses/)
	{
		$bDoCopy=0;
	}
	elsif (/LogView/)
	{
		$bDoCopyHelp=0;
		$dest = "$dest\Akubis";
		$source = "$project\\$config\\$dest.exe";
		$finalDestDir = "$destinationDir";
		$finalDest = "$finalDestDir\\$dest.exe";
	}
	else
	{
		$source = "$project\\$config\\$dest.exe";
		$finalDestDir = "$destinationDir\\$dest";
		$finalDest = "$finalDestDir\\$dest.exe";
	}
	
	# create non existing dir
	if (! -d "$finalDestDir")
	{
		&CreateNewDir($finalDestDir);
	}
	# back to project root dir, because CreateNewDir changes the current working dir and drive
	&ChangeDir("$theProjectDrive");

	if ($bDoCopy)
	{
		# .exe or .dll first ########################################
		#
		&MyDoCopy($source,$finalDest,1);	# do count errors

		# save the original source and dest with extension .org
		local($sourceOriginal) = $source;
		local($finalDestOriginal) = $finalDest;
		$sourceOriginal =~ s/\.exe/.org/;
		$sourceOriginal =~ s/\.dll/.org/;
		$finalDestOriginal =~ s/\.exe/.org/;
		$finalDestOriginal =~ s/\.dll/.org/;

		# followed by .map ##########################################
		#
		$source = $sourceOriginal;
		$source =~ s/\.org/.map/;
		$finalDest = "$destinationDir\\Maps\\$dest.map";

		$_ = $project;
		if (   /oemgui/
			|| /Network/
			)
		{
			# .map are not in Lib/ but in their configuration directories
			$source = "$project\\$config\\$dest.map";
		}

		$_ = $source;
		if (/\.map/)
		{
			if (!(-f $source))
			{
				print "using pdb instead of map $source \n";
				$source =~ s/\.map/.pdb/;
				$finalDest =~ s/\.map/.pdb/;
			}
		}
		&MyDoCopy($source,$finalDest,1);

		# followed by .hlp ##########################################
		#
		if ($bDoCopyHelp)
		{
			$source = $sourceOriginal;
			$finalDest = $finalDestOriginal;
			$source =~ s/\.org/.hlp/;
			$finalDest =~ s/\.org/.hlp/;
			&MyDoCopy($source,$finalDest,1);

			# followed by .cnt ######################################
			#
			$source =~ s/\.hlp/.cnt/;
			$finalDest =~ s/\.hlp/.cnt/;
			&MyDoCopy($source,$finalDest,1);
		}
	} 
	else 
	{
		# do not copy, static libs for example
	}
}
###################################################################################################
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
	print ("Scanning for history.txt...\n");
	# add CIPC to history projects
	# NOT YET take all dep projects
	
	# collect all projects
	local(@allProjects);
	for (@LibProjects) {
		push(@allProjects,$_);
	}
	for (@Projects) {
		push(@allProjects,$_);
	}

	# loop over all projects
	for (@allProjects) {
		local($project);
		$project=$_;
		&ChangeDir("$theProjectDrive");	# back to project root dir
		ChangeDir($project);
		if ($bTraceSuperHistory) {
			printf ("Searching history for $project in %s...\n",getcwd());
		}
		if (-f "history.txt") {
			# NEW 16.10.98
			# use Diff instead of History
			# Diff allows to specify style and width -DS200
			# but it requires the previous label for -VL"Build_XXX..."
			# so if the label is just like Build_XXX it is simple
			$prevBuildNumber = $buildNumber-1;
			$prevLabelText = "$versionInfo$prevBuildNumber";
			if ($bTraceSuperHistory) {
				print ("Search for differences to $prevLabelText\n");
			}
			# -IECW ignore case/end of line/white space changes
			print `$SAFE_CMD Diff \$/$project/history.txt -DS200 -IECW -VL$prevLabelText > SuperHistory.txt`;
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
###################################################################################################
close(STDERR);
close(STDOUT);
###################################################################################################
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
###################################################################################################
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
				die;
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
###################################################################################################
sub UpdateVersion
{
	if (-d "res") {
		chdir("res");
		
		$rcFile="$dest.rc2";
		$_ = $rcFile;

		if (-f "$rcFile" ) {
# replaced by SourceSafe check out and in
# 			chmod 0755, $rcFile; 
			if (   $project eq "Telekom/MasterControl"
				|| $project eq "Telekom/Evaluation"
				)
			{
				if ($VersionLabel eq "")
				{
					# -I no input
					# -VL version by label
					`$SAFE_CMD  Checkout \"\$\\$project\\res\\$rcFile\" -I-`;
					print ("Checkout $project/res/$rcFile\n");
				}
			}
			print ("updating $rcFile\n");
			`v:\\perl5\\bin\\superReplace.bat -updateBuild "VIDETE_VERSION.*\@AUTO_UPDATE" "VIDETE_VERSION($VersionMajor, $VersionMinor, $VersionSubIndex, $currentBuild, \\"$versionInfo$currentBuild\\0\\", // \@AUTO_UPDATE"`;
			if (   $project eq "Telekom/MasterControl"
				|| $project eq "Telekom/Evaluation"
				)
			{
				if ($VersionLabel eq "")
				{
					`$SAFE_CMD  Checkin \"\$\\$project\\res\\$rcFile\" -I-`;
					print ("Checkin $project/res/$rcFile\n");
				}
			}
# replaced by SourceSafe check out and in
#			chmod 0555, $rcFile;
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
###################################################################################################
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
###################################################################################################
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
###################################################################################################
sub SearchMSDEV
{
	if ( -d "d:/msdev98/vc" ) { # CAVEAT order is important. special 6.0 hack
		$MSDEV_DIR="d:\\msdev98\\vc";
	} elsif ( -d "c:/msdev98/vc98" ) { # CAVEAT order is important. special 6.0 hack
		$MSDEV_DIR="c:\\msdev98\\vc98";
	} else {
		die "MsDev not found\n";
	}
	print "Found $MSDEV_DIR\n";
}	# end of SearchMSDEV
###################################################################################################
sub SearchSS
{
	if ( -f "C:/msdev98/common/vss/win32/ss.exe" ) {
		$SAFE_CMD="C:\\msdev98\\common\\vss\\win32\\ss.exe";
	} elsif ( -f "D:/msdev98/Common/Vss/Win32/ss.exe" ) {
		$SAFE_CMD="D:\\msdev98\\Common\\Vss\\Win32\\ss.exe";
	} else {
		die "SourceSafe not found\n";
	}
	print "SourceSafe is $SAFE_CMD\n";
}
###################################################################################################
sub SetEnv
{
	# set env variables, not using V:
	# CAVEAT requires $theProjectDrive
	# CAVEAT requires $theDestinationDir
	
	$TmpInclude = "$MSDEV_DIR\\Include;$MSDEV_DIR\\mfc\\include;$MSDEV_DIR\\atl\\include;$MSDEV_DIR\\mfc\\src;";
	$TmpInclude .= "$theProjectDrive\\Project\\Include;$theProjectDrive\\Project\\Cipc;$theProjectDrive\\Project\\Lib;";
	# print ("Include path is: $TmpInclude\n");

	$ENV{'INCLUDE'}=$TmpInclude;
	$ENV{'LIB'} = "d:\\inetsdk\\lib;$MSDEV_DIR\\lib;$MSDEV_DIR\\mfc\\lib;$theProjectDrive\\Project\\Lib;$theProjectDrive\\Project\\Cipc\\Release";
	
	# make sure ftp.exe is in the path
	if (-d "c:\\Msdev98")
	{
		$ENV{'PATH'} = "$MSDEV_DIR\\bin;C:\\Winnt;C:\\Winnt\\system32;c:\\MsDev98\\Common\\Msdev98\\bin;c:\\Msdev98\\Common\\Tools;$theDestinationDir\\Dlls";
	}
	elsif (-d "D:\\Msdev98")
	{
		$ENV{'PATH'} = "$MSDEV_DIR\\bin;C:\\Winnt;C:\\Winnt\\system32;D:\\MsDev98\\Common\\Msdev98\\bin;D:\\Msdev98\\Common\\Tools;$theDestinationDir\\Dlls";
	}
	
	print "path is $ENV{'PATH'}\n";
}
###################################################################################################
__END__
:endofperl
