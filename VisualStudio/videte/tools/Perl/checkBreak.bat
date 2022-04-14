@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

#
# PERL PART FOLLOWS HERE
#


$projectDir="Unknown Directory";

if ($#ARGV==0) {	# last index, not count
	$projectDir=$ARGV[0];
} else {
	die "usage checkBreak projectDir";
}


if (! -d $projectDir) {
	die "Directory $projectDir not found";
}

chdir ($projectDir);

$allLines=0;

@someFiles= `dir /b /on *.*`;
for ($i=0;$i <= $#someFiles;$i++) {
	$oneLine=$someFiles[$i];
	$oneLine =~ s/[\n\r]//gm;
	$_=$oneLine;
	if (/\.cpp$/i || /\.h/i || /\.c$/i) {
		&CheckOneFile($oneLine);
	}
}


printf ("#STAT# %5d lines in $projectDir\n",$allLines);
# print ("checkBreak DONE!\n");
# end of main

sub CheckOneFile
{
	local ($file)=@_;

	local ($bOpenCase)=0;
	local ($bHadFallThrough);	# if "_FallThrough_" or "FALLTHROUGH" was found, to allow // comments
	local ($lastCaseLine)=0;
	local ($bHadSomeText)=0;

	local ($iLocks)=0;
	local ($iUnlocks)=0;

	open(INFILE,$file) || warn "cant open $file\n";
	$line=0;

	while (<INFILE>) {
		$oneLine = $_;
		# remember the special token BEFORE dropping the // comment
		if (/_FallThrough_/ || /FALLTHROUGH/) {
			$bHadFallThrough=1;
		} else {
			$bHadFallThrough=0;
		}

		# drop // comment
		$oneLine =~ s/\/\/.*$//;
		$line++;
		$allLines++;

		if (/goto/ && !/NEEDS_GOTO/ ) {
			# Giving up print ("$projectDir/$file($line): goto found in $_");
		} elsif (/switch/) {	# OOPS weak pattern
			# reset
			# NOT YET nesting
			$bOpenCase=0
		} elsif (/case\s+\w+:/) {
			if ($bOpenCase) {
				# already in case
				if ($bHadSomeText) {
					print ("$projectDir/$file($lastCaseLine) : error : missing break, near $_");
				}
			}
			$lastCaseLine = $line;
			# direct check
			if (/break\s*;/ || /return/ || /continue/ || /\#\w/ || $bHadFallThrough) {
				# allow single lines with case : return or break
				# even allow preprocessor directives
				$bOpenCase=0;
			} else {
				$bOpenCase=1;
				$bHadSomeText=0;	# NOT YET check rest of line
			}
			# print ("$projectDir/$file:($line)$_");
		} else {
			# no case
			if ($bOpenCase) {
				# already in case
				if (/break\s*;/ || /return/ || $bHadFallThrough) {
					$bOpenCase=0;
				} else {
					if (! /^#else/) {
						$bHadSomeText=1;
					} else {
						# ignore preprocessor #else
					}
				}
			} else {
				# plain text outside of case
			}
		}

		#if (/if/ && /\;/) {
		#	print ("$projectDir/$file($line) : warning : if and ; in one line, near $_");
		#}

		if (/Lock\(/) {
			$iLocks++;
		}
		if (/Unlock\(/) {
			$iUnlocks++;
		}

	}

	close(INFILE);

	if ($iUnlocks < $iLocks) {
		print ("$projectDir/$file($line) : error : nr of locks too large \n");
	}

}
__END__
:endofperl
