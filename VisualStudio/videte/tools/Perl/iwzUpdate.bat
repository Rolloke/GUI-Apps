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

$canSourceSafe=0;

$pattern="FooBarPattern";
$replace="";

$pattern = "FooBar";

($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
$month=eval($mon+1);
$filler=":";
if ( $min < 10) {
	$filler=":0";
}
$replace = "$1-$mday.$month.$year";

open(LOGFILE,">replace.log");

$doReplace=0;		# toggle for the wanted subroutine
$foundAFile=0;		# set if at least one file is found in the first run
$allAreWritable=1;	# set to 0 if one matched file does not have write permission

# first run only checks for all files and write permissions
&find (".");

print "DONE.\n";


# 
sub DoReplace
{

	($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
	$month=eval($mon+1);
	$filler=":";
	if ( $min < 10) {
		$filler="0";
	} else {
		$filler = "";
	}
	if ( $month< 10) {
		$monthFiller="0";
	} else {
		$monthFiller="";
	}
	if ( $hour< 10) {
		$hourFiller="0";
	} else {
		$hourFiller="";
	}



	local($file) = @_;
	
	print LOGFILE "\nFILE:$file\n\n";
	
	$numReplaces=0;

	open(FILE,$file) || warn "cant open $file\n";
	open(FILEOUT,">$file.new") || die "can't open foo.tmp\n";
	$bIsServicePack=0;
	while (<FILE>) {
		if ( /Service\!.*Pack/ ) {
			$bIsServicePack=1;
		}
		if (/^(Version=.*)\-(\d+.\d+\.\d+)$/ ) { 
			print $_;
			$numReplaces += s/$2/$mday\.$monthFiller$month\.$year/;
			print FILEOUT $_;
			print "-->:$_";
		} elsif ($bIsServicePack==1 
				&& /^(Icon3Description=.*) (\d+ \d+ \d+ \d+)$/ 
					) {		# 22 01 97 1430
			print $_;
			$numReplaces += s/$2/$mday $monthFiller$month $year $hourFiller$hour$filler$min/;
			print FILEOUT $_;
			print "-->:$_";
		} elsif ($bIsServicePack==0 &&
				 /^(Icon4Description=.*) (\d+ \d+ \d+ \d+)$/ 
					) {		# 22 01 97 1430
			print $_;
			$numReplaces += s/$2/$mday $monthFiller$month $year $hourFiller$hour$filler$min/;
			print FILEOUT $_;
			print "-->:$_";
		} else {
			print FILEOUT $_;
		}	
	}

	close(FILE);
	close(FILEOUT);

	rename($file,"$file.bak") || warn "could not rename $file --> $file.bak\n";
	rename("$file.new",$file) || warn "could not rename $file.new --> $file\n";

}

sub wanted
{
	$MyFile=$_;

	$dont_use_nlink=1;
	# print("FILE: $name\n");
	if ( -d "$dir/$_" ) {
                # print ("Subdirectory $name:\n");
		$prune=0;
	} else {
		# select files, either all or only rc files
			if ( /.iwz$/ || /.IWZ$/) {
				$file="$dir/$_";	# plus $dir
				if ($dir eq ".") {
					$file=$_;
				}
				$file=$name;
				print "File $file\n";		
				DoReplace($file);
			}
	}
	return 1==1;
}

close(LOGFILE);

__END__
:endofperl
