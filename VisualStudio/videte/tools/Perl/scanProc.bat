@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

#
# PERL PART FOLLOWS HERE
#

$theFile="C:/log/server.log";
$thePattern="@";

if ($#ARGV==0) {	# OOPS last index ?
	$outName=$ARGV[0];
} elsif ($#ARGV==1) {
	$outName=$ARGV[0];
	$thePattern=$ARGV[1];	# optional pattern
} elsif ($#ARGV==1) {
	$outName=$ARGV[0];
	$thePattern=$ARGV[1];	# optional pattern
	$theFile=$ARGV[2];	# optional input file
} else {
	die "Invalid args\n";
}

&DoScan($theFile);	# call main function

##############################################
# Variables 
##############################################

%stateValues;	# mapping NAMES-->state,
				# where state is x,x+1 x==OFF, x+1==ON
				# and x=0,2,4,6...
%indexMap;		# mapping name->count == x

$tickCount="00:00:00";

@sortedLines;	# the result from the inital 'grep'

sub PrintState
{
	# print current state
	print OUTFILE "$tickCount";
	foreach $channel (keys(%stateValues)) {
		print OUTFILE "@",$stateValues{$channel};
	}
	print OUTFILE "\n";
}

sub DoScan
{
	local($file) = @_;
	local($patternFound) = 0;
	local($queueLength)=0;

	$line=1;

	local($startTime)=0;

	# prescan
	open(FILE,$file) || warn "cant open $file\n";
	while (<FILE>) {
		# example:
		# ProcessLog:%09d:ON:%s\n"
		if (($thePattern eq "@" || /$thePattern/)
			&& /ProcessLog@(\d+:\d+:\d+)@(\w+)@(.+)/) {
			# store records with tickCount first, to allow sort
			$processName=$3;
			$processName =~ s/\s+/_/g;	# drop spaces
			$processName =~ s/@/#/g;	# drop delimiter
			$entry="$1\@$2\@$processName";
			push(@sortedLines,$entry);

			# strange counts like 3/8 --> 3
			$countOld = %stateValues;
			$countOld =~ s/\/.+//;

			$stateValues{"$processName"}=0;	# might add an new name

			$countNew = %stateValues;
			$countNew =~ s/\/.+//;

			# if it is a new entry, add to indexMap
			if ($countOld != $countNew) {
				$indexMap{"$processName"}=$countNew;
				print ("New Index $countNew for $processName\n");
			}
		}
	}
	close(FILE);


    open(OUTFILE,">$outName") || die "Could not open $outName\n";

	# header
	print OUTFILE "Time";
	foreach $processName (keys(%stateValues)) {
		print OUTFILE "\@",$processName;
		$stateValues{"$processName"}=2*$indexMap{"$processName"};
		
		print "$processName\n";
	}
	print OUTFILE "\n";

	# real scan
	foreach ( sort (@sortedLines) ) {
		# example:
		# TICK:VALUE:NAME
		if (/(\d+:\d+:\d+)@(\w+)@(.+)/) {
			$tickCount=$1;
			$processName=$3;
			$state=$2;
			$newState=0;
			if ($state eq "ON") {
				$newState=2*$indexMap{"$processName"}+1;
			} elsif ( $state eq "OFF") {
				$newState=2*$indexMap{"$processName"};
			}
			# old state
			&PrintState();
			# modify
			$stateValues{"$processName"}=$newState;
			#print new state
			&PrintState();
		}
		$line++;
	}
    close(OUTFILE);
}


__END__
:endofperl
