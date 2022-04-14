@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

$|=1;

#
# PERL PART FOLLOWS HERE
#
# patterns
# "\w+->\w+\s*=[^=]"	alle word->word = 
# "new\s+\w+\s*\["		alle new type [
#

use Cwd;
require "find.pl";
#require "finddepth.pl";

# $ENV{'ssdir'}="u:\\vss";


if ( -f "C:/vss/win32/ss.exe" ) {
	$SAFE_CMD="c:\\vss\\win32\\ss.exe";
} elsif ( -f "D:/vss/win32/ss.exe" ) {
	$SAFE_CMD="D:\\vss\\win32\\ss.exe";
} elsif (-f "E:/vss/win32/ss.exe" ) {
	$SAFE_CMD="E:\\vss\\win32\\ss.exe";
} elsif ( -f "F:/vss/win32/ss.exe" ) {
	$SAFE_CMD="F:\\vss\\win32\\ss.exe";
} elsif ( -f "D:/DevStudio/vss/win32/ss.exe" ) {
	$SAFE_CMD="D:\\DevStudio\\vss\\win32\\ss.exe"
} elsif ( -f "C:/MSDev98/Common/VSS/win32/ss.exe" ) {
	$SAFE_CMD="C:\\MSDev98\\Common\\VSS\\win32\\ss.exe"
} elsif ( -f "D:/MSDev98/Common/VSS/win32/ss.exe" ) {
	$SAFE_CMD="D:\\MSDev98\\Common\\VSS\\win32\\ss.exe"
} elsif ( -f "E:/MSDev98/Common/VSS/win32/ss.exe" ) {
	$SAFE_CMD="E:\\MSDev98\\Common\\VSS\\win32\\ss.exe"
} elsif ( -f "F:/MSDev98/Common/VSS/win32/ss.exe" ) {
	$SAFE_CMD="F:\\MSDev98\\Common\\VSS\\win32\\ss.exe"
} else {
       print "SourceSafe not found\n";
       $SAFE_CMD="C:\\Foo\\win32\\ssfoo.exe";
}

$canSourceSafe=1;
$runAsGrepOnly=1;
$updateVersionOnly=0;
$bDoCommentLog= 1;
$bOnceOnly=0;
$bPrependEmptyLine=0;	# prepend empty line before doing the replace
$bKeepOldLine=0;	# print the 
$numFound=0;

$pattern=$ARGV[0];
$replace="";
$fillerHour = "";
$fillerMinute="";
$fillerMonth = "";
$fillerDay = "";

