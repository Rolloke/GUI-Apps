		 @rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';



#
# PERL PART FOLLOWS HERE
#

$action="2x2";

if ($#ARGV==0) {	# last index, not count
	$fileIn=$ARGV[0];
} elsif ($#ARGV==1) {	# last index, not count
	$action=$ARGV[0];
	$fileIn=$ARGV[1];
} else {
	die "usage psposter.bat [2x2 3x3] input.ps";
}

if (! -f $fileIn) {
	die "InputFile $fileIn not found\n";
}

$fileOut = $fileIn;
$fileOut =~ s/\.ps//;
$fileOut .= "_$action.ps";

$cmd = "V:\\perl5\\bin\\pstops.exe -w1900mm -h2700mm ";
$cmd .= "\"";

if ($action eq "2x2") {
	$scale="1.9";
	$cmd .= "0\@$scale(0,-29cm)";
	$cmd .= ",0\@$scale(0,-2cm)";
	
	$cmd .= ",0\@$scale(-20cm,-29cm),";
	$cmd .= "0\@$scale(-20cm,-2cm)";
	
	$cmd .= "\" <$fileIn > $fileOut";
	print `$cmd`;
} elsif ($action eq "3x3") {

	$scale="2.75";

	$cmd .= "0\@$scale(0,-58cm)";
	$cmd .= ",0\@$scale(0,-31cm)";
	$cmd .= ",0\@$scale(0,-2cm)";

	$cmd .= ",0\@$scale(-18cm,-58cm)";
	$cmd .= ",0\@$scale(-18cm,-31cm)";
	$cmd .= ",0\@$scale(-18cm,-2cm)";

	$cmd .= ",0\@$scale(-36cm,-58cm)";
	$cmd .= ",0\@$scale(-36cm,-31cm)";
	$cmd .= ",0\@$scale(-36cm,-2cm)";


	$cmd .= "\" <$fileIn > $fileOut";
	print `$cmd`;
} else {
	print "Unknown action $action\n";
}


__END__
:endofperl
