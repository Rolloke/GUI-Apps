@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

#
# PERL PART FOLLOWS HERE
#

use Cwd;

#********************************************************
@ErrorRecipients=("127.0.0.1");

#********************************************************
# trace options
#********************************************************
$TraceDeleteEvenIfRO = 0;

#********************************************************
# <GLOBALS> 
#********************************************************
$sec=0;
$min=0;
$hour=0;
$mday=0;
$mon=0;
$year=0;

$bUseVS71        = 1;
$bCompileAllCfgs = 0;
$g_bGetDepend    = 1;	# disable for faster testing
$errorCount      = 0;
$warningCount    = 0;
$UnknownCount    = 0;
$CopyFailedCount = 0;
$bRebuildAll     = 0;
$bRebuildProj    = 0;	# rebuild automatic if dsp changed, see TrySourceSafe()

$theProjectDrive      = "D:";
$theProjectDir        = "$theProjectDrive/Project";
$theCurrentProjectDir = "UNKNOWN";

#********************************************************
$VersionLabel=""; # >>> leer fuer aktuelle Version <<<
# oder mit VersionsLabel passend zum SourceSafe $/Project label
# $VersionLabel="4.4.1.527";

#********************************************************
$totalLines=0;
$statText = "Statistic:\n";

#********************************************************
&SetTimeVariables();

#********************************************************
print "idip DLL-Make started at $mday.$month.$year, $hour:$min:$sec, version was '$VersionLabel'\n";

#**********************************************************
# handle command line parameters
for (@ARGV)
{
	if (/-allcfgs/i)
	{
		$bCompileAllCfgs = 1;
	}
	elsif (/-rebuild/i)
	{
		$bRebuildAll = 1;
	}
	elsif (/-vs60/i)
	{
		$bUseVS71 = 0;
	}
	elsif (/-vl:/i)
	{
		$VersionLabel = substr($_, 4);
	}
		elsif (/-ProjDrive:/i)
		{
			$DriveLetter = substr($_, 11, 1);
			$theProjectDrive = "$DriveLetter\:";
			if ($bTraceDebug) {print ("ProjectDrive is \"$theProjectDrive\"\n");}
			if (! -d "$g_theProjectDrive\\")
			{
				die "Project drive does not exist: $theProjectDrive\n";
			}
		}
	else
	{
		die "Unknown command line parameter: $_\n";
	}
}

#********************************************************
# Project and/or destination drive may be set by command line
$g_theProjectDir        = "$g_theProjectDrive\\Project";

#********************************************************
print ("Use Project dir $g_theProjectDir\n");
if ($bCompileAllCfgs)     { print ("Compiling all configurations\n"); }
if ($bRebuildAll)         { print ("Rebuild all\n"); }
if ($bUseVS71)            { print ("Using VisualStudio 7.1 (MSDEV 2003) in $theProjectDir\n"); }
else                      { print ("Using VisualStudio 6.0 in $theProjectDir\n"); }
if ($VersionLabel ne "")  {	print ("Version is \"$VersionLabel\"\n"); }

#********************************************************
# LibProjects
# CAVEAT1 jedes Project muss hier eingetragen sein!

@DllProjects=(
			  "Lib/WKClasses",		# already used in CIPC
			  "Tools/LogZipLib",	# used in CIPC
			  "Cipc",				# used in (nearly) all projects
			  "Lib/Skins",			# used in OemGui

			  "Capi/Capi4",
			  "DirectShow/Filters/baseclasses",			# a lib
			  "DirectShow/Common",						# dll
# gf obsolete			  "DirectShow/Filters/dump",				# ax
# gf obsolete			  "DirectShow/Filters/inftee",				# ax
# gf obsolete			  "DirectShow/Filters/mediasource",			# ax
# gf obsolete			  "DirectShow/Filters/PushSource",			# ax
# gf obsolete			  "DirectShow/Filters/Sample2MemRenderer",	# ax
# gf obsolete			  "DirectShow/Filters/WaveSplitter",		# ax

			  "Lib/codebase65",
			  "Lib/Crypto",
			  "Lib/DeviceDetect",
			  "Lib/HardwareInfo",
			  "Lib/ICPCONDll",
			  "Lib/JpegLib",					# gf todo 71
			  "Lib/NetComm",
			  "Lib/NFC",
			  "Lib/oemgui",
			  "Lib/VImage",
			  );

