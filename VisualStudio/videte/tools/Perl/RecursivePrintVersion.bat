@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

#
# PERL PART FOLLOWS HERE
#
# patterns
# "\w+->\w+\s*=[^=]"	alle word->word = 
# "new\s+\w+\s*\["		alle new type [
#

@sortList;
@nameList;

use Cwd;

open(STDERR,">&STDOUT");
$|=1;

open(OUTFILE,">VersionTmp.txt");

# first run only checks for all files and write permissions
print (OUTFILE "============ Test =========\n");
&DoOneDir("V:\\bin\\Test","exe");
&DoOneDir("V:\\bin\\Test\\Mico","exe");
&DoOneDir("V:\\bin\\Test\\Coco","exe");

print (OUTFILE "========== Test/ByName=============\n",sort @nameList);
print (OUTFILE "========== Test/Sorted=============\n",sort @sortList);

@sortList= ();
@nameList= ();

print ("============ TestGarny =========\n");
print (OUTFILE "============ TestGarny =========\n");
&DoOneDir("V:\\bin\\TestGarny","exe");
&DoOneDir("V:\\bin\\TestGarny\\Mico","exe");
&DoOneDir("V:\\bin\\TestGarny\\Coco","exe");



print (OUTFILE "========== TestGarny/Sorted=============\n",sort @nameList);
print (OUTFILE "========== TestGarny/Sorted=============\n",sort @sortList);

close(OUTFILE);


print "RecursivePrintVersion DONE.\n";

sub DoOneDir
{
	local($myDir,$mySuffix)=@_;

	chdir ($myDir);

	@someFiles = `dir /b /on *.$mySuffix`;
	for ($i=0;$i <= $#someFiles;$i++) {
		$oneLine=$someFiles[$i];

		$oneLine =~ s/[\n\r]//gm;
		&TryPrintVersion($oneLine); 
	}
}

sub wanted
{
	$MyFile=$_;
	
	$dont_use_nlink=1;
	if ( -d "$dir/$_" ) {
		# print("DIR : $name\n");
		$prune=0;
	} else {
		# select files
		if ( /.exe$/ || /.EXE$/) {
				$file=$_;
				print("FILE: $dir/$file\n");
				&TryPrintVersion($file,$dir);
		}
	}
	return 1==1;
}
# Usage:
#	require "find.pl";
#
#	&find('/foo','/bar');
#
#	sub wanted { ... }
#		where wanted does whatever you want.  $dir contains the
#		current directory name, and $_ the current filename within
#		that directory.  $name contains "$dir/$_".  You are cd'ed
#		to $dir when the function is called.  The function may
#		set $prune to prune the tree.
#
# This library is primarily for find2perl, which, when fed
#
#   find2perl / -name .nfs\* -mtime +7 -exec rm -f {} \; -o -fstype nfs -prune
#
# spits out something like this
#
#	sub wanted {
#	    /^\.nfs.*$/ &&
#	    (($dev,$ino,$mode,$nlink,$uid,$gid) = lstat($_)) &&
#	    int(-M _) > 7 &&
#	    unlink($_)
#	    ||
#	    ($nlink || (($dev,$ino,$mode,$nlink,$uid,$gid) = lstat($_))) &&
#	    $dev < 0 &&
#	    ($prune = 1);
#	}
#
# Set the variable $dont_use_nlink if you're using AFS, since AFS cheats.

use Cwd;

sub find {
    chop($cwd = cwd);
    foreach $topdir (@_) {
	(($topdev,$topino,$topmode,$topnlink) = stat($topdir))
	  || (warn("Can't stat $topdir: $!\n"), next);
	if (-d _) {
	    if (chdir($topdir)) {
		($dir,$_) = ($topdir,'.');
		$name = $topdir;
		&wanted;
		($fixtopdir = $topdir) =~ s,/$,, ;
		&finddir($fixtopdir,$topnlink);
	    }
	    else {
		warn "Can't cd to $topdir: $!\n";
	    }
	}
	else {
	    unless (($dir,$_) = $topdir =~ m#^(.*/)(.*)$#) {
		($dir,$_) = ('.', $topdir);
	    }
	    $name = $topdir;
	    chdir $dir && &wanted;
	}
	chdir $cwd;
    }
}