if ($#ARGV==0) {	# last index, not count
	if ($pattern eq "-updateVersion") {
		$pattern = "VERSION\\(.*\@AUTO_UPDATE";	# OOPS
		($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
		$month=eval($mon+1);
		$year=eval($year + 1900);

		if ($month<10) {
			$fillerMonth = "0";
		}
		if ($mday<10) {
			$fillerDay = "0";
		}
		if ($hour<10) {
			$fillerHour = "0";
		}
		if ( $min < 10) {
			$fillerMinute="0";
		}

		# filler only within the string, 
		# else 09 is an invalid octal digit
		$replace = "VERSION\($mday, $month, $hour, $min, \"$fillerDay$mday\/$fillerMonth$month\/$year , $fillerHour$hour:$fillerMinute$min\\0\", // \@AUTO_UPDATE";
		$updateVersionOnly=1;
		$runAsGrepOnly=0;
		print ("New Version: $fillerDay$mday, $fillerMonth$month, $fillerHour$hour, $fillerMinute$min\n");
	}
} else {
	if ( $ARGV[0] eq "-updateWSE") {
		$bDoCommentLog= 0;
		$pattern=$ARGV[1];
		$replace=$ARGV[2];
		$runAsGrepOnly=0;
	} elsif ( $ARGV[0] eq "-updateBuild") {
		$bDoCommentLog= 0;
		$pattern=$ARGV[1];
		$replace=$ARGV[2];
		$runAsGrepOnly=0;
	} else {
		$replace=$ARGV[1];
		$runAsGrepOnly=0;
	}
}

if ($updateVersionOnly==0) {
	print "$pattern:";
}
if ( $runAsGrepOnly) { 
	print "listing only\n";
} elsif ($updateVersionOnly) {
	# be quite
} else {
	print "--> $replace\n"; 
}

open(LOGFILE,">replace.log");

$doReplace=0;		# toggle for the wanted subroutine
$foundAFile=0;		# set if at least one file is found in the first run
$allAreWritable=1;	# set to 0 if one matched file does not have write permission

# first run only checks for all files and write permissions
&find (".");
#&finddepth(".");

# so what is the result of the first run ? Any files? All writable ?
if ( $foundAFile ) {
	if ( $runAsGrepOnly==0 ) {
		if ($allAreWritable) {
			$doReplace=1;	# activate replace mode in ContainsPattern
			&find(".");	# and do the second run
		} else {
			print "Some files are under write protection! None modified.\n";
		}
	} else {
		print "Found: $numReplaces\n";
	}
} else {
	if ( $runAsGrepOnly==0) {
		print "No files found\n";
	}
}

if ($updateVersionOnly) {
	# new 161097 prepend updateVersionInfo to history.txt
	if ( -f "history.txt" ) {
		if ( ! -w "history.txt" && $canSourceSafe) { 
			TrySourceSafe("history.txt"); 
		}
		if (-w "history.txt") {
			$bOnceOnly=1;
			$bPrependEmptyLine=1;
			$bKeepOldLine=1;
			$pattern="^(?!\/\/).*\$";	# StartOfLine not followed  by //
			$replace = "$fillerDay$mday.$fillerMonth$month.$year >>> UpdateVersion um $fillerHour$hour:$fillerMinute$min <<<";

			DoReplace("history.txt");
		} else {
			print "Could not checkout history.txt\n";		
		}
	} else {
		print ("history.txt not found!?\n");
	}
	# end of history.txt modification

	print "UpdateVersion DONE.\n";
} else {
	print "DONE.\n";
}

#
# check a file for the pattern, prints file:line if $runAsGrepOnly
#
sub ContainsPattern
{
	local($dir,$file)=@_;
	local($patternFound) = 0;

	open(FILE,$file) || warn "cant open $dir/$file\n";
	$line=1;
	while (<FILE>) {
		if (/$pattern/) {
			if ($runAsGrepOnly) {
				printf ("$dir/$file(%d):$_",$line);
			}
			$patternFound=1;
			$numReplaces += 1;
		}
		$line++;
	}
	close(FILE);
	return $patternFound;
}

# 
sub DoReplace
{
	local($file) = @_;
	
	print LOGFILE "\nFILE:$file\n\n";
	
	$numReplaces=0;

	open(FILE,$file) || warn "cant open $file\n";
	open(FILEOUT,">$file.new") || die "can't open foo.tmp\n";
	# print 'header' information
	if ("$replace" eq "GARNIX") {
		# no message for removes, allows to remove GlobalReplace
	} else {
		if ($updateVersionOnly==0 && $bDoCommentLog==1 ) {
			print FILEOUT "/* GlobalReplace: $pattern --> $replace */\n";
		}
	}

	while (<FILE>) {
		if (/$pattern/
			&& ($bOnceOnly==0	# do all lines
				|| ($bOnceOnly==1 && $numReplaces<1)	# only the first one
			)
			) {
			$myBackup = $_;
			if ($bPrependEmptyLine==1) {
				print FILEOUT "\n";
			}
			if ($updateVersionOnly==0) {
				print LOGFILE "old:$_"; 
			}
			$numReplaces += s/$pattern/$replace/g;
			if ("$replace" eq "GARNIX") {
				print LOGFILE "-->:removed";
			} else {
				print FILEOUT $_;	# the replacement
				if ($updateVersionOnly==0) {
					print LOGFILE "-->:$_";
				}
			}
			if ($bKeepOldLine==1) {
				print FILEOUT $myBackup;		
			}
		} else {
			print FILEOUT $_;
		}	
	}

	close(FILE);
	close(FILEOUT);

	rename($file,"$file.bak") || warn "could not rename $file --> $file.bak\n";
	rename("$file.new",$file) || warn "could not rename $file.new --> $file\n";


	if ($updateVersionOnly) {
		BackToSourceSafe($file);
	} else {
		print "~$file: $numReplaces\n";
	}
}

sub wanted
{
	$MyFile=$_;

	$dont_use_nlink=1;
	# print("FILE: $name\n");
	if ( -d "$dir/$_" ) {
                # print ("Subdirectory $name:\n");
		$prune=0;
	}
	# select files, either all or only rc files
	# //i is for ignore case
        if ( /\.cxx$/i || /\.foo$/i || /\.cpp$/i || /\.h$/i || /\.txt$/i || /\.c$/i
				|| /\.wse$/i 
				|| /\.rc$/i || /\.rc2$/i
				) {
		$file="$dir/$_";	# plus $dir
		if ($dir eq ".") {
			$file=$_;
		}
		$file=$name;
				
		$notAll=0;	# all matched files writable ?
		$patternFound = &ContainsPattern($dir,$_);	# OLD $_
		# open(FILEOUT,">foo.tmp") || warn "can't open foo.tmp\n";
		# first run only check, second run sets doReplace on TRUE
		if ( $doReplace==0 ) {
			if ($patternFound) {
				$foundAFile=1;
				if ($runAsGrepOnly==0) {
                    if ( ! -w $MyFile && $canSourceSafe) { 
						TrySourceSafe($MyFile); 
					}
					if ( -w $MyFile) {
						if ($updateVersionOnly==0) {
							# indicate as writable
							print "+$file\n";	
						}
					} else {
						# indicate as ReadOnly!
                        print "%$file\n";
						$allAreWritable=0;
					}
				}
			} else {
                # print "-$file\n";
			}
		} else {	# second run
			if ( $patternFound ) { 
				DoReplace($MyFile);
			}
		}
	} else {
		# print "plain $_\n";
	}
	return 1==1;
}

sub TrySourceSafe
{
    local($file)=@_;
    
    $proj=getcwd;
    $proj =~ s/[A-Z]:/\$/;

    open(BATFILE,">get.bat");
    print BATFILE "@","echo off\n";
# -I- do not ask for user input under any circumstances
# -C- do not ask for comment
    print BATFILE "$SAFE_CMD Checkout -C- \"$proj\\$file\"";
	if ($updateVersionOnly) {
		print BATFILE " -O\@vsstmp.txt";
	}
    close(BATFILE);
    print ("trying to checkout $file from SourceSafe...\n");
    system("./get.bat");
    unlink("./get.bat");
	if ($updateVersionOnly) {
		unlink ("\@vsstmp.txt");
	}
}

sub BackToSourceSafe
{
    local($file)=@_;
    
    $proj=getcwd;
    $proj =~ s/[A-Z]:/\$/;
    open(BATFILE,">put.bat");
    print BATFILE "@","echo off\n";
# -I- do not ask for user input under any circumstances
# -C- do not ask for comment
    print BATFILE "$SAFE_CMD Checkin -C- \"$proj\\$file\"";
	if ($updateVersionOnly) {
		print BATFILE " -O\@vsstmp.txt";
	}
    close(BATFILE);
    print ("trying to checkin  $file to SourceSafe...\n");
    system ("./put.bat");
	unlink ("./put.bat");
	if ($updateVersionOnly) {
		unlink ("\@vsstmp.txt");
	}
}


close(LOGFILE);

__END__
:endofperl
