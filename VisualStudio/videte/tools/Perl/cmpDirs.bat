@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

#
# PERL PART FOLLOWS HERE
#

if ($#ARGV==1) {	# last index, not count
	$dir1=$ARGV[0];
	$dir2=$ARGV[1];
} else {
	die "usage cmpDirs dir1 dir2";
}

if (! -d "$dir1") {
	die "$dir1 not fuond\n";
}

if (! -d "$dir2") {
	die "$dir2 not fuond\n";
}


chdir ("$dir1");

print ("Now in directory $dir1\n");

@someFiles = `dir /b /on *.*`;

	for ($i=0;$i <= $#someFiles;$i++) {
		$oneFile=$someFiles[$i];
		$_ = $oneFile;
		
		if ((/\.exe/i || /\.dll/i)) {
			$oneFile =~ s/[\n\r]//gm;
			
			if (-f "$dir2/$oneFile") {
				print ("======= $oneFile =======\n");
				print `v:\\perl5\\bin\\cmpExe.bat $oneFile $dir2/$oneFile`;
			} else {
				print ("$dir2/$oneFile not found\n");
			}
		}
	}

print ("cmpDirs $dir1 $dir2 DONE.\n");
__END__
:endofperl
