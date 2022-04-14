@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

#
# PERL PART FOLLOWS HERE
#

if ($#ARGV==1) {	# last index, not count
	$exe1=$ARGV[0];
	$exe2=$ARGV[1];
} else {
	die "usage cmpExe exe1 exe2";
}


# make sure ftp.exe is in the path
if (-d "c:\\Msdev98") {
	$MSDEV_DIR="c:\\Msdev98";
} elsif (-d "D:\\Msdev98") {
	$MSDEV_DIR="D:\\Msdev98";
} elsif (-d "E:\\Msdev98") {
	$MSDEV_DIR="E:\\Msdev98";
} else {
	die "no msdev found";
}

$tmpPath = $ENV{'PATH'};
$tmpPath .= ";$MSDEV_DIR\\bin;$MSDEV_DIR\\sharedide\\bin;$MSDEV_DIR\\Common\\Tools";
$tmpPath .= ";$MSDEV_DIR\\VC\\bin";
$tmpPath .= ";$MSDEV_DIR\\VC98\\bin";
$tmpPath .= ";$MSDEV_DIR\\Common\\Msdev98\\bin";
$tmpPath .= ";v:\\bin\\gnu";
$ENV{'PATH'} = $tmpPath;

# print "path is $ENV{'PATH'}\n";

	# print ("Dumping $exe1...\n");
	print `dumpbin /all $exe1 | grep -v "time date stamp" | grep -v "Dump of file" > $exe1.dump`;
	# print ("Dumping $exe2...\n");
	print `dumpbin /all $exe2 | grep -v "time date stamp" | grep -v "Dump of file" > $exe2.dump`;

	# --report-identical-files 
	print `diff $exe1.dump $exe2.dump`;

	unlink ("$exe1.dump");
	unlink ("$exe2.dump");

__END__
:endofperl