#********************************************************
# force "flush output"
$|=1;
select(STDERR); $|=1;
select(STDOUT); $|=1;

#********************************************************
# Create necessary dirs
print "\n########################################################\n";		# seperator
$theProjectDir="$theProjectDrive/Project";
&CreateNewDir($theProjectDir);
chdir("$theProjectDrive");

$theLibDir="$theProjectDir/Lib";
&CreateNewDir($theLibDir);

print ("Using $theProjectDir\n");
if (!chdir("$theProjectDir"))
{
	&NetSendError("chdir '$theProjectDir' failed");
	die "chdir '$theProjectDir' failed\n";
}

#********************************************************
print "\n########################################################\n";		# seperator
&SearchMSDEV();
&SearchSS();
#&SetEnv();

#********************************************************
# Get some dependency files from other projects <SIGH>
if ($g_bGetDepend)
{
	print "\n########################################################\n";		# seperator
	print ("Get some dependency files from other projects\n");

	&CreateNewDir("$theProjectDir/Include");
	print ("Getting Include ...\n");
	print `$SAFE_CMD Get \"\\$\\project\\Include" -I- \"-VL$VersionLabel\"`;

	&CreateNewDir("$theProjectDir/DirectShow/Lib");
	print ("Getting wmstub.lib ...\n");
	print `$SAFE_CMD Get \"\\$\\project\\DirectShow\\Lib\\wmstub.lib" -I- \"-VL$VersionLabel\"`;

	&CreateNewDir("$theProjectDir/Cipc");
	print ("Getting SecID.h ...\n");
	print `$SAFE_CMD Get \"\\$\\project\\Cipc\\SecID.h" -I- \"-VL$VersionLabel\"`;
	print ("Getting wk.h ...\n");
	print `$SAFE_CMD Get \"\\$\\project\\Cipc\\wk.h" -I- \"-VL$VersionLabel\"`;
	print ("Getting wk_profile.h ...\n");
	print `$SAFE_CMD Get \"\\$\\project\\Cipc\\wk_profile.h" -I- \"-VL$VersionLabel\"`;
	print ("Getting wk_trace.h ...\n");
	print `$SAFE_CMD Get \"\\$\\project\\Cipc\\wk_trace.h" -I- \"-VL$VersionLabel\"`;
	print ("Getting WK_ApplicationId.h ...\n");
	print `$SAFE_CMD Get \"\\$\\project\\Cipc\\WK_ApplicationId.h" -I- \"-VL$VersionLabel\"`;
	print ("Getting UnhandledException.h ...\n");
	print `$SAFE_CMD Get \"\\$\\project\\Cipc\\UnhandledException.h" -I- \"-VL$VersionLabel\"`;
	print ("Getting wk_util.h ...\n");
	print `$SAFE_CMD Get \"\\$\\project\\Cipc\\wk_util.h" -I- \"-VL$VersionLabel\"`;
	print ("Getting WK_Template.h ...\n");
	print `$SAFE_CMD Get \"\\$\\project\\Cipc\\WK_Template.h" -I- \"-VL$VersionLabel\"`;

	&CreateNewDir("$theProjectDir/Units/SaVic2/SaVicDll");
	print ("Getting SavicDefs.h ...\n");
	print `$SAFE_CMD Get \"\\$\\project\\Units\\SaVic2\\SaVicDll\\SavicDefs.h" -I- \"-VL$VersionLabel\"`;

	&CreateNewDir("$theProjectDir/Units/SaVic2/SaVicDA");
	print ("Getting SavicDirectAccess.h ...\n");
	print `$SAFE_CMD Get \"\\$\\project\\Units\\SaVic2\\SaVicDA\\SavicDirectAccess.h" -I- \"-VL$VersionLabel\"`;

	&CreateNewDir("$theProjectDir/Units/SaVic2/SaVicSys");
	print ("Getting [SavicSys]CJobQueue.h ...\n");
	print `$SAFE_CMD Get \"\\$\\project\\Units\\SaVic2\\SaVicSys\\CJobQueue.h" -I- \"-VL$VersionLabel\"`;

	&CreateNewDir("$theProjectDir/Lib");
	print ("Getting i7000.lib ...\n");
	print `$SAFE_CMD Get \"\\$\\project\\Lib\\i7000.lib" -I- \"-VL$VersionLabel\"`;
	print ("Getting ijl15.lib ...\n");
	print `$SAFE_CMD Get \"\\$\\project\\Lib\\ijl15.lib" -I- \"-VL$VersionLabel\"`;
	print ("Getting uart.lib ...\n");
	print `$SAFE_CMD Get \"\\$\\project\\Lib\\uart.lib" -I- \"-VL$VersionLabel\"`;
}