sub finddir {
    local($dir,$nlink) = @_;
    local($dev,$ino,$mode,$subcount);
    local($name);

    # Get the list of files in the current directory.

    opendir(DIR,'.') || (warn "Can't open $dir: $!\n", return);
    local(@filenames) = readdir(DIR);
    closedir(DIR);

    if ($nlink == 2 && !$dont_use_nlink) {  # This dir has no subdirectories.
	for (@filenames) {
	    next if $_ eq '.';
	    next if $_ eq '..';
	    $name = "$dir/$_";
	    $nlink = 0;
	    &wanted;
	}
    }
    else {                    # This dir has subdirectories.
	$subcount = $nlink - 2;
	for (@filenames) {
	    next if $_ eq '.';
	    next if $_ eq '..';
	    $nlink = $prune = 0;
	    $name = "$dir/$_";
	    &wanted;
	    if ($subcount > 0 || $dont_use_nlink) {    # Seen all the subdirs?

		# Get link count and check for directoriness.

		($dev,$ino,$mode,$nlink) = lstat($_) unless $nlink;
		
		if (-d _) {

		    # It really is a directory, so do it recursively.

		    if (!$prune && chdir $_) {
			&finddir($name,$nlink);
			chdir '..';
		    }
		    --$subcount;
		}
	    }
	}
    }
}
#######################################
sub TryPrintVersion
{

	local($file,$dir)=@_;
    `v:\\bin\\printVersion.exe .\\$file > footmp.txt`;
    open(VFILE,"<footmp.txt");

	local($oneVersion)="";
	local($onePlainVersion)="";
	local($oneFile)="";
	local($oneProduct)="";
	local($oneFileSize)="";
	local($oneFileTime)="";

	while (<VFILE>) {
		if (/^File[ \t]+:/) {
			$_ =~ s/^File.*: //;
			$_ =~ s/\.\\//;
			$_ =~ s/\.\///;
			$_ =~ s/[\ \t]+//;
			$_ =~ s/[\ \t]+//;
			$_ =~ s/\n//;
			$_ =~ s/\r//;
			printf(OUTFILE "%-25s","$_");;
			$oneFile = $_;
		} elsif (/^ProductName/) {
			$_ =~ s/^ProductName[ \t]*:/ : /;
			$_ =~ s/[\ \t]+//;
			$_ =~ s/[\ \t]+//;
			$_ =~ s/\n//;
			$_ =~ s/\r//;
			$oneProduct=$_;
			print (OUTFILE " : $_");
		} elsif (/^FileVersion/) {
			$_ =~ s/^FileVersion[ \t]*:/ : /;
			$_ =~ s/[\ \t]+//;
			$_ =~ s/[\ \t]+//;
			$_ =~ s/\/(\d)\//\/0$1\//;	# /digit/ --> /0digit/
			$_ =~ s/:(\d)\//:0$1\//;	# :digit, --> :0digit,
			$_ =~ s/,(\d):/,0$1:/;	# ,digit: --> ,0digit:
			$_ =~ s/\n//;
			$_ =~ s/\r//;
			printf (OUTFILE "$_");
			#
			$oneVersion = $_;
			$oneVersion =~ s/://;
			
			($versionA,$versionTime)
				= split(/,/,$oneVersion,2);
			($versionDay,$versionMonth,$versionYear)
				= split(/\//,$versionA,3);
			$oneVersion = "$versionYear/$versionMonth/$versionDay,$versionTime";
			$onePlainVersion = "$versionDay/$versionMonth/$versionYear,$versionTime";

		} elsif (/^FileTime[ \t]*:/) {			
			$_ =~ s/^FileTime[ \t]*://;
			$_ =~ s/[\ \t]+//;
			$_ =~ s/[\ \t]+//;
			$_ =~ s/\n//;
			$_ =~ s/\r//;
			$oneFileTime=$_;
		} elsif (/kein/) {
			print (OUTFILE " :  no info ");
			$oneVersion = "no info";
		} else {
			# print "...:$_";
		}
	}	# end of loop of one file
	close(VFILE);
	
	push(@sortList,"$oneVersion:$oneFile\n");
	push(@nameList,"$oneFile:$onePlainVersion\n");

	unlink("footmp.txt");
	print (OUTFILE "\n");
}


__END__
:endofperl
