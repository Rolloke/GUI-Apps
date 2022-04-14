
@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

#
# PERL PART FOLLOWS HERE
#


# NOT YET read cfg from command line

@Drives = ("C","D","E","F");

for ($d=0;$d<=$#Drives;$d++) {
	&CheckAndList($Drives[$d]);
}



sub CheckAndList
{
	local($drive)=@_;
	if (-d "$drive:/Project") {
		if (-d "$drive:/Project/Lib") {
			chdir ("$drive:/Project/Lib");

			# /ON order names
			@someFiles= `dir /b /ON *.lib`;
			local(@result);
			local(@resultDebug);

			#
			# pass 1, only relese variants
			#
			for ($i=0;$i <= $#someFiles;$i++) {
				$oneLine=$someFiles[$i];
				$oneLine =~ s/[\n\r]//gm;
				$_ = $oneLine;
				if (/d\.lib/ || /Debug\.lib/) {
					# see pass 2
				} else {
					$oneLine =~ s/\.lib//;
					push(@result,"$oneLine");
				}
				
			}

			# pass 2, only debug variants
			for ($i=0;$i <= $#someFiles;$i++) {
				$oneLine=$someFiles[$i];
				$oneLine =~ s/[\n\r]//gm;
				$_ = $oneLine;
				if (/d\.lib/ || /Debug\.lib/) {
					$oneLine =~ s/\.lib//;
					push(@resultDebug,"$oneLine");
				} else {
					# see pass 1
				}
				
			}

			

		# loop over all release results
		for ($i=0;$i <= $#result;$i++) {
	
			print ("LocalLib $drive:/Project/Lib/$result[$i]");

			$bHasDebug=0;
			for ($dr=0;$dr<=$#resultDebug;$dr++) {
				$_ = $resultDebug[$dr];
				if (/$result[$i]/) {
					$bHasDebug =1;
					# remove from @resultDebug
					splice(@resultDebug,$dr,1);
				} else {
					# print ("no match |$result[$i]|$resultDebug[$dr]|\n");
				}
			}

			if ($bHasDebug) {
				print (".lib and Debug\n");
			} else {
				print (".lib\n");
			}
		}	# end of loop over all release results

		# also print the libs remaining in @resultDebug
		for ($i=0;$i <= $#resultDebug;$i++) {
			print ("LocalLib $drive:/Project/Lib/$resultDebug[$i].lib\n");

		}	# end of loop over remaining debug results

		} else {
					# no Lib directory
		}
	} else {
		# no /Project driectory found
	}
}
__END__
:endofperl
