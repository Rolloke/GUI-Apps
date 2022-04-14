@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';



#
# PERL PART FOLLOWS HERE
#
unlink("docscan.log");

require "find.pl";


$|=1;

#open(STDERR,">&STDOUT");
#select(STDERR); $|=1;
#select(STDOUT); $|=1;

if ($#ARGV==1) {
} else {
}

# first run only checks for all files and write permissions
# scan for .mak and then read the makefile
use Cwd;

$HelpName="";
$HelpList="";
$theMakefile="";



&find (".");



print "DONE\n";


sub CreateList
{
	local($file) = @_;
	$HelpName = $file;
	$HelpName =~ s/\.mak/Help.hlp/;
	
	$HelpList= $file;	
	$HelpList =~ s/\.mak/Help.lis/;

	# truncate existing file
	open(LISFILE,">$HelpList") || warn "cant open $HelpList\n";
	close(LISFILE);
	
	print ("Scanning $file to $HelpList...\n");

	&ScanScourcesFromMakefile($theMakefile);

	# now $HelpList is filled, ready to run docscan
	$currentDirectory=getcwd;
	print ("Running V:/Help/DocScan.exe -hpjlist $HelpList in $currentDirectory...\n");
	$errorLog = "";
	if (-f "V:/Help/DocScan.exe" ) {
		`V:\\Help\\DocScan.exe -hpjlist $HelpList`;
	} else {
		die "Could not found DocScan.exe\n";
	}

	# print "Log:$errorLog\n";
	
	if (-f "docscan.log") {
		open( ERRORLOG,"docscan.log");
			while(<ERRORLOG>) {
			print "$_";
		}
		close(ERRORLOG);
	} else {
		die "Could not read docscan.log\n";
	}
	print ("Running hcrtf...\n");
	$hcrtfLog = `V:\\Help\\Hcrtf.exe -x AutoDoc.hpj`;

	print ("Running winhelp...\n");
	if (-f "AutoDoc.hlp") {
		`winhelp.exe AutoDoc.hlp`;
	} else {
		die "Could not read AutoDoc.hlp\n";
	}
}

sub ScanScourcesFromMakefile
{
	local($file) = @_;
	open(MAKFILE,$file) || warn "cant open $file\n";
	$line=1;

	while (<MAKFILE>) {
		if (/^SOURCE=/ && !/\.bmp/ && !/\.lib/ && !/\.rc[2]/ && !/\.hlp/) {
			$oneFile = $_;
			$oneFile =~ s/SOURCE=//;
			$oneFile =~ s/\n//;

			if (&ContainsHelp($oneFile)) {
				print ("found '$oneFile'\n");
				open(LISFILE,">>$HelpList") || warn "cant open $HelpList\n";
				print (LISFILE "$oneFile\n");
				close(LISFILE);
			} else {
				# print (" noFile:'$oneFile'\n");
			}
		}
		$line++;
	}	# end of <MAKFILE> loop
	close(MAKFILE);
	
}


sub ContainsHelp
{
	local($file) = @_;
	local($bSomethingFound)=0;
	open(FILE,$file) || warn "cant open $file\n";
	$line=1;
	while (<FILE>) {
		if (/\@TOPIC/ || /\@MD/) {
			# print ("$file:found $_\n");
			$bSomethingFound=1;
		}
		$line++;
	}
	close(FILE);

	return $bSomethingFound;
}

# 
sub wanted
{
	$MyFile=$_;

	$dont_use_nlink=1;
	# print("FILE: $name\n");
	if ( -d "$dir/$_" ) {
        # print ("Subdirectory $name:\n");
		$prune=1; # =1 do not descend
	}
	# select files
    if ( /\.mak$/) {
		$file="$dir/$_";	# plus $dir
		if ($dir eq ".") {
			$file=$_;
		}
		$file=$name;
		$theMakefile=$file;
		&CreateList($file)	# OOPS HERE ?
	} else {
		# print "plain $_\n";
	}
	return 1==1;
}


close(STDERR);
close(STDOUT);




__END__
:endofperl
