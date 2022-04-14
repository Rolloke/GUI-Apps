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

#********************************************************
# <GLOBALS> 
#********************************************************

@EmailRecipients=(
				  "Uwe.Freiwald\@videte.com",
				  "Tomas.Krogh\@videte.com",
				  "Rolf.Kary-Ehlers\@videte.com",
				  "Martin.Lueck\@videte.com",
				  "Wolfgang.Schumacher\@videte.com",
				 );

@NetSendRecipients=(
					"Uwe.Freiwald",
					"Wolfgang.Schumacher",
				   );

@NetSendRecipientsStartWarningSoftware=(
										"Uwe.Freiwald",
										"MartinXP",
										"TomasXP",
										"RolfXP",
									   );

@NetSendRecipientsStartWarningHardware=(
										"Bjoern",
									   );

$g_Second = 0;
$g_Minute = 0;
$g_Hour   = 0;
$g_Day    = 0;
$g_Month  = 0;
$g_Year   = 0;
$g_sDate  = "";

#--------------------------------------------------------
# set some global executable variables
# and collect them in array
@g_allExecutables = ();
$ZIP_CMD  = "v:\\bin\\tools\\zip\\pkzip25.exe";
push(@g_allExecutables, $ZIP_CMD);
$NERO_CMD = "c:\\Programme\\Ahead\\Nero\\NeroCmd.exe";
push(@g_allExecutables, $NERO_CMD);

#--------------------------------------------------------
# set some global directory variables
# and collect them in array
@g_allDirectories = ();
$g_sSaveDrive = "g:";
$g_sSaveBaseDir = "$g_sSaveDrive\\Sicherung";
push(@g_allDirectories, $g_sSaveBaseDir);
$g_sTempDir = "$g_sSaveBaseDir\\Temp";
push(@g_allDirectories, $g_sTempDir);
$g_sSoftwareSourceDir = "v:\\vss";
push(@g_allDirectories, $g_sSoftwareSourceDir);
$g_sHardwareMentorSourceDir  = "Q:\\Mentor";
push(@g_allDirectories, $g_sHardwareMentorSourceDir);
$g_sHardwareProjectSourceDir = "Q:\\Projekte";
push(@g_allDirectories, $g_sHardwareProjectSourceDir);

$g_sSoftwareZipFile = "";
$g_sHardwareMentorZipFile = "";
$g_sHardwareProjectZipFile = "";

$g_SoftwareZipLogFile = "$g_sSaveBaseDir\\SoftwareZipLog.txt";
$g_HardwareMentorZipLogFile = "$g_sSaveBaseDir\\HardwareMentorZipLog.txt";
$g_HardwareProjectZipLogFile = "$g_sSaveBaseDir\\HardwareProjectZipLog.txt";

$g_iWarnings = 0;
$g_bAbort = 0;
@g_LogOutput = ();
$g_LogFile = "$g_sSaveBaseDir\\SourceSafeBurnLog.txt";
@g_EmailOutput = ();
$g_EmailFile = "$g_sSaveBaseDir\\Email.txt";
$g_sSizeText = "0 MB";

#********************************************************
#--------------------------------------------------------
# Set all date specific variables
&SetTimeVariables();
$g_sDate = "$g_Year$g_Month$g_Day";
$g_sSaveDayDir = "$g_sSaveBaseDir\\$g_sDate";
push(@g_allDirectories, $g_sSaveDayDir);
$g_sHardwareMentorDestDir    = "$g_sSaveDayDir\\Hardware\\Mentor";
push(@g_allDirectories, $g_sHardwareMentorDestDir);
$g_sHardwareProjectDestDir   = "$g_sSaveDayDir\\Hardware\\Projekte";
push(@g_allDirectories, $g_sHardwareProjectDestDir);
$g_sSoftwareDestDir   = "$g_sSaveDayDir\\Software";
push(@g_allDirectories, $g_sSoftwareDestDir);

$g_sSoftwareZipFile = "$g_sDate.zip";
$g_sHardwareMentorZipFile = "$g_sDate\_MentorLibrary.zip";
$g_sHardwareProjectZipFile = "$g_sDate\_Projekte.zip";

&PrintOutput("SourceSafeBurn started\n");

#--------------------------------------------------------
# check for required executables
&CheckForRequiredExecs();

#--------------------------------------------------------
# check for required directories
&CheckForRequiredDirs();

