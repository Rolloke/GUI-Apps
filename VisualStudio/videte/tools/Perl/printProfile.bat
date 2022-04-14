@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

#
# PERL PART FOLLOWS HERE

&PrintProfile("foo.pro");

sub reverse {
 $b cmp $a
}

sub PrintProfile
{
	local($file)=@_;
	local(@valuesByHit)=();
	local(@valuesByPercent2)=();
	local(@valuesByTime)=();

    open(VFILE,"<$file");

	while (<VFILE>) {
		if (/\s+\d+\,\d+/) {
			$_ =~ s/[ \t]+/#/;
			$_ =~ s/[ \t]+/#/;
			$_ =~ s/[ \t]+/#/;
			$_ =~ s/[ \t]+/#/;
			$_ =~ s/[ \t]+/#/;
			$_ =~ s/[ \t]+/#/;
			($dummy,$funcTime,$percent,$sumTime,$percent2,
				$hitCount,$name)
			= split(/#/,$_,7);
			$oneRecord = sprintf("%6d %9.2f %9.2f %6d %9.2f %s",
						$hitCount,$funcTime,
						$percent,$sumTime,$percent2,
						$name);

			push(@valuesByHit,$oneRecord);

			push(@valuesByPercent2,"$percent2 $funcTime $percent $sumTime $hitCount $name" );
			push(@valuesByTime,"$funcTime $percent $sumTime $percent2 $hitCount $name");
		} 
	}	# end of loop of one file
	close(VFILE);
	print ("========== Test/Sorted by HitCount=============\n",reverse sort @valuesByHit);
	print ("========== Test/Sorted=============\n",sort @valuesBypercent2);
	print ("========== Test/Sorted=============\n",sort @valuesByTime);
}

__END__
:endofperl
