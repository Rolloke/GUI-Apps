@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

#
# PERL PART FOLLOWS HERE
#

$theFile="C:/log/cipc";

print "File $theFile\n";

&ContainsPattern($theFile);
print "DONE\n";

@knownNames;
@queueValues;

sub CalcColumn
{
	local($n)=@_;
	local($found)=0;
	local($x)=0;
	foreach (@knownNames) {
		if ($found==0 && $_ eq $n) {
			$found=1;
		} else {
			$x++;
		}
	}
	if ($found==0) {
		push(@knownNames,$n);
		$x=$#knownNames;
		push(@queueValues,0);
		print "PUSH  $n, $#knownNames,$#queueValues\n";
	}
	return $x;
}

sub PrintLine
{
	
}
sub ContainsPattern
{
	local($file) = @_;
	local($patternFound) = 0;
	local($queueLength)=0;
	open(FILE,$file) || warn "cant open $file\n";
	$line=1;
	while (<FILE>) {
		# example:
		# $CIPC 13389295:SWCodec1 Input:1
		if (/\$CIPC\s+(\d+):([\w\s]+):(\d+)/) {
			$tickCount=$1;
			$shmName=$2;
			$queueLength=eval($3);
			$x=&CalcColumn($shmName);
			
			$queueValues[$x]=$queueLength;

			print "$tickCount";
			for ($i=0;$i<=$#queueValues;$i++) {
				print ":",$queueValues[$i];
			}
			print "\n";

		}
		$line++;
	}

	print "Time:";
	foreach (@knownNames) {
		print "$_:";
	}
	print "\n";
	close(FILE);
}


__END__
:endofperl
