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
#require "finddepth.pl";


$canSourceSafe=0;
$runAsGrepOnly=1;

# first run only checks for all files and write permissions
&find (".");

# so what is the result of the first run ? Any files? All writable ?
print "DONE.\n";

#
# check a file for the pattern, prints file:line if $runAsGrepOnly
#
sub SearchPointer
{
	local($dir,$file)=@_;
	local($patternFound) = 0;
	local($lineno) = 0;
	local($isCPP) = 0;

	$_ = $file;
	if (/\.cpp$/) {
		$isCPP = 1;
	}
	# print ("Check $file\n");
	open(HFILE,$file) || warn "cant open $dir/$file\n";
	$lineno=0;
	while (<HFILE>) {
		$lineno++;
		s/\/\/.*$//;	# drop single line comment
		if ($isCPP) {
		    if (/(\w+)\s+m\_/) {
				if ($1 ne "return" && $1 ne "delete") {
					print ("$dir\\$file($lineno): local m_*:$_\n");
				}
			}
		} else {
			if (!/typedef/ && !/return/) {
				# pattern: word space * word;
				# that is TYPE optionalSpace STAR optionalSpace Var....
				if (/\w+\s*\*\s*(\w+)\s*;/) {
					$pointer = $1;
					&SearchInit($file,$pointer);
				}
			}
		}
	}
	close(HFILE);
}

sub SearchInit
{
	local($file,$pointer)=@_;
	local($lineno)=0;
	$found=0;
	$className="";
	$functionName="GLOBAL";
	# input is .h, search in .cpp
	$file =~ s/\.h/\.cpp/;
	# print ("Search $pointer in $file\n");
	open(CFILE,$file) || warn "cant open $dir/$file\n";
	while (<CFILE>) {
		$lineno++;
		s/\/\/.*$//;	# drop comment
		if (/(\w+)::(\w+)\s*\(/ ) {
			$className = $1;
			$functionName = $2;
		}
		if ((($className eq $functionName)
			|| ($functionName eq "InitInstance")
			|| ($functionName eq "Init"))
			&& /$pointer\s*=/) {
			$found=1;
		} else {
			# word space * word;
			if (/$pointer\s*=\s*NULL/) {
				if (   ! ($functionName eq $className || $className eq "GLOBAL") 
					&& ! ($functionName eq "Init")
					&& ! ($functionName eq "InitInstance")
					) {
					print ("$file($lineno):reset: in $className\:\:$functionName\n");
					print ("$file($lineno):reset:$_");
				} else {
					$found=1;
				}
			}
		}
	}
	close(CFILE);
	if ($found==0) {
		print ("$file(1): $pointer=NULL; not found!\n");
	}

}
# 
sub wanted
{
	$MyFile=$_;

	$dont_use_nlink=1;
	# print("FILE: $name\n");
	if ( -d "$dir/$_" ) {
                # print ("Subdirectory $name:\n");
		$prune=0;
	}
	# select files
        if ( /\.h$/ || /\.cpp$/) {
		$file="$dir/$_";	# plus $dir
		if ($dir eq ".") {
			$file=$_;
		}
		$file=$name;
			
		$patternFound = &SearchPointer($dir,$_);
		
		# open(FILEOUT,">foo.tmp") || warn "can't open foo.tmp\n";
		# first run only check, second run sets doReplace on TRUE
		if ( $doReplace==0 ) {
			if ($patternFound) {
				$foundAFile=1;
				if ($runAsGrepOnly==0) {
                                        if ( ! -w $MyFile && $canSourceSafe) { TrySourceSafe($MyFile); }
					if ( -w $MyFile) {
						print "+$file\n";
					} else {
                        print "%$file\n";
						$allAreWritable=0;
					}
				}
			} else {
                                # print "-$file\n";
			}
		} else {	# second run
			if ( $patternFound ) { 
				DoReplace($MyFile);
			}
		}
	} else {
		# print "plain $_\n";
	}
	return 1==1;
}


__END__
:endofperl