#********************************************************
# loop over all DllProjects
for (@DllProjects)
{
	print "\n########################################################\n";		# seperator
	local($project);
	$project=$_;

	chdir($theProjectDir);
	if (! -d $project)
	{
		&CreateNewDir($project);
		chdir($theProjectDir);
	}
	chdir($project);

	# get the latest/labeled version from source safe
	&TrySourceSafe("\$\\Project\\$project");
	$theCurrentProjectDir="$theProjectDir/$project";

	# remove not writable libs before compiling
	$_ = $project;
	# special cases not in LIB
	if (/Capi4/)
	{
		chdir($theCurrentProjectDir);
		if ($bUseVS71)
		{
			&DeleteEvenIfRO("DebugUnicode/Capi4_32_DebugU.lib",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("ReleaseUnicode/Capi4_32U.lib",$TraceDeleteEvenIfRO);
		}
		else
		{
			&DeleteEvenIfRO("Debug/Capi4_32_Debug.lib",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("Release/Capi4_32.lib",$TraceDeleteEvenIfRO);
		}
	}
	elsif (/Cipc/)
	{
		chdir($theCurrentProjectDir);
		if ($bUseVS71)
		{
			&DeleteEvenIfRO("DebugUnicode/CIPCDebugU.lib",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("DebugUnicode/CIPCDebugU.dll",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("ReleaseUnicode/CIPCU.lib",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("ReleaseUnicode/CIPCU.dll",$TraceDeleteEvenIfRO);
		}
		else
		{
			&DeleteEvenIfRO("Debug/CIPCDebug.lib",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("Debug/CIPCDebug.dll",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("Release/CIPC.lib",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("Release/CIPC.dll",$TraceDeleteEvenIfRO);
		}
	}
	elsif (/LogZipLib/)
	{
		chdir($theCurrentProjectDir);
		if ($bUseVS71)
		{
			&DeleteEvenIfRO("DebugUnicode/LogZipLibDebugU.lib",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("ReleaseUnicode/LogZipLibU.lib",$TraceDeleteEvenIfRO);
		}
		else
		{
			&DeleteEvenIfRO("Debug/LogZipLibDebug.lib",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("Release/LogZipLib.lib",$TraceDeleteEvenIfRO);
		}
	}
	elsif (/OemGui/)
	{
		# change to LIB dir
		chdir($theLibDir);
		$ProjectOnly = $project;
		$ProjectOnly =~ s/^Lib\///;	# drop leading Lib/
		$ProjectOnlyU = $ProjectOnly;
		$ProjectOnlyU .= "U";

		if ($bUseVS71)
		{
			&DeleteEvenIfRO("$ProjectOnlyU\.lib",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("$ProjectOnlyU\.dll",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("$ProjectOnly\Deu.dll",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("$ProjectOnly\Enu.dll",$TraceDeleteEvenIfRO);
			# Try to remove debug version
			&DeleteEvenIfRO("$ProjectOnly\DU.lib",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("$ProjectOnly\DU.dll",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("$ProjectOnly\DebugU.lib",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("$ProjectOnly\DebugU.dll",$TraceDeleteEvenIfRO);
		}
		else
		{
			&DeleteEvenIfRO("$ProjectOnly.lib",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("$ProjectOnly.dll",$TraceDeleteEvenIfRO);
			# Try to remove debug version
			&DeleteEvenIfRO("$ProjectOnly\D.lib",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("$ProjectOnly\D.dll",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("$ProjectOnly\Debug.lib",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("$ProjectOnly\Debug.dll",$TraceDeleteEvenIfRO);
		}

		$ProjectOnly = $project;
		# change back to project dir
		chdir($theCurrentProjectDir);
	}
	else
	{
		# Only projects in LIB

		# change to LIB dir
		chdir($theLibDir);
		$ProjectOnly = $project;
		$ProjectOnly =~ s/^Lib\///;	# drop leading Lib/
		$ProjectOnlyU = $ProjectOnly;
		$ProjectOnlyU .= "U";

		if ($bUseVS71)
		{
			&DeleteEvenIfRO("$ProjectOnlyU\.lib",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("$ProjectOnlyU\.dll",$TraceDeleteEvenIfRO);
			# Try to remove debug version
			&DeleteEvenIfRO("$ProjectOnly\DU.lib",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("$ProjectOnly\DU.dll",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("$ProjectOnly\DebugU.lib",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("$ProjectOnly\DebugU.dll",$TraceDeleteEvenIfRO);
		}
		else
		{
			&DeleteEvenIfRO("$ProjectOnly.lib",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("$ProjectOnly.dll",$TraceDeleteEvenIfRO);
			# Try to remove debug version
			&DeleteEvenIfRO("$ProjectOnly\D.lib",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("$ProjectOnly\D.dll",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("$ProjectOnly\Debug.lib",$TraceDeleteEvenIfRO);
			&DeleteEvenIfRO("$ProjectOnly\Debug.dll",$TraceDeleteEvenIfRO);
		}

		$ProjectOnly = $project;
		# change back to project dir
		chdir($theCurrentProjectDir);
	}
	
	&DoConfigurations($project);
}	# end of loop over all LibProjects

print "\n$statText";
print "Lines in total: $totalLines\n";

&SetTimeVariables();

$MakeDoneText = "idip DLL-Make DONE at $mday.$month.$year, $hour:$min:$sec";
print "\n$MakeDoneText\n";

$MakeErrorText = "OK";
if ($errorCount || $warningCount || $UnknownCount || $CopyFailedCount)
{
	$MakeErrorText = "$errorCount error[s], $warningCount warning[s], $UnknownCount Unknown projects, $CopyFailedCount copy failed";
	print "Es gab $MakeErrorText\n\n";
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

close(STDERR);
close(STDOUT);

######################################################################################################
# end of 'main'
#

#######################################################################
sub DoConfigurations
{ 
	local($project)=@_;
	
	local(@cfgs);
	local($projfile)="foo.dsp";
	local($dest)="foo";

	print("Build \"$project\"...\n");
		
	# CAVEAT2 jedes Project muss hier eingetragen sein!
	# special settings per project
	# set default values
	$projfile = "$project\.dsp";
	if ($bUseVS71)
	{
		$projfile = "$project\.vcproj";
	}
	$dest = "$project";

	#
	# CAVEAT order is important
	#

	if ($project eq "Capi/Capi4")
	{
		$projfile = "Capi4_32.dsp";
		if ($bUseVS71)
		{
			$projfile = "Capi4_32.vcproj";
		}
		$dest = "Capi4_32";
	}

	$projfile =~ s/^DirectShow\///;		# drop leading DirectShow/
	$dest =~ s/^DirectShow\///;			# drop leading DirectShow/

	$projfile =~ s/^Common\///;		# drop leading Common/
	$dest =~ s/^Common\///;			# drop leading Common/

	$projfile =~ s/^Filters\///;		# drop leading Filters/
	$dest =~ s/^Filters\///;			# drop leading Filters/

	$projfile =~ s/^DV\///;		# drop leading DV/
	$dest =~ s/^DV\///;			# drop leading DV/

	$projfile =~ s/^Lib\///;	# drop leading Lib/
	$dest =~ s/^Lib\///;		# drop leading Lib/

	$projfile =~ s/^Tools\///;	# drop leading Tools/
	$dest =~ s/^Tools\///;		# drop leading Tools/

	# NEW PROJECT add here, if on 'new' subdirectory

	# set the configurations which shall be compiled
	if (   $project =~ m/DirectShow/
		|| $project eq "Lib/codebase65"
		)
	{
		# Compile all or only Debug
		@cfgs = ("Debug");
		if ($bCompileAllCfgs)
		{
			push(@cfgs, "Release");
		}
	}
	else
	{
		# Compile all or only Debug
		if ($bUseVS71)
		{
			@cfgs = ("DebugUnicode");
			if ($project eq "Lib/oemgui")
			{
				push(@cfgs, "EnuUnicode");
			}
		}
		else
		{
			@cfgs = ("Debug");
		}
		if ($bCompileAllCfgs)
		{
			if ($bUseVS71)
			{
				push(@cfgs, "ReleaseUnicode");
			}
			else
			{
				push(@cfgs, "Release");
			}
		}
	}

	# Do some static code analysis (break, lock a.o.)
	# some projects are old or foreign stuff with an "individual" source code style
	# do not check anymore, because they are assumed valid and only pop useless warnings
	if (   $project eq "Lib/C4Lib"
		|| $project eq "Lib/codebase65"
		|| $project eq "Tools/LogZipLib"
		)
	{
		# skip these
	}
	else
	{
		&DoStaticAnalysis();
	}

	# Then loop over the given configurations.
	for (@cfgs) 
	{
		print ("\n");
#		print ("DoOne $project,$projfile,$dest\n");
		&DoOneConfiguration($projfile,$dest,$_);
	} 
}

#######################################################################
sub DoOneConfiguration
{
	local($projfileName,$dest,$cfg)=@_;
	local($oneLine)="";
	local($ei)=0;

	# print " .... $projfileName CFG=\"$dest - Win32 $cfg\"\n";

	chmod 0755, "$projfileName";	# allow for local conversion

	if ($bUseVS71)
	{
		local($builtType) = "build";
		if ($bRebuildAll || $bRebuildProj)
		{
			$builtType = "rebuild";
		}

		print "$builtType $dest - $cfg\n";
		# SYNTAX: devenv /build ConfigName [/project ProjName] [/projectconfig ConfigName] SolutionName
		# SYNTAX: devenv solutionfile.sln  /build solutionconfig
#		local($cmdLine) = "$MSDEV_DIR\\devenv $projfileName /$builtType $cfg /out D:\\Error.txt";
#		local($cmdLine) = "$MSDEV_DIR\\devenv $projfileName /$builtType $cfg";
		local($cmdLine) = "$MSDEV_DIR\\devenv $projfileName /$builtType $cfg";
#		print "$cmdLine\n";
		@errorLog = `$cmdLine `;
	}
	else
	{
		if ($bRebuildAll || $bRebuildProj)
		{
			@errorLog = `msdev.exe $projfileName /MAKE \"$dest - Win32 $cfg\" /REBUILD `;
		}
		else
		{
			@errorLog = `msdev.exe $projfileName /MAKE \"$dest - Win32 $cfg\" `;
		}
	}

	chmod 0555, "$projfileName";	# undo the above operation, make it readonly again

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
				$oneLine = "$theProjectDir/$project/$oneLine";
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
			if (/ COPY FAILED /) {
				$copyFailed++;
				$CopyFailedCount++;
			}
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
sub DoStaticAnalysis
{
#	print ("DoStaticAnalysis\n");
	# perform static analysis of sourceCode
	local($checkCmd)="v:\\perl5\\bin\\checkBreak.bat "; # getcwd is appended
	$checkCmd .= getcwd;
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
				$warningCount++;
			}
		}
	}
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
	local($dspPath) = $proj;
	local($dspFile) = "";
	if ($bTraceDebug) {print ("$dspPath\n");}

	local(@array)  = split(/\\/,$dspPath);     # Array der einzelnen Teilstrings

	# last one is project itself
	foreach $x (@array)
	{
		$dspFile = $x;
	}
	if ($dspFile eq "SecurityServer")
	{	# dsp file has different name than project
		$dspFile = "sec3";
	}
	if ($g_bUseVS71)
	{
		$dspFile .= "\.vcproj";
	}
	else
	{
		$dspFile .= "\.dsp";
	}
	if ($bTraceDebug) {print ("DSP file is $dspFile\n");}
	$dspPath = "$dspPath\\$dspFile";
	if ($bTraceDebug) {print ("DSP path is $dspPath\n");}

	# save old file time
	local($time_secs_old) = (stat($dspFile))[9]; 
	if ($bTraceDebug) {print ("time_secs_old $time_secs_old\n");}

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
	local($time_secs_new) = (stat($dspFile))[9]; 
	if ($bTraceDebug) {print ("time_secs_new $time_secs_new\n");}

	# if file time changed -> file changed -> rebuild all
	if ($time_secs_new != $time_secs_old)
	{
		$g_bRebuildProj = 1;
	}
	if ($bTraceDebug) {print ("Rebuild $g_bRebuildProj\n");}

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
sub DeleteEvenIfRO
{
	local($oneFile,$bDoMsg)=@_;

	if ($bDoMsg) {
		printf ("DeleteEvenIfRO for $oneFile, in %s, msg $bDoMsg\n",getcwd());
	}

	if (-f "$oneFile") {
		# remove write protectection 
		chmod 0755, "$oneFile";
		unlink ("$oneFile");

		if (-f "$oneFile") {
			print ("WARNING: $oneFile could not be removed!\n");
		} else {
			if ($bDoMsg) {
				print ("Removed $oneFile\n");
			}
		}
	} else {
		if ($bDoMsg) {
			print ("DeleteEvenIfRO: $oneFile not found\n");
		}
	}

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
sub SearchProject
{
	if (!(-d $theProjectDir))
	{
		if (-d "c:/project")
		{
			$theProjectDir = "c:\\project";
		}
		elsif (-d "e:/project")
		{
			$theProjectDir = "e:\\project";
		}
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

	if ($bUseVS71)
	{
		if ( -f "C:/MSDev2003/Common7/IDE/devenv.com" ) {
			$MSDEV_DIR="C:\\MSDev2003\\Common7\\IDE";
		} 
		elsif ( -f "D:/MSDev2003/Common7/IDE/devenv.com" ) {
			$MSDEV_DIR="D:\\MSDev2003\\Common7\\IDE";
		} 
		else {
			&NetSendError("MSDEV not found!");
			die "MsDev not found\n";
		}
	}
	else
	{
		if ( -d "C:/devstudio/vc" ) {
			$MSDEV_DIR="C:\\devstudio\\vc";
		}
		elsif ( -d "c:/msdev/vc" ) {
			$MSDEV_DIR="c:\\msdev\\vc";
		}
		elsif ( -f "c:/msdev/common/msdev98/bin/msdev.exe" ) {
			$MSDEV_DIR="c:\\msdev\\vc98";
		}
		elsif (-d "E:/devstudio/vc" ) {
			$MSDEV_DIR="E:\\devstudio\\vc";
		}
		elsif ( -d "F:/devstudio/vc" ) {
			$MSDEV_DIR="F:\\devstudio\\vc";
		}
		elsif ( -d "d:/msdev98/vc" ) { # CAVEAT order is important. special 6.0 hack
			$MSDEV_DIR="d:\\msdev98\\vc";
		}
		elsif ( -d "c:/msdev98/vc98" ) { # CAVEAT order is important. special 6.0 hack
			$MSDEV_DIR="c:\\msdev98\\vc98";
		}
		elsif ( -f "C:/Programme/Microsoft_Visual_Studio/Common/MSDev98/Bin/msdev.exe" ) {
			$MSDEV_DIR="C:\\Programme\\Microsoft_Visual_Studio\\Common\\MSDev98\\Bin";
		}
		elsif ( -f "C:/VisualStudio/Common/MSDev98/Bin/msdev.exe" ) {
			$MSDEV_DIR="C:\\VisualStudio\\Common\\MSDev98\\Bin";
		}
		elsif ( -f "C:/VS_60/Common/MSDev98/Bin/msdev.exe" ) {
			$MSDEV_DIR="C:\\VS_60\\Common\\MSDev98\\Bin";
		}
		elsif ( -f "C:/VS60/Common/MSDev98/Bin/msdev.exe" ) {
			$MSDEV_DIR="C:\\VS60\\Common\\MSDev98\\Bin";
		}
		elsif ( -f "C:/msdev60/MSDev98/Bin/msdev.exe" ) {
			$MSDEV_DIR="C:\\msdev60\\MSDev98\\Bin";
		}
		elsif ( -f "C:/Progra~1/Micros~1/Common/MSDev98/Bin/msdev.exe" ) {
			$MSDEV_DIR="C:\\Progra~1\\Micros~1\\Common\\MSDev98\\Bin";
		}
		elsif ( -f "C:/Progra~1/Micros~2/Common/MSDev98/Bin/msdev.exe" ) {
			$MSDEV_DIR="C:\\Progra~1\\Micros~2\\Common\\MSDev98\\Bin";
		}
		elsif ( -f "C:/Progra~1/Micros~3/Common/MSDev98/Bin/msdev.exe" ) {
			$MSDEV_DIR="C:\\Progra~1\\Micros~3\\Common\\MSDev98\\Bin";
		}
		 elsif ( -f "C:/Progra~1/Micros~4/Common/MSDev98/Bin/msdev.exe" ) {
			$MSDEV_DIR="C:\\Progra~1\\Micros~4\\Common\\MSDev98\\Bin";
		}
		elsif ( -f "C:/Progra~1/Micros~5/Common/MSDev98/Bin/msdev.exe" ) {
			$MSDEV_DIR="C:\\Progra~1\\Micros~5\\Common\\MSDev98\\Bin";
		}
		elsif ( -f "C:/Progra~1/Micros~6/Common/MSDev98/Bin/msdev.exe" ) {
			$MSDEV_DIR="C:\\Progra~1\\Micros~6\\Common\\MSDev98\\Bin";
		}
		elsif ( -f "C:/Progra~1/Micros~7/Common/MSDev98/Bin/msdev.exe" ) {
			$MSDEV_DIR="C:\\Progra~1\\Micros~7\\Common\\MSDev98\\Bin";
		}
		elsif ( -f "C:/Progra~1/Micros~8/Common/MSDev98/Bin/msdev.exe" ) {
			$MSDEV_DIR="C:\\Progra~1\\Micros~8\\Common\\MSDev98\\Bin";
		}
		elsif ( -f "C:/Progra~1/Micros~9/Common/MSDev98/Bin/msdev.exe" ) {
			$MSDEV_DIR="C:\\Progra~1\\Micros~9\\Common\\MSDev98\\Bin";
		}
		else {
			&NetSendError("MSDEV not found!");
			die "MsDev not found\n";
		}
	}
	print "VisualStudio is $MSDEV_DIR\n";
}	# end of SearchMSDEV
#######################################################################
sub SearchSS
{
	if ( -f "C:/vss/win32/ss.exe" ) {
		$SAFE_CMD="c:\\vss\\win32\\ss.exe";
	}
	elsif ( -f "c:/msdev/vss/win32/ss.exe" ) {
		$SAFE_CMD="C:\\msdev\\vss\\win32\\ss.exe";
	}
	elsif ( -f "c:/msdev98/common/vss/win32/ss.exe" ) {
		$SAFE_CMD="C:\\msdev98\\common\\vss\\win32\\ss.exe";
	}
	elsif ( -f "c:/msdev/common/vss/win32/ss.exe" ) {
		$SAFE_CMD="C:\\msdev\\common\\vss\\win32\\ss.exe";
	}
	elsif ( -f "D:/vss/win32/ss.exe" ) {
		$SAFE_CMD="D:\\vss\\win32\\ss.exe";
	}
	elsif (-f "E:/vss/win32/ss.exe" ) {
		$SAFE_CMD="E:\\vss\\win32\\ss.exe";
	}
	elsif ( -f "C:/Programme/Microsoft_Visual_Studio/Common/VSS/win32/ss.exe" ) {
		$SAFE_CMD="C:\\Programme\\Microsoft_Visual_Studio\\Common\\VSS\\win32\\ss.exe";
	}
	elsif ( -f "C:/VisualStudio/Common/VSS/win32/ss.exe" ) {
		$SAFE_CMD="C:\\VisualStudio\\Common\\VSS\\win32\\ss.exe";
	}
	elsif ( -f "C:/VS_60/Common/VSS/win32/ss.exe" ) {
		$SAFE_CMD="C:\\VS_60\\Common\\VSS\\win32\\ss.exe";
	}
	elsif ( -f "C:/VS60/Common/VSS/win32/ss.exe" ) {
		$SAFE_CMD="C:\\VS60\\Common\\VSS\\win32\\ss.exe";
	}
	elsif ( -f "C:/msdev60/VSS/win32/ss.exe" ) {
		$SAFE_CMD="C:\\msdev60\\VSS\\win32\\ss.exe";
	}
	elsif ( -f "C:/Progra~1/Micros~1/Common/VSS/win32/ss.exe" ) {
		$SAFE_CMD="C:\\Progra~1\\Micros~1\\Common\\VSS\\win32\\ss.exe";
	}
	elsif ( -f "C:/Progra~1/Micros~1/VSS/win32/ss.exe" ) {
		$SAFE_CMD="C:\\Progra~1\\Micros~1\\VSS\\win32\\ss.exe";
	}
	elsif ( -f "C:/Progra~1/Micros~2/Common/VSS/win32/ss.exe" ) {
		$SAFE_CMD="C:\\Progra~1\\Micros~2\\Common\\VSS\\win32\\ss.exe";
	}
	elsif ( -f "C:/Progra~1/Micros~2/VSS/win32/ss.exe" ) {
		$SAFE_CMD="C:\\Progra~1\\Micros~2\\VSS\\win32\\ss.exe";
	}
	elsif ( -f "C:/Progra~1/Micros~3/Common/VSS/win32/ss.exe" ) {
		$SAFE_CMD="C:\\Progra~1\\Micros~3\\Common\\VSS\\win32\\ss.exe";
	}
	elsif ( -f "C:/Progra~1/Micros~3/VSS/win32/ss.exe" ) {
		$SAFE_CMD="C:\\Progra~1\\Micros~3\\VSS\\win32\\ss.exe";
	}
	elsif ( -f "C:/Progra~1/Micros~4/Common/VSS/win32/ss.exe" ) {
		$SAFE_CMD="C:\\Progra~1\\Micros~4\\Common\\VSS\\win32\\ss.exe";
	}
	elsif ( -f "C:/Progra~1/Micros~4/VSS/win32/ss.exe" ) {
		$SAFE_CMD="C:\\Progra~1\\Micros~1\\VSS\\win32\\ss.exe";
	}
	elsif ( -f "C:/Progra~1/Micros~5/Common/VSS/win32/ss.exe" ) {
		$SAFE_CMD="C:\\Progra~1\\Micros~5\\Common\\VSS\\win32\\ss.exe";
	}
	elsif ( -f "C:/Progra~1/Micros~5/VSS/win32/ss.exe" ) {
		$SAFE_CMD="C:\\Progra~1\\Micros~5\\VSS\\win32\\ss.exe";
	}
	elsif ( -f "C:/Progra~1/Micros~6/Common/VSS/win32/ss.exe" ) {
		$SAFE_CMD="C:\\Progra~1\\Micros~6\\Common\\VSS\\win32\\ss.exe";
	}
	elsif ( -f "C:/Progra~1/Micros~7/Common/VSS/win32/ss.exe" ) {
		$SAFE_CMD="C:\\Progra~1\\Micros~7\\Common\\VSS\\win32\\ss.exe";
	}
	elsif ( -f "C:/Progra~1/Micros~8/Common/VSS/win32/ss.exe" ) {
		$SAFE_CMD="C:\\Progra~1\\Micros~8\\Common\\VSS\\win32\\ss.exe";
	}
	elsif ( -f "C:/Progra~1/Micros~9/Common/VSS/win32/ss.exe" ) {
		$SAFE_CMD="C:\\Progra~1\\Micros~9\\Common\\VSS\\win32\\ss.exe";
	}
	else {
#		$SAFE_CMD="ss.exe";
		&NetSendError("SourceSafe not found!");
		die "SourceSafe not found\n";
	}
	print "SourceSafe is $SAFE_CMD\n";
}
######################################################################################################
sub SetEnv
{
	if (g_bUseVS71)
	{
		# set env variables, not using V:
		# CAVEAT requires $g_theProjectDir
		
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
		$ENV{'INCLUDE'}=$TmpInclude;
		print ("INCLUDE IS: $TmpInclude\n");

		$TmpLib = "d:\\inetsdk\\lib;$MSDEV_DIR\\lib;$MSDEV_DIR\\mfc\\lib;";
		$TmpLib .= "$g_theProjectDir\\Lib;$g_theProjectDir\\Cipc\\Release;$g_theProjectDir\\DV\\Lib;";
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
	}	

	# make sure devenv is in the path
	$path = $ENV{'PATH'};
	$path .= ";$MSDEV_DIR;";
	$ENV{'PATH'} = $path;

	print "PATH is $ENV{'PATH'}\n";
}
#######################################################################
sub NetSendError
{
	local($sMessage)=@_;

	for (@ErrorRecipients)
	{
		`Net Send $_ "$sMessage from DLL_Make"`;
	}
}
#######################################################################
__END__
:endofperl
