@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

#
# PERL PART FOLLOWS HERE
#
$theFile=$ARGV[0];

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
		if (/Port [012347]/ || /Laufzeit/ || /CTimer/ || /^$/) {
			# suppress
		} else {
			printf ("$_");
		}
		$line++;
	}
	close(FILE);
}


__END__
:endofperl
