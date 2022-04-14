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

use Cwd;
require "find.pl";


&find (".");

sub Action
{
	local($dir,$file)=@_;
	print "$dir/$file\n";

	# NOT YET system("touch.exe $dir\\$file");

	return $patternFound;
}



sub wanted
{
	$dont_use_nlink=1;
	# print("FILE: $name\n");
	if ( -d "$dir/$_" ) {
        # print ("Now $dir/$name:\n");
		$prune=0;
	}
	# select files, either all or only rc files
		$file="$dir/$_";	# plus $dir
		if ($dir eq ".") {
			$file=$_;
		}
		$file=$name;
				
		&Action($dir,$_);
	
	return 1==1;
}