#--------------------------------------------------------
# Copy and compress software data
&SafeSoftwareFiles();

#--------------------------------------------------------
# Copy and compress hardware data
&SafeHardwareFiles();

#--------------------------------------------------------
# check for success of zipping (may be out of harddisk space)
&CheckForBurnFiles();

#--------------------------------------------------------
# Burning the backup
&BurnCD();

#--------------------------------------------------------
# Check drive space before deleting old subdirectories
&CheckDriveSpace($g_sSaveDrive);

#--------------------------------------------------------
# Delete old subdirectories
&DeleteEarlierSubDirs();

#--------------------------------------------------------
# Check drive space
&CheckDriveSpace($g_sSaveDrive);

#--------------------------------------------------------
# Log output and inform clients
$sDoneText = "SourceSafeBurn OK  $g_sSizeText";
if ($g_bAbort)
{
	$sDoneText = "SourceSafeBurn FAILED";
}
elsif ($g_iWarnings)
{
	$sDoneText = "SourceSafeBurn Completed, but $g_iWarnings Warnings";
}

# InformClients text without trailing \n!
&InformRecipients("$sDoneText");

&SetTimeVariables();
&PrintOutput("End SourceSafeBurn\n");

open (OUTPUT_FILE, ">$g_LogFile");
for (@g_LogOutput)
{
	print OUTPUT_FILE "$_";
}
close (OUTPUT_FILE);

###################################################################################################
# end of main


