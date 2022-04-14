@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

#
# PERL PART FOLLOWS HERE
#
if ($#ARGV==1) {
	$theFile=$ARGV[0];
	$pattern=$ARGV[1];
	$noLines=1;
} else {
	if ($#ARGV==2) {
		$theFile=$ARGV[0];
		$pattern=$ARGV[1];
		$noLines=0;
	} else {
		exit 1;
	}
}

print "File $theFile\n";

&ContainsPattern($theFile);
print "DONE\n";

sub ContainsPattern
{
	local($file) = @_;
	local($patternFound) = 0;
	open(FILE,$file) || warn "cant open $file\n";
	$line=1;
	while (<FILE>) {
		if (/$pattern/) {
			if ($noLines) {
				printf ("$file(%d):$_",$line);
			} else {
				printf ("$_");
			}
		}
		$line++;
	}
	close(FILE);
}


__END__
:endofperl
