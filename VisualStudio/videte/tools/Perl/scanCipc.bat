@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

#
# PERL PART FOLLOWS HERE
#

$theFile="C:/log/cipc";

&ContainsPattern($theFile);
print "DONE\n";

@knownNames;
%queueValues;

@sortedLines;

sub PrintLine
{
	
}
sub ContainsPattern
{
	local($file) = @_;
	local($patternFound) = 0;
	local($queueLength)=0;
	local($tickCount)=0;

	$line=1;

	local($startTime)=0;

	# prescan
	open(FILE,$file) || warn "cant open $file\n";
	while (<FILE>) {
		# example:
		# $CIPC 13389295:SWCodec1 Input:1
		if (!/Input/ && !/Relay/ && !/Control/
			&& /\$CIPC\s+(\d+):([\w\s]+):(\d+)/) {
			# store records with tickCount first, to allow sort
			$shmName=$2;
			$entry="$1:$3:$shmName";
			$queueValues{$2}=0;
			push(@sortedLines,$entry);
			#$tickCount=eval($1);
			#$shmName=$2;
			#$queueLength=eval($3);
		}
	}
	close(FILE);


	# header
	print "Time";
	foreach $channel (keys(%queueValues)) {
		print ":",$channel;
		$queueValues{$channel}=0;
	}
	print "\n";

	# real scan
	#open(FILE,$file) || warn "cant open $file\n";
	foreach ( sort (@sortedLines) ) {
		# example:
		# $CIPC 13389295:SWCodec1 Input:1
		# if (/Output/ && /\$CIPC\s+(\d+):([\w\s]+):(\d+)/) {
		if (/0000(\d+):(\d+):([\w\s]+)/) {
			if ($startTime==0) {
				$startTime=eval($1);
			}

			$tickCount=eval($1)-$startTime;
			$shmName=$3;
			$queueLength=eval($2);
			$queueValues{$shmName}=$queueLength;

			print "$tickCount";
			foreach $channel (keys(%queueValues)) {
				print ":",$queueValues{$channel};
			}
			print "\n";

		}
		$line++;
	}

	#close(FILE);
	print "DONE\n";
}


__END__
:endofperl
