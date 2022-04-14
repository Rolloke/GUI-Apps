@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

#
# PERL PART FOLLOWS HERE
#
$theFile=$ARGV[0];

print "File $theFile\n";

rename($theFile,"$theFile.bak");
&ContainsPattern("$theFile.bak");
unlink("$theFile.bak");

print "DONE\n";

sub ContainsPattern
{
	local($file) = @_;
	local($patternFound) = 0;
	open(FILE,$file) || warn "cant open $file\n";
	open(FILEOUT,">$theFile") || warn "cant open $theFile for writing\n";

	$line=1;
	while (<FILE>) {
		if (/LANGUAGE 7/) {
			$newText = $_;
			$newText =~ s/LANGUAGE 7/LANGUAGE 9/;
			print (FILEOUT "$newText");
		} elsif (/AFX_TARG_DEU/) {
			$newText = $_;
			$newText =~ s/AFX_TARG_DEU/AFX_TARG_ENU/;
			print (FILEOUT "$newText");
		} elsif (/l\.deu\\\\/) {
			$newText = $_;
			$newText =~ s/l\.deu\\\\//;
			print (FILEOUT "$newText");
		} elsif (/l\.deu\\/) {
			$newText = $_;
			$newText =~ s/l\.deu\\//;
			print (FILEOUT "$newText");
		} elsif (/LANG_GERMAN, SUBLANG_GERMAN/) {
			$newText = $_;
			$newText =~ s/LANG_GERMAN, SUBLANG_GERMAN/9, 1/;
			print (FILEOUT "$newText");
		} elsif (/^#include "res\\/) {
			print (FILEOUT "#define WK_LANG_BLOCK \"040904b0\"\n");
			print (FILEOUT "#define WK_LANG_BLOCK_NUM 0x409\n");
			print (FILEOUT "$_");
		} elsif (/German \(Germany\)/) {
			$newText = $_;
			$newText =~ s/German \(Germany\)/English \(U.S.\)/;
			print (FILEOUT "$newText");
		} else {
			print (FILEOUT "$_");
		}
		$line++;
	}
	close(FILE);
}


__END__
:endofperl




