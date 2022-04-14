@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';
#
# PERL PART FOLLOWS HERE
#

$maxVersions=10000;	# OOPS

open(STDERR,">&STDOUT");
$|=1;

open(OUTFILE,">SuperHistory.txt");


if ( -f "C:/vss/win32/ss.exe" ) {
	$SAFE_CMD="c:\\vss\\win32\\ss.exe";
} elsif ( -f "D:/vss/win32/ss.exe" ) {
	$SAFE_CMD="D:\\vss\\win32\\ss.exe";
} elsif (-f "E:/vss/win32/ss.exe" ) {
	$SAFE_CMD="E:\\vss\\win32\\ss.exe";
} elsif ( -f "F:/vss/win32/ss.exe" ) {
	$SAFE_CMD="F:\\vss\\win32\\ss.exe";
} elsif ( -f "c:/msdev/vss/win32/ss.exe" ) {
	$SAFE_CMD="C:\\msdev\\vss\\win32\\ss.exe";
} elsif ( -f "D:/DevStudio/vss/win32/ss.exe" ) {
	$SAFE_CMD="D:\\DevStudio\\vss\\win32\\ss.exe"
} elsif ( -f "D:/Msdev/common/vss/win32/ss.exe" ) {
	$SAFE_CMD="D:\\Msdev\\common\\vss\\win32\\ss.exe"
} elsif ( -f "c:/Msdev/common/vss/win32/ss.exe" ) {
	$SAFE_CMD="c:\\Msdev\\common\\vss\\win32\\ss.exe"
} else {
   die "SourceSafe not found\n";
   $SAFE_CMD="C:\\Foo\\win32\\ssfoo.exe";
}

if ($#ARGV!=1 && $#ARGV!=2) {	# last index, not count
	die "usage: superHistory.bat ProjectName \"pattern\" [singleFile]\n";
}

print "$ARGV[0],$ARGV[1],$ARGV[2],$ARGV[3]\n";

$project=$ARGV[0];
$pattern=$ARGV[1];
$bSingleFile=0;
$singleFileName="";

if ($#ARGV==2) {
	$singleFileName = $ARGV[2];
	$bSingleFile=1;
	print ("Single file $singleFileName\n");
}
$_ = $project;
if (/DV/)
{
	$project = "DV/$_";
}


&DoHistory();
&ListPatternOnly();

sub DoHistory
{
	if ($bSingleFile==0) {
		`$SAFE_CMD History -#$maxVersions -D \"\$/$project\" > TmpHistory.txt`;
	} else {
		`$SAFE_CMD History -#$maxVersions -D \"\$/$project/$singleFileName\" > TmpHistory.txt`;
	}
}

sub ListPatternOnly
{
	open(FILE,"TmpHistory.txt") || warn "cant open superHistory.bat\n";

	$line=1;
	$bFileNamePrined=0;
	$bPrintTo=0;

	while (<FILE>) {
		
		if ($bSingleFile==0) {
			# collect the filenames
			if (/^\*\*\*\*\*\s+(.*)\s+\*/) {
				# a file label
				$_ =~ s/[\ \t]+/ /gm;	# kill multiple blanks to do the split
				($foo,$currentFile,$foo) = split(/ /,$_,3);
				$_ =~ s/\n//;
				$_ =~ s/\r//;
				$bFileNamePrinted=0;
			}
		} else {
			# single File
			if (/^\*\*\*\*\*/) {	# a new version
				$bFileNamePrinted=0;
			}
		}
		
		if (/^User:/) {
			$_ =~ s/[\ \t]+/ /gm;	# kill multiple blanks
			$_ =~ s/Date:/ /;
			$_ =~ s/Time:/,/;
			$_ =~ s/\n//;
			$_ =~ s/\r//;
			$currentUser = $_;
		}

		if (/$pattern/ 
				&& !/^User/ 
				&& !/^Labeled/ 
				&& !/^$/ 
				&& !/^\*+.*\*+$/
				&& !/^History of/
				) {
			$_ =~ s/^\s*\d+//gm;	# drop the line number
			$_ =~ s/:[\ \t]+/:/gm;	# kill multiple blanks after the :
	
			# print the version info
			if ($bFileNamePrinted==0) {
				$bFileNamePrinted=1;
				if ($bSingleFile==0) {
					printf (">>> %-25s %s\n",$currentFile,$currentUser);
					printf (OUTFILE ">>> %-25s %s\n",$currentFile,$currentUser);
				} else {
					printf (">>>%s\n",$currentUser);
					printf (OUTFILE ">>> %s\n",$currentUser);
				}
			}

			if (/To:/ && $bPrintTo==0) {
				# pattern only in To:, so print the previous Change: first
				print ("$lastChange");
				print (OUTFILE "$lastChange");
				$bPrintTo=0;
			} elsif (/Change:/) {
				$bPrintTo=1;
			}

			# print the pattern line
			PrintLine($_);
		} else {
			# no pattern
			if (/Change:/) {
				$lastChange = $_;
				$bPrintTo=0;
			} elsif ($bPrintTo==1 && /To:/) {
				PrintLine($_);
				$bPrintTo=0;
			}

			$bIsChange = 0;
		}
		$line++;
	}

	close(FILE);
}

sub PrintLine
{
	local($x)=@_;
	$x =~ s/^\s*\d+//gm;	# drop the line number
	$x =~ s/Change:/Chg: /;
	$x =~ s/To:/To : /;
	$x =~ s/Del:/Del: /;
	$x =~ s/Ins:/Ins: /;
	$x =~ s/[\ \t]+/ /gm;	# kill multiple blanks after the :
	print ("$x");
	print (OUTFILE "$x");
}


close(OUTFILE);
print ("Wrote SuperHistory.txt\n");

__END__
:endofperl
