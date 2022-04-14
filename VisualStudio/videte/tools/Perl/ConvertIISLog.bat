@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

#
# PERL PART FOLLOWS HERE
#
use Cwd;
require "find.pl";

$outFile="IISToSA.log";

# truncate existing file
open(OUTFILE,">$outFile") || warn "cant open $outFile\n";

if(0) {
	for ($i=0;$i <= $#ARGV;$i++) {
		$theFile=$ARGV[$i];
		&ConvertLog($theFile);
	}
}


sub FindFilesByPattern
{
	# /od order date
	# /on order name
	@someFiles = `dir /b /on in*.log`;
	for ($i=0;$i <= $#someFiles;$i++) {
		$oneLine=$someFiles[$i];

		$oneLine =~ s/[\n\r]//gm;
		&ConvertLog($oneLine); 
	}
}


&FindFilesByPattern();
# &find (".");

close(OUTFILE);

print "DONE\n";

close(STDERR);
close(STDOUT);


sub ConvertLog
{
	local($file) = @_;
	print ("Convert$file to $outFile\n");


	open(INFILE,$file) || warn "cant open $file\n";
	$line=1;

	while (<INFILE>) {
		$oneLine = $_;
		$line++;

		$oneLine =~ s/W3SVC, WK_MSN_LUCKY, 134\.245\.76\.81,\s*\d+,\s*\d+,\s*\d+,\s*\d+,\s*\d+,//;
		$oneLine =~ s/GET,|POST,//;
		$oneLine =~ s/\s+/ /g;
		$oneLine =~ s/\s*DEVELOPER\\\w+\s*,//;
		$oneLine =~ s/\s*WK\_\w*\\\w+\s*,//;
		$oneLine =~ s/,\s*\-,/,/g;

		$oneLine =~ s/[,]\s*$//;


		# now it looks like
		# 134.245.76.26, 22.06.97, 10:36:12,   /wkdb/Default.htm, -, 
		($host,$dateText,$timeText,$wwwFile, $someArgs) = split(/,/,$oneLine,5);

		$dateText =~s/\.97//;

		$fileLine = "UnitLog\@$dateText,$timeText\@~1\@$wwwFile|$host|$someArgs";
		$fileLine =~ s/\|[\s\@]/\|/g;
		$fileLine =~ s/\|$//g;
		$fileLine =~ s/\|$//g;
		$fileLine =~ s/\|$//g;
		$fileLine =~ s/\@\s/\@/g;

		$hostLine="ProcessLog\@$dateText,$timeText\@~1\@$host|$wwwFile|$someArgs";
		$hostLine =~ s/\|[\s\@]/\|/g;
		$hostLine =~ s/\|$//g;
		$hostLine =~ s/\|$//g;
		$hostLine =~ s/\|$//g;
		$hostLine =~ s/\@\s/\@/g;

	
		print (OUTFILE "$fileLine\n");
		print (OUTFILE "$hostLine\n");

		
		# statlog format looks like this
		# UnitLog@24.06,23:02:51@-1@InputUnit|SIMUNITInput
		# TOPIC@date,time@VALUE@TEXT

	}	# end of <INFILE> loop

	close(INFILE);

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
        if ( /in.*\.log$/ ) {
			$file="$dir/$_";	# plus $dir
			if ($dir eq ".") {
				$file=$_;
			}
			$file=$name;
				
			&ConvertLog($file); 
		}

	return 1==1;
}




__END__
:endofperl