###################################################################################################
sub SetTimeVariables
{
	my ($isdst,$wday,$yday);
	
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
sub CheckForRequiredExecs
{
	if (!$g_bAbort)
	{
		foreach (@g_allExecutables)
		{
			if (! -f "$_")
			{
				&OnError("File not found '$_'\n");
			}
		}
	}
}

###################################################################################################
sub CheckForRequiredDirs
{
	if (!$g_bAbort)
	{
		foreach (@g_allDirectories)
		{
			if (! -d $_)
			{
				&CreateNewDir($_);
			}
		}
	}
}

###################################################################################################
sub SafeSoftwareFiles
{
	if (!$g_bAbort)
	{
		#--------------------------------------------------------
		# Warn Software that backup starts now
		&PrintOutput("Warning Software...\n");
		for (@NetSendRecipientsStartWarningSoftware)
		{
			`Net Send $_ "ACHTUNG: Software-Sicherung wird gestartet"`;
		}
		&PrintOutput("Warning Software completed\n");

		&CopyToTempDir($g_sSoftwareSourceDir);

		if (!$g_bAbort)
		{
			&CompressToDestination("$g_sSoftwareDestDir\\$g_sSoftwareZipFile", "$g_SoftwareZipLogFile");
		}

		&DeleteTempFiles();
	}
}

###################################################################################################
sub SafeHardwareFiles
{
	if (!$g_bAbort)
	{
		#--------------------------------------------------------
		# Warn Hardware that backup starts now
		&PrintOutput("Warning Hardware...\n");
		for (@NetSendRecipientsStartWarningHardware)
		{
			`Net Send $_ "ACHTUNG: Hardware-Sicherung wird gestartet"`;
		}
		&PrintOutput("Warning Hardware completed\n");

		&SafeHardwareMentorFiles();

		if (!$g_bAbort)
		{
			&SafeHardwareProjectFiles();
			if (!$g_bAbort)
			{
				&PrintOutput("Copying Mentor index files...\n");
				`copy Q:\\Mentor\\HW_Library\\CentLib.prp $g_sHardwareMentorDestDir`;
				`copy Q:\\Mentor\\HW_Library\\SysIndex.cbf $g_sHardwareMentorDestDir`;
				&PrintOutput("Copy completed\n");
			}
		}
	}
}

###################################################################################################
sub SafeHardwareMentorFiles
{
	if (!$g_bAbort)
	{
		&CopyToTempDir($g_sHardwareMentorSourceDir);

		if (!$g_bAbort)
		{
			&CompressToDestination("$g_sHardwareMentorDestDir\\$g_sHardwareMentorZipFile", "$g_HardwareMentorZipLogFile");
		}

		&DeleteTempFiles();
	}
}

###################################################################################################
sub SafeHardwareProjectFiles
{
	if (!$g_bAbort)
	{
		&CopyToTempDir($g_sHardwareProjectSourceDir);

		if (!$g_bAbort)
		{
			&CompressToDestination("$g_sHardwareProjectDestDir\\$g_sHardwareProjectZipFile", "$g_HardwareProjectZipLogFile");

		}
		&DeleteTempFiles();
	}
}

###################################################################################################
sub CopyToTempDir
{
	if (!$g_bAbort)
	{
		my ($Source) = @_;
		# temp dir may be not exist, e.g. deleted at prior DeleteTempFiles()
		&CreateNewDir($g_sTempDir);
		&PrintOutput("Copying $Source to $g_sTempDir...\n");
		my ($sOutput) = "";
		$sOutput = `xcopy /E /S /Q /C /H /R /Y $Source\\*.* $g_sTempDir\\`;
		&PrintOutput("$sOutput");
		&PrintOutput("Copy completed\n");
	}
}

###################################################################################################
sub CompressToDestination
{
	if (!$g_bAbort)
	{
		my ($Destination, $sZipLogFile) = @_;

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

		my ($EXE_CMD) = "$ZIP_CMD -add -recurse -directories $Destination $g_sTempDir\\*.*";

		&PrintOutput("Compressing $g_sTempDir to $Destination...\n");
#		print "$EXE_CMD\n";
		open (STDERR, ">$sZipLogFile");
		`$EXE_CMD`;
		close (STDERR);
		my ($bNoErrors) = 1;
		open (ZIPLOG_FILE, "$sZipLogFile");
		while (<ZIPLOG_FILE>)
		{
			if (/\w/)		# at least one word
			{
				$bNoErrors = 0;
				&OnWarning("$_");
			}
		}
		close (ZIPLOG_FILE);
		if ($bNoErrors)
		{
			&PrintOutput("Compress completed\n");
		}
	}
}

###################################################################################################
sub DeleteTempFiles
{
	&PrintOutput("Deleting temp files...\n");
	if (-d "$g_sTempDir" )
	{
		&ChangeToDir("$g_sSaveBaseDir");			# dir to delete MUST NOT BE the actual dir
	#	print `deltree /Y $g_sTempDir`;		# Win95 command
		print `rd /s /q $g_sTempDir`;		# WinNT command
	}
	&PrintOutput("Delete completed\n");
}

###################################################################################################
sub CheckForBurnFiles
{
	if (!$g_bAbort)
	{
		@allBurnFiles = (
							"$g_sSoftwareDestDir\\$g_sSoftwareZipFile",
							"$g_sHardwareMentorDestDir\\$g_sHardwareMentorZipFile",
							"$g_sHardwareProjectDestDir\\$g_sHardwareProjectZipFile",
							"$g_sHardwareMentorDestDir\\CentLib.prp",
							"$g_sHardwareMentorDestDir\\SysIndex.cbf",
						);
		my ($iWarningCount) = 0;
		my ($iSize) = 0;
		my ($iTotalSize) = 0;

		foreach (@allBurnFiles)
		{
			if (! -f "$_")
			{
				&OnWarning("File to burn not found '$_'\n");
				$iWarningCount++;
			}
			else
			{
				$iSize = (-s $_);            
#				print "$_i : $iSize\n";
				$iTotalSize += $iSize;
#				print "Total size : $iTotalSize\n";
			}
		}

		# if counter greater than array index, no file was found
#		print "Warnings: $iWarningCount > $#allBurnFiles?\n";
		if ($iWarningCount > $#allBurnFiles)
		{
			&OnError("No files to burn found\n");
		}

		$iTotalSize = $iTotalSize/1024/1024;	# Size in MB, caveat: can be float
		if($iTotalSize =~ /\./)	# search for point of float
		{
			$iTotalSize = $`;	# string in front of point => integer
			$iTotalSize++;		# round up
		}
#		print "Total size : $iTotalSize MB\n";
	
		if ($iTotalSize > 720)
		{
			my ($sMsg) = "Backup size too big: $iTotalSize";
			&OnError("$sMsg\n");
		}
		elsif ($iTotalSize > 700)
		{
			my ($sMsg) = "Backup greater than 700 MB: $iTotalSize";
			&OnWarning("$sMsg\n");
		}

		$g_sSizeText = "$iTotalSize MB";
	}
}

###################################################################################################
sub BurnCD
{
	if (!$g_bAbort)
	{
		my ($sParameter, $sErrorLogFile, $sNeroLogFile, $sOneLine);
		my ($bBurnOK) = 0;
		my ($bVerifyOK) = 0;
		$sErrorLogFile = "$g_sSaveBaseDir\\BurnErrorLog.txt";
		$sNeroLogFile = "$g_sSaveBaseDir\\neroerr.txt";
		unlink("$sErrorLogFile");
#		$Parameter = "--write --drivename i --real --iso Sicherung_$g_sDate --underrun_prot --verify --error_log $sErrorLogFile --disable_eject --detect_non_empty_cdrw --force_erase_cdrw";
#		$Parameter = "--write --drivename i --real --iso Sicherung_$g_sDate --underrun_prot --verify --disable_eject --detect_non_empty_cdrw --force_erase_cdrw";
		$Parameter = "--write --drivename i --real --iso Sicherung_$g_sDate --underrun_prot --verify --error_log $sErrorLogFile";

		&ChangeToDir("$g_sSaveBaseDir");			# change dir to get internal logfile there
		&PrintOutput ("Burning CD...\n");
#		print ("$NERO_CMD $Parameter $g_sSaveDayDir\n");
		`$NERO_CMD $Parameter "$g_sSaveDayDir"`;

		my ($bDebugging) = 0;

		open(FILE, "$sNeroLogFile");
		# Only check for OK, errors can be read in logfile itself
		while (<FILE>)
		{
			if (/Burn process completed successfully/)
			{
				$bBurnOK = 1;
			}
			elsif (/Data verification completed successfully/)
			{
				$bVerifyOK = 1;
			}
		}
		close(FILE);
		if ($bBurnOK && $bVerifyOK)
		{
			&PrintOutput ("Burn CD OK\n");
		}
		elsif (!$bBurnOK)
		{
			&OnError("Burn CD FAILED\n");
		}
		elsif (!$bVerifyOK)
		{
			&OnError("Verify CD FAILED\n");
		}
		else
		{
			&OnError("UNKNOWN ERROR, see $sNeroLogFile\n");
		}
	}
}

###################################################################################################
sub CheckDriveSpace
{
	my ($drive) = @_;
	my ($FreeDiskSpace) = &GetFreeDiskSpace($drive);
	my ($sOutput) = "Free disk space : $FreeDiskSpace Bytes\n\n";
	push(@g_EmailOutput, "$sOutput");


	# Warn if free disk space is low
	# Minimum 2 GB
	my ($MinGB) = 2;
	my ($MinFreeSpace) = $MinGB*1024*1024*1024;
	# remove dots
	$FreeDiskSpace =~ s/\.//g;
	if ($FreeDiskSpace < $MinFreeSpace)
	{
		OnWarning("Drive $drive : Free disk space below $MinGB GB!\n");
	}
}

#######################################################################
sub OnError
{
	my ($sMessage) = @_;
	my ($sOutput) = "ERRROR: $sMessage";
	PrintOutput($sOutput);
	push(@g_EmailOutput, "$sOutput");
	$g_bAbort = 1;
}

#######################################################################
sub OnWarning
{
	my ($sMessage)=@_;
	my ($sOutput) = "WARNING: $sMessage";
	PrintOutput($sOutput);
	push(@g_EmailOutput, "$sOutput");
	$g_iWarnings++;
}

#######################################################################
sub PrintOutput
{
	my ($sMessage) = @_;
	my ($sTime) = "";
	my ($sOutput) = "";

	&SetTimeVariables();
	$sTime = "$g_Day.$g_Month.$g_Year, $g_Hour:$g_Minute:$g_Second";
	$sOutput = "$sTime  $sMessage";
	print ("$sOutput");
	push(@g_LogOutput, "$sOutput");
}

#######################################################################
# InformClients text without trailing \n!
sub InformRecipients
{
	my ($sMessage)=@_;
	PrintOutput("$sMessage\n");
	for (@NetSendRecipients)
	{
		`Net Send $_ "$sMessage"`;
	}

	open (EMAIL_FILE, ">$g_EmailFile");
	print EMAIL_FILE "$sMessage";
	print EMAIL_FILE "\n\n";
	for (@g_EmailOutput)
	{
		print EMAIL_FILE "$_";
	}
	close (EMAIL_FILE);

	for (@EmailRecipients)
	{
		`v:\\bin\\tools\\email.exe mail01 Uwe.Freiwald\@videte.com $_ \"Sicherung vom $g_sDate\" \"$g_EmailFile\"`;
	}
}

#######################################################################
sub CreateNewDir
{
	my ($bDebugging) = 0;
	my($dir)=@_;
	if ($bDebugging) {print "CreateSubDirs $dir\n";}
	my(@subdirs) = split("\\\\",$dir);
	if ($bDebugging) {print "CreateSubDirs $#subdirs, @subdirs\n";}
	foreach (@subdirs)
	{
		if ($bDebugging) {print "CreateSubDir $_\n";}
		# special check for root/drive, that can not be created
		# subdirectories can exists like Units/SimUnits
		if (! -d $_ && !/:/)
		{
			if ($bDebugging) {printf ("Create $_ in %s\n",getcwd);}
			system("mkdir $_");
			if ($bDebugging) {print ("Created $_\n");}
		}
		if (-d $_)
		{
			chdir($_);
		}
		elsif (/:/)
		{
			chdir ("$_\\");	# absolute path, back to root dir
		}
		else
		{
			OnError("CreateNewDir $_ FAILED\n");
		}
	}
}

#######################################################################
sub ChangeToDir
{
	my($dir) = @_;
	my ($bDebugging) = 0;
	if ($bDebugging) {print "ChangeToDir $dir\n";}
	my(@subdirs) = split("\\\\",$dir);
	if ($bDebugging) {print "ChangeToDir $#subdirs, @subdirs\n";}
	foreach (@subdirs)
	{
		if ($bDebugging) {print "ChangeToDir $_\n";}
		# special check for root/drive, that can not be created
		# subdirectories can exists like Units/SimUnits
		if (/:/)
		{
			chdir ("$_\\");	# absolute path, back to root dir
			if ($bDebugging) {printf ("ChangeToDir $_, current is %s\n",getcwd);}
		}
		elsif (-d $_ )
		{
			chdir($_);
			if ($bDebugging) {printf ("ChangeToDir $_, current is %s\n",getcwd);}
		}
		else
		{
			OnError("ChangeToDir $_ FAILED\n");
		}
	}
}
#######################################################################
sub GetFreeDiskSpace
{
	my ($bDebugging) = 0;
	my($drive) = @_;
	if ($bDebugging) {print "CheckFreeDiskSpace $drive\n";}
	my(@Output) = `dir $drive`;
	my($FreeDiskSpace) = 0;
	for (@Output)
	{
		if ($bDebugging){print "$_";}
		if (/Bytes frei/)
		{
			if ($bDebugging){print "Found \"Bytes frei\": $_\n";}
			my(@array) = split(/ /);
			for (@array)
			{
				if ($bDebugging){if (/\w/){print "$_\n";}}
				if (/\./)
				{
					if ($bDebugging){print "Found \"\.\": $_\n";}
					$FreeDiskSpace = $_;
#					$FreeDiskSpace =~ s/\.//g;
				}
			}
		}
	}
	if ($bDebugging){print "Return: $FreeDiskSpace\n";}
	return $FreeDiskSpace;
}
#######################################################################
sub DeleteEarlierSubDirs
{
	my ($bDebugging) = 0;
	my ($date) = $g_sDate;

	# calculate the date one month earlier
	if ($g_Month <= 01)
	{
		$date = $date - 8900;
	}
	else
	{
		$date = $date - 100;
	}
	my ($sOneDir) = "";
	my ($sOutput) = "Delete subdirs before $date\n";
	PrintOutput($sOutput);
	push(@g_EmailOutput, "$sOutput");

	# get all subdirs
	&ChangeToDir($g_sSaveBaseDir);
	@someDirs= `dir /b /a:d /o:n *.*`;
	for ($i=0 ; $i<=$#someDirs ; $i++)
	{
		$sOneDir=$someDirs[$i];
		if ($bDebugging){print "subdir $sOneDir\n";}
		$sOneDir =~ s/[\n\r]//gm;
		if ($bDebugging){print "subdir $sOneDir\n";}
		if ($sOneDir < $date)
		{
			$sOutput = "Remove old dir: $sOneDir\n";
			push(@g_EmailOutput, "$sOutput");
			if ($bDebugging){print "subdir $sOneDir delete\n";}
			`rd $sOneDir /s /q`;
			if (-d $sOneDir)
			{
				OnError("DeleteEarlierSubDir $sOneDir FAILED\n");
			}
		}
		else
		{
			if ($bDebugging){print "subdir $sOneDir keep\n";}
		}
	}
}
###################################################################################################
__END__
:endofperl
