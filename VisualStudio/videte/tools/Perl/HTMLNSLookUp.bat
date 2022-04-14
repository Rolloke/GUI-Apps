@rem = '
@echo off
v:\perl5\bin\perl.exe %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';

$bDoPing=0;

if ($#ARGV==0 && $ARGV[0] eq "-ping") {
	$bDoPing=1;
}

$cmdFile = "C:/Temp/nstmp.inp";

open(CMDFILE,">$cmdFile") || warn "cant open $cmdFile\n";

print (CMDFILE "ls -d wk.de\n");
print (CMDFILE "exit\n");

close (CMDFILE);

@result = `nslookup < $cmdFile`;

print ("<HEAD>\n");
print ("</HEAD>\n");

($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
if ( $min < 10) {
	$minFiller=":0";
} else {
	$minFiller = "";
}

print ("Created with HTMLNSLookUp.bat am $mday.$mon.$year um $hour:$minFiller$min .\n");


print ("<BODY>\n");

@ipList = ();
@nameList = ();
@infoList = ();


# loop over the result from nslookup
for ($i=0;$i <= $#result;$i++) {
		$oneLine=$result[$i];
		$oneLine =~ s/\n//;

		$oneLine =~ s/^\s+//;	# drop leading whitespace
		$oneLine =~ s/\t/\|/gm;
		$oneLine =~ s/\s+/ /gm;

		# drop own domain
		$oneLine =~s/134\.245\.76\.//;
		$oneLine =~s/\.wk\.de//;

		# print ("<tr><td>$oneLine</tr>\n");

		$_ = $oneLine;
		if (/ SOA/ || / NS/ || /^\s*$/
			|| /\]/ 
			|| /Standard-Server/ || /Address:/ 
			|| />/ ) {
			# ingore
		} elsif (/HINFO/) {
			$oneLine =~ s/HINFO/|/;
			($name,$info) = split(/\|/,$oneLine,2);
			push(@infoList,"<tr><td align=\"right\">$name<td>$info</tr>\n");
		} else {
			($name,$type,$ip) = split(/ /,$oneLine,3);

			$typeToPrint = $type;
			$typeToPrint =~ s/CNAME\s*/alias f\&uuml;r/;
			$typeToPrint =~ s/A\s*/=/;

			if ($type eq "A" || $type eq "A ") {
				if (length $ip<3) {
					push(@ipList,"<tr><td align=\"right\"> $ip<td>$name</tr>\n");
				} else {
					push(@ipList,"<tr><td align=\"right\">$ip<td>$name</tr>\n");
				}
			}

			if ($bDoPing) {
				$bOnline=0;
				$sOnline ="";
				if ($type eq "A" || $type eq "A ") {
					@pingResult =`ping -n 1 -w 2000 $name`;
					for ($p=0;$p<=$#pingResult;$p++) {
						$_ = $pingResult[$p];
						if (/Antwort von/) {
							$bOnline=1;
						}
					}
				}

				if ($bOnline) {
					$sOnline  = "online";
				} else {
					$sOnline = "---";
				}
				push (@nameList, "<tr><td>$name<td align=\"center\">$typeToPrint<td align=\"right\">$ip<td>$sOnline</tr>\n");
			} else {
				# no ping
				push (@nameList, "<tr><td>$name<td align=\"center\">$typeToPrint<td align=\"right\">$ip</tr>\n");
			}

			print ("</tr>\n");
		}

		print ("</tr>\n");

}

#
# the real output
#

print ("<table border=\"2\">\n");	# outer table

print ("<TR><TD>Nach Nummern sortiert<TD>Nach Namen sortiert<TD>Infos</TR>\n");
print ("<TR><TD>\n");

print ("<table border=\"2\">\n");
print (sort @ipList);
print ("</table>\n");

print ("<TD>\n");
print ("<table border=\"2\">\n");
print (@nameList);	# CAVEAT no sort nslookup already did so, perl sorts A-Z a-z order
print ("</table>\n");

print ("<TD>\n");
print ("<table border=\"2\">\n");
print (@infoList);	# CAVEAT no sort nslookup already did so, perl sorts A-Z a-z order
print ("</table>\n");


print ("</table>\n");


print ("</BODY>\n");


__END__
:endofperl
