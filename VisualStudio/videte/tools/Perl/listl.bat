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

$outFile="ListL.log";

# truncate existing file
open(OUTFILE,">$outFile") || warn "cant open $outFile\n";

sub FindFilesByPattern
{
	# /od order date
	# /on order name
	@someFiles = `dir /b /on *.cpp`;
	for ($i=0;$i <= $#someFiles;$i++) {
		$oneLine=$someFiles[$i];

		$oneLine =~ s/[\n\r]//gm;
		&Action($oneLine); 
	}
}


&FindFilesByPattern();
# &find (".");

close(OUTFILE);

print "DONE\n";

close(STDERR);
close(STDOUT);


sub Action
{
	local($file) = @_;

	open(INFILE,$file) || warn "cant open $file\n";
	$line=1;

	$iMaxCol = 0;
	while (<INFILE>) {
		$oneLine = $_;
		$line++;
		$col = index($oneLine,"\n");
		# printf ("Col %d\n",$col);
		if ($col > $iMaxCol) {
			$iMaxCol = $col;
		}
	}	# end of <INFILE> loop

	close(INFILE);
	printf("%20s max %d\n",$file,$iMaxCol);
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

