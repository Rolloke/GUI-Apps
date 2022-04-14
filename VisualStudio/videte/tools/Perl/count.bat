@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

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

$lastDir="";

if ( -f "C:/vss/win32/ss.exe" ) {
	$SAFE_CMD="c:\\vss\\win32\\ss.exe";
} elsif ( -f "D:/vss/win32/ss.exe" ) {
	$SAFE_CMD="D:\\vss\\win32\\ss.exe";
} elsif (-f "E:/vss/win32/ss.exe" ) {
	$SAFE_CMD="E:\\vss\\win32\\ss.exe";
} elsif ( -f "F:/vss/win32/ss.exe" ) {
	$SAFE_CMD="F:\\vss\\win32\\ss.exe";
} else {
	die "SourceSafe not found\n";
}

$canSourceSafe=1;
$runAsGrepOnly=1;
$updateVersionOnly=0;

$pattern=$ARGV[0];
$replace="";

if ($#ARGV==0) {	# last index, not count
	if ($pattern eq "-updateVersion") {
		$pattern = "VERSION\\(.*\@AUTO_UPDATE";	# OOPS
		($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
		$month=eval($mon+1);
		$filler=":";
		if ( $min < 10) {
		$filler=":0";
		}
		$replace = "VERSION\($mday, $month, $hour, $min, \"$mday\/$month\/$year,$hour$filler$min\\0\", // \@AUTO_UPDATE";
		$updateVersionOnly=1;
		$runAsGrepOnly=0;
	}
} else {
	$replace=$ARGV[1];
	$runAsGrepOnly=0;
}

print "$pattern:";
if ( $runAsGrepOnly==0) { print "--> $replace"; }
else { print "listing only"; }
print "\n";

open(LOGFILE,">replace.log");

$doReplace=0;		# toggle for the wanted subroutine
$foundAFile=0;		# set if at least one file is found in the first run
$allAreWritable=1;	# set to 0 if one matched file does not have write permission
$numPerDir=0;
$numFound=0;

# first run only checks for all files and write permissions
&find (".");
printf ("%5d %s\n",$numFound,"TOTAL");

#
# check a file for the pattern, prints file:line if $runAsGrepOnly
#
sub ContainsPattern
{
	local($numPerFile)=0;
	local($dir,$file)=@_;
	# local($file) = @_;
	local($patternFound) = 0;
	open(FILE,$file) || warn "cant open $dir/$file\n";
	$line=1;
	while (<FILE>) {
		if (/^/) {
			$numPerDir++;
			$numPerFile++;
			$numFound++;
			$patternFound=1;
		}
		$line++;
	}
	close(FILE);
	# print "$dir/$file $numPerFile\n";
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
	print FILEOUT "/* GlobalReplace: $pattern --> $replace */\n";
	while (<FILE>) {
		if (/$pattern/) { 
			print LOGFILE "old:$_"; 
			$numReplaces += s/$pattern/$replace/g;
			print FILEOUT $_;
			print LOGFILE "-->:$_";
		} else {
			print FILEOUT $_;
		}	
	}

	close(FILE);
	close(FILEOUT);

	rename($file,"$file.bak") || warn "could not rename $file --> $file.bak\n";
	rename("$file.new",$file) || warn "could not rename $file.new --> $file\n";

	print "~$file: $numReplaces\n";
}

sub wanted
{
	$MyFile=$_;

	$dont_use_nlink=1;
	# print("FILE: $name\n");
	if ( -d "$dir/$_" ) {
		printf ("%5d %s\n",$numPerDir,$lastDir);
		$numPerDir=0;
        	# print ("Now $dir/$name:\n");
		$lastDir="$dir/$name";
		$prune=0;
	} else {
		# select files, either all or only rc files
	        if ( (/.cxx$/ || /.cpp$/ || /.h$/ || /.txt$/ || /.c$/) ) {
			$patternFound = &ContainsPattern($dir,$_);	# OLD $_
		} else {
			# print "plain $_\n";
		}
	}
	return 1==1;
}


close(LOGFILE);

__END__
:endofperl
