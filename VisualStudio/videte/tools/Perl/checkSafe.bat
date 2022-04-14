@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

#
# PERL PART FOLLOWS HERE
#

use Cwd;
require "find.pl";

if ( -f "C:/vss/win32/ss.exe" ) {
	$SAFE_CMD="c:\\vss\\win32\\ss.exe";
} elsif ( -f "D:/vss/win32/ss.exe" ) {
	$SAFE_CMD="D:\\vss\\win32\\ss.exe";
} elsif (-f "E:/vss/win32/ss.exe" ) {
	$SAFE_CMD="E:\\vss\\win32\\ss.exe";
} elsif ( -f "F:/vss/win32/ss.exe" ) {
	$SAFE_CMD="F:\\vss\\win32\\ss.exe";
} else {
	die "SourceSafe not found\n";
}

open(LOGFILE,">getall.log");

# store the inital dir
$startDir = getcwd;

&find (".");

print "DONE.\n";

@allFiles;

sub AddFile
{
	local($newFile) = @_;
	local($already) = 0;
	foreach $file (@allFiles) {
		if ($file eq $newFile) {
			$already=1;
		}
	}
	if ($already==0) {
		push(@allFiles,$newFile);
		# print ("added $newFile\n");
	}
}
sub ListSourceFiles
{
	local($file) = @_;
	local($patternFound) = 0;
	local(@allFiles);

	print ("reading $file...\n");
	open(FILE,$file) || warn "cant open $file\n";
	while (<FILE>) {
		# look for SOURCE, or read lines after DEP_CPP
		if ($inDep==0) {
			if (/^SOURCE=(.*)/) {
				&AddFile($1);
				# print ("$1\n");
			} 
			if (/DEP_CPP_.*=/) {
				$inDep=1;
			} else {
				$inDep=0;
			}
		} else {	# $inDep!=0
			if (/\$\(INCLUDE\)/) {
				# ignore these
			} elsif (/\"(.*)\"\\$/) {
				&AddFile($1);
				# print ("    $1\n");
			} else {
			   $inDep=0;
			}
		}
	}
	close(FILE);

	foreach $file (@allFiles) {
		print LOGFILE "$file\n";
		if ( -w $file ) {
			print ("$file is checked out\n");
		} else {
			# print ("$file\n");
			&CheckSourceSafe($file);
		}
	}

	return $patternFound;
}



sub wanted
{
	# select files
        if ( /.mak$/) {
		$file="$dir/$_";	# plus $dir
		if ($dir eq ".") {
			$file=$_;
		}
				
		&ListSourceFiles($_);
	} else {
		# print "plain $_\n";
	}
}

sub CheckSourceSafe
{
    local($file)=@_;

    $saveDir=getcwd;
	$saveDir =~ s/\\/\//g;

	$fileDir = $file;
	$fileDir =~ s/\\[A-Za-z0-9 \.\+\-\_]*$//;

	# take full path
	$file =~ s/^\.\\/$startDir\\/;
    #$file =~ s/^Project\//\$\/Project\//;
	$file =~ s/^[A-Za-z]:/\$/;	# drive --> $/

	while ( index($file,"..")>0 ) {
		$file =~ s/\\[A-Za-z0-9 \.\+\-\_]*\\\.\.\\/\\/;
	}
	chdir $fileDir || warn("no chdir $fileDir!?\n");
	# print LOGFILE ("file $file, fileDir $fileDir currentDir",getcwd,"\n");

	# convert to SourceSafe notation $/Project/...

    open(BATFILE,">gettmp.bat");
    print BATFILE "@","echo off\n";
    print BATFILE "$SAFE_CMD Get \"$file\"";
    close(BATFILE);
    # print ("updating $file from SourceSafe...\n");
    system "./gettmp.bat";
	chdir $saveDir;
}

close(LOGFILE);

__END__
:endofperl
